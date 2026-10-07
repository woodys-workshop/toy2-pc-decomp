#include "NGNLoader/ObjectLoad.h"
#include "NGNLoader/NGNLoader.h"
#include "Renderer/Renderer.h"
#include "Logger.h"
#include "Nu3D/Math.h"

namespace NGNLoader
{
	namespace ObjectLoad
	{
		// GLOBAL: TOY2 0x00B0574C
		Nu3D::Primitive* g_curPrimObject;

		// GLOBAL: TOY2 0x00AFBB08
		int32_t g_curVertexCount;

		// GLOBAL: TOY2 0x00B1A470
		int32_t g_curMaterialCount;

		// GLOBAL: TOY2 0x00AAD8B0
		int32_t g_unused1;

		// GLOBAL: TOY2 0x00508A64
		int32_t g_isHardwareRendering = 1;

		// GLOBAL: TOY2 0x00AAD7B0
		char g_curShapeName[256];

		// GLOBAL: TOY2 0x00B1A474
		uint8_t g_curShapeNameLen;

		// GLOBAL: TOY2 0x00B1A420
		char* g_nameTableEntries[20];

		// GLOBAL: TOY2 0x00B057A0
		char g_nameTableBuffer[5120];

		// GLOBAL: TOY2 0x00AE8234
		uint32_t g_textureTable[42];

		// GLOBAL: TOY2 0x00B05750
		Nu3D::Material* g_curMaterialList[20];

		// GLOBAL: TOY2 0x00AE8288
		Nu3D::ShapeVertex g_shapeVertices[2000];

		// GLOBAL: TOY2 0x00B626A8
		int32_t g_curVertexFlags;

		// GLOBAL: TOY2 0x00B0092C
		int16_t g_primVertexData[10000];

		// GLOBAL: TOY2 0x00B06BA0
		Nu3D::Vertex g_processedPrimVerts[2000];

		// GLOBAL: TOY2 0x00AFBB0C
		int16_t g_indexDataConversion[17702];

		// GLOBAL: TOY2 0x00508A6C
		int32_t g_drawTypeConversion[8] = { 0, 0, 2, 1, 3, 5, 4, -1 };

		// GLOBAL: TOY2 0x00B184E0
		int32_t g_mergedVerts[2000];

		// GLOBAL: TOY2 0x00B1A478
		int32_t g_groupMemberList[2000];

		// GLOBAL: TOY2 0x00AAD8B4
		int32_t g_groupMetadata[60000];

		// FUNCTION: TOY2 0x004CC430
		int32_t ExtractShapeName(FILE* stream)
		{
			fread(&g_curShapeNameLen, sizeof(uint8_t), 1, stream);
			fread(g_curShapeName, sizeof(char), g_curShapeNameLen, stream);

			g_curShapeName[g_curShapeNameLen] = '\0';
			return 1;
		}

		// FUNCTION: TOY2 0x004CB320
		int32_t ExtractShapeTextures(FILE* stream)
		{
			int16_t nameTableCount;
			int16_t texCount;
			uint8_t nameTableCountOffset;
			uint8_t nameTableLength;

			fread(&nameTableCount, sizeof(int16_t), 1, stream);
			fread(&texCount, sizeof(int16_t), 1, stream);
			fread(&nameTableCountOffset, sizeof(uint8_t), 1, stream);
			fread(&nameTableLength, sizeof(uint8_t), 1, stream);

			if ((nameTableCountOffset + 1) * nameTableCount > 5120)
				Logger::GetErrorHandler("C:\\projects\\nu3d\\objload.c", 228)("texture nametable size exceeds maximum allowed");

			if (texCount >= 20)
				Logger::GetErrorHandler("C:\\projects\\nu3d\\objload.c", 231)("number of textures %hd exceeds maximum allowed per shape %d", texCount, 20);

			int32_t i = 0;
			int32_t offset = 0;

			if (nameTableCount)
			{
				char** buffer = g_nameTableEntries;
				do
				{
					*buffer = &g_nameTableBuffer[offset];

					uint8_t texDataLength;
					fread(&texDataLength, sizeof(uint8_t), 1, stream);

					if (texDataLength)
						fread(*buffer, 1, texDataLength, stream);

					(*buffer)[texDataLength] = '\0';
					++buffer;

					++i;
					offset += nameTableCountOffset + 1;
				} while (i < nameTableCount);
			}

			i = 0;

			if (texCount)
			{
				uint32_t* nextIndex = g_textureTable;

				do
				{
					int16_t nameIdx;
					fread(&nameIdx, sizeof(int16_t), 1, stream);

					NGNTextureParams texParams;
					texParams.rawTexStr = g_nameTableEntries[nameIdx];
					fread(&texParams.isTex14, sizeof(NGNTextureParams) - sizeof(char*), 1, stream);

					if (nameTableLength != 26)
						fseek(stream, (uint8_t)(nameTableLength - 26), SEEK_CUR);

					*nextIndex = GetOrAllocateTexture(&texParams);

					++i;
					++nextIndex;
				} while (i < texCount);
			}

			return 1;
		}

		// FUNCTION: TOY2 0x004CB4E0
		int32_t ExtractShapeMaterials(FILE* stream)
		{
			int16_t textureIndex[16];
			Nu3D::MaterialFile materialFile;

			uint32_t materialCount;
			fread(&materialCount, sizeof(uint32_t), 1, stream);

			if (materialCount >= 20)
				Logger::GetErrorHandler("C:\\projects\\nu3d\\objload.c", 288)("Given material count of %d exceeds maximum allowed (%d)", materialCount, 20);

			uint32_t count = 0;

			if (materialCount)
			{
				Nu3D::Material** curMaterial = g_curMaterialList;

				do
				{
					int32_t oneColor;
					RGBColor3B color;
					uint32_t materialId;
					uint32_t blockSize;

					fread(&materialId, sizeof(uint32_t), 1, stream);
					fread(&blockSize, sizeof(uint32_t), 1, stream);

					if ((materialId & 1) != 0)
					{
						fread(&color, sizeof(uint8_t), 3, stream);

						materialFile.diffuseColor.r = color.r * Renderer::g_gammaCorrection * (1.0f / 255.0f);
						oneColor = color.b;
						blockSize = (blockSize - 3);
						materialFile.diffuseColor.g = color.g * Renderer::g_gammaCorrection * (1.0f / 255.0f);
						materialFile.diffuseColor.b = color.b * Renderer::g_gammaCorrection * (1.0f / 255.0f);
					}
					else
					{
						materialFile.diffuseColor.b = 1.0;
						materialFile.diffuseColor.g = 1.0;
						materialFile.diffuseColor.r = 1.0;
					}

					if ((materialId & 2) != 0)
					{
						fread(&color, sizeof(uint8_t), 3, stream);

						blockSize = (blockSize - 3);
						materialFile.ambientColor.r = color.r * Renderer::g_gammaCorrection * (1.0f / 255.0f);
						oneColor = color.b;
						materialFile.ambientColor.g = color.g * Renderer::g_gammaCorrection * (1.0f / 255.0f);
						materialFile.ambientColor.b = color.b * Renderer::g_gammaCorrection * (1.0f / 255.0f);
					}
					else
					{
						materialFile.ambientColor.b = 1.0;
						materialFile.ambientColor.g = 1.0;
						materialFile.ambientColor.r = 1.0;
					}

					if ((materialId & 4) != 0)
					{
						fread(&color, sizeof(uint8_t), 3, stream);

						blockSize = (blockSize - 3);
						materialFile.specularColor.r = color.r * Renderer::g_gammaCorrection * (1.0f / 255.0f);
						oneColor = color.b;
						materialFile.specularColor.g = color.g * Renderer::g_gammaCorrection * (1.0f / 255.0f);
						materialFile.specularColor.b = color.b * Renderer::g_gammaCorrection * (1.0f / 255.0f);
					}
					else
					{
						materialFile.specularColor.b = 2.0;
						materialFile.specularColor.g = 2.0;
						materialFile.specularColor.r = 2.0;
					}

					if ((materialId & 8) != 0)
					{
						fread(&color, sizeof(uint8_t), 1, stream);
						oneColor = color.r;
						blockSize = (blockSize - 1);
						materialFile.emissiveIntensity = color.r * (1.0f / 255.0f);
					}
					else
					{
						materialFile.emissiveIntensity = 0.0;
					}

					if ((materialId & 16) != 0)
					{
						fread(&color, sizeof(uint8_t), 1, stream);
						oneColor = color.r;
						blockSize = (blockSize - 1);
						materialFile.power = color.r * (1.0f / 255.0f);
					}
					else
					{
						materialFile.power = 1.0;
					}

					if ((materialId & 32) != 0)
					{
						fread(&color, sizeof(uint8_t), 1, stream);
						oneColor = color.r;
						blockSize = (blockSize - 1);
						materialFile.opacity = color.r * (1.0f / 255.0f);
					}
					else
					{
						materialFile.opacity = 1.0;
					}

					if ((materialId & 64) != 0)
					{
						fread(&materialFile.metadata, sizeof(int32_t), 1, stream);
						blockSize = (blockSize - 4);
					}
					else
					{
						materialFile.metadata = 0;
					}

					uint32_t size = (materialId >> 7) & 0xF;

					if (size && (fread(textureIndex, sizeof(int16_t), size, stream), blockSize = (blockSize - 2 * size), textureIndex[0] >= 0))
					{
						materialFile.texDataIndex = g_textureTable[textureIndex[0]];
					}
					else
					{
						materialFile.texDataIndex = 0;
					}

					*curMaterial = Nu3D::Material::CreateFromFile(&materialFile);

					if (blockSize)
						fseek(stream, blockSize, SEEK_CUR);

					++count;
					++curMaterial;

				} while (count < materialCount);

				g_curMaterialCount = count;

				return 1;
			}
			else
			{
				g_curMaterialCount = 0;
				return 1;
			}
		}

		// FUNCTION: TOY2 0x004CB970
		int32_t ExtractShapeVertices(FILE* stream)
		{
			uint32_t vertexFlags;
			int32_t vertexDataLength;
			uint32_t vertexCount;

			fread(&vertexFlags, sizeof(uint32_t), 1, stream);
			fread(&vertexDataLength, sizeof(int32_t), 1, stream);
			fread(&vertexCount, sizeof(uint32_t), 1, stream);

			if (vertexCount >= 2000)
				Logger::GetErrorHandler("C:\\projects\\nu3d\\objload.c", 428)("Given vertex count of %d exceeds maximum allowed (%d)", vertexCount, 2000);

			g_curVertexCount = vertexCount;
			g_curVertexFlags = (uint8_t)vertexFlags;

			int32_t vertexSize = vertexDataLength;

			if (vertexFlags & 1)
				vertexSize -= 12; // position

			if (vertexFlags & 2)
				vertexSize -= 12; // normals

			if (vertexFlags & 4)
				vertexSize -= 4; // diffuse color

			if (vertexFlags & 8)
				vertexSize -= 4; // specular color

			int32_t sizeCheck = vertexSize - 8 * (vertexFlags >> 4); // texcoord sets, 8 bytes each

			if (sizeCheck < 0)
				Logger::GetErrorHandler("C:\\projects\\nu3d\\objload.c", 449)("Given vertex size is not big enough to contain specified data");

			uint32_t curVertex = 0;

			if (vertexCount)
			{
				Nu3D::ShapeVertex* shapeVertex = g_shapeVertices;

				do
				{
					shapeVertex->primVerticesSize = 65535;
					shapeVertex->primIndex = 65535;

					if (vertexFlags & 1)
					{
						fread(shapeVertex, sizeof(Vector3F), 1, stream);

						shapeVertex->position.x = NGNLoader::g_vertexScaleVector.x * shapeVertex->position.x;
						shapeVertex->position.y = NGNLoader::g_vertexScaleVector.y * shapeVertex->position.y;
						shapeVertex->position.z = NGNLoader::g_vertexScaleVector.z * shapeVertex->position.z;
					}
					else
					{
						Logger::GetErrorHandler("C:\\projects\\nu3d\\objload.c", 462)("Vertex must contain a position");
					}

					if (vertexFlags & 2)
					{
						fread(&shapeVertex->normals, sizeof(Vector3F), 1, stream);
					}
					else
					{
						shapeVertex->normals.x = 0.0f;
						shapeVertex->normals.y = 1.0f;
						shapeVertex->normals.z = 0.0f;
					}

					ARGB color;

					if (vertexFlags & 4)
					{
						fread(&color, sizeof(uint8_t), sizeof(ARGB), stream);
					}
					else
					{
						color.a = 255;
						color.b = 255;
						color.g = 255;
						color.r = 255;
					}

					shapeVertex->diffuse.a = color.a;

					int32_t scaled = color.r * Renderer::g_gammaCorrection;

					if (scaled > 255)
						scaled = 255;

					shapeVertex->diffuse.r = (uint8_t)scaled;

					scaled = color.g * Renderer::g_gammaCorrection;

					if (scaled > 255)
						scaled = 255;

					shapeVertex->diffuse.g = (uint8_t)scaled;

					scaled = color.b * Renderer::g_gammaCorrection;

					if (scaled > 255)
						scaled = 255;

					shapeVertex->diffuse.b = (uint8_t)scaled;

					if (vertexFlags & 8)
					{
						fread(&color, sizeof(uint8_t), sizeof(ARGB), stream);
					}
					else
					{
						color.b = 255;
						color.g = 255;
						color.r = 255;
						color.a = 255;
					}

					if (vertexFlags & 240)
					{
						fread(&shapeVertex->coords, 4, 2, stream);
					}
					else
					{
						shapeVertex->coords.y = 0.0f;
						shapeVertex->coords.x = 0.0f;
					}

					if (sizeCheck)
						fseek(stream, sizeCheck, SEEK_CUR);

					++shapeVertex;
					++curVertex;

				} while (curVertex < vertexCount);
			}

			return 1;
		}

		// FUNCTION: TOY2 0x004CBC90
		int32_t ExtractShapePrimitives(FILE* stream)
		{
			uint32_t curPrim = 0;
			Nu3D::Primitive* primListHead = 0;
			int32_t totalLocalVertCount = 0;

			if (g_curPrimObject)
				Logger::GetErrorHandler("C:\\projects\\nu3d\\objload.c", 591)("object contains more than one primhd array");

			if (! g_curVertexCount)
				Logger::GetErrorHandler("C:\\projects\\nu3d\\objload.c", 594)("vertex array must be loaded prior to primhd array");

			uint32_t primitiveCount;
			fread(&primitiveCount, sizeof(uint32_t), 1, stream);

			uint32_t type;
			int16_t materialIndex;
			int16_t primCount;
			Nu3D::Primitive* primObject = 0;
			int32_t localVertCount;
			int32_t verticesSize;
			uint32_t processedPrimCount = 0;

			if (primitiveCount)
			{
				while (true)
				{
					fread(&type, sizeof(uint32_t), 1, stream);
					fread(&materialIndex, sizeof(int16_t), 1, stream);
					fread(&primCount, sizeof(int16_t), 1, stream);

					if (primCount >= 10000)
						Logger::GetErrorHandler("C:\\projects\\nu3d\\objload.c", 607)("primitive count %d has exceeded maximum allowed %d", primCount, 10000);

					fread(g_primVertexData, sizeof(int16_t), primCount, stream);

					if (type)
					{
						if (type < 7)
							break;
					}

					verticesSize = localVertCount;

				LBL_NEXT_PRIM:

					++curPrim;

					totalLocalVertCount += verticesSize;
					processedPrimCount = curPrim;

					if (curPrim >= primitiveCount)
						goto LBL_FINISH;
				}

				int32_t curVertex = 0;

				verticesSize = 0;
				localVertCount = 0;

				int32_t curVertIdx = 0;
				int16_t* indexData;
				int16_t indexCount;
				int32_t primVertexIndex;

				if (primCount)
				{
					Nu3D::Vertex* processedVerts = g_processedPrimVerts;

					do
					{
						int16_t* primVertexData = &g_primVertexData[curVertex];

						if (*primVertexData >= g_curVertexCount)
							Logger::GetErrorHandler("C:\\projects\\nu3d\\objload.c", 617)("out of range vrt id");

						int32_t shapeVertIndex = *primVertexData;

						if (g_shapeVertices[shapeVertIndex].primIndex == curPrim)
						{
							*primVertexData = g_shapeVertices[*primVertexData].primVerticesSize;
						}
						else
						{
							g_shapeVertices[*primVertexData].primIndex = curPrim;
							Nu3D::Vertex* copyDestVert = processedVerts;

							g_shapeVertices[shapeVertIndex].primVerticesSize = verticesSize;

							Nu3D::ShapeVertex* copySrcVert = &g_shapeVertices[shapeVertIndex];
							Nu3D::Vertex* processedVertsNext = processedVerts;

							*primVertexData = verticesSize++;

							memcpy(copyDestVert, copySrcVert, sizeof(Nu3D::Vertex));

							curPrim = processedPrimCount;
							curVertex = curVertIdx;
							processedVerts = processedVertsNext + 1;
						}

						curVertIdx = ++curVertex;

					} while (curVertex < primCount);

					localVertCount = verticesSize;
				}

				primObject = Nu3D::Primitive::Build(verticesSize, 1);

				if (! primObject)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\objload.c", 634)("unable to alloc OBJ3D with %d vrts", verticesSize);

				int32_t flagsTemp = Nu3D::g_defaultPrimitiveFlags | primObject->renderFlags;
				primObject->renderFlags = flagsTemp;

				if ((g_curVertexFlags & 4) != 0)
					primObject->renderFlags = flagsTemp | 0x40001000;

				if (materialIndex >= g_curMaterialCount)
				{
					RGBColor whiteColor;
					whiteColor.r = 1.0;
					whiteColor.g = 1.0;
					whiteColor.b = 1.0;

					primObject->materialIndex = Nu3D::Material::CreateFromColor(&whiteColor)->id;
				}
				else
				{
					primObject->materialIndex = g_curMaterialList[materialIndex]->id;
				}

				memcpy(primObject->patchVerts.data.vertices, g_processedPrimVerts, 4 * ((sizeof(Nu3D::Vertex) * verticesSize) >> 2));

				if (type == 4)
				{
					indexCount = primCount;

					if (g_isHardwareRendering)
					{
						int16_t* primVertexData = &g_primVertexData[primCount];
						int32_t newIndexCount = 3 * primCount / 2;

						for (indexData = &g_indexDataConversion[newIndexCount]; indexData != g_indexDataConversion; indexData[5] = primVertexData[3])
						{
							int16_t quadVertIdx = *(primVertexData - 4);

							primVertexData -= 4;
							indexData -= 6;

							*indexData = quadVertIdx;

							indexData[1] = primVertexData[1];
							indexData[2] = primVertexData[2];
							indexData[3] = primVertexData[0];
							indexData[4] = primVertexData[2];
						}

						indexCount = newIndexCount;
						type = 1;
						primCount = newIndexCount;

						goto LBL_EMIT_INDICES;
					}

					primVertexIndex = primCount;
					indexData = &g_indexDataConversion[primVertexIndex];

					if (indexData == g_indexDataConversion)
					{
					LBL_EMIT_INDICES:

						if (! Nu3D::Primitive::InitHeader(primObject->header, g_drawTypeConversion[type], indexCount))
							Logger::GetErrorHandler("C:\\projects\\nu3d\\objload.c", 692)(
								"unable to initialise primheader %d for %hd vid", processedPrimCount, primCount);

						memcpy(primObject->header->indices, indexData, 2 * primCount);

						if (type != 5 && type != 6)
						{
							Nu3D::Vertex* patchVertVertices = primObject->patchVerts.data.vertices;
							uint16_t* headerIndices = primObject->header->indices;

							memset(g_mergedVerts, 0, 4 * localVertCount);

							Nu3D::Vertex* vertices_ = patchVertVertices;
							int32_t mergedVertIndex = 0;

							memset(g_groupMemberList, 0, sizeof(g_groupMemberList));

							int32_t groupMember = 0;
							uint16_t* indices = headerIndices;
							curVertIdx = 0;

							if (localVertCount > 0)
							{
								int32_t groupMetadataOffset = 0;
								int32_t* groupMetadataPtr = g_groupMemberList;

								do
								{
									if (! g_mergedVerts[mergedVertIndex])
									{
										int32_t prevGroupMemberCount = groupMetadataPtr[1];
										++groupMetadataPtr;
										groupMetadataOffset += 30;

										int32_t groupMemberListIdx = prevGroupMemberCount + groupMetadataOffset;
										int32_t currentGroupIdx = prevGroupMemberCount + 1;
										uint32_t totalVertCount = localVertCount;

										g_groupMetadata[groupMemberListIdx] = mergedVertIndex;

										int32_t nextMergedVertIndex = mergedVertIndex + 1;
										int32_t nextGroupMember = ++groupMember;

										g_mergedVerts[mergedVertIndex] = groupMember;

										*groupMetadataPtr = currentGroupIdx;

										if (mergedVertIndex + 1 < totalVertCount)
										{
											int32_t* mergedVertPtr = &g_mergedVerts[nextMergedVertIndex];
											Vector3F* curVert = &vertices_[nextMergedVertIndex].position;

											do
											{
												if (! *mergedVertPtr)
												{
													Vector3F* vertPos = &vertices_[mergedVertIndex].position;

													if (curVert->x == vertPos->x && curVert->y == vertPos->y && curVert->z == vertPos->z)
													{
														groupMember = nextGroupMember;
														*mergedVertPtr = nextGroupMember;

														int32_t memberListIdx = currentGroupIdx + groupMetadataOffset;
														*groupMetadataPtr = ++currentGroupIdx;

														g_groupMetadata[memberListIdx] = nextMergedVertIndex;
													}
													else
													{
														groupMember = nextGroupMember;
													}
												}

												++nextMergedVertIndex;
												curVert += 3;
												++mergedVertPtr;

											} while (nextMergedVertIndex < localVertCount);

											headerIndices = indices;
										}

										if (*groupMetadataPtr == 1)
										{
											int32_t groupBaseOffset = groupMetadataOffset - 30;
											*groupMetadataPtr = 0;
											--groupMember;

											groupMetadataOffset = groupBaseOffset;
											--groupMetadataPtr;
											g_mergedVerts[mergedVertIndex] = 0;
										}
									}

									mergedVertIndex = ++curVertIdx;
								} while (curVertIdx < localVertCount);
							}

							if ((primObject->renderFlags & 32) == 0)
							{
								int32_t triangleIdx = 0;

								if (primCount / 3)
								{
									int32_t triMetaIdx = 0;

									do
									{
										Nu3D::Math::CalculatePlaneFromTriangle(
											&vertices_[headerIndices[0]].position,
											&vertices_[headerIndices[1]].position,
											&vertices_[headerIndices[2]].position,
											&primObject->header->triMeta[triMetaIdx]);

										headerIndices += 3;
										triMetaIdx = ++triangleIdx;

									} while (triangleIdx < primCount / 3);
								}
							}

							primObject = primObject;
							verticesSize = localVertCount;
						}

						if ((g_curVertexFlags & 4) != 0)
						{
							if (Nu3D::g_useAsDiffuseModulation)
							{
								int32_t primCount = 0;

								if (verticesSize > 0)
								{
									int32_t vertexLoopIndex = 0;

									do
									{
										float red = g_curMaterialList[materialIndex]->d3dMaterial.diffuse.r * 255.0;

										if (red >= 255.0)
											red = 255.0;

										primObject->patchVerts.data.vertices[vertexLoopIndex].diffuse.r = red;
										float green = g_curMaterialList[materialIndex]->d3dMaterial.diffuse.g * 255.0;

										if (green >= 255.0)
											green = 255.0;

										primObject->patchVerts.data.vertices[vertexLoopIndex].diffuse.g = green;
										float blue = g_curMaterialList[materialIndex]->d3dMaterial.diffuse.b * 255.0;

										if (blue >= 255.0)
											blue = 255.0;

										primObject->patchVerts.data.vertices[vertexLoopIndex].diffuse.b = blue;
										float alpha = g_curMaterialList[materialIndex]->d3dMaterial.diffuse.a * 255.0;

										if (alpha >= 255.0)
											alpha = 255.0;

										++primCount;
										primObject->patchVerts.data.vertices[vertexLoopIndex].diffuse.a = alpha;
										vertexLoopIndex = primCount;

									} while (primCount < verticesSize);
								}
							}
						}

						curPrim = processedPrimCount;
						primObject->listNext = primListHead;
						primListHead = primObject;

						goto LBL_NEXT_PRIM;
					}

					int16_t* primConvertData = &g_primVertexData[primVertexIndex];

					do
					{
						primConvertData -= 4;
						indexData -= 4;

						indexData[0] = primConvertData[3];
						indexData[1] = primConvertData[0];
						indexData[2] = primConvertData[2];
						indexData[3] = primConvertData[1];

					} while (indexData != g_indexDataConversion);
				}
				else
				{
					indexData = g_primVertexData;
				}

				indexCount = primCount;
				goto LBL_EMIT_INDICES;
			}

		LBL_FINISH:

			if (totalLocalVertCount != g_curVertexCount)
				Logger::DebugLog("VTX Duplications From %d To %d\r", g_curVertexCount, totalLocalVertCount);

			g_curPrimObject = primObject;
			return 1;
		}

		// FUNCTION: TOY2 0x004CB270
		void PrepareGlobals()
		{
			g_curVertexCount = 0;
			g_curMaterialCount = 0;

			g_unused1 = 0;
			g_curPrimObject = 0;

			g_isHardwareRendering = Renderer::g_isSoftwareRendering == 0;
		}

		// FUNCTION: TOY2 0x004CC970
		Nu3D::Primitive* ExtractShapeData(FILE* stream)
		{
			PrepareGlobals();

			int32_t stop = 0;
			int32_t chunkId;

			if (fread(&chunkId, sizeof(int32_t), 1, stream))
			{
				do
				{
					int32_t size;

					fread(&size, sizeof(int32_t), 1, stream);
					ftell(stream);

					switch (chunkId)
					{
						case 0:
							stop = 1;
							break;

						case 64:
							ExtractShapeName(stream);
							break;

						case 65:
							ExtractShapeTextures(stream);
							break;

						case 66:
							ExtractShapeMaterials(stream);
							break;

						case 67:
							ExtractShapeVertices(stream);
							break;

						case 68:
							ExtractShapePrimitives(stream);
							break;

						default:
							fseek(stream, size, SEEK_CUR);
							break;
					}

					ftell(stream);

				} while (! stop && fread(&chunkId, sizeof(int32_t), 1, stream));
			}

			Nu3D::Primitive* result = g_curPrimObject;

			if (g_curPrimObject)
			{
				Nu3D::Primitive::ComputeBounds(g_curPrimObject);
				return g_curPrimObject;
			}

			return result;
		}

		// FUNCTION: TOY2 0x004CB940
		Nu3D::Material* GetCurrentMatByIndex(uint32_t index)
		{
			if (index >= g_curMaterialCount || index < 0)
				return 0;
			else
				return g_curMaterialList[index];
		}
	}
}
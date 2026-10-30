#include "NGNLoader/NGNLoader.h"
#include "Nu3D/Portal.h"
#include "Toy2/Toy2.h"
#include "Nu3D/BmpDataNode.h"
#include "Nu3D/Patch.h"
#include "Nu3D/Creature.h"
#include "Logger.h"
#include "Renderer/Glue.h"
#include "Renderer/Renderer.h"
#include "Nu3D/Math.h"
#include "NGNLoader/ObjectLoad.h"

#include <windows.h>
#include <stddef.h>
#include <string.h>

namespace NGNLoader
{
	// GLOBAL: TOY2 0x00B62410
	NGNImage* g_ngnImage;

	// GLOBAL: TOY2 0x009F6240
	NGNTextureData g_textureDataFreeList[2000];

	// GLOBAL: TOY2 0x00A03D00
	NGNTextureDataSentinal g_textureData;

	// GLOBAL: TOY2 0x00A03D08
	NGNTextureCache g_textureCacheFreeList[1000];

	// GLOBAL: TOY2 0x00A4C148
	NGNTextureCacheSentinal g_textureCache;

	// GLOBAL: TOY2 0x00AAD7AC
	char* g_curFileName;

	// GLOBAL: TOY2 0x00AAD7A8;
	int32_t g_curPrimCount;

	// GLOBAL: TOY2 0x00508A58
	Vector3F g_vertexScaleVector = { 1.0, 1.0, 1.0 };

	// FUNCTION: TOY2 0x004B1190
	void FreeAllBmpDataNodes() { Nu3D::FreeAllBmpDataNodes_T(); }

	// FUNCTION: TOY2 0x004CB300
	void GetScaleVector(Vector3F* output) { *output = g_vertexScaleVector; }

	// FUNCTION: TOY2 0x004BB320
	NGNTextureData* GetTextureData(NGNTextureParams* texParams, int32_t ignoreParams)
	{
		NGNTextureParams localTexParams;
		NGNTextureCache* cacheHead = g_textureCache.activeList;

		memcpy(&localTexParams, texParams, sizeof(localTexParams));
		localTexParams.rawTexStr = 0;

		if (! g_textureCache.activeList)
			return 0;

		while (! ignoreParams && memcmp(&cacheHead->params, &localTexParams, sizeof(NGNTextureParams)) || strcmpi(cacheHead->texName, texParams->rawTexStr))
		{
			cacheHead = cacheHead->next;

			if (! cacheHead)
				return 0;
		}

		NGNTextureData* result = g_textureData.activeList;

		if (! g_textureData.activeList)
			return 0;

		while (result->textureCacheIndex != cacheHead->textureIndex)
		{
			result = result->next;

			if (! result)
				return 0;
		}

		return result;
	}

	// FUNCTION: TOY2 0x004BB4C0
	NGNTextureCache* AllocateTextureCache()
	{
		NGNTextureCache* result = g_textureCache.freeList;

		if (g_textureCache.freeList)
		{
			g_textureCache.freeList = g_textureCache.freeList->next;
			result->next = g_textureCache.activeList;

			if (g_textureCache.activeList)
				g_textureCache.activeList->prev = result;

			result->prev = 0;
			g_textureCache.activeList = result;
		}

		return result;
	}

	// FUNCTION: TOY2 0x004BB1B0
	NGNTextureData* AllocateTextureData()
	{
		NGNTextureData* result = g_textureData.freeList;

		if (g_textureData.freeList)
		{
			g_textureData.freeList = g_textureData.freeList->next;
			result->next = g_textureData.activeList;

			if (g_textureData.activeList)
				g_textureData.activeList->prev = result;

			result->prev = 0;
			g_textureData.activeList = result;
		}

		return result;
	}

	// FUNCTION: TOY2 0x004AC240
	Nu3D::BmpDataNode* LoadLocalBmpTexture(const char* rawTexStr, int32_t flags) { return Nu3D::LoadLocalBmpTexture(rawTexStr, flags); }

	// FUNCTION: TOY2 0x004BB3C0
	uint32_t GetOrAllocateTexture(NGNTextureParams* texParams)
	{
		char rawTexStrBuffer[256];

		NGNTextureData* cachedTextureData = GetTextureData(texParams, 0);

		// If its already cached, return it
		if (! cachedTextureData)
		{
			NGNTextureCache* textureCache = AllocateTextureCache();

			// Build a new entry if its new
			if (textureCache)
			{
				memcpy(&textureCache->params, texParams, sizeof(textureCache->params));

				textureCache->params.rawTexStr = 0;

				strcpy(textureCache->texName, texParams->rawTexStr);

				NGNTextureData* textureData = AllocateTextureData();

				if (textureData)
				{
					textureData->isTex14 = texParams->isTex14;
					textureData->color.b = texParams->color.b;
					textureData->color.g = texParams->color.g;
					textureData->color.r = texParams->color.r;

					strcpy(rawTexStrBuffer, texParams->rawTexStr);

					int32_t flags;

					if ((texParams->isTex14 & 2) != 0)
						flags = 1;
					else
						flags = 2 * (texParams->isTex14 & 4);

					textureData->bmpDataNode = LoadLocalBmpTexture(rawTexStrBuffer, flags);
					textureData->textureCacheIndex = textureCache->textureIndex;

					return textureData->textureIndex;
				}
			}

			return 0;
		}

		return cachedTextureData->textureIndex;
	}

	// FUNCTION: TOY2 0x004AC220
	Nu3D::BmpDataNode* LoadTextureContents(FILE* stream, const char* rawTexStr, int32_t flags) { return Nu3D::LoadTextureByStream(stream, rawTexStr, flags); }

	// FUNCTION: TOY2 0x004BC320
	Nu3D::Portal::PortalState* AllocAreaPortal(NGNImage* ngnImage)
	{
		if (ngnImage->portalEntryCount < ngnImage->areaPortalCount && ngnImage->portalStatePool)
			return &ngnImage->portalStatePool[ngnImage->portalEntryCount++];

		return 0;
	}

	// FUNCTION: TOY2 0x004BC2C0
	int32_t InsertPortal(NGNImage* ngnImage, int32_t sourceAreaIdx, int32_t targetAreaIdx, Nu3D::Portal::AreaPortal* portal)
	{
		if (Toy2::g_isElevatorHopLevel && targetAreaIdx == 15)
			targetAreaIdx = -1;

		Nu3D::Portal::PortalState* head = AllocAreaPortal(ngnImage);

		if (head)
		{
			head->targetAreaIdx = targetAreaIdx;
			head->portal = portal;
			head->sourceAreaIdx = sourceAreaIdx;

			head->next = ngnImage->portalHashTable->buckets[sourceAreaIdx].portalStateHead;
			ngnImage->portalHashTable->buckets[sourceAreaIdx].portalStateHead = head;

			return 1;
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004BC230
	void AllocPools(NGNImage* ngnImage, int32_t portalCount, int32_t maxScalerEntries)
	{
		ngnImage->portalEntryCount = 0;
		ngnImage->scalerEntryCount = 0;

		ngnImage->scalerEntryPool = (Nu3D::Portal::ScalerEntry*)malloc(sizeof(Nu3D::Portal::ScalerEntry) * maxScalerEntries);
		ngnImage->maxScalerEntries = maxScalerEntries;

		ngnImage->scalerEntryCount = 0;
		ngnImage->portalStatePool = (Nu3D::Portal::PortalState*)malloc(sizeof(Nu3D::Portal::PortalState) * portalCount);
		ngnImage->portalEntryCount = 0;
		ngnImage->areaPortalCount = portalCount;

		Nu3D::Portal::PortalHashTable* rotLookup = (Nu3D::Portal::PortalHashTable*)malloc(sizeof(Nu3D::Portal::PortalHashTable));

		ngnImage->portalHashTable = rotLookup;
		ngnImage->bucketCount = 64;

		memset(rotLookup, 0, sizeof(Nu3D::Portal::PortalHashTable));
	}

	// FUNCTION: TOY2 0x004B3350
	Nu3D::Portal::AreaPortal* AllocPortalVertices(int32_t vertexCount)
	{
		Nu3D::Portal::AreaPortal* portal = (Nu3D::Portal::AreaPortal*)malloc(sizeof(Vector3F) * vertexCount + sizeof(Nu3D::Portal::AreaPortal));

		if (portal)
		{
			memset(portal, 0, sizeof(Vector3F) * vertexCount + sizeof(Nu3D::Portal::AreaPortal));

			if (vertexCount)
			{
				// Points to space after struct, is the vertex space.
				portal->vertexCount = vertexCount;
				portal->vertices = reinterpret_cast<Vector3F*>(&portal[1]);
			}
		}

		return portal;
	}

	// FUNCTION: TOY2 0x004C4080
	void ParseTextures(FILE* stream, NGNImage* ngnImage)
	{
		NGNTextureParams texParams;
		char rawTexStr[256];

		memset(&texParams, 0, sizeof(texParams));
		texParams.rawTexStr = rawTexStr;

		int32_t textureCount;
		fread(&textureCount, sizeof(int32_t), 1, stream);

		for (uint32_t textureIndex = textureCount; textureIndex != 0; --textureIndex)
		{
			uint32_t dataOffset;
			uint32_t rawTexStrLen;

			fread(&dataOffset, sizeof(uint32_t), 1, stream);
			fread(&rawTexStrLen, sizeof(uint32_t), 1, stream);
			fread(rawTexStr, sizeof(char), rawTexStrLen, stream);

			rawTexStr[rawTexStrLen] = '\0';

			strlwr(rawTexStr);
			int32_t textureId = atoi(&rawTexStr[3]);

			int32_t flags = 0;
			int32_t isBGR = 0;

			if (textureId != 14 && textureId != 36 && textureId != 37)
				flags = 8;

			if (textureId > 31)
				flags |= 32;

			if (strstr(rawTexStr, "bgr"))
			{
				memcpy(rawTexStr, "tex", 3);
				isBGR = 1;
			}

			int32_t beforeOffset = ftell(stream);

			LoadTextureContents(stream, rawTexStr, flags);

			int32_t afterOffset = ftell(stream);

			if (afterOffset - beforeOffset != dataOffset)
				Logger::GetErrorHandler("C:\\projects\\nu3d\\world.c", 573)("bitmap parsed incorrectly in file %s", g_curFileName);

			texParams.isTex14 = strcmpi("tex14", rawTexStr) == 0;

			int32_t index = GetOrAllocateTexture(&texParams);

			if (textureId >= 0 && textureId < 64)
			{
				ngnImage->textureEntries[textureId].isBGR = isBGR;
				ngnImage->textureEntries[textureId].textureDataIndex = index;
			}
		}
	}

	// FUNCTION: TOY2 0x004C3EB0
	int32_t ParseLinker(FILE* stream, NGNImage* ngnImage)
	{
		int32_t maxLinkId;
		int32_t linkCount;

		fread(&linkCount, sizeof(int32_t), 1, stream);
		fread(&maxLinkId, sizeof(int32_t), 1, stream);

		++maxLinkId;

		Nu3D::Link::Linker* linkerArray = (Nu3D::Link::Linker*)malloc(sizeof(Nu3D::Link::Linker) * maxLinkId);

		ngnImage->links = linkerArray;

		if (! linkerArray)
			return 0;

		int32_t count = 0;

		memset(linkerArray, 0, sizeof(Nu3D::Link::Linker) * maxLinkId);

		ngnImage->maxLinkId = maxLinkId;

		if (linkCount > 0)
		{
			do
			{
				int16_t linkId;
				uint16_t shapeId;

				fread(&linkId, sizeof(int16_t), 1, stream);
				fread(&shapeId, sizeof(uint16_t), 1, stream);

				if (linkId >= 0 && linkId < maxLinkId)
				{
					int32_t type;

					if ((shapeId & 0x8000) == 0)
					{
						type = 0;
					}
					else
					{
						shapeId = -shapeId & 0x3FFF;
						type = 1;
					}

					if (shapeId < ngnImage->shapeCounts[type])
					{
						Nu3D::Link::Linker* link = &ngnImage->links[linkId];
						Nu3D::Link::DynamicScaler* dynamicScaler = &ngnImage->dynamicScalers[type][shapeId];

						link->dynamicScaler = dynamicScaler;

						memcpy(&link->transformMatrix, &dynamicScaler->transformMatrix, sizeof(link->transformMatrix));

						link->dynamicScaler->flags |= 2;

						link->currentPos = link->dynamicScaler->translation;
						link->targetPos = link->dynamicScaler->translation;
						link->currentRot = link->dynamicScaler->rotation;
						link->currentScale = link->dynamicScaler->scale;
					}
				}

				count += 1;

			} while (count < linkCount);
		}

		return linkCount;
	}

	// FUNCTION: TOY2 0x004CA040
	int32_t ExtractAnimations(FILE* stream, Nu3D::Creature* creature, int32_t chunkSize)
	{
		if (! creature->animData)
		{
			creature->animData = (void**)malloc(400);
			creature->animCount = 0;

			if (! creature->animData)
				return -1;
		}

		if (creature->animCount >= 100)
			return -1;

		creature->animData[creature->animCount] = malloc(chunkSize);

		if (! creature->animData[creature->animCount])
			return -1;

		fread(creature->animData[creature->animCount], 1, chunkSize, stream);

		return ++creature->animCount;
	}

	// FUNCTION: TOY2 0x004CA1D0
	int32_t ExtractShapePatch(FILE* stream, Nu3D::Creature* creature)
	{
		int16_t vertexCount;
		int16_t materialIndex;
		int32_t patchCount;
		int16_t indices1[4];
		int16_t indices2[4];

		fread(&patchCount, 4, 1, stream);

		int32_t patchIndex = 0;

		if (patchCount > 0)
		{
			do
			{
				Nu3D::Patch* patch;

				fread(&vertexCount, 2, 1, stream);
				fread(&materialIndex, 2, 1, stream);
				fread(indices1, 2, 4, stream);
				fread(indices2, 2, 4, stream);

				if (vertexCount)
				{
					patch = Nu3D::Patch::AllocAndResize(2 * vertexCount, 2);

					if (patch)
					{
						int32_t index = 0;

						if (vertexCount > 0)
						{
							int32_t increment = 0;

							do
							{
								Nu3D::CopyShapeVertex(index + indices2[0], &patch->patchVertices.data.vertices[increment]);
								Nu3D::CopyNormalsFromNearestVertex(creature, indices1[0], &patch->patchVertices.data.vertices[increment]);

								Nu3D::CopyShapeVertex(index + indices2[1], &patch->patchVertices.data.vertices[index + vertexCount]);
								Nu3D::CopyNormalsFromNearestVertex(creature, indices1[1], &patch->patchVertices.data.vertices[index + vertexCount]);

								++index;
								++increment;
							} while (index < vertexCount);
						}
					}
				}
				else
				{
					patch = Nu3D::Patch::AllocAndResize(4, 4);

					if (patch)
					{
						Nu3D::CopyShapeVertex(indices2[0], patch->patchVertices.data.vertices);
						Nu3D::CopyNormalsFromNearestVertex(creature, indices1[0], patch->patchVertices.data.vertices);

						Nu3D::CopyShapeVertex(indices2[1], patch->patchVertices.data.vertices + 1);
						Nu3D::CopyNormalsFromNearestVertex(creature, indices1[1], patch->patchVertices.data.vertices + 1);

						Nu3D::CopyShapeVertex(indices2[2], patch->patchVertices.data.vertices + 2);
						Nu3D::CopyNormalsFromNearestVertex(creature, indices1[2], patch->patchVertices.data.vertices + 2);

						Nu3D::CopyShapeVertex(indices2[3], patch->patchVertices.data.vertices + 3);
						Nu3D::CopyNormalsFromNearestVertex(creature, indices1[3], patch->patchVertices.data.vertices + 3);
					}
				}

				if (patch)
				{
					// NOTE: the original writes every other short of indexData, the last two writes land in the fields that follow it
					int16_t* sourceIndices = indices1;
					int16_t* indexData = patch->indexData;
					int32_t maxIndices = 4;

					do
					{
						*(int32_t*)indexData = *sourceIndices;
						indexData += 2;
						++sourceIndices;
						--maxIndices;
					} while (maxIndices);

					patch->materialId = ObjectLoad::GetCurrentMatByIndex(materialIndex)->id;
					patch->listNext = creature->patch;
					creature->patch = patch;

					Nu3D::Patch::CreateAllVertexBuffers(patch);
				}
				else
				{
					Logger::GetErrorHandler("C:\\projects\\nu3d\\hobjload.c", 220)("unable to create patch");
				}

				++patchIndex;
			} while (patchIndex < patchCount);
		}

		return patchIndex;
	}

	// FUNCTION: TOY2 0x004CA420
	Nu3D::Creature* ExtractCreatureData(FILE* stream)
	{
		int32_t exitLoop = 0;
		Nu3D::Creature* creature = 0;
		int32_t chunkId;
		int32_t chunkSize;

		if (fread(&chunkId, 4, 1, stream))
		{
			do
			{
				fread(&chunkSize, 4, 1, stream);

				int32_t nextChunkOffset = chunkSize + ftell(stream);

				if (chunkId > 516)
				{
					switch (chunkId)
					{
						case 517:
							ObjectLoad::PrepareGlobals();
							ObjectLoad::ExtractShapeTextures(stream);
							break;

						case 518:
							ObjectLoad::ExtractShapeMaterials(stream);
							break;

						case 519:
							ObjectLoad::ExtractShapeVertices(stream);
							break;

						case 520:
							ExtractShapePatch(stream, creature);
							break;

						default:
							goto LABEL_51;
					}

					goto NEXT_CHUNK_LBL;
				}

				if (chunkId == 516)
				{
					ExtractAnimations(stream, creature, chunkSize);
					goto NEXT_CHUNK_LBL;
				}

				if (chunkId > 513)
				{
					if (chunkId == 514)
					{
						// the names of the nodes
						if (creature)
						{
							creature->dataFlags |= 2;

							creature->nodeNames = (char**)malloc(4 * creature->nodeCount);

							if (creature->nodeNames)
							{
								for (int32_t nodeNameIndex = 0; nodeNameIndex < creature->nodeCount; ++nodeNameIndex)
								{
									uint8_t nodeNameLen;

									fread(&nodeNameLen, 1, 1, stream);

									creature->nodeNames[nodeNameIndex] = (char*)malloc(nodeNameLen + 1);

									if (creature->nodeNames[nodeNameIndex])
									{
										fread(creature->nodeNames[nodeNameIndex], nodeNameLen, 1, stream);
										creature->nodeNames[nodeNameIndex][nodeNameLen] = 0;
									}
								}
							}
						}

						goto NEXT_CHUNK_LBL;
					}

					// the shapes of the nodes
					if (creature)
					{
						creature->dataFlags |= 4;

						creature->primitives = (Nu3D::Primitive**)malloc(4 * creature->nodeCount);
						creature->unkVar10 = (int32_t**)malloc(4 * creature->nodeCount);

						if (creature->primitives)
						{
							int32_t nodeIndex = 0;

							if (creature->nodeCount > 0)
							{
								do
								{
									uint16_t nodeFlags;

									fread(&nodeFlags, 1, 2, stream);

									creature->unkVar10[nodeIndex] = 0;

									fread(&creature->unkVar10[nodeIndex], 1, 2, stream);

									if (nodeFlags & 1)
									{
										if (nodeFlags & 2)
											creature->flagsList[nodeIndex] = (int32_t*)((int32_t)creature->flagsList[nodeIndex] | 2);

										if (nodeFlags & 4)
											creature->flagsList[nodeIndex] = (int32_t*)((int32_t)creature->flagsList[nodeIndex] | 4);

										creature->primitives[nodeIndex] = ObjectLoad::ExtractShapeData(stream);

										Nu3D::Primitive::CreateAllVertexBuffers(creature->primitives[nodeIndex], 3);
									}
									else
									{
										creature->primitives[nodeIndex] = 0;
									}

									++nodeIndex;
								} while (nodeIndex < creature->nodeCount);
							}
						}
					}
				}
				else
				{
					if (chunkId != 513)
					{
						if (chunkId)
						{
							if (chunkId == 512)
							{
								// the start of a creature
								creature = (Nu3D::Creature*)malloc(48);

								if (creature)
								{
									memset(creature, 0, sizeof(Nu3D::Creature));
									creature->dataFlags = 0;
									fread(&creature->nodeCount, 1, 4, stream);

									creature->flagsList = (int32_t**)malloc(4 * creature->nodeCount);

									if (creature->flagsList)
										memset(creature->flagsList, 0, 4 * creature->nodeCount);
								}
							}
							else
							{
							LABEL_51:
								Logger::GetErrorHandler("C:\\projects\\nu3d\\hobjload.c", 354)("err");
								fseek(stream, chunkSize, 1);
							}
						}
						else
						{
							exitLoop = 1;
						}

						goto NEXT_CHUNK_LBL;
					}

					// the matrices of the nodes
					if (creature)
					{
						creature->dataFlags |= 1;

						creature->matrixList1 = (D3DMATRIX*)malloc(creature->nodeCount << 6);
						creature->matrixList2 = (D3DMATRIX*)malloc(creature->nodeCount << 6);
						creature->matrixList3 = (D3DMATRIX*)malloc(creature->nodeCount << 6);

						if (creature->matrixList1)
						{
							if (creature->matrixList3 && creature->matrixList2)
							{
								fread(creature->matrixList1, creature->nodeCount, 64, stream);
								memcpy(creature->matrixList3, creature->matrixList1, creature->nodeCount << 6);

								for (int32_t matrixIndex = 0; matrixIndex < creature->nodeCount; matrixIndex++)
									Nu3D::Math::BuildIdentityMatrix(&creature->matrixList2[matrixIndex]);
							}
						}
					}
				}

			NEXT_CHUNK_LBL:
				if (ftell(stream) != nextChunkOffset)
					fseek(stream, nextChunkOffset, 0);
			} while (! exitLoop && fread(&chunkId, 4, 1, stream));
		}

		return creature;
	}

	// FUNCTION: TOY2 0x004C4220
	uint32_t ParseCreatures(FILE* stream, NGNImage* ngnImage)
	{
		uint32_t creatureCount;
		fread(&creatureCount, 1, sizeof(uint32_t), stream);

		ngnImage->creatureCount = creatureCount;

		if (creatureCount)
		{
			Nu3D::Creature** creatureArray = (Nu3D::Creature**)malloc(sizeof(Nu3D::Creature*) * creatureCount);
			ngnImage->creatureData = creatureArray;

			if (! creatureArray)
				Logger::GetErrorHandler("C:\\projects\\nu3d\\world.c", 617)("unable to alloc space for %d creatures", creatureCount);

			memset(ngnImage->creatureData, 0, sizeof(Nu3D::Creature*) * creatureCount);

			uint8_t creatureValidFlags[512];
			memset(creatureValidFlags, 0, creatureCount);

			if ((int32_t)creatureCount > 0)
			{
				int32_t count = 0;

				do
				{
					int8_t charNameLen;
					fread(&charNameLen, 1, sizeof(int8_t), stream);

					if (charNameLen)
					{
						char charNameBuffer[256];
						fread(charNameBuffer, charNameLen, sizeof(char), stream);
						creatureValidFlags[count] = 1;
					}

					++count;

				} while (count < (int32_t)creatureCount);
			}

			for (int32_t index = 0; index < (int32_t)creatureCount; ++index)
			{
				if (creatureValidFlags[index])
					ngnImage->creatureData[index] = ExtractCreatureData(stream);
			}

			return creatureCount;
		}

		return creatureCount;
	}

	// FUNCTION: TOY2 0x004C3CA0
	void Parse266(FILE* stream, NGNImage* ngnImage)
	{
		// data266 isn't actually used anywhere in the game
		Vector3F scale;
		GetScaleVector(&scale);

		memset(ngnImage->data266, 0, sizeof(ngnImage->data266));

		int32_t entryCount = 0;
		fread(&entryCount, sizeof(entryCount), 1, stream);

		for (int32_t i = 0; i < entryCount; ++i)
		{
			int16_t count = 0;
			int16_t index = 0;

			fread(&count, sizeof(count), 1, stream);
			fread(&index, sizeof(index), 1, stream);

			Type266Entry* entry = (Type266Entry*)malloc(sizeof(Type266Entry));

			entry->count = 0;
			entry->index = 0;
			entry->points = 0;

			entry->count = count;
			entry->index = index;
			entry->points = (Vector3F*)malloc(sizeof(Vector3F) * count);

			const bool emptyCount = entry->count == 0;
			const bool negativeCount = entry->count < 0;

			if (! emptyCount && ! negativeCount)
			{
				for (int32_t j = 0; j < count; ++j)
				{
					fread(&entry->points[j].x, sizeof(float), 1, stream);
					fread(&entry->points[j].y, sizeof(float), 1, stream);
					fread(&entry->points[j].z, sizeof(float), 1, stream);

					entry->points[j].x *= scale.x;
					entry->points[j].y *= scale.y;
					entry->points[j].z *= scale.z;
				}
			}

			ngnImage->data266[index] = entry;
		}
	}

	// FUNCTION: TOY2 0x004C3DF0
	void ParseAreaPortalIdx(FILE* stream, NGNImage* ngnImage)
	{
		int32_t portalId, targetAreaIdx, sourceAreaIdx, portalCount;

		fread(&portalCount, sizeof(int32_t), 1, stream);

		do
		{
			if (! portalCount)
				break;

			AllocPools(ngnImage, portalCount, 4000);

			for (int32_t index = 0; index < portalCount; ++index)
			{
				fread(&portalId, sizeof(int32_t), 1, stream);
				fread(&sourceAreaIdx, sizeof(int32_t), 1, stream);
				fread(&targetAreaIdx, sizeof(int32_t), 1, stream);

				Nu3D::Portal::AreaPortal::CalculateBoundingSphere(ngnImage->areaPortals[portalId]);
				InsertPortal(ngnImage, sourceAreaIdx, targetAreaIdx, ngnImage->areaPortals[portalId]);
			}
		} while (0);
	}

	// FUNCTION: TOY2 0x004C3BE0
	void ParseAreaPortalPos(FILE* stream, NGNImage* ngnImage)
	{
		int32_t portalCount;
		fread(&portalCount, sizeof(int32_t), 1, stream);

		if (! portalCount)
			return;

		Nu3D::Portal::AreaPortal** portalAlloc = (Nu3D::Portal::AreaPortal**)malloc(sizeof(void*) * portalCount);
		ngnImage->areaPortals = portalAlloc;

		if (! portalAlloc)
			return;

		int32_t portalId = 0;

		if (portalCount > 0)
		{
			while (true)
			{
				int32_t vertexCount;
				fread(&vertexCount, sizeof(int32_t), 1, stream);

				ngnImage->areaPortals[portalId] = AllocPortalVertices(vertexCount);

				Nu3D::Portal::AreaPortal* portal = ngnImage->areaPortals[portalId];

				if (! portal)
					break;

				portal->portalId = portalId;

				fread(ngnImage->areaPortals[portalId++]->vertices, sizeof(Vector3F), vertexCount, stream);

				if (portalId >= portalCount)
					return;
			}

			ngnImage->actualPortalCount = portalId;
		}
	}

	// FUNCTION: TOY2 0x004C3740
	int32_t ParseGscale(FILE* stream, NGNImage* ngnImage)
	{
		Vector3F scaleVector;
		int32_t shapeDataLength;
		int32_t shapeCount;
		int32_t flipY;

		GetScaleVector(&scaleVector);

		int32_t flipX = scaleVector.x < 0.0f;
		int32_t curShape = 0;
		flipY = scaleVector.y < 0.0f;
		int32_t flipZ = scaleVector.z < 0.0f;

		fread(&shapeCount, sizeof(int32_t), 1, stream);
		fread(&shapeDataLength, sizeof(int32_t), 1, stream);

		shapeDataLength -= 40;
		int32_t hasExtraData = 0;

		if (shapeDataLength != 0)
		{
			hasExtraData = 1;
			shapeDataLength -= 4;
		}

		if (shapeCount)
		{
			ngnImage->dynamicScalers[ngnImage->gscaleType] = (Nu3D::Link::DynamicScaler*)malloc(sizeof(Nu3D::Link::DynamicScaler) * shapeCount);

			if (ngnImage->dynamicScalers[ngnImage->gscaleType] != 0)
			{
				ngnImage->shapeCounts[ngnImage->gscaleType] = shapeCount;

				if (shapeCount > curShape)
				{
					int32_t flipXOrig = flipX;
					int32_t flipYZ = flipZ ^ flipY;

					flipX = flipZ ^ flipX;
					flipY = flipY ^ flipXOrig;

					do
					{
						// yes, the original ASM does every single dynamicScalers load individually, which is odd because you would think
						// the compiler would just optimize it down to one load and then pointer re-use, but I guess not

						ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].flags = 0;

						fread(&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation, sizeof(Vector3F), 1, stream);
						fread(&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].rotation, sizeof(Vector3F), 1, stream);
						fread(&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].scale, sizeof(Vector3F), 1, stream);
						fread(&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].shapeId, sizeof(int32_t), 1, stream);

						ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].shapeId += g_curPrimCount;
						ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].gscaleType = ngnImage->gscaleType;

						if (hasExtraData)
						{
							fread(&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].packedAreaData, sizeof(int32_t), 1, stream);

							ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].packedFlags =
								(ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].packedAreaData >> 16) & 0xF;

							ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].areaIndex =
								ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].packedAreaData & 0xFF;
						}
						else
						{
							ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].packedAreaData = 0;
							ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].areaIndex = 0;
							ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].packedFlags = 0;
						}

						ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.x =
							scaleVector.x * ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.x;

						ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.y =
							scaleVector.y * ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.y;

						ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.z =
							scaleVector.z * ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.z;

						if (flipYZ)
							ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].rotation.x =
								-ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].rotation.x;

						if (flipX)
							ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].rotation.y =
								-ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].rotation.y;

						if (flipY)
							ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].rotation.z =
								-ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].rotation.z;

						Nu3D::Math::BuildIdentityMatrix(&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].transformMatrix);

						// clang-format off
					Nu3D::Math::ScaleMatrixByVector(
						&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].transformMatrix, 
						&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].scale
					);

					Nu3D::Math::RotateZFromLut(
						&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].transformMatrix,
						ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].rotation.z
					);

					Nu3D::Math::RotateYFromLut(
						&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].transformMatrix,
						ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].rotation.y
					);

					Nu3D::Math::PostRotateXFromLut(
						&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].transformMatrix,
						ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].rotation.x
					);

					Nu3D::Math::AddWorldSpaceTransform(
						&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].transformMatrix,
						&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation
					);

					ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].next = 0;

					Nu3D::Math::TransformVectorByMatrix(
						&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].boundsCenterWorld,
						&ngnImage->primitives[ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].shapeId]->boundsCenter,
						&ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].transformMatrix
					);
						// clang-format on

						if (curShape || ngnImage->gscaleType)
						{
							ngnImage->worldMinX = ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.x > ngnImage->worldMinX
								? ngnImage->worldMinX
								: ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.x;

							ngnImage->worldMaxX = ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.x < ngnImage->worldMaxX
								? ngnImage->worldMaxX
								: ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.x;

							ngnImage->worldMinZ = ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.z > ngnImage->worldMinZ
								? ngnImage->worldMinZ
								: ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.z;

							ngnImage->worldMaxZ = ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.z < ngnImage->worldMaxZ
								? ngnImage->worldMaxZ
								: ngnImage->dynamicScalers[ngnImage->gscaleType][curShape].translation.z;
						}
						else
						{
							ngnImage->worldMaxX = ngnImage->dynamicScalers[0]->translation.x;
							ngnImage->worldMinX = ngnImage->dynamicScalers[0]->translation.x;
							ngnImage->worldMaxZ = ngnImage->dynamicScalers[0]->translation.z;
							ngnImage->worldMinZ = ngnImage->dynamicScalers[0]->translation.z;
						}

						if (shapeDataLength)
							fseek(stream, shapeDataLength, 1);

						++curShape;

					} while (curShape < shapeCount);
				}

				ngnImage->worldMaxX = ngnImage->worldMaxX + 10.0f;
				ngnImage->worldMaxZ = ngnImage->worldMaxZ + 10.0f;
				ngnImage->worldMinX = ngnImage->worldMinX - 10.0f;
				ngnImage->worldMinZ = ngnImage->worldMinZ - 10.0f;

				return shapeCount;
			}
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004C35C0
	int32_t ParseGeometry(FILE* stream, NGNImage* ngnImage)
	{
		int32_t shapeCount;
		fread(&shapeCount, sizeof(int32_t), 1, stream);

		if (! shapeCount)
			return 0;

		g_curPrimCount = ngnImage->primCount;

		if (ngnImage->primCount)
		{
			shapeCount += ngnImage->primCount;

			Nu3D::Primitive** primList = (Nu3D::Primitive**)malloc(sizeof(Nu3D::Primitive*) * shapeCount);

			if (primList)
			{
				memcpy(primList, ngnImage->primitives, sizeof(Nu3D::Primitive*) * ngnImage->primCount);
				free(ngnImage->primitives);

				ngnImage->primitives = primList;
			}
		}
		else
		{
			ngnImage->primitives = (Nu3D::Primitive**)malloc(sizeof(Nu3D::Primitive*) * shapeCount);
		}

		if (! ngnImage->primitives)
			return 0;

		int32_t result = shapeCount;

		ngnImage->primCount = shapeCount;

		int32_t index = g_curPrimCount;

		if (g_curPrimCount < result)
		{
			do
			{
				ngnImage->primitives[index] = ObjectLoad::ExtractShapeData(stream);
				Nu3D::Primitive::CreateAllVertexBuffers(ngnImage->primitives[index], 3);

				result = shapeCount;
				++index;

			} while (index < shapeCount);
		}

		return result;
	}

	// FUNCTION: TOY2 0x004BB2F0
	uint32_t GetTextureDataIndexByName(char* texName)
	{
		NGNTextureParams texParams;
		texParams.rawTexStr = texName;

		NGNTextureData* textureData = GetTextureData(&texParams, 1);

		if (textureData)
			return textureData->textureIndex;
		else
			return 0;
	}

	// FUNCTION: TOY2 0x004B9630
	void BuildTex14(int32_t unused)
	{
		// the original never initialises the colour (it is whatever was on the stack), at runtime it is always black,
		// which is what keeps the reflection overlay faint, so use that to be deterministic
		RGBColor color;
#ifdef APPLY_FIXES
		color.r = 0.0;
		color.g = 0.0;
		color.b = 0.0;
#endif

		Nu3D::g_tex14Version1 = Nu3D::Material::CreateFromColor(&color);

		if (Nu3D::g_tex14Version1)
		{
			Nu3D::Material::SetOpacity(Nu3D::g_tex14Version1, 0.5);
			Nu3D::Material::AttachTexture(Nu3D::g_tex14Version1, GetTextureDataIndexByName("tex14"));
			Nu3D::g_tex14Version1->metadata |= 2;
		}

		Nu3D::g_tex14Version2 = Nu3D::Material::CreateFromColor(&color);

		if (Nu3D::g_tex14Version2)
		{
			Nu3D::Material::SetOpacity(Nu3D::g_tex14Version2, 0.5);
			Nu3D::Material::AttachTexture(Nu3D::g_tex14Version2, GetTextureDataIndexByName("tex14"));
			Nu3D::g_tex14Version2->metadata |= 16;
		}

		Nu3D::g_tex14Version3 = Nu3D::Material::CreateFromColor(&color);

		if (Nu3D::g_tex14Version3)
		{
			Nu3D::Material::SetOpacity(Nu3D::g_tex14Version3, 0.5);
			Nu3D::Material::AttachTexture(Nu3D::g_tex14Version3, GetTextureDataIndexByName("tex14"));
			Nu3D::g_tex14Version3->metadata |= 32;
		}
	}

	// FUNCTION: TOY2 0x004C36A0 [MATCHED]
	void BuildGrid(int32_t gridWidth, int32_t gridHeight, int32_t type, NGNImage* ngnImage)
	{
		Nu3D::Link::DynamicScaler** alloc = (Nu3D::Link::DynamicScaler**)malloc(sizeof(int32_t*) * gridHeight * gridWidth);

		ngnImage->spacialGrid[type] = alloc;

		if (&ngnImage->spacialGrid[0])
		{
			memset(alloc, 0, sizeof(int32_t*) * gridHeight * gridWidth);

			float cellWidth = (ngnImage->worldMaxX - ngnImage->worldMinX) / gridWidth;

			ngnImage->gridWidth = gridWidth;
			ngnImage->gridHeight = gridHeight;

			int32_t shape = 0;

			ngnImage->cellWidthInWorldUnits = cellWidth;
			ngnImage->cellHeightInWorldUnits = (ngnImage->worldMaxZ - ngnImage->worldMinZ) / gridHeight;

			if (ngnImage->shapeCounts[type] > 0)
			{
				do
				{
					Nu3D::Link::InsertScalerAtComputedCell(&ngnImage->dynamicScalers[type][shape], type, ngnImage);

					++shape;
				} while (shape < ngnImage->shapeCounts[type]);
			}
		}
	}

	// FUNCTION: TOY2 0x004C3240
	void BuildScalerEntries(NGNImage* ngnImage)
	{
		int32_t count = 0;

		while (ngnImage->dynamicScalers[count])
		{
			if (count >= 2)
				break;

			int32_t i = 0;

			if (ngnImage->shapeCounts[count] > 0)
			{
				do
				{
					Nu3D::Portal::AreaPortal::BuildScalerEntry(ngnImage, ngnImage->dynamicScalers[count][i].areaIndex, &ngnImage->dynamicScalers[count][i]);
					++i;
				} while (i < ngnImage->shapeCounts[count]);
			}

			++count;
		}
	}

	// FUNCTION: TOY2 0x004C33F0
	NGNImage* BuildImage(char* fileName)
	{
		int32_t terminate = 0;
		NGNImage* ngnImage = 0;

		g_curFileName = fileName;

		FILE* fileHandle = fopen(fileName, "rb");

		if (fileHandle)
		{
			ngnImage = (NGNImage*)malloc(sizeof(NGNImage));

			if (ngnImage)
			{
				memset(ngnImage, 0, sizeof(NGNImage));

				g_curPrimCount = 0;

				int32_t chunkSize;
				int32_t chunkHeaderId;

				if (fread(&chunkHeaderId, sizeof(int32_t), 1, fileHandle))
				{
					do
					{
						fread(&chunkSize, sizeof(int32_t), 1, fileHandle);
						ftell(fileHandle);

						DECOMP_PRINT(("[NGNLoader]: Loading chunk type %d\n", chunkHeaderId));

						switch (chunkHeaderId)
						{
							case 256:
								ParseGeometry(fileHandle, ngnImage);
								break;

							case 257:
								ParseGscale(fileHandle, ngnImage);
								++ngnImage->gscaleType;
								break;

							case 258:
								ParseAreaPortalPos(fileHandle, ngnImage);
								break;

							case 259:
								ParseAreaPortalIdx(fileHandle, ngnImage);
								break;

							case 266:
								Parse266(fileHandle, ngnImage);
								break;

							case 260:
								ParseTextures(fileHandle, ngnImage);
								break;

							case 261:
								ParseLinker(fileHandle, ngnImage);
								break;

							case 262:
								ParseCreatures(fileHandle, ngnImage);
								break;

							case 0:
								terminate = 1;
								break;

							default:
								fseek(fileHandle, chunkSize, 1);
								break;
						}

						ftell(fileHandle);

					} while (! terminate && fread(&chunkHeaderId, sizeof(int32_t), 1, fileHandle));
				}
			}

			fclose(fileHandle);
		}

		BuildTex14(14);

		if (ngnImage)
		{
			for (int32_t typeIndex = 0; typeIndex < ngnImage->gscaleType; ++typeIndex)
				BuildGrid(20, 20, typeIndex, ngnImage);

			BuildScalerEntries(ngnImage);
		}

		return ngnImage;
	}

	// FUNCTION: TOY2 0x004CEAE0
	void SetNewImage(char* fileName)
	{
		DECOMP_PRINT(("[NGNLoader]: Loading file %s\n", fileName));

		g_ngnImage = BuildImage(fileName);

		Nu3D::Portal::ClearVisibleAreaFlags();
	}

	// FUNCTION: TOY2 0x004BB720
	void Init()
	{
		FreeAllBmpDataNodes();

		uint32_t textureDataCount = 1;
		NGNTextureData* dataFreeListPtr = g_textureDataFreeList;

		do
		{
			++textureDataCount;
			dataFreeListPtr->next = dataFreeListPtr + 1;
			dataFreeListPtr[1].prev = dataFreeListPtr;
			dataFreeListPtr[1].textureIndex = textureDataCount;
			++dataFreeListPtr;

		} while (dataFreeListPtr < &g_textureDataFreeList[1999]);

		g_textureDataFreeList[0].textureIndex = 1;
		g_textureDataFreeList[0].prev = 0;
		g_textureDataFreeList[textureDataCount - 1].next = 0;

		g_textureData.freeList = g_textureDataFreeList;
		g_textureData.activeList = 0;

		uint32_t textureCacheCount = 1;
		NGNTextureCache* cacheFreeListPtr = g_textureCacheFreeList;

		do
		{
			cacheFreeListPtr->next = cacheFreeListPtr + 1;
			cacheFreeListPtr[1].prev = cacheFreeListPtr;
			cacheFreeListPtr[1].textureIndex = textureCacheCount;
			++cacheFreeListPtr;
			++textureCacheCount;

		} while (cacheFreeListPtr < &g_textureCacheFreeList[999]);

		g_textureCacheFreeList[0].textureIndex = textureCacheCount;
		g_textureCacheFreeList[0].prev = 0;

		g_textureCache.freeList = g_textureCacheFreeList;
		g_textureCache.activeList = 0;

		g_textureCacheFreeList[999].next = 0;
	}

	// FUNCTION: TOY2 0x0044FF50 [MATCHED]
	void DetectBackdropTextures()
	{
		if (GetTextureDataIndex(36))
		{
			Toy2::g_hasBackdrop = 2;
			Toy2::g_nextBackdropId = 36;
		}

		if (GetTextureDataIndex(37))
		{
			Toy2::g_hasStaticBackdrop = 1;
			Toy2::g_nextBackdropId = 37;
		}

		if (! Toy2::g_hasBackdrop)
		{
			int32_t idx;

			for (idx = 40; idx < 48; ++idx)
			{
				if (GetTextureDataIndex(idx))
				{
					Toy2::g_hasBackdrop = 1;
					Toy2::g_nextBackdropId = idx;
					break;
				}
			}

			if (! Toy2::g_hasBackdrop)
			{
				for (idx = 88; idx < 96; ++idx)
				{
					if (GetTextureDataIndex(idx))
					{
						Toy2::g_hasBackdrop = 2;
						Toy2::g_nextBackdropId = idx;
						break;
					}
				}
			}
		}

		if (Toy2::g_hasStaticBackdrop)
			Renderer::Glue::SetBackdrop(Toy2::g_nextBackdropId);
	}

	// FUNCTION: TOY2 0x004CE2C0
	int32_t GetTextureDataIndex(int32_t textureIndex)
	{
		if (g_ngnImage && textureIndex >= 0 && textureIndex < 64)
			return g_ngnImage->textureEntries[textureIndex].textureDataIndex;
		else
			return 0;
	}

	// FUNCTION: TOY2 0x004CE2F0
	int32_t GetTextureAuxiliaryData(int32_t textureIndex, int32_t* auxiliaryOut)
	{
		NGNImage* volatile* imagePointer = &g_ngnImage;
		NGNImage* image = *imagePointer;

		if (image && textureIndex >= 0 && textureIndex < 64 && image->textureEntries[textureIndex].auxiliaryValue)
		{
			*auxiliaryOut = image->textureEntries[textureIndex].auxiliaryData;
			return (*imagePointer)->textureEntries[textureIndex].auxiliaryValue;
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004BB0E0
	NGNTextureData* GetTextureDataByIndex(uint32_t texDataIndex)
	{
		// Texture zero aliases the frustum object immediately before the original pool.
		if (! texDataIndex)
			return (NGNTextureData*)&Nu3D::Viewport::g_fustrumInfo;

		return &g_textureDataFreeList[texDataIndex - 1];
	}

	// FUNCTION: TOY2 0x004BB5E0
	void RetrieveTextureData(
		int32_t texDataIndex,
		uint32_t* bitmapWidthOut,
		uint32_t* bitmapHeightOut,
		uint32_t* textureWidth,
		uint32_t* textureHeight,
		uint32_t** textureData)
	{
		if (texDataIndex)
		{
			Nu3D::BmpDataNode* bmpDataNode = g_textureDataFreeList[texDataIndex - 1].bmpDataNode;

			if (bmpDataNode)
			{
				if (bitmapWidthOut)
					*bitmapWidthOut = bmpDataNode->bitmapWidth;

				if (bitmapHeightOut)
					*bitmapHeightOut = bmpDataNode->bitmapHeight;

				if (textureWidth)
					*textureWidth = bmpDataNode->textureWidth;

				if (textureHeight)
					*textureHeight = bmpDataNode->textureHeight;

				if (textureData)
					*textureData = bmpDataNode->texData;
			}
		}
	}

	// FUNCTION: TOY2 0x004BB6C0
	HBITMAP GetBmpHandle(int32_t index)
	{
		if (index)
			return g_textureDataFreeList[index - 1].bmpDataNode->bitmapHandle;
		else
			return 0;
	}

	// FUNCTION: TOY2 0x004BB690
	int32_t CopyToDDSurfaceByIndex(int32_t texIndex, LPDIRECTDRAWSURFACE4 ddSurface)
	{
		if (texIndex)
			return Nu3D::CopyToDDSurface(g_textureDataFreeList[texIndex - 1].bmpDataNode, ddSurface);
		else
			return 0;
	}

	// FUNCTION: TOY2 0x004BB150
	NGNTextureData* ReleaseTextureData(NGNTextureData* list)
	{
		if (list->bmpDataNode)
			Nu3D::ReleaseBmpDataNode_T(list->bmpDataNode);

		NGNTextureData* next = list->next;
		list->bmpDataNode = 0;

		if (next)
			next->prev = list->prev;

		NGNTextureData* result = list->prev;

		if (result)
		{
			result->next = list->next;
			result = g_textureData.freeList;
			list->next = result;
		}
		else
		{
			g_textureData.activeList = list->next;
			list->next = g_textureData.freeList;
		}

		g_textureData.freeList = list;

		return result;
	}

	// FUNCTION: TOY2 0x004BB810
	void ReleaseTextureCache(NGNTextureCache* textureCache)
	{
		if (textureCache->next)
			textureCache->next->prev = textureCache->prev;

		NGNTextureCache* prev = textureCache->prev;

		if (prev)
			prev->next = textureCache->next;
		else
			g_textureCache.activeList = textureCache->next;

		textureCache->next = g_textureCache.freeList;
		g_textureCache.freeList = textureCache;
	}

	// FUNCTION: TOY2 0x004BB7D0
	void ReleaseAllTextures()
	{
		for (NGNTextureData* curTexDatas = g_textureData.activeList; g_textureData.activeList; curTexDatas = g_textureData.activeList)
			ReleaseTextureData(curTexDatas);

		for (NGNTextureCache* curTexCache = g_textureCache.activeList; g_textureCache.activeList; curTexCache = g_textureCache.activeList)
			ReleaseTextureCache(curTexCache);

		Nu3D::FreeAllBmpDataNodes_T();
	}

	STATIC_ASSERT(sizeof(NGNTextureData) == sizeof(Nu3D::Viewport::FustrumInfo));
	STATIC_ASSERT(offsetof(NGNTextureData, isTex14) == offsetof(Nu3D::Viewport::FustrumInfo, unkInt3));
	STATIC_ASSERT(offsetof(TextureEntry, auxiliaryData) == 0x8);
	STATIC_ASSERT(offsetof(TextureEntry, auxiliaryValue) == 0xC);
	STATIC_ASSERT(offsetof(Type266Entry, count) == 0x0);
	STATIC_ASSERT(offsetof(Type266Entry, index) == 0x2);
	STATIC_ASSERT(offsetof(Type266Entry, points) == 0x4);
	STATIC_ASSERT(sizeof(Vector3F) == 0xC);
	STATIC_ASSERT(offsetof(NGNImage, data266) == 0x54);
	STATIC_ASSERT(sizeof(((NGNImage*)0)->data266) == 0x200);
	STATIC_ASSERT(offsetof(NGNImage, portalHashTable) == 0x254);
	STATIC_ASSERT(offsetof(NGNImage, textureEntries) == 0x27C);
}

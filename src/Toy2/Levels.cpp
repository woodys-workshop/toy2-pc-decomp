#include "Toy2/Levels.h"

#include "SoftwareRenderer.h"
#include "FileUtils.h"
#include "Random.h"
#include "Logger.h"
#include "CharacterLoader.h"
#include "RawLoader.h"
#include "Collision.h"
#include "Renderer/Glue.h"

#include "NGNLoader/NGNLoader.h"
#include "Toy2/Toy2.h"
#include "Toy2/Collectables.h"
#include "Toy2/Actor.h"
#include "Toy2/Level.h"
#include "Toy2/Animation.h"
#include "AudioManager/AudioManager.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shadows.h"
#include "Renderer/Sprite.h"
#include "Renderer/SpriteSheets.h"
#include "Nu3D/Particles.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Light.h"
#include "Nu3D/Math.h"
#include "SaveManager.h"
#include "Toy2/D3DApp.h"
#include "Toy2/Weather.h"
#include "Toy2/Platform.h"
#include "Toy2/Buzz.h"

namespace Toy2
{
	namespace Levels
	{
		// GLOBAL: TOY2 0x00559C70
		RecordData* g_recordData[96];

		// GLOBAL: TOY2 0x0055A114
		int32_t g_levelLoadConfig;

		// GLOBAL: TOY2 0x0055A12C
		int32_t g_levelLoadConfigCopy;

		// GLOBAL: TOY2 0x00729128
		void* g_cachedAllBuffer;

		// GLOBAL: TOY2 0x005D2BE8
		uint8_t g_levelDataHeapBase[1249280];

		// GLOBAL: TOY2 0x00703C00
		uint8_t* g_levelDataHeapBasePtr;

		// GLOBAL: TOY2 0x00559DF4
		uint8_t* g_levelLoadArena;

		// GLOBAL: TOY2 0x0054DE98
		void* g_levelDataBase;

		// GLOBAL: TOY2 0x00556FB8
		int32_t g_unused1;

		// GLOBAL: TOY2 0x005571D0
		int32_t g_unused2;

		// GLOBAL: TOY2 0x00556FB4
		int32_t g_unused3;

		// GLOBAL: TOY2 0x005571CC
		int32_t g_unused4;

		// GLOBAL: TOY2 0x0054F090
		int32_t g_unused5;

		// GLOBAL: TOY2 0x00557AA8
		int32_t g_unused6;

		// GLOBAL: TOY2 0x00547EE8
		int32_t g_unused7;

		// GLOBAL: TOY2 0x00557AA0
		int32_t g_unused8;

		// GLOBAL: TOY2 0x0054DEB8
		int32_t g_unused9;

		// GLOBAL: TOY2 0x0054E07C
		int32_t g_unused10;

		// GLOBAL: TOY2 0x00556FA4
		int32_t g_unused11;

		// GLOBAL: TOY2 0x0054F61C
		int32_t g_unused12;

		// GLOBAL: TOY2 0x0054DD58
		int32_t g_hasZoneData;

		// GLOBAL: TOY2 0x0054BEF0
		int32_t g_type63CullDistance;

		// GLOBAL: TOY2 0x00557C20
		int16_t g_layerScaleTable[4095];

		// GLOBAL: TOY2 0x00557AAC
		int32_t g_type64Count;

		// GLOBAL: TOY2 0x00554040
		Type64 g_type64Structs[24];

		// GLOBAL: TOY2 0x00556FA8
		int32_t g_instanceRecordCount;

		// GLOBAL: TOY2 0x0054E048
		InstanceSection* g_instanceSection;

		// GLOBAL: TOY2 0x0054F628
		ObjectList* g_objectListBase;

		// GLOBAL: TOY2 0x0054DD80
		InstanceSection* g_secondInstanceSection;

		// GLOBAL: TOY2 0x00559C4C
		ObjectDescCache* g_objectDescCache;

		// GLOBAL: TOY2 0x0054F39C
		PortalZone g_portalEntries[20];

		// GLOBAL: TOY2 0x0054DD94
		char g_activeZoneVisTable[260];

		// GLOBAL: TOY2 0x00557A98
		int32_t g_currentType64Region;

		// GLOBAL: TOY2 0x0054D940
		DatContents g_datContents[20];

		// GLOBAL: TOY2 0x005574FC
		int32_t g_unk5574FC;

		// GLOBAL: TOY2 0x00547ED8
		int32_t g_unk547ED8;

		// GLOBAL: TOY2 0x0054E054
		int32_t g_unk54E054;

		// GLOBAL: TOY2 0x00550C20
		uint8_t* g_unk550C20;

		// FUNCTION: TOY2 0x004CEA20
		void FlushRenderer()
		{
			Renderer::Glue::ReleaseBackdrop();
			Nu3D::Light::Cleanup();

			if (NGNLoader::g_ngnImage)
				NGNLoader::NGNImage::Destroy(NGNLoader::g_ngnImage);

			NGNLoader::g_ngnImage = 0;

			Renderer::Cleanup();
		}

		// FUNCTION: TOY2 0x004885C0
		int32_t GetSectorAtPosition(Vector3I* position)
		{
			int32_t lowestY = 0x7FFFFFFF;

			if (! g_hasZoneData)
				return 0;

			int32_t positionX = position->x / 4;
			int32_t positionY = position->y / 4;
			int32_t positionZ = position->z / 4;

			Collision::g_candidateTriangleCount = 0;

			// the meshes that hold the sectors are the flagged ones
			uint16_t* meshList = &Collision::g_collisionMeshList[Collision::g_collisionGrid[255].start];
			int32_t meshCount = Collision::g_collisionGrid[255].count;

			if (meshCount <= 0)
				return -1;

			do
			{
				Collision::CollisionMeshInstance* mesh = &Collision::g_collisionMeshInstances[(int16_t)*meshList];

				if (positionX + 0x1000 - mesh->boundsMin.x < mesh->boundsExt.x + 0x2000 && positionZ + 0x1000 - mesh->boundsMin.z < mesh->boundsExt.z + 0x2000
					&& mesh->typeFlags)
				{
					int32_t cellX = (positionX + 0x1000 - mesh->origin.x) / 32;
					int32_t cellZ = (positionZ + 0x1000 - mesh->origin.z) / 32;
					int32_t localX = (positionX - mesh->origin.x) >> 5;
					int32_t localZ = (positionZ - mesh->origin.z) >> 5;

					Collision::CollisionTreeNode* node = (Collision::CollisionTreeNode*)mesh->collisionTree;

					while (node->flag >= 0)
					{
						Collision::CollisionTriangle* triangle = (Collision::CollisionTriangle*)(node + 1);
						int32_t triangleCount = node->count;

						if ((uint32_t)(cellX - node->minX) >= (uint32_t)(node->extX + 256) || (uint32_t)(cellZ - node->minZ) >= (uint32_t)(node->extZ + 256))
						{
							node = (Collision::CollisionTreeNode*)((uint8_t*)triangle + 44 * triangleCount);
						}
						else
						{
							if (triangleCount > 0)
							{
								for (int32_t i = triangleCount; i; --i, triangle++)
								{
									if ((uint32_t)(cellX - triangle->minX) >= (uint32_t)(triangle->extX + 256))
										continue;

									int32_t v0x = triangle->v0x;
									int32_t v0z = triangle->v0z;

									if ((uint32_t)(cellZ - (triangle->minZByte << 6) - v0z) >= (uint32_t)((triangle->extZByte + 4) << 6))
										continue;

									int32_t surfaceY;
									int32_t height;

									if (triangle->unk28 == 0x7FFF)
									{
										// a triangle
										if ((localX - v0x) * triangle->e1z + (localZ - v0z) * -triangle->e1x < 0)
											continue;

										if ((localX - (v0x + triangle->e2x)) * -triangle->e2z + triangle->e2x * (localZ - (v0z + triangle->e2z)) < 0)
											continue;

										if ((localX - (v0x + triangle->e1x)) * (triangle->e2z - triangle->e1z)
												+ (localZ - (v0z + triangle->e1z)) * (triangle->e1x - triangle->e2x)
											< 0)
											continue;

										surfaceY = 8 * (triangle->unk24 * (localZ - v0z) + triangle->unk20 * (localX - v0x)) / triangle->unk22;
										height = triangle->v0y;
									}
									else
									{
										// a quad, it is split into two triangles
										int32_t corner1Z = v0z + triangle->e1z;

										if ((localX - v0x) * triangle->e1z + (localZ - v0z) * -triangle->e1x < 0)
											continue;

										int32_t corner2Z = v0z + triangle->e2z;

										if ((localX - (v0x + triangle->e2x)) * -triangle->e2z + triangle->e2x * (localZ - corner2Z) < 0)
											continue;

										int32_t corner3Z = v0z + triangle->e3z;

										if ((localX - (v0x + triangle->e1x)) * (corner3Z - corner1Z) + (localZ - corner1Z) * (triangle->e1x - triangle->e3x) < 0
											|| (localX - (v0x + triangle->e3x)) * (corner2Z - corner3Z) + (localZ - corner3Z) * (triangle->e3x - triangle->e2x)
												< 0)
											continue;

										if ((localX - (v0x + triangle->e1x)) * (corner2Z - corner1Z) + (localZ - corner1Z) * (triangle->e1x - triangle->e2x) < 0
											&& triangle->unk28 != 0)
										{
											surfaceY = 8 * (triangle->unk2A * (localZ - triangle->e3z - v0z) + triangle->unk26 * (localX - triangle->e3x - v0x))
												/ triangle->unk28;
											height = triangle->v0y + triangle->e3y;
										}
										else
										{
											surfaceY = 8 * (triangle->unk24 * (localZ - v0z) + triangle->unk20 * (localX - v0x)) / triangle->unk22;
											height = triangle->v0y;
										}
									}

									int32_t worldY = mesh->origin.y + 4 * (8 * height - surfaceY);

									if (worldY >= positionY && worldY < lowestY)
									{
										lowestY = worldY;
										Collision::g_groundContactMeshIdx = *meshList;
									}
								}
							}

							node = (Collision::CollisionTreeNode*)triangle;
						}
					}
				}

				++meshList;
				--meshCount;
			} while (meshCount);

			if (lowestY == 0x7FFFFFFF)
				return -1;

			return (uint8_t)Collision::g_collisionMeshInstances[Collision::g_groundContactMeshIdx].unk;
		}

		// FUNCTION: TOY2 0x004CE8B0
		int32_t InitLevelDefaults()
		{
			Renderer::Init();
			Renderer::BuildGammaCorrectionLUT(g_toyCfgData.gammaCorrection);

			Nu3D::SetUseAsDiffuseModulation(0);
			Nu3D::SetDefaultPrimFlags(32);

			Nu3D::Light::BuildGlobalLights();
			Nu3D::Camera::ApplyTransformToCamera(0);

			Toy2::Animation::ResetNodeAngles();

			SoftwareRenderer::InitialisePrimarySurface_T();

			Renderer::GetBlendShadeCaps(&Renderer::g_deviceBlendShadeCapsCpy);

			SaveManager::SaveToFile(0, "default.cfg");

			FILE* fd = fopen("toy2.cfg", "wb");

			if (! fd)
				return 0;

			fwrite(&g_toyCfgData, 1, sizeof(g_toyCfgData), fd);
			fclose(fd);

			return 1;
		}

		// FUNCTION: TOY2 0x0043E6E0
		int32_t LoadDAT(int32_t levelId, int32_t fileSize)
		{
			Levels::g_unused1 = 256;
			Levels::g_unused2 = 256;

			g_hasZoneData = 0;
			Weather::g_spawnAccumulator = 0;
			Weather::g_weatherBasePointer = 0;

			Levels::g_unused3 = 512;
			Levels::g_unused4 = 512;

			int32_t cullBandWidth;

			if (D3DApp::g_renderMode == RENDERMODE_SOFTWARE || D3DApp::g_renderMode == RENDERMODE_D3D)
			{
				cullBandWidth = 1664;
				g_type63CullDistance = 1664;
			}
			else
			{
				cullBandWidth = g_type63CullDistance;
			}

			uint32_t scaleTableZeroCount = cullBandWidth - 512;

			Levels::g_unused5 = cullBandWidth;
			Levels::g_unused6 = cullBandWidth - 512;
			Levels::g_unused7 = 3072;

			int16_t* scaleTableCursor = g_layerScaleTable;
			Levels::g_unused8 = 170;

			if (cullBandWidth - 512 > 0)
			{
				scaleTableCursor = &g_layerScaleTable[scaleTableZeroCount];

				memset(g_layerScaleTable, 0, 4 * (scaleTableZeroCount >> 1));
				int16_t* scaleFillPtr = &g_layerScaleTable[2 * (scaleTableZeroCount >> 1)];

				for (int32_t oddTailCount = cullBandWidth & 1; oddTailCount; --oddTailCount)
					*scaleFillPtr++ = 0;
			}

			int16_t* region0600 = scaleTableCursor;
			int16_t* region0400 = scaleTableCursor + 170;

			Nu3D::MemSet32Util(region0600, 85, 0x6000600);
			Nu3D::MemSet32Util(region0400, 85, 0x4000400);

			for (int16_t* p = region0400 + 170; p != g_layerScaleTable + (sizeof(g_layerScaleTable) / sizeof(g_layerScaleTable[0])); ++p)
				*p = 512;

			uint8_t* base = reinterpret_cast<uint8_t*>(g_levelDataBase);
			int32_t type64Count = 0;
			Levels::g_unused9 = 0;

			g_type64Count = 0;

			Type64* end = g_type64Structs + (sizeof(g_type64Structs) / sizeof(g_type64Structs[0]));
			Type64* type64Init = g_type64Structs;

			do
			{
				(type64Init++)->recordPtr = 0;
			} while (type64Init < end);

			uint8_t* recordPtr = (base + 4);
			int16_t portalCounts[20];
			memset(portalCounts, 0, sizeof(portalCounts));

			int16_t type65Idx = 65;

			if (*reinterpret_cast<int32_t*>(recordPtr - 4) > 0)
			{
				int32_t totalRecordCount = *reinterpret_cast<int32_t*>(recordPtr - 4);

				DECOMP_PRINT(("Total Record Count -> %d\n", totalRecordCount));

				do
				{
					int16_t recordType = *reinterpret_cast<uint16_t*>(recordPtr + 2);

					if (recordType >= 0)
					{
						if (recordType == 64)
						{
							int32_t type64Idx = type64Count++;

							g_type64Count = type64Count;
							g_type64Structs[type64Idx].recordPtr = recordPtr;

							recordPtr += 12 * *reinterpret_cast<int16_t*>(recordPtr) + 4;
						}
						else
						{
							if (recordType <= 64)
							{
								g_recordData[recordType] = reinterpret_cast<RecordData*>(recordPtr);
							}
							else
							{
								// Portal records, the last point of the record holds the zone it belongs to, and its category
								g_hasZoneData = 1;
								g_recordData[type65Idx] = reinterpret_cast<RecordData*>(recordPtr);

								Vector3I* zoneAnchor = reinterpret_cast<Vector3I*>(recordPtr + 12 * *reinterpret_cast<int16_t*>(recordPtr) - 8);
								int32_t zone = zoneAnchor->x;

								// the counts are in bytes, an entry is 2 bytes
								g_portalEntries[zone].entries[portalCounts[zone] >> 1].recordIdx = (uint8_t)type65Idx;

								zone = zoneAnchor->x;
								g_portalEntries[zone].entries[portalCounts[zone] >> 1].categoryIdx = (uint8_t)zoneAnchor->y;

								portalCounts[zoneAnchor->x] += 2;

								// the points are squashed down to 16 bit, in place
								int32_t portalVertIdx = 0;
								int32_t* portalVert = reinterpret_cast<int32_t*>(recordPtr + 4);

								if (*reinterpret_cast<int16_t*>(recordPtr) > 0)
								{
									do
									{
										int16_t* portalVert16 = reinterpret_cast<int16_t*>(portalVert);

										int32_t coordRaw = portalVert[0];
										portalVert16[0] = coordRaw >> 2;

										coordRaw = portalVert[1];
										portalVert16[1] = coordRaw >> 2;

										coordRaw = portalVert[2];
										portalVert16[2] = coordRaw >> 2;

										portalVert += 3;
										++portalVertIdx;
									} while (portalVertIdx < *reinterpret_cast<int16_t*>(recordPtr));
								}

								type64Count = g_type64Count;
								++type65Idx;
							}

							if (*reinterpret_cast<uint16_t*>(recordPtr + 2) == 63)
								recordPtr += 16 * *reinterpret_cast<int16_t*>(recordPtr) + 4;
							else
								recordPtr += 12 * *reinterpret_cast<int16_t*>(recordPtr) + 4;
						}
					}
					else
					{
						g_recordData[-recordType] = reinterpret_cast<RecordData*>(recordPtr);
						recordPtr += 4 * ((3 * *reinterpret_cast<int16_t*>(recordPtr) + 1) / 2) + 16;
					}

					--totalRecordCount;
				} while (totalRecordCount);
			}

			// terminate the entry list of every zone
			int32_t portalByteOffset = 0;
			int16_t* portalCountsCursor = portalCounts;

			do
			{
				int32_t termOffset = portalByteOffset + *portalCountsCursor;
				portalByteOffset += 32;
				++portalCountsCursor;

				uint8_t* termEntry = reinterpret_cast<uint8_t*>(g_portalEntries) + termOffset;
				termEntry[0] = 0xFF;
				termEntry[1] = 0xFF;
			} while (portalByteOffset < 640);

			int32_t secondIdx = *reinterpret_cast<int32_t*>(recordPtr) << 7;
			g_instanceRecordCount = 0;

			InstanceSection* secondSection = reinterpret_cast<InstanceSection*>(recordPtr + 4 * (secondIdx / 4) + 4);
			ModelTypeData_B* sectionStorage = reinterpret_cast<ModelTypeData_B*>(reinterpret_cast<uint8_t*>(g_levelDataBase) + fileSize);

			DECOMP_PRINT(("Second Idx %d\n", secondIdx));

			g_instanceSection = secondSection;

			// The first batch of instances, each one is given a runtime copy in the storage that sits after the file
			InstanceSection* secondSection_ = secondSection;

			DECOMP_PRINT(("secondSection14 %d\n", secondSection->flags));

			if (secondSection_->flags)
			{
				do
				{
					++g_instanceRecordCount;

					if (secondSection_->modelId < 0)
						secondSection_->modelId = -secondSection_->modelId;

					if (secondSection_->flags < 0)
						secondSection_->flags = (secondSection_->flags | 0x50) - 16;

					if ((secondSection_->flags & 8) != 0)
					{
						ModelTypeData_A* typeData =
							reinterpret_cast<ModelTypeData_A*>(reinterpret_cast<uint8_t*>(g_levelDataBase) + reinterpret_cast<uint32_t>(secondSection_->unk6));

						secondSection_->unk6 = reinterpret_cast<ModelTypeData_B*>(typeData);
						typeData->meshPtr += reinterpret_cast<int32_t>(g_levelDataBase);

						if ((secondSection_->flags & 0x20) != 0)
						{
							secondSection_->flags |= 0x40;
							typeData->animPtr += reinterpret_cast<int32_t>(g_levelDataBase);
						}

						Nu3D::Math::EulerToRotationMatrix(
							reinterpret_cast<AnglesWithRoll*>(&typeData->rotation), reinterpret_cast<Matrix3x3I16*>(sectionStorage));

						int32_t mat_21 = sectionStorage->mat_21;
						sectionStorage->mat_11 = typeData->scaleX * sectionStorage->mat_11 / 4096;
						sectionStorage->mat_21 = mat_21 * typeData->scaleX / 4096;
						int32_t mat_12 = sectionStorage->mat_12;
						sectionStorage->rotation.x = typeData->scaleX * sectionStorage->rotation.x / 4096;
						int32_t mat_22 = sectionStorage->mat_22;

						sectionStorage->mat_12 = mat_12 * typeData->scaleY / 4096;
						int32_t secRotY = sectionStorage->rotation.y;
						sectionStorage->mat_22 = mat_22 * typeData->scaleY / 4096;
						int32_t mat_13 = sectionStorage->mat_13;
						sectionStorage->rotation.y = secRotY * typeData->scaleY / 4096;
						sectionStorage->mat_13 = mat_13 * typeData->scaleZ / 4096;
						int32_t secRotZ = sectionStorage->rotation.z;
						sectionStorage->mat_23 = typeData->scaleZ * sectionStorage->mat_23 / 4096;
						int32_t secRotZValue = secRotZ * typeData->scaleZ;

						sectionStorage->modelPtr = typeData;
						sectionStorage->unk11 = 0;
						sectionStorage->rotation.z = secRotZValue / 4096;

						secondSection_->unk6 = sectionStorage;
						sectionStorage->polyCount = Toy2::Level::CalculatePolyCount(secondSection_);
						typeData->flags &= 0xF;
					}
					else
					{
						ModelTypeData_B* typeData2 =
							reinterpret_cast<ModelTypeData_B*>(reinterpret_cast<uint8_t*>(g_levelDataBase) + reinterpret_cast<uint32_t>(secondSection_->unk6));

						secondSection_->unk6 = typeData2;
						typeData2->modelPtr = reinterpret_cast<uint8_t*>(typeData2->modelPtr) + reinterpret_cast<uint32_t>(g_levelDataBase);

						if ((secondSection_->flags & 0x20) != 0)
						{
							secondSection_->flags |= 0x40;
							typeData2->unk11 += reinterpret_cast<int32_t>(g_levelDataBase);
						}

						Nu3D::Math::EulerToRotationMatrix(
							reinterpret_cast<AnglesWithRoll*>(&typeData2->rotation), reinterpret_cast<Matrix3x3I16*>(sectionStorage));

						sectionStorage->modelPtr = typeData2;
						sectionStorage->unk11 = 0;
						secondSection_->unk6 = sectionStorage;

						sectionStorage->polyCount = Toy2::Level::CalculatePolyCount(secondSection_);
						typeData2->flags &= 0xF;
					}

					++secondSection_;
					++sectionStorage;
				} while (secondSection_->flags);
			}

			// The object list comes right after the last instance, and a second batch of instances after that
			g_objectListBase = reinterpret_cast<ObjectList*>(reinterpret_cast<uint8_t*>(secondSection_) + sizeof(InstanceSection));
			int32_t secondSectionCount = g_objectListBase->count;

			g_secondInstanceSection = reinterpret_cast<InstanceSection*>(reinterpret_cast<uint8_t*>(g_objectListBase) + 4 * secondSectionCount + 8);

			DECOMP_PRINT(("Second section count -> %d\n", secondSectionCount));
			DECOMP_PRINT(("Second Instance Flags -> %d\n", g_secondInstanceSection->flags));

			InstanceSection* secondInstance = g_secondInstanceSection;

			if (secondInstance->flags)
			{
				do
				{
					++g_instanceRecordCount;

					if (secondInstance->modelId < 0)
						secondInstance->modelId = -secondInstance->modelId;

					if (secondInstance->flags < 0)
						secondInstance->flags = (secondInstance->flags | 0x50) - 16;

					if ((secondInstance->flags & 8) != 0)
					{
						ModelTypeData_A* modelA2 =
							reinterpret_cast<ModelTypeData_A*>(reinterpret_cast<uint8_t*>(g_levelDataBase) + reinterpret_cast<uint32_t>(secondInstance->unk6));
						secondInstance->unk6 = reinterpret_cast<ModelTypeData_B*>(modelA2);
						modelA2->meshPtr += reinterpret_cast<int32_t>(g_levelDataBase);

						if ((secondInstance->flags & 0x20) != 0)
						{
							secondInstance->flags |= 0x40;
							modelA2->animPtr += reinterpret_cast<int32_t>(g_levelDataBase);
						}

						Nu3D::Math::EulerToRotationMatrix(
							reinterpret_cast<AnglesWithRoll*>(&modelA2->rotation), reinterpret_cast<Matrix3x3I16*>(sectionStorage));

						int32_t secMat21 = sectionStorage->mat_21;
						sectionStorage->mat_11 = modelA2->scaleX * sectionStorage->mat_11 / 4096;
						sectionStorage->mat_21 = secMat21 * modelA2->scaleX / 4096;
						int32_t secMat12 = sectionStorage->mat_12;
						sectionStorage->rotation.x = modelA2->scaleX * sectionStorage->rotation.x / 4096;
						int32_t secMat22 = sectionStorage->mat_22;
						sectionStorage->mat_12 = secMat12 * modelA2->scaleY / 4096;
						int32_t secRotY2 = sectionStorage->rotation.y;
						sectionStorage->mat_22 = secMat22 * modelA2->scaleY / 4096;
						int32_t secMat13 = sectionStorage->mat_13;
						sectionStorage->rotation.y = secRotY2 * modelA2->scaleY / 4096;
						sectionStorage->mat_13 = secMat13 * modelA2->scaleZ / 4096;
						int32_t secRotZ2 = sectionStorage->rotation.z;
						sectionStorage->mat_23 = modelA2->scaleZ * sectionStorage->mat_23 / 4096;
						int32_t secRotZ2Value = secRotZ2 * modelA2->scaleZ;
						sectionStorage->modelPtr = modelA2;
						sectionStorage->unk11 = 0;
						sectionStorage->rotation.z = secRotZ2Value / 4096;

						secondInstance->unk6 = sectionStorage;
						sectionStorage->polyCount = Toy2::Level::CalculatePolyCount(secondInstance);
						modelA2->flags &= 0xF;
					}
					else
					{
						ModelTypeData_B* modelB2 =
							reinterpret_cast<ModelTypeData_B*>(reinterpret_cast<uint8_t*>(g_levelDataBase) + reinterpret_cast<uint32_t>(secondInstance->unk6));
						secondInstance->unk6 = modelB2;
						modelB2->modelPtr = reinterpret_cast<uint8_t*>(modelB2->modelPtr) + reinterpret_cast<uint32_t>(g_levelDataBase);

						if ((secondInstance->flags & 0x20) != 0)
						{
							secondInstance->flags |= 0x40;
							modelB2->unk11 += reinterpret_cast<int32_t>(g_levelDataBase);
						}

						Nu3D::Math::EulerToRotationMatrix(
							reinterpret_cast<AnglesWithRoll*>(&modelB2->rotation), reinterpret_cast<Matrix3x3I16*>(sectionStorage));

						sectionStorage->modelPtr = modelB2;
						sectionStorage->unk11 = 0;
						secondInstance->unk6 = sectionStorage;
						sectionStorage->polyCount = Toy2::Level::CalculatePolyCount(secondInstance);

						modelB2->flags &= 0xF;
					}

					++secondInstance;
					++sectionStorage;
				} while (secondInstance->flags);
			}

			// The trailer of the file holds the size of the LOD data, -1 meaning there isn't any
			int32_t lodTrailerSize = *reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(g_levelDataBase) + fileSize - 4);

			DECOMP_PRINT(("Load trailer size -> %d\n", lodTrailerSize));
			DECOMP_PRINT(("Object List Base Count -> %d\n", g_objectListBase->count));

			if (lodTrailerSize == -1)
			{
				Levels::g_unused10 = -1;
				memset(g_activeZoneVisTable, 255, 256);
			}
			else
			{
				uint8_t* newLvlBase = reinterpret_cast<uint8_t*>(g_levelDataBase) + fileSize - lodTrailerSize;

				if (g_type64Count > 0)
				{
					Type64* record = g_type64Structs;
					int32_t type64Count_ = g_type64Count;

					do
					{
						// the bounds of the record, which is a box made out of its first two points
						record->unkVar0.x = 0x7FFFFFFF;
						record->unkVar0.y = 0x7FFFFFFF;
						record->unkVar0.z = 0x7FFFFFFF;

						record->unkVar3.x = 0x80000000;
						record->unkVar3.y = 0x80000000;
						record->unkVar3.z = 0x80000000;

						for (int32_t recordOffset = 0; recordOffset < 24; recordOffset += 12)
						{
							Vector3I* bboxVert = reinterpret_cast<Vector3I*>(reinterpret_cast<uint8_t*>(record->recordPtr) + recordOffset + 4);

							if (bboxVert->x < record->unkVar0.x)
								record->unkVar0.x = bboxVert->x;

							if (bboxVert->y < record->unkVar0.y)
								record->unkVar0.y = bboxVert->y;

							if (bboxVert->z < record->unkVar0.z)
								record->unkVar0.z = bboxVert->z;

							if (bboxVert->x > record->unkVar3.x)
								record->unkVar3.x = bboxVert->x;

							if (bboxVert->y > record->unkVar3.y)
								record->unkVar3.y = bboxVert->y;

							if (bboxVert->z > record->unkVar3.z)
								record->unkVar3.z = bboxVert->z;
						}

						int32_t extentZ = record->unkVar3.z - record->unkVar0.z;
						int32_t extentY = record->unkVar3.y - record->unkVar0.y;
						int32_t extentX = record->unkVar3.x - record->unkVar0.x;

						record->unkVar6 = (extentX >> 5) * (extentX >> 5) + (extentY >> 5) * (extentY >> 5) + (extentZ >> 5) * (extentZ >> 5);

						// the LOD data of this record, each block starts with its own size
						record->unkVar8 = newLvlBase;

						newLvlBase = newLvlBase + *reinterpret_cast<int16_t*>(newLvlBase);

						++record;
						--type64Count_;
					} while (type64Count_);
				}

				g_currentType64Region = -1;
				Levels::g_unused11 = 1;
				Toy2::Level::LoaderHelper();
				Levels::g_unused10 = 0;
			}

			Levels::g_unused12 = 400;

			if (g_objectDescCache)
				free(g_objectDescCache);

			g_objectDescCache = reinterpret_cast<ObjectDescCache*>(malloc(28 * (g_objectListBase->count + 1)));

			int32_t objIdx = 0;

			if (g_objectListBase->count >= 0)
			{
				do
				{
					uint32_t meshPtr = reinterpret_cast<uint32_t>(g_objectListBase->entries[objIdx]);

					if (meshPtr)
					{
						g_objectListBase->entries[objIdx] = reinterpret_cast<Object*>(reinterpret_cast<uint8_t*>(g_levelDataBase) + meshPtr);

						Object* object = g_objectListBase->entries[objIdx];

						g_objectDescCache[objIdx].unkInt7 = object->unk14;

						// the desc has a pointer to the data the cache wants at 0x14, it is bigger than ObjectDesc, which isn't fully known
						int16_t* objDesc = *reinterpret_cast<int16_t**>(reinterpret_cast<uint8_t*>(object->desc) + 0x14);

						g_objectDescCache[objIdx].unkInt8 = objDesc[6];
						g_objectDescCache[objIdx].unkInt9 = objDesc[7];
						g_objectDescCache[objIdx].unkInt10 = objDesc[8];
						g_objectDescCache[objIdx].unkInt4 = objDesc[9];
						g_objectDescCache[objIdx].unkInt5 = objDesc[10];
						g_objectDescCache[objIdx].unkInt6 = objDesc[11];
					}

					++objIdx;
				} while (objIdx <= g_objectListBase->count);
			}

			if (g_hasZoneData)
				Toy2::InitZoneData();

			return 32 * g_instanceRecordCount;
		}

		// FUNCTION: TOY2 0x004521A0
		void BuildLevelPath(int32_t level, char* output, const char* suffix)
		{
			if (level >= 10)
			{
				strcpy(output, "level");
				output[5] = level / 10 + 48;
				output[6] = level % 10 + 48;
			}
			else
			{
				strcpy(output, "level0");
				output[6] = level + 48;
			}

			strcpy(output + 7, "\\");
			strcat(output, suffix);
		}

		// FUNCTION: TOY2 0x00452FC0
		void InitLevelPlay(int32_t levelId)
		{
			SoftwareRenderer::SetLevelFileIndex(g_levelFileIndex);
			FlushRenderer();
			InitLevelDefaults();

			FileUtils::LoadFile("rand.dat", g_randDatBuffer);

			int32_t levelIdCpy = levelId;

			AudioManager::g_curTrackIndex = -1;
			g_isElevatorHopLevel = levelId != 10;

			Logger::Log("InitLevelPlay : START.\n");

			AudioManager::LoadSfxPackForLevel(levelId);

			memset(&g_recordData, 0, sizeof(g_recordData));
			memset(CharacterLoader::g_boneTransforms, 0, sizeof(CharacterLoader::g_boneTransforms));
			memset(Collision::g_collisionMeshInstances, 0, sizeof(Collision::g_collisionMeshInstances));
			memset(Collision::g_mathScratch, 0, sizeof(Collision::g_mathScratch));

			SoftwareRenderer::g_unk4F7400.x = -32768;
			SoftwareRenderer::g_unk4F7400.y = -32768;

			g_hasBackdrop = 0;

			CharacterLoader::g_unk54717C = 0;
			CharacterLoader::g_unk546D78 = 0;
			CharacterLoader::g_unk547CD0 = 0;

			SoftwareRenderer::g_unk500A1C = -1;

			Renderer::Shadows::g_shadowCount = 0;
			Renderer::Shadows::g_unusedShadowVar = 0;

			for (int32_t particleIdx = 0; particleIdx < 64; particleIdx++)
				Nu3D::Particles::g_particleInstances[particleIdx].lifetime = 0;

			Nu3D::Camera::InitViewMatrixGlobals();

			memset(CharacterLoader::g_alternateAllParse, 0, sizeof(CharacterLoader::g_alternateAllParse));
			memset(CharacterLoader::g_charFileDataCache, 0, sizeof(CharacterLoader::g_charFileDataCache));

			g_cachedAllBuffer = 0;

			if (levelId > 10)
			{
				g_levelLoadConfig |= 256;

				if ((g_levelLoadConfig & 192) == 0)
					g_levelLoadConfig |= 64;

				levelIdCpy = levelId - 10;
			}

			int32_t loadConfigCpy = g_levelLoadConfig;
			g_levelLoadConfigCopy = g_levelLoadConfig;

			uint8_t* dataBuffer = g_levelDataHeapBase;

			g_levelDataHeapBasePtr = g_levelDataHeapBase;
			g_levelLoadArena = g_levelDataHeapBase;

			char fileNameBuffer[128];
			fileNameBuffer[0] = '\0';

			if ((loadConfigCpy & 1) == 0)
			{
				int32_t rawVariant = ((loadConfigCpy >> 6) & 3) - 1;
				const char* rawFileName;

				if (rawVariant)
				{
					int32_t rawVariantStep = rawVariant - 1;

					if (rawVariantStep)
					{
						if (rawVariantStep == 1)
						{
							if (levelIdCpy >= 10)
							{
								strcpy(fileNameBuffer, "level");
								fileNameBuffer[5] = levelIdCpy / 10 + 48;
								fileNameBuffer[6] = levelIdCpy % 10 + 48;
							}
							else
							{
								strcpy(fileNameBuffer, "level0");
								fileNameBuffer[6] = levelIdCpy + 48;
							}

							strcpy(&fileNameBuffer[7], "\\");
							rawFileName = "level3.raw";
						}
						else
						{
							if (levelIdCpy >= 10)
							{
								strcpy(fileNameBuffer, "level");
								fileNameBuffer[5] = levelIdCpy / 10 + 48;
								fileNameBuffer[6] = levelIdCpy % 10 + 48;
							}
							else
							{
								strcpy(fileNameBuffer, "level0");
								fileNameBuffer[6] = levelIdCpy + 48;
							}

							strcpy(&fileNameBuffer[7], "\\");
							rawFileName = "level.raw";
						}
					}
					else
					{
						if (levelIdCpy >= 10)
						{
							strcpy(fileNameBuffer, "level");
							fileNameBuffer[5] = levelIdCpy / 10 + 48;
							fileNameBuffer[6] = levelIdCpy % 10 + 48;
						}
						else
						{
							strcpy(fileNameBuffer, "level0");
							fileNameBuffer[6] = levelIdCpy + 48;
						}

						strcpy(&fileNameBuffer[7], "\\");
						rawFileName = "level2.raw";
					}
				}
				else
				{
					if (levelIdCpy >= 10)
					{
						strcpy(fileNameBuffer, "level");
						fileNameBuffer[5] = levelIdCpy / 10 + 48;
						fileNameBuffer[6] = levelIdCpy % 10 + 48;
					}
					else
					{
						strcpy(fileNameBuffer, "level0");
						fileNameBuffer[6] = levelIdCpy + 48;
					}

					strcpy(&fileNameBuffer[7], "\\");
					rawFileName = "level1.raw";
				}

				strcat(fileNameBuffer, rawFileName);
				RawLoader::LoadPacketData(fileNameBuffer);
				loadConfigCpy = g_levelLoadConfig;
			}

			dataBuffer = g_levelLoadArena;

			char levelDigits[4];

			if ((loadConfigCpy & 2) == 0)
			{
				int32_t binVariant = ((loadConfigCpy >> 8) & 3) - 1;

				if (binVariant)
				{
					int32_t binVariantStep = binVariant - 1;

					if (binVariantStep)
					{
						if (binVariantStep == 1)
						{
							strcpy(fileNameBuffer, "..\\level");

							if (levelIdCpy + 10 >= 10)
							{
								levelDigits[2] = 0;
								levelDigits[0] = (levelIdCpy + 10) / 10 + 48;
								levelDigits[1] = (levelIdCpy + 10) % 10 + 48;
							}
							else
							{
								levelDigits[0] = '0';
								levelDigits[1] = levelIdCpy + 58;
								levelDigits[2] = 0;
							}
						}
						else
						{
							strcpy(fileNameBuffer, "..\\level");

							if (levelIdCpy >= 10)
							{
								levelDigits[2] = 0;
								levelDigits[0] = levelIdCpy / 10 + 48;
								levelDigits[1] = levelIdCpy % 10 + 48;
							}
							else
							{
								levelDigits[0] = '0';
								levelDigits[2] = 0;
								levelDigits[1] = levelIdCpy + 48;
							}
						}
					}
					else
					{
						strcpy(fileNameBuffer, "..\\level");

						if (levelIdCpy + 10 >= 10)
						{
							levelDigits[2] = 0;
							levelDigits[0] = (levelIdCpy + 10) / 10 + 48;
							levelDigits[1] = (levelIdCpy + 10) % 10 + 48;
						}
						else
						{
							levelDigits[0] = '0';
							levelDigits[1] = levelIdCpy + 58;
							levelDigits[2] = 0;
						}
					}
				}
				else
				{
					strcpy(fileNameBuffer, "..\\level");

					if (levelIdCpy + 10 >= 10)
					{
						levelDigits[0] = (levelIdCpy + 10) / 10 + 48;
						levelDigits[1] = (levelIdCpy + 10) % 10 + 48;
					}
					else
					{
						levelDigits[0] = 48;
						levelDigits[1] = levelIdCpy + 58;
					}

					levelDigits[2] = 0;
				}

				strcat(fileNameBuffer, levelDigits);
				strcat(fileNameBuffer, ".bin");
			}

			Renderer::InitSpriteSheets();

			const char* datFileName;
			void* bufferPtr;
			int32_t charLoaderFlag = 0;

			memset(fileNameBuffer, 0, sizeof(fileNameBuffer));

			if ((g_levelLoadConfig & 4) == 0)
			{
				bufferPtr = dataBuffer;
				g_levelDataBase = dataBuffer;

				switch ((g_levelLoadConfig >> 8) & 3)
				{
					case 1:
						if (levelIdCpy >= 10)
						{
							strcpy(fileNameBuffer, "level");
							fileNameBuffer[5] = levelIdCpy / 10 + 48;
							fileNameBuffer[6] = levelIdCpy % 10 + 48;
						}
						else
						{
							strcpy(fileNameBuffer, "level0");
							fileNameBuffer[6] = levelIdCpy + 48;
						}

						strcpy(&fileNameBuffer[7], "\\");
						datFileName = "level1.dat";
						break;

					case 2:
						if (levelIdCpy >= 10)
						{
							strcpy(fileNameBuffer, "level");
							fileNameBuffer[5] = levelIdCpy / 10 + 48;
							fileNameBuffer[6] = levelIdCpy % 10 + 48;
						}
						else
						{
							strcpy(fileNameBuffer, "level0");
							fileNameBuffer[6] = levelIdCpy + 48;
						}

						strcpy(&fileNameBuffer[7], "\\");
						datFileName = "level2.dat";
						break;

					case 3:
						if (levelIdCpy >= 10)
						{
							strcpy(fileNameBuffer, "level");
							fileNameBuffer[5] = levelIdCpy / 10 + 48;
							fileNameBuffer[6] = levelIdCpy % 10 + 48;
						}
						else
						{
							strcpy(fileNameBuffer, "level0");
							fileNameBuffer[6] = levelIdCpy + 48;
						}

						strcpy(&fileNameBuffer[7], "\\");
						datFileName = "level3.dat";
						break;

					default:
						if (levelIdCpy >= 10)
						{
							strcpy(fileNameBuffer, "level");
							fileNameBuffer[5] = levelIdCpy / 10 + 48;
							fileNameBuffer[6] = levelIdCpy % 10 + 48;
						}
						else
						{
							strcpy(fileNameBuffer, "level0");
							fileNameBuffer[6] = levelIdCpy + 48;
						}

						strcpy(&fileNameBuffer[7], "\\");
						datFileName = "level.dat";
						break;
				}

				strcat(fileNameBuffer, datFileName);

				int32_t fileSize = FileUtils::LoadFile(fileNameBuffer, bufferPtr);
				dataBuffer = dataBuffer + fileSize;

				int32_t offset = LoadDAT(levelIdCpy, fileSize);
				dataBuffer = dataBuffer + offset;
			}

			fileNameBuffer[0] = 0;
			FileUtils::AppendRegPathToBuffer();

			int32_t variant;
			int32_t variantStep;
			const char* ngnBaseName;
			int32_t loadConfigCpy2 = g_levelLoadConfig;

			Logger::Log("Level load config -> %d\n", g_levelLoadConfig);

			if ((g_levelLoadConfig & 4) == 0 || (g_levelLoadConfig & 1) == 0)
			{
				g_levelDataBase = dataBuffer;

				if (((g_levelLoadConfig >> 8) & 3) == 1)
				{
					if (levelIdCpy >= 10)
					{
						strcpy(fileNameBuffer, "level");
						fileNameBuffer[5] = levelIdCpy / 10 + 48;
						fileNameBuffer[6] = levelIdCpy % 10 + 48;
						loadConfigCpy2 = g_levelLoadConfig;
					}
					else
					{
						strcpy(fileNameBuffer, "level0");
						fileNameBuffer[6] = levelIdCpy + 48;
					}

					strcpy(&fileNameBuffer[7], "\\");
					ngnBaseName = "level1";
				}
				else
				{
					if (((g_levelLoadConfig >> 8) & 3) != 2)
					{
						if (((g_levelLoadConfig >> 8) & 3) == 3)
							BuildLevelPath(levelIdCpy, fileNameBuffer, "level3");
						else
							BuildLevelPath(levelIdCpy, fileNameBuffer, "level");

						loadConfigCpy2 = g_levelLoadConfig;

					LBL_BUILD_TEXTURE_SUFFIX:

						if ((loadConfigCpy2 & 4) == 0 && levelIdCpy && levelId != 16)
							goto LBL_LOAD_NGN;

						variant = ((loadConfigCpy2 >> 6) & 3) - 1;

						const char* tSuffix;

						if (variant)
						{
							variantStep = variant - 1;

							if (variantStep)
							{
								if (variantStep != 1)
								{
								LBL_LOAD_NGN:

									strcat(fileNameBuffer, ".ngn");
									strcat(FileUtils::g_fileNameBuffer, fileNameBuffer);

									NGNLoader::SetNewImage(FileUtils::g_fileNameBuffer);
									NGNLoader::DetectBackdropTextures();

									loadConfigCpy2 = g_levelLoadConfig;
									goto LBL_POST_NGN_LOAD;
								}

								tSuffix = "t3";
							}
							else
							{
								tSuffix = "t2";
							}
						}
						else
						{
							tSuffix = "t1";
						}

						strcat(fileNameBuffer, tSuffix);
						goto LBL_LOAD_NGN;
					}

					if (levelIdCpy >= 10)
					{
						strcpy(fileNameBuffer, "level");
						fileNameBuffer[5] = levelIdCpy / 10 + 48;
						fileNameBuffer[6] = levelIdCpy % 10 + 48;
						loadConfigCpy2 = g_levelLoadConfig;
					}
					else
					{
						strcpy(fileNameBuffer, "level0");
						fileNameBuffer[6] = levelIdCpy + 48;
					}

					strcpy(&fileNameBuffer[7], "\\");
					ngnBaseName = "level2";
				}

				strcat(fileNameBuffer, ngnBaseName);
				goto LBL_BUILD_TEXTURE_SUFFIX;
			}

		LBL_POST_NGN_LOAD:

			if ((loadConfigCpy2 & 8) == 0)
			{
				if (! g_cachedAllBuffer)
				{
					Collision::BuildCollisionWorld(levelIdCpy, &dataBuffer, (loadConfigCpy2 >> 8) & 3);
					loadConfigCpy2 = (loadConfigCpy2 & ~0xFF) | (g_levelLoadConfig & 0xFF);
				}

				if ((loadConfigCpy2 & 8) == 0)
				{
					Collectables::BuildPickupTable();
					loadConfigCpy2 = (loadConfigCpy2 & ~0xFF) | (g_levelLoadConfig & 0xFF);
				}
			}

			if ((loadConfigCpy2 & 16) == 0)
			{
				uint8_t creatureIdList[128];
				Actor::GetCreatureList(creatureIdList);
				CharacterLoader::Start(&charLoaderFlag, &dataBuffer, creatureIdList);
			}

			g_levelLoadArena = dataBuffer;
			g_levelLoadConfig = 0;

			Logger::Log("InitLevelPlay : END.\n");
		}

		// FUNCTION: TOY2 0x00434090
		int32_t Method2(int32_t a1, int16_t a2)
		{
			g_buzzActor.unkWord15 = a2;
			g_buzzActor.collisionFlags = 0;
			g_buzzActor.unkShort40 = 0;
			g_buzzActor.unkVar37 = 2;
			g_buzzActor.gravityVel = a1;
			Buzz::g_curMoveableObject = 0;
			Buzz::g_buzzSlipTimer = 0;
			Buzz::g_unk53C61C = 0;

			return 0;
		}

		// FUNCTION: TOY2 0x0048B660
		void Method1(int32_t a1, int32_t a2, int32_t a3, int32_t a4)
		{
			if (! Buzz::g_unk53C660 && ! Buzz::g_unk53C668 && ! Buzz::g_unk53C618)
			{
				if (Collision::g_buzzCollisionStates[a1].wasGroundedLastFrame
					&& a4 * Collision::g_buzzCollisionStates[a1].surfaceNormal.z + a3 * Collision::g_buzzCollisionStates[a1].surfaceNormal.y
							+ a2 * Collision::g_buzzCollisionStates[a1].surfaceNormal.x
						< 0)
				{
					Collision::g_unk7295A0 = 0;
				}
				else
				{
					int32_t strength = Collision::g_unk7295A0;

					if (Collision::g_unk7295A0 < 4096)
					{
						strength = Collision::g_unk7295A0 + 256;
						Collision::g_unk7295A0 += 256;
					}

					Collision::g_buzzCollisionStates[a1].responseVel.x += a2 * strength / 4096;
					Collision::g_buzzCollisionStates[a1].responseVel.y += a3 * strength / 4096;
					Collision::g_buzzCollisionStates[a1].responseVel.z += a4 * strength / 4096;
				}
			}
		}

		// FUNCTION: TOY2 0x0049FAB0
		void Method3(int32_t a1, int32_t a2)
		{
			int32_t index = a1;

			if (a2)
				index = Level::g_unk830E28 + a1;

			Vector3I* entry = &g_recordData[58]->data[index];

			if (entry->y != 0x80000000)
			{
				int32_t particleIdx = 0;

				Nu3D::Particles::ParticleInstance* particle = &Nu3D::Particles::g_particleInstances[0];

				// look for the particle that was spawned at the position of this record
				while (particle->pos.y != entry->y || particle->lifetime <= 0 || particle->pos.x != entry->x || particle->pos.z != entry->z
					   || particle->typeId != 2 * a2 + 113)
				{
					++particle;
					++particleIdx;

					if (particle >= &Nu3D::Particles::g_particleInstances[64])
					{
						entry->y = 0x80000000;
						return;
					}
				}

				Nu3D::Particles::g_particleInstances[particleIdx].lifetime = 1;

				entry->y = 0x80000000;
			}
		}
	}

	namespace Terrain
	{
		// FUNCTION: TOY2 0x00489980
		int32_t LoadAll(const char* fileName, int32_t index, void** buffer)
		{
			char fullFileName[100];
			uint16_t copyTable[100];
			int32_t meshCount = 0;
			int32_t entryIdx = 0;

			if (Levels::g_cachedAllBuffer)
			{
				*buffer = Levels::g_cachedAllBuffer;
			}
			else
			{
				strcpy(fullFileName, fileName);
				strcat(fullFileName, ".all");

				FileUtils::LoadFile(fullFileName, *buffer);
			}

			// the file starts with the collision trees, the table of the instances comes after them
			int32_t* table = (int32_t*)((uint8_t*)*buffer + 2 * *(int32_t*)*buffer);
			*buffer = (uint8_t*)*buffer + 4;

			memset(copyTable, 0xFF, sizeof(copyTable));
			memset(Collision::g_unk7290F0, 0, sizeof(Collision::g_unk7290F0));

			if (*table > 0)
			{
				int32_t meshIndex = index;
				TerrainInstance* instance = (TerrainInstance*)(table + 1);
				Collision::CollisionMeshInstance* mesh = &Collision::g_collisionMeshInstances[index];

				do
				{
					if (instance->type >= 256)
					{
						// types above 255 are markers, the slot in the tree data is added to a list for that type
						if (**(int32_t**)buffer == 0x12345678)
						{
							**(int32_t**)buffer = Collision::g_unk7290F0[instance->type - 256];
							Collision::g_unk7290F0[instance->type - 256] = (int32_t)((uint8_t*)*buffer + 4);
						}
					}
					else
					{
						++meshCount;

						if (instance->isCopy == 1)
						{
							// a copy of a mesh that was added before, with its own position
							int16_t source = copyTable[instance->copySlot];

							if (source < 0)
							{
								mesh->typeFlags = 0;
							}
							else
							{
								memcpy(mesh, &Collision::g_collisionMeshInstances[source], sizeof(Collision::CollisionMeshInstance));

								mesh->origin.x = instance->x;
								mesh->origin.y = instance->y;
								mesh->origin.z = instance->z;
								mesh->platformIdx = instance->type;

								mesh->origin.x *= 32;
								mesh->origin.y *= 32;
								mesh->origin.z *= 32;
							}

							++meshIndex;
							++mesh;
						}
						else
						{
							mesh->typeFlags = (int16_t)instance->type;
							mesh->unk = instance->unk40;
							mesh->origin.x = 32 * instance->x;
							mesh->origin.y = 32 * instance->y;
							mesh->origin.z = 32 * instance->z;
							mesh->collisionTree = (int32_t)*buffer;

							// a mesh that is on a platform tells the platform where it is
							if (instance->platformSlot)
							{
								Platform::PlatformDescriptor* platform = &Platform::g_platformDescriptors[instance->platformSlot - 1];

								platform->meshInstanceId = (int16_t)meshIndex;
								platform->origin.x = mesh->origin.x;
								platform->origin.y = mesh->origin.y;
								platform->origin.z = mesh->origin.z;

								mesh->platformIdx = instance->platformSlot - 1;
							}

							if (instance->copySlot >= 0)
								copyTable[instance->copySlot] = (uint16_t)meshIndex;

							++meshIndex;
							++mesh;
						}
					}

					*buffer = (uint8_t*)*buffer + 2 * instance->dataSize;
					instance++;
					++entryIdx;
				} while (entryIdx < *table);
			}

			Collision::g_unk72871C = (uint8_t*)*buffer;

			*buffer = (uint8_t*)*buffer + 0x33C0;

			for (int32_t i = 0; i < 0x33C0; i += 0x5C0)
			{
				*(int16_t*)(Collision::g_unk72871C + i + 0x18) = 0;
				*(int32_t*)(Collision::g_unk72871C + i + 0x1C) = 0;
			}

			return meshCount;
		}
	}
}

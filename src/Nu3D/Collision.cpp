#include "Nu3D/Collision.h"
#include "Nu3D/Math.h"
#include "Renderer/Shadows.h"
#include "Toy2/Collision.h"
#include "Toy2/Platform.h"
#include "Toy2/Shadow.h"

#include <STDLIB.H>

namespace Nu3D
{
	namespace Collision
	{
		// FUNCTION: TOY2 0x00481140
		int16_t IsPointInTriangle(
			int32_t x1,
			int32_t y1,
			int32_t z1,
			int32_t x2,
			int32_t y2,
			int32_t z2,
			int32_t x3,
			int32_t y3,
			int32_t z3,
			Vector3I16* normal)
		{
			int16_t y = normal->y;
			int32_t absY = abs(y);
			int32_t absX = abs(normal->x);
			int32_t v13;
			int32_t v14;
			int32_t v16;
			int32_t v17;
			int32_t v18;
			int32_t v19;
			int32_t v20;
			bool negative;

			// the triangle is tested in the plane the normal points the most along
			if (absX <= absY)
			{
				if (absY >= abs(normal->z))
				{
					if (y >= 0)
					{
						if (x2 * (z1 - z2) - z2 * (x1 - x2) >= 0 && z3 * x1 - x3 * z1 >= 0)
						{
							negative = (z2 - z3) * (x1 - x3) + (z1 - z3) * (x3 - x2) < 0;

							return !negative;
						}
					}
					else if (z2 * x1 - x2 * z1 >= 0 && x3 * (z1 - z3) - z3 * (x1 - x3) >= 0)
					{
						v13 = (x2 - x3) * (z1 - z2);
						v14 = (z3 - z2) * (x1 - x2);

						goto LABEL_23;
					}

					return 0;
				}

				if (absX < absY)
					goto LABEL_18;
			}

			if (absX < abs(normal->z))
			{
			LABEL_18:
				if (normal->z >= 0)
					return y2 * (x1 - x2) - x2 * (y1 - y2) >= 0 && x3 * y1 - y3 * x1 >= 0 && (x2 - x3) * (y1 - y3) + (y3 - y2) * (x1 - x3) >= 0;

				v17 = y1;
				v16 = x1;
				v19 = x2;
				v18 = y2;

				if (x2 * y1 - y2 * x1 < 0)
					return 0;

				v20 = x3;
				negative = y3 * (x1 - x3) - x3 * (y1 - y3) < 0;
			}
			else
			{
				if (normal->x < 0)
				{
					if (y2 * (z1 - z2) - z2 * (y1 - y2) >= 0 && z3 * y1 - y3 * z1 >= 0)
					{
						negative = (z2 - z3) * (y1 - y3) + (z1 - z3) * (y3 - y2) < 0;

						return !negative;
					}

					return 0;
				}

				v16 = z1;
				v17 = y1;
				v18 = y2;
				v19 = z2;

				if (z2 * y1 - y2 * z1 < 0)
					return 0;

				v20 = z3;
				negative = y3 * (z1 - z3) - z3 * (y1 - y3) < 0;
			}

			if (!negative)
			{
				v13 = (v20 - v19) * (v17 - v18);
				v14 = (v18 - y3) * (v16 - v19);

			LABEL_23:
				negative = v14 + v13 < 0;

				return !negative;
			}

			return 0;
		}

		// FUNCTION: TOY2 0x00485940
		void GatherTrianglesAtXZ(WorldPos* worldPos)
		{
			int32_t x = worldPos->x;
			int32_t y = worldPos->y;
			int32_t z = worldPos->z;

			Toy2::Collision::CollisionCacheEntry* cache = (Toy2::Collision::CollisionCacheEntry*)Toy2::Collision::g_unk72871C;
			Toy2::Collision::CollisionCacheEntry* entry;
			Toy2::Collision::CollisionMeshInstance* mesh;
			Toy2::Collision::CollisionTreeNode* node;
			Toy2::Collision::CollisionTriangle* triangle;
			Matrix3x3I16* rotation;
			int16_t* meshList = (int16_t*)Toy2::Collision::g_mathScratch;
			int32_t cellX = 0;
			int32_t cellZ = 0;
			int32_t found = -1;
			int32_t oldest = -1;
			int32_t oldestLifetime = 0x7FFFFFFF;
			int32_t prevMesh;
			int32_t valid = 0;
			int32_t meshCount;
			int32_t meshIdx;
			int32_t i;
			int32_t j;
			int32_t px;
			int32_t pz;
			int32_t dx;
			int32_t dy;
			int32_t dz;

			// is there a cached query within 0x2000 of this position, otherwise the entry with the lowest lifetime is replaced
			for (i = 0; i < 8; i++)
			{
				entry = &cache[i];

				if ((uint32_t)(x - entry->x + 0x2000) < 0x4000 && (uint32_t)(z - entry->z + 0x2000) < 0x4000 && entry->lifetime > 0)
				{
					found = i;
					break;
				}

				if (entry->lifetime < oldestLifetime)
				{
					oldestLifetime = entry->lifetime;
					oldest = i;
				}
			}

			Toy2::Collision::g_candidateTriangleCount = 0;

			if (found >= 0)
			{
				entry = &cache[found];
				entry->lifetime = 8;
				prevMesh = -1;

				// the cached triangles that are still close to the position
				for (i = entry->count - 1; i >= 0; i--)
				{
					meshIdx = entry->meshIdx[i];

					if (meshIdx == prevMesh)
					{
						if (!valid)
							continue;
					}
					else
					{
						prevMesh = meshIdx;
						valid = 0;
						mesh = &Toy2::Collision::g_collisionMeshInstances[meshIdx];

						if (x + 4096 - mesh->boundsMin.x < mesh->boundsExt.x + 0x2000 && z + 4096 - mesh->boundsMin.z < mesh->boundsExt.z + 0x2000 && mesh->typeFlags
							&& (mesh->unk & 0x100) == 0)
						{
							valid = 1;
							cellX = (x + 4096 - mesh->origin.x) / 32;
							cellZ = (z + 4096 - mesh->origin.z) / 32;
						}
						else
							continue;
					}

					triangle = entry->triangles[i];

					if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(triangle->extX + 256)
						&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z) < (uint32_t)((triangle->extZByte + 4) << 6))
					{
						if (Toy2::Collision::g_candidateTriangleCount < 240)
						{
							Toy2::Collision::g_candidateTris[Toy2::Collision::g_candidateTriangleCount] = triangle;
							Toy2::Collision::g_candidateMeshIdx[Toy2::Collision::g_candidateTriangleCount + 1] = (int16_t)meshIdx;
							Toy2::Collision::g_candidateTriangleCount++;
						}
					}
				}
			}
			else
			{
				// the meshes of the grid cells around the position
				meshCount = 0;

				for (i = 0; i < Toy2::Collision::g_unk729120; i++)
				{
					Toy2::Collision::CollisionGridCell* cell = &Toy2::Collision::g_collisionGrid[i];

					if ((uint32_t)(x + 20480 - cell->minX) < (uint32_t)(cell->extX + 40960) && (uint32_t)(z + 20480 - cell->minZ) < (uint32_t)(cell->extZ + 40960))
					{
						for (j = 0; j < cell->count; j++)
							meshList[meshCount + j] = Toy2::Collision::g_collisionMeshList[cell->start + j];

						meshCount += cell->count;
					}
				}

				entry = &cache[oldest];
				entry->count = 0;

				// the triangles of those meshes, from their collision trees
				for (i = 0; i < meshCount; i++)
				{
					mesh = &Toy2::Collision::g_collisionMeshInstances[meshList[i]];

					if (x + 20480 - mesh->boundsMin.x < mesh->boundsExt.x + 40960 && z + 20480 - mesh->boundsMin.z < mesh->boundsExt.z + 40960 && mesh->typeFlags
						&& (mesh->unk & 0x100) == 0)
					{
						cellX = (x + 20480 - mesh->origin.x) / 32;
						cellZ = (z + 20480 - mesh->origin.z) / 32;
						node = (Toy2::Collision::CollisionTreeNode*)mesh->collisionTree;

						while (node->flag >= 0)
						{
							int32_t triangleCount = node->count;

							triangle = (Toy2::Collision::CollisionTriangle*)(node + 1);

							if ((uint32_t)(cellX - node->minX) < (uint32_t)(node->extX + 1280) && (uint32_t)(cellZ - node->minZ) < (uint32_t)(node->extZ + 1280))
							{
								for (j = 0; j < triangleCount; j++)
								{
									if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(triangle->extX + 1280)
										&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z) < (uint32_t)((triangle->extZByte + 20) << 6) && entry->count < 240)
									{
										entry->triangles[entry->count] = triangle;
										entry->meshIdx[entry->count] = meshList[i];
										entry->count++;
									}

									triangle++;
								}

								node = (Toy2::Collision::CollisionTreeNode*)triangle;
							}
							else
								node = (Toy2::Collision::CollisionTreeNode*)(triangle + triangleCount);
						}
					}
				}

				entry->lifetime = 8;
				entry->x = worldPos->x;
				entry->z = worldPos->z;

				prevMesh = -1;

				// same as above, the cached triangles that are close to the position
				for (i = entry->count - 1; i >= 0; i--)
				{
					meshIdx = entry->meshIdx[i];

					if (meshIdx == prevMesh)
					{
						if (!valid)
							continue;
					}
					else
					{
						prevMesh = meshIdx;
						valid = 0;
						mesh = &Toy2::Collision::g_collisionMeshInstances[meshIdx];

						if (x + 4096 - mesh->boundsMin.x < mesh->boundsExt.x + 0x2000 && z + 4096 - mesh->boundsMin.z < mesh->boundsExt.z + 0x2000 && mesh->typeFlags
							&& (mesh->unk & 0x100) == 0)
						{
							valid = 1;
							cellX = (x + 4096 - mesh->origin.x) / 32;
							cellZ = (z + 4096 - mesh->origin.z) / 32;
						}
						else
							continue;
					}

					triangle = entry->triangles[i];

					if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(triangle->extX + 256)
						&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z) < (uint32_t)((triangle->extZByte + 4) << 6))
					{
						if (Toy2::Collision::g_candidateTriangleCount < 240)
						{
							Toy2::Collision::g_candidateTris[Toy2::Collision::g_candidateTriangleCount] = triangle;
							Toy2::Collision::g_candidateMeshIdx[Toy2::Collision::g_candidateTriangleCount + 1] = (int16_t)meshIdx;
							Toy2::Collision::g_candidateTriangleCount++;
						}
					}
				}
			}

			// the type 8 meshes (the platforms) are never cached, they move
			for (j = 0; j < Toy2::Collision::g_unk7284A2; j++)
				meshList[j] = Toy2::Collision::g_collisionMeshList[Toy2::Collision::g_unk7284A0 + j];

			rotation = (Matrix3x3I16*)&Toy2::Collision::g_mathScratch[948];

			for (i = 0; i < Toy2::Collision::g_unk7284A2; i++)
			{
				meshIdx = meshList[i];
				mesh = &Toy2::Collision::g_collisionMeshInstances[meshIdx];

				if (Toy2::Platform::g_platformDescriptors[mesh->platformIdx].flags & 8)
				{
					// the position is moved into the space of the rotated platform
					Vector3I16* rotAngle = &Toy2::Platform::g_platformDescriptors[mesh->platformIdx].rotAngle;

					*(int16_t*)&Toy2::Collision::g_mathScratch[980] = rotAngle->x >> 2;
					*(int16_t*)&Toy2::Collision::g_mathScratch[982] = rotAngle->y >> 2;
					*(int16_t*)&Toy2::Collision::g_mathScratch[984] = rotAngle->z >> 2;

					Math::EulerToRotationMatrix((AnglesWithRoll*)&Toy2::Collision::g_mathScratch[980], rotation);

					dy = y - mesh->origin.y;
					dx = x - mesh->origin.x;
					dz = z - mesh->origin.z;

					px = (rotation->m00 * dx + rotation->m10 * dy + rotation->m20 * dz) / 4096 + mesh->origin.x + 4096;
					pz = (rotation->m22 * dz + rotation->m02 * dx + rotation->m12 * dy) / 4096 + mesh->origin.z + 4096;
				}
				else
				{
					px = x + 4096;
					pz = z + 4096;
				}

				if (px - mesh->boundsMin.x < mesh->boundsExt.x + 0x2000 && pz - mesh->boundsMin.z < mesh->boundsExt.z + 0x2000 && mesh->typeFlags)
				{
					cellX = (px - mesh->origin.x) / 32;
					cellZ = (pz - mesh->origin.z) / 32;
					node = (Toy2::Collision::CollisionTreeNode*)mesh->collisionTree;

					while (node->flag >= 0)
					{
						int32_t triangleCount = node->count;

						triangle = (Toy2::Collision::CollisionTriangle*)(node + 1);

						if ((uint32_t)(cellX - node->minX) < (uint32_t)(node->extX + 256) && (uint32_t)(cellZ - node->minZ) < (uint32_t)(node->extZ + 256))
						{
							for (j = 0; j < triangleCount; j++)
							{
								if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(triangle->extX + 256)
									&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z) < (uint32_t)((triangle->extZByte + 4) << 6)
									&& Toy2::Collision::g_candidateTriangleCount < 240)
								{
									Toy2::Collision::g_candidateTris[Toy2::Collision::g_candidateTriangleCount] = triangle;
									Toy2::Collision::g_candidateTriangleCount++;
									Toy2::Collision::g_candidateMeshIdx[Toy2::Collision::g_candidateTriangleCount] = meshList[i];
								}

								triangle++;
							}

							node = (Toy2::Collision::CollisionTreeNode*)triangle;
						}
						else
							node = (Toy2::Collision::CollisionTreeNode*)(triangle + triangleCount);
					}
				}
			}
		}

		// FUNCTION: TOY2 0x00486280
		int32_t GetGroundHeight(WorldPos* worldPos, int32_t size)
		{
			int32_t triangleCount = Toy2::Collision::g_candidateTriangleCount;

			GatherTrianglesAtXZ(worldPos);

			WorldPos groundPos = *worldPos;

			ResolveGroundCeiling(&groundPos, 0x10000);

			if (size && Renderer::Shadows::g_shadowCount < 47)
				Toy2::Shadow::QueueStretched(groundPos.x, groundPos.y, groundPos.z, size, worldPos->y);

			Toy2::Collision::g_candidateTriangleCount = triangleCount;

			return groundPos.y;
		}

		// FUNCTION: TOY2 0x00486310
		int32_t GetGroundHeightEx(WorldPos* worldPos, int32_t size, int32_t headRoom)
		{
			int32_t triangleCount = Toy2::Collision::g_candidateTriangleCount;

			GatherTrianglesAtXZ(worldPos);

			WorldPos groundPos = *worldPos;

			ResolveGroundCeiling(&groundPos, headRoom);

			if (size && Renderer::Shadows::g_shadowCount < 47)
				Toy2::Shadow::QueueStretched(groundPos.x, groundPos.y, groundPos.z, size, worldPos->y);

			Toy2::Collision::g_candidateTriangleCount = triangleCount;

			return groundPos.y;
		}

		// FUNCTION: TOY2 0x00486520
		int32_t ResolveGroundCeiling(WorldPos* worldPos, int32_t headRoom)
		{
			int16_t* scratch = (int16_t*)Toy2::Collision::g_mathScratch;
			Matrix3x3I16* rotation = (Matrix3x3I16*)&Toy2::Collision::g_mathScratch[948];
			AnglesWithRoll angles;
			int16_t* triangle;
			int16_t* surfaceNormal;
			int16_t rotatedNormal[3];
			int32_t floorY = 0x7FFFFFFF;
			int32_t ceilingY = 0x80000000;
			int32_t prevMesh = -1;
			int32_t meshIdx;
			int32_t px = 0;
			int32_t pz = 0;
			int32_t rotated = 0;
			int32_t surfaceY;
			int32_t side;
			int32_t i;

			// the rotated triangle, v0 is the base vertex, e1 and e2 and e3 are the offsets to the other vertices
			int32_t v0x, v0y, v0z;
			int32_t e1x, e1z, e2x, e2z, e3x, e3y, e3z;
			int32_t n1x, n1y, n1z;
			int32_t n2x, n2y, n2z;
			int32_t edgeX;
			int32_t edgeZ;
			int32_t baseZ;
			int32_t base2Z;
			int32_t base3Z;

			Toy2::Collision::g_ceilingContactMeshIdx = -1;
			Toy2::Collision::g_groundContactMeshIdx = -1;

			for (i = 0; i < Toy2::Collision::g_candidateTriangleCount; i++)
			{
				meshIdx = Toy2::Collision::g_candidateMeshIdx[i + 1];
				triangle = (int16_t*)Toy2::Collision::g_candidateTris[i];

				if (meshIdx != prevMesh)
				{
					Toy2::Collision::CollisionMeshInstance* mesh = &Toy2::Collision::g_collisionMeshInstances[meshIdx];

					rotated = 0;

					if (mesh->typeFlags == 8)
					{
						px = (worldPos->x - mesh->origin.x) >> 5;
						pz = (worldPos->z - mesh->origin.z) >> 5;

						if (Toy2::Platform::g_platformDescriptors[mesh->platformIdx].flags & 8)
						{
							Vector3I16* rotAngle = &Toy2::Platform::g_platformDescriptors[mesh->platformIdx].rotAngle;

							angles.angles.pitch = rotAngle->x >> 2;
							angles.angles.yaw = rotAngle->y >> 2;
							angles.roll = rotAngle->z >> 2;

							Math::EulerToRotationMatrix(&angles, rotation);

							rotated = 1;
						}
					}
					else
					{
						px = (worldPos->x - mesh->origin.x) >> 5;
						pz = (worldPos->z - mesh->origin.z) >> 5;
					}

					prevMesh = meshIdx;
				}

				if (rotated == 1)
				{
					n1x = (rotation->m02 * triangle[18] + rotation->m01 * triangle[17] + rotation->m00 * triangle[16]) / 4096;
					n1y = (rotation->m12 * triangle[18] + rotation->m11 * triangle[17] + rotation->m10 * triangle[16]) / 4096;
					n1z = (rotation->m22 * triangle[18] + rotation->m21 * triangle[17] + rotation->m20 * triangle[16]) / 4096;

					v0x = (rotation->m02 * triangle[6] + rotation->m01 * triangle[5] + rotation->m00 * triangle[4]) / 4096;
					v0y = (rotation->m12 * triangle[6] + rotation->m11 * triangle[5] + rotation->m10 * triangle[4]) / 4096;
					v0z = (rotation->m22 * triangle[6] + rotation->m21 * triangle[5] + rotation->m20 * triangle[4]) / 4096;

					e1x = (rotation->m02 * triangle[9] + rotation->m01 * triangle[8] + rotation->m00 * triangle[7]) / 4096;
					e1z = (rotation->m22 * triangle[9] + rotation->m21 * triangle[8] + rotation->m20 * triangle[7]) / 4096;

					e2x = (rotation->m02 * triangle[12] + rotation->m01 * triangle[11] + rotation->m00 * triangle[10]) / 4096;
					e2z = (rotation->m22 * triangle[12] + rotation->m21 * triangle[11] + rotation->m20 * triangle[10]) / 4096;

					if (triangle[20] == 0x7FFF)
					{
						// a single triangle
						if ((int16_t)n1y >= 0)
						{
							if ((int16_t)n1y <= 0)
								continue;

							if ((pz - (e1z + v0z)) * e1x + -e1z * (px - (v0x + e1x)) < 0 || (px - v0x) * e2z + (pz - v0z) * -e2x < 0
								|| (px - (v0x + e2x)) * (e1z - e2z) + (pz - (e2z + v0z)) * (v0x + e2x - e1x - v0x) < 0)
								continue;

							side = 2;
						}
						else
						{
							if ((px - v0x) * e1z + (pz - v0z) * (v0x - e1x - v0x) < 0 || (pz - (e2z + v0z)) * e2x + -e2z * (px - (v0x + e2x)) < 0
								|| (px - (v0x + e1x)) * (v0z + e2z - (v0z + e1z)) + (pz - (v0z + e1z)) * (e1x - e2x) < 0)
								continue;

							side = 1;
						}

						rotatedNormal[0] = (int16_t)n1x;
						rotatedNormal[1] = (int16_t)n1y;
						rotatedNormal[2] = (int16_t)n1z;
						surfaceNormal = rotatedNormal;

						surfaceY = Toy2::Collision::g_collisionMeshInstances[meshIdx].origin.y
								   + 4 * (8 * v0y - 8 * ((int16_t)n1x * (px - v0x) + n1z * (pz - v0z)) / (int16_t)n1y);
					}
					else
					{
						// a quad, two triangles
						n2x = (rotation->m02 * triangle[21] + rotation->m01 * triangle[20] + rotation->m00 * triangle[19]) / 4096;
						n2y = (rotation->m12 * triangle[21] + rotation->m11 * triangle[20] + rotation->m10 * triangle[19]) / 4096;
						n2z = (rotation->m22 * triangle[21] + rotation->m21 * triangle[20] + rotation->m20 * triangle[19]) / 4096;

						e3x = (rotation->m02 * triangle[15] + rotation->m01 * triangle[14] + rotation->m00 * triangle[13]) / 4096;
						e3y = (rotation->m12 * triangle[15] + rotation->m11 * triangle[14] + rotation->m10 * triangle[13]) / 4096;
						e3z = (rotation->m22 * triangle[15] + rotation->m21 * triangle[14] + rotation->m20 * triangle[13]) / 4096;

						if ((int16_t)n1y >= 0)
						{
							if ((int16_t)n1y <= 0)
								continue;

							edgeX = v0x + e1x;
							edgeZ = e1z + v0z;

							if ((pz - edgeZ) * e1x + -e1z * (px - edgeX) < 0)
								continue;

							if ((px - v0x) * e2z + (pz - v0z) * -e2x < 0)
								continue;

							if ((px - (v0x + e3x)) * (v0z + e1z - (e3z + v0z)) + (pz - (e3z + v0z)) * (e3x - e1x) < 0
								|| (pz - (e2z + v0z)) * (v0x + e2x - e3x - v0x) + (px - (v0x + e2x)) * (e3z - e2z) < 0)
								continue;

							side = 2;
						}
						else
						{
							if ((px - v0x) * e1z + (pz - v0z) * (v0x - e1x - v0x) < 0)
								continue;

							if ((pz - (v0z + e2z)) * e2x + -e2z * (px - (v0x + e2x)) < 0)
								continue;

							edgeX = e1x + v0x;
							edgeZ = v0z + e1z;

							if ((pz - edgeZ) * (e1x - e3x) + (px - edgeX) * (e3z - e1z) < 0 || (px - (v0x + e3x)) * (e2z - e3z) + (pz - (e3z + v0z)) * (e3x - e2x) < 0)
								continue;

							side = 1;
						}

						if ((px - edgeX) * (v0z + e2z - edgeZ) + (pz - edgeZ) * (edgeX - e2x - v0x) >= 0 || !(int16_t)n2y)
						{
							// the first triangle
							rotatedNormal[0] = (int16_t)n1x;
							rotatedNormal[1] = (int16_t)n1y;
							rotatedNormal[2] = (int16_t)n1z;
							surfaceNormal = rotatedNormal;

							surfaceY = Toy2::Collision::g_collisionMeshInstances[meshIdx].origin.y
									   + 4 * (8 * v0y - 8 * ((int16_t)n1x * (px - v0x) + n1z * (pz - v0z)) / (int16_t)n1y);
						}
						else
						{
							// the second triangle
							rotatedNormal[0] = (int16_t)n2x;
							rotatedNormal[1] = (int16_t)n2y;
							rotatedNormal[2] = (int16_t)n2z;
							surfaceNormal = rotatedNormal;

							surfaceY = Toy2::Collision::g_collisionMeshInstances[meshIdx].origin.y
									   + 4 * (8 * (v0y + e3y) - 8 * ((int16_t)n2x * (px - e3x - v0x) + n2z * (pz - e3z - v0z)) / (int16_t)n2y);
						}
					}
				}
				else if (triangle[20] == 0x7FFF)
				{
					// a single triangle
					if (triangle[17] >= 0)
					{
						if (triangle[17] <= 0)
							continue;

						baseZ = triangle[6];
						v0x = triangle[4];

						if (triangle[7] * (pz - (triangle[9] + baseZ)) + -triangle[9] * (px - (v0x + triangle[7])) < 0 || triangle[12] * (px - v0x) + (pz - baseZ) * -triangle[10] < 0
							|| (triangle[9] - triangle[12]) * (px - (v0x + triangle[10])) + (pz - (baseZ + triangle[12])) * (triangle[10] - triangle[7]) < 0)
							continue;

						side = 2;
					}
					else
					{
						v0x = triangle[4];
						baseZ = triangle[6];

						if (triangle[9] * (px - v0x) + (pz - baseZ) * -triangle[7] < 0 || triangle[10] * (pz - (baseZ + triangle[12])) + -triangle[12] * (px - (v0x + triangle[10])) < 0
							|| (triangle[12] - triangle[9]) * (px - (v0x + triangle[7])) + (pz - (baseZ + triangle[9])) * (triangle[7] - triangle[10]) < 0)
							continue;

						side = 1;
					}

					surfaceNormal = triangle + 16;

					surfaceY = Toy2::Collision::g_collisionMeshInstances[meshIdx].origin.y
							   + 4 * (8 * triangle[5] - 8 * (triangle[16] * (px - v0x) + triangle[18] * (pz - baseZ)) / triangle[17]);
				}
				else
				{
					// a quad, two triangles
					if (triangle[17] >= 0)
					{
						if (triangle[17] <= 0)
							continue;

						baseZ = triangle[6];
						v0x = triangle[4];
						edgeX = v0x + triangle[7];
						edgeZ = triangle[9] + baseZ;

						if (triangle[7] * (pz - edgeZ) + -triangle[9] * (px - edgeX) < 0)
							continue;

						base2Z = baseZ + triangle[12];

						if (triangle[12] * (px - v0x) + (pz - baseZ) * -triangle[10] < 0)
							continue;

						base3Z = baseZ + triangle[15];

						if ((edgeZ - base3Z) * (px - (v0x + triangle[13])) + (pz - base3Z) * (triangle[13] - triangle[7]) < 0
							|| (px - (v0x + triangle[10])) * (base3Z - base2Z) + (pz - base2Z) * (triangle[10] - triangle[13]) < 0)
							continue;

						side = 2;
					}
					else
					{
						v0x = triangle[4];
						baseZ = triangle[6];
						edgeZ = baseZ + triangle[9];

						if (triangle[9] * (px - v0x) + (pz - baseZ) * -triangle[7] < 0)
							continue;

						base2Z = triangle[12] + baseZ;

						if (triangle[10] * (pz - base2Z) + -triangle[12] * (px - (v0x + triangle[10])) < 0)
							continue;

						base3Z = baseZ + triangle[15];
						edgeX = v0x + triangle[7];

						if ((px - (v0x + triangle[7])) * (base3Z - edgeZ) + (pz - edgeZ) * (triangle[7] - triangle[13]) < 0
							|| (base2Z - base3Z) * (px - (v0x + triangle[13])) + (pz - base3Z) * (triangle[13] - triangle[10]) < 0)
							continue;

						side = 1;
					}

					if ((base2Z - edgeZ) * (px - edgeX) + (pz - edgeZ) * (edgeX - triangle[10] - v0x) < 0 && triangle[20] != 0)
					{
						// the second triangle
						surfaceNormal = triangle + 19;

						surfaceY = Toy2::Collision::g_collisionMeshInstances[meshIdx].origin.y
								   + 4 * (8 * (triangle[5] + triangle[14])
										  - 8 * (triangle[19] * (px - triangle[13] - v0x) + triangle[21] * (pz - triangle[15] - baseZ)) / triangle[20]);
					}
					else
					{
						// the first triangle
						surfaceNormal = triangle + 16;

						surfaceY = Toy2::Collision::g_collisionMeshInstances[meshIdx].origin.y
								   + 4 * (8 * triangle[5] - 8 * (triangle[16] * (px - v0x) + triangle[18] * (pz - baseZ)) / triangle[17]);
					}
				}

				if (surfaceY < worldPos->y)
				{
					// above the position, the closest one is the ceiling
					if (surfaceY >= ceilingY && surfaceY > worldPos->y - headRoom && (surfaceY != ceilingY || surfaceNormal[1] > scratch[7]))
					{
						scratch[6] = surfaceNormal[0];
						scratch[7] = surfaceNormal[1];
						scratch[8] = surfaceNormal[2];
						ceilingY = surfaceY;
						Toy2::Collision::g_ceilingContactMeshIdx = (int16_t)meshIdx;
					}
				}
				else if (surfaceY < floorY && side == 1)
				{
					// below the position, the closest one that faces up is the floor
					floorY = surfaceY;
					scratch[0] = surfaceNormal[0];
					scratch[1] = surfaceNormal[1];
					scratch[2] = surfaceNormal[2];
					Toy2::Collision::g_groundContactMeshIdx = (int16_t)meshIdx;
				}
			}

			if (ceilingY != 0x80000000 && scratch[7] < 0)
			{
				Toy2::Collision::g_groundQueryResult = 1;
				worldPos->y = ceilingY;
				Toy2::Collision::g_floorSlope.z = scratch[8];
				Toy2::Collision::g_floorSlope.x = scratch[6];
				Toy2::Collision::g_floorSlope.y = scratch[7];
				Toy2::Collision::g_groundContactMeshIdx = Toy2::Collision::g_ceilingContactMeshIdx;

				return 0;
			}

			if (floorY != 0x7FFFFFFF)
			{
				Toy2::Collision::g_floorSlope.x = scratch[0];
				Toy2::Collision::g_floorSlope.y = scratch[1];
				Toy2::Collision::g_groundQueryResult = 2;
				Toy2::Collision::g_floorSlope.z = scratch[2];
				worldPos->y = floorY;

				return 0;
			}

			Toy2::Collision::g_groundQueryResult = 3;
			worldPos->y = 0x80000000;
			Toy2::Collision::g_floorSlope.y = 4096;

			return 0;
		}

		// FUNCTION: TOY2 0x00487A60
		BOOL IsFloorWalkable() { return Toy2::Collision::g_floorSlope.y >= (int16_t)0xE000; }

		// FUNCTION: TOY2 0x00487AB0
		int32_t GetSurfaceQuality(int32_t collisionState)
		{
			if ((uint8_t)Toy2::Collision::g_buzzCollisionStates[collisionState].collisionQuality == 0xFF)
				return -1;

			return (uint8_t)Toy2::Collision::g_buzzCollisionStates[collisionState].collisionQuality;
		}

		// STUB: TOY2 0x00487B20
		int32_t SweepAgainstEdges(int32_t* a1, int32_t* a2, int32_t* a3, uint32_t* a4, int32_t* a5, int32_t* a6, int32_t a7) { return 0; }

		// STUB: TOY2 0x004882F0
		int32_t RaycastAgainstEdges(int32_t* a1, int32_t* a2, int32_t* a3, uint32_t* a4, int16_t* a5, int32_t* a6, int32_t* a7) { return 0; }

		// STUB: TOY2 0x0048B750
		int32_t SweepAgainstCandidates(int32_t* a1, int32_t* a2, int32_t a3) { return 0; }

		// STUB: TOY2 0x0048C030
		// $TODO: this is a __usercall in the original, see IDA for the register arguments
		int32_t RaycastAgainstCandidates(int32_t a1, int32_t* a2, int32_t* a3) { return 0; }

		// STUB: TOY2 0x0048C860
		int32_t SweepAndSlide(WorldPos* startPos, WorldPos* displacement, int32_t collisionMask, int32_t useTopSector, int32_t radius) { return 0; }

		// STUB: TOY2 0x0048D530
		int32_t SweepQuery(int32_t* a1, int32_t* a2, int32_t a3, int32_t a4) { return 0; }
	}
}

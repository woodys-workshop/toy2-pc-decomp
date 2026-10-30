#include "Nu3D/Collision.h"
#include "Nu3D/Math.h"
#include "Renderer/Shadows.h"
#include "Toy2/Collision.h"
#include "Toy2/Platform.h"
#include "Toy2/Shadow.h"

#include <STDLIB.H>
#include <MATH.H>
#include <STRING.H>

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

							return ! negative;
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

						return ! negative;
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

			if (! negative)
			{
				v13 = (v20 - v19) * (v17 - v18);
				v14 = (v18 - y3) * (v16 - v19);

			LABEL_23:
				negative = v14 + v13 < 0;

				return ! negative;
			}

			return 0;
		}

		// FUNCTION: TOY2 0x00485940
		void GatherTrianglesAtXZ(Transform* transform)
		{
			int32_t x = transform->pos.x;
			int32_t y = transform->pos.y;
			int32_t z = transform->pos.z;

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
						if (! valid)
							continue;
					}
					else
					{
						prevMesh = meshIdx;
						valid = 0;
						mesh = &Toy2::Collision::g_collisionMeshInstances[meshIdx];

						if (x + 4096 - mesh->boundsMin.x < mesh->boundsExt.x + 0x2000 && z + 4096 - mesh->boundsMin.z < mesh->boundsExt.z + 0x2000
							&& mesh->typeFlags && (mesh->unk & 0x100) == 0)
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

					if ((uint32_t)(x + 20480 - cell->minX) < (uint32_t)(cell->extX + 40960)
						&& (uint32_t)(z + 20480 - cell->minZ) < (uint32_t)(cell->extZ + 40960))
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

					if (x + 20480 - mesh->boundsMin.x < mesh->boundsExt.x + 40960 && z + 20480 - mesh->boundsMin.z < mesh->boundsExt.z + 40960
						&& mesh->typeFlags && (mesh->unk & 0x100) == 0)
					{
						cellX = (x + 20480 - mesh->origin.x) / 32;
						cellZ = (z + 20480 - mesh->origin.z) / 32;
						node = (Toy2::Collision::CollisionTreeNode*)mesh->collisionTree;

						while (node->flag >= 0)
						{
							int32_t triangleCount = node->count;

							triangle = (Toy2::Collision::CollisionTriangle*)(node + 1);

							if ((uint32_t)(cellX - node->minX) < (uint32_t)(node->extX + 1280)
								&& (uint32_t)(cellZ - node->minZ) < (uint32_t)(node->extZ + 1280))
							{
								for (j = 0; j < triangleCount; j++)
								{
									if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(triangle->extX + 1280)
										&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z) < (uint32_t)((triangle->extZByte + 20) << 6)
										&& entry->count < 240)
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
				entry->x = transform->pos.x;
				entry->z = transform->pos.z;

				prevMesh = -1;

				// same as above, the cached triangles that are close to the position
				for (i = entry->count - 1; i >= 0; i--)
				{
					meshIdx = entry->meshIdx[i];

					if (meshIdx == prevMesh)
					{
						if (! valid)
							continue;
					}
					else
					{
						prevMesh = meshIdx;
						valid = 0;
						mesh = &Toy2::Collision::g_collisionMeshInstances[meshIdx];

						if (x + 4096 - mesh->boundsMin.x < mesh->boundsExt.x + 0x2000 && z + 4096 - mesh->boundsMin.z < mesh->boundsExt.z + 0x2000
							&& mesh->typeFlags && (mesh->unk & 0x100) == 0)
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
		int32_t GetGroundHeight(Transform* transform, int32_t size)
		{
			int32_t triangleCount = Toy2::Collision::g_candidateTriangleCount;

			GatherTrianglesAtXZ(transform);

			Transform groundPos = *transform;

			ResolveGroundCeiling(&groundPos, 0x10000);

			if (size && Renderer::Shadows::g_shadowCount < 47)
				Toy2::Shadow::QueueStretched(groundPos.pos.x, groundPos.pos.y, groundPos.pos.z, size, transform->pos.y);

			Toy2::Collision::g_candidateTriangleCount = triangleCount;

			return groundPos.pos.y;
		}

		// FUNCTION: TOY2 0x00486310
		int32_t GetGroundHeightEx(Transform* transform, int32_t size, int32_t headRoom)
		{
			int32_t triangleCount = Toy2::Collision::g_candidateTriangleCount;

			GatherTrianglesAtXZ(transform);

			Transform groundPos = *transform;

			ResolveGroundCeiling(&groundPos, headRoom);

			if (size && Renderer::Shadows::g_shadowCount < 47)
				Toy2::Shadow::QueueStretched(groundPos.pos.x, groundPos.pos.y, groundPos.pos.z, size, transform->pos.y);

			Toy2::Collision::g_candidateTriangleCount = triangleCount;

			return groundPos.pos.y;
		}

		// FUNCTION: TOY2 0x00486520
		int32_t ResolveGroundCeiling(Transform* transform, int32_t headRoom)
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
			int16_t v0x, v0y, v0z;
			int16_t e1x, e1z, e2x, e2z, e3x, e3y, e3z;
			int16_t n1x, n1y, n1z;
			int16_t n2x, n2y, n2z;
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
						px = (transform->pos.x - mesh->origin.x) >> 5;
						pz = (transform->pos.z - mesh->origin.z) >> 5;

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
						px = (transform->pos.x - mesh->origin.x) >> 5;
						pz = (transform->pos.z - mesh->origin.z) >> 5;
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

							if ((pz - edgeZ) * (e1x - e3x) + (px - edgeX) * (e3z - e1z) < 0
								|| (px - (v0x + e3x)) * (e2z - e3z) + (pz - (e3z + v0z)) * (e3x - e2x) < 0)
								continue;

							side = 1;
						}

						if ((px - edgeX) * (v0z + e2z - edgeZ) + (pz - edgeZ) * (edgeX - e2x - v0x) >= 0 || ! (int16_t)n2y)
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

						if (triangle[7] * (pz - (triangle[9] + baseZ)) + -triangle[9] * (px - (v0x + triangle[7])) < 0
							|| triangle[12] * (px - v0x) + (pz - baseZ) * -triangle[10] < 0
							|| (triangle[9] - triangle[12]) * (px - (v0x + triangle[10])) + (pz - (baseZ + triangle[12])) * (triangle[10] - triangle[7]) < 0)
							continue;

						side = 2;
					}
					else
					{
						v0x = triangle[4];
						baseZ = triangle[6];

						if (triangle[9] * (px - v0x) + (pz - baseZ) * -triangle[7] < 0
							|| triangle[10] * (pz - (baseZ + triangle[12])) + -triangle[12] * (px - (v0x + triangle[10])) < 0
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
							+ 4
								* (8 * (triangle[5] + triangle[14])
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

				if (surfaceY < transform->pos.y)
				{
					// above the position, the closest one is the ceiling
					if (surfaceY >= ceilingY && surfaceY > transform->pos.y - headRoom && (surfaceY != ceilingY || surfaceNormal[1] > scratch[7]))
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
				transform->pos.y = ceilingY;
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
				transform->pos.y = floorY;

				return 0;
			}

			Toy2::Collision::g_groundQueryResult = 3;
			transform->pos.y = 0x80000000;
			Toy2::Collision::g_floorSlope.y = 4096;

			return 0;
		}

		// FUNCTION: TOY2 0x00487A60
		BOOL IsFloorWalkable() { return Toy2::Collision::g_floorSlope.y >= (int16_t)0xE000; }

		// FUNCTION: TOY2 0x00487AB0 [MATCHED]
		int32_t GetSurfaceQuality(int32_t collisionState)
		{
			int16_t collisionQuality = Toy2::Collision::g_buzzCollisionStates[collisionState].collisionQuality;

			if ((uint8_t)collisionQuality == 0xFF)
				return -1;

			return (uint8_t)collisionQuality;
		}

		// FUNCTION: TOY2 0x00487B20
		int32_t SweepAgainstEdges(
			int32_t* bestDist,
			int32_t* hitT0,
			int32_t* hitT1,
			uint32_t* hitNormal,
			int32_t* position,
			int32_t* displacement,
			int32_t radius)
		{
			Vector3I16 normal;
			Vector3I16 tangent;
			int32_t* segment;
			int32_t hit = 0;
			int32_t count = 0;
			int32_t distance;
			int32_t endDistance;
			int32_t scaledRadius;
			int32_t endScaledRadius;
			int32_t hitX;
			int32_t hitZ;
			int32_t time;
			int32_t projection;
			int32_t sideX;
			int32_t sideZ;
			int32_t length;
			int32_t newBest;
			int32_t distanceStart;

			normal.y = 0;

			if (! Toy2::Collision::g_unk72D2AC)
				return 0;

			if (Toy2::Collision::g_unk72D2AC > 0)
			{
				segment = Toy2::Collision::g_unk728DA0;

				do
				{
					// the normal of the wall segment
					normal.x = (int16_t)(segment[2] - segment[5]);
					normal.z = (int16_t)(segment[3] - segment[0]);

					Math::NormalizeToFixedPoint16(&normal, &normal);

					if (abs(displacement[0]) >= 0x2000 || abs(displacement[2]) >= 0x2000)
					{
						distanceStart = (normal.z * ((position[2] >> 5) - segment[2]) + normal.x * ((position[0] >> 5) - segment[0])) >> 12;
						scaledRadius = radius / 32;
					}
					else
					{
						distanceStart = (normal.z * (position[2] - 32 * segment[2]) + normal.x * (position[0] - 32 * segment[0])) >> 12;
						scaledRadius = radius;
					}

					distance = distanceStart - scaledRadius;

					if (distance >= 0)
					{
						if (abs(displacement[0]) >= 0x2000 || abs(displacement[2]) >= 0x2000)
						{
							endDistance =
								(normal.x * (((displacement[0] + position[0]) >> 5) - segment[0])
								 + normal.z * (((displacement[2] + position[2]) >> 5) - segment[2]))
								>> 12;
							endScaledRadius = radius / 32;
						}
						else
						{
							endScaledRadius = radius;
							endDistance =
								(normal.x * (displacement[0] + position[0] - 32 * segment[0]) + normal.z * (displacement[2] + position[2] - 32 * segment[2]))
								>> 12;
						}

						endDistance = endDistance - endScaledRadius;

						if (endDistance < 0)
						{
							tangent.x = (int16_t)(segment[3] - segment[0]);
							tangent.z = (int16_t)(segment[5] - segment[2]);

							hitX = position[0] + distance * displacement[0] / (distance - endDistance) - ((radius * normal.x) >> 12);
							hitZ = position[2] + distance * displacement[2] / (distance - endDistance) - ((radius * normal.z) >> 12);

							if (tangent.x * (hitX - 32 * segment[0]) + tangent.z * (hitZ - 32 * segment[2]) >= -8192
								&& -(tangent.z * (hitZ - 32 * segment[5]) + tangent.x * (hitX - 32 * segment[3])) >= -8192)
							{
								newBest = (distance << 14) / (distance - endDistance);

								if (newBest < *bestDist)
								{
									hit = 1;
									normal.x *= 4;
									normal.z *= 4;
									*hitT0 = distance;
									*hitT1 = endDistance;
									hitNormal[0] = (uint16_t)normal.x | ((uint32_t)(uint16_t)normal.y << 16);
									hitNormal[1] = (uint16_t)normal.z;
									*bestDist = newBest;
								}
							}
						}
					}

					// the end of the wall segment
					distance = segment[2] - (position[2] >> 5);

					if ((segment[0] - (position[0] >> 5)) * (segment[0] - (position[0] >> 5)) + distance * distance
						< (displacement[0] >> 5) * (displacement[0] >> 5) + ((radius + 0x2000) >> 5) * ((radius + 0x2000) >> 5)
							+ (displacement[2] >> 5) * (displacement[2] >> 5))
					{
						tangent.z = (int16_t)(displacement[2] >> 5);
						tangent.x = (int16_t)(displacement[0] >> 5);
						tangent.y = 0;

						Math::NormalizeToFixedPoint16(&tangent, &tangent);

						projection = (tangent.x * (segment[0] - (position[0] >> 5)) + tangent.z * (segment[2] - (position[2] >> 5))) >> 12;

						if (projection >= 0)
						{
							sideZ = segment[2] - ((position[2] + ((projection * tangent.z) >> 7)) >> 5);
							sideX = segment[0] - ((position[0] + ((projection * tangent.x) >> 7)) >> 5);

							if (((radius + 32) >> 5) * ((radius + 32) >> 5) >= sideX * sideX + sideZ * sideZ)
							{
								time = projection - (int32_t)sqrt((double)(((radius + 64) >> 5) * ((radius + 64) >> 5) - sideZ * sideZ - sideX * sideX));

								if (time >= -2
									&& time * time <= (displacement[0] >> 5) * (displacement[0] >> 5) + (displacement[2] >> 5) * (displacement[2] >> 5))
								{
									if (time < 0)
										time = 0;

									length = (int32_t)sqrt(
										(double)((displacement[0] >> 5) * (displacement[0] >> 5) + (displacement[2] >> 5) * (displacement[2] >> 5)));
									newBest = (time << 14) / length;

									if (newBest < *bestDist)
									{
										normal.z = (int16_t)(position[2] + ((time * tangent.z) >> 7) - (int16_t)(32 * segment[2]));
										normal.x = (int16_t)(position[0] + ((time * tangent.x) >> 7) - 32 * segment[0]);

										Math::NormalizeToFixedPoint16(&normal, &normal);

										hit = 1;
										normal.x *= 4;
										normal.z *= 4;
										*hitT0 = time;
										*hitT1 = time - length;
										hitNormal[0] = (uint16_t)normal.x | ((uint32_t)(uint16_t)normal.y << 16);
										hitNormal[1] = (uint16_t)normal.z;
										*bestDist = newBest;
									}
								}
							}
						}
					}

					segment += 6;
					count += 2;
				} while (count < Toy2::Collision::g_unk72D2AC);
			}

			return hit;
		}

		// FUNCTION: TOY2 0x004882F0
		int32_t RaycastAgainstEdges(
			int32_t* bestDist,
			int32_t* hitT0,
			int32_t* hitT1,
			uint32_t* hitNormal,
			int16_t* flipped,
			int32_t* position,
			int32_t* direction)
		{
			Vector3I16 normal;
			Vector3I16 tangent;
			int32_t* segment;
			int32_t hit = 0;
			int32_t count = 0;
			int32_t startDistance;
			int32_t endDistance;
			int32_t hitX;
			int32_t hitZ;
			int32_t newBest;
			int16_t flip;

			normal.y = 0;

			if (! Toy2::Collision::g_unk72D2AC)
				return 0;

			if (Toy2::Collision::g_unk72D2AC > 0)
			{
				segment = Toy2::Collision::g_unk728DA0;

				do
				{
					normal.x = (int16_t)(segment[2] - segment[5]);
					normal.z = (int16_t)(segment[3] - segment[0]);

					Math::NormalizeToFixedPoint16(&normal, &normal);

					flip = 0;
					startDistance = (normal.x * ((position[0] >> 5) - segment[0]) + normal.z * ((position[2] >> 5) - segment[2])) >> 12;
					endDistance =
						(normal.x * (((position[0] + direction[0]) >> 5) - segment[0]) + normal.z * (((position[2] + direction[2]) >> 5) - segment[2])) >> 12;

					if (startDistance < endDistance)
					{
						startDistance = -startDistance;
						endDistance = -endDistance;
						flip = 1;
					}

					if (startDistance >= 0 && endDistance < 0)
					{
						tangent.x = (int16_t)(segment[3] - segment[0]);
						tangent.z = (int16_t)(segment[5] - segment[2]);

						hitX = position[0] + startDistance * direction[0] / (startDistance - endDistance);
						hitZ = position[2] + startDistance * direction[2] / (startDistance - endDistance);

						// NEG/JS tests the wrapped sign, including INT_MIN.
						if (tangent.x * (hitX - 32 * segment[0]) + tangent.z * (hitZ - 32 * segment[2]) >= 0
							&& 0U - (tangent.x * (hitX - 32U * segment[3]) + tangent.z * (hitZ - 32U * segment[5])) <= 0x7FFFFFFFU)
						{
							newBest = (startDistance << 14) / (startDistance - endDistance);

							if (newBest < *bestDist)
							{
								hit = 1;
								normal.x *= 4;
								normal.z *= 4;
								*hitT0 = startDistance;
								*hitT1 = endDistance;
								hitNormal[0] = (uint16_t)normal.x | ((uint32_t)(uint16_t)normal.y << 16);
								hitNormal[1] = (uint16_t)normal.z;
								*bestDist = newBest;
								*flipped = flip;
							}
						}
					}

					segment += 6;
					count += 2;
				} while (count < Toy2::Collision::g_unk72D2AC);
			}

			return hit;
		}

		// FUNCTION: TOY2 0x0048B750
		int32_t SweepAgainstCandidates(int32_t* displacement, int32_t* position, int32_t radius)
		{
			Toy2::Collision::CollisionTriangle* triangle;
			Toy2::Collision::CollisionMeshInstance* mesh;
			Matrix3x3I16* rotation = (Matrix3x3I16*)&Toy2::Collision::g_mathScratch[948];
			Vector3I16 angles;
			int32_t hitCount = 0;
			int32_t candidates = 0;
			int32_t bestDist = 0x7FFFFFFF;
			int32_t prevMesh = -1;
			int32_t meshIdx;
			int32_t i;
			int32_t scaledRadius;
			int32_t startX = 0;
			int32_t startY = 0;
			int32_t startZ = 0;
			int32_t endX = 0;
			int32_t endY = 0;
			int32_t endZ = 0;
			int32_t distance;
			int32_t endDistance;
			int32_t distanceDiff;
			int32_t hitX;
			int32_t hitY;
			int32_t cellX;
			int32_t cellY;
			int32_t cellZ;
			int32_t localX;
			int32_t localY;
			int32_t localZ;
			int32_t hitT0 = 0;
			int32_t hitT1 = 0;
			int32_t normalData[2] = { 0, 0 };
			int32_t packedNormal;
			int32_t platformIdx;

			if (Toy2::Collision::g_candidateTriangleCount - 1 >= 0)
			{
				scaledRadius = radius / 32;

				for (i = Toy2::Collision::g_candidateTriangleCount - 1; i >= 0; i--)
				{
					meshIdx = Toy2::Collision::g_candidateMeshIdx[i + 1];
					triangle = Toy2::Collision::g_candidateTris[i];

					if (meshIdx != prevMesh)
					{
						prevMesh = meshIdx;
						mesh = &Toy2::Collision::g_collisionMeshInstances[meshIdx];

						// the sweep is moved into the space of the mesh, a rotated platform turns it
						if (mesh->typeFlags == 8 && (Toy2::Platform::g_platformDescriptors[mesh->platformIdx].flags & 8) != 0)
						{
							platformIdx = mesh->platformIdx;

							angles.x = Toy2::Platform::g_platformDescriptors[platformIdx].rotAngle.x >> 2;
							angles.y = Toy2::Platform::g_platformDescriptors[platformIdx].rotAngle.y >> 2;
							angles.z = Toy2::Platform::g_platformDescriptors[platformIdx].rotAngle.z >> 2;

							Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, rotation);

							localX = position[0] - mesh->origin.x;
							localY = position[1] - mesh->origin.y;
							localZ = position[2] - mesh->origin.z;

							startX = (localX * rotation->m00 + localY * rotation->m10 + localZ * rotation->m20) / 4096;
							startY = (localZ * rotation->m21 + localX * rotation->m01 + localY * rotation->m11) / 4096;
							startZ = (localX * rotation->m02 + localY * rotation->m12 + localZ * rotation->m22) / 4096;

							localZ = displacement[2] - mesh->origin.z + position[2];
							localY = position[1] + displacement[1] - mesh->origin.y;
							localX = position[0] + displacement[0] - mesh->origin.x;

							endX = (localX * rotation->m00 + localY * rotation->m10 + localZ * rotation->m20) / 4096;
							endY = (localZ * rotation->m21 + localX * rotation->m01 + localY * rotation->m11) / 4096;
							endZ = (localX * rotation->m02 + localY * rotation->m12 + localZ * rotation->m22) / 4096;
						}
						else
						{
							startX = position[0] - mesh->origin.x;
							startY = position[1] - mesh->origin.y;
							startZ = position[2] - mesh->origin.z;
							endX = position[0] + displacement[0] - mesh->origin.x;
							endY = position[1] + displacement[1] - mesh->origin.y;
							endZ = position[2] + displacement[2] - mesh->origin.z;
						}
					}

					cellX = endX >> 5;

					distance =
						((triangle->unk24 * ((endZ >> 5) - triangle->v0z) + triangle->unk22 * ((endY >> 5) - triangle->v0y)
						  + triangle->unk20 * ((endX >> 5) - triangle->v0x))
						 >> 14)
						- scaledRadius;

					if (distance >= 0
						|| (endDistance =
								((triangle->unk24 * ((startZ >> 5) - triangle->v0z) + triangle->unk22 * ((startY >> 5) - triangle->v0y)
								  + triangle->unk20 * ((startX >> 5) - triangle->v0x))
								 >> 14)
								- scaledRadius,
							endDistance < 0))
					{
						cellY = endY >> 5;
					}
					else
					{
						distanceDiff = endDistance - distance;
						hitX = startX + endDistance * (endX - startX) / (endDistance - distance) - ((radius * triangle->unk20) >> 14);
						hitY = startY + endDistance * (endY - startY) / (endDistance - distance) - ((radius * triangle->unk22) >> 14);

						if (Math::PointIntersectsTriangle(
								(hitX >> 5) - triangle->v0x,
								(hitY >> 5) - triangle->v0y,
								((startZ + endDistance * (endZ - startZ) / (endDistance - distance) - ((radius * triangle->unk24) >> 14)) >> 5) - triangle->v0z,
								triangle->e1x,
								triangle->e1y,
								triangle->e1z,
								triangle->e2x,
								triangle->e2y,
								triangle->e2z,
								(Vector3I16*)&triangle->unk20,
								radius)
							&& (endDistance << 14) / distanceDiff < bestDist)
						{
							hitT0 = endDistance;
							hitT1 = distance;
							bestDist = (endDistance << 14) / distanceDiff;
							memcpy(&normalData[0], &triangle->unk20, 4);
							memcpy(&normalData[1], &triangle->unk24, 4);
							++hitCount;
						}

						cellY = endY >> 5;
						cellX = endX >> 5;
					}

					if (triangle->unk28 != 0x7FFF)
					{
						distance =
							((triangle->unk2A * ((endZ >> 5) - triangle->e1z - triangle->v0z) + triangle->unk28 * (cellY - triangle->e1y - triangle->v0y)
							  + triangle->unk26 * (cellX - triangle->e1x - triangle->v0x))
							 >> 14)
							- scaledRadius;

						if (distance < 0)
						{
							endDistance =
								((triangle->unk2A * ((startZ >> 5) - triangle->e1z - triangle->v0z)
								  + triangle->unk28 * ((startY >> 5) - triangle->e1y - triangle->v0y)
								  + triangle->unk26 * ((startX >> 5) - triangle->e1x - triangle->v0x))
								 >> 14)
								- scaledRadius;

							if (endDistance >= 0)
							{
								distanceDiff = endDistance - distance;
								hitX = startX + endDistance * (endX - startX) / (endDistance - distance) - ((radius * triangle->unk26) >> 14);
								hitY = startY + endDistance * (endY - startY) / (endDistance - distance) - ((radius * triangle->unk28) >> 14);

								if (Math::PointIntersectsTriangle(
										(hitX >> 5) - triangle->e1x - triangle->v0x,
										(hitY >> 5) - triangle->e1y - triangle->v0y,
										((startZ + endDistance * (endZ - startZ) / (endDistance - distance) - ((radius * triangle->unk2A) >> 14)) >> 5)
											- triangle->e1z - triangle->v0z,
										triangle->e3x - triangle->e1x,
										triangle->e3y - triangle->e1y,
										triangle->e3z - triangle->e1z,
										triangle->e2x - triangle->e1x,
										triangle->e2y - triangle->e1y,
										triangle->e2z - triangle->e1z,
										(Vector3I16*)&triangle->unk26,
										radius))
								{
									if ((endDistance << 14) / distanceDiff < bestDist)
									{
										hitT0 = endDistance;
										bestDist = (endDistance << 14) / distanceDiff;
										hitT1 = distance;
										memcpy(&normalData[0], &triangle->unk26, 4);
										memcpy(&normalData[1], &triangle->unk2A, 4);
										++hitCount;
									}
								}
							}
						}
					}
				}
			}

			if (SweepAgainstEdges(&bestDist, &hitT0, &hitT1, (uint32_t*)normalData, position, displacement, radius) == 1)
				++hitCount;

			if ((int16_t)hitCount <= 0)
			{
				position[0] += displacement[0];
				position[1] = displacement[1] + position[1];
				position[2] = displacement[2] + position[2];

				return 0;
			}

			distanceDiff = hitT0 - hitT1;
			position[0] += hitT0 * displacement[0] / (hitT0 - hitT1);
			position[1] += hitT0 * displacement[1] / distanceDiff;
			position[2] += hitT0 * displacement[2] / distanceDiff;

			Toy2::Collision::g_floorSlope.x = (int16_t)normalData[0];
			Toy2::Collision::g_floorSlope.y = (int16_t)(normalData[0] >> 16);
			Toy2::Collision::g_floorSlope.z = (int16_t)normalData[1];

			return 1;
		}

		// FUNCTION: TOY2 0x0048C030
		int32_t RaycastAgainstCandidates(int32_t* direction, int32_t* position, int32_t forwardedMode)
		{
			Toy2::Collision::CollisionTriangle* triangle;
			Toy2::Collision::CollisionMeshInstance* mesh;
			Matrix3x3I16* rotation = (Matrix3x3I16*)&Toy2::Collision::g_mathScratch[948];
			Vector3I16 angles;
			int32_t hitCount = 0;
			int32_t result = 0;
			int32_t bestDist = 0x7FFFFFFF;
			int32_t prevMesh = -1;
			int32_t index = 0;
			int32_t meshIdx;
			int32_t startX = 0;
			int32_t startY = 0;
			int32_t startZ = 0;
			int32_t endX = 0;
			int32_t endY = 0;
			int32_t endZ = 0;
			int32_t flipped = 0;
			int32_t startDistance;
			int32_t endDistance;
			int32_t distanceDiff;
			int32_t hitX;
			int32_t hitY;
			int32_t localX;
			int32_t localY;
			int32_t localZ;
			int32_t hitT0 = 0;
			int32_t hitT1 = 0;
			int32_t normalData[2] = { 0, 0 };
			int32_t isFlipped;
			int32_t platformIdx;

			if (Toy2::Collision::g_candidateTriangleCount > 0)
			{
				while (1)
				{
					meshIdx = Toy2::Collision::g_candidateMeshIdx[index + 1];
					triangle = Toy2::Collision::g_candidateTris[index];

					if (meshIdx != prevMesh)
					{
						prevMesh = meshIdx;
						mesh = &Toy2::Collision::g_collisionMeshInstances[meshIdx];

						if (mesh->typeFlags == 8 && (Toy2::Platform::g_platformDescriptors[mesh->platformIdx].flags & 8) != 0)
						{
							platformIdx = mesh->platformIdx;

							angles.x = Toy2::Platform::g_platformDescriptors[platformIdx].rotAngle.x >> 2;
							angles.y = Toy2::Platform::g_platformDescriptors[platformIdx].rotAngle.y >> 2;
							angles.z = Toy2::Platform::g_platformDescriptors[platformIdx].rotAngle.z >> 2;

							Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, rotation);

							localX = position[0] - mesh->origin.x;
							localY = position[1] - mesh->origin.y;
							localZ = position[2] - mesh->origin.z;

							startX = (localX * rotation->m00 + localY * rotation->m10 + localZ * rotation->m20) / 4096;
							startY = (localX * rotation->m01 + localY * rotation->m11 + localZ * rotation->m21) / 4096;
							startZ = (localX * rotation->m02 + localY * rotation->m12 + localZ * rotation->m22) / 4096;

							localZ = position[2] + direction[2] - mesh->origin.z;
							localY = position[1] + direction[1] - mesh->origin.y;
							localX = position[0] + direction[0] - mesh->origin.x;

							endX = (localX * rotation->m00 + localY * rotation->m10 + localZ * rotation->m20) / 4096;
							endY = (localX * rotation->m01 + localY * rotation->m11 + localZ * rotation->m21) / 4096;
							endZ = (localY * rotation->m12 + localZ * rotation->m22 + localX * rotation->m02) / 4096;
						}
						else
						{
							startX = position[0] - mesh->origin.x;
							startY = position[1] - mesh->origin.y;
							startZ = position[2] - mesh->origin.z;
							endX = position[0] + direction[0] - mesh->origin.x;
							endY = position[1] + direction[1] - mesh->origin.y;
							endZ = position[2] + direction[2] - mesh->origin.z;
						}
					}

					isFlipped = 0;

					endDistance =
						((triangle->unk24 * ((endZ >> 5) - triangle->v0z - triangle->e1z) + triangle->unk22 * ((endY >> 5) - triangle->v0y - triangle->e1y)
						  + triangle->unk20 * ((endX >> 5) - triangle->v0x - triangle->e1x))
						 >> 14);
					startDistance =
						((triangle->unk24 * ((startZ >> 5) - triangle->v0z - triangle->e1z) + triangle->unk22 * ((startY >> 5) - triangle->v0y - triangle->e1y)
						  + triangle->unk20 * ((startX >> 5) - triangle->v0x - triangle->e1x))
						 >> 14);

					// NOTE: the names are swapped, the first of them is the distance of the end of the ray
					if (startDistance < endDistance)
					{
						startDistance = -startDistance;
						endDistance = -endDistance;
						isFlipped = 1;
					}

					if (endDistance < 0 && startDistance >= 0)
					{
						distanceDiff = startDistance - endDistance;
						hitX = startX + startDistance * (endX - startX) / (startDistance - endDistance);
						hitY = startY + startDistance * (endY - startY) / (startDistance - endDistance);

						if (IsPointInTriangle(
								(hitX >> 5) - triangle->v0x,
								(hitY >> 5) - triangle->v0y,
								((startZ + startDistance * (endZ - startZ) / (startDistance - endDistance)) >> 5) - triangle->v0z,
								triangle->e1x,
								triangle->e1y,
								triangle->e1z,
								triangle->e2x,
								triangle->e2y,
								triangle->e2z,
								(Vector3I16*)&triangle->unk20))
						{
							if ((startDistance << 14) / distanceDiff < bestDist)
							{
								hitT0 = startDistance;
								hitT1 = endDistance;
								bestDist = (startDistance << 14) / distanceDiff;
								memcpy(&normalData[0], &triangle->unk20, 4);
								memcpy(&normalData[1], &triangle->unk24, 4);
								flipped = isFlipped;
								++hitCount;
							}
						}
					}

					if (triangle->unk28 != 0x7FFF)
					{
						isFlipped = 0;

						endDistance =
							(triangle->unk2A * (endZ - 32 * triangle->v0z - 32 * triangle->e3z)
							 + triangle->unk28 * (endY - 32 * triangle->v0y - 32 * triangle->e3y)
							 + triangle->unk26 * (endX - 32 * triangle->v0x - 32 * triangle->e3x))
							>> 14;
						startDistance =
							(triangle->unk2A * (startZ - 32 * triangle->v0z - 32 * triangle->e3z)
							 + triangle->unk28 * (startY - 32 * triangle->v0y - 32 * triangle->e3y)
							 + triangle->unk26 * (startX - 32 * triangle->e3x - 32 * triangle->v0x))
							>> 14;

						if (startDistance < endDistance)
						{
							startDistance = -startDistance;
							endDistance = -endDistance;
							isFlipped = 1;
						}

						if (endDistance < 0 && startDistance >= 0)
						{
							distanceDiff = startDistance - endDistance;
							hitX = startX + startDistance * (endX - startX) / (startDistance - endDistance);
							hitY = startY + startDistance * (endY - startY) / (startDistance - endDistance);

							if (IsPointInTriangle(
									(hitX >> 5) - triangle->e3x - triangle->v0x,
									(hitY >> 5) - triangle->e3y - triangle->v0y,
									((startZ + startDistance * (endZ - startZ) / (startDistance - endDistance)) >> 5) - triangle->e3z - triangle->v0z,
									triangle->e2x - triangle->e3x,
									triangle->e2y - triangle->e3y,
									triangle->e2z - triangle->e3z,
									triangle->e1x - triangle->e3x,
									triangle->e1y - triangle->e3y,
									triangle->e1z - triangle->e3z,
									(Vector3I16*)&triangle->unk26))
							{
								if ((startDistance << 14) / distanceDiff < bestDist)
								{
									bestDist = (startDistance << 14) / distanceDiff;
									hitT0 = startDistance;
									hitT1 = endDistance;
									memcpy(&normalData[0], &triangle->unk26, 4);
									memcpy(&normalData[1], &triangle->unk2A, 4);
									flipped = isFlipped;
									++hitCount;
								}
							}
						}
					}

					if (++index >= Toy2::Collision::g_candidateTriangleCount)
					{
						result = hitCount;
						break;
					}
				}
			}

			if (RaycastAgainstEdges(&bestDist, &hitT0, &hitT1, (uint32_t*)normalData, (int16_t*)&flipped, position, direction) == 1)
				++result;

			if ((int16_t)result <= 0 || (int16_t)flipped != 1)
				return 0;

			distanceDiff = hitT0 - hitT1;
			position[0] += hitT0 * direction[0] / (hitT0 - hitT1);
			position[1] += hitT0 * direction[1] / distanceDiff;
			position[2] += hitT0 * direction[2] / distanceDiff;

			return 1;
		}

		// FUNCTION: TOY2 0x0048C860
		int32_t SweepAndSlide(Transform* startPos, Transform* displacement, int32_t collisionMask, int32_t useTopSector, int32_t radius)
		{
			Toy2::Collision::CollisionCacheEntry* cameraEntry = (Toy2::Collision::CollisionCacheEntry*)Toy2::Collision::g_unk72871C + 8;
			Toy2::Collision::CollisionMeshInstance* mesh;
			Toy2::Collision::CollisionTreeNode* node;
			Toy2::Collision::CollisionTriangle* triangle;
			Matrix3x3I16* rotation = (Matrix3x3I16*)&Toy2::Collision::g_mathScratch[948];
			int16_t* meshList = (int16_t*)Toy2::Collision::g_mathScratch;
			Vector3I16 angles;
			Transform result;
			int32_t movement[3];
			int32_t minX;
			int32_t minY;
			int32_t minZ;
			int32_t extX;
			int32_t extY;
			int32_t extZ;
			int32_t padX;
			int32_t padY;
			int32_t padZ;
			int32_t cellExtX;
			int32_t cellExtY;
			int32_t cellExtZ;
			int32_t extXWide;
			int32_t extYWide;
			int32_t extZWide;
			int32_t meshMask;
			int32_t prevMesh = -1;
			int32_t valid = 0;
			int32_t cellX = 0;
			int32_t cellY = 0;
			int32_t cellZ = 0;
			int32_t meshIdx;
			int32_t meshCount;
			int32_t platformCount;
			int32_t i;
			int32_t j;
			int32_t count;
			int32_t speedSq;
			int32_t speed;
			int32_t platformIdx;
			int32_t platformReach;
			int32_t platformX;
			int32_t platformY;
			int32_t platformZ;
			int32_t localX;
			int32_t localY;
			int32_t localZ;
			int32_t originX;
			int32_t originY;
			int32_t originZ;
			int32_t doubleReach;
			int32_t nodeMargin;
			int32_t wallMinX;
			int32_t wallMinZ;
			int32_t wallMaxX;
			int32_t wallMaxZ;
			int32_t edgeCount;
			int32_t blockStart;
			int32_t blockEnd;
			int32_t blockLimit;
			int32_t steps;
			uint8_t* edgeNode;
			int32_t* edgeBlock;
			int32_t* edgePoint;
			uint16_t* platformMeshes;
			int32_t hit;

			Toy2::Collision::g_unk72D2AC = 0;
			Toy2::Collision::g_unk7291D8 = 0;

			movement[0] = displacement->pos.x;
			movement[1] = displacement->pos.y;
			movement[2] = displacement->pos.z;

			if (displacement->pos.x < 0)
			{
				minX = startPos->pos.x + 2048;
				extX = 4096 - displacement->pos.x;
			}
			else
			{
				minX = startPos->pos.x + displacement->pos.x + 2048;
				extX = displacement->pos.x + 4096;
			}

			if (displacement->pos.y >= 0)
			{
				minY = startPos->pos.y + displacement->pos.y + 2048;
				extY = displacement->pos.y + 4096;
			}
			else
			{
				minY = startPos->pos.y + 2048;
				extY = 4096 - displacement->pos.y;
			}

			if (displacement->pos.z >= 0)
			{
				minZ = startPos->pos.z + displacement->pos.z + 2048;
				extZ = displacement->pos.z + 4096;
			}
			else
			{
				minZ = startPos->pos.z + 2048;
				extZ = 4096 - displacement->pos.z;
			}

			Toy2::Collision::g_candidateTriangleCount = 0;

			padX = 5700 * radius / 4096;
			minZ = padX + minZ;
			minY = padX + minY;
			minX = padX + minX;
			padY = 5700 * radius / 2048;
			extXWide = padY + extX;
			extZWide = padY + extZ;
			cellExtX = ((uint32_t)extXWide + 31) >> 5;
			cellExtY = ((uint32_t)(padY + extY) + 31) >> 5;
			cellExtZ = ((uint32_t)extZWide + 31) >> 5;
			extYWide = padY + extY;

			meshMask = useTopSector != 0 ? 512 : 256;

			if (useTopSector && minX < cameraEntry->x && minY < cameraEntry->y && minZ < cameraEntry->z
				&& (uint32_t)(minX - extXWide) > (uint32_t)cameraEntry->unkC && (uint32_t)(minY - extYWide) > (uint32_t)cameraEntry->unk10
				&& (uint32_t)(minZ - extZWide) > (uint32_t)cameraEntry->unk14)
			{
				// everything is inside of the box that was gathered by PrepareCameraCollisionCandidates
				for (i = cameraEntry->count - 1; i >= 0; i--)
				{
					meshIdx = cameraEntry->meshIdx[i];

					if (meshIdx == prevMesh)
					{
						if (! valid)
							continue;
					}
					else
					{
						prevMesh = meshIdx;
						valid = 0;
						mesh = &Toy2::Collision::g_collisionMeshInstances[meshIdx];

						if ((uint32_t)(minX - mesh->boundsMin.x) < (uint32_t)(extXWide + mesh->boundsExt.x)
							&& (uint32_t)(minY - mesh->boundsMin.y) < (uint32_t)(extYWide + mesh->boundsExt.y)
							&& (uint32_t)(minZ - mesh->boundsMin.z) < (uint32_t)(extZWide + mesh->boundsExt.z) && mesh->typeFlags
							&& (mesh->unk & meshMask) == 0)
						{
							valid = 1;
							cellX = (minX - mesh->origin.x) / 32;
							cellY = (minY - mesh->origin.y) / 32;
							cellZ = (minZ - mesh->origin.z) / 32;
						}
						else
							continue;
					}

					triangle = cameraEntry->triangles[i];

					if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(cellExtX + triangle->extX)
						&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z) < (uint32_t)(cellExtZ + (triangle->extZByte << 6))
						&& (uint32_t)(cellY - (triangle->minYByte << 6) - triangle->v0y) < (uint32_t)(cellExtY + (triangle->extYByte << 6)))
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
				meshCount = 0;

				for (i = 0; i < Toy2::Collision::g_unk729120; i++)
				{
					Toy2::Collision::CollisionGridCell* cell = &Toy2::Collision::g_collisionGrid[i];

					if ((uint32_t)(minX - cell->minX) < (uint32_t)(extXWide + cell->extX) && (uint32_t)(minZ - cell->minZ) < (uint32_t)(extZWide + cell->extZ))
					{
						for (j = 0; j < cell->count; j++)
							meshList[meshCount + j] = Toy2::Collision::g_collisionMeshList[cell->start + j];

						meshCount += cell->count;
					}
				}

				for (i = 0; i < meshCount; i++)
				{
					meshIdx = meshList[i];
					mesh = &Toy2::Collision::g_collisionMeshInstances[meshIdx];

					if ((uint32_t)(minX - mesh->boundsMin.x) < (uint32_t)(extXWide + mesh->boundsExt.x)
						&& (uint32_t)(minY - mesh->boundsMin.y) < (uint32_t)(extYWide + mesh->boundsExt.y)
						&& (uint32_t)(minZ - mesh->boundsMin.z) < (uint32_t)(extZWide + mesh->boundsExt.z))
					{
						if (mesh->typeFlags)
						{
							if ((mesh->unk & meshMask) == 0)
							{
								cellY = (minY - mesh->origin.y) / 32;
								node = (Toy2::Collision::CollisionTreeNode*)mesh->collisionTree;
								cellX = (minX - mesh->origin.x) / 32;
								cellZ = (minZ - mesh->origin.z) / 32;

								while (node->flag >= 0)
								{
									count = node->count;
									triangle = (Toy2::Collision::CollisionTriangle*)(node + 1);

									if ((uint32_t)(cellX - node->minX) >= (uint32_t)(cellExtX + node->extX)
										|| (uint32_t)(cellZ - node->minZ) >= (uint32_t)(cellExtZ + node->extZ))
									{
										node = (Toy2::Collision::CollisionTreeNode*)(triangle + count);
									}
									else
									{
										for (j = 0; j < count; j++)
										{
											if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(cellExtX + triangle->extX)
												&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z)
													< (uint32_t)(cellExtZ + (triangle->extZByte << 6))
												&& (uint32_t)(cellY - (triangle->minYByte << 6) - triangle->v0y)
													< (uint32_t)(cellExtY + (triangle->extYByte << 6))
												&& triangle->unk22 < collisionMask && Toy2::Collision::g_candidateTriangleCount < 240)
											{
												Toy2::Collision::g_candidateTris[Toy2::Collision::g_candidateTriangleCount] = triangle;
												Toy2::Collision::g_candidateTriangleCount++;
												Toy2::Collision::g_candidateMeshIdx[Toy2::Collision::g_candidateTriangleCount] = (int16_t)meshIdx;
											}

											triangle++;
										}

										node = (Toy2::Collision::CollisionTreeNode*)triangle;
									}
								}
							}
						}
					}
				}
			}

			// the platforms are always checked
			platformMeshes = &Toy2::Collision::g_collisionMeshList[Toy2::Collision::g_unk7284A0];
			speedSq = movement[2] / 16 * (movement[2] / 16) + movement[1] / 16 * (movement[1] / 16) + movement[0] / 16 * (movement[0] / 16);
			speed = 16 * (int32_t)sqrt((double)speedSq);

			for (platformCount = Toy2::Collision::g_unk7284A2; platformCount > 0; platformCount--, platformMeshes++)
			{
				mesh = &Toy2::Collision::g_collisionMeshInstances[(int16_t)*platformMeshes];
				originY = mesh->origin.y;
				originX = mesh->origin.x;
				originZ = mesh->origin.z;
				localZ = startPos->pos.z;
				localY = startPos->pos.y;
				localX = startPos->pos.x;

				if (((originZ - localZ) >> 6) * ((originZ - localZ) >> 6) + ((originY - localY) >> 6) * ((originY - localY) >> 6)
						+ ((originX - localX) >> 6) * ((originX - localX) >> 6)
					<= speedSq + (mesh->boundingSqRadius >> 1))
				{
					platformIdx = mesh->platformIdx;
					platformReach = abs(Toy2::Platform::g_platformDescriptors[platformIdx].linVel.z) + speed
						+ abs(Toy2::Platform::g_platformDescriptors[platformIdx].linVel.y) + abs(Toy2::Platform::g_platformDescriptors[platformIdx].linVel.x);
					doubleReach = 2 * platformReach;
					nodeMargin = (platformReach + 15) / 16;

					if ((Toy2::Platform::g_platformDescriptors[platformIdx].flags & 8) != 0)
					{
						angles.x = Toy2::Platform::g_platformDescriptors[platformIdx].rotAngle.x >> 2;
						angles.y = Toy2::Platform::g_platformDescriptors[platformIdx].rotAngle.y >> 2;
						angles.z = Toy2::Platform::g_platformDescriptors[platformIdx].rotAngle.z >> 2;

						Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, rotation);

						originZ = mesh->origin.z;
						originY = mesh->origin.y;
						originX = mesh->origin.x;

						localY = startPos->pos.y - originY;
						localZ = startPos->pos.z - originZ;
						localX = startPos->pos.x - mesh->origin.x;

						platformX = platformReach + mesh->origin.x + (localZ * rotation->m20 + localY * rotation->m10 + localX * rotation->m00) / 4096;
						platformY = platformReach + originY + (localZ * rotation->m21 + localY * rotation->m11 + localX * rotation->m01) / 4096;
						platformZ = originZ + (localZ * rotation->m22 + localY * rotation->m12 + localX * rotation->m02) / 4096;
					}
					else
					{
						platformX = startPos->pos.x + platformReach;
						platformY = startPos->pos.y + platformReach;
						platformZ = startPos->pos.z;
					}

					platformZ = platformReach + platformZ;

					if (platformX - mesh->boundsMin.x < doubleReach + mesh->boundsExt.x && platformZ - mesh->boundsMin.z < doubleReach + mesh->boundsExt.z)
					{
						if (mesh->typeFlags)
						{
							if ((mesh->unk & meshMask) == 0)
							{
								node = (Toy2::Collision::CollisionTreeNode*)mesh->collisionTree;
								cellX = (platformX - originX) / 32;
								cellZ = (platformZ - originZ) / 32;

								while (node->flag >= 0)
								{
									count = node->count;
									triangle = (Toy2::Collision::CollisionTriangle*)(node + 1);

									if ((uint32_t)(cellX - node->minX) >= (uint32_t)(nodeMargin + node->extX)
										|| (uint32_t)(cellZ - node->minZ) >= (uint32_t)(nodeMargin + node->extZ))
									{
										node = (Toy2::Collision::CollisionTreeNode*)(triangle + count);
									}
									else
									{
										for (j = 0; j < count; j++)
										{
											if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(nodeMargin + triangle->extX)
												&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z)
													< (uint32_t)(nodeMargin + (triangle->extZByte << 6))
												&& (uint32_t)((platformY - originY) / 32 - (triangle->minYByte << 6) - triangle->v0y)
													< (uint32_t)(nodeMargin + (triangle->extYByte << 6))
												&& Toy2::Collision::g_candidateTriangleCount < 240)
											{
												Toy2::Collision::g_candidateTris[Toy2::Collision::g_candidateTriangleCount] = triangle;
												Toy2::Collision::g_candidateTriangleCount++;
												Toy2::Collision::g_candidateMeshIdx[Toy2::Collision::g_candidateTriangleCount] = *platformMeshes;
											}

											triangle++;
										}

										node = (Toy2::Collision::CollisionTreeNode*)triangle;
									}
								}
							}
						}
					}
				}
			}

			// the wall segments
			wallMinX = (minX - extXWide) / 32;
			wallMinZ = (minZ - extZWide) / 32;
			wallMaxX = minX / 32;
			wallMaxZ = minZ / 32;
			edgeCount = Toy2::Collision::g_unk72D2AC;
			edgeNode = (uint8_t*)Toy2::Collision::g_unk7290F0[1];

			if (edgeNode)
			{
				do
				{
					blockStart = 0;
					blockEnd = *(uint16_t*)edgeNode;

					if (blockEnd)
					{
						edgeBlock = (int32_t*)(edgeNode + 8);

						do
						{
							if (edgeBlock[0] == 0x80000000)
							{
								blockLimit = blockEnd;
							}
							else if (
								(uint32_t)(wallMaxX - edgeBlock[0]) >= (uint32_t)(cellExtX + edgeBlock[3])
								|| (uint32_t)(wallMaxZ - edgeBlock[6]) >= (uint32_t)(cellExtZ + edgeBlock[9]))
							{
								blockLimit = 0;
							}
							else
							{
								blockLimit = blockStart + 16;
							}

							if (blockStart < blockLimit)
							{
								edgePoint = edgeBlock + 1;

								for (steps = blockLimit - blockStart; steps; steps--)
								{
									if (((edgePoint[-2] >= wallMinX && edgePoint[1] <= wallMaxX) || (edgePoint[1] >= wallMinX && edgePoint[-2] <= wallMaxX))
										&& ((edgePoint[0] >= wallMinZ && edgePoint[3] <= wallMaxZ) || (edgePoint[3] >= wallMinZ && edgePoint[0] <= wallMaxZ))
										&& edgeCount < 32)
									{
										Toy2::Collision::g_unk728DA0[3 * edgeCount] = edgePoint[-2];
										Toy2::Collision::g_unk728DA0[3 * edgeCount + 1] = edgePoint[-1];
										Toy2::Collision::g_unk728DA0[3 * edgeCount + 2] = edgePoint[0];
										edgeCount++;
										Toy2::Collision::g_unk72D2AC = edgeCount;

										Toy2::Collision::g_unk728DA0[3 * edgeCount] = edgePoint[1];
										Toy2::Collision::g_unk728DA0[3 * edgeCount + 1] = edgePoint[2];
										Toy2::Collision::g_unk728DA0[3 * edgeCount + 2] = edgePoint[3];
										edgeCount++;
										Toy2::Collision::g_unk72D2AC = edgeCount;
									}

									edgePoint += 3;
								}
							}

							blockStart += 16;
							blockEnd = *(uint16_t*)edgeNode;
							edgeBlock += 48;
						} while (blockStart < blockEnd);
					}

					edgeNode = *(uint8_t**)(edgeNode - 4);
				} while (edgeNode);
			}

			hit = 0;

			if (Toy2::Collision::g_candidateTriangleCount > 0 || edgeCount > 0)
			{
				memcpy(&result, startPos, sizeof(result));

				hit = SweepAgainstCandidates(movement, &result.pos.x, radius);

				displacement->pos.x = result.pos.x - startPos->pos.x;
				displacement->pos.y = result.pos.y - startPos->pos.y;
				displacement->pos.z = result.pos.z - startPos->pos.z;
			}

			return hit;
		}

		// FUNCTION: TOY2 0x0048D530
		int32_t SweepQuery(int32_t* startPosition, int32_t* displacement, int32_t cameraMode, int32_t queryMode)
		{
			Toy2::Collision::CollisionCacheEntry* cameraEntry = (Toy2::Collision::CollisionCacheEntry*)Toy2::Collision::g_unk72871C + 8;
			Toy2::Collision::CollisionMeshInstance* mesh;
			Toy2::Collision::CollisionTreeNode* node;
			Toy2::Collision::CollisionTriangle* triangle;
			Matrix3x3I16* rotation = (Matrix3x3I16*)&Toy2::Collision::g_mathScratch[948];
			int16_t* meshList = (int16_t*)Toy2::Collision::g_mathScratch;
			Vector3I16 angles;
			int32_t query[4];
			int32_t movement[3];
			int32_t minX;
			int32_t minY;
			int32_t minZ;
			int32_t extX;
			int32_t extY;
			int32_t extZ;
			int32_t cellExtX;
			int32_t cellExtY;
			int32_t cellExtZ;
			int32_t meshMask;
			int32_t prevMesh = -1;
			int32_t valid = 0;
			int32_t cellX = 0;
			int32_t cellY = 0;
			int32_t cellZ = 0;
			int32_t meshIdx;
			int32_t meshCount;
			int32_t platformCount;
			int32_t listCount;
			int32_t i;
			int32_t j;
			int32_t count;
			int32_t speedSq;
			int32_t platformIdx;
			int32_t platformReach;
			int32_t platformX;
			int32_t platformY;
			int32_t platformZ;
			int32_t localX;
			int32_t localY;
			int32_t localZ;
			int32_t originX;
			int32_t originY;
			int32_t originZ;
			int32_t doubleReach;
			int32_t nodeMargin;
			int32_t wallMinX;
			int32_t wallMinZ;
			int32_t wallMaxX;
			int32_t wallMaxZ;
			int32_t edgeCount;
			int32_t blockStart;
			int32_t blockEnd;
			int32_t blockLimit;
			int32_t steps;
			uint8_t* edgeNode;
			int32_t* edgeBlock;
			int32_t* edgePoint;
			uint16_t* platformMeshes;
			int16_t* platformList;

			Toy2::Collision::g_unk72D2AC = 0;
			Toy2::Collision::g_candidateTriangleCount = 0;
			Toy2::Collision::g_unk7291D8 = 0;

			movement[0] = displacement[0];
			movement[1] = displacement[1];
			movement[2] = displacement[2];

			if (displacement[0] < 0)
			{
				minX = startPosition[0] + 2048;
				extX = 4096 - movement[0];
			}
			else
			{
				minX = startPosition[0] + movement[0] + 2048;
				extX = movement[0] + 4096;
			}

			if (movement[1] >= 0)
			{
				extY = movement[1] + 4096;
				minY = startPosition[1] + movement[1] + 2048;
			}
			else
			{
				extY = 4096 - movement[1];
				minY = startPosition[1] + 2048;
			}

			if (movement[2] >= 0)
			{
				minZ = startPosition[2] + movement[2] + 2048;
				extZ = movement[2] + 4096;
			}
			else
			{
				minZ = startPosition[2] + 2048;
				extZ = 4096 - movement[2];
			}

			cellExtX = ((uint32_t)extX + 31) >> 5;
			cellExtY = ((uint32_t)extY + 31) >> 5;
			cellExtZ = ((uint32_t)extZ + 31) >> 5;

			meshMask = cameraMode != 0 ? 512 : 256;

			if (cameraMode && minX < cameraEntry->x && minY < cameraEntry->y && minZ < cameraEntry->z && (uint32_t)(minX - extX) > (uint32_t)cameraEntry->unkC
				&& (uint32_t)(minY - extY) > (uint32_t)cameraEntry->unk10 && (uint32_t)(minZ - extZ) > (uint32_t)cameraEntry->unk14)
			{
				for (i = cameraEntry->count - 1; i >= 0; i--)
				{
					meshIdx = cameraEntry->meshIdx[i];

					if (meshIdx == prevMesh)
					{
						if (! valid)
							continue;
					}
					else
					{
						prevMesh = meshIdx;
						valid = 0;
						mesh = &Toy2::Collision::g_collisionMeshInstances[meshIdx];

						if ((uint32_t)(minX - mesh->boundsMin.x) < (uint32_t)(extX + mesh->boundsExt.x)
							&& (uint32_t)(minY - mesh->boundsMin.y) < (uint32_t)(extY + mesh->boundsExt.y)
							&& (uint32_t)(minZ - mesh->boundsMin.z) < (uint32_t)(extZ + mesh->boundsExt.z) && mesh->typeFlags && (mesh->unk & meshMask) == 0)
						{
							valid = 1;
							cellX = (minX - mesh->origin.x) / 32;
							cellY = (minY - mesh->origin.y) / 32;
							cellZ = (minZ - mesh->origin.z) / 32;
						}
						else
							continue;
					}

					triangle = cameraEntry->triangles[i];

					if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(cellExtX + triangle->extX)
						&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z) < (uint32_t)(cellExtZ + (triangle->extZByte << 6))
						&& (uint32_t)(cellY - (triangle->minYByte << 6) - triangle->v0y) < (uint32_t)(cellExtY + (triangle->extYByte << 6)))
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
				listCount = 0;

				for (i = 0; i < Toy2::Collision::g_unk729120; i++)
				{
					Toy2::Collision::CollisionGridCell* cell = &Toy2::Collision::g_collisionGrid[i];

					if ((uint32_t)(minX - cell->minX) < (uint32_t)(extX + cell->extX) && (uint32_t)(minZ - cell->minZ) < (uint32_t)(extZ + cell->extZ))
					{
						for (j = 0; j < cell->count; j++)
							meshList[listCount + j] = Toy2::Collision::g_collisionMeshList[cell->start + j];

						listCount += cell->count;
					}
				}

				// the platforms follow the meshes of the cells
				platformList = meshList + listCount;

				for (j = 0; j < Toy2::Collision::g_unk7284A2; j++)
					platformList[j] = Toy2::Collision::g_collisionMeshList[Toy2::Collision::g_unk7284A0 + j];

				meshCount = Toy2::Collision::g_unk7284A2 + listCount;

				for (i = 0; i < meshCount; i++)
				{
					meshIdx = meshList[i];
					mesh = &Toy2::Collision::g_collisionMeshInstances[meshIdx];

					if ((uint32_t)(minX - mesh->boundsMin.x) < (uint32_t)(extX + mesh->boundsExt.x)
						&& (uint32_t)(minY - mesh->boundsMin.y) < (uint32_t)(extY + mesh->boundsExt.y)
						&& (uint32_t)(minZ - mesh->boundsMin.z) < (uint32_t)(extZ + mesh->boundsExt.z))
					{
						if (mesh->typeFlags)
						{
							if ((mesh->unk & meshMask) == 0)
							{
								cellY = (minY - mesh->origin.y) / 32;
								node = (Toy2::Collision::CollisionTreeNode*)mesh->collisionTree;
								cellX = (minX - mesh->origin.x) / 32;
								cellZ = (minZ - mesh->origin.z) / 32;

								while (node->flag >= 0)
								{
									count = node->count;
									triangle = (Toy2::Collision::CollisionTriangle*)(node + 1);

									if ((uint32_t)(cellX - node->minX) >= (uint32_t)(cellExtX + node->extX)
										|| (uint32_t)(cellZ - node->minZ) >= (uint32_t)(cellExtZ + node->extZ))
									{
										node = (Toy2::Collision::CollisionTreeNode*)(triangle + count);
									}
									else
									{
										for (j = 0; j < count; j++)
										{
											if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(cellExtX + triangle->extX)
												&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z)
													< (uint32_t)(cellExtZ + (triangle->extZByte << 6))
												&& (uint32_t)(cellY - (triangle->minYByte << 6) - triangle->v0y)
													< (uint32_t)(cellExtY + (triangle->extYByte << 6))
												&& Toy2::Collision::g_candidateTriangleCount < 240)
											{
												Toy2::Collision::g_candidateTris[Toy2::Collision::g_candidateTriangleCount] = triangle;
												Toy2::Collision::g_candidateTriangleCount++;
												Toy2::Collision::g_candidateMeshIdx[Toy2::Collision::g_candidateTriangleCount] = (int16_t)meshIdx;
											}

											triangle++;
										}

										node = (Toy2::Collision::CollisionTreeNode*)triangle;
									}
								}
							}
						}
					}
				}
			}

			// the platforms again, with the area they move in
			platformMeshes = &Toy2::Collision::g_collisionMeshList[Toy2::Collision::g_unk7284A0];
			speedSq = movement[2] / 16 * (movement[2] / 16) + movement[1] / 16 * (movement[1] / 16) + movement[0] / 16 * (movement[0] / 16);

			for (platformCount = Toy2::Collision::g_unk7284A2; platformCount > 0; platformCount--, platformMeshes++)
			{
				localY = startPosition[1];
				localX = startPosition[0];
				mesh = &Toy2::Collision::g_collisionMeshInstances[(int16_t)*platformMeshes];
				originX = mesh->origin.x;
				originZ = mesh->origin.z;
				originY = mesh->origin.y;

				if (((originZ - startPosition[2]) >> 6) * ((originZ - startPosition[2]) >> 6) + ((originY - localY) >> 6) * ((originY - localY) >> 6)
						+ ((originX - localX) >> 6) * ((originX - localX) >> 6)
					<= speedSq + (mesh->boundingSqRadius >> 1))
				{
					platformIdx = mesh->platformIdx;
					platformReach = abs(Toy2::Platform::g_platformDescriptors[platformIdx].linVel.x)
						+ abs(Toy2::Platform::g_platformDescriptors[platformIdx].linVel.y) + abs(Toy2::Platform::g_platformDescriptors[platformIdx].linVel.z)
						+ 16 * (int32_t)sqrt((double)speedSq);
					doubleReach = 2 * platformReach;
					nodeMargin = (platformReach + 15) / 16;

					if ((Toy2::Platform::g_platformDescriptors[platformIdx].flags & 8) != 0)
					{
						angles.x = Toy2::Platform::g_platformDescriptors[platformIdx].rotAngle.x >> 2;
						angles.y = Toy2::Platform::g_platformDescriptors[platformIdx].rotAngle.y >> 2;
						angles.z = Toy2::Platform::g_platformDescriptors[platformIdx].rotAngle.z >> 2;

						Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, rotation);

						originZ = mesh->origin.z;
						originY = mesh->origin.y;
						localY = startPosition[1] - originY;
						originX = mesh->origin.x;
						localZ = startPosition[2] - originZ;
						localX = startPosition[0] - mesh->origin.x;

						platformX = platformReach + mesh->origin.x + (localZ * rotation->m20 + localY * rotation->m10 + localX * rotation->m00) / 4096;
						platformY = platformReach + originY + (localZ * rotation->m21 + localY * rotation->m11 + localX * rotation->m01) / 4096;
						platformZ = originZ + (localZ * rotation->m22 + localY * rotation->m12 + localX * rotation->m02) / 4096;
					}
					else
					{
						platformY = platformReach + startPosition[1];
						platformZ = startPosition[2];
						platformX = startPosition[0] + platformReach;
					}

					platformZ = platformReach + platformZ;

					if (platformX - mesh->boundsMin.x < doubleReach + mesh->boundsExt.x && platformZ - mesh->boundsMin.z < doubleReach + mesh->boundsExt.z)
					{
						if (mesh->typeFlags)
						{
							if ((mesh->unk & meshMask) == 0)
							{
								node = (Toy2::Collision::CollisionTreeNode*)mesh->collisionTree;
								cellY = (platformY - originY) / 32;
								cellX = (platformX - originX) / 32;
								cellZ = (platformZ - originZ) / 32;

								while (node->flag >= 0)
								{
									count = node->count;
									triangle = (Toy2::Collision::CollisionTriangle*)(node + 1);

									if ((uint32_t)(cellX - node->minX) >= (uint32_t)(nodeMargin + node->extX)
										|| (uint32_t)(cellZ - node->minZ) >= (uint32_t)(nodeMargin + node->extZ))
									{
										node = (Toy2::Collision::CollisionTreeNode*)(triangle + count);
									}
									else
									{
										for (j = 0; j < count; j++)
										{
											if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(nodeMargin + triangle->extX)
												&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z)
													< (uint32_t)(nodeMargin + (triangle->extZByte << 6))
												&& (uint32_t)(cellY - (triangle->minYByte << 6) - triangle->v0y)
													< (uint32_t)(nodeMargin + (triangle->extYByte << 6))
												&& Toy2::Collision::g_candidateTriangleCount < 240)
											{
												Toy2::Collision::g_candidateTris[Toy2::Collision::g_candidateTriangleCount] = triangle;
												Toy2::Collision::g_candidateTriangleCount++;
												Toy2::Collision::g_candidateMeshIdx[Toy2::Collision::g_candidateTriangleCount] = *platformMeshes;
											}

											triangle++;
										}

										node = (Toy2::Collision::CollisionTreeNode*)triangle;
									}
								}
							}
						}
					}
				}
			}

			// the wall segments
			wallMinX = (minX - extX) / 32;
			wallMinZ = (minZ - extZ) / 32;
			wallMaxX = minX / 32;
			wallMaxZ = minZ / 32;
			edgeCount = Toy2::Collision::g_unk72D2AC;
			edgeNode = (uint8_t*)Toy2::Collision::g_unk7290F0[1];

			if (edgeNode)
			{
				do
				{
					blockStart = 0;
					blockEnd = *(uint16_t*)edgeNode;

					if (blockEnd)
					{
						edgeBlock = (int32_t*)(edgeNode + 8);

						do
						{
							if (edgeBlock[0] == 0x80000000)
							{
								blockLimit = blockEnd;
							}
							else if (
								(uint32_t)(wallMaxX - edgeBlock[0]) >= (uint32_t)(cellExtX + edgeBlock[3])
								|| (uint32_t)(wallMaxZ - edgeBlock[6]) >= (uint32_t)(cellExtZ + edgeBlock[9]))
							{
								blockLimit = 0;
							}
							else
							{
								blockLimit = blockStart + 16;
							}

							if (blockStart < blockLimit)
							{
								edgePoint = edgeBlock + 1;

								for (steps = blockLimit - blockStart; steps; steps--)
								{
									if (((edgePoint[-2] >= wallMinX && edgePoint[1] <= wallMaxX) || (edgePoint[1] >= wallMinX && edgePoint[-2] <= wallMaxX))
										&& ((edgePoint[0] >= wallMinZ && edgePoint[3] <= wallMaxZ) || (edgePoint[3] >= wallMinZ && edgePoint[0] <= wallMaxZ))
										&& edgeCount < 32)
									{
										Toy2::Collision::g_unk728DA0[3 * edgeCount] = edgePoint[-2];
										Toy2::Collision::g_unk728DA0[3 * edgeCount + 1] = edgePoint[-1];
										Toy2::Collision::g_unk728DA0[3 * edgeCount + 2] = edgePoint[0];
										edgeCount++;
										Toy2::Collision::g_unk72D2AC = edgeCount;

										Toy2::Collision::g_unk728DA0[3 * edgeCount] = edgePoint[1];
										Toy2::Collision::g_unk728DA0[3 * edgeCount + 1] = edgePoint[2];
										Toy2::Collision::g_unk728DA0[3 * edgeCount + 2] = edgePoint[3];
										edgeCount++;
										Toy2::Collision::g_unk72D2AC = edgeCount;
									}

									edgePoint += 3;
								}
							}

							blockStart += 16;
							blockEnd = *(uint16_t*)edgeNode;
							edgeBlock += 48;
						} while (blockStart < blockEnd);
					}

					edgeNode = *(uint8_t**)(edgeNode - 4);
				} while (edgeNode);
			}

			if (Toy2::Collision::g_candidateTriangleCount <= 0 && edgeCount <= 0)
				return 0;

			query[0] = startPosition[0];
			query[1] = startPosition[1];
			query[2] = startPosition[2];
			query[3] = startPosition[3];

			i = RaycastAgainstCandidates(movement, query, queryMode);

			displacement[0] = query[0] - startPosition[0];
			displacement[1] = query[1] - startPosition[1];
			displacement[2] = query[2] - startPosition[2];

			return i;
		}
	}
}

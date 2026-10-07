#include "Toy2/Collision.h"
#include "Toy2/Levels.h"
#include "Toy2/Platform.h"
#include "Nu3D/Collision.h"
#include "Nu3D/Math.h"
#include "Renderer/Renderer.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

namespace Toy2
{
	namespace Collision
	{
		// GLOBAL: TOY2 0x007295A8
		CollisionMeshInstance g_collisionMeshInstances[300];

		// GLOBAL: TOY2 0x00554FA0
		uint8_t g_mathScratch[1024];

		// GLOBAL: TOY2 0x007270A0
		CollisionGridCell g_collisionGrid[256];

		// GLOBAL: TOY2 0x0072D2B0
		uint16_t g_collisionMeshList[2048];

		// GLOBAL: TOY2 0x00729178
		BuzzCollisionState g_buzzCollisionStates[2];

		// GLOBAL: TOY2 0x0072D2A0
		int32_t g_candidateTriangleCount;

		// GLOBAL: TOY2 0x007291DC
		CollisionTriangle* g_candidateTris[240];

		// GLOBAL: TOY2 0x007284B2
		// NOTE: the triangle k of the candidate list is at index k + 1, IDA names the word in front of the list
		int16_t g_candidateMeshIdx[241];

		// GLOBAL: TOY2 0x0072D2A6
		int16_t g_ceilingContactMeshIdx;

		// GLOBAL: TOY2 0x007290E8
		int16_t g_groundQueryResult;

		// GLOBAL: TOY2 0x007290E4
		int32_t g_unk7290E4;

		// GLOBAL: TOY2 0x007290A0
		int16_t g_unk7290A0[32];

		// GLOBAL: TOY2 0x0072D2A8
		int32_t g_unk72D2A8;

		// GLOBAL: TOY2 0x0072D2AC
		// NOTE: the number of points in g_unk728DA0
		int32_t g_unk72D2AC;

		// GLOBAL: TOY2 0x007291D8
		int16_t g_unk7291D8;

		// GLOBAL: TOY2 0x0072D298
		int16_t g_unk72D298;

		// GLOBAL: TOY2 0x007286A0
		CollisionTriangle* g_unk7286A0[19];

		// GLOBAL: TOY2 0x00729134
		int16_t g_unk729134[30];

		// GLOBAL: TOY2 0x00728DA0
		// NOTE: IDA sizes this as 12 bytes, up to 32 points of 3 ints are written
		int32_t g_unk728DA0[96];

		// GLOBAL: TOY2 0x0072959C
		int32_t g_unk72959C;

		// GLOBAL: TOY2 0x00729170
		int32_t g_unk729170;

		// GLOBAL: TOY2 0x00729130
		int16_t g_groundContactMeshIdx;

		// GLOBAL: TOY2 0x007284A0
		int16_t g_unk7284A0;

		// GLOBAL: TOY2 0x007284A2
		int16_t g_unk7284A2;

		// GLOBAL: TOY2 0x007295A0
		int32_t g_unk7295A0;

		// GLOBAL: TOY2 0x00729110
		int32_t g_unk729110;

		// GLOBAL: TOY2 0x00728718
		int32_t g_unk728718;

		// GLOBAL: TOY2 0x00728698
		// NOTE: IDA only names the x (+0) and z (+4) members, the y (+2) member is read as the high word of g_floorSlopeX
		Vector3I16 g_floorSlope;

		// GLOBAL: TOY2 0x00729118
		Vector3I16 g_floorSlopePrev;

		// GLOBAL: TOY2 0x0072871C
		uint8_t* g_unk72871C;

		// GLOBAL: TOY2 0x0072D2A4
		int16_t g_unk72D2A4;

		// GLOBAL: TOY2 0x007290E0
		int32_t g_unk7290E0;

		// GLOBAL: TOY2 0x0072912C
		int32_t g_unk72912C;

		// GLOBAL: TOY2 0x007290EC
		int32_t g_unk7290EC;

		// GLOBAL: TOY2 0x00729124
		int32_t g_unk729124;

		// GLOBAL: TOY2 0x00729120
		int32_t g_unk729120;

		// GLOBAL: TOY2 0x007290F0
		int32_t g_unk7290F0[8];

		// FUNCTION: TOY2 0x00489C30
		void BuildCollisionWorld(int32_t level, uint8_t** buffer, int32_t terrainNum)
		{
			char terrainFileName[260];
			int16_t meshCell[300];
			int32_t meshMinX[300];
			int32_t meshMinZ[300];
			int32_t meshMaxX[300];
			int32_t meshMaxZ[300];

			CollisionMeshInstance* mesh;
			CollisionTreeNode* node;
			CollisionTriangle* triangle;
			Platform::PlatformDescriptor* platform;
			int32_t i;
			int32_t j;
			int32_t value;

			g_unk7295A0 = 4096;
			g_unk729110 = -1;
			g_unk728718 = -1;
			g_unk72D2A4 = 0;
			g_unk7290E0 = 0;
			g_unk72912C = -1;

			// the counts of all the grid cells are cleared, the last two are the lists of the flagged meshes and of the type 8 meshes
			for (i = 0; i < 256; i++)
				g_collisionGrid[i].count = 0;

			g_unk7284A2 = 0;

			for (i = 0; i < 32; i++)
			{
				platform = &Platform::g_platformDescriptors[i];

				platform->meshInstanceId = -1;
				platform->rotAngle.x = 0;
				platform->rotAngle.y = 0;
				platform->rotAngle.z = 0;
				platform->angVel.x = 0;
				platform->angVel.y = 0;
				platform->angVel.z = 0;
				platform->prevAngVel.x = 0;
				platform->prevAngVel.y = 0;
				platform->prevAngVel.z = 0;
				platform->linVel.x = 0;
				platform->linVel.y = 0;
				platform->linVel.z = 0;
				platform->prevLinVel.x = 0;
				platform->prevLinVel.y = 0;
				platform->prevLinVel.z = 0;
				platform->flags = 0;
				platform->unkVar10 = 0;
			}

			switch (terrainNum)
			{
				case 1:
					Levels::BuildLevelPath(level, terrainFileName, "terr1");
					break;

				case 2:
					Levels::BuildLevelPath(level, terrainFileName, "terr2");
					break;

				case 3:
					Levels::BuildLevelPath(level, terrainFileName, "terr3");
					break;

				default:
					Levels::BuildLevelPath(level, terrainFileName, "terrain");
					break;
			}

			g_unk7290EC = Terrain::LoadAll(terrainFileName, 0, (void**)buffer);
			g_unk729124 = 0x80000000;

			// the bounds and the radius of every mesh, walking the triangles of its collision tree
			for (i = 0; i < 300; i++)
			{
				mesh = &g_collisionMeshInstances[i];

				if (mesh->typeFlags == 6 || mesh->typeFlags == 7 || mesh->typeFlags == 8)
				{
					node = (CollisionTreeNode*)mesh->collisionTree;

					int32_t minX = 0x7FFFFFFF;
					int32_t minY = 0x7FFFFFFF;
					int32_t minZ = 0x7FFFFFFF;
					int32_t maxX = 0x80000000;
					int32_t maxY = 0x80000000;
					int32_t maxZ = 0x80000000;
					int32_t radiusSq = 0;

					while (node->flag >= 0)
					{
						int32_t triangleCount = node->count;

						triangle = (CollisionTriangle*)(node + 1);

						if (triangleCount > 0)
						{
							int32_t remaining = triangleCount;

							do
							{
								if (triangle->minX < minX)
									minX = triangle->minX;

								value = triangle->minX + triangle->extX;

								if (value > maxX)
									maxX = value;

								value = (triangle->minYByte << 6) + triangle->v0y;

								if (value < minY)
									minY = value;

								value = (((triangle->extYByte << 6) + triangle->minYByte) << 6) + triangle->v0y;

								if (value > maxY)
									maxY = value;

								value = (triangle->minZByte << 6) + triangle->v0z;

								if (value < minZ)
									minZ = value;

								value = (((triangle->extZByte << 6) + triangle->minZByte) << 6) + triangle->v0z;

								if (value > maxZ)
									maxZ = value;

								value = (triangle->v0y << 5) + mesh->origin.y;

								if (value > g_unk729124)
									g_unk729124 = value;

								value = ((triangle->e1y + triangle->v0y) << 5) + mesh->origin.y;

								if (value > g_unk729124)
									g_unk729124 = value;

								value = ((triangle->e2y + triangle->v0y) << 5) + mesh->origin.y;

								if (value > g_unk729124)
									g_unk729124 = value;

								value = ((triangle->e3y + triangle->v0y) << 5) + mesh->origin.y;

								if (value > g_unk729124)
									g_unk729124 = value;

								value = triangle->v0z * triangle->v0z + triangle->v0y * triangle->v0y + triangle->v0x * triangle->v0x;

								if (radiusSq < value)
									radiusSq = value;

								value = (triangle->e1z + triangle->v0z) * (triangle->e1z + triangle->v0z) + (triangle->e1y + triangle->v0y) * (triangle->e1y + triangle->v0y) +
										(triangle->e1x + triangle->v0x) * (triangle->e1x + triangle->v0x);

								if (radiusSq < value)
									radiusSq = value;

								value = (triangle->e2z + triangle->v0z) * (triangle->e2z + triangle->v0z) + (triangle->e2y + triangle->v0y) * (triangle->e2y + triangle->v0y) +
										(triangle->e2x + triangle->v0x) * (triangle->e2x + triangle->v0x);

								if (radiusSq < value)
									radiusSq = value;

								value = (triangle->e3z + triangle->v0z) * (triangle->e3z + triangle->v0z) + (triangle->e3y + triangle->v0y) * (triangle->e3y + triangle->v0y) +
										(triangle->e3x + triangle->v0x) * (triangle->e3x + triangle->v0x);

								if (radiusSq < value)
									radiusSq = value;

								triangle++;
								--remaining;
							} while (remaining);
						}

						node = (CollisionTreeNode*)triangle;
					}

					mesh->boundingSqRadius = radiusSq;

					mesh->boundsMin.x = (minX << 5) + mesh->origin.x;
					mesh->boundsExt.x = (maxX << 5) - mesh->boundsMin.x + mesh->origin.x;
					mesh->boundsMin.y = (minY << 5) + mesh->origin.y;
					mesh->boundsExt.y = (maxY << 5) - mesh->boundsMin.y + mesh->origin.y;
					mesh->boundsMin.z = (minZ << 5) + mesh->origin.z;
					mesh->boundsExt.z = (maxZ << 5) - mesh->boundsMin.z + mesh->origin.z;

					++g_unk7290E0;
				}
			}

			// the bounds of the whole world in x and z, only from the meshes that are in the grid
			int32_t worldMinX = 0x7FFFFFFF;
			int32_t worldMinZ = 0x7FFFFFFF;
			int32_t worldMaxX = 0x80000000;
			int32_t worldMaxZ = 0x80000000;

			for (i = 0; i < 300; i++)
			{
				mesh = &g_collisionMeshInstances[i];

				if ((mesh->typeFlags == 6 || mesh->typeFlags == 7) && (mesh->unk & 0x400) == 0)
				{
					node = (CollisionTreeNode*)mesh->collisionTree;

					while (node->flag >= 0)
					{
						value = (node->minX << 5) + mesh->origin.x;

						if (value < worldMinX)
							worldMinX = value;

						value = ((node->extX + node->minX) << 5) + mesh->origin.x;

						if (value > worldMaxX)
							worldMaxX = value;

						value = (node->minZ << 5) + mesh->origin.z;

						if (value < worldMinZ)
							worldMinZ = value;

						value = ((node->extZ + node->minZ) << 5) + mesh->origin.z;

						if (value > worldMaxZ)
							worldMaxZ = value;

						node = (CollisionTreeNode*)((uint8_t*)node + 44 * node->count + 12);
					}
				}
			}

			// every mesh of the grid gets a cell, from the middle of its bounds, in a 5 by 5 grid over the world
			int32_t listCount = 0;
			g_unk7284A0 = 0;

			uint16_t* list = g_collisionMeshList;

			for (i = 0; i < 300; i++)
			{
				mesh = &g_collisionMeshInstances[i];

				if ((mesh->typeFlags == 6 || mesh->typeFlags == 7) && (mesh->unk & 0x400) == 0)
				{
					node = (CollisionTreeNode*)mesh->collisionTree;

					meshMinX[i] = 0x7FFFFFFF;
					meshMinZ[i] = 0x7FFFFFFF;
					meshMaxX[i] = 0x80000000;
					meshMaxZ[i] = 0x80000000;

					while (node->flag >= 0)
					{
						value = (node->minX << 5) + mesh->origin.x;

						if (value < meshMinX[i])
							meshMinX[i] = value;

						value = ((node->extX + node->minX) << 5) + mesh->origin.x;

						if (value > meshMaxX[i])
							meshMaxX[i] = value;

						value = (node->minZ << 5) + mesh->origin.z;

						if (value < meshMinZ[i])
							meshMinZ[i] = value;

						value = ((node->extZ + node->minZ) << 5) + mesh->origin.z;

						if (value > meshMaxZ[i])
							meshMaxZ[i] = value;

						node = (CollisionTreeNode*)((uint8_t*)node + 44 * node->count + 12);
					}

					int32_t cellX = 5 * ((meshMinX[i] + meshMaxX[i]) / 2 - worldMinX) / (worldMaxX - worldMinX);

					if (cellX < 0)
						cellX = 0;
					else if (cellX > 4)
						cellX = 4;

					int32_t cellZ = 5 * ((meshMinZ[i] + meshMaxZ[i]) / 2 - worldMinZ) / (worldMaxZ - worldMinZ);

					if (cellZ < 0)
					{
						meshCell[i] = cellX;
					}
					else
					{
						if (cellZ > 4)
							cellZ = 4;

						meshCell[i] = cellX + cellZ * 4 + cellZ;
					}
				}
				else if (mesh->typeFlags == 8 && (mesh->unk & 0x400) == 0)
				{
					*list = i;
					++g_unk7284A2;
					++listCount;
					++list;
				}
			}

			// the meshes that are flagged with 0x400 are a list of their own
			g_collisionGrid[255].start = listCount;

			int16_t flaggedCount = g_collisionGrid[255].count;
			list = &g_collisionMeshList[listCount];

			for (i = 0; i < 300; i++)
			{
				if (g_collisionMeshInstances[i].unk & 0x400)
				{
					*list = i;
					++listCount;
					++list;
					++flaggedCount;
				}
			}

			g_collisionGrid[255].count = flaggedCount;
			g_unk729120 = 0;

			CollisionGridCell* cell = g_collisionGrid;

			for (int32_t cellIdx = 0; cellIdx < 25; cellIdx++)
			{
				int32_t cellMinX = 0x7FFFFFFF;
				int32_t cellMinZ = 0x7FFFFFFF;
				int32_t cellMaxX = 0x80000000;
				int32_t cellMaxZ = 0x80000000;

				cell->start = listCount;
				list = &g_collisionMeshList[listCount];

				for (j = 0; j < 300; j++)
				{
					mesh = &g_collisionMeshInstances[j];

					if (meshCell[j] == cellIdx && (mesh->typeFlags == 6 || mesh->typeFlags == 7) && (mesh->unk & 0x400) == 0)
					{
						if (meshMinX[j] < cellMinX)
							cellMinX = meshMinX[j];

						if (meshMinZ[j] < cellMinZ)
							cellMinZ = meshMinZ[j];

						if (meshMaxX[j] > cellMaxX)
							cellMaxX = meshMaxX[j];

						if (meshMaxZ[j] > cellMaxZ)
							cellMaxZ = meshMaxZ[j];

						++cell->count;
						*list = j;
						++listCount;
						++list;
					}
				}

				// an empty cell is reused by the next one
				if (cell->count)
				{
					cell->minX = cellMinX;
					cell->minZ = cellMinZ;
					cell->extX = cellMaxX - cellMinX;
					cell->extZ = cellMaxZ - cellMinZ;

					++g_unk729120;
					++cell;
				}
			}

			for (i = 0; i < 2; i++)
			{
				g_buzzCollisionStates[i].contactSurfaceData = 0;
				g_buzzCollisionStates[i].contactSurfaceIndex = 0;
				g_buzzCollisionStates[i].stuckTimer = 0;
				g_buzzCollisionStates[i].floorOffset = 4000;
			}
		}

		// FUNCTION: TOY2 0x00480660
		int32_t SweepWallSegments(Toy2::Collision::RaycastQuery* rayQuery, int32_t* position, uint32_t* velocity, int32_t radius)
		{
			Vector3I16 normal;
			Vector3I16 tangent;
			int32_t* segment;
			int32_t hit;
			int32_t count;
			int32_t startX;
			int32_t startZ;
			int32_t planeX;
			int32_t planeZ;
			int32_t distance;
			int32_t endDistance;
			int32_t hitX;
			int32_t hitZ;
			int32_t bestDist;
			int32_t projection;
			int32_t diffX;
			int32_t diffZ;
			int32_t sideX;
			int32_t sideZ;
			int32_t reach;
			int32_t length;
			int32_t* vel = (int32_t*)velocity;

			normal.y = 0;

			if (!g_unk72D2AC)
				return 0;

			hit = 0;
			count = 0;

			if (g_unk72D2AC > 0)
			{
				segment = g_unk728DA0;

				do
				{
					// the normal of the wall, the segment turned by a quarter
					normal.x = (int16_t)(segment[2] - segment[5]);
					normal.z = (int16_t)(segment[3] - segment[0]);

					Nu3D::Math::NormalizeToFixedPoint16(&normal, &normal);

					startZ = position[2];
					planeZ = 32 * segment[2];
					startX = position[0];
					planeX = 32 * segment[0];
					diffX = position[0] - planeX;
					distance = ((normal.x * diffX + (startZ - planeZ) * normal.z) >> 12) - radius;

					if (distance >= 0)
					{
						sideZ = vel[2];
						endDistance = ((normal.x * (vel[0] + diffX) + normal.z * (sideZ + startZ - planeZ)) >> 12) - radius;

						tangent.x = (int16_t)(segment[3] - segment[0]);
						tangent.z = (int16_t)(segment[5] - segment[2]);

						if (endDistance >= 0)
							goto LABEL_14;

						hitX = startX + distance * vel[0] / (distance - endDistance) - ((radius * normal.x) >> 12);
						hitZ = startZ + distance * sideZ / (distance - endDistance) - ((radius * normal.z) >> 12);

						if (tangent.x * (hitX - planeX) + tangent.z * (hitZ - planeZ) < -8192)
							goto LABEL_14;

						if (-(tangent.x * (hitX - 32 * segment[3]) + tangent.z * (hitZ - 32 * segment[5])) < -8192)
							goto LABEL_14;

						bestDist = (distance << 14) / (distance - endDistance);

						if (bestDist < rayQuery->bestDist)
						{
							normal.x *= 4;
							normal.z *= 4;
							rayQuery->hitT0 = distance;
							rayQuery->hitT1 = endDistance;
							rayQuery->hitNormal.x = normal.x;
							rayQuery->hitNormal.y = normal.y;
							hit = 1;
							rayQuery->hitNormal.z = normal.z;
							rayQuery->bestDist = bestDist;
						}
					}

				LABEL_14:
					diffZ = segment[2] - position[2] / 32;

					if ((segment[0] - position[0] / 32) * (segment[0] - position[0] / 32) + diffZ * diffZ
						< vel[0] / 32 * (vel[0] / 32) + (radius + 0x2000) / 32 * ((radius + 0x2000) / 32) + vel[2] / 32 * (vel[2] / 32))
					{
						// the end of the wall
						tangent.y = 0;
						tangent.x = (int16_t)vel[0];
						tangent.z = (int16_t)vel[2];

						Nu3D::Math::NormalizeToFixedPoint16(&tangent, &tangent);

						startZ = position[2];
						startX = position[0];
						planeX = 32 * segment[0];
						projection = (tangent.x * (planeX - position[0]) + tangent.z * (32 * segment[2] - startZ)) >> 12;

						if (projection >= 0)
						{
							sideZ = (32 * segment[2] - (startZ + ((projection * tangent.z) >> 12))) * (32 * segment[2] - (startZ + ((projection * tangent.z) >> 12)));
							sideX = (planeX - (startX + ((projection * tangent.x) >> 12))) * (planeX - (startX + ((projection * tangent.x) >> 12)));

							if ((radius + 32) * (radius + 32) >= sideX + sideZ)
							{
								reach = projection - (int32_t)sqrt((double)((radius + 64) * (radius + 64) - sideZ - sideX));

								if (reach >= -32 && reach * reach <= vel[0] * vel[0] + vel[2] * vel[2])
								{
									if (reach < 0)
										reach = 0;

									length = (int32_t)sqrt((double)(vel[0] * vel[0] + vel[2] * vel[2]));
									bestDist = (reach << 14) / length;

									if (bestDist < rayQuery->bestDist)
									{
										normal.x = (int16_t)(position[0] + ((reach * tangent.x) >> 12) - 32 * segment[0]);
										normal.z = (int16_t)(position[2] + ((reach * tangent.z) >> 12) - (int16_t)(32 * segment[2]));

										Nu3D::Math::NormalizeToFixedPoint16(&normal, &normal);

										rayQuery->hitT0 = reach;
										normal.x = normal.x * 4;
										normal.z = normal.z * 4;
										rayQuery->hitNormal.x = normal.x;
										rayQuery->hitNormal.y = normal.y;
										hit = 1;
										rayQuery->hitT1 = reach - length;
										rayQuery->hitNormal.z = normal.z;
										rayQuery->bestDist = bestDist;
									}
								}
							}
						}
					}

					segment += 6;
					count += 2;
				} while (count < g_unk72D2AC);
			}

			return hit;
		}

		// FUNCTION: TOY2 0x004813B0
		int32_t SweepEdge(
			int32_t* segStart,
			int32_t* segEnd,
			int32_t edgeDirX,
			int32_t edgeDirY,
			int32_t edgeDirZ,
			Toy2::Collision::RaycastQuery* rayQuery,
			int32_t radius,
			int16_t* faceAdjacency)
		{
			// these live in g_mathScratch in the original
			Vector3I* direction = (Vector3I*)&g_mathScratch[0x10];
			int32_t* directionDot = (int32_t*)&g_mathScratch[0x1C];
			Vector3I* edgeDir = (Vector3I*)&g_mathScratch[0x20];
			Vector3I* tangent = (Vector3I*)&g_mathScratch[0x30];
			Vector3I* sideDir = (Vector3I*)&g_mathScratch[0x40];
			Vector3I* cross = (Vector3I*)&g_mathScratch[0x50];
			Vector3I* plane = (Vector3I*)&g_mathScratch[0x60];
			Vector3I* point = (Vector3I*)&g_mathScratch[0x70];
			Vector3I* normal = (Vector3I*)&g_mathScratch[0x80];
			int32_t a;
			int32_t b;
			int32_t c;
			int32_t d;
			int32_t e;
			int32_t f;
			int32_t distance;
			int32_t scale;
			int32_t length;
			int32_t time;
			int32_t bestDist;
			int32_t dot;
			int32_t endDistance;
			int32_t span;
			int32_t facing0;
			int32_t facing1;
			int64_t squared;

			*direction = *(Vector3I*)((uint8_t*)rayQuery + 0x3C);
			*directionDot = *(int32_t*)((uint8_t*)rayQuery + 0x48);
			edgeDir->x = edgeDirX;
			edgeDir->y = edgeDirY;
			edgeDir->z = edgeDirZ;

			Nu3D::Math::NormalizeToFixedPoint(edgeDir, edgeDir);
			Nu3D::Math::CrossProduct3D(direction, edgeDir, cross);

			if (abs(cross->x) >= 0x80000 || abs(cross->y) >= 0x80000 || abs(cross->z) >= 0x80000)
			{
				// the sweep is not parallel to the edge, the closest approach of the swept sphere to the edge
				a = cross->x >> 10;
				b = cross->y >> 10;
				c = cross->z >> 10;
				cross->x = a;
				cross->y >>= 10;
				cross->z >>= 10;

				if (a || b || c)
				{
					plane->x = a;
					plane->y = b;
					plane->z = c;
					*(int32_t*)&g_mathScratch[0x6C] = *(int32_t*)&g_mathScratch[0x5C];

					while (abs(a) > 0x4000 || abs(b) > 0x4000 || abs(c) > 0x4000)
					{
						a >>= 1;
						b >>= 1;
						c >>= 1;
					}

					plane->z = c;
					plane->y = b;
					plane->x = a;

					Nu3D::Math::NormalizeToFixedPoint(plane, plane);

					distance = (plane->x * segStart[0] + segStart[1] * plane->y + plane->z * segStart[2]) >> 10;

					if (abs(distance) <= 4 * radius + 192)
					{
						scale = (cross->x * plane->x + cross->y * plane->y + cross->z * plane->z) >> 12;

						if (scale)
						{
							point->x = segStart[0] >> 2;
							point->y = segStart[1] >> 2;
							point->z = segStart[2] >> 2;

							Nu3D::Math::CrossProduct3D(point, edgeDir, point);

							point->y >>= 10;
							point->z >>= 10;
							point->x >>= 10;

							time = -4 * ((plane->x * point->x + plane->y * point->y + plane->z * point->z) / scale);

							Nu3D::Math::CrossProduct3D(plane, edgeDir, sideDir);

							d = sideDir->y;
							e = sideDir->x;

							while (abs(e) > 0x10000 || abs(d) > 0x10000 || abs(sideDir->z) > 0x10000)
							{
								d >>= 3;
								sideDir->z >>= 3;
								e >>= 3;
							}

							sideDir->y = d;
							sideDir->x = e;

							while (abs(e) > 0x4000 || abs(d) > 0x4000 || abs(sideDir->z) > 0x4000)
							{
								e >>= 1;
								d >>= 1;
								sideDir->z >>= 1;
							}

							sideDir->y = d;
							sideDir->x = e;

							Nu3D::Math::NormalizeToFixedPoint(sideDir, sideDir);

							span = (sideDir->x * direction->x + sideDir->y * direction->y + sideDir->z * direction->z) >> 8;

							if (span)
							{
								squared = (int64_t)(16 * radius * radius - distance * distance);
								length = *directionDot;
								time = time - abs(((int32_t)sqrt((double)abs((int32_t)squared)) << 14) / span);

								if (time >= 0 && time < length)
								{
									bestDist = (time << 14) / length;

									if (bestDist < rayQuery->bestDist)
									{
										point->x = segStart[0] + time * (segEnd[0] - segStart[0]) / length;
										point->y = segStart[1] + time * (segEnd[1] - segStart[1]) / length;
										c = segStart[2] + time * (segEnd[2] - segStart[2]) / length;
										a = edgeDirY * point->y + edgeDirX * point->x;
										b = edgeDirZ * c + a;
										point->z = c;

										if (b >= 0)
										{
											d = edgeDirZ * edgeDirZ + edgeDirY * edgeDirY + edgeDirX * edgeDirX;

											if ((b >> 5) <= d)
											{
												if (b > 0x40000)
												{
													do
													{
														b >>= 1;
														d >>= 1;
													} while (b > 0x40000);
												}

												normal->x = point->x - edgeDirX * b / d;
												normal->y = point->y - edgeDirY * b / d;
												normal->z = c - edgeDirZ * b / d;

												Nu3D::Math::NormalizeToFixedPoint(normal, normal);

												facing0 = 0;
												e = 4 * normal->x;
												f = 4 * normal->y;
												endDistance = 4 * normal->z;
												normal->x *= 4;
												normal->y *= 4;
												normal->z *= 4;

												if (faceAdjacency[1] != 0x7FFF)
													facing0 = f * faceAdjacency[1] + e * faceAdjacency[0] + endDistance * faceAdjacency[2] > -524288;

												if ((faceAdjacency[4] != 0x7FFF && f * faceAdjacency[4] + normal->z * faceAdjacency[5] + e * faceAdjacency[3] > -524288)
													|| facing0)
												{
													rayQuery->hitT0 = time;
													rayQuery->hitT1 = time - length;
													rayQuery->bestDist = bestDist;
													rayQuery->hitNormal.x = (int16_t)normal->x;
													rayQuery->hitNormal.y = (int16_t)normal->y;
													rayQuery->hitNormal.z = (int16_t)normal->z;

													return 1;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			else if (cross->x || cross->y || cross->z)
			{
				// the sweep is parallel to the edge
				a = (segStart[0] + segEnd[0]) >> 1;
				plane->x = a;
				b = (segStart[1] + segEnd[1]) >> 1;
				plane->y = b;
				c = (segStart[2] + segEnd[2]) >> 1;
				d = edgeDirZ * c + edgeDirY * b + edgeDirX * a;
				plane->z = c;

				for (e = edgeDirY * edgeDirY + edgeDirX * edgeDirX + edgeDirZ * edgeDirZ; d > 0x40000; e >>= 1)
					d >>= 1;

				if (e)
				{
					f = a - edgeDirX * d / e;
					plane->x = f;
					a = b - edgeDirY * d / e;
					plane->y = a;
					b = c - edgeDirZ * d / e;
					plane->z = b;

					while (abs(f) > 0x4000 || abs(a) > 0x4000 || abs(b) > 0x4000)
					{
						f >>= 2;
						a >>= 2;
						b >>= 2;
					}

					plane->z = b;
					plane->y = a;
					plane->x = f;

					Nu3D::Math::NormalizeToFixedPoint(plane, plane);

					distance = ((plane->x * segStart[0] + segStart[1] * plane->y + plane->z * segStart[2]) >> 12) - radius;
					endDistance = ((plane->x * segEnd[0] + plane->y * segEnd[1] + plane->z * segEnd[2]) >> 12) - radius;

					if (distance >= 0 && endDistance < 0)
					{
						span = distance - endDistance;
						bestDist = (distance << 14) / (distance - endDistance);

						if (bestDist < rayQuery->bestDist)
						{
							tangent->x = segStart[0] + distance * (segEnd[0] - segStart[0]) / (distance - endDistance);
							tangent->y = segStart[1] + distance * (segEnd[1] - segStart[1]) / span;
							tangent->z = segStart[2] + distance * (segEnd[2] - segStart[2]) / span;
							dot = edgeDirZ * tangent->z + edgeDirY * tangent->y + edgeDirX * tangent->x;

							if (dot >= 0)
							{
								length = edgeDirY * edgeDirY + edgeDirX * edgeDirX + edgeDirZ * edgeDirZ;

								if ((dot >> 5) <= length)
								{
									for (; dot > 0x40000; length >>= 1)
										dot >>= 1;

									tangent->x = tangent->x - edgeDirX * dot / length;
									tangent->y = tangent->y - edgeDirY * dot / length;
									tangent->z -= edgeDirZ * dot / length;

									Nu3D::Math::NormalizeToFixedPoint(tangent, tangent);

									tangent->x *= 4;
									tangent->y *= 4;
									facing1 = 0;
									tangent->z *= 4;

									// NOTE: the original tests the normal of the previous edge sweep here (0x555020), not the one it just built
									if (faceAdjacency[1] != 0x7FFF)
										facing1 = normal->y * faceAdjacency[1] + normal->x * faceAdjacency[0] + normal->z * faceAdjacency[2] > -524288;

									if ((faceAdjacency[4] != 0x7FFF && normal->y * faceAdjacency[4] + normal->z * faceAdjacency[5] + normal->x * faceAdjacency[3] > -524288) || facing1)
									{
										rayQuery->hitT0 = distance;
										rayQuery->hitT1 = endDistance;
										rayQuery->bestDist = bestDist;
										rayQuery->hitNormal.x = (int16_t)normal->x;
										rayQuery->hitNormal.y = (int16_t)normal->y;
										rayQuery->hitNormal.z = (int16_t)normal->z;

										return 1;
									}
								}
							}
						}
					}
				}
			}

			return 0;
		}

		// FUNCTION: TOY2 0x00481D00
		int32_t SweepVertex(int32_t* segStart, int32_t* segEnd, Toy2::Collision::RaycastQuery* rayQuery, int32_t radius, int16_t* faceAdjacency)
		{
			Vector3I direction;
			Vector3I point;
			int32_t startX;
			int32_t startY;
			int32_t startZ;
			int32_t quarterRadius;
			int32_t distance;
			int32_t length;
			int32_t squaredRadius;
			int32_t squaredDist;
			int32_t time;
			int32_t bestDist;
			int32_t normalX;
			int32_t normalY;
			int32_t facing = 0;

			// the segment is scaled down, in the caller's arrays
			startX = segStart[0];
			startY = segStart[1];
			segStart[2] >>= 2;
			segStart[0] = startX >> 2;
			segStart[1] = startY >> 2;
			segEnd[0] = segEnd[0] >> 2;
			segEnd[1] = segEnd[1] >> 2;
			segEnd[2] = segEnd[2] >> 2;

			direction.x = segEnd[0] - segStart[0];
			direction.y = segEnd[1] - segStart[1];
			direction.z = segEnd[2] - segStart[2];

			Nu3D::Math::NormalizeToFixedPoint(&direction, &direction);

			startZ = segStart[2];
			startY = segStart[1];
			startX = segStart[0];
			quarterRadius = radius >> 2;
			distance = -(direction.x * segStart[0] + direction.y * startY + direction.z * segStart[2]) >> 12;

			if (distance < -(radius >> 2))
				return 0;

			length = ((direction.x * (segEnd[0] - startX) + direction.y * (segEnd[1] - startY) + direction.z * (segEnd[2] - startZ)) >> 8) + 128;

			if (distance > quarterRadius + (length >> 4))
				return 0;

			squaredRadius = quarterRadius * quarterRadius;
			squaredDist = (startZ + ((direction.z * distance) >> 12)) * (startZ + ((direction.z * distance) >> 12))
						  + (((direction.y * distance) >> 12) + startY) * (((direction.y * distance) >> 12) + startY)
						  + (startX + ((direction.x * distance) >> 12)) * (startX + ((direction.x * distance) >> 12));

			if (squaredDist > squaredRadius)
				return 0;

			time = distance - (int32_t)sqrt((double)(squaredRadius - squaredDist));

			if (time < 0 || time >= length)
				return 0;

			bestDist = (time << 14) / length;

			// NOTE: the original hangs here
			if (bestDist < 0)
			{
				while (1)
					;
			}

			if (bestDist >= rayQuery->bestDist)
				return 0;

			point.x = startX + ((direction.x * time) >> 12);
			point.y = startY + ((direction.y * time) >> 12);
			point.z = startZ + ((direction.z * time) >> 12);

			Nu3D::Math::NormalizeToFixedPoint(&point, &point);

			normalY = 4 * point.y;
			point.z *= 4;
			normalX = 4 * point.x;
			point.y *= 4;

			if (faceAdjacency[1] != 0x7FFF)
				facing = normalX * faceAdjacency[0] + normalY * faceAdjacency[1] + point.z * faceAdjacency[2] > -524288;

			if (faceAdjacency[4] == 0x7FFF || point.y * faceAdjacency[4] + normalX * faceAdjacency[3] + point.z * faceAdjacency[5] <= -524288)
			{
				if (!facing)
					return 0;
			}

			rayQuery->hitT0 = time;
			rayQuery->hitNormal.x = (int16_t)normalX;
			rayQuery->bestDist = (time << 14) / length;
			rayQuery->hitT1 = time - length;
			rayQuery->hitNormal.y = (int16_t)normalY;
			rayQuery->hitNormal.z = (int16_t)point.z;

			return 1;
		}

		// FUNCTION: TOY2 0x00481FB0
		int32_t SweepTriangle(int32_t a1, Toy2::Collision::RaycastQuery* rayQuery, int32_t a3, int32_t a4)
		{
			CollisionTriangle* triangle = (CollisionTriangle*)a1;
			// the normals of the two triangles of the face, the y of a normal is 0x7FFF when there is none
			int16_t faceAdjacency[8];
			int32_t segStart[3];
			int32_t segEnd[3];
			int32_t hit = 0;
			int32_t useFirst;
			int32_t useSecond;
			int32_t sweepEdges;
			int32_t distance;
			int32_t endDistance;
			int32_t startDistance2;
			int32_t endDistance2;
			int32_t bestDist;
			int32_t hitX;
			int32_t hitY;
			int32_t hitZ;
			int32_t planeY;
			int32_t planeZ;
			int32_t planeX;
			int32_t margin;

			planeX = triangle->unk22;
			planeY = 32 * triangle->v0z;
			planeZ = triangle->unk20;

			distance = ((triangle->unk24 * (rayQuery->origin.z - planeY) + planeX * (rayQuery->origin.y - 32 * triangle->v0y)
						 + planeZ * (rayQuery->origin.x - 32 * triangle->v0x))
						>> 14)
					   - a3;
			endDistance = ((triangle->unk24 * (rayQuery->endpoint.z - planeY) + planeX * (rayQuery->endpoint.y - 32 * triangle->v0y)
							+ planeZ * (rayQuery->endpoint.x - 32 * triangle->v0x))
						   >> 14)
						  - a3;

			if (endDistance < 0 && distance >= 0)
			{
				hitX = rayQuery->origin.x + distance * (rayQuery->endpoint.x - rayQuery->origin.x) / (distance - endDistance) - ((a3 * planeZ) >> 14);
				hitY = rayQuery->origin.y + distance * (rayQuery->endpoint.y - rayQuery->origin.y) / (distance - endDistance) - ((a3 * planeX) >> 14);
				hitZ = rayQuery->origin.z + distance * (rayQuery->endpoint.z - rayQuery->origin.z) / (distance - endDistance) - ((a3 * triangle->unk24) >> 14);

				if (Nu3D::Collision::IsPointInTriangle(
						(hitX >> 5) - triangle->v0x,
						(hitY >> 5) - triangle->v0y,
						(hitZ >> 5) - triangle->v0z,
						triangle->e1x,
						triangle->e1y,
						triangle->e1z,
						triangle->e2x,
						triangle->e2y,
						triangle->e2z,
						(Vector3I16*)&triangle->unk20))
				{
					bestDist = (distance << 14) / (distance - endDistance);

					if (bestDist < rayQuery->bestDist)
					{
						rayQuery->hitT0 = distance;
						rayQuery->hitT1 = endDistance;
						rayQuery->bestDist = bestDist;
						hit = 1;
						memcpy(&rayQuery->hitNormal.x, &triangle->unk20, 4);
						memcpy(&rayQuery->hitNormal.z, &triangle->unk24, 4);
						rayQuery->hitFace = triangle;
						rayQuery->hitFlags = a4;
					}
				}
			}

			if (triangle->unk28 == 0x7FFF)
				goto LABEL_20;

			planeY = 32 * triangle->e3z;
			planeX = 32 * triangle->v0x;
			planeZ = 32 * triangle->v0z;
			margin = 32 * triangle->e3x;
			hitY = 32 * triangle->e3y;

			startDistance2 = ((triangle->unk2A * (rayQuery->origin.z - planeY - planeZ) + triangle->unk28 * (rayQuery->origin.y - hitY - 32 * triangle->v0y)
							   + triangle->unk26 * (rayQuery->origin.x - margin - planeX))
							  >> 14)
							 - a3;
			endDistance2 = ((triangle->unk2A * (rayQuery->endpoint.z - planeY - planeZ) + triangle->unk28 * (rayQuery->endpoint.y - hitY - 32 * triangle->v0y)
							 + triangle->unk26 * (rayQuery->endpoint.x - margin - planeX))
							>> 14)
						   - a3;

			if (endDistance2 < 0 && startDistance2 >= 0)
			{
				hitX = rayQuery->origin.x + startDistance2 * (rayQuery->endpoint.x - rayQuery->origin.x) / (startDistance2 - endDistance2) - ((a3 * triangle->unk26) >> 14);
				hitY = rayQuery->origin.y + startDistance2 * (rayQuery->endpoint.y - rayQuery->origin.y) / (startDistance2 - endDistance2) - ((a3 * triangle->unk28) >> 14);
				hitZ = rayQuery->origin.z + startDistance2 * (rayQuery->endpoint.z - rayQuery->origin.z) / (startDistance2 - endDistance2) - ((a3 * triangle->unk2A) >> 14);

				if (Nu3D::Collision::IsPointInTriangle(
						(hitX >> 5) - triangle->e3x - triangle->v0x,
						(hitY >> 5) - triangle->e3y - triangle->v0y,
						(hitZ >> 5) - triangle->e3z - triangle->v0z,
						triangle->e2x - triangle->e3x,
						triangle->e2y - triangle->e3y,
						triangle->e2z - triangle->e3z,
						triangle->e1x - triangle->e3x,
						triangle->e1y - triangle->e3y,
						triangle->e1z - triangle->e3z,
						(Vector3I16*)&triangle->unk26))
				{
					bestDist = (startDistance2 << 14) / (startDistance2 - endDistance2);

					if (bestDist < rayQuery->bestDist)
					{
						rayQuery->hitT0 = startDistance2;
						rayQuery->hitT1 = endDistance2;
						rayQuery->bestDist = bestDist;
						memcpy(&rayQuery->hitNormal.x, &triangle->unk26, 4);
						memcpy(&rayQuery->hitNormal.z, &triangle->unk2A, 4);
						rayQuery->hitFace = triangle;
						rayQuery->hitFlags = a4 | 0x4000;
						hit = 1;
					}
				}
			}

			if (startDistance2 > 256 && endDistance2 > 256)
				goto LABEL_20;

			planeY = -256 - (a3 >> 1);

			if (startDistance2 < planeY && endDistance2 < planeY)
			{
			LABEL_20:
				useSecond = 0;
			}
			else
			{
				useSecond = 1;
			}

			useFirst = 0;

			if (distance <= 256 || endDistance <= 256)
			{
				planeX = -256 - (a3 >> 1);

				if (distance >= planeX || endDistance >= planeX)
					useFirst = 1;
			}

			sweepEdges = useFirst;

			if (useSecond + useFirst)
			{
				if (useFirst)
				{
					memcpy(&faceAdjacency[0], &triangle->unk20, 4);
					memcpy(&faceAdjacency[2], &triangle->unk24, 4);
				}
				else
				{
					faceAdjacency[1] = 0x7FFF;
				}

				if (useSecond)
				{
					memcpy(&faceAdjacency[3], &triangle->unk26, 4);
					memcpy(&faceAdjacency[5], &triangle->unk2A, 4);
				}
				else
				{
					faceAdjacency[4] = 0x7FFF;
				}

				if (triangle->unk28 != 0x7FFF)
				{
					if (useSecond)
					{
						// the fourth vertex of a quad
						segStart[0] = rayQuery->origin.x - 32 * triangle->e3x - 32 * triangle->v0x;
						segStart[1] = rayQuery->origin.y - 32 * triangle->e3y - 32 * triangle->v0y;
						segStart[2] = rayQuery->origin.z - 32 * triangle->e3z - 32 * triangle->v0z;
						segEnd[0] = rayQuery->endpoint.x - 32 * triangle->e3x - 32 * triangle->v0x;
						segEnd[1] = rayQuery->endpoint.y - 32 * triangle->e3y - 32 * triangle->v0y;
						segEnd[2] = rayQuery->endpoint.z - 32 * triangle->e3z - 32 * triangle->v0z;

						if (SweepVertex(segStart, segEnd, rayQuery, a3 + 64, faceAdjacency))
						{
							rayQuery->hitFace = triangle;
							hit = 1;
							rayQuery->hitFlags = a4 | 0x2000;
						}
					}
				}

				segStart[0] = rayQuery->origin.x - 32 * triangle->e2x - 32 * triangle->v0x;
				segStart[1] = rayQuery->origin.y - 32 * triangle->e2y - 32 * triangle->v0y;
				segStart[2] = rayQuery->origin.z - 32 * triangle->e2z - 32 * triangle->v0z;
				segEnd[0] = rayQuery->endpoint.x - 32 * triangle->e2x - 32 * triangle->v0x;
				segEnd[1] = rayQuery->endpoint.y - 32 * triangle->e2y - 32 * triangle->v0y;
				segEnd[2] = rayQuery->endpoint.z - 32 * triangle->e2z - 32 * triangle->v0z;

				if (SweepVertex(segStart, segEnd, rayQuery, a3 + 64, faceAdjacency))
				{
					rayQuery->hitFace = triangle;
					rayQuery->hitFlags = a4 | 0x2000;
					hit = 1;
				}

				segStart[0] = rayQuery->origin.x - 32 * triangle->e1x - 32 * triangle->v0x;
				segStart[1] = rayQuery->origin.y - 32 * triangle->e1y - 32 * triangle->v0y;
				segStart[2] = rayQuery->origin.z - 32 * triangle->e1z - 32 * triangle->v0z;
				segEnd[0] = rayQuery->endpoint.x - 32 * triangle->e1x - 32 * triangle->v0x;
				segEnd[1] = rayQuery->endpoint.y - 32 * triangle->e1y - 32 * triangle->v0y;
				segEnd[2] = rayQuery->endpoint.z - 32 * triangle->e1z - 32 * triangle->v0z;

				if (SweepVertex(segStart, segEnd, rayQuery, a3 + 64, faceAdjacency))
				{
					rayQuery->hitFace = triangle;
					hit = 1;
					rayQuery->hitFlags = a4 | 0x2000;
				}

				if (sweepEdges)
				{
					segStart[0] = rayQuery->origin.x - 32 * triangle->v0x;
					segStart[1] = rayQuery->origin.y - 32 * triangle->v0y;
					segStart[2] = rayQuery->origin.z - 32 * triangle->v0z;
					segEnd[0] = rayQuery->endpoint.x - 32 * triangle->v0x;
					segEnd[1] = rayQuery->endpoint.y - 32 * triangle->v0y;
					segEnd[2] = rayQuery->endpoint.z - 32 * triangle->v0z;

					if (SweepVertex(segStart, segEnd, rayQuery, a3 + 64, faceAdjacency))
					{
						rayQuery->hitFace = triangle;
						rayQuery->hitFlags = a4 | 0x2000;
						hit = 1;
					}

					segStart[0] = rayQuery->origin.x - 32 * triangle->v0x;
					segStart[1] = rayQuery->origin.y - 32 * triangle->v0y;
					segStart[2] = rayQuery->origin.z - 32 * triangle->v0z;
					segEnd[0] = rayQuery->endpoint.x - 32 * triangle->v0x;
					segEnd[1] = rayQuery->endpoint.y - 32 * triangle->v0y;
					segEnd[2] = rayQuery->endpoint.z - 32 * triangle->v0z;

					if (SweepEdge(segStart, segEnd, triangle->e1x, triangle->e1y, triangle->e1z, rayQuery, a3 + 32, faceAdjacency))
					{
						rayQuery->hitFace = triangle;
						hit = 1;
						rayQuery->hitFlags = a4 | 0x2000;
					}

					if (SweepEdge(segStart, segEnd, triangle->e2x, triangle->e2y, triangle->e2z, rayQuery, a3 + 32, faceAdjacency))
					{
						rayQuery->hitFace = triangle;
						hit = 1;
						rayQuery->hitFlags = a4 | 0x2000;
					}
				}

				segStart[0] = rayQuery->origin.x - 32 * (triangle->e1x + triangle->v0x);
				segStart[1] = rayQuery->origin.y - 32 * (triangle->e1y + triangle->v0y);
				segStart[2] = rayQuery->origin.z - 32 * (triangle->e1z + triangle->v0z);
				segEnd[0] = rayQuery->endpoint.x - 32 * (triangle->e1x + triangle->v0x);
				segEnd[1] = rayQuery->endpoint.y - 32 * (triangle->e1y + triangle->v0y);
				segEnd[2] = rayQuery->endpoint.z - 32 * (triangle->e1z + triangle->v0z);

				if (SweepEdge(segStart, segEnd, triangle->e2x - triangle->e1x, triangle->e2y - triangle->e1y, triangle->e2z - triangle->e1z, rayQuery, a3 + 32, faceAdjacency))
				{
					rayQuery->hitFace = triangle;
					hit = 1;
					rayQuery->hitFlags = a4 | 0x2000;
				}

				if (triangle->unk28 != 0x7FFF && useSecond)
				{
					segStart[0] = rayQuery->origin.x - 32 * (triangle->e3x + triangle->v0x);
					segStart[1] = rayQuery->origin.y - 32 * (triangle->e3y + triangle->v0y);
					segStart[2] = rayQuery->origin.z - 32 * (triangle->e3z + triangle->v0z);
					segEnd[0] = rayQuery->endpoint.x - 32 * (triangle->e3x + triangle->v0x);
					segEnd[1] = rayQuery->endpoint.y - 32 * (triangle->e3y + triangle->v0y);
					segEnd[2] = rayQuery->endpoint.z - 32 * (triangle->e3z + triangle->v0z);

					if (SweepEdge(segStart, segEnd, triangle->e2x - triangle->e3x, triangle->e2y - triangle->e3y, triangle->e2z - triangle->e3z, rayQuery, a3 + 32, faceAdjacency))
					{
						rayQuery->hitFace = triangle;
						hit = 1;
						rayQuery->hitFlags = a4 | 0x2000;
					}

					if (SweepEdge(segStart, segEnd, triangle->e1x - triangle->e3x, triangle->e1y - triangle->e3y, triangle->e1z - triangle->e3z, rayQuery, a3 + 32, faceAdjacency))
					{
						rayQuery->hitFace = triangle;
						rayQuery->hitFlags = a4 | 0x2000;
						return 1;
					}
				}
			}

			return hit;
		}

		// FUNCTION: TOY2 0x00482A00
		int32_t ResolveSubstep(
			Toy2::Collision::ResolvedVelocity* a1,
			WorldPos* a2,
			Vector3I* a3,
			Toy2::Collision::BuzzCollisionState* a4,
			uint16_t* a5,
			int32_t* a6,
			int32_t a7,
			int32_t a8)
		{
			RaycastQuery rayQuery;
			Vector3I velocity;
			Vector3I direction;
			Vector3I16 angles;
			Matrix3x3I16* rotation = (Matrix3x3I16*)&g_mathScratch[948];
			Matrix3x3I16* nextRotation = (Matrix3x3I16*)&g_mathScratch[980];
			Platform::PlatformDescriptor* platform;
			CollisionMeshInstance* mesh;
			int16_t prevMesh = -1;
			int16_t hitAny = 0;
			int32_t meshIdx = 0;
			int32_t triangle = 0;
			int32_t count;
			int32_t platformIdx;
			int32_t x;
			int32_t y;
			int32_t z;
			int32_t posX;
			int32_t posY;
			int32_t posZ;
			int32_t localX;
			int32_t localY;
			int32_t localZ;
			int32_t hitFlags;
			int32_t hitT0;
			int32_t hitT1;
			int32_t scaledT1;
			int32_t denominator;
			int32_t normalX;
			int32_t normalY;
			int32_t normalZ;
			int32_t velX;
			int32_t velY;
			int32_t velZ;
			int32_t dot;
			int32_t rotatedZ;
			int32_t platformVelY;
			int32_t* triangles = a6;
			uint16_t* meshes = a5;

			velocity.x = a1->vel.x + a1->platformVel.x;
			rayQuery.hitFlags = -1;
			velocity.y = a1->vel.y + a1->platformVel.y;
			rayQuery.bestDist = 0x7FFFFFFF;
			velocity.z = a1->vel.z + a1->platformVel.z;

			// the triangles in the collision grid are swept in the space of their mesh, a platform moves and rotates
			for (count = a7; count > 0; count--, triangles++, meshes++)
			{
				triangle = *triangles;
				meshIdx = (int16_t)*meshes;

				if ((int16_t)*meshes != prevMesh)
				{
					x = a2->x;
					y = a2->y;
					z = a2->z;
					prevMesh = (int16_t)meshIdx;
					mesh = &g_collisionMeshInstances[meshIdx];

					if (mesh->typeFlags == 8)
					{
						platformIdx = mesh->platformIdx;
						platform = &Platform::g_platformDescriptors[platformIdx];

						if ((platform->flags & 8) != 0)
						{
							angles.x = platform->rotAngle.x >> 2;
							angles.y = platform->rotAngle.y >> 2;
							angles.z = platform->rotAngle.z >> 2;

							Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, rotation);

							localX = x - mesh->origin.x;
							localZ = z - mesh->origin.z;

							rayQuery.origin.x = (localX * rotation->m00 + (y - mesh->origin.y) * rotation->m10 + localZ * rotation->m20) / 4096;
							rayQuery.origin.y = (localX * rotation->m01 + (y - mesh->origin.y) * rotation->m11 + localZ * rotation->m21) / 4096;
							rayQuery.origin.z = (localX * rotation->m02 + (y - mesh->origin.y) * rotation->m12 + localZ * rotation->m22) / 4096;

							angles.x = (platform->rotAngle.x + platform->prevAngVel.x) >> 2;
							angles.y = (platform->rotAngle.y + platform->prevAngVel.y) >> 2;
							angles.z = (platform->rotAngle.z + platform->prevAngVel.z) >> 2;

							Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, rotation);

							if (a8)
							{
								localZ = z - platform->prevLinVel.z - mesh->origin.z + velocity.z;
								localY = velocity.y + y - platform->prevLinVel.y - mesh->origin.y;
								localX = velocity.x + x - platform->prevLinVel.x - mesh->origin.x;
							}
							else
							{
								localZ = velocity.z + z - mesh->origin.z;
								localY = velocity.y + y - mesh->origin.y;
								localX = velocity.x + x - mesh->origin.x;
							}

							rayQuery.endpoint.x = (localZ * rotation->m20 + localY * rotation->m10 + localX * rotation->m00) / 4096;
							rayQuery.endpoint.y = (localZ * rotation->m21 + localY * rotation->m11 + localX * rotation->m01) / 4096;
							rayQuery.endpoint.z = (localZ * rotation->m22 + localY * rotation->m12 + localX * rotation->m02) / 4096;
						}
						else
						{
							rayQuery.origin.x = x - mesh->origin.x;
							rayQuery.origin.y = y - mesh->origin.y;
							rayQuery.origin.z = z - mesh->origin.z;

							if (a8)
							{
								rayQuery.endpoint.x = velocity.x + x - platform->prevLinVel.x - mesh->origin.x;
								rayQuery.endpoint.y = velocity.y + y - platform->prevLinVel.y - mesh->origin.y;
								rayQuery.endpoint.z = velocity.z + (z - platform->prevLinVel.z) - mesh->origin.z;
							}
							else
							{
								rayQuery.endpoint.x = rayQuery.origin.x + velocity.x;
								rayQuery.endpoint.y = rayQuery.origin.y + velocity.y;
								rayQuery.endpoint.z = z - mesh->origin.z + velocity.z;
							}
						}
					}
					else
					{
						rayQuery.origin.x = x - mesh->origin.x;
						rayQuery.origin.y = y - mesh->origin.y;
						rayQuery.origin.z = z - mesh->origin.z;

						rayQuery.endpoint.x = velocity.x + rayQuery.origin.x;
						rayQuery.endpoint.y = velocity.y + rayQuery.origin.y;
						rayQuery.endpoint.z = velocity.z + rayQuery.origin.z;
					}

					direction.x = rayQuery.endpoint.x - rayQuery.origin.x;
					direction.y = rayQuery.endpoint.y - rayQuery.origin.y;
					direction.z = rayQuery.endpoint.z - rayQuery.origin.z;

					while (abs(direction.x) > 0x4000 || abs(direction.y) > 0x4000 || abs(direction.z) > 0x4000)
					{
						direction.x >>= 1;
						direction.y >>= 1;
						direction.z >>= 1;
					}

					Nu3D::Math::NormalizeToFixedPoint(&direction, &direction);

					rayQuery.direction = direction;
					rayQuery.directionDot = ((direction.x * (rayQuery.endpoint.x - rayQuery.origin.x) + direction.y * (rayQuery.endpoint.y - rayQuery.origin.y)
											  + direction.z * (rayQuery.endpoint.z - rayQuery.origin.z))
											 >> 12)
											+ 256;
				}

				if (SweepTriangle(triangle, &rayQuery, a4->floorOffset, meshIdx))
					hitAny = 1;
			}

			if (SweepWallSegments(&rayQuery, &a2->x, (uint32_t*)&velocity, a4->floorOffset) == 1)
			{
				hitFlags = -1;
				rayQuery.hitFlags = -1;
				goto LABEL_27;
			}

			if (hitAny > 0)
			{
				hitFlags = rayQuery.hitFlags;

			LABEL_27:
				if (hitFlags >= 0 && g_collisionMeshInstances[hitFlags & 0x7FF].typeFlags == 8
					&& (Platform::g_platformDescriptors[g_collisionMeshInstances[hitFlags & 0x7FF].platformIdx].flags & 8) != 0)
				{
					hitT1 = rayQuery.hitT1 - 128;
				}
				else
				{
					rayQuery.hitT1 -= 96;
					hitT1 = rayQuery.hitT1;
				}

				scaledT1 = hitT1 >> 3;
				g_unk72959C = (rayQuery.hitT0 - 32) >> 3;
				g_unk729170 = scaledT1;
				posX = a2->x;
				rayQuery.hitT0 = (rayQuery.hitT0 - 32) >> 3;
				hitT0 = rayQuery.hitT0;
				rayQuery.hitT1 = scaledT1;
				posY = velocity.y * hitT0;
				a2->x = velocity.x * rayQuery.hitT0 / (rayQuery.hitT0 - scaledT1) + posX;
				posZ = posY / (hitT0 - scaledT1) + a2->y;
				a2->y = posZ;
				a2->z += velocity.z * hitT0 / (hitT0 - scaledT1);

				if (a8)
				{
					Platform::AdvancePartialMotion(hitT0, scaledT1, 0);
					hitT0 = rayQuery.hitT0;
				}

				if (rayQuery.hitFlags != -1 && g_collisionMeshInstances[rayQuery.hitFlags & 0x7FF].typeFlags == 8
					&& (platformIdx = g_collisionMeshInstances[rayQuery.hitFlags & 0x7FF].platformIdx, a4->platformIdx = platformIdx,
						(Platform::g_platformDescriptors[platformIdx].flags & 8) != 0))
				{
					// the normal of a rotated platform
					angles.x = Platform::g_platformDescriptors[platformIdx].rotAngle.x >> 2;
					angles.y = Platform::g_platformDescriptors[platformIdx].rotAngle.y >> 2;
					angles.z = Platform::g_platformDescriptors[platformIdx].rotAngle.z >> 2;

					Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, rotation);

					rayQuery.endpoint.x = (rayQuery.hitNormal.z * rotation->m02 + rayQuery.hitNormal.y * rotation->m01 + rayQuery.hitNormal.x * rotation->m00) / 4096;
					rayQuery.endpoint.y = (rayQuery.hitNormal.z * rotation->m12 + rayQuery.hitNormal.y * rotation->m11 + rayQuery.hitNormal.x * rotation->m10) / 4096;
					normalY = (int16_t)rayQuery.endpoint.y;
					hitT0 = rayQuery.hitT0;
					rotatedZ = rayQuery.hitNormal.z * rotation->m22 + rayQuery.hitNormal.y * rotation->m21 + rayQuery.hitNormal.x * rotation->m20;
					rayQuery.hitNormal.y = (int16_t)rayQuery.endpoint.y;
					rayQuery.endpoint.z = rotatedZ / 4096;
					normalZ = (int16_t)(rotatedZ / 4096);
					rayQuery.hitNormal.x = (int16_t)rayQuery.endpoint.x;
					rayQuery.hitNormal.z = (int16_t)normalZ;
				}
				else
				{
					normalY = rayQuery.hitNormal.y;
					normalZ = rayQuery.hitNormal.z;
				}

				denominator = hitT0 - rayQuery.hitT1;
				velX = a1->vel.x - hitT0 * a1->vel.x / denominator;
				a1->vel.x = velX;
				velY = a1->vel.y - hitT0 * a1->vel.y / denominator;
				a1->vel.y = velY;
				velZ = a1->vel.z - hitT0 * a1->vel.z / denominator;
				a1->vel.z = velZ;

				if (rayQuery.hitFlags >= 0)
				{
					if (g_collisionMeshInstances[(int16_t)rayQuery.hitFlags & 0x7FF].typeFlags != 8)
					{
						a1->platformVel.x -= hitT0 * a1->platformVel.x / denominator;
						a1->platformVel.y -= hitT0 * a1->platformVel.y / denominator;
						a1->platformVel.z -= hitT0 * a1->platformVel.z / denominator;
					}

					normalZ = rayQuery.hitNormal.z;
				}

				// the 16 bit value of the hit flags is what was tested above, the full value is used below
				if (normalY < -8192)
				{
					// a floor
					dot = (normalZ * velZ + normalY * velY + rayQuery.hitNormal.x * velX) / -2048;

					a1->vel.x = velX + dot * rayQuery.hitNormal.x / 0x20000;
					a1->vel.y = velY + dot * normalY / 0x20000;
					a1->vel.z = velZ + dot * normalZ / 0x20000;

					if (rayQuery.hitFlags >= 0 && g_collisionMeshInstances[rayQuery.hitFlags & 0x7FF].typeFlags == 8)
					{
						platformIdx = g_collisionMeshInstances[rayQuery.hitFlags & 0x7FF].platformIdx;
						platform = &Platform::g_platformDescriptors[platformIdx];

						a1->platformVel.x = (rayQuery.hitNormal.x >> 8) + platform->prevLinVel.x;
						a1->platformVel.y = (normalY >> 8) + platform->prevLinVel.y;
						a1->platformVel.z = (rayQuery.hitNormal.z >> 8) + platform->prevLinVel.z;

						if ((platform->flags & 8) != 0)
						{
							angles.x = platform->rotAngle.x >> 2;
							angles.y = platform->rotAngle.y >> 2;
							angles.z = platform->rotAngle.z >> 2;

							Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, rotation);

							angles.x = (platform->rotAngle.x + platform->prevAngVel.x) >> 2;
							angles.y = (platform->rotAngle.y + platform->prevAngVel.y) >> 2;
							angles.z = (platform->rotAngle.z + platform->prevAngVel.z) >> 2;

							Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, nextRotation);

							mesh = &g_collisionMeshInstances[platform->meshInstanceId];

							localZ = a2->z - mesh->origin.z;
							localY = a2->y - mesh->origin.y;
							localX = a2->x - mesh->origin.x;

							posX = localZ * rotation->m20 + localY * rotation->m10 + localX * rotation->m00;
							velocity.x = posX / 4096;
							velocity.y = (localZ * rotation->m21 + localY * rotation->m11 + localX * rotation->m01) / 4096;
							posZ = localZ * rotation->m22 + localY * rotation->m12 + localX * rotation->m02;

							platformVelY = a1->platformVel.y;

							a1->platformVel.x += (uint16_t)mesh->origin.x
												 + (velocity.x * nextRotation->m00 + velocity.y * nextRotation->m01 + posZ / 4096 * nextRotation->m02) / 4096
												 - (uint16_t)a2->x;
							a1->platformVel.y += (uint16_t)g_collisionMeshInstances[platform->meshInstanceId].origin.y
												 + (velocity.x * nextRotation->m10 + velocity.y * nextRotation->m11 + posZ / 4096 * nextRotation->m12) / 4096
												 - (uint16_t)a2->y;
							a1->platformVel.z += (uint16_t)g_collisionMeshInstances[platform->meshInstanceId].origin.z
												 + (velocity.x * nextRotation->m20 + velocity.y * nextRotation->m21 + posZ / 4096 * nextRotation->m22) / 4096
												 - (uint16_t)a2->z;

							if (platformVelY > a1->platformVel.y)
								a1->platformVel.y = (a1->platformVel.y - platformVelY) / 8 + a1->platformVel.y + 32;

							normalY = rayQuery.hitNormal.y;
						}

						platform->flags |= 2;
						platform->contactFace = rayQuery.hitFace;
					}
					else
					{
						dot = (normalY * a1->platformVel.y + rayQuery.hitNormal.x * a1->platformVel.x + normalZ * a1->platformVel.z) / -2048;

						a1->platformVel.x += dot * rayQuery.hitNormal.x / 0x20000;
						a1->platformVel.y += dot * normalY / 0x20000;
						a1->platformVel.z += dot * normalZ / 0x20000;
					}

					dot = (a3->y * normalY + a3->x * rayQuery.hitNormal.x + a3->z * rayQuery.hitNormal.z) / -2048;

					a3->x += dot * rayQuery.hitNormal.x / 0x20000;
					a3->y += dot * normalY / 0x20000;
					a3->z = a3->z + dot * rayQuery.hitNormal.z / 0x20000;
				}
				else
				{
					// a wall or a ceiling
					normalX = rayQuery.hitNormal.x;

					if (Renderer::g_frameDelta > 3)
					{
						posX = a3->x >> 1;
						a3->z >>= 1;
						a3->x = posX;
						posY = (normalX >> 6);
						a1->vel.x += posY;
						posZ = (normalZ >> 6);
						a1->vel.z += posZ;
						a3->x += posY;
						a3->z += posZ;
					}

					posX = a3->x;

					if (Renderer::g_frameDelta == 3)
					{
						a3->z >>= 1;
						a3->x = posX >> 1;
						a1->vel.x += normalX >> 7;
						a1->vel.z += normalZ >> 7;
						posY = a3->z;
						a3->x += normalX >> 7;
						a3->z = (normalZ >> 7) + posY;
					}
					else
					{
						a3->x = 15 * posX / 16;
						a3->z = 15 * a3->z / 16;
						posY = (3 * normalX) >> 9;
						a1->vel.x += posY;
						posZ = (3 * normalZ) >> 9;
						a1->vel.z += posZ;
						a3->x += posY;
						a3->z += posZ;
					}

					if (normalY <= 0 || a1->vel.y <= 0)
					{
						velZ = a1->vel.z;
						velY = a1->vel.y;
						velX = a1->vel.x;

						if (g_unk72D298)
							dot = (normalX * a1->vel.x + normalY * velY + normalZ * velZ) / -1024;
						else
							dot = (normalX * a1->vel.x + normalY * velY + normalZ * velZ) / -2048;

						a1->vel.x = velX + dot * normalX / 0x20000;
						a1->vel.y += dot * normalY / 0x20000;
						normalY = rayQuery.hitNormal.y;
						a1->vel.z += dot * normalZ / 0x20000;
					}

					if (rayQuery.hitFlags >= 0 && g_collisionMeshInstances[rayQuery.hitFlags & 0x7FF].typeFlags == 8)
					{
						a3->y += normalY >> 6;
						a1->vel.y += normalY >> 6;
						platform = &Platform::g_platformDescriptors[g_collisionMeshInstances[rayQuery.hitFlags & 0x7FF].platformIdx];
						a1->platformVel.x = (rayQuery.hitNormal.x >> 8) + platform->prevLinVel.x;
						a1->platformVel.z = (rayQuery.hitNormal.z >> 8) + platform->prevLinVel.z;

						if ((platform->flags & 8) != 0)
						{
							posX = rayQuery.hitNormal.x >> 7;
							a1->vel.x += rayQuery.hitNormal.x >> 7;
							a1->platformVel.x += posX;
							posZ = rayQuery.hitNormal.z >> 7;
							a1->vel.z += posZ;
							a1->platformVel.z += posZ;
						}

						platform->flags |= 1;
						platform->contactFace = rayQuery.hitFace;
					}
					else if (normalY <= 0 || a1->platformVel.y <= 0)
					{
						platformVelY = a1->platformVel.y;

						if (g_unk72D298)
							dot = (normalX * a1->platformVel.x + normalZ * a1->platformVel.z + normalY * platformVelY) / -1024;
						else
							dot = (normalX * a1->platformVel.x + normalZ * a1->platformVel.z + normalY * platformVelY) / -2048;

						a1->platformVel.x += dot * normalX / 0x20000;
						a1->platformVel.y += dot * normalY / 0x20000;
						a1->platformVel.z += dot * normalZ / 0x20000;
					}

					a4->surfaceNormal.x = rayQuery.hitNormal.x;
					a4->surfaceNormal.y = rayQuery.hitNormal.y;
					a4->surfaceNormal.z = rayQuery.hitNormal.z;

					if (rayQuery.hitFlags >= 0 && g_collisionMeshInstances[rayQuery.hitFlags & 0x7FF].typeFlags == 8)
						a4->wasGroundedLastFrame = 2;
					else
						a4->wasGroundedLastFrame = 1;

					if (normalY <= 0 || a3->y <= 0)
					{
						posZ = a3->z;
						posX = a3->x;

						if (g_unk72D298)
							dot = (a3->y * rayQuery.hitNormal.y + posX * normalX + posZ * normalZ) / -1024;
						else
							dot = (a3->y * rayQuery.hitNormal.y + (posX * normalX + posZ * normalZ)) / -2048;

						normalY = rayQuery.hitNormal.y;
						a3->x = a3->x + dot * normalX / 0x20000;
						a3->y += dot * normalY / 0x20000;
						a3->z += dot * normalZ / 0x20000;
					}

					posX = normalX / 96;
					a1->vel.x += posX;
					a1->vel.y += normalY / 96;
					a1->vel.z += normalZ / 96;
					posZ = a3->z;
					a3->x += posX;
					a3->z = normalZ / 96 + posZ;
					g_unk72D298 = 1;
				}

				hitFlags = rayQuery.hitFlags;

				if (rayQuery.hitFlags != -1 && (uint8_t)g_collisionMeshInstances[rayQuery.hitFlags & 0x7FF].unk != 0xFF)
					a4->collisionQuality = (a4->collisionQuality & 0xFF00) | (uint8_t)g_collisionMeshInstances[rayQuery.hitFlags & 0x7FF].unk;

				if (normalY >= -8192)
					return 2;

				a4->contactSurfaceData = (int32_t)rayQuery.hitFace;
				a4->contactSurfaceIndex = hitFlags;
				a4->unkVec = rayQuery.hitNormal;
				a4->unkWord12 = rayQuery.pad;

				return 1;
			}

			if (a8)
				Platform::AdvancePartialMotion(100, 0, 0);

			a2->x = velocity.x + a2->x;
			a2->y += velocity.y;
			a2->z = velocity.z + a2->z;
			a1->vel.x = 0;
			a1->vel.y = 0;
			a1->vel.z = 0;
			a1->platformVel.x = 0;
			a1->platformVel.y = 0;
			a1->platformVel.z = 0;

			return 0;
		}

		// FUNCTION: TOY2 0x00483EF0
		int32_t ResolvePlatformFooting(
			Toy2::Collision::ResolvedVelocity* resolvedVelocity,
			WorldPos* worldPos,
			Vector3I* velocity,
			Toy2::Collision::BuzzCollisionState* collisionState)
		{
			int16_t* surfaceData = (int16_t*)collisionState->contactSurfaceData;
			int32_t meshIdx = collisionState->contactSurfaceIndex & 0x7FF;
			int16_t unk = g_collisionMeshInstances[meshIdx].unk;
			Matrix3x3I16* rotation = (Matrix3x3I16*)&g_mathScratch[948];
			Vector3I16 angles;
			ResolvedVelocity substepVelocity;
			WorldPos substepPos;
			Vector3I substepMove;
			BuzzCollisionState substepState;
			int16_t substepMesh;
			int32_t substepTriangle;
			int16_t n1x;
			int16_t n1y;
			int16_t n1z;
			int16_t n2x;
			int16_t n2y;
			int16_t n2z;
			int32_t platformIdx;
			int32_t a;
			int32_t b;
			int32_t c;
			int32_t surfaceY;
			int32_t y;

			if ((uint8_t)unk != 0xFF)
				collisionState->collisionQuality = (collisionState->collisionQuality & 0xFF00) | (uint8_t)unk;

			if (g_collisionMeshInstances[meshIdx].typeFlags == 8 && (platformIdx = g_collisionMeshInstances[meshIdx].platformIdx, (Platform::g_platformDescriptors[platformIdx].flags & 8) != 0))
			{
				// the normals of a rotated platform
				angles.x = Platform::g_platformDescriptors[platformIdx].rotAngle.x >> 2;
				angles.y = Platform::g_platformDescriptors[platformIdx].rotAngle.y >> 2;
				angles.z = Platform::g_platformDescriptors[platformIdx].rotAngle.z >> 2;

				Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, rotation);

				a = surfaceData[16];
				b = surfaceData[17];
				c = surfaceData[18];

				n1x = (int16_t)((rotation->m02 * c + rotation->m01 * b + rotation->m00 * a) / 4096);
				n1y = (int16_t)((c * rotation->m12 + b * rotation->m11 + a * rotation->m10) / 4096);
				n1z = (int16_t)((c * rotation->m22 + b * rotation->m21 + a * rotation->m20) / 4096);

#ifdef APPLY_FIXES
				// the original tests n2y before it is written, that is a stack leftover
				if (surfaceData[20] == 0x7FFF)
#else
				if (n2y == 0x7FFF)
#endif
				{
					n2x = surfaceData[19];
					n2y = surfaceData[20];
					n2z = surfaceData[21];
				}
				else
				{
					c = surfaceData[21];
					b = surfaceData[20];
					a = surfaceData[19];

					n2x = (int16_t)((rotation->m02 * c + rotation->m01 * b + rotation->m00 * a) / 4096);
					n2y = (int16_t)((rotation->m12 * c + rotation->m11 * b + rotation->m10 * a) / 4096);
					n2z = (int16_t)((rotation->m22 * c + rotation->m21 * b + rotation->m20 * a) / 4096);
				}
			}
			else
			{
				n1x = surfaceData[16];
				n1y = surfaceData[17];
				n2x = surfaceData[19];
				n2y = surfaceData[20];
				n1z = surfaceData[18];
				n2z = surfaceData[21];
			}

			substepMove.x = resolvedVelocity->platformVel.x + resolvedVelocity->vel.x;
			substepMove.y = resolvedVelocity->vel.y + resolvedVelocity->platformVel.y;
			substepMove.z = resolvedVelocity->vel.z + resolvedVelocity->platformVel.z;
			substepPos.x = substepMove.x + worldPos->x;
			y = substepMove.y + worldPos->y;
			substepPos.z = substepMove.z + worldPos->z;

			surfaceY = n1y;

			if ((collisionState->contactSurfaceIndex & 0x4000) != 0)
				surfaceY = n2y;

			substepVelocity.vel.y = -surfaceY >> 6;
			substepMesh = collisionState->contactSurfaceIndex & 0x7FF;
			substepTriangle = collisionState->contactSurfaceData;
			substepPos.y = (surfaceY >> 8) + y;
			substepVelocity.vel.x = 0;
			substepVelocity.vel.z = 0;
			substepVelocity.platformVel.x = 0;
			substepVelocity.platformVel.y = 0;
			substepVelocity.platformVel.z = 0;

			memcpy(&substepState, collisionState, sizeof(substepState));

			if (ResolveSubstep(&substepVelocity, &substepPos, &substepMove, &substepState, (uint16_t*)&substepMesh, &substepTriangle, 1, 0) == 1)
			{
				if ((substepState.contactSurfaceIndex & 0x4000) != 0)
				{
					resolvedVelocity->vel.x = (int16_t)(substepPos.x + (n2x >> 9) - (int16_t)worldPos->x - resolvedVelocity->platformVel.x);
					resolvedVelocity->vel.y = (int16_t)(substepPos.y + (n2y >> 8) - (int16_t)worldPos->y - resolvedVelocity->platformVel.y);
					resolvedVelocity->vel.z = (int16_t)(substepPos.z + (n2z >> 9) - resolvedVelocity->platformVel.z - (int16_t)worldPos->z);
				}
				else
				{
					resolvedVelocity->vel.x = (int16_t)(substepPos.x + (n1x >> 9) - (int16_t)worldPos->x - resolvedVelocity->platformVel.x);
					resolvedVelocity->vel.y = (int16_t)(substepPos.y + (n1y >> 8) - (int16_t)worldPos->y - resolvedVelocity->platformVel.y);
					resolvedVelocity->vel.z = (int16_t)(substepPos.z + ((n1z >> 9) - resolvedVelocity->platformVel.z) - (int16_t)worldPos->z);
				}

				return 1;
			}

			if (n2y == 0x7FFF)
			{
				resolvedVelocity->vel.x += n1x >> 9;
				resolvedVelocity->vel.z += n1z >> 9;
				resolvedVelocity->vel.y += n1y >> 9;
			}
			else
			{
				resolvedVelocity->vel.x += (n1x + n2x) >> 9;
				resolvedVelocity->vel.y += (n1y + n2y) >> 9;
				resolvedVelocity->vel.z += (n1z + n2z) >> 9;
			}

			return 0;
		}

		// FUNCTION: TOY2 0x004878A0
		void MarkPlatformAsMoving(int32_t platformIdx)
		{
			int16_t meshInstanceId = Platform::g_platformDescriptors[platformIdx].meshInstanceId;

			if (meshInstanceId)
				g_collisionMeshInstances[meshInstanceId].typeFlags = 8;
		}

		// FUNCTION: TOY2 0x00487AD0
		BOOL IsSafeFooting(int32_t a1, uint8_t* a2)
		{
			if (*a2 != 1)
				return 0;

			if (g_buzzCollisionStates[a1].contactSurfaceIndex & 0x4000)
				return *(int16_t*)(g_buzzCollisionStates[a1].contactSurfaceData + 40) < (int16_t)0xF0C4;

			return *(int16_t*)(g_buzzCollisionStates[a1].contactSurfaceData + 34) < (int16_t)0xF0C4;
		}

		// FUNCTION: TOY2 0x0048E1B0
		BOOL IsMeshMoving() { return g_collisionMeshInstances[g_buzzCollisionStates[0].contactSurfaceIndex & 0x7FF].typeFlags == 8; }

		// FUNCTION: TOY2 0x0048E1D0
		int32_t GetGroundContactIdx()
		{
			if (g_groundContactMeshIdx == -1 || g_collisionMeshInstances[g_groundContactMeshIdx].typeFlags != 8)
				return -1;

			return g_collisionMeshInstances[g_groundContactMeshIdx].platformIdx;
		}
	}
}
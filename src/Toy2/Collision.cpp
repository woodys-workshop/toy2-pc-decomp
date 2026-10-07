#include "Toy2/Collision.h"
#include "Toy2/Levels.h"
#include "Toy2/Platform.h"

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

		// STUB: TOY2 0x00480660
		int32_t SweepWallSegments(Toy2::Collision::RaycastQuery* rayQuery, int32_t* a2, uint32_t* a3, int32_t a4) { return 0; }

		// STUB: TOY2 0x004813B0
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
			return 0;
		}

		// STUB: TOY2 0x00481D00
		int32_t SweepVertex(int32_t* segStart, int32_t* segEnd, Toy2::Collision::RaycastQuery* rayQuery, int32_t radius, int16_t* faceAdjacency) { return 0; }

		// STUB: TOY2 0x00481FB0
		int32_t SweepTriangle(int32_t a1, Toy2::Collision::RaycastQuery* raycastQuery, int32_t a3, int32_t a4) { return 0; }

		// STUB: TOY2 0x00482A00
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
			return 0;
		}

		// STUB: TOY2 0x00483EF0
		int32_t ResolvePlatformFooting(
			Toy2::Collision::ResolvedVelocity* resolvedVelocity,
			WorldPos* worldPos,
			Vector3I* velocity,
			Toy2::Collision::BuzzCollisionState* collisionState)
		{
			return 0;
		}

		// STUB: TOY2 0x004878A0
		void MarkPlatformAsMoving(int32_t platformIdx) {}

		// STUB: TOY2 0x00487AD0
		BOOL IsSafeFooting(int32_t a1, uint8_t* a2) { return 0; }

		// STUB: TOY2 0x0048E1B0
		BOOL IsMeshMoving() { return 0; }

		// STUB: TOY2 0x0048E1D0
		int32_t GetGroundContactIdx() { return 0; }
	}
}
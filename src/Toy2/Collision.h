#pragma once

#include "Common.h"
#include "Numerics.h"
#include <windows.h>

namespace Toy2
{
	namespace Collision
	{
		struct CollisionMeshInstance
		{
			Vector3I origin;
			int32_t platformIdx;
			int32_t collisionTree;
			Vector3I boundsMin;
			Vector3I boundsExt;
			int16_t typeFlags;
			int16_t unk;
			int32_t boundingSqRadius;
		};

		// every mesh has a collision tree, a list of nodes ended by a node with a negative flag, each is followed by its triangles
		struct CollisionTreeNode
		{
			int16_t flag;
			int16_t count;
			int16_t minX;
			int16_t extX;
			int16_t minZ;
			int16_t extZ;
		};

		struct CollisionTriangle
		{
			int16_t minX;
			int16_t extX;
			int8_t minYByte;
			int8_t extYByte;
			int8_t minZByte;
			int8_t extZByte;
			int16_t v0x;
			int16_t v0y;
			int16_t v0z;
			int16_t e1x;
			int16_t e1y;
			int16_t e1z;
			int16_t e2x;
			int16_t e2y;
			int16_t e2z;
			int16_t e3x;
			int16_t e3y;
			int16_t e3z;
			int16_t unk20;
			int16_t unk22;
			int16_t unk24;
			int16_t unk26;
			int16_t unk28;
			int16_t unk2A;
		};

		struct CollisionGridCell
		{
			int16_t start;
			int16_t count;
			int32_t minX;
			int32_t minZ;
			int32_t extX;
			int32_t extZ;
		};

		// one of the 8 cached ground queries of GatherTrianglesAtXZ (g_unk72871C), the triangles near an x / z position
		struct CollisionCacheEntry
		{
			int32_t x;
			int32_t y;
			int32_t z;
			int32_t unkC;
			int32_t unk10;
			int32_t unk14;
			int16_t lifetime;
			int16_t unk1A;
			int32_t count;
			int16_t meshIdx[240];
			CollisionTriangle* triangles[240];
		};

		struct BuzzCollisionState
		{
			int32_t contactSurfaceIndex;
			int32_t contactSurfaceData;
			Vector3I16 surfaceNormal;
			int16_t wasGroundedLastFrame;
			Vector3I16 unkVec;
			int16_t unkWord12;
			Vector3I16 pushbackVel;
			Vector3I16 responseVel;
			int32_t stuckTimer;
			int16_t platformIdx;
			int16_t unkWord20;
			int16_t floorOffset;
			int16_t collisionQuality;
		};

		struct RaycastQuery
		{
			Vector3I origin;
			int32_t originW;
			Vector3I endpoint;
			int32_t endpointW;
			int32_t bestDist;
			int32_t hitT0;
			int32_t hitT1;
			int32_t hitFlags;
			void* hitFace;
			Vector3I16 hitNormal;
			int16_t pad;
		};

		struct ResolvedVelocity
		{
			Vector3I16 vel;
			int16_t unused;
			Vector3I16 platformVel;
		};

		extern CollisionMeshInstance g_collisionMeshInstances[300];
		extern uint8_t g_mathScratch[1024];
		extern CollisionGridCell g_collisionGrid[256];
		extern uint16_t g_collisionMeshList[2048];
		extern BuzzCollisionState g_buzzCollisionStates[2];
		extern int32_t g_candidateTriangleCount;
		extern CollisionTriangle* g_candidateTris[240];
		extern int16_t g_candidateMeshIdx[241];
		extern int16_t g_ceilingContactMeshIdx;
		extern int32_t g_groundQueryResult;
		extern int16_t g_unk7284A0;
		extern int16_t g_unk7284A2;
		extern int32_t g_unk729120;
		extern int32_t g_unk72912C;
		extern int16_t g_groundContactMeshIdx;
		extern Vector3I16 g_floorSlope;
		extern Vector3I16 g_floorSlopePrev;
		extern uint8_t* g_unk72871C;
		extern int32_t g_unk7290F0[8];

		void BuildCollisionWorld(int32_t level, uint8_t** buffer, int32_t terrainNum);
		int32_t ResolveSubstep(
			Toy2::Collision::ResolvedVelocity* a1,
			WorldPos* a2,
			Vector3I* a3,
			Toy2::Collision::BuzzCollisionState* a4,
			uint16_t* a5,
			int32_t* a6,
			int32_t a7,
			int32_t a8);
		int32_t ResolvePlatformFooting(
			Toy2::Collision::ResolvedVelocity* resolvedVelocity,
			WorldPos* worldPos,
			Vector3I* velocity,
			Toy2::Collision::BuzzCollisionState* collisionState);
		void MarkPlatformAsMoving(int32_t platformIdx);
		BOOL IsSafeFooting(int32_t a1, uint8_t* a2);
		BOOL IsMeshMoving();
		int32_t GetGroundContactIdx();

		STATIC_ASSERT(sizeof(CollisionTreeNode) == 0xC);
		STATIC_ASSERT(sizeof(CollisionTriangle) == 0x2C);
		STATIC_ASSERT(sizeof(CollisionGridCell) == 0x14);
		STATIC_ASSERT(sizeof(CollisionMeshInstance) == 0x34);
		STATIC_ASSERT(sizeof(CollisionCacheEntry) == 0x5C0);
		STATIC_ASSERT(sizeof(BuzzCollisionState) == 0x30);
		STATIC_ASSERT(sizeof(RaycastQuery) == 0x3C);
		STATIC_ASSERT(sizeof(ResolvedVelocity) == 0xE);
	}
}
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

		STATIC_ASSERT(sizeof(CollisionMeshInstance) == 0x34);
		STATIC_ASSERT(sizeof(BuzzCollisionState) == 0x30);
		STATIC_ASSERT(sizeof(RaycastQuery) == 0x3C);
		STATIC_ASSERT(sizeof(ResolvedVelocity) == 0xE);
	}
}
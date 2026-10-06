#include "Toy2/Collision.h"

namespace Toy2
{
	namespace Collision
	{
		// GLOBAL: TOY2 0x007295A8
		CollisionMeshInstance g_collisionMeshInstances[300];

		// GLOBAL: TOY2 0x00554FA0
		uint8_t g_mathScratch[1024];

		// STUB: TOY2 0x00489C30
		void BuildCollisionWorld(int32_t level, uint8_t** buffer, int32_t terrainNum) {}

		// STUB: TOY2 0x00480660
		int32_t SweepWallSegments(Toy2::Collision::RaycastQuery* rayQuery, int32_t* a2, uint32_t* a3, int32_t a4) { return 0; }

		// STUB: TOY2 0x004813B0
		int32_t SweepEdge(int32_t* segStart, int32_t* segEnd, int32_t edgeDirX, int32_t edgeDirY, int32_t edgeDirZ, Toy2::Collision::RaycastQuery* rayQuery, int32_t radius, int16_t* faceAdjacency) { return 0; }

		// STUB: TOY2 0x00481D00
		int32_t SweepVertex(int32_t* segStart, int32_t* segEnd, Toy2::Collision::RaycastQuery* rayQuery, int32_t radius, int16_t* faceAdjacency) { return 0; }

		// STUB: TOY2 0x00481FB0
		int32_t SweepTriangle(int32_t a1, Toy2::Collision::RaycastQuery* raycastQuery, int32_t a3, int32_t a4) { return 0; }

		// STUB: TOY2 0x00482A00
		int32_t ResolveSubstep(Toy2::Collision::ResolvedVelocity* a1, WorldPos* a2, Vector3I* a3, Toy2::Collision::BuzzCollisionState* a4, uint16_t* a5, int32_t* a6, int32_t a7, int32_t a8) { return 0; }

		// STUB: TOY2 0x00483EF0
		int32_t ResolvePlatformFooting(Toy2::Collision::ResolvedVelocity* resolvedVelocity, WorldPos* worldPos, Vector3I* velocity, Toy2::Collision::BuzzCollisionState* collisionState) { return 0; }

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
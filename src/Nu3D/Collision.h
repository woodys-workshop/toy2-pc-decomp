#pragma once

#include "Common.h"
#include <windows.h>

struct Vector3I16;
struct WorldPos;

namespace Nu3D
{
	namespace Collision
	{
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
			Vector3I16* normal);
		void GatherTrianglesAtXZ(WorldPos* worldPos);
		int32_t GetGroundHeight(WorldPos* worldPos, int32_t size);
		int32_t GetGroundHeightEx(WorldPos* worldPos, int32_t size, int32_t headRoom);
		int32_t ResolveGroundCeiling(WorldPos* worldPos, int32_t headRoom);
		BOOL IsFloorWalkable();
		int32_t GetSurfaceQuality(int32_t collisionState);
		int32_t SweepAgainstEdges(int32_t* a1, int32_t* a2, int32_t* a3, uint32_t* a4, int32_t* a5, int32_t* a6, int32_t a7);
		int32_t RaycastAgainstEdges(int32_t* a1, int32_t* a2, int32_t* a3, uint32_t* a4, int16_t* a5, int32_t* a6, int32_t* a7);
		int32_t SweepAgainstCandidates(int32_t* a1, int32_t* a2, int32_t a3);
		int32_t RaycastAgainstCandidates(int32_t a1, int32_t* a2, int32_t* a3);
		int32_t SweepAndSlide(WorldPos* startPos, WorldPos* displacement, int32_t collisionMask, int32_t useTopSector, int32_t radius);
		int32_t SweepQuery(int32_t* a1, int32_t* a2, int32_t a3, int32_t a4);
	}
}

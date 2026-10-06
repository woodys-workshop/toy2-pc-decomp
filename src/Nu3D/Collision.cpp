#include "Nu3D/Collision.h"

namespace Nu3D
{
	namespace Collision
	{
		// STUB: TOY2 0x00481140
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
			return 0;
		}

		// STUB: TOY2 0x00485940
		void GatherTrianglesAtXZ(WorldPos* worldPos) {}

		// STUB: TOY2 0x00486280
		int32_t GetGroundHeight(WorldPos* worldPos, int32_t size) { return 0; }

		// STUB: TOY2 0x00486310
		int32_t GetGroundHeightEx(WorldPos* worldPos, int32_t size, int32_t headRoom) { return 0; }

		// STUB: TOY2 0x00486520
		int32_t ResolveGroundCeiling(WorldPos* worldPos, int32_t headRoom) { return 0; }

		// STUB: TOY2 0x00487A60
		BOOL IsFloorWalkable() { return 0; }

		// STUB: TOY2 0x00487AB0
		int32_t GetSurfaceQuality(int32_t collisionState) { return 0; }

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

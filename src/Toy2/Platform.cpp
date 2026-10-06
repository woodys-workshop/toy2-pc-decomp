#include "Toy2/Platform.h"

namespace Toy2
{
	namespace Platform
	{
		// STUB: TOY2 0x0042C2B0
		void InitPathPlatform(int32_t a1, int32_t a2, int32_t a3, int32_t linkId, int32_t linkId2, int32_t a6, int32_t a7, int32_t y) {}

		// STUB: TOY2 0x004878D0
		void DisableCollision(int32_t platformIdx) {}

		// STUB: TOY2 0x00487900
		void SetVelocity(int32_t platformIndex, int32_t velX, int32_t velY, int32_t velZ) {}

		// STUB: TOY2 0x00487970
		void SetAngularVelocity(int32_t idx, int16_t x, int16_t y, int16_t z) {}

		// STUB: TOY2 0x004879C0
		void GetOrigin(int32_t platformindex, Vector3I* outOrigin) {}

		// STUB: TOY2 0x00487A20
		void GetRotation(int32_t platformIdx, Vector3I* vecOut) {}

		// STUB: TOY2 0x00488510
		void SetOrigin(int32_t platformIndex, int32_t x, int32_t y, int32_t z) {}

		// STUB: TOY2 0x00488580
		int32_t GetFlags(int32_t platformIdx) { return 0; }

		// STUB: TOY2 0x004885A0
		int32_t GetContactFace32(int32_t index) { return 0; }

		// STUB: TOY2 0x00488A60
		void AddFlags(int32_t platformIdx, int16_t flags) {}

		// STUB: TOY2 0x00488A80
		void ClearFlags(int32_t platformIdx, int16_t flags) {}

		// STUB: TOY2 0x00488AA0
		void SetRotationAngles(int32_t platformIdx, int16_t x, int16_t y, int16_t z) {}

		// STUB: TOY2 0x00488AF0
		void GetRotationAngles(int32_t platformIdx, Vector3I* vecOut) {}

		// STUB: TOY2 0x00488BD0
		void CommitRotationToLink(int32_t platformIdx, int32_t linkId) {}

		// STUB: TOY2 0x00488C10
		void AdvancePartialMotion(int32_t a1, int32_t a2, int32_t a3) {}

		// STUB: TOY2 0x00488E10
		char UpdateBuzzPlatformMotion(int32_t a1, int32_t a2) { return 0; }

		// STUB: TOY2 0x0048ACC0
		void StepMotionScript(int32_t platformIndex, int32_t linkId, int32_t** scriptIP, int32_t** timer, int32_t** accelRamp) {}

		// STUB: TOY2 0x0048B640
		BOOL HadBuzzContactThisFrame(int32_t platformIdx) { return 0; }

		// STUB: TOY2 0x0048E200
		void CopyVelocity(int32_t srcPlatform, int32_t dstPlatform) {}

		// STUB: TOY2 0x0049EC00
		int32_t StepTiltPhysics(int32_t platformId, int32_t linkId, int32_t angVel, int32_t mode, int32_t minAngle, int32_t maxAngle, int32_t divisor)
		{
			return 0;
		}

		// STUB: TOY2 0x0042C200
		int32_t SamplePoint(int32_t a1, int32_t a2, uint32_t* a3) { return 0; }
	}
}

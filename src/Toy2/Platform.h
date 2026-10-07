#pragma once

#include "Common.h"
#include "Numerics.h"
#include <windows.h>

namespace Toy2
{
	namespace Platform
	{
		struct PlatformDescriptor
		{
			Vector3I origin;
			Vector3I16 rotAngle;
			int16_t unkVar10;
			Vector3I16 linVel;
			Vector3I16 prevLinVel;
			Vector3I16 angVel;
			Vector3I16 prevAngVel;
			void* contactFace;
			int16_t meshInstanceId;
			int16_t flags;
		};

		extern PlatformDescriptor g_platformDescriptors[32];

		int32_t SamplePoint(int32_t a1, int32_t a2, uint32_t* a3);
		void InitPathPlatform(int32_t a1, int32_t a2, int32_t a3, int32_t linkId, int32_t linkId2, int32_t a6, int32_t a7, int32_t y);
		void DisableCollision(int32_t platformIdx);
		void SetVelocity(int32_t platformIndex, int32_t velX, int32_t velY, int32_t velZ);
		void SetAngularVelocity(int32_t idx, int16_t x, int16_t y, int16_t z);
		void GetOrigin(int32_t platformindex, Vector3I* outOrigin);
		void GetRotation(int32_t platformIdx, Vector3I* vecOut);
		void SetOrigin(int32_t platformIndex, int32_t x, int32_t y, int32_t z);
		int32_t GetFlags(int32_t platformIdx);
		int32_t GetContactFace32(int32_t index);
		void AddFlags(int32_t platformIdx, int16_t flags);
		void ClearFlags(int32_t platformIdx, int16_t flags);
		void SetRotationAngles(int32_t platformIdx, int16_t x, int16_t y, int16_t z);
		void CommitRotationToLink(int32_t platformIdx, int32_t linkId);
		void AdvancePartialMotion(int32_t a1, int32_t a2, int32_t a3);
		char UpdateBuzzPlatformMotion(int32_t a1, int32_t a2);
		void StepMotionScript(int32_t platformIndex, int32_t linkId, int32_t** scriptIP, int32_t** timer, int32_t** accelRamp);
		BOOL HadBuzzContactThisFrame(int32_t platformIdx);
		void CopyVelocity(int32_t srcPlatform, int32_t dstPlatform);
		int32_t StepTiltPhysics(int32_t platformId, int32_t linkId, int32_t angVel, int32_t mode, int32_t minAngle, int32_t maxAngle, int32_t divisor);

		STATIC_ASSERT(sizeof(PlatformDescriptor) == 0x34);
	}
}

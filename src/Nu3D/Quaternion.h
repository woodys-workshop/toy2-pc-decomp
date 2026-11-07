#pragma once

#include "Nu3D/Math.h"

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;
};

namespace Nu3D
{
	namespace Math
	{
		void MatToQuat(float matrix[4][4], Quaternion* quat);
		void QuatToMatrix(Quaternion* quat, D3DMATRIX* matrix);
		void QuatAdd(Quaternion* result, Quaternion* a, Quaternion* b);
		void QuatSub(Quaternion* result, Quaternion* a, Quaternion* b);
		void QuatMul(Quaternion* result, Quaternion* a, Quaternion* b);
		void QuatInverse(Quaternion* result, Quaternion* quat);
		void QuatNormalize(Quaternion* result, Quaternion* quat);
		float QuatNormSquared(Quaternion* quat);
		void QuatLerp(Quaternion* result, Quaternion* from, Quaternion* to, float t);
		void QuatSlerp(Quaternion* res, Quaternion* from, Quaternion* to, float t);

		STATIC_ASSERT(sizeof(Quaternion) == 0x10);
	}
}

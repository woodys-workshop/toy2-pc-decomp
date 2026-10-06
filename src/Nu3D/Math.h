#pragma once

#include "Numerics.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include "Common.h"
#include <windows.h>


namespace Nu3D
{
	namespace Math
	{
		extern Vector3F g_matrixScaleVector;

		struct PointI16
		{
			int16_t x;
			int16_t y;
		};

		void BuildIdentityMatrix(D3DMATRIX* matrix);
		void ScaleMatrixByVector(D3DMATRIX* matrix, Vector3F* vector);
		void RotateZFromLut(D3DMATRIX* matrix, int32_t trigOffset);
		void RotateYFromLut(D3DMATRIX* matrix, int32_t trigOffset);
		void PostRotateXFromLut(D3DMATRIX* matrix, int32_t trigOffset);
		void AddWorldSpaceTransform(D3DMATRIX* matrix, Vector3F* offset);
		void TransformVectorByMatrix(Vector3F* result, Vector3F* sourceVector, D3DMATRIX* matrix);
		void CalculatePlaneFromTriangle(Vector3F* point1, Vector3F* point2, Vector3F* point3, Plane* plane);
		void VertexAdd(Vector3F* result, Vector3F* v1, Vector3F* v2);
		void VertexSubtract(Vector3F* result, Vector3F* v1, Vector3F* v2);
		void VertexCrossProduct(Vector3F* result, Vector3F* v1, Vector3F* v2);
		float VectorNormalize(Vector3F* output, Vector3F* vector);
		void ScaleVector(Vector3F* result, Vector3F* vector, float scale);
		void CalculatePlaneFromTriangle(Vector3F* point1, Vector3F* point2, Vector3F* point3, Plane* plane);
		int32_t NormalizeToFixedPoint(Vector3I* v1, Vector3I* v2);
		void ApplyRotateXFromLut(D3DMATRIX* matrix, int32_t trigOffset);
		void ScaleMatrix(D3DMATRIX* matrix);
		void CreateInverseMatrix(D3DMATRIX* output, D3DMATRIX* input);
		void MultiplyMatrix3x4(D3DMATRIX* output, D3DMATRIX* left, D3DMATRIX* right);
		void FullMatrixMultiply(D3DMATRIX* output, D3DMATRIX* left, D3DMATRIX* right);
		void InvertAffineMatrix(D3DMATRIX* output, D3DMATRIX* input);
		Matrix3x3I16* EulerToRotationMatrix(AnglesWithRoll* angles, Matrix3x3I16* matrix);
		D3DMATRIX* SetRotationYFromU16AngleLUT(D3DMATRIX* matrix, int16_t angle);
		int32_t CartesianToFixedAngle(int32_t x, int32_t y);
		float Abs(float value);
		float DotProduct(Vector3F* v1, Vector3F* v2);
		float GetSignedDistanceToPlane(Vector3F* point, Plane* plane);
		int32_t IsWithinDistance(Vector3I* v1, Vector3I* v2, int32_t maxDistance);
		void BuildMatrixFromDirection(D3DMATRIX* matrix, Vector3F* direction);

		void GetRightVector(D3DMATRIX *matrix, Vector3F *out);
		void GetUpVector(D3DMATRIX *matrix, Vector3F *out);
		void GetForwardVector(D3DMATRIX *matrix, Vector3F *out);
		void GetPositionVector(D3DMATRIX *matrix, Vector3F *out);

		void TransformPointByMatrix(Vector3F *out, Vector3F *point, D3DMATRIX *matrix);
		Vector3F* ProjectPoint(Vector3F* out, Vector3F* in, D3DMATRIX* matrix);
		Vector3F* TransformPointPerspective(Vector3F* result, Vector3F* point, D3DMATRIX* matrix);

		int32_t Cross2D(Nu3D::Math::PointI16 point1, Nu3D::Math::PointI16 point2, Nu3D::Math::PointI16 point3);
		int32_t NormalizeToFixedPoint16(Vector3I16* in, Vector3I16* out);
		int16_t PointIntersectsTriangle(int32_t pointX, int32_t pointY, int32_t pointZ, int32_t edge0X, int32_t edge0Y, int32_t edge0Z, int32_t edge1X, int32_t edge1Y, int32_t edge1Z, Vector3I16* normal, int32_t radius);
		void CrossProduct3D(Vector3I* a, Vector3I* b, Vector3I* out);
		BOOL IsWithinDistanceXZ(Vector3I* pos1, Vector3I* pos2, int32_t distance);
		D3DMATRIX* MatrixApplyScale(D3DMATRIX* a1, Vector3F* a2);
		D3DMATRIX* MatrixRotatePitch(D3DMATRIX* a1, uint16_t a2);
		D3DMATRIX* MatrixRotateYaw(D3DMATRIX* a1, uint16_t a2);
		D3DMATRIX* MatrixRotateRoll(D3DMATRIX* matrix, int16_t trigIndex);

		STATIC_ASSERT(sizeof(PointI16) == 0x4);
	}
}
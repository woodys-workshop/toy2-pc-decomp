#include "Nu3D/Math.h"
#include "Nu3D/Quaternion.h"
#include <MATH.H>
#include <STDLIB.h>

namespace Nu3D
{
	namespace Math
	{
		// NOTE: Methods in this class are kind of critical to get correct, since minor changes in how
		// math is calculated could cause weeks of debugging effort to fix. So I take extra care ensuring
		// that most, if not all of these methods are instruction matched from the moment they are implemented.

		// GLOBAL: TOY2 0x004DDA48
		D3DMATRIX g_identityMatrix = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };

		// GLOBAL: TOY2 0x00505548
		D3DMATRIX g_tempMatrix = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };

		// GLOBAL: TOY2 0x00508898
		Vector3F g_matrixScaleVector = { 1.0, 1.0, 1.0 };

		// FUNCTION: TOY2 0x004A9400 [MATCHED]
		void BuildIdentityMatrix(D3DMATRIX* matrix) { memcpy(matrix, &g_identityMatrix, sizeof(D3DMATRIX)); }

		// FUNCTION: TOY2 0x004A9860 [MATCHED]
		void ScaleMatrixByVector(D3DMATRIX* matrix, Vector3F* vector)
		{
			matrix->_11 = matrix->_11 * vector->x;
			matrix->_12 = vector->y * matrix->_12;
			matrix->_13 = vector->z * matrix->_13;
			matrix->_21 = matrix->_21 * vector->x;
			matrix->_22 = matrix->_22 * vector->y;
			matrix->_23 = matrix->_23 * vector->z;
			matrix->_31 = matrix->_31 * vector->x;
			matrix->_32 = matrix->_32 * vector->y;
			matrix->_33 = matrix->_33 * vector->z;
			matrix->_41 = matrix->_41 * vector->x;
			matrix->_42 = matrix->_42 * vector->y;
			matrix->_43 = matrix->_43 * vector->z;
		}

		// FUNCTION: TOY2 0x004A9D50 [MATCHED]
		void RotateZFromLut(D3DMATRIX* matrix, int32_t trigOffset)
		{
			float cos = Numerics::g_trigLUT[(trigOffset + 0x4000) & 0xFFFF];
			float sin = Numerics::g_trigLUT[trigOffset & 0xFFFF];

			float r1 = matrix->_11;
			float r0 = r1 * cos - sin * matrix->_12;
			float r2 = matrix->_21;
			float r3 = matrix->_31;

			matrix->_11 = r0;

			float r4 = matrix->_41;
			matrix->_12 = r1 * sin + cos * matrix->_12;

			matrix->_21 = r2 * cos - sin * matrix->_22;
			matrix->_22 = cos * matrix->_22 + r2 * sin;

			matrix->_31 = r3 * cos - sin * matrix->_32;
			matrix->_32 = cos * matrix->_32 + r3 * sin;

			matrix->_41 = r4 * cos - sin * matrix->_42;
			matrix->_42 = cos * matrix->_42 + r4 * sin;
		}

		// FUNCTION: TOY2 0x004A9B40 [MATCHED]
		void RotateYFromLut(D3DMATRIX* matrix, int32_t trigOffset)
		{
			float cos = Numerics::g_trigLUT[(trigOffset + 0x4000) & 0xFFFF];
			float sin = Numerics::g_trigLUT[trigOffset & 0xFFFF];

			float _11 = matrix->_11;

			float _21 = matrix->_21;
			float _31 = matrix->_31;

			matrix->_11 = matrix->_13 * sin + _11 * cos;

			float _41 = matrix->_41;
			matrix->_13 = matrix->_13 * cos - _11 * sin;

			matrix->_21 = matrix->_23 * sin + _21 * cos;
			matrix->_23 = matrix->_23 * cos - _21 * sin;

			matrix->_31 = matrix->_33 * sin + _31 * cos;
			matrix->_33 = matrix->_33 * cos - _31 * sin;

			matrix->_41 = matrix->_43 * sin + _41 * cos;
			matrix->_43 = matrix->_43 * cos - _41 * sin;
		}

		// FUNCTION: TOY2 0x004A9930 [MATCHED]
		void PostRotateXFromLut(D3DMATRIX* matrix, int32_t trigOffset)
		{
			float cosAngle = Numerics::g_trigLUT[(trigOffset + 0x4000) & 0xFFFF];
			float sinAngle = Numerics::g_trigLUT[trigOffset & 0xFFFF];

			float temp12 = matrix->_12;
			float temp13 = temp12 * cosAngle - sinAngle * matrix->_13;

			float temp22 = matrix->_22;
			float temp32 = matrix->_32;

			matrix->_12 = temp13;

			float temp42 = matrix->_42;
			matrix->_13 = cosAngle * matrix->_13 + temp12 * sinAngle;

			matrix->_22 = temp22 * cosAngle - sinAngle * matrix->_23;
			matrix->_23 = cosAngle * matrix->_23 + temp22 * sinAngle;

			matrix->_32 = temp32 * cosAngle - sinAngle * matrix->_33;
			matrix->_33 = cosAngle * matrix->_33 + temp32 * sinAngle;

			matrix->_42 = temp42 * cosAngle - sinAngle * matrix->_43;
			matrix->_43 = temp42 * sinAngle + cosAngle * matrix->_43;
		}

		// FUNCTION: TOY2 0x004A9750 [MATCHED]
		void AddWorldSpaceTransform(D3DMATRIX* matrix, Vector3F* offset)
		{
			matrix->_41 = offset->x + matrix->_41;
			matrix->_42 = offset->y + matrix->_42;
			matrix->_43 = offset->z + matrix->_43;
		}

		// FUNCTION: TOY2 0x004A8D20 [MATCHED]
		void TransformVectorByMatrix(Vector3F* result, Vector3F* sourceVector, D3DMATRIX* matrix)
		{
			float transformedY = matrix->_32 * sourceVector->z + matrix->_12 * sourceVector->x + matrix->_22 * sourceVector->y;
			float tempZ = matrix->_33 * sourceVector->z + matrix->_13 * sourceVector->x + matrix->_23 * sourceVector->y;

			float transformedX = matrix->_31 * sourceVector->z + matrix->_21 * sourceVector->y;
			float transformedZ = sourceVector->x * matrix->_11;

			result->x = transformedX + transformedZ;
			result->y = transformedY;
			result->z = tempZ;
		}

		// FUNCTION: TOY2 0x004A91B0 [MATCHED]
		void VertexAdd(Vector3F* result, Vector3F* v1, Vector3F* v2)
		{
			result->x = v1->x + v2->x;
			result->y = v1->y + v2->y;
			result->z = v1->z + v2->z;
		}

		// FUNCTION: TOY2 0x004A91E0 [MATCHED]
		void VertexSubtract(Vector3F* result, Vector3F* v1, Vector3F* v2)
		{
			result->x = v1->x - v2->x;
			result->y = v1->y - v2->y;
			result->z = v1->z - v2->z;
		}

		// FUNCTION: TOY2 0x004A9270 [MATCHED]
		void VertexCrossProduct(Vector3F* result, Vector3F* v1, Vector3F* v2)
		{
			float y = v2->x * v1->z - v2->z * v1->x;
			float z = v2->y * v1->x - v2->x * v1->y;

			result->x = v2->z * v1->y - v2->y * v1->z;
			result->y = y;
			result->z = z;
		}

		// FUNCTION: TOY2 0x004A9340 [MATCHED]
		float VectorNormalize(Vector3F* output, Vector3F* vector)
		{
			float lengthSquared = vector->x * vector->x + vector->y * vector->y + vector->z * vector->z;
			float magnitude = sqrt(lengthSquared);
			float scale = 1.0f / magnitude;

			output->x = scale * vector->x;
			output->y = scale * vector->y;
			output->z = scale * vector->z;

			return magnitude;
		}

		// FUNCTION: TOY2 0x004DA850 [MATCHED]
		void CalculatePlaneFromTriangle(Vector3F* point1, Vector3F* point2, Vector3F* point3, Plane* plane)
		{
			Vector3F edge1;
			Vector3F edge2;

			VertexSubtract(&edge2, point2, point1);
			VertexSubtract(&edge1, point3, point1);

			Vector3F normal;

			VertexCrossProduct(&normal, &edge2, &edge1);
			VectorNormalize(&plane->normal, &normal);

			plane->distance = -(point1->z * plane->normal.z + point1->y * plane->normal.y + point1->x * plane->normal.x);
		}

		// FUNCTION: TOY2 0x00451FD0 [MATCHED]
		int32_t NormalizeToFixedPoint(Vector3I* input, Vector3I* output)
		{
			float z = input->z;
			float y = input->y;
			float x = input->x;

			float sqrtVal = sqrt(x * x + y * y + z * z);

			output->x = (x * 4096.0f / sqrtVal);
			output->y = (input->y * 4096.0f / sqrtVal);
			output->z = (input->z * 4096.0f / sqrtVal);

			return 0;
		}

		// FUNCTION: TOY2 0x004A94C0 [MATCHED]
		void ApplyRotateXFromLut(D3DMATRIX* matrix, int32_t trigOffset)
		{
			float cosine = Numerics::g_trigLUT[(trigOffset + 0x4000) & 0xFFFF];

			matrix->_33 = cosine;
			matrix->_22 = cosine;

			float sine = Numerics::g_trigLUT[trigOffset & 0xFFFF];

			matrix->_11 = 1.0f;
			matrix->_23 = sine;
			matrix->_32 = -sine;
			matrix->_43 = 0.0f;
			matrix->_42 = 0.0f;
			matrix->_41 = 0.0f;
			matrix->_24 = 0.0f;
			matrix->_31 = 0.0f;
			matrix->_21 = 0.0f;
			matrix->_14 = 0.0f;
			matrix->_13 = 0.0f;
			matrix->_12 = 0.0f;
			matrix->_44 = 1.0f;
		}

		// FUNCTION: TOY2 0x004BB920 [MATCHED]
		void ScaleMatrix(D3DMATRIX* matrix)
		{
			matrix->_11 *= g_matrixScaleVector.x;
			matrix->_12 *= g_matrixScaleVector.x;
			matrix->_13 *= g_matrixScaleVector.x;
			matrix->_14 *= g_matrixScaleVector.x;
			matrix->_21 *= g_matrixScaleVector.y;
			matrix->_22 *= g_matrixScaleVector.y;
			matrix->_23 *= g_matrixScaleVector.y;
			matrix->_24 *= g_matrixScaleVector.y;
			matrix->_31 *= g_matrixScaleVector.z;
			matrix->_32 *= g_matrixScaleVector.z;
			matrix->_33 *= g_matrixScaleVector.z;
			matrix->_34 *= g_matrixScaleVector.z;
		}

		// FUNCTION: TOY2 0x004AA520 [MATCHED]
		void CreateInverseMatrix(D3DMATRIX* output, D3DMATRIX* input)
		{
			float translationX = -input->_41;
			float translationY = -input->_42;
			float translationZ = -input->_43;
			float transposeValue = input->_12;

			output->_12 = input->_21;
			output->_21 = transposeValue;
			transposeValue = input->_13;

			output->_13 = input->_31;
			output->_31 = transposeValue;
			transposeValue = input->_23;

			output->_23 = input->_32;
			output->_32 = transposeValue;
			output->_11 = input->_11;
			output->_22 = input->_22;
			output->_33 = input->_33;

			output->_34 = 0.0f;
			output->_24 = 0.0f;
			output->_14 = 0.0f;
			output->_44 = 1.0f;

			output->_41 = translationZ * output->_31 + translationX * output->_11 + translationY * output->_21;
			output->_42 = translationZ * output->_32 + translationX * output->_12 + translationY * output->_22;
			output->_43 = translationZ * output->_33 + translationX * output->_13 + translationY * output->_23;
		}

		// FUNCTION: TOY2 0x004AA100
		void FullMatrixMultiply(D3DMATRIX* output, D3DMATRIX* left, D3DMATRIX* right)
		{
			g_tempMatrix._11 = left->_11 * right->_11 + left->_14 * right->_41 + left->_13 * right->_31 + right->_21 * left->_12;
			g_tempMatrix._12 = right->_42 * left->_14 + left->_12 * right->_22 + right->_32 * left->_13 + right->_12 * left->_11;
			g_tempMatrix._13 = right->_33 * left->_13 + right->_13 * left->_11 + left->_14 * right->_43 + right->_23 * left->_12;
			g_tempMatrix._14 = right->_44 * left->_14 + left->_13 * right->_34 + left->_11 * right->_14 + right->_24 * left->_12;
			g_tempMatrix._21 = left->_23 * right->_31 + left->_24 * right->_41 + right->_21 * left->_22 + left->_21 * right->_11;
			g_tempMatrix._22 = left->_23 * right->_32 + left->_21 * right->_12 + left->_22 * right->_22 + left->_24 * right->_42;
			g_tempMatrix._23 = left->_24 * right->_43 + left->_23 * right->_33 + right->_23 * left->_22 + left->_21 * right->_13;
			g_tempMatrix._24 = right->_44 * left->_24 + left->_23 * right->_34 + left->_21 * right->_14 + right->_24 * left->_22;
			g_tempMatrix._31 = left->_33 * right->_31 + left->_34 * right->_41 + right->_21 * left->_32 + left->_31 * right->_11;
			g_tempMatrix._32 = left->_33 * right->_32 + left->_31 * right->_12 + left->_32 * right->_22 + left->_34 * right->_42;
			g_tempMatrix._33 = left->_34 * right->_43 + left->_33 * right->_33 + right->_23 * left->_32 + left->_31 * right->_13;
			g_tempMatrix._34 = right->_44 * left->_34 + left->_33 * right->_34 + left->_31 * right->_14 + right->_24 * left->_32;
			g_tempMatrix._41 = left->_43 * right->_31 + left->_44 * right->_41 + right->_21 * left->_42 + left->_41 * right->_11;
			g_tempMatrix._42 = left->_43 * right->_32 + left->_41 * right->_12 + left->_42 * right->_22 + left->_44 * right->_42;
			g_tempMatrix._43 = left->_44 * right->_43 + left->_43 * right->_33 + right->_23 * left->_42 + left->_41 * right->_13;
			g_tempMatrix._44 = right->_44 * left->_44 + left->_43 * right->_34 + left->_41 * right->_14 + right->_24 * left->_42;

			memcpy(output, &g_tempMatrix, sizeof(D3DMATRIX));
		}

		// FUNCTION: TOY2 0x004A9F60
		void MultiplyMatrix3x4(D3DMATRIX* output, D3DMATRIX* left, D3DMATRIX* right)
		{
			g_tempMatrix._11 = left->_13 * right->_31 + left->_11 * right->_11 + left->_12 * right->_21;
			g_tempMatrix._12 = left->_13 * right->_32 + left->_11 * right->_12 + left->_12 * right->_22;
			g_tempMatrix._13 = left->_13 * right->_33 + left->_11 * right->_13 + left->_12 * right->_23;
			g_tempMatrix._14 = 0.0f;
			g_tempMatrix._21 = left->_23 * right->_31 + left->_21 * right->_11 + left->_22 * right->_21;
			g_tempMatrix._22 = left->_23 * right->_32 + left->_21 * right->_12 + left->_22 * right->_22;
			g_tempMatrix._23 = left->_23 * right->_33 + left->_21 * right->_13 + left->_22 * right->_23;
			g_tempMatrix._24 = 0.0f;
			g_tempMatrix._31 = left->_33 * right->_31 + left->_31 * right->_11 + left->_32 * right->_21;
			g_tempMatrix._32 = left->_33 * right->_32 + left->_31 * right->_12 + left->_32 * right->_22;
			g_tempMatrix._33 = left->_33 * right->_33 + left->_31 * right->_13 + left->_32 * right->_23;
			g_tempMatrix._34 = 0.0f;
			g_tempMatrix._41 = left->_43 * right->_31 + left->_41 * right->_11 + left->_42 * right->_21 + right->_41;
			g_tempMatrix._42 = left->_43 * right->_32 + left->_41 * right->_12 + left->_42 * right->_22 + right->_42;
			g_tempMatrix._43 = left->_43 * right->_33 + left->_41 * right->_13 + left->_42 * right->_23 + right->_43;
			g_tempMatrix._44 = 1.0f;

			memcpy(output, &g_tempMatrix, sizeof(D3DMATRIX));
		}

		// FUNCTION: TOY2 0x004A8B30 [MATCHED]
		float Abs(float value)
		{
			uint32_t bits = *(uint32_t*)&value;
			bits &= 0x7FFFFFFF;
			return *(float*)&bits;
		}

		// FUNCTION: TOY2 0x004AA620
		void InvertAffineMatrix(D3DMATRIX* output, D3DMATRIX* input)
		{
			D3DMATRIX source;

			memcpy(&source, input, sizeof(D3DMATRIX));

			float* row0 = &source._11;
			float* row1 = &source._21;
			float* row2 = &source._31;

			BuildIdentityMatrix(output);

			if (Abs(row1[0]) > Abs(row2[0]))
			{
				if (Abs(row1[0]) > Abs(row0[0]))
				{
					row0 = &source._21;
					row1 = &source._11;
					output->_11 = 0.0f;
					output->_12 = 1.0f;
					output->_21 = 1.0f;
					output->_22 = 0.0f;
				}
			}
			else if (Abs(row2[0]) > Abs(row0[0]))
			{
				row0 = &source._31;
				row2 = &source._11;
				output->_11 = 0.0f;
				output->_13 = 1.0f;
				output->_31 = 1.0f;
				output->_33 = 0.0f;
			}

			float scale = 1.0f / row0[0];
			row0[1] *= scale;
			row0[2] *= scale;
			output->_11 *= scale;
			output->_12 *= scale;
			output->_13 *= scale;

			row1[1] -= row1[0] * row0[1];
			row2[1] -= row2[0] * row0[1];
			source._42 -= source._41 * row0[1];
			row1[2] -= row1[0] * row0[2];
			row2[2] -= row2[0] * row0[2];
			source._43 -= source._41 * row0[2];
			output->_21 -= row1[0] * output->_11;
			output->_22 -= row1[0] * output->_12;
			output->_23 -= row1[0] * output->_13;
			output->_31 -= row2[0] * output->_11;
			output->_32 -= row2[0] * output->_12;
			output->_33 -= row2[0] * output->_13;
			output->_41 -= source._41 * output->_11;
			output->_42 -= source._41 * output->_12;
			output->_43 -= source._41 * output->_13;

			if (Abs(row2[1]) > Abs(row1[1]))
			{
				float* rowSwap = row1;
				row1 = row2;
				row2 = rowSwap;

				float value = output->_21;
				output->_21 = output->_31;
				output->_31 = value;
				value = output->_22;
				output->_22 = output->_32;
				output->_32 = value;
				value = output->_23;
				output->_23 = output->_33;
				output->_33 = value;
			}

			scale = 1.0f / row1[1];
			row1[2] *= scale;
			output->_21 *= scale;
			output->_22 *= scale;
			output->_23 *= scale;

			row0[2] -= row0[1] * row1[2];
			row2[2] -= row2[1] * row1[2];
			source._43 -= source._42 * row1[2];
			output->_11 -= row0[1] * output->_21;
			output->_12 -= row0[1] * output->_22;
			output->_13 -= row0[1] * output->_23;
			output->_31 -= row2[1] * output->_21;
			output->_32 -= row2[1] * output->_22;
			output->_33 -= row2[1] * output->_23;
			output->_41 -= source._42 * output->_21;
			output->_42 -= source._42 * output->_22;
			output->_43 -= source._42 * output->_23;

			scale = 1.0f / row2[2];
			output->_31 *= scale;
			output->_32 *= scale;
			output->_33 *= scale;

			output->_11 -= row0[2] * output->_31;
			output->_12 -= row0[2] * output->_32;
			output->_13 -= row0[2] * output->_33;
			output->_21 -= row1[2] * output->_31;
			output->_22 -= row1[2] * output->_32;
			output->_23 -= row1[2] * output->_33;
			output->_41 -= source._43 * output->_31;
			output->_42 -= source._43 * output->_32;
			output->_43 -= source._43 * output->_33;
		}

		// FUNCTION: TOY2 0x004A9800
		void GetRightVector(D3DMATRIX* matrix, Vector3F* out)
		{
			out->x = matrix->_11;
			out->y = matrix->_12;
			out->z = matrix->_13;
		}

		// FUNCTION: TOY2 0x004A9820
		void GetUpVector(D3DMATRIX* matrix, Vector3F* out)
		{
			out->x = matrix->_21;
			out->y = matrix->_22;
			out->z = matrix->_23;
		}

		// FUNCTION: TOY2 0x004A9840
		void GetForwardVector(D3DMATRIX* matrix, Vector3F* out)
		{
			out->x = matrix->_31;
			out->y = matrix->_32;
			out->z = matrix->_33;
		}

		// FUNCTION: TOY2 0x004A97E0
		void GetPositionVector(D3DMATRIX* matrix, Vector3F* out)
		{
			out->x = matrix->_41;
			out->y = matrix->_42;
			out->z = matrix->_43;
		}

		// FUNCTION: TOY2 0x004A8BF0 [MATCHED]
		void TransformPointByMatrix(Vector3F* out, Vector3F* point, D3DMATRIX* matrix)
		{
			float y = matrix->_32 * point->z + matrix->_12 * point->x + matrix->_22 * point->y + matrix->_42;
			float z = matrix->_33 * point->z + matrix->_13 * point->x + matrix->_23 * point->y + matrix->_43;
			float x = matrix->_31 * point->z + matrix->_21 * point->y + point->x * matrix->_11 + matrix->_41;

			out->x = x;
			out->y = y;
			out->z = z;
		}

		// FUNCTION: TOY2 0x004A8C60 [MATCHED]
		Vector3F* ProjectPoint(Vector3F* out, Vector3F* in, D3DMATRIX* matrix)
		{
			float invW = 1.0f / (matrix->_34 * in->z + matrix->_24 * in->y + matrix->_14 * in->x + matrix->_44);
			float y = (matrix->_32 * in->z + matrix->_22 * in->y + matrix->_12 * in->x + matrix->_42) * invW;
			float z = (matrix->_33 * in->z + matrix->_23 * in->y + matrix->_13 * in->x + matrix->_43) * invW;
			Vector3F* result = out;
			float x = (matrix->_31 * in->z + matrix->_21 * in->y + in->x * matrix->_11 + matrix->_41) * invW;
			out->x = x;
			out->y = y;
			out->z = z;
			return result;
		}

		// FUNCTION: TOY2 0x00450C70 [MATCHED]
		Matrix3x3I16* EulerToRotationMatrix(AnglesWithRoll* angles, Matrix3x3I16* matrix)
		{
			int32_t sinPitch = Numerics::g_sinCosLUT[angles->angles.pitch & 0xFFF] / 4;
			int32_t cosPitch = Numerics::g_sinCosLUT[(angles->angles.pitch + 1024) & 0xFFF] / 4;
			int32_t sinYaw = Numerics::g_sinCosLUT[angles->angles.yaw & 0xFFF] / 4;
			int32_t cosYaw = Numerics::g_sinCosLUT[(angles->angles.yaw + 1024) & 0xFFF] / 4;
			int32_t sinRoll = Numerics::g_sinCosLUT[angles->roll & 0xFFF] / 4;
			int32_t cosRoll = Numerics::g_sinCosLUT[(angles->roll + 1024) & 0xFFF] / 4;

			matrix->m00 = cosYaw * cosRoll / 4096;
			matrix->m01 = cosYaw * sinRoll / -4096;
			matrix->m02 = sinYaw;

			int32_t pitchYaw = sinPitch * sinYaw / 4096;
			matrix->m10 = (cosPitch * sinRoll + cosRoll * pitchYaw) / 4096;
			matrix->m11 = (cosPitch * cosRoll - sinRoll * pitchYaw) / 4096;
			matrix->m12 = sinPitch * cosYaw / -4096;

			int32_t cosPitchSinYaw = cosPitch * sinYaw / 4096;
			matrix->m20 = (sinPitch * sinRoll - cosRoll * cosPitchSinYaw) / 4096;
			matrix->m21 = (sinPitch * cosRoll + sinRoll * cosPitchSinYaw) / 4096;
			matrix->m22 = cosPitch * cosYaw / 4096;

			return matrix;
		}

		// FUNCTION: TOY2 0x004A8D80 [MATCHED]
		Vector3F* TransformPointPerspective(Vector3F* result, Vector3F* point, D3DMATRIX* matrix)
		{
			float invW = 1.0f / (matrix->_34 * point->z + matrix->_24 * point->y + matrix->_14 * point->x);
			float y = (matrix->_32 * point->z + matrix->_22 * point->y + matrix->_12 * point->x) * invW;
			float z = (matrix->_33 * point->z + matrix->_23 * point->y + matrix->_13 * point->x) * invW;
			float x = (matrix->_31 * point->z + matrix->_21 * point->y + point->x * matrix->_11) * invW;

			result->x = x;
			result->y = y;
			result->z = z;

			return result;
		}

		// FUNCTION: TOY2 0x004A9530
		D3DMATRIX* SetRotationYFromU16AngleLUT(D3DMATRIX* matrix, int32_t angle)
		{
			float cos = Numerics::g_trigLUT[(angle + 0x4000) & 0xFFFF];

			matrix->_33 = cos;
			matrix->_11 = cos;

			float sin = Numerics::g_trigLUT[angle & 0xFFFF];

			matrix->_31 = sin;
			matrix->_13 = -sin;
			matrix->_22 = 1.0;
			matrix->_43 = 0.0;
			matrix->_42 = 0.0;
			matrix->_41 = 0.0;
			matrix->_24 = 0.0;
			matrix->_32 = 0.0;
			matrix->_23 = 0.0;
			matrix->_14 = 0.0;
			matrix->_21 = 0.0;
			matrix->_12 = 0.0;
			matrix->_44 = 1.0;

			return matrix;
		}

		// FUNCTION: TOY2 0x004A9210
		void ScaleVector(Vector3F* result, Vector3F* vector, float scale)
		{
			result->x = scale * vector->x;
			result->y = scale * vector->y;
			result->z = scale * vector->z;
		}

		// FUNCTION: TOY2 0x004520D0 [MATCHED]
		int32_t CartesianToFixedAngle(int32_t x, int32_t y)
		{
			double scaledX = x * 6.283185308;
			scaledX *= 0.000244140625;

			double scaledY = y * 6.283185308;
			scaledY *= 0.000244140625;

			double angle = atan2(scaledX, scaledY) * 4096.0;
			return angle * 0.15915494307111402;
		}

		// FUNCTION: TOY2 0x004A92C0
		float DotProduct(Vector3F* v1, Vector3F* v2) { return v1->z * v2->z + v1->y * v2->y + v1->x * v2->x; }

		// FUNCTION: TOY2 0x004DB040
		float GetSignedDistanceToPlane(Vector3F* point, Plane* plane) { return DotProduct(point, &plane->normal) + plane->distance; }

		// FUNCTION: TOY2 0x0049F400
		int32_t IsWithinDistance(Vector3I* v1, Vector3I* v2, int32_t maxDistance)
		{
			int32_t deltaX = (v1->x - v2->x) >> 8;
			int32_t deltaY = (v1->y - v2->y) >> 8;
			int32_t deltaZ = (v1->z - v2->z) >> 8;

			int32_t distanceSquared = deltaY * deltaY + deltaZ * deltaZ + deltaX * deltaX;

			if (distanceSquared < maxDistance * maxDistance)
				return distanceSquared + 1;

			return 0;
		}

		// FUNCTION: TOY2 0x004AAC40
		void BuildMatrixFromDirection(D3DMATRIX* matrix, Vector3F* direction)
		{
			Vector3F* yAxis = (Vector3F*)&matrix->_21;
			Vector3F* zAxis = (Vector3F*)&matrix->_31;

			float xAxisOriginalMagnitude = matrix->_11 * matrix->_11 + matrix->_12 * matrix->_12 + matrix->_13 * matrix->_13;
			float yAxisOriginalMagnitude = matrix->_21 * matrix->_21 + matrix->_22 * matrix->_22 + matrix->_23 * matrix->_23;

			float zAxisScale = sqrt(
				(zAxis->x * zAxis->x + matrix->_32 * matrix->_32 + matrix->_33 * matrix->_33)
				/ (direction->x * direction->x + direction->y * direction->y + direction->z * direction->z));

			zAxis->x = zAxisScale * direction->x;
			matrix->_32 = zAxisScale * direction->y;
			matrix->_33 = zAxisScale * direction->z;

			if (Abs(DotProduct(yAxis, zAxis)) > 0.86602539f)
			{
				VertexCrossProduct(yAxis, zAxis, (Vector3F*)&matrix->_11);

				float yScale = sqrt(yAxisOriginalMagnitude / (matrix->_21 * matrix->_21 + matrix->_22 * matrix->_22 + matrix->_23 * matrix->_23));

				yAxis->x = yScale * yAxis->x;
				matrix->_22 = yScale * matrix->_22;
				matrix->_23 = yScale * matrix->_23;

				VertexCrossProduct((Vector3F*)&matrix->_11, yAxis, zAxis);

				float xScale = sqrt(xAxisOriginalMagnitude / (matrix->_11 * matrix->_11 + matrix->_12 * matrix->_12 + matrix->_13 * matrix->_13));

				matrix->_11 = xScale * matrix->_11;
				matrix->_12 = xScale * matrix->_12;
				matrix->_13 = xScale * matrix->_13;
			}
			else
			{
				VertexCrossProduct((Vector3F*)&matrix->_11, yAxis, zAxis);

				float xScale = sqrt(xAxisOriginalMagnitude / (matrix->_11 * matrix->_11 + matrix->_12 * matrix->_12 + matrix->_13 * matrix->_13));

				matrix->_11 = xScale * matrix->_11;
				matrix->_12 = xScale * matrix->_12;
				matrix->_13 = xScale * matrix->_13;

				VertexCrossProduct(yAxis, zAxis, (Vector3F*)&matrix->_11);

				float yScale = sqrt(yAxisOriginalMagnitude / (matrix->_21 * matrix->_21 + matrix->_22 * matrix->_22 + matrix->_23 * matrix->_23));

				yAxis->x = yScale * yAxis->x;
				matrix->_22 = yScale * matrix->_22;
				matrix->_23 = yScale * matrix->_23;
			}
		}

		// FUNCTION: TOY2 0x00451F80
		int32_t Cross2D(PointI16 point1, PointI16 point2, PointI16 point3)
		{
			return (point3.x - point2.x) * (point1.y - point2.y) - (point1.x - point2.x) * (point3.y - point2.y);
		}

		// FUNCTION: TOY2 0x00452040 [MATCHED]
		int32_t NormalizeToFixedPoint16(Vector3I16* in, Vector3I16* out)
		{
			int16_t x = in->x;
			int32_t length = (int32_t)sqrt((double)(in->z * in->z + in->y * in->y + x * x));

			if (in->x)
				out->x = (int16_t)((x << 12) / length);

			int16_t y = in->y;
			if (y)
				out->y = (int16_t)((y << 12) / length);

			int16_t z = in->z;
			if (z)
				out->z = (int16_t)((z << 12) / length);

			return 0;
		}

		// FUNCTION: TOY2 0x00480AE0
		int16_t PointIntersectsTriangle(
			int32_t pointX,
			int32_t pointY,
			int32_t pointZ,
			int32_t edge0X,
			int32_t edge0Y,
			int32_t edge0Z,
			int32_t edge1X,
			int32_t edge1Y,
			int32_t edge1Z,
			Vector3I16* normal,
			int32_t radius)
		{
			int32_t absNormalY = abs(normal->y);
			int32_t absNormalX = abs(normal->x);
			int32_t scale;

			if (absNormalY >= absNormalX && absNormalY >= abs(normal->z))
			{
				if (normal->y < 0)
				{
					if (edge0Z * pointX - edge0X * pointZ >= 0 && edge1X * (pointZ - edge1Z) - edge1Z * (pointX - edge1X) >= 0
						&& (edge1Z - edge0Z) * (pointX - edge0X) + (edge0X - edge1X) * (pointZ - edge0Z) >= 0)
						return 1;
				}
				else
				{
					if (edge0X * (pointZ - edge0Z) - edge0Z * (pointX - edge0X) >= 0 && edge1Z * pointX - edge1X * pointZ >= 0
						&& (edge0Z - edge1Z) * (pointX - edge1X) + (pointZ - edge1Z) * (edge1X - edge0X) >= 0)
						return 1;
				}
			}
			else if (absNormalX >= absNormalY && absNormalX >= abs(normal->z))
			{
				if (normal->x < 0)
				{
					if (edge0Y * (pointZ - edge0Z) - edge0Z * (pointY - edge0Y) >= 0 && edge1Z * pointY - edge1Y * pointZ >= 0
						&& (edge0Z - edge1Z) * (pointY - edge1Y) + (pointZ - edge1Z) * (edge1Y - edge0Y) >= 0)
						return 1;
				}
				else
				{
					if (edge0Z * pointY - edge0Y * pointZ >= 0 && edge1Y * (pointZ - edge1Z) - edge1Z * (pointY - edge1Y) >= 0
						&& (edge0Y - edge1Y) * (pointZ - edge0Z) + (edge1Z - edge0Z) * (pointY - edge0Y) >= 0)
						return 1;
				}
			}
			else
			{
				if (normal->z < 0)
				{
					if (edge0X * pointY - edge0Y * pointX >= 0 && edge1Y * (pointX - edge1X) - edge1X * (pointY - edge1Y) >= 0
						&& (edge0Y - edge1Y) * (pointX - edge0X) + (edge1X - edge0X) * (pointY - edge0Y) >= 0)
						return 1;
				}
				else
				{
					if (edge0Y * (pointX - edge0X) - edge0X * (pointY - edge0Y) >= 0 && edge1X * pointY - edge1Y * pointX >= 0
						&& (edge0X - edge1X) * (pointY - edge1Y) + (edge1Y - edge0Y) * (pointX - edge1X) >= 0)
						return 1;
				}
			}

			int32_t dot = edge1Z * pointZ + edge1Y * pointY + edge1X * pointX;
			int32_t lenSq = edge1Z * edge1Z + edge1Y * edge1Y + edge1X * edge1X;

			if (dot >= 0 && dot <= lenSq)
			{
				if ((lenSq & 0xFFFFFF00) == 0)
					lenSq = 256;

				scale = (dot << 6) / (lenSq >> 8);

				if ((pointZ - ((edge1Z * scale) >> 14)) * (pointZ - ((edge1Z * scale) >> 14))
						+ (pointY - ((edge1Y * scale) >> 14)) * (pointY - ((edge1Y * scale) >> 14))
						+ (pointX - ((edge1X * scale) >> 14)) * (pointX - ((edge1X * scale) >> 14))
					< 2 * (radius / 31) * (radius / 31))
					return 1;
			}

			lenSq = edge0Z * edge0Z + edge0Y * edge0Y + edge0X * edge0X;
			dot = edge0Z * pointZ + edge0Y * pointY + edge0X * pointX;

			if (dot >= 0 && dot <= lenSq)
			{
				if ((lenSq & 0xFFFFFF00) == 0)
					lenSq = 256;

				scale = (dot << 6) / (lenSq >> 8);

				if ((pointZ - ((edge0Z * scale) >> 14)) * (pointZ - ((edge0Z * scale) >> 14))
						+ (pointY - ((edge0Y * scale) >> 14)) * (pointY - ((edge0Y * scale) >> 14))
						+ (pointX - ((edge0X * scale) >> 14)) * (pointX - ((edge0X * scale) >> 14))
					< 2 * (radius / 31) * (radius / 31))
					return 1;
			}

			dot = (edge0X - edge1X) * (pointX - edge1X) + (edge0Z - edge1Z) * (pointZ - edge1Z) + (edge0Y - edge1Y) * (pointY - edge1Y);
			lenSq = (edge0X - edge1X) * (edge0X - edge1X) + (edge0Y - edge1Y) * (edge0Y - edge1Y) + (edge0Z - edge1Z) * (edge0Z - edge1Z);

			if (dot >= 0 && dot <= lenSq)
			{
				if ((lenSq & 0xFFFFFF00) == 0)
					lenSq = 256;

				scale = (dot << 6) / (lenSq >> 8);

				if ((pointX - (((edge0X - edge1X) * scale) >> 14) - edge1X) * (pointX - (((edge0X - edge1X) * scale) >> 14) - edge1X)
						+ (pointY - (((edge0Y - edge1Y) * scale) >> 14) - edge1Y) * (pointY - (((edge0Y - edge1Y) * scale) >> 14) - edge1Y)
						+ (pointZ - (((edge0Z - edge1Z) * scale) >> 14) - edge1Z) * (pointZ - (((edge0Z - edge1Z) * scale) >> 14) - edge1Z)
					< 2 * (radius / 31) * (radius / 31))
					return 1;
			}

			if (pointZ * pointZ + pointY * pointY + pointX * pointX < 2 * (radius / 31) * (radius / 31)
				|| (pointZ - edge0Z) * (pointZ - edge0Z) + (pointX - edge0X) * (pointX - edge0X) + (pointY - edge0Y) * (pointY - edge0Y)
					< 2 * (radius / 31) * (radius / 31))
				return 1;

			return (pointX - edge1X) * (pointX - edge1X) + (pointZ - edge1Z) * (pointZ - edge1Z) + (pointY - edge1Y) * (pointY - edge1Y)
				< 2 * (radius / 31) * (radius / 31);
		}

		// FUNCTION: TOY2 0x00490B90 [MATCHED]
		void CrossProduct3D(Vector3I* a, Vector3I* b, Vector3I* out)
		{
			Vector3I result;

			result.x = a->y * b->z - a->z * b->y;
			result.y = a->z * b->x - a->x * b->z;
			result.z = a->x * b->y - a->y * b->x;

			*out = result;
		}

		// FUNCTION: TOY2 0x0049F3C0 [MATCHED]
		BOOL IsWithinDistanceXZ(Vector3I* pos1, Vector3I* pos2, int32_t distance)
		{
			int32_t deltaX = (pos1->x - pos2->x) >> 8;
			int32_t deltaZ = (pos1->z - pos2->z) >> 8;

			return deltaX * deltaX + deltaZ * deltaZ < distance * distance;
		}

		// FUNCTION: TOY2 0x004A98D0 [MATCHED]
		D3DMATRIX* MatrixApplyScale(D3DMATRIX* matrix, Vector3F* scale)
		{
			matrix->_11 = matrix->_11 * scale->x;
			matrix->_12 = matrix->_12 * scale->x;
			matrix->_13 = matrix->_13 * scale->x;
			matrix->_21 = scale->y * matrix->_21;
			matrix->_22 = scale->y * matrix->_22;
			matrix->_23 = scale->y * matrix->_23;
			matrix->_31 = scale->z * matrix->_31;
			matrix->_32 = matrix->_32 * scale->z;
			matrix->_33 = matrix->_33 * scale->z;

			return matrix;
		}

		// FUNCTION: TOY2 0x004A9AA0
		D3DMATRIX* MatrixRotatePitch(D3DMATRIX* matrix, int32_t angle)
		{
			float cosAngle = Numerics::g_trigLUT[(angle + 0x4000) & 0xFFFF];
			float sinAngle = Numerics::g_trigLUT[angle & 0xFFFF];

			float r1 = matrix->_21;
			float r2 = matrix->_22;
			float r3 = matrix->_23;

			matrix->_21 = r1 * cosAngle + sinAngle * matrix->_31;
			matrix->_22 = r2 * cosAngle + sinAngle * matrix->_32;
			matrix->_23 = r3 * cosAngle + sinAngle * matrix->_33;
			matrix->_31 = cosAngle * matrix->_31 - r1 * sinAngle;
			matrix->_32 = cosAngle * matrix->_32 - r2 * sinAngle;
			matrix->_33 = cosAngle * matrix->_33 - r3 * sinAngle;

			return matrix;
		}

		// FUNCTION: TOY2 0x004A9CB0
		D3DMATRIX* MatrixRotateYaw(D3DMATRIX* matrix, int32_t angle)
		{
			float cosAngle = Numerics::g_trigLUT[(angle + 0x4000) & 0xFFFF];
			float sinAngle = Numerics::g_trigLUT[angle & 0xFFFF];

			float r1 = matrix->_11;
			float r0 = r1 * cosAngle - sinAngle * matrix->_31;
			float r2 = matrix->_12;
			float r3 = matrix->_13;

			matrix->_11 = r0;
			matrix->_12 = r2 * cosAngle - sinAngle * matrix->_32;
			matrix->_13 = r3 * cosAngle - sinAngle * matrix->_33;
			matrix->_31 = cosAngle * matrix->_31 + r1 * sinAngle;
			matrix->_32 = cosAngle * matrix->_32 + r2 * sinAngle;
			matrix->_33 = cosAngle * matrix->_33 + r3 * sinAngle;

			return matrix;
		}

		// FUNCTION: TOY2 0x004A9EC0 [MATCHED]
		D3DMATRIX* MatrixRotateRoll(D3DMATRIX* matrix, int32_t trigIndex)
		{
			float cosAngle = Numerics::g_trigLUT[(trigIndex + 0x4000) & 0xFFFF];
			float sinAngle = Numerics::g_trigLUT[trigIndex & 0xFFFF];

			float r1 = matrix->_11;
			float r2 = matrix->_12;
			float r3 = matrix->_13;

			matrix->_11 = sinAngle * matrix->_21 + r1 * cosAngle;
			matrix->_12 = sinAngle * matrix->_22 + r2 * cosAngle;
			matrix->_13 = sinAngle * matrix->_23 + r3 * cosAngle;
			matrix->_21 = cosAngle * matrix->_21 - r1 * sinAngle;
			matrix->_22 = cosAngle * matrix->_22 - r2 * sinAngle;
			matrix->_23 = cosAngle * matrix->_23 - r3 * sinAngle;

			return matrix;
		}

		// FUNCTION: TOY2 0x004A92E0 [MATCHED]
		double GetLength(Vector3F* v1) { return sqrt(v1->x * v1->x + v1->y * v1->y + v1->z * v1->z); }

		// FUNCTION: TOY2 0x004AB090
		void MatToQuat(float matrix[4][4], Quaternion* quat)
		{
			float tr, s, q[4];
			int32_t i, j, k;
			int32_t nxt[3] = { 1, 2, 0 };

			tr = matrix[0][0] + (matrix[1][1] + matrix[2][2]);

			if (tr > 0.0)
			{
				s = sqrt(tr + 1.0f);
				quat->w = s / 2.0f;
				s = 0.5f / s;
				quat->x = (matrix[1][2] - matrix[2][1]) * s;
				quat->y = (matrix[2][0] - matrix[0][2]) * s;
				quat->z = (matrix[0][1] - matrix[1][0]) * s;
			}
			else
			{
				i = 0;
				if (matrix[1][1] > matrix[0][0])
					i = 1;
				if (matrix[2][2] > matrix[i][i])
					i = 2;
				j = nxt[i];
				k = nxt[j];

				s = sqrt((matrix[i][i] - (matrix[j][j] + matrix[k][k])) + 1.0f);

				q[i] = s * 0.5f;

				if (s != 0.0f)
					s = 0.5f / s;

				q[3] = (matrix[j][k] - matrix[k][j]) * s;
				q[j] = (matrix[i][j] + matrix[j][i]) * s;
				q[k] = (matrix[i][k] + matrix[k][i]) * s;

				quat->x = q[0];
				quat->y = q[1];
				quat->z = q[2];
				quat->w = q[3];
			}
		}

		// FUNCTION: TOY2 0x004AB1E0 [MATCHED]
		void QuatToMatrix(Quaternion* quat, D3DMATRIX* matrix)
		{
			float ww = quat->w * quat->w;
			float xx = quat->x * quat->x;
			float yy = quat->y * quat->y;
			float zz = quat->z * quat->z;
			float xy = quat->x * quat->y * 2.0f;
			float xz = quat->x * quat->z * 2.0f;
			float xw = quat->x * quat->w * 2.0f;
			float yz = quat->y * quat->z * 2.0f;
			float yw = quat->y * quat->w * 2.0f;
			float zw = quat->z * quat->w * 2.0f;

			matrix->_11 = ww + xx - yy - zz;
			matrix->_21 = xy - zw;
			matrix->_31 = xz + yw;
			matrix->_41 = 0.0f;
			matrix->_12 = xy + zw;
			matrix->_22 = ww - xx + yy - zz;
			matrix->_32 = yz - xw;
			matrix->_42 = 0.0f;
			matrix->_13 = xz - yw;
			matrix->_23 = yz + xw;
			matrix->_33 = ww - xx - yy + zz;
			matrix->_43 = 0.0f;
			matrix->_14 = 0.0f;
			matrix->_24 = 0.0f;
			matrix->_34 = 0.0f;
			matrix->_44 = 1.0f;
		}

		// FUNCTION: TOY2 0x004AB2E0 [MATCHED]
		void QuatAdd(Quaternion* result, Quaternion* a, Quaternion* b)
		{
			result->w = a->w + b->w;
			result->x = a->x + b->x;
			result->y = a->y + b->y;
			result->z = a->z + b->z;
		}

		// FUNCTION: TOY2 0x004AB310 [MATCHED]
		void QuatSub(Quaternion* result, Quaternion* a, Quaternion* b)
		{
			result->w = a->w - b->w;
			result->x = a->x - b->x;
			result->y = a->y - b->y;
			result->z = a->z - b->z;
		}

		// FUNCTION: TOY2 0x004AB340 [MATCHED]
		void QuatMul(Quaternion* result, Quaternion* a, Quaternion* b)
		{
			result->w = (a->w * b->w) - (a->x * b->x) - (a->y * b->y) - (a->z * b->z);
			result->x = (a->w * b->x) + (a->x * b->w) + (a->y * b->z) - (a->z * b->y);
			result->y = (a->w * b->y) + (a->y * b->w) + (a->z * b->x) - (a->x * b->z);
			result->z = (a->w * b->z) + (a->z * b->w) + (a->x * b->y) - (a->y * b->x);
		}

		// FUNCTION: TOY2 0x004AB3D0 [MATCHED]
		void QuatInverse(Quaternion* result, Quaternion* quat)
		{
			float normSquared = quat->x * quat->x + quat->y * quat->y + quat->z * quat->z + quat->w * quat->w;
			float scale = 1.0f / normSquared;

			result->w = scale * quat->w;
			result->x = -scale * quat->x;
			result->y = -scale * quat->y;
			result->z = -scale * quat->z;
		}

		// FUNCTION: TOY2 0x004AB430 [MATCHED]
		void QuatNormalize(Quaternion* result, Quaternion* quat)
		{
			float normSquared = quat->x * quat->x + quat->y * quat->y + quat->z * quat->z + quat->w * quat->w;

			if (normSquared > 0.0)
			{
				float length = sqrt(normSquared);
				float scale = 1.0f / length;

				result->w = scale * quat->w;
				result->x = scale * quat->x;
				result->y = scale * quat->y;
				result->z = scale * quat->z;
			}
			else
			{
				result->x = quat->x;
				result->y = quat->y;
				result->z = quat->z;
				result->w = quat->w;
			}
		}

		// FUNCTION: TOY2 0x004AB4B0 [MATCHED]
		float QuatNormSquared(Quaternion* quat) { return quat->x * quat->x + quat->y * quat->y + quat->z * quat->z + quat->w * quat->w; }

		// FUNCTION: TOY2 0x004AB4E0 [MATCHED]
		void QuatLerp(Quaternion* result, Quaternion* from, Quaternion* to, float t)
		{
			result->x = (1.0f - t) * from->x + t * to->x;
			result->y = (1.0f - t) * from->y + t * to->y;
			result->z = (1.0f - t) * from->z + t * to->z;
			result->w = (1.0f - t) * from->w + t * to->w;
		}

		// FUNCTION: TOY2 0x004AB540
		void QuatSlerp(Quaternion* res, Quaternion* from, Quaternion* to, float t)
		{
			float to1[4];
			double omega, cosom, sinom, scale0, scale1;

			cosom = from->x * to->x + from->y * to->y + from->z * to->z + from->w * to->w;

			if (cosom < 0.0)
			{
				cosom = -cosom;
				to1[0] = -to->x;
				to1[1] = -to->y;
				to1[2] = -to->z;
				to1[3] = -to->w;
			}
			else
			{
				to1[0] = to->x;
				to1[1] = to->y;
				to1[2] = to->z;
				to1[3] = to->w;
			}

			if ((1.0 - cosom) > 0.0)
			{
				omega = acos(cosom);
				sinom = sin(omega);
				scale0 = sin((1.0 - t) * omega) / sinom;
				scale1 = sin(t * omega) / sinom;
			}
			else
			{
				scale0 = 1.0 - t;
				scale1 = t;
			}

			res->x = scale0 * from->x + scale1 * to1[0];
			res->y = scale0 * from->y + scale1 * to1[1];
			res->z = scale0 * from->z + scale1 * to1[2];
			res->w = scale0 * from->w + scale1 * to1[3];
		}
	}
}

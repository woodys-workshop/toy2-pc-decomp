#pragma once

#include "Common.h"
#include "Numerics.h"

#include <directx6/ddraw.h>
#include <directx6/d3d.h>

namespace Nu3D
{
	namespace Portal
	{
		struct AreaPortal;
	}

	namespace Camera
	{
		struct ActiveCameraTransform
		{
			Vector3I pos;
			Angles angles;
			int16_t roll;
			int16_t unkShort;
		};

		struct CameraInstance
		{
			D3DMATRIX matrix;
			float fovRadians;
			float aspectHOverW;
			float nearZ;
			float farZ;
			float viewportLR;
			float viewportTB;
			Vector3F scaleVector;
		};

		extern int32_t g_billboardYaw;
		extern CameraInstance* g_primaryCamera;
		extern CameraInstance g_secondaryCamera;
		extern ActiveCameraTransform g_lastCameraTransform;
		extern Vector3F g_cameraWorldPos;
		extern int32_t g_cameraPitch;
		extern int32_t g_cameraYaw;
		extern int32_t g_cameraRoll;
		extern float g_cameraNearZ;
		extern float g_cameraFarZ;
		extern float g_farLayerNearZ;
		extern float g_farLayerFarZ;
		extern int32_t g_farLayerRendering;
		extern int32_t g_nearLayerEnabled;
		extern float g_farLayerViewportLR;
		extern float g_cameraViewportTB;
		extern int16_t g_cameraTintBlue;
		extern int16_t g_cameraTintGreen;
		extern int16_t g_cameraTintRed;

		void SetTint(uint8_t blue, uint8_t green, uint8_t red, uint8_t fadeSpeed);
		void InitViewMatrixGlobals();
		void FadeToTargetTint();
		void ApplyTransformToCamera(ActiveCameraTransform* camera);
		void ToggleCameraSkew(int32_t effect);
		void SetSkewPhase(int32_t skewPhase);
		void SetObjectViewMatrix(Matrix3x3I16* matrix);
		void RebuildTransformPipeline();
		D3DMATRIX* GetProjectionMatrix();
		void TransformPointProjective(Vector3F* outPoints, Vector3F* inPoints, int32_t count, D3DMATRIX* localMatrix);
		void UnprojectPointsFromCamera(Vector3F* out, Vector3F* in, int32_t count);
		int32_t ClipViewToPortal(CameraInstance* camera, Portal::AreaPortal* portal);

		STATIC_ASSERT(sizeof(ActiveCameraTransform) == 0x14);
		STATIC_ASSERT(sizeof(CameraInstance) == 0x64);
	}

	void DrawDebugPortalOutlines(Vector3F* points, int32_t count);

	namespace ViewMatrix
	{
		Matrix3x3I16* Helper1(Matrix3x3I16* input, Matrix3x3I16* transform, Matrix3x3I16* output);
	}
}
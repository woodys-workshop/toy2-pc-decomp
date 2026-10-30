#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Toy2/Actor.h"

#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include <windows.h>

namespace Nu3D
{
	// TODO: Fix
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

		struct NineWords
		{
			Matrix3x3I16 rotation;
			int16_t unkWord10;
			int32_t unkInt1;
			int32_t unkInt2;
			int32_t unkInt3;
		};

		struct VectorState
		{
			int32_t unkInt1;
			int32_t vectorCount;
			int32_t unkInt3;
			int32_t unkInt4;
			int32_t unkInt5;
			int32_t unkInt6;
			int32_t unkInt7;
			Vector3F* vectorBuffer;
		};

		extern int32_t g_billboardYaw;
		extern CameraInstance* g_primaryCamera;
		extern CameraInstance g_secondaryCamera;
		extern ActiveCameraTransform g_lastCameraTransform;
		extern Vector3F g_cameraWorldPos;
		extern int32_t g_cameraPitch;
		extern int32_t g_cameraYaw;
		extern float g_cameraNearZ;
		extern float g_cameraFarZ;
		extern float g_farLayerNearZ;
		extern float g_farLayerFarZ;
		extern int32_t g_farLayerRendering;
		extern int32_t g_nearLayerEnabled;
		extern float g_farLayerViewportLR;
		extern float g_cameraViewportTB;
		extern int32_t g_viewportWidth;
		extern int32_t g_viewportHeight;
		extern int32_t g_screenWidth;
		extern int32_t g_screenHeight;
		extern int32_t g_screenCenterX;
		extern int32_t g_cachedScreenCenterX;
		extern int32_t g_scaleFactorX;
		extern int32_t g_viewportRight;
		extern int32_t g_viewportBottom;
		extern int32_t g_viewportLeft;
		extern int32_t g_viewportTop;
		extern int32_t g_viewportCenterX;
		extern int32_t g_viewportCenterY;
		extern int32_t g_viewportRightFixed10;
		extern int32_t g_viewportLeftFixed10;
		extern Angles g_cameraAngles;
		extern int16_t g_cameraRollCopy;
		extern int32_t g_cameraUnk_5546B4;
		extern int32_t g_cameraUnk_553028;
		extern int32_t g_cameraUnk_55302C;
		extern int32_t g_cameraUnk_554DC0;
		extern int16_t g_cameraTintBlue;
		extern int16_t g_cameraTintGreen;
		extern int16_t g_cameraTintRed;
		extern uint8_t g_targetTintBlue;
		extern uint8_t g_targetTintGreen;
		extern uint8_t g_targetTintRed;
		extern uint8_t g_targetTintFadeSpeed;
		extern int16_t g_tintBlend;

		void SetTint(uint8_t blue, uint8_t green, uint8_t red, uint8_t fadeSpeed);
		void InitViewMatrixGlobals();
		int32_t FadeToTargetTint();
		void ApplyTransformToCamera(ActiveCameraTransform* camera);
		void ToggleCameraSkew(int32_t effect);
		void SetSkewPhase(int32_t skewPhase);
		void SetObjectViewMatrix(Matrix3x3I16* matrix);
		void RebuildTransformPipeline();
		D3DMATRIX* GetProjectionMatrix();
		void TransformPointProjective(Vector3F* outPoints, Vector3F* inPoints, int32_t count, D3DMATRIX* localMatrix);
		void UnprojectPointsFromCamera(Vector3F* out, Vector3F* in, int32_t count);
		int32_t ClipViewToPortal(CameraInstance* camera, Portal::AreaPortal* portal);
		void WorldToView(Vector3I* a1, Vector3I* a2);
		BOOL LineOfSightCheck(Nu3D::Camera::ActiveCameraTransform* cam, Toy2::Actor::Toy2Actor* actor);
		void EnableCameraSkew();
		int32_t SetObjectViewPosition(Nu3D::Camera::NineWords* a1);
		int32_t ProjectQuad(Vector3I* a1, Vector3I* a2, Vector3I* a3, Vector3I* a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8);
		void DrawDebugPortalOutlines(VectorState* vectorState);

		STATIC_ASSERT(sizeof(ActiveCameraTransform) == 0x14);
		STATIC_ASSERT(sizeof(CameraInstance) == 0x64);
		STATIC_ASSERT(sizeof(NineWords) == 0x20);
		STATIC_ASSERT(sizeof(VectorState) == 0x20);
	}

	namespace ViewMatrix
	{
		Matrix3x3I16* Helper1(Matrix3x3I16* input, Matrix3x3I16* transform, Matrix3x3I16* output);
	}
}

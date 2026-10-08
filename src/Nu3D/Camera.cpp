#include "Nu3D/Camera.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderQueue.h"
#include "Renderer/RenderType.h"
#include "Renderer/Sprite.h"
#include "DrawingDevice.h"
#include "Nu3d/Math.h"
#include "SoftwareRenderer.h"
#include "Nu3D/Viewport.h"
#include "Nu3D/Portal.h"
#include "Toy2/Collision.h"
#include "Toy2/D3DApp.h"
#include "Random.h"
#include "CharacterLoader.h"
#include "RawLoader.h"

#include <MATH.H>
#include <STDLIB.H>
#include <STRING.H>

namespace Nu3D
{
	namespace Camera
	{
		// GLOBAL: TOY2 0x0054DE9C
		int16_t g_cameraTintBlue;

		// GLOBAL: TOY2 0x00554038
		int16_t g_cameraTintGreen;

		// GLOBAL: TOY2 0x0054DD6C
		int16_t g_cameraTintRed;

		// GLOBAL: TOY2 0x0052ADB8
		uint8_t g_targetTintBlue;

		// GLOBAL: TOY2 0x0052AD8C
		uint8_t g_targetTintGreen;

		// GLOBAL: TOY2 0x0052C820
		uint8_t g_targetTintRed;

		// GLOBAL: TOY2 0x00529E50
		uint8_t g_targetTintFadeSpeed;

		// GLOBAL: TOY2 0x00557A9C
		int16_t g_tintBlend;

		// GLOBAL: TOY2 0x00B623FC
		CameraInstance* g_primaryCamera;

		// GLOBAL: TOY2 0x00E4D970
		float g_projectionScaleX;

		// GLOBAL: TOY2 0x00E4D9A0
		float g_projectionScaleY;

		// GLOBAL: TOY2 0x00B223C8
		ActiveCameraTransform g_lastCameraTransform;

		// GLOBAL: TOY2 0x00E4D998
		int32_t g_cameraPitch;

		// GLOBAL: TOY2 0x00E4D99C
		int32_t g_cameraYaw;

		// GLOBAL: TOY2 0x00E4D994
		int32_t g_cameraRoll;

		// GLOBAL: TOY2 0x00E4D980
		Vector3F g_cameraWorldPos;

		// GLOBAL: TOY2 0x00B62404
		int32_t g_cameraSkewEffect;

		// GLOBAL: TOY2 0x00B62408
		int32_t g_cameraSkewPhase;

		// GLOBAL: TOY2 0x00508D0C
		float g_cameraFarZ = 48000.0;

		// GLOBAL: TOY2 0x00508D10
		float g_cameraNearZ = 60.0;

		// GLOBAL: TOY2 0x00508D08
		float g_farLayerNearZ = 40.0;

		// GLOBAL: TOY2 0x00508D04
		float g_farLayerFarZ = 48000.0;

		// GLOBAL: TOY2 0x00508D1C
		int32_t g_farLayerRendering = 1;

		// GLOBAL: TOY2 0x00508D20
		int32_t g_nearLayerEnabled = 1;

		// GLOBAL: TOY2 0x00508D14
		float g_farLayerViewportLR = 10000.0;

		// GLOBAL: TOY2 0x00508D18
		float g_cameraViewportTB = 12000.0;

		// GLOBAL: TOY2 0x005D2AE0
		int32_t g_objectViewMatrix[12];

		// GLOBAL: TOY2 0x005D2B10
		int32_t g_unk5D2B10;

		// GLOBAL: TOY2 0x005D2B14
		int32_t g_unk5D2B14;

		// GLOBAL: TOY2 0x005D2B18
		int32_t g_unk5D2B18;

		// GLOBAL: TOY2 0x009F6014
		int32_t g_billboardYaw;

		// GLOBAL: TOY2 0x00E4D880
		CameraInstance g_secondaryCamera;

		// GLOBAL: TOY2 0x00A4C410
		int32_t g_flippedCamera;

		// GLOBAL: TOY2 0x00E4D7E4
		D3DMATRIX g_flippedProjMatrix;

		// GLOBAL: TOY2 0x00A4C3D0
		D3DMATRIX g_projMatrix;

		// GLOBAL: TOY2 0x00A4C290
		D3DMATRIX g_outProjMatrix;

		// GLOBAL: TOY2 0x00A4C2D0
		D3DMATRIX g_clipNormMatrix;

		// GLOBAL: TOY2 0x00A4C310
		D3DMATRIX g_screenSpaceMatrix;

		// GLOBAL: TOY2 0x00E4D824
		D3DMATRIX g_currentViewMatrix;

		// GLOBAL: TOY2 0x00A4C350
		D3DMATRIX g_clipToScreenMatrix;

		// GLOBAL: TOY2 0x00A4C390
		D3DMATRIX g_tempCombinedMatrix;

		// GLOBAL: TOY2 0x00A4C250
		D3DMATRIX g_viewProjScreenMatrix;

		// GLOBAL: TOY2 0x00A4C190
		D3DMATRIX g_viewProjMatrix;

		// GLOBAL: TOY2 0x00A4C210
		D3DMATRIX g_invViewProjMatrix;

		// GLOBAL: TOY2 0x00A4C1D0
		D3DMATRIX g_invViewProjScreenMatrix;

		// GLOBAL: TOY2 0x00547EE0
		Angles g_cameraAngles;

		// GLOBAL: TOY2 0x00547EE4
		int16_t g_cameraRollCopy;

		// GLOBAL: TOY2 0x0054F630
		Vector3I g_prevCameraPos;

		// GLOBAL: TOY2 0x0054F63C
		uint32_t g_prevMathScratch224;

		// GLOBAL: TOY2 0x0054E058
		Matrix3x3I16 g_prevCameraMatrix;

		// GLOBAL: TOY2 0x005546B4
		int32_t g_cameraUnk_5546B4;

		// GLOBAL: TOY2 0x00553028
		int32_t g_cameraUnk_553028;

		// GLOBAL: TOY2 0x0055302C
		int32_t g_cameraUnk_55302C;

		// GLOBAL: TOY2 0x00554DC0
		int32_t g_cameraUnk_554DC0;

		// GLOBAL: TOY2 0x0054DEA8
		Angles g_cameraEuler;

		// GLOBAL: TOY2 0x0054DEAC
		uint32_t g_cameraEulerRoll;

		// GLOBAL: TOY2 0x0054DD68
		int32_t g_cameraViewDirty;

		// GLOBAL: TOY2 0x0054C100
		Matrix3x3I16 g_cameraViewMatrix;

		// GLOBAL: TOY2 0x00882798
		int32_t g_viewportWidth;

		// GLOBAL: TOY2 0x00882768
		int32_t g_screenWidth;

		// GLOBAL: TOY2 0x00500A24
		int32_t g_screenCenterX = 320;

		// GLOBAL: TOY2 0x0053CA58
		int32_t g_cachedScreenCenterX;

		// GLOBAL: TOY2 0x008828A0
		int32_t g_scaleFactorX;

		// GLOBAL: TOY2 0x008828CC
		int32_t g_viewportRight;

		// GLOBAL: TOY2 0x008828C4
		int32_t g_viewportBottom;

		// GLOBAL: TOY2 0x008828E0
		int32_t g_viewportLeft;

		// GLOBAL: TOY2 0x008828E4
		int32_t g_viewportTop;

		// GLOBAL: TOY2 0x008828D8
		int32_t g_viewportCenterX;

		// GLOBAL: TOY2 0x008828DC
		int32_t g_viewportCenterY;

		// GLOBAL: TOY2 0x00882784
		int32_t g_viewportRightFixed10;

		// GLOBAL: TOY2 0x008828BC
		int32_t g_viewportLeftFixed10;

		// GLOBAL: TOY2 0x008828AC
		int32_t g_screenHeight;

		// GLOBAL: TOY2 0x0055A0F0
		int32_t g_unk55A0F0;

		// GLOBAL: TOY2 0x008828E8
		int32_t g_viewportHeight;

		// GLOBAL: TOY2 0x00547EDC
		int16_t g_swayPhaseA;

		// GLOBAL: TOY2 0x0054F620
		int16_t g_swayPhaseB;

		// GLOBAL: TOY2 0x0054DEC0
		int16_t g_swayPhaseC;

		// GLOBAL: TOY2 0x0054DEB2
		int16_t g_swayHistoryIndex;

		// GLOBAL: TOY2 0x00553034
		int16_t g_swayTarget;

		// GLOBAL: TOY2 0x0054E04C
		int16_t g_swayVelocity;

		// GLOBAL: TOY2 0x00554F0C
		int16_t g_swayHistory[64];

		// GLOBAL: TOY2 0x00556FA0
		int16_t g_unk556FA0;

		// GLOBAL: TOY2 0x0054BEF8
		AnglesWithRoll g_swayAngleHistory[64];

		// GLOBAL: TOY2 0x00557AB0
		Matrix3x3I16 g_swayMat0;

		// GLOBAL: TOY2 0x00557AD0
		Matrix3x3I16 g_swayMat1;

		// GLOBAL: TOY2 0x00557AF0
		Matrix3x3I16 g_swayMat2;

		// GLOBAL: TOY2 0x00557B10
		Matrix3x3I16 g_swayMat3;

		// FUNCTION: TOY2 0x004A1BB0 [MATCHED]
		void SetTint(uint8_t blue, uint8_t green, uint8_t red, uint8_t fadeSpeed)
		{
			g_targetTintBlue = blue;
			g_targetTintGreen = green;
			g_targetTintRed = red;
			g_targetTintFadeSpeed = fadeSpeed;
			g_tintBlend = -1;
		}

		// FUNCTION: TOY2 0x0044DF90
		void InitViewMatrixGlobals()
		{
			memset(g_swayHistory, 0, sizeof(g_swayHistory));
			memset(g_swayAngleHistory, 0, sizeof(g_swayAngleHistory));

			Renderer::g_enableCinematicBars = 0;
			g_swayPhaseC = 0;
			g_swayPhaseB = 0;
			g_swayTarget = 0;
			g_swayVelocity = 0;
			g_swayPhaseA = 0;
			g_swayHistoryIndex = 0;

			g_unk556FA0 = 0;
		}

		// FUNCTION: TOY2 0x004A1BE0
		void FadeToTargetTint()
		{
			int32_t anyChannelChanged = 0;
			int32_t fadeStep = Renderer::g_frameDelta * g_targetTintFadeSpeed / 2;

			if (! fadeStep)
			{
				g_targetTintFadeSpeed = 0;
				return;
			}

			if (g_cameraTintBlue != g_targetTintBlue)
			{
				anyChannelChanged = 1;

				if (g_cameraTintBlue >= g_targetTintBlue)
				{
					g_cameraTintBlue -= fadeStep;

					if (g_cameraTintBlue >= g_targetTintBlue)
						goto LBL_FADE_GREEN;
				}
				else
				{
					g_cameraTintBlue += fadeStep;

					if (g_cameraTintBlue <= g_targetTintBlue)
						goto LBL_FADE_GREEN;
				}
				g_cameraTintBlue = g_targetTintBlue;
			}

		LBL_FADE_GREEN:

			if (g_cameraTintGreen == g_targetTintGreen)
				goto LBL_FADE_RED;

			anyChannelChanged = 1;

			if (g_cameraTintGreen >= g_targetTintGreen)
			{
				g_cameraTintGreen -= fadeStep;
				if (g_cameraTintGreen >= g_targetTintGreen)
					goto LBL_FADE_RED;
			}
			else
			{
				g_cameraTintGreen += fadeStep;

				if (g_cameraTintGreen <= g_targetTintGreen)
					goto LBL_FADE_RED;
			}

			g_cameraTintGreen = g_targetTintGreen;

		LBL_FADE_RED:

			if (g_cameraTintRed == g_targetTintRed)
			{
				if (anyChannelChanged)
					return;

				g_targetTintFadeSpeed = 0;
				return;
			}

			if (g_cameraTintRed >= g_targetTintRed)
			{
				g_cameraTintRed -= fadeStep;
				if (g_cameraTintRed < g_targetTintRed)
					g_cameraTintRed = g_targetTintRed;
			}
			else
			{
				g_cameraTintRed += fadeStep;
				if (g_cameraTintRed > g_targetTintRed)
					g_cameraTintRed = g_targetTintRed;
			}
		}

		// FUNCTION: TOY2 0x004BB890 [MATCHED]
		CameraInstance* Build()
		{
			CameraInstance* cameraInstance = (CameraInstance*)malloc(sizeof(CameraInstance));

			Math::BuildIdentityMatrix(&cameraInstance->matrix);
			cameraInstance->fovRadians = 0.75f;
			cameraInstance->aspectHOverW = 1.0f;
			cameraInstance->nearZ = 1.0f;
			cameraInstance->viewportLR = 0.0f;
			cameraInstance->farZ = 1000.0f;
			cameraInstance->viewportTB = FLT_MAX;
			cameraInstance->aspectHOverW = (float)DrawingDevice::GetDestHeight() / (float)DrawingDevice::GetDestWidth();
			cameraInstance->scaleVector.z = 1.0;
			cameraInstance->scaleVector.y = 1.0;
			cameraInstance->scaleVector.x = 1.0;

			return cameraInstance;
		}

		// FUNCTION: TOY2 0x004BB9D0 [MATCHED]
		void SetMatrixScaleVector(Vector3F* vector) { Math::g_matrixScaleVector = *vector; }

		// FUNCTION: TOY2 0x004BB9C0 [MATCHED]
		void ScaleMatrix(CameraInstance* camera) { Math::ScaleMatrix(&camera->matrix); }

		// FUNCTION: TOY2 0x004BBC50 [MATCHED]
		void CreateInverseMatrix(D3DMATRIX* output, D3DMATRIX* input) { Math::CreateInverseMatrix(output, input); }

		// FUNCTION: TOY2 0x004BBC00 [MATCHED]
		void BuildPerspectiveProjectionLH(D3DMATRIX* output, float fieldOfView, float aspectRatio, float nearClip, float farClip)
		{
			float depthScale = farClip / (farClip - nearClip);
			float halfFieldOfView = fieldOfView * 0.5f;
			float cotangent = (float)(cos(halfFieldOfView) / sin(halfFieldOfView));

			memset(output, 0, sizeof(*output));
			output->_34 = 1.0f;
			output->_11 = cotangent * aspectRatio;
			output->_22 = cotangent;
			output->_33 = depthScale;
			output->_43 = -depthScale * nearClip;
		}

		// FUNCTION: TOY2 0x004BBA10 [MATCHED]
		void RebuildTransformPipeline()
		{
			Viewport::GetClipNormMatrix(&g_clipNormMatrix);
			Viewport::GetScreenSpaceMatrix(&g_screenSpaceMatrix);

			Math::FullMatrixMultiply(&g_tempCombinedMatrix, &g_projMatrix, &g_outProjMatrix);
			Math::FullMatrixMultiply(&g_clipToScreenMatrix, &g_tempCombinedMatrix, &g_clipNormMatrix);
			Math::FullMatrixMultiply(&g_viewProjScreenMatrix, &g_clipToScreenMatrix, &g_screenSpaceMatrix);
			Math::FullMatrixMultiply(&g_viewProjMatrix, &g_clipNormMatrix, &g_screenSpaceMatrix);

			Math::InvertAffineMatrix(&g_invViewProjMatrix, &g_viewProjMatrix);
			Math::InvertAffineMatrix(&g_invViewProjScreenMatrix, &g_viewProjScreenMatrix);

			g_currentViewMatrix = g_clipToScreenMatrix;
		}

		// FUNCTION: TOY2 0x004BB850
		D3DMATRIX* GetProjectionMatrix() { return &g_projMatrix; }

		// FUNCTION: TOY2 0x004BBE10
		void TransformPointProjective(Vector3F* outPoints, Vector3F* inPoints, int32_t count, D3DMATRIX* localMatrix)
		{
			D3DMATRIX tempMatrix;
			Vector3F* linPoints = inPoints;
			Vector3F* curPoint = &inPoints[count];

			if (localMatrix)
				Nu3D::Math::MultiplyMatrix3x4(&tempMatrix, localMatrix, &g_viewProjScreenMatrix);
			else
				memcpy(&tempMatrix, &g_viewProjScreenMatrix, sizeof(tempMatrix));

			if (inPoints < curPoint)
			{
				do
					Nu3D::Math::ProjectPoint(outPoints++, linPoints++, &tempMatrix);
				while (linPoints < curPoint);
			}
		}

		// FUNCTION: TOY2 0x004BC080
		void UnprojectPointsFromCamera(Vector3F* out, Vector3F* in, int32_t count)
		{
			float m11 = g_outProjMatrix._11;
			float negM43 = -g_outProjMatrix._43;
			float m22 = g_outProjMatrix._22;
			float m33 = g_outProjMatrix._33;

			Vector3F* end = &in[count];

			for (; in < end; ++out)
			{
				Math::TransformPointByMatrix(out, in++, &g_invViewProjMatrix);

				float z = (negM43 + out->z) / m33;

				out->z = z;
				out->x = z * out->x / m11;
				out->y = z * out->y / m22;

				Math::TransformPointByMatrix(out, out, &g_secondaryCamera.matrix);
			}
		}

		// FUNCTION: TOY2 0x004BACF0
		int32_t ClipViewToPortal(CameraInstance* camera, Portal::AreaPortal* portal)
		{
			Vector3F posVector;
			Vector3F points[100];

			Math::GetPositionVector(&camera->matrix, &posVector);
			TransformPointProjective(points, portal->vertices, portal->vertexCount, 0);

			// screen space bounding box of the portal (x/y), and the nearest/farthest depth
			float minX = FLT_MAX;
			float minY = FLT_MAX;
			float minZ = FLT_MAX;
			float maxX = -FLT_MAX;
			float maxY = -FLT_MAX;
			float maxZ = -FLT_MAX;

			if (portal->vertexCount > 0)
			{
				for (int32_t i = 0; i < portal->vertexCount; i++)
				{
					if (points[i].x <= minX)
						minX = points[i].x;

					if (points[i].y <= minY)
						minY = points[i].y;

					if (points[i].z <= minZ)
						minZ = points[i].z;

					if (points[i].x >= maxX)
						maxX = points[i].x;

					if (points[i].y >= maxY)
						maxY = points[i].y;

					if (maxZ <= points[i].z)
						maxZ = points[i].z;
				}

				// the portal reaches in front of or behind the camera, so it can't be clipped, always visible
				if (maxZ > 1.0)
					return 1;

				if (minZ < 0.0)
					return 1;
			}

			// completely outside of the current clip rect
			if (minX > Viewport::g_viewClipRect.bottom || maxX < Viewport::g_viewClipRect.top || minY > Viewport::g_viewClipRect.right
				|| maxY < Viewport::g_viewClipRect.left)
			{
				return 0;
			}

			Vector3F edge1;
			Vector3F edge2;
			Vector3F normal;

			Math::VertexSubtract(&edge1, &points[1], &points[0]);
			Math::VertexSubtract(&edge2, &points[1], &points[2]);
			Math::VertexCrossProduct(&normal, &edge1, &edge2);

			// clamp the portal bounds to the clip rect
			if (minX <= Viewport::g_viewClipRect.top)
				minX = Viewport::g_viewClipRect.top;

			if (maxX >= Viewport::g_viewClipRect.bottom)
				maxX = Viewport::g_viewClipRect.bottom;

			if (minY <= Viewport::g_viewClipRect.left)
				minY = Viewport::g_viewClipRect.left;

			if (maxY >= Viewport::g_viewClipRect.right)
				maxY = Viewport::g_viewClipRect.right;

			// facing away from the camera, or nothing left after clamping
			if (normal.z > 0.0 || minX >= maxX || minY >= maxY)
				return 0;

			// turn the clamped screen rect back into 4 world space points
			points[0].x = minX;
			points[0].y = maxY;
			points[1].x = minX;
			points[1].y = minY;
			points[2].y = minY;
			points[3].x = maxX;
			points[2].x = maxX;
			points[0].z = 0.5;
			points[1].z = 0.5;
			points[2].z = 0.5;
			points[3].y = maxY;
			points[3].z = 0.5;

			UnprojectPointsFromCamera(points, points, 4);

			if (Viewport::g_fustrumInfo.unkInt6)
			{
				VectorState vecState;
				vecState.vectorBuffer = points;
				vecState.vectorCount = 4;

				DrawDebugPortalOutlines(&vecState);
			}

			// rebuild the four side planes of the frustum so it only contains what is visible through the portal
			Math::CalculatePlaneFromTriangle(&points[1], &points[0], &posVector, &Viewport::g_frustumData.leftPlane);
			Math::CalculatePlaneFromTriangle(&points[3], &points[2], &posVector, &Viewport::g_frustumData.rightPlane);
			Math::CalculatePlaneFromTriangle(&points[2], &points[1], &posVector, &Viewport::g_frustumData.topPlane);
			Math::CalculatePlaneFromTriangle(&points[0], &points[3], &posVector, &Viewport::g_frustumData.bottomPlane);

			Viewport::g_viewClipRect.top = minX;
			Viewport::g_viewClipRect.right = maxY;
			Viewport::g_viewClipRect.left = minY;
			Viewport::g_viewClipRect.bottom = maxX;

			if (Viewport::g_fustrumInfo.unkInt7)
			{
				Viewport::SetClipRect(minX, minY, maxX, maxY);
				RebuildTransformPipeline();
			}

			return 1;
		}

		// FUNCTION: TOY2 0x004BA420
		void CalculateFrustumPlanes(CameraInstance* camera)
		{
			Vector3F rightVector;
			Vector3F upVector;
			Vector3F forwardVector;
			Vector3F posVector;

			Math::GetRightVector(&camera->matrix, &rightVector);
			Math::GetUpVector(&camera->matrix, &upVector);
			Math::GetForwardVector(&camera->matrix, &forwardVector);
			Math::GetPositionVector(&camera->matrix, &posVector);

			Vector3F point;

			float tanHalfFov = tan(camera->fovRadians * 0.5);

			point.z = camera->nearZ;

			float nearScale = tanHalfFov * camera->nearZ;
			float nearHalfX = nearScale / camera->aspectHOverW;
			float nearHalfY = nearScale;
			float farScale = tanHalfFov * camera->farZ;

			point.y = nearHalfY;

			float farHalfX = farScale / camera->aspectHOverW;
			float farHalfY = farScale;

			float lrScale = tanHalfFov * camera->viewportLR;
			float lrHalfX = lrScale / camera->aspectHOverW;
			float lrHalfY = lrScale;
			float tbScale = tanHalfFov * camera->viewportTB;
			float tbHalfX = tbScale / camera->aspectHOverW;
			float tbHalfY = tbScale;
			float negHalfX = -nearHalfX;

			point.x = negHalfX;

			Vector3F nearTL;
			Math::TransformPointByMatrix(&nearTL, &point, &camera->matrix);
			float nearZ0 = camera->nearZ;

			point.x = nearHalfX;
			point.y = nearHalfY;
			point.z = nearZ0;
			Vector3F nearTR;
			Math::TransformPointByMatrix(&nearTR, &point, &camera->matrix);

			float nearZ1 = camera->nearZ;
			nearHalfY = -nearHalfY;
			point.y = nearHalfY;
			point.x = nearHalfX;
			point.z = nearZ1;
			Vector3F nearBR;
			Math::TransformPointByMatrix(&nearBR, &point, &camera->matrix);

			float nearZ2 = camera->nearZ;
			point.x = negHalfX;
			point.y = nearHalfY;
			point.z = nearZ2;
			Vector3F nearBL;
			Math::TransformPointByMatrix(&nearBL, &point, &camera->matrix);

			float farZ0 = camera->farZ;
			point.y = farHalfY;
			negHalfX = -farHalfX;
			point.x = negHalfX;
			point.z = farZ0;
			Vector3F farTL;
			Math::TransformPointByMatrix(&farTL, &point, &camera->matrix);

			float farZ1 = camera->farZ;
			point.x = farHalfX;
			point.y = farHalfY;
			point.z = farZ1;
			Vector3F farTR;
			Math::TransformPointByMatrix(&farTR, &point, &camera->matrix);

			float farZ2 = camera->farZ;
			point.x = farHalfX;
			farHalfY = -farHalfY;
			point.y = farHalfY;
			point.z = farZ2;
			Vector3F farBR;
			Math::TransformPointByMatrix(&farBR, &point, &camera->matrix);

			float farZ3 = camera->farZ;
			point.x = negHalfX;
			point.y = farHalfY;
			point.z = farZ3;
			Vector3F farBL;
			Math::TransformPointByMatrix(&farBL, &point, &camera->matrix);

			Vector3F lrTL;
			Vector3F lrTR;
			Vector3F lrBR;
			Vector3F lrBL;

			if (camera->viewportLR > camera->nearZ)
			{
				float lrZ0 = camera->viewportLR;
				point.y = lrHalfY;
				negHalfX = -lrHalfX;
				point.x = negHalfX;
				point.z = lrZ0;
				Math::TransformPointByMatrix(&lrTL, &point, &camera->matrix);

				float lrZ1 = camera->viewportLR;
				point.x = lrHalfX;
				point.y = lrHalfY;
				point.z = lrZ1;
				Math::TransformPointByMatrix(&lrTR, &point, &camera->matrix);

				float lrZ2 = camera->viewportLR;
				point.x = lrHalfX;
				lrHalfY = -lrHalfY;
				point.y = lrHalfY;
				point.z = lrZ2;
				Math::TransformPointByMatrix(&lrBR, &point, &camera->matrix);

				float lrZ3 = camera->viewportLR;
				point.x = negHalfX;
				point.y = lrHalfY;
				point.z = lrZ3;
				Math::TransformPointByMatrix(&lrBL, &point, &camera->matrix);
			}

			Vector3F tbTL;
			Vector3F tbTR;
			Vector3F tbBR;
			Vector3F tbBL;

			if (camera->viewportTB < camera->farZ)
			{
				float tbZ0 = camera->viewportTB;
				point.y = tbHalfY;
				negHalfX = -tbHalfX;
				point.x = negHalfX;
				point.z = tbZ0;
				Math::TransformPointByMatrix(&tbTL, &point, &camera->matrix);

				float tbZ1 = camera->viewportTB;
				point.x = tbHalfX;
				point.y = tbHalfY;
				point.z = tbZ1;
				Math::TransformPointByMatrix(&tbTR, &point, &camera->matrix);

				float tbZ2 = camera->viewportTB;
				point.x = tbHalfX;
				tbHalfY = -tbHalfY;
				point.y = tbHalfY;
				point.z = tbZ2;
				Math::TransformPointByMatrix(&tbBR, &point, &camera->matrix);

				float tbZ3 = camera->viewportTB;
				point.x = negHalfX;
				point.y = tbHalfY;
				point.z = tbZ3;
				Math::TransformPointByMatrix(&tbBL, &point, &camera->matrix);
			}

			if (Viewport::g_fustrumInfo.unkInt5)
			{
				Math::CalculatePlaneFromTriangle(&farTL, &farBL, &farBR, &Viewport::g_frustumData.farPlane);

				if (camera->viewportTB >= camera->farZ)
					Viewport::g_frustumData.topBottomPlane = Viewport::g_frustumData.farPlane;
				else
					Math::CalculatePlaneFromTriangle(&tbTL, &tbBL, &tbBR, &Viewport::g_frustumData.topBottomPlane);

				Math::CalculatePlaneFromTriangle(&nearBR, &nearBL, &nearTL, &Viewport::g_frustumData.nearPlane);

				if (camera->viewportLR <= camera->nearZ)
					Viewport::g_frustumData.leftRightPlane = Viewport::g_frustumData.nearPlane;
				else
					Math::CalculatePlaneFromTriangle(&lrBR, &lrBL, &lrTL, &Viewport::g_frustumData.leftRightPlane);

				Math::CalculatePlaneFromTriangle(&farTL, &nearBL, &farBL, &Viewport::g_frustumData.leftPlane);
				Math::CalculatePlaneFromTriangle(&farTR, &farBR, &nearTR, &Viewport::g_frustumData.rightPlane);
				Math::CalculatePlaneFromTriangle(&farTL, &farTR, &nearTL, &Viewport::g_frustumData.topPlane);
				Math::CalculatePlaneFromTriangle(&farBR, &farBL, &nearBL, &Viewport::g_frustumData.bottomPlane);
			}
			else
			{
				Math::CalculatePlaneFromTriangle(&farBR, &farBL, &farTL, &Viewport::g_frustumData.farPlane);

				if (camera->viewportTB >= camera->farZ)
					Viewport::g_frustumData.topBottomPlane = Viewport::g_frustumData.farPlane;
				else
					Math::CalculatePlaneFromTriangle(&tbBR, &tbBL, &tbTL, &Viewport::g_frustumData.topBottomPlane);

				Math::CalculatePlaneFromTriangle(&nearTL, &nearBL, &nearBR, &Viewport::g_frustumData.nearPlane);

				if (camera->viewportLR <= camera->nearZ)
					Viewport::g_frustumData.leftRightPlane = Viewport::g_frustumData.nearPlane;
				else
					Math::CalculatePlaneFromTriangle(&lrTL, &lrBL, &lrBR, &Viewport::g_frustumData.leftRightPlane);

				Math::CalculatePlaneFromTriangle(&farBL, &nearBL, &farTL, &Viewport::g_frustumData.leftPlane);
				Math::CalculatePlaneFromTriangle(&nearTR, &farBR, &farTR, &Viewport::g_frustumData.rightPlane);
				Math::CalculatePlaneFromTriangle(&nearTL, &farTR, &farTL, &Viewport::g_frustumData.topPlane);
				Math::CalculatePlaneFromTriangle(&nearBL, &farBL, &farBR, &Viewport::g_frustumData.bottomPlane);
			}

			TransformPointProjective(&point, &farBL, 1, 0);
			Viewport::g_viewClipRect.top = point.x;
			Viewport::g_viewClipRect.right = point.y;

			TransformPointProjective(&point, &farTR, 1, 0);
			Viewport::g_viewClipRect.bottom = point.x;
			Viewport::g_viewClipRect.left = point.y;

			Viewport::g_fustrumInfo.fustrumCount = 8;
		}

		// FUNCTION: TOY2 0x004BBAB0 [MATCHED]
		void ApplyCameraTransforms(CameraInstance* camera)
		{
			g_secondaryCamera = *camera;
			ScaleMatrix(&g_secondaryCamera);

			if (g_flippedCamera == 0) // perma 0
			{
				CreateInverseMatrix(&g_projMatrix, &g_secondaryCamera.matrix);
				Math::ScaleMatrixByVector(&g_projMatrix, &g_secondaryCamera.scaleVector);
			}
			else
			{
				D3DMATRIX identity;
				D3DMATRIX inv1;
				D3DMATRIX inv2;

				Math::BuildIdentityMatrix(&identity);
				identity._22 = -1.0;
				Math::CreateInverseMatrix(&inv1, &g_secondaryCamera.matrix);
				Math::CreateInverseMatrix(&inv2, &g_flippedProjMatrix);

				g_projMatrix = inv2;

				if (g_flippedCamera == 1)
					Math::MultiplyMatrix3x4(&g_projMatrix, &g_projMatrix, &identity);

				Math::MultiplyMatrix3x4(&g_projMatrix, &g_projMatrix, &g_flippedProjMatrix);
				Math::MultiplyMatrix3x4(&g_projMatrix, &g_projMatrix, &inv1);
			}

			DrawingDevice::SetViewTransform(&g_projMatrix);

			float fovRadians = g_secondaryCamera.fovRadians;
			float aspectHOverW = g_secondaryCamera.aspectHOverW;

			BuildPerspectiveProjectionLH(&g_outProjMatrix, fovRadians, aspectHOverW, g_secondaryCamera.nearZ, g_secondaryCamera.farZ);
			DrawingDevice::SetProjectionTransform(&g_outProjMatrix);
			RebuildTransformPipeline();
			CalculateFrustumPlanes(&g_secondaryCamera);
		}

		// FUNCTION: TOY2 0x004B68A0 [MATCHED]
		void SetBillboardYaw(int32_t billboardYaw) { g_billboardYaw = billboardYaw; }

		// FUNCTION: TOY2 0x004CE020
		void ToggleCameraSkew(int32_t effect) { g_cameraSkewEffect = effect; }

		// FUNCTION: TOY2 0x004CE030
		void SetSkewPhase(int32_t skewPhase) { g_cameraSkewPhase += skewPhase; }

		// FUNCTION: TOY2 0x004507F0
		void SetObjectViewMatrix(Matrix3x3I16* matrix)
		{
			// the 3x3 matrix is stored as three rows of four dwords (the last dword of every row is unused)
			g_objectViewMatrix[0] = matrix->m00;
			g_objectViewMatrix[1] = matrix->m01;
			g_objectViewMatrix[2] = matrix->m02;
			g_objectViewMatrix[4] = matrix->m10;
			g_objectViewMatrix[5] = matrix->m11;
			g_objectViewMatrix[6] = matrix->m12;
			g_objectViewMatrix[8] = matrix->m20;
			g_objectViewMatrix[9] = matrix->m21;
			g_objectViewMatrix[10] = matrix->m22;
		}

		// FUNCTION: TOY2 0x00446FC0
		void SetupViewMatrix(ActiveCameraTransform* camTransform)
		{
			// TODO: Cleanup method

			// the original treats the (shared) math scratch buffer as an array of dwords here
			uint32_t* mathScratch = (uint32_t*)Toy2::Collision::g_mathScratch;
			Matrix3x3I16* scratchMatrix = (Matrix3x3I16*)&mathScratch[229];

			AnglesWithRoll euler;
			euler.unkRoll = 0;

			g_cameraAngles.pitch = camTransform->angles.pitch;

			uint16_t negYaw = -camTransform->angles.yaw & 0xFFF;
			euler.angles.pitch = g_cameraAngles.pitch;

			g_cameraAngles.yaw = negYaw;

			int16_t roll = camTransform->roll;
			euler.angles.yaw = negYaw;

			// remember last frame's camera before overwriting it
			g_prevCameraPos = *(Vector3I*)&mathScratch[221];
			g_prevMathScratch224 = mathScratch[224];

			// NOTE: the original copies 32 bytes here (and below), which also drags along 14 bytes of
			// whatever sits after the matrix; only the 3x3 matrix itself is meaningful
			memcpy(&g_prevCameraMatrix, scratchMatrix, sizeof(g_prevCameraMatrix));

			g_cameraRollCopy = roll;
			euler.roll = roll;

			g_cameraUnk_5546B4 = 0;
			g_cameraUnk_553028 = 0;
			g_cameraUnk_55302C = 0;
			g_cameraUnk_554DC0 = 0;

			g_cameraEuler = euler.angles;
			g_cameraEulerRoll = *(uint32_t*)&euler.roll;

			g_cameraViewDirty = 1;

			Math::EulerToRotationMatrix(&euler, &g_cameraViewMatrix);

			if (D3DApp::g_renderMode == RENDERMODE_SOFTWARE)
			{
				g_cameraViewMatrix.m00 = (g_viewportWidth * g_cameraViewMatrix.m00) / 320;
				g_cameraViewMatrix.m01 = (g_viewportWidth * g_cameraViewMatrix.m01) / 320;
				g_cameraViewMatrix.m02 = (g_viewportWidth * g_cameraViewMatrix.m02) / 320;
				g_cameraViewMatrix.m10 = (g_viewportHeight * g_cameraViewMatrix.m10) / 240;
				g_cameraViewMatrix.m11 = (g_viewportHeight * g_cameraViewMatrix.m11) / 240;
				g_cameraViewMatrix.m12 = g_viewportHeight * g_cameraViewMatrix.m12 / 240;
			}

			*(Vector3I*)&mathScratch[221] = camTransform->pos;
			mathScratch[225] = camTransform->pos.x / 32;
			int16_t phaseB = g_swayPhaseB + 24;
			mathScratch[226] = camTransform->pos.y / 32;
			int32_t posZ = camTransform->pos.z;
			g_swayPhaseA += 32;
			mathScratch[227] = posZ / 32;
			memcpy(scratchMatrix, &g_cameraViewMatrix, sizeof(g_cameraViewMatrix));

			// camera sway: a 64 entry history of a spring-driven value, plus a wobble angle per entry
			int32_t sinA = Numerics::g_sinCosLUT[g_swayPhaseA & 0xFFF];
			g_swayPhaseC += 13;
			g_swayPhaseB += 24;
			uint8_t newIndex = (g_swayHistoryIndex - 1) & 63;
			g_swayHistoryIndex = newIndex;
			int32_t prevIndex = (newIndex + 1) & 63;
			int16_t prevHistory = g_swayHistory[prevIndex];
			int16_t* prevHistoryPtr = &g_swayHistory[prevIndex];
			int16_t velocity;
			if (sinA / 16 + g_swayTarget - prevHistory <= 0)
				velocity = 62 * g_swayVelocity / 64 - 16;
			else
				velocity = 62 * g_swayVelocity / 64 + 16;
			int32_t phaseC = g_swayPhaseC;
			g_swayVelocity = velocity;
			int32_t curIndex = newIndex & 0x3F;
			g_swayHistory[curIndex] = prevHistory + velocity;
			int32_t phaseCIndex = (phaseC >> 5) & 0x7F;
			AnglesWithRoll* swayAngles = &g_swayAngleHistory[curIndex];
			int16_t swayRoll;
			if ((prevHistory + velocity) < 0)
			{
				swayAngles->angles.pitch = Numerics::g_sinCosLUT[phaseB & 0xFFF] / 1024 - Numerics::g_sinCosLUT[(phaseCIndex + 1024) & 0xFFF] / 512;
				swayRoll = Numerics::g_sinCosLUT[(phaseB + 1024) & 0xFFF] / 1024 - Numerics::g_sinCosLUT[phaseCIndex] / 512;
			}
			else
			{
				swayAngles->angles.pitch = Numerics::g_sinCosLUT[phaseB & 0xFFF] / 1024 + Numerics::g_sinCosLUT[(phaseCIndex + 1024) & 0xFFF] / 512;
				swayRoll = Numerics::g_sinCosLUT[phaseCIndex] / 512 + Numerics::g_sinCosLUT[(phaseB + 1024) & 0xFFF] / 1024;
			}
			swayAngles->roll = swayRoll;
			if (abs(sinA / 16 + g_swayTarget - *prevHistoryPtr) < 128)
			{
				int16_t randValue = *g_randDatBufferPtr++;
				g_swayTarget = 24 * randValue + 2048;
			}

			// Sway matrix 0 (history slot +0)
			Math::EulerToRotationMatrix(swayAngles, &g_swayMat0);
			int32_t blend0 = abs(g_swayHistory[g_swayHistoryIndex & 0x3F]);
			int32_t row0Scale0 = g_swayHistory[(g_swayHistoryIndex + 16) & 0x3F] / 16 + 3968;
			g_swayMat0.m00 = row0Scale0 * (blend0 * g_swayMat0.m00 / 4096 - blend0 + 4096) / 4096;
			g_swayMat0.m01 = row0Scale0 * (blend0 * g_swayMat0.m01 / 4096) / 4096;
			g_swayMat0.m02 = row0Scale0 * (blend0 * g_swayMat0.m02 / 4096) / 4096;
			g_swayMat0.m10 = blend0 * g_swayMat0.m10 / 4096;
			int32_t row2Scale0 = g_swayHistory[g_swayHistoryIndex & 0x3F] / 16 + 3968;
			g_swayMat0.m11 = blend0 * g_swayMat0.m11 / 4096 - blend0 + 4096;
			g_swayMat0.m12 = blend0 * g_swayMat0.m12 / 4096;
			g_swayMat0.m20 = row2Scale0 * (blend0 * g_swayMat0.m20 / 4096) / 4096;
			g_swayMat0.m21 = row2Scale0 * (blend0 * g_swayMat0.m21 / 4096) / 4096;
			g_swayMat0.m22 = row2Scale0 * (blend0 * g_swayMat0.m22 / 4096 - blend0 + 4096) / 4096;
			ViewMatrix::Helper1(scratchMatrix, &g_swayMat0, &g_swayMat0);

			// Sway matrix 1 (history slot -16)
			Math::EulerToRotationMatrix(&g_swayAngleHistory[(g_swayHistoryIndex - 16) & 0x3F], &g_swayMat1);
			int32_t blend1 = abs(g_swayHistory[(g_swayHistoryIndex - 16) & 0x3F]);
			int32_t row0Scale1 = g_swayHistory[g_swayHistoryIndex & 0x3F] / 16 + 3968;
			int32_t row2Scale1 = g_swayHistory[(g_swayHistoryIndex - 16) & 0x3F] / 16 + 3968;
			g_swayMat1.m00 = row0Scale1 * (blend1 * g_swayMat1.m00 / 4096 - blend1 + 4096) / 4096;
			g_swayMat1.m01 = row0Scale1 * (blend1 * g_swayMat1.m01 / 4096) / 4096;
			g_swayMat1.m02 = row0Scale1 * (blend1 * g_swayMat1.m02 / 4096) / 4096;
			g_swayMat1.m10 = blend1 * g_swayMat1.m10 / 4096;
			g_swayMat1.m11 = blend1 * g_swayMat1.m11 / 4096 - blend1 + 4096;
			g_swayMat1.m12 = blend1 * g_swayMat1.m12 / 4096;
			g_swayMat1.m20 = row2Scale1 * (blend1 * g_swayMat1.m20 / 4096) / 4096;
			g_swayMat1.m21 = row2Scale1 * (blend1 * g_swayMat1.m21 / 4096) / 4096;
			g_swayMat1.m22 = row2Scale1 * (blend1 * g_swayMat1.m22 / 4096 - blend1 + 4096) / 4096;
			ViewMatrix::Helper1(scratchMatrix, &g_swayMat1, &g_swayMat1);

			// Sway matrix 2 (history slot -32)
			Math::EulerToRotationMatrix(&g_swayAngleHistory[(g_swayHistoryIndex - 32) & 0x3F], &g_swayMat2);
			int32_t blend2 = abs(g_swayHistory[(g_swayHistoryIndex - 32) & 0x3F]);
			int32_t row0Scale2 = g_swayHistory[(g_swayHistoryIndex - 16) & 0x3F] / 16 + 3968;
			g_swayMat2.m00 = row0Scale2 * (blend2 * g_swayMat2.m00 / 4096 - blend2 + 4096) / 4096;
			int32_t row2Scale2 = g_swayHistory[(g_swayHistoryIndex - 32) & 0x3F] / 16 + 3968;
			g_swayMat2.m01 = row0Scale2 * (blend2 * g_swayMat2.m01 / 4096) / 4096;
			g_swayMat2.m02 = row0Scale2 * (blend2 * g_swayMat2.m02 / 4096) / 4096;
			g_swayMat2.m10 = blend2 * g_swayMat2.m10 / 4096;
			g_swayMat2.m11 = blend2 * g_swayMat2.m11 / 4096 - blend2 + 4096;
			g_swayMat2.m12 = blend2 * g_swayMat2.m12 / 4096;
			g_swayMat2.m20 = row2Scale2 * (blend2 * g_swayMat2.m20 / 4096) / 4096;
			g_swayMat2.m21 = row2Scale2 * (blend2 * g_swayMat2.m21 / 4096) / 4096;
			g_swayMat2.m22 = row2Scale2 * (blend2 * g_swayMat2.m22 / 4096 - blend2 + 4096) / 4096;
			ViewMatrix::Helper1(scratchMatrix, &g_swayMat2, &g_swayMat2);

			// Sway matrix 3 (history slot +16)
			Math::EulerToRotationMatrix(&g_swayAngleHistory[(g_swayHistoryIndex + 16) & 0x3F], &g_swayMat3);
			int32_t blend3 = abs(g_swayHistory[(g_swayHistoryIndex + 16) & 0x3F]);
			int32_t row0Scale3 = g_swayHistory[(g_swayHistoryIndex - 32) & 0x3F] / 16 + 3968;
			g_swayMat3.m00 = row0Scale3 * (blend3 * g_swayMat3.m00 / 4096 - blend3 + 4096) / 4096;
			g_swayMat3.m01 = row0Scale3 * (blend3 * g_swayMat3.m01 / 4096) / 4096;
			g_swayMat3.m02 = row0Scale3 * (blend3 * g_swayMat3.m02 / 4096) / 4096;
			g_swayMat3.m10 = blend3 * g_swayMat3.m10 / 4096;
			int32_t row2Scale3 = g_swayHistory[(g_swayHistoryIndex + 16) & 0x3F] / 16 + 3968;
			g_swayMat3.m11 = blend3 * g_swayMat3.m11 / 4096 - blend3 + 4096;
			g_swayMat3.m12 = blend3 * g_swayMat3.m12 / 4096;
			g_swayMat3.m20 = row2Scale3 * (blend3 * g_swayMat3.m20 / 4096) / 4096;
			g_swayMat3.m21 = row2Scale3 * (blend3 * g_swayMat3.m21 / 4096) / 4096;
			g_swayMat3.m22 = row2Scale3 * (blend3 * g_swayMat3.m22 / 4096 - blend3 + 4096) / 4096;
			ViewMatrix::Helper1(scratchMatrix, &g_swayMat3, &g_swayMat3);
		}

		// FUNCTION: TOY2 0x004CE050 [MATCHED]
		void ApplyTransformToCamera(ActiveCameraTransform* camera)
		{
			if (! g_primaryCamera)
			{
				Vector3F scale = { 1.0f, -1.0f, 1.0f };

				SetMatrixScaleVector(&scale);

				g_primaryCamera = Build();
				g_primaryCamera->aspectHOverW = 0.75f;
#ifdef APPLY_FIXES
				// EXPERIMENT: use the real aspect ratio instead of the fixed 4:3, fixes the stretched 16:9 image
				g_primaryCamera->aspectHOverW = (float)DrawingDevice::GetDestHeight() / (float)DrawingDevice::GetDestWidth();
#endif
				g_primaryCamera->fovRadians = 1.25f;
				g_primaryCamera->farZ = 32768.0f;
				g_primaryCamera->nearZ = 50.0f;

				float halfFieldOfView = g_primaryCamera->fovRadians * 0.5f;
				float cotangent = (float)(cos(halfFieldOfView) / sin(halfFieldOfView));

				g_projectionScaleX = DrawingDevice::GetDestWidth() * g_primaryCamera->aspectHOverW * cotangent;
				g_projectionScaleY = DrawingDevice::GetDestHeight() * cotangent;
			}

			if (! camera)
				return;

			g_lastCameraTransform = *camera;

			g_cameraPitch = -camera->angles.pitch * 16;
			g_cameraYaw = camera->angles.yaw << 4;
			g_cameraRoll = camera->roll << 4;

			Math::ApplyRotateXFromLut(&g_primaryCamera->matrix, g_cameraPitch);
			Math::RotateYFromLut(&g_primaryCamera->matrix, g_cameraYaw);
			Math::RotateZFromLut(&g_primaryCamera->matrix, g_cameraRoll);

			g_cameraWorldPos.x = camera->pos.x * 0.03125f;
			g_cameraWorldPos.y = camera->pos.y * 0.03125f;
			g_cameraWorldPos.z = camera->pos.z * 0.03125f;

			Math::AddWorldSpaceTransform(&g_primaryCamera->matrix, &g_cameraWorldPos);

			if (! g_cameraSkewEffect || Renderer::GetIsSoftwareRendering())
			{
				g_primaryCamera->scaleVector.z = 1.0f;
				g_primaryCamera->scaleVector.y = 1.0f;
				g_primaryCamera->scaleVector.x = 1.0f;
			}
			else
			{
				g_primaryCamera->scaleVector.x = Numerics::g_trigLUT[g_cameraSkewPhase & 0xFFFF] * 0.05f + 1.0f;
				g_primaryCamera->scaleVector.y = Numerics::g_trigLUT[(g_cameraSkewPhase - 0x8000) & 0xFFFF] * 0.05f + 1.0f;
				g_primaryCamera->scaleVector.z = Numerics::g_trigLUT[(g_cameraSkewPhase * 2) & 0xFFFF] * 0.05f + 1.0f;
			}

			ApplyCameraTransforms(g_primaryCamera);
			SetBillboardYaw(g_cameraYaw);
			SetupViewMatrix(camera);

			SoftwareRenderer::SetCameraNearFarZ(g_cameraNearZ, g_cameraFarZ);
		}

		// FUNCTION: TOY2 0x0043D9E0
		void WorldToView(Vector3I* a1, Vector3I* a2)
		{
			// the position is packed as 3 shorts
			int32_t pointX = (int16_t)a1->x;
			int32_t pointY = (int16_t)(a1->x >> 16);
			int32_t pointZ = (int16_t)a1->y;

			int32_t viewX = (pointX * g_objectViewMatrix[0] + pointY * g_objectViewMatrix[1] + pointZ * g_objectViewMatrix[2]) / 4096 + g_unk5D2B10;
			int32_t viewY = (pointX * g_objectViewMatrix[4] + pointY * g_objectViewMatrix[5] + pointZ * g_objectViewMatrix[6]) / 4096 + g_unk5D2B14;
			int32_t viewZ = (pointX * g_objectViewMatrix[8] + pointY * g_objectViewMatrix[9] + pointZ * g_objectViewMatrix[10]) / 4096 + g_unk5D2B18;

			a2->x = (int16_t)viewX;
			a2->y = (int16_t)viewY;
			a2->z = (int16_t)viewZ;
		}

		// FUNCTION: TOY2 0x00448F00
		BOOL LineOfSightCheck(Nu3D::Camera::ActiveCameraTransform* cam, Toy2::Actor::Toy2Actor* actor)
		{
			BOOL result = 0;

			if (*(int16_t*)CharacterLoader::g_unk547CD4[actor->creatureId])
			{
				RawLoader::CreatureListRam* creatureRam = actor->creatureRam;

				int32_t y = actor->boundingOffset.y + 32 * creatureRam->pos.y - cam->pos.y;
				int32_t z = 32 * creatureRam->pos.z - cam->pos.z;
				int32_t x = 32 * creatureRam->pos.x - cam->pos.x;

				int32_t boundingSphereRadius = actor->boundingSphereRadius;

				int32_t depth = (x * g_cameraViewMatrix.m20 + y * g_cameraViewMatrix.m21 + z * g_cameraViewMatrix.m22) / 0x20000;

				if (depth > -boundingSphereRadius)
				{
					int32_t scaledDepth = (depth << 8) / g_screenCenterX;

					if (scaledDepth
							> (abs((x * g_cameraViewMatrix.m00 + y * g_cameraViewMatrix.m01 + z * g_cameraViewMatrix.m02) / 0x20000) - boundingSphereRadius)
						&& scaledDepth
							> (abs((x * g_cameraViewMatrix.m10 + y * g_cameraViewMatrix.m11 + z * g_cameraViewMatrix.m12) / 0x20000) - boundingSphereRadius))
					{
						return 1;
					}
				}
			}

			return result;
		}

		// FUNCTION: TOY2 0x0044F820
		void EnableCameraSkew() { Nu3D::Camera::ToggleCameraSkew(1); }

		// FUNCTION: TOY2 0x00450850
		int32_t SetObjectViewPosition(Nu3D::Camera::NineWords* a1)
		{
			g_unk5D2B10 = a1->unkInt1;
			g_unk5D2B14 = a1->unkInt2;
			g_unk5D2B18 = a1->unkInt3;

			return g_unk5D2B18;
		}

		// FUNCTION: TOY2 0x00451280
		int32_t ProjectQuad(Vector3I* a1, Vector3I* a2, Vector3I* a3, Vector3I* a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8)
		{
			Vector3I* vertices[4];
			int16_t viewX[4];
			int16_t viewY[4];
			int16_t viewZ[4];
			int16_t screenY[4];
			int32_t i;

			vertices[0] = a1;
			vertices[1] = a2;
			vertices[2] = a3;
			vertices[3] = a4;

			// the vertices are packed as 3 shorts, they are moved by the object view matrix and the position
			for (i = 0; i < 4; i++)
			{
				int32_t pointX = (int16_t)vertices[i]->x;
				int32_t pointY = (int16_t)(vertices[i]->x >> 16);
				int32_t pointZ = (int16_t)vertices[i]->y;

				viewX[i] = (pointX * g_objectViewMatrix[0] + pointY * g_objectViewMatrix[1] + pointZ * g_objectViewMatrix[2]) / 4096 + g_unk5D2B10;
				viewY[i] = (pointX * g_objectViewMatrix[4] + pointY * g_objectViewMatrix[5] + pointZ * g_objectViewMatrix[6]) / 4096 + g_unk5D2B14;
				viewZ[i] = (pointX * g_objectViewMatrix[8] + pointY * g_objectViewMatrix[9] + pointZ * g_objectViewMatrix[10]) / 4096 + g_unk5D2B18;
			}

			for (i = 0; i < 4; i++)
			{
				g_unk55A0F0 = viewZ[i];

				if (! viewZ[i])
					g_unk55A0F0 = 1;

				screenY[i] = (int32_t)(((double)viewY[i] * g_projectionScaleY / g_unk55A0F0 + g_screenHeight) * 0.5);
			}

			// $TODO: the outputs are written at different offsets, the original does this as well
			*(int32_t*)a5 = screenY[0];
			*(int32_t*)(a6 + 4) = screenY[1];
			*(int32_t*)(a7 + 8) = screenY[2];
			*(int32_t*)(a8 + 12) = screenY[3];

			return (viewZ[0] + viewZ[1] + viewZ[2] + viewZ[3]) / 16;
		}

		// FUNCTION: TOY2 0x004B9B90
		void DrawDebugPortalOutlines(VectorState* vectorState)
		{
			int32_t oldFlags = Renderer::SetAdditionalRenderFlags(Renderer::RENDER_Z);
			int32_t curVectorIdx = 1;

			if (vectorState->vectorCount > 1)
			{
				int32_t nextVectorIdx = 1;

				do
				{
					RGBA color;
					color.r = -1;
					color.g = -1;
					color.b = -1;
					color.a = -1;

					Renderer::Sprite::QueueType10(&vectorState->vectorBuffer[nextVectorIdx - 1], &vectorState->vectorBuffer[nextVectorIdx], color);

					++curVectorIdx;
					++nextVectorIdx;
				} while (curVectorIdx < vectorState->vectorCount);
			}

			Renderer::SetAdditionalRenderFlags(oldFlags);
		}
	}

	namespace ViewMatrix
	{
		// TODO: Find a better place for this
		// FUNCTION: TOY2 0x00450AC0
		Matrix3x3I16* Helper1(Matrix3x3I16* input, Matrix3x3I16* transform, Matrix3x3I16* output)
		{
			// multiplies the 3x3 matrix in "input" by "transform" (12-bit fixed point), every result
			// is computed before anything is written, so output is allowed to alias transform
			Matrix3x3I16 result;

			int16_t* in = (int16_t*)input;
			int16_t* out = (int16_t*)&result;

			for (int32_t row = 0; row < 3; row++)
			{
				int32_t x = in[row * 3 + 0];
				int32_t y = in[row * 3 + 1];
				int32_t z = in[row * 3 + 2];

				out[row * 3 + 0] = (z * transform->m20 + y * transform->m10 + x * transform->m00) >> 12;
				out[row * 3 + 1] = (z * transform->m21 + y * transform->m11 + x * transform->m01) >> 12;
				out[row * 3 + 2] = (z * transform->m22 + y * transform->m12 + x * transform->m02) >> 12;
			}

			*output = result;
			return output;
		}
	}
}
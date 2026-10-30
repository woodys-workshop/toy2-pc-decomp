#include "Nu3D/Viewport.h"
#include "DrawingDevice.h"
#include "Renderer/Renderer.h"
#include "SoftwareRenderer.h"
#include "Nu3D/Math.h"
#include "Nu3D/Camera.h"

#include <stddef.h>
#include <STDIO.H>

namespace Nu3D
{
	namespace Viewport
	{
		// GLOBAL: TOY2 0x00884640
		D3DVIEWPORT2 g_pendingViewport;

		// GLOBAL: TOY2 0x00884680
		D3DVIEWPORT2 g_currentViewport;

		// GLOBAL: TOY2 0x00884638
		int32_t g_renderWidth;

		// GLOBAL: TOY2 0x00884634
		int32_t g_renderHeight;

		// GLOBAL: TOY2 0x008846AC
		float g_clipOffsetX;

		// GLOBAL: TOY2 0x008846B0
		float g_clipOffsetY;

		// GLOBAL: TOY2 0x00884630
		int32_t g_clipNormMatrixDirty;

		// GLOBAL: TOY2 0x008846B4
		int32_t g_screenSpaceMatrixDirty;

		// GLOBAL: TOY2 0x00884670
		float g_currentViewportX;

		// GLOBAL: TOY2 0x00884674
		float g_currentViewportY;

		// GLOBAL: TOY2 0x008845A0
		float g_minViewportY;

		// GLOBAL: TOY2 0x008845A4
		float g_minViewportX;

		// GLOBAL: TOY2 0x00884678
		float g_currentViewportWidth;

		// GLOBAL: TOY2 0x0088466C
		float g_currentViewportHeight;

		// GLOBAL: TOY2 0x00E4D958
		int32_t g_viewportChangeCount;

		// GLOBAL: TOY2 0x008845A8
		float g_drawDeviceWidth;

		// GLOBAL: TOY2 0x008845AC
		float g_drawDeviceHeight;

		// GLOBAL: TOY2 0x009F6224
		FustrumInfo g_fustrumInfo;

		// GLOBAL: TOY2 0x00E4D8E8
		ViewportRectAlt g_viewClipRect;

		// GLOBAL: TOY2 0x008845B0
		D3DMATRIX g_clipNormMatrixCache;

		// GLOBAL: TOY2 0x008845F0
		D3DMATRIX g_screenSpaceMatrixCache;

		// GLOBAL: TOY2 0x009F6030
		FrustumData g_frustumData;

		// FUNCTION: TOY2 0x004B55D0
		void Init()
		{
			DrawingDevice::BuildFreshViewport(&g_pendingViewport);

			memcpy(&g_currentViewport, &g_pendingViewport, sizeof(g_currentViewport));

			g_renderWidth = DrawingDevice::GetWidth();
			g_renderHeight = DrawingDevice::GetHeight();

			g_clipOffsetY = 0.0;
			g_clipOffsetX = 0.0;
			g_clipNormMatrixDirty = 1;
			g_screenSpaceMatrixDirty = 1;
		}

		// FUNCTION: TOY2 0x004B5630
		void Reset()
		{
			DrawingDevice::SetViewport(&g_currentViewport);

			memcpy(&g_pendingViewport, &g_currentViewport, sizeof(g_pendingViewport));

			g_currentViewportX = g_currentViewport.dwX;
			g_currentViewportY = g_currentViewport.dwY;

			g_minViewportY = 0.0;

			g_currentViewportWidth = g_currentViewport.dwWidth;

			g_minViewportX = 0.0;
			g_clipOffsetY = 0.0;
			g_clipOffsetX = 0.0;
			g_clipNormMatrixDirty = 1;

			g_currentViewportHeight = g_currentViewport.dwHeight;

			g_screenSpaceMatrixDirty = 1;
			g_viewportChangeCount = 0;

			g_drawDeviceWidth = g_renderWidth;
			g_drawDeviceHeight = g_renderHeight;
		}

		// FUNCTION: TOY2 0x004BAC40
		void GetViewClipRect(ViewportRect* output)
		{
			output->top = g_viewClipRect.top;
			output->bottom = g_viewClipRect.left;
			output->left = g_viewClipRect.bottom;
			output->right = g_viewClipRect.right;
		}

		// FUNCTION: TOY2 0x004B5550
		void GetBaseViewportRect(ViewportRectAlt* output)
		{
			output->left = g_minViewportX;
			memcpy(&output->top, &g_minViewportY, sizeof(float));
			output->right = g_drawDeviceWidth + g_minViewportX - 1.0f;
			output->bottom = g_drawDeviceHeight + g_minViewportY - 1.0f;
		}

		// FUNCTION: TOY2 0x004B5590 [MATCHED]
		void GetViewportRect(ViewportRectAlt* output)
		{
			output->left = g_currentViewportX;
			output->top = g_currentViewportY;
			output->right = g_currentViewportWidth + g_currentViewportX - 1.0f;
			output->bottom = g_currentViewportHeight + g_currentViewportY - 1.0f;
		}

		// FUNCTION: TOY2 0x004BA3A0
		void SetViewClipRect()
		{
			ViewportRectAlt spriteRect;

			if (Renderer::g_isSoftwareRendering)
			{
				g_viewClipRect.top = SoftwareRenderer::g_leftOffset;
				g_viewClipRect.left = SoftwareRenderer::g_topOffset;
				g_viewClipRect.bottom = SoftwareRenderer::g_rightOffset;
				g_viewClipRect.right = SoftwareRenderer::g_bottomOffset;
			}
			else
			{
				GetViewportRect(&spriteRect);
				g_viewClipRect.top = spriteRect.left;
				g_viewClipRect.left = spriteRect.top;
				g_viewClipRect.bottom = spriteRect.right;
				g_viewClipRect.right = spriteRect.bottom;
			}
		}

		// FUNCTION: TOY2 0x004B5AD0
		void BuildClipNormMatrix()
		{
			g_clipNormMatrixCache._12 = 0.0;
			g_clipNormMatrixCache._13 = 0.0;
			g_clipNormMatrixCache._14 = 0.0;
			g_clipNormMatrixCache._21 = 0.0;
			g_clipNormMatrixCache._23 = 0.0;
			g_clipNormMatrixCache._24 = 0.0;
			g_clipNormMatrixCache._31 = 0.0;
			g_clipNormMatrixCache._32 = 0.0;
			g_clipNormMatrixCache._34 = 0.0;
			g_clipNormMatrixCache._44 = 1.0;

			g_clipNormMatrixDirty = 0;

			g_clipNormMatrixCache._11 = 2.0 / g_pendingViewport.dvClipWidth;
			g_clipNormMatrixCache._22 = 2.0 / g_pendingViewport.dvClipHeight;
			g_clipNormMatrixCache._33 = 1.0 / g_pendingViewport.dvMaxZ - g_pendingViewport.dvMinZ;

			g_clipNormMatrixCache._41 =
				-1.0 - (g_pendingViewport.dvClipX / g_pendingViewport.dvClipWidth + g_pendingViewport.dvClipX / g_pendingViewport.dvClipWidth);

			g_clipNormMatrixCache._42 =
				1.0 - (g_pendingViewport.dvClipY / g_pendingViewport.dvClipHeight + g_pendingViewport.dvClipY / g_pendingViewport.dvClipHeight);

			g_clipNormMatrixCache._43 = -(g_pendingViewport.dvMinZ / g_pendingViewport.dvMaxZ - g_pendingViewport.dvMinZ);
		}

		// FUNCTION: TOY2 0x004B5AA0
		void GetClipNormMatrix(D3DMATRIX* out)
		{
			if (g_clipNormMatrixDirty)
				BuildClipNormMatrix();

			*out = g_clipNormMatrixCache;
		}

		// FUNCTION: TOY2 0x004B5C00
		void BuildScreenSpaceMatrix()
		{
			float width = (float)g_pendingViewport.dwWidth;
			float height = (float)g_pendingViewport.dwHeight;

			memset(&g_screenSpaceMatrixCache, 0, sizeof(g_screenSpaceMatrixCache));
			g_screenSpaceMatrixCache._11 = width * 0.5f;
			g_screenSpaceMatrixCache._22 = height * -0.5f;
			g_screenSpaceMatrixCache._33 = 1.0f;
			g_screenSpaceMatrixCache._41 = (float)g_pendingViewport.dwX + width * 0.5f;

			// Materialize unsigned Y so VC6 cannot use signed DWORD FIADD.
			volatile double viewportY = g_pendingViewport.dwY;
			g_screenSpaceMatrixCache._42 = viewportY + height * 0.5f;
			g_screenSpaceMatrixCache._44 = 1.0f;
			g_screenSpaceMatrixDirty = 0;
		}

		// FUNCTION: TOY2 0x004B5BD0
		void GetScreenSpaceMatrix(D3DMATRIX* out)
		{
			if (g_screenSpaceMatrixDirty)
				BuildScreenSpaceMatrix();

			*out = g_screenSpaceMatrixCache;
		}

		// FUNCTION: TOY2 0x004BABE0
		void CacheViewport(ViewportCache* viewportCache)
		{
			// The prefix spans two separate global objects.
			memcpy(&viewportCache->frustum, &g_frustumData, sizeof(g_frustumData));
			memcpy(viewportCache->frustumInfo, &g_fustrumInfo, sizeof(viewportCache->frustumInfo));

			// Copy float representations, including NaN payloads.
			memcpy(&viewportCache->clipLeft, &g_viewClipRect.top, sizeof(float));
			memcpy(&viewportCache->clipTop, &g_viewClipRect.left, sizeof(float));
			memcpy(&viewportCache->clipRight, &g_viewClipRect.bottom, sizeof(float));
			memcpy(&viewportCache->clipBottom, &g_viewClipRect.right, sizeof(float));
			viewportCache->frustumCount = g_fustrumInfo.fustrumCount;
		}

		// FUNCTION: TOY2 0x004B5700
		void SetClipRect(float left, float top, float right, float bottom)
		{
			float width = right - left + 1.0;
			float height = bottom - top + 1.0;

			if (left != g_currentViewportX || top != g_currentViewportY || width != g_currentViewportWidth || height != g_currentViewportHeight)
			{
				if (left <= g_minViewportX)
					left = g_minViewportX;

				if (top <= g_minViewportY)
					top = g_minViewportY;

				double widthLimit = g_drawDeviceWidth - 1.0;

				if (width >= widthLimit)
					width = widthLimit;

				double heightLimit = g_drawDeviceHeight - 1.0;

				if (height >= heightLimit)
					height = heightLimit;

				if (width >= 0.0 && height >= 0.0)
				{
					g_currentViewportX = left;
					g_currentViewportY = top;
					g_currentViewportWidth = width;
					g_currentViewportHeight = height;

					g_pendingViewport.dwX = left;
					g_pendingViewport.dwY = top;
					g_pendingViewport.dwWidth = width;
					g_pendingViewport.dwHeight = height;

					double clipXOffset = (left - g_minViewportX) / g_drawDeviceWidth;
					g_pendingViewport.dvClipX = clipXOffset + clipXOffset - 1.0 - g_clipOffsetX;

					double clipYOffset = (top - g_minViewportY) / g_drawDeviceHeight;
					g_pendingViewport.dvClipY = 1.0 - (clipYOffset + clipYOffset) - g_clipOffsetY;

					g_pendingViewport.dvClipWidth = width / g_drawDeviceWidth + width / g_drawDeviceWidth;
					g_pendingViewport.dvClipHeight = height / g_drawDeviceHeight + height / g_drawDeviceHeight;

					DrawingDevice::SetViewport(&g_pendingViewport);

					g_clipNormMatrixDirty = 1;
					g_screenSpaceMatrixDirty = 1;

					++g_viewportChangeCount;
				}
			}
		}

		// FUNCTION: TOY2 0x004B5A00
		void SetDepthRange(float minZ, float maxZ)
		{
			memcpy(&g_pendingViewport.dvMinZ, &minZ, sizeof(float));
			memcpy(&g_pendingViewport.dvMaxZ, &maxZ, sizeof(float));
			DrawingDevice::SetViewport(&g_pendingViewport);
			g_clipNormMatrixDirty = 1;
			g_screenSpaceMatrixDirty = 1;
		}

		// FUNCTION: TOY2 0x004B5A30
		void SetClipOffset(float offsetX, float offsetY)
		{
			memcpy(&g_clipOffsetX, &offsetX, sizeof(float));
			// Keep the old X bits in the consumed argument slot.
			memcpy(&offsetX, &g_currentViewportX, sizeof(float));
			g_currentViewportX += 1.0f;
			memcpy(&g_clipOffsetY, &offsetY, sizeof(float));
			SetClipRect(offsetX, g_currentViewportY, g_currentViewportWidth + offsetX - 1.0f, g_currentViewportHeight + g_currentViewportY - 1.0f);
		}

		// FUNCTION: TOY2 0x004BAC70
		void RestoreViewportCache(ViewportCache* viewportCache)
		{
			memcpy(&g_frustumData, &viewportCache->frustum, sizeof(g_frustumData));
			memcpy(&g_fustrumInfo, viewportCache->frustumInfo, sizeof(viewportCache->frustumInfo));

			memcpy(&g_viewClipRect.top, &viewportCache->clipLeft, sizeof(float));
			memcpy(&g_viewClipRect.left, &viewportCache->clipTop, sizeof(float));
			memcpy(&g_viewClipRect.bottom, &viewportCache->clipRight, sizeof(float));
			memcpy(&g_viewClipRect.right, &viewportCache->clipBottom, sizeof(float));
			g_fustrumInfo.fustrumCount = viewportCache->frustumCount;

			if (g_fustrumInfo.unkInt7)
				SetClipRect(g_viewClipRect.top, g_viewClipRect.left, g_viewClipRect.bottom, g_viewClipRect.right);

			Camera::RebuildTransformPipeline();
		}
	}
}

// TODO: Fix, should e in its own class
namespace Nu3D
{
	namespace Frustum
	{
		// FUNCTION: TOY2 0x004BA1F0
		int32_t TestSphereAllPlanes(Vector3F* point, float distance)
		{
			int32_t index = 2;
			int32_t result = 0;

			// the first two planes (near/far) are skipped, every plane after that uses two bits of the result:
			// 01 = sphere is fully outside this plane, 10 = sphere is intersecting this plane
			if (Viewport::g_fustrumInfo.fustrumCount > 2)
			{
				for (Plane* plane = &Viewport::g_frustumData.leftRightPlane;; ++plane)
				{
					float signedDistanceToPlane = Math::GetSignedDistanceToPlane(point, plane);

					if (signedDistanceToPlane > distance)
						break;

					if (signedDistanceToPlane > -distance)
						result |= 2 << (2 * index);

					if (++index >= Viewport::g_fustrumInfo.fustrumCount)
						return result;
				}

				return (1 << (2 * index)) | result;
			}

			return result;
		}

		// FUNCTION: TOY2 0x004BA270
		int32_t TestSphereAllPlanesAlt(Vector3F* point, float distance)
		{
			int32_t index = 2;
			int32_t result = 0;

			if (Viewport::g_fustrumInfo.fustrumCount > 2)
			{
				for (Plane* plane = &Viewport::g_frustumData.leftRightPlane;; ++plane)
				{
					float signedDistanceToPlane = Math::GetSignedDistanceToPlane(point, plane);

					if (signedDistanceToPlane > distance)
						break;

					if (signedDistanceToPlane > -distance)
						result |= 2 << (2 * index);

					if (++index >= Viewport::g_fustrumInfo.fustrumCount)
						return result;
				}

				return (1 << (2 * index)) | result;
			}

			return result;
		}

		// FUNCTION: TOY2 0x004BA2F0
		int32_t TestSphereDepthPlanes(Vector3F* point, float distance)
		{
			int32_t index = 1;
			int32_t result = 0;

			if (Viewport::g_fustrumInfo.fustrumCount <= 1)
				return result;

			// same as above but starting at the far plane, and skipping the left/right and top/bottom planes
			for (Plane* plane = &Viewport::g_frustumData.farPlane;; ++plane)
			{
				if (plane != &Viewport::g_frustumData.leftRightPlane && plane != &Viewport::g_frustumData.topBottomPlane)
				{
					float signedDistanceToPlane = Math::GetSignedDistanceToPlane(point, plane);

					if (signedDistanceToPlane > distance)
						break;

					if (signedDistanceToPlane > -distance)
						result |= 2 << (2 * index);
				}

				if (++index >= Viewport::g_fustrumInfo.fustrumCount)
					return result;
			}

			return (1 << (2 * index)) | result;
		}

		// FUNCTION: TOY2 0x004BA380
		int32_t ExchangeWinding(int32_t winding)
		{
			int32_t previous = Viewport::g_fustrumInfo.unkInt5;
			Viewport::g_fustrumInfo.unkInt5 = winding;
			return previous;
		}

		// FUNCTION: TOY2 0x004BA390
		int32_t GetWinding() { return Viewport::g_fustrumInfo.unkInt5; }
	}
}

namespace Nu3D
{
	STATIC_ASSERT(offsetof(Viewport::ViewportCache, frustum) == 0x0);
	STATIC_ASSERT(offsetof(Viewport::ViewportCache, frustumInfo) == 0x1F4);
	STATIC_ASSERT(offsetof(Viewport::ViewportCache, clipLeft) == 0x200);
	STATIC_ASSERT(offsetof(Viewport::ViewportCache, clipRight) == 0x204);
	STATIC_ASSERT(offsetof(Viewport::ViewportCache, clipTop) == 0x208);
	STATIC_ASSERT(offsetof(Viewport::ViewportCache, clipBottom) == 0x20C);
	STATIC_ASSERT(offsetof(Viewport::ViewportCache, frustumCount) == 0x210);
}

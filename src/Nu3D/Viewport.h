#pragma once

#include "Common.h"
#include "Numerics.h"

#include <directx6/ddraw.h>
#include <directx6/d3d.h>

namespace Nu3D
{
	namespace Viewport
	{
		struct ViewportRect
		{
			float top;
			float bottom;
			float left;
			float right;
		};

		struct ViewportRectAlt
		{
			float left;
			float top;
			float right;
			float bottom;
		};

		struct FustrumInfo
		{
			int32_t unkInt1; // 0x0
			int32_t unkInt2; // 0x4
			int32_t unkInt3; // 0x8
			int32_t fustrumCount; // 0xC
			int32_t unkInt5; // 0x10
			int32_t unkInt6; // 0x14
			int32_t unkInt7; // 0x18
		};

		struct FrustumData
		{
			Plane nearPlane;
			Plane farPlane;
			Plane leftRightPlane;
			Plane topBottomPlane;
			Plane leftPlane;
			Plane rightPlane;
			Plane topPlane;
			Plane bottomPlane;
			uint8_t unusedData[372];
		};

		// snapshot of the frustum planes + clip rect, used to restore the view after rendering through a portal
		struct ViewportCache
		{
			FrustumData frustum;
			int32_t frustumInfo[3];
			ViewportRectAlt clipRect;
			int32_t frustumCount;
		};

		extern FustrumInfo g_fustrumInfo;
		extern FrustumData g_frustumData;
		extern ViewportRectAlt g_viewClipRect;

		void Init();
		void Reset();
		void SetViewClipRect();
        void GetViewClipRect(ViewportRect* output);
		void GetClipNormMatrix(D3DMATRIX* out);
		void GetScreenSpaceMatrix(D3DMATRIX* out);
		void SetClipRect(float left, float top, float right, float bottom);
		void CacheViewport(ViewportCache* viewportCache);
		void RestoreViewportCache(ViewportCache* viewportCache);
	}

	namespace Frustum
	{
		int32_t TestSphereAllPlanes(Vector3F* point, float distance);
		int32_t TestSphereAllPlanesAlt(Vector3F* point, float distance);
		int32_t TestSphereDepthPlanes(Vector3F* point, float distance);
	}

	STATIC_ASSERT(sizeof(Viewport::ViewportRect) == 0x10);
	STATIC_ASSERT(sizeof(Viewport::FustrumInfo) == 0x1C);
	STATIC_ASSERT(sizeof(Viewport::FrustumData) == 0x1F4);
	STATIC_ASSERT(sizeof(Viewport::ViewportCache) == 0x214);
}
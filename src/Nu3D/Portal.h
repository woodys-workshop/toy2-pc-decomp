#pragma once

#include "Nu3D/Nu3D.h"
#include "Nu3D/Link.h"
#include "Common.h"
#include <windows.h>

namespace NGNLoader
{
	struct NGNImage;
}

namespace Nu3D
{
	// TODO: Fix
	namespace Area
	{
		void ResetPortalStates(NGNLoader::NGNImage* ngnImage);
	}

	namespace Portal
	{
		struct ScalerEntry
		{
			ScalerEntry* next;
			Nu3D::Link::DynamicScaler* scaler;
		};

		struct AreaPortal
		{
			int32_t portalId;
			int32_t vertexCount;
			Vector3F center;
			float radiusSquared;
			float radius;
			Vector3F* vertices;

			static void CalculateBoundingSphere(AreaPortal* portal);
			static ScalerEntry* AllocScalerEntry(NGNLoader::NGNImage* image);
			static int32_t BuildScalerEntry(NGNLoader::NGNImage* image, int32_t index, Link::DynamicScaler* scaler);
		};

		struct PortalState
		{
			PortalState* next;
			AreaPortal* portal;
			int32_t sourceAreaIdx;
			int32_t targetAreaIdx;
			int32_t visited;
		};

		struct PortalBucket
		{
			ScalerEntry* scalerHead;
			PortalState* portalStateHead;
		};

		struct PortalHashTable
		{
			PortalBucket buckets[64];
		};

		extern uint8_t g_visibleAreaFlags[64];
		extern int32_t g_renderedScalerCount;
		extern int32_t g_portalRecursionDepth;
		extern uint8_t g_minPortalRecursionDepth;
		extern int32_t g_hiddenShapeId;
		extern int32_t g_lastRenderableAreaIndex;
		extern Link::DynamicScaler* g_portalMirrorScaler;
		extern int32_t g_mirrorRenderToggle;

		void ClearVisibleAreaFlags();
		void MarkAllAreasVisible();
	}

	namespace Area
	{
		void RenderBucketThroughPortals(NGNLoader::NGNImage* ngnImage, int32_t bucketIdx, int32_t gscaleType, Portal::PortalState* rotEntry);
	}

	namespace Portal
	{
		STATIC_ASSERT(sizeof(AreaPortal) == 0x20);
		STATIC_ASSERT(sizeof(PortalState) == 0x14);
		STATIC_ASSERT(sizeof(PortalBucket) == 0x8);
		STATIC_ASSERT(sizeof(ScalerEntry) == 0x8);
		STATIC_ASSERT(sizeof(PortalHashTable) == 0x200);

		int32_t IsAreaVisible(uint8_t index);
	}
}
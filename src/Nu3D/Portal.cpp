#pragma once

#include "Nu3D/Portal.h"
#include "Nu3D/Math.h"
#include "Numerics.h"
#include "NGNLoader/NGNLoader.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Viewport.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderQueue.h"
#include <cmath>

namespace Nu3D
{
	// GLOBAL: TOY2 0x00A4C414
	uint8_t Portal::g_visibleAreaFlags[64];

	// GLOBAL: TOY2 0x00A4CC60
	int32_t Portal::g_renderedScalerCount;

	// GLOBAL: TOY2 0x005088A4
	int32_t Portal::g_portalRecursionDepth = -1;

	// GLOBAL: TOY2 0x00A4C458
	uint8_t Portal::g_minPortalRecursionDepth;

	// GLOBAL: TOY2 0x005088A8
	int32_t Portal::g_hiddenShapeId = -1;

	// GLOBAL: TOY2 0x005088AC
	int32_t Portal::g_lastRenderableAreaIndex = 64;

	// GLOBAL: TOY2 0x00E4D7C0
	Link::DynamicScaler* Portal::g_portalMirrorScaler;

	// GLOBAL: TOY2 0x00A4CC68
	int32_t Portal::g_mirrorRenderToggle;

	// FUNCTION: TOY2 0x004B33B0
	void Portal::AreaPortal::CalculateBoundingSphere(Portal::AreaPortal* portal)
	{
		float z = 3.4028235e38;
		float y = 3.4028235e38;
		float x = 3.4028235e38;

		int32_t vertexCount = portal->vertexCount;

		Vector3F vector;
		vector.z = -3.4028235e38;
		vector.y = -3.4028235e38;
		vector.x = -3.4028235e38;

		if (vertexCount > 0)
		{
			Vector3F* vertices = portal->vertices;
			int32_t verticesLeft = vertexCount;

			do
			{
				if (vertices->x <= x)
					x = vertices->x;

				if (vertices->x >= vector.x)
					vector.x = vertices->x;

				if (vertices->y <= y)
					y = vertices->y;

				if (vertices->y >= vector.y)
					vector.y = vertices->y;

				if (z >= vertices->z)
					z = vertices->z;

				if (vertices->z >= vector.z)
					vector.z = vertices->z;

				++vertices;
				--verticesLeft;

			} while (verticesLeft);
		}

		portal->center.x = (vector.x + x) * 0.5;
		portal->center.y = (vector.y + y) * 0.5;
		portal->center.z = (vector.z + z) * 0.5;

		Nu3D::Math::VertexSubtract(&vector, &vector, &portal->center);

		double radSqrt = vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;

		// TODO: naming is wrong here
		portal->radius = radSqrt;
		portal->radiusSquared = sqrt(radSqrt);
	}

	// FUNCTION: TOY2 0x004BC3A0
	Portal::ScalerEntry* Portal::AreaPortal::AllocScalerEntry(NGNLoader::NGNImage* image)
	{
		if (image->scalerEntryCount < image->maxScalerEntries)
		{
			ScalerEntry* pool = image->scalerEntryPool;

			if (! pool)
				return 0;

			ScalerEntry* newAlloc = &pool[image->scalerEntryCount];

			image->scalerEntryCount = image->scalerEntryCount + 1;

			return newAlloc;
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004BC360
	int32_t Portal::AreaPortal::BuildScalerEntry(NGNLoader::NGNImage* image, int32_t index, Link::DynamicScaler* scaler)
	{
		Portal::ScalerEntry* head = Portal::AreaPortal::AllocScalerEntry(image);

		if (! head)
			return 0;

		head->scaler = scaler;
		head->next = image->portalHashTable->buckets[index].scalerHead;

		image->portalHashTable->buckets[index].scalerHead = head;

		return 1;
	}

	// FUNCTION: TOY2 0x004BC120
	void Portal::ClearVisibleAreaFlags()
	{
		g_visibleAreaFlags[0] = 1;
		memset(&g_visibleAreaFlags[1], 0, sizeof(g_visibleAreaFlags) - 1);
	}

	// FUNCTION: TOY2 0x004BC140
	void Portal::MarkAllAreasVisible() { memset(g_visibleAreaFlags, 1, sizeof(g_visibleAreaFlags)); }

	// FUNCTION: TOY2 0x004BC170
	void Area::ResetPortalStates(NGNLoader::NGNImage* ngnImage)
	{
		for (int32_t i = 0; i < ngnImage->portalEntryCount; i++)
			ngnImage->portalStatePool[i].visited = 0;
	}

	// FUNCTION: TOY2 0x004BC460
	void Area::RenderBucketThroughPortals(NGNLoader::NGNImage* ngnImage, int32_t bucketIdx, int32_t gscaleType, Portal::PortalState* rotEntry)
	{
		// Renders everything in an area, then recursively through any portal of that area that is visible.
		// Every portal narrows the view (frustum and clip rect) down to what can be seen through it.

		int32_t areaIdx = bucketIdx;

		if (bucketIdx >= 64)
			areaIdx = Portal::g_lastRenderableAreaIndex;

		if (areaIdx >= 64)
			return;

		Viewport::ViewportCache viewportCache;
		Viewport::CacheViewport(&viewportCache);

		int32_t sourceAreaIdx = -1;

		if (rotEntry)
		{
			if (rotEntry->visited)
				return;

			sourceAreaIdx = rotEntry->sourceAreaIdx;

			if (! Camera::ClipViewToPortal(&Camera::g_secondaryCamera, rotEntry->portal))
				return;

			rotEntry->visited = 1;
		}

		Vector3F cameraPos;
		Math::GetPositionVector(&Camera::g_secondaryCamera.matrix, &cameraPos);

		if (Portal::g_minPortalRecursionDepth <= ++Portal::g_portalRecursionDepth)
		{
			Portal::g_visibleAreaFlags[areaIdx] = 1;

			for (Portal::ScalerEntry* scalerEntry = ngnImage->portalHashTable->buckets[areaIdx].scalerHead; scalerEntry; scalerEntry = scalerEntry->next)
			{
				Link::DynamicScaler* scaler = scalerEntry->scaler;

				if ((scaler->flags & 1) == 0 && scaler->gscaleType == gscaleType)
				{
					Vector3F center;
					Vector3F offset;

					Math::VertexAdd(&center, &scaler->translation, &scaler->boundsCenterWorld);
					Math::VertexSubtract(&offset, &center, &cameraPos);

					double renderDistance = offset.z * offset.z + offset.y * offset.y + offset.x * offset.x;

					// layer 1 is everything beyond the far distance, layer 0 is everything inside of the near distance
					bool inRange;

					if (scaler->gscaleType == 1)
						inRange = renderDistance > Renderer::g_farRenderDistance;
					else
						inRange = renderDistance < Renderer::g_nearRenderDistance;

					if (inRange)
					{
						int32_t testResult = Frustum::TestSphereAllPlanesAlt(&center, ngnImage->primitives[scaler->shapeId]->boundSphereRadius);

						if ((testResult & 0x55555555) == 0)
						{
							Link::DynamicScaler* entryScaler = scalerEntry->scaler;

							if ((entryScaler != Portal::g_portalMirrorScaler || Portal::g_mirrorRenderToggle)
								&& entryScaler->shapeId != Portal::g_hiddenShapeId)
							{
								Renderer::Set508718(entryScaler->packedFlags);

								// when the sphere is completely inside of the frustum no clipping is needed
								if (testResult)
									Renderer::RenderPrimitive(ngnImage->primitives[entryScaler->shapeId], &entryScaler->transformMatrix, 0);
								else
									Renderer::RenderPrimitive(ngnImage->primitives[entryScaler->shapeId], &entryScaler->transformMatrix, 0x2000);
							}

							if (scalerEntry->scaler == Portal::g_portalMirrorScaler)
								Portal::g_mirrorRenderToggle = 1 - Portal::g_mirrorRenderToggle;

							++Portal::g_renderedScalerCount;
						}
					}
				}
			}
		}

		for (Portal::PortalState* portalState = ngnImage->portalHashTable->buckets[areaIdx].portalStateHead; portalState; portalState = portalState->next)
		{
			int32_t targetAreaIdx = portalState->targetAreaIdx;

			if (targetAreaIdx != sourceAreaIdx && targetAreaIdx != -1
				&& (Frustum::TestSphereDepthPlanes(&portalState->portal->center, portalState->portal->radiusSquared) & 0x55555555) == 0)
			{
				RenderBucketThroughPortals(ngnImage, portalState->targetAreaIdx, gscaleType, portalState);
			}
		}

		Viewport::RestoreViewportCache(&viewportCache);
		--Portal::g_portalRecursionDepth;

		if (areaIdx < 64)
		{
			if (ngnImage->portalHashTable->buckets[areaIdx].scalerHead)
				Portal::g_lastRenderableAreaIndex = areaIdx;
		}

		Renderer::Set508718(5);
	}

	namespace Portal
	{
		// FUNCTION: TOY2 0x004BC160
		int32_t IsAreaVisible(uint8_t index) { return g_visibleAreaFlags[index & 63]; }
	}
}

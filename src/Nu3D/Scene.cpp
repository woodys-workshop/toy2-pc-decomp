#include "Nu3D/Scene.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Viewport.h"
#include "Nu3D/Portal.h"
#include "Nu3D/Link.h"
#include "Nu3D/Math.h"
#include "NGNLoader/NGNLoader.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderQueue.h"

#include <FLOAT.H>

namespace Nu3D
{
	namespace Scene
	{
		// FUNCTION: TOY2 0x004CDDD0
		void RenderWorldGeometry(int32_t sectorIndex, RenderLayerFlags layerFlags)
		{
			// Draws everything in the world, in two layers: the far layer (everything beyond the far render distance)
			// and the near layer. Each layer is either found by scanning the grid cells around the camera,
			// or by walking through the portals of the area the camera is in.
			NGNLoader::NGNImage* ngnImage = NGNLoader::g_ngnImage;

			if (! ngnImage)
				return;

			Viewport::Reset();

			Viewport::ViewportCache viewportCache;
			Viewport::CacheViewport(&viewportCache);

			Camera::CameraInstance* camera = Camera::g_primaryCamera;

			if (Camera::g_farLayerRendering)
			{
				camera->farZ = Camera::g_farLayerFarZ;
				camera->nearZ = Camera::g_farLayerNearZ;
				camera->viewportLR = Camera::g_farLayerViewportLR;
				camera->viewportTB = FLT_MAX;

				if ((layerFlags & RENDERLAYER_INFINITE_VIEWPORT) == RENDERLAYER_INFINITE_VIEWPORT)
				{
					camera->viewportLR = 0.0;
					camera->viewportTB = FLT_MAX;

					Renderer::SetRenderDistance(FLT_MAX, 0.0);
				}

				Camera::ApplyTransformToCamera(&Camera::g_lastCameraTransform);

				if (ngnImage->shapeCounts[1])
				{
					if (! ngnImage->portalEntryCount || sectorIndex < 0 || (layerFlags & RENDERLAYER_FAR_FORCE_SCAN) != 0)
					{
						RenderCellsInRadius((layerFlags & RENDERLAYER_FAR_FORCE_SCAN) != 0 ? 30 : 20, 1, ngnImage);
					}
					else
					{
						Area::ResetPortalStates(ngnImage);
						Area::RenderBucketThroughPortals(ngnImage, sectorIndex, 1, 0);
						Area::RenderBucketThroughPortals(ngnImage, 0, 1, 0);
					}
				}

				Renderer::FlushRenderQueues();
			}

			Viewport::RestoreViewportCache(&viewportCache);
			Viewport::Reset();

			camera->farZ = Camera::g_cameraFarZ;
			camera->nearZ = Camera::g_cameraNearZ;
			camera->viewportLR = 0.0;
			camera->viewportTB = Camera::g_cameraViewportTB;

			if ((layerFlags & RENDERLAYER_INFINITE_VIEWPORT) == RENDERLAYER_INFINITE_VIEWPORT)
			{
				camera->viewportLR = 0.0;
				camera->viewportTB = FLT_MAX;

				Renderer::SetRenderDistance(FLT_MAX, 0.0);
			}

			Camera::ApplyTransformToCamera(&Camera::g_lastCameraTransform);
			Viewport::CacheViewport(&viewportCache);

			if (Camera::g_nearLayerEnabled && ngnImage->shapeCounts[0])
			{
				if (! ngnImage->portalEntryCount || sectorIndex < 0 || (layerFlags & RENDERLAYER_NEAR_FORCE_SCAN) != 0)
				{
					Portal::MarkAllAreasVisible();
					RenderCellsInRadius((layerFlags & RENDERLAYER_NEAR_FORCE_SCAN) != 0 ? 30 : 15, 0, ngnImage);
				}
				else
				{
					Portal::ClearVisibleAreaFlags();
					Area::ResetPortalStates(ngnImage);
					Area::RenderBucketThroughPortals(ngnImage, sectorIndex, 0, 0);
					Area::RenderBucketThroughPortals(ngnImage, 0, 0, 0);
				}
			}

			Viewport::RestoreViewportCache(&viewportCache);

			camera->viewportLR = 0.0;
			camera->viewportTB = FLT_MAX;

			Camera::ApplyTransformToCamera(&Camera::g_lastCameraTransform);
		}

		// Renders every dynamic scaler in the grid cells around the camera
		// FUNCTION: TOY2 0x004BC720
		int32_t RenderCellsInRadius(int32_t radius, int32_t layer, NGNLoader::NGNImage* ngnImage)
		{
			Vector3F cameraPos;
			Math::GetPositionVector(&Camera::g_secondaryCamera.matrix, &cameraPos);

			Vector3I cell;

			if (! Link::ComputeCellFromXZ(&cell, cameraPos.x, cameraPos.z, layer, ngnImage))
				return Renderer::Set508718(5);

			for (int32_t y = cell.y - radius; y < cell.y + radius; y++)
			{
				for (int32_t x = cell.x - radius; x < cell.x + radius; x++)
				{
					Vector3I pos;
					pos.x = x;
					pos.y = y;
					pos.z = cell.z;

					Link::DynamicScaler** cellHead = Link::GetCellByPos(&pos, ngnImage);

					if (! cellHead)
						continue;

					for (Link::DynamicScaler* scaler = *cellHead; scaler; scaler = scaler->next)
					{
						if (scaler->gscaleType != layer || (scaler->flags & 1) != 0)
							continue;

						Vector3F center;
						Vector3F offset;

						Math::VertexAdd(&center, &scaler->translation, &scaler->boundsCenterWorld);
						Math::VertexSubtract(&offset, &center, &cameraPos);

						double distance = offset.z * offset.z + offset.y * offset.y + offset.x * offset.x;

						// the far layer is everything beyond the far distance, the near layer everything inside of the near distance
						if (layer == 1)
						{
							if (distance <= Renderer::g_farRenderDistance)
								continue;
						}
						else if (distance >= Renderer::g_nearRenderDistance)
						{
							continue;
						}

						int32_t testResult = Frustum::TestSphereAllPlanesAlt(&center, ngnImage->primitives[scaler->shapeId]->boundSphereRadius);

						if ((testResult & 0x55555555) == 0)
						{
							Renderer::Set508718(scaler->packedFlags);

							// when the sphere is completely inside of the frustum no clipping is needed
							if (testResult)
								Renderer::RenderPrimitive(ngnImage->primitives[scaler->shapeId], &scaler->transformMatrix, 0);
							else
								Renderer::RenderPrimitive(ngnImage->primitives[scaler->shapeId], &scaler->transformMatrix, 0x2000);
						}
					}
				}
			}

			return Renderer::Set508718(5);
		}

		// STUB: TOY2 0x004CDC20
		void RenderActors(Toy2::Actor::Toy2Actor** activeActorList) {}

		// STUB: TOY2 0x004CAA60
		void RenderActor(Nu3D::Creature* creature, D3DMATRIX* nodeMatrices, int32_t renderStage) {}
	}
}

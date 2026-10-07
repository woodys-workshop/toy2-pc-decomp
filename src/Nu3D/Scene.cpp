#include "Nu3D/Scene.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Viewport.h"
#include "Nu3D/Portal.h"
#include "Nu3D/Link.h"
#include "Nu3D/Math.h"
#include "NGNLoader/NGNLoader.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderQueue.h"
#include "Nu3D/Creature.h"

#include <FLOAT.H>
#include <string.h>
#include <directx6/ddraw.h>
#include <directx6/d3d.h>

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

		// FUNCTION: TOY2 0x004CAA60
		void RenderActor(Nu3D::Creature* creature, D3DMATRIX* nodeMatrices, int32_t renderStage)
		{
			int32_t curNode = 0;

			if (creature->nodeCount > 0)
			{
				D3DMATRIX* matrix = nodeMatrices;

				do
				{
					if (creature->primitives[curNode])
					{
						// the flags are stored in the pointer slots, bit 0 is hidden, bit 1 is ignore the node matrix, bit 2 is the vertex modulation
						int32_t flags = (int32_t)creature->flagsList[curNode];

						if ((flags & 1) == 0)
						{
							int32_t previousModulation;

							if ((flags & 4) != 0)
								previousModulation = Renderer::Set9F5FF8(1);

							if (((int32_t)creature->flagsList[curNode] & 2) != 0)
							{
								D3DMATRIX localMatrix;

								memcpy(&localMatrix, &Camera::g_secondaryCamera, sizeof(localMatrix));
								Math::ScaleMatrix(&localMatrix);

								localMatrix._41 = matrix->_41;
								localMatrix._43 = matrix->_43;
								localMatrix._42 = matrix->_42;

								Renderer::RenderPrimitive(creature->primitives[curNode], &localMatrix, renderStage);
							}
							else
							{
								Renderer::RenderPrimitive(creature->primitives[curNode], matrix, renderStage);
							}

							if (((int32_t)creature->flagsList[curNode] & 4) != 0)
								Renderer::Set9F5FF8(previousModulation);
						}
					}

					++curNode;
					++matrix;
				} while (curNode < creature->nodeCount);
			}

			Renderer::RenderPatchList(creature->patch, nodeMatrices, creature->flagsList, renderStage);
		}

		// FUNCTION: TOY2 0x004CDC20
		void RenderActors(Toy2::Actor::Toy2Actor** activeActorList)
		{
			int32_t previousFlags = Renderer::Set508718(0);

			if (NGNLoader::g_ngnImage->creatureCount)
			{
				Toy2::Actor::Toy2Actor** actor = activeActorList;

				if (*activeActorList)
				{
					D3DMATRIX* nodeMatrices = (D3DMATRIX*)g_nodeMatrices;
					Toy2::Actor::Toy2Actor* nextActor;

					do
					{
						if (((*actor)->actorFlags & 1) != 0)
						{
							if ((*actor)->useTint == 1)
							{
								Renderer::SetVertexColorModulation((*actor)->actorTint.r, (*actor)->actorTint.g, (*actor)->actorTint.b);

								Renderer::EnableVertexColorModulation(1);
							}

							Nu3D::Creature* creature = NGNLoader::g_ngnImage->creatureData[(*actor)->creatureId];

							if (creature)
								RenderActor(creature, nodeMatrices, 0);

							if ((*actor)->useTint == 1)
								Renderer::EnableVertexColorModulation(0);
						}

						nextActor = actor[1];
						++actor;
						nodeMatrices += 32;
					} while (nextActor);
				}
			}

			Renderer::Set508718(previousFlags);
		}
	}
}

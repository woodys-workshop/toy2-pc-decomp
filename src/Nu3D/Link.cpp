#pragma once

#include "Nu3D/Link.h"
#include "Nu3D/Math.h"
#include "NGNLoader/NGNLoader.h"
#include "Toy2/Levels.h"

namespace Nu3D
{
	namespace Link
	{
		// FUNCTION: TOY2 0x004C3130 [MATCHED]
		DynamicScaler** GetCellByPos(Vector3I* pos, NGNLoader::NGNImage* ngnImage)
		{
			int32_t x = pos->x;

			if (pos->x < 0)
				return 0;

			int32_t gridWidth = ngnImage->gridWidth;

			if (x >= gridWidth)
				return 0;

			int32_t y = pos->y;

			if (y < 0 || y >= ngnImage->gridHeight)
				return 0;
			else
				return &ngnImage->spacialGrid[pos->z][x] + gridWidth * y;
		}

		// FUNCTION: TOY2 0x004C30D0 [MATCHED]
		int32_t ComputeCellFromXZ(Vector3I* pos, float x, float z, int32_t layer, NGNLoader::NGNImage* ngnImage)
		{
			int32_t xTemp = ((x - ngnImage->worldMinX) / ngnImage->cellWidthInWorldUnits);
			int32_t yTemp = ((z - ngnImage->worldMinZ) / ngnImage->cellHeightInWorldUnits);

			if (xTemp < 0 || xTemp >= ngnImage->gridWidth || yTemp < 0 || yTemp >= ngnImage->gridHeight)
				return 0;

			pos->y = yTemp;
			pos->x = xTemp;
			pos->z = layer;

			return 1;
		}

		// FUNCTION: TOY2 0x004C31C0 [MATCHED]
		void InsertScalerAtComputedCell(DynamicScaler* scaler, int32_t type, NGNLoader::NGNImage* ngnImage)
		{
			Vector3F vector;
			Vector3I pos;

			Math::VertexAdd(&vector, &scaler->translation, &scaler->boundsCenterWorld);

			if (Link::ComputeCellFromXZ(&pos, vector.x, vector.z, type, ngnImage))
			{
				DynamicScaler** head = Link::GetCellByPos(&pos, ngnImage);
				DynamicScaler* headPtr = *head;

				scaler->next = *head;

				if (headPtr)
					headPtr->prev = scaler;

				scaler->prev = 0;
				scaler->cellHead = head;

				*head = scaler;
			}
			else
			{
				scaler->cellHead = 0;
			}
		}

		// FUNCTION: TOY2 0x004C3170 [MATCHED]
		void UnlinkScalerThenReinsert(DynamicScaler* scaler, NGNLoader::NGNImage* ngnImage)
		{
			DynamicScaler** cellHead = scaler->cellHead;

			if (cellHead)
			{
				DynamicScaler* prev = scaler->prev;

				if (prev)
					prev->next = scaler->next;
				else
					*cellHead = scaler->next;

				if (scaler->next)
					scaler->next->prev = scaler->prev;

				scaler->cellHead = 0;
			}

			InsertScalerAtComputedCell(scaler, scaler->gscaleType, ngnImage);
		}

		// FUNCTION: TOY2 0x004CCBE0 [MATCHED]
		void RebuildMatrixAndCommit(Linker* link)
		{
			D3DMATRIX* matrix = &link->dynamicScaler->transformMatrix;

			Math::BuildIdentityMatrix(matrix);
			Math::ScaleMatrixByVector(matrix, &link->currentScale);

			Math::RotateZFromLut(matrix, link->currentRot.z);
			Math::RotateYFromLut(matrix, link->currentRot.y);
			Math::PostRotateXFromLut(matrix, link->currentRot.x);

			Math::AddWorldSpaceTransform(matrix, &link->currentPos);

			Math::TransformVectorByMatrix(
				&link->dynamicScaler->boundsCenterWorld, &NGNLoader::g_ngnImage->primitives[link->dynamicScaler->shapeId]->boundsCenter, matrix);

			link->dynamicScaler->translation = link->currentPos;

			UnlinkScalerThenReinsert(link->dynamicScaler, NGNLoader::g_ngnImage);
		}

		// FUNCTION: TOY2 0x004CCB20 [MATCHED]
		void SetScaleFromFixedOffsets(int32_t linkId, int32_t x, int32_t y, int32_t z)
		{
			if (! NGNLoader::g_ngnImage)
				return;

			Linker* links = NGNLoader::g_ngnImage->links;

			if (! links)
				return;

			if (linkId < NGNLoader::g_ngnImage->maxLinkId)
			{
				Linker* link = &links[linkId];

				DynamicScaler* scaler = link->dynamicScaler;

				if (! scaler)
					return;

				if (x || y || z)
				{
					float xValue = x * scaler->scale.x;

					scaler->flags &= ~1;
					link->currentScale.x = xValue * 0.000244140625f;
					link->currentScale.y = y * scaler->scale.y * 0.000244140625f;
					link->currentScale.z = z * scaler->scale.z * 0.000244140625f;

					RebuildMatrixAndCommit(link);
				}
				else
				{
					scaler->flags |= 1u;
				}
			}
		}

		// FUNCTION: TOY2 0x004CCD40
		void GetRotation8Bit(int32_t linkId, Vector3I* out)
		{
			if (! NGNLoader::g_ngnImage)
				return;

			Linker* links = NGNLoader::g_ngnImage->links;

			if (! links)
				return;

			if (linkId < NGNLoader::g_ngnImage->maxLinkId)
			{
				Linker* link = &links[linkId];

				if (link->dynamicScaler)
				{
					out->x = link->currentRot.x >> 4;
					out->y = link->currentRot.y >> 4;
					out->z = link->currentRot.z >> 4;
				}
			}
		}

		// FUNCTION: TOY2 0x004CCCE0
		void SetRotationAbsolute8bit(int32_t linkId, int32_t x, int32_t y, int32_t z)
		{
			if (! NGNLoader::g_ngnImage)
				return;

			Linker* links = NGNLoader::g_ngnImage->links;

			if (! links)
				return;

			if (linkId < NGNLoader::g_ngnImage->maxLinkId)
			{
				Linker* link = &links[linkId];

				if (link->dynamicScaler)
				{
					link->currentRot.x = 16 * x;
					link->currentRot.y = 16 * y;
					link->currentRot.z = 16 * z;

					RebuildMatrixAndCommit(link);
				}
			}
		}

		// FUNCTION: TOY2 0x004CCEF0
		void GetCurrentPosFixed(int32_t linkId, Vector3I* out)
		{
			if (! NGNLoader::g_ngnImage)
				return;

			Linker* links = NGNLoader::g_ngnImage->links;

			if (! links)
				return;

			if (linkId < NGNLoader::g_ngnImage->maxLinkId)
			{
				Linker* link = &links[linkId];

				if (link->dynamicScaler)
				{
					int32_t shift = link->dynamicScaler->gscaleType != 0 ? 3 : 5;

					out->x = (int32_t)link->currentPos.x << shift;
					out->y = (int32_t)link->currentPos.y << shift;
					out->z = (int32_t)link->currentPos.z << shift;
				}
			}
		}

		// FUNCTION: TOY2 0x004CCE30
		void SetPositionRawAndCommit(int32_t linkId, int32_t x, int32_t y, int32_t z)
		{
			if (! NGNLoader::g_ngnImage)
				return;

			Linker* links = NGNLoader::g_ngnImage->links;

			if (! links)
				return;

			if (linkId < NGNLoader::g_ngnImage->maxLinkId)
			{
				Linker* link = &links[linkId];
				DynamicScaler* scaler = link->dynamicScaler;

				if (scaler)
				{
					if (scaler->gscaleType)
					{
						x *= 4;
						y *= 4;
						z *= 4;
					}

					link->currentPos.x = x;
					link->currentPos.y = y;
					link->currentPos.z = z;

					scaler->transformMatrix._41 = x;
					scaler->transformMatrix._42 = link->currentPos.y;
					scaler->transformMatrix._43 = link->currentPos.z;

					scaler->translation.x = link->currentPos.x;
					scaler->translation.y = link->currentPos.y;
					scaler->translation.z = link->currentPos.z;

					UnlinkScalerThenReinsert(scaler, NGNLoader::g_ngnImage);
				}
			}
		}

		// FUNCTION: TOY2 0x00438910
		void FollowWaypointPath(int32_t linkId, int32_t pathTableIdx, int32_t* progress)
		{
			// Moves a link along a path (a record of waypoints) a bit further each call, turning it to face the way it goes
			*progress += 64;

			Toy2::Levels::RecordData* path = Toy2::Levels::g_recordData[pathTableIdx];

			// loop back to the start once there are no waypoints left
			if ((*progress >> 8) > path->recordCount - 5)
				*progress = 0;

			// the top 24 bits of the progress are the waypoint, the bottom 8 bits are how far to the next one
			int32_t waypoint = *progress >> 8;
			int32_t blend = *progress & 0xFF;

			Vector3I* from = &path->data[waypoint];
			Vector3I* to = &path->data[waypoint + 1];

			int32_t targetX = (blend * to->x + (255 - blend) * from->x) / 8;
			int32_t targetY = (blend * to->y + (255 - blend) * from->y) / 8;
			int32_t targetZ = (blend * to->z + (255 - blend) * from->z) / 8;

			Vector3I currentPos;
			Vector3I currentRot;

			GetCurrentPosFixed(linkId, &currentPos);
			GetRotation8Bit(linkId, &currentRot);

			int32_t angleToTarget = Math::CartesianToFixedAngle(targetZ - currentPos.z, currentPos.x - targetX);
			int32_t angleDelta = (currentRot.y - angleToTarget - 2048) & 0xFFF;

			int32_t newRotY;

			// turn a quarter of the way towards the target each time
			if (angleDelta <= 2048)
				newRotY = currentRot.y - (angleDelta >> 2);
			else
				newRotY = ((4096 - angleDelta) >> 2) + currentRot.y;

			SetRotationAbsolute8bit(linkId, 0, newRotY, 0);
			SetPositionRawAndCommit(linkId, targetX >> 5, targetY >> 5, targetZ >> 5);
		}

		// FUNCTION: TOY2 0x004CCC70
		void SetRotationRelative8bit(int32_t linkId, int32_t x, int32_t y, int32_t z)
		{
			if (NGNLoader::g_ngnImage)
			{
				Link::Linker* links = NGNLoader::g_ngnImage->links;

				if (links)
				{
					if (linkId < NGNLoader::g_ngnImage->maxLinkId)
					{
						Link::Linker* link = &links[linkId];
						Link::DynamicScaler* scaler = link->dynamicScaler;

						if (scaler)
						{
							link->currentRot.x = 16 * x + scaler->rotation.x;
							link->currentRot.y = 16 * y + scaler->rotation.y;
							link->currentRot.z = 16 * z + scaler->rotation.z;

							Link::RebuildMatrixAndCommit(link);
						}
					}
				}
			}
		}

		// FUNCTION: TOY2 0x004CCDA0
		void TransformVectorInt3x3(int32_t linkId, Vector3I* out)
		{
			if (NGNLoader::g_ngnImage)
			{
				Link::Linker* links = NGNLoader::g_ngnImage->links;

				if (links)
				{
					if (linkId < NGNLoader::g_ngnImage->maxLinkId)
					{
						Link::Linker* link = &links[linkId];

						if (link->dynamicScaler)
						{
							Vector3F vector;

							vector.x = out->x;
							vector.y = out->y;
							vector.z = out->z;

							Math::TransformVectorByMatrix(&vector, &vector, &link->dynamicScaler->transformMatrix);

							out->x = vector.x;
							out->y = vector.y;
							out->z = vector.z;
						}
					}
				}
			}
		}

		// FUNCTION: TOY2 0x004CCF70
		void GetTargetPosFixed(int32_t linkId, Vector3I* out)
		{
			if (NGNLoader::g_ngnImage)
			{
				Nu3D::Link::Linker* links = NGNLoader::g_ngnImage->links;

				if (links)
				{
					if (linkId < NGNLoader::g_ngnImage->maxLinkId)
					{
						Nu3D::Link::Linker* link = &links[linkId];
						Nu3D::Link::DynamicScaler* scaler = link->dynamicScaler;

						if (scaler)
						{
							uint32_t shift = scaler->gscaleType != 0 ? 3 : 5;

							out->x = static_cast<int32_t>(link->targetPos.x) << shift;
							out->y = static_cast<int32_t>(link->targetPos.y) << shift;
							out->z = static_cast<int32_t>(link->targetPos.z) << shift;
						}
					}
				}
			}
		}

		// FUNCTION: TOY2 0x004CCFF0
		void SnapToOtherLinkUsingScale(int32_t linkIndex1, int32_t linkIndex2)
		{
			if (! NGNLoader::g_ngnImage)
				return;

			Linker* links = NGNLoader::g_ngnImage->links;

			if (! links)
				return;

			int32_t maxLinkId = NGNLoader::g_ngnImage->maxLinkId;

			if (linkIndex1 < maxLinkId && linkIndex2 < maxLinkId)
			{
				Linker* link1 = &links[linkIndex1];
				Linker* link2 = &links[linkIndex2];

				if (link2->dynamicScaler)
				{
					DynamicScaler* scaler = link1->dynamicScaler;

					if (scaler)
					{
						double y = (link1->currentPos.y - link2->currentPos.y) * link2->currentScale.y + link2->currentPos.y;
						float z = (link1->currentPos.z - link2->currentPos.z) * link2->currentScale.z + link2->currentPos.z;
						double x = (link1->currentPos.x - link2->currentPos.x) * link2->currentScale.x + link2->currentPos.x;

						link1->currentPos.z = z;
						link1->currentPos.x = x;
						float rawX = link1->currentPos.x;
						link1->currentPos.y = y;

						scaler->transformMatrix._41 = rawX;
						scaler->transformMatrix._42 = link1->currentPos.y;
						scaler->transformMatrix._43 = link1->currentPos.z;

						Vector3F* translation = &link1->dynamicScaler->translation;

						translation->x = link1->currentPos.x;
						translation->y = link1->currentPos.y;
						translation->z = link1->currentPos.z;

						UnlinkScalerThenReinsert(link1->dynamicScaler, NGNLoader::g_ngnImage);
					}
				}
			}
		}

		// FUNCTION: TOY2 0x004CD0C0
		void CopyShapeId(int32_t linkIndex1, int32_t linkIndex2)
		{
			if (! NGNLoader::g_ngnImage)
				return;

			Linker* links = NGNLoader::g_ngnImage->links;

			if (! links)
				return;

			int32_t maxLinkId = NGNLoader::g_ngnImage->maxLinkId;

			if (linkIndex1 < maxLinkId && linkIndex2 < maxLinkId)
				links[linkIndex1].dynamicScaler->shapeId = links[linkIndex2].dynamicScaler->shapeId;
		}
	}
}
#pragma once

#include "Numerics.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include "Common.h"

namespace NGNLoader
{
	struct NGNImage;
}

namespace Nu3D
{
	namespace Link
	{
		struct DynamicScaler
		{
			DynamicScaler* next;
			DynamicScaler* prev;
			DynamicScaler** cellHead;
			Vector3F translation;
			Vector3I rotation;
			Vector3F scale;
			D3DMATRIX transformMatrix;
			int32_t shapeId;
			Vector3F boundsCenterWorld;
			int32_t packedAreaData;
			int32_t areaIndex;
			int32_t packedFlags;
			int32_t flags;
			int32_t gscaleType;
		};

		struct Linker
		{
			Vector3F currentPos;
			Vector3F targetPos;
			Vector3I currentRot;
			Vector3F currentScale;
			DynamicScaler* dynamicScaler;
			D3DMATRIX transformMatrix;
		};

		DynamicScaler** GetCellByPos(Vector3I* pos, NGNLoader::NGNImage* ngnImage);
		int32_t ComputeCellFromXZ(Vector3I* pos, float x, float z, int32_t layer, NGNLoader::NGNImage* ngnImage);
		void InsertScalerAtComputedCell(DynamicScaler* scaler, int32_t type, NGNLoader::NGNImage* ngnImage);
		void GetCurrentPosFixed(int32_t linkId, Vector3I* out);
		void SetPositionRawAndCommit(int32_t linkId, int32_t x, int32_t y, int32_t z);
		void GetRotation8Bit(int32_t linkId, Vector3I* out);
		void SetRotationAbsolute8bit(int32_t linkId, int32_t x, int32_t y, int32_t z);
		void SetScaleFromFixedOffsets(int32_t linkId, int32_t x, int32_t y, int32_t z);
		void FollowWaypointPath(int32_t linkId, int32_t pathTableIdx, int32_t* progress);
		void SetRotationRelative8bit(int32_t linkId, int32_t x, int32_t y, int32_t z);
		void TransformVectorInt3x3(int32_t linkId, Vector3I* out);
		void GetTargetPosFixed(int32_t linkId, Vector3I* out);
		void SnapToOtherLinkUsingScale(int32_t linkIndex1, int32_t linkIndex2);
		void CopyShapeId(int32_t linkIndex1, int32_t linkIndex2);

		STATIC_ASSERT(sizeof(DynamicScaler) == 0x94);
		STATIC_ASSERT(sizeof(Linker) == 0x74);
	}
}
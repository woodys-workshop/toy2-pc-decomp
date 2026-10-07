#pragma once

#include "Numerics.h"
#include "Nu3D/Primitive.h"
#include "Nu3D/Patch.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include "Common.h"

namespace Nu3D
{
	struct NodeMatricesInner
	{
		D3DMATRIX matrices[32];
	};

	struct Creature
	{
		int32_t dataFlags;
		int32_t nodeCount;
		D3DMATRIX* matrixList1;
		D3DMATRIX* matrixList2;
		D3DMATRIX* matrixList3;
		char** nodeNames;
		Primitive** primitives;
		Patch* patch;
		int32_t** flagsList;
		int32_t** unkVar10;
		void** animData;
		int32_t animCount;

		static void Destroy(Creature* creature);
		static int32_t SetNodeVisible(Creature* creature, int32_t nodeIdx, int32_t visible);
		static void SetNodeVisibleByIndex(int32_t creatureIndex, int32_t nodeIdx, int32_t visible);
	};

	extern NodeMatricesInner g_nodeMatrices[64];

	STATIC_ASSERT(sizeof(NodeMatricesInner) == 0x800);
	STATIC_ASSERT(sizeof(Creature) == 0x30);
}
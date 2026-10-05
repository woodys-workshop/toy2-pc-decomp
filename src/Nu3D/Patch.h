#pragma once

#include "Nu3D/Nu3D.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>

namespace Nu3D
{
	struct Patch
	{
		struct PatchVertices
		{
			int32_t format;
			int32_t vertexCount;

			union
			{
				Vertex* vertices;
				VertexTL* verticesTL;
			} data;

			int32_t renderFlags;
			LPDIRECT3DVERTEXBUFFER vertexBuffer;

			static BOOL CreateVertexBuffer(PatchVertices* patchVertices, int32_t flags);
			static void ReleaseBuffer(PatchVertices* patchVertices);
			static void FreeVertices(PatchVertices* patchVertices);
		};

		Patch* next;
		Patch* prev;
		Patch* listNext;
		int32_t unkVar4;
		int32_t materialId;
		PatchVertices patchVertices;
		int32_t unkVar11;
		int16_t indexData[4];
		int32_t unkVar14;
		int32_t unkVar15;
	};

	extern Patch* g_patchListHead;
	;

	STATIC_ASSERT(sizeof(Patch) == 0x3C);
	STATIC_ASSERT(sizeof(Patch::PatchVertices) == 0x14);
}
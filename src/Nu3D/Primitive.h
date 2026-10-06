#pragma once

#include "Common.h"
#include "Nu3D/Material.h"
#include "Nu3D/Patch.h"

namespace Nu3D
{
	struct Primitive
	{
		struct Header
		{
			int32_t drawType;
			int32_t indexCount;
			uint16_t* indices;
			Plane* triMeta;

			static void Destroy(Header* primHeader);
		};

		Primitive* next;
		Primitive* prev;
		Primitive* listNext;
		int32_t renderFlags;
		int32_t materialIndex;
		Patch::PatchVertices patchVerts;
		int32_t headerCount;
		Header* header;
		float originRadius;
		float originRadiusSq;
		Vector3F boundsCenter;
		float boundSphereRadius;
		float boundSphereRadiusSq;

		static int32_t CreateAllVertexBuffers(Primitive* prim, int32_t flags);
		static void ComputeBounds(Primitive* primitive);
		static Primitive* Alloc();
		static void ResizePatchVerts(Patch::PatchVertices* patchVerts, int32_t verticesSize);
		static Header* BuildHeader(int32_t size);
		static Primitive* Build(int32_t verticesSize, int32_t headerCount);
		static int32_t InitHeader(Header* primHeader, int32_t drawType, int32_t indexCount);
		static void Destroy(Primitive* l_prim);
		static void Remove(Primitive* prim);
	};

	extern Primitive* g_primListHead;

	STATIC_ASSERT(sizeof(Primitive) == 0x4C);
	STATIC_ASSERT(sizeof(Primitive::Header) == 0x10);
}
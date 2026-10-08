#include "Nu3D/Primitive.h"
#include "Renderer/Renderer.h"
#include "Nu3D/Math.h"

#include <MATH.H>
#include <FLOAT.H>

namespace Nu3D
{
	// GLOBAL: TOY2 0x0088447C
	Primitive* g_primListHead;

	// FUNCTION: TOY2 0x004B32E0
	int32_t Primitive::CreateAllVertexBuffers(Primitive* prim, int32_t flags)
	{
		int32_t result = 1;

		if (! prim)
			return 1;

		do
		{
			result &= Patch::PatchVertices::CreateVertexBuffer(&prim->patchVerts, flags);
			prim = prim->listNext;
		} while (prim);

		return result;
	}

	// FUNCTION: TOY2 0x004CC480
	void Primitive::ComputeBounds(Primitive* primitive)
	{
		float maxRadiusSq = 0.0;
		float minX = FLT_MAX;
		float minY = FLT_MAX;
		float minZ = FLT_MAX;
		float maxX = -FLT_MAX;
		float maxY = -FLT_MAX;
		float maxZ = -FLT_MAX;

		Vector3F extent;

		Primitive* prim;

		// pass 1: bounding box, and the furthest point from the origin
		for (prim = primitive; prim; prim = prim->listNext)
		{
			int32_t drawType = prim->header->drawType;

			if (drawType < 4 || drawType > 5)
			{
				for (int32_t i = 0; i < prim->patchVerts.vertexCount; i++)
				{
					Vector3F* position = &prim->patchVerts.data.vertices[i].position;

					if (position->x <= minX)
						minX = position->x;

					if (position->y <= minY)
						minY = position->y;

					if (position->z <= minZ)
						minZ = position->z;

					if (position->x >= maxX)
						maxX = position->x;

					if (position->y >= maxY)
						maxY = position->y;

					if (position->z >= maxZ)
						maxZ = position->z;

					float lengthSq = position->y * position->y + position->x * position->x + position->z * position->z;

					if (lengthSq > maxRadiusSq)
						maxRadiusSq = lengthSq;
				}
			}
			else
			{
				// billboards: indices[0] is the centre vertex, indices[1] is the first of its 4 corner vertices
				for (int32_t h = 0; h < prim->headerCount; h++)
				{
					uint16_t* indices = prim->header[h].indices;
					// the highest squared length of the 4 corner vertices of the billboard
					float cornerRadiusSq = 0.0;
					int32_t firstCorner = indices[1];

					for (int32_t corner = 0; corner < 4; corner++)
					{
						Vector3F* cornerPosition = &prim->patchVerts.data.vertices[firstCorner + corner].position;
						float cornerLengthSq =
							cornerPosition->x * cornerPosition->x + cornerPosition->y * cornerPosition->y + cornerPosition->z * cornerPosition->z;

						if (cornerRadiusSq <= cornerLengthSq)
							cornerRadiusSq = cornerLengthSq;
					}

					Vector3F* center = &prim->patchVerts.data.vertices[indices[0]].position;

					Math::VectorNormalize(&extent, center);
					Math::ScaleVector(&extent, &extent, sqrt(cornerRadiusSq));
					Math::VertexAdd(&extent, &extent, center);

					if (minX >= extent.x)
						minX = extent.x;

					if (minY >= extent.y)
						minY = extent.y;

					if (minZ >= extent.z)
						minZ = extent.z;

					if (maxX <= extent.x)
						maxX = extent.x;

					if (maxY <= extent.y)
						maxY = extent.y;

					if (maxZ <= extent.z)
						maxZ = extent.z;

					float lengthSq = extent.z * extent.z + extent.y * extent.y + extent.x * extent.x;

					if (lengthSq > maxRadiusSq)
						maxRadiusSq = lengthSq;
				}
			}
		}

		primitive->originRadiusSq = maxRadiusSq;
		primitive->originRadius = sqrt(maxRadiusSq);
		primitive->boundsCenter.x = (maxX + minX) * 0.5;
		primitive->boundsCenter.y = (maxY + minY) * 0.5;
		primitive->boundsCenter.z = (maxZ + minZ) * 0.5;

		float maxDistanceSq = 0.0;

		// pass 2: the furthest point from the centre of the bounding box
		for (prim = primitive; prim; prim = prim->listNext)
		{
			int32_t drawType = prim->header->drawType;

			if (drawType < 4 || drawType > 5)
			{
				for (int32_t i = 0; i < prim->patchVerts.vertexCount; i++)
				{
					Vector3F offset;
					Math::VertexSubtract(&offset, &prim->patchVerts.data.vertices[i].position, &primitive->boundsCenter);

					float distanceSq = offset.z * offset.z + offset.y * offset.y + offset.x * offset.x;

					if (distanceSq > maxDistanceSq)
						maxDistanceSq = distanceSq;
				}
			}
			else
			{
				for (int32_t h = 0; h < prim->headerCount; h++)
				{
					uint16_t* indices = prim->header[h].indices;
					// the highest squared length of the 4 corner vertices of the billboard
					float cornerRadiusSq = 0.0;
					int32_t firstCorner = indices[1];

					for (int32_t corner = 0; corner < 4; corner++)
					{
						Vector3F* cornerPosition = &prim->patchVerts.data.vertices[firstCorner + corner].position;
						float cornerLengthSq =
							cornerPosition->x * cornerPosition->x + cornerPosition->y * cornerPosition->y + cornerPosition->z * cornerPosition->z;

						if (cornerRadiusSq <= cornerLengthSq)
							cornerRadiusSq = cornerLengthSq;
					}

					Vector3F offset;
					Math::VertexSubtract(&offset, &prim->patchVerts.data.vertices[indices[0]].position, &primitive->boundsCenter);

					Math::VectorNormalize(&extent, &offset);
					Math::ScaleVector(&extent, &extent, sqrt(cornerRadiusSq));
					Math::VertexAdd(&offset, &extent, &offset);

					float distanceSq = offset.z * offset.z + offset.y * offset.y + offset.x * offset.x;

					if (distanceSq > maxDistanceSq)
						maxDistanceSq = distanceSq;
				}
			}
		}

		primitive->boundSphereRadiusSq = maxDistanceSq;
		primitive->boundSphereRadius = sqrt(maxDistanceSq);
	}

	// FUNCTION: TOY2 0x004B3280
	Primitive* Primitive::Alloc()
	{
		Primitive* next;
		Primitive* prim = (Primitive*)malloc(sizeof(Primitive));

		if (prim)
		{
			memset(prim, 0, sizeof(Primitive));

			prim->next = g_primListHead;

			g_primListHead = prim;

			next = prim->next;
			prim->prev = 0;

			if (next)
				next->prev = prim;
		}

		return prim;
	}

	// FUNCTION: TOY2 0x004B3210
	void Primitive::ResizePatchVerts(Patch::PatchVertices* patchVerts, int32_t verticesSize)
	{
		Vertex* vertexList = (Vertex*)malloc(sizeof(Vertex) * verticesSize);

		if (vertexList)
		{
			if (patchVerts)
			{
				patchVerts->format = D3DFVF_0x152;
				patchVerts->data.vertices = vertexList;
				patchVerts->vertexCount = verticesSize;
			}
		}
	}

	// FUNCTION: TOY2 0x004B3240
	Primitive::Header* Primitive::BuildHeader(int32_t size)
	{
		Primitive::Header* headerList = (Primitive::Header*)malloc(sizeof(Primitive::Header) * size);

		if (headerList)
		{
			memset(headerList, 0, sizeof(Primitive::Header) * size);
			return headerList;
		}
		else
		{
			return 0;
		}
	}

	// FUNCTION: TOY2 0x004B31D0
	Primitive* Primitive::Build(int32_t verticesSize, int32_t headerCount)
	{
		Primitive* prim = Primitive::Alloc();

		if (prim)
		{
			ResizePatchVerts(&prim->patchVerts, verticesSize);

			prim->headerCount = headerCount;
			prim->header = BuildHeader(headerCount);
			prim->originRadiusSq = 0.0;
			prim->originRadius = 0.0;
		}

		return prim;
	}

	// FUNCTION: TOY2 0x004B2E60
	int32_t Primitive::InitHeader(Header* primHeader, int32_t drawType, int32_t indexCount)
	{
		primHeader->indices = (uint16_t*)malloc(sizeof(uint16_t) * indexCount);

		Plane* triMeta = (Plane*)malloc(sizeof(Plane) * indexCount / 3);
		uint16_t* indices = primHeader->indices;

		primHeader->indexCount = indexCount;
		primHeader->triMeta = triMeta;
		primHeader->drawType = drawType;

		return indices && triMeta;
	}

	// FUNCTION: TOY2 0x004B2E30
	void Primitive::Header::Destroy(Primitive::Header* primHeader)
	{
		if (primHeader->indices)
			free(primHeader->indices);

		if (primHeader->triMeta)
			free(primHeader->triMeta);

		primHeader->indices = 0;
	}

	// FUNCTION: TOY2 0x004B3110
	void Primitive::Destroy(Primitive* prim)
	{
		if (prim->listNext)
			Primitive::Destroy(prim->listNext);

		if (prim->header)
		{
			int32_t headerCount = 0;

			if (prim->headerCount > 0)
			{
				int32_t curHeaderIndex = 0;

				do
				{
					Primitive::Header::Destroy(&prim->header[curHeaderIndex]);
					++headerCount;
					++curHeaderIndex;
				} while (headerCount < prim->headerCount);
			}
		}

		Patch::PatchVertices::ReleaseBuffer(&prim->patchVerts);
		Patch::PatchVertices::FreeVertices(&prim->patchVerts);

		Primitive::Remove(prim);
	}

	// FUNCTION: TOY2 0x004B3190
	void Primitive::Remove(Primitive* prim)
	{
		if (prim->next)
			prim->next->prev = prim->prev;

		Primitive* prev = prim->prev;

		if (prev)
			prev->next = prim->next;
		else
			g_primListHead = prim->next;

		free(prim);
	}
}
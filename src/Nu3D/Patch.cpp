#include "Nu3D/Patch.h"
#include "Renderer/Renderer.h"
#include "Logger.h"

#include <STDLIB.H>

namespace Nu3D
{
    // GLOBAL: TOY2 0x00884480
    Patch *g_patchListHead;

	// FUNCTION: TOY2 0x004B2EB0
	void Patch::PatchVertices::ReleaseBuffer(PatchVertices* patchVertices)
	{
		if (patchVertices->vertexBuffer)
		{
			DrawingAPI::ReleaseVertexBuffer(patchVertices->vertexBuffer);
			patchVertices->vertexBuffer = 0;
		}
	}

	// FUNCTION: TOY2 0x004B3170
	void Patch::PatchVertices::FreeVertices(PatchVertices* patchVertices)
	{
		if (patchVertices->data.vertices)
			free(patchVertices->data.vertices);
	}

	// FUNCTION: TOY2 0x004B2ED0
	BOOL Patch::PatchVertices::CreateVertexBuffer(PatchVertices* patchVertices, int32_t flags)
	{
		patchVertices->renderFlags = 0;

		if (patchVertices->vertexBuffer)
			ReleaseBuffer(patchVertices);

		D3DVERTEXBUFFERDESC vertexDesc;

		vertexDesc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
		memset(&vertexDesc.dwCaps, 0, 12);

		if (Renderer::g_isSoftwareRendering)
			flags |= 4;

		if ((flags & 2) != 0)
			vertexDesc.dwCaps |= 0x10000;

		if ((flags & 4) != 0)
			vertexDesc.dwCaps |= 0x800;

		vertexDesc.dwFVF = patchVertices->format;
		vertexDesc.dwNumVertices = patchVertices->vertexCount;

		HRESULT result = DrawingAPI::CreateVertexBuffer(&vertexDesc, &patchVertices->vertexBuffer, (flags & 0x10) >> 2);

		if (result)
		{
			Logger::GetErrorHandler("C:\\projects\\nu3d\\objcore.c", 654)("vertex buffer allocation failure");
		}
		else if ((flags & 1) != 0)
		{
			// fill the buffer with the vertices we have in system memory
			LPVOID lockedData;

			result = DrawingAPI::LockVertexBuffer(patchVertices->vertexBuffer, 0x821, &lockedData, 0);

			if (result)
			{
				Logger::GetErrorHandler("C:\\projects\\nu3d\\objcore.c", 654)("vertex buffer allocation failure");
			}
			else
			{
				// format 0x152 vertices are 36 bytes (position, normal, colour, uv), 0x112 and 0x1C4 are 32 bytes
				if (patchVertices->format == 0x152)
					memcpy(lockedData, patchVertices->data.vertices, sizeof(Vertex) * patchVertices->vertexCount);
				else if (patchVertices->format == 0x112 || patchVertices->format == 0x1C4)
					memcpy(lockedData, patchVertices->data.verticesTL, sizeof(VertexTL) * patchVertices->vertexCount);

				DrawingAPI::UnlockVertexBuffer(patchVertices->vertexBuffer);
			}
		}

		patchVertices->renderFlags = flags;

		return result == 0;
	}
}

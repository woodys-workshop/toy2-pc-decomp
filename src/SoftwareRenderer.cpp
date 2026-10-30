#include "SoftwareRenderer.h"
#include "DrawingDevice.h"
#include "Renderer/Renderer.h"
#include "Nu3D/BmpDataNode.h"

#include <math.h>
#include <stdlib.h>

// Assembly-only CRT entry: ST(0) input, EDX:EAX result, no stack arguments.
extern "C" int64_t __cdecl _ftol();

namespace SoftwareRenderer
{
	typedef void(__cdecl* SpanFunction)(BVertex*, BVertex*, uint16_t*, uint32_t*, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
	STATIC_ASSERT(sizeof(SpanFunction) == 4);

	struct TextureSnapshot
	{
		uint32_t* texData;
		DDSURFACEDESC2* surfaceDesc;
	};

	STATIC_ASSERT(sizeof(TextureSnapshot) == 8);

	// GLOBAL: TOY2 0x004F7400
	PointI g_unk4F7400 = { -32768, -32768 };

	// GLOBAL: TOY2 0x00731F28
	BltCommand g_bltQueue[32];

	// GLOBAL: TOY2 0x00731C00
	int16_t g_bltQueueCount;

	// GLOBAL: TOY2 0x0087E50C
	uint8_t g_unk87E50C[16384];

	// GLOBAL: TOY2 0x00559C40
	int32_t g_unk559C40 = -1;

	// GLOBAL: TOY2 0x00839280
	int32_t g_unk839280 = -1;

	// GLOBAL: TOY2 0x00504D34
	void* g_unk504D34 = g_unk87E50C;

	// GLOBAL: TOY2 0x00839278
	int32_t g_unk839278;

	// GLOBAL: TOY2 0x00731CBC
	uint32_t g_unk731CBC;

	// GLOBAL: TOY2 0x00500A1C
	int32_t g_unk500A1C = 0xFFFFFFFF;

	// GLOBAL: TOY2 0x00830C60
	int32_t g_unk830C60;

	// GLOBAL: TOY2 0x00E4D950
	int32_t g_unkE4D950;

	// GLOBAL: TOY2 0x009F6008
	int32_t g_unk9F6008;

	// GLOBAL: TOY2 0x00A4CC74
	int32_t g_levelFileIndex;

	// GLOBAL: TOY2 0x009F5FF4
	Nu3D::Viewport::ViewportRect* g_viewportRect;

	// GLOBAL: TOY2 0x00E4D960
	int32_t g_softwareClearColor;

	// GLOBAL: TOY2 0x005088D4
	int32_t g_topOffset = -1;

	// GLOBAL: TOY2 0x005088D8
	int32_t g_bottomOffset = -1;

	// GLOBAL: TOY2 0x005088DC
	int32_t g_leftOffset = -1;

	// GLOBAL: TOY2 0x005088E0
	int32_t g_rightOffset = -1;

	// GLOBAL: TOY2 0x00DE20B4
	int32_t g_surfaceWidth;

	// GLOBAL: TOY2 0x00E4D7B0
	int32_t g_surfaceHeight;

	// GLOBAL: TOY2 0x00E4D7A0
	int32_t g_surfacePitch;

	// GLOBAL: TOY2 0x00DE20AC
	int32_t g_pixelMode;

	// GLOBAL: TOY2 0x00B626AC
	int32_t g_vertexAlpha;

	// GLOBAL: TOY2 0x00B626B0
	SpanFunction g_selectedSpan;

	// GLOBAL: TOY2 0x00B626B8
	int32_t g_vertexAlphaComplement;

	// GLOBAL: TOY2 0x00A4CC6C
	int32_t g_zoomLevel;

	// GLOBAL: TOY2 0x00E4D7BC
	int32_t g_zoomedWidth;

	// GLOBAL: TOY2 0x00B7FBB4
	int32_t g_zoomedHeight;

	// GLOBAL: TOY2 0x00DE20A4
	int32_t g_clipLeft;

	// GLOBAL: TOY2 0x00DBB088
	int32_t g_clipRight;

	// GLOBAL: TOY2 0x00B7FBBC
	int32_t g_clipTop;

	// GLOBAL: TOY2 0x00DE20B8
	int32_t g_clipBottom;

	// GLOBAL: TOY2 0x00DBB084
	float g_viewOffsetX;

	// GLOBAL: TOY2 0x00DE20B0
	float g_viewOffsetY;

	// GLOBAL: TOY2 0x00DFF580
	float g_viewScaleX;

	// GLOBAL: TOY2 0x00B626BC
	float g_viewScaleY;

	// GLOBAL: TOY2 0x00E4D7A8
	float g_zoomScaleX;

	// GLOBAL: TOY2 0x00B7FBA4
	float g_zoomScaleY;

	// GLOBAL: TOY2 0x00A4CC84
	uint8_t* g_frameBuffer;

	// GLOBAL: TOY2 0x00E4D7B4
	uint8_t* g_primarySurfacePixels;

	// GLOBAL: TOY2 0x00A4CC78
	uint16_t* g_colourScaleTables;

	// GLOBAL: TOY2 0x00E4D7AC
	uint16_t* g_blueScaleTable;

	// GLOBAL: TOY2 0x00DBB08C
	uint16_t* g_greenScaleTable;

	// GLOBAL: TOY2 0x00B7FBC0
	uint16_t* g_redScaleTable;

	// GLOBAL: TOY2 0x00E4D7B8
	uint16_t* g_unkE4D7B8;

	// GLOBAL: TOY2 0x00882910
	int32_t g_bitsPerPixel;

	// GLOBAL: TOY2 0x00884014
	D3DMATRIX* g_viewTransform;

	// GLOBAL: TOY2 0x00884018
	D3DMATRIX* g_projectionTransform;

	// GLOBAL: TOY2 0x008F7E90
	SortedTriangle g_sortedTriPool[3000];

	// GLOBAL: TOY2 0x0095B860
	SortedTriangle* g_depthBuckets[1024];

	// GLOBAL: TOY2 0x009F6010
	int32_t g_sortedTrianglesDisabled;

	// GLOBAL: TOY2 0x00508728
	int32_t g_instanceCounter = 3000;

	// GLOBAL: TOY2 0x00DBB090
	float g_cameraFarZ;

	// GLOBAL: TOY2 0x00DBB080
	float g_cameraNearZ;

	// FUNCTION: TOY2 0x004B5E40 [MATCHED]
	void SubmitSortedTriangle(
		int32_t renderFlags,
		Renderer::RenderEntry* renderEntry,
		int32_t textureIdx,
		Nu3D::VertexTL* vertex1,
		Nu3D::VertexTL* vertex2,
		Nu3D::VertexTL* vertex3)
	{
		do
		{
			if (g_sortedTrianglesDisabled || ! g_instanceCounter)
				break;

			SortedTriangle* triangle = &g_sortedTriPool[--g_instanceCounter];

			triangle->renderFlags = renderFlags;
			triangle->renderEntry = renderEntry;
			triangle->textureIdx = textureIdx;

			memcpy(&triangle->vertices[0], vertex1, sizeof(Nu3D::VertexTL));
			memcpy(&triangle->vertices[1], vertex2, sizeof(Nu3D::VertexTL));

			double z = triangle->vertices[1].position.z;

			memcpy(&triangle->vertices[2], vertex3, sizeof(Nu3D::VertexTL));

			// the nearest depth of the 3 vertices (depths are 0.0 - 1.0 here)
			double lowestZ;
			float nearestZ;

			if (z < triangle->vertices[2].position.z)
				lowestZ = triangle->vertices[1].position.z;
			else
				lowestZ = triangle->vertices[2].position.z;

			if (triangle->vertices[0].position.z < lowestZ)
			{
				nearestZ = triangle->vertices[0].position.z;
			}
			else
			{
				double innerZ = triangle->vertices[1].position.z;

				if (innerZ < triangle->vertices[2].position.z)
					nearestZ = triangle->vertices[1].position.z;
				else
					nearestZ = triangle->vertices[2].position.z;
			}

			float sortKey = nearestZ * 1023.0f;
			triangle->sortKey = sortKey;

			int32_t bucket = (int32_t)sortKey & 0x3FF;
			SortedTriangle* previous = 0;

			// every bucket is kept sorted, furthest (highest key) first
			SortedTriangle* current = g_depthBuckets[bucket];

			while (current && current->sortKey > sortKey)
			{
				previous = current;
				current = current->next;
			}

			if (previous)
			{
				previous->next = triangle;
				triangle->next = current;
			}
			else
			{
				g_depthBuckets[bucket] = triangle;
				triangle->next = current;
			}
		} while (0);
	}

	// FUNCTION: TOY2 0x004B5FB0
	HRESULT SubmitTriangleList(int32_t renderFlags, LPDIRECT3DVERTEXBUFFER buffer, Renderer::RenderEntry* renderEntry, uint16_t* indices, int32_t vertexCount)
	{
		LPVOID data;

		HRESULT result = DrawingAPI::LockVertexBuffer(buffer, 0x801, &data, 0);

		if (result)
			return result;

		Nu3D::VertexTL* vertices = (Nu3D::VertexTL*)data;

		// last triangle first
		for (int32_t i = vertexCount - 3; vertexCount > 0; i -= 3, vertexCount -= 3)
			SubmitSortedTriangle(renderFlags, renderEntry, 0, &vertices[indices[i]], &vertices[indices[i + 1]], &vertices[indices[i + 2]]);

		return DrawingAPI::UnlockVertexBuffer(buffer);
	}

	// FUNCTION: TOY2 0x004B6040
	HRESULT SubmitTriangleStrip(int32_t renderFlags, LPDIRECT3DVERTEXBUFFER buffer, Renderer::RenderEntry* renderEntry, uint16_t* indices, int32_t vertexCount)
	{
		LPVOID data;

		HRESULT result = DrawingAPI::LockVertexBuffer(buffer, 0x801, &data, 0);

		if (result)
			return result;

		Nu3D::VertexTL* vertices = (Nu3D::VertexTL*)data;

		int32_t index1 = indices[1];
		int32_t index2 = indices[2];
		uint16_t* nextIndex = indices + 3;
		int32_t remaining = vertexCount - 3;

		SubmitSortedTriangle(renderFlags, renderEntry, 0, &vertices[indices[0]], &vertices[index1], &vertices[index2]);

		if (vertexCount != 3)
		{
			do
			{
				int32_t previous = index1;
				index1 = index2;
				--remaining;
				index2 = *nextIndex++;

				// strips alternate their winding order, so every second triangle is flipped
				if ((remaining & 1) != 0)
					SubmitSortedTriangle(renderFlags, renderEntry, 0, &vertices[previous], &vertices[index1], &vertices[index2]);
				else
					SubmitSortedTriangle(renderFlags, renderEntry, 0, &vertices[index2], &vertices[index1], &vertices[previous]);
			} while (remaining);
		}

		return DrawingAPI::UnlockVertexBuffer(buffer);
	}

	// STUB: TOY2 0x00452130
	void SwapRenderBuffer() {}

	// FUNCTION: TOY2 0x004C20E0
	void SetLevelFileIndex(int32_t index) { g_levelFileIndex = index; }

	// FUNCTION: TOY2 0x004C1C40 [MATCHED]
	void InitialiseColourScaleTables()
	{
		g_colourScaleTables = (uint16_t*)malloc(0x80000);
		g_blueScaleTable = g_colourScaleTables;
		g_greenScaleTable = g_blueScaleTable + 65536;
		g_redScaleTable = g_greenScaleTable + 65536;
		g_unkE4D7B8 = g_redScaleTable + 65536;

		int32_t index = 0;

		if (! g_pixelMode)
		{
			for (int32_t row = 0; row < 256; ++row)
			{
				for (int32_t column = 0; column < 256; ++column)
				{
					uint32_t product = row * column;

					if (product > 0xFFFF)
						product = 0xFFFF;

					g_blueScaleTable[index++] = (uint16_t)((product >> 11) & 0x1F);
				}
			}

			index = 0;

			for (row = 0; row < 256; ++row)
			{
				for (int32_t column = 0; column < 256; ++column)
				{
					uint32_t product = row * column;

					if (product > 0xFFFF)
						product = 0xFFFF;

					g_greenScaleTable[index++] = (uint16_t)((product >> 6) & 0x3E0);
				}
			}

			index = 0;

			for (row = 0; row < 256; ++row)
			{
				for (int32_t column = 0; column < 256; ++column)
				{
					uint32_t product = row * column;

					if (product > 0xFFFF)
						product = 0xFFFF;

					g_redScaleTable[index++] = (uint16_t)((product >> 1) & 0x7C00);
				}
			}

			index = 0;

			for (row = 0; row < 256; ++row)
			{
				for (int32_t column = 0; column < 256; ++column)
				{
					uint32_t product = row * column;

					if (product > 0xFFFF)
						product = 0xFFFF;

					g_unkE4D7B8[index++] = (uint16_t)((product >> 11) & 0x1F);
				}
			}
		}
		else
		{
			index = 0;

			for (int32_t row = 0; row < 256; ++row)
			{
				for (int32_t column = 0; column < 256; ++column)
				{
					uint32_t product = row * column;

					if (product > 0xFFFF)
						product = 0xFFFF;

					g_blueScaleTable[index++] = (uint16_t)((product >> 11) & 0x1F);
				}
			}

			index = 0;

			for (row = 0; row < 256; ++row)
			{
				for (int32_t column = 0; column < 256; ++column)
				{
					uint32_t product = row * column;

					if (product > 0xFFFF)
						product = 0xFFFF;

					g_greenScaleTable[index++] = (uint16_t)((product >> 5) & 0x7C0);
				}
			}

			index = 0;

			for (row = 0; row < 256; ++row)
			{
				for (int32_t column = 0; column < 256; ++column)
				{
					uint32_t product = row * column;

					if (product > 0xFFFF)
						product = 0xFFFF;

					g_redScaleTable[index++] = (uint16_t)(product & 0xF800);
				}
			}

			index = 0;

			for (row = 0; row < 256; ++row)
			{
				for (int32_t column = 0; column < 256; ++column)
				{
					uint32_t product = row * column;

					if (product > 0xFFFF)
						product = 0xFFFF;

					g_unkE4D7B8[index++] = (uint16_t)((product >> 11) & 0x1F);
				}
			}
		}
	}

	// FUNCTION: TOY2 0x004C1E70 [MATCHED]
	void CommitZoom()
	{
		g_viewOffsetX = (float)g_leftOffset;
		g_viewOffsetY = (float)g_topOffset;
		g_viewScaleX = (float)((g_rightOffset - g_leftOffset + 1) * (1.0 / 320.0));
		g_viewScaleY = (float)((g_bottomOffset - g_topOffset + 1) * (1.0 / 220.0));
		g_zoomScaleX = (float)g_zoomedWidth / (float)g_surfaceWidth;
		g_zoomScaleY = (float)g_zoomedHeight / (float)g_surfaceHeight;
	}

	// FUNCTION: TOY2 0x004BCE00 [MATCHED]
	void InitialisePrimarySurface()
	{
		DDSURFACEDESC2 surfaceDesc;
		surfaceDesc.dwSize = sizeof(surfaceDesc);
		DrawingDevice::LockPrimarySurface(&surfaceDesc);

		g_primarySurfacePixels = (uint8_t*)surfaceDesc.lpSurface;

		int32_t top = 0;
		int32_t bottom = surfaceDesc.dwHeight - 1;
		int32_t left = 0;
		int32_t right = surfaceDesc.dwWidth - 1;

		g_surfacePitch = surfaceDesc.lPitch;
		g_surfaceWidth = surfaceDesc.dwWidth;
		g_surfaceHeight = surfaceDesc.dwHeight;

		if (bottom == 199)
		{
			left = 14;
			right = 306;
			g_zoomedWidth = 292;
		}

		if (surfaceDesc.ddpfPixelFormat.dwRGBBitCount == 16)
			g_pixelMode = surfaceDesc.ddpfPixelFormat.dwGBitMask != 0x3E0;
		else if (surfaceDesc.ddpfPixelFormat.dwBBitMask == 0xFF)
			g_pixelMode = 2;
		else if (surfaceDesc.ddpfPixelFormat.dwRBitMask == 0xFF)
			g_pixelMode = 3;

		DrawingDevice::UnlockPrimarySurface();

		g_clipTop = top;
		g_clipBottom = bottom;
		g_clipLeft = left;
		g_clipRight = right;
		g_topOffset = top;
		g_leftOffset = left;
		g_bottomOffset = bottom;
		g_rightOffset = right;
		g_zoomedWidth = g_surfaceWidth;
		g_zoomedHeight = g_surfaceHeight;
		CommitZoom();

		g_zoomLevel = 0;

		if (g_frameBuffer)
		{
			free(g_frameBuffer);
			g_frameBuffer = 0;
		}

		if (g_colourScaleTables)
			free(g_colourScaleTables);

		g_frameBuffer = (uint8_t*)malloc(surfaceDesc.dwHeight * surfaceDesc.lPitch);
		InitialiseColourScaleTables();
	}

	// FUNCTION: TOY2 0x004C1E60 [MATCHED]
	void InitialisePrimarySurface_T() { InitialisePrimarySurface(); }

	// STUB: TOY2 0x0047D0F0
	void Destroy() {}

	// FUNCTION: TOY2 0x004C1FC0 [MATCHED]
	void ZoomOut()
	{
		if (g_zoomLevel >= 10)
		{
			g_zoomLevel = 10;
			return;
		}

		++g_zoomLevel;
		g_zoomedWidth -= 16;
		g_zoomedHeight -= 12;
		g_leftOffset += 8;
		g_rightOffset -= 8;
		g_topOffset += 6;
		g_bottomOffset -= 6;
		g_clipLeft += 8;
		g_clipRight -= 8;
		g_clipTop += 6;
		g_clipBottom -= 6;
		CommitZoom();
	}

	// FUNCTION: TOY2 0x004C1F00 [MATCHED]
	void ZoomIn()
	{
		if (g_zoomLevel <= 0)
		{
			g_zoomLevel = 0;
			return;
		}

		--g_zoomLevel;
		g_zoomedWidth += 16;
		g_zoomedHeight += 12;
		g_leftOffset -= 8;
		g_rightOffset += 8;
		g_topOffset -= 6;
		g_bottomOffset += 6;
		g_clipLeft -= 8;
		g_clipRight += 8;
		g_clipTop -= 6;
		g_clipBottom += 6;
		CommitZoom();
	}

	// STUB: TOY2 0x004C17B0
	void PresentFrame() {}

	// STUB: TOY2 0x00490410
	void UnkFunc67(int32_t param1, int32_t param2) {}

	// STUB: TOY2 0x0048FB70
	void UnkFunc2() {}

	// STUB: TOY2 0x00490290
	int16_t UnkFunc3() { return 0; }

	// STUB: TOY2 0x004BCBE0
	void UnkFunc31() {}

	// STUB: TOY2 0x004BCC40
	void UnkFunc32() {}

	// STUB: TOY2 0x004BCB60
	void UnkFunc33() {}

	// STUB: TOY2 0x00470C70
	void UnkFunc7() {}

	// FUNCTION: TOY2 0x004C1C20
	void SetCameraNearFarZ(float cameraNearZ, float cameraFarZ)
	{
		g_cameraNearZ = cameraNearZ;
		g_cameraFarZ = cameraFarZ;
	}

	// STUB: TOY2 0x0040BAE0
	int32_t ClearRenderSurfaces() { return 0; }

	// STUB: TOY2 0x0040CD80
	int32_t ShowBackBuffer() { return 0; }

	// STUB: TOY2 0x004319E0
	void UnkFunc11() {}

	// STUB: TOY2 0x00454D30
	int32_t UnkRenderAPI5(int32_t* a1) { return 0; }

	// STUB: TOY2 0x004560E0
	int32_t UnkRenderAPI6(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00457440
	int32_t UnkRenderAPI7(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00458770
	int32_t UnkRenderAPI8(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00459AB0
	uint32_t* UnkRenderAPI10(int32_t* a1) { return 0; }

	// STUB: TOY2 0x0045A5C0
	uint32_t* UnkRenderAPI11(int32_t* a1) { return 0; }

	// STUB: TOY2 0x0045B0B0
	uint32_t* UnkRenderAPI12(int32_t* a1) { return 0; }

	// STUB: TOY2 0x0045BBC0
	int32_t UnkRenderAPI9(int32_t* a1) { return 0; }

	// STUB: TOY2 0x0045C6B0
	uint32_t* UnkRenderAPI4(int32_t* a1) { return 0; }

	// STUB: TOY2 0x0045D110
	uint32_t* UnkRenderAPI3(int32_t* a1) { return 0; }

	// STUB: TOY2 0x0045DB80
	int32_t UnkRenderAPI1(int32_t* a1) { return 0; }

	// STUB: TOY2 0x0045E390
	int32_t UnkRenderAPI13(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00461D20
	int32_t UnkRenderAPI14(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00463090
	int32_t UnkRenderAPI21(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00463B80
	uint32_t* UnkRenderAPI22(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00464690
	uint32_t* UnkRenderAPI23(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00465180
	uint32_t* UnkRenderAPI24(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00467080
	int32_t UnkRenderAPI17(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00468430
	uint32_t* UnkRenderAPI16(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00468E90
	uint32_t* UnkRenderAPI15(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00469900
	int32_t UnkRenderAPI18(int32_t* a1) { return 0; }

	// STUB: TOY2 0x0046AC60
	int32_t UnkRenderAPI19(int32_t* a1) { return 0; }

	// STUB: TOY2 0x0046BF90
	int32_t UnkRenderAPI20(int32_t* a1) { return 0; }

	// STUB: TOY2 0x0046D2C0
	int32_t UnkRenderAPI2(int32_t* a1, int32_t* a2, int32_t* a3, int32_t* a4, int32_t a5) { return 0; }

	// STUB: TOY2 0x0046D7B0
	int32_t UnkRenderAPI25(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00470BF0
	void SetPaletteOnAPI() {}

	// STUB: TOY2 0x00470D00
	void UnkFunc9(int32_t a1) {}

	// STUB: TOY2 0x00470D60
	int32_t UnkFunc10() { return 0; }

	// STUB: TOY2 0x00470FB0
	char* UnkFunc15() { return 0; }

	// STUB: TOY2 0x004710C0
	void UnkFunc12(uint8_t* a1, int32_t a2) {}

	// STUB: TOY2 0x00471190
	char* UnkFunc13() { return 0; }

	// STUB: TOY2 0x00471250
	char* UnkFunc14() { return 0; }

	// STUB: TOY2 0x00471300
	void SetNewPalette(int32_t a1, int32_t a2) {}

	// STUB: TOY2 0x00471520
	int32_t UnkRenderAPI26(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00471B30
	int32_t UnkRenderAPI30(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00472E70
	int32_t UnkRenderAPI31(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00474190
	int32_t UnkRenderAPI32(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00474C80
	int32_t UnkRenderAPI33(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00476340
	int32_t UnkRenderAPI29(int32_t* a1) { return 0; }

	// STUB: TOY2 0x00476D00
	int32_t UnkRenderAPI28(int32_t* a1) { return 0; }

	// STUB: TOY2 0x004776C0
	int32_t UnkRenderAPI27(int32_t* a1, int32_t* a2, int32_t* a3, int32_t* a4, int32_t a5) { return 0; }

	// STUB: TOY2 0x00477EB0
	int32_t UnkRenderAPI34(int32_t* a1) { return 0; }

	// STUB: TOY2 0x0047C800
	void UnkFunc4() {}

	// STUB: TOY2 0x0047C870
	void UnlockBackBuffer() {}

	// STUB: TOY2 0x0047C8B0
	int32_t UnkFunc1() { return 0; }

	// STUB: TOY2 0x0047D120
	void UnkFunc5() {}

	// STUB: TOY2 0x0047D210
	void UnkFunc8() {}

	// STUB: TOY2 0x0047D540
	int32_t UnkFunc6(uint32_t* a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5) { return 0; }

	// STUB: TOY2 0x0047D650
	void UnkFunc66(SoftwareRenderer::UnkSoft2* a1, int32_t a2) {}

	// FUNCTION: TOY2 0x004AC1F0
	int32_t GetCurrentTexture(TextureSnapshot* texture)
	{
		if (Nu3D::g_currentBmpDataNode)
		{
			uint32_t* texData = Nu3D::g_currentBmpDataNode->texData;
			texture->surfaceDesc = &Nu3D::g_currentBmpDataNode->surfaceDesc;
			texture->texData = texData;
			return 0;
		}

		return 1;
	}

	// FUNCTION: TOY2 0x004B2B80 [MATCHED]
	int32_t GetStrideFromFVF(int32_t fvf)
	{
		switch (fvf)
		{
			case D3DFVF_0x152:
				return 36;
			case D3DFVF_VERTEX:
			case D3DFVF_0x1C4:
				return 32;
			default:
				return 0;
		}
	}

	// FUNCTION: TOY2 0x004B6140
	void SubmitTriangleStripRaw(int32_t renderFlags, LPVOID vertexData, Renderer::RenderEntry* renderEntry, uint16_t* indices, int32_t vertexCount)
	{
		Nu3D::VertexTL* vertices = (Nu3D::VertexTL*)vertexData;

		int32_t index1 = indices[1];
		int32_t index2 = indices[2];
		int32_t remaining = vertexCount - 3;
		uint16_t* nextIndex = indices + 3;

		SubmitSortedTriangle(renderFlags, renderEntry, 0, &vertices[indices[0]], &vertices[index1], &vertices[index2]);

		while (remaining)
		{
			int32_t previous = index1;

			index1 = index2;
			index2 = *nextIndex++;

			// strips alternate their winding order, so every second triangle is flipped
			if ((--remaining & 1) != 0)
				SubmitSortedTriangle(renderFlags, renderEntry, 0, &vertices[previous], &vertices[index1], &vertices[index2]);
			else
				SubmitSortedTriangle(renderFlags, renderEntry, 0, &vertices[index2], &vertices[index1], &vertices[previous]);
		}
	}

	// FUNCTION: TOY2 0x004B6220
	HRESULT SubmitQuad(int32_t renderFlags, int32_t texIndex, LPDIRECT3DVERTEXBUFFER buffer, uint16_t* indices)
	{
		LPVOID data;

		HRESULT result = DrawingAPI::LockVertexBuffer(buffer, 0x801, &data, 0);

		if (! result)
		{
			Nu3D::VertexTL* vertices = (Nu3D::VertexTL*)data;

			SubmitSortedTriangle(renderFlags, 0, texIndex, &vertices[indices[0]], &vertices[indices[1]], &vertices[indices[2]]);
			SubmitSortedTriangle(renderFlags, 0, texIndex, &vertices[2], &vertices[indices[1]], &vertices[indices[3]]);

			return DrawingAPI::UnlockVertexBuffer(buffer);
		}

		return result;
	}

	// FUNCTION: TOY2 0x004BC430 [MATCHED]
	void GetRenderDistance(float* nearDistance, float* farDistance)
	{
		if (nearDistance)
			*nearDistance = (float)sqrt(Renderer::g_nearRenderDistance);

		if (farDistance)
			*farDistance = (float)sqrt(Renderer::g_farRenderDistance);
	}

	// STUB: TOY2 0x004BC900
	uint32_t UnkFunc27(int32_t a1, float* a2, float* a3, float* a4, uint32_t* a5) { return 0; }

	// STUB: TOY2 0x004BC980
	void UnkFunc30(const void** a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5) {}

	// STUB: TOY2 0x004BCAD0
	void UnkFunc29(const void** a1, int32_t a2, int32_t a3, int32_t a4) {}

	// STUB: TOY2 0x004BCC70
	void UnkFunc17(int32_t a1, int32_t a2, int32_t a3, int32_t a4) {}

	// STUB: TOY2 0x004BCF60
	BOOL UnkFunc28(float* a1, float* a2, float* a3) { return 0; }

	// STUB: TOY2 0x004BCFA0
	void UnkFunc26(int32_t a1) {}

	// STUB: TOY2 0x004BD040
	void UnkFunc24(int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6) {}

	// STUB: TOY2 0x004BE0B0
	void UnkFunc25(float** a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5) {}

	// STUB: TOY2 0x004C0100
	int32_t UnkFunc23(float** a1, int32_t a2) { return 0; }

	// STUB: TOY2 0x004C0320
	void UnkFunc22(int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6) {}

	// STUB: TOY2 0x004C14A0
	void UnkFunc19(int32_t a1, uint16_t* a2, uint32_t a3) {}

	// STUB: TOY2 0x004C1540
	void UnkFunc21(int32_t a1, uint16_t* a2, int32_t a3) {}

	// STUB: TOY2 0x004C1720
	void UnkFunc16(int32_t a1, int32_t a2, uint16_t* a3, uint32_t a4) {}

	// FUNCTION: TOY2 0x004C4340 [MATCHED]
	uint32_t* UnkFunc59(int32_t packedColour, uint32_t* red, int32_t* green, uint32_t* blue)
	{
		*red = ((uint32_t)packedColour >> 16 & 0xFF) << 8;
		*green = packedColour & 0xFF00;
		*blue = (packedColour & 0xFF) << 8;
		return blue;
	}

	// STUB: TOY2 0x004C4370
	int16_t UnkFunc48(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C4640
	int16_t UnkFunc47(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// FUNCTION: TOY2 0x004C48E0 [MATCHED]
	__declspec(naked) void __cdecl UnkFunc55(
		BVertex* edgeA,
		BVertex* edgeB,
		uint16_t* row,
		uint32_t* texture,
		int32_t redA,
		int32_t greenA,
		int32_t blueA,
		int32_t redB,
		int32_t greenB,
		int32_t blueB)
	{
		__asm
		{
			push ebp
			mov ebp, esp
			sub esp, 8
			push ebx
			push esi
			push edi
			mov edi, dword ptr [ebp + 8]
			fld dword ptr [edi]
			call _ftol
			mov ebx, dword ptr [ebp + 0xC]
			mov esi, eax
			fld dword ptr [ebx]
			call _ftol
			sub esi, eax
			mov dword ptr [ebp + 0xC], esi
			je LBL_SPAN_DONE
			test esi, esi
			jge LBL_ORDERED_ENDPOINTS
			mov ecx, dword ptr [ebp + 0x18]
			mov edx, dword ptr [ebp + 0x28]
			mov eax, edi
			mov edi, ebx
			mov ebx, eax
			mov eax, dword ptr [ebp + 0x24]
			mov dword ptr [ebp + 0x24], ecx
			mov ecx, dword ptr [ebp + 0x1C]
			mov dword ptr [ebp + 0x1C], edx
			mov edx, dword ptr [ebp + 0x2C]
			mov dword ptr [ebp + 0x28], ecx
			mov ecx, dword ptr [ebp + 0x20]
			neg esi
			mov dword ptr [ebp + 0xC], esi
			mov dword ptr [ebp + 8], ecx
			mov dword ptr [ebp + 0x20], edx
			mov dword ptr [ebp + 0x2C], ecx
			jmp LBL_CHECK_WIDTH
		LBL_ORDERED_ENDPOINTS:
			mov eax, dword ptr [ebp + 0x18]
		LBL_CHECK_WIDTH:
			cmp esi, 1
			jl LBL_SPAN_DONE
			sar esi, 1
			test esi, esi
			mov dword ptr [ebp + 0x18], esi
			jle LBL_LOAD_ENDPOINTS
			mov ecx, dword ptr [ebp + 0x24]
			sub eax, ecx
			mov ecx, dword ptr [ebp + 0x2C]
			cdq
			idiv esi
			mov edx, dword ptr [ebp + 0x28]
			mov dword ptr [ebp + 8], eax
			mov eax, dword ptr [ebp + 0x1C]
			sub eax, edx
			cdq
			idiv esi
			mov dword ptr [ebp - 4], eax
			mov eax, dword ptr [ebp + 0x20]
			sub eax, ecx
			cdq
			idiv esi
			mov dword ptr [ebp + 0x20], eax
		LBL_LOAD_ENDPOINTS:
			fld dword ptr [ebx]
			call _ftol
			fld dword ptr [edi]
			mov esi, eax
			mov dword ptr [ebp - 8], esi
			call _ftol
			mov dword ptr [ebp + 0x1C], eax
			mov eax, dword ptr [ebp + 0x10]
			lea ecx, [eax + esi * 2]
			mov dword ptr [ebp + 0x10], ecx
			mov eax, dword ptr [ebp - 8]
			and eax, 1
			je LBL_DRAW_PAIRS
			mov esi, dword ptr [ebp + 0x2C]
			and esi, 0xF800
			shr esi, 11
			mov eax, dword ptr [ebp + 0x28]
			and eax, 0xF800
			shr eax, 6
			add esi, eax
			mov eax, dword ptr [ebp + 0x24]
			and eax, 0xF800
			shr eax, 1
			add eax, esi
			mov ecx, dword ptr [ebp + 0x10]
			mov word ptr [ecx], ax
			add ecx, 2
			mov dword ptr [ebp + 0x10], ecx
			mov eax, dword ptr [ebp + 0xC]
			dec eax
			shr eax, 1
			mov dword ptr [ebp + 0x18], eax
		LBL_DRAW_PAIRS:
			mov edi, dword ptr [ebp + 0x10]
			mov ebx, dword ptr [ebp + 0x2C]
			mov ecx, dword ptr [ebp + 0x28]
			mov edx, dword ptr [ebp + 0x24]
			mov eax, dword ptr [ebp + 0x18]
			and eax, eax
			je LBL_DRAW_TAIL
		LBL_PAIR_LOOP:
			push eax
			mov esi, ebx
			shr esi, 11
			mov eax, ecx
			and eax, 0xF800
			shr eax, 6
			add esi, eax
			mov eax, edx
			and eax, 0xF800
			shr eax, 1
			add eax, esi
			push ax
			shl eax, 16
			pop ax
			mov dword ptr [edi], eax
			add edi, 4
			add ebx, dword ptr [ebp + 0x20]
			add ecx, dword ptr [ebp - 4]
			add edx, dword ptr [ebp + 8]
			pop eax
			dec eax
			jne LBL_PAIR_LOOP
		LBL_DRAW_TAIL:
			mov eax, dword ptr [ebp + 0x1C]
			and eax, 1
			je LBL_SPAN_DONE
			mov esi, ebx
			and esi, 0xF800
			shr esi, 11
			mov eax, ecx
			and eax, 0xF800
			shr eax, 6
			add esi, eax
			mov eax, edx
			and eax, 0xF800
			shr eax, 1
			add eax, esi
			mov word ptr [edi], ax
		LBL_SPAN_DONE:
			pop edi
			pop esi
			pop ebx
			mov esp, ebp
			pop ebp
			ret
		}
	}

	// FUNCTION: TOY2 0x004C4A60 [MATCHED]
	__declspec(naked) void __cdecl UnkFunc40(
		BVertex* edgeA,
		BVertex* edgeB,
		uint16_t* row,
		uint32_t* texture,
		int32_t redA,
		int32_t greenA,
		int32_t blueA,
		int32_t redB,
		int32_t greenB,
		int32_t blueB)
	{
		__asm
		{
			push ebp
			mov ebp, esp
			sub esp, 8
			push ebx
			push esi
			push edi
			mov edi, dword ptr [ebp + 8]
			fld dword ptr [edi]
			call _ftol
			mov ebx, dword ptr [ebp + 0xC]
			mov esi, eax
			fld dword ptr [ebx]
			call _ftol
			sub esi, eax
			mov dword ptr [ebp + 0xC], esi
			je LBL_SPAN_DONE
			test esi, esi
			jge LBL_ORDERED_ENDPOINTS
			mov ecx, dword ptr [ebp + 0x18]
			mov edx, dword ptr [ebp + 0x28]
			mov eax, edi
			mov edi, ebx
			mov ebx, eax
			mov eax, dword ptr [ebp + 0x24]
			mov dword ptr [ebp + 0x24], ecx
			mov ecx, dword ptr [ebp + 0x1C]
			mov dword ptr [ebp + 0x1C], edx
			mov edx, dword ptr [ebp + 0x2C]
			mov dword ptr [ebp + 0x28], ecx
			mov ecx, dword ptr [ebp + 0x20]
			neg esi
			mov dword ptr [ebp + 0xC], esi
			mov dword ptr [ebp + 8], ecx
			mov dword ptr [ebp + 0x20], edx
			mov dword ptr [ebp + 0x2C], ecx
			jmp LBL_CHECK_WIDTH
		LBL_ORDERED_ENDPOINTS:
			mov eax, dword ptr [ebp + 0x18]
		LBL_CHECK_WIDTH:
			cmp esi, 1
			jl LBL_SPAN_DONE
			sar esi, 1
			test esi, esi
			mov dword ptr [ebp + 0x18], esi
			jle LBL_LOAD_ENDPOINTS
			mov ecx, dword ptr [ebp + 0x24]
			sub eax, ecx
			mov ecx, dword ptr [ebp + 0x2C]
			cdq
			idiv esi
			mov edx, dword ptr [ebp + 0x28]
			mov dword ptr [ebp + 8], eax
			mov eax, dword ptr [ebp + 0x1C]
			sub eax, edx
			cdq
			idiv esi
			mov dword ptr [ebp - 4], eax
			mov eax, dword ptr [ebp + 0x20]
			sub eax, ecx
			cdq
			idiv esi
			mov dword ptr [ebp + 0x20], eax
		LBL_LOAD_ENDPOINTS:
			fld dword ptr [ebx]
			call _ftol
			fld dword ptr [edi]
			mov esi, eax
			mov dword ptr [ebp - 8], esi
			call _ftol
			mov dword ptr [ebp + 0x1C], eax
			mov eax, dword ptr [ebp + 0x10]
			lea ecx, [eax + esi * 2]
			mov dword ptr [ebp + 0x10], ecx
			mov eax, dword ptr [ebp - 8]
			and eax, 1
			je LBL_DRAW_PAIRS
			mov esi, dword ptr [ebp + 0x2C]
			and esi, 0xF800
			shr esi, 11
			mov eax, dword ptr [ebp + 0x28]
			and eax, 0xF800
			shr eax, 5
			add esi, eax
			mov eax, dword ptr [ebp + 0x24]
			and eax, 0xF800
			add eax, esi
			mov ecx, dword ptr [ebp + 0x10]
			mov word ptr [ecx], ax
			add ecx, 2
			mov dword ptr [ebp + 0x10], ecx
			mov eax, dword ptr [ebp + 0xC]
			dec eax
			shr eax, 1
			mov dword ptr [ebp + 0x18], eax
		LBL_DRAW_PAIRS:
			mov edi, dword ptr [ebp + 0x10]
			mov ebx, dword ptr [ebp + 0x2C]
			mov ecx, dword ptr [ebp + 0x28]
			mov edx, dword ptr [ebp + 0x24]
			mov eax, dword ptr [ebp + 0x18]
			and eax, eax
			je LBL_DRAW_TAIL
		LBL_PAIR_LOOP:
			push eax
			mov esi, ebx
			shr esi, 11
			mov eax, ecx
			and eax, 0xF800
			shr eax, 5
			add esi, eax
			mov eax, edx
			and eax, 0xF800
			add eax, esi
			push ax
			shl eax, 16
			pop ax
			mov dword ptr [edi], eax
			add edi, 4
			add ebx, dword ptr [ebp + 0x20]
			add ecx, dword ptr [ebp - 4]
			add edx, dword ptr [ebp + 8]
			pop eax
			dec eax
			jne LBL_PAIR_LOOP
		LBL_DRAW_TAIL:
			mov eax, dword ptr [ebp + 0x1C]
			and eax, 1
			je LBL_SPAN_DONE
			mov esi, ebx
			and esi, 0xF800
			shr esi, 11
			mov eax, ecx
			and eax, 0xF800
			shr eax, 5
			add esi, eax
			mov eax, edx
			and eax, 0xF800
			add eax, esi
			mov word ptr [edi], ax
		LBL_SPAN_DONE:
			pop edi
			pop esi
			pop ebx
			mov esp, ebp
			pop ebp
			ret
		}
	}

	// STUB: TOY2 0x004C4BE0
	int32_t UnkFunc49(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// FUNCTION: TOY2 0x004C4E00 [MATCHED]
	__declspec(naked) void __cdecl UnkFunc52(
		BVertex* edgeA,
		BVertex* edgeB,
		uint16_t* row,
		uint32_t* texture,
		int32_t redA,
		int32_t greenA,
		int32_t blueA,
		int32_t redB,
		int32_t greenB,
		int32_t blueB)
	{
		__asm
		{
			push ebp
			mov ebp, esp
			push ebx
			push esi
			push edi
			mov edi, dword ptr [ebp + 8]
			fld dword ptr [edi]
			call _ftol
			mov esi, eax
			mov eax, dword ptr [ebp + 0xC]
			fld dword ptr [eax]
			call _ftol
			sub esi, eax
			mov dword ptr [ebp + 8], esi
			je LBL_SPAN_DONE
			test esi, esi
			jge LBL_ORDERED_ENDPOINTS
			mov ecx, dword ptr [ebp + 0x18]
			mov eax, dword ptr [ebp + 0x24]
			mov ebx, dword ptr [ebp + 0x28]
			mov dword ptr [ebp + 0x24], ecx
			mov ecx, dword ptr [ebp + 0x1C]
			mov dword ptr [ebp + 0xC], edi
			mov edi, dword ptr [ebp + 0x2C]
			mov dword ptr [ebp + 0x28], ecx
			mov ecx, dword ptr [ebp + 0x20]
			neg esi
			mov dword ptr [ebp + 8], esi
			mov dword ptr [ebp + 0x2C], ecx
			jmp LBL_CALCULATE_SLOPES
		LBL_ORDERED_ENDPOINTS:
			mov edi, dword ptr [ebp + 0x20]
			mov ebx, dword ptr [ebp + 0x1C]
			mov eax, dword ptr [ebp + 0x18]
		LBL_CALCULATE_SLOPES:
			cmp esi, 1
			jl LBL_SPAN_DONE
			mov ecx, dword ptr [ebp + 0x24]
			sub eax, ecx
			mov ecx, dword ptr [ebp + 0x2C]
			cdq
			idiv esi
			mov edx, dword ptr [ebp + 0x28]
			mov dword ptr [ebp + 0x18], eax
			mov eax, ebx
			sub eax, edx
			cdq
			idiv esi
			mov dword ptr [ebp + 0x1C], eax
			mov eax, edi
			sub eax, ecx
			mov ecx, dword ptr [ebp + 0xC]
			cdq
			fld dword ptr [ecx]
			idiv esi
			mov dword ptr [ebp + 0x20], eax
			call _ftol
			mov edx, dword ptr [ebp + 0x10]
			lea eax, [edx + eax * 2]
			mov dword ptr [ebp + 0x10], eax
			mov edi, g_unkE4D7B8
			mov eax, dword ptr [ebp + 8]
		LBL_PIXEL_LOOP:
			push eax
			mov ecx, dword ptr [ebp + 0x10]
			mov eax, dword ptr [ecx]
			push eax
			mov ecx, dword ptr [ebp + 0x2C]
			shr ecx, 11
			and eax, 0x1F
			add eax, ecx
			cmp eax, 0x20
			jl LBL_BLUE_READY
			mov eax, 0x1F
		LBL_BLUE_READY:
			mov dword ptr [ebp + 0xC], eax
			mov ecx, dword ptr [ebp + 0x28]
			shr ecx, 11
			pop eax
			push eax
			shr eax, 5
			and eax, 0x1F
			add eax, ecx
			cmp eax, 0x20
			jl LBL_GREEN_READY
			mov eax, 0x1F
		LBL_GREEN_READY:
			shl eax, 5
			add dword ptr [ebp + 0xC], eax
			mov ecx, dword ptr [ebp + 0x24]
			shr ecx, 11
			pop eax
			shr eax, 10
			and eax, 0x1F
			add eax, ecx
			cmp eax, 0x20
			jl LBL_RED_READY
			mov eax, 0x1F
		LBL_RED_READY:
			shl eax, 10
			add eax, dword ptr [ebp + 0xC]
			mov ecx, dword ptr [ebp + 0x10]
			mov word ptr [ecx], ax
			add dword ptr [ebp + 0x10], 2
			mov eax, dword ptr [ebp + 0x18]
			add dword ptr [ebp + 0x24], eax
			mov eax, dword ptr [ebp + 0x1C]
			add dword ptr [ebp + 0x28], eax
			mov eax, dword ptr [ebp + 0x20]
			add dword ptr [ebp + 0x2C], eax
			pop eax
			dec eax
			jne LBL_PIXEL_LOOP
		LBL_SPAN_DONE:
			pop edi
			pop esi
			pop ebx
			pop ebp
			ret
		}
	}

	// FUNCTION: TOY2 0x004C4F30 [MATCHED]
	__declspec(naked) void __cdecl UnkFunc36(
		BVertex* edgeA,
		BVertex* edgeB,
		uint16_t* row,
		uint32_t* texture,
		int32_t redA,
		int32_t greenA,
		int32_t blueA,
		int32_t redB,
		int32_t greenB,
		int32_t blueB)
	{
		__asm
		{
			push ebp
			mov ebp, esp
			push ebx
			push esi
			push edi
			mov edi, dword ptr [ebp + 8]
			fld dword ptr [edi]
			call _ftol
			mov esi, eax
			mov eax, dword ptr [ebp + 0xC]
			fld dword ptr [eax]
			call _ftol
			sub esi, eax
			mov dword ptr [ebp + 8], esi
			je LBL_SPAN_DONE
			test esi, esi
			jge LBL_ORDERED_ENDPOINTS
			mov ecx, dword ptr [ebp + 0x18]
			mov eax, dword ptr [ebp + 0x24]
			mov ebx, dword ptr [ebp + 0x28]
			mov dword ptr [ebp + 0x24], ecx
			mov ecx, dword ptr [ebp + 0x1C]
			mov dword ptr [ebp + 0xC], edi
			mov edi, dword ptr [ebp + 0x2C]
			mov dword ptr [ebp + 0x28], ecx
			mov ecx, dword ptr [ebp + 0x20]
			neg esi
			mov dword ptr [ebp + 8], esi
			mov dword ptr [ebp + 0x2C], ecx
			jmp LBL_CALCULATE_SLOPES
		LBL_ORDERED_ENDPOINTS:
			mov edi, dword ptr [ebp + 0x20]
			mov ebx, dword ptr [ebp + 0x1C]
			mov eax, dword ptr [ebp + 0x18]
		LBL_CALCULATE_SLOPES:
			cmp esi, 1
			jl LBL_SPAN_DONE
			mov ecx, dword ptr [ebp + 0x24]
			sub eax, ecx
			mov ecx, dword ptr [ebp + 0x2C]
			cdq
			idiv esi
			mov edx, dword ptr [ebp + 0x28]
			mov dword ptr [ebp + 0x18], eax
			mov eax, ebx
			sub eax, edx
			cdq
			idiv esi
			mov dword ptr [ebp + 0x1C], eax
			mov eax, edi
			sub eax, ecx
			mov ecx, dword ptr [ebp + 0xC]
			cdq
			fld dword ptr [ecx]
			idiv esi
			mov dword ptr [ebp + 0x20], eax
			call _ftol
			mov edx, dword ptr [ebp + 0x10]
			lea eax, [edx + eax * 2]
			mov dword ptr [ebp + 0x10], eax
			mov edi, g_unkE4D7B8
			mov eax, dword ptr [ebp + 8]
		LBL_PIXEL_LOOP:
			push eax
			mov ecx, dword ptr [ebp + 0x10]
			mov eax, dword ptr [ecx]
			push eax
			mov ecx, dword ptr [ebp + 0x2C]
			shr ecx, 11
			and eax, 0x1F
			add eax, ecx
			cmp eax, 0x20
			jl LBL_BLUE_READY
			mov eax, 0x1F
		LBL_BLUE_READY:
			mov dword ptr [ebp + 0xC], eax
			mov ecx, dword ptr [ebp + 0x28]
			shr ecx, 11
			pop eax
			push eax
			shr eax, 6
			and eax, 0x1F
			add eax, ecx
			cmp eax, 0x20
			jl LBL_GREEN_READY
			mov eax, 0x1F
		LBL_GREEN_READY:
			shl eax, 6
			add dword ptr [ebp + 0xC], eax
			mov ecx, dword ptr [ebp + 0x24]
			shr ecx, 11
			pop eax
			shr eax, 11
			and eax, 0x1F
			add eax, ecx
			cmp eax, 0x20
			jl LBL_RED_READY
			mov eax, 0x1F
		LBL_RED_READY:
			shl eax, 11
			add eax, dword ptr [ebp + 0xC]
			mov ecx, dword ptr [ebp + 0x10]
			mov word ptr [ecx], ax
			add dword ptr [ebp + 0x10], 2
			mov eax, dword ptr [ebp + 0x18]
			add dword ptr [ebp + 0x24], eax
			mov eax, dword ptr [ebp + 0x1C]
			add dword ptr [ebp + 0x28], eax
			mov eax, dword ptr [ebp + 0x20]
			add dword ptr [ebp + 0x2C], eax
			pop eax
			dec eax
			jne LBL_PIXEL_LOOP
		LBL_SPAN_DONE:
			pop edi
			pop esi
			pop ebx
			pop ebp
			ret
		}
	}

	// STUB: TOY2 0x004C5060
	int32_t UnkFunc41(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C5280
	int32_t UnkFunc50(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// FUNCTION: TOY2 0x004C5490 [MATCHED]
	__declspec(naked) void __cdecl UnkFunc53(
		BVertex* edgeA,
		BVertex* edgeB,
		uint16_t* row,
		uint32_t* texture,
		int32_t redA,
		int32_t greenA,
		int32_t blueA,
		int32_t redB,
		int32_t greenB,
		int32_t blueB)
	{
		__asm
		{
			push ebp
			mov ebp, esp
			push ebx
			push esi
			push edi
			mov edi, dword ptr [ebp + 8]
			fld dword ptr [edi]
			call _ftol
			mov esi, eax
			mov eax, dword ptr [ebp + 0xC]
			fld dword ptr [eax]
			call _ftol
			sub esi, eax
			mov dword ptr [ebp + 8], esi
			je LBL_SPAN_DONE
			test esi, esi
			jge LBL_ORDERED_ENDPOINTS
			mov ecx, dword ptr [ebp + 0x18]
			mov eax, dword ptr [ebp + 0x24]
			mov ebx, dword ptr [ebp + 0x28]
			mov dword ptr [ebp + 0x24], ecx
			mov ecx, dword ptr [ebp + 0x1C]
			mov dword ptr [ebp + 0xC], edi
			mov edi, dword ptr [ebp + 0x2C]
			mov dword ptr [ebp + 0x28], ecx
			mov ecx, dword ptr [ebp + 0x20]
			neg esi
			mov dword ptr [ebp + 8], esi
			mov dword ptr [ebp + 0x2C], ecx
			jmp LBL_CALCULATE_SLOPES
		LBL_ORDERED_ENDPOINTS:
			mov edi, dword ptr [ebp + 0x20]
			mov ebx, dword ptr [ebp + 0x1C]
			mov eax, dword ptr [ebp + 0x18]
		LBL_CALCULATE_SLOPES:
			cmp esi, 1
			jl LBL_SPAN_DONE
			mov ecx, dword ptr [ebp + 0x24]
			sub eax, ecx
			mov ecx, dword ptr [ebp + 0x2C]
			cdq
			idiv esi
			mov edx, dword ptr [ebp + 0x28]
			mov dword ptr [ebp + 0x18], eax
			mov eax, ebx
			sub eax, edx
			cdq
			idiv esi
			mov dword ptr [ebp + 0x1C], eax
			mov eax, edi
			sub eax, ecx
			mov ecx, dword ptr [ebp + 0xC]
			cdq
			fld dword ptr [ecx]
			idiv esi
			mov dword ptr [ebp + 0x20], eax
			call _ftol
			mov edx, dword ptr [ebp + 0x10]
			lea eax, [edx + eax * 2]
			mov dword ptr [ebp + 0x10], eax
			mov edi, g_unkE4D7B8
			mov eax, dword ptr [ebp + 8]
		LBL_PIXEL_LOOP:
			push eax
			mov ecx, dword ptr [ebp + 0x10]
			mov ax, word ptr [ecx]
			push eax
			and eax, 0x1F
			movzx ecx, word ptr [ebp + 0x2C]
			shr ecx, 11
			sub eax, ecx
			jns LBL_BLUE_READY
			xor eax, eax
		LBL_BLUE_READY:
			mov dword ptr [ebp + 0xC], eax
			movzx ecx, word ptr [ebp + 0x28]
			shr ecx, 11
			pop eax
			push eax
			shr eax, 5
			and eax, 0x1F
			sub eax, ecx
			jns LBL_GREEN_READY
			xor eax, eax
		LBL_GREEN_READY:
			shl eax, 5
			add dword ptr [ebp + 0xC], eax
			movzx ecx, word ptr [ebp + 0x24]
			shr ecx, 11
			pop eax
			shr eax, 10
			and eax, 0x1F
			sub eax, ecx
			jns LBL_RED_READY
			xor eax, eax
		LBL_RED_READY:
			shl eax, 10
			add eax, dword ptr [ebp + 0xC]
			mov ecx, dword ptr [ebp + 0x10]
			mov word ptr [ecx], ax
			add dword ptr [ebp + 0x10], 2
			mov eax, dword ptr [ebp + 0x18]
			add dword ptr [ebp + 0x24], eax
			mov eax, dword ptr [ebp + 0x1C]
			add dword ptr [ebp + 0x28], eax
			mov eax, dword ptr [ebp + 0x20]
			add dword ptr [ebp + 0x2C], eax
			pop eax
			dec eax
			jne LBL_PIXEL_LOOP
		LBL_SPAN_DONE:
			pop edi
			pop esi
			pop ebx
			pop ebp
			ret
		}
	}

	// FUNCTION: TOY2 0x004C55B0 [MATCHED]
	__declspec(naked) void __cdecl UnkFunc37(
		BVertex* edgeA,
		BVertex* edgeB,
		uint16_t* row,
		uint32_t* texture,
		int32_t redA,
		int32_t greenA,
		int32_t blueA,
		int32_t redB,
		int32_t greenB,
		int32_t blueB)
	{
		__asm
		{
			push ebp
			mov ebp, esp
			push ebx
			push esi
			push edi
			mov edi, dword ptr [ebp + 8]
			fld dword ptr [edi]
			call _ftol
			mov esi, eax
			mov eax, dword ptr [ebp + 0xC]
			fld dword ptr [eax]
			call _ftol
			sub esi, eax
			mov dword ptr [ebp + 8], esi
			je LBL_SPAN_DONE
			test esi, esi
			jge LBL_ORDERED_ENDPOINTS
			mov ecx, dword ptr [ebp + 0x18]
			mov eax, dword ptr [ebp + 0x24]
			mov ebx, dword ptr [ebp + 0x28]
			mov dword ptr [ebp + 0x24], ecx
			mov ecx, dword ptr [ebp + 0x1C]
			mov dword ptr [ebp + 0xC], edi
			mov edi, dword ptr [ebp + 0x2C]
			mov dword ptr [ebp + 0x28], ecx
			mov ecx, dword ptr [ebp + 0x20]
			neg esi
			mov dword ptr [ebp + 8], esi
			mov dword ptr [ebp + 0x2C], ecx
			jmp LBL_CALCULATE_SLOPES
		LBL_ORDERED_ENDPOINTS:
			mov edi, dword ptr [ebp + 0x20]
			mov ebx, dword ptr [ebp + 0x1C]
			mov eax, dword ptr [ebp + 0x18]
		LBL_CALCULATE_SLOPES:
			cmp esi, 1
			jl LBL_SPAN_DONE
			mov ecx, dword ptr [ebp + 0x24]
			sub eax, ecx
			mov ecx, dword ptr [ebp + 0x2C]
			cdq
			idiv esi
			mov edx, dword ptr [ebp + 0x28]
			mov dword ptr [ebp + 0x18], eax
			mov eax, ebx
			sub eax, edx
			cdq
			idiv esi
			mov dword ptr [ebp + 0x1C], eax
			mov eax, edi
			sub eax, ecx
			mov ecx, dword ptr [ebp + 0xC]
			cdq
			fld dword ptr [ecx]
			idiv esi
			mov dword ptr [ebp + 0x20], eax
			call _ftol
			mov edx, dword ptr [ebp + 0x10]
			lea eax, [edx + eax * 2]
			mov dword ptr [ebp + 0x10], eax
			mov edi, g_unkE4D7B8
			mov eax, dword ptr [ebp + 8]
		LBL_PIXEL_LOOP:
			push eax
			mov ecx, dword ptr [ebp + 0x10]
			mov ax, word ptr [ecx]
			push eax
			and eax, 0x1F
			movzx ecx, word ptr [ebp + 0x2C]
			shr ecx, 11
			sub eax, ecx
			jns LBL_BLUE_READY
			xor eax, eax
		LBL_BLUE_READY:
			mov dword ptr [ebp + 0xC], eax
			movzx ecx, word ptr [ebp + 0x28]
			shr ecx, 11
			pop eax
			push eax
			shr eax, 6
			and eax, 0x1F
			sub eax, ecx
			jns LBL_GREEN_READY
			xor eax, eax
		LBL_GREEN_READY:
			shl eax, 6
			add dword ptr [ebp + 0xC], eax
			movzx ecx, word ptr [ebp + 0x24]
			shr ecx, 11
			pop eax
			shr eax, 11
			and eax, 0x1F
			sub eax, ecx
			jns LBL_RED_READY
			xor eax, eax
		LBL_RED_READY:
			shl eax, 11
			add eax, dword ptr [ebp + 0xC]
			mov ecx, dword ptr [ebp + 0x10]
			mov word ptr [ecx], ax
			add dword ptr [ebp + 0x10], 2
			mov eax, dword ptr [ebp + 0x18]
			add dword ptr [ebp + 0x24], eax
			mov eax, dword ptr [ebp + 0x1C]
			add dword ptr [ebp + 0x28], eax
			mov eax, dword ptr [ebp + 0x20]
			add dword ptr [ebp + 0x2C], eax
			pop eax
			dec eax
			jne LBL_PIXEL_LOOP
		LBL_SPAN_DONE:
			pop edi
			pop esi
			pop ebx
			pop ebp
			ret
		}
	}

	// STUB: TOY2 0x004C56D0
	int32_t UnkFunc42(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C58E0
	int32_t UnkFunc43(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C5AC0
	int32_t UnkFunc51(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// FUNCTION: TOY2 0x004C5D80 [MATCHED]
	__declspec(naked) void __cdecl UnkFunc56(
		BVertex* edgeA,
		BVertex* edgeB,
		uint16_t* row,
		uint32_t* texture,
		int32_t redA,
		int32_t greenA,
		int32_t blueA,
		int32_t redB,
		int32_t greenB,
		int32_t blueB)
	{
		__asm
		{
			push ebx
			push ebp
			push esi
			push edi
			mov edi, dword ptr [esp + 0x14]
			fld dword ptr [edi]
			call _ftol
			mov ebx, dword ptr [esp + 0x18]
			mov esi, eax
			fld dword ptr [ebx]
			call _ftol
			sub esi, eax
			jns LBL_FORWARD_SPAN
			mov eax, dword ptr [esp + 0x30]
			mov ecx, dword ptr [esp + 0x24]
			mov edx, dword ptr [esp + 0x28]
			mov ebp, dword ptr [esp + 0x2C]
			mov dword ptr [esp + 0x24], eax
			mov eax, dword ptr [esp + 0x34]
			mov ebx, edi
			mov edi, dword ptr [esp + 0x38]
			neg esi
			mov dword ptr [esp + 0x30], ecx
			mov dword ptr [esp + 0x34], edx
			mov dword ptr [esp + 0x28], eax
			mov dword ptr [esp + 0x38], ebp
			jmp LBL_CALCULATE_SLOPES
		LBL_FORWARD_SPAN:
			test esi, esi
			je LBL_SPAN_DONE
			mov ebp, dword ptr [esp + 0x38]
			mov edi, dword ptr [esp + 0x2C]
		LBL_CALCULATE_SLOPES:
			fld dword ptr [ebx]
			call _ftol
			mov ecx, dword ptr [esp + 0x1C]
			lea ebx, [ecx + eax * 2]
			mov eax, dword ptr [esp + 0x24]
			sub eax, dword ptr [esp + 0x30]
			cdq
			idiv esi
			mov edx, dword ptr [esp + 0x34]
			mov dword ptr [esp + 0x2C], eax
			mov eax, dword ptr [esp + 0x28]
			sub eax, edx
			cdq
			idiv esi
			mov dword ptr [esp + 0x28], eax
			mov eax, edi
			sub eax, ebp
			cdq
			idiv esi
			test esi, esi
			mov dword ptr [esp + 0x14], eax
			jle LBL_SPAN_DONE
			mov dword ptr [esp + 0x24], esi
		LBL_PIXEL_LOOP:
			mov edi, g_vertexAlpha
			xor ecx, ecx
			mov cx, word ptr [ebx]
			mov eax, g_vertexAlphaComplement
			mov esi, ecx
			mov edx, edi
			and esi, 0x1F
			imul edx, ebp
			shl esi, 3
			imul esi, eax
			shr edx, 8
			add esi, edx
			cmp esi, 0xF800
			jle LBL_BLUE_READY
			mov esi, 0xF800
		LBL_BLUE_READY:
			mov edx, ecx
			mov ebp, edi
			imul ebp, dword ptr [esp + 0x34]
			shr edx, 2
			and edx, 0xF8
			imul edx, eax
			shr ebp, 8
			add edx, ebp
			sar esi, 11
			cmp edx, 0xF800
			jle LBL_GREEN_READY
			mov edx, 0xF800
		LBL_GREEN_READY:
			shr ecx, 7
			and ecx, 0xF8
			imul ecx, eax
			mov eax, dword ptr [esp + 0x30]
			imul edi, eax
			shr edi, 8
			sar edx, 6
			add ecx, edi
			and edx, 0x3E0
			cmp ecx, 0xF800
			jle LBL_RED_READY
			mov ecx, 0xF800
		LBL_RED_READY:
			sar ecx, 1
			and ecx, 0x7C00
			mov ebp, dword ptr [esp + 0x38]
			add ecx, esi
			mov esi, dword ptr [esp + 0x2C]
			add eax, esi
			add ecx, edx
			mov edx, dword ptr [esp + 0x34]
			mov dword ptr [esp + 0x30], eax
			mov eax, dword ptr [esp + 0x28]
			mov word ptr [ebx], cx
			mov ecx, dword ptr [esp + 0x14]
			add edx, eax
			mov eax, dword ptr [esp + 0x24]
			add ebx, 2
			add ebp, ecx
			dec eax
			mov dword ptr [esp + 0x34], edx
			mov dword ptr [esp + 0x38], ebp
			mov dword ptr [esp + 0x24], eax
			jne LBL_PIXEL_LOOP
		LBL_SPAN_DONE:
			pop edi
			pop esi
			pop ebp
			pop ebx
			ret
		}
	}

	// FUNCTION: TOY2 0x004C5F00 [MATCHED]
	__declspec(naked) void __cdecl UnkFunc38(
		BVertex* edgeA,
		BVertex* edgeB,
		uint16_t* row,
		uint32_t* texture,
		int32_t redA,
		int32_t greenA,
		int32_t blueA,
		int32_t redB,
		int32_t greenB,
		int32_t blueB)
	{
		__asm
		{
			push ebx
			push ebp
			push esi
			push edi
			mov edi, dword ptr [esp + 0x14]
			fld dword ptr [edi]
			call _ftol
			mov ebx, dword ptr [esp + 0x18]
			mov esi, eax
			fld dword ptr [ebx]
			call _ftol
			sub esi, eax
			jns LBL_FORWARD_SPAN
			mov eax, dword ptr [esp + 0x30]
			mov ecx, dword ptr [esp + 0x24]
			mov edx, dword ptr [esp + 0x28]
			mov ebp, dword ptr [esp + 0x2C]
			mov dword ptr [esp + 0x24], eax
			mov eax, dword ptr [esp + 0x34]
			mov ebx, edi
			mov edi, dword ptr [esp + 0x38]
			neg esi
			mov dword ptr [esp + 0x30], ecx
			mov dword ptr [esp + 0x34], edx
			mov dword ptr [esp + 0x28], eax
			mov dword ptr [esp + 0x38], ebp
			jmp LBL_CALCULATE_SLOPES
		LBL_FORWARD_SPAN:
			test esi, esi
			je LBL_SPAN_DONE
			mov ebp, dword ptr [esp + 0x38]
			mov edi, dword ptr [esp + 0x2C]
		LBL_CALCULATE_SLOPES:
			fld dword ptr [ebx]
			call _ftol
			mov ecx, dword ptr [esp + 0x1C]
			lea ebx, [ecx + eax * 2]
			mov eax, dword ptr [esp + 0x24]
			sub eax, dword ptr [esp + 0x30]
			cdq
			idiv esi
			mov edx, dword ptr [esp + 0x34]
			mov dword ptr [esp + 0x2C], eax
			mov eax, dword ptr [esp + 0x28]
			sub eax, edx
			cdq
			idiv esi
			mov dword ptr [esp + 0x28], eax
			mov eax, edi
			sub eax, ebp
			cdq
			idiv esi
			test esi, esi
			mov dword ptr [esp + 0x14], eax
			jle LBL_SPAN_DONE
			mov dword ptr [esp + 0x24], esi
		LBL_PIXEL_LOOP:
			mov edi, g_vertexAlpha
			xor ecx, ecx
			mov cx, word ptr [ebx]
			mov eax, g_vertexAlphaComplement
			mov esi, ecx
			mov edx, edi
			and esi, 0x1F
			imul edx, ebp
			shl esi, 3
			imul esi, eax
			shr edx, 8
			add esi, edx
			cmp esi, 0xF800
			jle LBL_BLUE_READY
			mov esi, 0xF800
		LBL_BLUE_READY:
			mov edx, ecx
			mov ebp, edi
			imul ebp, dword ptr [esp + 0x34]
			shr edx, 3
			and edx, 0xF8
			imul edx, eax
			shr ebp, 8
			add edx, ebp
			sar esi, 11
			cmp edx, 0xF800
			jle LBL_GREEN_READY
			mov edx, 0xF800
		LBL_GREEN_READY:
			shr ecx, 8
			and ecx, 0xF8
			imul ecx, eax
			mov eax, dword ptr [esp + 0x30]
			imul edi, eax
			shr edi, 8
			sar edx, 5
			add ecx, edi
			and edx, 0x7C0
			cmp ecx, 0xF800
			jle LBL_RED_READY
			mov ecx, 0xF800
		LBL_RED_READY:
			and ecx, 0xF800
			mov ebp, dword ptr [esp + 0x38]
			add ecx, esi
			mov esi, dword ptr [esp + 0x2C]
			add eax, esi
			add ecx, edx
			mov edx, dword ptr [esp + 0x34]
			mov dword ptr [esp + 0x30], eax
			mov eax, dword ptr [esp + 0x28]
			mov word ptr [ebx], cx
			mov ecx, dword ptr [esp + 0x14]
			add edx, eax
			mov eax, dword ptr [esp + 0x24]
			add ebx, 2
			add ebp, ecx
			dec eax
			mov dword ptr [esp + 0x34], edx
			mov dword ptr [esp + 0x38], ebp
			mov dword ptr [esp + 0x24], eax
			jne LBL_PIXEL_LOOP
		LBL_SPAN_DONE:
			pop edi
			pop esi
			pop ebp
			pop ebx
			ret
		}
	}

	// STUB: TOY2 0x004C6080
	int32_t UnkFunc44(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C6340
	uint32_t* UnkFunc57(float* a1, int32_t a2) { return 0; }

	// STUB: TOY2 0x004C6B80
	uint32_t* UnkFunc46(float* a1, int32_t a2) { return 0; }

	// STUB: TOY2 0x004C7630
	uint32_t* UnkFunc45(float* a1, int32_t a2) { return 0; }

	// STUB: TOY2 0x004C80D0
	uint32_t* UnkFunc54(float* a1) { return 0; }

	// STUB: TOY2 0x004C8930
	uint32_t* UnkFunc39(float* a1) { return 0; }

	// STUB: TOY2 0x004C9190
	uint32_t* UnkFunc58(float* a1, int32_t a2) { return 0; }

	// STUB: TOY2 0x004C9A50
	uint32_t* UnkFunc34(float* a1) { return 0; }

	// STUB: TOY2 0x004C9D00
	uint32_t* UnkFunc35(int32_t a1) { return 0; }
}

namespace SoftwareDevice
{
	struct VertexBufferHeader
	{
		int32_t stride;
		uint32_t vertexCount;
	};

	STATIC_ASSERT(sizeof(VertexBufferHeader) == 8);

	// Drawing Methods

	// STUB: TOY2 0x004B9950
	HRESULT DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE primitiveType, LPDIRECT3DVERTEXBUFFER vertexBuffer, WORD* indices, DWORD indexCount, DWORD flags)
	{
		return DDERR_UNSUPPORTED;
	}

	// STUB: TOY2 0x004B99F0
	HRESULT DrawIndexedPrimitive(
		D3DPRIMITIVETYPE d3dptPrimitiveType,
		DWORD dwVertexTypeDesc,
		LPVOID lpvVertices,
		DWORD dwVertexCount,
		LPWORD lpwIndices,
		DWORD dwIndexCount,
		DWORD dwFlags)
	{
		return DDERR_UNSUPPORTED;
	}

	// Vertex Methods

	// FUNCTION: TOY2 0x004B2B20 [MATCHED]
	HRESULT ReleaseVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer)
	{
		free(buffer);
		return 0;
	}

	// FUNCTION: TOY2 0x004B2B30 [MATCHED]
	HRESULT CreateVertexBuffer(D3DVERTEXBUFFERDESC* desc, LPDIRECT3DVERTEXBUFFER* outBuffer, DWORD flags)
	{
		uint32_t vertexCount = desc->dwNumVertices;
		int32_t stride = SoftwareRenderer::GetStrideFromFVF(desc->dwFVF);

		if (stride)
		{
			VertexBufferHeader* buffer = (VertexBufferHeader*)malloc(stride * vertexCount + sizeof(VertexBufferHeader));

			if (buffer)
			{
				buffer->stride = stride;
				buffer->vertexCount = vertexCount;
				*outBuffer = (LPDIRECT3DVERTEXBUFFER)buffer;
				return 0;
			}

			return DDERR_OUTOFMEMORY;
		}

		return DDERR_INVALIDPARAMS;
	}

	// FUNCTION: TOY2 0x004B2BB0 [MATCHED]
	HRESULT LockVertexBuffer(LPDIRECT3DVERTEXBUFFER vertexBuffer, DWORD flags, LPVOID* outData, DWORD* outSize)
	{
		VertexBufferHeader* buffer = (VertexBufferHeader*)vertexBuffer;
		*outData = buffer + 1;

		if (outSize)
			*outSize = buffer->vertexCount * buffer->stride;

		return 0;
	}

	// FUNCTION: TOY2 0x004B2BD0
	HRESULT UnlockVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer) { return 0; }

	// FUNCTION: TOY2 0x004B2BE0
	HRESULT OptimizeVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer, LPDIRECT3DDEVICE3 device, DWORD flags) { return 0; }

	// STUB: TOY2 0x004C19E0
	HRESULT ProcessVerticesOnBuffer(
		LPDIRECT3DVERTEXBUFFER destBuffer,
		DWORD dwVertexOp,
		DWORD dwDestIndex,
		DWORD dwCount,
		LPDIRECT3DVERTEXBUFFER srcBuffer,
		DWORD dwSrcIndex,
		DWORD dwFlags)
	{
		return DDERR_UNSUPPORTED;
	}
}

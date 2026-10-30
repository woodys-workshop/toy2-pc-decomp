#include "SoftwareRenderer.h"
#include "DrawingDevice.h"
#include "Renderer/Renderer.h"
#include "Nu3D/BmpDataNode.h"
#include "Nu3D/Camera.h"
#include "Toy2/D3DApp.h"
#include "Logger.h"

#include <math.h>
#include <stdlib.h>

namespace SoftwareRenderer
{
	struct TextureSnapshot
	{
		uint32_t* texData;
		DDSURFACEDESC2* surfaceDesc;
	};

	STATIC_ASSERT(sizeof(TextureSnapshot) == 8);

	struct NativeVertex
	{
		int32_t x, y;
		int32_t red, green, blue;
		int32_t u, v;
	};

	struct NativePrimitive
	{
		NativeVertex vertices[4];
		NativePrimitive* next;
		uint16_t flags;
		uint8_t page;
		uint8_t unknown77;
		uint8_t secondaryUV[4][2];
	};

	STATIC_ASSERT(sizeof(NativeVertex) == 0x1C);
	STATIC_ASSERT(sizeof(NativePrimitive) == 0x80);
	STATIC_ASSERT(sizeof(PALETTEENTRY) == 4);

	// GLOBAL: TOY2 0x0084CBE0
	NativePrimitive* g_nativePrimitivePool;

	// GLOBAL: TOY2 0x00704E6C
	uint32_t g_nativeDivideNumerator;

	// GLOBAL: TOY2 0x00704E74
	int32_t g_nativeDivideTable[32768];

	// GLOBAL: TOY2 0x00724E74
	int32_t g_nativeMinimumDepth;

	// GLOBAL: TOY2 0x00534564
	uint8_t g_nativeLightingTable[128][256];

	// GLOBAL: TOY2 0x0070462C
	uint8_t* g_nativeColourCube;

	// GLOBAL: TOY2 0x00704630
	PALETTEENTRY g_nativeDisplayPalette[256];

	// GLOBAL: TOY2 0x00704A30
	uint8_t* g_nativeTintTable;

	// GLOBAL: TOY2 0x00704A34
	uint8_t* g_nativeAddTable;

	// GLOBAL: TOY2 0x00704A38
	PALETTEENTRY g_nativeBasePalette[256];

	// GLOBAL: TOY2 0x00704E38
	uint8_t* g_nativeBlend256;

	// GLOBAL: TOY2 0x00704E3C
	uint8_t* g_nativeBlend512;

	// GLOBAL: TOY2 0x00704E40
	uint8_t* g_nativeBlend768;

	// GLOBAL: TOY2 0x00704E44
	LPDIRECTDRAWPALETTE g_nativePalette;

	// GLOBAL: TOY2 0x00704E48
	uint8_t* g_nativeSubtractTable;

	// GLOBAL: TOY2 0x00703E20
	uint16_t g_nativeBlueTable[64];

	// GLOBAL: TOY2 0x00703EA0
	uint16_t g_nativeGreenTable[128];

	// GLOBAL: TOY2 0x00703FA0
	uint16_t g_nativeRedTable[64];

	// GLOBAL: TOY2 0x00704028
	uint16_t g_nativeBlueQuarter[64];

	// GLOBAL: TOY2 0x007040A8
	uint16_t g_nativeBlueThreeQuarter[64];

	// GLOBAL: TOY2 0x00704128
	uint16_t g_nativeBlueHalf[64];

	// GLOBAL: TOY2 0x007041A8
	uint16_t g_nativeGreenQuarter[128];

	// GLOBAL: TOY2 0x007042A8
	uint16_t g_nativeGreenThreeQuarter[128];

	// GLOBAL: TOY2 0x007043A8
	uint16_t g_nativeGreenHalf[128];

	// GLOBAL: TOY2 0x007044A8
	uint16_t g_nativeRedQuarter[64];

	// GLOBAL: TOY2 0x00704528
	uint16_t g_nativeRedThreeQuarter[64];

	// GLOBAL: TOY2 0x007045A8
	uint16_t g_nativeRedHalf[64];

	// GLOBAL: TOY2 0x00534558
	uint8_t* g_nativePixels;

	// GLOBAL: TOY2 0x00882778
	int32_t g_nativePixelPitch;

	// GLOBAL: TOY2 0x008827A0
	uint8_t* g_nativeTexturePages[64];

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
	int32_t g_unk839280;

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

	// FUNCTION: TOY2 0x0047D0F0
	void Destroy()
	{
		Logger::Log("QUIT : Destroying software renderer.\n");
		if (g_nativePrimitivePool)
			free(g_nativePrimitivePool);
	}

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

	// FUNCTION: TOY2 0x00470C70
	void UnkFunc7()
	{
		int32_t redTint = Nu3D::Camera::g_cameraTintBlue;
		int32_t blueTint = Nu3D::Camera::g_cameraTintRed;
		int32_t greenTint = Nu3D::Camera::g_cameraTintGreen;

		for (int32_t index = 1; index < 256; ++index)
		{
			g_nativeDisplayPalette[index].peRed = (uint8_t)(g_nativeBasePalette[index].peRed * redTint / 128);
			g_nativeDisplayPalette[index].peGreen = (uint8_t)(g_nativeBasePalette[index].peGreen * greenTint / 128);
			g_nativeDisplayPalette[index].peBlue = (uint8_t)(g_nativeBasePalette[index].peBlue * blueTint / 128);
		}

		g_nativePalette->SetEntries(0, 1, 255, &g_nativeDisplayPalette[1]);
	}

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

	// FUNCTION: TOY2 0x004319E0
	void UnkFunc11()
	{
		for (int32_t level = 0; level < 128; ++level)
		{
			for (int32_t index = 0; index < 256; ++index)
			{
				int32_t red = g_nativeBasePalette[index].peRed * level / 64;
				int32_t green = g_nativeBasePalette[index].peGreen * level / 64;
				int32_t blue = g_nativeBasePalette[index].peBlue * level / 64;

				if (red > 255)
					red = 255;
				if (green > 255)
					green = 255;
				if (blue > 255)
					blue = 255;

				int32_t bestDistance = 0x7FFFFFFF;
				uint8_t nearest;

				for (int32_t candidate = 1; candidate < 256; ++candidate)
				{
					int32_t redDelta = abs(red - g_nativeBasePalette[candidate].peRed) * 4;
					int32_t greenDelta = abs(green - g_nativeBasePalette[candidate].peGreen) * 5;
					int32_t blueDelta = abs(blue - g_nativeBasePalette[candidate].peBlue) * 3;
					int32_t distance = blueDelta * blueDelta + greenDelta * greenDelta + redDelta * redDelta;

					if (distance < bestDistance)
					{
						bestDistance = distance;
						nearest = (uint8_t)candidate;
					}
				}

				g_nativeLightingTable[level][index] = nearest;
			}
		}

		OutputDebugStringA("Generated lighting\n");
	}

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

	// FUNCTION: TOY2 0x00470BF0
	void SetPaletteOnAPI()
	{
		HRESULT result = D3DApp::g_d3dAppI.lpDD->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, g_nativeDisplayPalette, &g_nativePalette, 0);
		if (result < 0)
			Logger::LogDDError("d3dappi.lpDD->CreatePalette(0x00000004l|0x00000040l,&pal[0],&SonicRPalette,0)", result);

		result = D3DApp::g_d3dAppI.lpFrontBuffer->SetPalette(g_nativePalette);
		if (result < 0)
			Logger::LogDDError("d3dappi.lpFrontBuffer->SetPalette(SonicRPalette)", result);

		result = D3DApp::g_d3dAppI.lpBackBuffer->SetPalette(g_nativePalette);
		if (result < 0)
			Logger::LogDDError("d3dappi.lpBackBuffer->SetPalette(SonicRPalette)", result);
	}

	// FUNCTION: TOY2 0x00470D00
	void UnkFunc9(const uint8_t* rgb)
	{
		int32_t index;
		for (index = 0; index < 256; ++index)
		{
			g_nativeDisplayPalette[index].peRed = 0;
			g_nativeDisplayPalette[index].peGreen = 0;
			g_nativeDisplayPalette[index].peBlue = 0;
		}

		for (index = 0; index < 256; ++index)
		{
			g_nativeBasePalette[index].peRed = rgb[0];
			g_nativeBasePalette[index].peGreen = rgb[1];
			g_nativeBasePalette[index].peBlue = rgb[2];
			rgb += 3;
		}

		g_nativeBasePalette[0].peRed = 0;
		g_nativeBasePalette[0].peGreen = 0;
		g_nativeBasePalette[0].peBlue = 0;
		UnkFunc11();
	}

	// FUNCTION: TOY2 0x00470D60
	int32_t UnkFunc10()
	{
		uint8_t* output = g_nativeColourCube;
		int32_t red;
		for (red = 0; red < 256; red += 8)
		{
			for (int32_t green = 0; green < 256; green += 8)
			{
				for (int32_t blue = 0; blue < 256; blue += 8)
				{
					uint8_t nearest = 0;
					int32_t bestDistance = 9999999;

					for (int32_t candidate = 1; candidate < 256; ++candidate)
					{
						int32_t greenDelta = (g_nativeBasePalette[candidate].peGreen - green) * 5;
						int32_t redDelta = (g_nativeBasePalette[candidate].peRed - red) * 4;
						int32_t blueDelta = (g_nativeBasePalette[candidate].peBlue - blue) * 3;
						int32_t totalDelta = blueDelta + greenDelta + redDelta;
						int32_t distance = totalDelta * totalDelta + blueDelta * blueDelta + greenDelta * greenDelta + redDelta * redDelta;

						if (distance < bestDistance)
						{
							bestDistance = distance;
							nearest = (uint8_t)candidate;
						}
					}

					*output++ = nearest;
				}
			}
		}

		return red;
	}

	// FUNCTION: TOY2 0x00470FB0
	char* UnkFunc15()
	{
		PALETTEENTRY* source = g_nativeBasePalette;
		uint8_t* output = g_nativeTintTable;
		do
		{
			int32_t sourceGreen = source->peGreen;
			int32_t sourceRed = source->peRed;
			int32_t sourceBlue = source->peBlue;

			for (int32_t redOffset = -96; redOffset <= 128; redOffset += 32)
			{
				for (int32_t greenOffset = -96; greenOffset <= 128; greenOffset += 32)
				{
					for (int32_t blueOffset = -96; blueOffset <= 128; blueOffset += 32)
					{
						int32_t red = sourceRed + redOffset;
						int32_t green = sourceGreen + greenOffset;
						int32_t blue = sourceBlue + blueOffset;
						if (red < 0)
							red = 0;
						else if (red > 255)
							red = 255;
						if (green < 0)
							green = 0;
						else if (green > 255)
							green = 255;
						if (blue < 0)
							blue = 0;
						else if (blue > 255)
							blue = 255;

						*output++ = g_nativeColourCube[((red & ~7) * 32 + (green & ~7)) * 4 + (blue >> 3)];
					}
				}
			}
			++source;
		} while (source < g_nativeBasePalette + 256);

		// EAX holds the loop-end green-channel address.
		return (char*)((uint32_t)source + 1);
	}

	// FUNCTION: TOY2 0x004710C0
	void UnkFunc12(uint8_t* output, int32_t weight)
	{
		uint32_t outerWeight = 1024U - (uint32_t)weight;
		for (int32_t outer = 0; outer < 256; ++outer)
		{
			uint32_t outerRed = g_nativeBasePalette[outer].peRed * outerWeight;
			uint32_t outerGreen = g_nativeBasePalette[outer].peGreen * outerWeight;
			uint32_t outerBlue = g_nativeBasePalette[outer].peBlue * outerWeight;

			for (int32_t inner = 0; inner < 256; ++inner)
			{
				int32_t red = (int32_t)(g_nativeBasePalette[inner].peRed * (uint32_t)weight + outerRed) >> 10;
				int32_t green = (int32_t)(g_nativeBasePalette[inner].peGreen * (uint32_t)weight + outerGreen) >> 10;
				int32_t blue = (int32_t)(g_nativeBasePalette[inner].peBlue * (uint32_t)weight + outerBlue) >> 13;
				*output++ = g_nativeColourCube[((red & ~7) * 32 + (green & ~7)) * 4 + blue];
			}
		}
	}

	// FUNCTION: TOY2 0x00471190
	char* UnkFunc13()
	{
		PALETTEENTRY* source = g_nativeBasePalette;
		uint8_t* output = g_nativeAddTable;
		do
		{
			int32_t sourceRed = source->peRed;
			int32_t sourceGreen = source->peGreen;
			int32_t sourceBlue = source->peBlue;
			for (int32_t inner = 0; inner < 256; ++inner)
			{
				int32_t red = g_nativeBasePalette[inner].peRed + sourceRed;
				int32_t green = g_nativeBasePalette[inner].peGreen + sourceGreen;
				int32_t blue = g_nativeBasePalette[inner].peBlue + sourceBlue;
				if (red > 255)
					red = 255;
				if (green > 255)
					green = 255;
				if (blue > 255)
					blue = 255;

				*output++ = g_nativeColourCube[((red & ~7) * 32 + (green & ~7)) * 4 + (blue >> 3)];
			}
			++source;
		} while (source < g_nativeBasePalette + 256);

		return (char*)((uint32_t)source + 1);
	}

	// FUNCTION: TOY2 0x00471250
	char* UnkFunc14()
	{
		PALETTEENTRY* source = g_nativeBasePalette;
		uint8_t* output = g_nativeSubtractTable;
		do
		{
			int32_t sourceRed = source->peRed;
			int32_t sourceGreen = source->peGreen;
			int32_t sourceBlue = source->peBlue;
			for (int32_t inner = 0; inner < 256; ++inner)
			{
				int32_t red = sourceRed - g_nativeBasePalette[inner].peRed;
				int32_t green = sourceGreen - g_nativeBasePalette[inner].peGreen;
				int32_t blue = sourceBlue - g_nativeBasePalette[inner].peBlue;
				if (red < 0)
					red = 0;
				if (green < 0)
					green = 0;
				if (blue < 0)
					blue = 0;

				*output++ = g_nativeColourCube[((red & ~7) * 32 + (green & ~7)) * 4 + (blue >> 3)];
			}
			++source;
		} while (source < g_nativeBasePalette + 256);

		return (char*)((uint32_t)source + 1);
	}

	// FUNCTION: TOY2 0x00471300
	void SetNewPalette(int32_t paletteAddress, int32_t flags)
	{
		Logger::Log("SOFT : SetNewPalette.\n");
		UnkFunc9((const uint8_t*)paletteAddress);

		if (! g_nativeColourCube)
			g_nativeColourCube = (uint8_t*)malloc(0x8000);
		if (! g_nativeBlend256)
			g_nativeBlend256 = (uint8_t*)malloc(0x10000);
		if (! g_nativeBlend512)
			g_nativeBlend512 = (uint8_t*)malloc(0x10000);
		if (! g_nativeBlend768)
			g_nativeBlend768 = (uint8_t*)malloc(0x10000);
		if (! g_nativeAddTable)
			g_nativeAddTable = (uint8_t*)malloc(0x10000);
		if (! g_nativeSubtractTable)
			g_nativeSubtractTable = (uint8_t*)malloc(0x10000);
		if (! g_nativeTintTable)
			g_nativeTintTable = (uint8_t*)malloc(0x20000);

		if (flags & 1)
			UnkFunc10();
		if (flags & 0x10)
		{
			UnkFunc12(g_nativeBlend256, 256);
			UnkFunc12(g_nativeBlend512, 512);
			UnkFunc12(g_nativeBlend768, 768);
		}
		if (flags & 0x100)
			UnkFunc13();
		if (flags & 0x1000)
			UnkFunc14();
		if (flags & 0x10000)
			UnkFunc15();
	}

	// FUNCTION: TOY2 0x00471450
	uint32_t ReleaseNativePaletteTables()
	{
		if (g_nativeColourCube)
		{
			free(g_nativeColourCube);
			g_nativeColourCube = 0;
		}
		if (g_nativeBlend256)
		{
			free(g_nativeBlend256);
			g_nativeBlend256 = 0;
		}
		if (g_nativeBlend512)
		{
			free(g_nativeBlend512);
			g_nativeBlend512 = 0;
		}
		if (g_nativeBlend768)
		{
			free(g_nativeBlend768);
			g_nativeBlend768 = 0;
		}
		if (g_nativeAddTable)
		{
			free(g_nativeAddTable);
			g_nativeAddTable = 0;
		}
		if (g_nativeSubtractTable)
		{
			free(g_nativeSubtractTable);
			g_nativeSubtractTable = 0;
		}
		if (g_nativeTintTable)
		{
			free(g_nativeTintTable);
			g_nativeTintTable = 0;
		}
		if (g_nativePalette)
		{
			uint32_t references = g_nativePalette->Release();
			g_nativePalette = 0;
			return references;
		}

		return 0;
	}

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

	// FUNCTION: TOY2 0x0047C8B0
	int32_t UnkFunc1()
	{
		int32_t index;
		int32_t blueThreeQuarter;
		if (g_bitsPerPixel == 16)
		{
			for (index = 0; index < 64; ++index)
			{
				int32_t colour = index - 12;
				if (colour < 0)
					colour = 0;
				else if (colour > 31)
					colour = 31;

				g_nativeRedTable[index] = (uint16_t)(colour << 11);
				g_nativeRedQuarter[index] = (uint16_t)((colour * 10 / 32) << 11);
				g_nativeRedHalf[index] = (uint16_t)((colour * 17 / 32) << 11);
				g_nativeRedThreeQuarter[index] = (uint16_t)((colour * 25 / 32) << 11);
			}
			for (index = 0; index < 128; ++index)
			{
				int32_t colour = index - 24;
				if (colour < 0)
					colour = 0;
				else if (colour > 63)
					colour = 63;

				g_nativeGreenTable[index] = (uint16_t)(colour << 5);
				g_nativeGreenQuarter[index] = (uint16_t)((colour * 18 / 64) << 5);
				g_nativeGreenHalf[index] = (uint16_t)((colour * 33 / 64) << 5);
				g_nativeGreenThreeQuarter[index] = (uint16_t)((colour * 49 / 64) << 5);
			}
			for (index = 0; index < 64; ++index)
			{
				int32_t colour = index - 12;
				if (colour < 0)
					colour = 0;
				else if (colour > 31)
					colour = 31;

				g_nativeBlueTable[index] = (uint16_t)colour;
				g_nativeBlueQuarter[index] = (uint16_t)(colour * 10 / 32);
				g_nativeBlueHalf[index] = (uint16_t)(colour * 17 / 32);
				blueThreeQuarter = colour * 25 / 32;
				g_nativeBlueThreeQuarter[index] = (uint16_t)blueThreeQuarter;
			}
			return blueThreeQuarter;
		}

		for (index = 0; index < 64; ++index)
		{
			int32_t colour = index - 12;
			if (colour < 0)
				colour = 0;
			else if (colour > 31)
				colour = 31;

			g_nativeRedTable[index] = (uint16_t)(colour << 10);
			g_nativeRedQuarter[index] = (uint16_t)((colour * 10 / 32) << 10);
			g_nativeRedHalf[index] = (uint16_t)((colour * 17 / 32) << 10);
			g_nativeRedThreeQuarter[index] = (uint16_t)((colour * 25 / 32) << 10);
		}
		for (index = 0; index < 64; ++index)
		{
			int32_t colour = index - 12;
			if (colour < 0)
				colour = 0;
			else if (colour > 31)
				colour = 31;

			g_nativeGreenTable[index] = (uint16_t)(colour << 5);
			g_nativeGreenQuarter[index] = (uint16_t)((colour * 10 / 32) << 5);
			g_nativeGreenHalf[index] = (uint16_t)((colour * 17 / 32) << 5);
			g_nativeGreenThreeQuarter[index] = (uint16_t)((colour * 25 / 32) << 5);
		}
		for (index = 0; index < 64; ++index)
		{
			int32_t colour = index - 12;
			if (colour < 0)
				colour = 0;
			else if (colour > 31)
				colour = 31;

			g_nativeBlueTable[index] = (uint16_t)colour;
			g_nativeBlueQuarter[index] = (uint16_t)(colour * 10 / 32);
			g_nativeBlueHalf[index] = (uint16_t)(colour * 17 / 32);
			blueThreeQuarter = colour * 25 / 32;
			g_nativeBlueThreeQuarter[index] = (uint16_t)blueThreeQuarter;
		}
		return blueThreeQuarter;
	}

	// STUB: TOY2 0x0047D120
	void UnkFunc5() {}

	// STUB: TOY2 0x0047D210
	void UnkFunc8() {}

	// FUNCTION: TOY2 0x0047D540
	int32_t UnkFunc6(uint32_t* pixels, int32_t width, int32_t height, int32_t byteGap, int32_t value)
	{
		__asm
		{
			push    edi
			mov     edi, pixels
			mov     ecx, width
			mov     ebx, height
			mov     edx, byteGap
			mov     eax, value
		LBL_FILL_ROW:
			push    ecx
		LBL_FILL_PIXEL:
			mov     dword ptr [edi], eax
			add     edi, 4
			dec     ecx
			jnz     LBL_FILL_PIXEL
			pop     ecx
			add     edi, edx
			dec     ebx
			jnz     LBL_FILL_ROW
			pop     edi
		}
		// EAX retains the captured fill value.
	}

	// FUNCTION: TOY2 0x0047D650
	void UnkFunc66(SoftwareRenderer::UnkSoft2* records, int32_t count)
	{
		__asm
		{
			push    edi
			mov     edi, records
			mov     ecx, count
		LBL_CLEAR_MARKERS:
			mov     byte ptr [edi], 0
			add     edi, 4Ch
			dec     ecx
			jnz     LBL_CLEAR_MARKERS
			pop     edi
		}
	}

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

	// STUB: TOY2 0x004C4340
	uint32_t* UnkFunc59(int32_t a1, uint32_t* a2, int32_t* a3, uint32_t* a4) { return 0; }

	// STUB: TOY2 0x004C4370
	int16_t UnkFunc48(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C4640
	int16_t UnkFunc47(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C48E0
	int32_t UnkFunc55(uint32_t a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, uint32_t a7, int32_t a8, int32_t a9, uint32_t a10) { return 0; }

	// STUB: TOY2 0x004C4A60
	int32_t UnkFunc40(uint32_t a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, uint32_t a7, int32_t a8, int32_t a9, uint32_t a10) { return 0; }

	// STUB: TOY2 0x004C4BE0
	int32_t UnkFunc49(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C4E00
	int32_t UnkFunc52(float* a1, float* a2, int32_t a3, int32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9, uint32_t a10) { return 0; }

	// STUB: TOY2 0x004C4F30
	int32_t UnkFunc36(float* a1, float* a2, int32_t a3, int32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9, uint32_t a10) { return 0; }

	// STUB: TOY2 0x004C5060
	int32_t UnkFunc41(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C5280
	int32_t UnkFunc50(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C5490
	int32_t UnkFunc53(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C55B0
	int32_t UnkFunc37(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C56D0
	int32_t UnkFunc42(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C58E0
	int32_t UnkFunc43(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C5AC0
	int32_t UnkFunc51(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C5D80
	int32_t UnkFunc56(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

	// STUB: TOY2 0x004C5F00
	int32_t UnkFunc38(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10) { return 0; }

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

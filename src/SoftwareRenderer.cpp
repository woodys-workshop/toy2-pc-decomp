#include "SoftwareRenderer.h"
#include "DrawingDevice.h"
#include "Renderer/Renderer.h"
#include "Nu3D/BmpDataNode.h"
#include "Nu3D/Camera.h"
#include "Toy2/D3DApp.h"
#include "Toy2/Toy2.h"
#include "Logger.h"
#include "Nu3D/Math.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

namespace DrawingDevice
{
	extern D3DMATRIX* g_curWorldTransform;
}

namespace Renderer
{
	extern int32_t g_renderStateCache[8];
}

namespace SoftwareRenderer
{
	struct TextureSnapshot
	{
		uint32_t* texData;
		DDSURFACEDESC2* surfaceDesc;
	};

	STATIC_ASSERT(sizeof(TextureSnapshot) == 8);
	STATIC_ASSERT(offsetof(BVertex, x) == 0x00);
	STATIC_ASSERT(offsetof(BVertex, y) == 0x04);
	STATIC_ASSERT(offsetof(BVertex, z) == 0x08);
	STATIC_ASSERT(offsetof(BVertex, cameraY) == 0x0C);
	STATIC_ASSERT(offsetof(BVertex, diffuse) == 0x10);
	STATIC_ASSERT(offsetof(BVertex, cameraX) == 0x14);
	STATIC_ASSERT(offsetof(BVertex, u) == 0x18);
	STATIC_ASSERT(offsetof(BVertex, v) == 0x1C);
	STATIC_ASSERT(sizeof(BPolygon*) == 4);
	STATIC_ASSERT(offsetof(BPolygon, vertices) == 0x00);
	STATIC_ASSERT(offsetof(BPolygon, texture) == 0x80);
	STATIC_ASSERT(offsetof(BPolygon, count) == 0x84);
	STATIC_ASSERT(offsetof(BPolygon, flags) == 0x88);
	STATIC_ASSERT(offsetof(BPolygon, next) == 0x8C);
	STATIC_ASSERT(offsetof(BPolygon, unknown90) == 0x90);
	STATIC_ASSERT(offsetof(BPolygon, band) == 0x94);
	STATIC_ASSERT(offsetof(BPolygon, unknown98) == 0x98);

	int32_t GetCurrentTexture(TextureSnapshot* texture);
	void GetRenderDistance(float* nearDistance, float* farDistance);
	uint32_t UnkFunc27(uint32_t colour, float* red, float* green, float* blue, uint32_t* alphaMask);
	void UnkFunc30(BVertex* const* vertices, int32_t count, uint32_t* texture, uint32_t flags, int32_t layer);
	void UnkFunc29(BVertex* const* vertices, int32_t count, uint32_t* texture, uint32_t flags);
	void UnkFunc17(int32_t left, int32_t top, int32_t right, int32_t bottom);
	BOOL UnkFunc28(const BVertex* a, const BVertex* b, const BVertex* c);
	void UnkFunc26(BVertex* vertex);
	void UnkFunc24(BVertex* const* vertices, int32_t count, uint32_t* texture, uint32_t flags, int32_t layer, int32_t recursion);
	void UnkFunc25(BVertex* const* vertices, int32_t count, uint32_t* texture, uint32_t flags, int32_t layer);
	int32_t UnkFunc23(BVertex* const* vertices, int32_t count);
	void UnkFunc22(BVertex* const* vertices, int32_t count, uint32_t* texture, uint32_t flags, int32_t layer, int32_t recursion);
	void UnkFunc19(BVertex* vertices, uint16_t* indices, uint32_t indexCount, uint32_t flags);
	void UnkFunc21(BVertex* vertices, uint16_t* indices, uint32_t indexCount, uint32_t flags);
	void UnkFunc16(int32_t primitiveType, BVertex* vertices, uint16_t* indices, uint32_t indexCount, uint32_t flags);
	void UnkFunc34(BPolygon* polygon, int32_t count, uint32_t flags, uint32_t* texture, int32_t band);
	void UnkFunc35(BPolygon* polygon, int32_t count, uint32_t flags, uint32_t* texture, int32_t band);

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

	// Volatile aggregates keep VC6 spills out of the incoming argument cells.
	struct NativeStretchArguments
	{
		int32_t x0, y0;
		int32_t u0, startV;
		int32_t x1, y1;
		int32_t u1, endV;
		int32_t page;
	};

	struct NativeGlyphArguments
	{
		int32_t x, y;
		uint32_t colour, code;
	};

	STATIC_ASSERT(sizeof(NativeStretchArguments) == 0x24);
	STATIC_ASSERT(sizeof(NativeGlyphArguments) == 0x10);

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

	// GLOBAL: TOY2 0x00500C04
	int32_t g_nativeClearCountdown = 1;

	// GLOBAL: TOY2 0x0051ABD0
	HRESULT g_nativePresentResult;

	// GLOBAL: TOY2 0x0051A83C
	int16_t g_nativeRestoreRequested;

	// GLOBAL: TOY2 0x0051ABF0
	RECT g_nativePresentRect;

	// GLOBAL: TOY2 0x00503B34
	uint8_t g_nativeGlyphs[128][36] = {
#include "NativeGlyphs.inc"
	};

	// GLOBAL: TOY2 0x004F73A8
	int32_t g_nativeBackdropWidth = 256;

	// GLOBAL: TOY2 0x004F73AC
	int32_t g_nativeBackdropHeight = 128;

	// GLOBAL: TOY2 0x004F73D4
	int32_t g_nativeStaticBackdropWidth = 128;

	// GLOBAL: TOY2 0x004F73D8
	int32_t g_nativeStaticBackdropHeight = 160;

	// GLOBAL: TOY2 0x0072EFC0
	int32_t g_nativePreviousYaw;

	// GLOBAL: TOY2 0x00731DF0
	int32_t g_nativeAccumulatedYaw;

	// GLOBAL: TOY2 0x00731D6C
	int32_t g_nativeBackdropScroll;

	// GLOBAL: TOY2 0x00731C14
	int32_t g_nativeBackdropHorizon;

	// GLOBAL: TOY2 0x00731C18
	int32_t g_nativeBackdropDepth;

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

	// GLOBAL: TOY2 0x00B7FBE0
	BPolygon g_depthPolygons[15000];

	// GLOBAL: TOY2 0x00DBB0A0
	BPolygon g_fifoPolygons[1024];

	// GLOBAL: TOY2 0x00B626E0
	BPolygon* g_polygonHeads[30000];

	// GLOBAL: TOY2 0x00DBB094
	int32_t g_depthPolygonCount;

	// GLOBAL: TOY2 0x00DE20A8
	int32_t g_fifoPolygonCount;

	// GLOBAL: TOY2 0x00A4CC80
	int32_t g_queuePhase;

	// GLOBAL: TOY2 0x00B7FBA8
	float g_nearDistance;

	// GLOBAL: TOY2 0x00DE20A0
	float g_farDistance;

	// GLOBAL: TOY2 0x00B7FBB0
	float g_sortDepth;

	// GLOBAL: TOY2 0x00A4CC70
	float g_depthClipZ;

	// GLOBAL: TOY2 0x00B7FBA0
	int32_t g_samplingBand;

	// GLOBAL: TOY2 0x00508964
	int32_t g_clipCache[4] = { -1, -1, -1, -1 };

	// GLOBAL: TOY2 0x00B7FBB8
	uint32_t g_unkB7FBB8;

	// GLOBAL: TOY2 0x00B626C0
	uint32_t g_unkB626C0;

	// GLOBAL: TOY2 0x005088E4
	int32_t g_nearLayerOffsets[16] = { 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240 };

	// GLOBAL: TOY2 0x00508924
	int32_t g_farLayerOffsets[16] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60 };

	// $TODO: Verify the original scratch allocation extent.
	// GLOBAL: TOY2 0x00DFF5A0
	BVertex g_transformedVertices[10000];

	STATIC_ASSERT(sizeof(g_depthPolygons) == 0x23B4A0);
	STATIC_ASSERT(sizeof(g_fifoPolygons) == 0x27000);
	STATIC_ASSERT(sizeof(g_polygonHeads) == 0x1D4C0);
	STATIC_ASSERT(sizeof(g_transformedVertices) == 0x4E200);

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

		if (vertexCount)
		{
			uint16_t* triangle = indices + vertexCount + 1;
			do
			{
				triangle -= 3;
				vertexCount -= 3;
				Nu3D::VertexTL* vertices = (Nu3D::VertexTL*)data;
				SubmitSortedTriangle(renderFlags, renderEntry, 0, &vertices[triangle[-1]], &vertices[triangle[0]], &vertices[triangle[1]]);
			} while (vertexCount);
		}

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

	// FUNCTION: TOY2 0x004C1790
	void FlushDepthQueue()
	{
		UnkFunc33();
		UnkFunc32();
	}

	// FUNCTION: TOY2 0x004C17A0
	void FlushFIFOQueue()
	{
		UnkFunc31();
		UnkFunc32();
	}

	// STUB: TOY2 0x004C17B0
	void PresentFrame() {}

	// FUNCTION: TOY2 0x00490410
	void UnkFunc67(volatile int32_t x, volatile int32_t y)
	{
		using Toy2::g_hasStaticBackdrop;
		__asm
		{
			mov eax, g_hasStaticBackdrop
			test eax, eax
			mov eax, offset g_nativeStaticBackdropWidth
			jnz LBL_BACKDROP_WIDTH
			mov eax, offset g_nativeBackdropWidth
		LBL_BACKDROP_WIDTH:
			mov ecx, [eax]
			mov esi, x
			mov eax, esi
			cdq
			idiv ecx
			mov edx, esi
			mov edi, eax
			imul edi, ecx
			sub edx, edi
			jns LBL_BACKDROP_NONNEGATIVE
			mov edx, 1
			sub edx, eax
			mov eax, y
			imul edx, ecx
			add edx, esi
			mov dword ptr [g_unk4F7400 + 4], eax
			mov dword ptr [g_unk4F7400], edx
			jmp LBL_BACKDROP_DONE
		LBL_BACKDROP_NONNEGATIVE:
			mov ecx, y
			mov dword ptr [g_unk4F7400], edx
			mov dword ptr [g_unk4F7400 + 4], ecx
		LBL_BACKDROP_DONE:
		}
	}

	// STUB: TOY2 0x0048FB70
	void UnkFunc2() {}

	// FUNCTION: TOY2 0x00490290
	int16_t UnkFunc3()
	{
		volatile int32_t savedECX;
		__asm mov savedECX, ecx
		int32_t staticBackdrop = Toy2::g_hasStaticBackdrop;
		int32_t* width = staticBackdrop ? &g_nativeStaticBackdropWidth : &g_nativeBackdropWidth;
		int32_t* height = staticBackdrop ? &g_nativeStaticBackdropHeight : &g_nativeBackdropHeight;
		volatile int32_t& previousYaw = g_nativePreviousYaw;
		volatile int32_t& accumulatedYaw = g_nativeAccumulatedYaw;
		volatile int32_t& quantizedHistory = g_unk500A1C;
		int32_t lastQuantized = quantizedHistory;
		int32_t yaw = Nu3D::Camera::g_cameraAngles.yaw;
		int32_t quantized;
		volatile int32_t& scroll = g_nativeBackdropScroll;
		if (lastQuantized == -1)
		{
			previousYaw = -yaw;
			accumulatedYaw = -yaw;
			quantized = ((int32_t)((uint32_t)(-yaw) * 2240) / 8192) & 255;
			int32_t wrapWidth = *width;
			int32_t remainder;
			__asm
			{
				mov eax, quantized
				cdq
				idiv wrapWidth
				mov remainder, edx
			}
			scroll = remainder;
		}
		else
		{
			int32_t delta = (int32_t)(0U - ((uint32_t)previousYaw + yaw));
			if (delta < -2048)
				delta = (int32_t)((uint32_t)delta + 4096);
			else if (delta > 2048)
				delta = (int32_t)((uint32_t)delta - 4096);
			int32_t accumulated = (int32_t)((uint32_t)accumulatedYaw + delta);
			previousYaw = -yaw;
			accumulatedYaw = accumulated;
			quantized = ((int32_t)((uint32_t)accumulated * 2240) / 8192) & 255;
			delta = (int32_t)((uint32_t)quantized - lastQuantized);
			if (delta < -128)
				delta = (int32_t)((uint32_t)delta + 256);
			else if (delta > 128)
				delta = (int32_t)((uint32_t)delta - 256);
			int32_t previousScroll = scroll;
			int32_t wrapWidth = *width;
			int32_t numerator = (int32_t)((uint32_t)previousScroll + delta);
			int32_t remainder;
			__asm
			{
				mov eax, numerator
				cdq
				idiv wrapWidth
				mov remainder, edx
			}
			scroll = remainder;
			if (remainder < 0)
			{
				do
				{
					remainder = (int32_t)((uint32_t)remainder + wrapWidth);
				} while (remainder < 0);
				scroll = remainder;
			}
		}

		volatile int32_t& horizonCell = g_nativeBackdropHorizon;
		int32_t horizon = horizonCell;
		quantizedHistory = quantized;
		int32_t crop;
		if (horizon < 0)
		{
			crop = (int32_t)(0U - horizon);
			horizon = 0;
			horizonCell = 0;
		}
		else
			crop = 0;
		if (g_unk4F7400.x != -32768)
			scroll = g_unk4F7400.x;

		int32_t result = crop;
		if (crop <= *height && crop >= 0 && g_nativeBackdropDepth > 2048)
		{
			int32_t visibleHeight = (int32_t)((uint32_t)*height - crop);
			result = Toy2::g_levelFileIndex;
			if (result && ! staticBackdrop)
			{
				result = (int32_t)((uint32_t)horizon + visibleHeight);
				if (result > 240)
					savedECX = (int32_t)(240U - horizon);
			}
		}
		__asm
		{
			mov eax, result
			mov ax, 1
			mov ecx, savedECX
		}
	}

	// FUNCTION: TOY2 0x004BCBE0
	void UnkFunc31()
	{
		int32_t index = 0;
		int32_t limit = g_fifoPolygonCount;
		if (limit > 0)
		{
			BPolygon* polygon = g_fifoPolygons;
			do
			{
				UnkFunc35(polygon, polygon->count, polygon->flags, polygon->texture, polygon->band);
				limit = g_fifoPolygonCount;
				++polygon;
				++index;
			} while (index < limit);
		}
		g_queuePhase = 0;
	}

	// FUNCTION: TOY2 0x004BCC40
	void UnkFunc32()
	{
		if (g_queuePhase == 0)
		{
			memset(g_polygonHeads, 0, sizeof(g_polygonHeads));
			g_depthPolygonCount = 0;
			g_fifoPolygonCount = 0;
		}
	}

	// FUNCTION: TOY2 0x004BCB60
	void UnkFunc33()
	{
		if (g_queuePhase == 1)
		{
			for (int32_t bucket = 29999; bucket >= 0; --bucket)
			{
				BPolygon* polygon = g_polygonHeads[bucket];
				while (polygon)
				{
					UnkFunc34(polygon, polygon->count, polygon->flags, polygon->texture, polygon->band);
					polygon = polygon->next;
				}
			}
		}

		uint32_t phase = (uint32_t)g_queuePhase;
		g_unkB7FBB8 = 0;
		++phase;
		g_unkB626C0 = 0;
		g_queuePhase = (int32_t)phase;
	}

	// FUNCTION: TOY2 0x00470C70
	void UnkFunc7()
	{
		int32_t redTint = *(volatile int16_t*)&Nu3D::Camera::g_cameraTintBlue;
		int32_t blueTint = *(volatile int16_t*)&Nu3D::Camera::g_cameraTintRed;
		int32_t greenTint = *(volatile int16_t*)&Nu3D::Camera::g_cameraTintGreen;

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

	// FUNCTION: TOY2 0x0040BAE0
	int32_t ClearRenderSurfaces()
	{
		DDSURFACEDESC surfaceDesc;
		DDBLTFX fill;
		RECT destination;
		HRESULT result;
		if (D3DApp::g_renderMode != 1 || D3DApp::g_pcStruct.mode->rgbBitCount != 8)
		{
			if (D3DApp::g_d3dAppI.lpFrontBuffer)
			{
				memset(&surfaceDesc, 0, sizeof(surfaceDesc));
				surfaceDesc.dwSize = sizeof(surfaceDesc);
				result = D3DApp::g_d3dAppI.lpFrontBuffer->GetSurfaceDesc(&surfaceDesc);
				if (result < 0)
					Logger::LogDDError("D3DAppIGetSurfDesc(&ddsd, d3dappi.lpFrontBuffer)", result);
				memset(&fill, 0, sizeof(fill));
				fill.dwSize = sizeof(fill);
				SetRect(&destination, 0, 0, surfaceDesc.dwWidth, surfaceDesc.dwHeight);
				result = D3DApp::g_d3dAppI.lpFrontBuffer->Blt(&destination, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &fill);
				if (result < 0)
					Logger::LogDDError("d3dappi.lpFrontBuffer->Blt(&dst, 0, 0, 0x00000400l | 0x01000000l, &ddbltfx)", result);
			}
			if (D3DApp::g_d3dAppI.lpBackBuffer)
			{
				memset(&surfaceDesc, 0, sizeof(surfaceDesc));
				surfaceDesc.dwSize = sizeof(surfaceDesc);
				result = D3DApp::g_d3dAppI.lpBackBuffer->GetSurfaceDesc(&surfaceDesc);
				if (result < 0)
					Logger::LogDDError("D3DAppIGetSurfDesc(&ddsd, d3dappi.lpBackBuffer)", result);
				memset(&fill, 0, sizeof(fill));
				fill.dwSize = sizeof(fill);
				SetRect(&destination, 0, 0, surfaceDesc.dwWidth, surfaceDesc.dwHeight);
				result = D3DApp::g_d3dAppI.lpBackBuffer->Blt(&destination, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &fill);
				if (result < 0)
					Logger::LogDDError("d3dappi.lpBackBuffer->Blt(&dst, 0, 0, 0x00000400l | 0x01000000l, &ddbltfx)", result);
			}
		}
		return 1;
	}

	// FUNCTION: TOY2 0x0040CD80
	int32_t ShowBackBuffer()
	{
		if (! D3DApp::g_d3dAppI.bRenderingIsOK)
		{
			Logger::LogErrorNotSetLn("Cannot call D3DAppShowBackBuffer while bRenderingIsOK is FALSE.\n");
			return 0;
		}
		if (D3DApp::g_d3dAppI.bPaused)
			return 1;
		if (D3DApp::g_pcStruct.fullscreenMode)
		{
			g_nativePresentResult = D3DApp::g_d3dAppI.lpFrontBuffer->Flip(D3DApp::g_d3dAppI.lpBackBuffer, DDFLIP_WAIT);
			if (g_nativePresentResult == DDERR_SURFACELOST)
			{
				g_nativeRestoreRequested = 1;
				D3DApp::g_d3dAppI.lpFrontBuffer->Restore();
				D3DApp::g_d3dAppI.lpBackBuffer->Restore();
				ClearRenderSurfaces();
			}
			else if (g_nativePresentResult)
			{
				Logger::LogErrorNotSetLn("Flipping complex display surface failed.\n%s", Logger::ErrorToMessage(g_nativePresentResult));
				return 0;
			}
		}
		else
		{
			RECT source;
			SetRect(&source, 0, 0, D3DApp::g_d3dAppI.szClient.cx, D3DApp::g_d3dAppI.szClient.cy);
			SetRect(
				&g_nativePresentRect,
				D3DApp::g_d3dAppI.pClientOnPrimary.x,
				D3DApp::g_d3dAppI.pClientOnPrimary.y,
				D3DApp::g_d3dAppI.szClient.cx + D3DApp::g_d3dAppI.pClientOnPrimary.x,
				D3DApp::g_d3dAppI.szClient.cy + D3DApp::g_d3dAppI.pClientOnPrimary.y);
			g_nativePresentResult = D3DApp::g_d3dAppI.lpFrontBuffer->Blt(&g_nativePresentRect, D3DApp::g_d3dAppI.lpBackBuffer, &source, DDBLT_WAIT, 0);
			if (g_nativePresentResult == DDERR_SURFACELOST)
			{
				g_nativeRestoreRequested = 1;
				D3DApp::g_d3dAppI.lpFrontBuffer->Restore();
				D3DApp::g_d3dAppI.lpBackBuffer->Restore();
				ClearRenderSurfaces();
			}
			else if (g_nativePresentResult)
			{
				Logger::LogErrorNotSetLn("Blt of back buffer to front buffer failed.\n%s", Logger::ErrorToMessage(g_nativePresentResult));
				return 0;
			}
		}
		return 1;
	}

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

	// FUNCTION: TOY2 0x0045DB80
	int32_t UnkRenderAPI1(int32_t* primitiveWords)
	{
		volatile NativePrimitive* volatile primitive = (NativePrimitive*)primitiveWords;
		uint16_t flags = primitive->flags;
		uint32_t endU = (uint32_t)primitive->vertices[2].u - 1;
		uint32_t endV = (uint32_t)primitive->vertices[2].v - 1;
		primitive->vertices[2].u = (int32_t)endU;
		primitive->vertices[2].v = (int32_t)endV;
		int32_t greenOffset, redOffset, blueOffset;
		if (flags & 0x40)
		{
			greenOffset = primitive->vertices[0].green >> 16;
			redOffset = primitive->vertices[0].red >> 16;
			blueOffset = primitive->vertices[0].blue >> 16;
		}
		int32_t x2 = primitive->vertices[2].x;
		int32_t x0 = primitive->vertices[0].x;
		int32_t y0 = primitive->vertices[0].y;
		int32_t widthDelta = (int32_t)((uint32_t)x2 - x0);
		if (! widthDelta)
			widthDelta = 1;
		int32_t y2 = primitive->vertices[2].y;
		int32_t heightDelta = (int32_t)((uint32_t)y2 - y0);
		if (! heightDelta)
			heightDelta = 1;
		uint32_t startU = primitive->vertices[0].u;
		uint32_t initialV = primitive->vertices[0].v;
		int32_t stepU, stepV;
		volatile uint32_t* runningV;
		// The incoming pointer cell becomes V between the two signed divisions.
		__asm
		{
			mov eax, endU
			sub eax, startU
			cdq
			idiv widthDelta
			mov stepU, eax
			lea eax, primitiveWords
			mov runningV, eax
			mov ecx, initialV
			mov primitiveWords, ecx
			mov eax, endV
			sub eax, initialV
			cdq
			idiv heightDelta
			mov stepV, eax
		}
		uint32_t width = (uint32_t)widthDelta + 1;
		uint32_t rows = (uint32_t)heightDelta + 1;
		int32_t top = Nu3D::Camera::g_viewportTop;
		int32_t left = Nu3D::Camera::g_viewportLeft;
		uint16_t* destination = (uint16_t*)g_nativePixels + (int32_t)((uint32_t)g_nativePixelPitch * top + left);
		uint16_t* texture = (uint16_t*)g_nativeTexturePages[primitive->page];
		if (y0 < top)
		{
			*runningV += ((uint32_t)top - y0) * stepV;
			rows += (uint32_t)y0 - top;
		}
		else
			destination += (int32_t)(((uint32_t)y0 - top) * g_nativePixelPitch);
		int32_t bottom = Nu3D::Camera::g_viewportRight;
		if (y2 > bottom)
			rows += (uint32_t)bottom - y2;
		if (x0 < left)
		{
			startU += ((uint32_t)left - x0) * stepU;
			width += (uint32_t)x0 - left;
		}
		else
			destination += (int32_t)((uint32_t)x0 - left);
		int32_t right = Nu3D::Camera::g_viewportBottom;
		if (x2 >= right)
			width += (uint32_t)right - x2;

		if (flags & 0x20)
		{
			if (flags & 0x40)
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						if (raw != 0x03E0)
						{
							uint16_t selected = ((volatile uint16_t*)g_nativeGreenTable)[((raw >> 5) & 31) + greenOffset];
							selected = (uint16_t)(selected + ((volatile uint16_t*)g_nativeRedTable)[(raw >> 10) + redOffset]);
							uint32_t previous = *(volatile uint16_t*)destination;
							selected = (uint16_t)(selected + ((volatile uint16_t*)g_nativeBlueTable)[(raw & 31) + blueOffset]);
							uint32_t red = (previous & 0x7C00) + (selected & 0x7C00);
							if (red > 0x7C00)
								red = 0x7C00;
							uint32_t green = (previous & 0x03E0) + (selected & 0x03E0);
							if (green > 0x03E0)
								green = 0x03E0;
							uint32_t blue = (previous & 31) + (selected & 31);
							if (blue > 31)
								blue = 31;
							*destination = (uint16_t)(red | green | blue);
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
			else
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						if (raw != 0x03E0)
						{
							uint32_t previous = *destination;
							uint32_t red = (previous & 0x7C00) + (raw & 0x7C00);
							if (red > 0x7C00)
								red = 0x7C00;
							uint32_t green = (previous & 0x03E0) + (raw & 0x03E0);
							if (green > 0x03E0)
								green = 0x03E0;
							uint32_t blue = (previous & 31) + (raw & 31);
							if (blue > 31)
								blue = 31;
							*destination = (uint16_t)(red | green | blue);
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
			__asm
			{
				mov ecx, destination
				mov edx, stepV
			}
			return 0;
		}
		if (flags & 0x1000)
		{
			if (flags & 0x40)
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						if (raw != 0x03E0)
						{
							uint16_t selected = ((volatile uint16_t*)g_nativeGreenTable)[((raw >> 5) & 31) + greenOffset];
							selected = (uint16_t)(selected + ((volatile uint16_t*)g_nativeRedTable)[(raw >> 10) + redOffset]);
							uint32_t previous = *(volatile uint16_t*)destination;
							selected = (uint16_t)(selected + ((volatile uint16_t*)g_nativeBlueTable)[(raw & 31) + blueOffset]);
							int32_t red = (int32_t)(previous & 0x7C00) - (selected & 0x7C00);
							if (red < 0x0400)
								red = 0;
							int32_t green = (int32_t)(previous & 0x03E0) - (selected & 0x03E0);
							if (green < 0x20)
								green = 0;
							int32_t blue = (int32_t)(previous & 31) - (selected & 31);
							if (blue < 1)
								blue = 0;
							*destination = (uint16_t)(red | green | blue);
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
			else
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						if (raw != 0x03E0)
						{
							uint32_t previous = *destination;
							int32_t red = (int32_t)(previous & 0x7C00) - (raw & 0x7C00);
							if (red < 0x0400)
								red = 0;
							int32_t green = (int32_t)(previous & 0x03E0) - (raw & 0x03E0);
							if (green < 0x20)
								green = 0;
							int32_t blue = (int32_t)(previous & 31) - (raw & 31);
							if (blue < 1)
								blue = 0;
							*destination = (uint16_t)(red | green | blue);
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
			__asm
			{
				mov ecx, destination
				mov edx, stepV
			}
			return 0;
		}
		if (flags & 0x10)
		{
			if (flags & 0x40)
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						if (raw != 0x03E0)
						{
							uint16_t selected = g_nativeGreenTable[((raw >> 5) & 31) + greenOffset];
							selected = (uint16_t)(selected + g_nativeRedTable[(raw >> 10) + redOffset]);
							selected = (uint16_t)(selected + g_nativeBlueTable[(raw & 31) + blueOffset]);
							uint16_t previous = *destination;
							*destination = (uint16_t)(((selected >> 1) & 0x3DEF) + ((previous >> 1) & 0x3DEF));
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
				__asm
				{
					mov ecx, destination
					mov edx, stepV
				}
				return 0;
			}
			do
			{
				uint32_t u = startU;
				uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
				uint32_t count = width;
				do
				{
					uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
					if (raw != 0x03E0)
					{
						uint16_t previous = *destination;
						*destination = (uint16_t)(((previous >> 1) & 0x3DEF) + ((raw >> 1) & 0x3DEF));
					}
					++destination;
					u += stepU;
				} while (--count);
				destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
				*runningV += stepV;
			} while (--rows);
			__asm
			{
				mov ecx, destination
				mov edx, runningV
				mov edx, [edx]
			}
			return 0;
		}
		if (flags & 0x40)
		{
			do
			{
				uint32_t u = startU;
				uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
				uint32_t count = width;
				do
				{
					uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
					if (raw != 0x03E0)
					{
						uint16_t selected = g_nativeGreenTable[((raw >> 5) & 31) + greenOffset];
						selected = (uint16_t)(selected + g_nativeRedTable[(raw >> 10) + redOffset]);
						selected = (uint16_t)(selected + g_nativeBlueTable[(raw & 31) + blueOffset]);
						*destination = selected;
					}
					++destination;
					u += stepU;
				} while (--count);
				destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
				*runningV += stepV;
			} while (--rows);
			__asm
			{
				mov ecx, destination
				mov edx, runningV
				mov edx, [edx]
			}
			return 0;
		}
		do
		{
			uint32_t u = startU;
			uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
			uint32_t count = width;
			do
			{
				uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
				if (raw != 0x03E0)
					*destination = raw;
				++destination;
				u += stepU;
			} while (--count);
			destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
			*runningV += stepV;
		} while (--rows);
		__asm
		{
			mov ecx, destination
			xor edx, edx
		}
		return (int32_t)texture;
	}

	// STUB: TOY2 0x0045E390
	int32_t UnkRenderAPI13(int32_t* a1) { return 0; }

	// FUNCTION: TOY2 0x00461D20
	int32_t UnkRenderAPI14(int32_t* primitiveWords)
	{
		volatile NativePrimitive* volatile primitive = (NativePrimitive*)primitiveWords;
		uint16_t flags = primitive->flags;
		uint32_t endU = (uint32_t)primitive->vertices[2].u - 1;
		uint32_t endV = (uint32_t)primitive->vertices[2].v - 1;
		primitive->vertices[2].u = (int32_t)endU;
		primitive->vertices[2].v = (int32_t)endV;
		int32_t greenOffset, redOffset, blueOffset;
		if (flags & 0x40)
		{
			greenOffset = primitive->vertices[0].green >> 15;
			redOffset = primitive->vertices[0].red >> 16;
			blueOffset = primitive->vertices[0].blue >> 16;
		}
		int32_t x2 = primitive->vertices[2].x;
		int32_t x0 = primitive->vertices[0].x;
		int32_t y0 = primitive->vertices[0].y;
		int32_t widthDelta = (int32_t)((uint32_t)x2 - x0);
		if (! widthDelta)
			widthDelta = 1;
		int32_t y2 = primitive->vertices[2].y;
		int32_t heightDelta = (int32_t)((uint32_t)y2 - y0);
		if (! heightDelta)
			heightDelta = 1;
		uint32_t startU = primitive->vertices[0].u;
		uint32_t initialV = primitive->vertices[0].v;
		int32_t stepU, stepV;
		volatile uint32_t* runningV;
		// The incoming pointer cell becomes V between the two signed divisions.
		__asm
		{
			mov eax, endU
			sub eax, startU
			cdq
			idiv widthDelta
			mov stepU, eax
			lea eax, primitiveWords
			mov runningV, eax
			mov ecx, initialV
			mov primitiveWords, ecx
			mov eax, endV
			sub eax, initialV
			cdq
			idiv heightDelta
			mov stepV, eax
		}
		uint32_t width = (uint32_t)widthDelta + 1;
		uint32_t rows = (uint32_t)heightDelta + 1;
		int32_t top = Nu3D::Camera::g_viewportTop;
		int32_t left = Nu3D::Camera::g_viewportLeft;
		uint16_t* destination = (uint16_t*)g_nativePixels + (int32_t)((uint32_t)g_nativePixelPitch * top + left);
		uint16_t* texture = (uint16_t*)g_nativeTexturePages[primitive->page];
		if (y0 < top)
		{
			*runningV += ((uint32_t)top - y0) * stepV;
			rows += (uint32_t)y0 - top;
		}
		else
			destination += (int32_t)(((uint32_t)y0 - top) * g_nativePixelPitch);
		int32_t bottom = Nu3D::Camera::g_viewportRight;
		if (y2 > bottom)
			rows += (uint32_t)bottom - y2;
		if (x0 < left)
		{
			startU += ((uint32_t)left - x0) * stepU;
			width += (uint32_t)x0 - left;
		}
		else
			destination += (int32_t)((uint32_t)x0 - left);
		int32_t right = Nu3D::Camera::g_viewportBottom;
		if (x2 >= right)
			width += (uint32_t)right - x2;

		if (flags & 0x20)
		{
			if (flags & 0x40)
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						if (raw != 0x07C0)
						{
							uint16_t selected = ((volatile uint16_t*)g_nativeGreenTable)[((raw >> 5) & 63) + greenOffset];
							selected = (uint16_t)(selected + ((volatile uint16_t*)g_nativeRedTable)[(raw >> 11) + redOffset]);
							uint32_t previous = *(volatile uint16_t*)destination;
							selected = (uint16_t)(selected + ((volatile uint16_t*)g_nativeBlueTable)[(raw & 31) + blueOffset]);
							uint32_t red = (previous & 0xF800) + (selected & 0xF800);
							if (red > 0xF800)
								red = 0xF800;
							uint32_t green = (previous & 0x07E0) + (selected & 0x07E0);
							if (green > 0x07E0)
								green = 0x07E0;
							uint32_t blue = (previous & 31) + (selected & 31);
							if (blue > 31)
								blue = 31;
							*destination = (uint16_t)(red | green | blue);
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
			else
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						if (raw != 0x07C0)
						{
							uint32_t previous = *destination;
							uint32_t red = (previous & 0xF800) + (raw & 0xF800);
							if (red > 0xF800)
								red = 0xF800;
							uint32_t green = (previous & 0x07E0) + (raw & 0x07E0);
							if (green > 0x07E0)
								green = 0x07E0;
							uint32_t blue = (previous & 31) + (raw & 31);
							if (blue > 31)
								blue = 31;
							*destination = (uint16_t)(red | green | blue);
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
			__asm
			{
				mov ecx, destination
				mov edx, stepV
			}
			return 0;
		}
		if (flags & 0x1000)
		{
			if (flags & 0x40)
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						if (raw != 0x07C0)
						{
							uint16_t selected = ((volatile uint16_t*)g_nativeGreenTable)[((raw >> 5) & 63) + greenOffset];
							selected = (uint16_t)(selected + ((volatile uint16_t*)g_nativeRedTable)[(raw >> 11) + redOffset]);
							uint32_t previous = *(volatile uint16_t*)destination;
							selected = (uint16_t)(selected + ((volatile uint16_t*)g_nativeBlueTable)[(raw & 31) + blueOffset]);
							int32_t red = (int32_t)(previous & 0xF800) - (selected & 0xF800);
							if (red < 0x0800)
								red = 0;
							int32_t green = (int32_t)(previous & 0x07E0) - (selected & 0x07E0);
							if (green < 0x20)
								green = 0;
							int32_t blue = (int32_t)(previous & 31) - (selected & 31);
							if (blue < 1)
								blue = 0;
							*destination = (uint16_t)(red | green | blue);
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
			else
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						if (raw != 0x07C0)
						{
							uint32_t previous = *destination;
							int32_t red = (int32_t)(previous & 0xF800) - (raw & 0xF800);
							if (red < 0x0800)
								red = 0;
							int32_t green = (int32_t)(previous & 0x07E0) - (raw & 0x07E0);
							if (green < 0x20)
								green = 0;
							int32_t blue = (int32_t)(previous & 31) - (raw & 31);
							if (blue < 1)
								blue = 0;
							*destination = (uint16_t)(red | green | blue);
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
			__asm
			{
				mov ecx, destination
				mov edx, stepV
			}
			return 0;
		}
		if (flags & 0x10)
		{
			if (flags & 0x40)
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						if (raw != 0x07C0)
						{
							uint16_t selected = g_nativeGreenTable[((raw >> 5) & 63) + greenOffset];
							selected = (uint16_t)(selected + g_nativeRedTable[(raw >> 11) + redOffset]);
							selected = (uint16_t)(selected + g_nativeBlueTable[(raw & 31) + blueOffset]);
							uint16_t previous = *destination;
							*destination = (uint16_t)(((selected >> 1) & 0x7BEF) + ((previous >> 1) & 0x7BEF));
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
				__asm
				{
					mov ecx, destination
					mov edx, stepV
				}
				return 0;
			}
			do
			{
				uint32_t u = startU;
				uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
				uint32_t count = width;
				do
				{
					uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
					if (raw != 0x07C0)
					{
						uint16_t previous = *destination;
						*destination = (uint16_t)(((raw >> 1) & 0x7BEF) + ((previous >> 1) & 0x7BEF));
					}
					++destination;
					u += stepU;
				} while (--count);
				destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
				*runningV += stepV;
			} while (--rows);
			__asm
			{
				mov ecx, destination
				mov edx, runningV
				mov edx, [edx]
			}
			return 0;
		}
		if (flags & 0x40)
		{
			do
			{
				uint32_t u = startU;
				uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
				uint32_t count = width;
				do
				{
					uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
					if (raw != 0x07C0)
					{
						uint16_t selected = g_nativeGreenTable[((raw >> 5) & 63) + greenOffset];
						selected = (uint16_t)(selected + g_nativeRedTable[(raw >> 11) + redOffset]);
						selected = (uint16_t)(selected + g_nativeBlueTable[(raw & 31) + blueOffset]);
						*destination = selected;
					}
					++destination;
					u += stepU;
				} while (--count);
				destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
				*runningV += stepV;
			} while (--rows);
			__asm
			{
				mov ecx, destination
				mov edx, runningV
				mov edx, [edx]
			}
			return 0;
		}
		do
		{
			uint32_t u = startU;
			uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
			uint32_t count = width;
			do
			{
				uint16_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
				if (raw != 0x07C0)
					*destination = raw;
				++destination;
				u += stepU;
			} while (--count);
			destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
			*runningV += stepV;
		} while (--rows);
		__asm
		{
			mov ecx, destination
			xor edx, edx
		}
		return (int32_t)texture;
	}

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
		int32_t redSquare;
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
						redSquare = redDelta * redDelta;
						int32_t distance = totalDelta * totalDelta + blueDelta * blueDelta + greenDelta * greenDelta + redSquare;

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

		__asm
		{
			mov eax, 256
			mov ecx, output
			mov edx, redSquare
		}
	}

	// FUNCTION: TOY2 0x00470FB0
	char* UnkFunc15()
	{
		volatile PALETTEENTRY* source = g_nativeBasePalette;
		uint8_t* output = g_nativeTintTable;
		int32_t blueIndex;
		uint8_t* blueCube;
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

						blueIndex = blue >> 3;
						blueCube = g_nativeColourCube + blueIndex;
						*output++ = blueCube[((red & ~7) * 32 + (green & ~7)) * 4];
					}
				}
			}
			++source;
		} while (source < g_nativeBasePalette + 256);

		__asm
		{
			mov eax, offset g_nativeBasePalette
			add eax, 401h
			mov ecx, blueIndex
			mov edx, blueCube
		}
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
		volatile PALETTEENTRY* source = g_nativeBasePalette;
		uint8_t* output = g_nativeAddTable;
		int32_t blueIndex;
		uint8_t* blueCube;
		do
		{
			int32_t sourceRed = source->peRed;
			int32_t sourceGreen = source->peGreen;
			int32_t sourceBlue = source->peBlue;
			for (int32_t inner = 0; inner < 256; ++inner)
			{
				volatile PALETTEENTRY& entry = g_nativeBasePalette[inner];
				int32_t red = entry.peRed + sourceRed;
				int32_t green = entry.peGreen + sourceGreen;
				int32_t blue = entry.peBlue + sourceBlue;
				if (red > 255)
					red = 255;
				if (green > 255)
					green = 255;
				if (blue > 255)
					blue = 255;

				blueIndex = blue >> 3;
				blueCube = g_nativeColourCube + blueIndex;
				*output++ = blueCube[((red & ~7) * 32 + (green & ~7)) * 4];
			}
			++source;
		} while (source < g_nativeBasePalette + 256);

		__asm
		{
			mov eax, offset g_nativeBasePalette
			add eax, 401h
			mov ecx, blueIndex
			mov edx, blueCube
		}
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

	// FUNCTION: TOY2 0x00471520
	int32_t UnkRenderAPI26(int32_t* primitiveWords)
	{
		volatile NativePrimitive* volatile primitive = (NativePrimitive*)primitiveWords;
		uint16_t flags = primitive->flags;
		uint32_t endU = (uint32_t)primitive->vertices[2].u - 1;
		uint32_t endV = (uint32_t)primitive->vertices[2].v - 1;
		primitive->vertices[2].u = (int32_t)endU;
		primitive->vertices[2].v = (int32_t)endV;
		volatile uint32_t sourceScratch = flags & 0x40;
		uint32_t tintOffset;
		if (flags & 0x40)
		{
			int32_t red = primitive->vertices[0].red;
			int32_t green = primitive->vertices[0].green;
			int32_t blue = primitive->vertices[0].blue;
			tintOffset = ((uint32_t)(red >> 12) & 0xFFFFFFC0) + ((uint32_t)(green >> 15) & 0xFFFFFFF8) + (blue >> 18);
		}
		int32_t x2 = primitive->vertices[2].x;
		int32_t x0 = primitive->vertices[0].x;
		int32_t y0 = primitive->vertices[0].y;
		int32_t widthDelta = (int32_t)((uint32_t)x2 - x0);
		if (! widthDelta)
			widthDelta = 1;
		int32_t y2 = primitive->vertices[2].y;
		int32_t heightDelta = (int32_t)((uint32_t)y2 - y0);
		if (! heightDelta)
			heightDelta = 1;
		uint32_t initialV = primitive->vertices[0].v;
		uint32_t startU = primitive->vertices[0].u;
		volatile uint32_t* runningV;
		// Indexed sprites publish V before either division and reload both endpoints.
		__asm
		{
			lea eax, primitiveWords
			mov runningV, eax
			mov edx, initialV
			mov primitiveWords, edx
		}
		int32_t numerator = (int32_t)((uint32_t)primitive->vertices[2].u - startU);
		int32_t stepU, stepV;
		__asm
		{
			mov eax, numerator
			cdq
			idiv widthDelta
			mov stepU, eax
		}
		uint32_t currentV = *runningV;
		numerator = (int32_t)((uint32_t)primitive->vertices[2].v - currentV);
		__asm
		{
			mov eax, numerator
			cdq
			idiv heightDelta
			mov stepV, eax
		}
		uint32_t width = (uint32_t)widthDelta + 1;
		uint32_t rows = (uint32_t)heightDelta + 1;
		int32_t top = Nu3D::Camera::g_viewportTop;
		int32_t left = Nu3D::Camera::g_viewportLeft;
		uint8_t* destination = g_nativePixels + (int32_t)((uint32_t)g_nativePixelPitch * top + left);
		uint8_t* texture = g_nativeTexturePages[primitive->page];
		if (y0 < top)
		{
			*runningV += ((uint32_t)top - y0) * stepV;
			rows += (uint32_t)y0 - top;
		}
		else
			destination += (int32_t)(((uint32_t)y0 - top) * g_nativePixelPitch);
		int32_t bottom = Nu3D::Camera::g_viewportRight;
		if (y2 > bottom)
			rows += (uint32_t)bottom - y2;
		left = Nu3D::Camera::g_viewportLeft;
		if (x0 < left)
		{
			startU += ((uint32_t)left - x0) * stepU;
			width += (uint32_t)x0 - left;
		}
		else
			destination += (int32_t)((uint32_t)x0 - left);
		int32_t right = Nu3D::Camera::g_viewportBottom;
		if (x2 >= right)
			width += (uint32_t)right - x2;

		if (flags & 0x20)
		{
			if (flags & 0x40)
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint8_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						*(volatile uint8_t*)&sourceScratch = raw;
						if (raw)
						{
							uint32_t sample = sourceScratch & 255;
							uint8_t* tint = *(uint8_t* volatile*)&g_nativeTintTable;
							uint8_t selected = ((volatile uint8_t*)tint)[512 * sample + tintOffset];
							uint8_t* table = *(uint8_t* volatile*)&g_nativeAddTable;
							uint8_t previous = *destination;
							*destination = table[256 * selected + previous];
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
			else
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint8_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						*(volatile uint8_t*)&sourceScratch = raw;
						if (raw)
						{
							uint32_t sample = sourceScratch & 255;
							uint8_t* table = *(uint8_t* volatile*)&g_nativeAddTable;
							uint8_t previous = *destination;
							*destination = table[256 * sample + previous];
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
		}
		else if (flags & 0x1000)
		{
			if (flags & 0x40)
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint8_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						*(volatile uint8_t*)&sourceScratch = raw;
						if (raw)
						{
							uint32_t sample = sourceScratch & 255;
							uint8_t* tint = *(uint8_t* volatile*)&g_nativeTintTable;
							uint8_t selected = ((volatile uint8_t*)tint)[512 * sample + tintOffset];
							uint8_t* table = *(uint8_t* volatile*)&g_nativeSubtractTable;
							uint8_t previous = *destination;
							*destination = table[256 * previous + selected];
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
			else
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint8_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						*(volatile uint8_t*)&sourceScratch = raw;
						if (raw)
						{
							uint32_t sample = sourceScratch & 255;
							uint8_t* table = *(uint8_t* volatile*)&g_nativeSubtractTable;
							uint8_t previous = *destination;
							*destination = table[256 * previous + sample];
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
		}
		else if (flags & 0x10)
		{
			if (flags & 0x40)
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint8_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						*(volatile uint8_t*)&sourceScratch = raw;
						if (raw)
						{
							uint32_t sample = sourceScratch & 255;
							uint8_t* tint = *(uint8_t* volatile*)&g_nativeTintTable;
							uint8_t selected = ((volatile uint8_t*)tint)[512 * sample + tintOffset];
							uint8_t* table = *(uint8_t* volatile*)&g_nativeBlend512;
							uint8_t previous = *destination;
							*destination = table[256 * selected + previous];
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
			else
			{
				do
				{
					uint32_t u = startU;
					uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
					uint32_t count = width;
					do
					{
						uint8_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
						*(volatile uint8_t*)&sourceScratch = raw;
						if (raw)
						{
							uint32_t sample = sourceScratch & 255;
							uint8_t* table = *(uint8_t* volatile*)&g_nativeBlend512;
							uint8_t previous = *destination;
							*destination = table[256 * sample + previous];
						}
						++destination;
						u += stepU;
					} while (--count);
					destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
					*runningV += stepV;
				} while (--rows);
			}
		}
		else if (flags & 0x40)
		{
			do
			{
				uint32_t u = startU;
				uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
				uint32_t count = width;
				do
				{
					uint8_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
					*(volatile uint8_t*)&sourceScratch = raw;
					if (raw)
					{
						uint32_t sample = sourceScratch & 255;
						uint8_t* tint = *(uint8_t* volatile*)&g_nativeTintTable;
						*destination = tint[512 * sample + tintOffset];
					}
					++destination;
					u += stepU;
				} while (--count);
				destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
				*runningV += stepV;
			} while (--rows);
		}
		else
		{
			do
			{
				uint32_t u = startU;
				uint32_t rowOffset = (uint32_t)((volatile uint8_t*)runningV)[2] << 8;
				uint32_t count = width;
				do
				{
					uint8_t raw = texture[(int32_t)(rowOffset + ((int32_t)u >> 16))];
					if (raw)
						*destination = raw;
					++destination;
					u += stepU;
				} while (--count);
				destination += (int32_t)((uint32_t)g_nativePixelPitch - width);
				*runningV += stepV;
			} while (--rows);
		}
		__asm
		{
			mov ecx, destination
			xor edx, edx
		}
		return (int32_t)texture;
	}

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

	// FUNCTION: TOY2 0x0047C800
	void UnkFunc4()
	{
		DDSURFACEDESC surfaceDesc;
		memset(&surfaceDesc, 0, sizeof(surfaceDesc));
		surfaceDesc.dwSize = sizeof(surfaceDesc);
		HRESULT result;
		do
		{
			result = D3DApp::g_d3dAppI.lpBackBuffer->Lock(0, &surfaceDesc, 0, 0);
			if (! result)
			{
				g_nativePixels = (uint8_t*)surfaceDesc.lpSurface;
				return;
			}
		} while (result == DDERR_WASSTILLDRAWING);
		g_nativePixels = 0;
		Logger::Log("SOFT : ERROR - Failed to lock back buffer - %s.\n", Logger::ErrorToMessage(result));
	}

	// FUNCTION: TOY2 0x0047C870
	void UnlockBackBuffer()
	{
		HRESULT result = D3DApp::g_d3dAppI.lpBackBuffer->Unlock(0);
		g_nativePixels = 0;
		if (result)
			Logger::Log("SOFT : ERROR - Failed to unlock back buffer - %s.\n", Logger::ErrorToMessage(result));
	}

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

	// FUNCTION: TOY2 0x0047D120
	void UnkFunc5()
	{
		volatile uint32_t savedEDX;
		__asm
		{
			mov savedEDX, edx
		}
		int32_t latch = g_unk830C60;
		if (latch)
		{
			__asm
			{
				xor eax, eax
				mov ecx, latch
				mov edx, savedEDX
			}
			return;
		}
		int32_t width = Nu3D::Camera::g_screenWidth;
		int32_t gap = (int32_t)((uint32_t)g_nativePixelPitch - width);
		if (g_bitsPerPixel == 8)
		{
			gap /= 4;
			width /= 4;
		}
		else
		{
			gap /= 2;
			width /= 2;
		}
		uint32_t rows = Nu3D::Camera::g_screenHeight;
		uint8_t* destination = g_nativePixels;
		do
		{
			uint32_t count = width;
			do
			{
				*(uint32_t*)destination = 0;
				destination += 4;
			} while (--count);
			destination += gap;
		} while (--rows);
		int32_t countdown = (int32_t)((uint32_t)g_nativeClearCountdown - 1);
		g_unk830C60 = 1;
		g_nativeClearCountdown = countdown;
		__asm
		{
			mov eax, countdown
			mov ecx, width
			mov edx, gap
		}
	}

	// STUB: TOY2 0x0047D210
	void UnkFunc8(int32_t unusedState, int32_t unused) {}

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

	// FUNCTION: TOY2 0x0048EB70
	void ResetNativeBltQueue()
	{
		memset(g_bltQueue, 0, sizeof(g_bltQueue));
		g_bltQueueCount = 0;
	}

	// FUNCTION: TOY2 0x0048EB90
	int16_t ConsumeNativeBltQueue()
	{
		for (int16_t index = 0; index < g_bltQueueCount; ++index)
		{
			BltCommand* command = &g_bltQueue[index];
			D3DApp::g_d3dAppI.lpBackBuffer->BltFast(
				command->destX, command->destY, (LPDIRECTDRAWSURFACE2)command->lpSurface, &command->srcRect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			if (command->lpSurface)
			{
				command->lpSurface->Release();
				command->lpSurface = 0;
			}
		}
		g_bltQueueCount = 0;
		__asm mov ax, 1
	}

	// FUNCTION: TOY2 0x0047F790
	void ResetNativeClip()
	{
		int32_t width = Nu3D::Camera::g_screenWidth;
		int32_t height = Nu3D::Camera::g_screenHeight;
		Nu3D::Camera::g_viewportWidth = width;
		Nu3D::Camera::g_viewportHeight = height;
		Nu3D::Camera::g_viewportBottom = (int32_t)((uint32_t)width - 1);
		Nu3D::Camera::g_viewportRight = (int32_t)((uint32_t)height - 1);
		Nu3D::Camera::g_viewportLeft = 0;
		Nu3D::Camera::g_viewportCenterX = width / 2;
		Nu3D::Camera::g_viewportTop = 0;
		int32_t centerY = height / 2;
		Nu3D::Camera::g_viewportCenterY = centerY;
		Nu3D::Camera::g_viewportLeftFixed10 = 0;
		Nu3D::Camera::g_viewportRightFixed10 = (int32_t)(((uint32_t)width << 10) - 1);
		__asm mov eax, centerY
	}

	// FUNCTION: TOY2 0x004617D0
	void DrawNativeStretch16(volatile NativeStretchArguments arguments)
	{
		uint16_t* row = (uint16_t*)g_nativePixels + (int32_t)((uint32_t)g_nativePixelPitch * arguments.y0 + arguments.x0);
		int32_t dx = (int32_t)((uint32_t)arguments.x1 - arguments.x0);
		int32_t dy = (int32_t)((uint32_t)arguments.y1 - arguments.y0);
		// These incoming cells retain the coordinate when its difference is zero.
		if (dx)
			arguments.y0 = (int32_t)((uint32_t)arguments.u1 - arguments.u0) / dx;
		if (dy)
			arguments.x0 = (int32_t)((uint32_t)arguments.endV - arguments.startV) / dy;
		uint32_t width = (uint32_t)dx + 1;
		arguments.y1 = (int32_t)((uint32_t)dy + 1);
		uint32_t rows;
		int32_t stepV, pitch;
		do
		{
			uint32_t rowOffset = (uint32_t)((volatile uint8_t*)&arguments.startV)[2] << 8;
			uint16_t* source = (uint16_t*)((uint8_t* volatile*)g_nativeTexturePages)[arguments.page] + rowOffset;
			uint32_t u = arguments.u0;
			uint16_t* destination = row;
			uint32_t count = width;
			do
			{
				u = (u + arguments.y0) & 0x00FFFFFF;
				*destination++ = source[u >> 16];
			} while (--count);
			stepV = arguments.x0;
			uint32_t nextV = (uint32_t)arguments.startV + stepV;
			rows = arguments.y1;
			arguments.startV = (int32_t)nextV;
			pitch = *(volatile int32_t*)&g_nativePixelPitch;
			row += pitch;
			arguments.y1 = (int32_t)--rows;
		} while (rows);
		__asm
		{
			xor eax, eax
			mov ecx, stepV
			mov edx, pitch
		}
	}

	// FUNCTION: TOY2 0x00477DF0
	void DrawNativeStretch8(volatile NativeStretchArguments arguments)
	{
		uint8_t* row = g_nativePixels + (int32_t)((uint32_t)g_nativePixelPitch * arguments.y0 + arguments.x0);
		int32_t dx = (int32_t)((uint32_t)arguments.x1 - arguments.x0);
		int32_t dy = (int32_t)((uint32_t)arguments.y1 - arguments.y0);
		if (dx)
			arguments.y0 = (int32_t)((uint32_t)arguments.u1 - arguments.u0) / dx;
		if (dy)
			arguments.x0 = (int32_t)((uint32_t)arguments.endV - arguments.startV) / dy;
		uint32_t width = (uint32_t)dx + 1;
		arguments.y1 = (int32_t)((uint32_t)dy + 1);
		uint32_t rows;
		uint32_t nextV;
		int32_t pitch;
		do
		{
			uint32_t rowOffset = (uint32_t)((volatile uint8_t*)&arguments.startV)[2] << 8;
			uint8_t* source = ((uint8_t* volatile*)g_nativeTexturePages)[arguments.page] + rowOffset;
			uint32_t u = arguments.u0;
			uint8_t* destination = row;
			uint32_t count = width;
			do
			{
				u = (u + arguments.y0) & 0x00FFFFFF;
				*destination++ = source[u >> 16];
			} while (--count);
			int32_t stepV = arguments.x0;
			nextV = (uint32_t)arguments.startV + stepV;
			rows = arguments.y1;
			pitch = *(volatile int32_t*)&g_nativePixelPitch;
			arguments.startV = (int32_t)nextV;
			row += pitch;
			arguments.y1 = (int32_t)--rows;
		} while (rows);
		__asm
		{
			xor eax, eax
			mov ecx, pitch
			mov edx, nextV
		}
	}

	// FUNCTION: TOY2 0x00462D00
	int32_t DrawNativeGlyph16(volatile NativeGlyphArguments arguments)
	{
		uint8_t* glyph = g_nativeGlyphs[arguments.code & 255];
		int32_t drawX = arguments.x;
		int32_t advance = glyph[35];
		int32_t screenWidth = Nu3D::Camera::g_screenWidth;
		uint32_t resultEDX = drawX;
		arguments.code = (uint32_t)glyph;
		do
		{
			if (drawX >= screenWidth || drawX <= -5)
				break;
			int32_t drawY = arguments.y;
			int32_t screenHeight = Nu3D::Camera::g_screenHeight;
			if (drawY >= screenHeight || drawY <= -7)
				break;
			uint16_t* destination = (uint16_t*)g_nativePixels;
			int32_t width;
			if (drawX < 0)
			{
				glyph -= drawX;
				width = drawX + 5;
				arguments.code = (uint32_t)glyph;
			}
			else
			{
				width = 5;
				destination += drawX;
			}
			if (drawX > screenWidth - 5)
				width += screenWidth - drawX - 5;
			int32_t pitch = g_nativePixelPitch;
			int32_t height;
			if (drawY < 0)
			{
				glyph -= 5 * drawY;
				height = drawY + 7;
				arguments.code = (uint32_t)glyph;
				screenHeight = *(volatile int32_t*)&Nu3D::Camera::g_screenHeight;
			}
			else
			{
				height = 7;
				destination += (int32_t)((uint32_t)pitch * drawY);
			}
			if (drawY > screenHeight - 7)
				height += screenHeight - drawY - 7;
			resultEDX = height;
			if (height > 0)
			{
				arguments.x = height;
				uint16_t foreground;
				__asm
				{
					lea eax, arguments
					mov ax, word ptr [eax + 8]
					mov foreground, ax
				}
				resultEDX = ((uint32_t)height & 0xFFFF0000) | foreground;
				do
				{
					if (width > 0)
					{
						int32_t count = width;
						do
						{
							if (*glyph++)
							{
								destination[pitch + 1] = 0;
								*destination = foreground;
								pitch = *(volatile int32_t*)&g_nativePixelPitch;
							}
							++destination;
						} while (--count);
					}
					destination += pitch - width;
					glyph += 5 - width;
					height = (int32_t)((uint32_t)arguments.x - 1);
					arguments.x = height;
				} while (height);
			}
		} while (0);
		__asm
		{
			mov eax, advance
			mov ecx, advance
			mov edx, resultEDX
		}
	}

	// FUNCTION: TOY2 0x00462EC0
	int32_t DrawNativeGlyph16Unclipped(int32_t x, int32_t y, uint32_t colour, uint32_t code)
	{
		uint16_t* destination = (uint16_t*)g_nativePixels + (int32_t)((uint32_t)g_nativePixelPitch * y + x);
		uint8_t* glyph = g_nativeGlyphs[code & 255];
		uint16_t foreground = (uint16_t)colour;
		int32_t rows = 7;
		do
		{
			int32_t count = 5;
			do
			{
				if (*glyph++)
				{
					destination[Nu3D::Camera::g_screenWidth + 1] = 0;
					*destination = foreground;
				}
				++destination;
			} while (--count);
			destination += g_nativePixelPitch - 5;
		} while (--rows);
		return *glyph;
	}

	// FUNCTION: TOY2 0x00477BB0
	int32_t DrawNativeGlyph8(volatile NativeGlyphArguments arguments)
	{
		volatile uint32_t resultECX;
		__asm
		{
			mov resultECX, ecx
		}
		uint8_t* glyph = g_nativeGlyphs[arguments.code & 255];
		int32_t drawX = arguments.x;
		int32_t advance = glyph[35];
		int32_t screenWidth = Nu3D::Camera::g_screenWidth;
		int32_t resultEDX = drawX;
		arguments.code = advance;
		do
		{
			if (drawX >= screenWidth || drawX <= -5)
				break;
			int32_t drawY = arguments.y;
			int32_t screenHeight = Nu3D::Camera::g_screenHeight;
			resultECX = screenHeight;
			if (drawY >= screenHeight || drawY <= -7)
				break;
			uint8_t* destination = g_nativePixels;
			int32_t width;
			if (drawX < 0)
			{
				width = drawX + 5;
				glyph -= drawX;
			}
			else
			{
				width = 5;
				destination += drawX;
			}
			if (drawX > screenWidth - 5)
				width += screenWidth - drawX - 5;
			int32_t pitch = g_nativePixelPitch;
			int32_t height;
			if (drawY < 0)
			{
				height = drawY + 7;
				glyph -= 5 * drawY;
			}
			else
			{
				height = 7;
				destination += (int32_t)((uint32_t)pitch * drawY);
			}
			if (drawY > *(volatile int32_t*)&Nu3D::Camera::g_screenHeight - 7)
				height += *(volatile int32_t*)&Nu3D::Camera::g_screenHeight - drawY - 7;
			if (height > 0)
			{
				do
				{
					if (width > 0)
					{
						int32_t count = width;
						do
						{
							if (*glyph++)
							{
								destination[pitch + 1] = g_nativeColourCube[0];
								*destination = *(volatile uint8_t*)&arguments.colour;
								pitch = *(volatile int32_t*)&g_nativePixelPitch;
							}
							++destination;
						} while (--count);
					}
					destination += pitch - width;
					glyph += 5 - width;
				} while (--height);
			}
			resultECX = (uint32_t)destination;
			resultEDX = height;
		} while (0);
		__asm
		{
			mov eax, arguments.code
			mov ecx, resultECX
			mov edx, resultEDX
		}
	}

	// FUNCTION: TOY2 0x004AC1F0
	int32_t GetCurrentTexture(TextureSnapshot* texture)
	{
		Nu3D::BmpDataNode* node = Nu3D::g_currentBmpDataNode;
		if (node)
		{
			uint32_t* texData = node->texData;
			texture->surfaceDesc = &node->surfaceDesc;
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

	// FUNCTION: TOY2 0x004BC900
	uint32_t UnkFunc27(uint32_t colour, float* red, float* green, float* blue, uint32_t* alphaMask)
	{
		*red = (float)(((colour >> 16) & 0xFFu) * (1.0 / 255.0));
		*green = (float)(((colour >> 8) & 0xFFu) * (1.0 / 255.0));
		uint32_t blueChannel = colour & 0xFFu;
		colour &= 0xFF000000u;
		*blue = (float)(blueChannel * (1.0 / 255.0));
		*alphaMask = colour;
		return colour;
	}

	// FUNCTION: TOY2 0x004BC980
	void UnkFunc30(BVertex* const* vertices, int32_t count, uint32_t* texture, uint32_t flags, int32_t layer)
	{
		int32_t index = g_depthPolygonCount;
		if (index >= 15000)
			return;

		uint32_t key;
		if (g_unk9F6008 == 1)
			key = 2u - (uint32_t)(int64_t)(g_sortDepth * -25000.0f);
		else
			key = (uint32_t)(int64_t)g_sortDepth;

		if (g_samplingBand & 1)
			key += (uint32_t)g_farLayerOffsets[layer] * 3u + 1100u;
		else
			key += (uint32_t)g_nearLayerOffsets[layer] * 3u;

		int32_t bucket = (int32_t)key;
		if (bucket >= 30000)
			return;

		// Pool and head offsets use the original low-DWORD address arithmetic.
		BPolygon* polygon = (BPolygon*)((uint32_t)g_depthPolygons + (uint32_t)index * sizeof(BPolygon));
		g_depthPolygonCount = (int32_t)((uint32_t)index + 1u);
		memcpy(&polygon->vertices[0], vertices[0], sizeof(BVertex));
		memcpy(&polygon->vertices[1], vertices[1], sizeof(BVertex));
		memcpy(&polygon->vertices[2], vertices[2], sizeof(BVertex));
		if (count == 4)
			memcpy(&polygon->vertices[3], vertices[3], sizeof(BVertex));

		polygon->count = count;
		polygon->texture = texture;
		polygon->flags = flags;
		BPolygon** head = (BPolygon**)((uint32_t)g_polygonHeads + (uint32_t)bucket * sizeof(BPolygon*));
		polygon->next = *head;
		int32_t band = g_samplingBand;
		*head = polygon;
		polygon->band = band;
	}

	// FUNCTION: TOY2 0x004BCAD0
	void UnkFunc29(BVertex* const* vertices, int32_t count, uint32_t* texture, uint32_t flags)
	{
		int32_t index = g_fifoPolygonCount;
		if (index >= 1024)
			return;

		BPolygon* polygon = (BPolygon*)((uint32_t)g_fifoPolygons + (uint32_t)index * sizeof(BPolygon));
		g_fifoPolygonCount = (int32_t)((uint32_t)index + 1u);
		memcpy(&polygon->vertices[0], vertices[0], sizeof(BVertex));
		memcpy(&polygon->vertices[1], vertices[1], sizeof(BVertex));
		memcpy(&polygon->vertices[2], vertices[2], sizeof(BVertex));
		if (count == 4)
			memcpy(&polygon->vertices[3], vertices[3], sizeof(BVertex));

		polygon->count = count;
		polygon->texture = texture;
		polygon->flags = flags;
	}

	// FUNCTION: TOY2 0x004BCC70
	void UnkFunc17(int32_t left, int32_t top, int32_t right, int32_t bottom)
	{
		int32_t halfWidth;
		int32_t halfHeight;
		volatile float scaleX;
		volatile float scaleY;

		if (left != g_clipCache[0] || right != g_clipCache[2])
		{
			halfWidth = g_surfaceWidth >> 1;
			int32_t extent = (int32_t)((uint32_t)g_rightOffset - (uint32_t)g_leftOffset + 1u);
			scaleX = (float)(((double)extent * 1.7) / g_surfaceWidth);
		}
		if (top != g_clipCache[1] || bottom != g_clipCache[3])
		{
			halfHeight = g_surfaceHeight >> 1;
			int32_t extent = (int32_t)((uint32_t)g_bottomOffset - (uint32_t)g_topOffset + 1u);
			scaleY = (float)(((double)extent * 1.9) / g_surfaceHeight);
		}

		if (left != g_clipCache[0])
		{
			g_clipCache[0] = left;
			int32_t displacement = (int32_t)((uint32_t)left - (uint32_t)halfWidth);
			uint32_t converted = (uint32_t)(int64_t)((double)displacement * scaleX);
			int32_t edge = (int32_t)(converted + (uint32_t)halfWidth - 4u);
			if (edge < g_leftOffset)
				edge = g_leftOffset;
			g_clipLeft = edge;
		}
		if (right != g_clipCache[2])
		{
			g_clipCache[2] = right;
			int32_t displacement = (int32_t)((uint32_t)right - (uint32_t)halfWidth);
			uint32_t converted = (uint32_t)(int64_t)((double)displacement * scaleX);
			int32_t edge = (int32_t)(converted + (uint32_t)halfWidth + 4u);
			if (edge > g_rightOffset)
				edge = g_rightOffset;
			g_clipRight = edge;
		}
		if (top != g_clipCache[1])
		{
			g_clipCache[1] = top;
			int32_t displacement = (int32_t)((uint32_t)top - (uint32_t)halfHeight);
			uint32_t converted = (uint32_t)(int64_t)((double)displacement * scaleY);
			int32_t edge = (int32_t)(converted + (uint32_t)halfHeight - 6u);
			if (edge < g_topOffset)
				edge = g_topOffset;
			g_clipTop = edge;
		}
		if (bottom != g_clipCache[3])
		{
			g_clipCache[3] = bottom;
			int32_t displacement = (int32_t)((uint32_t)bottom - (uint32_t)halfHeight);
			uint32_t converted = (uint32_t)(int64_t)((double)displacement * scaleY);
			int32_t edge = (int32_t)(converted + (uint32_t)halfHeight + 32u);
			if (edge > g_bottomOffset)
				edge = g_bottomOffset;
			g_clipBottom = edge;
		}
	}

	// FUNCTION: TOY2 0x004BCF60
	BOOL UnkFunc28(const BVertex* a, const BVertex* b, const BVertex* c)
	{
		// One x87 expression retains the determinant; unordered also passes.
		return ! (((c->y - b->y) * (a->x - b->x) - (a->y - b->y) * (c->x - b->x)) >= 0.0);
	}

	static void UpdateSoftwareClipRect()
	{
		if (g_viewportRect)
		{
			// Getter storage is left, top, right, bottom.
			const float* rect = (const float*)g_viewportRect;
			UnkFunc17((int32_t)(int64_t)rect[0], (int32_t)(int64_t)rect[1], (int32_t)(int64_t)rect[2], (int32_t)(int64_t)rect[3]);
		}
	}

	// FUNCTION: TOY2 0x004BCFA0
	void UnkFunc26(BVertex* vertex)
	{
		float halfWidth = (float)((double)g_surfaceWidth * 0.5);
		float halfHeight = (float)((double)g_surfaceHeight * 0.5);
		vertex->cameraX = (int32_t)(int64_t)vertex->x;
		vertex->cameraY = vertex->y;
		if (vertex->z > 0.0)
		{
			vertex->x = (float)((((double)halfWidth * vertex->x) / ((double)vertex->z + 1.0)) * g_zoomScaleX * 1.7 + g_surfaceWidth / 2);
			vertex->y = (float)((((double)halfHeight * vertex->y) / ((double)vertex->z + 1.0)) * g_zoomScaleY * 1.9 + g_surfaceHeight / 2);
		}
	}

	static inline uint16_t CompareGeometryFloat(float value, const volatile float* bound)
	{
		uint16_t statusWord;
		__asm
		{
			mov ecx, bound
			fld value
			fcomp dword ptr [ecx]
			fnstsw ax
			mov statusWord, ax
		}
		return statusWord;
	}

	static inline BOOL SubdivisionUsesNearPass()
	{
		const float* currentNearZ = &Nu3D::Camera::g_secondaryCamera.nearZ;
		uint16_t statusWord;
		__asm
		{
			mov eax, currentNearZ
			fld dword ptr [eax]
			fcomp g_cameraNearZ
			fnstsw ax
			mov statusWord, ax
		}
		// C3 selects both equal and unordered comparisons.
		return (statusWord & 0x4000) != 0;
	}

	static inline float SubdivisionScreenMidpointX(int32_t first, int32_t second)
	{
		int32_t sum = (int32_t)((uint32_t)first + (uint32_t)second);
		const float half = 0.5f;
		float midpoint;
		__asm
		{
			fild sum
			fmul half
			fstp midpoint
		}
		return midpoint;
	}

	static inline uint32_t SubdivisionMidpointColour(const RGBColor& first, const RGBColor& second, uint32_t alphaMask)
	{
		uint32_t red = (uint32_t)(int64_t)(((double)first.r + second.r) * 0.5 * 255.0f);
		uint32_t green = (uint32_t)(int64_t)(((double)first.g + second.g) * 0.5 * 255.0f);
		uint32_t blue = (uint32_t)(int64_t)(((double)first.b + second.b) * 0.5 * 255.0f);
		return (((((red | 0xFFFFFF00u) << 8) | green) << 8) | blue) + alphaMask;
	}

	static inline uint32_t SubdivisionCentroidChannel(float first, float second, float third)
	{
		const double weight = 0.3333333333333333;
		const float scale = 255.0f;
		uint16_t controlWord, truncateWord;
		int64_t channel;
		// Separate multiplies retain the PC64 result immediately below an integer.
		__asm
		{
			fld first
			fadd second
			fadd third
			fmul weight
			fmul scale
			fnstcw controlWord
			mov ax, controlWord
			or ax, 0C00h
			mov truncateWord, ax
			fldcw truncateWord
			fistp channel
			fldcw controlWord
		}
		return (uint32_t)channel;
	}

	static inline uint32_t SubdivisionCentroidColour(const RGBColor& first, const RGBColor& second, const RGBColor& third, uint32_t alphaMask)
	{
		uint32_t red = SubdivisionCentroidChannel(first.r, second.r, third.r);
		uint32_t green = SubdivisionCentroidChannel(first.g, second.g, third.g);
		uint32_t blue = SubdivisionCentroidChannel(first.b, second.b, third.b);
		return (((((red | 0xFFFFFF00u) << 8) | green) << 8) | blue) + alphaMask;
	}

	// FUNCTION: TOY2 0x004BD040
	void UnkFunc24(BVertex* const* vertices, int32_t count, uint32_t* texture, uint32_t flags, int32_t layer, int32_t recursion)
	{
		BVertex* vertex3 = vertices[3];
		BVertex* vertex1 = vertices[1];
		BVertex* vertex0 = vertices[0];
		BVertex* vertex2 = vertices[2];
		if (g_unk9F6008 == 1)
		{
			int32_t savedLeft, savedTop, savedRight, savedBottom;
			if (! (vertex0->z >= 0.9))
			{
				savedLeft = g_clipLeft;
				savedTop = g_clipTop;
				savedBottom = g_clipBottom;
				savedRight = g_clipRight;
				g_clipTop = 0;
				g_clipLeft = 0;
				g_clipRight = (int32_t)((uint32_t)g_surfaceWidth - 1);
				g_clipBottom = (int32_t)((uint32_t)g_surfaceHeight - 1);
			}

			g_sortDepth = vertex0->z;
			BVertex hud[4];
			memcpy(&hud[0], vertex0, sizeof(BVertex));
			memcpy(&hud[1], vertex1, sizeof(BVertex));
			memcpy(&hud[2], vertex2, sizeof(BVertex));
			BVertex* hudVertices[4];
			hudVertices[0] = &hud[0];
			hudVertices[1] = &hud[1];
			hudVertices[2] = &hud[2];
			if (count == 3)
			{
				UnkFunc25(hudVertices, 3, texture, flags, layer);
			}
			else
			{
				memcpy(&hud[3], vertex3, sizeof(BVertex));
				if (hud[0].z > 0.9)
				{
					hud[0].x = (float)((double)g_viewScaleX * hud[0].x + g_viewOffsetX);
					hud[1].x = (float)((double)g_viewScaleX * hud[1].x + g_viewOffsetX);
					hud[2].x = (float)((double)g_viewScaleX * hud[2].x + g_viewOffsetX);
					hud[3].x = (float)((double)g_viewScaleX * hud[3].x + g_viewOffsetX);
					hud[0].y = (float)((double)g_viewScaleY * hud[0].y + g_viewOffsetY);
					hud[1].y = (float)((double)hud[1].y * g_viewScaleY + g_viewOffsetY);
					hud[2].y = (float)((double)hud[2].y * g_viewScaleY + g_viewOffsetY);
					hud[3].y = (float)((double)hud[3].y * g_viewScaleY + g_viewOffsetY);
				}
				hudVertices[3] = &hud[3];
				UnkFunc25(hudVertices, 4, texture, flags, layer);
			}

			if (! (vertex0->z >= 0.9))
			{
				g_clipTop = savedTop;
				g_clipBottom = savedBottom;
				g_clipLeft = savedLeft;
				g_clipRight = savedRight;
			}
			return;
		}

		int32_t active = 0;
		if (SubdivisionUsesNearPass())
		{
			volatile float activationZ;
			if (g_levelFileIndex == 3 || g_levelFileIndex == 4 || g_levelFileIndex == 11)
				activationZ = g_nearDistance;
			else
				activationZ = (float)(((double)g_farDistance - g_nearDistance) * 0.3 + g_nearDistance);
			g_samplingBand = 0;
			if ((CompareGeometryFloat(vertex0->z, &activationZ) & 0x4100) || (CompareGeometryFloat(vertex1->z, &activationZ) & 0x4100)
				|| (CompareGeometryFloat(vertex2->z, &activationZ) & 0x4100))
				active = 1;
			if (count == 4)
			{
				if (CompareGeometryFloat(vertex3->z, &activationZ) & 0x4100)
					active = 1;
				g_sortDepth = (float)(((((double)vertex2->z + vertex1->z) + vertex0->z) + vertex3->z) * 0.25f);
			}
			else
			{
				g_sortDepth = (float)((((double)vertex2->z + vertex1->z) + vertex0->z) * 0.3333333432674408f);
			}
			g_depthClipZ = 1.0f;
		}
		else
		{
			const double farWeight = 0.7;
			const float meanQuarter = 0.25f;
			const float meanThird = 0.3333333432674408f;
			// Activation uses the unspilled x87 plane; classification uses its binary32 store.
			__asm
			{
				fld g_farDistance
				cmp g_levelFileIndex, 3
				je LBL_FAR_THRESHOLD_READY
				fsub g_nearDistance
				fmul farWeight
				fadd g_nearDistance
			LBL_FAR_THRESHOLD_READY:
				mov g_samplingBand, 1
				mov ecx, vertex0
				fcom dword ptr [ecx + 8]
				fnstsw ax
				test ah, 41h
				jnz LBL_FAR_ACTIVE
				mov ecx, vertex1
				fcom dword ptr [ecx + 8]
				fnstsw ax
				test ah, 41h
				jnz LBL_FAR_ACTIVE
				mov ecx, vertex2
				fcom dword ptr [ecx + 8]
				fnstsw ax
				test ah, 41h
				jz LBL_FAR_MEAN
			LBL_FAR_ACTIVE:
				mov active, 1
			LBL_FAR_MEAN:
				mov ecx, vertex2
				fld dword ptr [ecx + 8]
				mov edx, vertex1
				fadd dword ptr [edx + 8]
				mov ecx, vertex0
				fadd dword ptr [ecx + 8]
				cmp count, 4
				jne LBL_FAR_TRIANGLE_MEAN
				mov ecx, vertex3
				fadd dword ptr [ecx + 8]
				fmul meanQuarter
				fld dword ptr [ecx + 8]
				fcomp st(2)
				fnstsw ax
				test ah, 1
				jnz LBL_FAR_STORE
				mov active, 1
				jmp LBL_FAR_STORE
			LBL_FAR_TRIANGLE_MEAN:
				fmul meanThird
			LBL_FAR_STORE:
				fstp g_sortDepth
				fstp g_depthClipZ
			}
		}
		if (! active)
			return;

		if (count == 3)
		{
			if (vertex0->z >= g_depthClipZ && vertex1->z >= g_depthClipZ && vertex2->z >= g_depthClipZ)
			{
				UnkFunc25(vertices, 3, texture, flags, layer);
				return;
			}
			if (! (vertex0->z >= g_depthClipZ || vertex1->z >= g_depthClipZ || vertex2->z >= g_depthClipZ))
				return;

			RGBColor colour0, colour1, colour2;
			uint32_t alphaMask;
			UnkFunc27(vertex0->diffuse, &colour0.r, &colour0.g, &colour0.b, &alphaMask);
			UnkFunc27(vertex1->diffuse, &colour1.r, &colour1.g, &colour1.b, &alphaMask);
			UnkFunc27(vertex2->diffuse, &colour2.r, &colour2.g, &colour2.b, &alphaMask);

			BVertex center, midpoint01, midpoint12, midpoint20;
			int32_t cameraXSum = (int32_t)(((uint32_t)vertex1->cameraX + (uint32_t)vertex2->cameraX) + (uint32_t)vertex0->cameraX);
			center.x = (float)((double)cameraXSum * 0.3333333333333333);
			center.y = (float)((((double)vertex1->cameraY + vertex2->cameraY) + vertex0->cameraY) * 0.3333333333333333);
			center.z = (float)((((double)vertex2->z + vertex1->z) + vertex0->z) * 0.3333333333333333);
			center.u = (float)((((double)vertex1->u + vertex2->u) + vertex0->u) * 0.3333333333333333);
			center.v = (float)((((double)vertex1->v + vertex2->v) + vertex0->v) * 0.3333333333333333);
			center.diffuse = SubdivisionCentroidColour(colour2, colour1, colour0, alphaMask);
			UnkFunc26(&center);

			cameraXSum = (int32_t)((uint32_t)vertex1->cameraX + (uint32_t)vertex0->cameraX);
			midpoint01.x = (float)((double)cameraXSum * 0.5);
			midpoint01.y = (float)(((double)vertex1->cameraY + vertex0->cameraY) * 0.5);
			midpoint01.z = (float)(((double)vertex1->z + vertex0->z) * 0.5);
			midpoint01.u = (float)(((double)vertex1->u + vertex0->u) * 0.5);
			midpoint01.v = (float)(((double)vertex1->v + vertex0->v) * 0.5);
			midpoint01.diffuse = SubdivisionMidpointColour(colour1, colour0, alphaMask);
			UnkFunc26(&midpoint01);

			cameraXSum = (int32_t)((uint32_t)vertex1->cameraX + (uint32_t)vertex2->cameraX);
			midpoint12.x = (float)((double)cameraXSum * 0.5);
			midpoint12.y = (float)(((double)vertex1->cameraY + vertex2->cameraY) * 0.5);
			midpoint12.z = (float)(((double)vertex2->z + vertex1->z) * 0.5);
			midpoint12.u = (float)(((double)vertex1->u + vertex2->u) * 0.5);
			midpoint12.v = (float)(((double)vertex1->v + vertex2->v) * 0.5);
			midpoint12.diffuse = SubdivisionMidpointColour(colour2, colour1, alphaMask);
			UnkFunc26(&midpoint12);

			cameraXSum = (int32_t)((uint32_t)vertex2->cameraX + (uint32_t)vertex0->cameraX);
			midpoint20.x = (float)((double)cameraXSum * 0.5);
			midpoint20.y = (float)(((double)vertex2->cameraY + vertex0->cameraY) * 0.5);
			midpoint20.z = (float)(((double)vertex2->z + vertex0->z) * 0.5);
			midpoint20.u = (float)(((double)vertex2->u + vertex0->u) * 0.5);
			midpoint20.v = (float)(((double)vertex2->v + vertex0->v) * 0.5);
			midpoint20.diffuse = SubdivisionMidpointColour(colour2, colour0, alphaMask);
			UnkFunc26(&midpoint20);

			int32_t childRecursion = (int32_t)((uint32_t)recursion + 1);
			BVertex* child[4];
			child[0] = &center;
			child[1] = &midpoint01;
			child[2] = vertex1;
			child[3] = &midpoint12;
			UnkFunc24(child, 4, texture, flags, layer, childRecursion);
			child[0] = &center;
			child[1] = &midpoint12;
			child[2] = vertex2;
			child[3] = &midpoint20;
			UnkFunc24(child, 4, texture, flags, layer, childRecursion);
			child[0] = &center;
			child[1] = &midpoint20;
			child[2] = vertex0;
			child[3] = &midpoint01;
			UnkFunc24(child, 4, texture, flags, layer, childRecursion);
			return;
		}

		if (count != 4)
			return;
		if (vertex0->z >= g_depthClipZ && vertex1->z >= g_depthClipZ && vertex2->z >= g_depthClipZ && vertex3->z >= g_depthClipZ)
		{
			UnkFunc25(vertices, 4, texture, flags, layer);
			return;
		}
		if (! (vertex0->z >= g_depthClipZ || vertex1->z >= g_depthClipZ || vertex2->z >= g_depthClipZ || vertex3->z >= g_depthClipZ))
			return;
		if (recursion > 1)
			return;

		RGBColor colour0, colour1, colour2, colour3;
		uint32_t alphaMask;
		UnkFunc27(vertex0->diffuse, &colour0.r, &colour0.g, &colour0.b, &alphaMask);
		UnkFunc27(vertex1->diffuse, &colour1.r, &colour1.g, &colour1.b, &alphaMask);
		UnkFunc27(vertex2->diffuse, &colour2.r, &colour2.g, &colour2.b, &alphaMask);
		UnkFunc27(vertex3->diffuse, &colour3.r, &colour3.g, &colour3.b, &alphaMask);

		BVertex center, midpoint01, midpoint12, midpoint23, midpoint30;
		int32_t cameraXSum = (int32_t)((uint32_t)vertex2->cameraX + (uint32_t)vertex0->cameraX);
		center.x = (float)((double)cameraXSum * 0.5);
		center.y = (float)(((double)vertex2->cameraY + vertex0->cameraY) * 0.5);
		center.z = (float)(((double)vertex2->z + vertex0->z) * 0.5);
		center.u = (float)(((double)vertex2->u + vertex0->u) * 0.5);
		center.v = (float)(((double)vertex2->v + vertex0->v) * 0.5);
		center.diffuse = SubdivisionMidpointColour(colour2, colour0, alphaMask);
		UnkFunc26(&center);

		cameraXSum = (int32_t)((uint32_t)vertex1->cameraX + (uint32_t)vertex0->cameraX);
		midpoint01.x = (float)((double)cameraXSum * 0.5);
		midpoint01.y = (float)(((double)vertex1->cameraY + vertex0->cameraY) * 0.5);
		midpoint01.z = (float)(((double)vertex1->z + vertex0->z) * 0.5);
		midpoint01.u = (float)(((double)vertex1->u + vertex0->u) * 0.5);
		midpoint01.v = (float)(((double)vertex1->v + vertex0->v) * 0.5);
		midpoint01.diffuse = SubdivisionMidpointColour(colour1, colour0, alphaMask);
		UnkFunc26(&midpoint01);

		cameraXSum = (int32_t)((uint32_t)vertex1->cameraX + (uint32_t)vertex2->cameraX);
		midpoint12.x = (float)((double)cameraXSum * 0.5);
		midpoint12.y = (float)(((double)vertex1->cameraY + vertex2->cameraY) * 0.5);
		midpoint12.z = (float)(((double)vertex2->z + vertex1->z) * 0.5);
		midpoint12.u = (float)(((double)vertex1->u + vertex2->u) * 0.5);
		midpoint12.v = (float)(((double)vertex1->v + vertex2->v) * 0.5);
		midpoint12.diffuse = SubdivisionMidpointColour(colour2, colour1, alphaMask);
		UnkFunc26(&midpoint12);

		cameraXSum = (int32_t)((uint32_t)vertex3->cameraX + (uint32_t)vertex2->cameraX);
		midpoint23.x = (float)((double)cameraXSum * 0.5);
		midpoint23.y = (float)(((double)vertex2->cameraY + vertex3->cameraY) * 0.5);
		midpoint23.z = (float)(((double)vertex2->z + vertex3->z) * 0.5);
		midpoint23.u = (float)(((double)vertex3->u + vertex2->u) * 0.5);
		midpoint23.v = (float)(((double)vertex3->v + vertex2->v) * 0.5);
		midpoint23.diffuse = SubdivisionMidpointColour(colour3, colour2, alphaMask);
		UnkFunc26(&midpoint23);

		cameraXSum = (int32_t)((uint32_t)vertex3->cameraX + (uint32_t)vertex0->cameraX);
		midpoint30.x = (float)((double)cameraXSum * 0.5);
		midpoint30.y = (float)(((double)vertex0->cameraY + vertex3->cameraY) * 0.5);
		midpoint30.z = (float)(((double)vertex3->z + vertex0->z) * 0.5);
		midpoint30.u = (float)(((double)vertex3->u + vertex0->u) * 0.5);
		midpoint30.v = (float)(((double)vertex3->v + vertex0->v) * 0.5);
		midpoint30.diffuse = SubdivisionMidpointColour(colour3, colour0, alphaMask);
		UnkFunc26(&midpoint30);

		int32_t childRecursion = (int32_t)((uint32_t)recursion + 1);
		BVertex* child[4];
		child[0] = &center;
		child[1] = &midpoint01;
		child[2] = vertex1;
		child[3] = &midpoint12;
		UnkFunc24(child, 4, texture, flags, layer, childRecursion);
		child[0] = &center;
		child[1] = &midpoint30;
		child[2] = vertex0;
		child[3] = &midpoint01;
		UnkFunc24(child, 4, texture, flags, layer, childRecursion);
		child[0] = &center;
		child[1] = &midpoint12;
		child[2] = vertex2;
		child[3] = &midpoint23;
		UnkFunc24(child, 4, texture, flags, layer, childRecursion);
		child[0] = &center;
		child[1] = &midpoint23;
		child[2] = vertex3;
		child[3] = &midpoint30;
		UnkFunc24(child, 4, texture, flags, layer, childRecursion);
	}

	static inline uint32_t Clip25InterpolateColour(uint32_t previousColour, uint32_t followingColour, float denominator, float distance)
	{
		float previousRed, previousGreen, previousBlue;
		float followingRed, followingGreen, followingBlue;
		uint32_t alphaMask;
		UnkFunc27(previousColour, &previousRed, &previousGreen, &previousBlue, &alphaMask);
		UnkFunc27(followingColour, &followingRed, &followingGreen, &followingBlue, &alphaMask);

		// Each normalized channel is stored before the multiply and truncation.
		volatile float red = ((followingRed - previousRed) / denominator) * distance + previousRed;
		volatile float green = ((followingGreen - previousGreen) / denominator) * distance + previousGreen;
		volatile float blue = ((followingBlue - previousBlue) / denominator) * distance + previousBlue;
		uint32_t colour = ((uint32_t)(int64_t)(red * 255.0f) & 0xFF) << 8;
		colour = (colour | ((uint32_t)(int64_t)(green * 255.0f) & 0xFFFF)) << 8;
		colour |= (uint32_t)(int64_t)(blue * 255.0f) & 0xFFFFFF;
		return colour + alphaMask;
	}

	// FUNCTION: TOY2 0x004BE0B0
	void UnkFunc25(BVertex* const* vertices, int32_t count, uint32_t* texture, uint32_t flags, int32_t layer)
	{
		int32_t outsideCount = 0;
		int32_t topCount = 0;
		int32_t bottomCount = 0;
		int32_t leftCount = 0;
		int32_t rightCount = 0;
		int32_t index;

		if (count > 0)
		{
			double top = g_clipTop;
			double bottom = g_clipBottom;
			double left = g_clipLeft;
			double right = g_clipRight;
			for (index = 0; index < count; ++index)
			{
				BVertex* vertex = vertices[index];
				if (! (vertex->y >= top))
				{
					++outsideCount;
					++topCount;
				}
				if (vertex->y > bottom)
				{
					++outsideCount;
					++bottomCount;
				}
				if (! (vertex->x >= left))
				{
					++outsideCount;
					++leftCount;
				}
				if (! (right >= vertex->x))
				{
					++outsideCount;
					++rightCount;
				}
			}
		}

		if (topCount == count || rightCount == count || leftCount == count || bottomCount == count)
			return;

		if (outsideCount == 0)
		{
			if (! (flags & 0x40) && g_unk9F6008 == 0 && ! UnkFunc28(vertices[0], vertices[1], vertices[2]))
			{
				if (count != 4 || ! UnkFunc28(vertices[1], vertices[2], vertices[3]))
					return;
			}

			if (g_unk9F6008 == 0)
			{
				UnkFunc30(vertices, count, texture, flags, layer);
				return;
			}
			if (vertices[0]->z > 0.85)
			{
				UnkFunc30(vertices, count, texture, flags, layer);
				return;
			}
			UnkFunc29(vertices, count, texture, flags);
			return;
		}

		BVertex bufferA[16];
		BVertex bufferB[16];
		BVertex* input = bufferB;
		BVertex* output = bufferA;
		for (index = 0; index < count; ++index)
			memcpy(&input[index], vertices[index], sizeof(BVertex));
		memcpy(&input[count], input, sizeof(BVertex));
		int32_t closedCount = count + 1;

		BVertex* following;
		BVertex* previous;
		BVertex* destination;
		BVertex* swapBuffer;
		int32_t readCount;
		int32_t writtenCount;
		double plane;
		volatile float denominator;
		volatile float distance;
		volatile float slope;

		// Each plane scans one outside run; scratch counts include the closing record.
		if (topCount != 0)
		{
			plane = g_clipTop;
			following = input;
			destination = output;
			if (following->y >= plane)
			{
				readCount = writtenCount = 0;
				BOOL nextInside;
				do
				{
					nextInside = ! (plane > following[1].y);
					memcpy(destination, following, sizeof(BVertex));
					++destination;
					++following;
					++writtenCount;
					++readCount;
				} while (nextInside);

				// 0x004BE336: top inside-first exit retains the copied outside depth.
				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->y - previous->y;
				distance = (float)(plane - previous->y);
				destination->x = ((destination->x - previous->x) / denominator) * distance + previous->x;
				destination->y = (float)plane;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				++destination;
				++writtenCount;

				plane = g_clipTop;
				while (plane > following->y)
				{
					++following;
					++readCount;
				}

				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->y - previous->y;
				distance = (float)(plane - previous->y);
				destination->y = (float)plane;
				destination->x = ((destination->x - previous->x) / denominator) * distance + previous->x;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				++destination;
				++writtenCount;

				if (readCount < closedCount)
				{
					memcpy(destination, following, (closedCount - readCount) * sizeof(BVertex));
					writtenCount += closedCount - readCount;
				}
			}
			else
			{
				do
				{
					++following;
				} while (plane > following->y);

				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->y - previous->y;
				distance = (float)(plane - previous->y);
				destination->y = (float)plane;
				destination->x = ((destination->x - previous->x) / denominator) * distance + previous->x;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				++destination;
				writtenCount = 1;

				plane = g_clipTop;
				while (! (plane > following->y))
				{
					memcpy(destination, following, sizeof(BVertex));
					++destination;
					++following;
					++writtenCount;
				}

				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->y - previous->y;
				distance = (float)(plane - previous->y);
				destination->x = ((destination->x - previous->x) / denominator) * distance + previous->x;
				destination->y = (float)plane;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				memcpy(destination + 1, output, sizeof(BVertex));
				writtenCount += 2;
			}

			closedCount = writtenCount;
			swapBuffer = input;
			input = output;
			output = swapBuffer;
		}

		if (closedCount < 3)
			return;

		plane = g_clipBottom;
		bottomCount = 0;
		for (index = 0; index < closedCount; ++index)
		{
			if (! (plane >= input[index].y))
				++bottomCount;
		}
		if (bottomCount == closedCount)
			return;

		if (bottomCount != 0)
		{
			following = input;
			destination = output;
			if (plane >= following->y)
			{
				readCount = writtenCount = 0;
				do
				{
					memcpy(destination, following, sizeof(BVertex));
					++destination;
					++following;
					++writtenCount;
					++readCount;
				} while (plane >= following->y);

				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->y - previous->y;
				distance = (float)(plane - previous->y);
				destination->x = ((destination->x - previous->x) / denominator) * distance + previous->x;
				destination->y = (float)plane;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				++destination;
				++writtenCount;

				plane = g_clipBottom;
				while (! (plane >= following->y))
				{
					++following;
					++readCount;
				}

				// 0x004BEB89: bottom inside-first entry, quotient store at 0x004BEBC1.
				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->y - previous->y;
				distance = (float)(plane - previous->y);
				slope = (destination->x - previous->x) / denominator;
				destination->y = (float)plane;
				destination->x = distance * slope + previous->x;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				++destination;
				++writtenCount;

				while (readCount < closedCount)
				{
					memcpy(destination, following, sizeof(BVertex));
					++destination;
					++following;
					++writtenCount;
					++readCount;
				}
			}
			else
			{
				do
				{
					++following;
				} while (! (plane >= following->y));

				// 0x004BED41: bottom outside-first entry spills the quotient before distance.
				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->y - previous->y;
				slope = (destination->x - previous->x) / denominator;
				distance = (float)(plane - previous->y);
				destination->y = (float)plane;
				destination->x = distance * slope + previous->x;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				++destination;
				writtenCount = 1;

				plane = g_clipBottom;
				while (plane >= following->y)
				{
					memcpy(destination, following, sizeof(BVertex));
					++destination;
					++following;
					++writtenCount;
				}

				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->y - previous->y;
				distance = (float)(plane - previous->y);
				destination->x = ((destination->x - previous->x) / denominator) * distance + previous->x;
				destination->y = (float)plane;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				memcpy(destination + 1, output, sizeof(BVertex));
				writtenCount += 2;
			}

			closedCount = writtenCount;
			swapBuffer = input;
			input = output;
			output = swapBuffer;
		}

		if (closedCount < 3)
			return;

		plane = g_clipLeft;
		leftCount = 0;
		for (index = 0; index < closedCount; ++index)
		{
			if (plane > input[index].x)
				++leftCount;
		}
		if (leftCount == closedCount)
			return;

		if (leftCount != 0)
		{
			following = input;
			destination = output;
			if (! (plane > following->x))
			{
				readCount = writtenCount = 0;
				do
				{
					memcpy(destination, following, sizeof(BVertex));
					++destination;
					++following;
					++writtenCount;
					++readCount;
				} while (! (plane > following->x));

				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->x - previous->x;
				distance = (float)(plane - previous->x);
				destination->y = ((destination->y - previous->y) / denominator) * distance + previous->y;
				destination->x = (float)plane;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				++destination;
				++writtenCount;

				plane = g_clipLeft;
				while (plane > following->x)
				{
					++following;
					++readCount;
				}

				// 0x004BF27C: left inside-first entry, quotient store at 0x004BF2AF.
				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->x - previous->x;
				distance = (float)(plane - previous->x);
				slope = (destination->y - previous->y) / denominator;
				destination->x = (float)plane;
				destination->y = distance * slope + previous->y;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				++destination;
				++writtenCount;

				while (readCount < closedCount)
				{
					memcpy(destination, following, sizeof(BVertex));
					++destination;
					++following;
					++writtenCount;
					++readCount;
				}
			}
			else
			{
				do
				{
					++following;
				} while (plane > following->x);

				// 0x004BF434: left outside-first entry, quotient store at 0x004BF46F.
				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->x - previous->x;
				distance = (float)(plane - previous->x);
				slope = (destination->y - previous->y) / denominator;
				destination->x = (float)plane;
				destination->y = distance * slope + previous->y;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				++destination;
				writtenCount = 1;

				plane = g_clipLeft;
				while (! (plane > following->x))
				{
					memcpy(destination, following, sizeof(BVertex));
					++destination;
					++following;
					++writtenCount;
				}

				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->x - previous->x;
				distance = (float)(plane - previous->x);
				destination->y = ((destination->y - previous->y) / denominator) * distance + previous->y;
				destination->x = (float)plane;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				memcpy(destination + 1, output, sizeof(BVertex));
				writtenCount += 2;
			}

			closedCount = writtenCount;
			swapBuffer = input;
			input = output;
			output = swapBuffer;
		}

		if (closedCount < 3)
			return;

		plane = g_clipRight;
		rightCount = 0;
		for (index = 0; index < closedCount; ++index)
		{
			if (! (plane >= input[index].x))
				++rightCount;
		}
		if (rightCount == closedCount)
			return;

		if (rightCount != 0)
		{
			following = input;
			destination = output;
			if (plane >= following->x)
			{
				readCount = writtenCount = 0;
				do
				{
					memcpy(destination, following, sizeof(BVertex));
					++destination;
					++following;
					++writtenCount;
					++readCount;
				} while (plane >= following->x);

				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->x - previous->x;
				distance = (float)(plane - previous->x);
				destination->y = ((destination->y - previous->y) / denominator) * distance + previous->y;
				destination->x = (float)plane;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				++destination;
				++writtenCount;

				plane = g_clipRight;
				while (! (plane >= following->x))
				{
					++following;
					++readCount;
				}

				// 0x004BF96F: right inside-first entry, quotient store at 0x004BF9A2.
				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->x - previous->x;
				distance = (float)(plane - previous->x);
				slope = (destination->y - previous->y) / denominator;
				destination->x = (float)plane;
				destination->y = distance * slope + previous->y;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				++destination;
				++writtenCount;

				while (readCount < closedCount)
				{
					memcpy(destination, following, sizeof(BVertex));
					++destination;
					++following;
					++writtenCount;
					++readCount;
				}
			}
			else
			{
				do
				{
					++following;
				} while (! (plane >= following->x));

				// 0x004BFB27: right outside-first entry spills the quotient before distance.
				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->x - previous->x;
				slope = (destination->y - previous->y) / denominator;
				distance = (float)(plane - previous->x);
				destination->x = (float)plane;
				destination->y = distance * slope + previous->y;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				++destination;
				writtenCount = 1;

				plane = g_clipRight;
				while (plane >= following->x)
				{
					memcpy(destination, following, sizeof(BVertex));
					++destination;
					++following;
					++writtenCount;
				}

				previous = following - 1;
				memcpy(destination, following, sizeof(BVertex));
				denominator = destination->x - previous->x;
				distance = (float)(plane - previous->x);
				destination->y = ((destination->y - previous->y) / denominator) * distance + previous->y;
				destination->x = (float)plane;
				destination->z = ((destination->z - previous->z) / denominator) * distance + previous->z;
				destination->u = ((destination->u - previous->u) / denominator) * distance + previous->u;
				destination->v = ((destination->v - previous->v) / denominator) * distance + previous->v;
				destination->diffuse = Clip25InterpolateColour(previous->diffuse, destination->diffuse, denominator, distance);
				memcpy(destination + 1, output, sizeof(BVertex));
				writtenCount += 2;
			}

			closedCount = writtenCount;
			input = output;
		}

		if (closedCount < 4)
			return;

		BVertex* emitted[4];
		emitted[0] = input;
		int32_t quadCount = (closedCount - 3) >> 1;
		following = input + 1;
		if (! (flags & 0x40) && ! UnkFunc28(input, following, following + 1) && ! UnkFunc28(following, following + 1, following + 2))
			return;

		// Clipped HUD polygons use the second vertex for queue selection and still test winding.
		if (g_unk9F6008 != 0 && ! (following->z > 0.85))
		{
			while (quadCount > 0)
			{
				emitted[1] = following;
				emitted[2] = ++following;
				emitted[3] = ++following;
				UnkFunc29(emitted, 4, texture, flags);
				--quadCount;
			}
			if (((closedCount - 1) & 1) == 1)
			{
				emitted[1] = following;
				emitted[2] = following + 1;
				UnkFunc29(emitted, 3, texture, flags);
			}
			return;
		}

		while (quadCount > 0)
		{
			emitted[1] = following;
			emitted[2] = ++following;
			emitted[3] = ++following;
			UnkFunc30(emitted, 4, texture, flags, layer);
			--quadCount;
		}
		if (((closedCount - 1) & 1) == 1)
		{
			emitted[1] = following;
			emitted[2] = following + 1;
			UnkFunc30(emitted, 3, texture, flags, layer);
		}
	}

	// FUNCTION: TOY2 0x004C0100
	int32_t UnkFunc23(BVertex* const* vertices, int32_t count)
	{
		// All four pointer slots are read before count selects any coordinate reads.
		BVertex* const volatile* slots = vertices;
		const BVertex* a = slots[0];
		const BVertex* b = slots[1];
		const BVertex* c = slots[2];
		const BVertex* d = slots[3];
		volatile float bound;

		if (count == 4)
		{
			bound = (float)g_clipLeft;
			if ((CompareGeometryFloat(a->x, &bound) & 0x0100) && (CompareGeometryFloat(b->x, &bound) & 0x0100) && (CompareGeometryFloat(c->x, &bound) & 0x0100)
				&& (CompareGeometryFloat(d->x, &bound) & 0x0100))
				return 1;
			bound = (float)g_clipRight;
			if (! (CompareGeometryFloat(a->x, &bound) & 0x4100) && ! (CompareGeometryFloat(b->x, &bound) & 0x4100)
				&& ! (CompareGeometryFloat(c->x, &bound) & 0x4100) && ! (CompareGeometryFloat(d->x, &bound) & 0x4100))
				return 1;
			bound = (float)g_clipTop;
			if ((CompareGeometryFloat(a->y, &bound) & 0x0100) && (CompareGeometryFloat(b->y, &bound) & 0x0100) && (CompareGeometryFloat(c->y, &bound) & 0x0100)
				&& (CompareGeometryFloat(d->y, &bound) & 0x0100))
				return 1;
			bound = (float)g_clipBottom;
			if (! (CompareGeometryFloat(a->y, &bound) & 0x4100) && ! (CompareGeometryFloat(b->y, &bound) & 0x4100)
				&& ! (CompareGeometryFloat(c->y, &bound) & 0x4100) && ! (CompareGeometryFloat(d->y, &bound) & 0x4100))
				return 1;
			return 0;
		}
		if (count == 3)
		{
			bound = (float)g_clipLeft;
			if ((CompareGeometryFloat(a->x, &bound) & 0x0100) && (CompareGeometryFloat(b->x, &bound) & 0x0100) && (CompareGeometryFloat(c->x, &bound) & 0x0100))
				return 1;
			bound = (float)g_clipRight;
			if (! (CompareGeometryFloat(a->x, &bound) & 0x4100) && ! (CompareGeometryFloat(b->x, &bound) & 0x4100)
				&& ! (CompareGeometryFloat(c->x, &bound) & 0x4100))
				return 1;
			bound = (float)g_clipTop;
			if ((CompareGeometryFloat(a->y, &bound) & 0x0100) && (CompareGeometryFloat(b->y, &bound) & 0x0100) && (CompareGeometryFloat(c->y, &bound) & 0x0100))
				return 1;
			bound = (float)g_clipBottom;
			if (! (CompareGeometryFloat(a->y, &bound) & 0x4100) && ! (CompareGeometryFloat(b->y, &bound) & 0x4100)
				&& ! (CompareGeometryFloat(c->y, &bound) & 0x4100))
				return 1;
			return 0;
		}
		return 1;
	}

	// FUNCTION: TOY2 0x004C0320
	void UnkFunc22(BVertex* const* vertices, int32_t count, uint32_t* texture, uint32_t flags, int32_t layer, int32_t recursion)
	{
		if (g_unk9F6008 == 0 && UnkFunc23(vertices, count) == 1)
			return;

		if (! SubdivisionUsesNearPass())
		{
			UnkFunc24(vertices, count, texture, flags, layer, recursion);
			return;
		}

		BVertex* vertex0 = vertices[0];
		BVertex* vertex1 = vertices[1];
		BVertex* vertex3 = vertices[3];
		BVertex* vertex2 = vertices[2];
		float maximumZ = vertex0->z;
		float minimumZ = vertex0->z;
		if (vertex1->z > maximumZ)
			maximumZ = vertex1->z;
		if (vertex2->z > maximumZ)
			maximumZ = vertex2->z;
		if (minimumZ > vertex1->z)
			minimumZ = vertex1->z;
		if (minimumZ > vertex2->z)
			minimumZ = vertex2->z;
		if (count != 3)
		{
			if (vertex3->z > maximumZ)
				maximumZ = vertex3->z;
			if (minimumZ > vertex3->z)
				minimumZ = vertex3->z;
		}
		if (! (minimumZ > 0.0))
		{
			UnkFunc24(vertices, count, texture, flags, layer, recursion);
			return;
		}

		volatile float depthSpan = maximumZ - minimumZ;
		if (count == 3)
		{
			if (minimumZ > 1200.0f || ! (depthSpan >= 400.0) || texture == 0)
			{
				UnkFunc24(vertices, 3, texture, flags, layer, recursion);
				return;
			}

			RGBColor colour0, colour1, colour2;
			uint32_t alphaMask;
			UnkFunc27(vertex0->diffuse, &colour0.r, &colour0.g, &colour0.b, &alphaMask);
			UnkFunc27(vertex1->diffuse, &colour1.r, &colour1.g, &colour1.b, &alphaMask);
			UnkFunc27(vertex2->diffuse, &colour2.r, &colour2.g, &colour2.b, &alphaMask);

			BVertex center, midpoint01, midpoint12, midpoint20;
			int32_t cameraXSum = (int32_t)(((uint32_t)vertex1->cameraX + (uint32_t)vertex2->cameraX) + (uint32_t)vertex0->cameraX);
			center.x = (float)((double)cameraXSum * 0.3333333333333333);
			center.y = (float)((((double)vertex1->cameraY + vertex2->cameraY) + vertex0->cameraY) * 0.3333333333333333);
			center.z = (float)((((double)vertex0->z + vertex2->z) + vertex1->z) * 0.3333333333333333);
			center.u = (float)((((double)vertex1->u + vertex2->u) + vertex0->u) * 0.3333333333333333);
			center.v = (float)((((double)vertex2->v + vertex0->v) + vertex1->v) * 0.3333333333333333);
			center.diffuse = SubdivisionCentroidColour(colour2, colour1, colour0, alphaMask);
			UnkFunc26(&center);

			cameraXSum = (int32_t)((uint32_t)vertex1->cameraX + (uint32_t)vertex0->cameraX);
			midpoint01.x = (float)((double)cameraXSum * 0.5);
			midpoint01.y = (float)(((double)vertex1->cameraY + vertex0->cameraY) * 0.5);
			midpoint01.z = (float)(((double)vertex0->z + vertex1->z) * 0.5);
			midpoint01.u = (float)(((double)vertex1->u + vertex0->u) * 0.5);
			midpoint01.v = (float)(((double)vertex0->v + vertex1->v) * 0.5);
			midpoint01.diffuse = SubdivisionMidpointColour(colour1, colour0, alphaMask);
			UnkFunc26(&midpoint01);

			cameraXSum = (int32_t)((uint32_t)vertex1->cameraX + (uint32_t)vertex2->cameraX);
			midpoint12.x = (float)((double)cameraXSum * 0.5);
			midpoint12.y = (float)(((double)vertex1->cameraY + vertex2->cameraY) * 0.5);
			midpoint12.z = (float)(((double)vertex2->z + vertex1->z) * 0.5);
			midpoint12.u = (float)(((double)vertex1->u + vertex2->u) * 0.5);
			midpoint12.v = (float)(((double)vertex2->v + vertex1->v) * 0.5);
			midpoint12.diffuse = SubdivisionMidpointColour(colour2, colour1, alphaMask);
			UnkFunc26(&midpoint12);

			cameraXSum = (int32_t)((uint32_t)vertex2->cameraX + (uint32_t)vertex0->cameraX);
			midpoint20.x = (float)((double)cameraXSum * 0.5);
			midpoint20.y = (float)(((double)vertex2->cameraY + vertex0->cameraY) * 0.5);
			midpoint20.z = (float)(((double)vertex0->z + vertex2->z) * 0.5);
			midpoint20.u = (float)(((double)vertex2->u + vertex0->u) * 0.5);
			midpoint20.v = (float)(((double)vertex2->v + vertex0->v) * 0.5);
			midpoint20.diffuse = SubdivisionMidpointColour(colour2, colour0, alphaMask);
			UnkFunc26(&midpoint20);

			BVertex* child[4];
			child[0] = &center;
			child[1] = &midpoint01;
			child[2] = vertex1;
			child[3] = &midpoint12;
			UnkFunc24(child, 4, texture, flags, layer, 0);
			child[0] = &center;
			child[1] = &midpoint12;
			child[2] = vertex2;
			child[3] = &midpoint20;
			UnkFunc24(child, 4, texture, flags, layer, 0);
			child[0] = &center;
			child[1] = &midpoint20;
			child[2] = vertex0;
			child[3] = &midpoint01;
			UnkFunc24(child, 4, texture, flags, layer, 0);
			return;
		}

		if (minimumZ > 1200.0f || ! (depthSpan >= 600.0))
		{
			UnkFunc24(vertices, count, texture, flags, layer, recursion);
			return;
		}

		RGBColor colour0, colour1, colour2, colour3;
		uint32_t alphaMask;
		UnkFunc27(vertex0->diffuse, &colour0.r, &colour0.g, &colour0.b, &alphaMask);
		UnkFunc27(vertex1->diffuse, &colour1.r, &colour1.g, &colour1.b, &alphaMask);
		UnkFunc27(vertex2->diffuse, &colour2.r, &colour2.g, &colour2.b, &alphaMask);
		UnkFunc27(vertex3->diffuse, &colour3.r, &colour3.g, &colour3.b, &alphaMask);

		BVertex center, midpoint01, midpoint12, midpoint23, midpoint30;
		if (texture)
		{
			center.x = SubdivisionScreenMidpointX(vertex2->cameraX, vertex0->cameraX);
			center.y = (float)(((double)vertex2->cameraY + vertex0->cameraY) * 0.5);
			center.z = (float)(((double)vertex0->z + vertex2->z) * 0.5);
			center.u = (float)(((double)vertex2->u + vertex0->u) * 0.5);
			center.v = (float)(((double)vertex2->v + vertex0->v) * 0.5);
			center.diffuse = SubdivisionMidpointColour(colour2, colour0, alphaMask);
			UnkFunc26(&center);

			midpoint01.x = SubdivisionScreenMidpointX(vertex1->cameraX, vertex0->cameraX);
			midpoint01.y = (float)(((double)vertex1->cameraY + vertex0->cameraY) * 0.5);
			midpoint01.z = (float)(((double)vertex0->z + vertex1->z) * 0.5);
			midpoint01.u = (float)(((double)vertex1->u + vertex0->u) * 0.5);
			midpoint01.v = (float)(((double)vertex0->v + vertex1->v) * 0.5);
			midpoint01.diffuse = SubdivisionMidpointColour(colour1, colour0, alphaMask);
			UnkFunc26(&midpoint01);

			midpoint12.x = SubdivisionScreenMidpointX(vertex1->cameraX, vertex2->cameraX);
			midpoint12.y = (float)(((double)vertex1->cameraY + vertex2->cameraY) * 0.5);
			midpoint12.z = (float)(((double)vertex2->z + vertex1->z) * 0.5);
			midpoint12.u = (float)(((double)vertex1->u + vertex2->u) * 0.5);
			midpoint12.v = (float)(((double)vertex2->v + vertex1->v) * 0.5);
			midpoint12.diffuse = SubdivisionMidpointColour(colour2, colour1, alphaMask);
			UnkFunc26(&midpoint12);

			midpoint23.x = SubdivisionScreenMidpointX(vertex2->cameraX, vertex3->cameraX);
			midpoint23.y = (float)(((double)vertex3->cameraY + vertex2->cameraY) * 0.5);
			midpoint23.z = (float)(((double)vertex3->z + vertex2->z) * 0.5);
			midpoint23.u = (float)(((double)vertex3->u + vertex2->u) * 0.5);
			midpoint23.v = (float)(((double)vertex3->v + vertex2->v) * 0.5);
			midpoint23.diffuse = SubdivisionMidpointColour(colour3, colour2, alphaMask);
			UnkFunc26(&midpoint23);

			midpoint30.x = SubdivisionScreenMidpointX(vertex3->cameraX, vertex0->cameraX);
			midpoint30.y = (float)(((double)vertex3->cameraY + vertex0->cameraY) * 0.5);
			midpoint30.z = (float)(((double)vertex3->z + vertex0->z) * 0.5);
			midpoint30.u = (float)(((double)vertex3->u + vertex0->u) * 0.5);
			midpoint30.v = (float)(((double)vertex3->v + vertex0->v) * 0.5);
			midpoint30.diffuse = SubdivisionMidpointColour(colour3, colour0, alphaMask);
			UnkFunc26(&midpoint30);
		}
		else
		{
			// Untextured screen records write only screen XYZ and diffuse.
			center.x = (float)(((double)vertex0->x + vertex2->x) * 0.5);
			center.y = (float)(((double)vertex0->y + vertex2->y) * 0.5);
			center.z = (float)(((double)vertex0->z + vertex2->z) * 0.5);
			center.diffuse = SubdivisionMidpointColour(colour2, colour0, alphaMask);

			midpoint01.x = (float)(((double)vertex0->x + vertex1->x) * 0.5);
			midpoint01.y = (float)(((double)vertex1->y + vertex0->y) * 0.5);
			midpoint01.z = (float)(((double)vertex0->z + vertex1->z) * 0.5);
			midpoint01.diffuse = SubdivisionMidpointColour(colour1, colour0, alphaMask);

			midpoint12.x = (float)(((double)vertex2->x + vertex1->x) * 0.5);
			midpoint12.y = (float)(((double)vertex1->y + vertex2->y) * 0.5);
			midpoint12.z = (float)(((double)vertex2->z + vertex1->z) * 0.5);
			midpoint12.diffuse = SubdivisionMidpointColour(colour2, colour1, alphaMask);

			midpoint23.x = (float)(((double)vertex3->x + vertex2->x) * 0.5);
			midpoint23.y = (float)(((double)vertex3->y + vertex2->y) * 0.5);
			midpoint23.z = (float)(((double)vertex3->z + vertex2->z) * 0.5);
			midpoint23.diffuse = SubdivisionMidpointColour(colour3, colour2, alphaMask);

			midpoint30.x = (float)(((double)vertex0->x + vertex3->x) * 0.5);
			midpoint30.y = (float)(((double)vertex3->y + vertex0->y) * 0.5);
			midpoint30.z = (float)(((double)vertex3->z + vertex0->z) * 0.5);
			midpoint30.diffuse = SubdivisionMidpointColour(colour3, colour0, alphaMask);
		}

		BVertex centerCopy1, centerCopy2, centerCopy3;
		memcpy(&centerCopy1, &center, sizeof(BVertex));
		memcpy(&centerCopy2, &center, sizeof(BVertex));
		memcpy(&centerCopy3, &center, sizeof(BVertex));
		BVertex* child[4];
		child[0] = &center;
		child[1] = &midpoint01;
		child[2] = vertex1;
		child[3] = &midpoint12;
		UnkFunc24(child, 4, texture, flags, layer, 0);
		child[0] = &centerCopy1;
		child[1] = &midpoint30;
		child[2] = vertex0;
		child[3] = &midpoint01;
		UnkFunc24(child, 4, texture, flags, layer, 0);
		child[0] = &centerCopy2;
		child[1] = &midpoint12;
		child[2] = vertex2;
		child[3] = &midpoint23;
		UnkFunc24(child, 4, texture, flags, layer, 0);
		child[0] = &centerCopy3;
		child[1] = &midpoint23;
		child[2] = vertex3;
		child[3] = &midpoint30;
		UnkFunc24(child, 4, texture, flags, layer, 0);
	}

	// FUNCTION: TOY2 0x004C14A0
	void UnkFunc19(BVertex* vertices, uint16_t* indices, uint32_t indexCount, uint32_t flags)
	{
		TextureSnapshot snapshot;
		uint32_t* texture = GetCurrentTexture(&snapshot) ? 0 : snapshot.texData;
		uint32_t triangles = indexCount / 3;
		BVertex* polygon[4];
		polygon[3] = 0;
		while (triangles)
		{
			polygon[0] = &vertices[indices[2]];
			polygon[1] = &vertices[indices[1]];
			polygon[2] = &vertices[indices[0]];
			UnkFunc22(polygon, 3, texture, Renderer::g_renderStateCache[0], g_unkE4D950, 0);
			indices += 3;
			--triangles;
		}
	}

	// FUNCTION: TOY2 0x004C1540
	void UnkFunc21(BVertex* vertices, uint16_t* indices, uint32_t indexCount, uint32_t flags)
	{
		TextureSnapshot snapshot;
		uint32_t* texture = GetCurrentTexture(&snapshot) ? 0 : snapshot.texData;
		BVertex* polygon[4];
		if (((indexCount - 2) & ~1u) == 0)
		{
			polygon[0] = &vertices[indices[0]];
			polygon[1] = &vertices[indices[1]];
			polygon[2] = &vertices[indices[2]];
			polygon[3] = 0;
			UnkFunc22(polygon, 3, texture, Renderer::g_renderStateCache[0], g_unkE4D950, 0);
			return;
		}

		uint32_t previous0 = indices[2];
		uint32_t previous1 = indices[3];
		polygon[0] = &vertices[previous0];
		polygon[1] = &vertices[previous1];
		polygon[2] = &vertices[indices[1]];
		polygon[3] = &vertices[indices[0]];
		indices += 4;
		UnkFunc22(polygon, 4, texture, Renderer::g_renderStateCache[0], g_unkE4D950, 0);
		uint32_t pairs = (indexCount - 4) >> 1;
		while (pairs)
		{
			uint32_t next0 = indices[0];
			uint32_t next1 = indices[1];
			polygon[0] = &vertices[next0];
			polygon[1] = &vertices[next1];
			polygon[2] = &vertices[previous1];
			polygon[3] = &vertices[previous0];
			indices += 2;
			UnkFunc22(polygon, 4, texture, Renderer::g_renderStateCache[0], g_unkE4D950, 0);
			previous0 = next0;
			previous1 = next1;
			--pairs;
		}
		if (indexCount & 1)
		{
			polygon[0] = &vertices[indices[0]];
			polygon[1] = &vertices[previous1];
			polygon[2] = &vertices[previous0];
			UnkFunc22(polygon, 3, texture, Renderer::g_renderStateCache[0], g_unkE4D950, 0);
		}
	}

	// FUNCTION: TOY2 0x004C1720
	void UnkFunc16(int32_t primitiveType, BVertex* vertices, uint16_t* indices, uint32_t indexCount, uint32_t flags)
	{
		GetRenderDistance(&g_nearDistance, &g_farDistance);
		if (primitiveType == D3DPT_TRIANGLELIST)
			UnkFunc19(vertices, indices, indexCount, flags);
		else if (primitiveType == D3DPT_TRIANGLESTRIP)
			UnkFunc21(vertices, indices, indexCount, flags);
		g_unk9F6008 = 0;
	}

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
	void UnkFunc34(BPolygon* polygon, int32_t count, uint32_t flags, uint32_t* texture, int32_t band) {}

	// STUB: TOY2 0x004C9D00
	void UnkFunc35(BPolygon* polygon, int32_t count, uint32_t flags, uint32_t* texture, int32_t band) {}
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

	// FUNCTION: TOY2 0x004B9950
	HRESULT DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE primitiveType, LPDIRECT3DVERTEXBUFFER vertexBuffer, WORD* indices, DWORD indexCount, DWORD flags)
	{
		LPVOID vertices;
		if (DrawingAPI::LockVertexBuffer(vertexBuffer, 0x801, &vertices, 0) == 0)
		{
			SoftwareRenderer::TextureSnapshot snapshot;
			SoftwareRenderer::GetCurrentTexture(&snapshot);
			SoftwareRenderer::UpdateSoftwareClipRect();
			SoftwareRenderer::UnkFunc16(primitiveType, (SoftwareRenderer::BVertex*)vertices, indices, indexCount, flags);
			DrawingAPI::UnlockVertexBuffer(vertexBuffer);
		}
		return 0;
	}

	// FUNCTION: TOY2 0x004B99F0
	HRESULT DrawIndexedPrimitive(
		D3DPRIMITIVETYPE primitiveType,
		DWORD vertexType,
		LPVOID vertices,
		DWORD vertexCount,
		LPWORD indices,
		DWORD indexCount,
		DWORD flags)
	{
		SoftwareRenderer::TextureSnapshot snapshot;
		SoftwareRenderer::GetCurrentTexture(&snapshot);
		SoftwareRenderer::UpdateSoftwareClipRect();
		SoftwareRenderer::UnkFunc16(primitiveType, (SoftwareRenderer::BVertex*)vertices, indices, indexCount, flags);
		return 0;
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

	// FUNCTION: TOY2 0x004C19E0
	HRESULT ProcessVerticesOnBuffer(
		LPDIRECT3DVERTEXBUFFER destBuffer,
		DWORD vertexOp,
		DWORD destIndex,
		DWORD count,
		LPDIRECT3DVERTEXBUFFER srcBuffer,
		DWORD srcIndex,
		DWORD flags)
	{
		float halfWidth = (float)((double)SoftwareRenderer::g_surfaceWidth * 0.5);
		float halfHeight = (float)((double)SoftwareRenderer::g_surfaceHeight * 0.5);
		SoftwareRenderer::GetRenderDistance(&SoftwareRenderer::g_nearDistance, &SoftwareRenderer::g_farDistance);
		LPVOID sourceData;
		DWORD sourceSize;
		DrawingAPI::LockVertexBuffer(srcBuffer, 0x11, &sourceData, &sourceSize);
		Nu3D::Vertex* source = (Nu3D::Vertex*)sourceData + srcIndex;
		D3DMATRIX transform;
		Nu3D::Math::MultiplyMatrix3x4(&transform, DrawingDevice::g_curWorldTransform, SoftwareRenderer::g_viewTransform);
		SoftwareRenderer::BVertex* output = SoftwareRenderer::g_transformedVertices;
		for (DWORD index = 0; index < count; ++index)
		{
			Nu3D::Math::TransformPointByMatrix((Vector3F*)output, &source->position, &transform);
			float cameraX = output->x;
			float cameraY = output->y;
			if (output->z > 0.0)
			{
				output->x =
					(float)((((double)cameraX * halfWidth) / ((double)output->z + 1.0)) * SoftwareRenderer::g_zoomScaleX * 1.7
							+ SoftwareRenderer::g_surfaceWidth / 2);
				output->y =
					(float)(((-((double)cameraY * halfHeight)) / ((double)output->z + 1.0)) * SoftwareRenderer::g_zoomScaleY * 1.9
							+ SoftwareRenderer::g_surfaceHeight / 2);
			}
			output->cameraX = (int32_t)(int64_t)cameraX;
			output->cameraY = -cameraY;
			output->diffuse = source->diffuse.value;
			output->u = source->coords.x;
			output->v = source->coords.y;
			++source;
			++output;
		}
		DrawingAPI::UnlockVertexBuffer(srcBuffer);
		LPVOID destData;
		DWORD destSize;
		DrawingAPI::LockVertexBuffer(destBuffer, 0x21, &destData, &destSize);
		memcpy((SoftwareRenderer::BVertex*)destData + destIndex, SoftwareRenderer::g_transformedVertices, count << 5);
		DrawingAPI::UnlockVertexBuffer(destBuffer);
		return 0;
	}
}

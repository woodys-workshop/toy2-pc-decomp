#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Nu3D/Viewport.h"
#include "Nu3D/Nu3D.h"
#include "Renderer/Renderer.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include <windows.h>

namespace Renderer
{
	struct RenderEntry;
}

namespace SoftwareRenderer
{
	struct BVertex
	{
		float x;
		float y;
		float z;
		float cameraY;
		uint32_t diffuse;
		int32_t cameraX;
		float u;
		float v;
	};

	struct BPolygon
	{
		BVertex vertices[4];
		uint32_t* texture;
		int32_t count;
		uint32_t flags;
		BPolygon* next;
		uint32_t unknown90;
		int32_t band;
		uint32_t unknown98;
	};

	STATIC_ASSERT(sizeof(BVertex) == 0x20);
	STATIC_ASSERT(sizeof(BPolygon) == 0x9C);

	// a triangle waiting to be drawn, they get sorted into 1024 buckets by depth
	struct SortedTriangle
	{
		SortedTriangle* next;
		int32_t unused1;
		int32_t renderFlags;
		int32_t textureIdx;
		Renderer::RenderEntry* renderEntry;
		Nu3D::VertexTL vertices[3];
		float sortKey;
	};

	struct UnkSoft2
	{
		uint32_t unkInt1;
		uint32_t unkInt2;
		uint32_t unkInt3;
		uint32_t unkInt4;
		uint32_t unkInt5;
		uint32_t unkInt6;
		uint32_t unkInt7;
		uint32_t unkInt8;
		uint32_t unkInt9;
		uint32_t unkInt10;
		uint32_t unkInt12;
		uint32_t unkInt13;
		uint32_t unkInt14;
		uint32_t unkInt15;
		uint32_t unkInt16;
		uint32_t unkInt17;
		uint32_t unkInt18;
		uint32_t unkInt19;
		uint32_t unkInt20;
	};

	struct BltCommand
	{
		int16_t destX;
		int16_t destY;
		RECT srcRect;
		IDirectDrawSurface* lpSurface;
		DDSURFACEDESC surfaceDesc;
	};

	STATIC_ASSERT(sizeof(BltCommand) == 0x84);
	STATIC_ASSERT(sizeof(SortedTriangle) == 0x78);
	STATIC_ASSERT(sizeof(UnkSoft2) == 0x4C);

	extern uint8_t g_unk87E50C[16384];
	extern BltCommand g_bltQueue[32];
	extern int16_t g_bltQueueCount;
	extern int32_t g_unk559C40;
	extern int32_t g_unk839280;
	extern void* g_unk504D34;
	extern int32_t g_unk839278;
	extern uint32_t g_unk731CBC;
	extern PointI g_unk4F7400;
	extern int32_t g_unk500A1C;
	extern int32_t g_unk830C60;
	extern int32_t g_unkE4D950;
	extern int32_t g_unk9F6008;
	extern Nu3D::Viewport::ViewportRect* g_viewportRect;
	extern int32_t g_softwareClearColor;
	extern int32_t g_topOffset;
	extern int32_t g_bottomOffset;
	extern int32_t g_leftOffset;
	extern int32_t g_rightOffset;
	extern int32_t g_bitsPerPixel;
	extern int32_t g_pixelMode;
	extern D3DMATRIX* g_viewTransform;
	extern D3DMATRIX* g_projectionTransform;
	extern SortedTriangle* g_depthBuckets[1024];
	extern int32_t g_instanceCounter;

	void SwapRenderBuffer();
	void SetLevelFileIndex(int32_t index);
	void InitialisePrimarySurface();
	void InitialisePrimarySurface_T();
	void Destroy();
	void ZoomOut();
	void ZoomIn();
	void PresentFrame();
	void FlushDepthQueue();
	void FlushFIFOQueue();

	void UnkFunc67(int32_t param1, int32_t param2);
	void UnkFunc2();
	int16_t UnkFunc3();
	void UnkFunc31();
	void UnkFunc32();
	void UnkFunc33();
	void UnkFunc7();
	void SetCameraNearFarZ(float cameraNearZ, float cameraFarZ);

	// The Submit* functions sort triangles on the CPU for hardware rendering.
	void SubmitSortedTriangle(
		int32_t renderFlags,
		Renderer::RenderEntry* renderEntry,
		int32_t textureIdx,
		Nu3D::VertexTL* vertex1,
		Nu3D::VertexTL* vertex2,
		Nu3D::VertexTL* vertex3);
	HRESULT SubmitTriangleList(int32_t renderFlags, LPDIRECT3DVERTEXBUFFER buffer, Renderer::RenderEntry* renderEntry, uint16_t* indices, int32_t vertexCount);
	HRESULT SubmitTriangleStrip(int32_t renderFlags, LPDIRECT3DVERTEXBUFFER buffer, Renderer::RenderEntry* renderEntry, uint16_t* indices, int32_t vertexCount);

	int32_t UnkRenderAPI5(int32_t* a1);
	int32_t UnkRenderAPI6(int32_t* a1);
	int32_t UnkRenderAPI7(int32_t* a1);
	int32_t UnkRenderAPI8(int32_t* a1);
	uint32_t* UnkRenderAPI10(int32_t* a1);
	uint32_t* UnkRenderAPI11(int32_t* a1);
	uint32_t* UnkRenderAPI12(int32_t* a1);
	int32_t UnkRenderAPI9(int32_t* a1);
	uint32_t* UnkRenderAPI4(int32_t* a1);
	uint32_t* UnkRenderAPI3(int32_t* a1);
	int32_t UnkRenderAPI1(int32_t* a1);
	int32_t UnkRenderAPI13(int32_t* a1);
	int32_t UnkRenderAPI14(int32_t* a1);
	int32_t UnkRenderAPI21(int32_t* a1);
	uint32_t* UnkRenderAPI22(int32_t* a1);
	uint32_t* UnkRenderAPI23(int32_t* a1);
	uint32_t* UnkRenderAPI24(int32_t* a1);
	int32_t UnkRenderAPI17(int32_t* a1);
	uint32_t* UnkRenderAPI16(int32_t* a1);
	uint32_t* UnkRenderAPI15(int32_t* a1);
	int32_t UnkRenderAPI18(int32_t* a1);
	int32_t UnkRenderAPI19(int32_t* a1);
	int32_t UnkRenderAPI20(int32_t* a1);
	int32_t UnkRenderAPI2(int32_t* a1, int32_t* a2, int32_t* a3, int32_t* a4, int32_t a5);
	int32_t UnkRenderAPI25(int32_t* a1);
	void SetPaletteOnAPI();
	void SetNewPalette(int32_t a1, int32_t a2);
	int32_t UnkRenderAPI26(int32_t* a1);
	int32_t UnkRenderAPI30(int32_t* a1);
	int32_t UnkRenderAPI31(int32_t* a1);
	int32_t UnkRenderAPI32(int32_t* a1);
	int32_t UnkRenderAPI33(int32_t* a1);
	int32_t UnkRenderAPI29(int32_t* a1);
	int32_t UnkRenderAPI28(int32_t* a1);
	int32_t UnkRenderAPI27(int32_t* a1, int32_t* a2, int32_t* a3, int32_t* a4, int32_t a5);
	int32_t UnkRenderAPI34(int32_t* a1);
	int32_t UnkFunc1();
	void UnkFunc8();
	void SubmitTriangleStripRaw(int32_t renderFlags, LPVOID vertexData, Renderer::RenderEntry* renderEntry, uint16_t* indices, int32_t vertexCount);
	HRESULT SubmitQuad(int32_t renderFlags, int32_t texIndex, LPDIRECT3DVERTEXBUFFER buffer, uint16_t* indices);
}

namespace SoftwareDevice
{
	// Drawing Methods
	HRESULT DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE primitiveType, LPDIRECT3DVERTEXBUFFER vertexBuffer, WORD* indices, DWORD indexCount, DWORD flags);
	HRESULT DrawIndexedPrimitive(
		D3DPRIMITIVETYPE d3dptPrimitiveType,
		DWORD dwVertexTypeDesc,
		LPVOID lpvVertices,
		DWORD dwVertexCount,
		LPWORD lpwIndices,
		DWORD dwIndexCount,
		DWORD dwFlags);

	// Vertex Methods
	HRESULT ReleaseVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer);
	HRESULT CreateVertexBuffer(D3DVERTEXBUFFERDESC* desc, LPDIRECT3DVERTEXBUFFER* outBuffer, DWORD flags);
	HRESULT LockVertexBuffer(LPDIRECT3DVERTEXBUFFER vertexBuffer, DWORD flags, LPVOID* outData, DWORD* outSize);
	HRESULT UnlockVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer);
	HRESULT OptimizeVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer, LPDIRECT3DDEVICE3 device, DWORD flags);
	HRESULT ProcessVerticesOnBuffer(
		LPDIRECT3DVERTEXBUFFER destBuffer,
		DWORD dwVertexOp,
		DWORD dwDestIndex,
		DWORD dwCount,
		LPDIRECT3DVERTEXBUFFER srcBuffer,
		DWORD dwSrcIndex,
		DWORD dwFlags);
}

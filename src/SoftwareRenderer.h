#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Nu3D/Viewport.h"
#include "Nu3D/Nu3D.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include <windows.h>

namespace Renderer
{
	struct RenderEntry;
}

namespace SoftwareRenderer
{
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

	STATIC_ASSERT(sizeof(SortedTriangle) == 0x78);

	extern PointI g_unk4F7400;
	extern int32_t g_unk500A1C;
	extern int32_t g_unk830C60;
	extern int32_t g_unkE4D950;
	extern int32_t g_unk9F6008;
	extern Nu3D::Viewport::ViewportRect* g_viewportRect;
	extern int32_t g_softwareClearColor;
	extern int32_t g_leftOffset;
	extern int32_t g_rightOffset;
	extern int32_t g_topOffset;
	extern int32_t g_bottomOffset;
	extern int32_t g_bitsPerPixel;
	extern D3DMATRIX* g_viewTransform;
	extern D3DMATRIX* g_projectionTransform;
	extern SortedTriangle g_sortedTriPool[3000];
	extern SortedTriangle* g_depthBuckets[1024];
	extern int32_t g_sortedTrianglesDisabled;
	extern int32_t g_instanceCounter;

	void SwapRenderBuffer();
	void SetLevelFileIndex(int32_t index);
	void InitialisePrimarySurface();
	void InitialisePrimarySurface_T();
	void Destroy();
	void ZoomOut();
	void ZoomIn();
	void PresentFrame();

	void UnkFunc67(int32_t param1, int32_t param2);
	void UnkFunc2();
	int16_t UnkFunc3();
	void UnkFunc31();
	void UnkFunc32();
	void UnkFunc33();
	void UnkFunc7();
	void SetCameraNearFarZ(float cameraNearZ, float cameraFarZ);

	// the Submit* functions queue triangles for the software rasterizer, which is not decompiled
	void SubmitSortedTriangle(int32_t renderFlags,
		Renderer::RenderEntry* renderEntry,
		int32_t textureIdx,
		Nu3D::VertexTL* vertex1,
		Nu3D::VertexTL* vertex2,
		Nu3D::VertexTL* vertex3);
	HRESULT SubmitTriangleList(int32_t renderFlags, LPDIRECT3DVERTEXBUFFER buffer, Renderer::RenderEntry* renderEntry, uint16_t* indices, int32_t vertexCount);
	HRESULT SubmitTriangleStrip(int32_t renderFlags, LPDIRECT3DVERTEXBUFFER buffer, Renderer::RenderEntry* renderEntry, uint16_t* indices, int32_t vertexCount);

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

	STATIC_ASSERT(sizeof(UnkSoft2) == 0x4C);

	int32_t ClearRenderSurfaces();
	int32_t ShowBackBuffer();
	void UnkFunc11();
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
	void UnkFunc9(int32_t a1);
	int32_t UnkFunc10();
	char* UnkFunc15();
	void UnkFunc12(uint8_t* a1, int32_t a2);
	char* UnkFunc13();
	char* UnkFunc14();
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
	void UnkFunc4();
	void UnlockBackBuffer();
	int32_t UnkFunc1();
	void UnkFunc5();
	void UnkFunc8();
	int32_t UnkFunc6(uint32_t* a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5);
	void UnkFunc66(SoftwareRenderer::UnkSoft2* a1, int32_t a2);
	int32_t UnkFunc20(uint32_t** a1);
	int32_t GetStrideFromFVF(int32_t fvf);
	void SubmitTriangleStripRaw(int32_t renderFlags, LPDIRECT3DVERTEXBUFFER vertexBuffer, Renderer::RenderEntry* renderEntry, uint16_t* indices, int32_t vertexCount);
	HRESULT SubmitQuad(int32_t renderFlags, int32_t texIndex, LPDIRECT3DVERTEXBUFFER buffer, uint16_t* indices);
	float* UnkFunc18(float* a1, float* a2);
	uint32_t UnkFunc27(int32_t a1, float* a2, float* a3, float* a4, uint32_t* a5);
	void UnkFunc30(const void** a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5);
	void UnkFunc29(const void** a1, int32_t a2, int32_t a3, int32_t a4);
	void UnkFunc17(int32_t a1, int32_t a2, int32_t a3, int32_t a4);
	BOOL UnkFunc28(float* a1, float* a2, float* a3);
	void UnkFunc26(int32_t a1);
	void UnkFunc24(int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6);
	void UnkFunc25(float** a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5);
	int32_t UnkFunc23(float** a1, int32_t a2);
	void UnkFunc22(int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6);
	void UnkFunc19(int32_t a1, uint16_t* a2, uint32_t a3);
	void UnkFunc21(int32_t a1, uint16_t* a2, int32_t a3);
	void UnkFunc16(int32_t a1, int32_t a2, uint16_t* a3, uint32_t a4);
	void InitialiseColourScaleTables();
	void CommitZoom();
	uint32_t* UnkFunc59(int32_t a1, uint32_t* a2, int32_t* a3, uint32_t* a4);
	int16_t UnkFunc48(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	int16_t UnkFunc47(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	int32_t UnkFunc55(uint32_t a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, uint32_t a7, int32_t a8, int32_t a9, uint32_t a10);
	int32_t UnkFunc40(uint32_t a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, uint32_t a7, int32_t a8, int32_t a9, uint32_t a10);
	int32_t UnkFunc49(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	int32_t UnkFunc52(float* a1, float* a2, int32_t a3, int32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9, uint32_t a10);
	int32_t UnkFunc36(float* a1, float* a2, int32_t a3, int32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8, uint32_t a9, uint32_t a10);
	int32_t UnkFunc41(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	int32_t UnkFunc50(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	int32_t UnkFunc53(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	int32_t UnkFunc37(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	int32_t UnkFunc42(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	int32_t UnkFunc43(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	int32_t UnkFunc51(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	int32_t UnkFunc56(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	int32_t UnkFunc38(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	int32_t UnkFunc44(float* a1, float* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, int32_t a9, int32_t a10);
	uint32_t* UnkFunc57(float* a1, int32_t a2);
	uint32_t* UnkFunc46(float* a1, int32_t a2);
	uint32_t* UnkFunc45(float* a1, int32_t a2);
	uint32_t* UnkFunc54(float* a1);
	uint32_t* UnkFunc39(float* a1);
	uint32_t* UnkFunc58(float* a1, int32_t a2);
	uint32_t* UnkFunc34(float* a1);
	uint32_t* UnkFunc35(int32_t a1);
}

namespace SoftwareDevice
{
	// Drawing Methods
	HRESULT DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE primitiveType, LPDIRECT3DVERTEXBUFFER vertexBuffer, WORD* indices, DWORD indexCount, DWORD flags);
	HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE d3dptPrimitiveType,
		DWORD dwVertexTypeDesc,
		LPVOID lpvVertices,
		DWORD dwVertexCount,
		LPWORD lpwIndices,
		DWORD dwIndexCount,
		DWORD dwFlags);

	// Vertex Methods
	HRESULT ReleaseVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer);
	HRESULT CreateVertexBuffer(D3DVERTEXBUFFERDESC* desc, LPDIRECT3DVERTEXBUFFER* outBuffer, DWORD flags);
	HRESULT LockVertexBuffer(LPDIRECT3DVERTEXBUFFER vertexBuffer, DWORD dwFlags, LPVOID* lplpData, DWORD* lpStride);
	HRESULT UnlockVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer);
	HRESULT OptimizeVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer, LPDIRECT3DDEVICE3 device, DWORD flags);
	HRESULT ProcessVerticesOnBuffer(LPDIRECT3DVERTEXBUFFER destBuffer,
		DWORD dwVertexOp,
		DWORD dwDestIndex,
		DWORD dwCount,
		LPDIRECT3DVERTEXBUFFER srcBuffer,
		DWORD dwSrcIndex,
		DWORD dwFlags);
}
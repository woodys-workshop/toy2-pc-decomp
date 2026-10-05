#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Nu3D/Viewport.h"
#include "Nu3D/Nu3D.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>

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
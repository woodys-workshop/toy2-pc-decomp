#pragma once

#include "Common.h"
#include "Renderer/RenderType.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include <windows.h>

namespace Nu3D
{
	struct BmpDataNode;
	struct Patch;
}


#define D3DFVF_0x152 (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define D3DFVF_0x1C4 (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

namespace Nu3D
{
	struct Primitive;
	struct Material;
	struct InstanceData;
}

namespace DrawingAPI
{
	// Drawing Methods
	typedef HRESULT Device_DrawIndexedPrimitiveVB(
		D3DPRIMITIVETYPE primitiveType, LPDIRECT3DVERTEXBUFFER vertexBuffer, WORD* indices, DWORD indexCount, DWORD flags);
	typedef HRESULT Device_DrawIndexedPrimitive(D3DPRIMITIVETYPE d3dptPrimitiveType,
		DWORD dwVertexTypeDesc,
		LPVOID lpvVertices,
		DWORD dwVertexCount,
		LPWORD lpwIndices,
		DWORD dwIndexCount,
		DWORD dwFlags);

	// Vertex Methods
	typedef HRESULT Device_ReleaseVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer);
	typedef HRESULT Device_CreateVertexBuffer(D3DVERTEXBUFFERDESC* desc, LPDIRECT3DVERTEXBUFFER* outBuffer, DWORD flags);
	typedef HRESULT Device_LockVertexBuffer(LPDIRECT3DVERTEXBUFFER vertexBuffer, DWORD dwFlags, LPVOID* lplpData, DWORD* lpStride);
	typedef HRESULT Device_UnlockVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer);
	typedef HRESULT Device_OptimizeVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer, LPDIRECT3DDEVICE3 device, DWORD flags);
	typedef HRESULT Device_ProcessVerticesOnBuffer(LPDIRECT3DVERTEXBUFFER destBuffer,
		DWORD dwVertexOp,
		DWORD dwDestIndex,
		DWORD dwCount,
		LPDIRECT3DVERTEXBUFFER srcBuffer,
		DWORD dwSrcIndex,
		DWORD dwFlags);

	extern Device_DrawIndexedPrimitive* DrawIndexedPrimitive;
	extern Device_DrawIndexedPrimitiveVB* DrawIndexedPrimitiveVB;

	extern Device_ReleaseVertexBuffer* ReleaseVertexBuffer;
	extern Device_CreateVertexBuffer* CreateVertexBuffer;
	extern Device_LockVertexBuffer* LockVertexBuffer;
	extern Device_UnlockVertexBuffer* UnlockVertexBuffer;
	extern Device_OptimizeVertexBuffer* OptimizeVertexBuffer;
	extern Device_ProcessVerticesOnBuffer* ProcessVerticesOnBuffer;
}

namespace Renderer
{
	struct RenderEntry
	{
		RenderEntry* next;
		Renderer::RenderType type;
		float distanceSquared;
		Nu3D::Primitive* primitive;
		Nu3D::InstanceData* instanceData;
		Nu3D::Material* material;

		static RenderEntry* AllocObj(Nu3D::Material* material, Nu3D::Primitive* prim, Nu3D::InstanceData* instance);
		static void InsertIntoBucket(RenderEntry* renderEntry);
		static RenderEntry* AllocPatch(Nu3D::Material* material, Nu3D::Primitive* prim, Nu3D::InstanceData* spriteStruct);
	};

	extern float g_gammaCorrection;
	extern int32_t g_isSoftwareRendering;
	extern uint32_t g_frameDelta;
	extern float g_virtualScreenWidth;
	extern float g_virtualScreenHeight;
	extern float g_parallaxCurHorizScroll;
	extern int32_t g_enableCinematicBars;
	extern float g_nearRenderDistance;
	extern float g_farRenderDistance;
	extern int32_t g_deviceBlendShadeCapsCpy;
	extern int32_t g_boundTextureIndices[8];
	extern Nu3D::Material* g_boundMaterial;

	void Cleanup();
	void Init();
	void ConfigureFog(float fogStart, float fogEnd, RGBA color);
	void SetFogEnable(int32_t enableFog);
	void SetRenderDistance(float nearDistance, float farDistance);
	void SetViewportPreset();
	void SetViewportPresetByDetail(int32_t detail);
	void ShowBlackFrames();
	void UpdateBackgroundScroll(int16_t horizontalScroll, int16_t verticalScroll);
	void DrawCinematicBars();
	void SetIsSoftwareRendering(int32_t value);
	void SetVirtualRatioTo54();
	void DoFrameDelay(int32_t isGameplayFrame);
	RGBA ApplyGammaCorrection(RGBA color);
	int32_t GetIsSoftwareRendering();
	float BuildGammaCorrectionLUT(float gammaCorrection);
	void GetBlendShadeCaps(int32_t* capsOut);

	// Draw Methods
	void ClearScreen(RGBA clearColor, int32_t clearFlags);
	int32_t BeginScene();
	void EndScene(int32_t presentFrame);

	// Render Methods
	void DrawMainMenuText(int16_t yPos, char* text, int32_t fadeAlpha);
	void DrawTintOverlay();

	void SetRenderState(int32_t newStateFlags);
	void SetTextureStageState(int32_t newState, DWORD textureStage);
	void InitRenderState(int32_t newStage);
	void ResetParallax();
	void RenderParallaxBackground(int32_t forceRender);
	void FlushRenderQueues();
	RGBA ModulateColorByAlpha(RGBA color, int32_t flags);
	void BindTexture(int32_t texIndex);

	STATIC_ASSERT(sizeof(RenderEntry) == 0x18);

	void DrawBlackBorderBox(int16_t xPos, int16_t yPos, int32_t width, int32_t height, uint8_t red, uint8_t green, uint8_t blue);
	void DrawString(int16_t yPos, char* text, uint8_t red, uint8_t green, uint8_t blue, int32_t useDynamicWidth);
	void BlitTextureByIndexOffset(int32_t texIndex, int32_t destX, int32_t destY, int32_t width, int32_t height, int32_t horizontalWrapOffset, int32_t verticalWrapOffset, int32_t srcX, int32_t srcY);
	void DrawMenuText(int16_t yPos, char* text, int32_t shadow);
	void DrawMenuTextScaled(int16_t xPos, int16_t yPos, const char* text, int32_t shadow, int32_t alignment, int32_t scale);
	void DrawChar(int16_t xPos, int16_t yPos, char character, uint8_t red, uint8_t green, uint8_t blue, int32_t useDynamicWidth);
	void DrawFormattedText(int32_t screenX, int32_t screenY, char* format, ...);
	void BlitBitmapWithWrapping(Nu3D::BmpDataNode* bmpDataNode, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t horizontalWrapOffset, int32_t verticalWrapOffset, LONG destX, LONG destY);
	void RenderPatchList(Nu3D::Patch* patch, D3DMATRIX* nodeMatrices, int32_t** flags, int32_t renderStage);
	void ProcessPatch(Nu3D::InstanceData* spriteStruct, Nu3D::Patch* patch);
	void SetVertexColorModulation(int32_t red, int32_t green, int32_t blue);
	int32_t EnableVertexColorModulation(int32_t a1);
	void DrawSingleTexturedTriangle(void* vertices, int32_t texIndex, int32_t renderState);
	int32_t Set9F5FF8(int32_t value);
	void BlitTextureByIndex(int32_t texIndex, LONG destX, LONG destY, int32_t width, int32_t height, int32_t horizontalWrapOffset, int32_t verticalWrapOffset, int32_t srcX, int32_t srcY);
}

namespace DevDraw
{
	extern int32_t g_vertexCount;

	int16_t DrawSlots();
	int16_t FlushDrawBufferSlot(int16_t bufferIndex);
	int16_t FlushTransparentDrawBufferSlot(int16_t bufferIndex);
}
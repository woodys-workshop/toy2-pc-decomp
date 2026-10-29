#pragma once

#include "Common.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include <windows.h>

namespace DrawingDevice
{
	struct DrawingDeviceSlot
	{
		int32_t valid;
		int32_t unkInt2;
		int32_t width;
		int32_t height;
		LPDIRECTDRAWSURFACE4 surface1;
		LPDIRECTDRAWSURFACE4 surface2;
	};

	struct DDAppDevice
	{
		struct DisplayMode
		{
			DDSURFACEDESC2 surfaceDesc;
			char modeText[40];
			DisplayMode* nextDisplayMode;
		};

		struct App
		{
			GUID guid;
			App* ref;
			char driverName[40];
			char driverDesc[40];
			DDCAPS ddCaps1;
			DDCAPS ddCaps2;
			HMONITOR hMonitor;
			DWORD vidMemFree;
			DWORD vidMemTotal;
			DWORD freeTextureMem;
			DWORD totalTextureMem;
			DWORD freeVideoMem;
			DWORD totalVideoMem;
			DDAppDevice* primaryDevice;
			DDAppDevice* deviceListHead;
			App* chainDDApp;
		};

		GUID guid;
		DDAppDevice* ref;
		char deviceName[40];
		D3DDEVICEDESC deviceDesc;
		int32_t isHardwareAccelerated;
		int32_t supportsCurrentMode;
		int32_t canRenderWindowedOnPrimary;
		DisplayMode* primaryDisplayMode;
		DisplayMode* displayModeListHead;
		DDAppDevice* nextDevice;
		App* ddAppParent;
	};

	struct CD3DFramework
	{
		HWND m_hWnd;
		int32_t m_bIsFullscreen;
		int32_t m_dwRenderWidth;
		int32_t m_dwRenderHeight;
		RECT m_rcScreenRect;
		RECT m_rcViewportRect;
		LPDIRECTDRAWSURFACE4 m_pddsFrontBuffer;
		LPDIRECTDRAWSURFACE4 m_pddsBackBuffer;
		LPDIRECTDRAWSURFACE4 m_pddsRenderTarget;
		LPDIRECTDRAWSURFACE4 m_pddsZBuffer;
		LPDIRECT3DDEVICE3 m_pd3dDevice;
		LPDIRECT3DVIEWPORT3 m_pvViewport;
		LPDIRECTDRAW4 m_pDD;
		LPDIRECT3D3 m_pD3D;
		D3DDEVICEDESC m_ddDeviceDesc;
		int32_t m_dwDeviceMemType;
		DDPIXELFORMAT m_ddpfZBuffer;
		int32_t m_initialized;
		DrawingDeviceSlot m_slots[8];

		CD3DFramework();

		void Release();
		int32_t Cleanup();

		HRESULT InitalizeForWindow(HWND hWnd, GUID* ddAppGuid, DDAppDevice* device, DDAppDevice::DisplayMode* displayMode, uint8_t flags);
		HRESULT InitalizeDeviceAndSurfaces(GUID* ddAppGuid, GUID* deviceGuid, DDAppDevice::DisplayMode* displayMode, uint8_t flags);
		HRESULT CreateDirectDraw(LPGUID lpGUID, uint8_t flags);
		HRESULT SelectD3DDeviceAndZFormat(GUID* deviceGuid, uint8_t flags);
		HRESULT CreatePrimaryChainAndRects(DDAppDevice::DisplayMode* displayMode, uint8_t flags);
		HRESULT CreateZBuffer();
		HRESULT CreateD3DDevice(const CLSID* guid);
		HRESULT CreateAndSetViewport();
		int32_t RestoreToGDISurface(int32_t refreshWindow);
		int32_t GetSlotSurfaceByIndex(uint32_t index, LPDIRECTDRAWSURFACE4* surfaceOut);

		static HRESULT Build(HWND hWnd, GUID* guid, DDAppDevice* device, DDAppDevice::DisplayMode* displayMode, uint8_t flags);
		static void InitSurfaceDesc(LPDDSURFACEDESC2 ddSurfaceDesc, DWORD flags, DWORD caps);
		static void BuildViewport(D3DVIEWPORT2* viewport, DWORD width, DWORD height);
		static HRESULT WINAPI EnumZBufferFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext);
	};

	extern CD3DFramework* g_drawingDevice;
	extern DDAppDevice::App* g_ddAppListHead;
	extern DDAppDevice::App* g_primaryDDApp;

	LPDIRECTDRAW4 GetDDraw4();
	LPDIRECT3D3 GetD3D();
	LPDIRECT3DDEVICE3 GetD3DDevice();
	LPDIRECT3DVIEWPORT3 GetViewport();
	int32_t GetWidth();
	int32_t GetHeight();
	int32_t GetDestWidth();
	int32_t GetDestHeight();
	LPDIRECTDRAWSURFACE4 GetBackBuffer();
	int32_t SetViewport(LPD3DVIEWPORT2 viewport);
	int32_t BuildFreshViewport(LPD3DVIEWPORT2 viewport);
	HRESULT CreateMaterial(LPDIRECT3DMATERIAL3* outMaterial);
	RECT* GetDestRect();
	void Quit();
	HRESULT GetChosenDevice_T(DDAppDevice::App** outApp, DDAppDevice** outDevice);
	DDAppDevice::App* GetListHead();
	LPD3DDEVICEDESC CopySurfaceDesc(LPD3DDEVICEDESC outSurfaceDesc);
	HRESULT SetRenderState(D3DRENDERSTATETYPE renderStateType, DWORD value);
	HRESULT SetLightState(D3DLIGHTSTATETYPE lightState, DWORD value);
	HRESULT CreateLight(LPDIRECT3DLIGHT* outLight);
	HRESULT SetLight(LPDIRECT3DLIGHT light, LPD3DLIGHT2 d3dLight);
	HRESULT AddLight(LPDIRECT3DLIGHT light);
	HRESULT DeleteLight(LPDIRECT3DLIGHT light);
	int32_t ReleaseLight(LPDIRECT3DLIGHT light);
	HRESULT SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE state, DWORD value);
	HRESULT ClearScreen(DWORD clearFlags, D3DCOLOR clearColor);
	HRESULT BeginScene();
	HRESULT PresentFrame();
	void EndScene();
	HRESULT BindTexWithStage(int32_t textureIndex, int32_t stageIndex);
	HRESULT BindTexToStage0(int32_t textureIndex);
	HRESULT SetViewTransform(D3DMATRIX* matrix);
	HRESULT SetWorldTransform(D3DMATRIX* matrix);
	HRESULT DrawPrimitive(D3DPRIMITIVETYPE primitiveType, DWORD vertexTypeDesc, void* vertices, DWORD vertexCount, DWORD flags);
	HRESULT SetProjectionTransform(D3DMATRIX* matrix);
	HRESULT LockPrimarySurface(DDSURFACEDESC2* surfaceDesc);
	HRESULT UnlockPrimarySurface();
	int32_t RestoreToGDISurface(int32_t refreshWindow);
	int32_t GetSlotSurfaceByIndex(uint32_t index, LPDIRECTDRAWSURFACE4* surface);
	HRESULT GetDC(LPDIRECTDRAWSURFACE4 drawSurface, HDC* deviceContext);
	HRESULT ReleaseDC(LPDIRECTDRAWSURFACE4 drawSurface, HDC hdc);

	STATIC_ASSERT(sizeof(DrawingDeviceSlot) == 0x18);
	STATIC_ASSERT(sizeof(CD3DFramework) == 0x234);
	STATIC_ASSERT(sizeof(DDAppDevice::DisplayMode) == 0xA8);
	STATIC_ASSERT(sizeof(DDAppDevice::App) == 0x384);
	STATIC_ASSERT(sizeof(DDAppDevice) == 0x154);
}

namespace HardwareDevice
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
	HRESULT LockVertexBuffer(LPDIRECT3DVERTEXBUFFER vertexBuffer, DWORD dwFlags, LPVOID* lplpData, DWORD* lpStride);
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

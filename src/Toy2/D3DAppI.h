#pragma once

#include "Toy2/D3DApp.h"

#define D3DAPP_MAXCLEARRECTS 30

struct Defaults
{
	RenderStateCache rs;
	BOOL bTexturesDisabled;
	BOOL bResizingDisabled;
	BOOL bClearsOn;
	char name[30];
};

STATIC_ASSERT(sizeof(Defaults) == 0x64);

namespace Toy2
{
	extern D3DAppInfo* g_d3dApp;
	void InitSoftwareRenderer();
	void InitDirect3DRenderer();
}

namespace SoftwareRenderer
{
	extern LPDIRECTDRAWPALETTE g_nativePalette;
}

namespace D3DApp
{
	extern HRESULT g_lastError;
	extern PALETTEENTRY g_originalPpe[256];
	extern BOOL (*g_d3dDeviceDestroyCallback)(LPVOID);
	extern LPVOID g_d3dDeviceDestroyCallbackContext;
	extern BOOL (*g_d3dDeviceCreateCallback)(int32_t, int32_t, LPDIRECT3DVIEWPORT2*, LPVOID);
	extern LPVOID g_d3dDeviceCreateCallbackContext;

	void LogErrorNotSet();

	BOOL CreateD3DApp(LPSTR lpCmdLine);
	BOOL D3DRestartShutdown();
	BOOL D3DRestartStartMode();
	BOOL D3DRestart();
	BOOL D3DAppCreateFromHWND(DWORD flags, HWND hwnd, D3DAppInfo** lpD3DApp);
	BOOL D3DAppSetRenderState(RenderStateCache* lpState);
	BOOL D3DAppGetRenderState(RenderStateCache* lpState);
	BOOL D3DAppClearBackBuffer(DWORD flags);
	BOOL D3DAppCheckForLostSurfaces(void);
	BOOL D3DAppPause(BOOL flag);
	HRESULT D3DAppLastError(void);
	BOOL D3DAppDestroy(void);

	void D3DAppISetDefaults(void);
	void D3DAppISetClientSize(HWND hwnd, int32_t w, int32_t h, BOOL bReturnFromFullscreen);
	void D3DAppIGetClientWin(HWND hwnd);
	void dpf(LPSTR fmt, ...);

	BOOL InitFontAndTextBuffers(void);
	void ReleaseFontAndTextBuffers(void);

	void OverrideDefaults(Defaults* defaults);
	void UnkFunc40A870();
	void UnkFunc40A8B0();
	int16_t InitTextureMaterials();
	BOOL RestoreSurfaces();
	void CleanUpAndPostQuit(void);
	void ShowLastError();
}

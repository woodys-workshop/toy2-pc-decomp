#pragma once

#include "Common.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>

#define D3DAPP_MAXTEXTURES 64
#define D3DAPP_MAXTEXTUREFORMATS 14
#define D3DAPP_WINDOWMINIMUM 50
#define D3DAPP_BOGUS -100

enum D3DTextureType
{
	D3DTEXTURE_TYPE_NULL = 0,
	D3DTEXTURE_TYPE_8BIT_PALL = 1,
	D3DTEXTURE_TYPE_16BIT_RGB = 2,
	D3DTEXTURE_TYPE_24BIT_RGB = 3,
	D3DTEXTURE_TYPE_32BIT_RGB = 4,
	D3DTEXTURE_TYPE_8BIT_PLAIN = 5,
	D3DTEXTURE_TYPE_16BIT_PLAIN = 6,
};

struct D3DAppMode
{
	int32_t w;
	int32_t h;
	int32_t bpp;
	BOOL bThisDriverCanDo;
};

struct D3DAppTextureFormat
{
	int32_t bPalettized;
	int32_t bAlphaPixels;
	int16_t indexBPP;
	int16_t rgbBPP;
	int16_t redBPP;
	int16_t blueBPP;
	int16_t greenBPP;
	int16_t alphaBPP;
	DDSURFACEDESC ddsd;
};

struct D3DAppInfo
{
	HWND hwnd;
	int32_t textureType[D3DAPP_MAXTEXTURES];
	int32_t textureState[D3DAPP_MAXTEXTURES];
	D3DTEXTUREHANDLE textureHandle[D3DAPP_MAXTEXTURES];
	LPDIRECTDRAWSURFACE3 lpTextureSurf[D3DAPP_MAXTEXTURES];
	LPDIRECTDRAWSURFACE3 lpSrcTextureSurf[D3DAPP_MAXTEXTURES];
	LPDIRECT3DTEXTURE2 lpTexture[D3DAPP_MAXTEXTURES];
	LPDIRECT3DTEXTURE2 lpSrcTexture[D3DAPP_MAXTEXTURES];
	LPDIRECT3DMATERIAL2 lpTextureMat[D3DAPP_MAXTEXTURES];
	D3DMATERIALHANDLE lpTextureMatHandle[D3DAPP_MAXTEXTURES];
	LPDIRECT3DMATERIAL2 lpSkyMat;
	LPDIRECT3DMATERIAL2 lpGroundMat;
	D3DMATERIALHANDLE lpGroundMatHandle;
	D3DMATERIALHANDLE lpSkyMatHandle;
	int32_t redShift[D3DAPP_MAXTEXTURES];
	int32_t greenShift[D3DAPP_MAXTEXTURES];
	int32_t blueShift[D3DAPP_MAXTEXTURES];
	int32_t redScale[D3DAPP_MAXTEXTURES];
	int32_t greenScale[D3DAPP_MAXTEXTURES];
	int32_t blueScale[D3DAPP_MAXTEXTURES];
	uint8_t unkTextureFlag[D3DAPP_MAXTEXTURES];
	uint8_t texturePalette[D3DAPP_MAXTEXTURES][768];
	int32_t colorKeyIndex[D3DAPP_MAXTEXTURES];
	LPDIRECT3D2 lpD3D;
	LPDIRECT3DDEVICE2 lpD3DDevice;
	LPDIRECT3DVIEWPORT2 lpD3DViewport;
	LPDIRECTDRAW2 lpDD;
	uint8_t bIsPrimary;
	LPDIRECTDRAWSURFACE3 lpFrontBuffer;
	LPDIRECTDRAWSURFACE3 lpBackBuffer;
	LPDIRECTDRAWSURFACE3 lpZBuffer;
	uint8_t bBackBufferInVideo;
	uint8_t bZBufferInVideo;
	D3DAppMode windowsDisplay;
	SIZE szClient;
	POINT pClientOnPrimary;
	uint8_t bPaused;
	uint8_t bAppActive;
	uint8_t bTexturesDisabled;
	uint8_t bOnlySystemMemory;
	uint8_t bOnlyEmulation;
	uint8_t bMinimized;
	uint8_t bRenderingIsOK;
};

struct RenderStateCache
{
	int32_t zWriteEnable;
	int32_t texturePerspective;
	int32_t shadeMode;
	int32_t textureFilter;
	int32_t cullMode;
	int32_t fillMode;
	int32_t ditherEnable;
	int32_t specularEnable;
	int32_t antiAlias;
	int32_t fogEnable;
	int32_t fogColor;
	int32_t fogTableMode;
	float fogStart;
	float fogEnd;
};

struct WindowData
{
	MSG wndEventMsg;
	HACCEL hAccTable;
	HINSTANCE hInstance;
	HINSTANCE hPrev;
	char* lpCmdLine;
	HWND mainHwnd;
	WNDCLASSEXA wndClass;
	int32_t nShowCmd;
	RenderStateCache stateCache;
	int32_t unkInt1;
	int32_t unkInt2;
	int32_t bClearsOn;
	int32_t bShowFrameRate;
	int32_t bShowInfo;
	int32_t bResizingDisabled;
	int32_t unkInt7;
	int32_t wndIsExiting;
	int32_t unkInt8;
	LPDIRECTDRAWSURFACE3 unusedDDSurface;
	LPDIRECTDRAWSURFACE3 unusedDDSurface2;
};

struct DisplayMode
{
	DWORD width;
	DWORD height;
	DWORD rgbBitCount;
};

struct InterfaceDevice
{
	int32_t valid;
	int32_t hasTexturing;
	int32_t isSquareTexturesOnly;
	int32_t isHardwareAccelerated;
	int32_t hasZBuffer;
	int32_t unkInt1;
	int32_t hasAlphaBlending;
	GUID guid;
	char baseName[64];
	char description[64];
	int32_t numTextureFormats;
	D3DAppTextureFormat textureFormat[D3DAPP_MAXTEXTUREFORMATS];
	uint8_t unkPad0[4];
	D3DDEVICEDESC hwDeviceDesc;
};

struct ExamineDevice
{
	int32_t valid;
	int32_t hasHardwareAccel;
	int32_t has3DSupport;
	int32_t hasVideoDMA;
	int32_t svbCaps;
	int32_t svbcKeyCaps;
	int32_t svbFxCaps;
	int32_t unkInt8;
	int32_t texVidMemTot;
	int32_t texVidMemFree;
	int32_t hasLowVideoMem;
	int32_t isPrimaryDisplay;
	int32_t isPreferredDevice;
	GUID deviceGUID;
	DDCAPS ddCaps;
	char driverName[64];
	char driverDesc[64];
	int32_t displayModeCount;
	int32_t selectedDisplayModeIndex;
	DisplayMode displayModes[128];
	IDirectDraw2* dd2;
	IDirect3D2* dd3D;
	int32_t deviceCount;
	InterfaceDevice interfaceDevices[6];
};

struct PC
{
	int32_t fullscreenMode;
	int32_t softwareRenderMode;
	int32_t ddDeviceCount;
	ExamineDevice examineDevices[4];
	ExamineDevice* defaultDriver;
	InterfaceDevice* selectedInterfaceDevice;
	DisplayMode* mode;
	DisplayMode* oldMode;
};

enum RenderMode
{
	RENDERMODE_SOFTWARE = 1,
	RENDERMODE_D3D = 2,
};

struct TPageSize
{
	int16_t width;
	int16_t height;
};

// Size Assertions
STATIC_ASSERT(sizeof(D3DAppMode) == 0x10);
STATIC_ASSERT(sizeof(D3DAppTextureFormat) == 0x80);
STATIC_ASSERT(sizeof(D3DAppInfo) == 0xD0A0);
STATIC_ASSERT(sizeof(RenderStateCache) == 0x38);
STATIC_ASSERT(sizeof(WindowData) == 0xC8);
STATIC_ASSERT(sizeof(DisplayMode) == 0xC);
STATIC_ASSERT(sizeof(InterfaceDevice) == 0x8B0);
STATIC_ASSERT(sizeof(ExamineDevice) == 0x3C74);
STATIC_ASSERT(sizeof(PC) == 0xF1EC);
STATIC_ASSERT(sizeof(TPageSize) == 0x4);

namespace SoftwareRenderer
{
	int32_t ClearRenderSurfaces();
}

namespace D3DApp
{
	extern D3DAppInfo g_d3dAppI;
	extern PC g_pcStruct;
	extern int16_t g_renderMode;
	extern int32_t g_changingCoopLevel;
	extern int32_t g_usesPalette;
	extern int32_t g_primaryPalettized;
	extern LPDIRECTDRAWPALETTE g_lpPalette;

	extern RenderStateCache g_d3dAppRS;
	extern SIZE g_szBuffers;
	extern LPDIRECTDRAWCLIPPER g_lpClipper;
	extern PALETTEENTRY g_ppe[256];

	HRESULT CreateSurface3(LPDIRECTDRAW2 lpDD, LPDDSURFACEDESC desc, LPDIRECTDRAWSURFACE3* surf, IUnknown* punk);
	HRESULT D3DAppICreateSurface(LPDDSURFACEDESC lpDDSurfDesc, LPDIRECTDRAWSURFACE3* lpDDSurface);
	HRESULT D3DAppIGetSurfDesc(LPDDSURFACEDESC lpDDSurfDesc, LPDIRECTDRAWSURFACE3 lpDDSurf);
	BOOL D3DAppICreateBuffers(HWND hwnd, int32_t w, int32_t h, int32_t bpp, BOOL bFullscreen, BOOL bIsHardware);
	BOOL D3DAppICheckForPalettized();
	BOOL D3DAppICreateZBuffer(int32_t w, int32_t h);
	BOOL D3DAppISetCoopLevel(HWND hwnd, BOOL bFullscreen);
	BOOL D3DAppIRestoreDispMode();
	BOOL D3DAppIRememberWindowsMode();
	DWORD D3DAppIBPPToDDBD(int32_t bpp);
	DWORD D3DAppTotalVideoMemory();
	BOOL D3DAppISetRenderState();
	HRESULT CALLBACK EnumTextureFormatsCallback(LPDDSURFACEDESC lpDDSD, LPVOID lpContext);
	HRESULT D3DAppCreateSurface(DDSURFACEDESC* ddsd, LPDIRECTDRAWSURFACE3* lplpSurf);

	extern int32_t g_checkAvailableMem;

	extern int32_t g_textureAlpha;
	extern int16_t g_unk50A690[D3DAPP_MAXTEXTURES];
	extern int32_t g_tpageIs8Bit[D3DAPP_MAXTEXTURES];
	extern TPageSize g_tpageSize[D3DAPP_MAXTEXTURES];
	extern D3DTEXTUREHANDLE g_masterTextureHandle[D3DAPP_MAXTEXTURES];
	extern uint8_t* g_tpageData[D3DAPP_MAXTEXTURES];
	extern int16_t g_colorKeyIndex;
	extern uint8_t* g_unk8827A0[D3DAPP_MAXTEXTURES];

	void GetVidMem();
	char* GetTextureTypeName(int32_t type);
	void FreeTPageData(int32_t tpage);
	BOOL UnkFunc409C80();
	D3DAppTextureFormat* MapTPageTextureFormats(int16_t tpage);
	BOOL D3DAppILoadTextureSurf(int32_t n, BOOL* bInVideo);
	BOOL D3DAppIGetTextureHandle(int32_t n);
	DWORD DDColorMatch(LPDIRECTDRAWSURFACE3 pdds, COLORREF rgb);
	HRESULT DDSetColorKey(LPDIRECTDRAWSURFACE3 pdds, COLORREF rgb);
	LPDIRECTDRAWSURFACE3 D3DAppILoadSurface(LPDIRECTDRAW2 lpDD, int32_t n, LPDDSURFACEDESC lpFormat, DWORD memoryflag);
}

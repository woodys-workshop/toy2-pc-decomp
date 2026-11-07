#pragma once

#include "Common.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include <windows.h>

namespace D3DTextr
{
	struct TextureContainer;

	LPDIRECTDRAWSURFACE4 D3DTextr_GetSurface(char* strName);
	LPDIRECTDRAWSURFACE4 GetContainerSurface(TextureContainer* ptcTexture);
	LPDIRECT3DTEXTURE2 D3DTextr_GetTexture(char* strName);
	LPDIRECT3DTEXTURE2 GetContainerTexture(TextureContainer* ptcTexture);
	LPVOID GetContainerBuffer(TextureContainer* ptcTexture);
	void GetContainerSurfaceDesc(TextureContainer* ptcTexture, DDSURFACEDESC2** ppddsd);
	TextureContainer* FindTexture_T(char* strName);

	HRESULT D3DTextr_Invalidate(char* strName);
	HRESULT InvalidateContainer(TextureContainer* ptcTexture);
	HRESULT D3DTextr_Restore(char* strName, LPDIRECT3DDEVICE3 pd3dDevice);
	HRESULT RestoreContainer(TextureContainer* ptcTexture, LPDIRECT3DDEVICE3 pd3dDevice);
	HRESULT D3DTextr_InvalidateAllTextures();
	HRESULT D3DTextr_RestoreAllTextures(LPDIRECT3DDEVICE3 pd3dDevice);

#define D3DTEXTR_TRANSPARENTWHITE 0x00000001
#define D3DTEXTR_TRANSPARENTBLACK 0x00000002
#define D3DTEXTR_32BITSPERPIXEL 0x00000004
#define D3DTEXTR_TRANSPARENTGREEN 0x00000008

	HRESULT D3DTextr_CreateTexture(char* strName, DWORD dwStage, DWORD dwFlags);
	HRESULT CreateTextureFromBitmaps(HBITMAP hbmBitmap, HBITMAP hbmAlpha, char* strName, DWORD dwStage, DWORD dwFlags);
	HRESULT D3DTextr_DestroyTexture(char* strName);
	HRESULT DestroyContainer(TextureContainer* ptcTexture, BOOL bForce);
	HRESULT DestroyContainer_T(TextureContainer* ptcTexture);
	void DestroyAllTextures();
	HRESULT AddRefContainer(TextureContainer* ptcTexture);
	void D3DTextr_SetTexturePath(char* strTexturePath);
}

// GLOBAL: TOY2 0x00884474
// g_ptcTextureList

// GLOBAL: TOY2 0x00508288
// g_strTexturePath

// GLOBAL: TOY2 0x00508488
// g_strRegValueName

// SYNTHETIC: TOY2 0x004B1900
// $E4

// SYNTHETIC: TOY2 0x004B1910
// $E1

// SYNTHETIC: TOY2 0x004B1920
// $E3

// SYNTHETIC: TOY2 0x004B1930
// $E2

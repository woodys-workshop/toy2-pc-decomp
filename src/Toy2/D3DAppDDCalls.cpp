#include "Toy2/D3DApp.h"
#include "Logger.h"

namespace D3DApp
{
	// GLOBAL: TOY2 0x0050A558
	RenderStateCache g_d3dAppRS;

	// GLOBAL: TOY2 0x0050A710
	SIZE g_szBuffers;

	// GLOBAL: TOY2 0x0050AA60
	LPDIRECTDRAWCLIPPER g_lpClipper = 0;

	// GLOBAL: TOY2 0x0050AF6C
	PALETTEENTRY g_ppe[256];
}

namespace D3DApp
{
	// FUNCTION: TOY2 0x00408BE0
	HRESULT CreateSurface3(LPDIRECTDRAW2 lpDD, LPDDSURFACEDESC desc, LPDIRECTDRAWSURFACE3* surf, IUnknown* punk)
	{
		LPDIRECTDRAWSURFACE tempsurf;
		HRESULT hr;

		hr = lpDD->CreateSurface(desc, &tempsurf, punk);
		if (hr < 0)
			Logger::LogDDError("lpDD->CreateSurface(desc, &tempsurf, punk)", hr);

		hr = tempsurf->QueryInterface(IID_IDirectDrawSurface3, (void**)surf);
		if (hr < 0)
			Logger::LogDDError("tempsurf->QueryInterface(IID_IDirectDrawSurface3,(void**) surf)", hr);

		if (tempsurf)
		{
			tempsurf->Release();
			tempsurf = 0;
		}

		return DD_OK;
	}

	// FUNCTION: TOY2 0x00408C50 [MATCHED]
	HRESULT D3DAppICreateSurface(LPDDSURFACEDESC lpDDSurfDesc, LPDIRECTDRAWSURFACE3* lpDDSurface)
	{
		if (g_d3dAppI.bOnlySystemMemory)
			lpDDSurfDesc->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

		return CreateSurface3(g_d3dAppI.lpDD, lpDDSurfDesc, lpDDSurface, 0);
	}

	// FUNCTION: TOY2 0x0040B290 [MATCHED]
	HRESULT D3DAppIGetSurfDesc(LPDDSURFACEDESC lpDDSurfDesc, LPDIRECTDRAWSURFACE3 lpDDSurf)
	{
		HRESULT result;

		memset(lpDDSurfDesc, 0, sizeof(DDSURFACEDESC));
		lpDDSurfDesc->dwSize = sizeof(DDSURFACEDESC);
		result = lpDDSurf->GetSurfaceDesc(lpDDSurfDesc);

		return result;
	}

	// FUNCTION: TOY2 0x0040B2C0
	BOOL D3DAppICreateBuffers(HWND hwnd, int32_t w, int32_t h, int32_t bpp, BOOL bFullscreen, BOOL bIsHardware)
	{
		DDSURFACEDESC ddsd;
		DDSCAPS ddscaps;
		HRESULT hr;

		if (g_lpClipper)
		{
			g_lpClipper->Release();
			g_lpClipper = 0;
		}

		if (g_d3dAppI.lpBackBuffer)
		{
			g_d3dAppI.lpBackBuffer->Release();
			g_d3dAppI.lpBackBuffer = 0;
		}

		if (g_d3dAppI.lpFrontBuffer)
		{
			g_d3dAppI.lpFrontBuffer->Release();
			g_d3dAppI.lpFrontBuffer = 0;
		}

		if (w < D3DAPP_WINDOWMINIMUM)
			w = D3DAPP_WINDOWMINIMUM;

		if (h < D3DAPP_WINDOWMINIMUM)
			h = D3DAPP_WINDOWMINIMUM;

		g_szBuffers.cx = w;
		g_szBuffers.cy = h;

		if (bFullscreen)
		{
			memset(&ddsd, 0, sizeof(DDSURFACEDESC));
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_3DDEVICE | DDSCAPS_COMPLEX;
			ddsd.dwBackBufferCount = 1;

			if (bIsHardware)
				ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;

			D3DAppICreateSurface(&ddsd, &g_d3dAppI.lpFrontBuffer);

			ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
			hr = g_d3dAppI.lpFrontBuffer->GetAttachedSurface(&ddscaps, &g_d3dAppI.lpBackBuffer);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpFrontBuffer->GetAttachedSurface(&ddscaps, &d3dappi.lpBackBuffer)", hr);

			hr = D3DAppIGetSurfDesc(&ddsd, g_d3dAppI.lpBackBuffer);

			if (hr < 0)
				Logger::LogDDError("D3DAppIGetSurfDesc(&ddsd, d3dappi.lpBackBuffer)", hr);

			g_d3dAppI.bBackBufferInVideo = (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) ? TRUE : FALSE;
		}
		else
		{
			memset(&ddsd, 0, sizeof(DDSURFACEDESC));
			ddsd.dwSize = sizeof(DDSURFACEDESC);
			ddsd.dwFlags = DDSD_CAPS;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			CreateSurface3(g_d3dAppI.lpDD, &ddsd, &g_d3dAppI.lpFrontBuffer, 0);

			ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
			ddsd.dwWidth = w;
			ddsd.dwHeight = h;
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

			if (bIsHardware)
				ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
			else
				ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

			D3DAppICreateSurface(&ddsd, &g_d3dAppI.lpBackBuffer);

			hr = D3DAppIGetSurfDesc(&ddsd, g_d3dAppI.lpBackBuffer);

			if (hr < 0)
				Logger::LogDDError("D3DAppIGetSurfDesc(&ddsd, d3dappi.lpBackBuffer)", hr);

			g_d3dAppI.bBackBufferInVideo = (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) ? TRUE : FALSE;

			hr = g_d3dAppI.lpDD->CreateClipper(0, &g_lpClipper, 0);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpDD->CreateClipper(0, &lpClipper, 0)", hr);

			hr = g_lpClipper->SetHWnd(0, hwnd);

			if (hr < 0)
				Logger::LogDDError("lpClipper->SetHWnd(0, hwnd)", hr);

			hr = g_d3dAppI.lpFrontBuffer->SetClipper(g_lpClipper);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpFrontBuffer->SetClipper(lpClipper)", hr);
		}

		SoftwareRenderer::ClearRenderSurfaces();

		return TRUE;
	}

	// FUNCTION: TOY2 0x0040B670
	BOOL D3DAppICheckForPalettized()
	{
		DDSURFACEDESC ddsd;
		HRESULT hr;

		hr = D3DAppIGetSurfDesc(&ddsd, g_d3dAppI.lpBackBuffer);

		if (hr < 0)
			Logger::LogDDError("D3DAppIGetSurfDesc(&ddsd, d3dappi.lpBackBuffer)", hr);

		g_primaryPalettized = (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) ? TRUE : FALSE;

		if (g_primaryPalettized)
		{
			int32_t i;

			HDC hdc = GetDC(0);
			GetSystemPaletteEntries(hdc, 0, (1 << 8), g_ppe);
			ReleaseDC(0, hdc);

			if (! g_pcStruct.fullscreenMode)
			{
				for (i = 0; i < 10; i++)
					g_ppe[i].peFlags = D3DPAL_READONLY;
				for (i = 10; i < 256 - 10; i++)
					g_ppe[i].peFlags = D3DPAL_FREE | PC_RESERVED;
				for (i = 256 - 10; i < 256; i++)
					g_ppe[i].peFlags = D3DPAL_READONLY;
			}
			else
			{
				g_ppe[0].peFlags = D3DPAL_READONLY;
				for (i = 1; i < 255; i++)
					g_ppe[i].peFlags = D3DPAL_FREE | PC_RESERVED;
				g_ppe[255].peFlags = D3DPAL_READONLY;
			}

			hr = g_d3dAppI.lpDD->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE, g_ppe, &g_lpPalette, 0);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpDD->CreatePalette(0x00000004l | 0x00000008l, ppe, &lpPalette, 0)", hr);

			hr = g_d3dAppI.lpBackBuffer->SetPalette(g_lpPalette);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpBackBuffer->SetPalette(lpPalette)", hr);

			hr = g_d3dAppI.lpFrontBuffer->SetPalette(g_lpPalette);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpFrontBuffer->SetPalette(lpPalette)", hr);

			g_usesPalette = TRUE;
		}

		return TRUE;
	}

	// FUNCTION: TOY2 0x0040B7E0
	BOOL D3DAppICreateZBuffer(int32_t w, int32_t h)
	{
		DDSURFACEDESC ddsd;
		DWORD devDepth;
		HRESULT hr;

		if (g_d3dAppI.lpZBuffer)
		{
			g_d3dAppI.lpZBuffer->Release();
			g_d3dAppI.lpZBuffer = 0;
		}

		if (! g_pcStruct.selectedInterfaceDevice->hasZBuffer)
			return TRUE;

		memset(&ddsd, 0, sizeof(DDSURFACEDESC));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_ZBUFFERBITDEPTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
		ddsd.dwHeight = h;
		ddsd.dwWidth = w;

		ddsd.ddsCaps.dwCaps |= g_pcStruct.selectedInterfaceDevice->isHardwareAccelerated ? DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM : DDSCAPS_SYSTEMMEMORY;

		devDepth = g_pcStruct.selectedInterfaceDevice->hwDeviceDesc.dwDeviceZBufferBitDepth;

		if (devDepth & DDBD_32)
			ddsd.dwZBufferBitDepth = 32;
		else if (devDepth & DDBD_24)
			ddsd.dwZBufferBitDepth = 24;
		else if (devDepth & DDBD_16)
			ddsd.dwZBufferBitDepth = 16;
		else if (devDepth & DDBD_8)
			ddsd.dwZBufferBitDepth = 8;
		else
		{
			Logger::Log("Unsupported Z-buffer depth requested by device.\n");
			return FALSE;
		}

		{
			LPDIRECTDRAWSURFACE surf;

			hr = g_d3dAppI.lpDD->CreateSurface(&ddsd, &surf, 0);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpDD->CreateSurface(&ddsd, &surf, 0)", hr);

			hr = surf->QueryInterface(IID_IDirectDrawSurface3, (void**)&g_d3dAppI.lpZBuffer);

			if (hr < 0)
				Logger::LogDDError("surf->QueryInterface(IID_IDirectDrawSurface3,(void**) &d3dappi.lpZBuffer)", hr);

			surf->Release();
		}

		hr = g_d3dAppI.lpBackBuffer->AddAttachedSurface(g_d3dAppI.lpZBuffer);

		if (hr < 0)
			Logger::LogDDError("d3dappi.lpBackBuffer->AddAttachedSurface(d3dappi.lpZBuffer)", hr);

		hr = D3DAppIGetSurfDesc(&ddsd, g_d3dAppI.lpZBuffer);

		if (hr < 0)
			Logger::LogDDError("D3DAppIGetSurfDesc(&ddsd, d3dappi.lpZBuffer)", hr);

		g_d3dAppI.bZBufferInVideo = (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) ? TRUE : FALSE;

		if (g_pcStruct.selectedInterfaceDevice->isHardwareAccelerated && ! g_d3dAppI.bZBufferInVideo)
		{
			Logger::LogErrorNotSetLn("Could not fit the Z-buffer in video memory for this hardware device.\n");

			if (g_d3dAppI.lpZBuffer)
			{
				g_d3dAppI.lpZBuffer->Release();
				g_d3dAppI.lpZBuffer = 0;
			}

			return FALSE;
		}

		return TRUE;
	}

	// FUNCTION: TOY2 0x0040B9D0 [MATCHED]
	BOOL D3DAppISetCoopLevel(HWND hwnd, BOOL bFullscreen)
	{
		HRESULT hr;

		if (bFullscreen)
		{
			g_changingCoopLevel = TRUE;
			hr = g_d3dAppI.lpDD->SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpDD->SetCooperativeLevel(hwnd, 0x00000010l | 0x00000001l | 0x00000040l)", hr);

			g_changingCoopLevel = FALSE;
		}
		else
		{
			g_changingCoopLevel = TRUE;
			hr = g_d3dAppI.lpDD->SetCooperativeLevel(hwnd, DDSCL_NORMAL);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpDD->SetCooperativeLevel(hwnd, 0x00000008l)", hr);

			g_changingCoopLevel = FALSE;
		}

		return TRUE;
	}

	// FUNCTION: TOY2 0x0040BA30 [MATCHED]
	BOOL D3DAppIRestoreDispMode()
	{
		HRESULT hr;

		g_changingCoopLevel = TRUE;
		hr = g_d3dAppI.lpDD->RestoreDisplayMode();

		if (hr < 0)
			Logger::LogDDError("d3dappi.lpDD->RestoreDisplayMode()", hr);

		g_changingCoopLevel = FALSE;

		return TRUE;
	}

	// FUNCTION: TOY2 0x0040BA70 [MATCHED]
	BOOL D3DAppIRememberWindowsMode()
	{
		DDSURFACEDESC ddsd;
		HRESULT hr;

		memset(&ddsd, 0, sizeof(DDSURFACEDESC));
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		hr = g_d3dAppI.lpDD->GetDisplayMode(&ddsd);

		if (hr < 0)
			Logger::LogDDError("d3dappi.lpDD->GetDisplayMode(&ddsd)", hr);

		g_d3dAppI.windowsDisplay.w = ddsd.dwWidth;
		g_d3dAppI.windowsDisplay.h = ddsd.dwHeight;
		g_d3dAppI.windowsDisplay.bpp = ddsd.ddpfPixelFormat.dwRGBBitCount;

		return TRUE;
	}
}

namespace SoftwareRenderer
{
	// FUNCTION: TOY2 0x0040BAE0 [MATCHED]
	int32_t ClearRenderSurfaces()
	{
		DDSURFACEDESC ddsd;
		RECT dst;
		DDBLTFX ddbltfx;
		HRESULT hr;

		if (D3DApp::g_renderMode == RENDERMODE_SOFTWARE && D3DApp::g_pcStruct.mode->rgbBitCount == 8)
			return TRUE;

		if (D3DApp::g_d3dAppI.lpFrontBuffer)
		{
			hr = D3DApp::D3DAppIGetSurfDesc(&ddsd, D3DApp::g_d3dAppI.lpFrontBuffer);
			if (hr < 0)
				Logger::LogDDError("D3DAppIGetSurfDesc(&ddsd, d3dappi.lpFrontBuffer)", hr);
			memset(&ddbltfx, 0, sizeof(ddbltfx));
			ddbltfx.dwSize = sizeof(DDBLTFX);
			SetRect(&dst, 0, 0, ddsd.dwWidth, ddsd.dwHeight);
			hr = D3DApp::g_d3dAppI.lpFrontBuffer->Blt(&dst, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpFrontBuffer->Blt(&dst, 0, 0, 0x00000400l | 0x01000000l, &ddbltfx)", hr);
		}
		if (D3DApp::g_d3dAppI.lpBackBuffer)
		{
			hr = D3DApp::D3DAppIGetSurfDesc(&ddsd, D3DApp::g_d3dAppI.lpBackBuffer);
			if (hr < 0)
				Logger::LogDDError("D3DAppIGetSurfDesc(&ddsd, d3dappi.lpBackBuffer)", hr);
			memset(&ddbltfx, 0, sizeof(ddbltfx));
			ddbltfx.dwSize = sizeof(DDBLTFX);
			SetRect(&dst, 0, 0, ddsd.dwWidth, ddsd.dwHeight);
			hr = D3DApp::g_d3dAppI.lpBackBuffer->Blt(&dst, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpBackBuffer->Blt(&dst, 0, 0, 0x00000400l | 0x01000000l, &ddbltfx)", hr);
		}
		return TRUE;
	}
}

namespace D3DApp
{

	// FUNCTION: TOY2 0x0040BC60 [MATCHED]
	DWORD D3DAppIBPPToDDBD(int32_t bpp)
	{
		switch (bpp)
		{
			case 1:
				return DDBD_1;
			case 2:
				return DDBD_2;
			case 4:
				return DDBD_4;
			case 8:
				return DDBD_8;
			case 16:
				return DDBD_16;
			case 24:
				return DDBD_24;
			case 32:
				return DDBD_32;
			default:
				return (DWORD)D3DAPP_BOGUS;
		}
	}

	// FUNCTION: TOY2 0x0040BCF0 [MATCHED]
	DWORD D3DAppTotalVideoMemory()
	{
		DDCAPS driverCaps, helCaps;
		HRESULT hr;

		memset(&driverCaps, 0, sizeof(DDCAPS));
		driverCaps.dwSize = sizeof(DDCAPS);
		memset(&helCaps, 0, sizeof(DDCAPS));
		helCaps.dwSize = sizeof(DDCAPS);
		hr = g_d3dAppI.lpDD->GetCaps(&driverCaps, &helCaps);

		if (hr < 0)
			Logger::LogDDError("d3dappi.lpDD->GetCaps(&DriverCaps, &HELCaps)", hr);

		if (driverCaps.dwVidMemTotal)
			return driverCaps.dwVidMemTotal;
		else
			return helCaps.dwVidMemTotal;
	}

	// FUNCTION: TOY2 0x0040BD70 [MATCHED]
	BOOL D3DAppISetRenderState()
	{
		LPDIRECT3DDEVICE2 pDev = g_d3dAppI.lpD3DDevice;
		HRESULT hr;

		if (g_renderMode != RENDERMODE_D3D)
			return TRUE;

		if (! g_d3dAppI.lpD3DViewport)
			return TRUE;

		hr = pDev->BeginScene();

		if (hr < 0)
			Logger::LogDDError("pDev->BeginScene()", hr);

		hr = pDev->SetCurrentViewport(g_d3dAppI.lpD3DViewport);

		if (hr < 0)
			Logger::LogDDError("pDev->SetCurrentViewport(d3dappi.lpD3DViewport)", hr);

		pDev->SetRenderState(D3DRENDERSTATE_SHADEMODE, (DWORD)g_d3dAppRS.shadeMode);
		pDev->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)g_d3dAppRS.texturePerspective);
		pDev->SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)(g_d3dAppRS.zWriteEnable && g_pcStruct.selectedInterfaceDevice->hasZBuffer));
		pDev->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, (DWORD)g_d3dAppRS.zWriteEnable);
		pDev->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
		pDev->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (DWORD)g_d3dAppRS.textureFilter);
		pDev->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (DWORD)g_d3dAppRS.textureFilter);
		pDev->SetRenderState(D3DRENDERSTATE_FILLMODE, (DWORD)g_d3dAppRS.fillMode);
		pDev->SetRenderState(D3DRENDERSTATE_DITHERENABLE, (DWORD)g_d3dAppRS.ditherEnable);
		pDev->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, (DWORD)g_d3dAppRS.specularEnable);
		pDev->SetRenderState(D3DRENDERSTATE_ANTIALIAS, (DWORD)g_d3dAppRS.antiAlias);
		pDev->SetRenderState(D3DRENDERSTATE_FOGCOLOR, (DWORD)g_d3dAppRS.fogColor);

		pDev->SetLightState(D3DLIGHTSTATE_AMBIENT, 0xFFFFFFFF);

		hr = pDev->EndScene();

		if (hr < 0)
			Logger::LogDDError("pDev->EndScene()", hr);

		return TRUE;
	}

	// FUNCTION: TOY2 0x0040C8F0 [MATCHED]
	HRESULT CALLBACK EnumTextureFormatsCallback(LPDDSURFACEDESC lpDDSD, LPVOID lpContext)
	{
		uint32_t m;
		int32_t r, g, b, a;
		InterfaceDevice* lpDevice = (InterfaceDevice*)lpContext;
		D3DAppTextureFormat* lpFormat = &lpDevice->textureFormat[lpDevice->numTextureFormats];

		memset(lpFormat, 0, sizeof(D3DAppTextureFormat));
		memcpy(&lpFormat->ddsd, lpDDSD, sizeof(DDSURFACEDESC));

		if (lpDDSD->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
		{
			lpFormat->bPalettized = TRUE;
			lpFormat->indexBPP = 8;
			lpFormat->rgbBPP = 0;
			lpFormat->bAlphaPixels = FALSE;
		}
		else if (lpDDSD->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
		{
			lpFormat->bPalettized = TRUE;
			lpFormat->indexBPP = 4;
			lpFormat->rgbBPP = 0;
			lpFormat->bAlphaPixels = FALSE;
		}
		else
		{
			lpFormat->bPalettized = FALSE;
			lpFormat->indexBPP = 0;
			lpFormat->rgbBPP = (int16_t)lpDDSD->ddpfPixelFormat.dwRGBBitCount;
			lpFormat->bAlphaPixels = lpDDSD->ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS;

			for (r = 0, m = lpDDSD->ddpfPixelFormat.dwRBitMask; ! (m & 1); r++, m >>= 1)
				;
			for (r = 0; m & 1; r++, m >>= 1)
				;
			for (g = 0, m = lpDDSD->ddpfPixelFormat.dwGBitMask; ! (m & 1); g++, m >>= 1)
				;
			for (g = 0; m & 1; g++, m >>= 1)
				;
			for (b = 0, m = lpDDSD->ddpfPixelFormat.dwBBitMask; ! (m & 1); b++, m >>= 1)
				;
			for (b = 0; m & 1; b++, m >>= 1)
				;

			lpFormat->redBPP = r;
			lpFormat->greenBPP = g;
			lpFormat->blueBPP = b;
			lpFormat->alphaBPP = 0;

			if (lpFormat->bAlphaPixels)
			{
				for (a = 0, m = lpDDSD->ddpfPixelFormat.dwRGBAlphaBitMask; ! (m & 1); a++, m >>= 1)
					;
				for (a = 0; m & 1; a++, m >>= 1)
					;

				lpFormat->alphaBPP = a;
			}
		}

		Logger::Log("\n");
		Logger::Log("DIRECT 3D Texture Format %d.\n", lpDevice->numTextureFormats);
		Logger::Log("Type %s", lpFormat->indexBPP ? "PAL" : "RGB");
		Logger::Log("%d ", lpFormat->indexBPP ? lpFormat->indexBPP : lpFormat->rgbBPP);

		if (! lpFormat->indexBPP)
			Logger::Log("ARGB %dx%dx%dx%d ", lpFormat->alphaBPP, lpFormat->redBPP, lpFormat->greenBPP, lpFormat->blueBPP);

		if (lpFormat->bAlphaPixels)
			Logger::Log("ALPHAPIXELS\n\n");
		else
			Logger::Log("PIXELS\n\n");

		lpDevice->numTextureFormats++;

		return DDENUMRET_OK;
	}

	// FUNCTION: TOY2 0x0040D230 [MATCHED]
	HRESULT D3DAppCreateSurface(DDSURFACEDESC* ddsd, LPDIRECTDRAWSURFACE3* lplpSurf) { return D3DAppICreateSurface(ddsd, lplpSurf); }
}

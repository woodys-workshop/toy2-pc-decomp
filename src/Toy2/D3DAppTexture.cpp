#include "Toy2/D3DApp.h"
#include "Logger.h"
#include "RawLoader.h"
#include <stdlib.h>

namespace D3DApp
{
	// GLOBAL: TOY2 0x004E0694
	int32_t g_textureAlpha = 128;

	// GLOBAL: TOY2 0x0050A690
	int16_t g_unk50A690[D3DAPP_MAXTEXTURES];

	// GLOBAL: TOY2 0x0050A778
	int32_t g_tpageIs8Bit[D3DAPP_MAXTEXTURES];

	// GLOBAL: TOY2 0x0050AA64
	TPageSize g_tpageSize[D3DAPP_MAXTEXTURES];

	// GLOBAL: TOY2 0x0050B36C
	D3DTEXTUREHANDLE g_masterTextureHandle[D3DAPP_MAXTEXTURES];

	// GLOBAL: TOY2 0x0051A9C8
	uint8_t* g_tpageData[D3DAPP_MAXTEXTURES];

	// GLOBAL: TOY2 0x0051ABCC
	int16_t g_colorKeyIndex;

	// GLOBAL: TOY2 0x008827A0
	uint8_t* g_unk8827A0[D3DAPP_MAXTEXTURES];
}

namespace D3DApp
{
	// FUNCTION: TOY2 0x00409AE0 [MATCHED]
	void GetVidMem()
	{
		DDSCAPS caps;
		DWORD total, free;
		HRESULT hr;

		if (! g_pcStruct.softwareRenderMode && g_checkAvailableMem != 1)
		{
			Logger::Log("GetVidMem : Checking size of hardware VRAM.\n");

			caps.dwCaps = DDSCAPS_TEXTURE;
			hr = g_d3dAppI.lpDD->GetAvailableVidMem(&caps, &total, &free);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpDD->GetAvailableVidMem(&caps, &total, &free)", hr);

			Logger::Log("GetVidMem : Total TEXTURE video memory is %d, free memory is %d.\n", total, free);

			caps.dwCaps = DDSCAPS_PRIMARYSURFACE;
			hr = g_d3dAppI.lpDD->GetAvailableVidMem(&caps, &total, &free);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpDD->GetAvailableVidMem(&caps, &total, &free)", hr);

			Logger::Log("GetVidMem : Total PRIMARY video memory is %d, free memory is %d.\n", total, free);
		}
	}

	// FUNCTION: TOY2 0x00409BB0 [MATCHED]
	char* GetTextureTypeName(int32_t type)
	{
		switch (type)
		{
			case D3DTEXTURE_TYPE_NULL:
				return "D3DTEXTURE_TYPE_NULL";
			case D3DTEXTURE_TYPE_8BIT_PALL:
				return "D3DTEXTURE_TYPE_8BIT_PALL";
			case D3DTEXTURE_TYPE_16BIT_RGB:
				return "D3DTEXTURE_TYPE_16BIT_RGB";
			case D3DTEXTURE_TYPE_24BIT_RGB:
				return "D3DTEXTURE_TYPE_24BIT_RGB";
			case D3DTEXTURE_TYPE_32BIT_RGB:
				return "D3DTEXTURE_TYPE_32BIT_RGB";
			case D3DTEXTURE_TYPE_8BIT_PLAIN:
				return "D3DTEXTURE_TYPE_8BIT_PLAIN";
			case D3DTEXTURE_TYPE_16BIT_PLAIN:
				return "D3DTEXTURE_TYPE_16BIT_PLAIN";
			default:
				return "UNKNOWN";
		}
	}

	// FUNCTION: TOY2 0x00409C10 [MATCHED]
	void FreeTPageData(int32_t tpage)
	{
		int32_t i;

		if (g_tpageData[tpage])
		{
			free(g_tpageData[tpage]);

			for (i = 0; i < D3DAPP_MAXTEXTURES; i++)
			{
				if (g_tpageData[i] == g_tpageData[tpage] && tpage != i && g_tpageData[tpage] && g_tpageData[i])
				{
					g_tpageData[i] = 0;
					g_unk8827A0[i] = 0;
				}
			}

			g_tpageData[tpage] = 0;
			g_unk8827A0[tpage] = 0;
		}
	}

	// FUNCTION: TOY2 0x00409C80 [MATCHED]
	BOOL UnkFunc409C80() { return TRUE; }

	// FUNCTION: TOY2 0x00409C90 [MATCHED]
	D3DAppTextureFormat* MapTPageTextureFormats(int16_t tpage)
	{
		int16_t i;

		for (i = 0; i < g_pcStruct.selectedInterfaceDevice->numTextureFormats; i++)
		{
			switch (g_d3dAppI.textureType[tpage])
			{
				case D3DTEXTURE_TYPE_16BIT_RGB:
				case D3DTEXTURE_TYPE_16BIT_PLAIN:
					if (g_pcStruct.selectedInterfaceDevice->textureFormat[i].rgbBPP == 16
						&& ! g_pcStruct.selectedInterfaceDevice->textureFormat[i].bAlphaPixels)
						return &g_pcStruct.selectedInterfaceDevice->textureFormat[i];
					break;
				case D3DTEXTURE_TYPE_24BIT_RGB:
					if (g_pcStruct.selectedInterfaceDevice->textureFormat[i].rgbBPP == 24
						&& ! g_pcStruct.selectedInterfaceDevice->textureFormat[i].bAlphaPixels)
						return &g_pcStruct.selectedInterfaceDevice->textureFormat[i];
					break;
				case D3DTEXTURE_TYPE_32BIT_RGB:
					if (g_pcStruct.selectedInterfaceDevice->textureFormat[i].rgbBPP == 32
						&& ! g_pcStruct.selectedInterfaceDevice->textureFormat[i].bAlphaPixels)
						return &g_pcStruct.selectedInterfaceDevice->textureFormat[i];
					break;
				case D3DTEXTURE_TYPE_8BIT_PALL:
				case D3DTEXTURE_TYPE_8BIT_PLAIN:
					if (g_pcStruct.selectedInterfaceDevice->textureFormat[i].indexBPP == 8)
						return &g_pcStruct.selectedInterfaceDevice->textureFormat[i];
					break;
			}
		}

		if (g_d3dAppI.textureType[tpage] == D3DTEXTURE_TYPE_8BIT_PALL && g_pcStruct.selectedInterfaceDevice->numTextureFormats)
		{
			Logger::Log("FAILURE to map tpage %d as a 8 bit palettized surface - searching for 16bit instead.\n", tpage);

			for (i = 0; i < g_pcStruct.selectedInterfaceDevice->numTextureFormats; i++)
			{
				if (g_pcStruct.selectedInterfaceDevice->textureFormat[i].rgbBPP == 16 && ! g_pcStruct.selectedInterfaceDevice->textureFormat[i].bAlphaPixels)
				{
					Logger::Log(
						"Found 16bit texture ARGB %dx%dx%dx%d.\n",
						g_pcStruct.selectedInterfaceDevice->textureFormat[i].alphaBPP,
						g_pcStruct.selectedInterfaceDevice->textureFormat[i].redBPP,
						g_pcStruct.selectedInterfaceDevice->textureFormat[i].greenBPP,
						g_pcStruct.selectedInterfaceDevice->textureFormat[i].blueBPP);

					return &g_pcStruct.selectedInterfaceDevice->textureFormat[i];
				}
			}

			Logger::Log("ERROR - No suitable 16bit texture format found - switching to default.\n");

			return &g_pcStruct.selectedInterfaceDevice->textureFormat[0];
		}

		return 0;
	}

	// FUNCTION: TOY2 0x0040A450
	BOOL D3DAppILoadTextureSurf(int32_t n, BOOL* bInVideo)
	{
		DDSURFACEDESC ddsd;
		LPDIRECTDRAWSURFACE3 lpSrcTextureSurf = 0;
		LPDIRECT3DTEXTURE2 lpSrcTexture = 0;
		LPDIRECTDRAWPALETTE lpDstPalette = 0;
		PALETTEENTRY ppe[256];
		DWORD pcaps;
		RECT rc;
		D3DAppTextureFormat* lpFormat;
		HRESULT hr;

		Logger::Log("TEXTURES : TPage %d is %s.\n", n, g_tpageIs8Bit[n] ? "8bit palletized format" : "16bit raw format");

		if (g_d3dAppI.lpTextureSurf[n])
		{
			g_d3dAppI.lpTextureSurf[n]->Release();
			g_d3dAppI.lpTextureSurf[n] = 0;
		}

		lpFormat = MapTPageTextureFormats(n);

		if (! lpFormat)
			return FALSE;

		lpSrcTextureSurf = D3DAppILoadSurface(g_d3dAppI.lpDD, n, &lpFormat->ddsd, DDSCAPS_SYSTEMMEMORY);

		if (! lpSrcTextureSurf)
		{
			if (lpSrcTexture)
			{
				lpSrcTexture->Release();
				lpSrcTexture = 0;
			}

			if (lpDstPalette)
			{
				lpDstPalette->Release();
				lpDstPalette = 0;
			}

			if (g_d3dAppI.lpTexture[n])
			{
				g_d3dAppI.lpTexture[n]->Release();
				g_d3dAppI.lpTexture[n] = 0;
			}

			if (g_d3dAppI.lpTextureSurf[n])
			{
				g_d3dAppI.lpTextureSurf[n]->Release();
				g_d3dAppI.lpTextureSurf[n] = 0;
			}

			return FALSE;
		}

		hr = lpSrcTextureSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&lpSrcTexture);

		if (hr < 0)
			Logger::LogDDError("lpSrcTextureSurf->QueryInterface(IID_IDirect3DTexture2, (LPVOID *) & lpSrcTexture)", hr);

		hr = D3DAppIGetSurfDesc(&ddsd, lpSrcTextureSurf);

		if (hr < 0)
			Logger::LogDDError("D3DAppIGetSurfDesc(&ddsd, lpSrcTextureSurf)", hr);

		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD;

		if (g_pcStruct.selectedInterfaceDevice->isHardwareAccelerated && ! g_unk50A690[n])
			ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
		else
			ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

		D3DAppICreateSurface(&ddsd, &g_d3dAppI.lpTextureSurf[n]);

		if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
			pcaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
		else if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
			pcaps = DDPCAPS_4BIT;
		else
			pcaps = 0;

		if (pcaps)
		{
			memset(ppe, 0, sizeof(PALETTEENTRY) * 256);
			hr = g_d3dAppI.lpDD->CreatePalette(pcaps, ppe, &lpDstPalette, 0);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpDD->CreatePalette(pcaps, ppe, &lpDstPalette, 0)", hr);

			hr = g_d3dAppI.lpTextureSurf[n]->SetPalette(lpDstPalette);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpTextureSurf[n]->SetPalette(lpDstPalette)", hr);
		}

		hr = g_d3dAppI.lpTextureSurf[n]->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&g_d3dAppI.lpTexture[n]);

		if (hr < 0)
			Logger::LogDDError("d3dappi.lpTextureSurf[n]->QueryInterface(IID_IDirect3DTexture2, (LPVOID *) & d3dappi.lpTexture[n])", hr);

		if (g_d3dAppI.textureType[n] >= D3DTEXTURE_TYPE_8BIT_PLAIN && g_d3dAppI.textureType[n] <= D3DTEXTURE_TYPE_16BIT_PLAIN)
		{
			SetRect(&rc, 0, 0, g_tpageSize[n].width, g_tpageSize[n].height);
			hr = g_d3dAppI.lpTextureSurf[n]->BltFast(0, 0, lpSrcTextureSurf, &rc, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpTextureSurf[n]->BltFast(0, 0, lpSrcTextureSurf, &rc, 0x00000000 | 0x00000010)", hr);
		}
		else
		{
			hr = g_d3dAppI.lpTexture[n]->Load(lpSrcTexture);

			if (hr < 0)
				Logger::LogDDError("d3dappi.lpTexture[n]->Load(lpSrcTexture)", hr);
		}

		if (g_d3dAppI.lpSrcTexture[n])
		{
			g_d3dAppI.lpSrcTexture[n]->Release();
			g_d3dAppI.lpSrcTexture[n] = 0;
		}

		g_d3dAppI.lpSrcTexture[n] = lpSrcTexture;

		if (g_d3dAppI.lpSrcTextureSurf[n])
		{
			g_d3dAppI.lpSrcTextureSurf[n]->Release();
			g_d3dAppI.lpSrcTextureSurf[n] = 0;
		}

		g_d3dAppI.lpSrcTextureSurf[n] = lpSrcTextureSurf;

		hr = D3DAppIGetSurfDesc(&ddsd, g_d3dAppI.lpTextureSurf[n]);

		if (hr < 0)
			Logger::LogDDError("D3DAppIGetSurfDesc(&ddsd, d3dappi.lpTextureSurf[n])", hr);

		if (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
		{
			Logger::Log("TEXTURES : TPage %d ended up in video memory.\n", n);
			*bInVideo = TRUE;
		}
		else
		{
			Logger::Log("TEXTURES : TPage %d ended up in system memory.\n", n);
			*bInVideo = FALSE;
		}

		return TRUE;
	}

	// FUNCTION: TOY2 0x0040A830 [MATCHED]
	BOOL D3DAppIGetTextureHandle(int32_t n)
	{
		HRESULT hr;

		hr = g_d3dAppI.lpTexture[n]->GetHandle(g_d3dAppI.lpD3DDevice, &g_masterTextureHandle[n]);
		g_d3dAppI.textureHandle[n] = g_masterTextureHandle[n];

		return hr == DD_OK;
	}

	// FUNCTION: TOY2 0x0040A870 [MATCHED]
	void UnkFunc40A870()
	{
		if (g_renderMode == RENDERMODE_D3D)
		{
			for (int16_t i = 0; i < D3DAPP_MAXTEXTURES; i++)
			{
				if (g_d3dAppI.textureState[i] != 0 && g_d3dAppI.textureState[i] != 4)
					g_d3dAppI.textureState[i] = 5;
			}
			UnkFunc409C80();
		}
	}

	// FUNCTION: TOY2 0x0040A8B0 [MATCHED]
	void UnkFunc40A8B0()
	{
		for (int16_t i = 0; i < D3DAPP_MAXTEXTURES; i++)
		{
			if (g_d3dAppI.textureState[i] != 0)
				g_d3dAppI.textureState[i] = 5;
		}
		UnkFunc409C80();
		memset(g_d3dAppI.textureState, 0, sizeof(g_d3dAppI.textureState));
		memset(g_d3dAppI.textureType, 0, sizeof(g_d3dAppI.textureType));
		memset(g_tpageSize, 0, sizeof(g_tpageSize));
		memset(g_d3dAppI.texturePalette, 0, sizeof(g_d3dAppI.texturePalette));
	}

	// FUNCTION: TOY2 0x0040A910 [MATCHED]
	DWORD DDColorMatch(LPDIRECTDRAWSURFACE3 pdds, COLORREF rgb)
	{
		COLORREF rgbT;
		HDC hdc;
		DWORD dw = CLR_INVALID;
		DWORD mask;
		DDSURFACEDESC ddsd;
		HRESULT hres;

		if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
		{
			rgbT = GetPixel(hdc, 0, 0);
			SetPixel(hdc, 0, 0, rgb);
			pdds->ReleaseDC(hdc);
		}

		ddsd.dwSize = sizeof(ddsd);
		while ((hres = pdds->Lock(0, &ddsd, 0, 0)) == DDERR_WASSTILLDRAWING)
			;

		if (hres == DD_OK)
		{
			dw = *(DWORD*)ddsd.lpSurface;
			mask = (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;

			if (ddsd.ddpfPixelFormat.dwRGBBitCount)
				mask |= ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;

			dw &= mask;
			pdds->Unlock(0);
		}

		if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
		{
			SetPixel(hdc, 0, 0, rgbT);
			pdds->ReleaseDC(hdc);
		}

		return dw;
	}

	// FUNCTION: TOY2 0x0040AA20 [MATCHED]
	HRESULT DDSetColorKey(LPDIRECTDRAWSURFACE3 pdds, COLORREF rgb)
	{
		DDCOLORKEY ddck;

		ddck.dwColorSpaceLowValue = DDColorMatch(pdds, rgb);
		ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;

		return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
	}

	// FUNCTION: TOY2 0x0040AA50
	LPDIRECTDRAWSURFACE3 D3DAppILoadSurface(LPDIRECTDRAW2 lpDD, int32_t n, LPDDSURFACEDESC lpFormat, DWORD memoryflag)
	{
		LPDIRECTDRAWSURFACE3 lpDDS;
		DDSURFACEDESC ddsd, format;
		DWORD dwWidth, dwHeight;
		int32_t i, j;
		uint8_t* lpData;
		char* lpC;
		LPDIRECTDRAWPALETTE lpDDPal;
		PALETTEENTRY ppe[256];
		DWORD pcaps;
		BOOL bQuant = FALSE;
		HRESULT ddrval;
		uint8_t k;

		dwWidth = g_tpageSize[n].width;
		dwHeight = g_tpageSize[n].height;
		lpData = g_tpageData[n];

		if (! lpData)
			return 0;

		memcpy(&format, lpFormat, sizeof(DDSURFACEDESC));

		if (format.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
		{
			bQuant = TRUE;
			pcaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
		}
		else if (format.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
		{
			bQuant = TRUE;
			pcaps = DDPCAPS_4BIT;
		}

		memcpy(&ddsd, lpFormat, sizeof(DDSURFACEDESC));
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | memoryflag;
		ddsd.dwHeight = dwHeight;
		ddsd.dwWidth = dwWidth;

		CreateSurface3(lpDD, &ddsd, &lpDDS, 0);

		memset(&ddsd, 0, sizeof(DDSURFACEDESC));
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		ddrval = lpDDS->Lock(0, &ddsd, 0, 0);

		if (ddrval < 0)
			Logger::LogDDError("lpDDS->Lock(0, &ddsd, 0, 0)", ddrval);

		if (! bQuant)
		{
			uint32_t* lpLP;
			uint16_t* lpSP;
			uint8_t* lpCP;
			uint32_t m;
			int32_t s;
			int32_t alphaScale;
			int32_t alpha;

			for (s = 0, m = format.ddpfPixelFormat.dwRBitMask; ! (m & 1); s++, m >>= 1)
				;
			g_d3dAppI.redShift[n] = s;
			g_d3dAppI.redScale[n] = 255 / (format.ddpfPixelFormat.dwRBitMask >> s);
			for (s = 0, m = format.ddpfPixelFormat.dwGBitMask; ! (m & 1); s++, m >>= 1)
				;
			g_d3dAppI.greenShift[n] = s;
			g_d3dAppI.greenScale[n] = 255 / (format.ddpfPixelFormat.dwGBitMask >> s);
			for (s = 0, m = format.ddpfPixelFormat.dwBBitMask; ! (m & 1); s++, m >>= 1)
				;
			g_d3dAppI.blueShift[n] = s;
			g_d3dAppI.blueScale[n] = 255 / (format.ddpfPixelFormat.dwBBitMask >> s);

			if (format.ddpfPixelFormat.dwRGBAlphaBitMask)
			{
				for (s = 0, m = format.ddpfPixelFormat.dwRGBAlphaBitMask; ! (m & 1); s++, m >>= 1)
					;
				alphaScale = 255 / (format.ddpfPixelFormat.dwRGBAlphaBitMask >> s);
				alpha = (g_textureAlpha / alphaScale) << s;
			}
			else
			{
				alpha = 0;
			}

			switch (format.ddpfPixelFormat.dwRGBBitCount)
			{
				case 32:
					for (j = 0; j < (int32_t)dwHeight; j++)
					{
						lpLP = (uint32_t*)(((char*)ddsd.lpSurface) + ddsd.lPitch * j);
						for (i = 0; i < (int32_t)dwWidth; i++)
						{
							int32_t r, g, b;
							r = *lpData++ / g_d3dAppI.redScale[n];
							g = *lpData++ / g_d3dAppI.greenScale[n];
							b = *lpData++ / g_d3dAppI.blueScale[n];
							*lpLP = (r << g_d3dAppI.redShift[n]) | (g << g_d3dAppI.greenShift[n]) | (b << g_d3dAppI.blueShift[n]) | alpha;
							lpLP++;
						}
					}
					break;
				case 16:
					switch (g_d3dAppI.textureType[n])
					{
						case D3DTEXTURE_TYPE_8BIT_PALL:
							memset(ppe, 0, sizeof(PALETTEENTRY) * 256);
							for (i = 0; i < 256; i++)
							{
								ppe[i].peRed = g_d3dAppI.texturePalette[n][i * 3];
								ppe[i].peGreen = g_d3dAppI.texturePalette[n][i * 3 + 1];
								ppe[i].peBlue = g_d3dAppI.texturePalette[n][i * 3 + 2];
							}
							for (j = 0; j < (int32_t)dwHeight; j++)
							{
								lpSP = (uint16_t*)(((char*)ddsd.lpSurface) + ddsd.lPitch * j);
								for (i = 0; i < (int32_t)dwWidth; i++)
								{
									int32_t r, g, b;
									k = *lpData++;
									r = ppe[k].peRed / g_d3dAppI.redScale[n];
									g = ppe[k].peGreen / g_d3dAppI.greenScale[n];
									b = ppe[k].peBlue / g_d3dAppI.blueScale[n];
									*lpSP = (r << g_d3dAppI.redShift[n]) | (g << g_d3dAppI.greenShift[n]) | (b << g_d3dAppI.blueShift[n]) | alpha;
									lpSP++;
								}
							}
							break;
						default:
							for (j = 0; j < (int32_t)dwHeight; j++)
							{
								lpSP = (uint16_t*)(((char*)ddsd.lpSurface) + ddsd.lPitch * j);
								for (i = 0; i < (int32_t)dwWidth; i++)
								{
									int32_t r, g, b;
									r = *lpData++ / g_d3dAppI.redScale[n];
									g = *lpData++ / g_d3dAppI.greenScale[n];
									b = *lpData++ / g_d3dAppI.blueScale[n];
									*lpSP = (r << g_d3dAppI.redShift[n]) | (g << g_d3dAppI.greenShift[n]) | (b << g_d3dAppI.blueShift[n]) | alpha;
									lpSP++;
								}
							}
							break;
					}
					break;
				case 8:
					for (j = 0; j < (int32_t)dwHeight; j++)
					{
						lpCP = (uint8_t*)(((char*)ddsd.lpSurface) + ddsd.lPitch * j);
						for (i = 0; i < (int32_t)dwWidth; i++)
						{
							int32_t r, g, b;
							r = *lpData++ / g_d3dAppI.redScale[n];
							g = *lpData++ / g_d3dAppI.greenScale[n];
							b = *lpData++ / g_d3dAppI.blueScale[n];
							*lpCP = (r << g_d3dAppI.redShift[n]) | (g << g_d3dAppI.greenShift[n]) | (b << g_d3dAppI.blueShift[n]) | alpha;
							lpCP++;
						}
					}
					break;
				default:
					ddrval = lpDDS->Unlock(0);

					if (ddrval < 0)
						Logger::LogDDError("lpDDS->Unlock(0)", ddrval);

					ddrval = lpDDS->Release();

					if (ddrval < 0)
						Logger::LogDDError("lpDDS->Release()", ddrval);

					Logger::LogErrorNotSetLn("Unknown pixel format (loadtex).");
					return 0;
			}

			ddrval = lpDDS->Unlock(0);

			if (ddrval < 0)
				Logger::LogDDError("lpDDS->Unlock(0)", ddrval);

			{
				DDCOLORKEY ckey;

				ckey.dwColorSpaceLowValue = ((0 / g_d3dAppI.redScale[n]) << g_d3dAppI.redShift[n])
					| ((255 / g_d3dAppI.greenScale[n]) << g_d3dAppI.greenShift[n]) | ((0 / g_d3dAppI.blueScale[n]) << g_d3dAppI.blueShift[n]) | alpha;
				ckey.dwColorSpaceHighValue = ckey.dwColorSpaceLowValue;
				ddrval = lpDDS->SetColorKey(DDCKEY_SRCBLT, &ckey);

				if (ddrval < 0)
					Logger::LogDDError("lpDDS->SetColorKey(0x00000008l, &ckey)", ddrval);
			}

			return lpDDS;
		}

		memset(ppe, 0, sizeof(PALETTEENTRY) * 256);
		for (i = 0; i < 256; i++)
		{
			ppe[i].peRed = g_d3dAppI.texturePalette[n][i * 3];
			ppe[i].peGreen = g_d3dAppI.texturePalette[n][i * 3 + 1];
			ppe[i].peBlue = g_d3dAppI.texturePalette[n][i * 3 + 2];
		}

		g_colorKeyIndex = -1;
		for (i = 0; i < 256; i++)
		{
			if (ppe[i].peRed == 0 && ppe[i].peGreen > 0xFA && ppe[i].peBlue == 0)
			{
				ppe[i].peGreen = 0xFF;
				g_colorKeyIndex = i;
				break;
			}
		}
		g_d3dAppI.colorKeyIndex[n] = g_colorKeyIndex;

		for (j = 0; j < (int32_t)dwHeight; j++)
		{
			lpC = ((char*)ddsd.lpSurface) + ddsd.lPitch * j;
			for (i = 0; i < (int32_t)dwWidth; i++)
			{
				k = *lpData++;
				if (ppe[k].peRed == 0 && ppe[k].peGreen > 0xFA && ppe[k].peBlue == 0)
					k = (uint8_t)g_colorKeyIndex;
				*lpC = k;
				lpC++;
			}
		}

		ddrval = lpDDS->Unlock(0);

		if (ddrval < 0)
			Logger::LogDDError("lpDDS->Unlock(0)", ddrval);

		if (g_colorKeyIndex >= 0)
		{
			DDCOLORKEY ckey;

			ckey.dwColorSpaceLowValue = g_colorKeyIndex;
			ckey.dwColorSpaceHighValue = ckey.dwColorSpaceLowValue;
			ddrval = lpDDS->SetColorKey(DDCKEY_SRCBLT, &ckey);

			if (ddrval < 0)
				Logger::LogDDError("lpDDS->SetColorKey(0x00000008l, &ckey)", ddrval);
		}

		ddrval = lpDD->CreatePalette(DDPCAPS_INITIALIZE | pcaps, ppe, &lpDDPal, 0);

		if (ddrval < 0)
			Logger::LogDDError("lpDD->CreatePalette(0x00000008l | pcaps, ppe, &lpDDPal, 0)", ddrval);

		ddrval = lpDDS->SetPalette(lpDDPal);

		if (ddrval < 0)
			Logger::LogDDError("lpDDS->SetPalette(lpDDPal)", ddrval);

		lpDDPal->Release();

		return lpDDS;
	}

	// FUNCTION: TOY2 0x00498140 [MATCHED]
	int16_t InitTextureMaterials()
	{
		D3DMATERIAL mat;
		HRESULT hr;
		int16_t i;

		if (g_renderMode != RENDERMODE_D3D)
			return 1;

		if (g_d3dAppI.lpSkyMat)
		{
			g_d3dAppI.lpSkyMat->Release();
			g_d3dAppI.lpSkyMat = 0;
		}
		hr = g_d3dAppI.lpD3D->CreateMaterial(&g_d3dAppI.lpSkyMat, 0);
		if (hr < 0)
			Logger::LogDDError("d3dappi.lpD3D->CreateMaterial(&d3dappi.lpSkyMat, 0)", hr);

		memset(&mat, 0, sizeof(mat));
		mat.dwSize = sizeof(mat);
		mat.diffuse.r = RawLoader::g_backdrop.topColor.r / 255.0f;
		mat.diffuse.g = RawLoader::g_backdrop.topColor.g / 255.0f;
		mat.diffuse.b = RawLoader::g_backdrop.topColor.b / 255.0f;
		mat.ambient.r = 0.0f;
		mat.ambient.g = 0.0f;
		mat.ambient.b = 0.0f;
		mat.specular.r = 0.0f;
		mat.specular.g = 0.0f;
		mat.specular.b = 0.0f;
		mat.power = 0.0f;
		mat.dwRampSize = 1;
		mat.emissive.r = 20.0f;
		mat.emissive.g = 20.0f;
		mat.emissive.b = 20.0f;
		g_d3dAppI.lpSkyMat->SetMaterial(&mat);
		g_d3dAppI.lpSkyMat->GetHandle(g_d3dAppI.lpD3DDevice, &g_d3dAppI.lpSkyMatHandle);

		if (g_d3dAppI.lpGroundMat)
		{
			g_d3dAppI.lpGroundMat->Release();
			g_d3dAppI.lpGroundMat = 0;
		}
		hr = g_d3dAppI.lpD3D->CreateMaterial(&g_d3dAppI.lpGroundMat, 0);
		if (hr < 0)
			Logger::LogDDError("d3dappi.lpD3D->CreateMaterial(&d3dappi.lpGroundMat, 0)", hr);

		memset(&mat, 0, sizeof(mat));
		mat.dwSize = sizeof(mat);
		mat.diffuse.r = RawLoader::g_backdrop.bottomColor.r / 255.0f;
		mat.diffuse.g = RawLoader::g_backdrop.bottomColor.g / 255.0f;
		mat.diffuse.b = RawLoader::g_backdrop.bottomColor.b / 255.0f;
		mat.ambient.r = 0.0f;
		mat.ambient.g = 0.0f;
		mat.ambient.b = 0.0f;
		mat.specular.r = 0.0f;
		mat.specular.g = 0.0f;
		mat.specular.b = 0.0f;
		mat.power = 0.0f;
		mat.dwRampSize = 1;
		mat.emissive.r = 20.0f;
		mat.emissive.g = 20.0f;
		mat.emissive.b = 20.0f;
		g_d3dAppI.lpGroundMat->SetMaterial(&mat);
		g_d3dAppI.lpGroundMat->GetHandle(g_d3dAppI.lpD3DDevice, &g_d3dAppI.lpGroundMatHandle);

		for (i = 0; i < D3DAPP_MAXTEXTURES; i++)
		{
			if (g_d3dAppI.lpTextureMat[i])
			{
				g_d3dAppI.lpTextureMat[i]->Release();
				g_d3dAppI.lpTextureMat[i] = 0;
			}
			if (g_d3dAppI.textureState[i] == 3)
			{
				if (g_d3dAppI.lpTextureMat[i])
				{
					g_d3dAppI.lpTextureMat[i]->Release();
					g_d3dAppI.lpTextureMat[i] = 0;
				}
				hr = g_d3dAppI.lpD3D->CreateMaterial(&g_d3dAppI.lpTextureMat[i], 0);
				if (hr < 0)
					Logger::LogDDError("d3dappi.lpD3D->CreateMaterial(&(d3dappi.lpTextureMat[i]), 0)", hr);

				memset(&mat, 0, sizeof(mat));
				mat.dwSize = sizeof(mat);
				mat.diffuse.r = 10.0f;
				mat.diffuse.g = 10.0f;
				mat.diffuse.b = 10.0f;
				mat.ambient.r = 10.0f;
				mat.ambient.g = 10.0f;
				mat.ambient.b = 10.0f;
				mat.specular.r = 10.0f;
				mat.specular.g = 10.0f;
				mat.specular.b = 10.0f;
				mat.emissive.r = 10.0f;
				mat.emissive.g = 10.0f;
				mat.emissive.b = 10.0f;
				mat.power = 20.0f;
				mat.hTexture = g_d3dAppI.textureHandle[i];
				mat.dwRampSize = 256;
				hr = g_d3dAppI.lpTextureMat[i]->SetMaterial(&mat);
				if (hr < 0)
					Logger::LogDDError("d3dappi.lpTextureMat[i]->SetMaterial(&mat)", hr);

				hr = g_d3dAppI.lpTextureMat[i]->GetHandle(g_d3dAppI.lpD3DDevice, &g_d3dAppI.lpTextureMatHandle[i]);
				if (hr < 0)
					Logger::LogDDError("d3dappi.lpTextureMat[i]->GetHandle(d3dappi.lpD3DDevice, &(d3dappi.lpTextureMatHandle[i]))", hr);
			}
		}
		return 1;
	}
}

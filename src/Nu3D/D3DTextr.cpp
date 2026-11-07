#include "Nu3D/D3DTextr.h"
#include "DrawingDevice.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace Nu3D
{
	extern int32_t g_isSoftwareRendering;
}

namespace D3DTextr
{
	struct TextureContainer
	{
		HBITMAP hbmBitmap;
		HBITMAP hbmAlpha;
		LPDIRECTDRAWSURFACE4 pddsSurface;
		LPDIRECT3DTEXTURE2 ptexTexture;
		DWORD dwStage;
		char strName[80];
		BOOL bHasAlpha;
		DWORD dwFlags;

		BYTE* pSurfaceBits[10];
		LPVOID pSurfaceBuffer;
		DDSURFACEDESC2 ddsd;
		DWORD dwRefCount;

		TextureContainer *pPrev, *pNext;

		~TextureContainer();
	};

	STATIC_ASSERT(sizeof(TextureContainer) == 0x120);

#define FOREACHTEXTURE(ptc) for (TextureContainer* ptc = g_ptcTextureList; NULL != ptc; ptc = ptc->pNext)
#define SAFE_DELETE(p)  \
	{                   \
		if (p)          \
		{               \
			delete (p); \
			(p) = NULL; \
		}               \
	}
#define SAFE_RELEASE(p)     \
	{                       \
		if (p)              \
		{                   \
			(p)->Release(); \
			(p) = NULL;     \
		}                   \
	}

	inline BOOL FileExists(char* n)
	{
		FILE* fp = fopen(n, "rb");
		return fp ? (0 == fclose(fp)) : FALSE;
	}

	static TextureContainer* g_ptcTextureList = NULL;
	static char g_strTexturePath[512] = "MEDIA\\";
	static char* g_strRegValueName = "DX6SDK Samples Path";

	// GLOBAL: TOY2 0x0050848C
	int32_t g_minAlphaBits = 4;

	struct TEXTURESEARCHINFO
	{
		DWORD dwDesiredBPP;
		BOOL bUseAlpha;
		BOOL bUsePalette;
		BOOL bUseFourCC;
		BOOL bFoundGoodFormat;

		DDPIXELFORMAT* pddpf;
	};

	STATIC_ASSERT(sizeof(TEXTURESEARCHINFO) == 0x18);

	static int32_t CountAlphaBits(DDPIXELFORMAT* pddpf);

	// FUNCTION: TOY2 0x004B2270 [MATCHED]
	static HRESULT CALLBACK TextureSearchCallback(DDPIXELFORMAT* pddpf, VOID* param)
	{
		if (NULL == pddpf || NULL == param)
			return DDENUMRET_OK;

		TEXTURESEARCHINFO* ptsi = (TEXTURESEARCHINFO*)param;

		if (pddpf->dwFlags & (DDPF_LUMINANCE | DDPF_BUMPLUMINANCE | DDPF_BUMPDUDV))
			return DDENUMRET_OK;

		if (ptsi->bUsePalette)
		{
			if (! (pddpf->dwFlags & DDPF_PALETTEINDEXED8))
				return DDENUMRET_OK;

			memcpy(ptsi->pddpf, pddpf, sizeof(DDPIXELFORMAT));
			ptsi->bFoundGoodFormat = TRUE;
			return DDENUMRET_CANCEL;
		}

		if (pddpf->dwRGBBitCount < 16)
			return DDENUMRET_OK;

		if (ptsi->bUseFourCC)
		{
			if (pddpf->dwFourCC == 0)
				return DDENUMRET_OK;

			return DDENUMRET_CANCEL;
		}

		if (pddpf->dwFourCC != 0)
			return DDENUMRET_OK;

		if ((ptsi->bUseAlpha == TRUE) && ! (pddpf->dwFlags & DDPF_ALPHAPIXELS))
			return DDENUMRET_OK;
		if ((ptsi->bUseAlpha == FALSE) && (pddpf->dwFlags & DDPF_ALPHAPIXELS))
			return DDENUMRET_OK;

		if (pddpf->dwRGBBitCount == ptsi->dwDesiredBPP)
		{
			if (ptsi->bUseAlpha && CountAlphaBits(pddpf) < g_minAlphaBits)
				return DDENUMRET_OK;

			memcpy(ptsi->pddpf, pddpf, sizeof(DDPIXELFORMAT));
			ptsi->bFoundGoodFormat = TRUE;
			return DDENUMRET_CANCEL;
		}

		return DDENUMRET_OK;
	}

	// FUNCTION: TOY2 0x004B2320 [MATCHED]
	static int32_t CountAlphaBits(DDPIXELFORMAT* pddpf)
	{
		DWORD dwAlphaMask = pddpf->dwRGBAlphaBitMask;
		int32_t nBits;

		for (nBits = 0; dwAlphaMask; dwAlphaMask >>= 1)
		{
			if ((dwAlphaMask & 1) != 0)
				nBits++;
		}

		return nBits;
	}

	// FUNCTION: TOY2 0x004B1890 [MATCHED]
	TextureContainer::~TextureContainer()
	{
		if (pSurfaceBuffer)
			free(pSurfaceBuffer);
		SAFE_DELETE(pNext);
		SAFE_RELEASE(ptexTexture);
		SAFE_RELEASE(pddsSurface);
		DeleteObject(hbmBitmap);
	}

	class CD3DTextureEngine
	{
	public:
		CD3DTextureEngine();
		~CD3DTextureEngine();
	};

	// GLOBAL: TOY2 0x0088446C
	CD3DTextureEngine g_StaticTextureEngine;

	// FUNCTION: TOY2 0x004B1940 [MATCHED]
	CD3DTextureEngine::CD3DTextureEngine() {}

	// FUNCTION: TOY2 0x004B1950 [MATCHED]
	CD3DTextureEngine::~CD3DTextureEngine() { SAFE_DELETE(g_ptcTextureList); }

	// FUNCTION: TOY2 0x004B19A0 [MATCHED]
	static TextureContainer* FindTexture(char* strTextureName)
	{
		FOREACHTEXTURE(ptcTexture)
		{
			if (! lstrcmpi(strTextureName, ptcTexture->strName))
				return ptcTexture;
		}

		return NULL;
	}

	// FUNCTION: TOY2 0x004B1C50 [MATCHED]
	static HRESULT FindTextureFile(char* strFilename, char* strTexturePath, char* strFullPath)
	{
		strcpy(strFullPath, strFilename);
		if (FileExists(strFullPath))
			return DD_OK;

		char* strPath = getenv("D3DPATH");
		if (NULL != strPath)
		{
			sprintf(strFullPath, "%s\\%s", strPath, strFilename);
			if (FileExists(strFullPath))
				return DD_OK;
		}

		HKEY key;
		LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\DirectX", 0, KEY_READ, &key);
		if (ERROR_SUCCESS == result)
		{
			char strPath[512];
			DWORD type, size = 512;
			result = RegQueryValueEx(key, g_strRegValueName, NULL, &type, (BYTE*)strPath, &size);

			RegCloseKey(key);

			if (ERROR_SUCCESS == result)
			{
				sprintf(strFullPath, "%s\\D3DIM\\Media\\%s", strPath, strFilename);
				if (FileExists(strFullPath))
					return DD_OK;

				sprintf(strFullPath, "%s\\%s%s", strPath, strTexturePath, strFilename);
				if (FileExists(strFullPath))
					return DD_OK;
			}
		}

		return DDERR_NOTFOUND;
	}

	// FUNCTION: TOY2 0x004B1B60 [MATCHED]
	static HRESULT LoadTextureImage(TextureContainer* ptcTexture)
	{
		char* strFilename = ptcTexture->strName;
		char* strExtension;
		char strPathname[256];

		if (NULL == (strExtension = strrchr(strFilename, '.')))
			return DDERR_UNSUPPORTED;

		if (NULL != (ptcTexture->hbmBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), strFilename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION)))
			return S_OK;

		if (FAILED(FindTextureFile(strFilename, g_strTexturePath, strPathname)))
			return DDERR_NOTFOUND;

		if (! lstrcmpi(strExtension, ".bmp"))
		{
			ptcTexture->hbmBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), strPathname, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

			if (NULL == ptcTexture->hbmBitmap)
				ptcTexture->hbmBitmap = (HBITMAP)LoadImage(NULL, strPathname, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

			return (ptcTexture->hbmBitmap) ? DD_OK : DDERR_NOTFOUND;
		}

		return DDERR_UNSUPPORTED;
	}

	// FUNCTION: TOY2 0x004B2340 [MATCHED]
	static HRESULT CopyBitmapToSurface(LPDIRECTDRAWSURFACE4 pddsTarget, HBITMAP hbmBitmap, DWORD dwFlags, HBITMAP hbmAlpha)
	{
		LPDIRECTDRAW4 pDD;
		pddsTarget->GetDDInterface((VOID**)&pDD);
		pDD->Release();

		BITMAP bm;
		GetObject(hbmBitmap, sizeof(BITMAP), &bm);

		DDSURFACEDESC2 ddsd;
		DrawingDevice::CD3DFramework::InitSurfaceDesc(&ddsd, 0, 0);
		pddsTarget->GetSurfaceDesc(&ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
		ddsd.ddsCaps.dwCaps2 = 0L;
		ddsd.dwWidth = bm.bmWidth;
		ddsd.dwHeight = bm.bmHeight;

		LPDIRECTDRAWSURFACE4 pddsTempSurface;
		HRESULT hr;
		if (FAILED(hr = pDD->CreateSurface(&ddsd, &pddsTempSurface, NULL)))
			return NULL;

		HDC hdcBitmap = CreateCompatibleDC(NULL);
		if (NULL == hdcBitmap)
		{
			pddsTempSurface->Release();
			return NULL;
		}
		SelectObject(hdcBitmap, hbmBitmap);

		if (ddsd.ddpfPixelFormat.dwRGBBitCount == 8)
		{
			DWORD pe[256];
			WORD wNumColors = GetDIBColorTable(hdcBitmap, 0, 256, (RGBQUAD*)pe);
			for (WORD i = 0; i < wNumColors; i++)
			{
				pe[i] = 0xFF000000 + RGB(GetBValue(pe[i]), GetGValue(pe[i]), GetRValue(pe[i]));

				if (dwFlags & D3DTEXTR_TRANSPARENTBLACK)
				{
					if ((pe[i] & 0x00FFFFFF) == 0x00000000)
						pe[i] &= 0x00FFFFFF;
				}
				else if (dwFlags & D3DTEXTR_TRANSPARENTWHITE)
				{
					if ((pe[i] & 0x00FFFFFF) == 0x00FFFFFF)
						pe[i] &= 0x00FFFFFF;
				}
				else if (dwFlags & D3DTEXTR_TRANSPARENTGREEN)
				{
					if ((pe[i] & 0x00FFFFFF) == 0x0000FF00)
						pe[i] &= 0x00FFFFFF;
				}
			}
			LPDIRECTDRAWPALETTE pPalette;
			if (dwFlags & (D3DTEXTR_TRANSPARENTWHITE | D3DTEXTR_TRANSPARENTBLACK))
				pDD->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALPHA, (PALETTEENTRY*)pe, &pPalette, NULL);
			else
				pDD->CreatePalette(DDPCAPS_8BIT, (PALETTEENTRY*)pe, &pPalette, NULL);
			pddsTempSurface->SetPalette(pPalette);
			pddsTarget->SetPalette(pPalette);
			SAFE_RELEASE(pPalette);
		}

		HDC hdcSurface;
		if (SUCCEEDED(pddsTempSurface->GetDC(&hdcSurface)))
		{
			BitBlt(hdcSurface, 0, 0, bm.bmWidth, bm.bmHeight, hdcBitmap, 0, 0, SRCCOPY);
			pddsTempSurface->ReleaseDC(hdcSurface);
		}
		DeleteDC(hdcBitmap);

		pddsTarget->Blt(NULL, pddsTempSurface, NULL, DDBLT_WAIT, NULL);

		if (ddsd.ddpfPixelFormat.dwRGBBitCount == 8)
		{
			LPDIRECTDRAWPALETTE pPalette;
			DWORD pe[256];
			pddsTempSurface->GetPalette(&pPalette);
			pPalette->GetEntries(0, 0, 256, (PALETTEENTRY*)&pe);
			pPalette->Release();

			pddsTarget->GetPalette(&pPalette);
			pPalette->GetEntries(0, 0, 256, (PALETTEENTRY*)&pe);
			pPalette->Release();
		}

		pddsTempSurface->Release();

		if (ddsd.ddpfPixelFormat.dwRGBAlphaBitMask)
		{
			if ((dwFlags & (D3DTEXTR_TRANSPARENTWHITE | D3DTEXTR_TRANSPARENTBLACK | D3DTEXTR_TRANSPARENTGREEN)) || hbmAlpha)
			{
				DDSURFACEDESC2 ddsd;
				DrawingDevice::CD3DFramework::InitSurfaceDesc(&ddsd, 0, 0);
				while (pddsTarget->Lock(NULL, &ddsd, 0, NULL) == DDERR_WASSTILLDRAWING)
					;

				DWORD dwAlphaMask = ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
				DWORD dwRGBMask = (ddsd.ddpfPixelFormat.dwRBitMask | ddsd.ddpfPixelFormat.dwGBitMask | ddsd.ddpfPixelFormat.dwBBitMask);
				DWORD dwColorkey = 0x00000000;
				if (dwFlags & D3DTEXTR_TRANSPARENTWHITE)
					dwColorkey = dwRGBMask;
				if (dwFlags & D3DTEXTR_TRANSPARENTGREEN)
					dwColorkey = ddsd.ddpfPixelFormat.dwGBitMask;

				if (hbmAlpha)
				{
					BITMAP bmAlpha;
					GetObject(hbmAlpha, sizeof(BITMAP), &bmAlpha);
					if (bmAlpha.bmBits)
					{
						for (DWORD y = 0; y < ddsd.dwHeight; y++)
						{
							WORD* p16 = (WORD*)((BYTE*)ddsd.lpSurface + y * ddsd.lPitch);
							DWORD* p32 = (DWORD*)((BYTE*)ddsd.lpSurface + y * ddsd.lPitch);
							BYTE* pAlpha = (BYTE*)bmAlpha.bmBits + (ddsd.dwHeight - y - 1) * bmAlpha.bmWidthBytes;
							BYTE* pSrc = (BYTE*)bm.bmBits + (ddsd.dwHeight - y - 1) * bm.bmWidthBytes;

							for (DWORD x = 0; x < ddsd.dwWidth; x++)
							{
								DWORD a = *pAlpha++;
								DWORD b = *pSrc++;
								DWORD g = *pSrc++;
								DWORD r = *pSrc++;

								if (ddsd.ddpfPixelFormat.dwRGBBitCount == 16)
								{
									*p16 = (WORD)(((a << 8) & dwAlphaMask) | ((b >> 4) & 0x0F) | ((r & 0xF0) << 4) | (g & 0xF0));
									p16++;
								}
								if (ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
								{
									*p32 |= (a << 24) & dwAlphaMask;
									p32++;
								}
							}
						}
					}
				}
				else
				{
					for (DWORD y = 0; y < ddsd.dwHeight; y++)
					{
						WORD* p16 = (WORD*)((BYTE*)ddsd.lpSurface + y * ddsd.lPitch);
						DWORD* p32 = (DWORD*)((BYTE*)ddsd.lpSurface + y * ddsd.lPitch);

						for (DWORD x = 0; x < ddsd.dwWidth; x++)
						{
							if (ddsd.ddpfPixelFormat.dwRGBBitCount == 16)
							{
								if ((*p16 &= dwRGBMask) != dwColorkey)
									*p16 |= dwAlphaMask;
								p16++;
							}
							if (ddsd.ddpfPixelFormat.dwRGBBitCount == 32)
							{
								if ((*p32 &= dwRGBMask) != dwColorkey)
									*p32 |= dwAlphaMask;
								p32++;
							}
						}
					}
				}
				pddsTarget->Unlock(NULL);
			}
		}

		return S_OK;
	}

	// FUNCTION: TOY2 0x004B1980 [MATCHED]
	LPDIRECTDRAWSURFACE4 D3DTextr_GetSurface(char* strName)
	{
		TextureContainer* ptcTexture = FindTexture(strName);

		return ptcTexture ? ptcTexture->pddsSurface : NULL;
	}

	// FUNCTION: TOY2 0x004B19E0 [MATCHED]
	LPDIRECTDRAWSURFACE4 GetContainerSurface(TextureContainer* ptcTexture) { return ptcTexture ? ptcTexture->pddsSurface : NULL; }

	// FUNCTION: TOY2 0x004B19F0 [MATCHED]
	LPDIRECT3DTEXTURE2 D3DTextr_GetTexture(char* strName)
	{
		TextureContainer* ptcTexture = FindTexture(strName);

		return ptcTexture ? ptcTexture->ptexTexture : NULL;
	}

	// FUNCTION: TOY2 0x004B1A10 [MATCHED]
	LPDIRECT3DTEXTURE2 GetContainerTexture(TextureContainer* ptcTexture) { return ptcTexture ? ptcTexture->ptexTexture : NULL; }

	// FUNCTION: TOY2 0x004B1A20 [MATCHED]
	LPVOID GetContainerBuffer(TextureContainer* ptcTexture) { return ptcTexture ? ptcTexture->pSurfaceBuffer : NULL; }

	// FUNCTION: TOY2 0x004B1A40 [MATCHED]
	void GetContainerSurfaceDesc(TextureContainer* ptcTexture, DDSURFACEDESC2** ppddsd) { *ppddsd = &ptcTexture->ddsd; }

	// FUNCTION: TOY2 0x004B1A50 [MATCHED]
	TextureContainer* FindTexture_T(char* strName) { return FindTexture(strName); }

	// FUNCTION: TOY2 0x004B1A60
	void D3DTextr_SetTexturePath(char* strTexturePath)
	{
		if (NULL == strTexturePath)
			strTexturePath = "";
		strcpy(g_strTexturePath, strTexturePath);
	}

	// FUNCTION: TOY2 0x004B1AA0 [MATCHED]
	HRESULT D3DTextr_CreateTexture(char* strName, DWORD dwStage, DWORD dwFlags)
	{
		if (NULL != FindTexture(strName))
			return S_OK;

		TextureContainer* ptcTexture = new TextureContainer();
		if (NULL == ptcTexture)
			return E_OUTOFMEMORY;
		ZeroMemory(ptcTexture, sizeof(TextureContainer));
		lstrcpy(ptcTexture->strName, strName);
		ptcTexture->dwStage = dwStage;
		ptcTexture->dwFlags = dwFlags;
		if (Nu3D::g_isSoftwareRendering)
			ptcTexture->dwFlags |= D3DTEXTR_32BITSPERPIXEL;

		if (FAILED(LoadTextureImage(ptcTexture)))
		{
			delete ptcTexture;
			return E_FAIL;
		}

		if (g_ptcTextureList)
			g_ptcTextureList->pPrev = ptcTexture;
		ptcTexture->pNext = g_ptcTextureList;
		g_ptcTextureList = ptcTexture;

		return S_OK;
	}

	// FUNCTION: TOY2 0x004B1E10 [MATCHED]
	HRESULT CreateTextureFromBitmaps(HBITMAP hbmBitmap, HBITMAP hbmAlpha, char* strName, DWORD dwStage, DWORD dwFlags)
	{
		if (NULL != FindTexture(strName))
			return S_OK;

		TextureContainer* ptcTexture = new TextureContainer();
		if (NULL == ptcTexture)
			return E_OUTOFMEMORY;
		ZeroMemory(ptcTexture, sizeof(TextureContainer));
		lstrcpy(ptcTexture->strName, strName);
		ptcTexture->dwStage = dwStage;
		ptcTexture->dwFlags = dwFlags;
		if (Nu3D::g_isSoftwareRendering)
			ptcTexture->dwFlags |= D3DTEXTR_32BITSPERPIXEL;

		ptcTexture->hbmBitmap = hbmBitmap;
		ptcTexture->hbmAlpha = hbmAlpha;
		if ((dwFlags & (D3DTEXTR_TRANSPARENTWHITE | D3DTEXTR_TRANSPARENTBLACK | D3DTEXTR_TRANSPARENTGREEN)) || hbmAlpha)
			ptcTexture->bHasAlpha = TRUE;

		if (g_ptcTextureList)
			g_ptcTextureList->pPrev = ptcTexture;
		ptcTexture->pNext = g_ptcTextureList;
		g_ptcTextureList = ptcTexture;

		return S_OK;
	}

	// FUNCTION: TOY2 0x004B1F30 [MATCHED]
	static HRESULT RestoreFromBitmap(TextureContainer* ptcTexture, LPDIRECT3DDEVICE3 pd3dDevice)
	{
		LPDIRECTDRAW4 pDD;
		if (NULL == (pDD = DrawingDevice::D3DUtil_GetDirectDrawFromDevice(pd3dDevice)))
			return E_FAIL;
		pDD->Release();

		D3DDEVICEDESC ddHwDesc, ddSwDesc;
		DWORD dwDeviceCaps;
		ddHwDesc.dwSize = sizeof(D3DDEVICEDESC);
		ddSwDesc.dwSize = sizeof(D3DDEVICEDESC);
		if (FAILED(pd3dDevice->GetCaps(&ddHwDesc, &ddSwDesc)))
			return E_FAIL;
		if (ddHwDesc.dwFlags)
			dwDeviceCaps = ddHwDesc.dpcTriCaps.dwTextureCaps;
		else
			dwDeviceCaps = ddSwDesc.dpcTriCaps.dwTextureCaps;

		BITMAP bm;
		HBITMAP hbmBitmap = ptcTexture->hbmBitmap;
		HBITMAP hbmAlpha = ptcTexture->hbmAlpha;
		GetObject(hbmBitmap, sizeof(BITMAP), &bm);
		DWORD dwWidth = (DWORD)bm.bmWidth;
		DWORD dwHeight = (DWORD)bm.bmHeight;

		DDSURFACEDESC2 ddsd;
		DrawingDevice::CD3DFramework::InitSurfaceDesc(&ddsd, 0, 0);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
		if (Nu3D::g_isSoftwareRendering)
			ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
		else
			ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
		ddsd.dwTextureStage = ptcTexture->dwStage;
		ddsd.dwWidth = dwWidth;
		ddsd.dwHeight = dwHeight;

		if (dwDeviceCaps & D3DPTEXTURECAPS_POW2)
		{
			for (ddsd.dwWidth = 1; dwWidth > ddsd.dwWidth; ddsd.dwWidth <<= 1)
				;
			for (ddsd.dwHeight = 1; dwHeight > ddsd.dwHeight; ddsd.dwHeight <<= 1)
				;
		}
		if (dwDeviceCaps & D3DPTEXTURECAPS_SQUAREONLY)
		{
			if (ddsd.dwWidth > ddsd.dwHeight)
				ddsd.dwHeight = ddsd.dwWidth;
			else
				ddsd.dwWidth = ddsd.dwHeight;
		}

		BOOL bUsePalette = (bm.bmBitsPixel <= 8);
		BOOL bUseAlpha = FALSE;

		if (ptcTexture->bHasAlpha)
		{
			bUseAlpha = TRUE;
			g_minAlphaBits = ptcTexture->hbmAlpha ? 4 : 1;
		}

		if (ptcTexture->dwFlags & (D3DTEXTR_TRANSPARENTWHITE | D3DTEXTR_TRANSPARENTBLACK | D3DTEXTR_TRANSPARENTGREEN))
		{
			if (bUsePalette)
			{
				if (dwDeviceCaps & D3DPTEXTURECAPS_ALPHAPALETTE)
				{
					bUseAlpha = TRUE;
					bUsePalette = TRUE;
				}
				else
				{
					bUseAlpha = TRUE;
					bUsePalette = FALSE;
				}
			}
		}

		TEXTURESEARCHINFO tsi;
		tsi.pddpf = &ddsd.ddpfPixelFormat;
		tsi.bUseAlpha = bUseAlpha;
		tsi.bUsePalette = bUsePalette;
		tsi.bUseFourCC = (ddsd.ddpfPixelFormat.dwFlags & DDPF_FOURCC);
		tsi.dwDesiredBPP = 16;
		tsi.bFoundGoodFormat = FALSE;
		if (ptcTexture->dwFlags & D3DTEXTR_32BITSPERPIXEL)
			tsi.dwDesiredBPP = 32;

		pd3dDevice->EnumTextureFormats(TextureSearchCallback, &tsi);

		if (FALSE == tsi.bFoundGoodFormat && bUsePalette)
		{
			tsi.bUsePalette = FALSE;
			tsi.dwDesiredBPP = 16;
			pd3dDevice->EnumTextureFormats(TextureSearchCallback, &tsi);
			if (FALSE == tsi.bFoundGoodFormat)
				return E_FAIL;
		}

		if (Nu3D::g_isSoftwareRendering)
		{
			ddsd.dwFlags |= DDSD_PITCH | DDSD_LPSURFACE;
			ddsd.lPitch = ddsd.dwWidth * 2;
			ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
			if (ptcTexture->dwFlags & D3DTEXTR_32BITSPERPIXEL)
				ddsd.lPitch <<= 1;
			ptcTexture->pSurfaceBuffer = malloc(ddsd.lPitch * ddsd.dwHeight);
			if (NULL == ptcTexture->pSurfaceBuffer)
				return E_FAIL;
			ddsd.lpSurface = ptcTexture->pSurfaceBuffer;
		}

		HRESULT hr;
		if (FAILED(hr = pDD->CreateSurface(&ddsd, &ptcTexture->pddsSurface, NULL)))
			return E_FAIL;

		if (FAILED(ptcTexture->pddsSurface->QueryInterface(IID_IDirect3DTexture2, (VOID**)&ptcTexture->ptexTexture)))
			return E_FAIL;

		ptcTexture->ddsd = ddsd;

		return CopyBitmapToSurface(ptcTexture->pddsSurface, hbmBitmap, ptcTexture->dwFlags, hbmAlpha);
	}

	// FUNCTION: TOY2 0x004B1EC0 [MATCHED]
	HRESULT D3DTextr_Restore(char* strName, LPDIRECT3DDEVICE3 pd3dDevice)
	{
		if (NULL == pd3dDevice)
			return DDERR_INVALIDPARAMS;

		TextureContainer* ptcTexture = FindTexture(strName);
		if (NULL == ptcTexture)
			return DDERR_NOTFOUND;

		SAFE_RELEASE(ptcTexture->ptexTexture);
		SAFE_RELEASE(ptcTexture->pddsSurface);

		return RestoreFromBitmap(ptcTexture, pd3dDevice);
	}

	// FUNCTION: TOY2 0x004B2880 [MATCHED]
	HRESULT RestoreContainer(TextureContainer* ptcTexture, LPDIRECT3DDEVICE3 pd3dDevice)
	{
		if (NULL == pd3dDevice)
			return DDERR_INVALIDPARAMS;

		if (NULL == ptcTexture)
			return DDERR_NOTFOUND;

		SAFE_RELEASE(ptcTexture->ptexTexture);
		SAFE_RELEASE(ptcTexture->pddsSurface);

		return RestoreFromBitmap(ptcTexture, pd3dDevice);
	}

	// FUNCTION: TOY2 0x004B28E0 [MATCHED]
	HRESULT D3DTextr_RestoreAllTextures(LPDIRECT3DDEVICE3 pd3dDevice)
	{
		FOREACHTEXTURE(ptcTexture) { D3DTextr_Restore(ptcTexture->strName, pd3dDevice); }

		return DD_OK;
	}

	// FUNCTION: TOY2 0x004B2910 [MATCHED]
	HRESULT D3DTextr_Invalidate(char* strName)
	{
		TextureContainer* ptcTexture = FindTexture(strName);
		if (NULL == ptcTexture)
			return DDERR_NOTFOUND;

		SAFE_RELEASE(ptcTexture->ptexTexture);
		SAFE_RELEASE(ptcTexture->pddsSurface);

		return DD_OK;
	}

	// FUNCTION: TOY2 0x004B2960 [MATCHED]
	HRESULT InvalidateContainer(TextureContainer* ptcTexture)
	{
		if (NULL == ptcTexture)
			return DDERR_NOTFOUND;

		SAFE_RELEASE(ptcTexture->ptexTexture);
		SAFE_RELEASE(ptcTexture->pddsSurface);

		return DD_OK;
	}

	// FUNCTION: TOY2 0x004B29A0 [MATCHED]
	HRESULT D3DTextr_InvalidateAllTextures()
	{
		FOREACHTEXTURE(ptcTexture)
		{
			SAFE_RELEASE(ptcTexture->ptexTexture);
			SAFE_RELEASE(ptcTexture->pddsSurface);
		}

		return DD_OK;
	}

	// FUNCTION: TOY2 0x004B29E0 [MATCHED]
	HRESULT DestroyContainer(TextureContainer* ptcTexture, BOOL bForce)
	{
		if (NULL == ptcTexture)
			return DDERR_NOTFOUND;

		if (ptcTexture->dwRefCount && ! bForce)
		{
			ptcTexture->dwRefCount--;
			return DD_OK;
		}

		// $BUG: the destructor frees this buffer again because the pointer is not cleared here
		if (ptcTexture->pSurfaceBuffer)
			free(ptcTexture->pSurfaceBuffer);
		if (ptcTexture->hbmBitmap)
			DeleteObject(ptcTexture->hbmBitmap);
		if (ptcTexture->hbmAlpha)
			DeleteObject(ptcTexture->hbmAlpha);

		if (ptcTexture->pPrev)
			ptcTexture->pPrev->pNext = ptcTexture->pNext;
		else
			g_ptcTextureList = ptcTexture->pNext;
		if (ptcTexture->pNext)
			ptcTexture->pNext->pPrev = ptcTexture->pPrev;
		ptcTexture->pNext = NULL;
		ptcTexture->pPrev = NULL;

		SAFE_DELETE(ptcTexture);

		return DD_OK;
	}

	// FUNCTION: TOY2 0x004B2AA0 [MATCHED]
	HRESULT DestroyContainer_T(TextureContainer* ptcTexture) { return DestroyContainer(ptcTexture, FALSE); }

	// FUNCTION: TOY2 0x004B2AB0 [MATCHED]
	HRESULT D3DTextr_DestroyTexture(char* strName)
	{
		TextureContainer* ptcTexture = FindTexture(strName);

		return DestroyContainer(ptcTexture, FALSE);
	}

	// FUNCTION: TOY2 0x004B2AD0 [MATCHED]
	void DestroyAllTextures()
	{
		TextureContainer* ptcNext;

		for (TextureContainer* ptcTexture = g_ptcTextureList; NULL != ptcTexture; ptcTexture = ptcNext)
		{
			ptcNext = ptcTexture->pNext;
			DestroyContainer(ptcTexture, TRUE);
		}
	}

	// FUNCTION: TOY2 0x004B2B00 [MATCHED]
	HRESULT AddRefContainer(TextureContainer* ptcTexture)
	{
		if (NULL == ptcTexture)
			return DDERR_NOTFOUND;

		ptcTexture->dwRefCount++;

		return DD_OK;
	}
}

#include "Toy2/D3DApp.h"
#include "Logger.h"

namespace D3DApp
{
	struct StatGlobs
	{
		HFONT hFont;
		SIZE szFrameRate;
		SIZE szInfo;
	};

	// GLOBAL: TOY2 0x0050A720
	LOGFONT g_statsLogFont;

	// GLOBAL: TOY2 0x0050A75C
	StatGlobs g_statglobs;
}

STATIC_ASSERT(sizeof(D3DApp::StatGlobs) == 0x14);

namespace D3DApp
{
	// FUNCTION: TOY2 0x0040F6F0 [MATCHED]
	void ReleaseFontAndTextBuffers(void)
	{
		if (g_windowData.unusedDDSurface2)
		{
			g_windowData.unusedDDSurface2->Release();
			g_windowData.unusedDDSurface2 = 0;
		}
		if (g_windowData.unusedDDSurface)
		{
			g_windowData.unusedDDSurface->Release();
			g_windowData.unusedDDSurface = 0;
		}
		if (g_statglobs.hFont)
		{
			DeleteObject(g_statglobs.hFont);
			g_statglobs.hFont = 0;
		}
	}

	// FUNCTION: TOY2 0x0040F740
	BOOL InitFontAndTextBuffers(void)
	{
		DDCOLORKEY ddck;
		DDSURFACEDESC ddsd;
		HDC hdc;
		char dummyinfo[] = "000x000x00 (MONO) 0000";
		char dummyfps[] = "000.00 fps 00000000.00 tps 0000.00 mppps";
		char fontName[64];

		if (g_windowData.unusedDDSurface2)
		{
			g_windowData.unusedDDSurface2->Release();
			g_windowData.unusedDDSurface2 = 0;
		}
		if (g_windowData.unusedDDSurface)
		{
			g_windowData.unusedDDSurface->Release();
			g_windowData.unusedDDSurface = 0;
		}
		if (g_statglobs.hFont)
		{
			DeleteObject(g_statglobs.hFont);
			g_statglobs.hFont = 0;
		}

		hdc = GetDC(g_windowData.mainHwnd);
		g_statglobs.hFont = 0;
		if (hdc)
		{
			LoadString(g_windowData.hInstance, 104, fontName, sizeof(fontName));
			memset(&g_statsLogFont, 0, sizeof(g_statsLogFont));
			g_statsLogFont.lfHeight = -MulDiv(7, GetDeviceCaps(hdc, LOGPIXELSY), 72);
			g_statsLogFont.lfWeight = FW_NORMAL;
			g_statsLogFont.lfItalic = FALSE;
			strcpy(g_statsLogFont.lfFaceName, fontName);
			g_statglobs.hFont = CreateFontIndirect(&g_statsLogFont);
		}

		hdc = GetDC(0);
		SelectObject(hdc, g_statglobs.hFont);
		GetTextExtentPoint(hdc, dummyfps, strlen(dummyfps), &g_statglobs.szFrameRate);
		GetTextExtentPoint(hdc, dummyinfo, strlen(dummyinfo), &g_statglobs.szInfo);
		ReleaseDC(0, hdc);

		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwHeight = g_statglobs.szFrameRate.cy;
		ddsd.dwWidth = g_statglobs.szFrameRate.cx;
		D3DAppICreateSurface(&ddsd, &g_windowData.unusedDDSurface);
		memset(&ddck, 0, sizeof(ddck));
		g_windowData.unusedDDSurface->SetColorKey(DDCKEY_SRCBLT, &ddck);

		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwHeight = g_statglobs.szInfo.cy;
		ddsd.dwWidth = g_statglobs.szInfo.cx;
		D3DAppICreateSurface(&ddsd, &g_windowData.unusedDDSurface2);
		memset(&ddck, 0, sizeof(ddck));
		g_windowData.unusedDDSurface2->SetColorKey(DDCKEY_SRCBLT, &ddck);

		return TRUE;
	}
}

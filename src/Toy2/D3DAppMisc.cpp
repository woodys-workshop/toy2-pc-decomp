#include "Toy2/D3DAppI.h"
#include "Logger.h"
#include <cstdio>

namespace Logger
{
	extern char g_errorNotSet[256];
}

namespace D3DApp
{
	// FUNCTION: TOY2 0x0040BEC0 [MATCHED]
	void D3DAppISetDefaults(void)
	{
		g_d3dAppI.bAppActive = TRUE;
		memset(&g_d3dAppRS, 0, sizeof(g_d3dAppRS));
		g_d3dAppRS.zWriteEnable = TRUE;
		g_d3dAppRS.texturePerspective = FALSE;
		g_d3dAppRS.shadeMode = D3DSHADE_GOURAUD;
		g_d3dAppRS.textureFilter = D3DFILTER_NEAREST;
		g_d3dAppRS.fillMode = D3DFILL_SOLID;
		g_d3dAppRS.ditherEnable = FALSE;
		g_d3dAppRS.specularEnable = TRUE;
		g_d3dAppRS.antiAlias = FALSE;
		g_d3dAppRS.fogEnable = FALSE;
		g_d3dAppRS.fogColor = RGB_MAKE(0, 0, 0);
		g_d3dAppRS.fogTableMode = D3DFOG_LINEAR;
		g_d3dAppRS.fogStart = D3DVAL(6.0);
		g_d3dAppRS.fogEnd = D3DVAL(11.0);

		g_lpClipper = 0;
		g_lpPalette = 0;
		g_primaryPalettized = FALSE;
		g_usesPalette = FALSE;
		g_changingCoopLevel = FALSE;
		memset(g_ppe, 0, sizeof(g_ppe));
		memset(g_originalPpe, 0, sizeof(g_originalPpe));
		g_lastError = DD_OK;
		memset(Logger::g_errorNotSet, 0, sizeof(Logger::g_errorNotSet));
		g_d3dDeviceDestroyCallback = 0;
		g_d3dDeviceDestroyCallbackContext = 0;
		g_d3dDeviceCreateCallback = 0;
		g_d3dDeviceCreateCallbackContext = 0;
	}

	// FUNCTION: TOY2 0x0040BFA0 [MATCHED]
	void D3DAppISetClientSize(HWND hwnd, int32_t w, int32_t h, BOOL bReturnFromFullscreen)
	{
		RECT rc;

		g_changingCoopLevel = TRUE;
		if (bReturnFromFullscreen)
		{
			SetRect(&rc, 0, 0, w, h);
			AdjustWindowRectEx(&rc, GetWindowLongA(hwnd, GWL_STYLE), GetMenu(hwnd) != 0, GetWindowLongA(hwnd, GWL_EXSTYLE));
			SetWindowPos(hwnd, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		}
		else
		{
			SendMessageA(hwnd, WM_SIZE, SIZE_RESTORED, (w + h) << 16);
			GetWindowRect(hwnd, &rc);
			SetWindowPos(hwnd, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		}
		g_changingCoopLevel = FALSE;
		g_d3dAppI.pClientOnPrimary.x = g_d3dAppI.pClientOnPrimary.y = 0;
		ClientToScreen(hwnd, &g_d3dAppI.pClientOnPrimary);
		g_d3dAppI.szClient.cx = w;
		g_d3dAppI.szClient.cy = h;
	}

	// FUNCTION: TOY2 0x0040C0B0 [MATCHED]
	void D3DAppIGetClientWin(HWND hwnd)
	{
		RECT rc;
		if (! g_pcStruct.fullscreenMode)
		{
			g_d3dAppI.pClientOnPrimary.x = g_d3dAppI.pClientOnPrimary.y = 0;
			ClientToScreen(hwnd, &g_d3dAppI.pClientOnPrimary);
			GetClientRect(hwnd, &rc);
			g_d3dAppI.szClient.cx = rc.right;
			g_d3dAppI.szClient.cy = rc.bottom;
		}
		else
		{
			g_d3dAppI.pClientOnPrimary.x = g_d3dAppI.pClientOnPrimary.y = 0;
			g_d3dAppI.szClient.cx = g_pcStruct.mode->width;
			g_d3dAppI.szClient.cy = g_pcStruct.mode->height;
		}
	}

	// FUNCTION: TOY2 0x0040C190 [MATCHED]
	void dpf(LPSTR fmt, ...)
	{
		char buff[256];
		va_list va;

		lstrcpyA(buff, "D3DApp: ");
		va_start(va, fmt);
		sprintf(&buff[lstrlenA(buff)], fmt, va);
		va_end(va);
		lstrcatA(buff, "\r\n");
		OutputDebugStringA(buff);
	}
}

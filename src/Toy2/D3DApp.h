#pragma once

#include "Toy2/D3DAppTypes.h"
#include "Common.h"

#include <windows.h>

namespace D3DApp
{
	extern int32_t g_no32bitColors;
	extern int32_t g_nShowCmd;

	extern HINSTANCE g_hInstance;
	extern HINSTANCE g_hPrev;

	extern char* g_lpCmdLine;

	extern D3DAppInfo g_d3dAppI;
	extern PC g_pcStruct;
	extern WindowData g_windowData;
	extern int16_t g_renderMode;

	int32_t BuildProfileMachine();
	int32_t BuildWindow();
	int32_t ProcessWndEvents();
	int32_t PostQuitMessage();

	// clang-format off
	int32_t WINAPI EnumerateDevices(LPGUID guid, LPSTR driverDesc, LPSTR driverName, LPVOID lpContext);
	HRESULT WINAPI EnumDisplayModes(LPDDSURFACEDESC surfaceDesc, LPVOID context);
	HRESULT WINAPI EnumDevices(LPGUID guid, LPSTR deviceDesc, LPSTR deviceName, LPD3DDEVICEDESC d3DHWDeviceDesc, LPD3DDEVICEDESC d3DHELDeviceDesc, LPVOID context);
	// clang-format on

	int32_t SortDisplayModes(const void* modeA, const void* modeB);

	LRESULT WINAPI ProfileWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT WINAPI NormalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void SysParmsOnExit();
}

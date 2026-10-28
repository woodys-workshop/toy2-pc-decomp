#include "ModeSelect.h"
#include "DrawingDevice.h"
#include "Logger.h"
#include "FileUtils.h"
#include "Toy2/D3DApp.h"
#include "Toy2/Toy2.h"

#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include <cstdio>
#include <windows.h>

namespace ModeSelect
{
	// GLOBAL: TOY2 0x00505590
	int32_t g_forceFullscreen = 1;

	// GLOBAL: TOY2 0x004ECC10
	int32_t g_unusedFlag1 = 1;

	// GLOBAL: TOY2 0x004ECC0C
	int32_t g_unusedFlag2 = 1;

	// GLOBAL: TOY2 0x00884034;
	DeviceFilterCallback_t g_ddDeviceFilterCallback;

	// GLOBAL: TOY2 0x00884020
	int32_t g_foundRefDevice;

	// GLOBAL: TOY2 0x00884024
	int32_t g_foundAnyD3DDevice;

	// GLOBAL: TOY2 0x0088401C
	LPDIRECTDRAW4 g_ddraw4;

	// GLOBAL: TOY2 0x0053C598
	HWND g_hWnd;

	// GLOBAL: TOY2 0x0053C59C
	HBITMAP g_backgroundBitmap;

	// GLOBAL: TOY2 0x0053C5BC
	int32_t g_lineHeight;

	// GLOBAL: TOY2 0x0053C5B8
	int32_t g_textStartY;

	// GLOBAL: TOY2 0x0053C5B4
	int32_t g_textStartX;

	// GLOBAL: TOY2 0x0053C5C0
	int32_t g_instructionLineHeight;

	// GLOBAL: TOY2 0x0053C5A4
	HFONT g_mainFont;

	// GLOBAL: TOY2 0x0053C5A8
	HFONT g_headerFont;

	// GLOBAL: TOY2 0x0053C5A0
	HFONT g_instructionFont;

	// GLOBAL: TOY2 0x0053C57C
	DrawingDevice::DDAppDevice::App* g_ddAppIterator;

	// GLOBAL: TOY2 0x0053C58C
	int32_t g_savedDriverIndex;

	// GLOBAL: TOY2 0x0053C590
	int32_t g_savedDeviceIndex;

	// GLOBAL: TOY2 0x0053C594
	int32_t g_savedModeIndex;

	// GLOBAL: TOY2 0x0053C5B0
	int32_t g_selectionState;

	// GLOBAL: TOY2 0x0053C5AC
	int32_t g_keyDown;

	// GLOBAL: TOY2 0x0053C580
	DrawingDevice::DDAppDevice::App* g_selectedDDApp;

	// GLOBAL: TOY2 0x0053C588
	DrawingDevice::DDAppDevice* g_ddAppSelectedDevice;

	// GLOBAL: TOY2 0x0053C574
	DrawingDevice::DDAppDevice::DisplayMode* g_ddAppSelectedDisplayMode;

	// GLOBAL: TOY2 0x0053C568
	HDC g_offscreenDC;

	// GLOBAL: TOY2 0x0053C564
	HGDIOBJ g_offscreenBitmap;

	// GLOBAL: TOY2 0x0053C570
	HGDIOBJ g_originalBitmap;

	// GLOBAL: TOY2 0x0053C5CC
	int32_t g_paintError;

	// GLOBAL: TOY2 0x0053C56C
	HGDIOBJ g_savedHeaderFont;

	// GLOBAL: TOY2 0x0053C584
	COLORREF g_savedTextColor;

	// GLOBAL: TOY2 0x0053C578
	int32_t g_savedBkMode;

	// GLOBAL: TOY2 0x004F574C
	int32_t g_highlightColor = 0xFFFF;

	// GLOBAL: TOY2 0x004F56FC
	const char* g_deviceSelectionInstructions[5] = {
		"Please select your preferred display", "device using the arrow keys.", "Press the space bar to confirm", "your choice.", 0,
	};

	// GLOBAL: TOY2 0x004F5710
	const char* g_renderMethodInstructions[7] = {
		"First, choose a Render Method",
		"using the up and down arrow keys.",
		"Select \"Hardware\" for 3D cards or",
		"\"Software\" for older or non-3D",
		"cards. Then press the space bar",
		"to continue.",
		0,
	};

	// GLOBAL: TOY2 0x004F572C
	const char* g_resolutionSelectionInstructions[8] = {
		"Choose a Screen Resolution using",
		"the up and down arrow keys.",
		"A higher resolution will look",
		"better but may play more slowly.",
		"Press the space bar to play the",
		"game.",
		0,
		reinterpret_cast<const char*>(5),
	};
}

namespace ModeSelect
{
	// FUNCTION: TOY2 0x004ACD90
	void MarkCompatibleBitDepthDevices()
	{
		using namespace DrawingDevice;

		LPDIRECTDRAW dd;
		if (DirectDrawCreate(0, &dd, 0) >= 0)
		{
			DDSURFACEDESC surfaceDesc;
			surfaceDesc.dwSize = sizeof(DDSURFACEDESC);

			dd->GetDisplayMode(&surfaceDesc);
			dd->Release();

			DDAppDevice::App* listHead = g_ddAppListHead;

			for (DWORD bitCount = surfaceDesc.ddpfPixelFormat.dwRGBBitCount; listHead; listHead = listHead->chainDDApp)
			{
				for (DDAppDevice* device = listHead->deviceListHead; device; device = device->nextDevice)
				{
					DDAppDevice::DisplayMode* modeIter = device->displayModeListHead;

					for (device->supportsCurrentMode = 0; modeIter; modeIter = modeIter->nextDisplayMode)
					{
						if (modeIter->surfaceDesc.ddpfPixelFormat.dwRGBBitCount == bitCount)
							device->supportsCurrentMode = 1;
					}
				}
			}
		}
	}

	// FUNCTION: TOY2 0x004ACBE0
	int32_t SelectPrimaryDevice(uint8_t selectionFlags)
	{
		using namespace DrawingDevice;

		MarkCompatibleBitDepthDevices();

		uint8_t flags = selectionFlags;

		if (! g_forceFullscreen)
		{
			flags = selectionFlags | 2;
			selectionFlags |= 2;
		}

		if ((flags & 60) != 0)
		{
			DDAppDevice::App* appIter = g_ddAppListHead;

			for (DDAppDevice::App* outerApp = g_ddAppListHead; appIter; outerApp = appIter)
			{
				DDAppDevice* deviceIter = appIter->deviceListHead;

				if (deviceIter)
				{
					while (true)
					{
						BOOL matchesDriverKind = 0;

						if (! memcmp(deviceIter->ref, &IID_IDirect3DRGBDevice, sizeof(GUID)))
						{
							if ((selectionFlags & 4) != 0)
							{
								g_primaryDDApp = outerApp;
								outerApp->primaryDevice = deviceIter;
								return 0;
							}
						}
						else if (! memcmp(deviceIter->ref, &IID_IDirect3DRefDevice, sizeof(GUID)))
						{
							if ((selectionFlags & 8) != 0)
							{
								g_primaryDDApp = outerApp;
								outerApp->primaryDevice = deviceIter;
								return 0;
							}
						}
						else
						{
							if ((selectionFlags & 16) != 0)
								matchesDriverKind = outerApp == g_ddAppListHead;

							if ((selectionFlags & 32) != 0 && outerApp != g_ddAppListHead || matchesDriverKind)
							{
								g_primaryDDApp = outerApp;
								outerApp->primaryDevice = deviceIter;
								return 0;
							}
						}

						deviceIter = deviceIter->nextDevice;

						if (! deviceIter)
						{
							appIter = outerApp;
							break;
						}
					}
				}

				appIter = appIter->chainDDApp;
			}

			return 0x81000005;
		}

		DDAppDevice::App* appIter;
		DDAppDevice* device;
		int32_t wantHardware;
		int32_t wantWindowed;
		int16_t attemptIndex = 0;

		while (true)
		{
			wantHardware = (attemptIndex & 1) == 0;
			wantWindowed = (~attemptIndex >> 1) & 1;

			if ((wantHardware != 1 || (flags & 1) == 0) && (wantWindowed != 1 || (flags & 2) == 0))
			{
				appIter = g_ddAppListHead;

				if (g_ddAppListHead)
					break;
			}

		LBL_NEXT_ATTEMPT:

			if (++attemptIndex >= 4)
			{
				if (g_foundAnyD3DDevice)
					return (g_foundRefDevice != 0) + 0x81000002;
				else
					return 0x81000001;
			}
		}

		while (true)
		{
			if (! wantWindowed || (appIter->ddCaps1.dwCaps2 & 0x80000) != 0)
			{
				device = appIter->deviceListHead;
				if (device)
					break;
			}

			appIter = appIter->chainDDApp;

			if (! appIter)
				goto LBL_NEXT_ATTEMPT;
		}

		while (wantHardware != device->isHardwareAccelerated || wantWindowed && ! device->supportsCurrentMode)
		{
			device = device->nextDevice;

			if (! device)
			{
				appIter = appIter->chainDDApp;

				if (! appIter)
					goto LBL_NEXT_ATTEMPT;
			}
		}

		device->canRenderWindowedOnPrimary = wantWindowed;

		g_primaryDDApp = appIter;
		appIter->primaryDevice = device;

		return 0;
	}

	// FUNCTION: TOY2 0x004ACFB0
	void SetForceFullscreen(int32_t forceFullscreen) { g_forceFullscreen = forceFullscreen; }

	// FUNCTION: TOY2 0x004AC390
	void SetForceFullscreen_T(int32_t forceFullscreen) { SetForceFullscreen(forceFullscreen); }

	// FUNCTION: TOY2 0x004AC9C0
	HRESULT CALLBACK EnumDisplayModes(LPDDSURFACEDESC2 lpDDSurfaceDesc2, LPVOID lpContext)
	{
		using namespace DrawingDevice;

		DDAppDevice* device = reinterpret_cast<DDAppDevice*>(lpContext);

		if (! lpDDSurfaceDesc2 || ! device)
			return D3DENUMRET_CANCEL;

		DWORD deviceRenderBitDepth = device->deviceDesc.dwDeviceRenderBitDepth;
		DWORD rgbBitCount = lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount;

		if (rgbBitCount == 32)
		{
			if ((deviceRenderBitDepth & 256) == 0)
				return D3DENUMRET_OK;

			if (D3DApp::g_no32bitColors)
				return D3DENUMRET_OK;
		}
		else if (rgbBitCount == 24)
		{
			if ((deviceRenderBitDepth & 512) == 0)
				return D3DENUMRET_OK;

			if (D3DApp::g_no32bitColors)
				return D3DENUMRET_OK;
		}
		else if (rgbBitCount == 16)
		{
			if ((deviceRenderBitDepth & 1024) == 0)
				return D3DENUMRET_OK;
		}
		else if (rgbBitCount == 8)
		{
			return D3DENUMRET_OK;
		}
		else if (rgbBitCount > 16)
		{
			if (D3DApp::g_no32bitColors)
				return D3DENUMRET_OK;
		}

		if (! device->isHardwareAccelerated)
		{
			if (rgbBitCount != 16)
				return D3DENUMRET_OK;

			if (lpDDSurfaceDesc2->dwWidth > 320)
				return D3DENUMRET_OK;
		}

		DWORD texMem;

		if (rgbBitCount == 16)
		{
			texMem = 6 * lpDDSurfaceDesc2->dwHeight * lpDDSurfaceDesc2->dwWidth;
		}
		else if (rgbBitCount == 24)
		{
			texMem = 9 * lpDDSurfaceDesc2->dwHeight * lpDDSurfaceDesc2->dwWidth;
		}
		else
		{
			if (rgbBitCount != 32)
				return D3DENUMRET_OK;

			texMem = 12 * lpDDSurfaceDesc2->dwHeight * lpDDSurfaceDesc2->dwWidth;
		}

		// $BUG: On some systems this VRAM calculation can short-circuit and cause the game
		// to not find a suitable d3d device

		if (device->isHardwareAccelerated)
		{
			DDAppDevice::App* ddAppParent = device->ddAppParent;

			if (ddAppParent->vidMemTotal == ddAppParent->totalTextureMem)
			{
				if (ddAppParent->vidMemFree - texMem < 0x80000)
					return D3DENUMRET_OK;
			}
			else if (ddAppParent->freeTextureMem < texMem)
			{
				return D3DENUMRET_OK;
			}
		}

		DDAppDevice::DisplayMode* displayMode = new DDAppDevice::DisplayMode;

		if (! displayMode)
			return D3DENUMRET_CANCEL;

		memset(displayMode, 0, sizeof(DDAppDevice::DisplayMode));
		memcpy(displayMode, lpDDSurfaceDesc2, sizeof(DDSURFACEDESC2));

		sprintf(displayMode->modeText, "%ld x %ld x %ld", lpDDSurfaceDesc2->dwWidth, lpDDSurfaceDesc2->dwHeight, rgbBitCount);

		DDAppDevice::DisplayMode* displayModeListHead = device->displayModeListHead;
		DDAppDevice::DisplayMode** nextDm;

		for (nextDm = &device->displayModeListHead; displayModeListHead; displayModeListHead = displayModeListHead->nextDisplayMode)
			nextDm = &displayModeListHead->nextDisplayMode;

		*nextDm = displayMode;

		if (lpDDSurfaceDesc2->dwWidth == 640 && lpDDSurfaceDesc2->dwHeight == 480 && lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount == 16)
			device->primaryDisplayMode = displayMode;

		if (! device->primaryDisplayMode)
			device->primaryDisplayMode = displayMode;

		return D3DENUMRET_OK;
	}

	// FUNCTION: TOY2 0x004AC760
	HRESULT CALLBACK D3DDeviceEnumCallback(
		GUID* lpGuid,
		LPSTR lpDeviceDescription,
		LPSTR lpDeviceName,
		LPD3DDEVICEDESC lpD3DHWDeviceDesc,
		LPD3DDEVICEDESC lpD3DHELDeviceDesc,
		LPVOID lpContext)
	{
		using namespace DrawingDevice;

		DDAppDevice::App* ddApp = reinterpret_cast<DDAppDevice::App*>(lpContext);

		if (! lpGuid || ! lpD3DHWDeviceDesc || ! lpD3DHELDeviceDesc || ! ddApp)
			return D3DENUMRET_CANCEL;

		if (! memcmp(lpGuid, &IID_IDirect3DNullDevice, sizeof(GUID)))
			return D3DENUMRET_OK;

		g_foundAnyD3DDevice = 1;

		int32_t flags = lpD3DHWDeviceDesc->dwFlags != 0;

		LPD3DDEVICEDESC deviceDesc = lpD3DHWDeviceDesc;

		if (! lpD3DHWDeviceDesc->dwFlags)
			deviceDesc = lpD3DHELDeviceDesc;

		if (ddApp->ref && ! lpD3DHWDeviceDesc->dwFlags)
			return D3DENUMRET_OK;

		if (g_ddDeviceFilterCallback && g_ddDeviceFilterCallback(&ddApp->ddCaps1, deviceDesc) < 0)
			return D3DENUMRET_OK;

		DDAppDevice* appDevice = new DDAppDevice;

		if (! appDevice)
			return D3DENUMRET_CANCEL;

		memset(appDevice, 0, sizeof(DDAppDevice));

		appDevice->guid = *lpGuid;
		appDevice->ref = appDevice;

		strncpy(appDevice->deviceName, lpDeviceName, sizeof(appDevice->deviceName) - 1);
		memcpy(&appDevice->deviceDesc, deviceDesc, sizeof(appDevice->deviceDesc));

		appDevice->isHardwareAccelerated = flags;
		appDevice->ddAppParent = ddApp;

		if (flags || ! ddApp->primaryDevice && (lpD3DHELDeviceDesc->dcmColorModel & 2) != 0)
			ddApp->primaryDevice = appDevice;

		g_ddraw4->EnumDisplayModes(0, 0, appDevice, EnumDisplayModes);

		DDSURFACEDESC2 surfaceDesc;
		surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
		g_ddraw4->GetDisplayMode(&surfaceDesc);

		DWORD bitCount = surfaceDesc.ddpfPixelFormat.dwRGBBitCount;

		if ((ddApp->ddCaps1.dwCaps2 & 0x80000) != 0 && ! g_forceFullscreen)
		{
			for (DDAppDevice::DisplayMode* displayMode = appDevice->displayModeListHead; displayMode; displayMode = displayMode->nextDisplayMode)
			{
				if (displayMode->surfaceDesc.ddpfPixelFormat.dwRGBBitCount == bitCount)
				{
					appDevice->supportsCurrentMode = 1;

					if (! ddApp->ref)
						appDevice->canRenderWindowedOnPrimary = 1;
				}
			}
		}

		if (appDevice->displayModeListHead)
		{
			DDAppDevice* deviceListHead = ddApp->deviceListHead;
			DDAppDevice** nextDeviceLink;

			for (nextDeviceLink = &ddApp->deviceListHead; deviceListHead; deviceListHead = deviceListHead->nextDevice)
				nextDeviceLink = &deviceListHead->nextDevice;

			*nextDeviceLink = appDevice;
		}
		else
		{
			delete appDevice;
		}

		if (! memcmp(lpGuid, &IID_IDirect3DRefDevice, sizeof(GUID)))
			g_foundRefDevice = 1;

		return D3DENUMRET_OK;
	}

	// FUNCTION: TOY2 0x004AC540
	BOOL WINAPI DDrawEnumCallbackExA(GUID* lpGUID, LPSTR lpDriverName, LPSTR lpDriverDescription, LPVOID lpContext, HMONITOR hm)
	{
		using namespace DrawingDevice;

		DDSCAPS2 ddsCaps;
		LPDIRECTDRAW directDraw;

		if (DirectDrawCreate(lpGUID, &directDraw, 0) < 0)
			return TRUE;

		if (directDraw->QueryInterface(IID_IDirectDraw4, (LPVOID*)&g_ddraw4) < 0)
		{
			directDraw->Release();
			return TRUE;
		}

		directDraw->Release();

		LPDIRECT3D3 d3d3;
		if (g_ddraw4->QueryInterface(IID_IDirect3D3, (LPVOID*)&d3d3) < 0)
		{
			g_ddraw4->Release();
			return TRUE;
		}

		DDAppDevice::App* ddApp = new DDAppDevice::App;

		if (ddApp)
		{
			memset(ddApp, 0, sizeof(DDAppDevice::App));

			if (lpGUID)
			{
				ddApp->guid = *lpGUID;
				ddApp->ref = ddApp;
			}

			strncpy(ddApp->driverName, lpDriverName, sizeof(ddApp->driverName) - 1);
			strncpy(ddApp->driverDesc, lpDriverDescription, sizeof(ddApp->driverDesc) - 1);

			ddApp->hMonitor = hm;
			ddApp->ddCaps1.dwSize = sizeof(ddApp->ddCaps1);
			ddApp->ddCaps2.dwSize = sizeof(ddApp->ddCaps2);

			g_ddraw4->GetCaps(&ddApp->ddCaps1, &ddApp->ddCaps2);

			ddApp->vidMemTotal = ddApp->ddCaps1.dwVidMemTotal;
			ddApp->vidMemFree = ddApp->ddCaps1.dwVidMemFree;

			memset(&ddsCaps.dwCaps2, 0, 12);
			ddsCaps.dwCaps = 16896;

			g_ddraw4->GetAvailableVidMem(&ddsCaps, &ddApp->totalTextureMem, &ddApp->freeTextureMem);

			memset(&ddsCaps.dwCaps2, 0, 12);
			ddsCaps.dwCaps = 4096;

			g_ddraw4->GetAvailableVidMem(&ddsCaps, &ddApp->totalVideoMem, &ddApp->freeVideoMem);
			d3d3->EnumDevices(D3DDeviceEnumCallback, ddApp);

			if (ddApp->deviceListHead)
			{
				DDAppDevice::App* chain = g_ddAppListHead;
				DDAppDevice::App** curChain;

				for (curChain = &g_ddAppListHead; chain; chain = chain->chainDDApp)
					curChain = &chain->chainDDApp;

				*curChain = ddApp;

				if (! lpGUID)
					g_primaryDDApp = ddApp;
			}
			else
			{
				delete ddApp;
			}

			d3d3->Release();
			g_ddraw4->Release();

			return TRUE;
		}

		return reinterpret_cast<BOOL>(ddApp);
	}

	// FUNCTION: TOY2 0x004ACBC0
	BOOL WINAPI DDrawEnumCallback(GUID* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext)
	{
		return DDrawEnumCallbackExA(lpGUID, lpDriverDescription, lpDriverName, 0, 0);
	}

	// FUNCTION: TOY2 0x004AC4D0
	int32_t EnumerateDrivers(DeviceFilterCallback_t callback)
	{
		typedef HRESULT(WINAPI * DirectDrawEnumerateExA_t)(LPDDENUMCALLBACKEXA lpCallback, LPVOID lpContext, DWORD dwFlags);

		g_ddDeviceFilterCallback = callback;
		g_foundRefDevice = 0;

		HMODULE moduleHandle = GetModuleHandleA("DDRAW.DLL");

		if (! moduleHandle)
			return 0x81000004;

		DirectDrawEnumerateExA_t procAddress = (DirectDrawEnumerateExA_t)GetProcAddress(moduleHandle, "DirectDrawEnumerateExA");

		if (procAddress)
			procAddress(DDrawEnumCallbackExA, 0, 7);
		else
			DirectDrawEnumerateA(DDrawEnumCallback, 0);

		return SelectPrimaryDevice(0);
	}

	// FUNCTION: TOY2 0x004AC3A0
	int32_t EnumerateDrivers_T(DeviceFilterCallback_t callback) { return EnumerateDrivers(callback); }

	// FUNCTION: TOY2 0x00412B00
	int32_t DeviceFilterCallback(LPDDCAPS caps, void* context) { return FALSE; }

	// FUNCTION: TOY2 0x00431C40
	BOOL DrawTextOutA(HDC hdc, int32_t x, int32_t y, const char* format, ...)
	{
		char buffer[512];
		va_list argList;

		va_start(argList, format);
		vsprintf(buffer, format, argList);

		return TextOutA(hdc, x, y, buffer, strlen(buffer));
	}

	// FUNCTION: TOY2 0x00433410
	COLORREF ApplyTextStyle(HDC hdc, HFONT hfont, COLORREF textColor, int32_t styleOp)
	{
		COLORREF result;

		if (styleOp == 1)
		{
			if (hfont)
				g_savedHeaderFont = SelectObject(hdc, hfont);

			g_savedTextColor = SetTextColor(hdc, textColor);
			result = SetBkMode(hdc, 1);
			g_savedBkMode = result;
		}
		else if (styleOp == 2)
		{
			if (hfont)
				SelectObject(hdc, g_savedHeaderFont);

			SetTextColor(hdc, g_savedTextColor);
			return SetBkMode(hdc, g_savedBkMode);
		}
		else
		{
			if (hfont)
				SelectObject(hdc, hfont);

			return SetTextColor(hdc, textColor);
		}

		return result;
	}

	// FUNCTION: TOY2 0x00432AD0
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg > WM_CLOSE)
		{
			if (msg != WM_QUIT)
			{
				if (msg == WM_KEYDOWN)
					g_keyDown = wParam;

				return DefWindowProcA(hWnd, msg, wParam, lParam);
			}

			Logger::GetErrorHandler("C:\\projects\\toy2\\modesel.cpp", 162)("");
			return DefWindowProcA(hWnd, msg, wParam, lParam);
		}

		switch (msg)
		{
			case WM_CLOSE:
				Logger::GetErrorHandler("C:\\projects\\toy2\\modesel.cpp", 162)("");
				return DefWindowProcA(hWnd, msg, wParam, lParam);

			case WM_CREATE:
				g_offscreenDC = 0;
				g_offscreenBitmap = 0;
				g_originalBitmap = 0;
				return DefWindowProcA(hWnd, msg, wParam, lParam);

			case WM_DESTROY:
				if (g_offscreenDC)
				{
					if (g_offscreenBitmap)
					{
						SelectObject(g_offscreenDC, g_originalBitmap);
						DeleteObject(g_offscreenBitmap);
					}

					DeleteDC(g_offscreenDC);
				}

				g_selectionState = 4;
				break;

			case WM_PAINT:
				PAINTSTRUCT paint;
				HDC paintDC = BeginPaint(hWnd, &paint);

				if (! paintDC)
				{
					g_paintError = 1;
					return DefWindowProcA(hWnd, msg, wParam, lParam);
				}

				RECT clientRect;
				GetClientRect(hWnd, &clientRect);

				if (! g_offscreenDC)
				{
					g_offscreenDC = CreateCompatibleDC(paintDC);

					if (! g_offscreenDC)
					{
						EndPaint(hWnd, &paint);
						g_paintError = 1;
						return DefWindowProcA(hWnd, msg, wParam, lParam);
					}

					g_offscreenBitmap = CreateCompatibleBitmap(paintDC, clientRect.right, clientRect.bottom);

					if (g_offscreenBitmap)
						g_originalBitmap = SelectObject(g_offscreenDC, g_offscreenBitmap);
				}

				if (g_backgroundBitmap)
				{
					HDC imageDC = CreateCompatibleDC(paintDC);

					if (imageDC)
					{
						BITMAP bitmapInfo;
						HGDIOBJ oldBitmap = SelectObject(imageDC, g_backgroundBitmap);

						GetObjectA(g_backgroundBitmap, sizeof(BITMAP), &bitmapInfo);
						StretchBlt(g_offscreenDC, 0, 0, clientRect.right, clientRect.bottom, imageDC, 0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, SRCCOPY);

						SelectObject(imageDC, oldBitmap);
						DeleteDC(imageDC);
					}
				}

				int32_t textX = g_textStartX;
				int32_t textY = g_textStartY;

				if (g_headerFont)
					g_savedHeaderFont = SelectObject(g_offscreenDC, g_headerFont);

				g_savedTextColor = SetTextColor(g_offscreenDC, 0xFFFF);
				g_savedBkMode = SetBkMode(g_offscreenDC, 1);

				if (g_ddAppIterator->chainDDApp)
				{
					if (g_headerFont)
						SelectObject(g_offscreenDC, g_headerFont);

					SetTextColor(g_offscreenDC, 0);
					DrawTextOutA(g_offscreenDC, textX - 2, textY - 2, "Device");
					DrawTextOutA(g_offscreenDC, textX + 2, textY + 2, "Device");

					if (g_headerFont)
						SelectObject(g_offscreenDC, g_headerFont);

					SetTextColor(g_offscreenDC, 0xFF);
					DrawTextOutA(g_offscreenDC, textX, textY, "Device");

					textY += 4 * g_lineHeight / 2;
				}

				if (g_headerFont)
					SelectObject(g_offscreenDC, g_headerFont);

				SetTextColor(g_offscreenDC, 0);
				DrawTextOutA(g_offscreenDC, textX - 2, textY - 2, "Render Method");
				DrawTextOutA(g_offscreenDC, textX + 2, textY + 2, "Render Method");

				if (g_headerFont)
					SelectObject(g_offscreenDC, g_headerFont);

				SetTextColor(g_offscreenDC, 0xFF);
				DrawTextOutA(g_offscreenDC, textX, textY, "Render Method");

				int32_t nextTextY = 4 * g_lineHeight / 2 + textY;

				if (g_headerFont)
					SelectObject(g_offscreenDC, g_headerFont);

				SetTextColor(g_offscreenDC, 0);
				DrawTextOutA(g_offscreenDC, textX - 2, nextTextY - 2, "Screen Resolution");
				DrawTextOutA(g_offscreenDC, textX + 2, nextTextY + 2, "Screen Resolution");

				if (g_headerFont)
					SelectObject(g_offscreenDC, g_headerFont);

				SetTextColor(g_offscreenDC, 0xFF);
				DrawTextOutA(g_offscreenDC, textX, nextTextY, "Screen Resolution");

				if (g_headerFont)
					SelectObject(g_offscreenDC, g_savedHeaderFont);

				SetTextColor(g_offscreenDC, g_savedTextColor);
				SetBkMode(g_offscreenDC, g_savedBkMode);

				if (g_mainFont)
					g_savedHeaderFont = SelectObject(g_offscreenDC, g_mainFont);

				g_savedTextColor = SetTextColor(g_offscreenDC, 0);
				g_savedBkMode = SetBkMode(g_offscreenDC, 1);

				int32_t deviceTextX = g_textStartX;
				int32_t deviceTextY = g_textStartY + g_lineHeight / 2;

				if (g_ddAppIterator->chainDDApp)
				{
					DrawTextOutA(g_offscreenDC, g_textStartX - 2, deviceTextY - 2, g_selectedDDApp->driverName);
					DrawTextOutA(g_offscreenDC, deviceTextX + 2, deviceTextY + 2, g_selectedDDApp->driverName);

					int32_t savedTextColor;

					if (g_selectionState)
						savedTextColor = 0x8888;
					else
						savedTextColor = g_highlightColor;

					if (g_mainFont)
						SelectObject(g_offscreenDC, g_mainFont);

					SetTextColor(g_offscreenDC, savedTextColor);
					DrawTextOutA(g_offscreenDC, deviceTextX, deviceTextY, g_selectedDDApp->driverName);

					deviceTextY += 4 * g_lineHeight / 2;
				}

				if (g_mainFont)
					SelectObject(g_offscreenDC, g_mainFont);

				SetTextColor(g_offscreenDC, 0);
				char* deviceName = g_ddAppSelectedDevice->deviceName;

				if (! strcmpi(g_ddAppSelectedDevice->deviceName, "rgb emulation"))
					deviceName = "Software";

				if (! strcmpi(deviceName, "direct3d hal"))
					deviceName = "Hardware";

				DrawTextOutA(g_offscreenDC, deviceTextX - 2, deviceTextY - 2, deviceName);
				DrawTextOutA(g_offscreenDC, deviceTextX + 2, deviceTextY + 2, deviceName);

				int32_t deviceHightlightColor;

				if (g_selectionState == 1)
					deviceHightlightColor = g_highlightColor;
				else
					deviceHightlightColor = 0x8888;

				if (g_mainFont)
					SelectObject(g_offscreenDC, g_mainFont);

				SetTextColor(g_offscreenDC, deviceHightlightColor);
				DrawTextOutA(g_offscreenDC, g_textStartX, deviceTextY, deviceName);

				int32_t modeTextY = 4 * g_lineHeight / 2 + deviceTextY;

				if (g_selectionState == 3)
				{
					if (g_mainFont)
						SelectObject(g_offscreenDC, g_mainFont);

					SetTextColor(g_offscreenDC, 0);

					char* selectedModeText;

					if (g_ddAppSelectedDevice->canRenderWindowedOnPrimary)
						selectedModeText = "Windowed";
					else
						selectedModeText = g_ddAppSelectedDisplayMode->modeText;

					DrawTextOutA(g_offscreenDC, g_textStartX - 2, modeTextY - 2, selectedModeText);

					if (g_ddAppSelectedDevice->canRenderWindowedOnPrimary)
						DrawTextOutA(g_offscreenDC, g_textStartX + 2, modeTextY + 2, "Windowed");
					else
						DrawTextOutA(g_offscreenDC, g_textStartX + 2, modeTextY + 2, g_ddAppSelectedDisplayMode->modeText);

					if (g_mainFont)
						SelectObject(g_offscreenDC, g_mainFont);

					SetTextColor(g_offscreenDC, 0xFF);

					char* windowedModeText;

					if (g_ddAppSelectedDevice->canRenderWindowedOnPrimary)
						windowedModeText = "Windowed";
					else
						windowedModeText = g_ddAppSelectedDisplayMode->modeText;

					DrawTextOutA(g_offscreenDC, g_textStartX, modeTextY, windowedModeText);
				}
				else
				{
					if (g_mainFont)
						SelectObject(g_offscreenDC, g_mainFont);

					SetTextColor(g_offscreenDC, 0);
					DrawTextOutA(g_offscreenDC, g_textStartX - 2, modeTextY - 2, g_ddAppSelectedDisplayMode->modeText);
					DrawTextOutA(g_offscreenDC, g_textStartX + 2, modeTextY + 2, g_ddAppSelectedDisplayMode->modeText);

					int32_t modeHighlightColor;

					if (g_selectionState == 2)
						modeHighlightColor = g_highlightColor;
					else
						modeHighlightColor = 0x8888;

					if (g_mainFont)
						SelectObject(g_offscreenDC, g_mainFont);

					SetTextColor(g_offscreenDC, modeHighlightColor);
					DrawTextOutA(g_offscreenDC, g_textStartX, modeTextY, g_ddAppSelectedDisplayMode->modeText);
				}

				int32_t instructionFirstLineY = 3 * g_lineHeight / 2 + modeTextY;
				const char** instructionLines;

				if (g_selectionState)
				{
					if (g_selectionState == 1)
					{
						instructionLines = g_renderMethodInstructions;
					}
					else
					{
						if (g_selectionState != 2)
						{
						LBL_FINISH_PAINT:

							if (g_mainFont)
								SelectObject(g_offscreenDC, g_savedHeaderFont);

							SetTextColor(g_offscreenDC, g_savedTextColor);
							SetBkMode(g_offscreenDC, g_savedBkMode);

							BitBlt(paintDC, 0, 0, clientRect.right, clientRect.bottom, g_offscreenDC, 0, 0, SRCCOPY);
							EndPaint(hWnd, &paint);

							g_paintError = 1;

							return DefWindowProcA(hWnd, msg, wParam, lParam);
						}

						instructionLines = g_resolutionSelectionInstructions;
					}
				}
				else
				{
					instructionLines = g_deviceSelectionInstructions;
				}

				if (instructionLines)
				{
					const char** shadowIter = instructionLines;

					int32_t instructionLineYIter = instructionFirstLineY;

					ApplyTextStyle(g_offscreenDC, g_instructionFont, 0, 0);

					const char* currentInstructionShadow = *instructionLines;

					if (*instructionLines)
					{
						int32_t shadowLineY = instructionFirstLineY + 2;

						do
						{
							DrawTextOutA(g_offscreenDC, g_textStartX - 2, shadowLineY - 4, currentInstructionShadow);
							DrawTextOutA(g_offscreenDC, g_textStartX + 2, shadowLineY, *shadowIter);

							currentInstructionShadow = shadowIter[1];
							++shadowIter;

							shadowLineY += g_instructionLineHeight;

						} while (currentInstructionShadow);
					}

					const char** lineIter = instructionLines;

					ApplyTextStyle(g_offscreenDC, g_instructionFont, 0xFFFFFF, 0);

					for (const char* instructionLine = *instructionLines; instructionLine;
						 instructionLineYIter = (instructionLineYIter + g_instructionLineHeight))
					{
						DrawTextOutA(g_offscreenDC, g_textStartX, instructionLineYIter, instructionLine);

						instructionLine = lineIter[1];
						++lineIter;
					}
				}

				goto LBL_FINISH_PAINT;
		}

		return DefWindowProcA(hWnd, msg, wParam, lParam);
	}

	// FUNCTION: TOY2 0x00431CA0
	int32_t SelectDDAppByIndex(int32_t index)
	{
		int32_t result = 0;
		DrawingDevice::DDAppDevice::App* iterator = g_ddAppIterator;

		for (g_selectedDDApp = g_ddAppIterator; result < index || index < 0; g_selectedDDApp = iterator)
		{
			iterator = iterator->chainDDApp;

			if (! iterator)
				break;

			++result;
		}

		if (index > result)
		{
			g_selectedDDApp = g_ddAppIterator;
			return 0;
		}

		return result;
	}

	// FUNCTION: TOY2 0x00431CE0 [MATCHED]
	int32_t SelectDeviceByIndex(int32_t index)
	{
		int32_t result = 0;

		DrawingDevice::DDAppDevice* nextDevice;
		DrawingDevice::DDAppDevice* deviceListHead = g_selectedDDApp->deviceListHead;

		for (g_ddAppSelectedDevice = deviceListHead; result < index || index < 0; g_ddAppSelectedDevice = nextDevice)
		{
			nextDevice = deviceListHead->nextDevice;

			if (! nextDevice)
				break;

			deviceListHead = deviceListHead->nextDevice;

			++result;
		}

		if (index > result)
		{
			result = 0;

			g_ddAppSelectedDevice = g_selectedDDApp->deviceListHead;
			g_selectedDDApp->primaryDevice = g_ddAppSelectedDevice;
		}
		else
		{
			g_selectedDDApp->primaryDevice = deviceListHead;
		}

		return result;
	}

	// FUNCTION: TOY2 0x00431DA0 [MATCHED]
	int32_t SelectDisplayModeByIndex(int32_t index)
	{
		int32_t result = 0;

		DrawingDevice::DDAppDevice::DisplayMode* nextDisplayMode;
		DrawingDevice::DDAppDevice::DisplayMode* displayModeListHead = g_ddAppSelectedDevice->displayModeListHead;

		for (g_ddAppSelectedDisplayMode = displayModeListHead; result < index || index < 0; g_ddAppSelectedDisplayMode = nextDisplayMode)
		{
			nextDisplayMode = displayModeListHead->nextDisplayMode;

			if (! nextDisplayMode)
				break;

			displayModeListHead = displayModeListHead->nextDisplayMode;

			++result;
		}

		if (index > result)
		{
			result = 0;
			g_ddAppSelectedDisplayMode = g_ddAppSelectedDevice->displayModeListHead;
			g_ddAppSelectedDevice->primaryDisplayMode = g_ddAppSelectedDisplayMode;
		}
		else
		{
			g_ddAppSelectedDevice->primaryDisplayMode = displayModeListHead;
		}

		return result;
	}

	// FUNCTION: TOY2 0x00431D40 [MATCHED]
	int32_t FindFirstHardwareDevice()
	{
		int32_t result = 0;

		g_ddAppSelectedDevice = g_selectedDDApp->deviceListHead;
		DrawingDevice::DDAppDevice* selectedDevice = g_ddAppSelectedDevice;

		while (selectedDevice)
		{
			if (selectedDevice->isHardwareAccelerated)
			{
				g_selectedDDApp->primaryDevice = selectedDevice;
				return result;
			}

			selectedDevice = selectedDevice->nextDevice;
			++result;
			g_ddAppSelectedDevice = selectedDevice;
		}

		g_ddAppSelectedDevice = g_selectedDDApp->deviceListHead;
		g_selectedDDApp->primaryDevice = g_ddAppSelectedDevice;
		return 0;
	}

	// FUNCTION: TOY2 0x00431E00 [MATCHED]
	int32_t SelectSuitableDisplayMode()
	{
		int32_t result = 0;

		DrawingDevice::DDAppDevice* selectedDevice = g_ddAppSelectedDevice;
		g_ddAppSelectedDisplayMode = selectedDevice->displayModeListHead;
		DrawingDevice::DDAppDevice::DisplayMode* nextDisplayMode = g_ddAppSelectedDisplayMode->nextDisplayMode;

		while (nextDisplayMode && g_ddAppSelectedDisplayMode->surfaceDesc.dwHeight < 480)
		{
			++result;
			g_ddAppSelectedDisplayMode = nextDisplayMode;
			nextDisplayMode = nextDisplayMode->nextDisplayMode;
		}

		selectedDevice->primaryDisplayMode = g_ddAppSelectedDisplayMode;

		return result;
	}

	// FUNCTION: TOY2 0x00432020
	void Run()
	{
		// $TODO: I'd like to clean/unwrap the labels in this method when I have some spare time
		using namespace DrawingDevice;

		WNDCLASSA wndClass;
		wndClass.style = CS_VREDRAW | CS_HREDRAW;
		wndClass.lpfnWndProc = WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = D3DApp::g_windowData.hInstance;
		wndClass.hIcon = LoadIconA(0, IDI_WINLOGO);
		wndClass.hCursor = LoadCursorA(0, IDC_ARROW);
		wndClass.hbrBackground = 0;
		wndClass.lpszMenuName = "";
		wndClass.lpszClassName = "Screen Mode Select";

		if (! RegisterClassA(&wndClass))
			return;

		int32_t wndExStyle = WS_EX_TOPMOST;

#ifdef APPLY_FIXES
		wndExStyle = 0;
#endif

		g_hWnd = CreateWindowExA(
			wndExStyle,
			"Screen Mode Select",
			"Screen Mode Select",
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			D3DApp::g_windowData.hInstance,
			0);

		if (! g_hWnd)
			return;

		HBITMAP createdDIBitmap = 0;

		char imagePath[512];
		FileUtils::GetPathValue(imagePath);

		strcat(imagePath, "pcbits\\rezsel.bmp");

		HANDLE loadedImage = LoadImageA(D3DApp::g_windowData.hInstance, imagePath, 0, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

		if (loadedImage)
		{
			DIBSECTION dibSection;
			GetObjectA(loadedImage, sizeof(DIBSECTION), &dibSection);
			HDC windowDC = GetDC(g_hWnd);

			if (windowDC)
			{
				HDC processedDC = CreateCompatibleDC(windowDC);

				if (processedDC)
				{
					HDC imageDC = CreateCompatibleDC(windowDC);

					if (imageDC)
					{
						createdDIBitmap = CreateDIBitmap(windowDC, &dibSection.dsBmih, 0, 0, 0, 0);

						if (createdDIBitmap)
						{
							HGDIOBJ oldProcessedBitmap = SelectObject(processedDC, createdDIBitmap);
							HGDIOBJ oldDIBitmap = SelectObject(imageDC, loadedImage);

							BitBlt(processedDC, 0, 0, dibSection.dsBmih.biWidth, dibSection.dsBmih.biHeight, imageDC, 0, 0, SRCCOPY);
							SelectObject(imageDC, oldDIBitmap);
							SelectObject(processedDC, oldProcessedBitmap);
						}

						DeleteDC(imageDC);
					}

					DeleteDC(processedDC);
				}
			}

			ReleaseDC(g_hWnd, windowDC);
			DeleteObject(loadedImage);
		}

		g_backgroundBitmap = createdDIBitmap;

		g_lineHeight = 40 * GetSystemMetrics(SM_CYFULLSCREEN) / 480;
		g_textStartY = 20 * GetSystemMetrics(SM_CYFULLSCREEN) / 480;
		g_textStartX = 20 * GetSystemMetrics(SM_CXFULLSCREEN) / 640;

		g_instructionLineHeight = 5 * g_lineHeight / 6;

		g_mainFont = CreateFontA(g_lineHeight, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "comic sans ms");
		g_headerFont = CreateFontA(g_lineHeight / 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "comic sans ms");
		g_instructionFont = CreateFontA(5 * g_lineHeight / 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "comic sans ms");

		g_ddAppIterator = DrawingDevice::GetListHead();

		int32_t suitableDisplayMode;

		if (Toy2::g_toyCfgData.driverIndex < 0)
		{
			g_savedDriverIndex = SelectDDAppByIndex(0);
			g_savedDeviceIndex = FindFirstHardwareDevice();

			suitableDisplayMode = SelectSuitableDisplayMode();
		}
		else
		{
			g_savedDriverIndex = SelectDDAppByIndex(Toy2::g_toyCfgData.driverIndex);
			g_savedDeviceIndex = SelectDeviceByIndex(Toy2::g_toyCfgData.deviceIndex);

			suitableDisplayMode = SelectDisplayModeByIndex(Toy2::g_toyCfgData.displayModeIndex);
		}

		g_savedModeIndex = suitableDisplayMode;

		ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
		UpdateWindow(g_hWnd);
		SetFocus(g_hWnd);

		g_selectionState = 0;

		while (true)
		{
			int32_t shouldExit = 0;

			MSG msg;
			if (PeekMessageA(&msg, 0, 0, 0, 0))
			{
				do
				{
					if (! GetMessageA(&msg, 0, 0, 0))
						shouldExit = 1;

					DispatchMessageA(&msg);

				} while (PeekMessageA(&msg, 0, 0, 0, 0));

				if (shouldExit)
					break;
			}

			if (g_selectionState == 4)
				break;

			int32_t shouldRedraw = 0;
			int32_t nextDeviceCount;
			DDAppDevice* nextDevice;
			DDAppDevice::App* nextApp;
			DDAppDevice::DisplayMode* nextDisplayMode;
			DDAppDevice::DisplayMode* modeIterator;
			int32_t nextModeCount;

			switch (g_selectionState)
			{
				case 0: {
					if (! g_ddAppIterator->chainDDApp)
					{
					LBL_ENTER_DEVICE_SELECT:
						g_selectionState = 1;
						shouldRedraw = 1;
						goto LBL_NEXT_ITERATION;
					}

					switch (g_keyDown)
					{
						case VK_RETURN:
						case VK_SPACE: {
							DDAppDevice* deviceListHead = g_selectedDDApp->deviceListHead;

							if (deviceListHead->nextDevice)
								goto LBL_ENTER_DEVICE_SELECT;

							int32_t restoredDeviceIndex = 0;
							DDAppDevice* restoredDevice;

							for (g_ddAppSelectedDevice = g_selectedDDApp->deviceListHead; restoredDeviceIndex < g_savedDeviceIndex || g_savedDeviceIndex < 0;
								 g_ddAppSelectedDevice = restoredDevice)
							{
								restoredDevice = deviceListHead->nextDevice;

								if (! restoredDevice)
									break;

								deviceListHead = deviceListHead->nextDevice;
								++restoredDeviceIndex;
							}

							if (g_savedDeviceIndex > restoredDeviceIndex)
							{
								deviceListHead = g_selectedDDApp->deviceListHead;
								restoredDeviceIndex = 0;
								g_ddAppSelectedDevice = deviceListHead;
							}

							g_selectedDDApp->primaryDevice = deviceListHead;
							g_savedDeviceIndex = restoredDeviceIndex;

							DDAppDevice::DisplayMode* restoredModeListHead = g_ddAppSelectedDevice->displayModeListHead;
							int32_t restoredModeIndex = 0;
							DDAppDevice::DisplayMode* restoredMode;

							for (g_ddAppSelectedDisplayMode = restoredModeListHead; restoredModeIndex < g_savedModeIndex || g_savedModeIndex < 0;
								 g_ddAppSelectedDisplayMode = restoredMode)
							{
								restoredMode = restoredModeListHead->nextDisplayMode;

								if (! restoredMode)
									break;

								restoredModeListHead = restoredModeListHead->nextDisplayMode;
								++restoredModeIndex;
							}

							if (g_savedModeIndex > restoredModeIndex)
							{
								restoredModeListHead = g_ddAppSelectedDevice->displayModeListHead;
								restoredModeIndex = 0;
								g_ddAppSelectedDisplayMode = restoredModeListHead;
							}

							g_ddAppSelectedDevice->primaryDisplayMode = restoredModeListHead;
							g_savedModeIndex = restoredModeIndex;
							g_selectionState = 2;
							shouldRedraw = 1;

							goto LBL_NEXT_ITERATION;
						}

						case VK_UP: {
							DDAppDevice::App* currentApp = g_ddAppIterator;
							int32_t deviceIndex = g_savedDriverIndex - 1;
							int32_t appIndex;

							for (appIndex = 0;; ++appIndex)
							{
								g_selectedDDApp = currentApp;

								if (appIndex >= deviceIndex && deviceIndex >= 0)
									break;

								if (! currentApp->chainDDApp)
									break;

								currentApp = currentApp->chainDDApp;
							}

							if (deviceIndex > appIndex)
							{
								currentApp = g_ddAppIterator;
								appIndex = 0;
								g_selectedDDApp = g_ddAppIterator;
							}

							g_savedDriverIndex = appIndex;
							int32_t deviceCount = 0;
							g_ddAppSelectedDevice = currentApp->deviceListHead;
							DDAppDevice* currentDevice = g_ddAppSelectedDevice;

							if (! g_ddAppSelectedDevice)
								goto LBL_UP_DEVICE_FALLBACK;

							int32_t minResolutionHeight;

							while (! currentDevice->isHardwareAccelerated)
							{
								currentDevice = currentDevice->nextDevice;
								++deviceCount;
								g_ddAppSelectedDevice = currentDevice;

								if (! currentDevice)
								{
								LBL_UP_DEVICE_FALLBACK:

									g_ddAppSelectedDevice = currentApp->deviceListHead;
									currentApp->primaryDevice = g_ddAppSelectedDevice;
									minResolutionHeight = 0;

									goto LBL_UP_DEVICE_DONE;
								}
							}

							currentApp->primaryDevice = currentDevice;
							minResolutionHeight = deviceCount;

						LBL_UP_DEVICE_DONE:

							g_savedDeviceIndex = minResolutionHeight;
							int32_t modeCount = 0;
							DDAppDevice::DisplayMode* currentMode = g_ddAppSelectedDevice->displayModeListHead;
							g_ddAppSelectedDisplayMode = currentMode;

							for (DDAppDevice::DisplayMode* nextMode = currentMode->nextDisplayMode; nextMode; nextMode = nextMode->nextDisplayMode)
							{
								if (currentMode->surfaceDesc.dwHeight >= 480)
									break;

								currentMode = nextMode;
								++modeCount;
								g_ddAppSelectedDisplayMode = nextMode;
							}

							g_ddAppSelectedDevice->primaryDisplayMode = currentMode;
							g_savedModeIndex = modeCount;
							shouldRedraw = 1;

							goto LBL_NEXT_ITERATION;
						}

						case VK_DOWN: {
							int32_t nextAppIndex = 0;
							int32_t nextDeviceIndex = g_savedDriverIndex + 1;

							for (nextApp = g_ddAppIterator;; nextApp = nextApp->chainDDApp)
							{
								g_selectedDDApp = nextApp;

								if (nextAppIndex >= nextDeviceIndex && nextDeviceIndex >= 0)
									break;

								if (! nextApp->chainDDApp)
									break;

								++nextAppIndex;
							}

							if (nextDeviceIndex > nextAppIndex)
							{
								nextApp = g_ddAppIterator;
								nextAppIndex = 0;
								g_selectedDDApp = g_ddAppIterator;
							}

							g_savedDriverIndex = nextAppIndex;
							nextDeviceCount = 0;
							g_ddAppSelectedDevice = nextApp->deviceListHead;
							nextDevice = g_ddAppSelectedDevice;

							if (! g_ddAppSelectedDevice)
								goto LBL_SCAN_DEVICE_FALLBACK;

							break;
						}

						default:
							goto LBL_NEXT_ITERATION;
					}

					break;
				}
				case 1: {
					DDAppDevice* fallbackDevice = g_selectedDDApp->deviceListHead;

					if (fallbackDevice->nextDevice)
					{
						switch (g_keyDown)
						{
							case VK_BACK:
								goto LBL_BACK_TO_DRIVER_SELECT;

							case VK_RETURN:
							case VK_SPACE:
								g_selectionState = 2;
								shouldRedraw = 1;
								goto LBL_NEXT_ITERATION;

							case VK_UP: {
								int32_t fallbackModeIndex = 0;
								int32_t upArrowDeviceIndex = g_savedDeviceIndex - 1;

								DDAppDevice* upArrowDevice;

								while (true)
								{
									upArrowDevice = fallbackDevice;
									g_ddAppSelectedDevice = fallbackDevice;

									if (fallbackModeIndex >= upArrowDeviceIndex && upArrowDeviceIndex >= 0)
										break;

									fallbackDevice = fallbackDevice->nextDevice;

									if (! fallbackDevice)
										break;

									++fallbackModeIndex;
								}

								if (upArrowDeviceIndex > fallbackModeIndex)
								{
									upArrowDevice = g_selectedDDApp->deviceListHead;
									fallbackModeIndex = 0;
									g_ddAppSelectedDevice = upArrowDevice;
								}

								g_selectedDDApp->primaryDevice = upArrowDevice;
								g_savedDeviceIndex = fallbackModeIndex;

								int32_t upArrowModeCount = 0;

								DDAppDevice::DisplayMode* upArrowMode = g_ddAppSelectedDevice->displayModeListHead;
								g_ddAppSelectedDisplayMode = upArrowMode;

								for (DDAppDevice::DisplayMode* upArrowModeIter = upArrowMode->nextDisplayMode; upArrowModeIter;
									 upArrowModeIter = upArrowModeIter->nextDisplayMode)
								{
									if (upArrowMode->surfaceDesc.dwHeight >= 480)
										break;

									upArrowMode = upArrowModeIter;
									++upArrowModeCount;
									g_ddAppSelectedDisplayMode = upArrowModeIter;
								}

								g_ddAppSelectedDevice->primaryDisplayMode = upArrowMode;
								g_savedModeIndex = upArrowModeCount;
								shouldRedraw = 1;

								goto LBL_NEXT_ITERATION;
							}

							case VK_DOWN: {
								DDAppDevice* downArrowDevice;

								int32_t downArrowIndex;
								int32_t downArrowDeviceIndex = g_savedDeviceIndex + 1;

								for (downArrowIndex = 0;; ++downArrowIndex)
								{
									downArrowDevice = fallbackDevice;
									g_ddAppSelectedDevice = fallbackDevice;

									if (downArrowIndex >= downArrowDeviceIndex && downArrowDeviceIndex >= 0)
										break;

									fallbackDevice = fallbackDevice->nextDevice;

									if (! fallbackDevice)
										break;
								}

								if (downArrowDeviceIndex > downArrowIndex)
								{
									downArrowDevice = g_selectedDDApp->deviceListHead;
									downArrowIndex = 0;
									g_ddAppSelectedDevice = downArrowDevice;
								}

								g_selectedDDApp->primaryDevice = downArrowDevice;
								g_savedDeviceIndex = downArrowIndex;
								g_savedModeIndex = SelectSuitableDisplayMode();
								shouldRedraw = 1;

								goto LBL_NEXT_ITERATION;
							}

							default:
								goto LBL_NEXT_ITERATION;
						}

						goto LBL_NEXT_ITERATION;
					}

					DDAppDevice* selectedDevice = g_selectedDDApp->deviceListHead;
					int32_t fallbackDeviceIndex = 0;

					DDAppDevice* fallbackDeviceIter;

					for (g_ddAppSelectedDevice = selectedDevice; fallbackDeviceIndex < g_savedDeviceIndex || g_savedDeviceIndex < 0;
						 g_ddAppSelectedDevice = fallbackDeviceIter)
					{
						fallbackDeviceIter = selectedDevice->nextDevice;

						if (! fallbackDeviceIter)
							break;

						selectedDevice = selectedDevice->nextDevice;
						++fallbackDeviceIndex;
					}

					if (g_savedDeviceIndex > fallbackDeviceIndex)
					{
						selectedDevice = g_selectedDDApp->deviceListHead;
						fallbackDeviceIndex = 0;
						g_ddAppSelectedDevice = selectedDevice;
					}

					g_selectedDDApp->primaryDevice = selectedDevice;
					g_savedDeviceIndex = fallbackDeviceIndex;

					DDAppDevice::DisplayMode* selectedModeListHead = g_ddAppSelectedDevice->displayModeListHead;
					int32_t restoredModeIndex = 0;

					DDAppDevice::DisplayMode* fallbackMode;

					for (g_ddAppSelectedDisplayMode = selectedModeListHead; restoredModeIndex < g_savedModeIndex || g_savedModeIndex < 0;
						 g_ddAppSelectedDisplayMode = fallbackMode)
					{
						fallbackMode = selectedModeListHead->nextDisplayMode;

						if (! fallbackMode)
							break;

						selectedModeListHead = selectedModeListHead->nextDisplayMode;
						++restoredModeIndex;
					}

					if (g_savedModeIndex > restoredModeIndex)
					{
						selectedModeListHead = g_ddAppSelectedDevice->displayModeListHead;
						restoredModeIndex = 0;

						g_ddAppSelectedDisplayMode = selectedModeListHead;
					}

					g_ddAppSelectedDevice->primaryDisplayMode = selectedModeListHead;
					g_savedModeIndex = restoredModeIndex;
					g_selectionState = 2;
					shouldRedraw = 1;

					goto LBL_NEXT_ITERATION;
				}

				case 2: {
					switch (g_keyDown)
					{
						case VK_BACK:
							if (g_selectedDDApp->deviceListHead->nextDevice)
								goto LBL_ENTER_DEVICE_SELECT;

						LBL_BACK_TO_DRIVER_SELECT:

							if (! g_ddAppIterator->chainDDApp)
								goto LBL_NEXT_ITERATION;

							g_selectionState = 0;
							break;

						case VK_RETURN:
						case VK_SPACE:
							goto LBL_COMMIT_EXIT;

						case VK_UP:
							g_savedModeIndex = SelectDisplayModeByIndex(g_savedModeIndex - 1);
							shouldRedraw = 1;

							goto LBL_NEXT_ITERATION;

						case VK_DOWN:
							g_savedModeIndex = SelectDisplayModeByIndex(g_savedModeIndex + 1);
							shouldRedraw = 1;

							goto LBL_NEXT_ITERATION;

						default:
							goto LBL_NEXT_ITERATION;
					}

					shouldRedraw = 1;
					goto LBL_NEXT_ITERATION;
				}

				case 3: {
					switch (g_keyDown)
					{
						case VK_BACK:
							g_selectionState = 2;
							shouldRedraw = 1;
							goto LBL_NEXT_ITERATION;

						case VK_RETURN:
						case VK_SPACE:
						LBL_COMMIT_EXIT:
							g_selectionState = 4;
							shouldRedraw = 1;
							goto LBL_NEXT_ITERATION;

						case VK_UP:
						case VK_DOWN:
							g_ddAppSelectedDevice->canRenderWindowedOnPrimary = 1 - g_ddAppSelectedDevice->canRenderWindowedOnPrimary;
							shouldRedraw = 1;
							goto LBL_NEXT_ITERATION;

						default:
							goto LBL_NEXT_ITERATION;
					}

					goto LBL_NEXT_ITERATION;
				}

				default:
					goto LBL_NEXT_ITERATION;
			}

			int32_t nextMinResHeight;

			while (! nextDevice->isHardwareAccelerated)
			{
				nextDevice = nextDevice->nextDevice;
				++nextDeviceCount;
				g_ddAppSelectedDevice = nextDevice;

				if (! nextDevice)
				{
				LBL_SCAN_DEVICE_FALLBACK:
					g_ddAppSelectedDevice = nextApp->deviceListHead;
					nextApp->primaryDevice = g_ddAppSelectedDevice;

					nextMinResHeight = 0;
					goto LBL_SCAN_DEVICE_DONE;
				}
			}

			nextApp->primaryDevice = nextDevice;
			nextMinResHeight = nextDeviceCount;
		LBL_SCAN_DEVICE_DONE:
			g_savedDeviceIndex = nextMinResHeight;

			nextModeCount = 0;

			nextDisplayMode = g_ddAppSelectedDevice->displayModeListHead;
			g_ddAppSelectedDisplayMode = nextDisplayMode;

			for (modeIterator = nextDisplayMode->nextDisplayMode; modeIterator; modeIterator = modeIterator->nextDisplayMode)
			{
				if (nextDisplayMode->surfaceDesc.dwHeight >= 480)
					break;

				nextDisplayMode = modeIterator;
				++nextModeCount;
				g_ddAppSelectedDisplayMode = modeIterator;
			}

			g_ddAppSelectedDevice->primaryDisplayMode = nextDisplayMode;
			g_savedModeIndex = nextModeCount;
			shouldRedraw = 1;
		LBL_NEXT_ITERATION:

			g_keyDown = 0;
			if (shouldRedraw)
				InvalidateRect(g_hWnd, 0, 0);
		}

		if (g_instructionFont)
			DeleteObject(g_instructionFont);

		if (g_headerFont)
			DeleteObject(g_headerFont);

		if (g_mainFont)
			DeleteObject(g_mainFont);

		if (g_backgroundBitmap)
			DeleteObject(g_backgroundBitmap);

		DestroyWindow(g_hWnd);

		g_primaryDDApp = g_selectedDDApp;

		Toy2::g_toyCfgData.driverIndex = g_savedDriverIndex;
		Toy2::g_toyCfgData.deviceIndex = g_savedDeviceIndex;
		Toy2::g_toyCfgData.displayModeIndex = g_savedModeIndex;

		g_ddAppSelectedDevice->canRenderWindowedOnPrimary = 0;
	}

	// FUNCTION: TOY2 0x004334B0
	void Show()
	{
		ShowCursor(0);
		Run();
		ShowCursor(1);
	}
}

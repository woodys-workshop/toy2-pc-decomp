#include "ModeSelect.h"
#include "DrawingDevice.h"
#include "Logger.h"
#include "FileUtils.h"
#include "UserDlg.h"
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

	// FUNCTION: TOY2 0x004ACBE0 [MATCHED]
	HRESULT SelectPrimaryDevice(DWORD selectionFlags)
	{
		using namespace DrawingDevice;

		MarkCompatibleBitDepthDevices();

		if (! g_forceFullscreen)
			selectionFlags |= 2;

		if (selectionFlags & 0x3C)
		{
			for (DDAppDevice::App* pDriver = g_ddAppListHead; pDriver; pDriver = pDriver->chainDDApp)
			{
				for (DDAppDevice* pDevice = pDriver->deviceListHead; pDevice; pDevice = pDevice->nextDevice)
				{
					BOOL bFound = FALSE;

					if (IsEqualGUID(*pDevice->pGUID, IID_IDirect3DRGBDevice))
					{
						if (selectionFlags & 4)
							bFound = TRUE;
					}
					else if (IsEqualGUID(*pDevice->pGUID, IID_IDirect3DRefDevice))
					{
						if (selectionFlags & 8)
							bFound = TRUE;
					}
					else
					{
						if (selectionFlags & 16)
							if (pDriver == g_ddAppListHead)
								bFound = TRUE;
						if (selectionFlags & 32)
							if (pDriver != g_ddAppListHead)
								bFound = TRUE;
					}

					if (bFound)
					{
						g_primaryDDApp = pDriver;
						g_primaryDDApp->primaryDevice = pDevice;
						return S_OK;
					}
				}
			}
			return 0x81000005;
		}

		for (WORD pass = 0; pass < 4; pass++)
		{
			BOOL bSeekHardware = (pass & 0x1) ? FALSE : TRUE;
			BOOL bSeekWindowed = (pass & 0x2) ? FALSE : TRUE;

			if ((TRUE == bSeekHardware) && (selectionFlags & 1))
				continue;
			if ((TRUE == bSeekWindowed) && (selectionFlags & 2))
				continue;

			for (DDAppDevice::App* pDriver = g_ddAppListHead; pDriver; pDriver = pDriver->chainDDApp)
			{
				DDCAPS* pCaps = &pDriver->ddDriverCaps;

				if (bSeekWindowed)
					if (0 == (pCaps->dwCaps2 & DDCAPS2_CANRENDERWINDOWED))
						continue;

				for (DDAppDevice* pDevice = pDriver->deviceListHead; pDevice; pDevice = pDevice->nextDevice)
				{
					if (bSeekHardware != pDevice->isHardwareAccelerated)
						continue;
					if (bSeekWindowed && FALSE == pDevice->supportsCurrentMode)
						continue;

					pDevice->canRenderWindowedOnPrimary = bSeekWindowed;
					g_primaryDDApp = pDriver;
					g_primaryDDApp->primaryDevice = pDevice;

					return S_OK;
				}
			}
		}

		if (FALSE == g_foundAnyD3DDevice)
			return 0x81000001;
		if (FALSE == g_foundRefDevice)
			return 0x81000002;

		return 0x81000003;
	}

	// FUNCTION: TOY2 0x004AC3C0 [MATCHED]
	HRESULT SelectPrimaryDevice_T(DWORD selectionFlags) { return SelectPrimaryDevice(selectionFlags); }

	// FUNCTION: TOY2 0x004ACFB0
	void SetForceFullscreen(int32_t forceFullscreen) { g_forceFullscreen = forceFullscreen; }

	// FUNCTION: TOY2 0x004AC390
	void SetForceFullscreen_T(int32_t forceFullscreen) { SetForceFullscreen(forceFullscreen); }

	// FUNCTION: TOY2 0x004AC9C0 [MATCHED]
	HRESULT CALLBACK EnumDisplayModes(LPDDSURFACEDESC2 lpDDSurfaceDesc2, LPVOID lpContext)
	{
		using namespace DrawingDevice;

		if (NULL == lpDDSurfaceDesc2 || NULL == lpContext)
			return DDENUMRET_CANCEL;

		DDAppDevice* pDevice = (DDAppDevice*)lpContext;
		DDAppDevice::DisplayMode* pNewMode;
		DWORD dwBitDepth = pDevice->deviceDesc.dwDeviceRenderBitDepth;
		DWORD dwModeDepth = lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount;

		if ((32 == dwModeDepth) && (0 == (dwBitDepth & DDBD_32)))
			return DDENUMRET_OK;
		if ((24 == dwModeDepth) && (0 == (dwBitDepth & DDBD_24)))
			return DDENUMRET_OK;
		if ((16 == dwModeDepth) && (0 == (dwBitDepth & DDBD_16)))
			return DDENUMRET_OK;
		if (8 == dwModeDepth)
			return DDENUMRET_OK;
		if ((dwModeDepth > 16) && D3DApp::g_no32bitColors)
			return DDENUMRET_OK;

		if (FALSE == pDevice->isHardwareAccelerated)
		{
			if (16 != dwModeDepth)
				return DDENUMRET_OK;
			if (lpDDSurfaceDesc2->dwWidth > 320)
				return DDENUMRET_OK;
		}

		DWORD dwVidMemNeeded;

		switch (dwModeDepth)
		{
			case 16:
				dwVidMemNeeded = lpDDSurfaceDesc2->dwWidth * lpDDSurfaceDesc2->dwHeight * 6;
				break;
			case 24:
				dwVidMemNeeded = lpDDSurfaceDesc2->dwWidth * lpDDSurfaceDesc2->dwHeight * 9;
				break;
			case 32:
				dwVidMemNeeded = lpDDSurfaceDesc2->dwWidth * lpDDSurfaceDesc2->dwHeight * 12;
				break;
			default:
				return DDENUMRET_OK;
		}

		// $BUG: On some systems this VRAM calculation can short-circuit and cause the game
		// to not find a suitable d3d device

		if (pDevice->isHardwareAccelerated)
		{
			DDAppDevice::App* pDriver = pDevice->ddAppParent;

			if (pDriver->vidMemTotal != pDriver->totalPrimaryMem)
			{
				if ((int32_t)pDriver->freePrimaryMem < (int32_t)dwVidMemNeeded)
					return DDENUMRET_OK;
			}
			else
			{
				if ((int32_t)(pDriver->vidMemFree - dwVidMemNeeded) < 0x80000)
					return DDENUMRET_OK;
			}
		}

		if (NULL == (pNewMode = new DDAppDevice::DisplayMode))
			return DDENUMRET_CANCEL;

		memset(pNewMode, 0, sizeof(DDAppDevice::DisplayMode));
		memcpy(&pNewMode->surfaceDesc, lpDDSurfaceDesc2, sizeof(DDSURFACEDESC2));
		sprintf(pNewMode->modeText, "%ld x %ld x %ld", lpDDSurfaceDesc2->dwWidth, lpDDSurfaceDesc2->dwHeight, dwModeDepth);

		DDAppDevice::DisplayMode** pMode = &pDevice->displayModeListHead;
		while (*pMode)
			pMode = &((*pMode)->nextDisplayMode);
		(*pMode) = pNewMode;

		if ((640 == lpDDSurfaceDesc2->dwWidth) && (480 == lpDDSurfaceDesc2->dwHeight) && (16 == lpDDSurfaceDesc2->ddpfPixelFormat.dwRGBBitCount))
			pDevice->primaryDisplayMode = pNewMode;

		if (NULL == pDevice->primaryDisplayMode)
			pDevice->primaryDisplayMode = pNewMode;

		return DDENUMRET_OK;
	}

	// FUNCTION: TOY2 0x004AC760 [MATCHED]
	HRESULT CALLBACK D3DDeviceEnumCallback(
		GUID* lpGuid,
		LPSTR lpDeviceDescription,
		LPSTR lpDeviceName,
		LPD3DDEVICEDESC lpD3DHWDeviceDesc,
		LPD3DDEVICEDESC lpD3DHELDeviceDesc,
		LPVOID lpContext)
	{
		using namespace DrawingDevice;

		DDAppDevice::App* pDriver = (DDAppDevice::App*)lpContext;
		DDAppDevice* pNewDevice;

		if (NULL == lpGuid || NULL == lpD3DHWDeviceDesc || NULL == lpD3DHELDeviceDesc || NULL == pDriver)
			return D3DENUMRET_CANCEL;

		if (IsEqualGUID(*lpGuid, IID_IDirect3DNullDevice))
			return D3DENUMRET_OK;

		g_foundAnyD3DDevice = TRUE;

		BOOL bIsHardware = (0 != lpD3DHWDeviceDesc->dwFlags);
		D3DDEVICEDESC* pDesc = bIsHardware ? lpD3DHWDeviceDesc : lpD3DHELDeviceDesc;

		if (pDriver->pGUID != NULL)
			if (FALSE == bIsHardware)
				return D3DENUMRET_OK;

		if (g_ddDeviceFilterCallback)
			if (FAILED(g_ddDeviceFilterCallback(&pDriver->ddDriverCaps, pDesc)))
				return D3DENUMRET_OK;

		if (NULL == (pNewDevice = new DDAppDevice))
			return D3DENUMRET_CANCEL;
		memset(pNewDevice, 0, sizeof(DDAppDevice));

		memcpy(&pNewDevice->guid, lpGuid, sizeof(GUID));
		pNewDevice->pGUID = &pNewDevice->guid;
		strncpy(pNewDevice->deviceName, lpDeviceName, 39);
		memcpy(&pNewDevice->deviceDesc, pDesc, sizeof(D3DDEVICEDESC));
		pNewDevice->isHardwareAccelerated = bIsHardware;
		pNewDevice->ddAppParent = pDriver;

		if (pNewDevice->isHardwareAccelerated)
			pDriver->primaryDevice = pNewDevice;
		else
		{
			if (NULL == pDriver->primaryDevice)
				if (D3DCOLOR_RGB & lpD3DHELDeviceDesc->dcmColorModel)
					pDriver->primaryDevice = pNewDevice;
		}

		g_ddraw4->EnumDisplayModes(0, NULL, pNewDevice, EnumDisplayModes);

		DDSURFACEDESC2 ddsd;
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		g_ddraw4->GetDisplayMode(&ddsd);
		DWORD dwDisplayBPP = ddsd.ddpfPixelFormat.dwRGBBitCount;

		if (pDriver->ddDriverCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED && ! g_forceFullscreen)
		{
			for (DDAppDevice::DisplayMode* pMode = pNewDevice->displayModeListHead; pMode; pMode = pMode->nextDisplayMode)
			{
				if (pMode->surfaceDesc.ddpfPixelFormat.dwRGBBitCount == dwDisplayBPP)
				{
					pNewDevice->supportsCurrentMode = TRUE;
					if (NULL == pDriver->pGUID)
						pNewDevice->canRenderWindowedOnPrimary = TRUE;
				}
			}
		}

		if (pNewDevice->displayModeListHead)
		{
			DDAppDevice** pDevice = &pDriver->deviceListHead;
			while (*pDevice)
				pDevice = &((*pDevice)->nextDevice);
			(*pDevice) = pNewDevice;
		}
		else
			delete pNewDevice;

		if (IsEqualGUID(*lpGuid, IID_IDirect3DRefDevice))
			g_foundRefDevice = TRUE;

		return D3DENUMRET_OK;
	}

	// FUNCTION: TOY2 0x004AC540 [MATCHED]
	BOOL WINAPI DDrawEnumCallbackExA(GUID* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm)
	{
		using namespace DrawingDevice;

		LPDIRECTDRAW pDD;
		if (FAILED(DirectDrawCreate(lpGUID, &pDD, 0L)))
			return D3DENUMRET_OK;

		if (FAILED(pDD->QueryInterface(IID_IDirectDraw4, (VOID**)&g_ddraw4)))
		{
			pDD->Release();
			return D3DENUMRET_OK;
		}
		pDD->Release();

		LPDIRECT3D3 pD3D;
		if (FAILED(g_ddraw4->QueryInterface(IID_IDirect3D3, (VOID**)&pD3D)))
		{
			g_ddraw4->Release();
			return D3DENUMRET_OK;
		}

		DDAppDevice::App* pNewDriver = new DDAppDevice::App;
		if (NULL == pNewDriver)
			return D3DENUMRET_CANCEL;

		memset(pNewDriver, 0, sizeof(DDAppDevice::App));

		if (NULL != lpGUID)
		{
			memcpy(&pNewDriver->guid, lpGUID, sizeof(GUID));
			pNewDriver->pGUID = &pNewDriver->guid;
		}
		strncpy(pNewDriver->driverDesc, lpDriverDescription, 39);
		strncpy(pNewDriver->driverName, lpDriverName, 39);
		pNewDriver->hMonitor = hm;

		pNewDriver->ddDriverCaps.dwSize = sizeof(DDCAPS);
		pNewDriver->ddHELCaps.dwSize = sizeof(DDCAPS);
		g_ddraw4->GetCaps(&pNewDriver->ddDriverCaps, &pNewDriver->ddHELCaps);

		pNewDriver->vidMemTotal = pNewDriver->ddDriverCaps.dwVidMemTotal;
		pNewDriver->vidMemFree = pNewDriver->ddDriverCaps.dwVidMemFree;

		DDSCAPS2 ddsCaps;
		memset(&ddsCaps, 0, sizeof(DDSCAPS2));
		ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;
		g_ddraw4->GetAvailableVidMem(&ddsCaps, &pNewDriver->totalPrimaryMem, &pNewDriver->freePrimaryMem);

		memset(&ddsCaps, 0, sizeof(DDSCAPS2));
		ddsCaps.dwCaps = DDSCAPS_TEXTURE;
		g_ddraw4->GetAvailableVidMem(&ddsCaps, &pNewDriver->totalTextureMem, &pNewDriver->freeTextureMem);

		pD3D->EnumDevices(D3DDeviceEnumCallback, pNewDriver);

		if (pNewDriver->deviceListHead)
		{
			DDAppDevice::App** pDriver = &g_ddAppListHead;
			while (*pDriver)
				pDriver = &((*pDriver)->chainDDApp);
			(*pDriver) = pNewDriver;

			if (NULL == lpGUID)
				g_primaryDDApp = pNewDriver;
		}
		else
			delete pNewDriver;

		pD3D->Release();
		g_ddraw4->Release();
		return DDENUMRET_OK;
	}

	// FUNCTION: TOY2 0x004ACBC0
	BOOL WINAPI DDrawEnumCallback(GUID* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext)
	{
		return DDrawEnumCallbackExA(lpGUID, lpDriverDescription, lpDriverName, 0, 0);
	}

	// FUNCTION: TOY2 0x004AC450 [MATCHED]
	void D3DEnum_FreeResources()
	{
		using namespace DrawingDevice;

		while (g_ddAppListHead)
		{
			DDAppDevice::App* pDriverVictim = g_ddAppListHead;
			g_ddAppListHead = g_ddAppListHead->chainDDApp;

			while (pDriverVictim->deviceListHead)
			{
				DDAppDevice* pDeviceVictim = pDriverVictim->deviceListHead;
				pDriverVictim->deviceListHead = pDeviceVictim->nextDevice;

				while (pDeviceVictim->displayModeListHead)
				{
					DDAppDevice::DisplayMode* pModeVictim = pDeviceVictim->displayModeListHead;
					pDeviceVictim->displayModeListHead = pModeVictim->nextDisplayMode;
					delete pModeVictim;
				}
				delete pDeviceVictim;
			}
			delete pDriverVictim;
		}
	}

	// FUNCTION: TOY2 0x004AC3B0 [MATCHED]
	void D3DEnum_FreeResources_T() { D3DEnum_FreeResources(); }

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

	// FUNCTION: TOY2 0x004ACE30 [MATCHED]
	INT D3DEnum_UserDlgSelectDriver(HWND hwndParent, BOOL bCurrentlyWindowed)
	{
		using namespace DrawingDevice;

		INT nResult = -1;

		if (NULL == g_primaryDDApp)
			return -1;

		MarkCompatibleBitDepthDevices();

		if (! g_forceFullscreen)
			bCurrentlyWindowed = FALSE;

		g_primaryDDApp->primaryDevice->canRenderWindowedOnPrimary = bCurrentlyWindowed;

		HINSTANCE hInstance = (HINSTANCE)GetWindowLongA(hwndParent, GWL_HINSTANCE);

		DLGTEMPLATE* pDlgSelect = UserDlg::BuildDriverSelectTemplate();
		if (pDlgSelect)
		{
			nResult = DialogBoxIndirectParamA(hInstance, pDlgSelect, hwndParent, (DLGPROC)UserDlg::DriverSelectProc, 0L);
			delete pDlgSelect;
		}

		return nResult;
	}

	// FUNCTION: TOY2 0x004AC3D0 [MATCHED]
	INT D3DEnum_UserDlgSelectDriver_T(HWND hwndParent, BOOL bCurrentlyWindowed) { return D3DEnum_UserDlgSelectDriver(hwndParent, bCurrentlyWindowed); }

	// FUNCTION: TOY2 0x004ACEB0 [MATCHED]
	INT ResourceDlgSelectDriver(LPCSTR lpTemplateName, DLGPROC lpDialogFunc, HWND hwndParent, BOOL bCurrentlyWindowed)
	{
		using namespace DrawingDevice;

		if (NULL == g_primaryDDApp)
			return -1;

		MarkCompatibleBitDepthDevices();

		if (! g_forceFullscreen)
			bCurrentlyWindowed = FALSE;

		g_primaryDDApp->primaryDevice->canRenderWindowedOnPrimary = bCurrentlyWindowed;

		HINSTANCE hInstance = (HINSTANCE)GetWindowLongA(hwndParent, GWL_HINSTANCE);

		return DialogBoxParamA(hInstance, lpTemplateName, hwndParent, lpDialogFunc, 0L);
	}

	// FUNCTION: TOY2 0x004ACF10 [MATCHED]
	HRESULT D3DEnum_GetSelectedDriver(LPGUID* ppDriverGUID, LPGUID* ppDeviceGUID, LPDDSURFACEDESC2* ppddsd, BOOL* pbWindowed, BOOL* pbIsHardware)
	{
		using namespace DrawingDevice;

		if ((! ppDriverGUID) || (! ppDeviceGUID))
			return E_INVALIDARG;

		if (NULL == g_primaryDDApp)
			return 0x81000001;

		(*ppDriverGUID) = g_primaryDDApp->pGUID;
		(*ppDeviceGUID) = g_primaryDDApp->primaryDevice->pGUID;

		if (ppddsd)
			(*ppddsd) = &g_primaryDDApp->primaryDevice->primaryDisplayMode->surfaceDesc;
		if (pbWindowed)
			(*pbWindowed) = g_primaryDDApp->primaryDevice->canRenderWindowedOnPrimary;
		if (pbIsHardware)
			(*pbIsHardware) = g_primaryDDApp->primaryDevice->isHardwareAccelerated;

		return S_OK;
	}

	// FUNCTION: TOY2 0x004AC3F0 [MATCHED]
	HRESULT D3DEnum_GetSelectedDriver_T(LPGUID* ppDriverGUID, LPGUID* ppDeviceGUID, LPDDSURFACEDESC2* ppddsd, BOOL* pbWindowed, BOOL* pbIsHardware)
	{
		return D3DEnum_GetSelectedDriver(ppDriverGUID, ppDeviceGUID, ppddsd, pbWindowed, pbIsHardware);
	}

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

	// FUNCTION: TOY2 0x00433410 [MATCHED]
	COLORREF ApplyTextStyle(HDC hdc, HFONT hfont, COLORREF textColor, int32_t styleOp)
	{
		switch (styleOp)
		{
			case 1:
				if (hfont)
					g_savedHeaderFont = SelectObject(hdc, hfont);

				g_savedTextColor = SetTextColor(hdc, textColor);
				g_savedBkMode = SetBkMode(hdc, 1);
				return g_savedBkMode;

			case 2:
				if (hfont)
					SelectObject(hdc, g_savedHeaderFont);

				SetTextColor(hdc, g_savedTextColor);
				return SetBkMode(hdc, g_savedBkMode);

			default:
				if (hfont)
					SelectObject(hdc, hfont);

				return SetTextColor(hdc, textColor);
		}
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

			Logger::GetErrorHandler("C:\\projects\\toy2\\modesel.cpp", 162)(Toy2::g_emptyStr);
			return DefWindowProcA(hWnd, msg, wParam, lParam);
		}

		switch (msg)
		{
			case WM_CLOSE:
				Logger::GetErrorHandler("C:\\projects\\toy2\\modesel.cpp", 162)(Toy2::g_emptyStr);
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

					if (! g_offscreenDC)
					{
						EndPaint(hWnd, &paint);
						g_paintError = 1;
						return DefWindowProcA(hWnd, msg, wParam, lParam);
					}
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

				ApplyTextStyle(g_offscreenDC, g_headerFont, 0xFFFF, 1);

				if (g_ddAppIterator->chainDDApp)
				{
					ApplyTextStyle(g_offscreenDC, g_headerFont, 0, 0);
					DrawTextOutA(g_offscreenDC, textX - 2, textY - 2, "Device");
					DrawTextOutA(g_offscreenDC, textX + 2, textY + 2, "Device");

					ApplyTextStyle(g_offscreenDC, g_headerFont, 0xFF, 0);
					DrawTextOutA(g_offscreenDC, textX, textY, "Device");

					textY += 4 * g_lineHeight / 2;
				}

				ApplyTextStyle(g_offscreenDC, g_headerFont, 0, 0);
				DrawTextOutA(g_offscreenDC, textX - 2, textY - 2, "Render Method");
				DrawTextOutA(g_offscreenDC, textX + 2, textY + 2, "Render Method");

				ApplyTextStyle(g_offscreenDC, g_headerFont, 0xFF, 0);
				DrawTextOutA(g_offscreenDC, textX, textY, "Render Method");

				int32_t nextTextY = 4 * g_lineHeight / 2 + textY;

				ApplyTextStyle(g_offscreenDC, g_headerFont, 0, 0);
				DrawTextOutA(g_offscreenDC, textX - 2, nextTextY - 2, "Screen Resolution");
				DrawTextOutA(g_offscreenDC, textX + 2, nextTextY + 2, "Screen Resolution");

				ApplyTextStyle(g_offscreenDC, g_headerFont, 0xFF, 0);
				DrawTextOutA(g_offscreenDC, textX, nextTextY, "Screen Resolution");

				ApplyTextStyle(g_offscreenDC, g_headerFont, 0, 2);
				ApplyTextStyle(g_offscreenDC, g_mainFont, 0, 1);

				int32_t deviceTextX = g_textStartX;
				int32_t deviceTextY = g_textStartY + g_lineHeight / 2;

				if (g_ddAppIterator->chainDDApp)
				{
					DrawTextOutA(g_offscreenDC, deviceTextX - 2, deviceTextY - 2, g_selectedDDApp->driverDesc);
					DrawTextOutA(g_offscreenDC, deviceTextX + 2, deviceTextY + 2, g_selectedDDApp->driverDesc);

					int32_t savedTextColor;

					if (g_selectionState)
						savedTextColor = 0x8888;
					else
						savedTextColor = g_highlightColor;

					ApplyTextStyle(g_offscreenDC, g_mainFont, savedTextColor, 0);
					DrawTextOutA(g_offscreenDC, deviceTextX, deviceTextY, g_selectedDDApp->driverDesc);

					deviceTextY += 4 * g_lineHeight / 2;
				}

				ApplyTextStyle(g_offscreenDC, g_mainFont, 0, 0);
				char* deviceName = g_ddAppSelectedDevice->deviceName;

				if (! strcmpi(g_ddAppSelectedDevice->deviceName, "rgb emulation"))
					deviceName = "Software";

				if (! strcmpi(deviceName, "direct3d hal"))
					deviceName = "Hardware";

				DrawTextOutA(g_offscreenDC, deviceTextX - 2, deviceTextY - 2, deviceName);
				DrawTextOutA(g_offscreenDC, deviceTextX + 2, deviceTextY + 2, deviceName);

				int32_t deviceHighlightColor;

				if (g_selectionState == 1)
					deviceHighlightColor = g_highlightColor;
				else
					deviceHighlightColor = 0x8888;

				ApplyTextStyle(g_offscreenDC, g_mainFont, deviceHighlightColor, 0);
				DrawTextOutA(g_offscreenDC, deviceTextX, deviceTextY, deviceName);

				int32_t modeTextY = 4 * g_lineHeight / 2 + deviceTextY;

				if (g_selectionState == 3)
				{
					ApplyTextStyle(g_offscreenDC, g_mainFont, 0, 0);

					char* selectedModeText;

					if (g_ddAppSelectedDevice->canRenderWindowedOnPrimary)
						selectedModeText = "Windowed";
					else
						selectedModeText = g_ddAppSelectedDisplayMode->modeText;

					DrawTextOutA(g_offscreenDC, deviceTextX - 2, modeTextY - 2, selectedModeText);

					if (g_ddAppSelectedDevice->canRenderWindowedOnPrimary)
						DrawTextOutA(g_offscreenDC, deviceTextX + 2, modeTextY + 2, "Windowed");
					else
						DrawTextOutA(g_offscreenDC, deviceTextX + 2, modeTextY + 2, g_ddAppSelectedDisplayMode->modeText);

					ApplyTextStyle(g_offscreenDC, g_mainFont, 0xFF, 0);

					char* windowedModeText;

					if (g_ddAppSelectedDevice->canRenderWindowedOnPrimary)
						windowedModeText = "Windowed";
					else
						windowedModeText = g_ddAppSelectedDisplayMode->modeText;

					DrawTextOutA(g_offscreenDC, deviceTextX, modeTextY, windowedModeText);
				}
				else
				{
					ApplyTextStyle(g_offscreenDC, g_mainFont, 0, 0);
					DrawTextOutA(g_offscreenDC, deviceTextX - 2, modeTextY - 2, g_ddAppSelectedDisplayMode->modeText);
					DrawTextOutA(g_offscreenDC, deviceTextX + 2, modeTextY + 2, g_ddAppSelectedDisplayMode->modeText);

					int32_t modeHighlightColor;

					if (g_selectionState == 2)
						modeHighlightColor = g_highlightColor;
					else
						modeHighlightColor = 0x8888;

					ApplyTextStyle(g_offscreenDC, g_mainFont, modeHighlightColor, 0);
					DrawTextOutA(g_offscreenDC, deviceTextX, modeTextY, g_ddAppSelectedDisplayMode->modeText);
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

							ApplyTextStyle(g_offscreenDC, g_mainFont, 0, 2);

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
					HDC instructionDC = g_offscreenDC;
					HFONT instructionFont = g_instructionFont;
					int32_t instructionLineHeight = g_instructionLineHeight;
					const char** shadowIter = instructionLines;

					int32_t instructionLineYIter = instructionFirstLineY;

					ApplyTextStyle(instructionDC, instructionFont, 0, 0);

					const char* currentInstructionShadow = *instructionLines;

					if (*instructionLines)
					{
						int32_t shadowLineY = instructionFirstLineY + 2;

						do
						{
							DrawTextOutA(instructionDC, deviceTextX - 2, shadowLineY - 4, currentInstructionShadow);
							DrawTextOutA(instructionDC, deviceTextX + 2, shadowLineY, *shadowIter);

							currentInstructionShadow = shadowIter[1];
							++shadowIter;

							shadowLineY += instructionLineHeight;

						} while (currentInstructionShadow);
					}

					const char** lineIter = instructionLines;

					ApplyTextStyle(instructionDC, instructionFont, 0xFFFFFF, 0);

					for (const char* instructionLine = *instructionLines; instructionLine;
						 instructionLineYIter = (instructionLineYIter + instructionLineHeight))
					{
						DrawTextOutA(instructionDC, deviceTextX, instructionLineYIter, instructionLine);

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
		wndClass.lpszMenuName = Toy2::g_emptyStr;
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

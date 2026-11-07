#include "UserDlg.h"
#include "DrawingDevice.h"

#include <windows.h>

#define IDC_STATIC 0xFFFF
#define IDC_DRIVER_COMBO 1000
#define IDC_DEVICE_COMBO 1001
#define IDC_MODE_COMBO 1002

namespace ModeSelect
{
	extern int32_t g_forceFullscreen;
}

namespace UserDlg
{
	// FUNCTION: TOY2 0x004B1380 [MATCHED]
	void CopyToWideChar(WCHAR** pstrOut, LPSTR strIn)
	{
		DWORD dwLen = lstrlenA(strIn);
		WCHAR* strOut = *pstrOut;

		dwLen = MultiByteToWideChar(CP_ACP, 0, strIn, dwLen, strOut, dwLen);
		strOut[dwLen++] = L'\0';
		*pstrOut += dwLen;
	}

	// FUNCTION: TOY2 0x004B13C0 [MATCHED]
	void AddDialogControl(WORD** pp, DWORD dwStyle, SHORT x, SHORT y, SHORT cx, SHORT cy, WORD id, LPSTR strClassName, LPSTR strTitle)
	{
		DLGITEMTEMPLATE* p = (DLGITEMTEMPLATE*)(((((ULONG)(*pp)) + 3) >> 2) << 2);

		p->style = dwStyle | WS_CHILD | WS_VISIBLE;
		p->dwExtendedStyle = 0L;
		p->x = x;
		p->y = y;
		p->cx = cx;
		p->cy = cy;
		p->id = id;

		*pp = (WORD*)(++p);

		CopyToWideChar((WCHAR**)pp, strClassName);
		CopyToWideChar((WCHAR**)pp, strTitle);

		(*pp)++;
	}

	// FUNCTION: TOY2 0x004B11A0
	DLGTEMPLATE* BuildDriverSelectTemplate()
	{
		DLGTEMPLATE* pDlgTemplate = new DLGTEMPLATE[50];
		if (NULL == pDlgTemplate)
			return NULL;
		memset(pDlgTemplate, 0, 50 * sizeof(DLGTEMPLATE));

		DLGTEMPLATE* pdt = pDlgTemplate;
		pdt->style = DS_MODALFRAME | DS_NOIDLEMSG | DS_SETFOREGROUND | DS_3DLOOK | DS_CENTER | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | DS_SETFONT;
		pdt->dwExtendedStyle = 0L;
		pdt->cdit = 8;
		pdt->x = 0;
		pdt->y = 0;
		pdt->cx = 179;
		pdt->cy = 84;

		WORD* pw = (WORD*)(++pdt);
		*pw++ = L'\0';
		*pw++ = L'\0';
		CopyToWideChar((WCHAR**)&pw, "Select New Direct3D Driver");
		*pw++ = 8;
		CopyToWideChar((WCHAR**)&pw, "Arial");

		AddDialogControl(&pw, BS_PUSHBUTTON | WS_TABSTOP, 65, 65, 51, 14, IDOK, "BUTTON", "OK");
		AddDialogControl(&pw, BS_PUSHBUTTON | WS_TABSTOP, 123, 65, 51, 14, IDCANCEL, "BUTTON", "Cancel");
		AddDialogControl(&pw, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 36, 5, 138, 45, IDC_DRIVER_COMBO, "COMBOBOX", "");
		AddDialogControl(&pw, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 36, 24, 138, 45, IDC_DEVICE_COMBO, "COMBOBOX", "");
		AddDialogControl(&pw, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 36, 44, 138, 45, IDC_MODE_COMBO, "COMBOBOX", "");
		AddDialogControl(&pw, SS_LEFT, 5, 5, 22, 13, IDC_STATIC, "STATIC", "Driver:");
		AddDialogControl(&pw, SS_LEFT, 5, 24, 25, 13, IDC_STATIC, "STATIC", "Device:");
		AddDialogControl(&pw, SS_LEFT, 5, 44, 22, 13, IDC_STATIC, "STATIC", "Mode:");

		return pDlgTemplate;
	}

	// FUNCTION: TOY2 0x004B1680 [MATCHED]
	void UpdateComboBoxesContent(
		HWND hDlg,
		DrawingDevice::DDAppDevice::App** ppCurrentDriver,
		DrawingDevice::DDAppDevice** ppCurrentDevice,
		DrawingDevice::DDAppDevice::DisplayMode** ppCurrentMode,
		BOOL bWindowed)
	{
		using namespace DrawingDevice;

		if ((NULL == ppCurrentDriver) || (NULL == ppCurrentDevice) || (NULL == ppCurrentMode))
			return;

		if (NULL == *ppCurrentDriver)
			(*ppCurrentDriver) = GetListHead();

		if (NULL == *ppCurrentDevice)
			(*ppCurrentDevice) = (*ppCurrentDriver)->deviceListHead;

		SendDlgItemMessageA(hDlg, IDC_DRIVER_COMBO, CB_RESETCONTENT, 0, 0);
		SendDlgItemMessageA(hDlg, IDC_DEVICE_COMBO, CB_RESETCONTENT, 0, 0);
		SendDlgItemMessageA(hDlg, IDC_MODE_COMBO, CB_RESETCONTENT, 0, 0);

		for (DDAppDevice::App* pDriver = GetListHead(); pDriver; pDriver = pDriver->chainDDApp)
		{
			DWORD dwItem = SendDlgItemMessageA(hDlg, IDC_DRIVER_COMBO, CB_ADDSTRING, 0, (LPARAM)pDriver->driverDesc);

			SendDlgItemMessageA(hDlg, IDC_DRIVER_COMBO, CB_SETITEMDATA, (WPARAM)dwItem, (LPARAM)pDriver);

			if (pDriver == (*ppCurrentDriver))
				SendDlgItemMessageA(hDlg, IDC_DRIVER_COMBO, CB_SETCURSEL, (WPARAM)dwItem, 0L);
		}

		for (DDAppDevice* pDevice = (*ppCurrentDriver)->deviceListHead; pDevice; pDevice = pDevice->nextDevice)
		{
			DWORD dwItem = SendDlgItemMessageA(hDlg, IDC_DEVICE_COMBO, CB_ADDSTRING, 0, (LPARAM)pDevice->deviceName);

			SendDlgItemMessageA(hDlg, IDC_DEVICE_COMBO, CB_SETITEMDATA, (WPARAM)dwItem, (LPARAM)pDevice);

			if (pDevice == (*ppCurrentDevice))
				SendDlgItemMessageA(hDlg, IDC_DEVICE_COMBO, CB_SETCURSEL, (WPARAM)dwItem, 0L);
		}

		if ((*ppCurrentDriver)->ddDriverCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED && (NULL == (*ppCurrentDriver)->pGUID)
			&& (*ppCurrentDevice)->supportsCurrentMode && ModeSelect::g_forceFullscreen)
		{
			DWORD dwItem = SendDlgItemMessageA(hDlg, IDC_MODE_COMBO, CB_ADDSTRING, 0, (LPARAM) "Windowed mode");

			SendDlgItemMessageA(hDlg, IDC_MODE_COMBO, CB_SETITEMDATA, (WPARAM)dwItem, (LPARAM)NULL);

			if (bWindowed)
				SendDlgItemMessageA(hDlg, IDC_MODE_COMBO, CB_SETCURSEL, (WPARAM)dwItem, 0L);
		}

		for (DDAppDevice::DisplayMode* pMode = (*ppCurrentDevice)->displayModeListHead; pMode; pMode = pMode->nextDisplayMode)
		{
			DWORD dwItem = SendDlgItemMessageA(hDlg, IDC_MODE_COMBO, CB_ADDSTRING, 0, (LPARAM)pMode->modeText);

			SendDlgItemMessageA(hDlg, IDC_MODE_COMBO, CB_SETITEMDATA, (WPARAM)dwItem, (LPARAM)pMode);

			if (! bWindowed && (pMode == (*ppCurrentMode)))
				SendDlgItemMessageA(hDlg, IDC_MODE_COMBO, CB_SETCURSEL, (WPARAM)dwItem, 0L);
		}
	}

	// FUNCTION: TOY2 0x004B1440
	BOOL CALLBACK DriverSelectProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
	{
		using namespace DrawingDevice;

		// GLOBAL: TOY2 0x0088445C
		static DDAppDevice::App* g_oldDriver;
		// GLOBAL: TOY2 0x00884454
		static DDAppDevice::App* g_newDriver;
		// GLOBAL: TOY2 0x00884450
		static DDAppDevice* g_oldDevice;
		// GLOBAL: TOY2 0x00884458
		static DDAppDevice* g_newDevice;
		// GLOBAL: TOY2 0x0088444C
		static DDAppDevice::DisplayMode* g_oldMode;
		// GLOBAL: TOY2 0x00884460
		static DDAppDevice::DisplayMode* g_newMode;
		// GLOBAL: TOY2 0x00884468
		static BOOL g_oldWindowed;
		// GLOBAL: TOY2 0x00884464
		static BOOL g_newWindowed;

		if (WM_INITDIALOG == uiMsg)
		{
			g_newDriver = g_oldDriver = g_primaryDDApp;
			g_newDevice = g_oldDevice = g_oldDriver->primaryDevice;
			g_newMode = g_oldMode = g_oldDevice->primaryDisplayMode;
			g_newWindowed = g_oldWindowed = g_oldDevice->canRenderWindowedOnPrimary;

			UpdateComboBoxesContent(hDlg, &g_oldDriver, &g_oldDevice, &g_oldMode, g_oldWindowed);
			return TRUE;
		}

		if (WM_COMMAND == uiMsg)
		{
			if (IDOK == (WORD)wParam)
			{
				if ((g_oldDriver != g_newDriver) || (g_oldDevice != g_newDevice) || (g_oldMode != g_newMode) || (g_oldWindowed != g_newWindowed))
				{
					g_primaryDDApp = g_newDriver;
					g_primaryDDApp->primaryDevice = g_newDevice;
					g_primaryDDApp->primaryDevice->primaryDisplayMode = g_newMode;
					g_primaryDDApp->primaryDevice->canRenderWindowedOnPrimary = g_newWindowed;

					EndDialog(hDlg, IDOK);
					return TRUE;
				}
				else
				{
					EndDialog(hDlg, IDCANCEL);
					return TRUE;
				}
			}
			else if (IDCANCEL == (WORD)wParam)
			{
				EndDialog(hDlg, IDCANCEL);
				return TRUE;
			}
			else if (CBN_SELENDOK == (WORD)(wParam >> 16))
			{
				DWORD dwIndex = SendMessageA((HWND)lParam, CB_GETCURSEL, 0, 0);
				LONG pNewObject = SendMessageA((HWND)lParam, CB_GETITEMDATA, dwIndex, 0);

				if (CB_ERR == dwIndex)
					return TRUE;

				switch ((WORD)wParam)
				{
					case IDC_DRIVER_COMBO:
						if (pNewObject && pNewObject != (LONG)g_newDriver)
						{
							g_newDriver = (DDAppDevice::App*)pNewObject;
							g_newDevice = g_newDriver->primaryDevice;
							g_newMode = g_newDevice->primaryDisplayMode;
							g_newWindowed = g_newDevice->canRenderWindowedOnPrimary;
						}
						break;
					case IDC_DEVICE_COMBO:
						if (pNewObject && pNewObject != (LONG)g_newDevice)
						{
							g_newDevice = (DDAppDevice*)pNewObject;
							g_newMode = g_newDevice->primaryDisplayMode;
							g_newWindowed = g_newDevice->canRenderWindowedOnPrimary;
						}
						break;
					case IDC_MODE_COMBO:
						if (pNewObject)
						{
							g_newMode = (DDAppDevice::DisplayMode*)pNewObject;
							g_newWindowed = FALSE;
						}
						else
							g_newWindowed = TRUE;
						break;
				}

				UpdateComboBoxesContent(hDlg, &g_newDriver, &g_newDevice, &g_newMode, g_newWindowed);

				return TRUE;
			}
		}
		return FALSE;
	}
}

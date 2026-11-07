#pragma once

#include "Common.h"
#include <windows.h>

namespace UserDlg
{
	DLGTEMPLATE* BuildDriverSelectTemplate();
	BOOL CALLBACK DriverSelectProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
}

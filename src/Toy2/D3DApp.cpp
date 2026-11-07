#include "Toy2/D3DAppI.h"
#include "Toy2/Toy2.h"
#include "Logger.h"
#include <cstdio>

namespace D3DApp
{
	// GLOBAL: TOY2 0x0050AF64
	int32_t g_changingCoopLevel = 0;

	// GLOBAL: TOY2 0x004E0690
	int32_t g_checkAvailableMem = 1;

	// GLOBAL: TOY2 0x00882C20
	int32_t g_sysParamsInfo;

	// GLOBAL: TOY2 0x00884038
	int32_t g_no32bitColors = 0;

	// GLOBAL: TOY2 0x0051B0B8
	D3DAppInfo g_d3dAppI;

	// GLOBAL: TOY2 0x0050B650
	PC g_pcStruct;

	// GLOBAL: TOY2 0x00534488
	WindowData g_windowData;

	// GLOBAL: TOY2 0x00534554
	int16_t g_renderMode;

	// GLOBAL: TOY2 0x0050A770
	int32_t g_readyForRender = 0;

	// GLOBAL: TOY2 0x0051ABD4
	int32_t g_usesPalette;

	// GLOBAL: TOY2 0x0051AAC8
	int32_t g_primaryPalettized;

	// GLOBAL: TOY2 0x0050AA58
	LPDIRECTDRAWPALETTE g_lpPalette = 0;

	// GLOBAL: TOY2 0x00882E30
	int32_t g_unused1;

	// GLOBAL: TOY2 0x00882C28
	int32_t g_unused2;

	// GLOBAL: TOY2 0x00504E58
	int32_t g_windowCreationError = 1;

	// GLOBAL: TOY2 0x0051ABD0
	HRESULT g_lastError;

	// GLOBAL: TOY2 0x0050AB64
	PALETTEENTRY g_originalPpe[256];

	// GLOBAL: TOY2 0x0051ABE0
	SIZE g_szLastClient;

	// GLOBAL: TOY2 0x0051A940
	BOOL (*g_d3dDeviceDestroyCallback)(LPVOID);

	// GLOBAL: TOY2 0x0051A9C4
	LPVOID g_d3dDeviceDestroyCallbackContext;

	// GLOBAL: TOY2 0x0050A718
	BOOL (*g_d3dDeviceCreateCallback)(int32_t, int32_t, LPDIRECT3DVIEWPORT2*, LPVOID);

	// GLOBAL: TOY2 0x0051AFB0
	LPVOID g_d3dDeviceCreateCallbackContext;
}

namespace D3DApp
{
	// FUNCTION: TOY2 0x004318F0
	void LogErrorNotSet()
	{
		char* errorNotSet = Logger::GetErrorNotSet();
		Logger::LogLn(errorNotSet);
	}

	// FUNCTION: TOY2 0x004093A0
	int32_t BuildProfileMachine()
	{
		memset(&g_pcStruct, 0, sizeof(g_pcStruct));

		Logger::Log("---------------------\n");
		Logger::Log("BEGIN EXAMINE MACHINE\n\n");

		int16_t count = 0;

		WNDCLASSA wndClass;
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = ProfileWndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = g_windowData.hInstance;
		wndClass.hIcon = LoadIconA(0, IDI_WINLOGO);
		wndClass.hCursor = LoadCursorA(0, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = Toy2::g_emptyStr;
		wndClass.lpszClassName = "Examining Machine";

		if (! RegisterClassA(&wndClass))
			return 0;

		int16_t curDeviceCount = 1;

		g_windowData.mainHwnd = CreateWindowExA(
			WS_EX_APPWINDOW,
			"Examining Machine",
			"Examining Machine",
			WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			g_windowData.hInstance,
			0);

		ShowWindow(g_windowData.mainHwnd, SW_SHOWNORMAL);
		UpdateWindow(g_windowData.mainHwnd);
		SetFocus(g_windowData.mainHwnd);

		g_pcStruct.ddDeviceCount = 0;

		HRESULT ddEnumResult = DirectDrawEnumerateA(EnumerateDevices, 0);
		if (ddEnumResult < 0)
			Logger::LogDDError("DirectDrawEnumerateA(ExamineDDEnumCallback, 0)", ddEnumResult);

		Logger::Log("END EXAMINE MACHINE\n");
		Logger::Log("-------------------\n\n");
		Logger::Log("--------------------\n");
		Logger::Log("BEGIN SELECT DRIVERS\n\n");

		g_pcStruct.defaultDriver = g_pcStruct.examineDevices;
		g_pcStruct.examineDevices[0].isPreferredDevice = 1;

		Logger::Log(
			"INFO DirectDraw - Default driver >>>%s,%s<<< selected.\n", g_pcStruct.examineDevices[0].driverName, g_pcStruct.examineDevices[0].driverDesc);

		if (g_pcStruct.ddDeviceCount > 1)
		{
			int32_t deviceIter = 1;

			do
			{
				ExamineDevice* curDevice = &g_pcStruct.examineDevices[deviceIter];

				if (curDevice->valid && g_pcStruct.examineDevices[deviceIter].hasHardwareAccel && g_pcStruct.examineDevices[deviceIter].has3DSupport)
				{
					g_pcStruct.defaultDriver = curDevice;

					Logger::Log("INFO DirectDraw - Override default driver, new driver >>>%s,%s<<< selected.\n", curDevice->driverName, curDevice->driverDesc);
				}

				deviceIter = ++curDeviceCount;
			} while (curDeviceCount < g_pcStruct.ddDeviceCount);
		}

		if (g_pcStruct.defaultDriver)
		{
			const char* hasHardwareAccel = "has";

			if (! g_pcStruct.defaultDriver->hasHardwareAccel)
				hasHardwareAccel = "has no";

			Logger::Log("INFO DirectDraw - Device %s hardware acceleration.\n", hasHardwareAccel);
		}
		else
		{
			Logger::Log("INFO No available DirectDraw devices.\n");
		}

		Logger::Log("\n");

		g_pcStruct.softwareRenderMode = 0;

		if (g_pcStruct.defaultDriver->isPrimaryDisplay)
		{
			Logger::Log("INFO Direct3D - Searching for hardware RGB driver.\n");
			ExamineDevice* defaultDriver = g_pcStruct.defaultDriver;

			int32_t hardwareRGBCount = 0;
			int32_t softwareRGBCount = 0;

			InterfaceDevice* hardwareRGBDevices[6];
			InterfaceDevice* softwareRGBDevices[6];

			if (g_pcStruct.defaultDriver->deviceCount > 0)
			{
				int32_t curInterfaceDev = 0;

				InterfaceDevice** softwareDevices = softwareRGBDevices;
				InterfaceDevice** hardwareDevices = hardwareRGBDevices;

				do
				{
					InterfaceDevice* interfaceDevice = &g_pcStruct.defaultDriver->interfaceDevices[curInterfaceDev];

					if (interfaceDevice->isHardwareAccelerated)
					{
						if ((interfaceDevice->hwDeviceDesc.dcmColorModel & 2) != 0)
						{
							*hardwareDevices++ = interfaceDevice;
							++hardwareRGBCount;
						}
					}
					else if ((interfaceDevice->hwDeviceDesc.dcmColorModel & 2) != 0)
					{
						*softwareDevices = interfaceDevice;
						++softwareRGBCount;
						++softwareDevices;
					}

					curInterfaceDev = ++count;
				} while (count < defaultDriver->deviceCount);
			}

			if (hardwareRGBCount && hardwareRGBDevices[0]->hasTexturing)
			{
				g_pcStruct.selectedInterfaceDevice = hardwareRGBDevices[0];
				g_pcStruct.fullscreenMode = 1;
				Logger::Log("INFO Direct3D - Hardware RGB driver >>>%s,%s<<< selected\n", hardwareRGBDevices[0]->baseName, hardwareRGBDevices[0]->description);
			}
			else
			{
				if (g_checkAvailableMem)
				{
					g_pcStruct.fullscreenMode = 0;

					if (softwareRGBCount)
					{
						g_pcStruct.selectedInterfaceDevice = softwareRGBDevices[0];
						Logger::Log("INFO Direct3D - Driver >>>%s,%s<<< selected\n", softwareRGBDevices[0]->baseName, softwareRGBDevices[0]->description);
						defaultDriver = g_pcStruct.defaultDriver;
					}

					defaultDriver->hasHardwareAccel = 0;
				}
				else
				{
					Logger::Log("INFO Direct3D - Found no hardware RGB drivers.\n");
					g_pcStruct.defaultDriver = g_pcStruct.examineDevices;

					Logger::Log(
						"INFO DirectDraw - Resetting to default driver >>>%s,%s<<<.\n",
						g_pcStruct.examineDevices[0].driverName,
						g_pcStruct.examineDevices[0].driverDesc);

					g_pcStruct.fullscreenMode = 1;
					g_pcStruct.softwareRenderMode = 1;
				}
			}
		}
		else
		{
			int32_t deviceCount = g_pcStruct.defaultDriver->deviceCount;
			int16_t interfaceDeviceIndex = 0;

			if (deviceCount > 0)
			{
				int32_t loopCounter1 = 0;

				while (! g_pcStruct.defaultDriver->interfaceDevices[loopCounter1].isHardwareAccelerated)
				{
					loopCounter1 = ++interfaceDeviceIndex;

					if (interfaceDeviceIndex >= deviceCount)
					{
						deviceCount = -1;
						break;
					}
				}

				if (deviceCount > 0)
				{
					g_pcStruct.fullscreenMode = 1;
					g_pcStruct.selectedInterfaceDevice = &g_pcStruct.defaultDriver->interfaceDevices[interfaceDeviceIndex];

					Logger::Log(
						"INFO Direct3D - Hardware driver >>>%s, %s<<< selected\n",
						g_pcStruct.defaultDriver->interfaceDevices[interfaceDeviceIndex].baseName,
						g_pcStruct.defaultDriver->interfaceDevices[interfaceDeviceIndex].description);
				}
			}

			if (deviceCount <= 0)
			{
				g_pcStruct.defaultDriver = g_pcStruct.examineDevices;

				Logger::Log("INFO Direct3D - No suitable hardware drivers found, scanning software renderers.\n");
				Logger::Log(
					"INFO DirectDraw - Resetting to default driver >>>%s,%s<<<.\n", g_pcStruct.defaultDriver->driverName, g_pcStruct.defaultDriver->driverDesc);

				if (g_checkAvailableMem && g_pcStruct.defaultDriver->deviceCount)
				{
					g_pcStruct.selectedInterfaceDevice = g_pcStruct.defaultDriver->interfaceDevices;
					g_pcStruct.fullscreenMode = 0;
					g_pcStruct.softwareRenderMode = 0;

					Logger::Log("INFO Direct3D - Using software renderer.\n");
				}
				else
				{
					g_pcStruct.fullscreenMode = 1;
					g_pcStruct.softwareRenderMode = 1;

					Logger::Log("INFO Direct3D - Not using any drivers, resetting to native software renderer.\n");
				}
			}
		}

		if (g_pcStruct.selectedInterfaceDevice)
		{
			const char* d3dHardwareStatus = "has";

			if (! g_pcStruct.selectedInterfaceDevice->isHardwareAccelerated)
				d3dHardwareStatus = "has no";

			Logger::Log("INFO Direct3D - Device %s hardware acceleration.\n", d3dHardwareStatus);
		}
		else
		{
			Logger::Log("INFO No available Direct3D devices.\n");
		}

		Logger::Log("\n");
		const char* fullscreenStatus = "TRUE";

		if (! g_pcStruct.fullscreenMode)
			fullscreenStatus = "FALSE";

		Logger::Log("INFO Fullscreen mode is %s.\n", fullscreenStatus);
		const char* softwareRenderStatus = "TRUE";

		if (! g_pcStruct.softwareRenderMode)
			softwareRenderStatus = "FALSE";

		Logger::Log("INFO Native software render mode is %s.\n", softwareRenderStatus);

		ExamineDevice* defaultDriver = g_pcStruct.defaultDriver;
		int32_t softwareRenderMode = g_pcStruct.softwareRenderMode;

		g_pcStruct.mode = g_pcStruct.defaultDriver->displayModes;

		if (g_pcStruct.softwareRenderMode)
		{
			int16_t displayModeLoopIndex = 0;

			if (g_pcStruct.defaultDriver->displayModeCount > 0)
			{
				int32_t displayModeCounter = 0;

				do
				{
					if (defaultDriver->displayModes[displayModeCounter].width == 640 && defaultDriver->displayModes[displayModeCounter].height == 480
						&& defaultDriver->displayModes[displayModeCounter].rgbBitCount == 16)
					{
						g_pcStruct.mode = &defaultDriver->displayModes[displayModeCounter];
						defaultDriver->selectedDisplayModeIndex = displayModeCounter;
						defaultDriver = g_pcStruct.defaultDriver;
					}

					displayModeCounter = ++displayModeLoopIndex;
				} while (displayModeLoopIndex < defaultDriver->displayModeCount);

				softwareRenderMode = g_pcStruct.softwareRenderMode;
			}
		}
		else
		{
			int16_t deviceLoopIndex = 0;

			if (g_pcStruct.defaultDriver->displayModeCount > 0)
			{
				while (true)
				{
					int32_t defaultWidth = defaultDriver->displayModes[deviceLoopIndex].width;

					if (g_checkAvailableMem)
					{
						if (defaultWidth == 320 && defaultDriver->displayModes[deviceLoopIndex].height == 240
							&& defaultDriver->displayModes[deviceLoopIndex].rgbBitCount == 16)
						{
							g_pcStruct.mode = &defaultDriver->displayModes[deviceLoopIndex];
							defaultDriver->selectedDisplayModeIndex = deviceLoopIndex;
							defaultDriver = g_pcStruct.defaultDriver;
						}
					}
					else if (
						defaultWidth == 640 && defaultDriver->displayModes[deviceLoopIndex].height == 480
						&& defaultDriver->displayModes[deviceLoopIndex].rgbBitCount == 16)
					{
						g_pcStruct.mode = &defaultDriver->displayModes[deviceLoopIndex];
						defaultDriver->selectedDisplayModeIndex = deviceLoopIndex;
						defaultDriver = g_pcStruct.defaultDriver;
					}

					if (++deviceLoopIndex >= defaultDriver->displayModeCount)
						break;
				}

				softwareRenderMode = g_pcStruct.softwareRenderMode;
			}
		}

		g_renderMode = (RenderMode)(2 - (softwareRenderMode != 0));
		const char* renderMode = "native software render";

		if (g_renderMode != RENDERMODE_SOFTWARE)
			renderMode = "Direct3d render";

		Logger::Log("INFO RenderMode set to %s mode.\n", renderMode);
		Logger::Log("INFO Screen mode %dx%dx%d selected.\n", g_pcStruct.mode->width, g_pcStruct.mode->height, g_pcStruct.mode->rgbBitCount);
		Logger::Log("\n");

		DestroyWindow(g_windowData.mainHwnd);

		while (GetMessageA(&g_windowData.wndEventMsg, 0, 0, 0))
		{
			if (g_windowData.wndEventMsg.message == WM_QUIT)
				break;

			if (! g_windowData.mainHwnd || ! TranslateAcceleratorA(g_windowData.mainHwnd, g_windowData.hAccTable, &g_windowData.wndEventMsg))
			{
				TranslateMessage(&g_windowData.wndEventMsg);
				DispatchMessageA(&g_windowData.wndEventMsg);
			}
		}

		Logger::Log("END SELECT DRIVERS\n");
		Logger::Log("------------------\n\n");

		return 1;
	}

	// FUNCTION: TOY2 0x004A6B30 [MATCHED]
	int32_t BuildWindow()
	{
		g_unused1 = 0;
		g_unused2 = 0;

		memset(&g_windowData.wndClass, 0, sizeof(g_windowData.wndClass));

		g_windowData.wndClass.cbSize = 48;

		CoInitialize(0);

		g_windowData.wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		g_windowData.wndClass.lpfnWndProc = NormalWndProc;
		g_windowData.wndClass.cbClsExtra = 0;
		g_windowData.wndClass.cbWndExtra = 0;
		g_windowData.wndClass.hInstance = g_windowData.hInstance;
		g_windowData.wndClass.hIcon = LoadIconA(g_windowData.hInstance, "AppIcon");
		g_windowData.wndClass.lpszMenuName = "AppMenu";
		g_windowData.wndClass.lpszClassName = "Toy2";
		g_windowData.wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

		if (! RegisterClassExA(&g_windowData.wndClass))
		{
			char* buffer;
			FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				0,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)&buffer,
				0,
				0);
			MessageBoxA(0, buffer, "GetLastError", MB_ICONINFORMATION);
			LocalFree(buffer);
			return 0;
		}

		HWND window = CreateWindowExA(WS_EX_TOPMOST, "Toy2", "Toy2", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, 0, 0, g_windowData.hInstance, 0);

		g_windowData.mainHwnd = window;
		g_d3dAppI.hwnd = window;

		if (! window)
		{
			Logger::LogLn("CreateWindowEx failed");
			return 0;
		}

		UpdateWindow(window);
		g_windowData.hAccTable = LoadAcceleratorsA(g_windowData.hInstance, "AppAccel");

		if (SystemParametersInfoA(SPI_SETSCREENSAVERRUNNING, 1u, &g_sysParamsInfo, 0))
			atexit(SysParmsOnExit);

		g_windowCreationError = 0;

		return 1;
	}

	// FUNCTION: TOY2 0x004A6CC0
	int32_t ProcessWndEvents()
	{
		if (PeekMessageA(&g_windowData.wndEventMsg, 0, 0, 0, 1))
		{
			if (g_windowData.wndEventMsg.message == WM_QUIT)
			{
				g_windowData.wndIsExiting = 1;
				return 0;
			}

			if (g_windowData.wndEventMsg.message != WM_ACTIVATEAPP
				&& (! g_windowData.mainHwnd || ! TranslateAcceleratorA(g_windowData.mainHwnd, g_windowData.hAccTable, &g_windowData.wndEventMsg)))
			{
				TranslateMessage(&g_windowData.wndEventMsg);
				DispatchMessageA(&g_windowData.wndEventMsg);
			}
		}

		return 1;
	}

	// FUNCTION: TOY2 0x004A6D30
	int32_t PostQuitMessage()
	{
		::PostQuitMessage(0);
		return 1;
	}

	// FUNCTION: TOY2 0x00408D30
	int32_t WINAPI EnumerateDevices(LPGUID guid, LPSTR driverDesc, LPSTR driverName, LPVOID lpContext)
	{
		memset(&g_pcStruct.examineDevices[g_pcStruct.ddDeviceCount], 0, sizeof(g_pcStruct.examineDevices[g_pcStruct.ddDeviceCount]));

		ExamineDevice* curDevice = &g_pcStruct.examineDevices[g_pcStruct.ddDeviceCount];

		LPDIRECTDRAW lpDD;
		if (DirectDrawCreate(guid, &lpDD, 0) < 0)
		{
			memset(&g_pcStruct.examineDevices[g_pcStruct.ddDeviceCount], 0, sizeof(g_pcStruct.examineDevices[g_pcStruct.ddDeviceCount]));
			return 1;
		}

		HRESULT query1Result = lpDD->QueryInterface(IID_IDirectDraw2, (void**)&curDevice->dd2);

		if (query1Result < 0)
			Logger::LogDDError("tempDD->QueryInterface(IID_IDirectDraw2, (LPVOID*)&(ddinfo->DDDevice))", query1Result);

		if (lpDD)
		{
			lpDD->Release();
			lpDD = 0;
		}

		g_changingCoopLevel = 1;
		curDevice->dd2->SetCooperativeLevel(g_windowData.mainHwnd, 81);
		g_changingCoopLevel = 0;

		memset(&curDevice->ddCaps, 0, sizeof(curDevice->ddCaps));

		curDevice->ddCaps.dwSize = 380;

		if (curDevice->dd2->GetCaps(&curDevice->ddCaps, 0) < 0)
		{
			curDevice->dd2->Release();
			memset(&g_pcStruct.examineDevices[g_pcStruct.ddDeviceCount], 0, sizeof(g_pcStruct.examineDevices[g_pcStruct.ddDeviceCount]));
			return 1;
		}

		DDSCAPS ddsCaps;
		ddsCaps.dwCaps = 4096;

		// $TODO: Honestly who knows whats going on here (compiler artifact?)
		if (guid && guid != (GUID*)2 && g_checkAvailableMem != 1)
		{
			HRESULT availMemResult = curDevice->dd2->GetAvailableVidMem(&ddsCaps, (LPDWORD)&curDevice->texVidMemTot, (LPDWORD)&curDevice->texVidMemFree);

			if (availMemResult < 0)
				Logger::LogDDError("ddinfo->DDDevice->GetAvailableVidMem(&caps, &ddinfo->TexVidMemTotal, &ddinfo->TexVidMemFree)", availMemResult);

			uint32_t texVidMemFree = (uint32_t)curDevice->texVidMemFree;

			if (texVidMemFree < 0x200000)
			{
				Logger::Log("DIRECT DRAW DEVICE : Device %s,%s reports less than %d texture memory free - skipping.\n\n", driverName, driverDesc, 0x200000);

				curDevice->dd2->Release();
				memset(&g_pcStruct.examineDevices[g_pcStruct.ddDeviceCount], 0, sizeof(g_pcStruct.examineDevices[g_pcStruct.ddDeviceCount]));
				return 1;
			}

			curDevice->hasLowVideoMem = texVidMemFree < 0x400000;
		}

		int32_t dwCaps = curDevice->ddCaps.dwCaps;

		curDevice->valid = 1;
		curDevice->has3DSupport = dwCaps & 1;
		curDevice->hasHardwareAccel = (~dwCaps >> 25) & 1;

		int32_t dwSVBCaps = curDevice->ddCaps.dwSVBCaps;

		curDevice->hasVideoDMA = (uint32_t)dwSVBCaps >> 31;

		if (dwSVBCaps < 0)
		{
			int32_t dwSVBCKeyCaps = curDevice->ddCaps.dwSVBCKeyCaps;
			int32_t dwSVBFXCaps = curDevice->ddCaps.dwSVBFXCaps;

			curDevice->svbCaps = dwSVBCaps;
			curDevice->svbcKeyCaps = dwSVBCKeyCaps;
			curDevice->svbFxCaps = dwSVBFXCaps;
		}

		strncpy(curDevice->driverName, driverName, sizeof(curDevice->driverName));
		strncpy(curDevice->driverDesc, driverDesc, sizeof(curDevice->driverDesc));

		if (! guid || guid == (GUID*)2)
		{
			curDevice->hasHardwareAccel = 0;
		}
		else
		{
			curDevice->isPrimaryDisplay = 0;
			curDevice->deviceGUID = *guid;
		}

		HRESULT enumModesResult = curDevice->dd2->EnumDisplayModes(0, 0, curDevice, EnumDisplayModes);

		if (enumModesResult < 0)
			Logger::LogDDError("ddinfo->DDDevice->EnumDisplayModes(0, 0, (LPVOID)ddinfo, ExamineDDModesEnumCallback)", enumModesResult);

		qsort(curDevice->displayModes, curDevice->displayModeCount, sizeof(DisplayMode), SortDisplayModes);

		Logger::Log("DIRECT DRAW DEVICE BASE NAME : %s.\n", curDevice->driverName);
		Logger::Log("DIRECT DRAW DEVICE DESCRIPTION : %s.\n", curDevice->driverDesc);

		const char* has3D = "TRUE";

		if (! curDevice->has3DSupport)
			has3D = "FALSE";

		Logger::Log("DIRECT DRAW DEVICE 3D SUPPORT : %s.\n", has3D);

		const char* hasDDHardware = "TRUE";

		if (! curDevice->hasHardwareAccel)
			hasDDHardware = "FALSE";

		Logger::Log("DIRECT DRAW DEVICE HARDWARE : %s.\n", hasDDHardware);

		const char* hasVideoDMA = "TRUE";

		if (! curDevice->hasVideoDMA)
			hasVideoDMA = "FALSE";

		Logger::Log("DIRECT DRAW SYSTEM-VIDEO DMA : %s.\n", hasVideoDMA);

		if (guid)
		{
			const char* hasLowVidMem = "TRUE";

			if (! curDevice->hasLowVideoMem)
				hasLowVidMem = "FALSE";

			Logger::Log("DIRECT DRAW LOW VIDEO MEMORY STATUS  : %s.\n", hasLowVidMem);
		}

		Logger::Log("DIRECT DRAW DEVICE SCREEN MODES.\n");
		Logger::Log("\n");

		int32_t iter = 0;

		if (curDevice->displayModeCount > 0)
		{
			DisplayMode* item = &curDevice->displayModes[0];

			do
			{
				Logger::Log("DDMODE %i - %dx%dx%d bit.\n", iter++, item->width, item->height, item->rgbBitCount);
				++item;
			} while (iter < curDevice->displayModeCount);
		}

		HRESULT query3D2Result = curDevice->dd2->QueryInterface(IID_IDirect3D2, (void**)&curDevice->dd3D);

		if (query3D2Result < 0)
		{
			Logger::LogDDError("ddinfo->DDDevice->QueryInterface(IID_IDirect3D2, (LPVOID *) &ddinfo->D3DDevice)", query3D2Result);
		}

		HRESULT enumDevicesResult = curDevice->dd3D->EnumDevices(EnumDevices, curDevice);

		if (enumDevicesResult < 0)
			Logger::LogDDError("ddinfo->D3DDevice->EnumDevices(ExamineD3DEnumCallback, (LPVOID*)ddinfo)", enumDevicesResult);

		++g_pcStruct.ddDeviceCount;

		if (curDevice->dd3D)
		{
			curDevice->dd3D->Release();
			curDevice->dd3D = 0;
		}

		if (curDevice->dd2)
		{
			curDevice->dd2->Release();
			curDevice->dd2 = 0;
		}

		return 1;
	}

	// FUNCTION: TOY2 0x004092F0 [MATCHED]
	HRESULT WINAPI EnumDisplayModes(LPDDSURFACEDESC surfaceDesc, LPVOID context)
	{
		ExamineDevice* examineContext = (ExamineDevice*)context;

		DWORD dwWidth = surfaceDesc->dwWidth;

		if (dwWidth < 320 || surfaceDesc->dwHeight < 200)
			return 1;

		DisplayMode* displayMode = &examineContext->displayModes[examineContext->displayModeCount];

		displayMode->width = dwWidth;
		displayMode->height = surfaceDesc->dwHeight;
		displayMode->rgbBitCount = surfaceDesc->ddpfPixelFormat.dwRGBBitCount;

		++examineContext->displayModeCount;

		return examineContext->displayModeCount != 128;
	}

	// FUNCTION: TOY2 0x00409130 [MATCHED]
	HRESULT WINAPI
	EnumDevices(LPGUID guid, LPSTR deviceDesc, LPSTR deviceName, LPD3DDEVICEDESC d3DHWDeviceDesc, LPD3DDEVICEDESC d3DHELDeviceDesc, LPVOID context)
	{
		ExamineDevice* examineContext = (ExamineDevice*)context;
		InterfaceDevice* item = &examineContext->interfaceDevices[examineContext->deviceCount];

		memset(item, 0, sizeof(InterfaceDevice));

		item->isHardwareAccelerated = d3DHWDeviceDesc->dcmColorModel ? TRUE : FALSE;
		memcpy(&item->guid, guid, sizeof(GUID));
		lstrcpyA(item->baseName, deviceName);
		lstrcpyA(item->description, deviceDesc);

		if (d3DHWDeviceDesc->dcmColorModel)
			memcpy(&item->hwDeviceDesc, d3DHWDeviceDesc, sizeof(D3DDEVICEDESC));
		else
			memcpy(&item->hwDeviceDesc, d3DHELDeviceDesc, sizeof(D3DDEVICEDESC));

		item->hasTexturing = (item->hwDeviceDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE) ? TRUE : FALSE;
		item->isSquareTexturesOnly = (item->hwDeviceDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) ? TRUE : FALSE;
		item->hasAlphaBlending = (item->hwDeviceDesc.dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_BOTHSRCALPHA) ? TRUE : FALSE;
		item->hasZBuffer = item->hwDeviceDesc.dwDeviceZBufferBitDepth ? TRUE : FALSE;

		Logger::Log("\n");
		Logger::Log("DIRECT 3D DEVICE BASE NAME : %s.\n", item->baseName);
		Logger::Log("DIRECT 3D DEVICE DESCRIPTION : %s.\n", item->description);

		Logger::Log("DIRECT 3D DEVICE TextureFlag\t:\t%s.\n", item->hasTexturing ? "TRUE" : "FALSE");
		Logger::Log("DIRECT 3D DEVICE ZBufferFlag : %s.\n", item->hasZBuffer ? "TRUE" : "FALSE");
		Logger::Log("DIRECT 3D DEVICE HardwareAccel : %s.\n", item->isHardwareAccelerated ? "TRUE" : "FALSE");
		Logger::Log("DIRECT 3D DEVICE SquareOnly : %s.\n", item->isSquareTexturesOnly ? "TRUE" : "FALSE");
		Logger::Log("DIRECT 3D DEVICE Alpha blending is %s.\n", item->hasAlphaBlending ? "ON" : "OFF");
		Logger::Log("\n");

		item->valid = TRUE;
		++examineContext->deviceCount;

		return D3DENUMRET_OK;
	}

	// FUNCTION: TOY2 0x00408CD0 [MATCHED]
	int32_t SortDisplayModes(const void* modeA, const void* modeB)
	{
		DisplayMode* displayModeA = (DisplayMode*)modeA;
		DisplayMode* displayModeB = (DisplayMode*)modeB;

		int32_t bitCountA = displayModeA->rgbBitCount;
		int32_t bitCountB = displayModeB->rgbBitCount;

		if (bitCountB > bitCountA)
			return -1;

		if (bitCountB < bitCountA)
			return 1;

		int32_t widthA = displayModeA->width;
		int32_t widthB = displayModeB->width;

		if (widthB > widthA)
			return -1;

		if (widthB < widthA)
			return 1;

		int32_t heightA = displayModeA->height;
		int32_t heightB = displayModeB->height;

		if (heightA < heightB)
			return -1;

		return heightB < heightA;
	}

	// FUNCTION: TOY2 0x00409360 [MATCHED]
	LRESULT WINAPI ProfileWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			case WM_DESTROY:
				g_windowData.mainHwnd = 0;
				::PostQuitMessage(0);
				break;
		}

		return DefWindowProcA(hWnd, msg, wParam, lParam);
	}

	// FUNCTION: TOY2 0x0040CAC0 [MATCHED]
	BOOL ProcessWndProc(BOOL* bStopProcessing, LRESULT* lresult, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		*bStopProcessing = FALSE;
		if (! g_readyForRender)
			return TRUE;
		switch (message)
		{
			case WM_SIZE:
				if (! g_changingCoopLevel)
				{
					*bStopProcessing = TRUE;
				}
				break;
			case WM_MOVE:
				g_d3dAppI.pClientOnPrimary.x = g_d3dAppI.pClientOnPrimary.y = 0;
				ClientToScreen(hwnd, &g_d3dAppI.pClientOnPrimary);
				break;
			case WM_ACTIVATE:
				if (g_usesPalette && g_primaryPalettized && g_d3dAppI.lpFrontBuffer)
				{
					g_d3dAppI.lpFrontBuffer->SetPalette(g_lpPalette);
				}
				break;
			case WM_ACTIVATEAPP:
				g_d3dAppI.bAppActive = TRUE;
				*bStopProcessing = TRUE;
				break;
			case WM_SETCURSOR:
				if (g_pcStruct.fullscreenMode && ! g_d3dAppI.bPaused)
				{
					*lresult = 1;
					*bStopProcessing = TRUE;
					return TRUE;
				}
				break;
			case WM_MOVING:
				if (g_pcStruct.fullscreenMode)
				{
					GetWindowRect(hwnd, (LPRECT)lParam);
					*lresult = 1;
					*bStopProcessing = TRUE;
					return TRUE;
				}
				break;
			case WM_GETMINMAXINFO:
				if (g_pcStruct.fullscreenMode)
				{
					((LPMINMAXINFO)lParam)->ptMaxTrackSize.x = g_pcStruct.mode->width;
					((LPMINMAXINFO)lParam)->ptMaxTrackSize.y = g_pcStruct.mode->height;
					((LPMINMAXINFO)lParam)->ptMinTrackSize.x = g_pcStruct.mode->width;
					((LPMINMAXINFO)lParam)->ptMinTrackSize.y = g_pcStruct.mode->height;
					*lresult = 0;
					*bStopProcessing = TRUE;
					return TRUE;
				}
				else
				{
					((LPMINMAXINFO)lParam)->ptMaxTrackSize.x = g_d3dAppI.windowsDisplay.w;
					((LPMINMAXINFO)lParam)->ptMaxTrackSize.y = g_d3dAppI.windowsDisplay.h;
					*lresult = 0;
					*bStopProcessing = TRUE;
					return TRUE;
				}
				break;
			case WM_PAINT:
				BeginPaint(hwnd, &ps);
				EndPaint(hwnd, &ps);
				*lresult = 1;
				*bStopProcessing = TRUE;
				return TRUE;
			case WM_NCPAINT:
				if (g_pcStruct.fullscreenMode && ! g_d3dAppI.bPaused)
				{
					*lresult = 0;
					*bStopProcessing = TRUE;
					return TRUE;
				}
				break;
		}
		return TRUE;
	}

	// FUNCTION: TOY2 0x004A6D40 [MATCHED]
	LRESULT WINAPI NormalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		BOOL bStop;
		LRESULT lresult;

		if (! ProcessWndProc(&bStop, &lresult, hWnd, msg, wParam, lParam))
		{
			LogErrorNotSet();
			return 0;
		}

		switch (msg)
		{
			case WM_DESTROY:
			case WM_NCDESTROY:
				g_windowData.wndIsExiting = 1;
				break;
		}

		return DefWindowProcA(hWnd, msg, wParam, lParam);
	}

	// FUNCTION: TOY2 0x004A6B10
	void SysParmsOnExit() { SystemParametersInfoA(SPI_SETSCREENSAVERRUNNING, g_sysParamsInfo, &g_sysParamsInfo, 0); }

	// FUNCTION: TOY2 0x00409E30
	BOOL CreateD3DApp(LPSTR lpCmdLine)
	{
		Defaults defaults;
		HRESULT hr;

		if (! D3DAppCreateFromHWND(0, g_windowData.mainHwnd, &Toy2::g_d3dApp))
		{
			LogErrorNotSet();
			return FALSE;
		}

		if (! D3DAppGetRenderState(&defaults.rs))
		{
			LogErrorNotSet();
			return FALSE;
		}
		lstrcpyA(defaults.name, "BUGS");
		defaults.bTexturesDisabled = FALSE;
		defaults.bResizingDisabled = g_windowData.bResizingDisabled;
		defaults.bClearsOn = g_windowData.bClearsOn;
		OverrideDefaults(&defaults);
		g_windowData.bClearsOn = defaults.bClearsOn;
		g_windowData.bResizingDisabled = defaults.bResizingDisabled;
		SetWindowTextA(g_windowData.mainHwnd, defaults.name);

		if (g_renderMode == RENDERMODE_D3D)
		{
			memcpy(&g_windowData.stateCache, &defaults.rs, sizeof(RenderStateCache));
			if (! D3DAppSetRenderState(&g_windowData.stateCache))
			{
				LogErrorNotSet();
				return FALSE;
			}

			hr = g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE)", hr);

			hr = g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, 1);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3DDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, 1)", hr);

			hr = g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1)", hr);

			hr = g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_BOTHSRCALPHA);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_BOTHSRCALPHA)", hr);

			hr = g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_CLAMP);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_CLAMP)", hr);

			hr = g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (DWORD)D3DFILTER_NEAREST);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (DWORD) D3DFILTER_NEAREST)", hr);

			hr = g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (DWORD)D3DFILTER_NEAREST);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (DWORD) D3DFILTER_NEAREST)", hr);

			hr = g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1)", hr);

			hr = g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SUBPIXELX, 1);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SUBPIXELX, 1)", hr);

			hr = g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE)", hr);

			if (g_pcStruct.selectedInterfaceDevice->hwDeviceDesc.dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL)
			{
				g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0x1000);
				g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 1);
				g_d3dAppI.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			}
		}
		return TRUE;
	}

	// FUNCTION: TOY2 0x0040C1F0
	BOOL D3DAppCreateFromHWND(DWORD flags, HWND hwnd, D3DAppInfo** lpD3DApp)
	{
		LPDIRECTDRAW tempDD;
		LPDIRECTDRAWSURFACE tempsurf;
		LPDIRECT3DVIEWPORT2 lpD3DViewport;
		D3DVIEWPORT2 viewData;
		int32_t w, h;
		HDC hdc;
		HRESULT hr;

		if (g_pcStruct.softwareRenderMode)
		{
			g_d3dAppI.bOnlySystemMemory = ! g_pcStruct.defaultDriver->hasHardwareAccel;
		}
		else
		{
			g_d3dAppI.bOnlySystemMemory = TRUE;
			if (g_pcStruct.defaultDriver->hasHardwareAccel || g_pcStruct.selectedInterfaceDevice->isHardwareAccelerated)
				g_d3dAppI.bOnlySystemMemory = FALSE;
		}
		Logger::Log("SYSTEM : OnlySystemMemory status %s.\n", g_d3dAppI.bOnlySystemMemory ? "SYSTEM MEMORY ONLY" : "VIDEO MEMORY ONLY");
		g_d3dAppI.bIsPrimary = g_pcStruct.defaultDriver->isPrimaryDisplay;

		if (g_d3dAppI.lpDD)
		{
			g_d3dAppI.lpDD->Release();
			g_d3dAppI.lpDD = 0;
		}
		hr = DirectDrawCreate(&g_pcStruct.defaultDriver->deviceGUID, &tempDD, 0);
		if (hr < 0)
			Logger::LogDDError("DirectDrawCreate(&PC.DD->Guid, &tempDD, 0)", hr);
		hr = tempDD->QueryInterface(IID_IDirectDraw2, (LPVOID*)&g_d3dAppI.lpDD);
		if (hr < 0)
			Logger::LogDDError("tempDD->QueryInterface(IID_IDirectDraw2, (LPVOID*)&d3dappi.lpDD)", hr);
		if (tempDD)
			tempDD->Release();

		hdc = GetDC(0);
		GetSystemPaletteEntries(hdc, 0, (1 << 8), (LPPALETTEENTRY)(&g_originalPpe[0]));
		memcpy(g_ppe, g_originalPpe, sizeof(g_ppe));
		ReleaseDC(0, hdc);

		D3DAppIRememberWindowsMode();

		if (! g_pcStruct.softwareRenderMode)
		{
			hr = g_d3dAppI.lpDD->QueryInterface(IID_IDirect3D2, (LPVOID*)&g_d3dAppI.lpD3D);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpDD->QueryInterface(IID_IDirect3D2, (LPVOID *) &d3dappi.lpD3D)", hr);
		}

		if (! g_pcStruct.fullscreenMode)
		{
			D3DAppIGetClientWin(hwnd);
			D3DAppISetCoopLevel(hwnd, FALSE);
			w = g_d3dAppI.szClient.cx;
			h = g_d3dAppI.szClient.cy;
			if (! D3DAppICreateBuffers(hwnd, w, h, D3DAPP_BOGUS, FALSE, g_pcStruct.defaultDriver->hasHardwareAccel))
				goto LBL_EXIT_WITH_ERROR;
		}
		else
		{
			w = g_pcStruct.mode->width;
			h = g_pcStruct.mode->height;
			Logger::Log("D3DAPPCREATE : Mode set to %dx%d.\n", w, h);
			D3DAppIGetClientWin(hwnd);
			D3DAppISetCoopLevel(hwnd, TRUE);
			g_szLastClient = g_d3dAppI.szClient;
			g_d3dAppI.szClient.cx = w;
			g_d3dAppI.szClient.cy = h;
			g_changingCoopLevel = TRUE;
			hr = g_d3dAppI.lpDD->SetDisplayMode(w, h, g_pcStruct.mode->rgbBitCount, 0, 0);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpDD->SetDisplayMode(w, h, PC.Mode->bpp, 0, 0)", hr);
			Logger::Log("MODE: Width = %d Height = %d Bpp = %d.\n", w, h, g_pcStruct.mode->rgbBitCount);
			g_changingCoopLevel = FALSE;
			if (! D3DAppICreateBuffers(hwnd, w, h, g_pcStruct.mode->rgbBitCount, TRUE, g_pcStruct.defaultDriver->hasHardwareAccel))
				goto LBL_EXIT_WITH_ERROR;
		}

		if (! D3DAppICheckForPalettized())
			goto LBL_EXIT_WITH_ERROR;

		if (! g_pcStruct.softwareRenderMode)
		{
			if (! D3DAppICreateZBuffer(w, h))
				goto LBL_EXIT_WITH_ERROR;

			if (g_d3dAppI.lpD3DDevice)
			{
				g_d3dAppI.lpD3DDevice->Release();
				g_d3dAppI.lpD3DDevice = 0;
			}
			if (g_pcStruct.selectedInterfaceDevice->isHardwareAccelerated && ! g_d3dAppI.bBackBufferInVideo)
				Logger::LogLn("Could not fit the rendering surfaces in video memory for this hardware device.\n");
			g_pcStruct.selectedInterfaceDevice->numTextureFormats = 0;
			hr = g_d3dAppI.lpBackBuffer->QueryInterface(IID_IDirectDrawSurface, (void**)&tempsurf);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpBackBuffer->QueryInterface(IID_IDirectDrawSurface, (void**)&tempsurf)", hr);
			hr = g_d3dAppI.lpD3D->CreateDevice(g_pcStruct.selectedInterfaceDevice->guid, tempsurf, &g_d3dAppI.lpD3DDevice);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3D->CreateDevice(PC.D3D->Guid, tempsurf, &d3dappi.lpD3DDevice)", hr);
			if (tempsurf)
				tempsurf->Release();
			if (g_pcStruct.selectedInterfaceDevice->hasTexturing)
			{
				hr = g_d3dAppI.lpD3DDevice->EnumTextureFormats(EnumTextureFormatsCallback, (LPVOID)g_pcStruct.selectedInterfaceDevice);
				if (hr < 0)
					Logger::LogDDError("d3dappi.lpD3DDevice->EnumTextureFormats(&CreateD3DEnumTextureFormatsCallback, (LPVOID) PC.D3D)", hr);
			}

			hr = g_d3dAppI.lpD3D->CreateViewport(&lpD3DViewport, 0);
			if (hr != D3D_OK)
			{
				Logger::LogLn("Create D3D viewport failed.\n%s", Logger::ErrorToMessage(hr));
				CleanUpAndPostQuit();
			}
			else
			{
				hr = g_d3dAppI.lpD3DDevice->AddViewport(lpD3DViewport);
				if (hr != D3D_OK)
				{
					Logger::LogLn("Add D3D viewport failed.\n%s", Logger::ErrorToMessage(hr));
					CleanUpAndPostQuit();
				}
				else
				{
					memset(&viewData, 0, sizeof(D3DVIEWPORT2));
					viewData.dwSize = sizeof(D3DVIEWPORT2);
					viewData.dwX = viewData.dwY = 0;
					viewData.dwWidth = w;
					viewData.dwHeight = h;
					viewData.dvClipX = -1.0f;
					viewData.dvClipWidth = 2.0f;
					viewData.dvClipHeight = h * 2.0f / w;
					viewData.dvClipY = viewData.dvClipHeight / 2.0f;
					viewData.dvMinZ = 0.0f;
					viewData.dvMaxZ = 1.0f;
					hr = lpD3DViewport->SetViewport2(&viewData);
					if (hr != D3D_OK)
					{
						Logger::LogLn("SetViewport failed.\n%s", Logger::ErrorToMessage(hr));
						CleanUpAndPostQuit();
					}
					else
					{
						g_d3dAppI.lpD3DViewport = lpD3DViewport;
						InitFontAndTextBuffers();
					}
				}
			}
		}

		UnkFunc409C80();
		if (! D3DAppISetRenderState())
			goto LBL_EXIT_WITH_ERROR;

		g_readyForRender = TRUE;
		g_d3dAppI.bRenderingIsOK = TRUE;
		return TRUE;

	LBL_EXIT_WITH_ERROR:
		ReleaseFontAndTextBuffers();
		if (g_d3dAppI.lpD3DViewport)
		{
			g_d3dAppI.lpD3DDevice->DeleteViewport(g_d3dAppI.lpD3DViewport);
			g_d3dAppI.lpD3DViewport->Release();
			g_d3dAppI.lpD3DViewport = 0;
		}
		if (g_d3dAppI.lpD3DDevice)
		{
			g_d3dAppI.lpD3DDevice->Release();
			g_d3dAppI.lpD3DDevice = 0;
		}
		if (g_d3dAppI.lpZBuffer)
		{
			g_d3dAppI.lpZBuffer->Release();
			g_d3dAppI.lpZBuffer = 0;
		}
		if (g_lpPalette)
		{
			g_lpPalette->Release();
			g_lpPalette = 0;
		}
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
		if (g_pcStruct.fullscreenMode)
		{
			D3DAppIRestoreDispMode();
			D3DAppISetCoopLevel(hwnd, FALSE);
		}
		if (g_d3dAppI.lpD3D)
		{
			g_d3dAppI.lpD3D->Release();
			g_d3dAppI.lpD3D = 0;
		}
		if (g_d3dAppI.lpDD)
		{
			g_d3dAppI.lpDD->Release();
			g_d3dAppI.lpDD = 0;
		}
		return FALSE;
	}

	// FUNCTION: TOY2 0x0040CD20 [MATCHED]
	BOOL D3DAppSetRenderState(RenderStateCache* lpState)
	{
		if (! lpState)
			lpState = &g_d3dAppRS;
		if (lpState != &g_d3dAppRS)
			memcpy(&g_d3dAppRS, lpState, sizeof(RenderStateCache));
		if (g_d3dAppI.bRenderingIsOK)
		{
			if (! D3DAppISetRenderState())
				return FALSE;
		}
		return TRUE;
	}

	// FUNCTION: TOY2 0x0040CD60 [MATCHED]
	BOOL D3DAppGetRenderState(RenderStateCache* lpState)
	{
		memcpy(lpState, &g_d3dAppRS, sizeof(RenderStateCache));
		return TRUE;
	}

	// FUNCTION: TOY2 0x0040CF10 [MATCHED]
	BOOL D3DAppClearBackBuffer(DWORD flags)
	{
		if (! g_d3dAppI.bRenderingIsOK)
		{
			Logger::LogErrorNotSetLn("Cannot call D3DAppClearBackBuffer while bRenderingIsOK is FALSE.\n");
			return FALSE;
		}
		if (g_d3dAppRS.zWriteEnable)
		{
			int32_t clearflags;
			D3DRECT dummy;
			HRESULT hr;

			clearflags = D3DCLEAR_ZBUFFER;
			dummy.x1 = dummy.y1 = 0;
			dummy.x2 = g_d3dAppI.szClient.cx;
			dummy.y2 = g_d3dAppI.szClient.cy;
			hr = g_d3dAppI.lpD3DViewport->SetBackground(g_d3dAppI.lpGroundMatHandle);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3DViewport->SetBackground(d3dappi.lpGroundMatHandle)", hr);
			hr = g_d3dAppI.lpD3DViewport->Clear(1, &dummy, clearflags);
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpD3DViewport->Clear(1, &dummy, clearflags)", hr);
		}
		return TRUE;
	}

	// FUNCTION: TOY2 0x0040CFB0 [MATCHED]
	BOOL D3DAppCheckForLostSurfaces(void)
	{
		int32_t i;
		BOOL b = FALSE;

		if (g_d3dAppI.lpFrontBuffer)
		{
			if (g_d3dAppI.lpFrontBuffer->IsLost() == DDERR_SURFACELOST)
			{
				g_lastError = g_d3dAppI.lpFrontBuffer->Restore();
				if (g_lastError != DD_OK)
					goto LBL_EXIT_WITH_ERROR;
				b = TRUE;
			}
		}
		if (g_d3dAppI.lpBackBuffer)
		{
			if (g_d3dAppI.lpBackBuffer->IsLost() == DDERR_SURFACELOST)
			{
				g_lastError = g_d3dAppI.lpBackBuffer->Restore();
				if (g_lastError != DD_OK)
					goto LBL_EXIT_WITH_ERROR;
				b = TRUE;
			}
		}
		if (g_d3dAppI.lpZBuffer)
		{
			if (g_d3dAppI.lpZBuffer->IsLost() == DDERR_SURFACELOST)
			{
				g_lastError = g_d3dAppI.lpZBuffer->Restore();
				if (g_lastError != DD_OK)
					goto LBL_EXIT_WITH_ERROR;
				b = TRUE;
			}
		}
		if (b)
		{
			Logger::LogLn("Lost surface - CheckForLostSurfaces.\n");
			SoftwareRenderer::ClearRenderSurfaces();
		}
		for (i = 0; i < D3DAPP_MAXTEXTURES; i++)
		{
			b = FALSE;
			if (g_d3dAppI.lpTextureSurf[i])
			{
				if (g_d3dAppI.lpTextureSurf[i]->IsLost() == DDERR_SURFACELOST)
				{
					g_lastError = g_d3dAppI.lpTextureSurf[i]->Restore();
					if (g_lastError != DD_OK)
						goto LBL_EXIT_WITH_ERROR;
					b = TRUE;
				}
			}
			if (b)
			{
				Logger::LogLn("Lost surface %d.\n", i);
			}
		}
		return TRUE;

	LBL_EXIT_WITH_ERROR:
		Logger::LogLn("Restoring of a lost surface failed.\n%s", Logger::ErrorToMessage(g_lastError));
		return FALSE;
	}

	// FUNCTION: TOY2 0x0040D0D0
	BOOL D3DAppPause(BOOL flag)
	{
		// GLOBAL: TOY2 0x00528158
		static int32_t g_pauseCount;
		HRESULT hr;

		if (g_pauseCount != 0)
		{
			if (flag)
			{
				++g_pauseCount;
				return TRUE;
			}
			else
			{
				--g_pauseCount;
				if (g_pauseCount != 0)
					return TRUE;
			}
		}

		g_d3dAppI.bPaused = flag;
		if (! flag)
		{
			if (g_pcStruct.fullscreenMode && g_primaryPalettized && g_lpPalette)
			{
				hr = g_lpPalette->SetEntries(0, 0, 256, &g_ppe[0]);
				if (hr < 0)
					Logger::LogDDError("lpPalette->SetEntries(0, 0, 256, &ppe[0])", hr);
			}
		}
		if (flag && g_pcStruct.fullscreenMode)
		{
			if (g_primaryPalettized && g_lpPalette)
			{
				hr = g_lpPalette->GetEntries(0, 0, 256, &g_ppe[0]);
				if (hr < 0)
					Logger::LogDDError("lpPalette->GetEntries(0, 0, 256, &ppe[0])", hr);
				memcpy(&g_originalPpe[10], &g_ppe[10], sizeof(PALETTEENTRY) * (246 - 10));
				hr = g_lpPalette->SetEntries(0, 0, 256, &g_originalPpe[0]);
				if (hr < 0)
					Logger::LogDDError("lpPalette->SetEntries(0, 0, 256, &Originalppe[0])", hr);
			}
			if (g_d3dAppI.lpDD)
			{
				hr = g_d3dAppI.lpDD->FlipToGDISurface();
				if (hr < 0)
					Logger::LogDDError("d3dappi.lpDD->FlipToGDISurface()", hr);
			}
			DrawMenuBar(g_d3dAppI.hwnd);
			RedrawWindow(g_d3dAppI.hwnd, 0, 0, RDW_FRAME);
		}
		return TRUE;
	}

	// FUNCTION: TOY2 0x0040D2B0 [MATCHED]
	HRESULT D3DAppLastError(void) { return g_lastError; }

	// FUNCTION: TOY2 0x0040D2D0
	BOOL D3DAppDestroy(void)
	{
		int16_t i;

		g_d3dAppI.bRenderingIsOK = FALSE;
		g_d3dAppI.hwnd = 0;
		ReleaseFontAndTextBuffers();
		if (g_d3dAppI.lpD3DViewport)
		{
			g_d3dAppI.lpD3DDevice->DeleteViewport(g_d3dAppI.lpD3DViewport);
			g_d3dAppI.lpD3DViewport->Release();
			g_d3dAppI.lpD3DViewport = 0;
		}
		if (g_renderMode == RENDERMODE_D3D)
		{
			for (i = 0; i < D3DAPP_MAXTEXTURES; i++)
			{
				if (g_d3dAppI.textureState[i] && g_d3dAppI.textureState[i] != 4)
					g_d3dAppI.textureState[i] = 5;
			}
			UnkFunc409C80();
		}
		if (g_d3dAppI.lpD3DDevice)
		{
			g_d3dAppI.lpD3DDevice->Release();
			g_d3dAppI.lpD3DDevice = 0;
		}
		if (g_d3dAppI.lpZBuffer)
		{
			g_d3dAppI.lpZBuffer->Release();
			g_d3dAppI.lpZBuffer = 0;
		}
		if (g_lpPalette)
		{
			g_lpPalette->Release();
			g_lpPalette = 0;
		}
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
		if (g_pcStruct.fullscreenMode)
		{
			D3DAppIRestoreDispMode();
			D3DAppISetCoopLevel(g_d3dAppI.hwnd, FALSE);
		}
		if (g_d3dAppI.lpD3D)
		{
			g_d3dAppI.lpD3D->Release();
			g_d3dAppI.lpD3D = 0;
		}
		if (g_d3dAppI.lpDD)
		{
			g_d3dAppI.lpDD->Release();
			g_d3dAppI.lpDD = 0;
		}
		return TRUE;
	}

	// FUNCTION: TOY2 0x00497D70 [MATCHED]
	void OverrideDefaults(Defaults* defaults)
	{
		lstrcpyA(defaults->name, "Loading - A Bugs Life");
		defaults->rs.texturePerspective = TRUE;
		defaults->rs.ditherEnable = TRUE;
		g_windowData.bShowInfo = FALSE;
		g_windowData.bShowFrameRate = TRUE;
		defaults->rs.specularEnable = FALSE;
	}

	// FUNCTION: TOY2 0x00431820 [MATCHED]
	BOOL RestoreSurfaces()
	{
		HRESULT d3drval;

		if (! D3DAppCheckForLostSurfaces())
		{
			return FALSE;
		}
		if (g_windowData.unusedDDSurface->IsLost() == DDERR_SURFACELOST)
		{
			d3drval = g_windowData.unusedDDSurface->Restore();
			if (d3drval != DD_OK)
			{
				return FALSE;
			}
		}
		if (g_windowData.unusedDDSurface2->IsLost() == DDERR_SURFACELOST)
		{
			d3drval = g_windowData.unusedDDSurface2->Restore();
			if (d3drval != DD_OK)
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	// FUNCTION: TOY2 0x004318E0 [MATCHED]
	void CleanUpAndPostQuit(void) {}

	// FUNCTION: TOY2 0x004A6A90 [MATCHED]
	void ShowLastError()
	{
		LPVOID lpMsgBuf;

		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, 0);
		MessageBoxA(0, (LPCSTR)lpMsgBuf, "GetLastError", MB_OK | MB_ICONINFORMATION);
		LocalFree(lpMsgBuf);
	}
}

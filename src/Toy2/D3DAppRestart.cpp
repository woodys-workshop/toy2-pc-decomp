#include "Toy2/D3DAppI.h"
#include "SoftwareRenderer.h"
#include "Nu3D/Camera.h"
#include "Logger.h"

namespace SoftwareRenderer
{
	// GLOBAL: TOY2 0x00882778
	int32_t g_nativePixelPitch;

	// GLOBAL: TOY2 0x0088277C
	int32_t g_nativeScreenScaleX;

	// GLOBAL: TOY2 0x00882780
	int32_t g_nativeScreenScale;

	// GLOBAL: TOY2 0x008828C8
	int32_t g_nativeHalfScreenWidth;

	// GLOBAL: TOY2 0x008828D0
	int32_t g_nativeHalfScreenHeight;

	// GLOBAL: TOY2 0x008828F8
	int32_t g_nativeScreenScaleXCopy;

	// GLOBAL: TOY2 0x008828FC
	int32_t g_nativeScreenScaleY;
}

namespace Toy2
{
	// FUNCTION: TOY2 0x00499A40 [MATCHED]
	void DestroyDirect3DRenderer() { Logger::Log("QUIT : Destroying Direct3D renderer.\n"); }

	// FUNCTION: TOY2 0x00499BA0
	void S3DSetResolution(int32_t width, int32_t height, int32_t bpp)
	{
		DDSURFACEDESC ddsd;
		DDSCAPS ddscaps;
		LPDIRECTDRAW tempDD;
		HRESULT hr;

		Logger::Log("S3D_SetResolution(%i,%i)...0\n", width, height);
		memset(&ddsd, 0, sizeof(ddsd));
		ddscaps.dwCaps = 0;

		if (D3DApp::g_lpPalette)
		{
			D3DApp::g_lpPalette->Release();
			D3DApp::g_lpPalette = 0;
		}
		if (SoftwareRenderer::g_nativePalette)
		{
			SoftwareRenderer::g_nativePalette->Release();
			SoftwareRenderer::g_nativePalette = 0;
		}
		if (D3DApp::g_d3dAppI.lpBackBuffer)
		{
			hr = D3DApp::g_d3dAppI.lpBackBuffer->Release();
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpBackBuffer->Release()", hr);
			D3DApp::g_d3dAppI.lpBackBuffer = 0;
		}
		if (D3DApp::g_d3dAppI.lpFrontBuffer)
		{
			hr = D3DApp::g_d3dAppI.lpFrontBuffer->Release();
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpFrontBuffer->Release()", hr);
			D3DApp::g_d3dAppI.lpFrontBuffer = 0;
		}
		if (D3DApp::g_d3dAppI.lpDD)
		{
			hr = D3DApp::g_d3dAppI.lpDD->Release();
			if (hr < 0)
				Logger::LogDDError("d3dappi.lpDD->Release()", hr);
			D3DApp::g_d3dAppI.lpDD = 0;
		}

		Nu3D::Camera::g_screenWidth = width;
		Nu3D::Camera::g_screenHeight = height;
		SoftwareRenderer::g_bitsPerPixel = bpp;
		hr = DirectDrawCreate(0, &tempDD, 0);
		if (hr < 0)
			Logger::LogDDError("DirectDrawCreate(0, &tempDD, 0)", hr);
		hr = tempDD->QueryInterface(IID_IDirectDraw2, (LPVOID*)&D3DApp::g_d3dAppI.lpDD);
		if (hr < 0)
			Logger::LogDDError("tempDD->QueryInterface(IID_IDirectDraw2, (LPVOID *) & d3dappi.lpDD)", hr);
		if (tempDD)
		{
			tempDD->Release();
			tempDD = 0;
		}
		hr = D3DApp::g_d3dAppI.lpDD->SetCooperativeLevel(D3DApp::g_d3dAppI.hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX);
		if (hr < 0)
			Logger::LogDDError("d3dappi.lpDD->SetCooperativeLevel(d3dappi.hwnd, 0x00000010l | 0x00000001l | 0x00000040l)", hr);
		hr = D3DApp::g_d3dAppI.lpDD->SetDisplayMode(Nu3D::Camera::g_screenWidth, Nu3D::Camera::g_screenHeight, SoftwareRenderer::g_bitsPerPixel, 0, 0);
		if (hr < 0)
			Logger::LogDDError("d3dappi.lpDD->SetDisplayMode(SWIDTH, SHEIGHT, SBPP, 0, 0)", hr);

		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;
		hr = D3DApp::D3DAppICreateSurface(&ddsd, &D3DApp::g_d3dAppI.lpFrontBuffer);
		if (hr < 0)
			Logger::LogDDError("D3DAppICreateSurface(&ddsd, &d3dappi.lpFrontBuffer)", hr);
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		hr = D3DApp::g_d3dAppI.lpFrontBuffer->GetAttachedSurface(&ddscaps, &D3DApp::g_d3dAppI.lpBackBuffer);
		if (hr < 0)
			Logger::LogDDError("d3dappi.lpFrontBuffer->GetAttachedSurface(&ddscaps, &d3dappi.lpBackBuffer)", hr);

		Logger::Log("SWIDTH=%i\n", Nu3D::Camera::g_screenWidth);
		Logger::Log("SHEIGHT=%i\n", Nu3D::Camera::g_screenHeight);
		Logger::Log("TWIDTH=%i\n", SoftwareRenderer::g_nativePixelPitch);
		Logger::Log("SBPP=%i\n", SoftwareRenderer::g_bitsPerPixel);
		D3DApp::g_d3dAppI.szClient.cx = Nu3D::Camera::g_screenWidth;
		D3DApp::g_d3dAppI.szClient.cy = Nu3D::Camera::g_screenHeight;
		SoftwareRenderer::g_nativeHalfScreenWidth = Nu3D::Camera::g_screenWidth / 2;
		SoftwareRenderer::g_nativeHalfScreenHeight = Nu3D::Camera::g_screenHeight / 2;
		SoftwareRenderer::g_nativeScreenScale = Nu3D::Camera::g_screenWidth / 320;
		SoftwareRenderer::g_nativeScreenScaleX = (Nu3D::Camera::g_screenWidth << 8) / 320;
		SoftwareRenderer::g_nativeScreenScaleXCopy = SoftwareRenderer::g_nativeScreenScaleX;
		SoftwareRenderer::g_nativeScreenScaleY = (Nu3D::Camera::g_screenHeight * 256) / 240;
		InitSoftwareRenderer();
		Logger::Log("S3D_SetResolution(%i,%i)...1\n", width, height);
	}
}

namespace D3DApp
{
	// GLOBAL: TOY2 0x0050A590
	uint8_t* g_tpageDataBackup[D3DAPP_MAXTEXTURES];

	// GLOBAL: TOY2 0x0051A840
	int32_t g_textureStateBackup[D3DAPP_MAXTEXTURES];

	// GLOBAL: TOY2 0x0051A944
	int16_t g_unk50A690Backup[D3DAPP_MAXTEXTURES];

	// GLOBAL: TOY2 0x0051AACC
	int32_t g_textureTypeBackup[D3DAPP_MAXTEXTURES];

	// GLOBAL: TOY2 0x0051AFB4
	int32_t g_tpageIs8BitBackup[D3DAPP_MAXTEXTURES];

	// FUNCTION: TOY2 0x0040A0C0
	BOOL D3DRestartShutdown()
	{
		Logger::Log("%s\n", g_renderMode == RENDERMODE_D3D ? "RENDER_D3D" : "RENDER_SOFT");

		for (int32_t i = 0; i < D3DAPP_MAXTEXTURES; i++)
		{
			if (g_d3dAppI.textureState[i] == 3)
				g_d3dAppI.textureState[i] = 4;
		}
		memcpy(g_textureStateBackup, g_d3dAppI.textureState, sizeof(g_textureStateBackup));
		memcpy(g_textureTypeBackup, g_d3dAppI.textureType, sizeof(g_textureTypeBackup));
		memcpy(g_tpageDataBackup, g_tpageData, sizeof(g_tpageDataBackup));
		memcpy(g_tpageIs8BitBackup, g_tpageIs8Bit, sizeof(g_tpageIs8BitBackup));
		memcpy(g_unk50A690Backup, g_unk50A690, sizeof(g_unk50A690Backup));

		switch (g_renderMode)
		{
			case RENDERMODE_SOFTWARE:
				SoftwareRenderer::Destroy();
				break;
			case RENDERMODE_D3D:
				Toy2::DestroyDirect3DRenderer();
				break;
		}

		g_d3dAppI.bRenderingIsOK = FALSE;
		if (g_renderMode == RENDERMODE_D3D)
		{
			ReleaseFontAndTextBuffers();
			if (g_d3dAppI.lpD3DViewport)
			{
				g_d3dAppI.lpD3DDevice->DeleteViewport(g_d3dAppI.lpD3DViewport);
				g_d3dAppI.lpD3DViewport->Release();
				g_d3dAppI.lpD3DViewport = 0;
			}
			UnkFunc40A870();
		}
		if (g_d3dAppI.lpSkyMat)
		{
			g_d3dAppI.lpSkyMat->Release();
			g_d3dAppI.lpSkyMat = 0;
		}
		if (g_d3dAppI.lpGroundMat)
		{
			g_d3dAppI.lpGroundMat->Release();
			g_d3dAppI.lpGroundMat = 0;
		}
		if (g_d3dAppI.lpD3DDevice)
		{
			g_d3dAppI.lpD3DDevice->Release();
			g_d3dAppI.lpD3DDevice = 0;
		}
		if (g_lpClipper)
		{
			g_lpClipper->Release();
			g_lpClipper = 0;
		}
		if (g_d3dAppI.lpZBuffer)
		{
			g_d3dAppI.lpBackBuffer->DeleteAttachedSurface(0, g_d3dAppI.lpZBuffer);
			if (g_d3dAppI.lpZBuffer)
			{
				g_d3dAppI.lpZBuffer->Release();
				g_d3dAppI.lpZBuffer = 0;
			}
		}
		if (g_lpPalette)
		{
			g_lpPalette->Release();
			g_lpPalette = 0;
		}
		if (SoftwareRenderer::g_nativePalette)
		{
			SoftwareRenderer::g_nativePalette->Release();
			SoftwareRenderer::g_nativePalette = 0;
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
		if (g_renderMode == RENDERMODE_D3D)
		{
			if (g_d3dAppI.lpD3D)
			{
				g_d3dAppI.lpD3D->Release();
				g_d3dAppI.lpD3D = 0;
			}
		}
		if (g_d3dAppI.lpDD)
		{
			g_d3dAppI.lpDD->Release();
			g_d3dAppI.lpDD = 0;
		}

		Logger::Log("%s\n", g_renderMode == RENDERMODE_D3D ? "RENDER_D3D" : "RENDER_SOFT");
		return TRUE;
	}

	// FUNCTION: TOY2 0x0040A350 [MATCHED]
	BOOL D3DRestartStartMode()
	{
		Logger::Log("D3DRESTART : Starting mode %dx%d.\n", g_pcStruct.mode->width, g_pcStruct.mode->height);
		CreateD3DApp(g_windowData.lpCmdLine);
		Logger::Log("%s\n", g_renderMode == RENDERMODE_D3D ? "RENDER_D3D" : "RENDER_SOFT");

		switch (g_renderMode)
		{
			case RENDERMODE_SOFTWARE:
				Toy2::InitSoftwareRenderer();
				break;
			case RENDERMODE_D3D:
				Toy2::InitDirect3DRenderer();
				break;
		}
		memcpy(g_d3dAppI.textureState, g_textureStateBackup, sizeof(g_textureStateBackup));
		memcpy(g_d3dAppI.textureType, g_textureTypeBackup, sizeof(g_textureTypeBackup));
		memcpy(g_tpageData, g_tpageDataBackup, sizeof(g_tpageDataBackup));
		memcpy(g_tpageIs8Bit, g_tpageIs8BitBackup, sizeof(g_tpageIs8BitBackup));
		memcpy(g_unk50A690, g_unk50A690Backup, sizeof(g_unk50A690Backup));

		for (int32_t i = 0; i < D3DAPP_MAXTEXTURES; i++)
		{
			if (g_d3dAppI.textureState[i] == 4)
				g_d3dAppI.textureState[i] = 1;
		}
		UnkFunc409C80();
		InitTextureMaterials();
		return TRUE;
	}

	// FUNCTION: TOY2 0x0040A440 [MATCHED]
	BOOL D3DRestart()
	{
		D3DRestartShutdown();
		D3DRestartStartMode();
		return TRUE;
	}
}

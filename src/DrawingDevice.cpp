#include "DrawingDevice.h"
#include "Nu3D/BmpDataNode.h"
#include "NGNLoader/NGNLoader.h"
#include "SoftwareRenderer.h"
#include "Common.h"

#include <STDIO.H>

namespace DrawingDevice
{
	// GLOBAL: TOY2 0x00884008
	CD3DFramework* g_drawingDevice;

	// GLOBAL: TOY2 0x00884028
	DDAppDevice::App* g_ddAppListHead;

	// GLOBAL: TOY2 0x00884030
	DDAppDevice::App* g_primaryDDApp;

	// GLOBAL: TOY2 0x00884010
	D3DMATRIX* g_curWorldTransform;

	// GLOBAL: TOY2 0x005281D4
	int32_t g_validDrawDevice;

	// GLOBAL: TOY2 0x005281D0
	LPDIRECT3DDEVICE3 g_unusedD3D;

	// GLOBAL: TOY2 0x0088403C
	int32_t g_viewportTopOffset;

	// GLOBAL: TOY2 0x00884040
	int32_t g_viewportBottomOffset;

	// GLOBAL: TOY2 0x00E4D8E4
	int32_t g_setTexCalls;

	/* ------ CD3DFramework ------- */

	// FUNCTION: TOY2 0x004AEC80
	CD3DFramework::CD3DFramework()
	{
		m_hWnd = 0;
		m_bIsFullscreen = 0;
		m_dwRenderWidth = 0;
		m_dwRenderHeight = 0;
		m_pddsFrontBuffer = 0;
		m_pddsBackBuffer = 0;
		m_pddsRenderTarget = 0;
		m_pddsZBuffer = 0;
		m_pd3dDevice = 0;
		m_pvViewport = 0;
		m_pDD = 0;
		m_pD3D = 0;
		m_dwDeviceMemType = 0;
		m_initialized = 0;

		memset(m_slots, 0, sizeof(m_slots));
	}

	// FUNCTION: TOY2 0x004AECD0
	void CD3DFramework::Release() { Cleanup(); }

	// FUNCTION: TOY2 0x004AECE0
	int32_t CD3DFramework::Cleanup()
	{
		ULONG drawReleaseResult = 0;
		ULONG deviceReleaseResult = 0;

		if (m_pvViewport)
		{
			m_pvViewport->Release();
			m_pvViewport = 0;
		}

		if (m_pd3dDevice)
			deviceReleaseResult = m_pd3dDevice->Release();

		m_pd3dDevice = 0;

		if (! m_bIsFullscreen)
		{
			if (m_pddsBackBuffer)
			{
				m_pddsBackBuffer->Release();
				m_pddsBackBuffer = 0;
			}
		}

		if (m_pddsRenderTarget)
		{
			m_pddsRenderTarget->Release();
			m_pddsRenderTarget = 0;
		}

		if (m_pddsZBuffer)
		{
			m_pddsZBuffer->Release();
			m_pddsZBuffer = 0;
		}

		if (m_pddsFrontBuffer)
		{
			m_pddsFrontBuffer->Release();
			m_pddsFrontBuffer = 0;
		}

		if (m_pD3D)
		{
			m_pD3D->Release();
			m_pD3D = 0;
		}

		if (m_pDD)
		{
			m_pDD->SetCooperativeLevel(m_hWnd, 8);
			drawReleaseResult = m_pDD->Release();
		}

		m_pDD = 0;

		if (drawReleaseResult || deviceReleaseResult)
			return 0x8200000B;
		else
			return 0;
	}

	// FUNCTION: TOY2 0x004AEDA0
	HRESULT CD3DFramework::InitalizeForWindow(HWND hWnd, GUID* ddAppGuid, DDAppDevice* device, DDAppDevice::DisplayMode* displayMode, uint8_t flags)
	{
		if (! hWnd || ! displayMode && (flags & 1) != 0)
			return DDERR_INVALIDPARAMS;

		m_hWnd = hWnd;
		m_bIsFullscreen = flags & 1;

		HRESULT result = InitalizeDeviceAndSurfaces(ddAppGuid, &device->guid, displayMode, flags);

		if ((result & 0x80000000) != 0)
		{
			Cleanup();

			if (result == DDERR_GENERIC)
				return 0x82000000;
		}

		return result;
	}

	// FUNCTION: TOY2 0x004AEE10
	HRESULT CD3DFramework::InitalizeDeviceAndSurfaces(GUID* ddAppGuid, GUID* deviceGuid, DDAppDevice::DisplayMode* displayMode, uint8_t flags)
	{
		HRESULT result = CreateDirectDraw(ddAppGuid, flags);

		if (FAILED(result))
			return result;

		if (! deviceGuid || (result = SelectD3DDeviceAndZFormat(deviceGuid, flags), SUCCEEDED(result)))
		{
			result = CreatePrimaryChainAndRects(displayMode, flags);

			if (SUCCEEDED(result))
			{
				if (! deviceGuid)
					return 0;

				if ((flags & 4) == 0 || (result = CreateZBuffer(), SUCCEEDED(result)))
				{
					result = CreateD3DDevice(deviceGuid);

					if (SUCCEEDED(result))
					{
						result = CreateAndSetViewport();

						if (SUCCEEDED(result))
						{
							if (m_initialized)
								return 0x82000000;

							m_slots[0].width = m_dwRenderWidth;
							m_slots[0].height = m_dwRenderHeight;
							m_slots[0].surface1 = m_pddsBackBuffer;
							m_slots[0].valid = 1;
							m_slots[0].surface2 = m_pddsZBuffer;

							m_initialized = 1;

							return 0;
						}
					}
				}
			}
		}

		return result;
	}

	// FUNCTION: TOY2 0x004AEEE0
	HRESULT CD3DFramework::CreateDirectDraw(LPGUID lpGUID, uint8_t flags)
	{
		LPDIRECTDRAW lpDD;
		if (DirectDrawCreate(lpGUID, &lpDD, 0) < 0)
			return 0x82000001;

		if (lpDD->QueryInterface(IID_IDirectDraw4, (LPVOID*)&m_pDD) >= 0)
		{
			lpDD->Release();

			DWORD coopLevel = DDSCL_NORMAL;

			if (m_bIsFullscreen)
				coopLevel = DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT;

			if ((flags & 16) == 0)
				coopLevel |= DDSCL_FPUSETUP;

			return m_pDD->SetCooperativeLevel(m_hWnd, coopLevel) >= 0 ? 0 : 0x82000002;
		}
		else
		{
			lpDD->Release();
			return 0x82000001;
		}
	}

	// FUNCTION: TOY2 0x004AEF80
	HRESULT CD3DFramework::SelectD3DDeviceAndZFormat(GUID* deviceGuid, uint8_t flags)
	{
		if (m_pDD->QueryInterface(IID_IDirect3D3, (LPVOID*)&m_pD3D) < 0)
			return 0x82000003;

		D3DFINDDEVICESEARCH findDevSearch;
		D3DFINDDEVICERESULT findDevResult;

		memset(&findDevResult, 0, sizeof(findDevResult));
		memset(&findDevSearch, 0, sizeof(findDevSearch));

		findDevResult.dwSize = sizeof(D3DFINDDEVICERESULT);

		findDevSearch.guid = *deviceGuid;
		findDevSearch.dwSize = sizeof(D3DFINDDEVICESEARCH);
		findDevSearch.dwFlags = D3DFDS_GUID;

		if (m_pD3D->FindDevice(&findDevSearch, &findDevResult) < 0)
			return 0x82000003;

		D3DDEVICEDESC* ddHwDesc;
		D3DDEVICEDESC* d3dDesc;

		if (findDevResult.ddHwDesc.dwFlags)
		{
			m_dwDeviceMemType = DDSCAPS_VIDEOMEMORY;

			d3dDesc = &m_ddDeviceDesc;
			ddHwDesc = &findDevResult.ddHwDesc;
		}
		else
		{
			m_dwDeviceMemType = DDSCAPS_SYSTEMMEMORY;

			d3dDesc = &m_ddDeviceDesc;
			ddHwDesc = &findDevResult.ddSwDesc;
		}

		memcpy(d3dDesc, ddHwDesc, sizeof(D3DDEVICEDESC));
		memset(&m_ddpfZBuffer, 0, sizeof(m_ddpfZBuffer));

		if ((flags & 8) != 0)
			m_ddpfZBuffer.dwFlags = DDPF_ZBUFFER | DDPF_STENCILBUFFER;
		else
			m_ddpfZBuffer.dwFlags = DDPF_ZBUFFER;

		m_pD3D->EnumZBufferFormats(*deviceGuid, EnumZBufferFormats, &m_ddpfZBuffer);

		return m_ddpfZBuffer.dwSize != sizeof(DDPIXELFORMAT) ? 0x82000005 : 0;
	}

	// FUNCTION: TOY2 0x004AF110
	HRESULT CD3DFramework::CreatePrimaryChainAndRects(DDAppDevice::DisplayMode* displayMode, uint8_t flags)
	{
		DDSURFACEDESC2 d3dDesc;
		HRESULT surfaceResult;

		if ((flags & 1) != 0)
		{
			SetRect(&m_rcViewportRect, 0, 0, displayMode->surfaceDesc.dwWidth, displayMode->surfaceDesc.dwHeight);

			m_rcScreenRect.left = m_rcViewportRect.left;
			m_rcScreenRect.top = m_rcViewportRect.top;

			DWORD dmFlags = 0;

			m_rcScreenRect.right = m_rcViewportRect.right;
			m_dwRenderWidth = m_rcViewportRect.right;

			m_rcScreenRect.bottom = m_rcViewportRect.bottom;
			m_dwRenderHeight = m_rcViewportRect.bottom;

			if (m_rcViewportRect.right == 320 && m_rcViewportRect.bottom == 200)
				dmFlags = displayMode->surfaceDesc.ddpfPixelFormat.dwRGBBitCount == 8;

			if (m_pDD->SetDisplayMode(
					m_rcViewportRect.right,
					m_rcViewportRect.bottom,
					displayMode->surfaceDesc.ddpfPixelFormat.dwRGBBitCount,
					displayMode->surfaceDesc.dwRefreshRate,
					dmFlags)
				< 0)
				return 0x82000009;

			InitSurfaceDesc(&d3dDesc, DDSD_CAPS, 0);

			d3dDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

			if ((flags & 2) != 0)
			{
				d3dDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
				d3dDesc.dwBackBufferCount = 1;
				d3dDesc.dwFlags |= D3DDD_LINECAPS;
			}

			HRESULT frontBufferResult = m_pDD->CreateSurface(&d3dDesc, &m_pddsFrontBuffer, 0);

			if (frontBufferResult < 0)
				return frontBufferResult != DDERR_OUTOFVIDEOMEMORY ? 0x82000007 : DDERR_OUTOFVIDEOMEMORY;

			if ((flags & 2) == 0)
			{
				m_pddsRenderTarget = m_pddsFrontBuffer;
				m_pddsRenderTarget->AddRef();
				return 0;
			}

			DDSCAPS2 ddsCaps;
			ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

			surfaceResult = m_pddsFrontBuffer->GetAttachedSurface(&ddsCaps, &m_pddsBackBuffer);
		}
		else
		{
			GetClientRect(m_hWnd, &m_rcViewportRect);

			m_rcViewportRect.top += DrawingDevice::g_viewportTopOffset;
			m_rcViewportRect.bottom = m_rcViewportRect.bottom - DrawingDevice::g_viewportBottomOffset;

			GetClientRect(m_hWnd, &m_rcScreenRect);

			m_rcScreenRect.bottom = m_rcScreenRect.bottom - DrawingDevice::g_viewportTopOffset;
			m_rcScreenRect.bottom = m_rcScreenRect.bottom - DrawingDevice::g_viewportTopOffset - DrawingDevice::g_viewportBottomOffset;

			ClientToScreen(m_hWnd, (LPPOINT)&m_rcScreenRect);
			ClientToScreen(m_hWnd, (LPPOINT)&m_rcScreenRect.right);

			m_dwRenderWidth = m_rcViewportRect.right;
			m_dwRenderHeight = m_rcViewportRect.bottom;

			InitSurfaceDesc(&d3dDesc, DDSD_CAPS, 0);
			d3dDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			if ((flags & 2) == 0)
				d3dDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

			HRESULT frontBufferResult = m_pDD->CreateSurface(&d3dDesc, &m_pddsFrontBuffer, 0);

			if (frontBufferResult < 0)
				return frontBufferResult != DDERR_OUTOFVIDEOMEMORY ? 0x82000007 : DDERR_OUTOFVIDEOMEMORY;

			LPDIRECTDRAWCLIPPER clipper;
			if (m_pDD->CreateClipper(0, &clipper, 0) < 0)
				return 0x82000008;

			clipper->SetHWnd(0, m_hWnd);
			m_pddsFrontBuffer->SetClipper(clipper);

			if (clipper)
			{
				clipper->Release();
				clipper = 0;
			}

			if ((flags & 2) == 0)
			{
				ClientToScreen(m_hWnd, (LPPOINT)&m_rcViewportRect);
				ClientToScreen(m_hWnd, (LPPOINT)&m_rcViewportRect.right);
				goto LBL_ASSIGN_RT;
			}

			d3dDesc.dwHeight = m_dwRenderHeight;
			d3dDesc.dwWidth = m_dwRenderWidth;

			d3dDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
			d3dDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

			surfaceResult = m_pDD->CreateSurface(&d3dDesc, &m_pddsBackBuffer, 0);
		}

		if (surfaceResult < 0)
			return surfaceResult != DDERR_OUTOFVIDEOMEMORY ? 0x8200000A : DDERR_OUTOFVIDEOMEMORY;

	LBL_ASSIGN_RT:

		if ((flags & 2) == 0)
		{
			m_pddsRenderTarget = m_pddsFrontBuffer;
			m_pddsRenderTarget->AddRef();
			return 0;
		}

		m_pddsRenderTarget = m_pddsBackBuffer;
		m_pddsRenderTarget->AddRef();

		return 0;
	}

	// FUNCTION: TOY2 0x004AF420
	HRESULT CD3DFramework::CreateZBuffer()
	{
		if ((m_ddDeviceDesc.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ZBUFFERLESSHSR) != 0)
			return 0;

		DDSURFACEDESC2 surfaceDesc;
		InitSurfaceDesc(&surfaceDesc, DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT, 0);

		surfaceDesc.ddsCaps.dwCaps = m_dwDeviceMemType | DDSCAPS_ZBUFFER;
		surfaceDesc.dwWidth = m_dwRenderWidth;
		surfaceDesc.dwHeight = m_dwRenderHeight;

		memcpy(&surfaceDesc.ddpfPixelFormat, &m_ddpfZBuffer, sizeof(surfaceDesc.ddpfPixelFormat));

		HRESULT result = m_pDD->CreateSurface(&surfaceDesc, &m_pddsZBuffer, 0);

		if (result >= 0)
			return m_pddsRenderTarget->AddAttachedSurface(m_pddsZBuffer) >= 0 ? 0 : 0x82000005;
		else
			return result != DDERR_OUTOFVIDEOMEMORY ? 0x82000005 : DDERR_OUTOFVIDEOMEMORY;
	}

	// FUNCTION: TOY2 0x004AF4E0
	HRESULT CD3DFramework::CreateD3DDevice(const CLSID* guid)
	{
		DDSURFACEDESC2 surfaceDesc;
		surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);

		m_pDD->GetDisplayMode(&surfaceDesc);

		if (surfaceDesc.ddpfPixelFormat.dwRGBBitCount > 8)
			return m_pD3D->CreateDevice(*guid, m_pddsRenderTarget, &m_pd3dDevice, 0) >= 0 ? 0 : 0x82000004;
		else
			return 0x8200000D;
	}

	// FUNCTION: TOY2 0x004AF550
	HRESULT CD3DFramework::CreateAndSetViewport()
	{
		D3DVIEWPORT2 viewport2;
		CD3DFramework::BuildViewport(&viewport2, m_dwRenderWidth, m_dwRenderHeight);

		if (m_pD3D->CreateViewport(&m_pvViewport, 0) < 0)
			return 0x82000006;

		if (m_pd3dDevice->AddViewport(m_pvViewport) < 0)
			return 0x82000006;

		if (m_pvViewport->SetViewport2(&viewport2) >= 0)
			return m_pd3dDevice->SetCurrentViewport(m_pvViewport) >= 0 ? 0 : 0x82000006;

		return 0x82000006;
	}

	// FUNCTION: TOY2 0x004AF640
	int32_t CD3DFramework::RestoreToGDISurface(int32_t refreshWindow)
	{
		if (m_pDD)
		{
			if (m_bIsFullscreen)
			{
				m_pDD->FlipToGDISurface();

				if (refreshWindow)
				{
					DrawMenuBar(m_hWnd);
					RedrawWindow(m_hWnd, 0, 0, RDW_FRAME);
				}
			}
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004AFA20
	int32_t CD3DFramework::GetSlotSurfaceByIndex(int32_t index, LPDIRECTDRAWSURFACE4* surfaceOut)
	{
		if (index > 8)
			return 0x8200000F;

		DrawingDeviceSlot* slot = &m_slots[index];

		if (! slot->valid)
			return 0x8200000F;

		int32_t result = 0;

		*surfaceOut = slot->surface1;

		return result;
	}

	// FUNCTION: TOY2 0x004ABEB0
	HRESULT CD3DFramework::Build(HWND hWnd, GUID* guid, DDAppDevice* device, DDAppDevice::DisplayMode* displayMode, uint8_t flags)
	{
		g_drawingDevice = new CD3DFramework();
		Nu3D::g_currentBmpDataNode = 0;
		return g_drawingDevice->InitalizeForWindow(hWnd, guid, device, displayMode, flags);
	}

	// FUNCTION: TOY2 0x004AD610
	void CD3DFramework::InitSurfaceDesc(LPDDSURFACEDESC2 ddSurfaceDesc, DWORD flags, DWORD caps)
	{
		memset(ddSurfaceDesc, 0, sizeof(DDSURFACEDESC2));

		ddSurfaceDesc->dwSize = sizeof(DDSURFACEDESC2);
		ddSurfaceDesc->dwFlags = flags;
		ddSurfaceDesc->ddsCaps.dwCaps = caps;
		ddSurfaceDesc->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	}

	// FUNCTION: TOY2 0x004AD640
	void CD3DFramework::BuildViewport(D3DVIEWPORT2* viewport, DWORD width, DWORD height)
	{
		memset(viewport, 0, sizeof(D3DVIEWPORT2));

		viewport->dwWidth = width;
		viewport->dwHeight = height;
		viewport->dwSize = sizeof(D3DVIEWPORT2);

		viewport->dvMaxZ = 1.0;
		viewport->dvClipX = -1.0;
		viewport->dvClipWidth = 2.0;
		viewport->dvClipY = 1.0;
		viewport->dvClipHeight = 2.0;
	}

	// FUNCTION: TOY2 0x004AF0D0
	HRESULT WINAPI CD3DFramework::EnumZBufferFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext)
	{
		LPDDPIXELFORMAT pixelFormat = reinterpret_cast<LPDDPIXELFORMAT>(lpContext);
		HRESULT result = 0;

		if (lpDDPixFmt)
		{
			if (pixelFormat)
			{
				if (lpDDPixFmt->dwFlags != pixelFormat->dwFlags)
					return 1;

				memcpy(pixelFormat, lpDDPixFmt, sizeof(DDPIXELFORMAT));

				if (lpDDPixFmt->dwRGBBitCount != 16)
					return 1;
			}
		}

		return result;
	}

	/* ------ DrawingDevice ------- */

	// FUNCTION: TOY2 0x004ABA40
	LPDIRECTDRAW4 GetDDraw4() { return g_drawingDevice->m_pDD; }

	// FUNCTION: TOY2 0x004ABA70
	LPDIRECT3D3 GetD3D() { return g_drawingDevice->m_pD3D; }

	// FUNCTION: TOY2 0x004ABA80
	LPDIRECT3DDEVICE3 GetD3DDevice() { return g_drawingDevice->m_pd3dDevice; }

	// FUNCTION: TOY2 0x004ABBC0
	int32_t GetWidth() { return g_drawingDevice->m_dwRenderWidth; }

	// FUNCTION: TOY2 0x004ABBD0
	int32_t GetHeight() { return g_drawingDevice->m_dwRenderHeight; }

	// FUNCTION: TOY2 0x004ABD90
	int32_t GetDestWidth() { return g_drawingDevice->m_rcViewportRect.right - g_drawingDevice->m_rcViewportRect.left; }

	// FUNCTION: TOY2 0x004ABDA0
	int32_t GetDestHeight() { return g_drawingDevice->m_rcViewportRect.bottom - g_drawingDevice->m_rcViewportRect.top; }

	// FUNCTION: TOY2 0x004ABBF0
	LPDIRECTDRAWSURFACE4 GetBackBuffer() { return g_drawingDevice->m_pddsBackBuffer; }

	// FUNCTION: TOY2 0x004ABB30
	int32_t SetViewport(LPD3DVIEWPORT2 viewport)
	{
		if (g_drawingDevice->m_pvViewport)
			g_drawingDevice->m_pvViewport->SetViewport2(viewport);

		return -1;
	}

	// FUNCTION: TOY2 0x004ABB80
	int32_t BuildFreshViewport(LPD3DVIEWPORT2 viewport)
	{
		D3DVIEWPORT2 freshViewport;

		CD3DFramework::BuildViewport(&freshViewport, g_drawingDevice->m_dwRenderWidth, g_drawingDevice->m_dwRenderHeight);
		memcpy(viewport, &freshViewport, sizeof(D3DVIEWPORT2));

		return 0;
	}

	// FUNCTION: TOY2 0x004AC100
	HRESULT CreateMaterial(LPDIRECT3DMATERIAL3* outMaterial) { return g_drawingDevice->m_pD3D->CreateMaterial(outMaterial, 0); }

	// FUNCTION: TOY2 0x004ABD80
	RECT* GetDestRect() { return &g_drawingDevice->m_rcViewportRect; }

	// FUNCTION: TOY2 0x004ABF00
	void Destroy()
	{
		if (g_drawingDevice)
		{
			g_drawingDevice->Release();
			delete g_drawingDevice;
		}

		g_drawingDevice = 0;
	}

	// FUNCTION: TOY2 0x00412B10
	void Quit()
	{
		if (g_validDrawDevice)
		{
			if (g_unusedD3D)
			{
				g_unusedD3D->Release();
				g_unusedD3D = 0;
			}

			Destroy();

			g_validDrawDevice = 0;
		}
	}

	// FUNCTION: TOY2 0x004ACFC0
	HRESULT GetChosenDevice(DDAppDevice::App** outApp, DDAppDevice** outDevice)
	{
		DDAppDevice::App* primaryApp = g_primaryDDApp;

		if (! g_primaryDDApp)
			return 0x81000001;

		if (outApp)
		{
			*outApp = g_primaryDDApp;
			primaryApp = g_primaryDDApp;
		}

		if (outDevice)
			*outDevice = primaryApp->primaryDevice;

		return 0;
	}

	// FUNCTION: TOY2 0x004AC420
	HRESULT GetChosenDevice_T(DDAppDevice::App** outApp, DDAppDevice** outDevice) { return GetChosenDevice(outApp, outDevice); }

	// FUNCTION: TOY2 0x004AD000
	DDAppDevice::App* GetListHead() { return g_ddAppListHead; }

	// FUNCTION: TOY2 0x004ABCA0
	LPD3DDEVICEDESC CopySurfaceDesc(LPD3DDEVICEDESC outSurfaceDesc)
	{
		D3DDEVICEDESC tempDesc;

		memcpy(&tempDesc, &g_drawingDevice->m_ddDeviceDesc, sizeof(D3DDEVICEDESC));
		memcpy(outSurfaceDesc, &tempDesc, sizeof(D3DDEVICEDESC));

		return outSurfaceDesc;
	}

	// FUNCTION: TOY2 0x004AC370
	HRESULT SetRenderState(D3DRENDERSTATETYPE renderStateType, DWORD value) { return g_drawingDevice->m_pd3dDevice->SetRenderState(renderStateType, value); }

	// FUNCTION: TOY2 0x004ABFA0
	HRESULT SetLightState(D3DLIGHTSTATETYPE lightState, DWORD value) { return g_drawingDevice->m_pd3dDevice->SetLightState(lightState, value); }

	// FUNCTION: TOY2 0x004ABAE0
	LPDIRECT3DVIEWPORT3 GetViewport() { return g_drawingDevice->m_pvViewport; }

	// FUNCTION: TOY2 0x004ABF30
	HRESULT CreateLight(LPDIRECT3DLIGHT* outLight) { return GetD3D()->CreateLight(outLight, 0); }

	// FUNCTION: TOY2 0x004ABF50
	HRESULT SetLight(LPDIRECT3DLIGHT light, LPD3DLIGHT d3dLight) { return light->SetLight(d3dLight); }

	// FUNCTION: TOY2 0x004ABF60
	HRESULT AddLight(LPDIRECT3DLIGHT light) { return GetViewport()->AddLight(light); }

	// FUNCTION: TOY2 0x004ABF80
	HRESULT DeleteLight(LPDIRECT3DLIGHT light) { return GetViewport()->DeleteLight(light); }

	// FUNCTION: TOY2 0x004ABFC0
	int32_t ReleaseLight(LPDIRECT3DLIGHT light) { return light->Release(); }

	// FUNCTION: TOY2 0x004ABFD0
	HRESULT SetWorldTransform(D3DMATRIX* matrix)
	{
		g_curWorldTransform = matrix;

		return g_drawingDevice->m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, matrix);
	}

	// FUNCTION: TOY2 0x004AC2A0
	HRESULT DrawPrimitive(D3DPRIMITIVETYPE primitiveType, DWORD vertexTypeDesc, void* vertices, DWORD vertexCount, DWORD flags)
	{
		return g_drawingDevice->m_pd3dDevice->DrawPrimitive(primitiveType, vertexTypeDesc, vertices, vertexCount, flags);
	}

	// FUNCTION: TOY2 0x004AC150
	HRESULT SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE state, DWORD value)
	{
		return g_drawingDevice->m_pd3dDevice->SetTextureStageState(stage, state, value);
	}

	// FUNCTION: TOY2 0x004ABAF0
	HRESULT ClearScreen(DWORD clearFlags, D3DCOLOR clearColor)
	{
		LPDIRECT3DVIEWPORT3 viewport = g_drawingDevice->m_pvViewport;

		if (viewport)
		{
			LPRECT destRect = GetDestRect();

			return viewport->Clear2(1, (LPD3DRECT)destRect, clearFlags, clearColor, 1.0, 0);
		}

		return -1;
	}

	// FUNCTION: TOY2 0x004ABA90
	HRESULT BeginScene()
	{
		LPDIRECT3DDEVICE3 device = GetD3DDevice();

		if (device)
			return device->BeginScene();
		else
			return -1;
	}

	// FUNCTION: TOY2 0x004ABD40
	HRESULT PresentFrame()
	{
		LPDIRECTDRAWSURFACE4 frontBuffer = g_drawingDevice->m_pddsFrontBuffer;

		if (! frontBuffer)
			return 0x8200000E;

		LPDIRECTDRAWSURFACE4 backBuffer = g_drawingDevice->m_pddsBackBuffer;

		if (! backBuffer)
			return frontBuffer->IsLost();

		if (g_drawingDevice->m_bIsFullscreen)
			return frontBuffer->Flip(0, 1);

		return frontBuffer->Blt(&g_drawingDevice->m_rcScreenRect, backBuffer, &g_drawingDevice->m_rcViewportRect, DDBLT_WAIT, 0);
	}

	// FUNCTION: TOY2 0x004ABAD0
	void EndScene()
	{
		LPDIRECT3DDEVICE3 device = GetD3DDevice();

		if (device)
			device->EndScene();
	}

	// FUNCTION: TOY2 0x004BB590
	HRESULT BindTexWithStage(int32_t textureIndex, int32_t stageIndex)
	{
		Nu3D::BmpDataNode* bmpDataNode;

		++g_setTexCalls;

		if (textureIndex && (bmpDataNode = NGNLoader::g_textureDataFreeList[textureIndex].bmpDataNode) != 0)
			return Nu3D::SetTexture(stageIndex, bmpDataNode);
		else
			return Nu3D::SetTexture(stageIndex, 0);
	}

	// FUNCTION: TOY2 0x004BB540
	HRESULT BindTexToStage0(int32_t textureIndex)
	{
		Nu3D::BmpDataNode* bmpDataNode;

		++g_setTexCalls;

		if (textureIndex && (bmpDataNode = NGNLoader::g_textureDataFreeList[textureIndex].bmpDataNode) != 0)
			return Nu3D::SetTexture(0, bmpDataNode);
		else
			return Nu3D::SetTexture(0, 0);
	}

	// FUNCTION: TOY2 0x004ABFF0
	HRESULT SetViewTransform(D3DMATRIX* matrix)
	{
		SoftwareRenderer::g_viewTransform = matrix;

		return g_drawingDevice->m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW, matrix);
	}

	// FUNCTION: TOY2 0x004AC010
	HRESULT SetProjectionTransform(D3DMATRIX* matrix)
	{
		SoftwareRenderer::g_projectionTransform = matrix;

		return g_drawingDevice->m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, matrix);
	}

	// FUNCTION: TOY2 0x004ABC40
	HRESULT LockPrimarySurface(DDSURFACEDESC2* surfaceDesc) { return g_drawingDevice->m_pddsFrontBuffer->Lock(0, surfaceDesc, 2081, 0); }

	// FUNCTION: TOY2 0x004ABC60
	HRESULT UnlockPrimarySurface() { return g_drawingDevice->m_pddsFrontBuffer->Unlock(0); }

	// FUNCTION: TOY2 0x004ABD50
	int32_t RestoreToGDISurface(int32_t refreshWindow) { return g_drawingDevice->RestoreToGDISurface(refreshWindow); }

	// FUNCTION: TOY2 0x004ABE30
	int32_t GetSlotSurfaceByIndex(int32_t index, LPDIRECTDRAWSURFACE4* surface) { return g_drawingDevice->GetSlotSurfaceByIndex(index, surface); }

	// FUNCTION: TOY2 0x004AC180
	HRESULT SetTexture(DWORD flags, LPDIRECT3DTEXTURE2 texture) { return g_drawingDevice->m_pd3dDevice->SetTexture(flags, texture); }

	// FUNCTION: TOY2 0x004ABE90
	HRESULT GetDC(LPDIRECTDRAWSURFACE4 drawSurface, HDC* deviceContext) { return drawSurface->GetDC(deviceContext); }

	// FUNCTION: TOY2 0x004ABEA0
	HRESULT ReleaseDC(LPDIRECTDRAWSURFACE4 drawSurface, HDC hdc) { return drawSurface->ReleaseDC(hdc); }
}

namespace HardwareDevice
{
	// FUNCTION: TOY2 0x004AC340
	HRESULT DrawIndexedPrimitiveVB(D3DPRIMITIVETYPE primitiveType, LPDIRECT3DVERTEXBUFFER vertexBuffer, WORD* indices, DWORD indexCount, DWORD flags)
	{
		return DrawingDevice::g_drawingDevice->m_pd3dDevice->DrawIndexedPrimitiveVB(primitiveType, vertexBuffer, indices, indexCount, flags);
	}

	// FUNCTION: TOY2 0x004AC300
	HRESULT DrawIndexedPrimitive(
		D3DPRIMITIVETYPE d3dptPrimitiveType,
		DWORD dwVertexTypeDesc,
		LPVOID lpvVertices,
		DWORD dwVertexCount,
		LPWORD lpwIndices,
		DWORD dwIndexCount,
		DWORD dwFlags)
	{
		return DrawingDevice::g_drawingDevice->m_pd3dDevice->DrawIndexedPrimitive(
			d3dptPrimitiveType, dwVertexTypeDesc, lpvVertices, dwVertexCount, lpwIndices, dwIndexCount, dwFlags);
	}

	// Vertex Methods

	// FUNCTION: TOY2 0x004AC070
	HRESULT ReleaseVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer) { return buffer->Release(); }

	// FUNCTION: TOY2 0x004AC080
	HRESULT CreateVertexBuffer(D3DVERTEXBUFFERDESC* desc, LPDIRECT3DVERTEXBUFFER* outBuffer, DWORD flags)
	{
		return DrawingDevice::g_drawingDevice->m_pD3D->CreateVertexBuffer(desc, outBuffer, flags, 0);
	}

	// FUNCTION: TOY2 0x004AC0A0
	HRESULT LockVertexBuffer(LPDIRECT3DVERTEXBUFFER vertexBuffer, DWORD dwFlags, LPVOID* lplpData, DWORD* lpStride)
	{
		return vertexBuffer->Lock(dwFlags, lplpData, lpStride);
	}

	// FUNCTION: TOY2 0x004AC0C0
	HRESULT UnlockVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer) { return buffer->Unlock(); }

	// FUNCTION: TOY2 0x004AC0D0
	HRESULT OptimizeVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer, LPDIRECT3DDEVICE3 device, DWORD flags) { return buffer->Optimize(device, flags); }

	// FUNCTION: TOY2 0x004AC030
	HRESULT ProcessVerticesOnBuffer(
		LPDIRECT3DVERTEXBUFFER destBuffer,
		DWORD dwVertexOp,
		DWORD dwDestIndex,
		DWORD dwCount,
		LPDIRECT3DVERTEXBUFFER srcBuffer,
		DWORD dwSrcIndex,
		DWORD dwFlags)
	{
		return destBuffer->ProcessVertices(dwVertexOp, dwDestIndex, dwCount, srcBuffer, dwSrcIndex, DrawingDevice::g_drawingDevice->m_pd3dDevice, dwFlags);
	}
}

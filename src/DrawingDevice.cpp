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

	// FUNCTION: TOY2 0x004AEDA0 [MATCHED]
	HRESULT CD3DFramework::InitalizeForWindow(HWND hWnd, GUID* ddAppGuid, DDAppDevice* device, DDAppDevice::DisplayMode* displayMode, DWORD flags)
	{
		HRESULT hr;

		if (! hWnd || (! displayMode && (flags & 1)))
			return E_INVALIDARG;

		m_hWnd = hWnd;
		m_bIsFullscreen = (flags & 1) ? TRUE : FALSE;

		if (FAILED(hr = InitalizeDeviceAndSurfaces(ddAppGuid, &device->guid, displayMode, flags)))
		{
			Cleanup();
			if (E_FAIL == hr)
				hr = 0x82000000;
		}
		return hr;
	}

	// FUNCTION: TOY2 0x004AEE10 [MATCHED]
	HRESULT CD3DFramework::InitalizeDeviceAndSurfaces(GUID* ddAppGuid, GUID* deviceGuid, DDAppDevice::DisplayMode* displayMode, DWORD flags)
	{
		HRESULT hr;

		if (FAILED(hr = CreateDirectDraw(ddAppGuid, flags)))
			return hr;

		if (deviceGuid)
			if (FAILED(hr = SelectD3DDeviceAndZFormat(deviceGuid, flags)))
				return hr;

		if (FAILED(hr = CreatePrimaryChainAndRects(displayMode, flags)))
			return hr;

		if (! deviceGuid)
			return S_OK;

		if (flags & 4)
			if (FAILED(hr = CreateZBuffer()))
				return hr;

		if (FAILED(hr = CreateD3DDevice(deviceGuid)))
			return hr;

		if (FAILED(hr = CreateAndSetViewport()))
			return hr;

		if (m_initialized)
			return 0x82000000;

		m_slots[0].valid = 1;
		m_slots[0].width = m_dwRenderWidth;
		m_slots[0].height = m_dwRenderHeight;
		m_slots[0].surface1 = m_pddsBackBuffer;
		m_slots[0].surface2 = m_pddsZBuffer;
		m_initialized = 1;

		return S_OK;
	}

	// FUNCTION: TOY2 0x004AEEE0 [MATCHED]
	HRESULT CD3DFramework::CreateDirectDraw(LPGUID lpGUID, DWORD flags)
	{
		LPDIRECTDRAW pDD;
		if (FAILED(DirectDrawCreate(lpGUID, &pDD, 0)))
			return 0x82000001;

		if (FAILED(pDD->QueryInterface(IID_IDirectDraw4, (VOID**)&m_pDD)))
		{
			pDD->Release();
			return 0x82000001;
		}
		pDD->Release();

		DWORD dwCoopFlags = DDSCL_NORMAL;
		if (m_bIsFullscreen)
			dwCoopFlags = DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN;

		if (0L == (flags & 16))
			dwCoopFlags |= DDSCL_FPUSETUP;

		if (FAILED(m_pDD->SetCooperativeLevel(m_hWnd, dwCoopFlags)))
			return 0x82000002;

		return S_OK;
	}

	// FUNCTION: TOY2 0x004AEF80 [MATCHED]
	HRESULT CD3DFramework::SelectD3DDeviceAndZFormat(GUID* deviceGuid, DWORD flags)
	{
		if (FAILED(m_pDD->QueryInterface(IID_IDirect3D3, (VOID**)&m_pD3D)))
			return 0x82000003;

		D3DFINDDEVICERESULT devResult;
		D3DFINDDEVICESEARCH devSearch;
		memset(&devResult, 0, sizeof(D3DFINDDEVICERESULT));
		memset(&devSearch, 0, sizeof(D3DFINDDEVICESEARCH));
		devResult.dwSize = sizeof(D3DFINDDEVICERESULT);
		devSearch.dwSize = sizeof(D3DFINDDEVICESEARCH);
		devSearch.dwFlags = D3DFDS_GUID;
		memcpy(&devSearch.guid, deviceGuid, sizeof(GUID));

		if (FAILED(m_pD3D->FindDevice(&devSearch, &devResult)))
			return 0x82000003;

		if (0L == devResult.ddHwDesc.dwFlags)
		{
			m_dwDeviceMemType = DDSCAPS_SYSTEMMEMORY;
			memcpy(&m_ddDeviceDesc, &devResult.ddSwDesc, sizeof(D3DDEVICEDESC));
		}
		else
		{
			m_dwDeviceMemType = DDSCAPS_VIDEOMEMORY;
			memcpy(&m_ddDeviceDesc, &devResult.ddHwDesc, sizeof(D3DDEVICEDESC));
		}

		memset(&m_ddpfZBuffer, 0, sizeof(DDPIXELFORMAT));

		if (flags & 8)
			m_ddpfZBuffer.dwFlags = DDPF_ZBUFFER | DDPF_STENCILBUFFER;
		else
			m_ddpfZBuffer.dwFlags = DDPF_ZBUFFER;

		m_pD3D->EnumZBufferFormats(*deviceGuid, EnumZBufferFormats, (VOID*)&m_ddpfZBuffer);

		if (sizeof(DDPIXELFORMAT) != m_ddpfZBuffer.dwSize)
			return 0x82000005;

		return S_OK;
	}

	// FUNCTION: TOY2 0x004AF110 [MATCHED]
	HRESULT CD3DFramework::CreatePrimaryChainAndRects(DDAppDevice::DisplayMode* displayMode, DWORD flags)
	{
		HRESULT hr;

		if (flags & 1)
		{
			SetRect(&m_rcViewportRect, 0, 0, displayMode->surfaceDesc.dwWidth, displayMode->surfaceDesc.dwHeight);
			memcpy(&m_rcScreenRect, &m_rcViewportRect, sizeof(RECT));
			m_dwRenderWidth = m_rcViewportRect.right;
			m_dwRenderHeight = m_rcViewportRect.bottom;

			DWORD dwModeFlags = 0;

			if ((320 == m_dwRenderWidth) && (200 == m_dwRenderHeight) && (8 == displayMode->surfaceDesc.ddpfPixelFormat.dwRGBBitCount))
				dwModeFlags |= DDSDM_STANDARDVGAMODE;

			if (FAILED(m_pDD->SetDisplayMode(
					m_dwRenderWidth,
					m_dwRenderHeight,
					displayMode->surfaceDesc.ddpfPixelFormat.dwRGBBitCount,
					displayMode->surfaceDesc.dwRefreshRate,
					dwModeFlags)))
				return 0x82000009;

			DDSURFACEDESC2 ddsd;
			InitSurfaceDesc(&ddsd, DDSD_CAPS, 0);
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

			if (flags & 2)
			{
				ddsd.dwFlags |= DDSD_BACKBUFFERCOUNT;
				ddsd.ddsCaps.dwCaps |= DDSCAPS_FLIP | DDSCAPS_COMPLEX;
				ddsd.dwBackBufferCount = 1;
			}

			if (FAILED(hr = m_pDD->CreateSurface(&ddsd, &m_pddsFrontBuffer, 0)))
			{
				if (hr != DDERR_OUTOFVIDEOMEMORY)
					return 0x82000007;
				return DDERR_OUTOFVIDEOMEMORY;
			}

			if (flags & 2)
			{
				DDSCAPS2 ddscaps;
				ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
				if (FAILED(hr = m_pddsFrontBuffer->GetAttachedSurface(&ddscaps, &m_pddsBackBuffer)))
				{
					if (hr != DDERR_OUTOFVIDEOMEMORY)
						return 0x8200000A;
					return DDERR_OUTOFVIDEOMEMORY;
				}
			}
		}
		else
		{
			GetClientRect(m_hWnd, &m_rcViewportRect);
			m_rcViewportRect.top += g_viewportTopOffset;
			m_rcViewportRect.bottom -= g_viewportBottomOffset;
			GetClientRect(m_hWnd, &m_rcScreenRect);
			m_rcScreenRect.bottom -= g_viewportTopOffset;
			m_rcScreenRect.bottom -= g_viewportBottomOffset;
			ClientToScreen(m_hWnd, (POINT*)&m_rcScreenRect.left);
			ClientToScreen(m_hWnd, (POINT*)&m_rcScreenRect.right);
			m_dwRenderWidth = m_rcViewportRect.right;
			m_dwRenderHeight = m_rcViewportRect.bottom;

			DDSURFACEDESC2 ddsd;
			InitSurfaceDesc(&ddsd, DDSD_CAPS, 0);
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			if (0L == (flags & 2))
				ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;

			if (FAILED(hr = m_pDD->CreateSurface(&ddsd, &m_pddsFrontBuffer, 0)))
			{
				if (hr != DDERR_OUTOFVIDEOMEMORY)
					return 0x82000007;
				return DDERR_OUTOFVIDEOMEMORY;
			}

			LPDIRECTDRAWCLIPPER pcClipper;
			if (FAILED(hr = m_pDD->CreateClipper(0, &pcClipper, 0)))
				return 0x82000008;

			pcClipper->SetHWnd(0, m_hWnd);
			m_pddsFrontBuffer->SetClipper(pcClipper);
			if (pcClipper)
			{
				pcClipper->Release();
				pcClipper = 0;
			}

			if (flags & 2)
			{
				ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
				ddsd.dwWidth = m_dwRenderWidth;
				ddsd.dwHeight = m_dwRenderHeight;
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

				if (FAILED(hr = m_pDD->CreateSurface(&ddsd, &m_pddsBackBuffer, 0)))
				{
					if (hr != DDERR_OUTOFVIDEOMEMORY)
						return 0x8200000A;
					return DDERR_OUTOFVIDEOMEMORY;
				}
			}
			else
			{
				ClientToScreen(m_hWnd, (POINT*)&m_rcViewportRect.left);
				ClientToScreen(m_hWnd, (POINT*)&m_rcViewportRect.right);
			}
		}

		if (flags & 2)
			m_pddsRenderTarget = m_pddsBackBuffer;
		else
			m_pddsRenderTarget = m_pddsFrontBuffer;
		m_pddsRenderTarget->AddRef();

		return S_OK;
	}

	// FUNCTION: TOY2 0x004AF420 [MATCHED]
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

		if (result < 0)
			return result != DDERR_OUTOFVIDEOMEMORY ? 0x82000005 : DDERR_OUTOFVIDEOMEMORY;

		return m_pddsRenderTarget->AddAttachedSurface(m_pddsZBuffer) < 0 ? 0x82000005 : 0;
	}

	// FUNCTION: TOY2 0x004AF4E0 [MATCHED]
	HRESULT CD3DFramework::CreateD3DDevice(const CLSID* guid)
	{
		DDSURFACEDESC2 surfaceDesc;
		surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);

		m_pDD->GetDisplayMode(&surfaceDesc);

		if (surfaceDesc.ddpfPixelFormat.dwRGBBitCount <= 8)
			return 0x8200000D;

		return m_pD3D->CreateDevice(*guid, m_pddsRenderTarget, &m_pd3dDevice, 0) < 0 ? 0x82000004 : 0;
	}

	// FUNCTION: TOY2 0x004AF550 [MATCHED]
	HRESULT CD3DFramework::CreateAndSetViewport()
	{
		D3DVIEWPORT2 viewport2;
		CD3DFramework::BuildViewport(&viewport2, m_dwRenderWidth, m_dwRenderHeight);

		if (m_pD3D->CreateViewport(&m_pvViewport, 0) < 0)
			return 0x82000006;

		if (m_pd3dDevice->AddViewport(m_pvViewport) < 0)
			return 0x82000006;

		if (m_pvViewport->SetViewport2(&viewport2) < 0)
			return 0x82000006;

		return m_pd3dDevice->SetCurrentViewport(m_pvViewport) < 0 ? 0x82000006 : 0;
	}

	// FUNCTION: TOY2 0x004AF5F0 [MATCHED]
	HRESULT CD3DFramework::PresentFrame()
	{
		LPDIRECTDRAWSURFACE4 frontBuffer = m_pddsFrontBuffer;

		if (! frontBuffer)
			return 0x8200000E;

		LPDIRECTDRAWSURFACE4 backBuffer = m_pddsBackBuffer;

		if (! backBuffer)
			return frontBuffer->IsLost();

		if (m_bIsFullscreen)
			return frontBuffer->Flip(0, 1);

		return frontBuffer->Blt(&m_rcScreenRect, backBuffer, &m_rcViewportRect, DDBLT_WAIT, 0);
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

	// FUNCTION: TOY2 0x004AF680 [MATCHED]
	HRESULT CD3DFramework::RestoreSurfaces()
	{
		if (m_pddsFrontBuffer && m_pddsFrontBuffer->IsLost())
			m_pddsFrontBuffer->Restore();

		if (m_pddsBackBuffer && m_pddsBackBuffer->IsLost())
			m_pddsBackBuffer->Restore();

		if (m_pddsZBuffer && m_pddsZBuffer->IsLost())
			m_pddsZBuffer->Restore();

		return 0;
	}

	// FUNCTION: TOY2 0x004AF6E0 [MATCHED]
	void CD3DFramework::Move(int32_t x, int32_t y)
	{
		if (! m_bIsFullscreen)
		{
			SetRect(&m_rcScreenRect, x, y, x + m_dwRenderWidth, y + m_dwRenderHeight);

			if (! m_pddsBackBuffer)
				memcpy(&m_rcViewportRect, &m_rcScreenRect, sizeof(RECT));
		}
	}

	// FUNCTION: TOY2 0x004AF730 [MATCHED]
	int32_t CD3DFramework::CreateSlotSurface(int32_t width, int32_t height)
	{
		DDSURFACEDESC2 ddsd;
		LPDIRECTDRAWSURFACE4 pddsSurface;
		LPDIRECTDRAWSURFACE4 pddsZBuffer;
		HRESULT hr;
		uint32_t slot;

		for (slot = 0; slot < 8; slot++)
			if (! m_slots[slot].valid)
				break;

		if (slot == 8)
			return -1;

		m_initialized++;

		InitSurfaceDesc(&ddsd, DDSD_CAPS, 0);
		m_pddsRenderTarget->GetSurfaceDesc(&ddsd);

		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE | DDSCAPS_TEXTURE;
		ddsd.dwWidth = width;
		ddsd.dwHeight = height;

		if (FAILED(hr = m_pDD->CreateSurface(&ddsd, &pddsSurface, 0)))
		{
			ddsd.ddsCaps.dwCaps = DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE;

			if (FAILED(hr = m_pDD->CreateSurface(&ddsd, &pddsSurface, 0)))
				return hr;
		}

		if (m_pddsZBuffer)
		{
			InitSurfaceDesc(&ddsd, DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT, 0);

			ddsd.ddsCaps.dwCaps = m_dwDeviceMemType | DDSCAPS_ZBUFFER;
			ddsd.dwWidth = width;
			ddsd.dwHeight = height;

			memcpy(&ddsd.ddpfPixelFormat, &m_ddpfZBuffer, sizeof(ddsd.ddpfPixelFormat));

			if (FAILED(hr = m_pDD->CreateSurface(&ddsd, &pddsZBuffer, 0)))
				return hr;

			if (FAILED(hr = pddsSurface->AddAttachedSurface(pddsZBuffer)))
				return hr;
		}

		m_slots[slot].valid = 1;
		m_slots[slot].unkInt2 = 0;
		m_slots[slot].width = width;
		m_slots[slot].height = height;
		m_slots[slot].surface1 = pddsSurface;
		// $BUG: pddsZBuffer is never assigned when the framework has no Z buffer
		m_slots[slot].surface2 = pddsZBuffer;

		return slot;
	}

	// FUNCTION: TOY2 0x004AF8D0 [MATCHED]
	int32_t CD3DFramework::ReleaseSlotSurface(uint32_t index)
	{
		if (index > 8)
			return 0x8200000F;

		if (index == 0)
			return 0x8200000F;

		if (! m_slots[index].valid)
			return 0x8200000F;

		if (! m_slots[index].unkInt2)
			return 0x82000010;

		if (m_slots[index].surface1)
		{
			m_slots[index].surface1->Release();
			m_slots[index].surface1 = 0;
		}

		if (m_slots[index].surface2)
		{
			m_slots[index].surface2->Release();
			m_slots[index].surface2 = 0;
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004AF960 [MATCHED]
	int32_t CD3DFramework::SetSlotAsRenderTarget(uint32_t index)
	{
		LPDIRECTDRAWSURFACE4 pddsTarget;
		LPDIRECTDRAWSURFACE4 pddsZBuffer;
		HRESULT hr;

		if (index > 8)
			return 0x8200000F;

		if (! m_slots[index].valid)
			return 0x8200000F;

		if (index == 0)
		{
			pddsZBuffer = m_slots[0].surface2;
			pddsTarget = m_pddsBackBuffer;
		}
		else
		{
			pddsZBuffer = m_slots[index].surface2;
			pddsTarget = m_slots[index].surface1;
		}

		if (FAILED(hr = m_pd3dDevice->SetRenderTarget(pddsTarget, 0)))
			return hr;

		m_dwRenderWidth = m_slots[index].width;
		m_dwRenderHeight = m_slots[index].height;
		m_pddsRenderTarget = pddsTarget;

		m_rcScreenRect.right = m_rcScreenRect.left + m_dwRenderWidth - 1;
		m_rcScreenRect.bottom = m_rcScreenRect.top + m_dwRenderHeight - 1;
		m_rcViewportRect.right = m_rcViewportRect.left + m_dwRenderWidth - 1;
		m_rcViewportRect.bottom = m_rcViewportRect.top + m_dwRenderHeight - 1;

		m_pddsZBuffer = pddsZBuffer;

		return 0;
	}

	// FUNCTION: TOY2 0x004AFA20 [MATCHED]
	int32_t CD3DFramework::GetSlotSurfaceByIndex(uint32_t index, LPDIRECTDRAWSURFACE4* surfaceOut)
	{
		if (index > 8)
			return 0x8200000F;

		if (! m_slots[index].valid)
			return 0x8200000F;

		*surfaceOut = m_slots[index].surface1;

		return 0;
	}

	// FUNCTION: TOY2 0x004AFA60 [MATCHED]
	int32_t CD3DFramework::GetSlotSurfaceCaps(uint32_t index, LPDDSCAPS2 caps)
	{
		if (index > 8)
			return 0x8200000F;

		if (! m_slots[index].valid)
			return 0x8200000F;

		m_slots[index].surface1->GetCaps(caps);

		return 0;
	}

	// FUNCTION: TOY2 0x004AFAA0 [MATCHED]
	int32_t CD3DFramework::GetSlotDimensions(uint32_t index, int32_t* width, int32_t* height)
	{
		if (index > 8)
			return 0x8200000F;

		if (! m_slots[index].valid)
			return 0x8200000F;

		*width = m_slots[index].width;
		*height = m_slots[index].height;

		return 0;
	}

	// FUNCTION: TOY2 0x004ABEB0
	HRESULT CD3DFramework::Build(HWND hWnd, GUID* guid, DDAppDevice* device, DDAppDevice::DisplayMode* displayMode, DWORD flags)
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

	// FUNCTION: TOY2 0x004AF0D0 [MATCHED]
	HRESULT WINAPI CD3DFramework::EnumZBufferFormats(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext)
	{
		LPDDPIXELFORMAT pixelFormat = (LPDDPIXELFORMAT)lpContext;

		if (lpDDPixFmt && pixelFormat)
		{
			if (lpDDPixFmt->dwFlags == pixelFormat->dwFlags)
			{
				memcpy(pixelFormat, lpDDPixFmt, sizeof(DDPIXELFORMAT));

				if (lpDDPixFmt->dwRGBBitCount == 16)
					return D3DENUMRET_CANCEL;
			}

			return D3DENUMRET_OK;
		}

		return D3DENUMRET_CANCEL;
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

	// FUNCTION: TOY2 0x004ABF00 [MATCHED]
	void Destroy()
	{
		CD3DFramework* drawingDevice = g_drawingDevice;

		if (g_drawingDevice)
		{
			g_drawingDevice->Release();
			delete drawingDevice;
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

	// FUNCTION: TOY2 0x004ACFC0 [MATCHED]
	HRESULT GetChosenDevice(DDAppDevice::App** outApp, DDAppDevice** outDevice)
	{
		if (! g_primaryDDApp)
			return 0x81000001;

		if (outApp)
			*outApp = g_primaryDDApp;

		if (outDevice)
			*outDevice = g_primaryDDApp->primaryDevice;

		return 0;
	}

	// FUNCTION: TOY2 0x004AC420
	HRESULT GetChosenDevice_T(DDAppDevice::App** outApp, DDAppDevice** outDevice) { return GetChosenDevice(outApp, outDevice); }

	// FUNCTION: TOY2 0x004AD000
	DDAppDevice::App* GetListHead() { return g_ddAppListHead; }

	// FUNCTION: TOY2 0x004AC440 [MATCHED]
	DDAppDevice::App* GetListHead_T() { return GetListHead(); }

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
	HRESULT SetLight(LPDIRECT3DLIGHT light, LPD3DLIGHT2 d3dLight) { return light->SetLight((LPD3DLIGHT)d3dLight); }

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

	// FUNCTION: TOY2 0x004ABD40 [MATCHED]
	HRESULT PresentFrame() { return g_drawingDevice->PresentFrame(); }

	// FUNCTION: TOY2 0x004ABD30 [MATCHED]
	HRESULT RestoreSurfaces() { return g_drawingDevice->RestoreSurfaces(); }

	// FUNCTION: TOY2 0x004ABAB0 [MATCHED]
	HRESULT PresentAndRestoreSurfaces()
	{
		HRESULT result = PresentFrame();

		if (result == DDERR_SURFACELOST)
			return RestoreSurfaces();

		return result;
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

		if (textureIndex && (bmpDataNode = NGNLoader::g_textureDataFreeList[textureIndex - 1].bmpDataNode) != 0)
			return Nu3D::SetTexture(stageIndex, bmpDataNode);
		else
			return Nu3D::SetTexture(stageIndex, 0);
	}

	// FUNCTION: TOY2 0x004BB540
	HRESULT BindTexToStage0(int32_t textureIndex)
	{
		Nu3D::BmpDataNode* bmpDataNode;

		++g_setTexCalls;

		if (textureIndex && (bmpDataNode = NGNLoader::g_textureDataFreeList[textureIndex - 1].bmpDataNode) != 0)
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

	// FUNCTION: TOY2 0x004ABDB0 [MATCHED]
	void Move(int32_t x, int32_t y) { g_drawingDevice->Move(x, y); }

	// FUNCTION: TOY2 0x004ABDD0 [MATCHED]
	int32_t CreateSlotSurface(int32_t width, int32_t height) { return g_drawingDevice->CreateSlotSurface(width, height); }

	// FUNCTION: TOY2 0x004ABDF0 [MATCHED]
	int32_t ReleaseSlotSurface(uint32_t index) { return g_drawingDevice->ReleaseSlotSurface(index); }

	// FUNCTION: TOY2 0x004ABE10 [MATCHED]
	int32_t SetSlotAsRenderTarget(uint32_t index) { return g_drawingDevice->SetSlotAsRenderTarget(index); }

	// FUNCTION: TOY2 0x004ABE30
	int32_t GetSlotSurfaceByIndex(uint32_t index, LPDIRECTDRAWSURFACE4* surface) { return g_drawingDevice->GetSlotSurfaceByIndex(index, surface); }

	// FUNCTION: TOY2 0x004ABE50 [MATCHED]
	int32_t GetSlotSurfaceCaps(uint32_t index, LPDDSCAPS2 caps) { return g_drawingDevice->GetSlotSurfaceCaps(index, caps); }

	// FUNCTION: TOY2 0x004ABE70 [MATCHED]
	int32_t GetSlotDimensions(uint32_t index, int32_t* width, int32_t* height) { return g_drawingDevice->GetSlotDimensions(index, width, height); }

	// FUNCTION: TOY2 0x004AC180
	HRESULT SetTexture(DWORD flags, LPDIRECT3DTEXTURE2 texture) { return g_drawingDevice->m_pd3dDevice->SetTexture(flags, texture); }

	// FUNCTION: TOY2 0x004ABE90
	HRESULT GetDC(LPDIRECTDRAWSURFACE4 drawSurface, HDC* deviceContext) { return drawSurface->GetDC(deviceContext); }

	// FUNCTION: TOY2 0x004ABEA0
	HRESULT ReleaseDC(LPDIRECTDRAWSURFACE4 drawSurface, HDC hdc) { return drawSurface->ReleaseDC(hdc); }

	// FUNCTION: TOY2 0x004AD5D0 [MATCHED]
	void D3DUtil_InitDeviceDesc(D3DDEVICEDESC& ddDevDesc)
	{
		memset(&ddDevDesc, 0, sizeof(D3DDEVICEDESC));
		ddDevDesc.dwSize = sizeof(D3DDEVICEDESC);
		ddDevDesc.dtcTransformCaps.dwSize = sizeof(D3DTRANSFORMCAPS);
		ddDevDesc.dlcLightingCaps.dwSize = sizeof(D3DLIGHTINGCAPS);
		ddDevDesc.dpcLineCaps.dwSize = sizeof(D3DPRIMCAPS);
		ddDevDesc.dpcTriCaps.dwSize = sizeof(D3DPRIMCAPS);
	}

	// FUNCTION: TOY2 0x004AD690 [MATCHED]
	void D3DUtil_InitMaterial(D3DMATERIAL& mtrl, FLOAT r, FLOAT g, FLOAT b)
	{
		memset(&mtrl, 0, sizeof(D3DMATERIAL));
		mtrl.dwSize = sizeof(D3DMATERIAL);
		mtrl.dcvDiffuse.r = mtrl.dcvAmbient.r = r;
		mtrl.dcvDiffuse.g = mtrl.dcvAmbient.g = g;
		mtrl.dcvDiffuse.b = mtrl.dcvAmbient.b = b;
		mtrl.dcvEmissive.r = r * 0.15f;
		mtrl.dcvEmissive.g = r * 0.15f;
		mtrl.dcvEmissive.b = r * 0.15f;
		mtrl.dwRampSize = 16L;
	}

	// FUNCTION: TOY2 0x004AD6F0 [MATCHED]
	void D3DUtil_InitLight(D3DLIGHT& light, D3DLIGHTTYPE ltType, FLOAT x, FLOAT y, FLOAT z)
	{
		memset(&light, 0, sizeof(D3DLIGHT));
		light.dwSize = sizeof(D3DLIGHT);
		light.dltType = ltType;
		light.dcvColor.r = 1.0f;
		light.dcvColor.g = 1.0f;
		light.dcvColor.b = 1.0f;
		light.dvPosition.x = light.dvDirection.x = x;
		light.dvPosition.y = light.dvDirection.y = y;
		light.dvPosition.z = light.dvDirection.z = z;
	}

	// FUNCTION: TOY2 0x004AD750 [MATCHED]
	void InitLight2(D3DLIGHT2& light, D3DLIGHTTYPE ltType, FLOAT x, FLOAT y, FLOAT z)
	{
		memset(&light, 0, sizeof(D3DLIGHT2));
		light.dwSize = sizeof(D3DLIGHT2);
		light.dltType = ltType;
		light.dcvColor.r = 1.0f;
		light.dcvColor.g = 1.0f;
		light.dcvColor.b = 1.0f;
		light.dvPosition.x = light.dvDirection.x = x;
		light.dvPosition.y = light.dvDirection.y = y;
		light.dvPosition.z = light.dvDirection.z = z;
		light.dwFlags = D3DLIGHT_ACTIVE;
	}

	// FUNCTION: TOY2 0x004AD7B0 [MATCHED]
	LPDIRECTDRAW4 D3DUtil_GetDirectDrawFromDevice(LPDIRECT3DDEVICE3 pd3dDevice)
	{
		LPDIRECTDRAW4 pDD = 0;
		LPDIRECTDRAWSURFACE4 pddsRender;

		if (pd3dDevice)
		{
			if (SUCCEEDED(pd3dDevice->GetRenderTarget(&pddsRender)))
			{
				pddsRender->GetDDInterface((VOID**)&pDD);
				pddsRender->Release();
			}
		}
		return pDD;
	}

	// FUNCTION: TOY2 0x004AD7F0 [MATCHED]
	DWORD D3DUtil_GetDeviceMemoryType(LPDIRECT3DDEVICE3 pd3dDevice)
	{
		D3DDEVICEDESC ddHwDesc, ddSwDesc;
		ddHwDesc.dwSize = sizeof(D3DDEVICEDESC);
		ddSwDesc.dwSize = sizeof(D3DDEVICEDESC);
		if (FAILED(pd3dDevice->GetCaps(&ddHwDesc, &ddSwDesc)))
			return 0L;

		if (ddHwDesc.dwFlags)
			return DDSCAPS_VIDEOMEMORY;

		return DDSCAPS_SYSTEMMEMORY;
	}
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
	HRESULT LockVertexBuffer(LPDIRECT3DVERTEXBUFFER vertexBuffer, DWORD dwFlags, LPVOID* lplpData, DWORD* sizeBytes)
	{
		return vertexBuffer->Lock(dwFlags, lplpData, sizeBytes);
	}

	// FUNCTION: TOY2 0x004AC0C0
	HRESULT UnlockVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer) { return buffer->Unlock(); }

	// FUNCTION: TOY2 0x004AC0D0
	HRESULT OptimizeVertexBuffer(LPDIRECT3DVERTEXBUFFER buffer, LPDIRECT3DDEVICE3 device, DWORD flags) { return buffer->Optimize(device, flags); }

	// FUNCTION: TOY2 0x004AC030 [MATCHED]
	HRESULT ProcessVerticesOnBuffer(
		LPDIRECT3DVERTEXBUFFER destBuffer,
		DWORD dwVertexOp,
		DWORD dwDestIndex,
		DWORD dwCount,
		LPDIRECT3DVERTEXBUFFER srcBuffer,
		DWORD dwSrcIndex,
		DWORD dwFlags)
	{
		LPDIRECT3DDEVICE3 device = DrawingDevice::g_drawingDevice->m_pd3dDevice;

		return destBuffer->ProcessVertices(dwVertexOp, dwDestIndex, dwCount, srcBuffer, dwSrcIndex, device, dwFlags);
	}
}

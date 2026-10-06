#include "Renderer/Glue.h"
#include "DrawingDevice.h"
#include "NGNLoader/NGNLoader.h"
#include "Logger.h"
#include "Renderer/Renderer.h"

namespace Renderer
{
	namespace Glue
	{
		// GLOBAL: TOY2 0x00B62414
		LPDIRECTDRAWSURFACE4 g_sysMemBackdrop;

		// GLOBAL: TOY2 0x00B6240C
		int32_t g_selectedTex;

		// FUNCTION: TOY2 0x004CE4D0
		int32_t BackdropBltFast()
		{
			if (! g_sysMemBackdrop)
				return 0;

			LPDIRECTDRAWSURFACE4 backBuffer = DrawingDevice::GetBackBuffer();

			DrawingDevice::GetWidth();
			DrawingDevice::GetHeight();

			backBuffer->BltFast(0, 0, g_sysMemBackdrop, 0, 16);

			return 1;
		}

		// FUNCTION: TOY2 0x004CE340
		void ReleaseBackdrop()
		{
			if (g_sysMemBackdrop)
			{
				g_sysMemBackdrop->Release();
				g_sysMemBackdrop = 0;

				Logger::DebugLog("glueReleaseBackdrop()\r\n");
			}

			g_selectedTex = -1;
		}

		// FUNCTION: TOY2 0x004CE380
		HBITMAP SetBackdrop(int32_t textureIndex)
		{
			int32_t texIndex = NGNLoader::GetTextureDataIndex(textureIndex);

			if (g_sysMemBackdrop && texIndex == g_selectedTex)
				return (HBITMAP)1;

			ReleaseBackdrop();
			g_selectedTex = texIndex;

			if (Renderer::GetIsSoftwareRendering())
				return 0;

			HBITMAP bmpHandle = NGNLoader::GetBmpHandle(texIndex);

			if (bmpHandle)
			{
				LPDIRECTDRAWSURFACE4 backBuffer = DrawingDevice::GetBackBuffer();

				DDSURFACEDESC2 surfaceDesc;
				surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);

				backBuffer->GetSurfaceDesc(&surfaceDesc);

				surfaceDesc.dwFlags = 4103;
				surfaceDesc.ddsCaps.dwCaps = 16448;

				memset(&surfaceDesc.ddsCaps.dwCaps2, 0, 12);

				Logger::DebugLog("glueSetBackdrop()\r\n");

				LPDIRECTDRAW4 ddraw4 = DrawingDevice::GetDDraw4();

				if (ddraw4->CreateSurface(&surfaceDesc, &g_sysMemBackdrop, 0))
				{
					Logger::DebugLog("Failed to create video memory backdrop\r\n");

					surfaceDesc.ddsCaps.dwCaps = 2112;

					if (ddraw4->CreateSurface(&surfaceDesc, &g_sysMemBackdrop, 0))
					{
						Logger::DebugLog("Failed to create system memory backdrop\r\n");
						return 0;
					}
				}

				if (! NGNLoader::CopyToDDSurfaceByIndex(texIndex, g_sysMemBackdrop))
				{
					g_sysMemBackdrop->Release();
					g_sysMemBackdrop = 0;
					return 0;
				}

				return (HBITMAP)1;
			}

			return bmpHandle;
		}
	}
}
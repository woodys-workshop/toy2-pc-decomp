#include "Renderer/ApplyFixes.h"

#ifdef APPLY_FIXES

	#include "Renderer/Renderer.h"
	#include "Toy2/Toy2.h"
	#include "DrawingDevice.h"
	#include "Version.h"
	#include <windows.h>
	#include <stdio.h>
	#include <string.h>

namespace Renderer
{
	namespace ApplyFixes
	{
		// Replaces the body of DoFrameDelay
		void LockFrameRate()
		{
			// The original derives the frame time from an rdtsc calibrated clock and a Sleep(1) loop, which is
			// unreliable on modern CPUs and timer resolutions and rounds up to whole 1/60s steps (so 17ms = 30fps).
			// This version locks to a steady 60fps (30fps in demo mode, like the original) using the performance counter.
			static LARGE_INTEGER frequency;
			static LARGE_INTEGER nextFrameTime;
			static int32_t timingInitialized = 0;

			if (! timingInitialized)
			{
				timingInitialized = 1;
				timeBeginPeriod(1);
				QueryPerformanceFrequency(&frequency);
				QueryPerformanceCounter(&nextFrameTime);
			}

			// Gameplay speed is expressed in 1/60s steps, 2 in demo mode
			int32_t frameSteps = Toy2::g_demoMode ? 2 : 1;
			g_frameDelta = frameSteps;

			int64_t frameTicks = frequency.QuadPart * frameSteps / 60;
			nextFrameTime.QuadPart += frameTicks;

			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);

			if (now.QuadPart > nextFrameTime.QuadPart + frameTicks * 4)
			{
				// Fell too far behind (loading, window drag, breakpoint), resync instead of fast forwarding
				nextFrameTime.QuadPart = now.QuadPart;
			}
			else
			{
				// Sleep away the bulk of the wait, spin for the last stretch so we land on the deadline
				while (true)
				{
					QueryPerformanceCounter(&now);
					int64_t remainingTicks = nextFrameTime.QuadPart - now.QuadPart;

					if (remainingTicks <= 0)
						break;

					if (remainingTicks * 1000 / frequency.QuadPart >= 2)
						Sleep(1);
				}
			}
		}

		// Called from EndScene, right before the frame is presented
		void DrawVersionStamp()
		{
			// Stamps the decomp version and git hash in the top left of the back buffer, so it shows on every screen
			if (DrawingDevice::g_drawingDevice->m_pddsBackBuffer)
			{
				LPDIRECTDRAWSURFACE4 versionSurface = DrawingDevice::g_drawingDevice->m_pddsBackBuffer;

				DDSURFACEDESC2 versionDesc;
				memset(&versionDesc, 0, sizeof(versionDesc));
				versionDesc.dwSize = sizeof(versionDesc);

				HDC versionDC;

				if (versionSurface->GetSurfaceDesc(&versionDesc) == DD_OK && versionSurface->GetDC(&versionDC) == DD_OK)
				{
					char versionText[64];
					sprintf(versionText, "toy2-decomp-0.1 (%s)", TOY2_GIT_HASH);

					int32_t fontHeight = versionDesc.dwHeight / 34;

					if (fontHeight < 12)
						fontHeight = 12;

					HFONT versionFont = CreateFontA(
						-fontHeight,
						0,
						0,
						0,
						FW_NORMAL,
						0,
						0,
						0,
						ANSI_CHARSET,
						OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						ANTIALIASED_QUALITY,
						DEFAULT_PITCH | FF_SWISS,
						"Arial");
					HGDIOBJ oldFont = SelectObject(versionDC, versionFont);

					SetBkMode(versionDC, TRANSPARENT);

					SetTextColor(versionDC, RGB(0, 0, 0));
					TextOutA(versionDC, 9, 7, versionText, strlen(versionText));

					SetTextColor(versionDC, RGB(255, 255, 255));
					TextOutA(versionDC, 8, 6, versionText, strlen(versionText));

					SelectObject(versionDC, oldFont);
					DeleteObject(versionFont);

					versionSurface->ReleaseDC(versionDC);
				}
			}
		}
	}
}

#endif

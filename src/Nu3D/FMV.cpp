#include "Nu3D/FMV.h"
#include "DrawingDevice.h"
#include "InputManager.h"
#include "InputTypes.h"
#include "Logger.h"
#include "Renderer/Renderer.h"
#include "Toy2/Toy2.h"

#include <directx6/ddraw.h>

#include <AMSTREAM.H>
#include <DDSTREAM.H>
#include <MMSTREAM.H>
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>

namespace Nu3D
{
	namespace FMV
	{
		struct RECTF
		{
			float left;
			float top;
			float right;
			float bottom;
		};

		struct Nu3DFMV
		{
			IAMMultiMediaStream* m_multiStream;
			IMediaStream* m_primaryVideo;
			IDirectDrawMediaStream* m_ddMediaStream;
			IDirectDrawSurface* m_ddSurface;
			IDirectDrawStreamSample* m_ddMediaSample;
			LPDIRECTDRAWSURFACE4 m_ddSurface4;
			int32_t m_flags;
			Nu3D::FMV::RECTF m_dimensions;
		};

		STATIC_ASSERT(sizeof(RECTF) == 0x10);
		STATIC_ASSERT(sizeof(Nu3DFMV) == 0x2C);

		// GLOBAL: TOY2 0x00B62668
		int32_t g_nu3dCoInitialized;

		// FUNCTION: TOY2 0x004DB8C0
		void Destroy(Nu3D::FMV::Nu3DFMV* fmv)
		{
			if (fmv->m_ddMediaSample)
				fmv->m_ddMediaSample->Release();

			if (fmv->m_ddMediaStream)
				fmv->m_ddMediaStream->Release();

			if (fmv->m_primaryVideo)
				fmv->m_primaryVideo->Release();

			if (fmv->m_multiStream)
				fmv->m_multiStream->Release();

			free(fmv);
		}

		// FUNCTION: TOY2 0x004DB910
		void SetDimensions(Nu3D::FMV::Nu3DFMV* fmv, int32_t left, int32_t top, int32_t right, int32_t bottom)
		{
			fmv->m_dimensions.left = (float)left;
			fmv->m_dimensions.top = (float)top;
			fmv->m_dimensions.right = (float)right;
			fmv->m_dimensions.bottom = (float)bottom;
		}

		// FUNCTION: TOY2 0x004DB940
		uint32_t IsPlaying(Nu3D::FMV::Nu3DFMV* fmv) { return (~fmv->m_flags >> 2) & 1; }

		// FUNCTION: TOY2 0x004DBB00
		void SetPaused(Nu3D::FMV::Nu3DFMV* fmv, int32_t paused)
		{
			HRESULT setStateResult;

			if (paused)
			{
				fmv->m_flags |= 4;

				setStateResult = fmv->m_multiStream->SetState(STREAMSTATE_STOP);
				if (setStateResult < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 344)("Failed with HRESULT(0x%8.8X)\n", setStateResult);
			}
			else
			{
				fmv->m_flags &= ~4;

				setStateResult = fmv->m_multiStream->SetState(STREAMSTATE_RUN);
				if (setStateResult < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 350)("Failed with HRESULT(0x%8.8X)\n", setStateResult);
			}
		}

		// FUNCTION: TOY2 0x004DBB80
		HRESULT Seek(Nu3D::FMV::Nu3DFMV* fmv, int32_t streamTime) { return fmv->m_multiStream->Seek((STREAM_TIME)10000000 * streamTime); }

		// FUNCTION: TOY2 0x004DB950
		void UpdateAndRenderFMV(Nu3D::FMV::Nu3DFMV* fmv)
		{
			HRESULT updateResult = fmv->m_ddMediaSample->Update(2, 0, 0, 0);
			RECT rect1;
			RECT rect2;
			DDSURFACEDESC2 surfaceDesc2;

			if (updateResult <= E_INVALIDARG)
			{
				switch (updateResult)
				{
					case E_INVALIDARG:
						Logger::DebugLog("FMV - Update INVALIDARG\r\n");
						goto LABEL_20;
					case E_POINTER:
						Logger::DebugLog("FMV - Update POINTER\r\n");
						goto LABEL_20;
					case E_ABORT:
						Logger::DebugLog("FMV - Update ABORT\r\n");
						goto LABEL_20;
					case MS_E_BUSY:
						Logger::DebugLog("FMV - Update BUSY\r\n");
						goto LABEL_20;
				}

				goto LABEL_13;
			}

			if (! updateResult)
				goto LABEL_14;

			if (updateResult == MS_S_PENDING)
			{
				Logger::DebugLog("FMV - Update PENDING\r\n");
				goto LABEL_20;
			}

			if (updateResult - MS_S_PENDING != 2)
			{
			LABEL_13:
				Logger::DebugLog("FMV - Update UNKNOWN\r\n");
			LABEL_14:

				if (updateResult != MS_S_ENDOFSTREAM)
					goto LABEL_20;

				goto LABEL_15;
			}

			Logger::DebugLog("FMV - Update ENDOFSTREAM\r\n");
		LABEL_15:

			if ((fmv->m_flags & 2) != 0)
			{
				fmv->m_multiStream->Seek(0);
			}
			else
			{
				fmv->m_multiStream->SetState(STREAMSTATE_STOP);

				fmv->m_multiStream->Seek(0);

				fmv->m_flags |= 4;
			}
		LABEL_20:

			rect1.left = (int32_t)fmv->m_dimensions.left;
			rect1.right = (int32_t)(fmv->m_dimensions.right + fmv->m_dimensions.left) - 1;
			rect1.top = (int32_t)fmv->m_dimensions.top;
			rect1.bottom = (int32_t)(fmv->m_dimensions.bottom + fmv->m_dimensions.top) - 1;

			memset(&surfaceDesc2, 0, sizeof(surfaceDesc2));

			surfaceDesc2.dwSize = 124;
			fmv->m_ddSurface4->GetSurfaceDesc(&surfaceDesc2);

			rect2.left = 0;
			rect2.right = surfaceDesc2.dwWidth - 1;
			rect2.top = 0;
			rect2.bottom = surfaceDesc2.dwHeight - 1;

			DrawingDevice::GetBackBuffer()->Blt(&rect1, fmv->m_ddSurface4, &rect2, 0x1000000, 0);
		}

		// FUNCTION: TOY2 0x004DB600
		Nu3D::FMV::Nu3DFMV* CreateFMVInstance(LPCCH filePath)
		{
			Nu3D::FMV::Nu3DFMV* alloc = (Nu3D::FMV::Nu3DFMV*)malloc(sizeof(Nu3D::FMV::Nu3DFMV));

			if (alloc)
			{
				IAMMultiMediaStream* multiStreamMedia;
				WCHAR wideCharPath[260];
				RECT surfaceRect;
				DWORD information;
				HRESULT result;

				memset(alloc, 0, sizeof(Nu3D::FMV::Nu3DFMV));

				if (! g_nu3dCoInitialized)
				{
					CoInitialize(0);
					g_nu3dCoInitialized = 1;
				}

				CoCreateInstance(CLSID_AMMultiMediaStream, 0, CLSCTX_INPROC_SERVER, IID_IAMMultiMediaStream, (void**)&multiStreamMedia);

				result = multiStreamMedia->Initialize(STREAMTYPE_READ, 0, 0);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 130)("Failed ot initialize HRESULT(0x%8.8X)\n", result);

				result = multiStreamMedia->AddMediaStream(DrawingDevice::GetDDraw4(), &MSPID_PrimaryVideo, 0, 0);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 135)("Failed to add video media stream HRESULT(0x%8.8X)\n", result);

				result = multiStreamMedia->AddMediaStream(0, &MSPID_PrimaryAudio, 1, 0);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 141)("Failed to add audio media stream HRESULT(0x%8.8X)\n", result);

				MultiByteToWideChar(0, 0, filePath, -1, wideCharPath, 260);

				if (multiStreamMedia->OpenFile(wideCharPath, 0) < 0)
				{
					multiStreamMedia->Release();
					free(alloc);
					return 0;
				}

				alloc->m_multiStream = multiStreamMedia;
				multiStreamMedia->AddRef();

				IAMMultiMediaStream* multiStreamMediaCpy = multiStreamMedia;

				if (! multiStreamMedia)
				{
					printf("Could not create a CLSID_MultiMediaStream object\nCheck you have run regsvr32 amstream.dll\n");
					multiStreamMediaCpy = multiStreamMedia;
				}

				multiStreamMediaCpy->Release();

				result = alloc->m_multiStream->GetMediaStream(MSPID_PrimaryVideo, &alloc->m_primaryVideo);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 167)("Failed to get media stream HRESULT(0x%8.8X)\n", result);

				result = alloc->m_primaryVideo->QueryInterface(IID_IDirectDrawMediaStream, (void**)&alloc->m_ddMediaStream);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 173)("Failed to query DirectDrawMediaStream interface HRESULT(0x%8.8X)\n", result);

				result = alloc->m_ddMediaStream->CreateSample(0, 0, 0, &alloc->m_ddMediaSample);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 179)("Failed to create sample HRESULT(0x%8.8X)\n", result);

				result = alloc->m_ddMediaSample->GetSurface(&alloc->m_ddSurface, &surfaceRect);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 185)("Failed get surface HRESULT(0x%8.8X)\n", result);

				result = alloc->m_ddSurface->QueryInterface(IID_IDirectDrawSurface4, (void**)&alloc->m_ddSurface4);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 191)("Failed to query DirectDrawSurface4 interface HRESULT(0x%8.8X)\n", result);

				information = 2;

				if (alloc->m_multiStream->GetInformation(&information, 0) == 0)
				{
					alloc->m_flags |= 1;
					return alloc;
				}

				alloc->m_flags &= ~1;
			}

			return alloc;
		}

		// FUNCTION: TOY2 0x004CE5F0
		int32_t PlayMovie(LPCCH filePath)
		{
			int32_t exited = 0;

			Renderer::ShowBlackFrames();

			Nu3D::FMV::Nu3DFMV* instance = CreateFMVInstance(filePath);

			if (! instance)
				return 0;

			int32_t height = DrawingDevice::GetDestHeight();
			int32_t width = DrawingDevice::GetDestWidth();

			SetDimensions(instance, 0, 0, width, height);
			Seek(instance, 0);
			SetPaused(instance, 0);

			do
			{
				Toy2::ProcessMiscEvents();
				UpdateAndRenderFMV(instance);

				if (Renderer::BeginScene())
				{
					int32_t isSoftwareRendering = Renderer::g_isSoftwareRendering;
					Renderer::g_isSoftwareRendering = 0;

					Renderer::EndScene(1);

					Renderer::g_isSoftwareRendering = isSoftwareRendering;
				}

				if (InputManager::FindKeyReleased() || (InputManager::GetCurButtonsPressed() & (INPUT_CANCEL | INPUT_SPIN | INPUT_JUMP | INPUT_FIRE)) != 0)
					exited = 1;

				Sleep(1);
			} while (IsPlaying(instance) && ! exited);

			Destroy(instance);

			return exited;
		}
	}
}

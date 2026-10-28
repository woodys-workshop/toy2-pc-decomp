#include "Nu3D/FMV.h"
#include "DrawingDevice.h"
#include "InputManager.h"
#include "InputTypes.h"
#include "Logger.h"
#include "Numerics.h"
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
		enum FMVFlags
		{
			FMV_FLAG_INFO_OK = 1,
			FMV_FLAG_LOOP = 2,
			FMV_FLAG_STOPPED = 4,
		};

		struct FMVInstance
		{
			IAMMultiMediaStream* multiStream;
			IMediaStream* primaryVideo;
			IDirectDrawMediaStream* ddMediaStream;
			IDirectDrawSurface* ddSurface;
			IDirectDrawStreamSample* ddMediaSample;
			LPDIRECTDRAWSURFACE4 ddSurface4;
			uint32_t flags;
			RectF dimensions;
		};

		STATIC_ASSERT(sizeof(FMVInstance) == 0x2C);

		// GLOBAL: TOY2 0x00B62668
		int32_t g_nu3dCoInitialized;

		// FUNCTION: TOY2 0x004DB8C0 [MATCHED]
		void Destroy(FMVInstance* fmv)
		{
			if (fmv->ddMediaSample)
				fmv->ddMediaSample->Release();

			if (fmv->ddMediaStream)
				fmv->ddMediaStream->Release();

			if (fmv->primaryVideo)
				fmv->primaryVideo->Release();

			if (fmv->multiStream)
				fmv->multiStream->Release();

			free(fmv);
		}

		// FUNCTION: TOY2 0x004DB910 [MATCHED]
		void SetDimensions(FMVInstance* fmv, int32_t left, int32_t top, int32_t right, int32_t bottom)
		{
			fmv->dimensions.left = (float)left;
			fmv->dimensions.top = (float)top;
			fmv->dimensions.right = (float)right;
			fmv->dimensions.bottom = (float)bottom;
		}

		// FUNCTION: TOY2 0x004DB940 [MATCHED]
		uint32_t IsPlaying(FMVInstance* fmv) { return ! (fmv->flags & FMV_FLAG_STOPPED); }

		// FUNCTION: TOY2 0x004DBB00 [MATCHED]
		void SetPaused(FMVInstance* fmv, int32_t paused)
		{
			HRESULT setStateResult;

			if (paused)
			{
				fmv->flags |= FMV_FLAG_STOPPED;

				setStateResult = fmv->multiStream->SetState(STREAMSTATE_STOP);
				if (setStateResult < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 344)("Failed with HRESULT(0x%8.8X)\n", setStateResult);
			}
			else
			{
				fmv->flags &= ~FMV_FLAG_STOPPED;

				setStateResult = fmv->multiStream->SetState(STREAMSTATE_RUN);
				if (setStateResult < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 350)("Failed with HRESULT(0x%8.8X)\n", setStateResult);
			}
		}

		// FUNCTION: TOY2 0x004DBB80 [MATCHED]
		HRESULT Seek(FMVInstance* fmv, int32_t streamTime) { return fmv->multiStream->Seek((STREAM_TIME)10000000 * streamTime); }

		// FUNCTION: TOY2 0x004DB950 [MATCHED]
		void UpdateAndRenderFMV(FMVInstance* fmv)
		{
			HRESULT updateResult = fmv->ddMediaSample->Update(2, 0, 0, 0);

			switch (updateResult)
			{
				case MS_E_BUSY:
					Logger::DebugLog("FMV - Update BUSY\r\n");
					break;
				case E_ABORT:
					Logger::DebugLog("FMV - Update ABORT\r\n");
					break;
				case E_POINTER:
					Logger::DebugLog("FMV - Update POINTER\r\n");
					break;
				case E_INVALIDARG:
					Logger::DebugLog("FMV - Update INVALIDARG\r\n");
					break;
				case 0:
					break;
				case MS_S_ENDOFSTREAM:
					Logger::DebugLog("FMV - Update ENDOFSTREAM\r\n");
					break;
				case MS_S_PENDING:
					Logger::DebugLog("FMV - Update PENDING\r\n");
					break;
				default:
					Logger::DebugLog("FMV - Update UNKNOWN\r\n");
					break;
			}

			if (updateResult == MS_S_ENDOFSTREAM)
			{
				if ((fmv->flags & FMV_FLAG_LOOP) != 0)
				{
					fmv->multiStream->Seek(0);
				}
				else
				{
					fmv->multiStream->SetState(STREAMSTATE_STOP);
					fmv->multiStream->Seek(0);
					fmv->flags |= FMV_FLAG_STOPPED;
				}
			}

			RECT destRect;
			destRect.left = (int32_t)fmv->dimensions.left;
			destRect.right = (int32_t)(fmv->dimensions.right + fmv->dimensions.left) - 1;
			destRect.top = (int32_t)fmv->dimensions.top;
			destRect.bottom = (int32_t)(fmv->dimensions.bottom + fmv->dimensions.top) - 1;

			DDSURFACEDESC2 surfaceDesc;
			memset(&surfaceDesc, 0, sizeof(surfaceDesc));

			surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
			fmv->ddSurface4->GetSurfaceDesc(&surfaceDesc);

			RECT srcRect;
			srcRect.left = 0;
			srcRect.right = surfaceDesc.dwWidth - 1;
			srcRect.top = 0;
			srcRect.bottom = surfaceDesc.dwHeight - 1;

			DrawingDevice::GetBackBuffer()->Blt(&destRect, fmv->ddSurface4, &srcRect, DDBLT_WAIT, NULL);
		}

		// FUNCTION: TOY2 0x004DB600
		FMVInstance* CreateFMVInstance(LPCCH filePath)
		{
			FMVInstance* alloc = (FMVInstance*)malloc(sizeof(FMVInstance));

			if (alloc)
			{
				memset(alloc, 0, sizeof(FMVInstance));

				if (! g_nu3dCoInitialized)
				{
					CoInitialize(0);
					g_nu3dCoInitialized = 1;
				}

				IAMMultiMediaStream* multiStreamMedia;
				CoCreateInstance(CLSID_AMMultiMediaStream, 0, CLSCTX_INPROC_SERVER, IID_IAMMultiMediaStream, (void**)&multiStreamMedia);

				HRESULT result = multiStreamMedia->Initialize(STREAMTYPE_READ, 0, 0);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 130)("Failed ot initialize HRESULT(0x%8.8X)\n", result);

				result = multiStreamMedia->AddMediaStream(DrawingDevice::GetDDraw4(), &MSPID_PrimaryVideo, 0, 0);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 135)("Failed to add video media stream HRESULT(0x%8.8X)\n", result);

				result = multiStreamMedia->AddMediaStream(0, &MSPID_PrimaryAudio, 1, 0);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 141)("Failed to add audio media stream HRESULT(0x%8.8X)\n", result);

				WCHAR wideCharPath[260];
				MultiByteToWideChar(0, 0, filePath, -1, wideCharPath, 260);

				if (multiStreamMedia->OpenFile(wideCharPath, 0) < 0)
				{
					multiStreamMedia->Release();
					free(alloc);
					return 0;
				}

				alloc->multiStream = multiStreamMedia;
				multiStreamMedia->AddRef();

				if (! multiStreamMedia)
					printf("Could not create a CLSID_MultiMediaStream object\nCheck you have run regsvr32 amstream.dll\n");

				multiStreamMedia->Release();

				result = alloc->multiStream->GetMediaStream(MSPID_PrimaryVideo, &alloc->primaryVideo);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 167)("Failed to get media stream HRESULT(0x%8.8X)\n", result);

				result = alloc->primaryVideo->QueryInterface(IID_IDirectDrawMediaStream, (void**)&alloc->ddMediaStream);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 173)("Failed to query DirectDrawMediaStream interface HRESULT(0x%8.8X)\n", result);

				result = alloc->ddMediaStream->CreateSample(0, 0, 0, &alloc->ddMediaSample);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 179)("Failed to create sample HRESULT(0x%8.8X)\n", result);

				RECT surfaceRect;
				result = alloc->ddMediaSample->GetSurface(&alloc->ddSurface, &surfaceRect);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 185)("Failed get surface HRESULT(0x%8.8X)\n", result);

				result = alloc->ddSurface->QueryInterface(IID_IDirectDrawSurface4, (void**)&alloc->ddSurface4);
				if (result < 0)
					Logger::GetErrorHandler("C:\\projects\\nu3d\\fmv.c", 191)("Failed to query DirectDrawSurface4 interface HRESULT(0x%8.8X)\n", result);

				DWORD information = 2;
				if (alloc->multiStream->GetInformation(&information, 0) == 0)
				{
					alloc->flags |= FMV_FLAG_INFO_OK;
					return alloc;
				}

				alloc->flags &= ~FMV_FLAG_INFO_OK;
			}

			return alloc;
		}

		// FUNCTION: TOY2 0x004CE5F0 [MATCHED]
		int32_t PlayMovie(LPCCH filePath)
		{
			int32_t exited = 0;

			Renderer::ShowBlackFrames();

			FMVInstance* instance = CreateFMVInstance(filePath);

			if (instance)
			{
				SetDimensions(instance, 0, 0, DrawingDevice::GetDestWidth(), DrawingDevice::GetDestHeight());
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
			}

			return exited;
		}
	}
}

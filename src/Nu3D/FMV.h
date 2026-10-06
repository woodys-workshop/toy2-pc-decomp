#pragma once

#include "Common.h"
#include <windows.h>
#include <directx6/ddraw.h>

struct IAMMultiMediaStream;
struct IMediaStream;
struct IDirectDrawMediaStream;
struct IDirectDrawStreamSample;

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

		STATIC_ASSERT(sizeof(RECTF) == 0x10);

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

		STATIC_ASSERT(sizeof(Nu3DFMV) == 0x2C);

		int32_t PlayMovie(LPCCH filePath);
		Nu3D::FMV::Nu3DFMV* CreateFMVInstance(LPCCH filePath);
		void Destroy(Nu3D::FMV::Nu3DFMV* fmv);
		void SetDimensions(Nu3D::FMV::Nu3DFMV* fmv, int32_t left, int32_t top, int32_t right, int32_t bottom);
		uint32_t IsPlaying(Nu3D::FMV::Nu3DFMV* fmv);
		void UpdateAndRenderFMV(Nu3D::FMV::Nu3DFMV* fmv);
		void SetPaused(Nu3D::FMV::Nu3DFMV* fmv, int32_t paused);
		HRESULT Seek(Nu3D::FMV::Nu3DFMV* fmv, int32_t streamTime);
	}
}

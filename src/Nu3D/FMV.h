#pragma once

#include "Common.h"
#include <windows.h>
#include <directx6/ddraw.h>

struct IAMMultiMediaStream;
struct IDirectDrawMediaStream;
struct IDirectDrawStreamSample;
struct IMediaStream;

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

		int32_t PlayMovie(LPCCH filePath);

		STATIC_ASSERT(sizeof(RECTF) == 0x10);
		STATIC_ASSERT(sizeof(Nu3DFMV) == 0x2C);
	}
}

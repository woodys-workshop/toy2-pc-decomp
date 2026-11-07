#pragma once

#include "Common.h"

#include <WINDOWS.H>
#include <MMSYSTEM.H>
#include <DSOUND.H>

namespace AudioManager
{
	struct StreamHandles
	{
		HANDLE bufferNotifyEvent;
		HANDLE streamStopEvent;
		HANDLE threadEvent;
		HANDLE threadSemaphore;
	};

	struct WAVEINFOCA
	{
		WAVEFORMATEX* pwfx;
		HMMIO hmmio;
		MMCKINFO mmck;
		MMCKINFO mmckInRIFF;
		LPDIRECTSOUNDBUFFER lpDSBStreamBuffer;
		DWORD dwBufferSize;
		DWORD dwNotifySize;
		DWORD dwNextWriteOffset;
		DWORD dwProgress;
		DWORD dwLastPos;
		BOOL bDonePlaying;
		BOOL bLoopFile;
		BOOL bFoundEnd;
	};

	STATIC_ASSERT(sizeof(StreamHandles) == 0x10);
	STATIC_ASSERT(sizeof(WAVEINFOCA) == 0x54);

	extern StreamHandles g_streamHandles;
	extern WAVEINFOCA g_wiWave;
	extern int32_t g_musicVolume;
	extern LPDIRECTSOUNDBUFFER g_musicSoundBuffer;
	extern LPDIRECTSOUND g_directSound;

	void ShutdownHandles();
	void SignalThreadExit();
	int32_t PlayTrackByIndex(int32_t index, int32_t loop);
	void ThreadPlay(LPSTR fileName, int32_t looping);
	void QueuePlay(const char* fileName, int32_t loopFlag);
	void SetMusicVolume(int32_t volume);

	int32_t WaveOpenFile(char* pszFileName, HMMIO* phmmioIn, WAVEFORMATEX** ppwfxInfo, MMCKINFO* pckInRIFF);
	int32_t WaveStartDataRead(HMMIO* phmmioIn, MMCKINFO* pckIn, MMCKINFO* pckInRIFF);
	int32_t WaveReadFile(HMMIO hmmioIn, UINT cbRead, BYTE* pbDest, MMCKINFO* pckIn, UINT* cbActualRead);
	int32_t WaveCloseReadFile(HMMIO* phmmio, WAVEFORMATEX** ppwfxSrc);

	namespace Stream
	{
		int32_t Init();
		void Stop();
		int32_t IsThreadReady();
		DWORD __cdecl ThreadProc();
	}
}

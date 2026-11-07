#include "AudioManager/AudioManager.h"
#include "AudioManager/MusicStream.h"
#include "FileUtils.h"

#include <STDLIB.H>
#include <STRING.H>

#define DPF 1 ? (void)0 : (void)

namespace AudioManager
{
	// GLOBAL: TOY2 0x005282CC
	int32_t g_curTrackIndex;

	// GLOBAL: TOY2 0x005282B8
	StreamHandles g_streamHandles;

	// GLOBAL: TOY2 0x005282C8
	int32_t g_musicVolume;

	// GLOBAL: TOY2 0x005282D8
	CRITICAL_SECTION g_audioCriticalSection;

	// GLOBAL: TOY2 0x005282F0
	LPDIRECTSOUNDBUFFER g_musicSoundBuffer;

	// GLOBAL: TOY2 0x005282F4
	LPDIRECTSOUNDNOTIFY g_directSoundNotify;

	// GLOBAL: TOY2 0x005282F8
	int32_t g_audioStreamInitialized;

	// GLOBAL: TOY2 0x005282FC
	int32_t g_audioStreamActive;

	// GLOBAL: TOY2 0x004ECCD8
	const char* g_gameMusicNames[22] = {
		"house",	"neighbou", "buzvred", "constr",   "alley",	   "slime",	   "als_toyb", "spacelnd", "buzvbuz", "elev",	  "als_pent",
		"buzvzurg", "convey",	"tarmac",  "buzvpros", "miniboss", "minirace", "over",	   "complete", "ygafim",  "titlescr", "levcomp"
	};

	enum
	{
		NUM_PLAY_NOTIFICATIONS = 16
	};

	// GLOBAL: TOY2 0x005281D8
	WAVEINFOCA g_wiWave;

	// GLOBAL: TOY2 0x00528230
	DSBPOSITIONNOTIFY g_positionNotifiers[NUM_PLAY_NOTIFICATIONS + 1];

	void OnExit();
	int32_t LoadFile(LPSTR fileName);

	// FUNCTION: TOY2 0x00412EE0
	void ShutdownHandles()
	{
		if (g_streamHandles.threadSemaphore)
			CloseHandle(g_streamHandles.threadSemaphore);

		if (g_streamHandles.threadEvent)
			CloseHandle(g_streamHandles.threadEvent);

		if (g_streamHandles.streamStopEvent)
			CloseHandle(g_streamHandles.streamStopEvent);

		if (g_streamHandles.bufferNotifyEvent)
			CloseHandle(g_streamHandles.bufferNotifyEvent);

		g_streamHandles.threadSemaphore = 0;
		g_streamHandles.threadEvent = 0;
		g_streamHandles.streamStopEvent = 0;
		g_streamHandles.bufferNotifyEvent = 0;
	}

	// FUNCTION: TOY2 0x00412F40 [MATCHED]
	int32_t CreateHandles()
	{
		g_streamHandles.bufferNotifyEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
		if (g_streamHandles.bufferNotifyEvent)
		{
			g_streamHandles.streamStopEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
			if (g_streamHandles.streamStopEvent)
			{
				g_streamHandles.threadEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
				if (g_streamHandles.threadEvent)
				{
					g_streamHandles.threadSemaphore = CreateSemaphoreA(NULL, 0, 1, NULL);
					if (g_streamHandles.threadSemaphore)
						return 1;

					CloseHandle(g_streamHandles.threadEvent);
					g_streamHandles.threadEvent = 0;
				}

				CloseHandle(g_streamHandles.streamStopEvent);
				g_streamHandles.streamStopEvent = 0;
			}

			CloseHandle(g_streamHandles.bufferNotifyEvent);
			g_streamHandles.bufferNotifyEvent = 0;
		}

		return 0;
	}

	namespace Stream
	{
		// FUNCTION: TOY2 0x00412FF0 [MATCHED]
		int32_t Init()
		{
			DWORD dwThreadId;

			if (! g_audioStreamInitialized)
			{
				InitializeCriticalSection(&g_audioCriticalSection);

				if (CreateHandles())
				{
					if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, NULL, 0, &dwThreadId))
					{
						while (! IsThreadReady())
							;

						g_audioStreamInitialized = 1;
						atexit(OnExit);

						return 1;
					}

					ShutdownHandles();
				}
			}

			return 0;
		}
	}

	// FUNCTION: TOY2 0x00413140
	void OnExit() { SignalThreadExit(); }

	// FUNCTION: TOY2 0x00413150
	int32_t PlayTrackByIndex(int32_t index, int32_t loop)
	{
		char fileName[512];

		StopAndWait();

		if (index >= 22 || index < 0)
			return 0;

		FileUtils::AppendCDPath(fileName);

		strcat(fileName, "audio\\");
		strcat(fileName, g_gameMusicNames[index]);
		strcat(fileName, ".wav");

		QueuePlay(fileName, loop);
		g_curTrackIndex = index;

		return 1;
	}

	// FUNCTION: TOY2 0x00413240 [MATCHED]
	void ThreadPlay(LPSTR fileName, int32_t looping)
	{
		if (g_directSound)
		{
			if (LoadFile(fileName))
			{
				g_wiWave.bLoopFile = looping & 1;

				g_musicSoundBuffer->SetCurrentPosition(0);
				SetMusicVolume(g_musicVolume);

				g_musicSoundBuffer->Play(0, 0, 1);
			}
		}
	}

	namespace Stream
	{
		// FUNCTION: TOY2 0x004132A0
		void Stop()
		{
			if (g_directSound)
			{
				if (g_musicSoundBuffer)
				{
					g_musicSoundBuffer->Stop();

					WaveCloseReadFile(&g_wiWave.hmmio, &g_wiWave.pwfx);

					g_directSoundNotify->Release();
					g_directSoundNotify = 0;

					g_musicSoundBuffer->Release();
					g_musicSoundBuffer = 0;
				}
			}
		}
	}

	// FUNCTION: TOY2 0x00413300
	int32_t IsStreamActive() { return g_audioStreamActive; }

	// FUNCTION: TOY2 0x00413310 [MATCHED]
	int32_t LoadFile(LPSTR fileName)
	{
		DSBUFFERDESC dsbd;
		HRESULT dsRetVal;
		int32_t nChkErr;
		int32_t nRem;
		DWORD dwSize;
		int32_t i;
		LPBYTE lpWrite1, lpWrite2;
		DWORD dwLen1, dwLen2;
		UINT uActualBytesWritten;
		DWORD dwBytes;

		if (! g_quiet)
		{
			if ((nChkErr = WaveOpenFile(fileName, &g_wiWave.hmmio, &g_wiWave.pwfx, &g_wiWave.mmckInRIFF)) == 0)
			{
				if ((nChkErr = WaveStartDataRead(&g_wiWave.hmmio, &g_wiWave.mmck, &g_wiWave.mmckInRIFF)) == 0)
				{
					g_wiWave.dwNotifySize = g_wiWave.pwfx->nSamplesPerSec * 3 * (DWORD)g_wiWave.pwfx->nBlockAlign;
					g_wiWave.dwNotifySize = g_wiWave.dwNotifySize / NUM_PLAY_NOTIFICATIONS;
					if ((nRem = g_wiWave.dwNotifySize % (DWORD)g_wiWave.pwfx->nBlockAlign) != 0)
					{
						g_wiWave.dwNotifySize += (g_wiWave.pwfx->nBlockAlign - nRem);
					}
					g_wiWave.dwBufferSize = g_wiWave.dwNotifySize * NUM_PLAY_NOTIFICATIONS;

					memset(&dsbd, 0, sizeof(DSBUFFERDESC));
					dsbd.dwSize = sizeof(DSBUFFERDESC);
					dsbd.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLDEFAULT | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS;
					dsbd.dwBufferBytes = g_wiWave.dwBufferSize;
					dsbd.lpwfxFormat = g_wiWave.pwfx;

					dsRetVal = g_directSound->CreateSoundBuffer(&dsbd, &g_musicSoundBuffer, NULL);
					if (dsRetVal == DS_OK)
					{
						g_wiWave.lpDSBStreamBuffer = g_musicSoundBuffer;
						g_wiWave.bFoundEnd = FALSE;
						g_wiWave.dwNextWriteOffset = 0;
						g_wiWave.bLoopFile = FALSE;

						dsRetVal = g_musicSoundBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&g_directSoundNotify);
						if (dsRetVal == DS_OK)
						{
							dwSize = g_wiWave.dwNotifySize;

							g_positionNotifiers[0].dwOffset = dwSize;
							g_positionNotifiers[0].hEventNotify = g_streamHandles.bufferNotifyEvent;

							for (i = 1; i < NUM_PLAY_NOTIFICATIONS; i++)
							{
								g_positionNotifiers[i].dwOffset = g_positionNotifiers[i - 1].dwOffset + dwSize;
								g_positionNotifiers[i].hEventNotify = g_streamHandles.bufferNotifyEvent;
							}

							g_positionNotifiers[i - 1].dwOffset -= 1;
							g_positionNotifiers[i].dwOffset = DSBPN_OFFSETSTOP;
							g_positionNotifiers[i].hEventNotify = g_streamHandles.streamStopEvent;

							g_directSoundNotify->SetNotificationPositions(NUM_PLAY_NOTIFICATIONS + 1, g_positionNotifiers);

							dwBytes = g_wiWave.dwBufferSize;

							dsRetVal = g_musicSoundBuffer->Lock(0, dwBytes, (LPVOID*)&lpWrite1, &dwLen1, (LPVOID*)&lpWrite2, &dwLen2, 0);

							if (dwLen1 != dwBytes)
							{
								DPF(0, "FillDataBuffer: dwBytes != dwLen1");
							}

							if (dsRetVal == DS_OK)
							{
								nChkErr = WaveReadFile(g_wiWave.hmmio, (UINT)dwLen1, lpWrite1, &g_wiWave.mmck, &uActualBytesWritten);

								if (uActualBytesWritten < dwLen1)
								{
									if (! g_wiWave.bLoopFile)
									{
										if (uActualBytesWritten < g_wiWave.dwNotifySize)
											g_wiWave.bFoundEnd = TRUE;

										FillMemory(
											lpWrite1 + uActualBytesWritten, dwLen1 - uActualBytesWritten, (BYTE)(g_wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0));
									}
									else
									{
										UINT uWritten = uActualBytesWritten;
										while (uWritten < dwLen1)
										{
											nChkErr = WaveStartDataRead(&g_wiWave.hmmio, &g_wiWave.mmck, &g_wiWave.mmckInRIFF);
											nChkErr = WaveReadFile(
												g_wiWave.hmmio, (UINT)dwLen1 - uWritten, lpWrite1 + uWritten, &g_wiWave.mmck, &uActualBytesWritten);
											uWritten += uActualBytesWritten;
										}
									}
								}

								dsRetVal = g_musicSoundBuffer->Unlock((LPVOID)lpWrite1, dwLen1, NULL, 0);

								g_wiWave.dwNextWriteOffset += dwLen1;
								if (g_wiWave.dwNextWriteOffset >= g_wiWave.dwBufferSize)
									g_wiWave.dwNextWriteOffset -= g_wiWave.dwBufferSize;

								g_wiWave.dwProgress = 0;
								g_wiWave.dwLastPos = 0;
							}

							g_wiWave.bDonePlaying = FALSE;

							return 1;
						}

						g_musicSoundBuffer->Release();
					}
				}

				WaveCloseReadFile(&g_wiWave.hmmio, &g_wiWave.pwfx);
			}
		}

		return 0;
	}
}

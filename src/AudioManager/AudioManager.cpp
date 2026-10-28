#include "AudioManager/AudioManager.h"
#include "Toy2/Toy2.h"
#include "Toy2/D3DApp.h"
#include "Toy2/Collision.h"
#include "Renderer/Renderer.h"
#include "FileUtils.h"
#include "Logger.h"
#include "Random.h"

#include <MMSYSTEM.H>
#include <DSOUND.H>
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
#include <MATH.H>

// TODO: probably refactor into a couple files, its a little large

namespace AudioManager
{
	struct AudioConfig
	{
		WORD formatTag;
		WORD channels;
		DWORD sampleRate;
		DWORD avgBytesPerSec;
		WORD blockAlign;
		WORD bitsPerSample;
	};

	struct SoundEffect
	{
		char** wavList;
		int32_t effectId;
	};

	struct AudioChannel
	{
		int16_t bufferIndex;
		int16_t timer;
		int16_t curVolume;
		int16_t targetVolume;
		int16_t repeatCounter;
	};

	// the stream thread waits for the first three of them at once, so they have to stay in this order
	struct StreamHandles
	{
		HANDLE bufferNotifyEvent;
		HANDLE streamStopEvent;
		HANDLE threadEvent;
		HANDLE threadSemaphore;
	};

	STATIC_ASSERT(sizeof(AudioConfig) == 0x10);
	STATIC_ASSERT(sizeof(SoundEffect) == 0x8);
	STATIC_ASSERT(sizeof(AudioChannel) == 0xA);
	STATIC_ASSERT(sizeof(StreamHandles) == 0x10);

	// GLOBAL: TOY2 0x005282CC
	int32_t g_curTrackIndex;

	// GLOBAL: TOY2 0x00830E3C
	int16_t g_pitchArray[14];

	// the music that is streamed from the audio folder
	// GLOBAL: TOY2 0x005281D8
	LPWAVEFORMATEX g_musicWaveFormat;

	// GLOBAL: TOY2 0x005281DC
	HMMIO g_musicFileHandle;

	// GLOBAL: TOY2 0x005281E0
	MMCKINFO g_musicDataChunk;

	// GLOBAL: TOY2 0x005281F4
	MMCKINFO g_musicRiffChunk;

	// GLOBAL: TOY2 0x00528208
	LPDIRECTSOUNDBUFFER g_musicSoundBufferCopy;

	// GLOBAL: TOY2 0x0052820C
	DWORD g_totalBufferSize;

	// GLOBAL: TOY2 0x00528210
	DWORD g_chunkSize;

	// GLOBAL: TOY2 0x00528214
	DWORD g_writeOffset;

	// GLOBAL: TOY2 0x00528218
	DWORD g_totalBytesPlayed;

	// GLOBAL: TOY2 0x0052821C
	DWORD g_lastPlayCursor;

	// GLOBAL: TOY2 0x00528220
	int32_t g_streamFinished;

	// GLOBAL: TOY2 0x00528224
	int32_t g_looping;

	// GLOBAL: TOY2 0x00528228
	int32_t g_eofReached;

	// 16 positions for the chunks of the buffer and one to stop
	// GLOBAL: TOY2 0x00528230
	DSBPOSITIONNOTIFY g_positionNotifiers[17];

	// the stream thread waits on the first three of them, the order matters
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

	// GLOBAL: TOY2 0x0053C844
	int32_t g_pendingLoopFlag;

	// 1 stops the music, 2 plays the file in g_fileNameBuffer and 3 closes the handles
	// GLOBAL: TOY2 0x0053C848
	int32_t g_setThreadEvent;

	// GLOBAL: TOY2 0x0053C84C
	CHAR g_fileNameBuffer[516];

	// GLOBAL: TOY2 0x0053CA50
	int32_t g_streamThreadReady;

	// GLOBAL: TOY2 0x00559C68
	int32_t g_curLoopingTrack;

	// the sound effects, every effect has 6 buffers that can play at the same time
	// GLOBAL: TOY2 0x004FCDB0
	int32_t g_sfxVolume = 256;

	// GLOBAL: TOY2 0x004FCDBC
	int32_t g_reloadEffectIndex = -1;

	// the sound pack of the level that is loaded
	// GLOBAL: TOY2 0x004FCDC0
	int32_t g_curSfxPack = 1;

	// GLOBAL: TOY2 0x00724E78
	DWORD g_playCursor;

	// GLOBAL: TOY2 0x00724E7C
	HRESULT g_lastDSoundError;

	// GLOBAL: TOY2 0x00724E80
	int32_t g_directSoundInit;

	// GLOBAL: TOY2 0x00724E84
	int32_t g_audioDeviceCount;

	// GLOBAL: TOY2 0x00724E88
	uint8_t* g_wavSourceData;

	// GLOBAL: TOY2 0x00724E8C
	char g_audioDeviceDecs[32][32];

	// GLOBAL: TOY2 0x00725290
	AudioConfig* g_audioConfig;

	// the owner of a buffer, 1 when it has none
	// GLOBAL: TOY2 0x00725294
	void* g_soundBufferStates[768];

	// GLOBAL: TOY2 0x00725E94
	int32_t g_pendingTrackIndex;

	// GLOBAL: TOY2 0x00725E98
	LPGUID g_audioDeviceGuids[32];

	// GLOBAL: TOY2 0x00725F24
	LPDIRECTSOUND g_directSound;

	// GLOBAL: TOY2 0x00725F28
	int32_t g_unused;

	// GLOBAL: TOY2 0x00725F2C
	int32_t g_wavDataSize;

	// GLOBAL: TOY2 0x00726230
	int32_t g_unk726230;

	// GLOBAL: TOY2 0x00726234
	int16_t g_soundEffectSampleRates[128];

	// GLOBAL: TOY2 0x00726334
	DWORD g_writeCursor;

	// GLOBAL: TOY2 0x00726338
	LPDIRECTSOUNDBUFFER g_soundEffectBuffers[768];

	// GLOBAL: TOY2 0x00726F3C
	int32_t g_audioShutdownPending;

	// the looping sounds (and the sounds of actors) that are tracked
	// GLOBAL: TOY2 0x00830E58
	AudioChannel g_audioChannels[32];

	// GLOBAL: TOY2 0x004ECCD8
	const char* g_gameMusicNames[22] = {
		"house",	"neighbou", "buzvred", "constr",   "alley",	   "slime",	   "als_toyb", "spacelnd", "buzvbuz", "elev",	  "als_pent",
		"buzvzurg", "convey",	"tarmac",  "buzvpros", "miniboss", "minirace", "over",	   "complete", "ygafim",  "titlescr", "levcomp"
	};

	// GLOBAL: TOY2 0x004FD668
	int16_t g_dSoundVolumeConversion[160] = {
		-10000, -9800, -9600, -9300, -9000, -8700, -8300, -8000, -7250, -6500, -5750, -5500, -5250, -5000, -4500, -4000, -3900, -3800, -3700, -3600,
		-3500,	-3400, -3300, -3200, -3100, -3000, -2920, -2840, -2770, -2700, -2630, -2560, -2490, -2420, -2350, -2280, -2210, -2140, -2070, -2000,
		-1960,	-1912, -1864, -1830, -1790, -1750, -1710, -1670, -1630, -1590, -1550, -1510, -1470, -1430, -1390, -1340, -1290, -1250, -1210, -1168,
		-1126,	-1084, -1042, -1000, -985,	-970,  -955,  -940,	 -925,	-910,  -895,  -880,	 -865,	-850,  -835,  -820,	 -805,	-790,  -775,  -759,
		-743,	-727,  -711,  -695,	 -679,	-663,  -647,  -631,	 -615,	-599,  -583,  -567,	 -551,	-535,  -519,  -503,	 -487,	-471,  -455,  -439,
		-423,	-407,  -391,  -375,	 -359,	-343,  -327,  -311,	 -295,	-279,  -263,  -247,	 -231,	-215,  -199,  -183,	 -167,	-151,  -135,  -119,
		-103,	-87,   -71,	  -55,	 -39,	-23,   -7,	  -6,	 -5,	-4,	   -3,	  -2,	 -1,	0,	   0,	  0,	 0,		0,	   0,	  0,
		0,		0,	   0,	  0,	 0,		0,	   0,	  0,	 0,		0,	   0,	  0,	 0,		0,	   0,	  0,	 0,		0,	   0,	  0
	};

	// pairs of a level and a sound, the preset points to the first pair of its sound
	// GLOBAL: TOY2 0x005028E8
	int16_t g_unk5028E8[52] = {
		1, 88,	   2, 95,	  8, 92,	 1,	 -32676, 2,	 -32680, 2, -32681, 5,	-32679, 11, -32673, 2,	92, 5, 87, 5,  -32680, 3,  -32679, 14, -32679,
		6, -32676, 3, -32681, 9, -32676, 12, -32681, 15, -32681, 4, 91,		13, 88,		4,	90,		13, 89, 3, 88, 14, 90,	   13, 90,	   15, 90
	};

	// GLOBAL: TOY2 0x00502950
	SoundEffectPreset g_soundEffectPresets[218] = {
		{ 23, 5120, 64, 0, 2, 0, 0, 0 },		 { 50, 2816, 64, 0, 2, 1, 0, 0 },		  { 11, 5120, 96, 0, 2, 0, 0, 0 },
		{ 50, 5120, 64, 0, 2, 0, 0, 0 },		 { 50, 4608, 64, 0, 2, 0, 0, 0 },		  { 50, 4096, 64, 0, 2, 1, 0, 0 },
		{ 28, 5120, 96, 0, 2, 0, 5, 255 },		 { 39, 5120, 96, 96, 1, 0, 0, 0 },		  { 38, 5120, 96, 0, 2, 0, 0, 0 },
		{ 46, 5120, 80, 0, 2, 0, 5, 150 },		 { 32, 5120, 110, 0, 2, 0, 20, 255 },	  { 33, 5120, 80, 0, 2, 0, 10, 200 },
		{ 87, 4352, 96, 0, 2, 0, 0, 0 },		 { 43, 5120, 80, 0, 2, 0, 0, 0 },		  { 44, 5120, 64, 0, 2, 0, 0, 0 },
		{ 22, 5120, 96, 0, 4, 10, 20, 255 },	 { 27, 5120, 80, 0, 4, 0, 0, 0 },		  { 25, 5120, 64, 0, 2, 0, 0, 0 },
		{ 20, 5120, 48, 0, 2, 0, 0, 0 },		 { 2, 4352, 48, 40, -1, 0, 0, 0 },		  { 3, 4352, 40, 48, -1, 0, 0, 0 },
		{ 5, 5120, 96, 0, -1, 0, 0, 0 },		 { -32761, 5120, 80, 10, -1, 0, 0, 0 },	  { 4, 5120, 80, 0, -1, 0, 0, 0 },
		{ 6, 5120, 64, 0, -1, 0, 15, 200 },		 { 8, 5120, 80, 0, 2, 0, 0, 0 },		  { 10, 5120, 96, 0, 3, 7, 0, 0 },
		{ 21, 5120, 64, 0, 4, 0, 0, 0 },		 { 47, 5632, 96, 0, 2, 10, 10, 120 },	  { 48, 5120, 64, 64, 2, 0, 0, 0 },
		{ 49, 5120, 64, 64, 2, 0, 0, 0 },		 { 36, 5120, 64, 0, -1, 0, 0, 0 },		  { 95, 5120, 96, 0, -1, 0, 0, 0 },
		{ 96, 3072, 96, 0, -1, 0, 0, 0 },		 { 94, 5120, 96, 0, -1, 0, 0, 0 },		  { -32755, 5120, 64, 0, -1, 0, 0, 0 },
		{ 90, 5120, 64, 0, 3, 0, 10, 140 },		 { 91, 5120, 64, 0, 4, 0, 10, 140 },	  { -32742, 6144, 96, 0, -1, 0, 4, 200 },
		{ -32751, -3, 96, 0, -1, 0, 0, 0 },		 { 37, 6144, 48, 0, -1, 0, 0, 0 },		  { 35, 4096, 96, 0, -1, 0, 0, 0 },
		{ 34, 5120, 96, 0, -1, 0, 0, 0 },		 { 35, 4608, 96, 0, -1, 0, 0, 0 },		  { -32723, 4608, 64, 10, 4, 0, 0, 0 },
		{ 15, 5376, 64, 0, -1, 0, 0, 0 },		 { -32738, 4608, 64, 0, -1, 0, 4, 255 },  { 1, 4608, 96, 0, 4, 0, 15, 255 },
		{ 19, 5376, 64, 0, 4, 0, 0, 0 },		 { 4, 3584, 64, 0, -1, 0, 0, 0 },		  { 89, 5120, 64, 0, -1, 0, 0, 0 },
		{ 42, 4608, 96, 0, -1, 0, 0, 0 },		 { 16400, 5120, 96, 0, -1, 0, 15, 255 },  { -32675, -1, 96, 0, -1, 0, 0, 0 },
		{ 16384, 5120, 48, 0, 3, 0, 0, 0 },		 { 16390, -2, 48, 0, -1, 0, 0, 0 },		  { 90, 5120, 96, 0, -1, 0, 0, 0 },
		{ 18, 5120, 64, 0, 2, 0, 0, 0 },		 { 18, 3584, 96, 0, 2, 0, 0, 0 },		  { 22, 4608, 96, 0, 4, 0, 0, 0 },
		{ -32742, 0, 96, 0, -1, 0, 0, 0 },		 { 40, 5120, 64, 64, -1, 0, 0, 0 },		  { 41, 5120, 64, 64, -1, 0, 0, 0 },
		{ 35, 5120, 64, 64, -1, 0, 0, 0 },		 { -32754, 4608, 110, 10, 3, 0, 4, 200 }, { -32755, 0, 64, 0, -1, 0, 0, 0 },
		{ 37, 4608, 48, 0, -1, 0, 0, 0 },		 { 23, 6144, 48, 0, 2, 0, 0, 0 },		  { -32755, 3584, 96, 10, 4, 0, 0, 0 },
		{ 90, 5120, 80, 0, -1, 0, 0, 0 },		 { 2, 1536, 96, 0, -1, 0, 0, 0 },		  { 91, 4352, 110, 0, -1, 0, 0, 0 },
		{ 93, 5120, 64, 0, -1, 0, 0, 0 },		 { 94, 5120, 96, 0, -1, 0, 0, 0 },		  { 9, 5120, 96, 0, 2, 0, 0, 0 },
		{ -32677, 5120, 96, 30, 2, 0, 0, 0 },	 { 12, 5120, 110, 0, -1, 0, 0, 0 },		  { -32739, 0, 96, 30, -1, 0, 4, 100 },
		{ 9, 4352, 96, 0, 2, 0, 0, 0 },			 { 16, 5120, 96, 0, -1, 0, 0, 0 },		  { -32744, 5120, 96, 10, 3, 0, 4, 100 },
		{ 51, 5120, 96, 0, 2, 0, 0, 0 },		 { 52, 5120, 96, 0, 2, 0, 10, 100 },	  { -32715, 2048, 128, 10, 3, 0, 4, 100 },
		{ 51, 5120, 96, 0, 2, 0, 0, 0 },		 { 37, 6144, 32, 0, 2, 0, 0, 0 },		  { 31, 5120, 96, 0, 2, 0, 0, 0 },
		{ -32709, 5120, 64, 10, 3, 0, 0, 0 },	 { 60, 5120, 96, 0, 2, 0, 0, 0 },		  { 61, 5120, 96, 0, 2, 0, 0, 0 },
		{ 58, 5120, 96, 0, 2, 0, 0, 0 },		 { -32713, 5120, 96, 30, 1, 0, 0, 0 },	  { -32714, 5120, 96, 0, -1, 0, 4, 100 },
		{ -32711, 5120, 96, 30, 3, 0, 0, 0 },	 { -32712, 5120, 48, 0, -1, 0, 0, 0 },	  { 16394, 4608, 24, 60, 1, 0, 0, 0 },
		{ 89, 4096, 110, 10, 3, 0, 0, 0 },		 { -32681, 4608, 128, 70, 1, 0, 0, 0 },	  { 88, 4608, 96, 0, -1, 0, 0, 0 },
		{ 89, 4608, 110, 0, -1, 0, 0, 0 },		 { 18, 5120, 96, 0, 2, 0, 0, 0 },		  { -32678, 4608, 96, 0, -1, 0, 0, 0 },
		{ -32677, 4352, 128, 10, -1, 0, 0, 0 },	 { 16410, 4352, 24, 120, 1, 0, 0, 0 },	  { 89, 5120, 96, 0, 2, 0, 0, 0 },
		{ 87, 5120, 96, 0, 2, 0, 15, 255 },		 { 88, 4352, 96, 0, 2, 0, 0, 0 },		  { 16418, 5120, 80, 0, -1, 0, 0, 0 },
		{ 16422, 5120, 96, 0, -1, 0, 0, 0 },	 { 92, 5120, 96, 0, -1, 0, 0, 0 },		  { -32675, 5120, 96, 30, 2, 0, 0, 0 },
		{ 16404, 3584, 24, 60, 2, 0, 0, 0 },	 { 16426, 4352, 96, 0, 2, 0, 15, 200 },	  { 87, 4608, 96, 0, 0, 0, 7, 100 },
		{ 88, 5120, 96, 0, 2, 0, 0, 0 },		 { 89, 5120, 96, 0, 2, 0, 0, 0 },		  { 90, 5120, 96, 0, 2, 0, 0, 0 },
		{ -32677, 0, 96, 0, -1, 0, 0, 0 },		 { 92, 5120, 96, 0, 2, 0, 0, 0 },		  { 93, 5120, 96, 0, 2, 0, 0, 0 },
		{ 94, 5120, 64, 0, 2, 0, 0, 0 },		 { 95, 5120, 96, 0, 2, 0, 0, 0 },		  { -32672, 0, 96, 0, -1, 0, 4, 100 },
		{ 87, 5120, 96, 0, 2, 0, 0, 0 },		 { 88, 5120, 96, 0, 2, 0, 0, 0 },		  { 89, 5120, 96, 0, 2, 0, 0, 0 },
		{ -32678, 0, 128, 0, -1, 0, 0, 0 },		 { -32677, 0, 128, 0, -1, 0, 0, 0 },	  { 93, 5376, 128, 0, -1, 0, 0, 0 },
		{ 94, 7168, 64, 0, -1, 0, 0, 0 },		 { -32673, 0, 96, 0, -1, 0, 0, 0 },		  { 87, 5120, 110, 0, 2, 0, 0, 0 },
		{ 88, 5120, 110, 0, 2, 0, 0, 0 },		 { 89, 5120, 96, 0, 2, 0, 0, 0 },		  { 90, 5120, 96, 0, 2, 0, 0, 0 },
		{ -32714, 4352, 128, 0, -1, 0, 4, 100 }, { 89, 5120, 96, 0, 2, 0, 0, 0 },		  { 90, 5120, 96, 0, 2, 0, 0, 0 },
		{ 91, 5120, 96, 0, 2, 0, 0, 0 },		 { 94, 4096, 96, 0, 2, 0, 0, 0 },		  { -32675, 0, 80, 0, -1, 0, 0, 0 },
		{ 95, 4352, 48, 0, 2, 0, 0, 0 },		 { 96, 5120, 96, 0, 2, 0, 0, 0 },		  { -32681, 5120, 96, 0, 2, 0, 0, 0 },
		{ 88, 5120, 96, 0, 2, 0, 0, 0 },		 { 87, 5120, 96, 0, 2, 0, 0, 0 },		  { 88, 5120, 96, 0, 2, 0, 10, 255 },
		{ 89, 5120, 96, 0, 2, 0, 0, 0 },		 { 90, 5632, 96, 0, 2, 0, 0, 0 },		  { -32677, 5120, 32, 0, 2, 0, 0, 0 },
		{ 92, 5120, 96, 0, 2, 0, 0, 0 },		 { 93, 5120, 96, 0, -1, 0, 0, 0 },		  { 96, 5120, 96, 0, -1, 0, 0, 0 },
		{ -32673, 5120, 64, 0, 2, 0, 4, 255 },	 { 96, 5120, 96, 0, 2, 0, 0, 0 },		  { -32681, 5120, 128, 0, 2, 0, 0, 0 },
		{ -32677, 5120, 128, 0, 2, 0, 0, 0 },	 { 90, 5120, 96, 0, 2, 0, 0, 0 },		  { -32679, 5120, 64, 30, 2, 0, 0, 0 },
		{ 88, 4608, 96, 0, -1, 0, 0, 0 },		 { 91, 4096, 96, 0, -1, 0, 0, 0 },		  { 90, 5120, 96, 0, 2, 0, 0, 0 },
		{ 92, 5120, 96, 0, 2, 0, 0, 0 },		 { -32675, 0, 64, 0, -1, 0, 0, 0 },		  { 94, 5120, 96, 0, 2, 0, 0, 0 },
		{ 94, 5120, 96, 0, 2, 0, 0, 0 },		 { 16430, 5120, 96, 0, 2, 0, 10, 255 },	  { 22, 5120, 96, 0, 4, 0, 10, 255 },
		{ -32681, 5120, 64, 0, -1, 0, 0, 0 },	 { 69, 5376, 128, 128, -1, 0, 0, 0 },	  { 70, 5376, 128, 0, -1, 0, 0, 0 },
		{ 71, 5376, 128, 0, -1, 0, 0, 0 },		 { 72, 5376, 128, 0, -1, 0, 0, 0 },		  { 73, 5376, 128, 0, -1, 0, 0, 0 },
		{ 74, 5376, 128, 0, -1, 0, 0, 0 },		 { 75, 5376, 128, 0, -1, 0, 0, 0 },		  { 76, 5376, 128, 0, -1, 0, 0, 0 },
		{ 77, 5376, 128, 0, -1, 0, 0, 0 },		 { 78, 5376, 128, 0, -1, 0, 0, 0 },		  { 79, 5376, 128, 0, -1, 0, 0, 0 },
		{ 80, 5376, 128, 0, -1, 0, 0, 0 },		 { 81, 5376, 128, 0, -1, 0, 0, 0 },		  { 82, 5376, 128, 0, -1, 0, 0, 0 },
		{ 83, 5376, 128, 0, -1, 0, 0, 0 },		 { 96, 5376, 128, 0, -1, 0, 0, 0 },		  { 91, 5376, 96, 0, -1, 0, 0, 0 },
		{ 92, 5376, 96, 0, -1, 0, 0, 0 },		 { 93, 5376, 96, 0, -1, 0, 0, 0 },		  { 94, 5376, 96, 0, -1, 0, 0, 0 },
		{ 98, 5376, 96, 0, -1, 0, 0, 0 },		 { 95, 5376, 128, 0, -1, 0, 0, 0 },		  { 97, 5376, 128, 0, -1, 0, 0, 0 },
		{ 96, 5376, 96, 0, -1, 0, 0, 0 },		 { 94, 5120, 96, 0, 2, 0, 0, 0 },		  { 91, 5376, 128, 0, -1, 0, 0, 0 },
		{ 92, 5376, 128, 0, -1, 0, 0, 0 },		 { 93, 5376, 128, 0, -1, 0, 0, 0 },		  { 95, 5376, 128, 0, -1, 0, 0, 0 },
		{ 96, 5376, 128, 0, -1, 0, 0, 0 },		 { 97, 5376, 128, 0, -1, 0, 0, 0 },		  { 98, 5376, 128, 0, -1, 0, 0, 0 },
		{ 99, 5376, 128, 0, -1, 0, 0, 0 },		 { 100, 5376, 128, 0, -1, 0, 0, 0 },	  { 101, 5376, 128, 0, -1, 0, 0, 0 },
		{ 102, 5376, 128, 0, -1, 0, 0, 0 },		 { 103, 5376, 128, 0, -1, 0, 0, 0 },	  { 67, 5376, 96, 96, -1, 0, 0, 0 },
		{ 68, 5376, 96, 96, -1, 0, 0, 0 },		 { 93, 5376, 96, 96, -1, 0, 0, 0 },		  { 92, 5376, 96, 96, -1, 0, 0, 0 },
		{ 93, 5376, 96, 96, -1, 0, 0, 0 },		 { 47, 5632, 96, 0, 2, 0, 10, 120 },	  { 97, 5376, 96, 96, -1, 0, 0, 0 },
		{ 93, 5376, 96, 96, -1, 0, 0, 0 },		 { 95, 5376, 128, 128, -1, 0, 0, 0 },	  { 94, 5376, 96, 96, -1, 0, 0, 0 },
		{ 99, 5376, 96, 96, -1, 0, 0, 0 },		 { 100, 5376, 128, 128, -1, 0, 0, 0 }
	};

	// GLOBAL: TOY2 0x005036F0
	int16_t g_soundScript1[28] = { 2, 0, 0, 41, 6144, 64, 2, 41, 7040, 64, 2, 41, 7680, 64, 10, 41, 7680, 48, 10, 41, 7680, 32, 10, 41, 7680, 16, 10, -1 };

	// GLOBAL: TOY2 0x00503728
	int16_t g_soundScript2[24] = { 0, 0, 0, 32, 5120, 96, 12, 32, 4608, 96, 15, 32, 5376, 96, 10, 33, 4864, 96, 16, 33, 4096, 96, 18, -1 };

	// GLOBAL: TOY2 0x00503758
	int16_t g_soundScript3[22] = { 0, 0, 0, 41, 4608, 64, 4, 41, 5504, 64, 4, 41, 6144, 64, 4, 41, 5504, 64, 4, -2, 16, 0 };

	// GLOBAL: TOY2 0x00503784
	int16_t g_soundScript4[24] = { 0, 0, 0, 91, 5120, 94, 20, 91, 5504, 94, 20, 91, 4864, 94, 20, 91, 5760, 94, 20, 91, 4736, 94, 20, -1 };

	// GLOBAL: TOY2 0x005037B4
	int16_t g_soundScript5[44] = {
		0, 0,  0,	 41, 4864, 64, 5,	 41, 4352, 64, 5,	 41, 4864, 64, 5,	 41, 5248, 64, 5,	 41, 4864, 64,
		5, 41, 5248, 64, 5,	   41, 5888, 64, 10,   41, 5888, 48, 10,   41, 5888, 32, 10,   41, 5888, 16, 10,   -1
	};

	// GLOBAL: TOY2 0x0050380C
	int16_t g_soundScript6[14] = { 0, 0, 0, 41, 4608, 64, 10, 41, 4608, 64, 10, -2, 16, 0 };

	// GLOBAL: TOY2 0x00503828
	int16_t* g_soundScripts[6] = { g_soundScript1, g_soundScript2, g_soundScript3, g_soundScript4, g_soundScript5, g_soundScript6 };

	// GLOBAL: TOY2 0x004FCDC4
	char* g_sfxWavList4FCDC4[71] = {
		"BOXFALL",	"BUZBRTH1", "BUZBRTH2", "BUZCLIMB", "BUZDIE1",	"BUZDIZZY", "BUZFALL1", "BUZJMP1",	"SPLAT",	"BUZJMP3",	"BUZLASER", "BOOTLNCH",
		"BUZPOLE",	"BOOTLOOP", "BUZPUSH1", "50COINS",	"BUZPWRUP", "SPLASH",	"BUZSHAKE", "BUZSKID",	"BUZSPIN",	"BUZSTOMP", "BUZSTPL",	"GRAVBOOT",
		"BUZTSPIN", "BUZWHIRL", "BUZWING",	"BUZYLASR", "COSSHLD",	"DRAGHEVY", "GUNFIRE2", "EXPLOBIG", "EXPLOSMA", "GENBEEP2", "GENBEEP4", "GENBEEP6",
		"GENBEEP7", "LASRBURN", "LASRRIC1", "PICKUP1",	"PICKUP5",	"PZZATOKN", "ROB2LASR", "ROBBOING", "ROBCOPTR", "ROBHIT",	"SPRNGJMP", "TEXTBOX1",
		"TEXTBOX2", "WETSTEPL", "DISCFIRE", "GRAPHOOK", "GRAPLOOP", "LASRBEAM", "PODHOVER", "CLOKTICK", "TOYPLANE", "SNAKEHIS", "VULTFLAP", "VULTSQ1",
		"VULTDIE",	"",			"",			"",			"",			"",			"",			"",			"",			"",			0
	};

	// GLOBAL: TOY2 0x004FCEE0
	char* g_sfxWavList4FCEE0[11] = { "HOTSPLSH", "CARSKID", "COTLATCH", "ELECDRIL", "ELECSAW", "CARENGIN", "ROBOWHIR", "ROBWIND", "SHEEPBAA", "SPADEFAL", 0 };

	// GLOBAL: TOY2 0x004FCF0C
	char* g_sfxWavList4FCF0C[11] = { "UNDRWATR", "CARENGIN", "FLARE", "INFLATE", "MOWER", "PLANKDRP", "SOLDRHUH", "SOLDRYES", "CARSKID", "kiteboss", 0 };

	// GLOBAL: TOY2 0x004FCF38
	char* g_sfxWavList4FCF38[7] = { "BIPLANE", "FLYBY1", "FLYBY2", "PLANEGUN", "SPUTTER", "GRDNWIND", 0 };

	// GLOBAL: TOY2 0x004FCF54
	char* g_sfxWavList4FCF54[11] = { "PISTONDN", "PISTONUP", "RIVETS", "YIPPEE", "OVERHERE", "LIFTSTOP", "LIFTLOOP", "Switch", "DrilBos1", "DrilBos2", 0 };

	// GLOBAL: TOY2 0x004FCF80
	char* g_sfxWavList4FCF80[10] = { "PlankDrp", "RainLoop", "UndrWatr", "Clown2", "Balloon", "DuckQuak", "ElecFan", "Elecshok", "Switch", 0 };

	// GLOBAL: TOY2 0x004FCFA8
	char* g_sfxWavList4FCFA8[6] = { "BigWind", "Thunder", "RainLoop", "TrashLid", "AllBoss", 0 };

	// GLOBAL: TOY2 0x004FCFC0
	char* g_sfxWavList4FCFC0[11] = { "BaskBall", "Cockerel", "HitLocks", "TennisBl", "BBronco", "Chicken", "ChickSqk", "DragnBrF", "DragnRrr", "SkBoard", 0 };

	// GLOBAL: TOY2 0x004FCFEC
	char* g_sfxWavList4FCFEC[12] = {
		"Vidgame1", "Vidgame2", "Vidgame3", "UfoLoop", "BuggyEng", "CarSkid", "usali01", "LIFTSTOP", "LiftLoop", "usali02", "usbuz79", 0
	};

	// GLOBAL: TOY2 0x004FD01C
	char* g_sfxWavList4FD01C[7] = { "AlienDie", "AlnBoss1", "AlnExplo", "AlnPods", "Earthqke", "alienhum", 0 };

	// GLOBAL: TOY2 0x004FD038
	char* g_sfxWavList4FD038[11] = { "LiftLoop", "LiftStop", "SpdrMove", "SpdrRoar", "SpidrDie", "VENTWIND", "VentFan", "DragnBrF", "Elecbuzz", "MouseSqk", 0 };

	// GLOBAL: TOY2 0x004FD064
	char* g_sfxWavList4FD064[10] = { "HorsShoe", "Cannon", "Gunfire4", "Gunfire5", "BathWatr", "RabChomp", "ToyTrain", "MaleHit", "UndrWatr", 0 };

	// GLOBAL: TOY2 0x004FD08C
	char* g_sfxWavList4FD08C[5] = { "VENTWIND", "JediExpl", "JediWind", "ZurgShot", 0 };

	// GLOBAL: TOY2 0x004FD0A0
	char* g_sfxWavList4FD0A0[5] = { "ConvBelt", "Overhere", "Yippee", "PickAxe", 0 };

	// GLOBAL: TOY2 0x004FD0B4
	char* g_sfxWavList4FD0B4[6] = { "Helicopt", "", "RainLoop", "Thunder", "proplane", 0 };

	// GLOBAL: TOY2 0x004FD0CC
	char* g_sfxWavList4FD0CC[5] = { "PlaneTxi", "Gunfire4", "Gunfire5", "PickAxe", 0 };

	// GLOBAL: TOY2 0x004FD0E0
	char* g_sfxWavList4FD0E0[23] = {
		"Select",  "Switch",  "Cancel",	 "GenBeep2", "GenBeep4", "GenBeep6", "GenBeep7", "Level01", "Level02", "Level03", "Level04", "Level05",
		"Level06", "Level07", "Level08", "Level09",	 "Level10",	 "Level11",	 "Level12",	 "Level13", "Level14", "Level15", 0
	};

	// GLOBAL: TOY2 0x004FD1C8
	char* g_sfxWavList4FD1C8[8] = { "USREX14", "USREX05", "USHAMM05", "USHAMM01", "USHAMM17", "USMRP02", "USMRP12", 0 };

	// GLOBAL: TOY2 0x004FD1E8
	char* g_sfxWavList4FD1E8[21] = {
		"USBUZ81", "USBUZ49", "BuzzInf", "USREX14", "USREX15", "USHAMM05", "USHAMM03", "USHAMM18", "USMRP01", "USMRP31", "",
		"USBOP02", "USBOP04", "USBOP05", "",		"",		   "",		   "",		   "",		   "",		  0
	};

	// GLOBAL: TOY2 0x004FD23C
	char* g_sfxWavList4FD23C[15] = {
		"USBUZ43", "USBUZ81", "BUZZINF", "USREX13", "USREX05", "USHAMM04", "USHAMM01", "USHAMM18", "", "", "", "USSARG01", "USSARG05", "USSARG07", 0
	};

	// GLOBAL: TOY2 0x004FD278
	char* g_sfxWavList4FD278[4] = { "USBUZ55", "USBUZ50", "BUZZINF", 0 };

	// GLOBAL: TOY2 0x004FD288
	char* g_sfxWavList4FD288[18] = {
		"USBUZ81", "USBUZ47", "BUZZINF", "USREX14", "USREX15", "USHAMM05", "USHAMM03", "USHAMM18", "USMRP06",
		"USMRP11", "",		  "",		 "",		"",		   "USSLIN13", "USSLIN15", "USSLIN17", 0
	};

	// GLOBAL: TOY2 0x004FD2D0
	char* g_sfxWavList4FD2D0[18] = {
		"USBUZ51", "USBUZ81", "BUZZINF", "USREX12", "USREX05", "USHAMM04", "USHAMM06", "USHAMM17", "", "", "", "", "", "", "USSLIN13", "USSLIN01", "USSLIN17", 0
	};

	// GLOBAL: TOY2 0x004FD318
	char* g_sfxWavList4FD318[4] = { "USBUZ43", "USBUZ55", "BUZZINF", 0 };

	// GLOBAL: TOY2 0x004FD328
	char* g_sfxWavList4FD328[11] = { "USBUZ81", "USBUZ51", "BUZZINF", "USREX21", "USREX04", "USHAMM04", "USHAMM02", "USHAMM18", "USMRP02", "USMRP09", 0 };

	// GLOBAL: TOY2 0x004FD354
	char* g_sfxWavList4FD354[9] = { "USBUZ81", "USBUZ49", "BUZZINF", "USREX12", "USREX15", "USHAMM04", "USHAMM02", "USHAMM18", 0 };

	// GLOBAL: TOY2 0x004FD378
	char* g_sfxWavList4FD378[4] = { "USBUZ81", "USBUZ55", "BUZZINF", 0 };

	// GLOBAL: TOY2 0x004FD388
	char* g_sfxWavList4FD388[11] = { "USBUZ51", "USBUZ81", "BUZZINF", "USREX14", "USREX05", "USHAMM05", "USHAMM01", "USHAMM17", "USMRP02", "USMRP12", 0 };

	// GLOBAL: TOY2 0x004FD3B4
	char* g_sfxWavList4FD3B4[15] = {
		"USBUZ51", "USBUZ81", "BUZZINF", "USREX13", "USREX20", "USHAMM04", "USHAMM02", "USHAMM17", "", "", "", "USJESS07", "USJESS06", "USJESS03", 0
	};

	// GLOBAL: TOY2 0x004FD3F0
	char* g_sfxWavList4FD3F0[33] = {
		"USBUZ81", "USBUZ55", "BUZZINF", "", "", "", "", "",		 "",		 "",		 "",		 "",		 "",		 "",		 "",		 "", "",
		"",		   "",		  "",		 "", "", "", "", "USZURG01", "USZURG04", "USZURG06", "USZURG07", "USZURG10", "USZURG12", "USZURG13", "USZURG16", 0
	};

	// GLOBAL: TOY2 0x004FD474
	char* g_sfxWavList4FD474[31] = {
		"USBUZ81", "USBUZ47", "BUZZINF", "USREX01", "USREX04", "USHAMM05", "USHAMM03", "USHAMM18", "", "",		   "USMRP10",  "", "", "", "", "", "", "", "",
		"",		   "",		  "",		 "",		"",		   "USPROS03", "USPROS27", "USPROS17", "", "USPROS19", "USPROS20", 0
	};

	// GLOBAL: TOY2 0x004FD4F0
	char* g_sfxWavList4FD4F0[21] = {
		"USBUZ43", "USBUZ81", "BUZZINF", "USREX21",	 "USREX05",	 "USHAMM05", "USHAMM02", "USHAMM17", "", "", "",
		"",		   "",		  "",		 "USSLIN13", "USSLIN01", "USSLIN17", "",		 "",		 "", 0
	};

	// GLOBAL: TOY2 0x004FD544
	char* g_sfxWavList4FD544[38] = {
		"USBUZ47",	"USBUZ55",	"BUZZINF",	"",			"",			"",			"",			"",		   "",		   "",
		"",			"",			"",			"",			"",			"",			"",			"",		   "",		   "",
		"",			"",			"",			"",			"USPROS03", "USPROS27", "USPROS17", "MaleHit", "USPROS19", "USPROS20",
		"USPROS02", "USPROS05", "USPROS07", "USPROS09", "USPROS10", "USPROS12", "USJESS01", 0
	};

	// the pack has no sounds, the original points into zeroed memory (0x00726F40)
	char* g_sfxWavListEmpty[1] = { 0 };

	// GLOBAL: TOY2 0x004FD140
	SoundEffect g_sfxPack1[17] = {
		{ g_sfxWavList4FCDC4, 1 },	{ g_sfxWavList4FCEE0, 87 }, { g_sfxWavList4FCF0C, 87 }, { g_sfxWavList4FCFA8, 87 }, { g_sfxWavList4FCF54, 87 },
		{ g_sfxWavList4FCF80, 87 }, { g_sfxWavList4FCF38, 87 }, { g_sfxWavList4FCFC0, 87 }, { g_sfxWavList4FCFEC, 87 }, { g_sfxWavList4FD01C, 87 },
		{ g_sfxWavList4FD038, 87 }, { g_sfxWavList4FD064, 87 }, { g_sfxWavList4FD08C, 87 }, { g_sfxWavList4FD0A0, 87 }, { g_sfxWavList4FD0B4, 87 },
		{ g_sfxWavList4FD0CC, 87 }, { g_sfxWavList4FD0E0, 1 }
	};

	// GLOBAL: TOY2 0x004FD5E0
	SoundEffect g_sfxPack2[17] = {
		{ g_sfxWavList4FD1C8, 70 }, { g_sfxWavList4FD1E8, 67 }, { g_sfxWavList4FD23C, 67 }, { g_sfxWavList4FD318, 67 }, { g_sfxWavList4FD288, 67 },
		{ g_sfxWavList4FD2D0, 67 }, { g_sfxWavList4FD278, 67 }, { g_sfxWavList4FD328, 67 }, { g_sfxWavList4FD354, 67 }, { g_sfxWavList4FD378, 67 },
		{ g_sfxWavList4FD388, 67 }, { g_sfxWavList4FD3B4, 67 }, { g_sfxWavList4FD3F0, 67 }, { g_sfxWavList4FD474, 67 }, { g_sfxWavList4FD4F0, 67 },
		{ g_sfxWavList4FD544, 67 }, { g_sfxWavListEmpty, 67 }
	};

	// GLOBAL: TOY2 0x00534484
	int32_t g_quiet;

	// GLOBAL: TOY2 0x00534074
	char g_langPrefix[4];

	// FUNCTION: TOY2 0x00436D40
	int32_t StopAndWait()
	{
		if (g_musicSoundBuffer && g_streamHandles.threadEvent)
		{
			g_setThreadEvent = 1;
			SetEvent(g_streamHandles.threadEvent);
			WaitForSingleObject(g_streamHandles.threadSemaphore, 0xFFFFFFFF);
		}

		return 1;
	}

	// FUNCTION: TOY2 0x00413300
	int32_t IsStreamActive() { return g_audioStreamActive; }

	// FUNCTION: TOY2 0x0047D840
	void StopAndFlush()
	{
		if (g_directSoundInit)
		{
			if (IsStreamActive())
			{
				StopAndWait();

				while (IsStreamActive())
					;
			}

			g_audioShutdownPending = 0;
		}
	}

	// FUNCTION: TOY2 0x0047D8E0
	int32_t IsEffectPlaying(int32_t bufferIndex)
	{
		DWORD status;

		if (! g_directSoundInit)
			return 0;

		g_lastDSoundError = g_soundEffectBuffers[bufferIndex]->GetStatus(&status);

		return status;
	}

	// FUNCTION: TOY2 0x0047E3C0 [MATCHED]
	BOOL CALLBACK Enumerate(LPGUID lpGuid, LPCSTR description, LPCSTR lpcstrModule, LPVOID lpContext)
	{
		strcpy(g_audioDeviceDecs[g_audioDeviceCount], description);

		g_audioDeviceGuids[g_audioDeviceCount + 1] = lpGuid;
		++g_audioDeviceCount;

		return 1;
	}

	// FUNCTION: TOY2 0x0047E420
	int32_t CreateDirectSoundBuffer(IDirectSound* directSound, IDirectSoundBuffer** soundBuffer, DWORD bufferBytes)
	{
		WAVEFORMATEX waveFormat;
		DSBUFFERDESC bufferDesc;

		memset(&waveFormat, 0, sizeof(waveFormat));

		waveFormat.wFormatTag = g_audioConfig->formatTag;
		waveFormat.nChannels = g_audioConfig->channels;
		waveFormat.nSamplesPerSec = g_audioConfig->sampleRate;

		waveFormat.nBlockAlign = g_audioConfig->blockAlign;
		waveFormat.nAvgBytesPerSec = g_audioConfig->avgBytesPerSec;
		waveFormat.wBitsPerSample = g_audioConfig->bitsPerSample;

		bufferDesc.dwFlags = 0;
		bufferDesc.dwBufferBytes = bufferBytes;
		bufferDesc.dwReserved = 0;
		bufferDesc.dwSize = 20;
		bufferDesc.lpwfxFormat = &waveFormat;
		bufferDesc.dwFlags = 32992;

		if (! directSound->CreateSoundBuffer(&bufferDesc, soundBuffer, 0))
			return 1;

		*soundBuffer = 0;

		return 0;
	}

	// FUNCTION: TOY2 0x0047E4E0
	BOOL WriteToBuffer(IDirectSoundBuffer* soundBuffer, DWORD offset, char* sourceData, DWORD dataSize)
	{
		LPVOID audioPtr1;
		DWORD audioBytes1;
		LPVOID audioPtr2;
		DWORD audioBytes2;

		HRESULT lockResult = soundBuffer->Lock(offset, dataSize, &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, 0);

		if (lockResult == DSERR_BUFFERLOST)
		{
			soundBuffer->Restore();
			lockResult = soundBuffer->Lock(offset, dataSize, &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, 0);
		}

		if (lockResult)
			return 0;

		memcpy(audioPtr1, sourceData, audioBytes1);

		// the lock wrapped around the end of the buffer
		if (audioPtr2)
			memcpy(audioPtr2, sourceData + audioBytes1, audioBytes2);

		return ! soundBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2);
	}

	// FUNCTION: TOY2 0x004A4130
	MMRESULT WaveReadFile(HMMIO hmmio, int32_t ckSize, uint8_t* buffer, LPMMCKINFO dataChunk, DWORD* outBytesRead)
	{
		MMIOINFO mmioInfo;
		uint32_t bytesToRead;
		DWORD chunkSize;
		uint32_t totalBytesRead;

		MMRESULT result = mmioGetInfo(hmmio, &mmioInfo, 0);

		if (result)
			goto ERROR_LBL;

		bytesToRead = ckSize;
		chunkSize = dataChunk->cksize;

		if (ckSize > chunkSize)
			bytesToRead = dataChunk->cksize;

		totalBytesRead = 0;
		dataChunk->cksize = chunkSize - bytesToRead;

		if (! bytesToRead)
		{
		FINALISE_LBL:
			result = mmioSetInfo(hmmio, &mmioInfo, 0);

			if (! result)
			{
				*outBytesRead = bytesToRead;
				return result;
			}

		ERROR_LBL:
			*outBytesRead = 0;
			return result;
		}

		HPSTR endRead = mmioInfo.pchEndRead;

		char* next = mmioInfo.pchNext;

		while (1)
		{
			if (next == endRead)
			{
				result = mmioAdvance(hmmio, &mmioInfo, 0);

				if (result)
					goto ERROR_LBL;

				next = mmioInfo.pchNext;
				endRead = mmioInfo.pchEndRead;

				if (mmioInfo.pchNext == mmioInfo.pchEndRead)
					break;
			}

			uint8_t byteValue = *next++;
			buffer[totalBytesRead++] = byteValue;
			mmioInfo.pchNext = next;

			if (totalBytesRead >= bytesToRead)
				goto FINALISE_LBL;
		}

		result = 57603;
		*outBytesRead = 0;

		return result;
	}

	namespace Wave
	{
		// FUNCTION: TOY2 0x004A3F10
		MMRESULT OpenFile(LPSTR fileName, HMMIO* outHmmio, LPWAVEFORMATEX* outWaveFormat, LPMMCKINFO outRiffChunk)
		{
			MMRESULT result;
			PCMWAVEFORMAT waveFormatHeader;
			MMCKINFO fmtChunkInfo;
			WORD extraFormatBytes;

			*outWaveFormat = 0;

			HMMIO hmmio = 0;
			FILE* fileHandle = fopen(fileName, "rb");

			if (! fileHandle || (fclose(fileHandle), hmmio = mmioOpenA(fileName, 0, MMIO_ALLOCBUF | MMIO_READ), hmmio == 0))
			{
				result = 57600;
				goto LABEL_24;
			}

			result = mmioDescend(hmmio, outRiffChunk, 0, 0);

			if (! result)
			{
				if (outRiffChunk->ckid != mmioFOURCC('R', 'I', 'F', 'F') || outRiffChunk->fccType != mmioFOURCC('W', 'A', 'V', 'E'))
					goto INVALID_FORMAT;

				fmtChunkInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
				result = mmioDescend(hmmio, &fmtChunkInfo, outRiffChunk, MMIO_FINDCHUNK);

				if (! result)
				{
					if (fmtChunkInfo.cksize < 16)
					{
					INVALID_FORMAT:
						result = 57601;
						goto LABEL_24;
					}

					if (mmioRead(hmmio, (HPSTR)&waveFormatHeader, 16) == 16)
					{
						if (waveFormatHeader.wf.wFormatTag == WAVE_FORMAT_PCM)
						{
							extraFormatBytes = 0;
						}
						else
						{
							if (mmioRead(hmmio, (HPSTR)&extraFormatBytes, 2) != 2)
							{
								result = 57602;
								goto LABEL_24;
							}
						}

						LPWAVEFORMATEX allocatedFormat = (LPWAVEFORMATEX)GlobalAlloc(0, extraFormatBytes + 18);
						*outWaveFormat = allocatedFormat;

						if (allocatedFormat)
						{
							memcpy(allocatedFormat, &waveFormatHeader, 16);
							allocatedFormat->cbSize = extraFormatBytes;

							if (! extraFormatBytes || mmioRead(hmmio, (HPSTR)((uint8_t*)*outWaveFormat + 18), extraFormatBytes) == extraFormatBytes)
							{
								result = mmioAscend(hmmio, &fmtChunkInfo, 0);

								if (! result)
								{
									*outHmmio = hmmio;
									return 0;
								}
							}
							else
							{
								result = 0xE101;
							}
						}
						else
						{
							result = 57344;
						}
					}
					else
					{
						result = 57602;
					}
				}
			}

		LABEL_24:
			if (*outWaveFormat)
			{
				GlobalFree(*outWaveFormat);
				*outWaveFormat = 0;
			}

			if (hmmio)
			{
				mmioClose(hmmio, 0);
				*outHmmio = 0;
				return result;
			}
			else
			{
				*outHmmio = 0;
			}

			return result;
		}

	}

	// FUNCTION: TOY2 0x004A4680
	MMRESULT WaveLoadFile(LPSTR fileName, int32_t* dataSize, int32_t* unused, AudioConfig** audioConfig, HGLOBAL* waveData)
	{
		HMMIO wavSourceData;
		MMCKINFO mmckInfo;
		MMCKINFO parentChunk;

		*waveData = 0;
		*audioConfig = 0;
		*dataSize = 0;

		MMRESULT result = Wave::OpenFile(fileName, &wavSourceData, (LPWAVEFORMATEX*)audioConfig, &parentChunk);

		if (result)
		{
			Logger::DebugLog("Unable to open %s\r\n", fileName);
		}
		else
		{
			mmioSeek(wavSourceData, parentChunk.dwDataOffset + 4, 0);

			mmckInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
			result = mmioDescend(wavSourceData, &mmckInfo, &parentChunk, MMIO_FINDCHUNK);

			if (! result)
			{
				uint8_t* allocatedData = (uint8_t*)GlobalAlloc(0, mmckInfo.cksize);
				*waveData = allocatedData;

				if (allocatedData)
				{
					DWORD bytesRead;

					result = WaveReadFile(wavSourceData, mmckInfo.cksize, allocatedData, &mmckInfo, &bytesRead);

					if (! result)
					{
						*dataSize = bytesRead;
						goto LABEL_12;
					}
				}
				else
				{
					result = 57344;
				}
			}
		}

		if (*waveData)
		{
			GlobalFree(*waveData);
			*waveData = 0;
		}

		if (*audioConfig)
		{
			GlobalFree(*audioConfig);
			*audioConfig = 0;
		}

	LABEL_12:
		if (wavSourceData)
			mmioClose(wavSourceData, 0);

		return result;
	}

	// FUNCTION: TOY2 0x0047E5B0
	void LoadSoundEffect(const char* name, int32_t effect, int32_t addLang)
	{
		char filePath[1024];

		if (g_reloadEffectIndex == -1 || effect == g_reloadEffectIndex)
		{
			FileUtils::GetPathValue(filePath);
			strcat(filePath, "sfx\\");

			if (addLang)
				strcat(filePath, g_langPrefix);

			strcat(filePath, name);
			Logger::Log("LoadSoundEffect : Loading %s, effect %d.\n", filePath, effect, 1);

			if (WaveLoadFile(filePath, &g_wavDataSize, &g_unused, &g_audioConfig, (HGLOBAL*)&g_wavSourceData))
			{
				Logger::Log("LoadSoundEffect : Calling WaveLoadFile failed - no sound effect %s, effect %d.\n", filePath, effect);
			}
			else
			{
				int32_t bufferBaseIndex = 6 * effect;
				LPDIRECTSOUNDBUFFER* primaryBuffer = &g_soundEffectBuffers[6 * effect];

				if (CreateDirectSoundBuffer(g_directSound, primaryBuffer, g_wavDataSize) == 1)
				{
					WriteToBuffer(*primaryBuffer, 0, (char*)g_wavSourceData, g_wavDataSize);

					WORD sampleRate = g_audioConfig->sampleRate;

					LPDIRECTSOUNDBUFFER buffer = *primaryBuffer;
					g_soundEffectSampleRates[effect] = sampleRate;

					buffer->SetFrequency(sampleRate);
					(*primaryBuffer)->SetPan(0);
					(*primaryBuffer)->SetVolume(0);

					g_soundBufferStates[6 * effect] = (void*)1;
					int32_t duplicatesRemaining = 5;

					do
					{
						LPDIRECTSOUNDBUFFER* duplicateBuffer = &g_soundEffectBuffers[bufferBaseIndex + 1];

						g_lastDSoundError = g_directSound->DuplicateSoundBuffer(*primaryBuffer, &g_soundEffectBuffers[bufferBaseIndex + 1]);

						if (! g_lastDSoundError)
						{
							(*duplicateBuffer)->SetFrequency(g_soundEffectSampleRates[effect]);
							(*duplicateBuffer)->SetPan(0);
							(*duplicateBuffer)->SetVolume(0);

							g_soundBufferStates[bufferBaseIndex + 1] = (void*)1;
						}

						++bufferBaseIndex;
						--duplicatesRemaining;
					} while (duplicatesRemaining);
				}
			}
		}
	}

	// FUNCTION: TOY2 0x004A3BC0
	void ResetChannelsTable()
	{
		AudioChannel* channel = g_audioChannels;

		do
		{
			channel->bufferIndex = -1;
			++channel;
		} while (channel < &g_audioChannels[32]);
	}

	// FUNCTION: TOY2 0x00436C90
	void SignalThreadExit()
	{
		if (g_streamHandles.threadEvent)
		{
			g_setThreadEvent = 3;
			g_streamThreadReady = 0;
			SetEvent(g_streamHandles.threadEvent);
		}
	}

	// FUNCTION: TOY2 0x00413140
	void OnExit() { SignalThreadExit(); }

	namespace Stream
	{
		// FUNCTION: TOY2 0x00436CC0
		int32_t IsThreadReady() { return g_streamThreadReady; }

	}

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

	namespace Wave
	{
		// FUNCTION: TOY2 0x004A40F0
		MMRESULT SeekToChunk(HMMIO* musicHandle, LPMMCKINFO info, MMCKINFO* infoParent)
		{
			mmioSeek(*musicHandle, infoParent->dwDataOffset + 4, 0);
			info->ckid = mmioFOURCC('d', 'a', 't', 'a');
			return mmioDescend(*musicHandle, info, infoParent, MMIO_FINDCHUNK);
		}

		// FUNCTION: TOY2 0x004A4200
		int32_t CloseFile(HMMIO* hMmio, HGLOBAL* hGlobal)
		{
			if (*hGlobal)
			{
				GlobalFree(*hGlobal);
				*hGlobal = 0;
			}

			if (*hMmio)
			{
				mmioClose(*hMmio, 0);
				*hMmio = 0;
			}

			return 0;
		}

	}

	// FUNCTION: TOY2 0x00413310
	int32_t LoadFile(LPSTR fileName)
	{
		DSBUFFERDESC bufferDesc;
		DWORD bufferBytes;
		DWORD blockAlign;
		LPVOID audioPtr1;
		DWORD audioBytes1;
		LPVOID audioPtr2;
		DWORD audioBytes2;
		DWORD bytesRead;

		if (g_quiet || Wave::OpenFile(fileName, &g_musicFileHandle, &g_musicWaveFormat, &g_musicRiffChunk))
			return 0;

		if (Wave::SeekToChunk(&g_musicFileHandle, &g_musicDataChunk, &g_musicRiffChunk))
			goto LBL_CLOSE_FILE;

		bufferBytes = (3 * g_musicWaveFormat->nBlockAlign * g_musicWaveFormat->nSamplesPerSec) >> 4;
		g_chunkSize = bufferBytes;

		blockAlign = g_musicWaveFormat->nBlockAlign;

		if (bufferBytes % blockAlign)
		{
			bufferBytes += blockAlign - bufferBytes % blockAlign;
			g_chunkSize = bufferBytes;
		}

		bufferDesc.dwSize = 20;
		bufferDesc.dwFlags = 98784;
		bufferDesc.dwReserved = 0;

		g_totalBufferSize = 16 * bufferBytes;

		bufferDesc.dwBufferBytes = 16 * bufferBytes;
		bufferDesc.lpwfxFormat = g_musicWaveFormat;

		if (g_directSound->CreateSoundBuffer(&bufferDesc, &g_musicSoundBuffer, 0))
		{
		LBL_CLOSE_FILE:
			Wave::CloseFile(&g_musicFileHandle, (HGLOBAL*)&g_musicWaveFormat);
			return 0;
		}

		g_eofReached = 0;

		g_musicSoundBufferCopy = g_musicSoundBuffer;

		g_writeOffset = 0;
		g_looping = 0;

		if (g_musicSoundBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&g_directSoundNotify))
		{
			g_musicSoundBuffer->Release();
			goto LBL_CLOSE_FILE;
		}

		DWORD chunkSize = g_chunkSize;
		HANDLE notifyEvent = g_streamHandles.bufferNotifyEvent;

		g_positionNotifiers[0].dwOffset = g_chunkSize;
		g_positionNotifiers[0].hEventNotify = g_streamHandles.bufferNotifyEvent;

		DSBPOSITIONNOTIFY* value = &g_positionNotifiers[1];

		do
		{
			DWORD offset = value[-1].dwOffset;

			++value;

			value[-1].dwOffset = chunkSize + offset;
			value[-1].hEventNotify = notifyEvent;
		} while (value < &g_positionNotifiers[16]);

		--g_positionNotifiers[15].dwOffset;

		g_positionNotifiers[16].dwOffset = -1;
		g_positionNotifiers[16].hEventNotify = g_streamHandles.streamStopEvent;

		g_directSoundNotify->SetNotificationPositions(17, g_positionNotifiers);

		if (! g_musicSoundBuffer->Lock(0, g_totalBufferSize, &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, 0))
		{
			WaveReadFile(g_musicFileHandle, audioBytes1, (uint8_t*)audioPtr1, &g_musicDataChunk, &bytesRead);

			DWORD totalRead = bytesRead;
			DWORD lockedBytes = audioBytes1;

			if (bytesRead < audioBytes1)
			{
				if (g_looping)
				{
					do
					{
						Wave::SeekToChunk(&g_musicFileHandle, &g_musicDataChunk, &g_musicRiffChunk);
						WaveReadFile(g_musicFileHandle, audioBytes1 - totalRead, (uint8_t*)audioPtr1 + totalRead, &g_musicDataChunk, &bytesRead);

						totalRead += bytesRead;
						lockedBytes = audioBytes1;
					} while (totalRead < audioBytes1);
				}
				else
				{
					if (bytesRead < g_chunkSize)
						g_eofReached = 1;

					// the rest of the buffer is silence
					memset((uint8_t*)audioPtr1 + bytesRead, g_musicWaveFormat->wBitsPerSample != 8 ? 0 : 128, audioBytes1 - bytesRead);

					lockedBytes = audioBytes1;
				}
			}

			g_musicSoundBuffer->Unlock(audioPtr1, lockedBytes, 0, 0);
			DWORD nextOffset = g_writeOffset + audioBytes1;
			g_writeOffset += audioBytes1;

			if (g_writeOffset >= g_totalBufferSize)
				g_writeOffset = nextOffset - g_totalBufferSize;

			g_totalBytesPlayed = 0;
			g_lastPlayCursor = 0;
		}

		g_streamFinished = 0;

		return 1;
	}

	// FUNCTION: TOY2 0x0049AE40
	void SetMusicVolume(int32_t volume)
	{
		g_musicVolume = volume;
		LONG dsVolume = g_dSoundVolumeConversion[150 * volume / 64];

		Logger::Log("SETMUSICVOL : Vol %d \n", dsVolume);

		if (g_musicSoundBuffer)
		{
			HRESULT result = g_musicSoundBuffer->SetVolume(dsVolume);

			if (result)
			{
				const char* error;

				switch (result)
				{
					case DSERR_PRIOLEVELNEEDED:
						error = "The caller does not have the priority level required for the function to succeed. ";
						break;
					case DSERR_BADFORMAT:
						error = "The specified wave format is not supported. ";
						break;
					case DSERR_NODRIVER:
						error = "No sound driver is available for use. ";
						break;
					case DSERR_ALREADYINITIALIZED:
						error = "The object is already initialized. ";
						break;
					case DSERR_BUFFERLOST:
						error = "The buffer memory has been lost and must be restored. ";
						break;
					case DSERR_OTHERAPPHASPRIO:
						error = "Another application has a higher priority level, preventing this call from succeeding ";
						break;
					case DSERR_UNINITIALIZED:
						error =
							"The IDirectSound::Initialize method has not been called or has not been called successfully before other methods were called. ";
						break;
					case DSERR_INVALIDCALL:
						error = "This function is not valid for the current state of this object. ";
						break;
					case DSERR_INVALIDPARAM:
						error = "An invalid parameter was passed to the returning function. ";
						break;
					case DSERR_ALLOCATED:
						error = "The request failed because resources, such as a priority level, were already in use by another caller. ";
						break;
					case DSERR_CONTROLUNAVAIL:
						error = "The control (volume, pan, and so forth) requested by the caller is not available. ";
						break;
					case DSERR_OUTOFMEMORY:
						error = "The DirectSound subsystem could not allocate sufficient memory to complete the caller's request. ";
						break;
					case DSERR_NOAGGREGATION:
						error = "The object does not support aggregation. ";
						break;
					case DSERR_GENERIC:
						error = "An undetermined error occurred inside the DirectSound subsystem. ";
						break;
					case DSERR_UNSUPPORTED:
						error = "The function called is not supported at this time. ";
						break;
					case DSERR_NOINTERFACE:
						error = "The requested COM interface is not available. ";
						break;
					default:
						error = "Unknown error!!";
						break;
				}

				Logger::Log("SETMUSICVOL : Failed to set volume - error is %s.\n", error);
			}
		}
	}

	// FUNCTION: TOY2 0x00413240 [MATCHED]
	void ThreadPlay(LPSTR fileName, int32_t looping)
	{
		if (g_directSound)
		{
			if (LoadFile(fileName))
			{
				g_looping = looping & 1;

				g_musicSoundBuffer->SetCurrentPosition(0);
				SetMusicVolume(g_musicVolume);

				g_musicSoundBuffer->Play(0, 0, 1);
			}
		}
	}

	// FUNCTION: TOY2 0x00436D80
	void FillBuffer()
	{
		LPVOID audioPtr1;
		DWORD audioBytes1;
		DWORD bytesOut = 0;
		DWORD curPlayCursor;
		DWORD curWriteCursor;

		if (g_directSound && g_musicSoundBuffer)
		{
			if (! g_musicSoundBuffer->GetCurrentPosition(&curPlayCursor, &curWriteCursor))
			{
				DWORD bytesPlayed;

				if (curPlayCursor >= g_lastPlayCursor)
					bytesPlayed = curPlayCursor - g_lastPlayCursor;
				else
					bytesPlayed = curPlayCursor + g_totalBufferSize - g_lastPlayCursor;

				g_lastPlayCursor = curPlayCursor;
				g_totalBytesPlayed += bytesPlayed;
			}

			if (g_eofReached)
			{
				if (! g_musicSoundBuffer->Lock(g_writeOffset, g_chunkSize, &audioPtr1, &audioBytes1, 0, 0, 0))
				{
					// the end of the file was reached, keep writing silence
					memset(audioPtr1, g_musicWaveFormat->wBitsPerSample != 8 ? 0 : 0x80, audioBytes1);

					g_musicSoundBuffer->Unlock(audioPtr1, audioBytes1, 0, 0);
				}

				if (g_musicRiffChunk.cksize > g_chunkSize && g_totalBytesPlayed >= g_musicRiffChunk.cksize - g_chunkSize
					|| g_totalBytesPlayed >= g_musicRiffChunk.cksize)
				{
					g_streamFinished = 1;
				}
			}
			else if (! g_musicSoundBuffer->Lock(g_writeOffset, g_chunkSize, &audioPtr1, &audioBytes1, 0, 0, 0))
			{
				WaveReadFile(g_musicFileHandle, audioBytes1, (uint8_t*)audioPtr1, &g_musicDataChunk, &bytesOut);
				DWORD totalRead = bytesOut;

				if (bytesOut < audioBytes1)
				{
					if (g_looping)
					{
						do
						{
							if (Wave::SeekToChunk(&g_musicFileHandle, &g_musicDataChunk, &g_musicRiffChunk))
								break;

							if (WaveReadFile(g_musicFileHandle, audioBytes1 - totalRead, (uint8_t*)audioPtr1 + totalRead, &g_musicDataChunk, &bytesOut))
								break;

							totalRead += bytesOut;
						} while (totalRead < audioBytes1);
					}
					else
					{
						g_eofReached = 1;

						memset((uint8_t*)audioPtr1 + bytesOut, g_musicWaveFormat->wBitsPerSample != 8 ? 0 : 0x80, audioBytes1 - bytesOut);
					}
				}

				g_musicSoundBuffer->Unlock(audioPtr1, audioBytes1, 0, 0);
				DWORD nextOffset = audioBytes1 + g_writeOffset;
				g_writeOffset += audioBytes1;

				if (g_writeOffset >= g_totalBufferSize)
					g_writeOffset = nextOffset - g_totalBufferSize;
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

					Wave::CloseFile(&g_musicFileHandle, (HGLOBAL*)&g_musicWaveFormat);

					g_directSoundNotify->Release();
					g_directSoundNotify = 0;

					g_musicSoundBuffer->Release();
					g_musicSoundBuffer = 0;
				}
			}
		}

		// FUNCTION: TOY2 0x00437010
		DWORD WINAPI ThreadProc(LPVOID context)
		{
			g_streamThreadReady = 1;

			do
			{
				while (g_streamHandles.bufferNotifyEvent)
				{
					DWORD wait = WaitForMultipleObjects(3, &g_streamHandles.bufferNotifyEvent, 0, 0xFFFFFFFF);

					if (wait == -1)
						break;

					if (wait)
					{
						DWORD waitMinus1 = wait - 1;

						if (waitMinus1)
						{
							if (waitMinus1 == 1)
							{
								switch (g_setThreadEvent)
								{
									case 1:
										Stop();
										ResetEvent(g_streamHandles.streamStopEvent);
										break;
									case 2:
										ThreadPlay(g_fileNameBuffer, g_pendingLoopFlag);
										break;
									case 3:
										ShutdownHandles();
										break;
								}

								g_setThreadEvent = 0;
								ReleaseSemaphore(g_streamHandles.threadSemaphore, 1, 0);
							}
						}
						else
						{
							Stop();
						}
					}
					else if (g_streamFinished == 1)
					{
						Stop();
						ResetEvent(g_streamHandles.streamStopEvent);
					}
					else
					{
						FillBuffer();
					}
				}
			} while (g_streamThreadReady);

			return 1;
		}

		// FUNCTION: TOY2 0x00412FF0
		int32_t Init()
		{
			DWORD threadId;

			if (g_audioStreamInitialized)
				return 0;

			InitializeCriticalSection(&g_audioCriticalSection);
			g_streamHandles.bufferNotifyEvent = CreateEventA(0, 0, 0, 0);

			if (! g_streamHandles.bufferNotifyEvent)
				return 0;

			g_streamHandles.streamStopEvent = CreateEventA(0, 0, 0, 0);

			if (! g_streamHandles.streamStopEvent)
			{
			LBL_BUFFER_NOTIFY:
				CloseHandle(g_streamHandles.bufferNotifyEvent);
				g_streamHandles.bufferNotifyEvent = 0;
				return 0;
			}

			g_streamHandles.threadEvent = CreateEventA(0, 0, 0, 0);

			if (! g_streamHandles.threadEvent)
			{
			LBL_STREAM_STOP:
				CloseHandle(g_streamHandles.streamStopEvent);
				g_streamHandles.streamStopEvent = 0;
				goto LBL_BUFFER_NOTIFY;
			}

			g_streamHandles.threadSemaphore = CreateSemaphoreA(0, 0, 1, 0);

			if (! g_streamHandles.threadSemaphore)
			{
				CloseHandle(g_streamHandles.threadEvent);
				g_streamHandles.threadEvent = 0;
				goto LBL_STREAM_STOP;
			}

			if (! CreateThread(0, 0, ThreadProc, 0, 0, &threadId))
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
				return 0;
			}

			while (! IsThreadReady())
				;

			g_audioStreamInitialized = 1;
			atexit(OnExit);

			return 1;
		}

	}

	// FUNCTION: TOY2 0x0047EDE0
	void Init()
	{
		char sfxNameBuffer[256];
		char context;

		ResetChannelsTable();

		memset(g_soundEffectBuffers, 0, sizeof(g_soundEffectBuffers));
		memset(g_soundBufferStates, 0, sizeof(g_soundBufferStates));

		g_unk726230 = 0;
		g_audioDeviceCount = 0;
		g_directSoundInit = 0;

		if (! g_quiet)
		{
			DirectSoundEnumerateA(Enumerate, &context);
			DirectSoundCreate(g_audioDeviceGuids[1], &g_directSound, 0);

			if (g_directSound)
			{
				g_directSound->SetCooperativeLevel(D3DApp::g_windowData.mainHwnd, DSSCL_PRIORITY);
				g_directSoundInit = 1;

				if (IsStreamActive())
				{
					StopAndWait();

					while (IsStreamActive())
						;
				}

				g_audioShutdownPending = 0;

				if (g_directSoundInit)
				{
					int32_t index = 767;

					for (int32_t i = 0; i >= -767; --i)
					{
						if (g_soundEffectBuffers[i + 767])
						{
							if ((IsEffectPlaying(index) & 1) == 1)
								g_soundEffectBuffers[i + 767]->Stop();

							g_soundEffectBuffers[i + 767]->Release();
							g_soundEffectBuffers[i + 767] = 0;

							g_soundBufferStates[i + 767] = 0;
						}

						--index;
					}

					memset(g_soundEffectBuffers, 0, sizeof(g_soundEffectBuffers));
					memset(g_soundBufferStates, 0, sizeof(g_soundBufferStates));

					for (int32_t channel = 0; channel < 32; channel++)
					{
						g_audioChannels[channel].bufferIndex = -1;
						g_audioChannels[channel].timer = -1;
					}
				}

				char** wavList = g_sfxPack1[0].wavList;
				int32_t effectId = g_sfxPack1[0].effectId;

				if (*g_sfxPack1[0].wavList)
				{
					char* nextEffect;

					do
					{
						if (**wavList)
						{
							sprintf(sfxNameBuffer, "%s.wav", *wavList);
							LoadSoundEffect(sfxNameBuffer, effectId, 0);
						}

						nextEffect = wavList[1];
						++wavList;
						++effectId;
					} while (nextEffect);
				}

				g_curSfxPack = 0;
				Stream::Init();
			}
		}
	}

	// FUNCTION: TOY2 0x0049AE20 [MATCHED]
	void SetSfxVolume(int32_t volume) { g_sfxVolume = (volume & 0xFF) << 1; }

	// FUNCTION: TOY2 0x004A3ED0
	void SetVolumes(int32_t musicVolume, int32_t soundVolume)
	{
		SetMusicVolume(musicVolume);
		SetSfxVolume(soundVolume);
	}

	// FUNCTION: TOY2 0x0049E8D0
	void SetVolumesProcessed(int32_t musicVolume, int32_t sfxVolume)
	{
		SetVolumes(2 * Toy2::g_musicSliderToAmplitude[musicVolume] / 3, 3 * Toy2::g_soundSliderToAmplitude[sfxVolume] / 2);
	}

	// FUNCTION: TOY2 0x004A3EF0
	void FlushSoundVoices()
	{
		Logger::Log("FlushSoundVoices : Start.\n");
		Logger::Log("FlushSoundVoices : End.\n");
	}

	// FUNCTION: TOY2 0x0047EC20
	void LoadSfxPackForLevel(int32_t levelId)
	{
		char buffer[256];

		if (g_directSoundInit)
		{
			if (IsStreamActive())
			{
				StopAndWait();

				while (IsStreamActive())
					;
			}

			g_audioShutdownPending = 0;

			if (g_directSoundInit)
			{
				int32_t index = 767;

				for (int32_t i = 0; i >= -767; --i)
				{
					if (g_soundEffectBuffers[i + 767])
					{
						if ((IsEffectPlaying(index) & 1) == 1)
							g_soundEffectBuffers[i + 767]->Stop();

						g_soundEffectBuffers[i + 767]->Release();
						g_soundEffectBuffers[i + 767] = 0;
						g_soundBufferStates[i + 767] = 0;
					}

					--index;
				}

				memset(g_soundEffectBuffers, 0, sizeof(g_soundEffectBuffers));
				memset(g_soundBufferStates, 0, sizeof(g_soundBufferStates));

				for (int32_t channel = 0; channel < 32; channel++)
				{
					g_audioChannels[channel].bufferIndex = -1;
					g_audioChannels[channel].timer = -1;
				}
			}
		}

		char** wavList = g_sfxPack1[0].wavList;
		int32_t effectId = g_sfxPack1[0].effectId;

		if (*g_sfxPack1[0].wavList)
		{
			char* nextEffect;

			do
			{
				if (**wavList)
				{
					sprintf(buffer, "%s.wav", *wavList);
					LoadSoundEffect(buffer, effectId, 0);
				}

				nextEffect = wavList[1];
				++wavList;
				++effectId;
			} while (nextEffect);
		}

		if (levelId > 0 && levelId <= 16)
		{
			char** levelWavList = g_sfxPack1[levelId].wavList;
			int32_t levelEffectId = g_sfxPack1[levelId].effectId;

			for (const char* wavName = *levelWavList; wavName; ++levelEffectId)
			{
				if (*wavName)
				{
					sprintf(buffer, "%s.wav", wavName);
					LoadSoundEffect(buffer, levelEffectId, 0);
				}

				wavName = levelWavList[1];
				++levelWavList;
			}

			char** levelWavList2 = g_sfxPack2[levelId].wavList;
			int32_t levelEffectId2 = g_sfxPack2[levelId].effectId;

			for (const char* wavName2 = *levelWavList2; wavName2; ++levelEffectId2)
			{
				if (*wavName2)
				{
					sprintf(buffer, "%s.wav", wavName2);
					LoadSoundEffect(buffer, levelEffectId2, 0);
				}

				wavName2 = levelWavList2[1];
				++levelWavList2;
			}
		}

		g_curSfxPack = levelId;
	}

	// FUNCTION: TOY2 0x0047DE50
	int32_t PlaySoundBuffer(int32_t soundIndex, int32_t leftVolume, int32_t rightVolume, void* owner, int32_t volume, int32_t loop)
	{
		DWORD status;
		char buffer[256];

		if (! g_directSoundInit)
			return -1;

		int32_t bufferIndex = 6 * soundIndex;

		if (! g_soundBufferStates[6 * soundIndex])
			return -1;

		g_lastDSoundError = g_soundEffectBuffers[6 * soundIndex]->GetStatus(&status);

		// the buffer memory was lost, load the sounds of the level again
		if ((status & DSBSTATUS_BUFFERLOST) != 0)
		{
			int32_t first = bufferIndex & 0xFFFF8;
			int32_t last = (bufferIndex & 0xFFFF8) + 8;

			if (first < last)
			{
				do
				{
					int32_t directSoundInit = g_directSoundInit;
					g_soundBufferStates[first] = 0;

					if (directSoundInit)
						g_soundEffectBuffers[first]->Release();

					++first;
				} while (first < last);
			}

			int32_t curPack = g_curSfxPack;
			g_reloadEffectIndex = 6 * soundIndex / 8;

			if (g_directSoundInit)
			{
				if (IsStreamActive())
				{
					StopAndWait();

					while (IsStreamActive())
						;
				}

				g_audioShutdownPending = 0;

				if (g_directSoundInit)
				{
					for (int32_t i = 0; i >= -767; --i)
					{
						LPDIRECTSOUNDBUFFER soundBuffer = g_soundEffectBuffers[i + 767];

						if (soundBuffer)
						{
							char statusByte;

							if (g_directSoundInit)
							{
								g_lastDSoundError = soundBuffer->GetStatus(&status);
								statusByte = status;
							}
							else
							{
								statusByte = 0;
							}

							if ((statusByte & DSBSTATUS_PLAYING) == 1)
								g_soundEffectBuffers[i + 767]->Stop();

							g_soundEffectBuffers[i + 767]->Release();
							g_soundEffectBuffers[i + 767] = 0;
							g_soundBufferStates[i + 767] = 0;
						}
					}

					memset(g_soundEffectBuffers, 0, sizeof(g_soundEffectBuffers));
					memset(g_soundBufferStates, 0, sizeof(g_soundBufferStates));

					for (int32_t channel = 0; channel < 32; channel++)
					{
						g_audioChannels[channel].bufferIndex = -1;
						g_audioChannels[channel].timer = -1;
					}
				}
			}

			char** wavList = g_sfxPack1[0].wavList;
			int32_t effectId = g_sfxPack1[0].effectId;

			if (*g_sfxPack1[0].wavList)
			{
				char* nextEffect;

				do
				{
					if (**wavList)
					{
						sprintf(buffer, "%s.wav", *wavList);
						LoadSoundEffect(buffer, effectId, 0);
					}

					nextEffect = wavList[1];
					++wavList;
					++effectId;
				} while (nextEffect);
			}

			if (curPack > 0 && curPack <= 16)
			{
				char** packWavList = g_sfxPack1[curPack].wavList;
				int32_t packEffectId = g_sfxPack1[curPack].effectId;

				for (const char* wavName = *packWavList; wavName; ++packEffectId)
				{
					if (*wavName)
					{
						sprintf(buffer, "%s.wav", wavName);
						LoadSoundEffect(buffer, packEffectId, 0);
					}

					wavName = packWavList[1];
					++packWavList;
				}

				char** pack2WavList = g_sfxPack2[curPack].wavList;
				int32_t pack2EffectId = g_sfxPack2[curPack].effectId;

				for (const char* wavName2 = *pack2WavList; wavName2; ++pack2EffectId)
				{
					if (*wavName2)
					{
						sprintf(buffer, "%s.wav", wavName2);
						LoadSoundEffect(buffer, pack2EffectId, 0);
					}

					wavName2 = pack2WavList[1];
					++pack2WavList;
				}
			}

			g_curSfxPack = curPack;
			g_reloadEffectIndex = -1;
		}

		int32_t rightChannel = rightVolume;
		DWORD highestCursor = 0;
		int32_t bestBuffer = -1;
		DWORD sampleRate = g_soundEffectSampleRates[soundIndex];
		LONG pan = 10000 * (rightVolume - leftVolume) / 128;

		if (pan <= 10000)
		{
			if (pan < -10000)
				pan = -10000;
		}
		else
		{
			pan = 10000;
		}

		if (rightVolume < leftVolume)
			rightChannel = leftVolume;

		LONG dsVolume = g_dSoundVolumeConversion[rightChannel * g_sfxVolume / 256];

		if (owner)
		{
			int32_t voice = 0;
			void** state = &g_soundBufferStates[bufferIndex];

			while (*state != owner)
			{
				++voice;

				++state;

				if (voice >= 6)
					goto LABEL_57;
			}

			int32_t found = voice + bufferIndex;
			g_lastDSoundError = (&g_soundEffectBuffers[voice])[bufferIndex]->GetCurrentPosition(&g_playCursor, &g_writeCursor);

			if (! g_lastDSoundError)
			{
				g_soundEffectBuffers[found]->SetFrequency(sampleRate);
				g_soundEffectBuffers[found]->SetPan(pan);
				g_soundEffectBuffers[found]->SetVolume(dsVolume);

				if (! g_playCursor)
				{
					LPDIRECTSOUNDBUFFER soundBuffer = g_soundEffectBuffers[found];

					if (loop)
					{
						g_lastDSoundError = soundBuffer->Play(0, 0, 1);
						return found;
					}

					g_lastDSoundError = soundBuffer->Play(0, 0, 0);
				}
			}

			return found;
		}
		else
		{
			owner = (void*)1;

		LABEL_57:
			int32_t voiceCount = 0;

			do
			{
				int32_t current = bufferIndex;

				if (g_soundBufferStates[bufferIndex])
				{
					g_lastDSoundError = g_soundEffectBuffers[bufferIndex]->GetCurrentPosition(&g_playCursor, &g_writeCursor);

					if (! g_lastDSoundError)
					{
						if (! g_playCursor)
						{
							g_soundEffectBuffers[current]->SetFrequency(sampleRate);
							g_soundEffectBuffers[current]->SetPan(pan);
							g_soundEffectBuffers[current]->SetVolume(dsVolume);
							g_soundEffectBuffers[current]->SetCurrentPosition(0);

							LPDIRECTSOUNDBUFFER soundBuffer = g_soundEffectBuffers[current];
							HRESULT playResult;

							if (loop)
								playResult = soundBuffer->Play(0, 0, 1);
							else
								playResult = soundBuffer->Play(0, 0, 0);

							g_lastDSoundError = playResult;
							g_soundBufferStates[current] = owner;

							return current;
						}

						// remember the buffer that is the furthest into its sound
						if (highestCursor < g_playCursor)
						{
							highestCursor = g_playCursor;
							bestBuffer = current;
						}
					}
				}

				bool keepGoing = ++voiceCount < 6;
				bufferIndex = current + 1;

				if (! keepGoing)
					break;
			} while (1);

			if (bestBuffer == -1)
				return -1;

			g_soundEffectBuffers[bestBuffer]->SetFrequency(sampleRate);
			g_soundEffectBuffers[bestBuffer]->SetPan(pan);
			g_soundEffectBuffers[bestBuffer]->SetVolume(dsVolume);
			g_soundEffectBuffers[bestBuffer]->SetCurrentPosition(0);

			LPDIRECTSOUNDBUFFER soundBuffer = g_soundEffectBuffers[bestBuffer];
			HRESULT playResult;

			if (loop)
				playResult = soundBuffer->Play(0, 0, 1);
			else
				playResult = soundBuffer->Play(0, 0, 0);

			g_lastDSoundError = playResult;
			g_soundBufferStates[bestBuffer] = owner;

			return bestBuffer;
		}
	}

	// FUNCTION: TOY2 0x004A37E0
	int32_t PlayOneShotSoundGlobal(int32_t soundIndex, int32_t volume, int32_t leftVolume, int32_t rightVolume)
	{
		return PlaySoundBuffer(soundIndex + 1, leftVolume, rightVolume, 0, volume, 0);
	}

	// FUNCTION: TOY2 0x00436CE0
	void QueuePlay(const char* fileName, int32_t loopFlag)
	{
		if (g_streamHandles.threadEvent)
		{
			HANDLE threadEvent = g_streamHandles.threadEvent;
			strcpy(g_fileNameBuffer, fileName);

			g_setThreadEvent = 2;

			g_pendingLoopFlag = loopFlag;
			SetEvent(threadEvent);
			WaitForSingleObject(g_streamHandles.threadSemaphore, 0xFFFFFFFF);
		}
	}

	// FUNCTION: TOY2 0x00413150
	int32_t PlayTrackByIndex(int32_t index, int32_t loop)
	{
		char fileName[512];

		StopAndWait();

		if (index > 21)
			return 0;

		FileUtils::AppendCDPath(fileName);

		strcat(fileName, "audio\\");
		strcat(fileName, g_gameMusicNames[index]);
		strcat(fileName, ".wav");

		QueuePlay(fileName, loop);
		g_curTrackIndex = index;

		return 1;
	}

	// FUNCTION: TOY2 0x0047D7F0
	void PlayMusicOneShot(int32_t trackIndex)
	{
		if (g_directSoundInit)
		{
			if (IsStreamActive())
			{
				StopAndWait();

				while (IsStreamActive())
					;
			}

			g_audioShutdownPending = 0;
			PlayTrackByIndex(trackIndex, 0);
		}
	}

	// FUNCTION: TOY2 0x0047D880
	void PlayMusicLooping(int16_t trackIndex)
	{
		if (g_directSoundInit)
		{
			if (IsStreamActive())
			{
				StopAndWait();

				while (IsStreamActive())
					;
			}

			g_audioShutdownPending = 0;

			PlayTrackByIndex(trackIndex, 1);
			g_curLoopingTrack = trackIndex;
		}
	}

	// FUNCTION: TOY2 0x004A3980
	int32_t PlayLoopingSound3D(void* owner, int32_t soundIndex, int32_t volume, int32_t leftVolume, int16_t rightVolume)
	{
		int32_t unclampedVolume = leftVolume;
		int32_t leftVolumeCopy = leftVolume;
		int32_t savedVolume = leftVolume;

		if (leftVolume >= 0)
		{
			if (leftVolume > 150)
				leftVolume = 150;
		}
		else
		{
			leftVolume = 0;
		}

		if (unclampedVolume < 0)
		{
			leftVolumeCopy = 0;
			savedVolume = leftVolumeCopy;
		}
		else if (unclampedVolume > 150)
		{
			leftVolumeCopy = 150;
			savedVolume = leftVolumeCopy;
		}

		int32_t matchIndex = -1;
		int32_t index = 0;

		void** state = g_soundBufferStates;
		AudioChannel* channel = g_audioChannels;

		do
		{
			if (channel->bufferIndex != -1 && *state == owner)
				matchIndex = index;

			++channel;

			++index;
			++state;
		} while (channel < &g_audioChannels[32]);

		if (matchIndex == -1)
		{
			int32_t playedBuffer = PlaySoundBuffer(soundIndex + 1, leftVolume, leftVolumeCopy, owner, volume, 1);

			if (playedBuffer != -1)
			{
				int32_t freeChannel = -1;
				int32_t channelIndex = 0;
				AudioChannel* curChannel = g_audioChannels;

				do
				{
					if (curChannel->bufferIndex == playedBuffer)
					{
						g_audioChannels[channelIndex].timer = 16;
						g_audioChannels[channelIndex].curVolume = volume;
						g_audioChannels[channelIndex].targetVolume = volume;
						g_audioChannels[channelIndex].repeatCounter = rightVolume;

						return (leftVolume + savedVolume) / 2;
					}

					if (curChannel->bufferIndex == -1)
						freeChannel = channelIndex;

					++curChannel;
					++channelIndex;
				} while (curChannel < &g_audioChannels[32]);

				if (freeChannel != -1)
				{
					g_audioChannels[freeChannel].bufferIndex = playedBuffer;
					g_audioChannels[freeChannel].timer = 16;
					g_audioChannels[freeChannel].curVolume = volume;
					g_audioChannels[freeChannel].targetVolume = volume;
					g_audioChannels[freeChannel].repeatCounter = rightVolume;
				}

				leftVolumeCopy = savedVolume;
			}

			return (leftVolumeCopy + leftVolume) / 2;
		}
		else
		{
			g_audioChannels[matchIndex].targetVolume = volume;
			int16_t retriggerDelay = g_audioChannels[matchIndex].repeatCounter;

			if (retriggerDelay <= 0)
			{
				g_audioChannels[matchIndex].repeatCounter = rightVolume;
				g_audioChannels[matchIndex].targetVolume = volume;
			}
			else
			{
				g_audioChannels[matchIndex].repeatCounter = retriggerDelay - 1;
			}

			int16_t curVolume = g_audioChannels[matchIndex].curVolume;

			if (g_audioChannels[matchIndex].targetVolume > curVolume)
				g_audioChannels[matchIndex].curVolume = curVolume + 64;

			int16_t newVolume = g_audioChannels[matchIndex].curVolume;

			if (g_audioChannels[matchIndex].targetVolume < newVolume)
				g_audioChannels[matchIndex].curVolume = newVolume - 64;

			g_audioChannels[matchIndex].bufferIndex =
				PlaySoundBuffer(soundIndex + 1, leftVolume, leftVolumeCopy, owner, g_audioChannels[matchIndex].curVolume, 1);

			g_audioChannels[matchIndex].timer = 16;
			return (leftVolumeCopy + leftVolume) / 2;
		}
	}

	// FUNCTION: TOY2 0x004A3B90
	int32_t PlayLoopingSound3DPositional(void* owner, int32_t soundIndex, int32_t volume, int32_t leftVolume, void* unused, int16_t rightVolume)
	{
		return PlayLoopingSound3D(owner, soundIndex, volume, leftVolume, rightVolume);
	}

	// FUNCTION: TOY2 0x0047D670 [MATCHED]
	void LoadSoundPack(SoundEffect* sfxPack, int32_t wavId)
	{
		char buffer[256];

		if (wavId >= 0 && wavId <= 16)
		{
			char** wavList = sfxPack[wavId].wavList;
			int32_t effectId = sfxPack[wavId].effectId;

			while (*wavList)
			{
				if (**wavList)
				{
					sprintf(buffer, "%s.wav", *wavList);
					LoadSoundEffect(buffer, effectId, 0);
				}

				++wavList;
				++effectId;
			}
		}
	}

	// FUNCTION: TOY2 0x0047E7D0
	void ReleaseAllBuffers()
	{
		if (g_directSoundInit)
		{
			for (int32_t i = 0; i >= -767; --i)
			{
				LPDIRECTSOUNDBUFFER soundBuffer = g_soundEffectBuffers[i + 767];

				if (soundBuffer)
				{
					char statusByte;
					DWORD status;

					if (g_directSoundInit)
					{
						g_lastDSoundError = soundBuffer->GetStatus(&status);
						statusByte = status;
					}
					else
					{
						statusByte = 0;
					}

					if ((statusByte & DSBSTATUS_PLAYING) == 1)
						g_soundEffectBuffers[i + 767]->Stop();

					g_soundEffectBuffers[i + 767]->Release();
					g_soundEffectBuffers[i + 767] = 0;
					g_soundBufferStates[i + 767] = 0;
				}
			}
		}
	}

	// FUNCTION: TOY2 0x0047D930
	BOOL ReassertChannelPlaying(int32_t bufferIndex)
	{
		DWORD status;
		char buffer[256];

		if (! g_directSoundInit)
			return 0;

		g_lastDSoundError = g_soundEffectBuffers[bufferIndex]->GetStatus(&status);

		// the buffer memory was lost, load the sounds of the level again
		if ((status & DSBSTATUS_BUFFERLOST) != 0)
		{
			int32_t first = bufferIndex & 0xFFFF8;
			int32_t last = (bufferIndex & 0xFFFF8) + 8;

			if (first < last)
			{
				do
				{
					int32_t directSoundInit = g_directSoundInit;
					g_soundBufferStates[first] = 0;

					if (directSoundInit)
						g_soundEffectBuffers[first]->Release();

					++first;
				} while (first < last);
			}

			int32_t curPack = g_curSfxPack;
			g_reloadEffectIndex = bufferIndex / 8;

			if (g_directSoundInit)
			{
				if (IsStreamActive())
				{
					StopAndWait();

					while (IsStreamActive())
						;
				}

				g_audioShutdownPending = 0;

				if (g_directSoundInit)
				{
					ReleaseAllBuffers();

					memset(g_soundEffectBuffers, 0, sizeof(g_soundEffectBuffers));
					memset(g_soundBufferStates, 0, sizeof(g_soundBufferStates));

					for (int32_t channel = 0; channel < 32; channel++)
					{
						g_audioChannels[channel].bufferIndex = -1;
						g_audioChannels[channel].timer = -1;
					}
				}
			}

			LoadSoundPack(g_sfxPack1, 0);

			if (curPack > 0)
			{
				LoadSoundPack(g_sfxPack1, curPack);

				if (curPack <= 16)
				{
					char** wavList = g_sfxPack2[curPack].wavList;
					int32_t effectId = g_sfxPack2[curPack].effectId;

					for (const char* wavName = *wavList; wavName; ++effectId)
					{
						if (*wavName)
						{
							sprintf(buffer, "%s.wav", wavName);
							LoadSoundEffect(buffer, effectId, 0);
						}

						wavName = wavList[1];
						++wavList;
					}
				}
			}

			g_curSfxPack = curPack;
			g_reloadEffectIndex = -1;
		}

		char statusByte;

		if (g_directSoundInit)
		{
			g_lastDSoundError = g_soundEffectBuffers[bufferIndex]->GetStatus(&status);
			statusByte = status;
		}
		else
		{
			statusByte = 0;
		}

		BOOL result = 1;

		if ((statusByte & DSBSTATUS_LOOPING) != 0)
		{
			g_lastDSoundError = g_soundEffectBuffers[bufferIndex]->Play(0, 0, 0);

			if (! g_lastDSoundError)
				return 0;

			g_lastDSoundError = g_soundEffectBuffers[bufferIndex]->Stop();

			if (! g_lastDSoundError)
				return 0;
		}

		return result;
	}

	// FUNCTION: TOY2 0x0047E850
	void ReleaseBuffers()
	{
		if (g_directSoundInit)
		{
			StopAndWait();

			if (g_directSoundInit)
			{
				for (int32_t i = 0; i >= -767; --i)
				{
					LPDIRECTSOUNDBUFFER soundBuffer = g_soundEffectBuffers[i + 767];

					if (soundBuffer)
					{
						char statusByte;
						DWORD status;

						if (g_directSoundInit)
						{
							g_lastDSoundError = soundBuffer->GetStatus(&status);
							statusByte = status;
						}
						else
						{
							statusByte = 0;
						}

						if ((statusByte & DSBSTATUS_PLAYING) == 1)
							g_soundEffectBuffers[i + 767]->Stop();

						g_soundEffectBuffers[i + 767]->Release();
						g_soundEffectBuffers[i + 767] = 0;
						g_soundBufferStates[i + 767] = 0;
					}
				}
			}

			if (g_directSound)
				g_directSound->Release();

			g_directSound = 0;
			ResetChannelsTable();
			g_unk726230 = 0;

			memset(g_soundEffectBuffers, 0, sizeof(g_soundEffectBuffers));
			memset(g_soundBufferStates, 0, sizeof(g_soundBufferStates));

			g_audioDeviceCount = 0;
			g_directSoundInit = 0;
		}
	}

	// FUNCTION: TOY2 0x004A3810
	int32_t PlayOneShotSound3D(int32_t soundIndex, int32_t volume, int32_t minVolume, Vector3I* worldPos)
	{
		// the camera position and the first two rows of the view matrix are kept in the scratch memory
		uint32_t* scratch = (uint32_t*)Toy2::Collision::g_mathScratch;
		int16_t* matrix = (int16_t*)&Toy2::Collision::g_mathScratch[916];

		int32_t z = worldPos->z - scratch[223];
		int32_t y = worldPos->y - scratch[222];
		int32_t x = worldPos->x - scratch[221];

		int32_t forward = (z * matrix[2] + y * matrix[1] + x * matrix[0]) / 0x20000;
		int32_t side = (z * matrix[8] + y * matrix[7] + x * matrix[6]) / 0x20000;

		int32_t sideSquared = side / 2 * (side / 2);

		int32_t leftVolume = minVolume * ((3072 - (int32_t)sqrt((double)(sideSquared + (forward + 1024) * (forward + 1024)))) / 24) / 128;
		int32_t rightVolume = minVolume * ((3072 - (int32_t)sqrt((double)(sideSquared + (forward - 1024) * (forward - 1024)))) / 24) / 128;

		if (leftVolume >= 0)
		{
			if (leftVolume > 128)
				leftVolume = 128;
		}
		else
		{
			leftVolume = 0;
		}

		if (rightVolume >= 0)
		{
			if (rightVolume > 128)
				rightVolume = 128;
		}
		else
		{
			rightVolume = 0;
		}

		PlaySoundBuffer(soundIndex + 1, leftVolume, rightVolume, 0, volume, 0);

		return (rightVolume + leftVolume) / 2;
	}

	// FUNCTION: TOY2 0x0049E660
	void PlaySoundEffect(int32_t effectId, Vector3I* worldPos)
	{
		if (effectId >= 0)
		{
			SoundEffectPreset* soundPreset = &g_soundEffectPresets[effectId];

			int32_t feedbackVolume = 150;
			int16_t soundBufferIndex = soundPreset->soundBufferIndex;

			// the sound depends on the level
			if ((soundPreset->soundBufferIndex & 0x4000) != 0)
			{
				int16_t* entry = &g_unk5028E8[soundBufferIndex & 0x3FFF];

				if (*entry != Toy2::g_levelFileIndex)
				{
					int16_t nextLevel;

					do
					{
						nextLevel = entry[2];
						entry += 2;
					} while (nextLevel != Toy2::g_levelFileIndex);
				}

				soundBufferIndex = entry[1];
			}

			if (soundBufferIndex)
			{
				int32_t pitch = soundPreset->pitch;

				if (pitch > 0)
				{
					int16_t randomShift = soundPreset->unkWord5;

					if (randomShift >= 0)
						pitch += (8 * *g_randDatBufferPtr++) >> randomShift;
				}
				else
				{
					pitch = g_pitchArray[-pitch];
				}

				if (abs((int32_t)worldPos) >= 256)
				{
					int32_t result;

					if (soundBufferIndex <= 0)
						result = PlayLoopingSound3DPositional(
							worldPos, (soundBufferIndex & 0x7FFF) - 1, pitch, soundPreset->volume, worldPos, soundPreset->unkWord4);
					else
						result = PlayOneShotSound3D(soundBufferIndex - 1, pitch, soundPreset->volume, worldPos);

					feedbackVolume = result;
				}
				else
				{
					int32_t unkWord4 = soundPreset->unkWord4;
					int32_t volume = soundPreset->volume;

					if (soundBufferIndex <= 0)
						PlayLoopingSound3D((void*)(int32_t)soundPreset->pitch, (soundBufferIndex & 0x7FFF) - 1, pitch, volume, unkWord4);
					else
						PlayOneShotSoundGlobal(soundBufferIndex - 1, pitch, volume, unkWord4);
				}
			}

			int16_t unkWord6 = soundPreset->unkWord6;

			if (Toy2::g_feedbackTimerA < 2 * unkWord6)
				Toy2::g_feedbackTimerA = 2 * unkWord6;

			int16_t unkWord7 = soundPreset->unkWord7;

			if (Toy2::g_feedbackTimerB < 2 * unkWord7)
				Toy2::g_feedbackTimerB = 2 * unkWord7;

			int32_t feedback = feedbackVolume * soundPreset->unkWord8 / 64;

			if (feedback > 255)
				feedback = 255;

			if (Toy2::g_feedbackTimerA_sub < feedback)
				Toy2::g_feedbackTimerA_sub = feedback;
		}
		else
		{
			int32_t player = Toy2::g_curSequencePlayer;
			int32_t index = Toy2::g_curSequencePlayer;

			Toy2::g_soundSequencePlayers[index].pos.x = worldPos->x;
			Toy2::g_soundSequencePlayers[index].pos.y = worldPos->y;
			Toy2::g_soundSequencePlayers[index].pos.z = worldPos->z;
			Toy2::g_soundSequencePlayers[index].scriptCursor = g_soundScripts[-effectId - 1];

			Toy2::g_soundSequencePlayers[index].countdown = 0;

			int16_t* scriptCursor = Toy2::g_soundSequencePlayers[index].scriptCursor;

			Toy2::g_soundSequencePlayers[index].scriptCursor = scriptCursor + 3;

			int16_t feedbackA = *scriptCursor;

			if (*scriptCursor || scriptCursor[1])
			{
				if (Toy2::g_feedbackTimerA < 2 * feedbackA)
					Toy2::g_feedbackTimerA = 2 * feedbackA;

				int16_t feedbackB = scriptCursor[1];

				if (Toy2::g_feedbackTimerB < 2 * feedbackB)
					Toy2::g_feedbackTimerB = 2 * feedbackB;

				int16_t feedbackC = scriptCursor[2];

				if (Toy2::g_feedbackTimerA_sub < 2 * feedbackC)
					Toy2::g_feedbackTimerA_sub = 2 * feedbackC;
			}

			Toy2::g_curSequencePlayer = player + 1;

			if (player + 1 >= 7)
				Toy2::g_curSequencePlayer = 0;
		}
	}

	// FUNCTION: TOY2 0x0049E910
	void StartSoundSequenceOnActor(int32_t scriptIndex, Toy2::Actor::Toy2Actor* actor)
	{
		Toy2::g_soundSequencePlayers[7].pos.x = actor->transform.pos.x;
		Toy2::g_soundSequencePlayers[7].pos.y = actor->transform.pos.y;

		int32_t z = actor->transform.pos.z;

		Toy2::g_soundSequencePlayers[7].countdown = 0;
		Toy2::g_soundSequencePlayers[7].pos.z = z;

		int16_t* scriptCursor = g_soundScripts[-scriptIndex - 1];

		Toy2::g_soundSequencePlayers[7].scriptCursor = scriptCursor + 3;

		int16_t feedbackA = *scriptCursor;

		if (*scriptCursor || scriptCursor[1])
		{
			if (Toy2::g_feedbackTimerA < 2 * feedbackA)
				Toy2::g_feedbackTimerA = 2 * feedbackA;

			int16_t feedbackB = scriptCursor[1];

			if (Toy2::g_feedbackTimerB < 2 * feedbackB)
				Toy2::g_feedbackTimerB = 2 * feedbackB;

			int16_t feedbackC = scriptCursor[2];

			if (Toy2::g_feedbackTimerA_sub < 2 * feedbackC)
				Toy2::g_feedbackTimerA_sub = 2 * feedbackC;
		}
	}

	// FUNCTION: TOY2 0x0049E9C0
	void ClearSequence7Cursor() { Toy2::g_soundSequencePlayers[7].scriptCursor = 0; }

	// FUNCTION: TOY2 0x0049E9D0
	void UpdateSoundSequences()
	{
		// the original stops at the address of g_freezeBuzz, which is behind the 8 players
		for (int32_t player = 0; player < 8; player++)
		{
			Toy2::SoundSequencePlayer* sequence = &Toy2::g_soundSequencePlayers[player];

			if (sequence->scriptCursor)
			{
				sequence->countdown -= Renderer::g_frameDelta;

				if ((sequence->countdown & 0x8000u) != 0)
				{
					int16_t* scriptCursor = sequence->scriptCursor;
					int16_t command = *sequence->scriptCursor;

					if (command >= 0)
					{
					LABEL_9:
						PlayOneShotSound3D(*scriptCursor - 1, scriptCursor[1], scriptCursor[2], &sequence->pos);

						sequence->countdown = scriptCursor[3];
						sequence->scriptCursor += 4;
					}
					else
					{
						while (1)
						{
							if (command == -1)
								sequence->scriptCursor = 0;

							if (*scriptCursor != -2)
								break;

							scriptCursor = &sequence->scriptCursor[-scriptCursor[1]];
							sequence->scriptCursor = scriptCursor;

							command = *scriptCursor;

							if (*scriptCursor >= 0)
								goto LABEL_9;
						}
					}
				}
			}
		}
	}

	// FUNCTION: TOY2 0x004A3BE0
	void UpdateChannels()
	{
		if (g_audioShutdownPending)
		{
			if (! --g_audioShutdownPending)
			{
				if (g_audioDeviceGuids[0])
					PlayTrackByIndex(g_pendingTrackIndex, 0);
				else
					PlayTrackByIndex(g_pendingTrackIndex, 1);
			}
		}

		AudioChannel* channel = g_audioChannels;

		do
		{
			if (channel->bufferIndex != -1 && channel->timer != -1)
			{
				int16_t timer = channel->timer - 1;
				channel->timer = timer;

				if (timer <= 0)
				{
					if (ReassertChannelPlaying(channel->bufferIndex))
						channel->timer = 1;
					else
						channel->bufferIndex = -1;
				}
			}

			++channel;
		} while (channel < &g_audioChannels[32]);
	}

	// FUNCTION: TOY2 0x004A3C80
	int32_t PlayOneShotSound3DActor(void* owner, int32_t soundIndex, int32_t pitch, int32_t volume, Vector3I* pos)
	{
		int32_t leftVolume;
		int32_t rightVolume;

		if (pos != 0)
		{
			int32_t* scratch = (int32_t*)Toy2::Collision::g_mathScratch;
			int16_t* matrix = (int16_t*)&Toy2::Collision::g_mathScratch[916];

			int32_t z = (pos->z >> 9) - (scratch[223] >> 9);
			int32_t y = (pos->y >> 9) - (scratch[222] >> 9);
			int32_t x = (pos->x >> 9) - (scratch[221] >> 9);

			int32_t forward = (z * matrix[2] + y * matrix[1] + x * matrix[0]) / 4096;
			int32_t side = (z * matrix[8] + y * matrix[7] + x * matrix[6]) / 4096;

			int32_t sideSquared = side / 2 * (side / 2);

			leftVolume = (16 * (192 - (int32_t)sqrt((double)(sideSquared + (forward + 64) * (forward + 64))))) / 24;
			rightVolume = (16 * (192 - (int32_t)sqrt((double)(sideSquared + (forward - 64) * (forward - 64))))) / 24;
		}
		else
		{
			rightVolume = volume;
			leftVolume = volume;
		}

		if (leftVolume >= 0)
		{
			if (leftVolume > 150)
				leftVolume = 150;
		}
		else
		{
			leftVolume = 0;
		}

		if (rightVolume >= 0)
		{
			if (rightVolume > 150)
				rightVolume = 150;
		}
		else
		{
			rightVolume = 0;
		}

		int32_t playedBuffer = PlaySoundBuffer(soundIndex + 1, leftVolume, rightVolume, owner, 0, 0);

		if (playedBuffer != -1)
		{
			int32_t freeChannel = -1;
			int32_t channelIndex = 0;
			AudioChannel* curChannel = g_audioChannels;

			do
			{
				if (curChannel->bufferIndex == playedBuffer)
				{
					g_audioChannels[channelIndex].timer = -1;
					return (rightVolume + leftVolume) / 2;
				}

				if (curChannel->bufferIndex == -1)
					freeChannel = channelIndex;

				++curChannel;
				++channelIndex;
			} while (curChannel < &g_audioChannels[32]);

			if (freeChannel != -1)
			{
				g_audioChannels[freeChannel].bufferIndex = playedBuffer;
				g_audioChannels[freeChannel].timer = -1;
			}
		}

		return (rightVolume + leftVolume) / 2;
	}

	// FUNCTION: TOY2 0x004A3E60
	int32_t IsActorSoundPlaying(Toy2::Buzz::Toy2BuzzActor* buzzActor)
	{
		if (g_directSoundInit)
		{
			int32_t index = 0;
			AudioChannel* voice = g_audioChannels;

			while (voice->bufferIndex == -1 || voice->timer != -1 || g_soundBufferStates[voice->bufferIndex] != buzzActor)
			{
				++voice;
				++index;

				if (voice >= &g_audioChannels[32])
					return 0;
			}

			if ((IsEffectPlaying(g_audioChannels[index].bufferIndex) & 1) != 0)
				return 1;

			g_audioChannels[index].bufferIndex = -1;
		}

		return 0;
	}

	namespace Preset
	{
		// FUNCTION: TOY2 0x0049EA60
		int32_t PlayOneShotSound2(int32_t soundIndex, Toy2::Actor::Toy2Actor* owner)
		{
			return PlayOneShotSound3DActor(
				owner,
				g_soundEffectPresets[soundIndex].soundBufferIndex - 1,
				g_soundEffectPresets[soundIndex].pitch,
				g_soundEffectPresets[soundIndex].volume,
				&owner->transform.pos);
		}

		// FUNCTION: TOY2 0x0049EA90
		int32_t PlayOneShotSound(int32_t soundIndex, Toy2::Actor::Toy2Actor* owner)
		{
			return PlayOneShotSound3DActor(
				owner,
				g_soundEffectPresets[soundIndex].soundBufferIndex - 1,
				g_soundEffectPresets[soundIndex].pitch,
				g_soundEffectPresets[soundIndex].volume,
				&owner->transform.pos);
		}
	}
}

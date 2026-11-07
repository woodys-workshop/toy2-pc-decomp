#include "AudioManager/AudioManager.h"
#include "Toy2/Toy2.h"
#include "Toy2/D3DApp.h"
#include "Toy2/Collision.h"
#include "Renderer/Renderer.h"
#include "FileUtils.h"
#include "Logger.h"
#include "Random.h"
#include "AudioManager/MusicStream.h"

#include <MMSYSTEM.H>
#include <DSOUND.H>
#include <STDIO.H>
#include <STDLIB.H>
#include <STRING.H>
#include <MATH.H>
#include <WINDOWSX.H>

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

	STATIC_ASSERT(sizeof(AudioConfig) == 0x10);
	STATIC_ASSERT(sizeof(SoundEffect) == 0x8);
	STATIC_ASSERT(sizeof(AudioChannel) == 0xA);

	// GLOBAL: TOY2 0x00830E3C
	int16_t g_pitchArray[14];

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
	uint16_t g_soundEffectSampleRates[128];

	// GLOBAL: TOY2 0x00726334
	DWORD g_writeCursor;

	// GLOBAL: TOY2 0x00726338
	LPDIRECTSOUNDBUFFER g_soundEffectBuffers[768];

	// GLOBAL: TOY2 0x00726F3C
	int32_t g_audioShutdownPending;

	// the looping sounds (and the sounds of actors) that are tracked
	// GLOBAL: TOY2 0x00830E58
	AudioChannel g_audioChannels[32];

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
	int16_t g_soundScriptData[156] = {
		2,	  0,  0,	41, 6144, 64, 2,	41, 7040, 64, 2,	41, 7680, 64, 10,	41, 7680, 48, 10,	41, 7680, 32, 10,	41, 7680, 16,
		10,	  -1, 0,	0,	0,	  32, 5120, 96, 12,	  32, 4608, 96, 15,	  32, 5376, 96, 10,	  33, 4864, 96, 16,	  33, 4096, 96, 18,	  -1,
		0,	  0,  0,	41, 4608, 64, 4,	41, 5504, 64, 4,	41, 6144, 64, 4,	41, 5504, 64, 4,	-2, 16,	  0,  0,	0,	0,	  91,
		5120, 94, 20,	91, 5504, 94, 20,	91, 4864, 94, 20,	91, 5760, 94, 20,	91, 4736, 94, 20,	-1, 0,	  0,  0,	41, 4864, 64,
		5,	  41, 4352, 64, 5,	  41, 4864, 64, 5,	  41, 5248, 64, 5,	  41, 4864, 64, 5,	  41, 5248, 64, 5,	  41, 5888, 64, 10,	  41,
		5888, 48, 10,	41, 5888, 32, 10,	41, 5888, 16, 10,	-1, 0,	  0,  0,	41, 4608, 64, 10,	41, 4608, 64, 10,	-2, 16,	  0
	};

	// GLOBAL: TOY2 0x00503828
	int16_t* g_soundScripts[6] = {
		g_soundScriptData, g_soundScriptData + 28, g_soundScriptData + 52, g_soundScriptData + 74, g_soundScriptData + 98, g_soundScriptData + 142
	};

	STATIC_ASSERT(sizeof(g_soundScriptData) == 312);

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
	int32_t IsEffectPlaying(DWORD bufferIndex)
	{
		if (! g_directSoundInit)
			return 0;

		g_lastDSoundError = g_soundEffectBuffers[bufferIndex]->GetStatus(&bufferIndex);

		return bufferIndex;
	}

	// FUNCTION: TOY2 0x0047E3C0 [MATCHED]
	BOOL CALLBACK Enumerate(LPGUID lpGuid, LPCSTR description, LPCSTR lpcstrModule, LPVOID lpContext)
	{
		strcpy(g_audioDeviceDecs[g_audioDeviceCount], description);

		g_audioDeviceGuids[g_audioDeviceCount + 1] = lpGuid;
		++g_audioDeviceCount;

		return 1;
	}

	// FUNCTION: TOY2 0x0047E420 [MATCHED]
	int32_t CreateDirectSoundBuffer(IDirectSound* directSound, IDirectSoundBuffer** soundBuffer, DWORD bufferBytes)
	{
		PCMWAVEFORMAT waveFormat;
		DSBUFFERDESC bufferDesc;

		STATIC_ASSERT(sizeof(waveFormat) == 0x10);
		STATIC_ASSERT(sizeof(bufferDesc) == 0x14);

		memset(&waveFormat, 0, sizeof(waveFormat));

		waveFormat.wf.wFormatTag = g_audioConfig->formatTag;
		waveFormat.wf.nChannels = g_audioConfig->channels;
		waveFormat.wf.nSamplesPerSec = g_audioConfig->sampleRate;

		waveFormat.wf.nBlockAlign = g_audioConfig->blockAlign;
		waveFormat.wf.nAvgBytesPerSec = g_audioConfig->avgBytesPerSec;
		waveFormat.wBitsPerSample = g_audioConfig->bitsPerSample;

		memset(&bufferDesc, 0, sizeof(bufferDesc));
		bufferDesc.dwSize = 20;
		bufferDesc.dwFlags = 32992;
		bufferDesc.dwBufferBytes = bufferBytes;
		bufferDesc.lpwfxFormat = (LPWAVEFORMATEX)&waveFormat;

		if (! directSound->CreateSoundBuffer(&bufferDesc, soundBuffer, 0))
			return 1;

		*soundBuffer = 0;

		return 0;
	}

	// FUNCTION: TOY2 0x0047E4E0 [MATCHED]
	BOOL WriteToBuffer(LPDIRECTSOUNDBUFFER lpDsb, DWORD dwOffset, LPBYTE lpbSoundData, DWORD dwSoundBytes)
	{
		LPVOID lpvPtr1;
		DWORD dwBytes1;
		LPVOID lpvPtr2;
		DWORD dwBytes2;
		DWORD dwAudio1;
		DWORD dwAudio2;
		HRESULT hr;

		hr = lpDsb->Lock(dwOffset, dwSoundBytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);

		if (DSERR_BUFFERLOST == hr)
		{
			lpDsb->Restore();
			hr = lpDsb->Lock(dwOffset, dwSoundBytes, &lpvPtr1, &dwAudio1, &lpvPtr2, &dwAudio2, 0);
		}

		if (DS_OK == hr)
		{
			CopyMemory(lpvPtr1, lpbSoundData, dwBytes1);

			if (NULL != lpvPtr2)
			{
				CopyMemory(lpvPtr2, lpbSoundData + dwBytes1, dwBytes2);
			}

			hr = lpDsb->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);

			if (DS_OK == hr)
			{
				return TRUE;
			}
		}

		return FALSE;
	}

	enum
	{
		ER_MEM = 0xE000,
		ER_CANNOTOPEN = 0xE100,
		ER_NOTWAVEFILE = 0xE101,
		ER_CANNOTREAD = 0xE102,
		ER_CORRUPTWAVEFILE = 0xE103,
		ER_CANNOTWRITE = 0xE104
	};

	STATIC_ASSERT(sizeof(PCMWAVEFORMAT) == 0x10);
	STATIC_ASSERT(sizeof(WAVEFORMATEX) == 0x12);
	STATIC_ASSERT(sizeof(MMCKINFO) == 0x14);
	STATIC_ASSERT(sizeof(MMIOINFO) == 0x48);

	BOOL riffCopyChunk(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck);

	// FUNCTION: TOY2 0x004A3F10 [MATCHED]
	int32_t WaveOpenFile(char* pszFileName, HMMIO* phmmioIn, WAVEFORMATEX** ppwfxInfo, MMCKINFO* pckInRIFF)
	{
		HMMIO hmmioIn;
		MMCKINFO ckIn;
		PCMWAVEFORMAT pcmWaveFormat;
		WORD cbExtraAlloc;
		int32_t nError;
		FILE* fileHandle;

		*ppwfxInfo = NULL;
		nError = 0;
		hmmioIn = NULL;

		fileHandle = fopen(pszFileName, "rb");
		if (! fileHandle || (fclose(fileHandle), hmmioIn = mmioOpen(pszFileName, NULL, MMIO_ALLOCBUF | MMIO_READ), hmmioIn == NULL))
		{
			nError = ER_CANNOTOPEN;
			goto ERROR_READING_WAVE;
		}

		if ((nError = mmioDescend(hmmioIn, pckInRIFF, NULL, 0)) != 0)
			goto ERROR_READING_WAVE;

		if (pckInRIFF->ckid != FOURCC_RIFF || pckInRIFF->fccType != mmioFOURCC('W', 'A', 'V', 'E'))
		{
			nError = ER_NOTWAVEFILE;
			goto ERROR_READING_WAVE;
		}

		ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if ((nError = mmioDescend(hmmioIn, &ckIn, pckInRIFF, MMIO_FINDCHUNK)) != 0)
			goto ERROR_READING_WAVE;

		if (ckIn.cksize < sizeof(PCMWAVEFORMAT))
		{
			nError = ER_NOTWAVEFILE;
			goto ERROR_READING_WAVE;
		}

		if (mmioRead(hmmioIn, (HPSTR)&pcmWaveFormat, sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat))
		{
			nError = ER_CANNOTREAD;
			goto ERROR_READING_WAVE;
		}

		if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
			cbExtraAlloc = 0;
		else
		{
			if (mmioRead(hmmioIn, (HPSTR)&cbExtraAlloc, sizeof(cbExtraAlloc)) != sizeof(cbExtraAlloc))
			{
				nError = ER_CANNOTREAD;
				goto ERROR_READING_WAVE;
			}
		}

		if ((*ppwfxInfo = (WAVEFORMATEX*)GlobalAlloc(GMEM_FIXED, sizeof(WAVEFORMATEX) + cbExtraAlloc)) == NULL)
		{
			nError = ER_MEM;
			goto ERROR_READING_WAVE;
		}

		memcpy(*ppwfxInfo, &pcmWaveFormat, sizeof(pcmWaveFormat));
		(*ppwfxInfo)->cbSize = cbExtraAlloc;

		if (cbExtraAlloc != 0)
		{
			if (mmioRead(hmmioIn, (HPSTR)((BYTE*)&(*ppwfxInfo)->cbSize + sizeof(cbExtraAlloc)), cbExtraAlloc) != cbExtraAlloc)
			{
				nError = ER_NOTWAVEFILE;
				goto ERROR_READING_WAVE;
			}
		}

		if ((nError = mmioAscend(hmmioIn, &ckIn, 0)) != 0)
			goto ERROR_READING_WAVE;

		goto TEMPCLEANUP;

	ERROR_READING_WAVE:
		if (*ppwfxInfo != NULL)
		{
			GlobalFree(*ppwfxInfo);
			*ppwfxInfo = NULL;
		}

		if (hmmioIn != NULL)
		{
			mmioClose(hmmioIn, 0);
			hmmioIn = NULL;
		}

	TEMPCLEANUP:
		*phmmioIn = hmmioIn;
		return nError;
	}

	// FUNCTION: TOY2 0x004A40F0 [MATCHED]
	int32_t WaveStartDataRead(HMMIO* phmmioIn, MMCKINFO* pckIn, MMCKINFO* pckInRIFF)
	{
		int32_t nError;

		mmioSeek(*phmmioIn, pckInRIFF->dwDataOffset + sizeof(FOURCC), SEEK_SET);
		pckIn->ckid = mmioFOURCC('d', 'a', 't', 'a');
		nError = mmioDescend(*phmmioIn, pckIn, pckInRIFF, MMIO_FINDCHUNK);

		return nError;
	}

	// FUNCTION: TOY2 0x004A4130 [MATCHED]
	int32_t WaveReadFile(HMMIO hmmioIn, UINT cbRead, BYTE* pbDest, MMCKINFO* pckIn, UINT* cbActualRead)
	{
		MMIOINFO mmioinfoIn;
		int32_t nError;
		UINT cT;
		UINT cbDataIn;

		if ((nError = mmioGetInfo(hmmioIn, &mmioinfoIn, 0)) != 0)
			goto ERROR_CANNOT_READ;

		cbDataIn = cbRead;
		if (cbDataIn > pckIn->cksize)
			cbDataIn = pckIn->cksize;

		pckIn->cksize -= cbDataIn;

		for (cT = 0; cT < cbDataIn; cT++)
		{
			if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
			{
				if ((nError = mmioAdvance(hmmioIn, &mmioinfoIn, MMIO_READ)) != 0)
					goto ERROR_CANNOT_READ;

				if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
				{
					nError = ER_CORRUPTWAVEFILE;
					goto ERROR_CANNOT_READ;
				}
			}

			pbDest[cT] = (BYTE)*mmioinfoIn.pchNext++;
		}

		if ((nError = mmioSetInfo(hmmioIn, &mmioinfoIn, 0)) != 0)
			goto ERROR_CANNOT_READ;

		*cbActualRead = cbDataIn;
		goto FINISHED_READING;

	ERROR_CANNOT_READ:
		*cbActualRead = 0;

	FINISHED_READING:
		return nError;
	}

	// FUNCTION: TOY2 0x004A4200 [MATCHED]
	int32_t WaveCloseReadFile(HMMIO* phmmio, WAVEFORMATEX** ppwfxSrc)
	{
		if (*ppwfxSrc != NULL)
		{
			GlobalFree(*ppwfxSrc);
			*ppwfxSrc = NULL;
		}

		if (*phmmio != NULL)
		{
			mmioClose(*phmmio, 0);
			*phmmio = NULL;
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004A4240 [MATCHED]
	int32_t WaveCreateFile(char* pszFileName, HMMIO* phmmioOut, WAVEFORMATEX* pwfxDest, MMCKINFO* pckOut, MMCKINFO* pckOutRIFF)
	{
		int32_t nError;
		DWORD dwFactChunk;
		MMCKINFO ckOut1;

		dwFactChunk = (DWORD)-1;
		nError = 0;

		*phmmioOut = mmioOpen(pszFileName, NULL, MMIO_ALLOCBUF | MMIO_READWRITE | MMIO_CREATE);
		if (*phmmioOut == NULL)
		{
			nError = ER_CANNOTWRITE;
			goto ERROR_CANNOT_WRITE;
		}

		pckOutRIFF->fccType = mmioFOURCC('W', 'A', 'V', 'E');
		pckOutRIFF->cksize = 0;
		if ((nError = mmioCreateChunk(*phmmioOut, pckOutRIFF, MMIO_CREATERIFF)) != 0)
			goto ERROR_CANNOT_WRITE;

		pckOut->ckid = mmioFOURCC('f', 'm', 't', ' ');
		pckOut->cksize = sizeof(PCMWAVEFORMAT);
		if ((nError = mmioCreateChunk(*phmmioOut, pckOut, 0)) != 0)
			goto ERROR_CANNOT_WRITE;

		if (pwfxDest->wFormatTag == WAVE_FORMAT_PCM)
		{
			if (mmioWrite(*phmmioOut, (HPSTR)pwfxDest, sizeof(PCMWAVEFORMAT)) != sizeof(PCMWAVEFORMAT))
			{
				nError = ER_CANNOTWRITE;
				goto ERROR_CANNOT_WRITE;
			}
		}
		else
		{
			if ((UINT)mmioWrite(*phmmioOut, (HPSTR)pwfxDest, sizeof(*pwfxDest) + pwfxDest->cbSize) != sizeof(*pwfxDest) + pwfxDest->cbSize)
			{
				nError = ER_CANNOTWRITE;
				goto ERROR_CANNOT_WRITE;
			}
		}

		if ((nError = mmioAscend(*phmmioOut, pckOut, 0)) != 0)
			goto ERROR_CANNOT_WRITE;

		ckOut1.ckid = mmioFOURCC('f', 'a', 'c', 't');
		ckOut1.cksize = 0;
		if ((nError = mmioCreateChunk(*phmmioOut, &ckOut1, 0)) != 0)
			goto ERROR_CANNOT_WRITE;

		if (mmioWrite(*phmmioOut, (HPSTR)&dwFactChunk, sizeof(dwFactChunk)) != sizeof(dwFactChunk))
		{
			nError = ER_CANNOTWRITE;
			goto ERROR_CANNOT_WRITE;
		}

		if ((nError = mmioAscend(*phmmioOut, &ckOut1, 0)) != 0)
			nError = ER_CANNOTWRITE;

	ERROR_CANNOT_WRITE:
		return nError;
	}

	// FUNCTION: TOY2 0x004A4360 [MATCHED]
	int32_t WaveStartDataWrite(HMMIO* phmmioOut, MMCKINFO* pckOut, MMIOINFO* pmmioinfoOut)
	{
		int32_t nError;

		pckOut->ckid = mmioFOURCC('d', 'a', 't', 'a');
		pckOut->cksize = 0;
		if ((nError = mmioCreateChunk(*phmmioOut, pckOut, 0)) != 0)
			return nError;

		nError = mmioGetInfo(*phmmioOut, pmmioinfoOut, 0);
		return nError;
	}

	// FUNCTION: TOY2 0x004A43A0 [MATCHED]
	int32_t WaveWriteFile(HMMIO hmmioOut, UINT cbWrite, BYTE* pbSrc, MMCKINFO* pckOut, UINT* cbActualWrite, MMIOINFO* pmmioinfoOut)
	{
		int32_t nError = 0;
		UINT cT;

		*cbActualWrite = 0;

		for (cT = 0; cT < cbWrite; cT++)
		{
			if (pmmioinfoOut->pchNext == pmmioinfoOut->pchEndWrite)
			{
				pmmioinfoOut->dwFlags |= MMIO_DIRTY;
				if ((nError = mmioAdvance(hmmioOut, pmmioinfoOut, MMIO_WRITE)) != 0)
					goto ERROR_CANNOT_WRITE;
			}

			BYTE* next = (BYTE*)pmmioinfoOut->pchNext;
			*next++ = pbSrc[cT];
			pmmioinfoOut->pchNext = (HPSTR)next;
			(*cbActualWrite)++;
		}

	ERROR_CANNOT_WRITE:
		return nError;
	}

	// FUNCTION: TOY2 0x004A4410 [MATCHED]
	int32_t WaveCloseWriteFile(HMMIO* phmmioOut, MMCKINFO* pckOut, MMCKINFO* pckOutRIFF, MMIOINFO* pmmioinfoOut, DWORD cSamples)
	{
		int32_t nError;

		if (*phmmioOut == NULL)
			return 0;

		pmmioinfoOut->dwFlags |= MMIO_DIRTY;
		if ((nError = mmioSetInfo(*phmmioOut, pmmioinfoOut, 0)) != 0)
			goto ERROR_CANNOT_WRITE;

		if ((nError = mmioAscend(*phmmioOut, pckOut, 0)) != 0)
			goto ERROR_CANNOT_WRITE;

		if ((nError = mmioAscend(*phmmioOut, pckOutRIFF, 0)) != 0)
			goto ERROR_CANNOT_WRITE;

		mmioSeek(*phmmioOut, 0, SEEK_SET);
		if ((nError = mmioDescend(*phmmioOut, pckOutRIFF, NULL, 0)) != 0)
			goto ERROR_CANNOT_WRITE;

		pckOut->ckid = mmioFOURCC('f', 'a', 'c', 't');
		if (mmioDescend(*phmmioOut, pckOut, pckOutRIFF, MMIO_FINDCHUNK) == 0)
		{
			mmioWrite(*phmmioOut, (HPSTR)&cSamples, sizeof(DWORD));
			mmioAscend(*phmmioOut, pckOut, 0);
		}

		nError = mmioAscend(*phmmioOut, pckOutRIFF, 0);

	ERROR_CANNOT_WRITE:
		if (*phmmioOut != NULL)
		{
			mmioClose(*phmmioOut, 0);
			*phmmioOut = NULL;
		}

		return nError;
	}

	// FUNCTION: TOY2 0x004A44F0 [MATCHED]
	int32_t WaveCopyUselessChunks(HMMIO* phmmioIn, MMCKINFO* pckIn, MMCKINFO* pckInRiff, HMMIO* phmmioOut, MMCKINFO* pckOut, MMCKINFO* pckOutRiff)
	{
		int32_t nError;

		if ((nError = mmioSeek(*phmmioIn, pckInRiff->dwDataOffset + sizeof(FOURCC), SEEK_SET)) == -1)
		{
			nError = ER_CANNOTREAD;
			goto ERROR_IN_PROC;
		}

		nError = 0;

		while (mmioDescend(*phmmioIn, pckIn, pckInRiff, 0) == 0)
		{
			if (pckIn->dwDataOffset + pckIn->cksize > pckInRiff->dwDataOffset + pckInRiff->cksize)
				goto ERROR_IN_PROC;

			switch (pckIn->ckid)
			{
				case mmioFOURCC('p', 'l', 's', 't'):
				case mmioFOURCC('D', 'I', 'S', 'P'):
					riffCopyChunk(*phmmioIn, *phmmioOut, pckIn);
					break;
			}

			mmioAscend(*phmmioIn, pckIn, 0);
		}

	ERROR_IN_PROC:
		{
			int32_t nErrorT;
			nErrorT = mmioSeek(*phmmioIn, pckInRiff->dwDataOffset + sizeof(FOURCC), SEEK_SET);
		}

		return nError;
	}

	// FUNCTION: TOY2 0x004A45B0 [MATCHED]
	BOOL riffCopyChunk(HMMIO hmmioSrc, HMMIO hmmioDst, const LPMMCKINFO lpck)
	{
		MMCKINFO ck;
		HPSTR hpBuf;

		hpBuf = (HPSTR)GlobalAllocPtr(GHND, lpck->cksize);
		if (! hpBuf)
			return FALSE;

		ck.ckid = lpck->ckid;
		ck.cksize = lpck->cksize;
		if (mmioCreateChunk(hmmioDst, &ck, 0))
			goto rscc_Error;

		if (mmioRead(hmmioSrc, hpBuf, lpck->cksize) != (LONG)lpck->cksize)
			goto rscc_Error;

		if (mmioWrite(hmmioDst, hpBuf, lpck->cksize) != (LONG)lpck->cksize)
			goto rscc_Error;

		if (mmioAscend(hmmioDst, &ck, 0))
			goto rscc_Error;

		if (hpBuf)
			GlobalFreePtr(hpBuf);

		return TRUE;

	rscc_Error:
		if (hpBuf)
			GlobalFreePtr(hpBuf);

		return FALSE;
	}

	// FUNCTION: TOY2 0x004A4680
	int32_t WaveLoadFile(char* pszFileName, UINT* cbSize, DWORD* unusedSampleCount, WAVEFORMATEX** ppwfxInfo, BYTE** ppbData)
	{
		HMMIO hmmioIn;
		MMCKINFO ckInRiff;
		MMCKINFO ckIn;
		int32_t nError;
		UINT cbActualRead;

		*ppbData = NULL;
		*ppwfxInfo = NULL;
		*cbSize = 0;

		if ((nError = WaveOpenFile(pszFileName, &hmmioIn, ppwfxInfo, &ckInRiff)) != 0)
		{
			Logger::DebugLog("Unable to open %s\r\n", pszFileName);
			goto ERROR_LOADING;
		}

		mmioSeek(hmmioIn, ckInRiff.dwDataOffset + sizeof(FOURCC), SEEK_SET);
		ckIn.ckid = mmioFOURCC('d', 'a', 't', 'a');
		if ((nError = mmioDescend(hmmioIn, &ckIn, &ckInRiff, MMIO_FINDCHUNK)) != 0)
			goto ERROR_LOADING;

		if ((*ppbData = (BYTE*)GlobalAlloc(GMEM_FIXED, ckIn.cksize)) == NULL)
		{
			nError = ER_MEM;
			goto ERROR_LOADING;
		}

		if ((nError = WaveReadFile(hmmioIn, ckIn.cksize, *ppbData, &ckIn, &cbActualRead)) != 0)
			goto ERROR_LOADING;

		*cbSize = cbActualRead;
		goto DONE_LOADING;

	ERROR_LOADING:
		if (*ppbData != NULL)
		{
			GlobalFree(*ppbData);
			*ppbData = NULL;
		}

		if (*ppwfxInfo != NULL)
		{
			GlobalFree(*ppwfxInfo);
			*ppwfxInfo = NULL;
		}

	DONE_LOADING:
		if (hmmioIn != NULL)
		{
			mmioClose(hmmioIn, 0);
			hmmioIn = NULL;
		}

		return nError;
	}

	// FUNCTION: TOY2 0x004A47A0 [MATCHED]
	int32_t WaveSaveFile(char* pszFileName, UINT cbSize, DWORD cSamples, WAVEFORMATEX* pwfxDest, BYTE* pbData)
	{
		HMMIO hmmioOut;
		MMCKINFO ckOut;
		MMCKINFO ckOutRIFF;
		MMIOINFO mmioinfoOut;
		UINT cbActualWrite;
		int32_t nError;

		if ((nError = WaveCreateFile(pszFileName, &hmmioOut, pwfxDest, &ckOut, &ckOutRIFF)) != 0)
			return nError;

		ckOut.ckid = mmioFOURCC('d', 'a', 't', 'a');
		ckOut.cksize = 0;
		if ((nError = mmioCreateChunk(hmmioOut, &ckOut, 0)) != 0)
			return nError;

		if ((nError = mmioGetInfo(hmmioOut, &mmioinfoOut, 0)) != 0)
			return nError;

		{
			HMMIO writeHandle = hmmioOut;
			UINT cT;
			int32_t writeError = 0;

			cbActualWrite = 0;
			for (cT = 0; cT < cbSize; cT++)
			{
				if (mmioinfoOut.pchNext == mmioinfoOut.pchEndWrite)
				{
					mmioinfoOut.dwFlags |= MMIO_DIRTY;
					if ((writeError = mmioAdvance(writeHandle, &mmioinfoOut, MMIO_WRITE)) != 0)
						break;
				}

				BYTE* next = (BYTE*)mmioinfoOut.pchNext;
				*next++ = pbData[cT];
				mmioinfoOut.pchNext = (HPSTR)next;
				cbActualWrite++;
			}

			nError = writeError;
		}

		if (nError != 0)
			goto ERROR_SAVING;

		nError = WaveCloseWriteFile(&hmmioOut, &ckOut, &ckOutRIFF, &mmioinfoOut, cSamples);

	ERROR_SAVING:
		return nError;
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

			if (! WaveLoadFile(filePath, (UINT*)&g_wavDataSize, (DWORD*)&g_unused, (WAVEFORMATEX**)&g_audioConfig, &g_wavSourceData))
			{
				int32_t bufferBaseIndex = 6 * effect;
				LPDIRECTSOUNDBUFFER* primaryBuffer = &g_soundEffectBuffers[bufferBaseIndex];

				if (CreateDirectSoundBuffer(g_directSound, primaryBuffer, g_wavDataSize) == 1)
				{
					WriteToBuffer(*primaryBuffer, 0, g_wavSourceData, g_wavDataSize);

					WORD sampleRate = g_audioConfig->sampleRate;

					LPDIRECTSOUNDBUFFER buffer = *primaryBuffer;
					g_soundEffectSampleRates[effect] = sampleRate;

					buffer->SetFrequency(sampleRate);
					(*primaryBuffer)->SetPan(0);
					(*primaryBuffer)->SetVolume(0);

					g_soundBufferStates[bufferBaseIndex] = (void*)1;
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
			else
			{
				Logger::Log("LoadSoundEffect : Calling WaveLoadFile failed - no sound effect %s, effect %d.\n", filePath, effect);
			}
		}
	}

	// FUNCTION: TOY2 0x004A3BC0
	void ResetChannelsTable()
	{
		for (int32_t channel = 0; channel < 32; ++channel)
		{
			g_audioChannels[channel].bufferIndex = -1;
		}
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

	namespace Stream
	{
		// FUNCTION: TOY2 0x00436CC0
		int32_t IsThreadReady() { return g_streamThreadReady; }

		// FUNCTION: TOY2 0x00436CD0 [MATCHED]
		int32_t GetSetThreadEvent() { return g_setThreadEvent; }

	}

	// FUNCTION: TOY2 0x0049ACC0 [MATCHED]
	const char* DSErrorToString(HRESULT error)
	{
		switch (error)
		{
			case DSERR_ALLOCATED:
				return "The request failed because resources, such as a priority level, were already in use by another caller. ";
			case DSERR_ALREADYINITIALIZED:
				return "The object is already initialized. ";
			case DSERR_BADFORMAT:
				return "The specified wave format is not supported. ";
			case DSERR_BUFFERLOST:
				return "The buffer memory has been lost and must be restored. ";
			case DSERR_CONTROLUNAVAIL:
				return "The control (volume, pan, and so forth) requested by the caller is not available. ";
			case DSERR_GENERIC:
				return "An undetermined error occurred inside the DirectSound subsystem. ";
			case DSERR_INVALIDCALL:
				return "This function is not valid for the current state of this object. ";
			case DSERR_INVALIDPARAM:
				return "An invalid parameter was passed to the returning function. ";
			case DSERR_NOAGGREGATION:
				return "The object does not support aggregation. ";
			case DSERR_NODRIVER:
				return "No sound driver is available for use. ";
			case DSERR_NOINTERFACE:
				return "The requested COM interface is not available. ";
			case DSERR_OTHERAPPHASPRIO:
				return "Another application has a higher priority level, preventing this call from succeeding ";
			case DSERR_OUTOFMEMORY:
				return "The DirectSound subsystem could not allocate sufficient memory to complete the caller's request. ";
			case DSERR_PRIOLEVELNEEDED:
				return "The caller does not have the priority level required for the function to succeed. ";
			case DSERR_UNINITIALIZED:
				return "The IDirectSound::Initialize method has not been called or has not been called successfully before other methods were called. ";
			case DSERR_UNSUPPORTED:
				return "The function called is not supported at this time. ";
			default:
				return "Unknown error!!";
		}
	}

	// FUNCTION: TOY2 0x0049AE40 [MATCHED]
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
					case DSERR_ALLOCATED:
						error = "The request failed because resources, such as a priority level, were already in use by another caller. ";
						break;
					case DSERR_ALREADYINITIALIZED:
						error = "The object is already initialized. ";
						break;
					case DSERR_BADFORMAT:
						error = "The specified wave format is not supported. ";
						break;
					case DSERR_BUFFERLOST:
						error = "The buffer memory has been lost and must be restored. ";
						break;
					case DSERR_CONTROLUNAVAIL:
						error = "The control (volume, pan, and so forth) requested by the caller is not available. ";
						break;
					case DSERR_GENERIC:
						error = "An undetermined error occurred inside the DirectSound subsystem. ";
						break;
					case DSERR_INVALIDCALL:
						error = "This function is not valid for the current state of this object. ";
						break;
					case DSERR_INVALIDPARAM:
						error = "An invalid parameter was passed to the returning function. ";
						break;
					case DSERR_NOAGGREGATION:
						error = "The object does not support aggregation. ";
						break;
					case DSERR_NODRIVER:
						error = "No sound driver is available for use. ";
						break;
					case DSERR_NOINTERFACE:
						error = "The requested COM interface is not available. ";
						break;
					case DSERR_OTHERAPPHASPRIO:
						error = "Another application has a higher priority level, preventing this call from succeeding ";
						break;
					case DSERR_OUTOFMEMORY:
						error = "The DirectSound subsystem could not allocate sufficient memory to complete the caller's request. ";
						break;
					case DSERR_PRIOLEVELNEEDED:
						error = "The caller does not have the priority level required for the function to succeed. ";
						break;
					case DSERR_UNINITIALIZED:
						error =
							"The IDirectSound::Initialize method has not been called or has not been called successfully before other methods were called. ";
						break;
					case DSERR_UNSUPPORTED:
						error = "The function called is not supported at this time. ";
						break;
					default:
						error = "Unknown error!!";
						break;
				}

				Logger::Log("SETMUSICVOL : Failed to set volume - error is %s.\n", error);
			}
		}
	}

	// FUNCTION: TOY2 0x00436D80 [MATCHED]
	void FillBuffer()
	{
		LPVOID audioPtr1;
		DWORD audioBytes1;
		UINT bytesOut = 0;
		DWORD curPlayCursor;
		DWORD curWriteCursor;

		if (g_directSound && g_musicSoundBuffer)
		{
			if (! g_musicSoundBuffer->GetCurrentPosition(&curPlayCursor, &curWriteCursor))
			{
				DWORD bytesPlayed;

				if (curPlayCursor < g_wiWave.dwLastPos)
					bytesPlayed = g_wiWave.dwBufferSize - g_wiWave.dwLastPos + curPlayCursor;
				else
					bytesPlayed = curPlayCursor - g_wiWave.dwLastPos;

				g_wiWave.dwProgress += bytesPlayed;
				g_wiWave.dwLastPos = curPlayCursor;
			}

			if (! g_wiWave.bFoundEnd)
			{
				if (! g_musicSoundBuffer->Lock(g_wiWave.dwNextWriteOffset, g_wiWave.dwNotifySize, &audioPtr1, &audioBytes1, 0, 0, 0))
				{
					WaveReadFile(g_wiWave.hmmio, audioBytes1, (uint8_t*)audioPtr1, &g_wiWave.mmck, &bytesOut);
					DWORD totalRead = bytesOut;

					if (bytesOut < audioBytes1)
					{
						if (! g_wiWave.bLoopFile)
						{
							g_wiWave.bFoundEnd = TRUE;

							memset((uint8_t*)audioPtr1 + bytesOut, (uint8_t)(g_wiWave.pwfx->wBitsPerSample == 8 ? 0x80 : 0), audioBytes1 - bytesOut);
						}
						else
						{
							while (totalRead < audioBytes1)
							{
								if (WaveStartDataRead(&g_wiWave.hmmio, &g_wiWave.mmck, &g_wiWave.mmckInRIFF))
									break;

								if (WaveReadFile(g_wiWave.hmmio, audioBytes1 - totalRead, (uint8_t*)audioPtr1 + totalRead, &g_wiWave.mmck, &bytesOut))
									break;

								totalRead += bytesOut;
							}
						}
					}

					g_musicSoundBuffer->Unlock(audioPtr1, audioBytes1, 0, 0);
					g_wiWave.dwNextWriteOffset += audioBytes1;

					if (g_wiWave.dwNextWriteOffset >= g_wiWave.dwBufferSize)
						g_wiWave.dwNextWriteOffset -= g_wiWave.dwBufferSize;
				}
			}
			else
			{
				if (! g_musicSoundBuffer->Lock(g_wiWave.dwNextWriteOffset, g_wiWave.dwNotifySize, &audioPtr1, &audioBytes1, 0, 0, 0))
				{
					memset(audioPtr1, (uint8_t)(g_wiWave.pwfx->wBitsPerSample == 8 ? 0x80 : 0), audioBytes1);

					g_musicSoundBuffer->Unlock(audioPtr1, audioBytes1, 0, 0);
				}

				if (g_wiWave.mmckInRIFF.cksize > g_wiWave.dwNotifySize && g_wiWave.dwProgress >= g_wiWave.mmckInRIFF.cksize - g_wiWave.dwNotifySize
					|| g_wiWave.dwProgress >= g_wiWave.mmckInRIFF.cksize)
				{
					g_wiWave.bDonePlaying = TRUE;
				}
			}
		}
	}

	namespace Stream
	{
		// FUNCTION: TOY2 0x00437000 [MATCHED]
		void Stop_T() { Stop(); }

		// FUNCTION: TOY2 0x00437010 [MATCHED]
		DWORD __cdecl ThreadProc()
		{
			DWORD hRet;

			g_streamThreadReady = 1;

			do
			{
				while (g_streamHandles.bufferNotifyEvent)
				{
					hRet = WaitForMultipleObjects(3, &g_streamHandles.bufferNotifyEvent, FALSE, INFINITE);

					if (hRet == WAIT_FAILED)
						break;

					switch (hRet - WAIT_OBJECT_0)
					{
						case 0:
							if (g_wiWave.bDonePlaying == TRUE)
							{
								Stop();
								ResetEvent(g_streamHandles.streamStopEvent);
							}
							else
							{
								FillBuffer();
							}
							break;

						case 1:
							Stop();
							break;

						case 2:
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
							ReleaseSemaphore(g_streamHandles.threadSemaphore, 1, NULL);
							break;
					}
				}
			} while (g_streamThreadReady);

			return TRUE;
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
				g_directSound->SetCooperativeLevel(D3DApp::g_windowData.mainHwnd, DSSCL_EXCLUSIVE);
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
	void SetVolumesProcessed(int32_t musicSliderIndex, int32_t sfxSliderIndex)
	{
		SetVolumes(2 * Toy2::g_musicSliderToAmplitude[musicSliderIndex] / 3, 3 * Toy2::g_soundSliderToAmplitude[sfxSliderIndex] / 2);
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
		WORD sampleRate = g_soundEffectSampleRates[soundIndex];
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
					goto LBL_SELECT_PLAYBACK_BUFFER;
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

		LBL_SELECT_PLAYBACK_BUFFER:
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

	// FUNCTION: TOY2 0x0047E7D0 [MATCHED]
	void ReleaseAllBuffers()
	{
		if (g_directSoundInit)
		{
			for (int32_t i = 0; i >= -767; --i)
			{
				LPDIRECTSOUNDBUFFER soundBuffer = g_soundEffectBuffers[i + 767];

				if (soundBuffer)
				{
					DWORD status;
					DWORD statusFlags;

					if (! g_directSoundInit)
					{
						statusFlags = 0;
					}
					else
					{
						g_lastDSoundError = soundBuffer->GetStatus(&status);
						statusFlags = status;
					}

					statusFlags &= DSBSTATUS_PLAYING;
					if (statusFlags == 1)
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
		if (effectId < 0)
		{
			int32_t player = Toy2::g_curSequencePlayer;
			Toy2::SoundSequencePlayer* sequence = &Toy2::g_soundSequencePlayers[player];

			sequence->pos.x = worldPos->x;
			sequence->pos.y = worldPos->y;
			sequence->pos.z = worldPos->z;
			sequence->scriptCursor = g_soundScripts[-effectId - 1];
			sequence->countdown = 0;

			int16_t* scriptCursor = sequence->scriptCursor;
			sequence->scriptCursor = scriptCursor + 3;

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
		else
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
						pitch += (8 * *g_randDatBufferPtr++) >> (randomShift & 31);
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
	}

	// FUNCTION: TOY2 0x0049E910
	void StartSoundSequenceOnActor(int32_t scriptIndex, Toy2::CreatureActor* actor)
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
		Toy2::SoundSequencePlayer* sequence = Toy2::g_soundSequencePlayers;

		do
		{
			if (sequence->scriptCursor)
			{
				sequence->countdown -= (uint16_t)Renderer::g_frameDelta;

				if ((int16_t)sequence->countdown < 0)
				{
					int16_t* scriptCursor = sequence->scriptCursor;
					int16_t command = *scriptCursor;

					while (command < 0)
					{
						if (command == -1)
							sequence->scriptCursor = 0;

						if (*scriptCursor != -2)
							goto LBL_NEXT_SEQUENCE;

						scriptCursor = sequence->scriptCursor - scriptCursor[1];
						sequence->scriptCursor = scriptCursor;
						command = *scriptCursor;
					}

					PlayOneShotSound3D(*scriptCursor - 1, scriptCursor[1], scriptCursor[2], &sequence->pos);
					sequence->countdown = scriptCursor[3];
					sequence->scriptCursor += 4;
				}
			}

		LBL_NEXT_SEQUENCE:
			++sequence;
		} while (sequence < &Toy2::g_soundSequencePlayers[8]);
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

		for (int32_t channel = 0; channel < 32; ++channel)
		{
			if (g_audioChannels[channel].bufferIndex != -1 && g_audioChannels[channel].timer != -1)
			{
				int16_t timer = g_audioChannels[channel].timer - 1;
				g_audioChannels[channel].timer = timer;

				if (timer <= 0)
				{
					if (ReassertChannelPlaying(g_audioChannels[channel].bufferIndex) == 1)
						g_audioChannels[channel].timer = 1;
					else
						g_audioChannels[channel].bufferIndex = -1;
				}
			}
		}
	}

	// FUNCTION: TOY2 0x004A3C80
	int32_t PlayOneShotSound3DActor(void* owner, int32_t soundIndex, int32_t pitch, int32_t volume, Vector3I* pos, int32_t unused)
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
	int32_t IsActorSoundPlaying(Toy2::BuzzActor* buzzActor)
	{
		if (g_directSoundInit)
		{
			for (int32_t index = 0; index < 32; ++index)
			{
				if (g_audioChannels[index].bufferIndex != -1 && g_audioChannels[index].timer == -1
					&& g_soundBufferStates[g_audioChannels[index].bufferIndex] == buzzActor)
				{
					if ((IsEffectPlaying(g_audioChannels[index].bufferIndex) & 1) != 0)
						return 1;

					g_audioChannels[index].bufferIndex = -1;
					break;
				}
			}
		}

		return 0;
	}

	namespace Preset
	{
		// FUNCTION: TOY2 0x0049EA60 [MATCHED]
		int32_t PlayOneShotSound2(int32_t soundIndex, Toy2::CreatureActor* owner)
		{
			SoundEffectPreset* soundPreset = &g_soundEffectPresets[soundIndex];

			return PlayOneShotSound3DActor(owner, soundPreset->soundBufferIndex - 1, soundPreset->pitch, soundPreset->volume, &owner->transform.pos, 0);
		}

		// FUNCTION: TOY2 0x0049EA90 [MATCHED]
		int32_t PlayOneShotSound(int32_t soundIndex, Toy2::CreatureActor* owner)
		{
			SoundEffectPreset* soundPreset = &g_soundEffectPresets[soundIndex];

			return PlayOneShotSound3DActor(owner, soundPreset->soundBufferIndex - 1, soundPreset->pitch, soundPreset->volume, &owner->transform.pos, 1);
		}
	}
}

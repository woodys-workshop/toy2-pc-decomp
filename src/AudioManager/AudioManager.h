#pragma once

#include "Common.h"
#include <windows.h>
#include <mmsystem.h>

struct IDirectSound;
struct IDirectSoundBuffer;
struct Vector3I;

namespace Toy2
{
	namespace Actor
	{
		struct Toy2Actor;
	}
}

namespace Toy2
{
	namespace Buzz
	{
		struct Toy2BuzzActor;
	}
}


namespace AudioManager
{
	extern int32_t g_curTrackIndex;

	void StopAndFlush();
	void Init();
	void SetVolumesProcessed(int32_t musicVolume, int32_t sfxVolume);
	void FlushSoundVoices();
	int32_t StopAndWait();
	void LoadSfxPackForLevel(int32_t levelId);
	int32_t PlayOneShotSoundGlobal(int32_t soundIndex, int32_t volume, int32_t leftVolume, int32_t rightVolume);
	void PlayMusicOneShot(int32_t trackIndex);
	void PlayMusicLooping(int16_t trackIndex);
	int32_t PlayLoopingSound3DPositional(void* owner, int32_t soundIndex, int32_t volume, int32_t leftVolume, void* unused, int16_t rightVolume);

	struct AudioConfig
	{
		WORD formatTag;
		WORD channels;
		DWORD sampleRate;
		DWORD avgBytesPerSec;
		WORD blockAlign;
		WORD bitsPerSample;
	};

	STATIC_ASSERT(sizeof(AudioConfig) == 0x10);

	struct SoundEffect
	{
		char** wavList;
		int32_t effectId;
	};

	STATIC_ASSERT(sizeof(SoundEffect) == 0x8);

	void ShutdownHandles();
	int32_t PlayTrackByIndex(int32_t index, int32_t Loop);
	void ThreadPlay(LPSTR fileName, bool looping);
	int32_t IsStreamActive();
	int32_t LoadFile(LPSTR fileName);
	void SignalThreadExit();
	void QueuePlay(const char* fileName, int32_t loopFlag);
	void FillBuffer();
	void LoadSoundPack(AudioManager::SoundEffect* sfxPack, const char* wavId);
	int32_t IsEffectPlaying(int32_t bufferIndex);
	BOOL ReassertChannelPlaying(int32_t bufferIndex);
	int32_t PlaySoundBuffer(int32_t soundIndex, int32_t leftVolume, int32_t rightVolume, void* owner, int32_t volume, int32_t loop);
	BOOL Enumerate(LPGUID lpGuid, LPCSTR l_description, LPCSTR lpcstrModule, LPVOID lpContext);
	int32_t CreateDirectSoundBuffer(IDirectSound* directSound, IDirectSoundBuffer** soundBuffer, DWORD bufferBytes);
	BOOL WriteToBuffer(IDirectSoundBuffer* soundBuffer, DWORD offset, char* sourceData, DWORD dataSize);
	void LoadSoundEffect(const char* name, int32_t effect, int32_t addLang);
	void ReleaseAllBuffers();
	void ReleaseBuffers();
	void SetSfxVolume(int32_t volume);
	void SetMusicVolume(int32_t volume);
	void PlaySoundEffect(int32_t effectId, Vector3I* worldPos);
	void StartSoundSequenceOnActor(int32_t scriptIndex, Toy2::Actor::Toy2Actor* actor);
	void ClearSequence7Cursor();
	void UpdateSoundSequences();
	int32_t PlayOneShotSound3D(int32_t soundIndex, int32_t volume, int32_t minVolume, Vector3I* worldPos);
	int32_t PlayLoopingSound3D(void* owner, int32_t soundIndex, int32_t volume, int32_t leftVolume, int16_t rightVolume);
	void ResetChannelsTable();
	void UpdateChannels();
	int32_t PlayOneShotSound3DActor(void* owner, int32_t soundIndex, int32_t pitch, int32_t volume, Vector3I* pos);
	int32_t IsActorSoundPlaying(Toy2::Buzz::Toy2BuzzActor* buzzActor);
	void SetVolumes(int32_t musicVolume, int32_t soundVolume);
	MMRESULT WaveReadFile(HMMIO hmmio, int32_t ckSize, uint8_t* buffer, LPMMCKINFO dataChunk, AudioManager::AudioConfig** outBytesRead);
	MMRESULT WaveLoadFile(LPSTR fileName, int32_t* dataSize, int32_t unused, AudioManager::AudioConfig** audioConfig, HMMIO* wavSourceData);

	namespace Stream
	{
		int32_t Init();
		void Stop();
		int32_t IsThreadReady();
		DWORD WINAPI ThreadProc(LPVOID context);
	}

	namespace Preset
	{
		int32_t PlayOneShotSound2(int32_t soundIndex, Toy2::Actor::Toy2Actor* owner);
		int32_t PlayOneShotSound(int32_t soundIndex, Toy2::Actor::Toy2Actor* owner);
	}

	namespace Wave
	{
		MMRESULT OpenFile(LPSTR fileName, HMMIO* outHmmio, LPWAVEFORMATEX** outWaveFormat, LPMMCKINFO outRiffChunk);
		MMRESULT SeekToChunk(HMMIO* musicHandle, LPMMCKINFO info, MMCKINFO* infoParent);
		int32_t CloseFile(HMMIO* hMmio, HGLOBAL* hGlobal);
	}
}
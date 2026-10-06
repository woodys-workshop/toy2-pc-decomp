#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Toy2/Actor.h"
#include "Toy2/Buzz.h"
#include <windows.h>

namespace AudioManager
{
	extern int32_t g_curTrackIndex;

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
	int32_t IsStreamActive();
	void SignalThreadExit();
	void ReleaseBuffers();
	void PlaySoundEffect(int32_t effectId, Vector3I* worldPos);
	void StartSoundSequenceOnActor(int32_t scriptIndex, Toy2::Actor::Toy2Actor* actor);
	void ClearSequence7Cursor();
	void UpdateSoundSequences();
	void UpdateChannels();
	int32_t PlayOneShotSound3DActor(void* owner, int32_t soundIndex, int32_t pitch, int32_t volume, Vector3I* pos);
	int32_t IsActorSoundPlaying(Toy2::Buzz::Toy2BuzzActor* buzzActor);
	void SetVolumes(int32_t musicVolume, int32_t soundVolume);

	STATIC_ASSERT(sizeof(AudioConfig) == 0x10);
	STATIC_ASSERT(sizeof(SoundEffect) == 0x8);

	namespace Preset
	{
		int32_t PlayOneShotSound2(int32_t soundIndex, Toy2::Actor::Toy2Actor* owner);
		int32_t PlayOneShotSound(int32_t soundIndex, Toy2::Actor::Toy2Actor* owner);
	}
}
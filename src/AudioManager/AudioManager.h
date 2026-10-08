#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Toy2/Actor.h"
#include "Toy2/Buzz.h"

#include <WINDOWS.H>

namespace AudioManager
{
	struct SoundEffectPreset
	{
		int16_t soundBufferIndex;
		int16_t pitch;
		int16_t volume;
		int16_t unkWord4;
		int16_t unkWord5;
		int16_t unkWord6;
		int16_t unkWord7;
		int16_t unkWord8;
	};

	extern int32_t g_curTrackIndex;
	extern int16_t g_pitchArray[14];
	extern SoundEffectPreset g_soundEffectPresets[218];

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
	void ReleaseBuffers();
	void PlaySoundEffect(int32_t effectId, Vector3I* worldPos);
	void StartSoundSequenceOnActor(int32_t scriptIndex, Toy2::Actor::Toy2Actor* actor);
	void UpdateSoundSequences();
	void UpdateChannels();
	int32_t PlayOneShotSound3DActor(void* owner, int32_t soundIndex, int32_t pitch, int32_t volume, Vector3I* pos);
	int32_t IsActorSoundPlaying(Toy2::Buzz::Toy2BuzzActor* buzzActor);
	void SetVolumes(int32_t musicVolume, int32_t soundVolume);

	STATIC_ASSERT(sizeof(SoundEffectPreset) == 0x10);

	namespace Preset
	{
		int32_t PlayOneShotSound2(int32_t soundIndex, Toy2::Actor::Toy2Actor* owner);
		int32_t PlayOneShotSound(int32_t soundIndex, Toy2::Actor::Toy2Actor* owner);
	}
}

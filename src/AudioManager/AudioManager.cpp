#include "AudioManager/AudioManager.h"
#include <mmsystem.h>
#include <DSOUND.H>

namespace AudioManager
{
	// GLOBAL: TOY2 0x005282CC
	int32_t g_curTrackIndex;

	// GLOBAL: TOY2 0x00830E3C
	int16_t g_pitchArray[14];

	// STUB: TOY2 0x0047D840
	void StopAndFlush() {}

	// STUB: TOY2 0x0047EDE0
	void Init() {}

	// STUB: TOY2 0x0049E8D0
	void SetVolumesProcessed(int32_t musicVolume, int32_t sfxVolume) {}

	// STUB: TOY2 0x004A3EF0
	void FlushSoundVoices() {}

	// STUB: TOY2 0x00436D40
	int32_t StopAndWait() { return 1; }

	// STUB: TOY2 0x0047EC20
	void LoadSfxPackForLevel(int32_t levelId) {}

	// STUB: TOY2 0x004A37E0
	int32_t PlayOneShotSoundGlobal(int32_t soundIndex, int32_t volume, int32_t leftVolume, int32_t rightVolume) { return 0; }

	// STUB: TOY2 0x0047D7F0
	void PlayMusicOneShot(int32_t trackIndex) {}

	// STUB: TOY2 0x0047D880
	void PlayMusicLooping(int16_t trackIndex) {}

	// STUB: TOY2 0x004A3B90
	int32_t PlayLoopingSound3DPositional(void* owner, int32_t soundIndex, int32_t volume, int32_t leftVolume, void* unused, int16_t rightVolume) { return 0; }

	// STUB: TOY2 0x00412EE0
	void ShutdownHandles() {}

	// STUB: TOY2 0x00413150
	int32_t PlayTrackByIndex(int32_t index, int32_t Loop) { return 0; }

	// STUB: TOY2 0x00413240
	void ThreadPlay(LPSTR fileName, bool looping) {}

	// STUB: TOY2 0x00413300
	int32_t IsStreamActive() { return 0; }

	// STUB: TOY2 0x00413310
	int32_t LoadFile(LPSTR fileName) { return 0; }

	// STUB: TOY2 0x00436C90
	void SignalThreadExit() {}

	// STUB: TOY2 0x00436CE0
	void QueuePlay(const char* fileName, int32_t loopFlag) {}

	// STUB: TOY2 0x00436D80
	void FillBuffer() {}

	// STUB: TOY2 0x0047D670
	void LoadSoundPack(AudioManager::SoundEffect* sfxPack, const char* wavId) {}

	// STUB: TOY2 0x0047D8E0
	int32_t IsEffectPlaying(int32_t bufferIndex) { return 0; }

	// STUB: TOY2 0x0047D930
	BOOL ReassertChannelPlaying(int32_t bufferIndex) { return 0; }

	// STUB: TOY2 0x0047DE50
	int32_t PlaySoundBuffer(int32_t soundIndex, int32_t leftVolume, int32_t rightVolume, void* owner, int32_t volume, int32_t loop) { return 0; }

	// STUB: TOY2 0x0047E3C0
	BOOL Enumerate(LPGUID lpGuid, LPCSTR l_description, LPCSTR lpcstrModule, LPVOID lpContext) { return 0; }

	// STUB: TOY2 0x0047E420
	int32_t CreateDirectSoundBuffer(IDirectSound* directSound, IDirectSoundBuffer** soundBuffer, DWORD bufferBytes) { return 0; }

	// STUB: TOY2 0x0047E4E0
	BOOL WriteToBuffer(IDirectSoundBuffer* soundBuffer, DWORD offset, char* sourceData, DWORD dataSize) { return 0; }

	// STUB: TOY2 0x0047E5B0
	void LoadSoundEffect(const char* name, int32_t effect, int32_t addLang) {}

	// STUB: TOY2 0x0047E7D0
	void ReleaseAllBuffers() {}

	// STUB: TOY2 0x0047E850
	void ReleaseBuffers() {}

	// STUB: TOY2 0x0049AE20
	void SetSfxVolume(int32_t volume) {}

	// STUB: TOY2 0x0049AE40
	void SetMusicVolume(int32_t volume) {}

	// STUB: TOY2 0x0049E660
	void PlaySoundEffect(int32_t effectId, Vector3I* worldPos) {}

	// STUB: TOY2 0x0049E910
	void StartSoundSequenceOnActor(int32_t scriptIndex, Toy2::Actor::Toy2Actor* actor) {}

	// STUB: TOY2 0x0049E9C0
	void ClearSequence7Cursor() {}

	// STUB: TOY2 0x0049E9D0
	void UpdateSoundSequences() {}

	// STUB: TOY2 0x004A3810
	int32_t PlayOneShotSound3D(int32_t soundIndex, int32_t volume, int32_t minVolume, Vector3I* worldPos) { return 0; }

	// STUB: TOY2 0x004A3980
	int32_t PlayLoopingSound3D(void* owner, int32_t soundIndex, int32_t volume, int32_t leftVolume, int16_t rightVolume) { return 0; }

	// STUB: TOY2 0x004A3BC0
	void ResetChannelsTable() {}

	// STUB: TOY2 0x004A3BE0
	void UpdateChannels() {}

	// STUB: TOY2 0x004A3C80
	int32_t PlayOneShotSound3DActor(void* owner, int32_t soundIndex, int32_t pitch, int32_t volume, Vector3I* pos) { return 0; }

	// STUB: TOY2 0x004A3E60
	int32_t IsActorSoundPlaying(Toy2::Buzz::Toy2BuzzActor* buzzActor) { return 0; }

	// STUB: TOY2 0x004A3ED0
	void SetVolumes(int32_t musicVolume, int32_t soundVolume) {}

	// STUB: TOY2 0x004A4130
	MMRESULT WaveReadFile(HMMIO hmmio, int32_t ckSize, uint8_t* buffer, LPMMCKINFO dataChunk, AudioManager::AudioConfig** outBytesRead) { return 0; }

	// STUB: TOY2 0x004A4680
	MMRESULT WaveLoadFile(LPSTR fileName, int32_t* dataSize, int32_t unused, AudioManager::AudioConfig** audioConfig, HMMIO* wavSourceData) { return 0; }

	namespace Stream
	{
		// STUB: TOY2 0x00412FF0
		int32_t Init() { return 0; }

		// STUB: TOY2 0x004132A0
		void Stop() {}

		// STUB: TOY2 0x00436CC0
		int32_t IsThreadReady() { return 0; }

		// STUB: TOY2 0x00437010
		DWORD WINAPI ThreadProc(LPVOID context) { return 0; }
	}

	namespace Preset
	{
		// STUB: TOY2 0x0049EA60
		int32_t PlayOneShotSound2(int32_t soundIndex, Toy2::Actor::Toy2Actor* owner) { return 0; }

		// STUB: TOY2 0x0049EA90
		int32_t PlayOneShotSound(int32_t soundIndex, Toy2::Actor::Toy2Actor* owner) { return 0; }
	}

	namespace Wave
	{
		// STUB: TOY2 0x004A3F10
		MMRESULT OpenFile(LPSTR fileName, HMMIO* outHmmio, LPWAVEFORMATEX** outWaveFormat, LPMMCKINFO outRiffChunk) { return 0; }

		// STUB: TOY2 0x004A40F0
		MMRESULT SeekToChunk(HMMIO* musicHandle, LPMMCKINFO info, MMCKINFO* infoParent) { return 0; }

		// STUB: TOY2 0x004A4200
		int32_t CloseFile(HMMIO* hMmio, HGLOBAL* hGlobal) { return 0; }
	}
}
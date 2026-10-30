#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Toy2/Buzz.h"
#include "Toy2/Actor.h"
#include "Nu3D/Particles.h"

namespace Toy2
{
	struct ToyCfg
	{
		uint32_t flags;
		int32_t detail;
		float gammaCorrection;
		int32_t driverIndex;
		int32_t deviceIndex;
		int32_t displayModeIndex;
	};

	// TODO: Perhaps it should be somewhere else?
	struct ViewportPreset
	{
		float viewportLR;
		float viewportTB;
		float farLayerNearZ;
		float nearZ;
		float nearDistance;
		float farDistance;
	};

	struct SectorBackdropTexIdTable
	{
		int32_t primary[9];
		int32_t secondary[9];
	};

	struct SaveNameBuffer
	{
		char name[256];
	};

	enum PauseMenuType
	{
		MENU_MAIN = 0,
		MENU_CAMERA = 1,
		MENU_VOLUME_CONTROL = 2,
		MENU_AREYOUSURE = 3,
		MENU_SHORTENEDPAUSE = 4,
	};

	struct TickAccumulators
	{
		uint8_t pad0;
		uint8_t pad1;
		uint8_t tick2Acc;
		uint8_t tick3Acc;
		uint8_t tick4Acc;
		uint8_t tick5Acc;
		uint8_t tick6Acc;
		uint8_t tick7Acc;
		uint8_t tick8Acc;
		uint8_t tick16Acc;
		uint8_t tick32Acc;
		uint8_t tick64Acc;
	};

	struct SoundSequencePlayer
	{
		Vector3I pos;
		int16_t* scriptCursor;
		uint16_t countdown;
		uint16_t pad;
	};

	struct SpawnLocation
	{
		int32_t x;
		int32_t y;
		int32_t z;
		int32_t rotation;
	};

	struct BeamShot
	{
		Vector3I headPos;
		Vector3I originPos;
		Vector3I velocity;
		int16_t lifeTimer;
		int16_t moveTimer;
		uint8_t tintB;
		uint8_t tintG;
		uint8_t tintR;
		uint8_t halfWidth;
	};

	void SetBackdropByIndex(int32_t index);
	void ProcessMiscEventsEx();
	void RenderGame(int32_t fullRender);
	void RenderHUD();
	uint32_t ComputeTokenProgress();
	int32_t EnterLevel(int32_t levelIndex);
	void InterpolatePathPoint(int32_t index, int32_t path, Vector3I* vec);
	void ResetBuzzState();
	void InitZoneData();
	void ShowModelNode(int32_t creatureIndex, int32_t nodeIdx);
	void HideModelNode(int32_t creatureIndex, int32_t nodeIdx);
	void ProcessMiscEvents();
	int32_t ShowControlConfig();
	void ShowGraphicsConfig();
	void PlayLevelMusic();
	void ActivateGravityBoots();
	void RespawnCosmicShield();
	Toy2::BeamShot* SpawnBeamShot(
		int32_t beamType,
		int16_t aimYaw,
		int16_t aimPitch,
		Transform* origin,
		Transform* headOffset,
		int32_t range,
		Toy2::Actor::Toy2Actor* autoAimActor);
	int32_t ShowTokenSparkle(int32_t linkId);
	void CheckForQuit();

	extern ToyCfg g_toyCfgData;
	extern ViewportPreset g_viewportPresets[3];
	extern SpawnLocation g_spawnLocations[17];
	extern Actor::Toy2Actor g_levelActors[64];
	extern int32_t g_levelFileIndex;
	extern int32_t g_isElevatorHopLevel;
	extern int32_t g_hasBackdrop;
	extern int32_t g_mainMenuState;
	extern int32_t g_attractModeTimer;
	extern int32_t g_returnedToTitle;
	extern int32_t g_saveLoaded;
	extern int32_t g_showBlackFrames;
	extern int32_t g_curSectorCopy1;
	extern int32_t g_curSectorCopy2;
	extern int32_t g_unk52B7E0;
	extern int32_t g_demoMode;
	extern int32_t g_hasStaticBackdrop;
	extern int32_t g_nextBackdropId;
	extern int16_t g_levelIndex;
	extern int32_t g_levelFileConversion[16];
	extern Buzz::Toy2BuzzActor g_buzzActor;
	extern int16_t g_levelTransition;
	extern int16_t g_levelTransitionTimer;
	extern int16_t g_feedbackTimerA;
	extern int16_t g_feedbackTimerB;
	extern int16_t g_feedbackTimerA_sub;
	extern int16_t g_hudWidgetTimers[11];
	extern int16_t g_hudWidgetPositions[12];
	extern SoundSequencePlayer g_soundSequencePlayers[8];
	extern int32_t g_curSequencePlayer;
	extern TickAccumulators g_tickAccumulators;
	extern BeamShot g_beamShots[4];
	extern Levels::Type63* g_cosmicShieldPickup;
	extern int32_t g_cosmicShieldPickupYPos;
	extern int32_t g_cosmicShieldRotYaw;
	extern int32_t g_cosmicShieldSpin;
	extern Levels::Type63* g_rocketBootsPickup;
	extern int32_t g_rocketBootsPickupY;
	extern int32_t g_rocketBootsDuration;
	extern int32_t g_gravityBootsDuration;
	extern int32_t g_gravityBootsHoverHeight;
	extern int32_t g_grappleTimer;
	extern int32_t g_grappleState;
	extern int32_t g_grappleDistance;
	extern int32_t g_grappleProgress;
	extern Transform g_grappleTargetPos;
	extern int32_t g_discLauncherTimer;
	extern int32_t g_discLauncherAmmo;
	extern int32_t g_unk50A0BC;
	extern int32_t g_unk50A0B0;
	extern uint16_t g_playerUnlocks;
	extern int16_t g_musicSliderToAmplitude[16];
	extern int16_t g_soundSliderToAmplitude[16];
	extern char g_emptyStr[8];
	extern int32_t g_unk830CC4;
	extern int32_t g_unk830C90;
	extern int32_t g_unk830C94;
	extern int32_t g_unk830D4C;
	extern int32_t g_collectionCount;
	extern int32_t g_unk52F2F8;
	extern Nu3D::Particles::ParticleInstance* g_unk830E2C;
	extern uint8_t g_underwaterTintBlue;
	extern uint8_t g_underwaterTintGreen;
	extern uint8_t g_underwaterTintRed;
	extern int32_t g_unk52AD64;
	extern int32_t g_unk830D44;
	extern uint8_t g_type59VisibleIndices[36];
	extern uint8_t g_tickState[12];

	STATIC_ASSERT(sizeof(ToyCfg) == 0x18);
	STATIC_ASSERT(sizeof(ViewportPreset) == 0x18);
	STATIC_ASSERT(sizeof(SectorBackdropTexIdTable) == 0x48);
	STATIC_ASSERT(sizeof(TickAccumulators) == 0xC);
	STATIC_ASSERT(sizeof(SoundSequencePlayer) == 0x14);
	STATIC_ASSERT(sizeof(SpawnLocation) == 0x10);
	STATIC_ASSERT(sizeof(BeamShot) == 0x2C);
	STATIC_ASSERT(sizeof(SaveNameBuffer) == 0x100);

	namespace Ini
	{
		struct KeyNameEntry
		{
			uint16_t scancode;
			uint16_t unk2;
			uint16_t keyName;
		};

		extern KeyNameEntry g_keyNameTable[123];
		STATIC_ASSERT(sizeof(KeyNameEntry) == 6);
	}

	namespace Game
	{
		void MainLoop();
		void InitActor(Toy2::Actor::Toy2Actor* actor, int32_t initialSpawn);
	}
}

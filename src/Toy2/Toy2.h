#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Toy2/Actor.h"


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
		int32_t primary[10];
		int32_t secondary[8];
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
	uint32_t ComputeTokenProgress();
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
	Toy2::BeamShot* SpawnBeamShot(int32_t beamType, int16_t aimYaw, int16_t aimPitch, WorldPos* origin, WorldPos* headOffset, int32_t range, Toy2::Actor::Toy2Actor* autoAimActor);
	int32_t ShowTokenSparkle();

	extern ToyCfg g_toyCfgData;
	extern ViewportPreset g_viewportPresets[3];
	extern int32_t g_levelFileIndex;
	extern int32_t g_isElevatorHopLevel;
	extern int32_t g_hasBackdrop;
	extern int32_t g_mainMenuState;
	extern int32_t g_attractModeTimer;
	extern int32_t g_returnedToTitle;
	extern int32_t g_saveLoaded;
	extern int32_t g_showBlackFrames;
	extern int32_t g_demoMode;
	extern int32_t g_hasStaticBackdrop;
	extern int32_t g_renderGame;
	extern int32_t g_curSector;
	extern int32_t g_curSectorCopy1;
	extern int32_t g_curSectorCopy2;
	extern int16_t g_pickupAnimTimer;
	extern uint8_t g_type59VisibleIndices[36];
	extern int32_t g_nextBackdropId;
	extern int16_t g_levelIndex;
	extern int32_t g_levelFileConversion[16];

	STATIC_ASSERT(sizeof(ToyCfg) == 0x18);
	STATIC_ASSERT(sizeof(ViewportPreset) == 0x18);
	STATIC_ASSERT(sizeof(BeamShot) == 0x2C);

	namespace Game
	{
		void InitActor(Toy2::Actor::Toy2Actor* actor, int32_t initialSpawn);
	}
}

#pragma once

#include "Common.h"

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

	void SetBackdropByIndex(int32_t index);
	void ProcessMiscEventsEx();
	void RenderGame(int32_t fullRender);
	uint32_t ComputeTokenProgress();

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
}

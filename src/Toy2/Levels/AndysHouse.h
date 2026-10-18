#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Toy2/Collectables.h"

namespace Toy2
{
	namespace AndysHouse
	{
		extern int16_t g_unk4F0EDC[6];
		extern Collectables::TokenTableEntry g_tutorialHints[10];
		extern int16_t g_unk4F0F90[24];
		extern int32_t g_sparkPathIndex;
		extern int32_t g_sparkPresetIndex;
		extern int32_t g_sparkPresetTimer;
		extern int32_t g_unk52F4F4;
		extern int32_t g_unk52F568;
		extern int32_t g_link14Rotation;
		extern int32_t g_liftVelocity;
		extern int32_t g_atticTilt;
		extern int32_t g_tinRobotLastPhase;
		extern int32_t g_unk52F4FC;
		extern int32_t g_unk52F5AC;
		extern int32_t g_inTinRobotFight;
		extern int32_t g_raceCarPathIndex;
		extern int32_t g_raceCarLaps;
		extern int32_t g_raceBuzzLastSide;
		extern int32_t g_unk52F580;
		extern int32_t g_unk52F574;
		extern int32_t g_unk52F570;
		extern int32_t g_unk52F56C;
		extern int32_t g_unk52F57C;
		extern int32_t g_unk52F594;
		extern int32_t g_potatoHeadBlitAngle;
		extern int32_t g_unk52F500;
		extern int32_t g_unk52F59C;
		extern Vector3I g_liftPos;
		extern int32_t g_liftMaxY;
		extern int32_t g_unk52F530;
		extern Transform g_link9Pos;
		extern Transform g_link17Pos;
		extern Transform g_link18Pos;
		extern Transform g_link10Pos;
		extern Transform g_link16Pos;
		extern int32_t g_tinRobotFlashTimer;
		extern int32_t g_tinRobotFlash;

		void Interactions();
	}
}

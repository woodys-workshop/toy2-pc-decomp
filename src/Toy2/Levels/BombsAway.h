#pragma once

#include "Common.h"
#include "Numerics.h"

namespace Toy2
{
	namespace BombsAway
	{
		extern int32_t g_damageLevel;
		extern int32_t g_dropY;
		extern int32_t g_wobblePhase;
		extern int32_t g_dropVelocity;
		extern int32_t g_flashSoundDelay;
		extern int32_t g_currentSize;
		extern int32_t g_flashTimer;
		extern int32_t g_prevCutsceneTimer;
		extern int32_t g_hitFlashTimer;
		extern int32_t g_unusedInt;
		extern int32_t g_introSoundTimer;
		extern int32_t g_tauntTimer;
		extern int32_t g_textureScrollV;
		extern int32_t g_textureScrollU;
		extern int32_t g_savedRecordY;
		extern int32_t g_tintPhaseR;
		extern int32_t g_tintPhaseB;
		extern int32_t g_tintPhaseG;
		extern int32_t g_bossState;
		extern int32_t g_hitCutsceneLength;
		extern int32_t g_targetSize;
		extern int32_t g_lastBossPhase;

		void Interactions();

	}
}

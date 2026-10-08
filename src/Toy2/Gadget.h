#pragma once

#include "Common.h"

namespace Toy2
{
	namespace Gadget
	{
		enum PlayerUnlocks
		{
			UNLOCK_SHIELD = 0x1,
			UNLOCK_ROCKET = 0x2,
			UNLOCK_DISC = 0x4,
			UNLOCK_HOVER = 0x8,
			UNLOCK_GRAPPLE = 0x10,
		};

		extern int16_t g_unk503A1E[19];
		extern uint8_t g_shieldUnlockTable[16];
		extern uint8_t g_rocketUnlockTable[80];
		extern uint8_t g_discUnlockTable[20];
		extern uint8_t g_hoverUnlockTable[16];
		extern uint8_t g_grappleUnlockTable[12];
		extern int32_t g_unk830D1C;
		extern int32_t g_unk830C98;
		extern int32_t g_periodicHintSoundTimer;
		extern int32_t g_unk830D34;
		extern int32_t g_unk830D24;
		extern int32_t g_unk830CA4;
		extern int32_t g_unk830D48;

		void InitLevelUnlockGeometry();
		void ApplyUnlockToGeometry(uint8_t* table, int32_t showScale);
	}
}

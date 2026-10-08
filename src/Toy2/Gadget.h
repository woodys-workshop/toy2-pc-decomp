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

		extern int32_t g_unk830D1C;
		extern int32_t g_unk830D48;

		void InitLevelUnlockGeometry();
		void ApplyUnlockToGeometry(uint8_t* table, int32_t showScale);
	}
}

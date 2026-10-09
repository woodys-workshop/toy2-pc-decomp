#pragma once

#include "Common.h"

namespace Toy2
{
	namespace AirportInfiltration
	{
		// the y position of a hidden pickup and the one it is reset to when it shows up
		struct HiddenCollectible
		{
			int32_t* yPtr;
			int32_t savedY;
		};

		STATIC_ASSERT(sizeof(HiddenCollectible) == 0x8);

		void InitHiddenCollectibles();
		void Interactions();

		extern int32_t g_unk52FEE8;
		extern int32_t g_unk52FF1C;
		extern int32_t g_unk52FF2C;
		extern int32_t g_unk52FF30;
		extern int32_t g_unk52FF3C;
		extern int32_t g_unk52FF44;

		extern Vector3I g_unk4F4668[5];
		extern char* g_unk4F46AC[5];
		extern int32_t g_unk52FE38;
		extern int32_t g_unk52FE3C;
		extern int32_t g_unk52FE40;
		extern HiddenCollectible g_unk52FEEC[5];
		extern int32_t g_unk52FF18;
		extern int32_t g_unk52FF20;
		extern int32_t g_unk52FF24;
		extern int32_t g_unk52FF28;
		extern int32_t g_unk52FF34;
		extern int32_t g_unk52FF38;
	}
}

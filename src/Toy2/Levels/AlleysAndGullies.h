#pragma once

#include "Common.h"
#include "Numerics.h"

namespace Toy2
{
	namespace AlleysAndGullies
	{
		// the y position of a hidden pickup and the one it is reset to when it shows up
		struct HiddenCollectible
		{
			int32_t* yPtr;
			int32_t savedY;
		};

		STATIC_ASSERT(sizeof(HiddenCollectible) == 0x8);

		void Init();
		void Interactions();

		extern int32_t g_unk52F8B0;
		extern int32_t g_unk52F8B8;
		extern int32_t g_unk52F8E8;
		extern int32_t g_unk52F8FC;

		extern int16_t g_unk4F215C[14];
		extern int16_t g_unk4F2178[6];
		extern char* g_unk4F2184[5];
		extern int32_t g_unk52F8A8;
		extern int32_t g_unk52F8AC;
		extern int32_t g_unk52F8B4;
		extern int32_t g_unk52F8C0;
		extern int32_t g_unk52F8C4;
		extern int32_t g_unk52F8C8;
		extern int32_t g_unk52F8D0;
		extern int32_t g_unk52F8D4;
		extern int32_t g_unk52F8D8;
		extern int32_t g_unk52F8DC;
		extern int32_t g_unk52F8E0;
		extern int32_t g_unk52F8EC;
		extern int32_t g_unk52F8F0;
		extern int32_t g_unk52F8F4;
		extern int32_t g_unk52F8F8;
		extern int32_t g_unk52F900;
		extern int32_t g_unk52F904;
		extern int32_t g_unk52F908;
		extern int32_t g_unk52F90C;
		extern Vector3I g_unk52F910;
		extern int32_t g_unk52F91C;
		extern Vector3I g_unk52F920;
		extern int32_t g_unk52F92C;
		extern int32_t g_unk52F930;
		extern int32_t g_unk52F934;
		extern int32_t g_unk52F938;
		extern int32_t g_unk52F93C;
		extern int32_t g_unk52F940;
		extern int32_t g_unk52F944;
		extern int32_t g_unk52F948;
		extern int32_t g_unk52F94C;
		extern int32_t g_unk52F950;
		extern HiddenCollectible g_unk52F954[5];
		extern int32_t g_unk52F97C;
		extern int32_t g_unk52F980;
		extern int32_t g_unk52F984;
		extern int32_t g_unk52F988;
	}
}

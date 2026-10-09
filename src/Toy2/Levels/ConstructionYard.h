#pragma once

#include "Common.h"
#include "Toy2/Collectables.h"
#include "Numerics.h"

namespace Toy2
{
	namespace ConstructionYard
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

		extern int32_t g_unk4F1CCC[4];
		extern int32_t g_unk4F1CDC[4];
		extern int32_t g_unk4F1CEC[4];
		extern int32_t g_unk4F1CFC[4];
		extern int32_t g_unk52F798;
		extern int32_t g_unk52F7CC;
		extern int32_t g_unk52F804;
		extern int32_t g_unk52F864;
		extern int32_t g_unk52F884;

		extern int32_t g_unk4F1B64[16];
		extern int32_t g_unk4F1BA4[9];
		extern int32_t g_unk4F1BC8[11];
		extern int32_t g_unk4F1BF4[10];
		extern int32_t g_unk4F1C1C[9];
		extern int32_t g_unk4F1C40[9];
		extern int32_t g_unk4F1C64[9];
		extern int16_t g_unk4F1C88[8];
		extern int16_t g_unk4F1C98[6];
		extern Collectables::TokenTableEntry g_unk4F1CA4[2];
		extern char* g_unk4F1CB8[5];
		extern int32_t g_unk52F2F8;
		extern int32_t g_unk52F760;
		extern int32_t g_unk52F764;
		extern int32_t g_unk52F768;
		extern int32_t g_unk52F76C;
		extern int32_t g_unk52F770;
		extern int32_t g_unk52F774;
		extern int32_t g_unk52F778;
		extern int32_t g_unk52F77C;
		extern int32_t g_unk52F780;
		extern int32_t g_unk52F784;
		extern int32_t g_unk52F788;
		extern int32_t g_unk52F78C;
		extern int32_t g_unk52F790;
		extern int32_t g_unk52F794;
		extern int32_t g_unk52F79C;
		extern int32_t g_unk52F7A0;
		extern int32_t g_unk52F7A4;
		extern int32_t g_unk52F7A8;
		extern int32_t g_unk52F7AC;
		extern int32_t g_unk52F7B0;
		extern int32_t g_unk52F7B4;
		extern int32_t g_unk52F7B8;
		extern int32_t g_unk52F7BC;
		extern int32_t g_unk52F7C0;
		extern int32_t g_unk52F7C4;
		extern int32_t g_unk52F7C8;
		extern int32_t g_unk52F7D0;
		extern int32_t g_unk52F7D4;
		extern int32_t g_unk52F7D8;
		extern int32_t g_unk52F7DC;
		extern int32_t g_unk52F7E0;
		extern int32_t g_unk52F7E4;
		extern int32_t g_unk52F7E8;
		extern int32_t g_unk52F7EC;
		extern int32_t g_unk52F7F0;
		extern int32_t g_unk52F7F4;
		extern int32_t g_unk52F7F8;
		extern int32_t g_unk52F7FC;
		extern int32_t g_unk52F800;
		extern int32_t g_unk52F808;
		extern int32_t g_unk52F80C;
		extern int32_t g_unk52F810;
		extern int32_t g_unk52F83C;
		extern int32_t g_unk52F840;
		extern int32_t g_unk52F844;
		extern int32_t g_unk52F848;
		extern int32_t g_unk52F850;
		extern int32_t g_unk52F854;
		extern int32_t g_unk52F858;
		extern int32_t g_unk52F85C;
		extern int32_t g_unk52F860;
		extern int32_t g_unk52F868;
		extern int32_t g_unk52F86C;
		extern int32_t* g_unk52F870;
		extern int32_t* g_unk52F874;
		extern int32_t* g_unk52F878;
		extern int32_t* g_unk52F87C;
		extern int32_t g_unk52F880;
		extern int32_t g_unk52F888;
		extern int32_t g_unk52F88C;
		extern int32_t g_unk52F890;
		extern int32_t* g_unk52F894;
		extern int32_t* g_unk52F898;
		extern int32_t* g_unk52F89C;
		extern int32_t* g_unk52F8A0;
		extern int32_t g_unk52F8A4;
		extern HiddenCollectible g_unk52F814[5];
	}
}

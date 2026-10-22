#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Toy2/Collectables.h"

namespace Toy2
{
	namespace AlsPenthouse
	{
		// the y position of a hidden pickup and the one it is reset to when it shows up
		struct HiddenCollectible
		{
			int32_t* yPtr;
			int32_t savedY;
		};

		STATIC_ASSERT(sizeof(HiddenCollectible) == 0x8);

		struct SwitchEntry
		{
			int16_t mask;
			int16_t linkA;
			int16_t linkB;
			int16_t rotLinkId;
			int16_t rotX;
			int16_t rotY;
			int16_t rotZ;
		};

		STATIC_ASSERT(sizeof(SwitchEntry) == 0xE);

		void Method2(int32_t a1);
		void Method8();
		int32_t Method6(int32_t a1);
		void InitHiddenCollectibles();
		void Interactions();

		extern int32_t g_unk52FD10;
		extern int32_t g_unk52FD2C;
		extern int32_t g_unk52FD40;
		extern int32_t g_unk52FD70;

		extern int16_t g_unk4F3E00[48];
		extern int16_t g_unk4F3E60[10];
		extern int32_t g_unk4F3E74[24];
		extern int32_t g_unk4F3ED4[12];
		extern char* g_unk4F3F04[5];
		extern int16_t g_unk4F3F18[6];
		extern Collectables::TokenTableEntry g_unk4F3F24[3];
		extern SwitchEntry g_unk4F3F48[7];
		extern uint8_t g_unk4F3FAC[8];
		extern uint8_t g_unk4F3FB4[24];
		extern uint8_t g_unk4F3FCC[16];
		extern uint8_t g_unk4F3FDC[12];
		extern int32_t g_unk4F3FE8[15];
		extern uint8_t g_unk4F4024[33];
		extern int32_t g_unk52FD08;
		extern int32_t g_unk52FD0C;
		extern int32_t g_unk52FD14;
		extern int32_t g_unk52FD18;
		extern int32_t g_unk52FD1C;
		extern int32_t g_unk52FD20;
		extern int32_t g_unk52FD28;
		extern int32_t g_unk52FD30;
		extern int32_t g_unk52FD34;
		extern int32_t g_unk52FD38;
		extern int32_t g_unk52FD3C;
		extern int32_t g_unk52FD44;
		extern int32_t g_unk52FD48;
		extern int32_t g_unk52FD4C;
		extern int32_t g_unk52FD50;
		extern int32_t g_unk52FD54;
		extern int32_t g_unk52FD74;
		extern int32_t g_unk52FD78;
		extern int32_t g_unk52FD7C;
		extern int32_t g_unk52FD80;
		extern int32_t g_unk52FD84;
		extern int32_t g_unk52FD88;
		extern int32_t g_unk52FD8C;
		extern int32_t g_unk52FD90;
		extern int32_t g_unk52FDA8;
		extern int32_t g_unk52FDAC;
		extern int32_t g_unk52FDB0;
		extern int32_t g_unk52FDB4;
		extern int32_t g_unk52FDB8;
		extern int32_t g_unk52FDBC;
		extern int32_t g_unk52FDC0;
		extern int32_t g_unk52FDC4;
		extern int32_t g_unk52FDF0;
		extern int32_t g_unk52FDF4;
		extern int32_t g_unk52FDF8;
		extern int32_t g_unk52FDFC;
		extern int32_t g_unk52FD58[6];
		extern Vector3I g_unk52FD98;
		extern HiddenCollectible g_unk52FDC8[5];
	}
}

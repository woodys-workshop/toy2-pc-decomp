#include "Numerics.h"
#include "Toy2/Levels/ElevatorHop.h"

namespace Toy2
{
	namespace ElevatorHop
	{
		// GLOBAL: TOY2 0x004F3614
		int16_t g_unk4F3614[9] = { 1, 2, 3, 6, 7, 8, 9, 10, -1 };

		// GLOBAL: TOY2 0x004F3628
		int16_t g_unk4F3628[9] = { 0, 11, 12, 13, 14, 15, 16, 17, -1 };

		// GLOBAL: TOY2 0x004F3688
		int32_t g_unk4F3688[21] = {
			65544,	65545, 131080, 196612,	1179648, 131081, 65544, 65545,	 8323075, 524384, 589825,
			524289, 65538, 0,	   1179648, 131081,	 65544,	 65545, 8323075, 32,	  39
		};

		// GLOBAL: TOY2 0x004F36DC
		int32_t g_unk4F36DC[41] = {
			16777224, 16777225, 33554440, 196612,	786433,	 33554441, 16777224, 16777225, 8323075, 524320,	 590080, 524544,  66048, 0,
			786432,	  33554441, 16777224, 16777225, 8323075, 32,	   39,		 -271682,  -64262,	-1024,	 -1536,	 -271682, -1094, -1536,
			1536,	  -384898,	-980742,  -512,		1536,	 -320066,  -64326,	 -1536,	   1024,	-271682, -1094,	 -1024,	  -1024
		};

		// GLOBAL: TOY2 0x004F3798
		int16_t g_unk4F3798[6] = { 107, 108, 110, 106, 109, 0 };

		// GLOBAL: TOY2 0x004F37A4
		Collectables::TokenTableEntry g_unk4F37A4[4] = {
			{ 112, 34, "if you line up the ^wire^ you will open the ^door^ and activate the ^elevators^.", 0 },
			{ 113, 35, "the switch in the control room will activate this ^shortcut^ fan.", 0 },
			{ 114, 36, "the switch at the top of the elevator shaft will activate this ^shortcut^ fan.", 0 },
			{ -1, 0, 0, 0 },
		};

		// GLOBAL: TOY2 0x0052FC78
		int32_t g_unk52FC78[10];

		// GLOBAL: TOY2 0x0052FCA0
		int32_t g_unk52FCA0[10];

		// GLOBAL: TOY2 0x0052FC10
		int32_t g_unk52FC10;

		// GLOBAL: TOY2 0x0052FCDC
		int32_t g_unk52FCDC;

		// GLOBAL: TOY2 0x0052FC44
		int32_t g_unk52FC44;

		// GLOBAL: TOY2 0x0052FCC8
		int32_t g_unk52FCC8;

		// GLOBAL: TOY2 0x0052FC1C
		int32_t g_unk52FC1C;

		// GLOBAL: TOY2 0x0052FC28
		int32_t g_unk52FC28;

		// GLOBAL: TOY2 0x0052FC38
		int32_t g_unk52FC38;

		// GLOBAL: TOY2 0x0052FD00
		int32_t g_unk52FD00;

		// GLOBAL: TOY2 0x0052FC54
		int32_t g_unk52FC54;

		// GLOBAL: TOY2 0x0052FC6C
		int32_t g_unk52FC6C;

		// GLOBAL: TOY2 0x0052FC2C
		int32_t g_unk52FC2C;

		// GLOBAL: TOY2 0x0052FC58
		int32_t g_unk52FC58;

		// GLOBAL: TOY2 0x0052FC64
		int32_t g_unk52FC64;

		// GLOBAL: TOY2 0x0052FC74
		int32_t g_unk52FC74;

		// GLOBAL: TOY2 0x0052FC60
		int32_t g_unk52FC60;

		// GLOBAL: TOY2 0x0052FC70
		int32_t g_unk52FC70;

		// GLOBAL: TOY2 0x0052FCFC
		int32_t g_unk52FCFC;

		// GLOBAL: TOY2 0x0052FC18
		int32_t g_unk52FC18;

		// GLOBAL: TOY2 0x0052FC68
		int32_t g_unk52FC68;

		// GLOBAL: TOY2 0x0052FCEC
		int32_t g_unk52FCEC;

		// GLOBAL: TOY2 0x0052FCD0
		int32_t g_unk52FCD0;

		// GLOBAL: TOY2 0x0052FC14
		int32_t g_unk52FC14;

		// GLOBAL: TOY2 0x0052FC50
		int32_t g_unk52FC50;

		// GLOBAL: TOY2 0x0052FC30
		int32_t g_unk52FC30;

		// GLOBAL: TOY2 0x0052FCD8
		int32_t g_unk52FCD8;

		// GLOBAL: TOY2 0x0052FC24
		int32_t g_unk52FC24;

		// GLOBAL: TOY2 0x0052FCCC
		int32_t g_unk52FCCC;

		// GLOBAL: TOY2 0x0052FCE8
		int32_t g_unk52FCE8;

		// GLOBAL: TOY2 0x0052FCD4
		int32_t g_unk52FCD4;

		// GLOBAL: TOY2 0x0052FCE4
		int32_t g_unk52FCE4;

		// GLOBAL: TOY2 0x0052FC3C
		int32_t g_unk52FC3C;

		// GLOBAL: TOY2 0x0052FCF4
		int32_t* g_unk52FCF4;

		// GLOBAL: TOY2 0x0052FCF0
		int32_t* g_unk52FCF0;

		// GLOBAL: TOY2 0x004F3780
		int32_t g_unk4F3780[6] = { 400, 40, 40, 40, 40, 200 };

		// GLOBAL: TOY2 0x0052FC20
		int32_t g_unk52FC20;

		// GLOBAL: TOY2 0x0052FC34
		int32_t g_unk52FC34;

		// GLOBAL: TOY2 0x0052FC40
		int32_t g_unk52FC40;

		// GLOBAL: TOY2 0x0052FC48
		int32_t g_unk52FC48;

		// GLOBAL: TOY2 0x0052FC4C
		int32_t g_unk52FC4C;

		// GLOBAL: TOY2 0x0052FCE0
		int32_t g_unk52FCE0;

		// GLOBAL: TOY2 0x0052FCF8
		int32_t g_unk52FCF8;

		// STUB: TOY2 0x00425680
		void Method2() {}

		// STUB: TOY2 0x00425A80
		void InitMouse() {}

		// STUB: TOY2 0x00425EB0
		void Method1(Vector3I* a2, int32_t arg4, int32_t a3) {}

		// STUB: TOY2 0x00425F60
		void Interactions() {}
	}
}

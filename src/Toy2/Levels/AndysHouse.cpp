#include "Numerics.h"
#include "Toy2/Levels/AndysHouse.h"

namespace Toy2
{
	namespace AndysHouse
	{
		// GLOBAL: TOY2 0x004F0EDC
		int16_t g_unk4F0EDC[6] = { 57, 58, 59, 60, 48, 0 };

		// GLOBAL: TOY2 0x004F0EE8
		Collectables::TokenTableEntry g_unk4F0EE8[10] = {
			{ 67,
			  31,
			  "you can change camera mode using the pause menu. active camera will always try and turn to view buzz from behind. passive camera will not turn. "
			  "the "
			  "camera left and right buttons turn the ^camera^ in ^passive^ mode and turn ^buzz^ in ^active^ mode.",
			  2048 },
			{ 62, 32, "if you jump at a horizontal bar you will swing around it. press jump when you have left the bar to jump farther and higher.", 2048 },
			{ 63,
			  33,
			  "you can foot ^stomp^ where you see the ^stomp^ icon by pressing jump and then pressing ^spin^ while you are in the air. this will activate "
			  "switches "
			  "and catapults and will also act as an attack.",
			  2435 },
			{ 64, 34, "if you see the ^push^ icon you can run into the object and push it out of the way. ", 2048 },
			{ 68, 35, "if you press the jump button once and then press it again you can do an extended jump. try it to reach the open drawer.", 727 },
			{ 65,
			  36,
			  "you can grab onto poles. when you are holding on you can push up and down to move up and down the pole. you can press left and right to turn on "
			  "the "
			  "pole. press jump to let go of the pole.",
			  226 },
			{ 69, 37, "you can jump onto zip lines to slide down them. press jump to let go of the line.", 48 },
			{ 66,
			  38,
			  "you can target your laser by using your visor view. press the visor toggle button to activate the view. then press the target lock button to "
			  "swap "
			  "between visible targets, then press fire to use your laser. use it to shoot the catches holding up the side of the crib.",
			  3072 },
			{ 71,
			  40,
			  "if you press the fire button you can attack with your laser. if you press the spin button you can attack with your spin attack. hold down the "
			  "button to charge up the attack. your spin attack can deflect objects that are fired at you.",
			  0 },
			{ -1, 0, 0, 0 },
		};

		// GLOBAL: TOY2 0x004F0F90
		int16_t g_unk4F0F90[24] = { 1, 5, 1, 2, 6, 2, 3, 7, 3, 4, 1, 4, 7, 3, 7, 8, 4, 8, 0, 0, 0, -1, 0, 0 };

		// GLOBAL: TOY2 0x0052F4F4
		int32_t g_unk52F4F4;

		// GLOBAL: TOY2 0x0052F4F8
		int32_t g_unk52F4F8;

		// GLOBAL: TOY2 0x0052F4FC
		int32_t g_unk52F4FC;

		// GLOBAL: TOY2 0x0052F500
		int32_t g_unk52F500;

		// GLOBAL: TOY2 0x0052F504
		int32_t g_unk52F504;

		// GLOBAL: TOY2 0x0052F508
		int32_t g_unk52F508[4];

		// GLOBAL: TOY2 0x0052F518
		int32_t g_unk52F518[4];

		// GLOBAL: TOY2 0x0052F528
		int32_t g_unk52F528;

		// GLOBAL: TOY2 0x0052F52C
		int32_t g_unk52F52C;

		// GLOBAL: TOY2 0x0052F530
		int32_t g_unk52F530;

		// GLOBAL: TOY2 0x0052F538
		int32_t g_unk52F538[4];

		// GLOBAL: TOY2 0x0052F548
		int32_t g_unk52F548[4];

		// GLOBAL: TOY2 0x0052F558
		int32_t g_unk52F558[4];

		// GLOBAL: TOY2 0x0052F568
		int32_t g_unk52F568;

		// GLOBAL: TOY2 0x0052F56C
		int32_t g_unk52F56C;

		// GLOBAL: TOY2 0x0052F570
		int32_t g_unk52F570;

		// GLOBAL: TOY2 0x0052F574
		int32_t g_unk52F574;

		// GLOBAL: TOY2 0x0052F578
		int32_t g_inTinRobotFight;

		// GLOBAL: TOY2 0x0052F57C
		int32_t g_unk52F57C;

		// GLOBAL: TOY2 0x0052F580
		int32_t g_unk52F580;

		// GLOBAL: TOY2 0x0052F584
		int32_t g_unk52F584;

		// GLOBAL: TOY2 0x0052F58C
		int32_t g_unk52F58C;

		// GLOBAL: TOY2 0x0052F590
		int32_t g_unk52F590;

		// GLOBAL: TOY2 0x0052F594
		int32_t g_unk52F594;

		// GLOBAL: TOY2 0x0052F598
		int32_t g_unk52F598;

		// GLOBAL: TOY2 0x0052F59C
		int32_t g_unk52F59C;

		// GLOBAL: TOY2 0x0052F5A8
		int32_t g_unk52F5A8;

		// GLOBAL: TOY2 0x0052F5AC
		int32_t g_unk52F5AC;

		// GLOBAL: TOY2 0x0052F5B0
		int32_t g_unk52F5B0;

		// GLOBAL: TOY2 0x0052F5B4
		int32_t g_unk52F5B4;

		// GLOBAL: TOY2 0x0052F5B8
		Vector3I g_unk52F5B8;

		// GLOBAL: TOY2 0x0052F5C8
		int32_t g_unk52F5C8;

		// GLOBAL: TOY2 0x0052F5CC
		int32_t g_unk52F5CC;

		// GLOBAL: TOY2 0x0052F5D0
		int32_t g_unk52F5D0;

		// STUB: TOY2 0x00417380
		void Method2(int16_t a1, int32_t linkId, Vector3I* v1, int16_t a4, int32_t a5) {}

		// STUB: TOY2 0x00417510
		void Method1(int16_t a1, int32_t linkId, Vector3I* v1, int16_t a4) {}

		// STUB: TOY2 0x00417680
		void Interactions() {}
	}
}

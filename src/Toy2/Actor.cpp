#include "Toy2/Actor.h"
#include "Toy2/Toy2.h"
#include "RawLoader.h"
#include "Random.h"
#include "Numerics.h"
#include "Nu3D/Math.h"
#include "Nu3D/Collision.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shadows.h"
#include "AudioManager/AudioManager.h"
#include "Toy2/Buzz.h"
#include "Toy2/Animation.h"
#include "Toy2/Particles.h"
#include "Toy2/Lighting.h"
#include "Nu3D/Particles.h"
#include "CharacterLoader.h"

#include <string.h>
#include <stdlib.h>

namespace Toy2
{
	namespace Actor
	{
		// GLOBAL: TOY2 0x0052F1D0
		Toy2Actor* g_activeActors[65];

		// GLOBAL: TOY2 0x00529D48
		Toy2Actor* g_activeActorArray[64];

		// GLOBAL: TOY2 0x0052ADD8
		uint8_t g_unk52ADD8[512];

		// GLOBAL: TOY2 0x0050A54C
		int32_t g_unk50A54C = -1;

		// GLOBAL: TOY2 0x0052EF48
		int32_t g_unk52EF48;

		// GLOBAL: TOY2 0x0052EF88
		int32_t g_unk52EF88;

		// GLOBAL: TOY2 0x004DF728
		uint16_t g_moveScript1[10] = { 0x000D, 0x0000, 0x0002, 0x0016, 0xFFE0, 0xFFE0, 0x0004, 0xFFFF, 0x0001, 0x0000 };

		// GLOBAL: TOY2 0x004DF73C
		uint16_t g_moveScript2[70] = {
			0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0000, 0x0002, 0x0011, 0x0008, 0x0021, 0x0001, 0x003C, 0x0004, 0x0005, 0x0000, 0x0019, 0x0001, 0x0001, 0x000A,
			0x0004, 0x000D, 0x0001, 0x0004, 0x0001, 0x0016, 0x0004, 0x000D, 0x0001, 0x0005, 0x0018, 0xF800, 0x0017, 0x000E, 0x0007, 0x000D, 0x0001, 0x0006,
			0x0001, 0x000C, 0x0004, 0xFFFF, 0x0025, 0x001B, 0x0019, 0x0001, 0x0001, 0x000A, 0x0004, 0x000D, 0x0001, 0x0004, 0x0001, 0x0016, 0x0004, 0x000D,
			0x0001, 0x0001, 0x0018, 0xF800, 0x0017, 0x000E, 0x0007, 0x000D, 0x0001, 0x0006, 0x0001, 0x000C, 0x0004, 0xFFFF, 0x0041, 0x0000
		};

		// GLOBAL: TOY2 0x004DF7C8
		uint16_t g_moveScript3[6] = { 0x0016, 0xFFE0, 0xFFE0, 0x0004, 0xFFFF, 0x0001 };

		// GLOBAL: TOY2 0x004DF7D4
		uint16_t g_moveScript4[14] = { 0x0016, 0xFFC0, 0xFFC0, 0x000D, 0x0000, 0x0007, 0x0005, 0x0100, 0x0002, 0x001F, 0x0020, 0x0004, 0xFFFF, 0x0006 };

		// GLOBAL: TOY2 0x004DF7F0
		uint16_t g_moveScript5[16] = {
			0x000D, 0x0000, 0x0002, 0x0016, 0xFFE0, 0xFFE0, 0x001C, 0xFC00, 0x0017, 0x0020, 0x0002, 0x003F, 0x0080, 0x0004, 0xFFFF, 0x0008
		};

		// GLOBAL: TOY2 0x004DF810
		uint16_t g_moveScript6[110] = {
			0x000D, 0x0000, 0x0001, 0x0016, 0xFFE0, 0xFFE0, 0x0011, 0x0004, 0xFFFF, 0x0002, 0x000D, 0x0000, 0x0001, 0x0016, 0xFFE0, 0xFFE0,
			0x0011, 0x0008, 0x0004, 0x0004, 0xFFFF, 0x0004, 0x0016, 0xFFC0, 0xFFC0, 0x000D, 0x0001, 0x0009, 0x0001, 0x002C, 0x0004, 0x000D,
			0x0002, 0x0001, 0x0001, 0x017D, 0x0004, 0x000C, 0xFEF7, 0x0000, 0x000D, 0x0003, 0x0009, 0x0001, 0x0040, 0x0004, 0x0016, 0xFFE0,
			0xFFE0, 0x000D, 0x0005, 0x000A, 0x0017, 0x0022, 0x0001, 0x0020, 0x0004, 0x000D, 0x0005, 0x000A, 0x0017, 0x0022, 0x0001, 0x0020,
			0x0004, 0x000D, 0x0005, 0x000A, 0x0017, 0x0022, 0x0001, 0x0020, 0x0004, 0x000C, 0xFEFF, 0x0100, 0x0016, 0xFFC0, 0xFFC0, 0x000D,
			0x0004, 0x0009, 0x0001, 0x002C, 0x0004, 0x000C, 0xFFF7, 0x0008, 0xFFFF, 0x004E, 0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0006, 0x0004,
			0x0001, 0x0020, 0x0004, 0xFFFF, 0x001A, 0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0007, 0x000B, 0x0004, 0xFFFF, 0x0001
		};

		// GLOBAL: TOY2 0x004DF8EC
		uint16_t g_moveScript7[10] = { 0x000D, 0x0000, 0x0001, 0x0016, 0xFFE0, 0xFFE0, 0x0004, 0xFFFF, 0x0001, 0x0000 };

		// GLOBAL: TOY2 0x004DF900
		uint16_t g_moveScript8[10] = { 0x000D, 0x0000, 0x0007, 0x0016, 0xFFE0, 0xFFE0, 0x0004, 0xFFFF, 0x0001, 0x0000 };

		// GLOBAL: TOY2 0x004DF914
		uint16_t g_moveScript9[10] = { 0x000D, 0x0000, 0x000D, 0x0016, 0xFFE0, 0xFFE0, 0x0004, 0xFFFF, 0x0001, 0x0000 };

		// GLOBAL: TOY2 0x004DF928
		uint16_t g_moveScript10[14] = { 0x000D, 0x0000, 0x000E, 0x0016, 0xFFF0, 0xFFF0, 0x0005, 0x0400, 0x0002, 0x003F, 0x0040, 0x0004, 0xFFFF, 0x0006 };

		// GLOBAL: TOY2 0x004DF944
		uint16_t g_moveScript11[14] = { 0x000D, 0x0004, 0x0007, 0x0016, 0xFFC0, 0xFFC0, 0x0005, 0x0080, 0x0002, 0x003F, 0x0040, 0x0004, 0xFFFF, 0x0006 };

		// GLOBAL: TOY2 0x004DF960
		uint16_t g_moveScript12[50] = {
			0x0016, 0xFFF0, 0xFFF0, 0x000D, 0x0000, 0x0007, 0x0001, 0x0080, 0x0004, 0x000E, 0x0003, 0x0008, 0x0003, 0xFFFF, 0x000A, 0x0016, 0xFFE0,
			0xFFE0, 0x000D, 0x0001, 0x0004, 0x0001, 0x0018, 0x0004, 0x0016, 0xFFF0, 0xFFF0, 0x000D, 0x0003, 0x0007, 0x0001, 0x0080, 0x0004, 0x000E,
			0x0003, 0x0008, 0x0003, 0xFFFF, 0x000A, 0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0002, 0x0004, 0x0001, 0x0018, 0x0004, 0xFFFF, 0x0030
		};

		// GLOBAL: TOY2 0x004DF9C4
		uint16_t g_moveScript13[82] = {
			0x000D, 0x0000, 0x000E, 0x001E, 0x0007, 0x0016, 0xFFE0, 0xFFE0, 0x0011, 0x0008, 0x0005, 0x0005, 0x0400, 0xFFFF, 0xFFFD, 0x001B, 0x000C,
			0xFFFB, 0x0004, 0x0014, 0x0060, 0x001D, 0x000A, 0x0004, 0x0012, 0x0100, 0x0008, 0x0003, 0xFFFF, 0x0005, 0x000C, 0xFFFB, 0x0000, 0x0001,
			0x003C, 0x0004, 0x000C, 0xFFFB, 0x0004, 0x0014, 0x00FF, 0x001D, 0x0002, 0x0011, 0x0008, 0x0005, 0x0005, 0x0400, 0xFFFF, 0xFFFD, 0x001B,
			0x0001, 0x003C, 0x0004, 0x000C, 0xFFFB, 0x0000, 0x0001, 0x001E, 0x0004, 0xFFFF, 0x0034, 0x001E, 0x0004, 0x0016, 0xFFC0, 0xFFC0, 0x000D,
			0x0001, 0x0007, 0x000C, 0xFEFB, 0x0000, 0x0001, 0x001C, 0x0004, 0x000C, 0xFEFF, 0x0100, 0xFFFF, 0x004F, 0x0000
		};

		// GLOBAL: TOY2 0x004DFA68
		uint16_t g_moveScript14[32] = {
			0x0016, 0xFFE0, 0xFFE0, 0x000C, 0xFFF3, 0x0000, 0x001E, 0x0004, 0x000D, 0x0000, 0x0002, 0x0004, 0xFFFF, 0x0001, 0x001E, 0x0007,
			0x000C, 0xFFF3, 0x000C, 0x0001, 0x0300, 0x0004, 0x001E, 0x0004, 0x000C, 0xFFF7, 0x0000, 0x0001, 0x0100, 0x0004, 0xFFFF, 0x0010
		};

		// GLOBAL: TOY2 0x004DFAA8
		uint16_t g_moveScript15[108] = {
			0x0016, 0xFFC0, 0xFFC0, 0x000D, 0x0001, 0x0001, 0x0004, 0xFFFF, 0x0001, 0x0016, 0xFFC0, 0xFFC0, 0x000D, 0x0001, 0x0001, 0x0001, 0x0001, 0x0004,
			0x0010, 0x0640, 0x0008, 0x0025, 0x0016, 0xFFE8, 0xFFE8, 0x001F, 0x0006, 0x0001, 0x000F, 0x0004, 0x000D, 0x0003, 0x0004, 0x0001, 0x0021, 0x0004,
			0x000D, 0x0003, 0x0005, 0x0018, 0xF800, 0x0017, 0x00D3, 0x0019, 0x0002, 0x0007, 0x0017, 0x003B, 0x0021, 0x0003, 0x000D, 0x0003, 0x0006, 0x0001,
			0x0026, 0x0004, 0xFFFF, 0x002F, 0x0004, 0x0004, 0x0004, 0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0007, 0x0001, 0x0021, 0x0002, 0x0001, 0x0028, 0x0020,
			0x0021, 0x0001, 0x0001, 0x0028, 0x0020, 0xFFFF, 0x0044, 0x0016, 0xFFED, 0xFFED, 0x001E, 0x0004, 0x000D, 0x0005, 0x0001, 0x0001, 0x0096, 0x0004,
			0x001E, 0x0007, 0xFFFF, 0x0053, 0x0016, 0xFFC0, 0xFFC0, 0x001E, 0x0004, 0x000D, 0x0001, 0x0001, 0x0001, 0x00B4, 0x0004, 0xFFFF, 0x0003, 0x0000
		};

		// GLOBAL: TOY2 0x004DFB80
		uint16_t g_moveScript16[14] = { 0x000D, 0x0000, 0x0002, 0x0016, 0xFFD0, 0xFFD0, 0x0005, 0x0080, 0x0002, 0x003F, 0x0040, 0x0004, 0xFFFF, 0x0006 };

		// GLOBAL: TOY2 0x004DFB9C
		uint16_t g_moveScript17[14] = { 0x0016, 0xFFC0, 0xFFC0, 0x000D, 0x0000, 0x0002, 0x0005, 0x0100, 0x0002, 0x001F, 0x0020, 0x0004, 0xFFFF, 0x0006 };

		// GLOBAL: TOY2 0x004DFBB8
		uint16_t g_moveScript18[78] = {
			0x000C, 0xFFFB, 0x0004, 0x0016, 0x007F, 0xFFC0, 0x000D, 0x0000, 0x0001, 0x000F, 0x0180, 0x0005, 0x0080, 0x0004, 0x0011, 0x0008,
			0x000C, 0x0022, 0x0008, 0x001D, 0x000E, 0x000F, 0x0008, 0x0003, 0xFFFF, 0x000B, 0xFFFF, 0x000F, 0x0016, 0x0040, 0xFFC0, 0x000D,
			0x0001, 0x0014, 0x000F, 0x0400, 0x0011, 0x0008, 0x0003, 0xFFFF, 0x0024, 0x001B, 0x0004, 0x0022, 0x0008, 0x0003, 0xFFFF, 0x000A,
			0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0002, 0x000A, 0x000C, 0xFFFB, 0x0000, 0x0001, 0x003C, 0x0020, 0x0022, 0x0008, 0x000C, 0x000D,
			0x0003, 0x000A, 0x0001, 0x0012, 0x0004, 0x000C, 0xFFFB, 0x0004, 0xFFFF, 0x002C, 0x0020, 0xFFFF, 0x000F, 0x0000
		};

		// GLOBAL: TOY2 0x004DFC54
		uint16_t g_moveScript19[18] = {
			0x0016, 0xFFC0, 0xFFC0, 0x000D, 0x0000, 0x0002, 0x000C, 0xFFF3, 0x0000, 0x0004, 0xFFFF, 0x0001, 0x000C, 0xFFF3, 0x000C, 0x0004, 0xFFFF, 0x0001
		};

		// GLOBAL: TOY2 0x004DFC78
		uint16_t g_moveScript20[24] = {
			0x000D, 0x0000, 0x0002, 0x0016, 0xFFD0, 0xFFD0, 0x0005, 0x0080, 0x000C, 0xFFFB, 0x0004, 0x0002,
			0x003F, 0x0040, 0x0004, 0x000C, 0xFFFB, 0x0000, 0x0002, 0x003F, 0x0040, 0x0004, 0xFFFF, 0x0010
		};

		// GLOBAL: TOY2 0x004DFCA8
		uint16_t g_moveScript21[10] = { 0x000D, 0x0001, 0x0002, 0x0016, 0xFFD0, 0xFFD0, 0x0004, 0xFFFF, 0x0001, 0x0000 };

		// GLOBAL: TOY2 0x004DFCBC
		uint16_t g_moveScript22[68] = {
			0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0000, 0x0002, 0x0001, 0x001E, 0x0010, 0x0BB8, 0x0008, 0x0025, 0x0010, 0x07D0, 0x0008, 0x0004, 0x0004,
			0xFFFF, 0x0009, 0x0023, 0x0002, 0x0019, 0x0001, 0x0001, 0x000A, 0x0004, 0x000D, 0x0002, 0x0004, 0x0001, 0x0016, 0x0004, 0x000D, 0x0002,
			0x0005, 0x0018, 0xFB00, 0x0017, 0x000E, 0x0007, 0x000D, 0x0002, 0x0006, 0x0001, 0x000C, 0x0004, 0xFFFF, 0x002B, 0x0021, 0x0001, 0x0004,
			0xFFFF, 0x0001, 0x000D, 0x0001, 0x0001, 0x0001, 0x005C, 0x0004, 0x000D, 0x0000, 0x0002, 0x0001, 0x005C, 0x0004, 0xFFFF, 0x0035, 0x0000
		};

		// GLOBAL: TOY2 0x004DFD44
		uint16_t g_moveScript23[52] = {
			0x0021, 0x0001, 0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0001, 0x0009, 0x000C, 0xFFF3, 0x0000, 0x001E, 0x0000, 0x0001, 0x003C, 0x0004, 0x000C, 0xFFFB,
			0x0004, 0x0014, 0x00FE, 0x001D, 0x0000, 0x0001, 0x0018, 0x0004, 0x0016, 0xFFC0, 0xFFC0, 0x000D, 0x0000, 0x0002, 0x0001, 0x000A, 0x0004, 0x001E,
			0x0007, 0x000C, 0xFFF7, 0x0008, 0x0014, 0x0040, 0x001D, 0x0005, 0x0005, 0x0200, 0x0002, 0x003F, 0x0040, 0x0004, 0xFFFF, 0x0007
		};

		// GLOBAL: TOY2 0x004DFDAC
		uint16_t g_moveScript24[28] = {
			0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0001, 0x000E, 0x000C, 0xFFFB, 0x0004, 0x0005, 0x0800, 0x0002, 0x003F, 0x0070,
			0x0004, 0x000D, 0x0000, 0x0002, 0x000C, 0xFFFB, 0x0000, 0x0002, 0x001F, 0x0020, 0x0004, 0xFFFF, 0x0016, 0x0000
		};

		// GLOBAL: TOY2 0x004DFDE4
		uint16_t g_moveScript25[70] = {
			0x0016, 0xFFE0, 0xFFE0, 0x000C, 0xFFFB, 0x0000, 0x000D, 0x0000, 0x0002, 0x001E, 0x0004, 0x0004, 0xFFFF, 0x0001, 0x001E, 0x0007, 0x000D, 0x0000,
			0x0002, 0x0001, 0x003C, 0x0020, 0x0010, 0x03E8, 0x0008, 0x0011, 0x000C, 0xFFFB, 0x0004, 0x000D, 0x0001, 0x0001, 0x001B, 0x0001, 0x000A, 0x0004,
			0x0010, 0x03E8, 0x0008, 0x0003, 0xFFFF, 0x0008, 0x000C, 0xFFFB, 0x0000, 0x000D, 0x0003, 0x0009, 0x0001, 0x0020, 0x0020, 0x0021, 0x001E, 0x0001,
			0x0038, 0x0020, 0xFFFF, 0x0028, 0x000C, 0xFFFB, 0x0000, 0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0002, 0x000C, 0x0004, 0xFFFF, 0x0001
		};

		// GLOBAL: TOY2 0x004DFE70
		uint16_t g_moveScript26[10] = { 0x000D, 0x0000, 0x000F, 0x0016, 0xFFF0, 0xFFF0, 0x0004, 0xFFFF, 0x0001, 0x0000 };

		// GLOBAL: TOY2 0x004DFE84
		uint16_t g_moveScript27[18] = {
			0x000D, 0x0000, 0x0010, 0x0016, 0xFFF0, 0xFFF0, 0x001A, 0x0400, 0x0000, 0x0001, 0x0014, 0x0004, 0x0021, 0x000A, 0x0004, 0xFFFF, 0x0001, 0x0000
		};

		// GLOBAL: TOY2 0x004DFEA8
		uint16_t g_moveScript28[38] = {
			0x0016, 0xFFE0, 0xFFE0, 0x000C, 0xFFFB, 0x0000, 0x000D, 0x0000, 0x0002, 0x001E, 0x0004, 0x0004, 0xFFFF,
			0x0001, 0x001E, 0x0007, 0x000D, 0x0000, 0x0002, 0x0001, 0x003C, 0x0020, 0x000C, 0xFFFB, 0x0004, 0x000D,
			0x0001, 0x0001, 0x0016, 0xFFC0, 0xFFC0, 0x001B, 0x0001, 0x000A, 0x0004, 0xFFFF, 0x0004, 0x0000
		};

		// GLOBAL: TOY2 0x004DFEF4
		uint16_t g_moveScript29[22] = {
			0x0016, 0xFFE0, 0xFFE0, 0x000C, 0xFFF3, 0x0000, 0x000D, 0x0000, 0x0002, 0x001E, 0x0004,
			0x0004, 0xFFFF, 0x0001, 0x001E, 0x0007, 0x000C, 0xFFF3, 0x000C, 0x0004, 0xFFFF, 0x0001
		};

		// GLOBAL: TOY2 0x004DFF20
		uint16_t g_moveScript30[62] = {
			0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0001, 0x0001, 0x0011, 0x0008, 0x0004, 0x0004, 0xFFFF, 0x0004, 0x001F, 0x0000, 0x0019, 0x0001,
			0x000D, 0x0000, 0x0004, 0x0001, 0x0016, 0x0004, 0x000D, 0x0000, 0x0005, 0x0018, 0xFA00, 0x0017, 0x005A, 0x0007, 0x000D, 0x0000,
			0x0006, 0x0001, 0x0016, 0x0004, 0x001F, 0xFF00, 0x0019, 0x0001, 0x000D, 0x0000, 0x0004, 0x0001, 0x0016, 0x0004, 0x000D, 0x0000,
			0x0005, 0x0018, 0xFA00, 0x0017, 0x005A, 0x0007, 0x000D, 0x0000, 0x0006, 0x0001, 0x0016, 0x0004, 0xFFFF, 0x0039
		};

		// GLOBAL: TOY2 0x004DFF9C
		uint16_t g_moveScript31[10] = { 0x000D, 0x0000, 0x0002, 0x0016, 0xFFC0, 0xFFC0, 0x0004, 0xFFFF, 0x0001, 0x0000 };

		// GLOBAL: TOY2 0x004DFFB0
		uint16_t g_moveScript32[10] = { 0x000D, 0x0002, 0x0002, 0x0016, 0xFFD0, 0xFFD0, 0x0004, 0xFFFF, 0x0001, 0x0000 };

		// GLOBAL: TOY2 0x004DFFC4
		uint16_t g_moveScript33[14] = { 0x0016, 0xFFC0, 0xFFC0, 0x000D, 0x0000, 0x0002, 0x0005, 0x0400, 0x0002, 0x003F, 0x0040, 0x0004, 0xFFFF, 0x0006 };

		// GLOBAL: TOY2 0x004DFFE0
		uint16_t g_moveScript34[64] = {
			0x0016, 0xFFD0, 0xFFD0, 0x000C, 0xFFF3, 0x0000, 0x000D, 0x0003, 0x0002, 0x001E, 0x0004, 0x0004, 0xFFFF, 0x0001, 0x001E, 0x0007,
			0x000C, 0xFFFB, 0x0004, 0x000D, 0x0000, 0x0001, 0x001B, 0x0001, 0x0021, 0x0004, 0x001B, 0x0001, 0x0021, 0x0004, 0x0010, 0x0FA0,
			0x0008, 0x0003, 0xFFFF, 0x000F, 0x000C, 0xFFFB, 0x0000, 0x000D, 0x0001, 0x0008, 0x0001, 0x0015, 0x0020, 0x0021, 0x001E, 0x0001,
			0x0033, 0x0020, 0xFFFF, 0x0022, 0x000C, 0xFFFB, 0x0000, 0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0002, 0x0015, 0x0004, 0xFFFF, 0x0001
		};

		// GLOBAL: TOY2 0x004E0060
		uint16_t g_moveScript35[38] = {
			0x0016, 0xFFD0, 0xFFD0, 0x000D, 0x0000, 0x0001, 0x000C, 0xFFFB, 0x0004, 0x0005, 0x0200, 0x0002, 0x003F,
			0x0080, 0x0004, 0x000C, 0xFFFB, 0x0000, 0x001B, 0x000D, 0x0001, 0x0016, 0x0001, 0x0027, 0x0020, 0x0021,
			0x0001, 0x0001, 0x003F, 0x0020, 0x0011, 0x0008, 0x0003, 0xFFFF, 0x001E, 0xFFFF, 0x0011, 0x0000
		};

		// GLOBAL: TOY2 0x004E00AC
		uint16_t g_moveScript36[46] = {
			0x0016, 0xFFC0, 0xFFC0, 0x000C, 0xFFF3, 0x0000, 0x000D, 0x0001, 0x0001, 0x001E, 0x0004, 0x0004, 0xFFFF, 0x0001, 0x0001, 0x003C,
			0x0004, 0x001E, 0x0007, 0x000C, 0xFFFB, 0x0004, 0x0016, 0x0040, 0xFFC0, 0x000D, 0x0000, 0x0002, 0x001B, 0x0001, 0x0042, 0x0004,
			0xFFFF, 0x0004, 0x000C, 0xFFFB, 0x0000, 0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0002, 0x0002, 0x0004, 0xFFFF, 0x0001
		};

		// GLOBAL: TOY2 0x004E0108
		uint16_t g_moveScript37[14] = { 0x000D, 0x0000, 0x000F, 0x0016, 0xFFE0, 0xFFE0, 0x0005, 0x0080, 0x0002, 0x003F, 0x0040, 0x0004, 0xFFFF, 0x0006 };

		// GLOBAL: TOY2 0x004E0124
		uint16_t g_moveScript38[28] = {
			0x0016, 0xFFE0, 0xFFE0, 0x0005, 0x0000, 0x000D, 0x0000, 0x0004, 0x0001, 0x0016, 0x0004, 0x000D, 0x0000, 0x0005,
			0x0018, 0xFA00, 0x0017, 0x000E, 0x0007, 0x000D, 0x0000, 0x0006, 0x0001, 0x0014, 0x0004, 0xFFFF, 0x0016, 0x0000
		};

		// GLOBAL: TOY2 0x004E015C
		uint16_t g_moveScript39[14] = { 0x0016, 0xFFE0, 0xFFE0, 0x000C, 0xFFF3, 0x0000, 0x000D, 0x0001, 0x0002, 0x001E, 0x0004, 0x0004, 0xFFFF, 0x0001 };

		// GLOBAL: TOY2 0x004E0178
		uint16_t g_moveScript40[58] = {
			0x0016, 0xFFE0, 0xFFE0, 0x000C, 0xFFF3, 0x0000, 0x000D, 0x0000, 0x0001, 0x001E, 0x0004, 0x0004, 0xFFFF, 0x0001, 0x001E,
			0x0006, 0x0016, 0xFFE0, 0xFFE0, 0x000C, 0xFFF3, 0x0000, 0x000D, 0x0000, 0x0001, 0x0001, 0x0090, 0x0004, 0x0016, 0xFFC0,
			0xFFC0, 0x000C, 0xFFF3, 0x000C, 0x000D, 0x0001, 0x0001, 0x0005, 0x0080, 0x0002, 0x003F, 0x0040, 0x0004, 0xFFFF, 0x0006,
			0x000C, 0xFFF3, 0x0000, 0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0002, 0x0009, 0x0004, 0xFFFF, 0x0001, 0x0000
		};

		// GLOBAL: TOY2 0x004E01EC
		uint16_t g_moveScript41[30] = {
			0x0016, 0xFFE0, 0xFFE0, 0x0005, 0x0000, 0x0019, 0x0001, 0x000D, 0x0000, 0x0004, 0x0001, 0x0016, 0x0004, 0x000D, 0x0000,
			0x0005, 0x0018, 0xFB00, 0x0017, 0x000E, 0x0007, 0x000D, 0x0000, 0x0006, 0x0001, 0x0014, 0x0004, 0xFFFF, 0x0018, 0x0000
		};

		// GLOBAL: TOY2 0x004E0228
		uint16_t g_moveScript42[58] = {
			0x0016, 0xFFE0, 0xFFE0, 0x000C, 0xFFF3, 0x0000, 0x000D, 0x0000, 0x0002, 0x001E, 0x0004, 0x0004, 0xFFFF, 0x0001, 0x001E,
			0x0006, 0x0016, 0xFFE0, 0xFFE0, 0x000C, 0xFFF3, 0x0000, 0x000D, 0x0000, 0x0002, 0x0001, 0x0018, 0x0004, 0x0016, 0xFFC0,
			0xFFC0, 0x000C, 0xFFF3, 0x000C, 0x000D, 0x0004, 0x0001, 0x0005, 0x0080, 0x0002, 0x003F, 0x0040, 0x0004, 0xFFFF, 0x0006,
			0x000C, 0xFFF3, 0x0000, 0x0016, 0xFFE0, 0xFFE0, 0x000D, 0x0001, 0x0015, 0x0004, 0xFFFF, 0x0001, 0x0000
		};

		// GLOBAL: TOY2 0x004E029C
		uint16_t g_moveScript43[10] = { 0x000D, 0x0000, 0x0010, 0x0016, 0xFFE0, 0xFFE0, 0x0004, 0xFFFF, 0x0001, 0x0000 };

		// GLOBAL: TOY2 0x004E02B0
		uint16_t g_moveScript44[10] = { 0x000D, 0x0000, 0x0001, 0x0016, 0xFFC0, 0xFFC0, 0x0004, 0xFFFF, 0x0001, 0x0000 };

		// GLOBAL: TOY2 0x004E02C4
		uint16_t* g_movementData[44] = {
			g_moveScript1,	g_moveScript2,	g_moveScript3,	g_moveScript4,	g_moveScript5,	g_moveScript6,	g_moveScript7,	g_moveScript8,	g_moveScript9,
			g_moveScript10, g_moveScript11, g_moveScript12, g_moveScript13, g_moveScript14, g_moveScript15, g_moveScript16, g_moveScript17, g_moveScript18,
			g_moveScript19, g_moveScript20, g_moveScript21, g_moveScript22, g_moveScript23, g_moveScript24, g_moveScript25, g_moveScript26, g_moveScript27,
			g_moveScript28, g_moveScript29, g_moveScript30, g_moveScript31, g_moveScript32, g_moveScript33, g_moveScript34, g_moveScript35, g_moveScript36,
			g_moveScript37, g_moveScript38, g_moveScript39, g_moveScript40, g_moveScript41, g_moveScript42, g_moveScript43, g_moveScript44
		};

		// GLOBAL: TOY2 0x004E0374
		uint8_t g_unk4E0374[28] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
			0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0xFF, 0x18, 0x00, 0x00
		};

		// GLOBAL: TOY2 0x004E0390
		uint8_t g_unk4E0390[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0x0C, 0x00, 0x00 };

		// GLOBAL: TOY2 0x004E03A0
		uint8_t g_unk4E03A0[4] = { 0x00, 0x00, 0xFF, 0x00 };

		// GLOBAL: TOY2 0x004E03A4
		uint8_t g_unk4E03A4[12] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0xFF, 0x01, 0x00, 0x00 };

		// GLOBAL: TOY2 0x004E03B0
		uint8_t g_unk4E03B0[12] = { 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0xFF, 0x01 };

		// GLOBAL: TOY2 0x004E03BC
		uint8_t g_unk4E03BC[8] = { 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0xFF, 0x01 };

		// GLOBAL: TOY2 0x004E03C4
		uint8_t g_unk4E03C4[20] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x10, 0x00, 0x00 };

		// GLOBAL: TOY2 0x004E03D8
		uint8_t g_unk4E03D8[36] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
			0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0xFF, 0x20, 0x00, 0x00
		};

		// GLOBAL: TOY2 0x004E03FC
		uint8_t g_unk4E03FC[28] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
			0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0xFF, 0x00, 0x00, 0x00
		};

		// GLOBAL: TOY2 0x004E0418
		uint8_t g_unk4E0418[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0xFF, 0x00 };

		// GLOBAL: TOY2 0x004E0420
		uint8_t g_unk4E0420[20] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x00, 0x00, 0x00 };

		// GLOBAL: TOY2 0x004E0434
		uint8_t g_unk4E0434[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0x00, 0x00, 0x00 };

		// GLOBAL: TOY2 0x004E0444
		uint8_t g_unk4E0444[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x05, 0x00 };

		// GLOBAL: TOY2 0x004E044C
		uint8_t g_unk4E044C[8] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0xFF, 0x06 };

		// GLOBAL: TOY2 0x004E0454
		uint8_t g_unk4E0454[12] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0xFF, 0x08, 0x00, 0x00 };

		// GLOBAL: TOY2 0x004E0460
		uint8_t g_unk4E0460[20] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x10, 0x00, 0x00 };

		// GLOBAL: TOY2 0x004E0474
		uint8_t g_unk4E0474[12] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0xFF, 0x01, 0x00, 0x00, 0x00 };

		// GLOBAL: TOY2 0x004E0480
		uint8_t g_unk4E0480[20] = { 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0xFF, 0x01, 0x00, 0x00, 0x00 };

		// GLOBAL: TOY2 0x004E0494
		uint8_t g_unk4E0494[12] = { 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0xFF, 0x01 };

		// GLOBAL: TOY2 0x004E04A0
		uint8_t g_unk4E04A0[20] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0xFF, 0x12 };

		// GLOBAL: TOY2 0x004E04B4
		uint8_t g_unk4E04B4[20] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0xFF, 0x00 };

		// GLOBAL: TOY2 0x004E04C8
		uint8_t g_unk4E04C8[40] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
			0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0xFF, 0x01
		};

		// GLOBAL: TOY2 0x004E04F0
		uint8_t g_unk4E04F0[76] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
			0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
			0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
			0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0xFF, 0x01, 0x00, 0x00, 0x00
		};

		// GLOBAL: TOY2 0x004E053C
		uint8_t g_unk4E053C[52] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
			0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23,
			0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0xFF, 0x01, 0x00, 0x00
		};

		// GLOBAL: TOY2 0x004E0570
		uint8_t g_unk4E0570[24] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x16
		};

		// NOTE: indexed by creatureId - 1, the original indexes from 0x004E05EE
		// GLOBAL: TOY2 0x004E05F0
		int16_t g_unk4E05F0[62] = {
			150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 0,   0,	70,	 150, 150, 150, 0,	 150, 150, 150, 150,
			150, 0,	  150, 150, 150, 150, 50,  0,	0,	 150, 150, 150, 150, 150, 150, 150, 100, 150, 150, 150, 150,
			150, 150, 150, 150, 150, 300, 150, 150, 150, 150, 150, 350, 0,	 150, 300, 150, 150, 150, 150, 0
		};

		// GLOBAL: TOY2 0x0052FE34
		int32_t g_unk52FE34;

		// GLOBAL: TOY2 0x004E0588
		uint8_t* g_unk4E0588[26] = {
			(uint8_t*)0xBFF, g_unk4E0374, g_unk4E0390, g_unk4E03A0, g_unk4E03A4, g_unk4E03B0, g_unk4E03BC, g_unk4E03C4, g_unk4E03D8,
			g_unk4E03FC,	 g_unk4E0418, g_unk4E0420, g_unk4E0434, g_unk4E0444, g_unk4E044C, g_unk4E0454, g_unk4E0460, g_unk4E0474,
			g_unk4E0480,	 g_unk4E0494, g_unk4E04A0, g_unk4E04B4, g_unk4E04C8, g_unk4E04F0, g_unk4E053C, g_unk4E0570
		};

		// FUNCTION: TOY2 0x00407150
		void InitCreatureRam()
		{
			memset(g_levelActors, 0, sizeof(g_levelActors));
			memset(g_unk52ADD8, 0, sizeof(g_unk52ADD8));

			g_unk50A54C = -1;
			g_activeActors[0] = 0;
			g_unk52EF48 = 0;
			g_unk52EF88 = 0;

			for (int32_t i = 0; i < 64; i++)
			{
				RawLoader::CreatureListRam* creatureRam = &RawLoader::g_creatureListRam[i];
				uint8_t creatureId = creatureRam->creatureId;

				g_levelActors[i].secondaryAnimIdx = (int16_t)0xFFFF;

				if (creatureId)
				{
					g_levelActors[i].creatureRam = creatureRam;

					if (creatureId == CREATURE_2_ZGCAR)
						creatureRam->entCtrl.actorPhase = 4;

					Game::InitActor(&g_levelActors[i], 1);
				}
			}
		}

		// FUNCTION: TOY2 0x00414A80
		void GetCreatureList(uint8_t* creatureIdList)
		{
			InitCreatureRam();

			int32_t count = 1;

			*creatureIdList = 0;

			for (int32_t i = 0; i < 64; i++)
			{
				if (g_levelActors[i].creatureId)
					creatureIdList[count++] = (uint8_t)g_levelActors[i].creatureId;
			}

			creatureIdList[count] = 0xFF;
		}

		// FUNCTION: TOY2 0x004019D0
		void UpdatePrimaryAnimation(Toy2::Actor::Toy2Actor* actor)
		{
			uint8_t* creatureAnims = CharacterLoader::g_unk547CD4[actor->creatureId];

			Animation::EvaluateClip(*(int16_t**)(creatureAnims + 4 * actor->primaryAnimIdx + 8), actor->unkVar7, *(uint16_t*)(creatureAnims + 4), 0);
		}

		// FUNCTION: TOY2 0x00405C80
		void StepCreatureAnimFrame(Toy2::Actor::Toy2Actor* actor)
		{
			uint8_t* sequence = (uint8_t*)actor->unkVar30;

			if (sequence[2] != 0xFF || sequence[3] != 0)
			{
				++sequence;
				actor->unkVar30 = (int32_t)sequence;

				if (*sequence == 0xFF)
				{
					if (sequence[1] == 1)
					{
						actor->unkVar30 = (int32_t)(sequence - 1);
						actor->unkVar7 |= 0xFFFF;
					}
					else
					{
						actor->unkVar30 = (int32_t)(sequence - sequence[1]);
					}
				}

				uint8_t* current = (uint8_t*)actor->unkVar30;
				int32_t unkVar7 = actor->unkVar7;

				actor->unkVar7 = unkVar7 & 0xFFFF;
				actor->unkVar7 = (*current << 16) + (unkVar7 & 0xFFFF);
			}
			else
			{
				actor->unkVar7 = (*sequence << 16) + 0xFFFF;
			}
		}

		// FUNCTION: TOY2 0x00405CF0
		void SetAnimation(Toy2::Actor::Toy2Actor* actor, int16_t state, int32_t animIdx)
		{
			actor->primaryAnimIdx = state;

			uint8_t* animSet = g_unk4E0588[animIdx];

			actor->unkVar30 = (int32_t)animSet;
			actor->unkVar7 = *animSet << 16;
		}

		// FUNCTION: TOY2 0x00405D20
		void Kill(Toy2::Actor::Toy2Actor* actor, uint8_t phase)
		{
			if ((phase & 1) != 0)
			{
				int32_t burstCount = 0;
				int32_t burstSound = 0;
				int32_t spawnBurst = 0;

				if ((actor->actorFlags & ACTOR_NO_DESPAWN) == 0)
				{
					Nu3D::Particles::ParticleInstance* particle =
						Nu3D::Particles::SpawnInstance(actor->pos.x, actor->pos.y - 4096, actor->pos.z, 0, -2048, 0, 128, 0, 0, 61);
					int32_t groundHeight = Nu3D::Collision::GetGroundHeight((WorldPos*)particle, 0);
					particle->groundHeightY = groundHeight;

					if (groundHeight == 0x80000000)
						particle->groundHeightY = particle->pos.y;
				}

				actor->actorFlags |= ACTOR_NO_DESPAWN;
				((uint8_t*)&actor->areaIndex)[2] = (uint8_t)-32;
				((uint8_t*)&actor->areaIndex)[3] = (uint8_t)-32;
				actor->velX = 0;
				actor->velForward = 0;
				actor->gravityVel = -1024;

				switch (actor->creatureId)
				{
					case 3:
					case 14:
						actor->primaryAnimIdx = 2;
						actor->unkVar30 = (int32_t)g_unk4E0588[1];
						burstCount = 3;
						actor->unkVar29 = -94;
						actor->unkVar7 = *g_unk4E0588[1] << 16;
						spawnBurst = 1;
						break;
					case 4:
						actor->primaryAnimIdx = 2;
						actor->unkVar30 = (int32_t)g_unk4E0588[7];
						burstCount = 3;
						actor->unkVar29 = -62;
						actor->unkVar7 = *g_unk4E0588[7] << 16;
						spawnBurst = 1;
						break;
					case 15:
						actor->unkVar29 = -1;
						Particles::SpawnBurstRingAtPoint(&actor->pos, 0x20, 0x20, 0x20, 0x2000);
						burstSound = 1;
						break;
					case 16:
					case 25:
						actor->unkVar29 = -1;
						burstCount = 6;
						AudioManager::PlaySoundEffect(-2, &actor->pos);
						spawnBurst = 1;
						break;
					case 20:
						actor->primaryAnimIdx = 1;
						actor->unkVar30 = (int32_t)g_unk4E0588[20];
						burstCount = 3;
						actor->unkVar29 = -70;
						actor->unkVar7 = *g_unk4E0588[20] << 16;
						spawnBurst = 1;
						break;
					case 21:
						Particles::SpawnBurstRingAtActor(actor, 0x60, 0x70, 0x80, 12288);
						actor->unkVar29 = -8;
						break;
					case 22:
						actor->unkVar29 = -1;
						Particles::SpawnBurstRingAtPoint(&actor->pos, 0x80, 0, 0, 20480);
						burstSound = 1;
						break;
					case 24:
						actor->gravityVel = -2816;
						actor->primaryAnimIdx = 2;
						actor->unkVar30 = (int32_t)g_unk4E0588[1];
						burstCount = 3;
						actor->unkVar29 = -94;
						actor->unkVar7 = *g_unk4E0588[1] << 16;
						actor->unkShort2 = 10000;
						spawnBurst = 1;
						break;
					case 27:
						actor->unkVar29 = -1;
						burstCount = 4;
						spawnBurst = 1;
						break;
					case 31:
						actor->unkVar29 = -1;
						Particles::SpawnBurstRingAtPoint(&actor->pos, 0, 0, 0x80, 12288);
						burstSound = 1;
						break;
					case 32:
						actor->unkVar29 = -1;
						Particles::SpawnBurstRingAtPoint(&actor->pos, 0, 0x80, 0, 12288);
						burstSound = 1;
						break;
					case 33:
					case 46:
						actor->unkVar29 = -1;
						burstCount = -5;
						spawnBurst = 1;
						break;
					case 41:
						actor->unkVar29 = -1;
						burstCount = -5;
						AudioManager::PlaySoundEffect(89, &actor->pos);
						spawnBurst = 1;
						break;
					case 48:
						actor->unkVar29 = -1;
						Particles::SpawnBurstRingAtPoint(&actor->pos, 0x80, 0, 0, 0);
						burstSound = 1;
						break;
					case 54:
						actor->unkVar29 = -1;
						Particles::SpawnBurstRingAtPoint(&actor->pos, 0, 0, 0x80, 20480);
						burstCount = 6;
						AudioManager::PlaySoundEffect(-2, &actor->pos);
						spawnBurst = 1;
						break;
					default:
						actor->unkVar29 = -1;
						break;
				}

				if (burstSound)
					AudioManager::PlaySoundEffect(-2, &actor->pos);

				if (spawnBurst)
				{
					int16_t* attachOffset = (int16_t*)actor->unkVar34;
					int32_t spawnX = actor->pos.x + attachOffset[0];
					int32_t spawnY = actor->pos.y + attachOffset[1];
					int32_t spawnZ = actor->pos.z + attachOffset[2];

					if (burstCount <= 0)
					{
						int32_t debrisCount = -burstCount;

						for (int32_t i = 0; i < debrisCount; i++)
						{
							Nu3D::Particles::ParticleInstance* debris = Nu3D::Particles::SpawnFromPreset(spawnX, spawnY, spawnZ, 99, 10 * (i & 1) + 4);
							debris->velY -= 256;
							debris->lifetime = (*g_randDatBufferPtr++ & 0x1F) + 120;
							uint8_t shade = (*g_randDatBufferPtr++ & 0x7F) + 64;
							debris->colourR = shade;
							debris->colourG = shade;
							debris->colourB = shade;
							uint8_t direction = *g_randDatBufferPtr++ & 1;
							uint8_t spin = *g_randDatBufferPtr;

							if (direction)
								debris->rotSpeed = (spin & 0x3F) + 64;
							else
								debris->rotSpeed = -64 - (spin & 0x3F);

							++g_randDatBufferPtr;
							Nu3D::Particles::SpawnFromPreset(spawnX, spawnY, spawnZ, 17, 4);
						}

						AudioManager::PlaySoundEffect(10, &actor->pos);
					}
					else
					{
						bool last;

						do
						{
							Nu3D::Particles::SpawnFromPreset(spawnX, spawnY, spawnZ, 35, 14)->rotSpeed = *g_randDatBufferPtr - 128;
							last = burstCount == 1;
							++g_randDatBufferPtr;
							--burstCount;
						} while (! last);

						Lighting::SpawnLight(spawnX, spawnY, spawnZ, 0xF08000, 32, (int32_t)actor);
						AudioManager::PlaySoundEffect(10, &actor->pos);
					}
				}
			}

			if ((phase & 2) != 0)
			{
				g_unk50A54C = (int32_t)actor;

				if (! actor->unkShort2)
					actor->creatureId = 0;

				actor->actorFlags &= 0xFFFC;
				actor->actorPhase = 0;
				actor->unkVar13_ = 0;

				int32_t actorIdx = 0;

				if (g_activeActors[0] != actor)
				{
					Toy2Actor** cursor = g_activeActors;
					Toy2Actor* next;

					do
					{
						next = cursor[1];
						++cursor;
						++actorIdx;
					} while (next != actor);
				}

				Toy2Actor** tail = &g_activeActors[actorIdx + 1];
				int32_t lastIdx = actorIdx;

				if (*tail)
				{
					Toy2Actor* nextActor;

					do
					{
						nextActor = tail[1];
						++tail;
						++lastIdx;
					} while (nextActor);
				}

				g_activeActors[actorIdx] = g_activeActors[lastIdx];
				g_activeActors[lastIdx] = 0;
			}
		}

		// FUNCTION: TOY2 0x004076F0
		void UpdateAIMovement(Toy2::Actor::Toy2Actor* actor)
		{
			RawLoader::CreatureListRam* creatureRam = actor->creatureRam;
			Toy2Actor::ActorBehaviourContext behaviourContext;
			uint16_t* movementData = actor->movementData;
			int32_t updateFlags = 0;
			int32_t snapFlag = 0;
			int32_t scriptFlag = 0;

			behaviourContext.targetFlags = 0;

			int32_t boundHalfXShift = creatureRam->boundHalfX << 8;
			int32_t boundHalfZShift = creatureRam->boundHalfZ << 8;
			int32_t latSpeedNoTarget = 2 * creatureRam->latSpeedNoTarget;
			int32_t sinBound = Numerics::g_sinCosLUT[8 * creatureRam->boundAngle] >> 2;
			int32_t latSpeedTarget = 2 * creatureRam->latSpeedTarget;
			int32_t cosBound = Numerics::g_sinCosLUT[(8 * creatureRam->boundAngle + 1024) & 0xFFF] >> 2;
			int32_t actorFlags = actor->actorFlags & 4;

			if (actor->damageCooldownTimer > 0)
			{
				int16_t newTimer = actor->damageCooldownTimer - Renderer::g_frameDelta;
				actor->damageCooldownTimer = newTimer;

				if (newTimer < 0)
					actor->damageCooldownTimer = 0;

				latSpeedNoTarget = 32;
				latSpeedTarget = 32;
			}

			actor->unkShort1 -= Renderer::g_frameDelta;

			if (actor->unkShort1 < 0)
			{
				while (1)
				{
					uint16_t* scriptStart = movementData;

					switch ((int16_t)*movementData)
					{
						case -1:
							movementData -= (int16_t)movementData[1];
							break;
						case 1:
							actor->unkShort1 = movementData[1];
							movementData += 2;
							break;
						case 2:
							actor->unkShort1 = *g_randDatBufferPtr & movementData[1];
							++g_randDatBufferPtr;
							actor->unkShort1 += movementData[2];
							movementData += 3;
							break;
						case 3: {
							int32_t distance;
							int16_t param = movementData[1];

							if (param >= 0)
							{
								distance = param;
							}
							else
							{
								int16_t boundHalfX = creatureRam->boundHalfX;
								int16_t boundHalfZ = creatureRam->boundHalfZ;

								if (boundHalfX <= boundHalfZ)
									distance = -64 * boundHalfX / param;
								else
									distance = -64 * boundHalfZ / param;
							}

							uint8_t high = *g_randDatBufferPtr++;
							int32_t angle = (*g_randDatBufferPtr++ + (high << 8)) & 0xFFF;

							actor->motionTargetPos.x = actor->boundary.x + ((distance * Numerics::g_sinCosLUT[(angle - 2048) & 0xFFF]) >> 9);
							actor->motionTargetPos.y = actor->boundary.y;

							movementData += 2;

							actor->motionTargetPos.z = actor->boundary.z + ((distance * Numerics::g_sinCosLUT[(angle - 1024) & 0xFFF]) >> 9);
							break;
						}
						case 5: {
							int32_t margin = 32 * (int16_t)movementData[1];
							int32_t offsetX;
							int32_t offsetZ;

							if (boundHalfXShift >= margin)
							{
								offsetX = 2 * creatureRam->boundHalfX * *g_randDatBufferPtr++;

								if (offsetX < margin)
									offsetX = margin;

								if (offsetX > 2 * boundHalfXShift - margin)
									offsetX = 2 * boundHalfXShift - margin;

								offsetX = offsetX - boundHalfXShift;
							}
							else
							{
								offsetX = 0;
							}

							if (boundHalfZShift >= margin)
							{
								offsetZ = 2 * *g_randDatBufferPtr++ * creatureRam->boundHalfZ;

								if (offsetZ < margin)
									offsetZ = margin;

								if (offsetZ > 2 * boundHalfZShift - margin)
									offsetZ = 2 * boundHalfZShift - margin;

								offsetZ = offsetZ - boundHalfZShift;
							}
							else
							{
								offsetZ = 0;
							}

							int32_t rotatedX = sinBound * offsetZ + cosBound * offsetX;
							int32_t rotatedZ = cosBound * offsetZ - sinBound * offsetX;

							actor->motionTargetPos.x = actor->boundary.x + (rotatedX >> 12);
							actor->motionTargetPos.y = actor->boundary.y;
							actor->motionTargetPos.z = actor->boundary.z + (rotatedZ >> 12);
							movementData += 2;
							break;
						}
						case 8:
							if (scriptFlag > 0)
								movementData = &movementData[(int16_t)movementData[1] + 1];
							else
								movementData += 2;
							break;
						case 11:
							movementData += 2;
							break;
						case 12:
							actor->actorFlags &= movementData[1];
							actor->actorFlags = actor->actorFlags | movementData[2];

							movementData += 3;
							break;
						case 13: {
							actor->primaryAnimIdx = movementData[1];

							uint8_t* animSet = g_unk4E0588[(int16_t)movementData[2]];

							actor->unkVar30 = (int32_t)animSet;
							actor->unkVar7 = *animSet << 16;
							movementData += 3;
							break;
						}
						case 14:
							scriptFlag = 1;

							if (! ((uint8_t)movementData[1] & *g_randDatBufferPtr++))
							{
								movementData += 2;
								break;
							}

							scriptFlag = 0;
							movementData += 2;
							break;
						case 15:
							creatureRam->speedTarget = (int16_t)movementData[1] >> 4;
							movementData += 2;
							break;
						case 16: {
							int32_t diffX = (actor->pos.x - g_buzzActor.posAngles.pos.x) >> 5;
							int32_t diffZ = (actor->pos.z - g_buzzActor.posAngles.pos.z) >> 5;
							int16_t range = movementData[1];

							scriptFlag = diffX * diffX + diffZ * diffZ < range * range;
							movementData += 2;
							break;
						}
						case 17: {
							int32_t diffX = (g_buzzActor.posAngles.pos.x - actor->boundary.x) >> 5;
							int32_t diffZ = (g_buzzActor.posAngles.pos.z - actor->boundary.z) >> 5;

							// NOTE: the original compares these unsigned
							if ((uint32_t)(boundHalfXShift + ((cosBound * diffX - sinBound * diffZ) >> 7)) >= (uint32_t)(2 * boundHalfXShift))
							{
								scriptFlag = 0;
							}
							else
							{
								scriptFlag = 1;

								if ((uint32_t)(boundHalfZShift + ((sinBound * diffX + cosBound * diffZ) >> 7)) >= (uint32_t)(2 * boundHalfZShift))
									scriptFlag = 0;
							}

							++movementData;
							break;
						}
						case 18: {
							int32_t diffX = (actor->pos.x - actor->motionTargetPos.x) >> 5;
							int32_t diffZ = (actor->pos.z - actor->motionTargetPos.z) >> 5;
							int16_t range = movementData[1];

							scriptFlag = diffX * diffX + diffZ * diffZ < range * range;
							movementData += 2;
							break;
						}
						case 19:
							// NOTE: scriptFlag is only ever 0 or 1, so this never jumps
							if (scriptFlag < 0)
								movementData = &movementData[(int16_t)movementData[1] + 1];
							else
								movementData += 2;
							break;
						case 20:
							creatureRam->speedNoTarget = (uint8_t)movementData[1];
							movementData += 2;
							break;
						case 21:
							actor->unkVar9 = 0x80000000;
							++movementData;
							break;
						case 22:
							((uint8_t*)&actor->areaIndex)[2] = (uint8_t)movementData[1];
							((uint8_t*)&actor->areaIndex)[3] = (uint8_t)movementData[2];
							movementData += 3;
							break;
						case 23:
							AudioManager::PlaySoundEffect((int16_t)movementData[1], &actor->pos);
							movementData += 2;
							break;
						case 24: {
							int32_t divisor = -(int16_t)movementData[1] >> 5;

							actor->velX = (actor->motionTargetPos.x - actor->pos.x) / divisor;
							actor->velForward = (actor->motionTargetPos.z - actor->pos.z) / divisor;

							actor->actorFlags |= ACTOR_VEL_CLAMP_OVERRIDE;
							actor->gravityVel = (int16_t)movementData[1];
							movementData += 2;
							break;
						}
						case 25: {
							int16_t mode = movementData[1];
							int32_t angle;

							if (mode == 1)
							{
								angle = Nu3D::Math::CartesianToFixedAngle(actor->pos.x - actor->motionTargetPos.x, actor->pos.z - actor->motionTargetPos.z);
							}
							else
							{
								if (mode != 2)
								{
									movementData += 2;
									break;
								}

								angle =
									Nu3D::Math::CartesianToFixedAngle(actor->pos.x - g_buzzActor.posAngles.pos.x, actor->pos.z - g_buzzActor.posAngles.pos.z);
							}

							actor->targetYaw = (angle - 2048) & 0xFFF;
							movementData += 2;
							break;
						}
						case 26:
							actor->velX = (int16_t)movementData[1];
							actor->velForward = (int16_t)movementData[2];
							movementData += 3;
							break;
						case 27:
							actor->motionTargetPos.x = g_buzzActor.posAngles.pos.x;

							if ((actor->actorFlags & ACTOR_STUNNED) == 0)
								actor->motionTargetPos.y = g_buzzActor.posAngles.pos.y;

							++movementData;
							actor->motionTargetPos.z = g_buzzActor.posAngles.pos.z;
							break;
						case 28:
							actor->gravityVel = (int16_t)movementData[1];
							movementData += 2;
							break;
						case 29:
							creatureRam->rotSpeed = (uint8_t)movementData[1];
							movementData += 2;
							break;
						case 30:
							*(uint8_t*)&creatureRam->defenseMode = (uint8_t)movementData[1];
							movementData += 2;
							break;
						case 31: {
							int32_t angle =
								Nu3D::Math::CartesianToFixedAngle(g_buzzActor.posAngles.pos.x - actor->pos.x, g_buzzActor.posAngles.pos.z - actor->pos.z)
								& 0xFFF;
							int32_t scale = (int16_t)movementData[1];

							if (scale <= 0)
							{
								angle = (angle + scale + 128) & 0xFFF;
								scale = 1;
							}

							actor->motionTargetPos.x = actor->pos.x + scale * Numerics::g_sinCosLUT[angle];
							movementData += 2;
							actor->motionTargetPos.z = actor->pos.z + scale * Numerics::g_sinCosLUT[(angle + 1024) & 0xFFF];
							break;
						}
						case 33:
							actor->unkShort4 = movementData[1];
							movementData += 2;
							break;
						case 34:
							scriptFlag = Buzz::g_fireChargeTimer != 0;
							++movementData;
							break;
						case 35: {
							int32_t angle =
								Nu3D::Math::CartesianToFixedAngle(g_buzzActor.posAngles.pos.x - actor->pos.x, g_buzzActor.posAngles.pos.z - actor->pos.z)
								& 0xFFF;

							actor->motionTargetPos.x = actor->pos.x - (int16_t)movementData[1] * Numerics::g_sinCosLUT[angle];
							actor->motionTargetPos.z = actor->pos.z - (int16_t)movementData[1] * Numerics::g_sinCosLUT[(angle + 1024) & 0xFFF];
							movementData += 2;
							break;
						}
						default:
							break;
					}

					if (movementData == scriptStart)
					{
						if (movementData == actor->movementData)
							++movementData;

						break;
					}
				}
			}

			actor->movementData = movementData;

			switch ((int16_t)*movementData)
			{
				case 6:
					snapFlag = 1;
					actor->unkShort1 = 20;
					break;
				case 7:
					snapFlag = 2;
					actor->unkShort1 = 20;
					break;
				case 32:
					actor->targetYaw =
						(Nu3D::Math::CartesianToFixedAngle(actor->pos.x - g_buzzActor.posAngles.pos.x, actor->pos.z - g_buzzActor.posAngles.pos.z) - 2048)
						& 0xFFF;
					break;
			}

			int32_t targetX = actor->motionTargetPos.x;
			int32_t targetY = actor->motionTargetPos.y;
			int32_t targetZ = actor->motionTargetPos.z;

			if (creatureRam->initialFacingAngle)
				actor->targetYaw = 16 * creatureRam->initialFacingAngle;

			if ((actor->actorFlags & ACTOR_GROUNDED) != 0
				&& (uint32_t)(boundHalfXShift
							  + ((cosBound * ((g_buzzActor.posAngles.pos.x - actor->boundary.x) >> 5)
								  - sinBound * ((g_buzzActor.posAngles.pos.z - actor->boundary.z) >> 5))
								 >> 7))
					< (uint32_t)(2 * boundHalfXShift)
				&& (uint32_t)(boundHalfZShift
							  + ((sinBound * ((g_buzzActor.posAngles.pos.x - actor->boundary.x) >> 5)
								  + cosBound * ((g_buzzActor.posAngles.pos.z - actor->boundary.z) >> 5))
								 >> 7))
					< (uint32_t)(2 * boundHalfZShift))
			{
				int32_t chaseFlag = 1;
				int32_t heightOffset;
				int32_t pushRadius;

				behaviourContext.targetFlags |= 1;

				targetX = g_buzzActor.posAngles.pos.x;
				targetZ = g_buzzActor.posAngles.pos.z;

				switch (actor->creatureId)
				{
					case CREATURE_ZURG3:
					case CREATURE_2_ZPOD:
						heightOffset = 20480;
						pushRadius = 1200;
						break;
					case CREATURE_TINMAN:
						pushRadius = 0;
						heightOffset = 2048;
						chaseFlag = 0;
						break;
					case CREATURE_2_ZKITE:
						heightOffset = -1;
						pushRadius = 1500;
						chaseFlag = 0;
						break;
					case CREATURE_5_ZEMP:
						pushRadius = 2000;
						heightOffset = g_unk52FE34 != 0 ? 20480 : 2048;
						break;
					default:
						heightOffset = 2048;
						pushRadius = 0;
						break;
				}

				if (abs(actor->pos.y - g_buzzActor.posAngles.pos.y) >= 0x10000)
				{
					targetY = actor->boundary.y;
				}
				else if (heightOffset != -1)
				{
					targetY = g_buzzActor.posAngles.pos.y - heightOffset;
				}

				if (chaseFlag)
					actor->unkShort1 += Renderer::g_frameDelta;

				actorFlags = 1;

				if (pushRadius)
				{
					int32_t diffX = (actor->pos.x - targetX) >> 5;
					int32_t diffZ = (actor->pos.z - targetZ) >> 5;

					if (diffZ * diffZ + diffX * diffX < pushRadius * pushRadius)
					{
						int32_t pushAngle = Nu3D::Math::CartesianToFixedAngle(diffX, diffZ);

						actor->pos.x -= (actor->pos.x - ((pushRadius * Numerics::g_sinCosLUT[pushAngle & 0xFFF]) >> 9) - targetX) >> 4;
						actor->pos.z -= (actor->pos.z - ((pushRadius * Numerics::g_sinCosLUT[(pushAngle + 1024) & 0xFFF]) >> 9) - targetZ) >> 4;
					}
				}
			}

			int32_t yaw = actor->yawAngle;
			int32_t sinYaw = Numerics::g_sinCosLUT[-yaw & 0xFFF] >> 2;
			int32_t cosYaw = Numerics::g_sinCosLUT[(1024 - actor->yawAngle) & 0xFFF] >> 2;

			int32_t frameDelta = Renderer::g_frameDelta;

			int32_t strafeSpeed = (cosYaw * actor->velX + sinYaw * actor->velForward) / 4096;
			int32_t forwardSpeed = (cosYaw * actor->velForward - sinYaw * actor->velX) / 4096;

			if ((actor->actorFlags & ACTOR_VEL_CLAMP_OVERRIDE) == 0)
			{
				if (strafeSpeed < 0)
				{
					strafeSpeed = latSpeedNoTarget * frameDelta / 4 + strafeSpeed;

					if (strafeSpeed > 0)
						strafeSpeed = 0;
				}
				else if (strafeSpeed > 0)
				{
					strafeSpeed = latSpeedNoTarget * frameDelta / -4 + strafeSpeed;

					if (strafeSpeed < 0)
						strafeSpeed = 0;
				}

				if (forwardSpeed < 0)
				{
					forwardSpeed += latSpeedTarget * frameDelta / 4;

					if (forwardSpeed > 0)
						forwardSpeed = 0;
				}
				else if (forwardSpeed > 0)
				{
					forwardSpeed += latSpeedTarget * frameDelta / -4;

					if (forwardSpeed < 0)
						forwardSpeed = 0;
				}
			}

			if (actorFlags)
			{
				if (actor->creatureRam->rotSpeed)
				{
					actor->targetYaw = (Nu3D::Math::CartesianToFixedAngle(actor->pos.x - targetX, actor->pos.z - targetZ) - 2048) & 0xFFF;
					frameDelta = Renderer::g_frameDelta;
				}

				if (creatureRam->speedNoTarget == 0xFF)
				{
					if (forwardSpeed > -8 * creatureRam->speedTarget)
						forwardSpeed += frameDelta * (latSpeedTarget + 32) / -4;
				}
				else if (forwardSpeed < 16 * creatureRam->speedTarget)
				{
					forwardSpeed += frameDelta * (latSpeedTarget + creatureRam->speedNoTarget) / 4;
				}

				updateFlags = ACTOR_UPDATE_MOVING;
			}

			int32_t yawDiff = (actor->targetYaw - actor->yawAngle) & 0xFFF;

			if (yawDiff >= 0x800)
				yawDiff -= 4096;

			actor->yawAngle = (actor->yawAngle + (yawDiff * frameDelta * actor->creatureRam->rotSpeed) / 128) & 0xFFF;

			int32_t sinMove = Numerics::g_sinCosLUT[yaw] >> 2;
			int32_t cosMove = Numerics::g_sinCosLUT[(yaw + 1024) & 0xFFF] >> 2;
			int32_t moveX = (cosMove * strafeSpeed + forwardSpeed * sinMove) / 4096;
			int32_t moveZ = (forwardSpeed * cosMove - sinMove * strafeSpeed) / 4096;

			actor->velX = moveX;
			actor->velForward = moveZ;

			if (creatureRam->speedTarget)
			{
				actor->pos.x += Renderer::g_frameDelta * moveX;
				actor->pos.z += Renderer::g_frameDelta * moveZ;
			}
			else
			{
				strafeSpeed = 0;
				forwardSpeed = 0;
				actor->velX = 0;
				actor->velForward = 0;
			}

			int32_t localX = actor->pos.x - actor->boundary.x;
			int32_t localZ = actor->pos.z - actor->boundary.z;
			int32_t clampX = (cosBound * localX - sinBound * localZ) >> 12;
			int32_t clampZ = (cosBound * localZ + sinBound * localX) >> 12;
			int32_t clamped = 0;

			if (clampX > creatureRam->boundHalfX << 8)
			{
				clampX = creatureRam->boundHalfX << 8;
				clamped = 1;
			}

			if (clampZ > creatureRam->boundHalfZ << 8)
			{
				clampZ = creatureRam->boundHalfZ << 8;
				clamped = 1;
			}

			if (clampX < -256 * creatureRam->boundHalfX)
			{
				clampX = -256 * creatureRam->boundHalfX;
				clamped = 1;
			}

			if (clampZ >= -256 * creatureRam->boundHalfZ)
			{
				if (clamped)
				{
					actor->pos.x = actor->boundary.x + ((cosBound * clampX + sinBound * clampZ) >> 12);
					actor->pos.z = actor->boundary.z + ((cosBound * clampZ - sinBound * clampX) >> 12);
				}
			}
			else
			{
				clampZ = -256 * creatureRam->boundHalfZ;

				actor->pos.x = actor->boundary.x + ((cosBound * clampX + sinBound * clampZ) >> 12);
				actor->pos.z = actor->boundary.z + ((cosBound * clampZ - sinBound * clampX) >> 12);
			}

			int32_t curY = actor->pos.y;

			if ((actor->actorFlags & ACTOR_FLOOR_SNAP) != 0)
			{
				if ((actor->actorFlags & ACTOR_STUNNED) == 0 || (actor->actorFlags & ACTOR_OOB_SPECIAL_MOVE) != 0)
				{
					int32_t snapY = frameDelta * (targetY - curY) / 64 + curY;
					actor->pos.y -= 6400;
					int32_t groundHeight = Nu3D::Collision::GetGroundHeight((WorldPos*)&actor->pos, g_unk4E05F0[actor->creatureId - 1]);
					actor->pos.y = groundHeight;

					if (groundHeight == 0x80000000)
						actor->pos.y = snapY;

					if (actor->pos.y <= snapY)
					{
						int32_t fall = (actor->pos.y - snapY) >> 2;

						if (fall < -1024)
							fall = -1024;

						actor->pos.y = snapY + fall * Renderer::g_frameDelta;
					}
					else
					{
						actor->pos.y = snapY;
					}
				}
				else
				{
					actor->pos.y = actor->motionTargetPos.y;

					if (Renderer::Shadows::g_shadowCount < 47)
					{
						int32_t recordIdx = Renderer::Shadows::g_shadowCount;
						int16_t newCount = Renderer::Shadows::g_shadowCount + 1;
						Renderer::Shadows::g_shadowRecords[recordIdx].worldX = actor->pos.x;
						Renderer::Shadows::g_shadowRecords[recordIdx].groundY = actor->motionTargetPos.y;
						Renderer::Shadows::g_shadowRecords[recordIdx].worldZ = actor->pos.z;

						Renderer::Shadows::g_shadowCount = newCount;
						Renderer::Shadows::g_shadowRecords[recordIdx].size = g_unk4E05F0[actor->creatureId - 1];
					}
				}
			}
			else
			{
				actor->gravityVel = (Renderer::g_frameDelta << 8) / 4 + actor->gravityVel;

				if (actor->gravityVel > 2048)
					actor->gravityVel = 2048;

				int32_t newY = curY + Renderer::g_frameDelta * actor->gravityVel;
				actor->pos.y = newY;
				int32_t fallY = newY;

				if ((actor->actorFlags & ACTOR_STUNNED) != 0)
				{
					actor->pos.y = actor->motionTargetPos.y;

					if (Renderer::Shadows::g_shadowCount < 47)
					{
						int32_t recordIdx = Renderer::Shadows::g_shadowCount;
						int16_t newCount = Renderer::Shadows::g_shadowCount + 1;
						Renderer::Shadows::g_shadowRecords[recordIdx].worldX = actor->pos.x;
						Renderer::Shadows::g_shadowRecords[recordIdx].groundY = actor->motionTargetPos.y;
						Renderer::Shadows::g_shadowRecords[recordIdx].worldZ = actor->pos.z;
						Renderer::Shadows::g_shadowCount = newCount;
						Renderer::Shadows::g_shadowRecords[recordIdx].size = g_unk4E05F0[actor->creatureId - 1];
					}
				}
				else
				{
					actor->pos.y = newY - 6400;
					int32_t groundHeight = Nu3D::Collision::GetGroundHeight((WorldPos*)&actor->pos, g_unk4E05F0[actor->creatureId - 1]);
					actor->pos.y = groundHeight;

					if (groundHeight == 0x80000000)
					{
						fallY = curY;
						actor->pos.y = actor->lastValidYPosition;
					}
					else
					{
						actor->lastValidYPosition = groundHeight;
					}
				}

				int32_t groundY = actor->pos.y;

				if (fallY <= groundY - 512)
				{
					actor->pos.y = fallY;
				}
				else
				{
					if (snapFlag)
					{
						actor->unkShort1 = 0;
						actor->movementData = actor->movementData + 1;

						if ((actor->actorFlags & ACTOR_VEL_CLAMP_OVERRIDE) != 0)
						{
							actor->actorFlags &= ~ACTOR_VEL_CLAMP_OVERRIDE;

							if (snapFlag == 2)
							{
								actor->velX = 0;
								actor->velForward = 0;
								actor->gravityVel = 0;
								updateFlags += 2;

								goto animUpdate;
							}
						}
					}
					else
					{
						int32_t fall = (groundY - fallY) >> 2;

						if (fall < -1024)
							fall = -1024;

						actor->pos.y = fallY + fall * Renderer::g_frameDelta;
					}

					actor->gravityVel = 0;
					updateFlags += ACTOR_UPDATE_GROUNDED;
				}
			}

		animUpdate:
			int32_t areaIndexByte;

			if ((updateFlags & ACTOR_UPDATE_GROUNDED) != 0)
				areaIndexByte = ((uint8_t*)&actor->areaIndex)[2];
			else
				areaIndexByte = ((uint8_t*)&actor->areaIndex)[3];

			int32_t animSpeed;

			if ((int8_t)areaIndexByte < 0)
				animSpeed = -(int8_t)areaIndexByte << 9;
			else
				animSpeed = areaIndexByte * forwardSpeed / 2;

			if (animSpeed > 0)
			{
				int32_t animStep = Renderer::g_frameDelta * animSpeed;

				if (animStep > 0x10000)
				{
					uint32_t wholeSteps = (animStep - 1) >> 16;
					animStep += -65536 * wholeSteps;

					do
					{
						updateFlags |= ACTOR_UPDATE_ANIM_TICK;
						StepCreatureAnimFrame(actor);
						--wholeSteps;
					} while (wholeSteps);
				}

				int32_t oldVar7 = actor->unkVar7;
				actor->unkVar7 = oldVar7 + animStep;

				if (((oldVar7 ^ (oldVar7 + animStep)) & 0xFFFF0000) != 0)
				{
					updateFlags |= ACTOR_UPDATE_ANIM_TICK;
					StepCreatureAnimFrame(actor);
				}
			}

			if (actor->actorBehaviour)
			{
				behaviourContext.localStrafeSpeed = (int16_t)strafeSpeed;
				behaviourContext.updateFlags = (int16_t)updateFlags;
				behaviourContext.actor = actor;
				behaviourContext.localForwardSpeed = (int16_t)forwardSpeed;

				actor->actorBehaviour(&behaviourContext);
			}
		}

		// STUB: TOY2 0x00408A60
		void HandleDamage(Toy2::Actor::Toy2Actor* actor, int32_t angle, int32_t hitType) {}

		// STUB: TOY2 0x004104D0
		void HitType1Particles(int32_t a1, int32_t a2, int32_t a3) {}

		// STUB: TOY2 0x0043C1C0
		void ResolveBoneAttachmentPos(Vector3I* localOffset, Toy2::Actor::Toy2Actor* actor, int32_t boneIndex) {}

		// STUB: TOY2 0x0049F460
		BOOL IsInsideBounds(Toy2::Actor::Toy2Actor* actor, int32_t minX, int32_t maxX, int32_t minZ, int32_t maxZ) { return 0; }

		// STUB: TOY2 0x004A1CE0
		void CollectQuestReward(int32_t actorIdx, int32_t recordIdx, int32_t camAngle, int32_t camAngleValue, int32_t pickupSlotIdx) {}

		// STUB: TOY2 0x004A1E60
		void RotatingHint(int32_t actorIdx, int32_t recordIdx, char** hintTable) {}

		// STUB: TOY2 0x004A2480
		void ItemReturnReward(
			int32_t actorIdx,
			int32_t recordIdx,
			char* textInProgress,
			char* textFoundPiece,
			char* textComplete,
			int32_t camAngleValue,
			int32_t camAngle)
		{
			// not implemented
		}

		// STUB: TOY2 0x004A26F0
		void PlayPeriodicHintSound(int32_t actorId, int32_t soundIdx) {}

		// FUNCTION: TOY2 0x004A28B0
		void PopulateActiveActors()
		{
			Toy2Actor* actor = g_activeActors[0];
			int32_t actorIndex = 0;

			if (g_activeActors[0])
			{
				int32_t arrayIndex = 0;

				do
				{
					++actorIndex;

					g_activeActorArray[arrayIndex] = actor;

					arrayIndex = actorIndex;

					actor = g_activeActors[actorIndex];
				} while (actor);
			}

			g_activeActorArray[actorIndex] = (Toy2Actor*)&g_buzzActor;
			g_activeActorArray[actorIndex + 1] = 0;
		}

		// FUNCTION: TOY2 0x004CDBF0
		int32_t FindInActorList(Toy2::Actor::Toy2Actor* actor)
		{
			Toy2Actor** next = Animation::g_animActorList;

			if (! Animation::g_animActorList)
				return -1;

			Toy2Actor* cur = *Animation::g_animActorList;
			int32_t index = 0;

			if (! *Animation::g_animActorList)
				return -1;

			while (actor != cur)
			{
				cur = next[1];

				++next;
				++index;

				if (! cur)
					return -1;
			}

			return index;
		}

		// FUNCTION: TOY2 0x004CDBB0
		void SetNodeAngle(Toy2::Actor::Toy2Actor* actor, int32_t index, int32_t x, int32_t y, int32_t z)
		{
			int32_t offset = FindInActorList(actor);

			if (offset >= 0)
			{
				int32_t nodeAngleIndex = index + 32 * offset;

				Animation::g_nodeAngles[nodeAngleIndex].x = x;
				Animation::g_nodeAngles[nodeAngleIndex].y = y;
				Animation::g_nodeAngles[nodeAngleIndex].z = z;
			}
		}
	}
}
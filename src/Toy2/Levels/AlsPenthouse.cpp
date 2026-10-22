#include "Numerics.h"
#include "Toy2/Toy2.h"
#include "Toy2/Actor.h"
#include "Toy2/Buzz.h"
#include "Toy2/Camera.h"
#include "Toy2/Collectables.h"
#include "Toy2/Dialogue.h"
#include "Toy2/Gadget.h"
#include "Toy2/KiteTail.h"
#include "Toy2/Level.h"
#include "Toy2/Levels.h"
#include "Toy2/Lighting.h"
#include "Toy2/MoveableObject.h"
#include "Toy2/Platform.h"
#include "Toy2/Particles.h"
#include "Toy2/Weather.h"
#include "Toy2/Cutscene.h"
#include "Toy2/Collision.h"
#include "Nu3D/Link.h"
#include "Nu3D/Math.h"
#include "Nu3D/Particles.h"
#include "Nu3D/Collision.h"
#include "Nu3D/Camera.h"
#include "Renderer/Renderer.h"
#include "Renderer/Sprite.h"
#include "Renderer/LensFlare.h"
#include "Renderer/Effects.h"
#include "Renderer/Shadows.h"
#include "AudioManager/AudioManager.h"
#include "InputManager.h"
#include "Random.h"
#include "SaveManager.h"
#include "Nullsub.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Toy2/Levels/AlsPenthouse.h"

namespace Toy2
{
	namespace AlsPenthouse
	{
		// GLOBAL: TOY2 0x004F3E00
		int16_t g_unk4F3E00[48] = {
			2, 0, 7, 0, 10, 0, 1, 1, 8, 1, 5, 1, 9, 1, 6, 0, 3, 0, 6, 0, 1, 0, 4, 0, 0, 0, 1, 0, 3, 1, 12, 0, 11, 0, 4, 1, 11, 1, 2, 1, 10, 1, 9, 1, 11, 1, 8, 0
		};

		// GLOBAL: TOY2 0x004F3E60
		int16_t g_unk4F3E60[10] = { 30, 19, 13, 29, 20, 0, 81, 21, 14, -1 };

		// GLOBAL: TOY2 0x004F3E74
		int32_t g_unk4F3E74[24] = { 50, 59, 66, 73, 51, 60, 67, 74, 58, 65, 72, 79, 52, 61, 68, 75, 53, 62, 69, 76, 54, 63, 70, 77 };

		// GLOBAL: TOY2 0x004F3ED4
		int32_t g_unk4F3ED4[12] = { 273873, 128840, 270869, 253905, 128840, 270869, 253905, 128840, 247643, 273873, 128840, 247643 };

		// GLOBAL: TOY2 0x004F3F04
		char* g_unk4F3F04[5] = {
			"^hamm^ is in the ^bathroom^! you can reach the ^bathroom^ through a hidden vent. look out for any ^locks^ that you can shoot!",
			"^jesse^ has lost her critters. she is on the ^kitchen table^.",
			"^bullseye^ has a ^challenge^ for you. he is in the entrance hall.",
			"you can get to the token on top of the ^train bed^ if you solve the train puzzle. you need to get the train to stop on the dead end track. you "
			"can change the track points using the yellow switches.",
			"the ^gunslinger^ boss is in the woody's roundup set in the ^trophy room^."
		};

		// GLOBAL: TOY2 0x004F3F18
		int16_t g_unk4F3F18[6] = { 96, 98, 99, 100, 97, 0 };

		// GLOBAL: TOY2 0x004F3F24
		Collectables::TokenTableEntry g_unk4F3F24[3] = {
			{ 120,
			  22,
			  "^stomp^ the buttons to raise and lower the water level. the button in the bottom of the tub will drain all the water. the other buttons will raise the "
			  "water to their level. ",
			  3072 },
			{ 119,
			  21,
			  "^stomp^ the buttons to change the track points and make the train switch tracks. get the train to go onto the dead end track so that you can push the "
			  "box to the ^train bed^ to reach the pizza planet ^token^.",
			  3072 },
			{ -1, 0, 0, 0 }
		};

		// GLOBAL: TOY2 0x004F3F48
		SwitchEntry g_unk4F3F48[7] = {
			{ 1, 36, 17, 84, -350, 0, 0 }, { 2, 37, 18, 84, 0, 0, 0 },	   { 4, 31, 12, 86, 175, 0, 0 }, { 8, 32, 13, 86, 350, 0, 0 },
			{ 16, 33, 14, 86, 0, 0, 0 },   { 32, 34, 15, 85, 0, 0, -350 }, { 64, 35, 16, 85, 0, 0, 0 }
		};

		// GLOBAL: TOY2 0x004F3FAC
		uint8_t g_unk4F3FAC[8] = { 7, 1, 8, 2, 6, 3, 9, 4 };

		// GLOBAL: TOY2 0x004F3FB4
		uint8_t g_unk4F3FB4[24] = { 2, 11, 48, 9, 17, 56, 48, 7, 18, 57, 83, 8, 5, 5, 48, 4, 11, 7, 48, 6, 10, 6, 48, 5 };

		// GLOBAL: TOY2 0x004F3FCC
		uint8_t g_unk4F3FCC[16] = { 7, 16, 0, 3, 8, 32, 20, 0, 6, 64, 40, 1, 9, 128, 72, 2 };

		// GLOBAL: TOY2 0x004F3FDC
		uint8_t g_unk4F3FDC[12] = { 1, 10, 8, 3, 3, 11, 9, 0, 4, 12, 10, 96 };

		// GLOBAL: TOY2 0x004F3FE8
		int32_t g_unk4F3FE8[15] = { -16000, 43, 256, -32000, 20, 1, -64000, 21, 2, -95999, 22, 4, -524288, 23, 8 };

		// GLOBAL: TOY2 0x004F4024
		uint8_t g_unk4F4024[33] = { 44, 43, 20, 21, 22, 23, 91, 12, 13, 14, 15, 16, 17, 18, 31, 32, 33, 34, 35, 36, 37, 66, 67, 68, 69, 70, 72, 73, 74, 75, 76, 77, 79 };

		// GLOBAL: TOY2 0x0052FD08
		int32_t g_unk52FD08;

		// GLOBAL: TOY2 0x0052FD0C
		int32_t g_unk52FD0C;

		// GLOBAL: TOY2 0x0052FD14
		int32_t g_unk52FD14;

		// GLOBAL: TOY2 0x0052FD18
		int32_t g_unk52FD18;

		// GLOBAL: TOY2 0x0052FD1C
		int32_t g_unk52FD1C;

		// GLOBAL: TOY2 0x0052FD20
		int32_t g_unk52FD20;

		// GLOBAL: TOY2 0x0052FD28
		int32_t g_unk52FD28;

		// GLOBAL: TOY2 0x0052FD30
		int32_t g_unk52FD30;

		// GLOBAL: TOY2 0x0052FD34
		int32_t g_unk52FD34;

		// GLOBAL: TOY2 0x0052FD38
		int32_t g_unk52FD38;

		// GLOBAL: TOY2 0x0052FD3C
		int32_t g_unk52FD3C;

		// GLOBAL: TOY2 0x0052FD44
		int32_t g_unk52FD44;

		// GLOBAL: TOY2 0x0052FD48
		int32_t g_unk52FD48;

		// GLOBAL: TOY2 0x0052FD4C
		int32_t g_unk52FD4C;

		// GLOBAL: TOY2 0x0052FD50
		int32_t g_unk52FD50;

		// GLOBAL: TOY2 0x0052FD54
		int32_t g_unk52FD54;

		// GLOBAL: TOY2 0x0052FD74
		int32_t g_unk52FD74;

		// GLOBAL: TOY2 0x0052FD78
		int32_t g_unk52FD78;

		// GLOBAL: TOY2 0x0052FD7C
		int32_t g_unk52FD7C;

		// GLOBAL: TOY2 0x0052FD80
		int32_t g_unk52FD80;

		// GLOBAL: TOY2 0x0052FD84
		int32_t g_unk52FD84;

		// GLOBAL: TOY2 0x0052FD88
		int32_t g_unk52FD88;

		// GLOBAL: TOY2 0x0052FD8C
		int32_t g_unk52FD8C;

		// GLOBAL: TOY2 0x0052FD90
		int32_t g_unk52FD90;

		// GLOBAL: TOY2 0x0052FDA8
		int32_t g_unk52FDA8;

		// GLOBAL: TOY2 0x0052FDAC
		int32_t g_unk52FDAC;

		// GLOBAL: TOY2 0x0052FDB0
		int32_t g_unk52FDB0;

		// GLOBAL: TOY2 0x0052FDB4
		int32_t g_unk52FDB4;

		// GLOBAL: TOY2 0x0052FDB8
		int32_t g_unk52FDB8;

		// GLOBAL: TOY2 0x0052FDBC
		int32_t g_unk52FDBC;

		// GLOBAL: TOY2 0x0052FDC0
		int32_t g_unk52FDC0;

		// GLOBAL: TOY2 0x0052FDC4
		int32_t g_unk52FDC4;

		// GLOBAL: TOY2 0x0052FDF0
		int32_t g_unk52FDF0;

		// GLOBAL: TOY2 0x0052FDF4
		int32_t g_unk52FDF4;

		// GLOBAL: TOY2 0x0052FDF8
		int32_t g_unk52FDF8;

		// GLOBAL: TOY2 0x0052FDFC
		int32_t g_unk52FDFC;

		// GLOBAL: TOY2 0x0052FD58
		int32_t g_unk52FD58[6];

		// GLOBAL: TOY2 0x0052FD98
		Vector3I g_unk52FD98;

		// GLOBAL: TOY2 0x0052FDC8
		HiddenCollectible g_unk52FDC8[5];

		// GLOBAL: TOY2 0x0052FD10
		int32_t g_unk52FD10;

		// GLOBAL: TOY2 0x0052FD2C
		int32_t g_unk52FD2C;

		// GLOBAL: TOY2 0x0052FD40
		int32_t g_unk52FD40;

		// GLOBAL: TOY2 0x0052FD70
		int32_t g_unk52FD70;

		// FUNCTION: TOY2 0x00428700
		void Method10(Vector3I* a1, int16_t a2)
		{
			int32_t x;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			int32_t v6;
			int32_t v7;
			int32_t spawnZ;

			if (g_unk52FD8C > 1000)
			{
				x = a1->x;
				v3 = a1->y - 0x4000;
				v4 = a1->x + ((*g_randDatBufferPtr - 128) << 7) - g_buzzActor.posAngles.pos.x;
				spawnZ = a1->z;
				v5 = (spawnZ + ((*++g_randDatBufferPtr - 128) << 7) - g_buzzActor.posAngles.pos.z) >> 5;
				++g_randDatBufferPtr;
				v6 = (int32_t)sqrt((double)((v4 >> 5) * (v4 >> 5) + v5 * v5));
				if (v6 >= 2000)
				{
					v7 = 2 * v6 / 3;
					Nu3D::Particles::SpawnInstance(
						x,
						v3,
						spawnZ,
						v7 * Numerics::g_sinCosLUT[(a2 + 1024) & 0xFFF] / 0x4000,
						((g_buzzActor.posAngles.pos.y - v3) << 7) / ((v6 << 12) / v7) - (((v6 << 12) / v7) << 7) / 256,
						v7 * Numerics::g_sinCosLUT[(a2 - 2048) & 0xFFF] / 0x4000,
						128,
						0,
						0,
						92);
					AudioManager::PlaySoundEffect(146, a1);
				}
			}
		}

		// FUNCTION: TOY2 0x00428890
		void Method2(int32_t a1)
		{
			int32_t i;
			int32_t v2;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			int32_t v6;
			int32_t v7;

			i = 0;

			do
			{
				v2 = g_unk4F3F48[i].mask;

				if ((v2 & g_unk52FD14) != 0 && (v2 & a1) == 0)
				{
					Nu3D::Link::SetScaleFromFixedOffsets(g_unk4F3F48[i].linkA, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(g_unk4F3F48[i].linkB, 0, 0, 0);
					g_unk52FD14 &= -1 - g_unk4F3F48[i].mask;
					Nu3D::Link::SetRotationRelative8bit(g_unk4F3F48[i].rotLinkId, g_unk4F3F48[i].rotX, g_unk4F3F48[i].rotY, g_unk4F3F48[i].rotZ);
				}

				++i;
			} while (i < 7);

			if ((g_unk52FD14 & 1) == 0 && (a1 & 1) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(36, 4096, 4096, 4096);
				g_unk4F3E00[10] = 5;
				g_unk52FD14 |= 1u;
				g_unk4F3E00[11] = 1;
				g_unk4F3E00[0] = 2;
				g_unk4F3E00[1] = 0;
			}

			if ((g_unk52FD14 & 2) == 0 && (a1 & 2) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(37, 4096, 4096, 4096);
				v3 = g_unk52FD14;
				*(uint8_t*)&v3 = g_unk52FD14 | 2;
				g_unk4F3E00[10] = 1;
				g_unk52FD14 = v3;
				g_unk4F3E00[11] = 1;
				g_unk4F3E00[0] = 3;
				g_unk4F3E00[1] = 0;
			}

			if ((g_unk52FD14 & 4) == 0 && (a1 & 4) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(31, 4096, 4096, 4096);
				v4 = g_unk52FD14;
				*(uint8_t*)&v4 = g_unk52FD14 | 4;
				g_unk4F3E00[12] = 9;
				g_unk52FD14 = v4;
				g_unk4F3E00[13] = 0;
				g_unk4F3E00[38] = 2;
				g_unk4F3E00[39] = 1;
				g_unk4F3E00[28] = 3;
				g_unk4F3E00[29] = 1;
			}

			if ((g_unk52FD14 & 8) == 0 && (a1 & 8) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(32, 4096, 4096, 4096);
				v5 = g_unk52FD14;
				*(uint8_t*)&v5 = g_unk52FD14 | 8;
				g_unk4F3E00[12] = 10;
				g_unk52FD14 = v5;
				g_unk4F3E00[13] = 1;
				g_unk4F3E00[38] = 4;
				g_unk4F3E00[39] = 1;
				g_unk4F3E00[28] = 3;
				g_unk4F3E00[29] = 1;
			}

			if ((g_unk52FD14 & 0x10) == 0 && (a1 & 0x10) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(33, 4096, 4096, 4096);
				v6 = g_unk52FD14;
				*(uint8_t*)&v6 = g_unk52FD14 | 0x10;
				g_unk4F3E00[12] = 8;
				g_unk52FD14 = v6;
				g_unk4F3E00[13] = 1;
				g_unk4F3E00[38] = 2;
				g_unk4F3E00[39] = 1;
				g_unk4F3E00[28] = 4;
				g_unk4F3E00[29] = 1;
			}

			if ((g_unk52FD14 & 0x20) == 0 && (a1 & 0x20) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(34, 4096, 4096, 4096);
				v7 = g_unk52FD14;
				*(uint8_t*)&v7 = g_unk52FD14 | 0x20;
				g_unk4F3E00[42] = 9;
				g_unk52FD14 = v7;
				g_unk4F3E00[43] = 1;
				g_unk4F3E00[30] = 12;
				g_unk4F3E00[31] = 0;
			}

			if ((g_unk52FD14 & 0x40) == 0 && (a1 & 0x40) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(35, 4096, 4096, 4096);
				g_unk4F3E00[42] = 8;
				g_unk52FD14 |= 0x40u;
				g_unk4F3E00[43] = 0;
				g_unk4F3E00[30] = 11;
				g_unk4F3E00[31] = 0;
			}
		}

		// FUNCTION: TOY2 0x00428BA0
		void Method4()
		{
			int32_t v0;
			int32_t v1;
			int32_t i;

			v0 = g_unk52FD14;
			v1 = (g_unk52FD08 + (int32_t)Renderer::g_frameDelta) & 0x1F;
			g_unk52FD08 = v1;
			if (v1 >= 24 || (g_unk52FD14 & 0x800) != 0)
			{
				if (v1 > 24 && (g_unk52FD14 & 0x800) != 0)
				{
					((uint8_t*)&v0)[1] = (uint8_t)((g_unk52FD14) >> 8) & 0xF7;
					i = 0;
					g_unk52FD14 = v0;
					do
					{
						if ((g_unk4F3F48[i].mask & v0) != 0)
						{
							Nu3D::Link::SetScaleFromFixedOffsets(g_unk4F3F48[i].linkB, 4096, 4096, 4096);
							Nu3D::Link::SetScaleFromFixedOffsets(g_unk4F3F48[i].linkA, 0, 0, 0);
							v0 = g_unk52FD14;
						}
						++i;
					} while (i < 7);
				}
			}
			else
			{
				((uint8_t*)&v0)[1] = (uint8_t)((g_unk52FD14) >> 8) | 8;
				i = 0;
				g_unk52FD14 = v0;
				do
				{
					if ((g_unk4F3F48[i].mask & v0) != 0)
					{
						Nu3D::Link::SetScaleFromFixedOffsets(g_unk4F3F48[i].linkA, 4096, 4096, 4096);
						Nu3D::Link::SetScaleFromFixedOffsets(g_unk4F3F48[i].linkB, 0, 0, 0);
						v0 = g_unk52FD14;
					}
					++i;
				} while (i < 7);
			}
		}

		// FUNCTION: TOY2 0x00428C80
		void Method1(int16_t a1)
		{
			int16_t v1;
			int32_t v2;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			int32_t v6;
			int32_t v7;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			int32_t v11;

			if (Camera::g_activeCameraTransform.pos.y <= Buzz::g_underwaterYLevel)
				v1 = a1;
			else
				v1 = 0;

			if ((g_unk52FD3C & 0x100) != 0 && (v1 & 0x100) == 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(43, 0, 0, 0);
				v2 = g_unk52FD3C;
				((uint8_t*)&v2)[1] = (uint8_t)((g_unk52FD3C) >> 8) & 0xFE;
				g_unk52FD3C = v2;
			}

			if ((g_unk52FD3C & 1) != 0 && (v1 & 1) == 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(20, 0, 0, 0);
				v3 = g_unk52FD3C;
				*(uint8_t*)&v3 = g_unk52FD3C & 0xFE;
				g_unk52FD3C = v3;
			}

			if ((g_unk52FD3C & 2) != 0 && (v1 & 2) == 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(21, 0, 0, 0);
				v4 = g_unk52FD3C;
				*(uint8_t*)&v4 = g_unk52FD3C & 0xFD;
				g_unk52FD3C = v4;
			}

			if ((g_unk52FD3C & 4) != 0 && (v1 & 4) == 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(22, 0, 0, 0);
				v5 = g_unk52FD3C;
				*(uint8_t*)&v5 = g_unk52FD3C & 0xFB;
				g_unk52FD3C = v5;
			}

			if ((g_unk52FD3C & 8) != 0 && (v1 & 8) == 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(23, 0, 0, 0);
				v6 = g_unk52FD3C;
				*(uint8_t*)&v6 = g_unk52FD3C & 0xF7;
				g_unk52FD3C = v6;
			}

			if ((g_unk52FD3C & 0x100) == 0 && (v1 & 0x100) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(43, 4096, 4096, 4096);
				v7 = g_unk52FD3C;
				((uint8_t*)&v7)[1] = (uint8_t)((g_unk52FD3C) >> 8) | 1;
				g_unk52FD3C = v7;
			}

			if ((g_unk52FD3C & 1) == 0 && (v1 & 1) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(20, 4096, 4096, 4096);
				v8 = g_unk52FD3C;
				*(uint8_t*)&v8 = g_unk52FD3C | 1;
				g_unk52FD3C = v8;
			}

			if ((g_unk52FD3C & 2) == 0 && (v1 & 2) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(21, 4096, 4096, 4096);
				v9 = g_unk52FD3C;
				*(uint8_t*)&v9 = g_unk52FD3C | 2;
				g_unk52FD3C = v9;
			}

			if ((g_unk52FD3C & 4) == 0 && (v1 & 4) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(22, 4096, 4096, 4096);
				v10 = g_unk52FD3C;
				*(uint8_t*)&v10 = g_unk52FD3C | 4;
				g_unk52FD3C = v10;
			}

			if ((g_unk52FD3C & 8) == 0 && (v1 & 8) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(23, 4096, 4096, 4096);
				v11 = g_unk52FD3C;
				*(uint8_t*)&v11 = g_unk52FD3C | 8;
				g_unk52FD3C = v11;
			}
		}

		// FUNCTION: TOY2 0x00428E70
		void Method8()
		{
			int16_t v0;
			Levels::RecordData* v1;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			int32_t recordCount;
			int32_t v7;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			uint8_t* v11;
			bool v12;
			int32_t v13;
			Vector3I a1;
			Vector3I p_v1;

			if (g_unk52FD4C)
			{
				if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &g_unk52FD98, 48))
				{
					v0 = Nu3D::Math::CartesianToFixedAngle(g_buzzActor.posAngles.pos.x - g_unk52FD98.x, g_buzzActor.posAngles.pos.z - g_unk52FD98.z);
					Buzz::HandleDamage(v0, 1u);
					g_unk52FDF0 = 16;
					if (g_unk52FDB4 > 40)
						g_unk52FDB4 = 40;
				}
				else if (g_unk52FDF0 < 256)
				{
					g_unk52FDF0 += 8;
				}
				if ((g_unk52FD14 & 0x400) == 0)
				{
					v1 = Levels::g_recordData[g_unk52FD4C];
					a1.x = 4 * v1->data[g_unk52FD84].x - g_unk52FD98.x / 8;
					v3 = 4 * v1->data[g_unk52FD84].y - g_unk52FD98.y / 8;
					a1.y = v3;
					v4 = 4 * v1->data[g_unk52FD84].z - g_unk52FD98.z / 8;
					a1.z = v4;
					if (abs(a1.x) < 1024 && abs(v3) < 1024 && abs(v4) < 1024)
					{
						if (g_unk52FDBC)
						{
							v5 = g_unk52FD84 - 1;
							recordCount = -1;
							--g_unk52FD84;
						}
						else
						{
							v5 = ++g_unk52FD84;
							recordCount = v1->recordCount;
						}
						if (v5 == recordCount)
						{
							if (g_unk52FDBC)
							{
								v7 = g_unk4F3E00[4 * g_unk52FD4C - 1];
								v8 = g_unk4F3E00[4 * g_unk52FD4C - 2];
							}
							else
							{
								v7 = g_unk4F3E00[4 * g_unk52FD4C - 3];
								v8 = g_unk4F3E00[4 * g_unk52FD4C - 4];
							}
							g_unk52FDBC = v7;
							g_unk52FD4C = v8;
							if (v7)
								g_unk52FD84 = Levels::g_recordData[v8]->recordCount - 1;
							else
								g_unk52FD84 = 0;
						}
					}
					Nu3D::Math::NormalizeToFixedPoint(&a1, &a1);
					g_unk52FD98.x += (a1.x * g_unk52FDF0 * (int32_t)Renderer::g_frameDelta) >> 10;
					g_unk52FD98.y += (a1.y * g_unk52FDF0 * (int32_t)Renderer::g_frameDelta) >> 10;
					g_unk52FD98.z += (a1.z * g_unk52FDF0 * (int32_t)Renderer::g_frameDelta) >> 10;
					v9 = (g_unk52FD0C - Nu3D::Math::CartesianToFixedAngle(-a1.z, -a1.x)) & 0xFFF;
					if (v9 <= 2048)
						v10 = g_unk52FD0C - (v9 >> 2);
					else
						v10 = ((4096 - v9) >> 2) + g_unk52FD0C;
					g_unk52FD0C = v10;
					Nu3D::Link::SetRotationRelative8bit(38, 0, v10, 0);
					Nu3D::Link::SetRotationRelative8bit(80, 0, g_unk52FD0C, 0);
				}
				Nu3D::Link::SetPositionRawAndCommit(38, g_unk52FD98.x >> 5, g_unk52FD98.y >> 5, g_unk52FD98.z >> 5);
				Nu3D::Link::SetPositionRawAndCommit(80, g_unk52FD98.x >> 7, g_unk52FD98.y >> 7, g_unk52FD98.z >> 7);
				if (g_tickState[4])
				{
					if (Nu3D::Math::IsWithinDistance(&Camera::g_activeCameraTransform.pos, &g_unk52FD98, 768))
					{
						Nu3D::Particles::SpawnFromPreset(
							g_unk52FD98.x - (Numerics::g_sinCosLUT[(g_unk52FD0C + 1024) & 0xFFF] >> 2),
							g_unk52FD98.y - 0x4000,
							g_unk52FD98.z - (Numerics::g_sinCosLUT[g_unk52FD0C & 0xFFF] >> 2),
							17,
							27)
							->rotSpeed = *g_randDatBufferPtr - 128;
						v11 = g_randDatBufferPtr + 1;
						v12 = (g_unk52FDB4 - (int32_t)Renderer::g_frameDelta) <= 0;
						++g_randDatBufferPtr;
						g_unk52FDB4 -= (int32_t)Renderer::g_frameDelta;
						if (v12)
						{
							g_unk52FDB4 = *v11 + 30;
							g_randDatBufferPtr = v11 + 1;
							AudioManager::PlaySoundEffect(151, &g_unk52FD98);
						}
					}
				}
				Nu3D::Link::GetCurrentPosFixed(30, &p_v1);
				if (MoveableObject::g_unk52FD24 <= 0)
				{
					if (Nu3D::Math::IsWithinDistance(&p_v1, &g_unk52FD98, 96))
					{
						v13 = g_unk52FD14;
						MoveableObject::g_unk52FD24 = 180;
						((uint8_t*)&v13)[1] = (uint8_t)((g_unk52FD14) >> 8) | 4;
					}
					else
					{
						v13 = g_unk52FD14;
						((uint8_t*)&v13)[1] = (uint8_t)((g_unk52FD14) >> 8) & 0xFB;
					}
					g_unk52FD14 = v13;
				}
				else
				{
					MoveableObject::g_unk52FD24 -= (int32_t)Renderer::g_frameDelta;
					if (Nu3D::Math::IsWithinDistance(&p_v1, &g_unk52FD98, 96))
						MoveableObject::g_unk52FD24 = 180;
				}
				if (! g_unk52FD4C)
					Collision::MarkPlatformAsMoving(27);
			}
		}

		// FUNCTION: TOY2 0x004292C0
		int32_t Method6(int32_t a1)
		{
			int32_t v1;
			uint8_t* v2;
			int32_t v3;
			int32_t result;
			Vector3I a2;

			v1 = 16;
			v2 = g_unk4F3FAC;
			v3 = 4;
			do
			{
				if ((v1 & g_unk52FD3C) != 0)
					goto LABEL_6;
				if ((v1 & a1) != 0)
				{
					Platform::GetOrigin(v2[0], &a2);
					a2.y += 3600;
					Platform::SetOrigin(v2[0], a2.x, a2.y, a2.z);
					Nu3D::Link::SetScaleFromFixedOffsets(v2[1], 4096, 2400, 4096);
					result = v1 | g_unk52FD3C;
					g_unk52FD3C |= v1;
				}
				if ((v1 & g_unk52FD3C) != 0)
				{
				LABEL_6:
					if ((v1 & a1) == 0)
					{
						Platform::GetOrigin(v2[0], &a2);
						a2.y -= 3600;
						Platform::SetOrigin(v2[0], a2.x, a2.y, a2.z);
						Nu3D::Link::SetScaleFromFixedOffsets(v2[1], 4096, 4094, 4096);
						result = -1 - v1;
						g_unk52FD3C &= -1 - v1;
					}
				}
				v1 *= 2;
				v2 += 2;
				--v3;
			} while (v3);
			return result;
		}

		// FUNCTION: TOY2 0x004293D0
		void Method5()
		{
			int32_t v0;
			int32_t v1;
			int32_t v2;
			int32_t v3;
			Vector3I v4;

			v0 = g_unk52FD3C;
			if ((g_unk52FD3C & 0x10) != 0)
			{
				v1 = 1;
			}
			else if ((g_unk52FD3C & 0x20) != 0)
			{
				v1 = 2;
			}
			else if ((g_unk52FD3C & 0x40) != 0)
			{
				v1 = 3;
			}
			else
			{
				v1 = 4;
				if ((g_unk52FD3C & 0x80u) == 0)
					v1 = v3;
			}
			v2 = (g_unk52FDB8 + 32 * (int32_t)Renderer::g_frameDelta) & 0x3FF;
			g_unk52FDB8 = v2;
			if ((g_unk52FD3C & 0x200) != 0 || v2 <= 768)
			{
				if ((g_unk52FD3C & 0x200) != 0 && v2 < 768)
				{
					((uint8_t*)&v0)[1] = (uint8_t)((g_unk52FD3C) >> 8) & 0xFD;
					g_unk52FD3C = v0;
					Nu3D::Link::GetCurrentPosFixed(v1, &v4);
					Nu3D::Link::SetPositionRawAndCommit(44, v4.x >> 5, v4.y >> 5, v4.z >> 5);
					Nu3D::Link::SetScaleFromFixedOffsets(v1, 4096, 2048, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(44, 0, 0, 0);
				}
			}
			else
			{
				((uint8_t*)&v0)[1] = (uint8_t)((g_unk52FD3C) >> 8) | 2;
				g_unk52FD3C = v0;
				Nu3D::Link::GetCurrentPosFixed(v1, &v4);
				Nu3D::Link::SetPositionRawAndCommit(44, v4.x >> 5, v4.y >> 5, v4.z >> 5);
				Nu3D::Link::SetScaleFromFixedOffsets(v1, 0, 0, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(44, 4096, 2048, 4096);
			}
		}

		// FUNCTION: TOY2 0x004295B0
		void Method9(int32_t a1, int32_t linkId, int32_t a3, int32_t a4)
		{
			int16_t v4;
			int32_t v5;
			int32_t v6;
			int16_t v7;
			int32_t v8;
			int32_t v9;
			Vector3I v10;
			Vector3I p_v1;

			Nu3D::Link::GetCurrentPosFixed(linkId, &p_v1);
			if (g_curSectorCopy2 == a1 && Nu3D::Math::IsWithinDistance(&p_v1, &g_buzzActor.posAngles.pos, 1152))
			{
				if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &p_v1, 50))
				{
					v4 = Nu3D::Math::CartesianToFixedAngle(g_buzzActor.posAngles.pos.x - p_v1.x, g_buzzActor.posAngles.pos.z - p_v1.z);
					if ((g_unk52FD48 & a4) != 0)
						Buzz::HandleDamage(v4, 1u);
					else
						Buzz::HandleDamage(v4, 3u);
				}
				if ((a4 & g_unk52FD48) != 0)
				{
					if (g_tickState[7])
					{
						Nu3D::Particles::SpawnFromPreset(p_v1.x, p_v1.y - 0x2000, p_v1.z, 17, 26)->rotSpeed = *g_randDatBufferPtr - 128;
						++g_randDatBufferPtr;
					}
				}
				else
				{
					v5 = g_buzzActor.posAngles.pos.x - p_v1.x;
					v10.x = g_buzzActor.posAngles.pos.x - p_v1.x;
					v6 = g_buzzActor.posAngles.pos.z - p_v1.z;
					v10.z = g_buzzActor.posAngles.pos.z - p_v1.z;
					v10.y = p_v1.y;
					while (abs(v5) > 0x4000 || abs(v6) > 0x4000)
					{
						v5 >>= 1;
						v6 >>= 1;
					}
					v10.z = v6;
					v10.x = v5;
					v7 = Nu3D::Math::CartesianToFixedAngle(-v6, v5);
					Nu3D::Link::GetRotation8Bit(linkId, &v10);
					v8 = (v7 - (uint16_t)(v10.y)) & 0xFFF;
					if (v8 >= 2048)
					{
						if (4096 - v8 >= 8 * (int32_t)Renderer::g_frameDelta)
							v9 = v10.y - 8 * (int32_t)Renderer::g_frameDelta;
						else
							v9 = v10.y + v8 - 4096;
					}
					else if (v8 >= 8 * (int32_t)Renderer::g_frameDelta)
					{
						v9 = 8 * (int32_t)Renderer::g_frameDelta + v10.y;
					}
					else
					{
						v9 = v8 + v10.y;
					}
					v10.y = v9;
					Nu3D::Link::SetRotationRelative8bit(linkId, 0, v9, 0);
					Nu3D::Link::SetRotationRelative8bit(a3, 0, v10.y, 0);
					Method10(&p_v1, v10.y);
				}
			}
		}

		// FUNCTION: TOY2 0x00429800
		void Method12(int32_t a1)
		{
			int32_t v1;
			int32_t v2;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			Vector3I a2;

			v1 = g_unk4F3E74[a1 + 3];
			v2 = g_unk4F3E74[a1 + 1];
			v3 = g_unk4F3E74[a1];
			v4 = g_unk4F3E74[a1 + 2];
			Nu3D::Link::GetRotation8Bit(v3, &a2);
			Nu3D::Link::SetRotationRelative8bit(v4, 0, a2.y, 0);
			Nu3D::Link::SetRotationRelative8bit(v1, 0, a2.y, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(v4, 4096, 4096, 4096);
			Nu3D::Link::SetScaleFromFixedOffsets(v1, 4096, 4096, 4096);
			Nu3D::Link::SetScaleFromFixedOffsets(v3, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(v2, 0, 0, 0);
			Nu3D::Link::GetCurrentPosFixed(v3, &a2);
			v5 = 4;
			do
			{
				Nu3D::Particles::SpawnFromPreset(a2.x, a2.y - 12288, a2.z, 35, 14)->rotSpeed = *g_randDatBufferPtr - 128;
				--v5;
				++g_randDatBufferPtr;
			} while (v5);
			AudioManager::PlaySoundEffect(-2, &a2);
		}

		// FUNCTION: TOY2 0x00429910
		void Method3()
		{
			uint8_t* v0;
			int32_t* v1;
			int32_t* v2;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			int32_t v6;
			int32_t v7;
			int32_t v8;
			Vector3I p_v1;

			v0 = &g_unk4F3FB4[2];
			v1 = g_unk4F3E74;
			v2 = g_unk52FD58;

			do
			{
				if (! *v2)
				{
					Nu3D::Link::GetCurrentPosFixed(v0[-1], &p_v1);

					if (Nu3D::Math::IsWithinDistance(&p_v1, &g_buzzActor.posAngles.pos, 1120))
					{
						if (Nu3D::Math::IsWithinDistance(&p_v1, &g_buzzActor.posAngles.pos, 992))
						{
							if ((g_unk52FD48 & 0x200) != 0 || g_unk52FDF8 <= 768)
							{
								if ((g_unk52FD48 & 0x200) != 0 && g_unk52FDF8 < 768)
								{
									v4 = g_unk52FD48;
									((uint8_t*)&v4)[1] = (uint8_t)((g_unk52FD48) >> 8) & 0xFD;
									g_unk52FD48 = v4;
									Nu3D::Link::SetScaleFromFixedOffsets(v0[-1], 4096, 4096, 4096);
									Nu3D::Link::SetScaleFromFixedOffsets(v0[0], 0, 0, 0);
								}
							}
							else
							{
								v3 = g_unk52FD48;
								((uint8_t*)&v3)[1] = (uint8_t)((g_unk52FD48) >> 8) | 2;
								g_unk52FD48 = v3;
								Nu3D::Link::SetPositionRawAndCommit(v0[0], p_v1.x >> 5, p_v1.y >> 5, p_v1.z >> 5);
								Nu3D::Link::SetScaleFromFixedOffsets(v0[-1], 0, 0, 0);
								Nu3D::Link::SetScaleFromFixedOffsets(v0[0], 4096, 4096, 4096);
							}
						}
						else if ((g_unk52FD48 & 0x200) != 0)
						{
							v5 = g_unk52FD48;
							((uint8_t*)&v5)[1] = (uint8_t)((g_unk52FD48) >> 8) & 0xFD;
							v8 = v0[-1];
							g_unk52FD48 = v5;
							Nu3D::Link::SetScaleFromFixedOffsets(v8, 4096, 4096, 4096);
							Nu3D::Link::SetScaleFromFixedOffsets(v0[0], 0, 0, 0);
						}
					}

					if (Buzz::g_groundSlamTicker == -40 && (Platform::GetFlags(v0[-2]) & 3) == 2)
					{
						Nu3D::Link::SetScaleFromFixedOffsets(v0[-1], 4096, 2048, 4096);
						Platform::GetOrigin(v0[-2], &p_v1);
						p_v1.y += 3600;
						Platform::SetOrigin(v0[-2], p_v1.x, p_v1.y, p_v1.z);
						v7 = *v1;
						*v2 = 60;
						Nu3D::Link::GetCurrentPosFixed(v7, &p_v1);
						Camera::BeginScriptedCutsceneAtPoint(&p_v1, 180, 32);
						Camera::g_cutsceneFocusPos.y -= 0x2000;
						v6 = v0[0];
						Camera::g_cutsceneEyePos.y -= 0x4000;
						Nu3D::Link::SetScaleFromFixedOffsets(v6, 0, 0, 0);
						Levels::Method3(v0[1], 1);
					}
				}

				++v2;

				v0 += 4;
				v1 += 4;
			} while (v2 < g_unk52FD58 + 6);
		}

		// FUNCTION: TOY2 0x00429CE0
		void InitHiddenCollectibles()
		{
			int32_t i;
			Levels::Type63* entry;

			i = 0;

			if (Levels::g_recordData[63]->recordCount)
			{
				entry = (Levels::Type63*)Levels::g_recordData[63]->data;

				do
				{
					if (entry->props.linkId == 101)
						g_unk52FDC8[0].yPtr = &entry->pos.y;

					if (entry->props.linkId == 102)
						g_unk52FDC8[1].yPtr = &entry->pos.y;

					if (entry->props.linkId == 103)
						g_unk52FDC8[2].yPtr = &entry->pos.y;

					if (entry->props.linkId == 104)
						g_unk52FDC8[3].yPtr = &entry->pos.y;

					if (entry->props.linkId == 105)
						g_unk52FDC8[4].yPtr = &entry->pos.y;

					++entry;
					++i;
				} while (i < Levels::g_recordData[63]->recordCount);
			}

			for (i = 0; i < 5; ++i)
			{
				g_unk52FDC8[i].savedY = *g_unk52FDC8[i].yPtr;
				*g_unk52FDC8[i].yPtr = 0x80000000;
				Nu3D::Link::SetScaleFromFixedOffsets(i + 101, 0, 0, 0);
			}
		}

		// FUNCTION: TOY2 0x00429FB0
		void Method7(int32_t arg0, int32_t linkid, int32_t arg8, int32_t* a4, int32_t* a5)
		{
			int32_t v5;
			int32_t v6;
			int32_t v7;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			Vector3I a1;
			Vector3I v12;

			Platform::GetOrigin(arg0, &a1);
			if (arg8 >= g_unk52FDFC)
				v5 = g_unk52FDFC - arg8;
			else
				v5 = 0;
			if (Buzz::g_groundSlamTicker == -40 && (Platform::GetFlags(arg0) & 3) == 2)
				*a4 = 1280;
			if (*a5 >= 0)
				v6 = -48 * (int32_t)Renderer::g_frameDelta + *a4;
			else
				v6 = 48 * (int32_t)Renderer::g_frameDelta + *a4;
			*a4 = v6;
			v7 = *a5;
			v8 = *a5 + (int32_t)Renderer::g_frameDelta * v6;
			*a5 = v8;
			if (v8 >= 0 && v7 < 0)
			{
				if (*a4 <= 512)
					*a4 = 512;
				else
					*a4 >>= 1;
				if (v5)
					Nu3D::Particles::SpawnFromPreset(a1.x, a1.y - *a5, a1.z, 57, 2);
			}
			if (*a5 > 0x2000)
				*a4 = 0;
			if (*a5 < -2048)
				*a4 = 0;
			Nu3D::Link::GetTargetPosFixed(linkid, &v12);
			Nu3D::Link::SetPositionRawAndCommit(linkid, a1.x >> 5, a1.y >> 5, a1.z >> 5);
			if (v5 <= -4096)
				v9 = v5 + *a5 + v12.y;
			else
				v9 = v5 - v5 * *a5 / 4096 + v12.y;
			v10 = v9 - a1.y;
			v12.y = v10;
			if (v10 < -2048)
			{
				v10 = -2048;
			LABEL_28:
				v12.y = v10;
				goto LABEL_29;
			}
			if (v10 > 2048)
			{
				v10 = 2048;
				goto LABEL_28;
			}
		LABEL_29:
			Platform::SetVelocity(arg0, 0, v10, 0);
		}

		// FUNCTION: TOY2 0x0042A130
		void Interactions()
		{
			int32_t v0;
			int32_t v1;
			int32_t v3;
			uint8_t* v4;
			int32_t v5;
			uint8_t v6;
			int32_t v7;
			bool v8;
			int32_t v9;
			uint8_t* v10;
			int32_t v11;
			int32_t v12;
			int16_t v13;
			int32_t v14;
			int32_t v15;
			int32_t v16;
			int32_t v17;
			int32_t v18;
			int32_t v19;
			int16_t v20;
			int16_t v21;
			char v22;
			int32_t i;
			int32_t v24;
			int32_t v25;
			int32_t v27;
			int32_t v29;
			int32_t v31;
			int32_t v32;
			int32_t v33;
			int32_t v34;
			int32_t v35;
			int32_t v36;
			int32_t v37;
			Levels::RecordData* v38;
			int32_t v40;
			int32_t v41;
			int32_t v42;
			int32_t v43;
			int32_t v44;
			int32_t v45;
			bool v46;
			int32_t z;
			int32_t v49;
			int32_t v50;
			int32_t v51;
			int32_t v52;
			int32_t v53;
			int32_t v54;
			int32_t v55;
			int32_t v56;
			int32_t v57;
			Vector3I p_v1;
			Vector3I v60;

			if (g_buzzActor.inhibitMovementTimer)
			{
				if (! g_unk52FDC0)
				{
					Platform::DisableCollision(25);
					Collision::MarkPlatformAsMoving(26);
					g_unk52FDC0 = 1;
				}
			}
			else if (g_unk52FDC0)
			{
				Platform::DisableCollision(26);
				Collision::MarkPlatformAsMoving(25);
				g_unk52FDC0 = 0;
			}

			v0 = g_unk52FDFC;
			g_unk52FD78 += 32 * (int32_t)Renderer::g_frameDelta;

			if (g_unk52FDFC < g_unk52FD30)
			{
				AudioManager::PlaySoundEffect(149, (Vector3I*)1);
				v1 = g_unk52FD30;
				g_unk52FDFC += (int32_t)Renderer::g_frameDelta << 9;

				v0 = g_unk52FDFC;

				if (g_unk52FDFC <= g_unk52FD30)
					goto LABEL_12;
				goto LABEL_11;
			}

			if (g_unk52FDFC > g_unk52FD30)
			{
				AudioManager::PlaySoundEffect(149, (Vector3I*)1);
				v1 = g_unk52FD30;
				g_unk52FDFC += -64 * (int32_t)Renderer::g_frameDelta;

				v0 = g_unk52FDFC;
				if (g_unk52FDFC < g_unk52FD30)
				{
				LABEL_11:
					v0 = v1;
					g_unk52FDFC = v1;
				}
			}
		LABEL_12:

			if (v0)
			{
				Buzz::g_currentFluidType = 1;
				Buzz::g_underwaterYLevel = v0 + 0x2EA18;

				v3 = 0;

				while (v0 < g_unk4F3FE8[3 * v3])
				{
					if (++v3 >= 5)
						goto LABEL_19;
				}

				Nu3D::Link::GetTargetPosFixed(g_unk4F3FE8[3 * v3 + 1], &v60);
				v60.y += g_unk52FDFC;
				Nu3D::Link::SetPositionRawAndCommit(g_unk4F3FE8[3 * v3 + 1], v60.x >> 5, v60.y >> 5, v60.z >> 5);

				Method1(g_unk4F3FE8[3 * v3 + 2]);
			}
			else
			{
				Buzz::g_currentFluidType = 0;
				Method1(0);
			}
		LABEL_19:

			if (Buzz::g_groundSlamTicker == -40)
			{
				v4 = &g_unk4F3FDC[3];
				v5 = 3;

				do
				{
					if ((Platform::GetFlags(*(v4 - 3)) & 3) == 2)
					{
						Levels::Method3(*(v4 - 2), 1);

						v6 = *v4;
						v7 = *(v4 - 1);
						v8 = *v4 == 0;

						g_unk52FD74 = 24;
						g_unk52FD44 = v7;

						if (v8)
						{
							v9 = g_unk52FD14;
							*(uint8_t*)&v9 = g_unk52FD14 & 0xE3;
							Method2(v9 + ((g_unk52FD14 >> 2) & 4) + 2 * (g_unk52FD14 & 0xC));
						}
						else
						{
							Method2(g_unk52FD14 ^ v6);
						}
					}

					v4 += 4;
					--v5;
				} while (v5);

				v10 = &g_unk4F3FCC[2];
				v11 = 4;

				do
				{
					if ((Platform::GetFlags(*(v10 - 2)) & 3) == 2)
					{
						Method6(*(v10 - 1));
						v53 = v10[1];
						g_unk52FD30 = -1600 * *v10;
						Levels::Method3(v53, 1);
					}

					v10 += 4;
					--v11;
				} while (v11);
			}

			Method3();
			Method4();

			v12 = g_unk52FDAC;

			if (g_unk52FDFC < -64000 && g_unk52FDAC)
			{
				g_unk52FDAC = 0;
				Nu3D::Link::SetScaleFromFixedOffsets(91, 0, 0, 0);
			LABEL_38:
				v12 = g_unk52FDAC;
				goto LABEL_39;
			}

			if (g_unk52FDFC == -64000)
			{
				if (g_unk52FDAC >= 683)
					goto LABEL_44;

				g_unk52FDAC += 2 * (int32_t)Renderer::g_frameDelta;
				v13 = g_unk52FDAC;

				if (g_unk52FDAC > 683)
				{
					v13 = 683;
					g_unk52FDAC = 683;
				}

				Nu3D::Link::SetScaleFromFixedOffsets(
					91,
					Numerics::g_sinCosLUT[(v13 + 341) & 0xFFF] >> 2,
					Numerics::g_sinCosLUT[(v13 + 341) & 0xFFF] >> 2,
					Numerics::g_sinCosLUT[(v13 + 341) & 0xFFF] >> 2);
				goto LABEL_38;
			}
		LABEL_39:

			if (g_unk52FDFC > -64000 && v12)
			{
				v14 = v12 - 2 * (int32_t)Renderer::g_frameDelta;
				g_unk52FDAC = v14;

				if (v14 > 0)
				{
					Nu3D::Link::SetScaleFromFixedOffsets(
						91,
						Numerics::g_sinCosLUT[(v14 + 341) & 0xFFF] >> 2,
						Numerics::g_sinCosLUT[(v14 + 341) & 0xFFF] >> 2,
						Numerics::g_sinCosLUT[(v14 + 341) & 0xFFF] >> 2);
				}
				else
				{
					g_unk52FDAC = 0;
					Nu3D::Link::SetScaleFromFixedOffsets(91, 0, 0, 0);
				}
			}
		LABEL_44:

			Method5();

			p_v1.x = 183342;
			p_v1.y = 192043;
			p_v1.z = -195587;

			if (Nu3D::Math::IsWithinDistance(&p_v1, &g_buzzActor.posAngles.pos, 128))
			{
				Method6(16);
				g_unk52FD30 = 0;
			}

			Method7(13, 39, -2600, &g_unk52FD90, &g_unk52FD28);
			Method7(14, 40, -2600, &g_unk52FD7C, &g_unk52FD1C);
			Method7(15, 41, -66600, &g_unk52FD88, &g_unk52FD20);
			Method7(16, 42, -2600, &g_unk52FD50, &g_unk52FD18);

			Method8();

			v15 = g_unk52FD74;

			if (g_unk52FD74)
			{
				v16 = g_unk52FD44;

				if (g_unk52FD74 == 24)
				{
					Nu3D::Link::GetCurrentPosFixed(g_unk52FD44, &v60);
					Nu3D::Link::SetPositionRawAndCommit(49, v60.x >> 5, v60.y >> 5, v60.z >> 5);
					Nu3D::Link::SetScaleFromFixedOffsets(v16, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(49, 4096, 4096, 4096);
					v15 = g_unk52FD74;
				}

				v17 = (int32_t)Renderer::g_frameDelta;
				v18 = v15 - (int32_t)Renderer::g_frameDelta;
				g_unk52FD74 = v18;

				if (v18 >= 0)
				{
					if (v18)
						goto LABEL_54;
				}
				else
				{
					g_unk52FD74 = 0;
				}

				Nu3D::Link::SetScaleFromFixedOffsets(v16, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(49, 0, 0, 0);
			}

			v17 = (int32_t)Renderer::g_frameDelta;
		LABEL_54:
			v19 = v17 + g_unk52FD8C;

			g_unk52FD8C += v17;

			if (g_unk52FD8C > 300)
				g_unk52FD8C = v19 + 1000;

			g_unk52FDF8 = (g_unk52FDF8 + 32 * v17) & 0x3FF;

			Method9(3, 50, 59, 1);
			Method9(2, 51, 60, 2);
			Method9(5, 52, 61, 8);
			Method9(4, 53, 62, 16);
			Method9(1, 54, 63, 32);
			Method9(2, 58, 65, 4);

			if (g_unk52FD8C > 1000)
				g_unk52FD8C -= 1300;

			if (g_buzzActor.collisionFlags && (Platform::GetFlags(12) & 3) == 2 && ((int16_t*)Platform::GetContactFace32(12))[2] < -12288)
			{
				Levels::Method3(13, 1);

				if (Buzz::g_groundSlamTicker)
				{
					Buzz::g_groundSlamTicker = 0;
					Levels::Method2(-3072, 2);
				}
				else
				{
					Levels::Method2(-2432, 2);
				}

				AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
			}

			v20 = g_unk52FD54;

			if (g_unk52FD54)
			{
				if (g_unk52FD54 == 1)
				{
					Platform::SetRotationAngles(23, 0, 942, 0);
					v20 = g_unk52FD54;
				}

				Nu3D::Link::SetRotationRelative8bit(25, 0, Numerics::g_sinCosLUT[v20 & 0xFFF] / 18, 0);
				Nu3D::Link::SetRotationRelative8bit(26, 0, Numerics::g_sinCosLUT[g_unk52FD54 & 0xFFF] / 18, 0);

				if (g_unk52FD54 < 1024)
					g_unk52FD54 += 16 * (int32_t)Renderer::g_frameDelta;
			}

			v21 = g_unk52FD38;

			if (g_unk52FD38)
			{
				if (g_unk52FD38 == 1)
				{
					Platform::SetRotationAngles(22, 0, -1060, 0);
					v21 = g_unk52FD38;
				}

				Nu3D::Link::SetRotationRelative8bit(28, 0, -2 * Numerics::g_sinCosLUT[v21 & 0xFFF] / 33, 0);
				Nu3D::Link::SetRotationRelative8bit(90, 0, -2 * Numerics::g_sinCosLUT[g_unk52FD38 & 0xFFF] / 33, 0);

				if (g_unk52FD38 < 1024)
					g_unk52FD38 += 16 * (int32_t)Renderer::g_frameDelta;
			}

			Nu3D::Link::GetCurrentPosFixed(81, &p_v1);
			Nu3D::Link::SetPositionRawAndCommit(87, p_v1.x >> 7, p_v1.y >> 7, p_v1.z >> 7);

			Nu3D::Link::GetCurrentPosFixed(29, &p_v1);
			Nu3D::Link::SetPositionRawAndCommit(64, p_v1.x >> 7, p_v1.y >> 7, p_v1.z >> 7);

			v22 = 0;

			for (i = 0; i < 6; ++i)
			{
				v24 = g_unk52FD58[i];

				if (v24 > 0)
				{
					v25 = v24 - (int32_t)Renderer::g_frameDelta;
					g_unk52FD58[i] = v25;

					if (v25 <= 0)
					{
						g_unk52FD58[i] = -1;
						Method12(i * 4);
						g_unk52FD48 |= 1 << v22;
					}
				}

				++v22;
			}

			Actor::CollectQuestReward(0, 15, -1, 0, 0);
			Actor::RotatingHint(12, 20, g_unk4F3F04);
			Actor::PlayPeriodicHintSound(1, 178);

			if ((g_levelActors[1].actorFlags & Actor::ACTOR_PLAYER_TRIGGER) != 0)
			{
				g_levelActors[1].actorFlags &= ~0x200u;

				if (g_collectionCount >= 0)
				{
					if (g_collectionCount == 5)
					{
						AudioManager::Preset::PlayOneShotSound2(180, &g_levelActors[1]);
						Dialogue::Begin(1, 17, "thanks for finding my ^critters^ buzz! here is a pizza planet ^token^!", -1, 0, 1);
						g_collectionCount = -1;
					}
					else
					{
						AudioManager::Preset::PlayOneShotSound2(179, &g_levelActors[1]);
						Dialogue::Begin(
							1,
							17,
							"howdy buzz! all my ^critters^ have escaped! if you can find all ^five^ of them and come and see me, i will give you a pizza "
							"planet ^token^.",
							-1,
							0,
							-1);
					}
				}
			}

			if ((g_levelActors[2].actorFlags & 0x200) != 0)
			{
				g_levelActors[2].actorFlags &= ~0x200u;

				if (g_unk52F2F8)
				{
					if (g_unk830D4C >= 5)
					{
						if (g_unk52F2F8 != 3)
						{
							AudioManager::PlaySoundEffect(145, &g_levelActors[2].transform.pos);
							Dialogue::Begin(2, 18, "well done! you found all of my horseshoes! here is your pizza planet ^token^!", -1, 0, 2);
							g_unk52F2F8 = 3;
							goto LABEL_107;
						}
					}
					else
					{
						AudioManager::PlaySoundEffect(145, &g_levelActors[2].transform.pos);
						Dialogue::Begin(2, 18, "quick! you need to find more horseshoes!", -1, 0, -1);
					}
				}
				else
				{
					AudioManager::PlaySoundEffect(145, &g_levelActors[2].transform.pos);

					v27 = 0;

					Dialogue::Begin(
						2,
						18,
						"howdy buzz! if you can bring me ^five^ horseshoes before you run out of time, i will give you a pizza planet ^token^.",
						-1,
						0,
						-1);
					g_unk830D4C = 0;
					g_unk52F2F8 = 1;

					do
					{
						*g_unk52FDC8[v27].yPtr = g_unk52FDC8[v27].savedY;
						Nu3D::Link::SetScaleFromFixedOffsets(v27 + 101, 4096, 4096, 4096);
						++v27;
					} while (v27 < 5);

					v57 = v27;
				}
			}

			if (g_unk52F2F8 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_107;

				g_unk52F2F8 = 2;
				g_unk52AD64 = 127;
			}
			else if (g_unk52F2F8 != 2)
			{
				goto LABEL_107;
			}

			if (g_curSectorCopy1 != 1 && g_curSectorCopy1 != 4 && g_curSectorCopy1 != 8)
				g_unk52AD64 = 99;

			if (g_tickState[11])
				--g_unk52AD64;

			if (g_unk52AD64 < 100)
			{
				v29 = 0;
				g_unk52AD64 = 100;
				g_unk52F2F8 = 0;

				do
				{
					*g_unk52FDC8[v29].yPtr = 0x80000000;
					Nu3D::Link::SetScaleFromFixedOffsets(v29 + 101, 0, 0, 0);
					v57 = ++v29;
				} while (v29 < 5);
			}
		LABEL_107:

			if ((Camera::g_inCutscene & 1) != 0)
				g_unk52FD8C = 0;

			if (g_curSectorCopy1 == 2)
			{
				g_unk52FD80 -= (int32_t)Renderer::g_frameDelta;

				if (g_unk52FD80 < 0)
				{
					v31 = ++g_unk52FDF4;

					if (g_unk52FDF4 < 2)
					{
						if (v31)
						{
							v32 = 384;
							v33 = -378850;
							v34 = -348753;
							v35 = 384;
							goto LABEL_114;
						}
					}
					else
					{
						g_unk52FDF4 = 0;
					}

					v32 = -384;
					v33 = -386992;
					v34 = -356895;
					v35 = -384;
				LABEL_114:

					v54 = *g_randDatBufferPtr++ - 128;
					Nu3D::Particles::SpawnInstance(v33, 100264, v34, v32, -2048, v35, 0, 0, v54, 76);
					g_unk52FD80 = 100;
				}
			}

			if (! g_unk52FD54)
				g_unk52FD54 = g_levelActors[10].actorPhase != 1;

			if (! g_unk52FD38)
				g_unk52FD38 = g_levelActors[9].actorPhase != 1;

			v36 = g_unk52FD40;

			if (! g_unk52FD40)
			{
				if (Actor::IsInsideBounds((Actor::Toy2Actor*)&g_buzzActor, -726990, -656078, -241532, -177452) && g_buzzActor.posAngles.pos.y < 128072)
				{
					g_unk52FD40 = 1;
					Dialogue::Begin(11, 16, "ha ha ha ha ... defeat the ^gunslinger^ boss to get a pizza planet ^token^!", -1, 0, -1);
				}
				v36 = g_unk52FD40;
			}

			if (v36 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_134;

				g_unk830D44 = 180;
				g_unk52FD40 = 2;

				g_levelActors[11].movementData = Actor::g_movementData[33] + 14;
				g_levelActors[11].unkShort1 = 0;
				g_levelActors[11].creatureRam->initialFacingAngle = 0;

				v36 = g_unk52FD40;
			}

			if (v36 >= 3)
			{
				if (v36 >= 120)
				{
					if (v36 != 200)
					{
						Collectables::Activate(4, 0);
						g_unk52FD40 = 200;
					}
				}
				else
				{
					g_unk52FD40 = (int32_t)Renderer::g_frameDelta + v36;
				}
			}
		LABEL_134:

			if (g_curSectorCopy2 == 1)
			{
				if (g_tickState[2])
				{
					p_v1.x = 71804;
					p_v1.y = 179602;
					p_v1.z = -80754;

					if (Nu3D::Math::IsWithinDistance(&Camera::g_activeCameraTransform.pos, &p_v1, 768))
					{
						p_v1.x += 32 * (*g_randDatBufferPtr++ - 128);
						v37 = (*g_randDatBufferPtr++ - 128) << 6;
						p_v1.z += v37;
						Nu3D::Particles::SpawnFromPreset(p_v1.x, p_v1.y, p_v1.z, 101, 10);
					}
				}
			}

			if (g_curSectorCopy1 != 4)
			{
				v38 = Levels::g_recordData[19];
				v56 = 0x7FFFFFFF;
				v55 = 0;

				if (! Levels::g_recordData[19]->recordCount)
					goto LABEL_147;

				do
				{
					v40 = 32 * v38->data[v55].y;
					v41 = 32 * v38->data[v55].z;
					v42 = 32 * v38->data[v55].x;

					if (((Camera::g_activeCameraTransform.pos.x - v42) >> 8) * ((Camera::g_activeCameraTransform.pos.x - v42) >> 8)
							+ ((Camera::g_activeCameraTransform.pos.z - v41) >> 8) * ((Camera::g_activeCameraTransform.pos.z - v41) >> 8)
							+ ((Camera::g_activeCameraTransform.pos.y - v40) >> 8) * ((Camera::g_activeCameraTransform.pos.y - v40) >> 8)
						< 1000000)
					{
						Renderer::LensFlare::RegisterLight(v42, v40, v41, 32, 32, 32, 128);

						v43 = g_buzzActor.posAngles.pos.x - v42;
						v44 = ((g_buzzActor.posAngles.pos.z - v41) >> 8) * ((g_buzzActor.posAngles.pos.z - v41) >> 8)
							+ ((g_buzzActor.posAngles.pos.y - v40 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v40 - 0x2000) >> 8);

						v45 = (v43 >> 8) * (v43 >> 8);

						if (v44 + v45 < v56)
						{
							v56 = v44 + v45;
							v57 = v55;
						}
					}

					v38 = Levels::g_recordData[19];
					v46 = ++v55 < Levels::g_recordData[19]->recordCount;
				} while (v46);

				if (v56 < 0x10000)
				{
					Lighting::g_dynamicLights[1].pos.x = 32 * Levels::g_recordData[19]->data[v57].x;
					Lighting::g_dynamicLights[1].pos.y = 32 * Levels::g_recordData[19]->data[v57].y;
					z = Levels::g_recordData[19]->data[v57].z;
					Lighting::g_dynamicLights[1].lifetime = 1;
					Lighting::g_dynamicLights[1].pos.z = 32 * z;
					Lighting::g_dynamicLights[1].colorR = 127;
					Lighting::g_dynamicLights[1].colorG = 127;
					Lighting::g_dynamicLights[1].colorB = 127;
					Lighting::g_dynamicLights[1].groupId = (int32_t)&Levels::g_recordData[19]->data[v57];
				}
				else
				{
				LABEL_147:
					Lighting::g_dynamicLights[1].lifetime = 0;
				}
			}

			if (g_curSectorCopy2 == 5)
			{
				Renderer::BlitTextureByIndexOffset(7, 128, 160, 32, 32, 0, (Numerics::g_sinCosLUT[g_unk52FDA8] >> 9) & 0x1F, 32, 0);
				g_unk52FDA8 = (g_unk52FDA8 + 8 * (int32_t)Renderer::g_frameDelta) & 0xFFF;
			}

			if ((g_levelActors[31].actorFlags & 2) != 0 && g_levelActors[31].transform.pos.y > MoveableObject::g_moveableObjects[1].pos.y - 15360)
			{
				v49 = (MoveableObject::g_moveableObjects[1].pos.z - g_levelActors[31].transform.pos.z) >> 5;
				if (((MoveableObject::g_moveableObjects[1].pos.x - g_levelActors[31].transform.pos.x)
					 >> 5) * ((MoveableObject::g_moveableObjects[1].pos.x - g_levelActors[31].transform.pos.x) >> 5)
						+ v49 * v49
					< 490000)
				{
					if (g_levelActors[31].transform.pos.y >= MoveableObject::g_moveableObjects[1].pos.y - 13312)
					{
						v50 = Nu3D::Math::CartesianToFixedAngle((MoveableObject::g_moveableObjects[1].pos.x - g_levelActors[31].transform.pos.x) >> 5, v49)
							& 0xFFF;

						v51 = Numerics::g_sinCosLUT[(v50 + 1024) & 0xFFF];

						g_levelActors[31].transform.pos.x = MoveableObject::g_moveableObjects[1].pos.x - ((700 * Numerics::g_sinCosLUT[v50]) >> 9);
						g_levelActors[31].transform.pos.z = MoveableObject::g_moveableObjects[1].pos.z - ((700 * v51) >> 9);
					}
					else
					{
						g_levelActors[31].transform.pos.y = MoveableObject::g_moveableObjects[1].pos.y - 15360;
					}
				}
			}

			if (g_curSectorCopy2 == 4)
			{
				g_unk52FDB0 -= (int32_t)Renderer::g_frameDelta;

				if (g_unk52FDB0 <= 0)
				{
					g_unk52FDB0 = 60;
					v52 = g_unk52FDC4 + 3;

					g_unk52FDC4 += 3;

					if (g_unk52FDC4 >= 12)
					{
						v52 = 0;
						g_unk52FDC4 = 0;
					}

					Nu3D::Particles::SpawnFromPreset(g_unk4F3ED4[v52], g_unk4F3ED4[v52 + 1], g_unk4F3ED4[v52 + 2], 116, 2);
				}
			}

			if (Buzz::g_curMoveableObject == 2)
				Levels::Method3(1, 0);

			PlayLevelMusic();
		}

	}
}

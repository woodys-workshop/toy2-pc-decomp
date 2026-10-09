#include "Toy2/Levels/TarmacTrouble.h"
#include "Toy2/Toy2.h"
#include "Toy2/Actor.h"
#include "Toy2/Buzz.h"
#include "Toy2/Camera.h"
#include "Toy2/Collectables.h"
#include "Toy2/Dialogue.h"
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
#include "AudioManager/AudioManager.h"
#include "InputManager.h"
#include "Random.h"
#include "Numerics.h"
#include "SaveManager.h"
#include "Nullsub.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace Toy2
{
	namespace TarmacTrouble
	{
		// GLOBAL: TOY2 0x004F4C54
		char* g_unk4F4C54[5] = {
			"i think i saw ^hamm^ in zone ^5^.",
			"the ^airport little tike^ has lost his ^luggage^. he is in zone ^3^.",
			"^slinky^ has a ^challenge^ for you. he is in the middle of the airport next to the central pole. by the way, you can jump on the plane from the "
			"central pole.",
			"there is a ^token^ being carried by the ^helicopter^. signal the ^helicopter^ to land by solving the ^lights^ puzzle in zone ^1^. you can use the "
			"^zipline^ at the end of this trolley to reach the puzzle area.",
			"the ^blacksmith^ boss is on the roof of the building in zone ^7^. he is near the radar tower. try and reach it by running down the wings of the "
			"^plane^."
		};

		// GLOBAL: TOY2 0x0052FF48
		int32_t g_unk52FF48;

		// GLOBAL: TOY2 0x0052FF70
		int32_t g_unk52FF70;

		// GLOBAL: TOY2 0x0052FF90
		int32_t g_unk52FF90;

		// GLOBAL: TOY2 0x0052FFB0
		int32_t g_unk52FFB0;

		// GLOBAL: TOY2 0x0052FFC8
		int32_t g_unk52FFC8;

		// GLOBAL: TOY2 0x0052FFD0
		int32_t g_unk52FFD0;

		// GLOBAL: TOY2 0x0052FF50
		Vector3I g_unk52FF50;

		// GLOBAL: TOY2 0x0052FF98
		Vector3I g_unk52FF98;

		// GLOBAL: TOY2 0x0052FFD8
		Vector3I g_unk52FFD8;

		// GLOBAL: TOY2 0x004F4C68
		int16_t g_unk4F4C68[6] = { 119, 118, 117, 116, 115, 0 };

		// GLOBAL: TOY2 0x004F4C40
		Collectables::TokenTableEntry g_unk4F4C40[2] = {
			{ 150,
			  30,
			  "use the ^buttons^ to change the bottom row of lights to match the top row of lights. the puzzle will reset if you take more than ^3^ tries. if "
			  "you complete the puzzle the helicopter will move down so that you can reach the pizza planet ^token^.",
			  2048 },
			{ -1, 0, 0, 0 },
		};

		// GLOBAL: TOY2 0x0052FF80
		Vector3I g_unk52FF80;

		// GLOBAL: TOY2 0x0052FF60
		int32_t g_unk52FF60;

		// GLOBAL: TOY2 0x0052FFB8
		int32_t g_unk52FFB8;

		// GLOBAL: TOY2 0x0052FF64
		int32_t g_unk52FF64;

		// GLOBAL: TOY2 0x0052FFBC
		int32_t g_unk52FFBC;

		// GLOBAL: TOY2 0x0052FF68
		int32_t g_unk52FF68;

		// GLOBAL: TOY2 0x0052FFC0
		int32_t g_unk52FFC0;

		// GLOBAL: TOY2 0x0052FFB4
		int32_t g_unk52FFB4;

		// GLOBAL: TOY2 0x0052FFEC
		int32_t g_unk52FFEC;

		// GLOBAL: TOY2 0x0052FF7C
		int32_t g_unk52FF7C;

		// GLOBAL: TOY2 0x0052FFD4
		int32_t g_unk52FFD4;

		// GLOBAL: TOY2 0x0052FFCC
		int32_t g_unk52FFCC;

		// GLOBAL: TOY2 0x0052FFA8
		int32_t g_unk52FFA8;

		// GLOBAL: TOY2 0x0052FF4C
		int32_t g_unk52FF4C;

		// GLOBAL: TOY2 0x0052FF74
		int32_t g_unk52FF74;

		// GLOBAL: TOY2 0x0052FF78
		int32_t g_unk52FF78;

		// GLOBAL: TOY2 0x0052FFAC
		int32_t g_unk52FFAC;

		// GLOBAL: TOY2 0x0052FFE8
		int32_t g_unk52FFE8;

		// FUNCTION: TOY2 0x0042D710
		int32_t Method3(int32_t a1)
		{
			int32_t v1;
			int32_t v2;
			int32_t v4;

			v1 = 0;
			switch (a1)
			{
				case 0:
					v1 = (g_unk52FFD0 & 1) == 0;
					if ((g_unk52FFD0 & 8) != 0)
						++v1;
					g_unk52FFD0 = g_unk52FFD0 & 0xF6 | 1;
					g_unk52FF48 = 76;
					g_unk52FF90 = 12;
					Nu3D::Link::SetScaleFromFixedOffsets(80, 4096, 4096, 4096);
					goto LABEL_14;
				case 1:
					v2 = (g_unk52FFD0 & 2) == 0;
					if ((g_unk52FFD0 & 1) != 0)
						++v2;
					g_unk52FFD0 = g_unk52FFD0 & 0xFC | 2;
					g_unk52FF48 = 77;
					g_unk52FF90 = 12;
					Nu3D::Link::SetScaleFromFixedOffsets(81, 4096, 4096, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(g_unk52FF48, 0, 0, 0);
					return v2;
				case 2:
					v4 = (g_unk52FFD0 & 4) == 0;
					if ((g_unk52FFD0 & 2) != 0)
						++v4;
					g_unk52FFD0 = g_unk52FFD0 & 0xF9 | 4;
					g_unk52FF48 = 78;
					g_unk52FF90 = 12;
					Nu3D::Link::SetScaleFromFixedOffsets(82, 4096, 4096, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(g_unk52FF48, 0, 0, 0);
					return v4;
				case 3:
					v1 = (g_unk52FFD0 & 8) == 0;
					if ((g_unk52FFD0 & 4) != 0)
						++v1;
					g_unk52FFD0 = g_unk52FFD0 & 0xF3 | 8;
					g_unk52FF48 = 79;
					g_unk52FF90 = 12;
					Nu3D::Link::SetScaleFromFixedOffsets(83, 4096, 4096, 4096);
				LABEL_14:
					Nu3D::Link::SetScaleFromFixedOffsets(g_unk52FF48, 0, 0, 0);
					return v1;
				default:
					return v1;
			}
		}

		// FUNCTION: TOY2 0x0042D8B0
		void Method4()
		{
			if ((g_unk52FFD0 & 8) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(43, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(35, 0, 0, 0);
			}
			else
			{
				Nu3D::Link::SetScaleFromFixedOffsets(35, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(43, 0, 0, 0);
			}
			if ((g_unk52FFD0 & 4) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(44, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(36, 0, 0, 0);
			}
			else
			{
				Nu3D::Link::SetScaleFromFixedOffsets(36, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(44, 0, 0, 0);
			}
			if ((g_unk52FFD0 & 2) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(51, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(37, 0, 0, 0);
			}
			else
			{
				Nu3D::Link::SetScaleFromFixedOffsets(37, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(51, 0, 0, 0);
			}
			if ((g_unk52FFD0 & 1) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(52, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(38, 0, 0, 0);
			}
			else
			{
				Nu3D::Link::SetScaleFromFixedOffsets(38, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(52, 0, 0, 0);
			}
		}

		// FUNCTION: TOY2 0x0042D9C0
		void Method5()
		{
			if ((g_unk52FFD0 & INPUT_LEFT) == 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(31, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(39, 0, 0, 0);
			}
			else
			{
				Nu3D::Link::SetScaleFromFixedOffsets(39, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(31, 0, 0, 0);
			}

			if ((g_unk52FFD0 & INPUT_DOWN) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(40, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(32, 0, 0, 0);
			}
			else
			{
				Nu3D::Link::SetScaleFromFixedOffsets(32, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(40, 0, 0, 0);
			}

			if ((g_unk52FFD0 & INPUT_RIGHT) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(41, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(33, 0, 0, 0);
			}
			else
			{
				Nu3D::Link::SetScaleFromFixedOffsets(33, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(41, 0, 0, 0);
			}

			if ((g_unk52FFD0 & INPUT_UP) != 0)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(42, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(34, 0, 0, 0);
			}
			else
			{
				Nu3D::Link::SetScaleFromFixedOffsets(34, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(42, 0, 0, 0);
			}
		}

		// FUNCTION: TOY2 0x0042DB10
		void Method2()
		{
			uint8_t* v0;
			bool v1;
			int32_t v2;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			int32_t i;
			int32_t v7;
			int32_t v8;

			do
			{
				v0 = g_randDatBufferPtr + 1;
				v1 = g_randDatBufferPtr + 1 <= &g_randDatBuffer[1500];
				g_unk52FFD0 = 17 * (*g_randDatBufferPtr++ & 0xF);

				if (! v1)
				{
					v0 -= 1500;
					g_randDatBufferPtr = v0;
				}

				v2 = *v0 & 3;
				g_randDatBufferPtr = v0 + 1;

				v3 = Method3(v2);
				v8 = *g_randDatBufferPtr++ & 3;

				v4 = Method3(v8) + v3;
				v7 = *g_randDatBufferPtr++ & 3;

				v5 = Method3(v7) + v4;
			} while ((g_unk52FFD0 & 0xF0) == 16 * (g_unk52FFD0 & 0xF) || v5 < 6);

			Nu3D::Link::SetScaleFromFixedOffsets(76, 4096, 4096, 4096);
			Nu3D::Link::SetScaleFromFixedOffsets(77, 4096, 4096, 4096);
			Nu3D::Link::SetScaleFromFixedOffsets(78, 4096, 4096, 4096);
			Nu3D::Link::SetScaleFromFixedOffsets(79, 4096, 4096, 4096);

			Nu3D::Link::SetScaleFromFixedOffsets(80, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(81, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(82, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(83, 0, 0, 0);

			g_unk52FF90 = 0;
			g_unk52FF70 = 3;
			g_unk52FFD0 = ((g_unk52FFD0 >> 4) & 0xF) + 16 * (g_unk52FFD0 & 0xF);

			Method4();
			Method5();

			for (i = 54; i < 58; ++i)
			{
				if (i == g_unk52FF70 + 54)
					Nu3D::Link::SetScaleFromFixedOffsets(i, 4096, 4096, 4096);
				else
					Nu3D::Link::SetScaleFromFixedOffsets(i, 0, 0, 0);
			}
		}

		// FUNCTION: TOY2 0x0042DCB0
		void Method1()
		{
			int32_t v0;
			int32_t v1;
			int32_t v2;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			int32_t v6;
			int32_t v7;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			int16_t v11;
			int32_t v12;
			int32_t v13;
			int32_t y;
			int32_t v15;
			int32_t v16;
			int32_t v17;
			int32_t v18;
			int32_t v19;
			int32_t v20;
			int32_t v21;
			int32_t v22;
			Vector3I p_v2;
			Vector3I a2;

			Platform::GetRotation(0, &g_unk52FF80);
			Platform::SetAngularVelocity(0, 0, 4 * (int32_t)Renderer::g_frameDelta, 0);
			g_unk52FF80.y >>= 2;
			Nu3D::Link::GetCurrentPosFixed(0, &p_v2);
			Platform::GetOrigin(0, &a2);
			a2.x = (3 * (p_v2.x - a2.x)) >> 2;
			a2.z = (3 * (p_v2.z - a2.z)) >> 2;
			Platform::SetVelocity(0, a2.x, 0, a2.z);
			v18 = g_unk52FF64 - g_unk52FFBC;
			v0 = Numerics::g_sinCosLUT[((uint16_t)(g_unk52FF80.y) + 1024) & 0xFFF];
			v1 = Numerics::g_sinCosLUT[-g_unk52FF80.y & 0xFFF];
			v20 = ((v0 * (g_unk52FF60 >> 8) - v1 * (g_unk52FF68 >> 8)) >> 6) - g_unk52FFB8;
			v2 = 0;
			v16 = ((v0 * (g_unk52FF68 >> 8) + (g_unk52FF60 >> 8) * v1) >> 6) - g_unk52FFC0;
			do
			{
				if (v2 == 3)
				{
					v2 = 4;
				}
				else if (v2 == 16)
				{
					v2 = 84;
				}
				Nu3D::Link::GetTargetPosFixed(v2, &p_v2);
				v3 = Numerics::g_sinCosLUT[((uint16_t)(g_unk52FF80.y) + 1024) & 0xFFF];
				v4 = Numerics::g_sinCosLUT[-g_unk52FF80.y & 0xFFF];
				v5 = (p_v2.z - g_unk52FFC0) >> 4;
				v6 = (p_v2.x - g_unk52FFB8) >> 4;
				v7 = v4 * v6;
				v8 = v3 * v6 - v4 * v5;
				v9 = g_unk52FFC0 + ((v3 * v5 + v7) >> 10) + v16;
				v10 = g_unk52FFB8 + (v8 >> 10) + v20;
				Nu3D::Link::SetPositionRawAndCommit(v2, v10 >> 5, (v18 + p_v2.y) >> 5, v9 >> 5);
				switch (v2)
				{
					case 8:
					case 9:
						Nu3D::Link::SetRotationRelative8bit(v2, 0, g_unk52FF80.y, 807 * g_unk52FF80.y);
						break;
					case 10:
						Nu3D::Link::SetRotationRelative8bit(10, 0, g_unk52FF80.y + 1024, -15 * g_unk52FF80.y);
						p_v2.x = v10;
						p_v2.y += 3072;
						p_v2.z = v9;
						if (! Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &p_v2, 80))
							break;
					LABEL_16:
						v11 = Nu3D::Math::CartesianToFixedAngle(g_buzzActor.posAngles.pos.x - p_v2.x, g_buzzActor.posAngles.pos.z - p_v2.z);
						Buzz::HandleDamage(v11, 3u);
						break;
					case 11:
						Nu3D::Link::SetRotationRelative8bit(11, 0, g_unk52FF80.y + 1024, -21 * g_unk52FF80.y);
						p_v2.x = v10;
						p_v2.z = v9;
						p_v2.y += 3072;
						if (! Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &p_v2, 80))
							break;
						goto LABEL_16;
					case 13:
						Nu3D::Link::SetRotationRelative8bit(13, 0, g_unk52FF80.y + 700, -18 * g_unk52FF80.y);
						p_v2.x = v10;
						p_v2.z = v9;
						p_v2.y += 3072;
						if (! Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &p_v2, 80))
							break;
						goto LABEL_16;
					default:
						Nu3D::Link::SetRotationRelative8bit(v2, 0, g_unk52FF80.y, 0);
						break;
				}
				++v2;
			} while (v2 < 87);
			v12 = 16;
			v21 = v20 >> 2;
			v19 = v18 >> 2;
			v17 = v16 >> 2;
			while (2)
			{
				Nu3D::Link::GetTargetPosFixed(v12, &p_v2);
				v13 = Numerics::g_sinCosLUT[((uint16_t)(g_unk52FF80.y) + 1024) & 0xFFF];
				v22 = Numerics::g_sinCosLUT[-g_unk52FF80.y & 0xFFF];
				Nu3D::Link::SetPositionRawAndCommit(
					v12,
					(v21 + (g_unk52FFB8 >> 2) + ((v13 * (p_v2.x - (g_unk52FFB8 >> 2)) - v22 * (p_v2.z - (g_unk52FFC0 >> 2))) >> 14)) >> 5,
					(v19 + p_v2.y) >> 5,
					(v17 + (g_unk52FFC0 >> 2) + ((v13 * (p_v2.z - (g_unk52FFC0 >> 2)) + v22 * (p_v2.x - (g_unk52FFB8 >> 2))) >> 14)) >> 5);
				switch (v12)
				{
					case 23:
					case 24:
						v15 = 807 * g_unk52FF80.y;
						y = g_unk52FF80.y;
						break;
					case 25:
						Nu3D::Link::SetRotationRelative8bit(25, 0, g_unk52FF80.y + 1024, -15 * g_unk52FF80.y);
						goto LABEL_32;
					case 26:
						Nu3D::Link::SetRotationRelative8bit(26, 0, g_unk52FF80.y + 1024, -21 * g_unk52FF80.y);
						goto LABEL_32;
					case 28:
						Nu3D::Link::SetRotationRelative8bit(28, 0, g_unk52FF80.y + 700, -18 * g_unk52FF80.y);
						goto LABEL_32;
					default:
						v15 = 0;
						y = g_unk52FF80.y;
						break;
				}
				Nu3D::Link::SetRotationRelative8bit(v12, 0, y, v15);
			LABEL_32:
				if (++v12 < 31)
					continue;
				break;
			}
		}

		// FUNCTION: TOY2 0x0042E1D0
		void Method6()
		{
			int32_t* v0;
			Vector3I v1;

			Nu3D::Link::GetTargetPosFixed(3, &v1);
			Nu3D::Link::SetPositionRawAndCommit(3, v1.x >> 5, (v1.y + g_unk52FF7C + Numerics::g_sinCosLUT[g_unk52FFEC & 0xFFF] / 2) >> 5, v1.z >> 5);
			Nu3D::Link::GetTargetPosFixed(48, &v1);
			Nu3D::Link::SetPositionRawAndCommit(48, v1.x >> 5, (v1.y + g_unk52FF7C + Numerics::g_sinCosLUT[g_unk52FFEC & 0xFFF] / 2) >> 5, v1.z >> 5);
			Nu3D::Link::GetTargetPosFixed(66, &v1);
			Nu3D::Link::SetPositionRawAndCommit(66, v1.x >> 5, (v1.y + g_unk52FF7C + Numerics::g_sinCosLUT[g_unk52FFEC & 0xFFF] / 2) >> 5, v1.z >> 5);
			Nu3D::Link::GetTargetPosFixed(67, &v1);
			Nu3D::Link::SetPositionRawAndCommit(67, v1.x >> 5, (v1.y + g_unk52FF7C + Numerics::g_sinCosLUT[g_unk52FFEC & 0xFFF] / 2) >> 5, v1.z >> 5);
			Nu3D::Link::GetTargetPosFixed(68, &v1);
			Nu3D::Link::SetPositionRawAndCommit(68, v1.x >> 5, (v1.y + g_unk52FF7C + Numerics::g_sinCosLUT[g_unk52FFEC & 0xFFF] / 2) >> 5, v1.z >> 5);
			Nu3D::Link::GetTargetPosFixed(116, &v1);
			Nu3D::Link::SetPositionRawAndCommit(116, v1.x >> 5, (g_unk52FF7C + Numerics::g_sinCosLUT[g_unk52FFEC & 0xFFF] / 4 + v1.y - 40960) >> 5, v1.z >> 5);
			v0 = &((Levels::Type63*)Levels::g_recordData[63]->data)[g_unk52FFCC].pos.y;
			if (*v0 != 0x80000000)
				*v0 = (g_unk52FF7C + Numerics::g_sinCosLUT[g_unk52FFEC & 0xFFF] / 4 + v1.y - 40960) >> 5;
			Nu3D::Link::GetTargetPosFixed(49, &v1);
			Nu3D::Link::SetPositionRawAndCommit(49, v1.x >> 5, (v1.y + ((g_unk52FF7C + Numerics::g_sinCosLUT[g_unk52FFEC & 0xFFF] / 2) >> 2)) >> 5, v1.z >> 5);
			Nu3D::Link::GetTargetPosFixed(50, &v1);
			Nu3D::Link::SetPositionRawAndCommit(50, v1.x >> 5, (v1.y + ((g_unk52FF7C + Numerics::g_sinCosLUT[g_unk52FFEC & 0xFFF] / 2) >> 2)) >> 5, v1.z >> 5);
			Nu3D::Link::SetRotationRelative8bit(48, 0, 21 * g_unk52FFEC, 0);
			Nu3D::Link::SetRotationRelative8bit(50, 0, 21 * g_unk52FFEC, 0);
			g_unk52FFEC += (int32_t)Renderer::g_frameDelta + 32 * (int32_t)Renderer::g_frameDelta;
		}

		// FUNCTION: TOY2 0x0042E790
		void Interactions()
		{
			char v0;
			int32_t v1;
			int32_t v2;
			bool v3;
			int32_t v4;
			int32_t v5;
			int32_t v6;
			int32_t v7;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			int32_t GroundHeight;
			int32_t v12;
			int32_t v13;
			int32_t v14;
			uint8_t v15;
			int32_t v16;
			uint8_t* v17;
			int32_t v18;
			int32_t v19;
			int32_t v20;
			Levels::RecordData* v21;
			int32_t v22;
			int32_t v23;
			int32_t v24;
			int32_t v25;
			int32_t v26;
			int32_t v27;
			int32_t v28;
			int32_t v29;
			int32_t v30;
			int32_t v31;
			int32_t v32;
			int32_t v34;
			int32_t v35;
			int32_t v36;
			int32_t v37;
			uint32_t recordCount;
			Vector3I p_v2;

			AudioManager::PlaySoundEffect(111, 0);

			Method1();

			p_v2.x = -862912;
			p_v2.y = -63991;
			p_v2.z = -626034;

			if (! Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &p_v2, 500))
				goto LABEL_37;

			if (! g_unk52FFB4)
				Method2();

			v0 = g_unk52FFD0;
			g_unk52FFB4 = 1;

			v1 = g_unk52FF70;

			if ((g_unk52FFD0 & 0xF0) == 16 * (g_unk52FFD0 & 0xF))
			{
				if (g_unk52FF70 >= 0)
				{
					v2 = ((int32_t)Renderer::g_frameDelta + g_unk52FF70) & 0x3F;
					v3 = v2 < 0x14;
					g_unk52FF70 = v2;

					if (v2 > 20)
					{
						if (! g_unk52FFD0)
						{
							g_unk52FFD0 = 255;

							Method4();
							Method5();

							v0 = g_unk52FFD0;
							v2 = g_unk52FF70;
						}

						v3 = v2 < 20;
					}

					if (v3 && v0)
					{
						g_unk52FFD0 = 0;
						Method4();
						Method5();
					}

					if (g_unk52FF7C < 48000)
						g_unk52FF7C += (int32_t)Renderer::g_frameDelta << 7;

					if (g_unk52FFC8 >= 0)
					{
						if (! g_unk52FFC8)
						{
							Nu3D::Link::GetCurrentPosFixed(116, &p_v2);
							Camera::BeginScriptedCutsceneAtPoint(&p_v2, 300, 32);
							Camera::g_cutsceneEyePos.y += 0x8000;
							AudioManager::StartSoundSequenceOnActor(-5, (Actor::Toy2Actor*)&g_buzzActor);
						}

						Nu3D::Link::GetCurrentPosFixed(116, &p_v2);
						Camera::g_cutsceneFocusPos = p_v2;
						g_unk52FFC8 += (int32_t)Renderer::g_frameDelta;

						Nu3D::Link::GetCurrentPosFixed(116, &p_v2);

						if (g_unk52FFC8 >= 300)
							g_unk52FFC8 = -1;
					}
					goto LABEL_37;
				}
				goto LABEL_23;
			}

			if (g_unk52FF70 <= 0)
			{
				if (! g_unk52FF70)
				{
					g_unk52FFD0 = 0;

					Method4();
					Method5();

					v1 = g_unk52FF70;
				}
			LABEL_23:

				g_unk52FF70 = v1 - (int32_t)Renderer::g_frameDelta;

				if (v1 - (int32_t)Renderer::g_frameDelta < -60)
				{
					Method2();

					if (! Camera::g_cutsceneTimer)
						AudioManager::StartSoundSequenceOnActor(-6, (Actor::Toy2Actor*)&g_buzzActor);
				}

				goto LABEL_37;
			}

			if (Buzz::g_groundSlamTicker == -40 && Buzz::g_unk52B81C >= 32 && Buzz::g_unk52B81C <= 35)
			{
				v4 = g_unk52FF70 - 1;
				v5 = 54;

				--g_unk52FF70;
				while (1)
				{
					if (v5 == v4 + 54)
						Nu3D::Link::SetScaleFromFixedOffsets(v5, 4096, 4096, 4096);
					else
						Nu3D::Link::SetScaleFromFixedOffsets(v5, 0, 0, 0);

					if (++v5 >= 58)
						break;

					v4 = g_unk52FF70;
				}

				Levels::Method3(0, 1);
				Levels::Method3(1, 1);
				Levels::Method3(2, 1);
				Levels::Method3(3, 1);

				if (g_unk52FF70 >= 0)
				{
					Method3(Buzz::g_unk52B81C - 32);
					Method4();
				}
			}
		LABEL_37:

			if (g_unk52FF90 > 0)
			{
				g_unk52FF90 -= (int32_t)Renderer::g_frameDelta;
				if (g_unk52FF90 <= 0)
				{
					Nu3D::Link::SetScaleFromFixedOffsets(g_unk52FF48, 4096, 4096, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(g_unk52FF48 + 4, 0, 0, 0);
				}
			}

			Method6();

			g_unk52FFD4 += (int32_t)Renderer::g_frameDelta;
			v6 = Numerics::g_sinCosLUT[(11 * g_unk52FFD4) & 0xFFF];

			Nu3D::Link::SetRotationRelative8bit(
				69, 0, v6 * Numerics::g_sinCosLUT[(17 * g_unk52FFD4) & 0xFFF] / 0x200000, v6 * Numerics::g_sinCosLUT[(7 * g_unk52FFD4) & 0xFFF] / 0x400000);
			v7 = Numerics::g_sinCosLUT[(11 * g_unk52FFD4) & 0xFFF];

			Nu3D::Link::SetRotationRelative8bit(
				70, 0, v7 * Numerics::g_sinCosLUT[(17 * g_unk52FFD4) & 0xFFF] / 0x200000, v7 * Numerics::g_sinCosLUT[(7 * g_unk52FFD4) & 0xFFF] / 0x400000);

			Actor::CollectQuestReward(39, 2, -1, 0, 0);

			Actor::RotatingHint(47, 10, g_unk4F4C54);

			if ((g_levelActors[40].actorFlags & 0x200) != 0)
			{
				g_levelActors[40].actorFlags &= ~0x200u;

				if (g_collectionCount >= 0)
				{
					if (g_collectionCount == 5)
					{
						Dialogue::Begin(40, 3, "thanks for finding my ^luggage^ buzz! here is a pizza planet ^token^!", -1, 0, 1);
						g_collectionCount = -1;
					}
					else
					{
						Dialogue::Begin(
							40,
							3,
							"hi buzz! i have lost ^five^ pieces of ^luggage^. if you can find them for me, i will give you a pizza planet ^token^.",
							-1,
							0,
							-1);
					}
				}
			}

			Actor::PlayPeriodicHintSound(38, 181);

			if ((g_levelActors[38].actorFlags & 0x200) != 0)
			{
				g_levelActors[38].actorFlags &= ~0x200u;

				if (Collectables::g_pickupSlots[2].state != 2)
				{
					if (! g_unk52F2F8)
					{
						AudioManager::Preset::PlayOneShotSound2(182, &g_levelActors[38]);
						Dialogue::Begin(
							38,
							4,
							"hi buzz! if you can reach the end of this path without ^jumping^ or touching the ^green^ slime, i will give you a pizza planet "
							"^token^.",
							-1,
							0,
							2);
						g_unk52F2F8 = 1;

						goto LABEL_51;
					}

					Dialogue::Begin(38, 4, "quick! you can still reach the token!", -1, 0, -1);
				}
			}

			if (g_unk52F2F8 != 1)
			{
				if (g_unk52F2F8 != 2)
					goto LABEL_64;
				goto LABEL_54;
			}
		LABEL_51:

			if (Renderer::g_enableCinematicBars)
				goto LABEL_64;

			g_unk52F2F8 = 2;
			g_unk52AD64 = 170;
		LABEL_54:

			if (Collectables::g_pickupSlots[2].state == 2)
			{
				g_unk52F2F8 = 0;
			}
			else
			{
				if (g_buzzActor.unkWord15 && g_buzzActor.unkWord15 < 5 || ! Buzz::g_unk52B81C)
					g_unk52AD64 = 99;

				if (g_tickState[11])
					--g_unk52AD64;

				if (g_unk52AD64 < 100)
				{
					g_unk52AD64 = 100;
					g_unk52F2F8 = 0;
					Collectables::Deactivate(2);
				}
			}
		LABEL_64:

			Nu3D::Link::GetCurrentPosFixed(0, &p_v2);
			g_levelActors[43].boundary = p_v2;
			g_levelActors[43].pos.x = p_v2.x;
			v8 = g_unk52FFAC;
			g_levelActors[43].pos.z = p_v2.z;

			if (! g_unk52FFAC)
			{
				if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &g_levelActors[46].pos, 300))
				{
					g_unk52FFAC = 1;
					Dialogue::Begin(46, 1, "ha ha ha ha ... defeat the ^blacksmith^ boss to get a pizza planet ^token^!", -1, 0, -1);
				}

				v8 = g_unk52FFAC;
			}

			if (v8 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_76;

				g_unk830D44 = 180;
				g_unk52FFAC = 2;

				g_levelActors[46].movementData = Actor::g_movementData[39] + 14;
				g_levelActors[46].unkShort1 = 0;
				g_levelActors[46].creatureRam->initialFacingAngle = 0;
				v8 = g_unk52FFAC;
			}

			if (v8 >= 3)
			{
				if (v8 >= 120)
				{
					if (v8 != 200)
					{
						Collectables::Activate(4, 0);
						g_unk52FFAC = 200;
					}
				}
				else
				{
					g_unk52FFAC = (int32_t)Renderer::g_frameDelta + v8;
				}
			}
		LABEL_76:

			Nu3D::Link::GetCurrentPosFixed(0, &p_v2);
			g_unk52FF98.x = p_v2.x + 3 * (Camera::g_activeCameraTransform.pos.x - p_v2.x) / 4;
			g_unk52FF98.y = p_v2.y + 3 * (Camera::g_activeCameraTransform.pos.y - p_v2.y) / 4;
			g_unk52FF98.z = p_v2.z + 3 * (Camera::g_activeCameraTransform.pos.z - p_v2.z) / 4;

			AudioManager::PlaySoundEffect(156, &g_unk52FF98);
			Nu3D::Link::GetCurrentPosFixed(68, &p_v2);
			g_unk52FF50.x = p_v2.x + 3 * (Camera::g_activeCameraTransform.pos.x - p_v2.x) / 4;
			g_unk52FF50.y = p_v2.y + 3 * (Camera::g_activeCameraTransform.pos.y - p_v2.y) / 4;
			g_unk52FF50.z = p_v2.z + 3 * (Camera::g_activeCameraTransform.pos.z - p_v2.z) / 4;

			AudioManager::PlaySoundEffect(155, &g_unk52FF50);
			Weather::StepPrecipitation(256, 4096, 51);

			if (g_tickState[8])
			{
				v9 = Numerics::g_sinCosLUT[Camera::g_activeCameraTransform.angles.yaw];
				v10 = Numerics::g_sinCosLUT[(Camera::g_activeCameraTransform.angles.yaw + 1024) & 0xFFF];

				p_v2.x = v9 + Camera::g_activeCameraTransform.pos.x + 2 * v9 + ((*g_randDatBufferPtr++ - 128) << 8);
				p_v2.y = Camera::g_activeCameraTransform.pos.y - 0x8000;
				p_v2.z = v10 + Camera::g_activeCameraTransform.pos.z + 2 * v10 + ((*g_randDatBufferPtr++ - 128) << 8);
				GroundHeight = Nu3D::Collision::GetGroundHeight((WorldPos*)&p_v2, 0);

				if (GroundHeight != 0x80000000 && GroundHeight > Camera::g_activeCameraTransform.pos.y)
					Nu3D::Particles::SpawnFromPreset(p_v2.x, GroundHeight, p_v2.z, 27, 2);
			}

			v12 = g_unk52FFA8;
			v13 = g_unk52FFA8 - (int32_t)Renderer::g_frameDelta;
			g_unk52FFA8 -= (int32_t)Renderer::g_frameDelta;

			if (g_unk52FFA8 < 32)
			{
				if (v12 < 32)
				{
					v17 = g_randDatBufferPtr;
				}
				else
				{
					v14 = *g_randDatBufferPtr++;
					v15 = *g_randDatBufferPtr;
					g_unk52FFB0 = v14;

					v16 = (Camera::g_activeCameraTransform.angles.yaw + v15 - 128) & 0xFFF;
					v17 = ++g_randDatBufferPtr;
					v18 = v14 * Numerics::g_sinCosLUT[v16];
					v19 = v14 * Numerics::g_sinCosLUT[(v16 + 1024) & 0xFFF];

					g_unk52FFD8.x = Camera::g_activeCameraTransform.pos.x + (v18 >> 5);
					g_unk52FFD8.y = Camera::g_activeCameraTransform.pos.y;
					g_unk52FFD8.z = Camera::g_activeCameraTransform.pos.z + (v19 >> 5);
				}

				if (Nu3D::Camera::g_targetTintFadeSpeed)
				{
					Nu3D::Camera::g_cameraTintBlue = 128;
					Nu3D::Camera::g_cameraTintGreen = 128;
					Nu3D::Camera::g_cameraTintRed = 128;
					g_unk52FFA8 = 10000;
				}
				else if (v13 >= 0)
				{
					if (Nu3D::Camera::g_cameraTintRed > 64)
					{
						Nu3D::Camera::g_cameraTintBlue = 3 * v13 + 128;
						Nu3D::Camera::g_cameraTintGreen = 3 * v13 + 128;
						Nu3D::Camera::g_cameraTintRed = 3 * v13 + 128;
					}
				}
				else
				{
					v20 = *v17;
					g_randDatBufferPtr = v17 + 1;
					g_unk52FFA8 = 2 * v20 + 32;
				}
			}

			g_unk52FFB0 -= (int32_t)Renderer::g_frameDelta;

			if (g_unk52FFB0 <= 0)
			{
				g_unk52FFB0 = 20000;
				AudioManager::PlaySoundEffect(112, &g_unk52FFD8);
			}

			v21 = Levels::g_recordData[40];
			v36 = 0x7FFFFFFF;

			if ((g_buzzActor.posAngles.pos.x >> 8) * (g_buzzActor.posAngles.pos.x >> 8)
					+ (g_buzzActor.posAngles.pos.z >> 8) * (g_buzzActor.posAngles.pos.z >> 8)
				>= 0x8E5144)
			{
				v22 = 16;
				v34 = 16;
				recordCount = Levels::g_recordData[40]->recordCount;

				if (recordCount <= 0x10)
				{
				LABEL_112:
					Lighting::g_dynamicLights[1].lifetime = 0;
					PlayLevelMusic();
					return;
				}
			}
			else
			{
				v22 = 0;
				recordCount = 16;
				v34 = 0;
			}

			v35 = v22;

			do
			{
				v23 = 32 * v21->data[v35].y;
				v24 = 32 * v21->data[v35].z;
				v25 = 32 * v21->data[v35].x;

				if (((Camera::g_activeCameraTransform.pos.z - v24) >> 8) * ((Camera::g_activeCameraTransform.pos.z - v24) >> 8)
						+ ((Camera::g_activeCameraTransform.pos.y - v23) >> 8) * ((Camera::g_activeCameraTransform.pos.y - v23) >> 8)
						+ ((Camera::g_activeCameraTransform.pos.x - v25) >> 8) * ((Camera::g_activeCameraTransform.pos.x - v25) >> 8)
					>= 0x40000)
				{
					v32 = v37;
					v21 = Levels::g_recordData[40];
				}
				else
				{
					v26 = 128
						- ((int32_t)sqrt((
							   double)(((Camera::g_activeCameraTransform.pos.z - v24) >> 8) * ((Camera::g_activeCameraTransform.pos.z - v24) >> 8)
									   + ((Camera::g_activeCameraTransform.pos.y - v23) >> 8) * ((Camera::g_activeCameraTransform.pos.y - v23) >> 8)
									   + ((Camera::g_activeCameraTransform.pos.x - v25) >> 8) * ((Camera::g_activeCameraTransform.pos.x - v25) >> 8)))
						   >> 2);

					if (v26 > 0)
					{
						if (v35 < 16)
						{
							v27 = 0;
							v28 = v26;
						}
						else
						{
							v27 = v26;
							v28 = v26;
							v26 = 0;
						}

						Renderer::LensFlare::RegisterLight(v25, v23, v24, v26, v28, v27, 128);
					}

					v29 = g_buzzActor.posAngles.pos.x - v25;
					v21 = Levels::g_recordData[40];

					v30 = ((g_buzzActor.posAngles.pos.z - v24) >> 8) * ((g_buzzActor.posAngles.pos.z - v24) >> 8)
						+ ((g_buzzActor.posAngles.pos.y - v23 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v23 - 0x2000) >> 8);

					v31 = (v29 >> 8) * (v29 >> 8);

					if (v30 + v31 >= v36)
					{
						v32 = v37;
					}
					else
					{
						v36 = v30 + v31;
						v32 = v34;
						v37 = v34;
					}
				}

				v3 = ++v34 < recordCount;
				++v35;
			} while (v3);

			if (v36 >= 0x10000)
				goto LABEL_112;

			Lighting::g_dynamicLights[1].pos.x = 32 * v21->data[v32].x;
			Lighting::g_dynamicLights[1].pos.y = 32 * v21->data[v32].y;
			Lighting::g_dynamicLights[1].pos.z = 32 * v21->data[v32].z;

			if (v32 < 16)
			{
				Lighting::g_dynamicLights[1].colorB = 0;
				Lighting::g_dynamicLights[1].colorR = -1;
				Lighting::g_dynamicLights[1].colorG = -1;
			}
			else
			{
				Lighting::g_dynamicLights[1].colorR = 0;
				Lighting::g_dynamicLights[1].colorG = -1;
				Lighting::g_dynamicLights[1].colorB = -1;
			}

			Lighting::g_dynamicLights[1].lifetime = 1;
			Lighting::g_dynamicLights[1].groupId = (int32_t)&v21->data[v32];

			PlayLevelMusic();
		}

	}
}

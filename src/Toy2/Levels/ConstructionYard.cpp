#include "Toy2/Levels/ConstructionYard.h"
#include "Toy2/Toy2.h"
#include "Toy2/Actor.h"
#include "Toy2/Buzz.h"
#include "Toy2/Camera.h"
#include "Toy2/Collectables.h"
#include "Toy2/Dialogue.h"
#include "Toy2/Gadget.h"
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
#include "Numerics.h"

namespace Toy2
{
	namespace ConstructionYard
	{
		// GLOBAL: TOY2 0x004F1B64
		int32_t g_unk4F1B64[16] = { 1, 48, 2, 49, 4, 50, 0, 0, 1, -48, 2, -49, 4, -50, 0, 0 };

		// GLOBAL: TOY2 0x004F1BA4
		int32_t g_unk4F1BA4[9] = { 0x40004, 0x140000, 0x7F0003, 0x10060, 0x0, 0x140000, 0x7F0003, 0x20, 0xF };

		// GLOBAL: TOY2 0x004F1BC8
		int32_t g_unk4F1BC8[11] = { 0x40004, 0x140001, 0x1000008, 0x7F0003, 0x90060, 0x10100, 0x0, 0x140000, 0x7F0003, 0x20, 0x13 };

		// GLOBAL: TOY2 0x004F1BF4
		int32_t g_unk4F1BF4[10] = { 0x40004, 0x140002, 0x3F0003, 0x10060, 0x0, 0x140000, 0x1000006, 0x7F0003, 0x60, 0x11 };

		// GLOBAL: TOY2 0x004F1C1C
		int32_t g_unk4F1C1C[9] = { 0x40004, 0x140003, 0x7F0003, 0x10060, 0x0, 0x140000, 0x7F0003, 0x20, 0xF };

		// GLOBAL: TOY2 0x004F1C40
		int32_t g_unk4F1C40[9] = { 0x1, 0x3160000, 0x3000C, 0x2000FF, 0x1, 0x0, 0x3000C, 0x2000FF, 0x100000 };

		// GLOBAL: TOY2 0x004F1C64
		int32_t g_unk4F1C64[9] = { 0x1, 0x3160000, 0x3000C, 0x20007F, 0x1, 0x0, 0x3000C, 0x20007F, 0x100000 };

		// GLOBAL: TOY2 0x004F1C88
		int16_t g_unk4F1C88[8] = { 32, 0, 3, 12, 18, 5, -1, 0 };

		// GLOBAL: TOY2 0x004F1C98
		int16_t g_unk4F1C98[6] = { 96, 98, 100, 97, 99, 0 };

		// GLOBAL: TOY2 0x004F1CA4
		Collectables::TokenTableEntry g_unk4F1CA4
			[2] = { { 116, 33, "if you can mix the the paint to match the colors on the wall you will get a pizza planet ^token^.", 0 }, { -1, 0, 0, 0 } };

		// GLOBAL: TOY2 0x004F1CB8
		char* g_unk4F1CB8[5] = {
			"i think i saw hamm over by the ^wheelbarrow^.",
			"the ^foreman^ has lost his little tike workers. he is over by the ^trailer^.",
			"^slinky^ is right under the girders. he has a ^challenge^ for you!",
			"there is a paint mixing puzzle to solve in the ^trailer^!",
			"the ^jackhammer^ boss is at the top of the girders. i better warn you though, you need to help ^mr. potato head^ find his eye to get the ^disk "
			"launcher^ to defeat the ^jackhammer^ boss."
		};

		// GLOBAL: TOY2 0x0052F760
		int32_t g_unk52F760;

		// GLOBAL: TOY2 0x0052F764
		int32_t g_unk52F764;

		// GLOBAL: TOY2 0x0052F768
		int32_t g_unk52F768;

		// GLOBAL: TOY2 0x0052F76C
		int32_t g_unk52F76C;

		// GLOBAL: TOY2 0x0052F770
		int32_t g_unk52F770;

		// GLOBAL: TOY2 0x0052F774
		int32_t g_unk52F774;

		// GLOBAL: TOY2 0x0052F778
		int32_t g_unk52F778;

		// GLOBAL: TOY2 0x0052F77C
		int32_t g_unk52F77C;

		// GLOBAL: TOY2 0x0052F780
		int32_t g_unk52F780;

		// GLOBAL: TOY2 0x0052F784
		int32_t g_unk52F784;

		// GLOBAL: TOY2 0x0052F788
		int32_t g_unk52F788;

		// GLOBAL: TOY2 0x0052F78C
		int32_t g_unk52F78C;

		// GLOBAL: TOY2 0x0052F790
		int32_t g_unk52F790;

		// GLOBAL: TOY2 0x0052F794
		int32_t g_unk52F794;

		// GLOBAL: TOY2 0x0052F79C
		int32_t g_unk52F79C;

		// GLOBAL: TOY2 0x0052F7A0
		int32_t g_unk52F7A0;

		// GLOBAL: TOY2 0x0052F7A4
		int32_t g_unk52F7A4;

		// GLOBAL: TOY2 0x0052F7A8
		int32_t g_unk52F7A8;

		// GLOBAL: TOY2 0x0052F7AC
		int32_t g_unk52F7AC;

		// GLOBAL: TOY2 0x0052F7B0
		int32_t g_unk52F7B0;

		// GLOBAL: TOY2 0x0052F7B4
		int32_t g_unk52F7B4;

		// GLOBAL: TOY2 0x0052F7B8
		int32_t g_unk52F7B8;

		// GLOBAL: TOY2 0x0052F7BC
		int32_t g_unk52F7BC;

		// GLOBAL: TOY2 0x0052F7C0
		int32_t g_unk52F7C0;

		// GLOBAL: TOY2 0x0052F7C4
		int32_t g_unk52F7C4;

		// GLOBAL: TOY2 0x0052F7C8
		int32_t g_unk52F7C8;

		// GLOBAL: TOY2 0x0052F7D0
		int32_t g_unk52F7D0;

		// GLOBAL: TOY2 0x0052F7D4
		int32_t g_unk52F7D4;

		// GLOBAL: TOY2 0x0052F7D8
		int32_t g_unk52F7D8;

		// GLOBAL: TOY2 0x0052F7DC
		int32_t g_unk52F7DC;

		// GLOBAL: TOY2 0x0052F7E0
		int32_t g_unk52F7E0;

		// GLOBAL: TOY2 0x0052F7E4
		int32_t g_unk52F7E4;

		// GLOBAL: TOY2 0x0052F7E8
		int32_t g_unk52F7E8;

		// GLOBAL: TOY2 0x0052F7EC
		int32_t g_unk52F7EC;

		// GLOBAL: TOY2 0x0052F7F0
		int32_t g_unk52F7F0;

		// GLOBAL: TOY2 0x0052F7F4
		int32_t g_unk52F7F4;

		// GLOBAL: TOY2 0x0052F7F8
		int32_t g_unk52F7F8;

		// GLOBAL: TOY2 0x0052F7FC
		int32_t g_unk52F7FC;

		// GLOBAL: TOY2 0x0052F800
		int32_t g_unk52F800;

		// GLOBAL: TOY2 0x0052F808
		int32_t g_unk52F808;

		// GLOBAL: TOY2 0x0052F80C
		int32_t g_unk52F80C;

		// GLOBAL: TOY2 0x0052F810
		int32_t g_unk52F810;

		// GLOBAL: TOY2 0x0052F83C
		int32_t g_unk52F83C;

		// GLOBAL: TOY2 0x0052F840
		int32_t g_unk52F840;

		// GLOBAL: TOY2 0x0052F844
		int32_t g_unk52F844;

		// GLOBAL: TOY2 0x0052F848
		int32_t g_unk52F848;

		// GLOBAL: TOY2 0x0052F850
		int32_t g_unk52F850;

		// GLOBAL: TOY2 0x0052F854
		int32_t g_unk52F854;

		// GLOBAL: TOY2 0x0052F858
		int32_t g_unk52F858;

		// GLOBAL: TOY2 0x0052F85C
		int32_t g_unk52F85C;

		// GLOBAL: TOY2 0x0052F860
		int32_t g_unk52F860;

		// GLOBAL: TOY2 0x0052F868
		int32_t g_unk52F868;

		// GLOBAL: TOY2 0x0052F86C
		int32_t g_unk52F86C;

		// GLOBAL: TOY2 0x0052F870
		int32_t* g_unk52F870;

		// GLOBAL: TOY2 0x0052F874
		int32_t* g_unk52F874;

		// GLOBAL: TOY2 0x0052F878
		int32_t* g_unk52F878;

		// GLOBAL: TOY2 0x0052F87C
		int32_t* g_unk52F87C;

		// GLOBAL: TOY2 0x0052F880
		int32_t g_unk52F880;

		// GLOBAL: TOY2 0x0052F888
		int32_t g_unk52F888;

		// GLOBAL: TOY2 0x0052F88C
		int32_t g_unk52F88C;

		// GLOBAL: TOY2 0x0052F890
		int32_t g_unk52F890;

		// GLOBAL: TOY2 0x0052F894
		int32_t* g_unk52F894;

		// GLOBAL: TOY2 0x0052F898
		int32_t* g_unk52F898;

		// GLOBAL: TOY2 0x0052F89C
		int32_t* g_unk52F89C;

		// GLOBAL: TOY2 0x0052F8A0
		int32_t* g_unk52F8A0;

		// GLOBAL: TOY2 0x0052F8A4
		int32_t g_unk52F8A4;

		// GLOBAL: TOY2 0x0052F814
		HiddenCollectible g_unk52F814[5];

		// GLOBAL: TOY2 0x004F1CCC
		int32_t g_unk4F1CCC[4] = { -336724, -158292, -129428, 49196 };

		// GLOBAL: TOY2 0x004F1CDC
		int32_t g_unk4F1CDC[4] = { -496094, -317790, -289694, -111262 };

		// GLOBAL: TOY2 0x004F1CEC
		int32_t g_unk4F1CEC[4] = { -336724, -158291, -129428, 49197 };

		// GLOBAL: TOY2 0x004F1CFC
		int32_t g_unk4F1CFC[4] = { -496094, -317789, -289694, -111261 };

		// GLOBAL: TOY2 0x0052F798
		int32_t g_unk52F798;

		// GLOBAL: TOY2 0x0052F7CC
		int32_t g_unk52F7CC;

		// GLOBAL: TOY2 0x0052F804
		int32_t g_unk52F804;

		// GLOBAL: TOY2 0x0052F864
		int32_t g_unk52F864;

		// GLOBAL: TOY2 0x0052F884
		int32_t g_unk52F884;

		// FUNCTION: TOY2 0x0041BC20
		void MoveDrills(int32_t* a1, int32_t* a2, int32_t* a3, int32_t linkid, int32_t a5, int32_t a6)
		{
			int32_t v6;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			Nu3D::Particles::ParticleInstance* v11;
			Vector3I p_v1;
			int32_t IsWithinDistance;

			Nu3D::Link::GetCurrentPosFixed(linkid, &p_v1);
			if (Nu3D::Math::IsWithinDistance(&p_v1, &g_buzzActor.posAngles.pos, 1152))
			{
				Nu3D::Link::GetTargetPosFixed(linkid, &p_v1);
				v6 = (int32_t)Renderer::g_frameDelta + *a1;
				*a1 = v6;
				if (v6 >= 50)
				{
					if (v6 < 150)
					{
						p_v1.y -= 51200;
						Nu3D::Link::SetPositionRawAndCommit(linkid, p_v1.x >> 5, p_v1.y >> 5, p_v1.z >> 5);
						*a3 = 0;
						*a2 = 0;
						goto LABEL_20;
					}
					if (v6 >= 250)
					{
						if (v6 > a5)
						{
							*a1 = 0;
							AudioManager::PlaySoundEffect(106, &p_v1);
						}
						goto LABEL_20;
					}
					v8 = 384 * (int32_t)Renderer::g_frameDelta + *a2;
					*a2 = v8;
					v9 = v8 + *a3;
					*a3 = v9;
					if (v9 > 51200)
					{
						*a3 = 51200;
						if (*a2 > 3000)
						{
							Nu3D::Particles::SpawnFromPreset(p_v1.x, p_v1.y - 3072, p_v1.z, 25, 2);
							AudioManager::PlaySoundEffect(105, &p_v1);
							v10 = 8;
							do
							{
								v11 = Nu3D::Particles::SpawnFromPreset(p_v1.x, p_v1.y, p_v1.z, 66, 14);
								v11->groundAlignRot = 16 * *g_randDatBufferPtr++;
								v11->rotSpeed = *g_randDatBufferPtr - 128;
								--v10;
								++g_randDatBufferPtr;
							} while (v10);
							IsWithinDistance = Nu3D::Math::IsWithinDistance(&p_v1, &g_buzzActor.posAngles.pos, 500);
							if (IsWithinDistance)
								Camera::g_unk50A510 = 40 - ((int32_t)sqrt((double)IsWithinDistance) >> 4);
						}
						*a2 /= -4;
					}
					p_v1.y = p_v1.y + *a3 - 51200;
					Nu3D::Link::SetPositionRawAndCommit(linkid, p_v1.x >> 5, p_v1.y >> 5, p_v1.z >> 5);
					if (*a3 > 46080)
					{
					LABEL_4:
						Collision::MarkPlatformAsMoving(a6);
					LABEL_20:
						Nu3D::Link::GetCurrentPosFixed(linkid, &p_v1);
						Nu3D::Link::SetPositionRawAndCommit(linkid + 18, p_v1.x >> 7, p_v1.y >> 7, p_v1.z >> 7);
						return;
					}
				}
				else
				{
					p_v1.y += -1024 * v6;
					Nu3D::Link::SetPositionRawAndCommit(linkid, p_v1.x >> 5, p_v1.y >> 5, p_v1.z >> 5);
					if (*a1 < 5)
						goto LABEL_4;
				}
				Platform::DisableCollision(a6);
				goto LABEL_20;
			}
		}

		// FUNCTION: TOY2 0x0041BEE0
		void Method1(int32_t arg0, int32_t a2, int32_t a3, int32_t a4)
		{
			int32_t z;
			Vector3I v5;
			Vector3I p_x;
			Vector3I l_origin;

			Nu3D::Link::GetCurrentPosFixed(arg0, &p_x);
			Nu3D::Link::GetCurrentPosFixed(arg0, &p_x);
			Nu3D::Link::GetTargetPosFixed(arg0, &v5);
			v5.y = p_x.y - v5.y;
			Nu3D::Link::SetScaleFromFixedOffsets(arg0 + 57, 4096, ((a4 + v5.y) << 12) / a4, 4096);
			p_x.x >>= 5;
			p_x.y >>= 5;
			p_x.z >>= 5;
			Nu3D::Link::SetPositionRawAndCommit(arg0 + 8, p_x.x, p_x.y, p_x.z);
			Nu3D::Link::GetCurrentPosFixed(arg0 + 16, &v5);
			v5.x = p_x.x;
			v5.y >>= 5;
			v5.z = p_x.z;
			Nu3D::Link::SetPositionRawAndCommit(arg0 + 16, p_x.x, v5.y, p_x.z);
			Nu3D::Link::SetPositionRawAndCommit(arg0 + 57, v5.x, v5.y, v5.z);
			Nu3D::Link::GetCurrentPosFixed(arg0 + 20, &v5);
			v5.y >>= 5;
			if (a3)
			{
				v5.x = p_x.x;
				z = v5.z >> 5;
			}
			else
			{
				v5.x >>= 5;
				z = p_x.z;
			}
			v5.z = z;
			Nu3D::Link::SetPositionRawAndCommit(arg0 + 20, v5.x, v5.y, z);
			Platform::GetOrigin(a2, &l_origin);
			Platform::SetVelocity(a2, 32 * v5.x - l_origin.x, 32 * v5.y - l_origin.y, 32 * v5.z - l_origin.z);
			p_x.x >>= 2;
			p_x.y >>= 2;
			p_x.z >>= 2;
			Nu3D::Link::SetPositionRawAndCommit(arg0 + 4, p_x.x, p_x.y, p_x.z);
			Nu3D::Link::GetCurrentPosFixed(arg0 + 24, &v5);
			v5.y >>= 5;
			if (a3)
			{
				v5.x = p_x.x;
				v5.z >>= 5;
			}
			else
			{
				v5.x >>= 5;
				v5.z = p_x.z;
			}
			Nu3D::Link::SetPositionRawAndCommit(arg0 + 24, v5.x, v5.y, v5.z);
			Nu3D::Link::GetRotation8Bit(arg0, &p_x);
			Nu3D::Link::SetRotationAbsolute8bit(arg0 + 4, p_x.x, p_x.y, p_x.z);
		}

		// FUNCTION: TOY2 0x0041C100
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
					if (entry->props.linkId == 110)
						g_unk52F814[0].yPtr = &entry->pos.y;
					if (entry->props.linkId == 111)
						g_unk52F814[1].yPtr = &entry->pos.y;
					if (entry->props.linkId == 112)
						g_unk52F814[2].yPtr = &entry->pos.y;
					if (entry->props.linkId == 113)
						g_unk52F814[3].yPtr = &entry->pos.y;
					if (entry->props.linkId == 114)
						g_unk52F814[4].yPtr = &entry->pos.y;
					++entry;
					++i;
				} while (i < Levels::g_recordData[63]->recordCount);
			}
			for (i = 0; i < 5; ++i)
			{
				g_unk52F814[i].savedY = *g_unk52F814[i].yPtr;
				*g_unk52F814[i].yPtr = 0x80000000;
				Nu3D::Link::SetScaleFromFixedOffsets(i + 110, 0, 0, 0);
			}
		}

		// FUNCTION: TOY2 0x0041C190
		void Init()
		{
			int32_t v0;
			int32_t v1;
			int32_t v2;
			uint8_t v3;
			Vector3I v4;

			Collectables::Init(g_unk4F1C98, 117);
			MoveableObject::InitTable(g_unk4F1C88);
			Collectables::LoadTokenTable((int32_t*)g_unk4F1CA4);
			v0 = 0;
			v1 = 0;
			g_unk52F2F8 = 0;
			g_unk52AD64 = 0;
			g_unk52F7DC = 0;
			for (g_unk52F784 = 0; Levels::g_portalEntries[1].entries[v0].recordIdx != 0xFF; ++v0)
			{
				if (Levels::g_portalEntries[1].entries[v0].categoryIdx == 2)
					v1 = v0 * 2;
			}
			v2 = v0 * 2 - 2;
			g_unk52F7F0 = v2;
			v3 = *(&Levels::g_portalEntries[1].entries[0].recordIdx + v2);
			g_unk52F7E8 = *(&Levels::g_portalEntries[1].entries[0].categoryIdx + v2);

			*(&Levels::g_portalEntries[1].entries[0].recordIdx + v2) = *(&Levels::g_portalEntries[1].entries[0].recordIdx + v1);
			*(&Levels::g_portalEntries[1].entries[0].categoryIdx + v2) = *(&Levels::g_portalEntries[1].entries[0].categoryIdx + v1);
			*(&Levels::g_portalEntries[1].entries[0].recordIdx + v1) = v3;
			*(&Levels::g_portalEntries[1].entries[0].categoryIdx + v1) = g_unk52F7E8;

			g_unk52F880 = *(&Levels::g_portalEntries[1].entries[0].recordIdx + v2);
			g_unk52F7E4 = *(int32_t*)Levels::g_portalEntries[2].entries;
			g_unk52F7E8 = Levels::g_portalEntries[2].entries[1].recordIdx;
			g_unk52F7D0 = 4096;
			g_unk52F7C8 = 0;
			g_unk52F810 = 128;
			g_unk52F7F4 = 60;
			g_unk52F76C = 0;
			g_unk52F7CC = 0;
			Buzz::g_underwaterYLevel = 0;
			Buzz::g_unk830D30 = 0;
			Buzz::g_currentFluidType = 0;
			g_unk52F7D4 = 0;
			g_unk52F804 = 0;
			g_unk52F864 = 0;
			g_unk52F884 = g_levelActors[24].actorPhase;
			g_unk52F798 = 0;
			g_unk52F7F8 = 0;
			g_unk52F874 = g_unk4F1BA4;
			g_unk52F854 = 0;
			g_unk52F78C = 0;
			g_unk52F7FC = 0;
			g_unk52F870 = g_unk4F1BC8;
			g_unk52F860 = 0;
			g_unk52F790 = 0;
			g_unk52F800 = 0;
			g_unk52F87C = g_unk4F1BF4;
			g_unk52F85C = 0;
			g_unk52F794 = 0;
			g_unk52F808 = 0;
			g_unk52F878 = g_unk4F1C1C;
			g_unk52F858 = 0;
			g_unk52F788 = 0;
			Platform::SetRotationAngles(1, 0, 3072, 0);
			Platform::SetRotationAngles(2, 0, 3072, 0);
			Platform::SetRotationAngles(3, 0, 0, 0);
			Platform::SetRotationAngles(4, 0, 0, 0);
			Platform::AddFlags(1, 256);
			Platform::AddFlags(2, 256);
			Platform::AddFlags(3, 256);
			Platform::AddFlags(4, 256);
			g_unk52F850 = 0;
			g_unk52F8A4 = 0;
			g_unk52F7EC = 0;
			g_unk52F83C = 0;
			Platform::AddFlags(26, 256);
			Nu3D::Link::SetRotationRelative8bit(28, 0, 0, 614);
			Nu3D::Link::SetRotationRelative8bit(29, 0, 0, 614);
			g_unk52F7E0 = 0;
			Nu3D::Link::SetScaleFromFixedOffsets(51, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(52, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(53, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(33, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(34, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(35, 0, 0, 0);
			g_unk52F7C0 = 0;
			g_unk52F898 = g_unk4F1C64;
			g_unk52F894 = g_unk4F1C64;
			g_unk52F7C4 = 0;
			g_unk52F770 = 0;
			g_unk52F86C = 0;
			g_unk52F764 = 0;
			g_unk52F80C = 0;
			g_unk52F7D8 = 0;
			g_unk52F760 = 0;
			g_unk52F868 = 0;
			g_levelActors[5].useTint = 1;
			g_levelActors[5].unkVar13_ = (int16_t)32768;
			g_levelActors[5].unkVar13 = 4096;
			g_levelActors[5].unkVar12 = 4096;
			g_levelActors[5].unkVar12_ = 0;
			g_unk52F888 = 0;
			g_unk52F77C = 0;
			g_unk52F890 = 0;
			g_unk52F780 = 0;
			g_unk52F8A0 = g_unk4F1C40;
			g_unk52F88C = 0;
			g_unk52F774 = 0;
			g_unk52F89C = g_unk4F1C40;
			g_unk52F768 = 0;
			g_unk52F778 = 0;
			g_unk52F7A4 = 0;
			g_unk52F7AC = 0;
			g_unk52F7B8 = 0;
			Platform::DisableCollision(10);
			g_unk52F7A0 = 33;
			g_unk52F7B0 = 0;
			g_unk52F7B4 = 0;
			Platform::DisableCollision(12);
			g_unk52F79C = 66;
			g_unk52F7A8 = 0;
			g_unk52F7BC = 0;
			Platform::DisableCollision(11);
			MoveDrills(&g_unk52F7A4, &g_unk52F7B8, &g_unk52F7AC, 36, 300, 10);
			MoveDrills(&g_unk52F7A0, &g_unk52F7B4, &g_unk52F7B0, 37, 327, 12);
			MoveDrills(&g_unk52F79C, &g_unk52F7BC, &g_unk52F7A8, 38, 349, 11);
			Nu3D::Link::GetCurrentPosFixed(36, &v4);
			Nu3D::Link::SetPositionRawAndCommit(54, v4.x >> 7, v4.y >> 7, v4.z >> 7);
			Nu3D::Link::GetCurrentPosFixed(37, &v4);
			Nu3D::Link::SetPositionRawAndCommit(55, v4.x >> 7, v4.y >> 7, v4.z >> 7);
			Nu3D::Link::GetCurrentPosFixed(38, &v4);
			Nu3D::Link::SetPositionRawAndCommit(56, v4.x >> 7, v4.y >> 7, v4.z >> 7);
			InitHiddenCollectibles();
			g_unk52F840 = -77776;
			g_unk52F844 = -146166;
			g_unk52F848 = 102834;
		}

		// FUNCTION: TOY2 0x0041C640
		void Interactions()
		{
			uint8_t v0;
			int32_t v1;
			uint8_t v2;
			uint8_t v3;
			int32_t v4;
			int32_t v5;
			Nu3D::Particles::ParticleInstance* v6;
			int32_t z;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			int32_t v11;
			int32_t v12;
			int32_t v14;
			int32_t v15;
			int32_t v16;
			int32_t v17;
			int32_t v18;
			int32_t v19;
			int32_t v20;
			int32_t v21;
			int32_t v22;
			Vector3I* data;
			int32_t v24;
			int32_t v25;
			int16_t v26;
			int32_t v27;
			int32_t v28;
			int32_t v29;
			int32_t* v30;
			int32_t v31;
			int32_t v32;
			int32_t v34;
			BOOL IsInsideBounds;
			int32_t y;
			int32_t v38;
			uint32_t v39;
			int32_t v40;
			int32_t v41;
			Levels::RecordData* v42;
			int32_t v44;
			int32_t v45;
			int32_t v46;
			uint8_t v47;
			bool v48;
			int32_t v50;
			int32_t v51;
			int32_t v52;
			int32_t v53;
			int32_t colorR;
			int32_t v55;
			int32_t v56;
			int32_t colorB;
			int32_t x;
			Vector3I p_v1;
			Vector3I a2;

			Nu3D::Link::GetCurrentPosFixed(12, &p_v1);
			Nu3D::Link::SetPositionRawAndCommit(13, p_v1.x >> 7, p_v1.y >> 7, p_v1.z >> 7);

			p_v1.x = 334743;
			p_v1.y = -66813;
			p_v1.z = 386446;

			if (Nu3D::Math::IsWithinDistance(&p_v1, &g_buzzActor.posAngles.pos, 250))
			{
				v0 = g_unk52F7E4;

				*(&Levels::g_portalEntries[1].entries[0].recordIdx + g_unk52F7F0) = g_unk52F880;

				v1 = g_unk52F7D0;
				Levels::g_portalEntries[2].entries[0].recordIdx = v0;
				Levels::g_portalEntries[2].entries[1].recordIdx = g_unk52F7E8;

				if (g_unk52F7D0 > 0)
				{
					v1 = g_unk52F7D0 - 512;
				LABEL_6:
					g_unk52F7D0 = v1;
				}
			}
			else
			{
				v2 = g_unk52F7E4;
				v3 = g_unk52F7E8;

				*(&Levels::g_portalEntries[1].entries[0].recordIdx + g_unk52F7F0) = -1;

				v1 = g_unk52F7D0;
				Levels::g_portalEntries[2].entries[1].recordIdx = v2;
				Levels::g_portalEntries[2].entries[0].recordIdx = v3;

				if (g_unk52F7D0 < 4096)
				{
					v1 = g_unk52F7D0 + 512;
					goto LABEL_6;
				}
			}

			Nu3D::Link::SetScaleFromFixedOffsets(63, 4096, v1, 4096);
			Nu3D::Link::SetScaleFromFixedOffsets(64, 4096, g_unk52F7D0, 4096);

			g_unk52F7F8 = Platform::StepTiltPhysics(1, 0, g_unk52F7F8, 2, -224, 224, 16);

			if (Platform::g_unk559E80 == 1 || Platform::g_unk559E80 == 7)
			{
				Platform::SetVelocity(1, 0, 0, 0);
				Platform::SetVelocity(7, 0, 0, 0);
			}
			else
			{
				Platform::StepMotionScript(1, 0, (int16_t**)&g_unk52F874, &g_unk52F854, &g_unk52F78C);
			}

			Method1(0, 7, 0, 137600);

			g_unk52F7FC = Platform::StepTiltPhysics(2, 1, g_unk52F7FC, 2, -224, 224, 16);

			if (Platform::g_unk559E80 == 2 || Platform::g_unk559E80 == 8)
			{
				Platform::SetVelocity(2, 0, 0, 0);
				Platform::SetVelocity(8, 0, 0, 0);
			}
			else
			{
				Platform::StepMotionScript(2, 1, (int16_t**)&g_unk52F870, &g_unk52F860, &g_unk52F790);
			}

			Method1(1, 8, 1, 48000);
			g_unk52F800 = Platform::StepTiltPhysics(3, 2, g_unk52F800, 2, -224, 224, 16);

			if (Platform::g_unk559E80 == 3 || Platform::g_unk559E80 == 9)
			{
				Platform::SetVelocity(3, 0, 0, 0);
				Platform::SetVelocity(9, 0, 0, 0);
			}
			else
			{
				Platform::StepMotionScript(3, 2, (int16_t**)&g_unk52F87C, &g_unk52F85C, &g_unk52F794);
			}

			Method1(2, 9, 0, 72000);
			g_unk52F808 = Platform::StepTiltPhysics(4, 3, g_unk52F808, 2, -224, 224, 16);

			if (Platform::g_unk559E80 == 4 || Platform::g_unk559E80 == 6)
			{
				Platform::SetVelocity(4, 0, 0, 0);
				Platform::SetVelocity(6, 0, 0, 0);
			}
			else
			{
				Platform::StepMotionScript(4, 3, (int16_t**)&g_unk52F878, &g_unk52F858, &g_unk52F788);
			}

			Method1(3, 6, 1, 52800);

			Platform::StepMotionScript(14, 41, (int16_t**)&g_unk52F898, &g_unk52F888, &g_unk52F77C);
			Platform::StepMotionScript(15, 42, (int16_t**)&g_unk52F894, &g_unk52F890, &g_unk52F780);
			Platform::StepMotionScript(16, 43, (int16_t**)&g_unk52F8A0, &g_unk52F88C, &g_unk52F774);
			Platform::StepMotionScript(17, 44, (int16_t**)&g_unk52F89C, &g_unk52F768, &g_unk52F778);

			a2.x = g_unk52F840 + 3 * (Camera::g_activeCameraTransform.pos.x - g_unk52F840) / 4;
			a2.y = g_unk52F844 + 3 * (Camera::g_activeCameraTransform.pos.y - g_unk52F844) / 4;
			a2.z = g_unk52F848 + 3 * (Camera::g_activeCameraTransform.pos.z - g_unk52F848) / 4;

			if (g_unk52F7E0)
			{
				Platform::GetRotation(26, &p_v1);

				if (g_unk52F7E0 == 1)
				{
					if (p_v1.z <= -2457)
					{
						g_unk52F7E0 = 2;
						Platform::SetAngularVelocity(26, 0, 0, 0);
						g_unk52F7E0 = 2;
						AudioManager::PlaySoundEffect(109, &a2);
					}
					else
					{
						Platform::SetAngularVelocity(26, 0, 0, -8 * (int32_t)Renderer::g_frameDelta);
						AudioManager::PlaySoundEffect(110, &a2);
					}

					goto LABEL_37;
				}

				v4 = g_unk52F7E0;
				v5 = (int32_t)Renderer::g_frameDelta + g_unk52F7E0;
				g_unk52F7E0 += (int32_t)Renderer::g_frameDelta;

				if (v4 < 240)
				{
					if (v5 < 240)
					{
					LABEL_37:
						Nu3D::Link::SetRotationRelative8bit(28, 0, 0, p_v1.z / 4 + 614);
						Nu3D::Link::SetRotationRelative8bit(29, 0, 0, p_v1.z / 4 + 614);

						goto LABEL_42;
					}

					AudioManager::PlaySoundEffect(109, &a2);
					v5 = g_unk52F7E0;
				}

				if (v5 >= 240)
				{
					if (p_v1.z >= -32)
					{
						if (p_v1.z >= 0)
						{
							Platform::SetAngularVelocity(26, 0, 0, 0);
							Platform::ClearFlags(26, 8);
							AudioManager::PlaySoundEffect(109, &a2);
							g_unk52F7E0 = 0;
						}
						else
						{
							Platform::SetAngularVelocity(26, 0, 0, -(uint16_t)(p_v1.z));
							AudioManager::PlaySoundEffect(110, &a2);
						}
					}
					else
					{
						Platform::SetAngularVelocity(26, 0, 0, 8 * (int32_t)Renderer::g_frameDelta);
						AudioManager::PlaySoundEffect(110, &a2);
					}
				}

				goto LABEL_37;
			}

			if (g_buzzActor.collisionFlags && Buzz::g_groundSlamTicker && Buzz::g_unk52B81C == 36)
			{
				g_unk52F7E0 = 1;
				Levels::Method3(3, 1);
				AudioManager::PlaySoundEffect(109, &a2);
			}
		LABEL_42:

			MoveDrills(&g_unk52F7A4, &g_unk52F7B8, &g_unk52F7AC, 36, 300, 10);
			MoveDrills(&g_unk52F7A0, &g_unk52F7B4, &g_unk52F7B0, 37, 327, 12);
			MoveDrills(&g_unk52F79C, &g_unk52F7BC, &g_unk52F7A8, 38, 349, 11);

			if (g_unk52F83C < 0)
			{
				p_v1.x = 32 * Levels::g_recordData[0]->data[g_unk52F8A4].x;
				p_v1.y = 32 * Levels::g_recordData[0]->data[g_unk52F8A4].y;
				p_v1.z = 32 * Levels::g_recordData[0]->data[g_unk52F8A4].z;

				if (Nu3D::Math::IsWithinDistanceXZ(&p_v1, &g_buzzActor.posAngles.pos, 1280) && g_buzzActor.posAngles.pos.y > p_v1.y
					&& g_buzzActor.posAngles.pos.y - p_v1.y < 204800)
				{
					v6 = Nu3D::Particles::SpawnFromPreset(p_v1.x, p_v1.y, p_v1.z, 67, 19);
					v6->rotSpeed = ((*g_randDatBufferPtr++ & 1) << 7) - 64;

					z = Levels::g_recordData[0]->data[g_unk52F8A4].z;
					v8 = Levels::g_recordData[0]->data[g_unk52F8A4 + 1].z;
					x = Levels::g_recordData[0]->data[g_unk52F8A4 + 1].x;

					if (abs(Levels::g_recordData[0]->data[g_unk52F8A4].x - x) <= abs(z - v8))
					{
						if (z <= v8)
							v6->velZ = 384;
						else
							v6->velZ = -384;
					}
					else if (Levels::g_recordData[0]->data[g_unk52F8A4].x <= x)
					{
						v6->velX = 384;
					}
					else
					{
						v6->velX = -384;
					}
				}

				v9 = (*g_randDatBufferPtr++ & 0x1F) + 16;
				g_unk52F83C = v9;
				g_unk52F8A4 += 2;

				if (g_unk52F8A4 > Levels::g_recordData[0]->recordCount)
					g_unk52F8A4 = 0;
			}

			v10 = g_unk52F7EC;
			v11 = 0;
			g_unk52F83C -= (int32_t)Renderer::g_frameDelta;

			if (g_unk52F7EC < 0)
			{
				p_v1.x = 30779;
				p_v1.y = -512091;
				p_v1.z = -475844;

				if (Nu3D::Math::IsWithinDistanceXZ(&p_v1, &g_buzzActor.posAngles.pos, 900) && g_buzzActor.posAngles.pos.y + 153600 > p_v1.y
					&& g_buzzActor.posAngles.pos.y - 102400 < p_v1.y)
				{
					v12 = *g_randDatBufferPtr++ & 0xE;
					g_unk52F850 = v12;

					p_v1.x = 32 * Levels::g_recordData[1]->data[v12].x;
					p_v1.y = 32 * Levels::g_recordData[1]->data[v12].y;
					p_v1.z = 32 * Levels::g_recordData[1]->data[v12].z;

					v14 = (p_v1.x - 32 * Levels::g_recordData[1]->data[v12 + 1].x) >> 8;
					v15 = (p_v1.z - 32 * Levels::g_recordData[1]->data[v12 + 1].z) >> 8;
					x = (Nu3D::Math::CartesianToFixedAngle(v14, v15) - 2048) & 0xFFF;

					v16 = (int32_t)sqrt((double)(v14 * v14 + v15 * v15));
					v17 = 6 * v16 / 2;
					v18 = 32 * (32 * Levels::g_recordData[1]->data[g_unk52F850 + 1].y - p_v1.y) / ((v16 << 12) / v17) - 144 * ((v16 << 12) / v17) / 64;

					v53 = ((*g_randDatBufferPtr & 1) << 7) - 64;
					v19 = v17 * Numerics::g_sinCosLUT[(x + 1024) & 0xFFF];
					++g_randDatBufferPtr;

					Nu3D::Particles::SpawnInstance(p_v1.x, p_v1.y, p_v1.z, v17 * Numerics::g_sinCosLUT[x] / 0x2000, v18, v19 / 0x2000, 144, 0, v53, 84);
					v11 = 0;
				}

				v10 = (*g_randDatBufferPtr++ & 0x1F) + 64;
			}

			g_unk52F7EC = v10 - (int32_t)Renderer::g_frameDelta;
			Platform::GetOrigin(0, &p_v1);

			v20 = g_unk52F7D8;
			g_levelActors[5].pos = p_v1;

			if (g_unk52F7D8 <= 0)
			{
				v21 = g_unk52F80C + 2 * g_unk52F764 + g_unk52F764;

				if (v21 == 4 || v21 == 8 || v21 == 12)
				{
					g_unk52F760 = 32;
					g_unk52F80C = 0;
					g_unk52F764 = 0;

					AudioManager::StartSoundSequenceOnActor(-6, (Actor::Toy2Actor*)&g_buzzActor);
					v20 = g_unk52F7D8;
				}
			}

			g_unk52F7C4 = 0;

			if (g_unk52F760 > 0)
			{
				*(uint16_t*)&v24 = g_unk52F86C;
				g_unk52F868 = 0;
				g_levelActors[5].unkVar12_ = g_unk52F760 * g_unk52F86C / 32;
				g_unk52F760 -= (int32_t)Renderer::g_frameDelta;
			}
			else
			{
				if (g_buzzActor.collisionFlags && Buzz::g_groundSlamTicker && Buzz::g_unk52B81C >= 32 && Buzz::g_unk52B81C <= 35 && v20 <= 0)
				{
					g_unk52F7C0 = Buzz::g_unk52B81C - 31;
					g_unk52F7D8 = 60;

					Levels::Method3(0, 1);
					Levels::Method3(1, 1);
					Levels::Method3(2, 1);

					v20 = g_unk52F7D8;
				}

				v22 = 0;
				data = Levels::g_recordData[3]->data;
				do
				{
					if (abs((p_v1.x >> 5) - data->x) < 200)
						g_unk52F7C4 = v22 + 1;

					++v22;
					++data;
				} while (v22 <= Levels::g_recordData[3]->recordCount);

				v11 = g_unk52F7C4;

				if (v20 == 60)
				{
					if (g_unk52F7C4 > 4 && g_unk52F7C4 - 3 == g_unk52F7C0)
					{
						if (g_unk52F80C)
						{
							if (g_unk52F764)
							{
								g_unk52F868 = 1;
							}
							else
							{
								g_unk52F868 = 0;
								g_unk52F764 = g_unk52F7C4 - 4;
							}
						}
						else
						{
							g_unk52F80C = g_unk52F7C4 - 4;
						}
					}

					*(uint16_t*)&v24 = g_unk52F86C;
				LABEL_83:

					if (g_unk52F7C0 <= 1)
						goto LABEL_98;

					Nu3D::Link::SetScaleFromFixedOffsets(g_unk52F7C0 + 31, 4096, (60 - v20) << 9, 4096);
					goto LABEL_97;
				}

				*(uint16_t*)&v24 = g_unk52F86C;
			}

			if (v20 <= 0)
				goto LABEL_117;

			if (v20 > 52)
				goto LABEL_83;

			if (v20 >= 12)
			{
				if (g_unk52F7C0 <= 1)
					goto LABEL_98;

				Nu3D::Link::SetScaleFromFixedOffsets(g_unk52F7C0 + 31, 4096, 4096, 4096);
			}
			else
			{
				if (g_unk52F7C0 <= 1)
					goto LABEL_98;

				Nu3D::Link::SetScaleFromFixedOffsets(g_unk52F7C0 + 31, 0, 0, 0);
			}
		LABEL_97:

			v20 = g_unk52F7D8;
			v11 = g_unk52F7C4;

			*(uint16_t*)&v24 = g_unk52F86C;
		LABEL_98:

			if (! g_unk52F868 && v11 - 3 == g_unk52F7C0 && v11 >= 5 && v11 <= 7)
			{
				switch (g_unk52F764 + g_unk52F80C + 2 * g_unk52F764)
				{
					case 1:
						g_levelActors[5].actorTint.r = 1664;
						g_levelActors[5].actorTint.g = 0;
						g_levelActors[5].actorTint.b = 0;
						v25 = 60;
						goto LABEL_114;
					case 2:
						g_levelActors[5].actorTint.r = 0;
						g_levelActors[5].actorTint.g = 0;
						g_levelActors[5].actorTint.b = 1536;
						v25 = 60;
						goto LABEL_114;
					case 3:
						g_levelActors[5].actorTint.r = 1536;
						g_levelActors[5].actorTint.g = 1408;
						g_levelActors[5].actorTint.b = 0;
						v25 = 60;
						goto LABEL_114;
					case 4:
					case 8:
					case 12:
						goto LABEL_113;
					case 5:
						g_levelActors[5].actorTint.g = 0;
						v26 = 8 * v20 + 1056;
						g_levelActors[5].actorTint.r = 14 * (60 - v20);
						goto LABEL_112;
					case 6:
						g_levelActors[5].actorTint.r = 1536;
						g_levelActors[5].actorTint.b = 0;
						g_levelActors[5].actorTint.g = 10 * v20 + 808;
						goto LABEL_113;
					case 7:
						g_levelActors[5].actorTint.g = 0;
						g_levelActors[5].actorTint.r = 12 * v20 + 944;
						g_levelActors[5].actorTint.b = 18 * (60 - v20);
						goto LABEL_113;
					case 9:
						g_levelActors[5].actorTint.r = 8 * (3 * v20 + 12);
						g_levelActors[5].actorTint.g = 6 * v20 + 1048;
						g_levelActors[5].actorTint.b = 0;
						goto LABEL_113;
					case 10:
						g_levelActors[5].actorTint.r = 2 * v20 + 1544;
						g_levelActors[5].actorTint.b = 0;
						g_levelActors[5].actorTint.g = 12 * (60 - v20);
						goto LABEL_113;
					case 11:
						g_levelActors[5].actorTint.r = 0;
						g_levelActors[5].actorTint.g = 16 * (60 - v20);
						v26 = 8 * (3 * v20 + 12);
					LABEL_112:
						g_levelActors[5].actorTint.b = v26;
					LABEL_113:
						v25 = 124;
					LABEL_114:
						v24 = 32 * (v25 - v20);
						g_unk52F86C = v24;
						break;
					default:
						break;
				}

				g_levelActors[5].unkVar12_ = v24;
			}

			g_unk52F7D8 = v20 - (int32_t)Renderer::g_frameDelta;
		LABEL_117:

			if (v11 > 0 && v11 < 4)
			{
				v27 = g_unk52F80C + 2 * g_unk52F764;

				if (v11 == 1)
				{
					if (g_unk52F764 + v27 == 5 || g_unk52F764 + v27 == 7)
					{
						v28 = g_unk52F770 | 1;
						goto LABEL_130;
					}
				}
				else if (v11 == 2)
				{
					if (g_unk52F764 + v27 == 6 || g_unk52F764 + v27 == 10)
					{
						v28 = g_unk52F770 | 2;

						goto LABEL_130;
					}
				}
				else if (g_unk52F764 + v27 == 9 || g_unk52F764 + v27 == 11)
				{
					v28 = g_unk52F770 | 4;
				LABEL_130:
					g_unk52F770 = v28;
					g_unk52F760 = 32;
					g_unk52F80C = 0;
					g_unk52F764 = 0;
					AudioManager::StartSoundSequenceOnActor(-5, (Actor::Toy2Actor*)&g_buzzActor);
				}
			}

			if (g_levelActors[5].unkVar12_ < 384)
				*(uint8_t*)&g_levelActors[5].actorFlags &= ~1u;

			if (g_tickState[8])
			{
				v29 = g_tickAccumulators.tick64Acc >> 3;

				if ((g_unk52F770 & g_unk4F1B64[2 * v29]) != 0)
				{
					v30 = &g_unk4F1B64[2 * v29 + 1];
					v31 = *v30;

					if (*v30 >= 0)
					{
						Nu3D::Link::SetScaleFromFixedOffsets(v31, 4096, 4096, 4096);
						Nu3D::Link::SetScaleFromFixedOffsets(*v30 + 3, 0, 0, 0);
					}
					else
					{
						Nu3D::Link::SetScaleFromFixedOffsets(-v31, 0, 0, 0);
						Nu3D::Link::SetScaleFromFixedOffsets(3 - *v30, 4096, 4096, 4096);
					}
				}
			}

			if (g_unk52F770 == 7 && ! Collectables::g_pickupSlots[3].state)
				Collectables::Activate(3, 0);

			Actor::PlayPeriodicHintSound(26, 181);

			if ((g_levelActors[26].actorFlags & 0x200) != 0)
			{
				g_levelActors[26].actorFlags &= ~0x200u;

				if (g_unk52F2F8)
				{
					if (g_unk830D4C >= 5)
					{
						if (g_unk52F2F8 != 3)
						{
							AudioManager::Preset::PlayOneShotSound2(183, &g_levelActors[26]);
							Dialogue::Begin(26, 31, "well done buzz! you have found all the ^wrenches^! here is your pizza planet ^token^!", -1, 0, 2);
							g_unk52F2F8 = 3;
							goto LABEL_158;
						}
					}
					else
					{
						Dialogue::Begin(26, 31, "hurry up buzz! you still have ^wrenches^ to find!", -1, 0, -1);
					}
				}
				else
				{
					AudioManager::Preset::PlayOneShotSound2(182, &g_levelActors[26]);
					Dialogue::Begin(
						26,
						31,
						"hi buzz! if you can find ^five^ missing ^wrenches^ and bring them to me in time, i will give you a pizza planet ^token^!",
						-1,
						0,
						-1);
					g_unk830D4C = 0;
					g_unk52F2F8 = 1;

					v32 = 0;

					do
					{
						*g_unk52F814[v32].yPtr = g_unk52F814[v32].savedY;
						Nu3D::Link::SetScaleFromFixedOffsets(v32 + 110, 4096, 4096, 4096);
						++v32;
					} while (v32 < 5);
				}
			}

			if (g_unk52F2F8 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_158;

				g_unk52F2F8 = 2;
				g_unk52AD64 = 150;
			}
			else if (g_unk52F2F8 != 2)
			{
				goto LABEL_158;
			}

			if (g_tickState[11])
				--g_unk52AD64;

			if (g_unk52AD64 < 100)
			{
				g_unk52AD64 = 100;
				g_unk52F2F8 = 0;

				v34 = 0;

				do
				{
					*g_unk52F814[v34].yPtr = 0x80000000;
					Nu3D::Link::SetScaleFromFixedOffsets(v34 + 110, 0, 0, 0);
					++v34;
				} while (v34 < 5);
			}
		LABEL_158:

			Actor::ItemReturnReward(
				21,
				30,
				"hi buzz! you need a ^disk launcher^ to defeat the angry ^jackhammer^ boss. find my missing ^eye^, i will give you the ^disk launcher^!",
				"wow! thanks buzz! in return for finding my ^eye^ i will let you use the ^disk launcher^. it should help you defeat the toughest enemies!",
				"the ^disk launcher^ will help you defeat the toughest enemies!",
				1088,
				3136);
			Actor::CollectQuestReward(20, 29, 3600, 1760, 0);
			Actor::RotatingHint(17, 35, g_unk4F1CB8);

			if ((g_levelActors[18].actorFlags & 0x200) != 0)
			{
				g_levelActors[18].actorFlags &= ~0x200u;

				if (g_collectionCount >= 0)
				{
					if (g_collectionCount == 5)
					{
						Dialogue::Begin(18, 32, "thanks for finding my ^little tikes^ buzz! here is a pizza planet ^token^!", -1, 0, 1);
						g_collectionCount = -1;
					}
					else
					{
						Dialogue::Begin(
							18,
							32,
							"hi buzz! if you find my ^five^ missing ^little tikes^ and come back and find me, i will give you a pizza planet ^token^.",
							-1,
							0,
							-1);
					}
				}
			}

			IsInsideBounds = Actor::IsInsideBounds((Actor::Toy2Actor*)&g_buzzActor, -378881, 90015, -538392, -69160);
			y = g_buzzActor.posAngles.pos.y;

			if (IsInsideBounds && g_buzzActor.posAngles.pos.y > -36235)
			{
				Buzz::g_underwaterYLevel = -31352;
				Buzz::g_currentFluidType = 2;
				g_unk52F7D4 = 1;
			}
			else
			{
				if (g_unk52F7D4)
					Buzz::g_unk53C60C = 0xB40018;

				g_unk52F7D4 = 0;
				Buzz::g_underwaterYLevel = 0;
				Buzz::g_currentFluidType = 0;
			}

			v38 = g_unk52F804;
			g_levelActors[24].visibilityDistance = g_buzzActor.posAngles.pos.y >= -561543 ? 1800 : 3800;

			if (! g_unk52F804)
			{
				if (! g_buzzActor.collisionFlags || g_buzzActor.posAngles.pos.y >= -638917)
					goto LABEL_188;

				if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &g_levelActors[24].pos, 300))
				{
					g_unk52F804 = 1;
					Dialogue::Begin(
						24,
						34,
						"ha ha ha ha ... defeat the ^jackhammer^ boss to get a pizza planet ^token^! i hope you have the ^disk launcher^!",
						1408,
						3520,
						-1);
				}

				y = g_buzzActor.posAngles.pos.y;
				v38 = g_unk52F804;
			}

			if (v38 == 1)
			{
				if (! Renderer::g_enableCinematicBars)
				{
					g_unk52F804 = 2;

					g_levelActors[24].unkShort1 = 0;
					g_levelActors[24].movementData = Actor::g_movementData[18] + 12;

					g_unk830D44 = 180;
				LABEL_179:

					if (! g_levelActors[24].creatureId)
					{
						v39 = (int32_t)Renderer::g_frameDelta;
						g_unk52B7E0 = 0;
						g_unk52F804 = (int32_t)Renderer::g_frameDelta + 3;
						goto LABEL_189;
					}

					if (y < -515621)
						g_hudWidgetTimers[7] = 90;
				}
			}
			else
			{
				if (v38 == 2)
					goto LABEL_179;

				if (v38 >= 3)
				{
					if (v38 < 120)
					{
						v39 = (int32_t)Renderer::g_frameDelta;
						g_unk52F804 = (int32_t)Renderer::g_frameDelta + v38;

						goto LABEL_189;
					}

					if (v38 != 200)
					{
						Collectables::Activate(4, 0);
						g_unk52F804 = 200;
					}
				}
			}
		LABEL_188:

			v39 = (int32_t)Renderer::g_frameDelta;
		LABEL_189:
			v40 = g_unk52F810;

			g_unk52F7F4 -= v39;

			if (g_unk52F7F4 < 0)
			{
				v40 = (g_unk52F810 + 0x80) & 0x80;
				g_unk52F810 = v40;
				g_unk52F7F4 = *g_randDatBufferPtr++ & 0x3F;
			}

			if (g_unk52F7C8 >= v40)
			{
				g_unk52F7C8 -= 2 * v39;
				v41 = g_unk52F7C8;

				if (g_unk52F7C8 >= v40)
					goto LABEL_196;
			}
			else
			{
				g_unk52F7C8 += 2 * v39;
				v41 = g_unk52F7C8;

				if (g_unk52F7C8 <= v40)
					goto LABEL_196;
			}

			v41 = v40;
			g_unk52F7C8 = v40;
		LABEL_196:

			if (v41)
			{
				if (v41 <= 64)
				{
					colorB = 0;
					colorR = 2 * v41;
				}
				else
				{
					colorR = 128;
					colorB = 2 * v41 - 128;
				}

				v42 = Levels::g_recordData[6];
				v56 = 0x7FFFFFFF;
				v55 = 0;

				if (! Levels::g_recordData[6]->recordCount)
					goto LABEL_210;

				do
				{
					v44 = 32 * v42->data[v55].y;
					v45 = 32 * v42->data[v55].z;
					v46 = 32 * v42->data[v55].x;

					if (((Camera::g_activeCameraTransform.pos.x - v46) >> 8) * ((Camera::g_activeCameraTransform.pos.x - v46) >> 8)
							+ ((Camera::g_activeCameraTransform.pos.y - v44) >> 8) * ((Camera::g_activeCameraTransform.pos.y - v44) >> 8)
							+ ((Camera::g_activeCameraTransform.pos.z - v45) >> 8) * ((Camera::g_activeCameraTransform.pos.z - v45) >> 8)
						>= 1000000)
					{
						v47 = colorR;
					}
					else
					{
						v47 = colorR;

						Renderer::LensFlare::RegisterLight(v46, v44, v45, colorR, colorR, colorB, 64);

						if (((g_buzzActor.posAngles.pos.y - v44 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v44 - 0x2000) >> 8)
								+ ((g_buzzActor.posAngles.pos.z - v45) >> 8) * ((g_buzzActor.posAngles.pos.z - v45) >> 8)
								+ ((g_buzzActor.posAngles.pos.x - v46) >> 8) * ((g_buzzActor.posAngles.pos.x - v46) >> 8)
							< v56)
						{
							v56 = ((g_buzzActor.posAngles.pos.y - v44 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v44 - 0x2000) >> 8)
								+ ((g_buzzActor.posAngles.pos.z - v45) >> 8) * ((g_buzzActor.posAngles.pos.z - v45) >> 8)
								+ ((g_buzzActor.posAngles.pos.x - v46) >> 8) * ((g_buzzActor.posAngles.pos.x - v46) >> 8);
							x = v55;
						}

						v42 = Levels::g_recordData[6];
					}

					v48 = ++v55 < v42->recordCount;
				} while (v48);

				if (v56 < 0x10000)
				{
					Lighting::g_dynamicLights[1].pos.x = 32 * v42->data[x].x;
					Lighting::g_dynamicLights[1].pos.y = 32 * v42->data[x].y;
					Lighting::g_dynamicLights[1].pos.z = 32 * v42->data[x].z;

					Lighting::g_dynamicLights[1].colorR = v47;
					Lighting::g_dynamicLights[1].colorG = v47;
					Lighting::g_dynamicLights[1].colorB = colorB;
					Lighting::g_dynamicLights[1].lifetime = 1;
					Lighting::g_dynamicLights[1].groupId = (int32_t)&v42->data[x];
				}
				else
				{
				LABEL_210:
					Lighting::g_dynamicLights[1].lifetime = 0;
				}
			}

			if (Buzz::g_groundSlamTicker)
			{
				if ((g_unk52F784 & 1) == 0 && Platform::HadBuzzContactThisFrame(22))
				{
					Platform::SetRotationAngles(22, -384, 0, 0);
					Nu3D::Link::SetRotationRelative8bit(65, -384, 0, 0);
					g_unk52F784 |= 1u;
					Levels::Method3(4, 1);
				}

				if (Buzz::g_groundSlamTicker)
				{
					if ((g_unk52F784 & 2) == 0 && Platform::HadBuzzContactThisFrame(23))
					{
						Platform::SetRotationAngles(23, -384, 0, 0);
						Nu3D::Link::SetRotationRelative8bit(66, -384, 0, 0);
						g_unk52F784 |= 2u;
						Levels::Method3(5, 1);
					}

					if (Buzz::g_groundSlamTicker && (g_unk52F784 & 4) == 0 && Platform::HadBuzzContactThisFrame(24))
					{
						Platform::SetRotationAngles(24, -384, 0, 0);
						Nu3D::Link::SetRotationRelative8bit(67, -384, 0, 0);
						g_unk52F784 |= 4u;
						Levels::Method3(6, 1);
					}
				}
			}

			v50 = 0;

			if ((g_unk52F784 & 1) != 0)
				v50 = -89600;

			if ((g_unk52F784 & 2) != 0)
				v50 = -243200;

			if ((g_unk52F784 & 4) != 0)
			{
				v50 = -406400;
			}
			else if (! v50)
			{
				goto LABEL_243;
			}

			if ((g_unk52F784 & 8) != 0)
			{
				Nu3D::Link::GetTargetPosFixed(68, &p_v1);
				Platform::GetOrigin(21, &a2);
				Nu3D::Link::SetPositionRawAndCommit(68, a2.x >> 5, a2.y >> 5, a2.z >> 5);

				if (a2.y + 57600 >= p_v1.y)
				{
					g_unk52F7DC -= 8 * (int32_t)Renderer::g_frameDelta;
					v51 = g_unk52F7DC;

					if (g_unk52F7DC < 0)
					{
						v51 = 0;
						g_unk52F7DC = 0;
						g_unk52F784 &= ~8u;
					}
				}
				else
				{
					g_unk52F7DC += 8 * (int32_t)Renderer::g_frameDelta;
					v51 = g_unk52F7DC;

					if (g_unk52F7DC > 1024)
					{
						g_unk52F7DC = 1024;

						Platform::SetVelocity(21, 0, (int32_t)Renderer::g_frameDelta << 10, 0);
						goto LABEL_243;
					}
				}

				Platform::SetVelocity(21, 0, (int32_t)Renderer::g_frameDelta * v51, 0);
				goto LABEL_243;
			}

			Nu3D::Link::GetTargetPosFixed(68, &p_v1);
			Platform::GetOrigin(21, &a2);
			Nu3D::Link::SetPositionRawAndCommit(68, a2.x >> 5, a2.y >> 5, a2.z >> 5);

			v48 = a2.y <= v50 + p_v1.y;
			p_v1.y += v50;

			if (v48)
			{
				g_unk52F7DC -= 8 * (int32_t)Renderer::g_frameDelta;
				v52 = g_unk52F7DC;

				if (g_unk52F7DC < 0)
				{
					v52 = 0;
					g_unk52F7DC = 0;
					g_unk52F784 |= 8u;
				}
			}
			else
			{
				g_unk52F7DC += 8 * (int32_t)Renderer::g_frameDelta;
				v52 = g_unk52F7DC;

				if (g_unk52F7DC > 1024)
				{
					v52 = 1024;
					g_unk52F7DC = 1024;
				}
			}

			Platform::SetVelocity(21, 0, -((int32_t)Renderer::g_frameDelta * v52), 0);
		LABEL_243:

			PlayLevelMusic();
		}

	}
}

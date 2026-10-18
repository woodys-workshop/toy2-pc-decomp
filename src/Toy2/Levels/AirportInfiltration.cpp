#include "Numerics.h"
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
#include "SaveManager.h"
#include "Nullsub.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Toy2/Levels/AirportInfiltration.h"

namespace Toy2
{
	namespace AirportInfiltration
	{
		// GLOBAL: TOY2 0x004F4668
		Vector3I g_unk4F4668[5] = { { 0, 0, 500 }, { 500, 0, 0 }, { 0, 0, -1280 }, { -1280, 0, 0 }, { 0, -1280, 0 } };

		// GLOBAL: TOY2 0x004F46AC
		char* g_unk4F46AC[5] = {
			"^hamm^ is on the shelf at the end of the x-ray room.",
			"the ^little tike pilot^ has lost his passengers. he is in the repair hangar.",
			"^rocky gilbraltor^ has a ^challenge^ for you. he is on the floor of the conveyor belt room.",
			"you can get to the ^token^ on top of the broken plane using the ^hover boots^. mr. potato head has them near the stack of luggage.",
			"the ^prospector^ boss is on the conveyor belt at the top of the luggage stack. you need the ^hover boots^ to reach him."
		};

		// GLOBAL: TOY2 0x0052FE38
		int32_t g_unk52FE38;

		// GLOBAL: TOY2 0x0052FE3C
		int32_t g_unk52FE3C;

		// GLOBAL: TOY2 0x0052FE40
		int32_t g_unk52FE40;

		// GLOBAL: TOY2 0x0052FEEC
		HiddenCollectible g_unk52FEEC[5];

		// GLOBAL: TOY2 0x0052FF18
		int32_t g_unk52FF18;

		// GLOBAL: TOY2 0x0052FF20
		int32_t g_unk52FF20;

		// GLOBAL: TOY2 0x0052FF24
		int32_t g_unk52FF24;

		// GLOBAL: TOY2 0x0052FF28
		int32_t g_unk52FF28;

		// GLOBAL: TOY2 0x0052FF34
		int32_t g_unk52FF34;

		// GLOBAL: TOY2 0x0052FF38
		int32_t g_unk52FF38;

		// GLOBAL: TOY2 0x0052FEE8
		int32_t g_unk52FEE8;

		// GLOBAL: TOY2 0x0052FF1C
		int32_t g_unk52FF1C;

		// GLOBAL: TOY2 0x0052FF2C
		int32_t g_unk52FF2C;

		// GLOBAL: TOY2 0x0052FF30
		int32_t g_unk52FF30;

		// GLOBAL: TOY2 0x0052FF3C
		int32_t g_unk52FF3C;

		// GLOBAL: TOY2 0x0052FF44
		int32_t g_unk52FF44;

		// FUNCTION: TOY2 0x0042C3E0
		void Method2(int32_t arg0, int32_t arg4)
		{
			int32_t unkInt6;
			int32_t unkInt1;
			int32_t unkInt2;
			int32_t x;
			int32_t v6;
			int32_t v7;
			int32_t y;
			int32_t v9;
			int32_t v10;
			int32_t z;
			int32_t v12;
			int32_t v13;
			int32_t v14;
			int32_t v15;
			int16_t unkShort4;
			int32_t unkShort3;
			uint16_t unkShort1;
			int32_t v19;
			Vector3I p_v2;
			Vector3I a1;
			Vector3I a2;
			Vector3I v23;

			if (Platform::g_unk559E80 != Platform::g_pathPlatforms[arg0].platformIdx && arg0 != 2 && arg0 != 3
				|| Platform::g_unk559E80 != Platform::g_pathPlatforms[2].platformIdx && Platform::g_unk559E80 != Platform::g_pathPlatforms[3].platformIdx)
			{
				unkInt6 = Platform::g_pathPlatforms[arg0].pathIdx;
				unkInt1 = Platform::g_pathPlatforms[arg0].pathPosition;
				if (unkInt1 <= (Levels::g_recordData[unkInt6]->recordCount - 2) << 12)
				{
					Platform::SamplePoint(unkInt6, unkInt1, &v23);
					Platform::GetOrigin(Platform::g_pathPlatforms[arg0].platformIdx, &p_v2);
					unkInt2 = Platform::g_pathPlatforms[arg0].unkInt2;
					if (unkInt2 < arg4)
						Platform::g_pathPlatforms[arg0].unkInt2 = unkInt2 + 4 * (int32_t)Renderer::g_frameDelta;
					if (Platform::g_pathPlatforms[arg0].unkInt2 > arg4 >> 1 && Platform::g_pathPlatforms[arg0].unkInt3 == p_v2.x
						&& Platform::g_pathPlatforms[arg0].unkInt4 == p_v2.z)
					{
						if (arg0 == 2 || arg0 == 3)
						{
							Platform::g_pathPlatforms[2].unkInt2 = -arg4;
							Platform::g_pathPlatforms[3].unkInt2 = -arg4;
							if (arg0 == 2)
							{
								Platform::GetOrigin(Platform::g_pathPlatforms[3].platformIdx, &a2);
								x = a2.x;
								if (p_v2.x - a2.x > 0)
								{
									x = p_v2.x - 317000;
									a2.x = p_v2.x - 317000;
								}
								Platform::SetOrigin(Platform::g_pathPlatforms[3].platformIdx, x, a2.y, a2.z);
							}
							else
							{
								Platform::GetOrigin(Platform::g_pathPlatforms[2].platformIdx, &a2);
								v6 = a2.x;
								if (p_v2.x - a2.x > 0)
								{
									v6 = p_v2.x - 317000;
									a2.x = p_v2.x - 317000;
								}
								Platform::SetOrigin(Platform::g_pathPlatforms[2].platformIdx, v6, a2.y, a2.z);
							}
						}
						else
						{
							Platform::g_pathPlatforms[arg0].unkInt2 = -arg4;
						}
					}
					v7 = v23.y;
					y = p_v2.y;
					v9 = v23.x;
					v10 = p_v2.x;
					z = p_v2.z;
					Platform::g_pathPlatforms[arg0].unkInt3 = p_v2.x;
					v12 = 32 * v7 - y;
					v13 = 32 * v23.z;
					v14 = 32 * v9 - v10;
					Platform::g_pathPlatforms[arg0].unkInt4 = z;
					v15 = v13 - z;
					a1.x = v14;
					a1.y = v12;
					a1.z = v15;
					if (abs(v14) < 0x4000 && abs(v12) < 0x4000 && abs(v15) < 0x4000)
						Platform::g_pathPlatforms[arg0].pathPosition += 4096;
					while (abs(v14) > 0x4000 || abs(v12) > 0x4000 || abs(v15) > 0x4000)
					{
						v14 >>= 2;
						v12 >>= 2;
						v15 >>= 2;
					}
					a1.x = v14;
					a1.z = v15;
					a1.y = v12;
					Nu3D::Math::NormalizeToFixedPoint(&a1, &a1);
					Platform::SetVelocity(
						Platform::g_pathPlatforms[arg0].platformIdx,
						(a1.x * Platform::g_pathPlatforms[arg0].unkInt2 * (int32_t)Renderer::g_frameDelta) >> 10,
						(a1.y * Platform::g_pathPlatforms[arg0].unkInt2 * (int32_t)Renderer::g_frameDelta) >> 10,
						(a1.z * Platform::g_pathPlatforms[arg0].unkInt2 * (int32_t)Renderer::g_frameDelta) >> 10);
				}
				else
				{
					Platform::g_pathPlatforms[arg0].pathPosition = 0;
					Platform::SamplePoint(Platform::g_pathPlatforms[arg0].pathIdx, 0, &v23);
					Platform::SetOrigin(Platform::g_pathPlatforms[arg0].platformIdx, 32 * v23.x, 32 * v23.y, 32 * v23.z);
					Platform::GetOrigin(Platform::g_pathPlatforms[arg0].platformIdx, &p_v2);
				}
				unkShort4 = Platform::g_pathPlatforms[arg0].linkId2;
				if (unkShort4)
					Nu3D::Link::SetPositionRawAndCommit(unkShort4, p_v2.x >> 5, p_v2.y >> 5, p_v2.z >> 5);
				Nu3D::Link::SetPositionRawAndCommit(Platform::g_pathPlatforms[arg0].linkId, p_v2.x >> 5, p_v2.y >> 5, p_v2.z >> 5);
				if (Buzz::g_groundSlamTicker && Platform::g_pathPlatforms[arg0].rotationY != 1024
					&& (Platform::GetFlags(Platform::g_pathPlatforms[arg0].platformIdx) & 2) == 2)
				{
					Buzz::g_groundSlamTicker = 0;
					AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
					Levels::Method2(-3072, 2);
					unkShort3 = Platform::g_pathPlatforms[arg0].linkId;
					g_unk52FE38 = -512;
					g_unk52FE3C = unkShort3;
					Platform::g_pathPlatforms[arg0].unkShort2 = 0;
				}
				unkShort1 = Platform::g_pathPlatforms[arg0].rotationY;
				if (unkShort1 != 1024 && g_tickState[11] && Platform::g_pathPlatforms[arg0].unkShort2)
				{
					v19 = (unkShort1 + 1024) & 0xFFF;
					if (Nu3D::Math::IsWithinDistance(&Camera::g_activeCameraTransform.pos, &p_v2, 600))
						Nu3D::Particles::SpawnFromPreset(
							p_v2.x + Numerics::g_sinCosLUT[v19], p_v2.y - 0x4000, p_v2.z + Numerics::g_sinCosLUT[(v19 + 1024) & 0xFFF], 115, 2);
				}
			}
		}

		// FUNCTION: TOY2 0x0042C810
		void Method1(Vector3I* spawnPos, int32_t index)
		{
			Nu3D::Particles::ParticleInstance* l_particle;
			int16_t v3;

			if (g_tickState[index != 4 ? 9 : 7])
			{
				if (index == 4)
					l_particle = Nu3D::Particles::SpawnFromPreset(spawnPos->x, spawnPos->y, spawnPos->z, 79, 2);
				else
					l_particle = Nu3D::Particles::SpawnFromPreset(spawnPos->x, spawnPos->y, spawnPos->z, 78, 2);

				l_particle->velX = g_unk4F4668[index].x;
				l_particle->velY = g_unk4F4668[index].y;
				l_particle->velZ = g_unk4F4668[index].z;

				v3 = g_tickAccumulators.tick32Acc << 6;

				l_particle->rotSpeed = -256;
				l_particle->groundAlignRot = 4095 - v3;
			}
		}

		// FUNCTION: TOY2 0x0042C8A0
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
					if (entry->props.linkId == 56)
						g_unk52FEEC[0].yPtr = &entry->pos.y;
					if (entry->props.linkId == 57)
						g_unk52FEEC[1].yPtr = &entry->pos.y;
					if (entry->props.linkId == 58)
						g_unk52FEEC[2].yPtr = &entry->pos.y;
					if (entry->props.linkId == 59)
						g_unk52FEEC[3].yPtr = &entry->pos.y;
					if (entry->props.linkId == 60)
						g_unk52FEEC[4].yPtr = &entry->pos.y;
					++entry;
					++i;
				} while (i < Levels::g_recordData[63]->recordCount);
			}
			for (i = 0; i < 5; ++i)
			{
				g_unk52FEEC[i].savedY = *g_unk52FEEC[i].yPtr;
				*g_unk52FEEC[i].yPtr = 0x80000000;
				Nu3D::Link::SetScaleFromFixedOffsets(i + 56, 0, 0, 0);
			}
		}

		// FUNCTION: TOY2 0x0042CA60
		void Interactions()
		{
			int32_t v0;
			int32_t v1;
			int32_t v2;
			Levels::RecordData* v3;
			int32_t v4;
			int32_t v5;
			int32_t v6;
			int32_t v7;
			char Flags;
			char v9;
			int32_t i;
			int32_t j;
			int32_t v12;
			int32_t* v13;
			int32_t v14;
			int32_t* v15;
			int32_t v16;
			Levels::RecordData* v17;
			int32_t v18;
			int32_t v19;
			int32_t v20;
			int32_t v21;
			bool v22;
			Levels::RecordData* v23;
			int32_t z;
			int32_t v25;
			int32_t v26;
			int32_t v27;
			int32_t v28;
			int32_t v29;
			Vector3I a1;
			int32_t v31;

			if (g_curSectorCopy2 == 2 || g_curSectorCopy2 == 4 || g_curSectorCopy2 == 5)
			{
				g_unk52FF20 = Platform::StepTiltPhysics(3, 4, g_unk52FF20, 2, -224, 224, 128);
				Platform::CommitRotationToLink(3, 6);

				g_unk52FF24 = Platform::StepTiltPhysics(4, 5, g_unk52FF24, 2, -224, 224, 128);
				Platform::CommitRotationToLink(4, 7);
			}

			v0 = 0;

			if (Levels::g_datContents[4].unkByte1)
			{
				v1 = g_unk52FF18;
				v2 = (g_unk52FF28 + 6 * (int32_t)Renderer::g_frameDelta) & 0xFFF;
				g_unk52FF28 = v2;

				if (v2 <= 2048)
				{
					if (g_unk52FF18 < 128)
					{
						v1 = g_unk52FF18 + 2;
						goto LABEL_11;
					}
				}
				else if (g_unk52FF18 > 0)
				{
					v1 = g_unk52FF18 - 2;
				LABEL_11:
					g_unk52FF18 = v1;
				}

				g_unk52FF38 += -3 * (int32_t)Renderer::g_frameDelta * v1;
				g_unk52FF34 += -3 * (int32_t)Renderer::g_frameDelta * (128 - v1);

				Nu3D::Link::SetRotationRelative8bit(0, 0, 0, g_unk52FF38);
				Nu3D::Link::SetRotationRelative8bit(1, 0, 0, g_unk52FF34);
				Nu3D::Link::SetRotationRelative8bit(2, 0, 0, g_unk52FF38);
				Nu3D::Link::SetRotationRelative8bit(3, 0, 0, g_unk52FF34);

				if (g_curSectorCopy2 == 4)
				{
					v3 = Levels::g_recordData[1];
					v26 = 0;

					if (Levels::g_recordData[1]->recordCount)
					{
						do
						{
							g_unk52FF18 = 128 - g_unk52FF18;

							if (g_unk52FF18 > 32)
							{
								a1.x = 32 * v3->data[v0].x;
								a1.y = 32 * v3->data[v0].y;
								a1.z = 32 * v3->data[v0].z;

								if (g_unk52FF18 > 64 || g_unk52FF28 < 2560)
								{
									Method1(&a1, 3);

									v3 = Levels::g_recordData[1];
								}

								v4 = v3->data[v0].x / 8;
								v31 = v4 - g_buzzActor.posAngles.pos.x / 256;
								v5 = v3->data[v0].y / 8 - g_buzzActor.posAngles.pos.y / 256;
								v6 = v3->data[v0].z / 8 - g_buzzActor.posAngles.pos.z / 256;
								v7 = v4 - v3->data[v0 + 1].x / 8 - 32;

								if (v5 * v5 + v6 * v6 < 0x10000 && v31 > 0 && v31 < v7)
								{
									Levels::Method1(0, 32 * v31 * g_unk52FF18 / v7 - 32 * g_unk52FF18, 0, 0);
									v3 = Levels::g_recordData[1];
								}
							}

							v0 += 2;
							v26 += 2;
						} while (v26 < v3->recordCount);
					}
				}
			}

			Flags = Platform::GetFlags(0);

			if (Buzz::g_groundSlamTicker && (Flags & 2) == 2)
			{
				Buzz::g_groundSlamTicker = 0;
				Levels::Method2(-3072, 2);
				Levels::Method3(0, 1);
				AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
				g_unk52FE38 = -512;
				g_unk52FE3C = 9;
			}

			v9 = Platform::GetFlags(14);

			if (Buzz::g_groundSlamTicker && (v9 & 2) == 2)
			{
				Buzz::g_groundSlamTicker = 0;
				Levels::Method2(-3072, 2);
				AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
				Levels::Method3(1, 1);
				g_unk52FE38 = -512;
				g_unk52FE3C = 28;
			}

			Renderer::BlitTextureByIndexOffset(5, 0, 0, 64, 64, 0, (-1 - g_tickAccumulators.tick64Acc) & 0x3F, 0, 64);

			if (g_unk52FE38)
			{
				g_unk52FE38 += 32 * (int32_t)Renderer::g_frameDelta;

				if (g_unk52FE38 > 0)
					g_unk52FE38 = 0;

				Nu3D::Link::GetRotation8Bit(g_unk52FE3C, &a1);
				Nu3D::Link::SetRotationAbsolute8bit(g_unk52FE3C, 0, a1.y, g_unk52FE38);
			}

			Method2(0, 125);
			Method2(1, 125);
			Method2(2, 125);
			Method2(3, 125);
			Method2(4, 125);

			if (g_curSectorCopy2 == 3)
				goto LABEL_39;

			if (g_unk52FE40)
			{
				for (i = 16; i < 20; ++i)
					Nu3D::Link::SetScaleFromFixedOffsets(i, 0, 0, 0);

				g_unk52FE40 = 0;
			}

			if (g_curSectorCopy2 == 3)
			{
			LABEL_39:
				if (! g_unk52FE40)
				{
					for (j = 16; j < 20; ++j)
						Nu3D::Link::SetScaleFromFixedOffsets(j, 4096, 4096, 4096);
					g_unk52FE40 = 1;
				}
			}

			if (Buzz::g_unk52B81C == 32)
				Levels::Method1(0, 0, 0, 1024);

			if (Buzz::g_unk52B81C == 33)
				Levels::Method1(0, 1024, 0, 0);

			if (Buzz::g_unk52B81C == 34)
				Levels::Method1(0, -512, 0, 886);

			if (Buzz::g_unk52B81C == 35)
				Levels::Method1(0, -658, 0, -784);

			if (Buzz::g_unk52B81C == 36)
				Levels::Method1(0, -1024, 0, 0);

			Actor::ItemReturnReward(
				8,
				31,
				"mmm.. mumpf! mmmmu mo mmmme ^hover boots^ mmmm mub ^mouth^ mmmum mi mo mmmump!",
				"wow! thanks buzz! in return for finding my ^mouth^ i will let you use the ^hover boots^. you can float higher by holding down the ^jump^ "
				"button and you can float lower by letting go of the ^jump^ button.",
				"with the ^hover boots^ you can float higher by holding down the ^jump^ button and you can float lower by letting go of the ^jump^ button.",
				-1,
				3584);
			Actor::CollectQuestReward(6, 29, -1, 0, 0);
			Actor::RotatingHint(33, 34, g_unk4F46AC);

			if ((g_levelActors[7].actorFlags & 0x200) != 0)
			{
				g_levelActors[7].actorFlags &= ~0x200u;
				if (g_collectionCount >= 0)
				{
					if (g_collectionCount == 5)
					{
						Dialogue::Begin(7, 33, "thanks for finding my ^little tike passengers^ buzz! here is a pizza planet ^token^!", -1, 0, 1);
						g_collectionCount = -1;
					}
					else
					{
						Dialogue::Begin(
							7,
							33,
							"hi buzz! if you can find ^five little tike passengers^ for my next flight i will give you a pizza planet ^token^.",
							-1,
							0,
							-1);
					}
				}
			}

			Actor::PlayPeriodicHintSound(5, 181);

			if ((g_levelActors[5].actorFlags & 0x200) != 0)
			{
				g_levelActors[5].actorFlags &= ~0x200u;

				if (g_unk52F2F8)
				{
					if (g_unk830D4C >= 5)
					{
						if (g_unk52F2F8 != 3)
						{
							AudioManager::PlaySoundEffect(183, &g_levelActors[5].transform.pos);
							Dialogue::Begin(5, 30, "well done! you have found all of my weights! here is your pizza planet ^token^!", -1, 0, 2);
							g_unk52F2F8 = 3;
							goto LABEL_75;
						}
					}
					else
					{
						Dialogue::Begin(5, 30, "quick! you need to find more weights!", -1, 0, -1);
					}
				}
				else
				{
					AudioManager::PlaySoundEffect(182, &g_levelActors[5].transform.pos);
					Dialogue::Begin(
						5, 30, "hi buzz! if you can bring me ^five^ weights before you run out of time, i will give you a pizza planet ^token^.", -1, 0, -1);
					g_unk830D4C = 0;
					g_unk52F2F8 = 1;

					for (v12 = 0; v12 < 5; ++v12)
					{
						*g_unk52FEEC[v12].yPtr = g_unk52FEEC[v12].savedY;
						Nu3D::Link::SetScaleFromFixedOffsets(v12 + 56, 4096, 4096, 4096);
					}
				}
			}

			if (g_unk52F2F8 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_75;

				g_unk52F2F8 = 2;
				g_unk52AD64 = 160;
			}
			else if (g_unk52F2F8 != 2)
			{
				goto LABEL_75;
			}

			if (g_tickState[11])
				--g_unk52AD64;

			if (g_unk52AD64 < 100)
			{
				g_unk52AD64 = 100;
				g_unk52F2F8 = 0;

				for (v14 = 0; v14 < 5; ++v14)
				{
					*g_unk52FEEC[v14].yPtr = 0x80000000;
					Nu3D::Link::SetScaleFromFixedOffsets(v14 + 56, 0, 0, 0);
				}
			}
		LABEL_75:

			v16 = g_unk52FF1C;
			if (! g_unk52FF1C)
			{
				if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &g_levelActors[32].transform.pos, 300) && g_buzzActor.posAngles.pos.y < -222734
					&& g_buzzActor.collisionFlags)
				{
					g_unk52FF1C = 1;
					Dialogue::Begin(32, 32, "ha ha ha ha ... defeat the ^prospector^ boss to get a pizza planet ^token^!", -1, 0, -1);
				}

				v16 = g_unk52FF1C;
			}

			if (v16 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_89;

				g_unk830D44 = 180;
				g_unk52FF1C = 2;
				g_levelActors[32].movementData = Actor::g_movementData[41] + 14;
				g_levelActors[32].unkShort1 = 0;
				g_levelActors[32].creatureRam->initialFacingAngle = 0;

				v16 = g_unk52FF1C;
			}

			if (v16 >= 3)
			{
				if (v16 >= 120)
				{
					if (v16 != 200)
					{
						Collectables::Activate(4, 0);
						g_unk52FF1C = 200;
					}
				}
				else
				{
					g_unk52FF1C = (int32_t)Renderer::g_frameDelta + v16;
				}
			}
		LABEL_89:

			v17 = Levels::g_recordData[10];
			v27 = 0x7FFFFFFF;
			v25 = 0;

			if (! Levels::g_recordData[10]->recordCount)
				goto LABEL_98;

			v18 = 0;
			v28 = 0;

			do
			{
				v19 = 32 * v17->data[v25].y;
				v20 = 32 * v17->data[v25].z;
				v21 = 32 * v17->data[v25].x;

				if (((Camera::g_activeCameraTransform.pos.x - v21) >> 8) * ((Camera::g_activeCameraTransform.pos.x - v21) >> 8)
						+ ((Camera::g_activeCameraTransform.pos.y - v19) >> 8) * ((Camera::g_activeCameraTransform.pos.y - v19) >> 8)
						+ ((Camera::g_activeCameraTransform.pos.z - v20) >> 8) * ((Camera::g_activeCameraTransform.pos.z - v20) >> 8)
					< 1000000)
				{
					Renderer::LensFlare::RegisterLight(v21, v19, v20, 48, 64, 80, 64);
					if (((g_buzzActor.posAngles.pos.y - v19 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v19 - 0x2000) >> 8)
							+ ((g_buzzActor.posAngles.pos.z - v20) >> 8) * ((g_buzzActor.posAngles.pos.z - v20) >> 8)
							+ ((g_buzzActor.posAngles.pos.x - v21) >> 8) * ((g_buzzActor.posAngles.pos.x - v21) >> 8)
						< v27)
					{
						v27 = ((g_buzzActor.posAngles.pos.y - v19 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v19 - 0x2000) >> 8)
							+ ((g_buzzActor.posAngles.pos.z - v20) >> 8) * ((g_buzzActor.posAngles.pos.z - v20) >> 8)
							+ ((g_buzzActor.posAngles.pos.x - v21) >> 8) * ((g_buzzActor.posAngles.pos.x - v21) >> 8);
						v29 = v25;
					}

					v17 = Levels::g_recordData[10];
				}

				v18 = v28 + 12;
				v22 = ++v25 < v17->recordCount;
				v28 += 12;
			} while (v22);

			if (v27 < 0x10000)
			{
				Lighting::g_dynamicLights[1].pos.x = 32 * v17->data[v29].x;
				Lighting::g_dynamicLights[1].pos.y = 32 * v17->data[v29].y;

				z = v17->data[v29].z;

				Lighting::g_dynamicLights[1].colorR = 111;
				Lighting::g_dynamicLights[1].pos.z = 32 * z;
				Lighting::g_dynamicLights[1].colorG = 127;
				Lighting::g_dynamicLights[1].colorB = -113;
				Lighting::g_dynamicLights[1].lifetime = 1;
				Lighting::g_dynamicLights[1].groupId = (int32_t)&v17->data[v29];
				PlayLevelMusic();
			}
			else
			{
			LABEL_98:
				Lighting::g_dynamicLights[1].lifetime = 0;
				PlayLevelMusic();
			}
		}

	}
}

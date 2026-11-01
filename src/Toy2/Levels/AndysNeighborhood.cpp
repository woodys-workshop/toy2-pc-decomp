#include "Toy2/Levels/AndysNeighborhood.h"

#include "Toy2/Toy2.h"
#include "Toy2/KiteTail.h"
#include "Toy2/Camera.h"
#include "Toy2/Collectables.h"
#include "Toy2/Dialogue.h"
#include "Toy2/MoveableObject.h"
#include "Toy2/Platform.h"
#include "Toy2/Collision.h"

#include "Nu3D/Link.h"
#include "Nu3D/Math.h"
#include "Nu3D/Collision.h"

#include "Renderer/Renderer.h"
#include "AudioManager/AudioManager.h"
#include "Random.h"

namespace Toy2
{
	namespace AndysNeighborhood
	{
		// GLOBAL: TOY2 0x004F1508
		int16_t g_unk4F1508[8] = { 0, 1, 3, 2, 3, 1, 0, 2 };

		// GLOBAL: TOY2 0x004F1518
		int16_t g_unk4F1518[8] = { -2, 2, 4, -2, 11, 20, -1, 0 };

		// GLOBAL: TOY2 0x004F1528
		char* g_unk4F1528[5] = {
			"^hamm^ is on the first branch of the ^tree^. climb the ^swing set^ to get to him.",
			"^sarge^ has lost his troops. he is on the steps of andy's house. the troops are using ^flares^ to help you find them. ",
			"^r.c. car^ is waiting to race you around the garden. i better warn you though you won't be able to beat him without your ^rocket jet boots^ and "
			"your ^rocket jet boots^ aren't available until you help ^mr. potato head^ on another level.",
			"you can inflate the ^rubber duck^ to get to the token over the ^swimming pool^.",
			"the ^kite boss^ is at the top of the tree. hey buzz, you can find a rope on the way up! jump onto it and it will fall to the ground. that way you "
			"can quickly get back into the tree."
		};

		// $TODO: The original loader reads past this unterminated table; adjacent data placement is unresolved.
		// GLOBAL: TOY2 0x004F153C
		Collectables::TokenTableEntry g_unk4F153C[1] = {
			{ 70, 12, "the soldier will surrender if you quickly ^stomp^ on the ^molehills^ that he is near.", 2048 }
		};

		// GLOBAL: TOY2 0x004F154C
		int16_t g_unk4F154C[6] = { 51, 49, 50, 48, 52, 0 };

		// GLOBAL: TOY2 0x0052F5DC
		int32_t g_unk52F5DC;

		// GLOBAL: TOY2 0x0052F5E0
		int32_t g_unk52F5E0;

		// GLOBAL: TOY2 0x0052F5E4
		int32_t g_unk52F5E4;

		// GLOBAL: TOY2 0x0052F5E8
		int32_t g_unk52F5E8;

		// GLOBAL: TOY2 0x0052F5EC
		int32_t g_unk52F5EC;

		// GLOBAL: TOY2 0x0052F5F0
		int32_t g_unk52F5F0;

		// GLOBAL: TOY2 0x0052F5F8
		Vector3I g_unk52F5F8;

		// GLOBAL: TOY2 0x0052F608
		int16_t g_unk52F608[2];

		// GLOBAL: TOY2 0x0052F60C
		int32_t g_unk52F60C;

		// GLOBAL: TOY2 0x0052F610
		int32_t g_unk52F610;

		// GLOBAL: TOY2 0x0052F614
		int32_t g_unk52F614;

		// GLOBAL: TOY2 0x0052F618
		int32_t g_unk52F618;

		// GLOBAL: TOY2 0x0052F61C
		int32_t g_unk52F61C;

		// GLOBAL: TOY2 0x0052F620
		int32_t g_unk52F620;

		// GLOBAL: TOY2 0x0052F628
		Vector3I g_unk52F628;

		// GLOBAL: TOY2 0x0052F638
		int16_t g_unk52F638[2];

		// GLOBAL: TOY2 0x0052F640
		int32_t g_unk52F640;

		// GLOBAL: TOY2 0x0052F648
		Vector3I g_unk52F648;

		// GLOBAL: TOY2 0x0052F658
		int32_t g_unk52F658;

		// GLOBAL: TOY2 0x0052F660
		int32_t g_unk52F660;

		// GLOBAL: TOY2 0x0052F668
		int32_t g_unk52F668;

		// GLOBAL: TOY2 0x0052F66C
		int32_t g_unk52F66C;

		// GLOBAL: TOY2 0x0052F674
		int32_t g_unk52F674;

		// GLOBAL: TOY2 0x0052F678
		int32_t g_unk52F678;

		// GLOBAL: TOY2 0x0052F680
		Vector3I g_unk52F680;

		// GLOBAL: TOY2 0x0052F690
		int32_t g_unk52F690;

		// GLOBAL: TOY2 0x0052F6A8
		int32_t g_unk52F6A8;

		// GLOBAL: TOY2 0x0052F6B0
		int32_t g_unk52F6B0;

		// GLOBAL: TOY2 0x0052F6B4
		int32_t g_unk52F6B4;

		// GLOBAL: TOY2 0x0052F6BC
		int32_t g_unk52F6BC;

		// GLOBAL: TOY2 0x0052F6C0
		int32_t g_unk52F6C0;

		// GLOBAL: TOY2 0x0052F6C4
		int32_t g_unk52F6C4;

		// GLOBAL: TOY2 0x0052F6C8
		int32_t g_unk52F6C8;

		// GLOBAL: TOY2 0x0052F6CC
		int32_t g_unk52F6CC;

		// GLOBAL: TOY2 0x0052F6D0
		int32_t g_unk52F6D0;

		// GLOBAL: TOY2 0x0052F6D4
		int32_t g_unk52F6D4;

		// GLOBAL: TOY2 0x0052F6D8
		int32_t g_unk52F6D8;

		// GLOBAL: TOY2 0x0052F6DC
		int32_t g_unk52F6DC;

		// GLOBAL: TOY2 0x0052F6E0
		int32_t g_unk52F6E0;

		// GLOBAL: TOY2 0x0052F6E4
		int32_t g_unk52F6E4;

		// GLOBAL: TOY2 0x0052F6E8
		int32_t g_unk52F6E8;

		// GLOBAL: TOY2 0x0052F63C
		int32_t g_unk52F63C;

		// GLOBAL: TOY2 0x0052F65C
		int32_t g_unk52F65C;

		// GLOBAL: TOY2 0x0052F664
		int32_t g_unk52F664;

		// GLOBAL: TOY2 0x0052F5D8
		int32_t g_zKiteFlashTimer;

		// GLOBAL: TOY2 0x0052F6AC
		int32_t g_zKitePhase;

		// GLOBAL: TOY2 0x0052F6B8
		int32_t g_zKiteYawPhase;

		// GLOBAL: TOY2 0x0052F6EC
		int32_t g_zKiteRollPhase;

		// FUNCTION: TOY2 0x00418E50
		void Initialize()
		{
			Collectables::Init(g_unk4F154C, 65);
			Collectables::Activate(3, 1);
			MoveableObject::InitTable(g_unk4F1518);
			Collectables::LoadTokenTable((int32_t*)g_unk4F153C);
			Buzz::g_underwaterYLevel = 17408;
			Buzz::g_unk830D30 = 17408;
			g_underwaterTintBlue = 80;
			g_underwaterTintGreen = 96;
			g_underwaterTintRed = 128;
			Platform::SetRotationAngles(0, 0, 2401, 0);
			Platform::SetRotationAngles(1, 0, 2890, 0);
			g_levelActors[26].actorFlags |= Actor::ACTOR_NO_DESPAWN;
			g_levelActors[4].unkShort4 = 100;
			g_zKitePhase = g_levelActors[26].actorPhase;
			g_zKiteYawPhase = 0;
			g_zKiteRollPhase = 0;
			g_unk52F65C = 0;
			g_unk52F664 = 0;
			g_zKiteFlashTimer = 0;
			g_unk52F63C = 0;
			g_unk52F6DC = 0;
			g_unk52F6E0 = 0;
			g_unk52F2F8 = 0;
			g_unk52F668 = 0;
			g_unk52F620 = 0;
			g_unk52AD64 = 0;
			g_unk52F660 = 0;
			g_unk52F5E0 = 0;
			g_unk52F6B0 = 0;
			g_unk52F6E4 = 0;
			g_unk52F678 = 1;
			g_unk52F6B4 = 4095;
			g_unk52F674 = 0;
			g_unk52F690 = 0;
			g_unk52F60C = 0;
			g_unk52F6C8 = 4096;
			g_unk52F6CC = 4096;
			g_unk52F618 = 0;
			g_unk52F614 = 0;
			g_unk52F6D8 = 3072;
			g_unk52F66C = 0;
			g_unk52F610 = 0;
			g_unk52F5F0 = 750;
			g_unk52F5E4 = 0;
			g_unk52F5EC = 0;
			g_unk52F640 = 0;
			g_unk52F5DC = 0;
			g_unk52F6D4 = 0;
			g_unk52F61C = 0;
			g_unk52F5E8 = 0;
			g_unk52F6D0 = 0;
			g_unk52F638[0] = 0;
			g_unk52F658 = 0;
			g_unk52F6E8 = 0;
			Nu3D::Link::SetRotationRelative8bit(5, 0, 0, 750);
			Nu3D::Link::GetCurrentPosFixed(6, &g_unk52F648);
			Platform::DisableCollision(5);
			Platform::DisableCollision(4);
			Platform::DisableCollision(12);
			Platform::AddFlags(7, 256);
			Platform::SetOrigin(7, g_unk52F648.x, g_unk52F648.y, g_unk52F648.z);
			Nu3D::Link::SetScaleFromFixedOffsets(21, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(32, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(33, 0, 0, 0);
			Nu3D::Link::GetCurrentPosFixed(25, &g_unk52F680);
			g_unk52F608[0] = 280;
			Nu3D::Link::SetScaleFromFixedOffsets(27, 4096, 280, 4096);
			Levels::g_recordData[61]->data[0].y = g_unk52F680.y;
			g_unk52F6A8 = 0;
			g_unk52F6C4 = 0;
			KiteTail::Init(&g_levelActors[26].transform.pos, 16, 4096, 0, 96);
		}

		// FUNCTION: TOY2 0x00448080
		void Method1()
		{
			int32_t l_index;
			Levels::Type63* l_data;
			int32_t l_unk;
			Transform l_pos;

			l_index = 0;
			l_data = (Levels::Type63*)Levels::g_recordData[63]->data;

			if (Levels::g_recordData[63]->recordCount)
			{
				do
				{
					if (l_data->pos.y != 0x80000000 && l_data->props.linkId < 0x30u)
					{
						l_pos.pos.x = 32 * l_data->pos.x;
						l_pos.pos.z = 32 * l_data->pos.z;
						l_pos.pos.y = 32 * (l_data->pos.y - 10);

						l_unk = Nu3D::Collision::GetGroundHeightEx(&l_pos, 0, 0) >> 5;

						l_data->props.floorPos = l_unk;

						if (l_unk == 0x7FFF)
							l_data->props.floorPos = 32766;

						if (l_data->props.floorPos - l_data->pos.y > 6144)
							l_data->props.floorPos = 0x7FFF;
					}
					++l_data;
					++l_index;
				} while (l_index < Levels::g_recordData[63]->recordCount);
			}
		}

		// FUNCTION: TOY2 0x004190C0
		void Interactions()
		{
			int32_t v74;
			Transform transform;

			g_unk52F6DC = Platform::StepTiltPhysics(0, 0, g_unk52F6DC, 0, -448, 448, 8);
			g_unk52F6E0 = Platform::StepTiltPhysics(1, 1, g_unk52F6E0, 0, -448, 224, 8);

			if (! g_buzzActor.collisionFlags || ! Buzz::g_groundSlamTicker || Buzz::g_unk52B81C < 8 || (v74 = Buzz::g_unk52B81C, Buzz::g_unk52B81C > 11))
			{
				v74 = 0;
			}

			if (g_buzzActor.posAngles.pos.x > -149248)
			{
				Buzz::g_underwaterYLevel = 6144;
				Buzz::g_currentFluidType = 2;
			}
			else
			{
				Buzz::g_underwaterYLevel = 17408;
				Buzz::g_currentFluidType = 1;
			}

			Actor::CollectQuestReward(15, 8, 3072, 1024, 0);
			Actor::RotatingHint(27, 11, g_unk4F1528);
			Actor::PlayPeriodicHintSound(6, 178);

			if ((g_levelActors[6].actorFlags & 0x200) != 0)
			{
				g_levelActors[6].actorFlags &= ~512u;

				if (g_collectionCount >= 0)
				{
					if (g_collectionCount == 5)
					{
						AudioManager::Preset::PlayOneShotSound2(180, &g_levelActors[6]);
						Dialogue::Begin(6, 7, "great job locating the ^troops^! here is your pizza planet ^token^.", 1792, 512, 1);
						g_collectionCount = -1;
					}
					else
					{
						AudioManager::Preset::PlayOneShotSound2(179, &g_levelActors[6]);
						Dialogue::Begin(
							6,
							7,
							"i need your help, space ranger! ^five^ of my ^troops^ are lost. look for the ^flares^ they are using to signal to you. locate "
							"them for me and i will give you a pizza planet ^token^!",
							1792,
							512,
							-1);
					}
				}
			}

			if ((g_levelActors[29].actorFlags & 0x200) != 0)
			{
				g_levelActors[29].actorFlags &= ~512u;
				if (Collectables::g_pickupSlots[2].state != 2 && ! g_unk52F2F8)
				{
					Dialogue::Begin(
						29,
						5,
						"hi buzz! i challenge you to a race! we will race ^3^ laps around the garden. if you beat me, i will give you a pizza planet ^token.^ "
						"get ready to race 3... 2... 1...",
						0,
						0,
						-1);

					g_unk52F2F8 = 1;
					g_unk52F6B0 = 0;
					g_unk52F668 = 0;
					g_unk52F620 = 0;
					g_unk52AD64 = 0;
					g_unk52F660 = 0;
					g_unk52F5E0 = 1;

					g_levelActors[29].actorPhase = 202;
				}
			}

			if (g_unk52F2F8 == 1)
			{
				if (! Renderer::g_enableCinematicBars)
				{
					g_levelActors[29].actorFlags |= 4;
					g_unk52F2F8 = 2;
				}
			}

			if (g_unk52F2F8 >= 2)
			{
				if (g_unk52F2F8 == 2)
				{
					int32_t px = g_buzzActor.posAngles.pos.x;
					int32_t pz = g_buzzActor.posAngles.pos.z;

					if (g_unk52F6B0 < 4)
					{
						int32_t v3 = 0;

						if (px > 152386)
							v3 = 1;

						if (pz > 248855)
							v3 += 2;

						if (v3 == g_unk4F1508[g_unk52F6B0])
							++g_unk52F6B0;
					}
					else
					{
						if (g_unk52F6B0 < 8)
						{
							int32_t v5 = 0;

							if (px > 47554)
								v5 = 1;

							if (pz > -270441)
								v5 += 2;

							if (v5 == g_unk4F1508[g_unk52F6B0])
								++g_unk52F6B0;
						}

						if (g_unk52F6B0 == 8 && pz > -148073)
						{
							g_unk52F6B0 = 0;
							++g_unk52AD64;
						}
					}
				}

				if (g_levelActors[29].transform.pos.y > 0x2000)
				{
					g_levelActors[29].transform.pos.y = 0x2000;
					int32_t v6 = 32 * (*g_randDatBufferPtr - 128);

					g_randDatBufferPtr += 2;
					Nu3D::Particles::SpawnFromPreset(g_levelActors[29].transform.pos.x + v6, 0x2000, v6 + g_levelActors[29].transform.pos.z, 53, 4);
				}

				int32_t v7 = Levels::g_recordData[1]->data[g_unk52F668].x - (g_levelActors[29].transform.pos.x >> 5);
				int32_t* p_z = &Levels::g_recordData[1]->data[g_unk52F668].z;

				if (v7 * v7 + (*p_z - (g_levelActors[29].transform.pos.z >> 5)) * (*p_z - (g_levelActors[29].transform.pos.z >> 5)) < 360000)
				{
					g_levelActors[29].boundary.x = 32 * Levels::g_recordData[1]->data[g_unk52F668].x;
					g_levelActors[29].boundary.z = 32 * *p_z;

					if (g_unk52F620 < 3)
					{
						++g_unk52F668;
						if (g_unk52F668 > 64)
						{
							g_unk52F668 = 0;
							++g_unk52F620;
						}
					}
					else
					{
						if (g_levelActors[29].transform.pos.z > -148073)
						{
							if (g_unk52F2F8 == 2 && g_unk52AD64 < 3)
								g_unk52F2F8 = 3;

							g_levelActors[29].actorPhase = 1;
						}
					}
				}

				g_levelActors[29].motionTargetPos.x = 32 * Levels::g_recordData[1]->data[g_unk52F668].x;
				g_levelActors[29].motionTargetPos.z = 32 * Levels::g_recordData[1]->data[g_unk52F668].z;

				if (g_unk52AD64 == 3)
				{
					Collectables::Activate(2, 0);
					g_unk52F2F8 = 3;
				}
			}

			if ((g_buzzActor.posAngles.pos.x < -159744 || g_buzzActor.posAngles.pos.x > 162225 && g_buzzActor.posAngles.pos.z < 0
				 || g_buzzActor.posAngles.pos.y < -99029)
				&& g_unk52F2F8 && (g_levelActors[29].actorFlags & 1) == 0 && ! Collectables::g_pickupSlots[2].state)
			{
				g_unk52F2F8 = 0;
				Game::InitActor(&g_levelActors[29], 0);
			}

			if ((g_levelActors[4].actorFlags & 2) != 0 && (g_levelActors[4].actorFlags & 0x20) != 0
				&& ((g_buzzActor.posAngles.pos.x - g_levelActors[4].transform.pos.x)
					>> 5) * ((g_buzzActor.posAngles.pos.x - g_levelActors[4].transform.pos.x) >> 5)
						+ ((g_buzzActor.posAngles.pos.z - g_levelActors[4].transform.pos.z) >> 5)
							* ((g_buzzActor.posAngles.pos.z - g_levelActors[4].transform.pos.z) >> 5)
						+ ((g_buzzActor.posAngles.pos.y - g_levelActors[4].transform.pos.y) >> 5)
							* ((g_buzzActor.posAngles.pos.y - g_levelActors[4].transform.pos.y) >> 5)
					< 589824
				&& g_levelActors[4].motionTargetPos.y != 20480)
			{
				g_unk52F5E8 = g_unk52F5DC;
				g_levelActors[4].motionTargetPos.y = 20480;
				g_unk52F61C = 100;
				AudioManager::PlaySoundEffect(49, &g_levelActors[4].transform.pos);
			}

			if (g_levelActors[4].transform.pos.y > 18432)
			{
				++g_unk52F5DC;

				if (g_unk52F5DC >= Levels::g_recordData[2]->recordCount)
					g_unk52F5DC -= Levels::g_recordData[2]->recordCount;

				if (Levels::g_recordData[2]->data[g_unk52F5DC].y == 0x80000000 || g_unk52F5DC == g_unk52F5E8)
				{
					int32_t v13;

					for (v13 = 0; v13 < Levels::g_recordData[2]->recordCount; ++v13)
					{
						++g_unk52F5DC;

						if (g_unk52F5DC >= Levels::g_recordData[2]->recordCount)
							g_unk52F5DC = 0;

						if (Levels::g_recordData[2]->data[g_unk52F5DC].y != 0x80000000 && g_unk52F5DC != g_unk52F5E8)
							break;
					}
				}

				g_levelActors[4].motionTargetPos.x = 32 * Levels::g_recordData[2]->data[g_unk52F5DC].x;
				g_levelActors[4].motionTargetPos.y = 32 * Levels::g_recordData[2]->data[g_unk52F5DC].y;
				g_levelActors[4].motionTargetPos.z = 32 * Levels::g_recordData[2]->data[g_unk52F5DC].z;

				g_levelActors[4].boundary.x = g_levelActors[4].motionTargetPos.x;
				g_levelActors[4].boundary.y = g_levelActors[4].motionTargetPos.y;
				g_levelActors[4].boundary.z = g_levelActors[4].motionTargetPos.z;
				g_levelActors[4].transform.pos.x = g_levelActors[4].motionTargetPos.x;
				g_levelActors[4].transform.pos.y = 14336;
				g_levelActors[4].transform.pos.z = g_levelActors[4].motionTargetPos.z;
				g_levelActors[4].unkVar9 = 416;
			}

			int32_t v75;

			for (v75 = 0; v75 < (int32_t)Renderer::g_frameDelta; ++v75)
			{
				if (Levels::g_recordData[2]->data[g_unk52F6D4].y == 0x80000000)
				{
					if (((Camera::g_activeCameraTransform.pos.x - 32 * Levels::g_recordData[2]->data[g_unk52F6D4].x)
						 >> 8) * ((Camera::g_activeCameraTransform.pos.x - 32 * Levels::g_recordData[2]->data[g_unk52F6D4].x) >> 8)
							+ ((Camera::g_activeCameraTransform.pos.z - 32 * Levels::g_recordData[2]->data[g_unk52F6D4].z) >> 8)
								* ((Camera::g_activeCameraTransform.pos.z - 32 * Levels::g_recordData[2]->data[g_unk52F6D4].z) >> 8)
							+ ((Camera::g_activeCameraTransform.pos.y + 2992) >> 8) * ((Camera::g_activeCameraTransform.pos.y + 2992) >> 8)
						< 0x40000)
					{
						Nu3D::Particles::SpawnFromPreset(
							32 * Levels::g_recordData[2]->data[g_unk52F6D4].x, -2992, 32 * Levels::g_recordData[2]->data[g_unk52F6D4].z, 58, 3);
					}
				}

				++g_unk52F6D4;

				if (g_unk52F6D4 >= Levels::g_recordData[2]->recordCount)
					g_unk52F6D4 = 0;
			}

			if (g_unk52F61C > 0)
			{
				g_unk52F61C -= (int32_t)Renderer::g_frameDelta;

				if (g_unk52F61C <= 0)
				{
					g_unk52F61C = 0;
					AudioManager::ClearSequence7Cursor();
				}

				if (g_unk52F61C < 80 && g_unk52F61C + (int32_t)Renderer::g_frameDelta >= 80)
				{
					g_unk52F5F8.x = 32 * Levels::g_recordData[2]->data[g_unk52F5E8].x;
					g_unk52F5F8.y = -9136;
					g_unk52F5F8.z = 32 * Levels::g_recordData[2]->data[g_unk52F5E8].z;

					Nu3D::Particles::SpawnFromPreset(g_unk52F5F8.x, -9136, g_unk52F5F8.z, 59, 2);
					AudioManager::StartSoundSequenceOnActor(-3, (Actor::Toy2Actor*)&g_unk52F5F8);
				}
			}

			if (g_buzzActor.collisionFlags)
			{
				if (Buzz::g_groundSlamTicker < -20 && g_unk52F5DC != g_unk52F5E8)
				{
					if (g_unk52F61C)
					{
						if (((g_buzzActor.posAngles.pos.x - 32 * Levels::g_recordData[2]->data[g_unk52F5E8].x)
							 >> 8) * ((g_buzzActor.posAngles.pos.x - 32 * Levels::g_recordData[2]->data[g_unk52F5E8].x) >> 8)
								+ ((g_buzzActor.posAngles.pos.z - 32 * Levels::g_recordData[2]->data[g_unk52F5E8].z) >> 8)
									* ((g_buzzActor.posAngles.pos.z - 32 * Levels::g_recordData[2]->data[g_unk52F5E8].z) >> 8)
								+ ((g_buzzActor.posAngles.pos.y + 2992) >> 8) * ((g_buzzActor.posAngles.pos.y + 2992) >> 8)
							< 2304)
						{
							AudioManager::ClearSequence7Cursor();
							Levels::g_recordData[2]->data[g_unk52F5E8].y = 0x80000000;
							g_unk52F61C = 0;

							if (++g_unk52F6D0 >= 6)
							{
								g_levelActors[4].actorFlags = (g_levelActors[4].actorFlags & 0xFF5B) | 0x84;
							}

							int32_t i;

							for (i = 0; i < 64; ++i)
							{
								if (Nu3D::Particles::g_particleInstances[i].typeId == 59 && Nu3D::Particles::g_particleInstances[i].lifetime > 0)
									Nu3D::Particles::g_particleInstances[i].lifetime = 1;
							}
						}
					}
				}
			}

			if (g_tickState[g_buzzActor.posAngles.pos.y >= -48640 ? 4 : 9] && (*g_randDatBufferPtr++ & 7) == 0)
			{
				g_unk52F678 = 1;
			}

			if (g_unk52F678)
			{
				int32_t v76 = 32 * Levels::g_recordData[0]->data[g_unk52F6E4].y;

				if (g_buzzActor.posAngles.pos.y - v76 > 28672 && g_buzzActor.posAngles.pos.y - v76 < 327680)
				{
					int32_t v31 = 32 * (Levels::g_recordData[0]->data[g_unk52F6E4].x + 4 * *g_randDatBufferPtr++ - 512);
					int32_t v32 = 32 * (Levels::g_recordData[0]->data[g_unk52F6E4].z + 4 * *g_randDatBufferPtr++ - 512);

					if (((g_buzzActor.posAngles.pos.x - v31) >> 8) * ((g_buzzActor.posAngles.pos.x - v31) >> 8)
							+ ((g_buzzActor.posAngles.pos.z - v32) >> 8) * ((g_buzzActor.posAngles.pos.z - v32) >> 8)
						< 640000)
					{
						Nu3D::Particles::ParticleInstance* v33 = Nu3D::Particles::SpawnFromPreset(v31, v76, v32, 55, 19);
						v33->pos.y = -16384;
						v33->groundHeightY = Nu3D::Collision::GetGroundHeight((Transform*)v33, 0);
						v33->pos.y = v76;
						v33->rotSpeed = *g_randDatBufferPtr - 128;
						++g_randDatBufferPtr;

						if (v33->groundHeightY == 0x80000000)
							v33->groundHeightY = 5216;

						g_unk52F678 = 0;
					}
				}

				++g_unk52F6E4;

				if (g_unk52F6E4 >= Levels::g_recordData[0]->recordCount)
					g_unk52F6E4 = 0;
			}

			g_unk52F658 -= (int32_t)Renderer::g_frameDelta;

			if (g_unk52F658 < 0)
				g_unk52F658 = 0;

			if (Buzz::g_unk52B81C == 11 && g_buzzActor.unkShort40)
			{
				if (! g_unk52F658 && ! g_unk52F690)
				{
					Levels::Method3(3, 1);
					AudioManager::PlaySoundEffect(56, &g_buzzActor.posAngles.pos);
					g_unk52F658 = 30;
					g_unk52F690 = 3072;
				}
			}
			else
			{
				g_unk52F690 = 0;
			}

			if (g_unk52F6B4 != 4096)
			{
				if (g_unk52F60C)
				{
					if (Buzz::g_groundSlamTicker < 0)
						g_unk52F6B4 += 48 * (int32_t)Renderer::g_frameDelta;
					else
						g_unk52F6B4 += 32 * (int32_t)Renderer::g_frameDelta;

					if (g_unk52F6B4 >= 4096)
					{
						g_unk52F6B4 = 4096;
						g_unk52F6BC = 850;
						g_unk52F6C0 = -1810;
						AudioManager::PlaySoundEffect(11, &g_unk52F648);
					}

					Nu3D::Link::SetScaleFromFixedOffsets(6, 4096, g_unk52F6B4, 4096);
				}
				else if (g_unk52F6B4 > 1024)
				{
					g_unk52F6B4 -= 10 * (int32_t)Renderer::g_frameDelta;

					if (g_unk52F6B4 <= 1024)
					{
						g_unk52F6B4 = 1024;
					}

					Nu3D::Link::SetScaleFromFixedOffsets(6, 4096, g_unk52F6B4, 4096);
				}
			}
			else
			{
				if (Buzz::g_groundSlamTicker == -40 && (Platform::GetFlags(7) & 3) == 2 && g_buzzActor.posAngles.pos.y < 17408)
				{
					g_unk52F6C0 = 1448;
				}

				Nu3D::Link::SetPositionRawAndCommit(6, g_unk52F648.x >> 5, g_unk52F648.y >> 5, g_unk52F648.z >> 5);

				if (g_unk52F6BC > 0)
				{
					g_unk52F6BC -= 4 * (int32_t)Renderer::g_frameDelta;

					if (g_unk52F6BC < 0)
					{
						g_unk52F6BC = 0;
					}
				}

				if (g_unk52F648.y < 17408)
				{
					g_unk52F6C0 += 48 * (int32_t)Renderer::g_frameDelta;

					if (g_unk52F6C0 > 1536)
						g_unk52F6C0 = 1536;
				}
				else
				{
					g_unk52F6C0 -= 48 * (int32_t)Renderer::g_frameDelta;
				}

				g_unk52F648.x += (int32_t)Renderer::g_frameDelta * g_unk52F6BC;
				int32_t v38 = g_unk52F648.y;
				g_unk52F648.y += (int32_t)Renderer::g_frameDelta * g_unk52F6C0;

				if (g_unk52F648.y >= 17408 && v38 < 17408)
				{
					Nu3D::Particles::SpawnFromPreset(g_unk52F648.x, 17408, g_unk52F648.z, 57, 2);

					if (! Buzz::g_groundSlamTicker)
					{
						if (g_unk52F6C0 > 543)
							g_unk52F6C0 = g_unk52F6C0 >> 1;
						else
							g_unk52F6C0 = 543;
					}

					AudioManager::PlaySoundEffect(57, &g_unk52F648);
				}

				Platform::GetOrigin(7, &transform.pos);

				Platform::SetVelocity(7, g_unk52F648.x - transform.pos.x, g_unk52F648.y - transform.pos.y - 4096, g_unk52F648.z - transform.pos.z);
			}

			if (g_unk52F674)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(7, 4096, 4096 - g_unk52F674, 4096);
			}

			g_unk52F60C = 0;

			if (g_unk52F674 != g_unk52F690)
			{
				if (g_unk52F674 - g_unk52F690 < -128)
					g_unk52F60C = 1;

				g_unk52F674 = (int32_t)Renderer::g_frameDelta * (g_unk52F674 - g_unk52F690) / -8 + g_unk52F674;
			}

			int32_t v42 = 0;

			if (g_unk52F6C8 == 4096 && v74 == 8)
			{
				g_unk52F6C8 = 4095;
				v42 = 56;

				Levels::Method3(0, 1);
				Nu3D::Link::SetScaleFromFixedOffsets(32, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(34, 0, 0, 0);
			}

			if (g_unk52F6CC == 4096 && v74 == 9)
			{
				g_unk52F6CC = 4095;
				v42 = 56;

				Levels::Method3(1, 1);
				Nu3D::Link::SetScaleFromFixedOffsets(33, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(35, 0, 0, 0);
			}

			int32_t v44 = 0;

			if (g_unk52F6C8 != 4096)
			{
				v44 = 1;

				if (g_unk52F6C8 > 0)
				{
					g_unk52F6C8 -= 64 * (int32_t)Renderer::g_frameDelta;

					if (g_unk52F6C8 < 0)
					{
						g_unk52F6C8 = 0;
					}

					Nu3D::Link::SetScaleFromFixedOffsets(2, 4096, g_unk52F6C8, 4096);
				}
			}

			if (g_unk52F6CC != 4096)
			{
				v44 += 2;

				if (g_unk52F6CC > 0)
				{
					g_unk52F6CC -= 64 * (int32_t)Renderer::g_frameDelta;

					if (g_unk52F6CC < 0)
					{
						g_unk52F6CC = 0;
					}

					Nu3D::Link::SetScaleFromFixedOffsets(3, 4096, g_unk52F6CC, 4096);
				}
			}

			if (v44)
			{
				if (v44 != 3)
				{
					int32_t v50 = (Numerics::g_sinCosLUT[g_unk52F6D8] >> 9) + 32;
					g_unk52F614 = v50;
					g_unk52F6D8 = (g_unk52F6D8 + 32 * (int32_t)Renderer::g_frameDelta) & 0xFFF;

					int32_t v51 = v50;

					if (v44 == 1)
						v51 = -v50;

					Nu3D::Link::SetRotationRelative8bit(4, v51, 0, v50);
					Nu3D::Link::SetRotationRelative8bit(8, v51, 0, g_unk52F614);
					Nu3D::Link::SetRotationRelative8bit(9, v51, 0, g_unk52F614);
					Nu3D::Link::SetRotationRelative8bit(10, v51, 0, g_unk52F614);

					g_unk52F618 = v51;
				}
				else
				{
					if (! g_unk52F610)
					{
						g_unk52F610 = 1;
						Collision::MarkPlatformAsMoving(5);
						Platform::DisableCollision(6);
						Method1();
					}

					int32_t v55 = 0;

					if (g_unk52F614 < 128)
					{
						g_unk52F614 += 2 * (int32_t)Renderer::g_frameDelta;

						if (g_unk52F614 >= 128)
						{
							g_unk52F614 = 128;
						}

						v55 = 1;
					}

					if (g_unk52F618 > 0)
					{
						g_unk52F618 -= 2 * (int32_t)Renderer::g_frameDelta;

						if (g_unk52F618 <= 0)
						{
							g_unk52F618 = 0;
						}

						v55 = 1;
					}

					if (g_unk52F618 < 0)
					{
						g_unk52F618 += 2 * (int32_t)Renderer::g_frameDelta;

						if (g_unk52F618 >= 0)
						{
							g_unk52F618 = 0;
						}

						v55 = 1;
					}

					if (v55)
					{
						Nu3D::Link::SetRotationRelative8bit(4, g_unk52F618, 0, g_unk52F614);
						Nu3D::Link::SetRotationRelative8bit(8, g_unk52F618, 0, g_unk52F614);
						Nu3D::Link::SetRotationRelative8bit(9, g_unk52F618, 0, g_unk52F614);
						Nu3D::Link::SetRotationRelative8bit(10, g_unk52F618, 0, g_unk52F614);
					}
					else if (g_unk52F610 == 1)
					{
						g_unk52F610 = 2;
						Nu3D::Link::SetScaleFromFixedOffsets(21, 4096, 4096, 4096);
						Nu3D::Link::SetScaleFromFixedOffsets(4, 0, 0, 0);

						v42 = 59;
					}
				}
			}

			if (v42)
			{
				transform.pos.x = 365184;
				transform.pos.y = 4096;
				transform.pos.z = -377127;

				AudioManager::PlaySoundEffect(v42, &transform.pos);
			}

			if (v74 == 10 && ! g_unk52F66C && g_unk52F610)
			{
				g_unk52F66C = 2;
			}

			if (g_unk52F66C)
			{
				if (g_unk52F66C > 0)
				{
					g_unk52F66C += (int32_t)Renderer::g_frameDelta;

					if (g_unk52F66C > 32)
					{
						g_unk52F66C = -32;
					}
					else if (g_unk52F66C > 6 && g_unk52F66C - (int32_t)Renderer::g_frameDelta <= 6)
					{
						Buzz::g_groundSlamTicker = 0;
						Levels::Method3(2, 1);
						AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
						Levels::Method2(-4224, 2);

						g_buzzActor.actorFlags |= 0x44;
						g_buzzActor.velX = Numerics::g_sinCosLUT[0];
						g_buzzActor.velForward = Numerics::g_sinCosLUT[1024];
						g_buzzActor.posAngles.angles.yaw = 0;
						g_buzzActor.facingAngle = 0;
						Camera::g_unk52AD98 = 64;
					}
				}
				else
				{
					g_unk52F66C += (int32_t)Renderer::g_frameDelta;

					if (g_unk52F66C > 0)
					{
						g_unk52F66C = 0;
					}
				}

				Nu3D::Link::SetRotationRelative8bit(21, (-32 * abs(g_unk52F66C)) & 0xFFF, 0, 0);
			}

			if (g_unk52F5E4 != 0x80000000)
			{
				if (g_unk52F5F0 < 665)
					g_unk52F5E4 += (int32_t)Renderer::g_frameDelta;
				else
					g_unk52F5E4 = Buzz::g_curMoveableObject != 1 ? 0 : 8;

				int32_t v60 = g_unk52F5F0;
				g_unk52F5F0 += (int32_t)Renderer::g_frameDelta * g_unk52F5E4 / -2;

				if (v60 >= 665 && g_unk52F5F0 < 665)
				{
					Platform::DisableCollision(2);
					Platform::DisableCollision(3);
					Collision::MarkPlatformAsMoving(4);

					Buzz::g_curMoveableObject = 0;
				}

				if (g_unk52F5F0 <= 0)
				{
					g_unk52F5F0 = 0;
					g_unk52F5E4 = -(g_unk52F5E4 >> 2);

					if (abs(g_unk52F5E4) < 4)
					{
						g_unk52F5E4 = 0x80000000;
					}
					else
					{
						Nu3D::Link::GetCurrentPosFixed(5, &transform.pos);
						AudioManager::PlaySoundEffect(52, &transform.pos);
					}
				}

				Nu3D::Link::SetRotationRelative8bit(5, 0, 0, g_unk52F5F0);
				Nu3D::Link::SetRotationRelative8bit(11, 0, 0, g_unk52F5F0);
			}

			if (! g_unk52F5EC)
			{
				if (g_buzzActor.posAngles.pos.y < -66297)
				{
					if (MoveableObject::g_moveableObjects[1].segProgress)
					{
						g_unk52F5EC = 1;
						Platform::DisableCollision(11);
						Collision::MarkPlatformAsMoving(12);

						g_buzzActor.velX = 0;
						g_buzzActor.velForward = 0;
						Buzz::g_curMoveableObject = 0;
					}
				}
				else
				{
					MoveableObject::g_moveableObjects[1].segProgress = 0;
				}
			}
			else
			{
				if (Buzz::g_curMoveableObject == 2)
					Buzz::g_curMoveableObject = 0;

				if (g_unk52F5EC < 581)
				{
					g_unk52F5EC += (int32_t)Renderer::g_frameDelta * g_unk52F640;

					if (g_unk52F5EC >= 581)
					{
						g_unk52F5EC = 581;
						Nu3D::Link::GetCurrentPosFixed(30, &transform.pos);
						AudioManager::PlaySoundEffect(52, &transform.pos);
					}

					Nu3D::Link::SetRotationRelative8bit(30, 0, 0, 4095 - g_unk52F5EC);
					Nu3D::Link::SetRotationRelative8bit(31, 0, 0, 4095 - g_unk52F5EC);
					g_unk52F640 += (int32_t)Renderer::g_frameDelta;
				}
			}

			if (! g_unk52F664)
			{
				if (g_buzzActor.collisionFlags && g_buzzActor.posAngles.pos.y < -466944 && g_buzzActor.posAngles.pos.x > 134342
					&& g_buzzActor.posAngles.pos.x < 221126 && g_buzzActor.posAngles.pos.z > 184692 && g_buzzActor.posAngles.pos.z < 313332)
				{
					g_unk52F664 = 1; // Entered cutscene
					Dialogue::Begin(26, 9, "ha ha ha ha ... defeat the ^zurg kite^ to get a ^token^!", -1, 0, -1);
				}
			}

			if (g_unk52F664 == 1)
			{
				if (! Renderer::g_enableCinematicBars)
				{
					g_unk52F664 = 2; // Started fight
					g_levelActors[26].unkShort1 = 0;
					g_levelActors[26].movementData = Actor::g_movementData[13] + 14;
					g_unk830D44 = 180;
				}
			}

			if (g_unk52F664 == 2)
			{
				if (! g_levelActors[26].creatureId)
				{
					g_unk52F664 = 3;
					g_unk52B7E0 = 0;
				}
				else
				{
					g_unk52B7E0 = 54 * g_levelActors[26].actorPhase / 10;
				}
			}

			if (g_unk52F664 >= 3)
			{
				if (g_unk52F664 < 120)
				{
					g_unk52F664 += (int32_t)Renderer::g_frameDelta;
				}
				else if (g_unk52F664 != 200)
				{
					Collectables::Activate(4, 0);
					g_unk52F664 = 200;
				}
			}

			if (g_unk52F664 < 3)
			{
				transform.pos.x = 0;
				transform.pos.y = 384;
				transform.pos.z = 0;

				Actor::ResolveBoneAttachmentPos(&transform.pos, &g_levelActors[26], 0);
				g_unk52F628 = transform.pos;
			}
			else if (g_unk52F664 != 200)
			{
				int32_t v65 = (g_zKiteYawPhase + ((int32_t)Renderer::g_frameDelta << 6)) & 0xFFF;
				g_zKiteYawPhase = v65;
				int32_t v67 = Numerics::g_sinCosLUT[v65] >> 5;

				int32_t v70 = (g_buzzActor.posAngles.pos.x - g_unk52F628.x) >> 8;
				int32_t v71 = (g_buzzActor.posAngles.pos.y - g_unk52F628.y) >> 8;
				int32_t v77 = (g_buzzActor.posAngles.pos.z - g_unk52F628.z) >> 8;

				g_unk52F628.x += v67;
				g_unk52F628.y += 192;
				g_unk52F628.z += v67;

				if (g_tickState[4] && v70 * v70 + v77 * v77 + v71 * v71 < 0x40000)
					Nu3D::Particles::SpawnFromPreset(g_unk52F628.x, g_unk52F628.y, g_unk52F628.z, 58, 3);
			}

			if (g_unk52F664 == 200 || ! Nu3D::Math::IsWithinDistance(&Camera::g_activeCameraTransform.pos, &g_unk52F628, 800))
			{
				KiteTail::Deactivate(0);
			}
			else
			{
				KiteTail::Simulate(&g_unk52F628, 0);
				KiteTail::Activate(0);
			}

			g_levelActors[0].visibilityDistance = Actor::IsInsideBounds((Actor::Toy2Actor*)&g_buzzActor, -511196, -161372, -481680, -656) ? 3800 : 1800;

			if (g_curSectorCopy1 == 1)
			{
				if (Buzz::g_currentFluidType == 1)
				{
					if (Camera::g_activeCameraTransform.pos.y > Buzz::g_underwaterYLevel)
						Nu3D::Link::SetScaleFromFixedOffsets(20, 0, 0, 0);
					else
						Nu3D::Link::SetScaleFromFixedOffsets(20, 4096, 4096, 4096);
				}

				Renderer::BlitTextureByIndexOffset(5, 128, 192, 64, 64, 0, ((Numerics::g_sinCosLUT[g_unk52F6C4] & 0xFFFF) >> 9) & 0x3F, 64, 0);
				g_unk52F6C4 = (g_unk52F6C4 + 8 * (int32_t)Renderer::g_frameDelta) & 0xFFF;
			}

			Vector3I* data = Levels::g_recordData[61]->data;

			if (! Buzz::g_unk53C5E4 && ! g_unk52F6A8 && Buzz::g_unk53C668)
			{
				g_unk52F6A8 = 2;
				AudioManager::PlaySoundEffect(33, &g_buzzActor.posAngles.pos);
				data[1].x = 2;
			}

			if (g_unk52F6A8 > 0)
			{
				g_unk52F680.y += (int32_t)Renderer::g_frameDelta * g_unk52F6A8;
				data[1].y += (int32_t)Renderer::g_frameDelta * g_unk52F6A8;

				g_unk52F6A8 += ((int32_t)Renderer::g_frameDelta << 6) / 4;

				if (g_unk52F6A8 > 2048)
					g_unk52F6A8 = 2048;

				if (g_unk52F680.y > -9472)
				{
					g_unk52F680.y = -9472;
					data[1].y -= g_unk52F680.y + 9472; // Quirk of the original: always subtracts 0, the y was just set to -9472.
					data[1].x = 0;
					g_unk52F6A8 = -1;
				}

				data->y = g_unk52F680.y;
				Nu3D::Link::SetPositionRawAndCommit(25, g_unk52F680.x >> 5, g_unk52F680.y >> 5, g_unk52F680.z >> 5);
				int32_t v73 = ((g_unk52F680.y + 361068) << 8) / 21970;

				if (v73 > 4096)
					v73 = 4096;
				else if (v73 < 0)
					v73 = 0;

				Nu3D::Link::SetScaleFromFixedOffsets(27, 4096, v73, 4096);
			}

			PlayLevelMusic();
		}

	}
}

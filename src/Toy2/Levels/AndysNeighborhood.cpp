#include "Toy2/Levels/AndysNeighborhood.h"
#include "Toy2/Toy2.h"
#include "Toy2/KiteTail.h"
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
	namespace AndysNeighborhood
	{
		// GLOBAL: TOY2 0x004F1508
		int16_t g_unk4F1508[8] = { 0, 1, 3, 2, 3, 1, 0, 2 };

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

		// GLOBAL: TOY2 0x0052F2F8
		int32_t g_unk52F2F8;

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

		// GLOBAL: TOY2 0x0052F60C
		int16_t g_unk52F60C[2];

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
		int16_t g_unk52F674[2];

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

		// FUNCTION: TOY2 0x004190C0
		void Interactions()
		{
			int32_t v0;
			int32_t v1;
			int32_t v2;
			int32_t v3;
			bool v4;
			int32_t v5;
			int32_t v6;
			int32_t v7;
			int32_t* p_z;
			Levels::RecordData* v9;
			int32_t v10;
			int32_t v11;
			int32_t v12;
			int32_t v13;
			int32_t z;
			int32_t v16;
			int32_t v17;
			int32_t v19;
			int32_t v20;
			int32_t v21;
			int32_t x;
			int32_t v23;
			int32_t v24;
			int16_t v25;
			int32_t v26;
			uint8_t* v27;
			char v28;
			int32_t v29;
			int32_t v30;
			int32_t v31;
			int32_t v32;
			Nu3D::Particles::ParticleInstance* v33;
			int32_t v34;
			int32_t v35;
			int32_t v36;
			int32_t v37;
			int32_t v38;
			int32_t v39;
			int32_t v40;
			int32_t v41;
			int32_t v42;
			int32_t v43;
			int32_t v44;
			int32_t v45;
			bool v46;
			int32_t v47;
			int32_t v48;
			int32_t v49;
			int32_t v50;
			int32_t v51;
			int32_t v52;
			bool v53;
			int32_t v54;
			int32_t v55;
			int32_t v56;
			bool v57;
			int32_t v58;
			int32_t v59;
			int32_t v60;
			int32_t v61;
			int32_t v62;
			int32_t v63;
			int32_t v64;
			int32_t v65;
			int32_t v66;
			int32_t v67;
			int32_t v68;
			int32_t v69;
			int32_t v70;
			int32_t v71;
			Vector3I* data;
			int32_t v73;
			int32_t v74;
			int32_t v75;
			int32_t v76;
			int32_t v77;
			Vector3I a2;

			g_unk52F6DC = Platform::StepTiltPhysics(0, 0, g_unk52F6DC, 0, -448, 448, 8);
			g_unk52F6E0 = Platform::StepTiltPhysics(1, 1, g_unk52F6E0, 0, -448, 224, 8);

			if (! g_buzzActor.collisionFlags || ! Buzz::g_groundSlamTicker || Buzz::g_unk52B81C < 8 || (v74 = Buzz::g_unk52B81C, Buzz::g_unk52B81C > 11))
			{
				v74 = 0;
			}

			if (g_buzzActor.posAngles.pos.x <= -149248)
			{
				Buzz::g_underwaterYLevel = 17408;
				Buzz::g_currentFluidType = 1;
			}
			else
			{
				Buzz::g_underwaterYLevel = 6144;
				Buzz::g_currentFluidType = 2;
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

			v0 = g_unk52F2F8;

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

					v1 = 0;
					v0 = 1;
					v2 = 0;

					g_unk52F2F8 = 1;
					g_unk52F6B0 = 0;
					g_unk52F668 = 0;
					g_unk52F620 = 0;
					g_unk52AD64 = 0;
					g_unk52F660 = 0;
					g_unk52F5E0 = 1;

					g_levelActors[29].actorPhase = 202;
				LABEL_20:

					if (Renderer::g_enableCinematicBars)
						goto LABEL_54;

					*(uint8_t*)&g_levelActors[29].actorFlags |= 4u;
					g_unk52F2F8 = 2;

					goto LABEL_25;
				}
			}

			if (g_unk52F2F8 == 1)
			{
				v1 = g_unk52F6B0;
				v2 = g_unk52F668;
				goto LABEL_20;
			}

			if (g_unk52F2F8 < 2)
				goto LABEL_54;

			if (g_unk52F2F8 == 2)
			{
				v1 = g_unk52F6B0;
				v2 = g_unk52F668;
			LABEL_25:

				if (v1 >= 4)
				{
					v4 = v1 == 8;

					if (v1 < 8)
					{
						v5 = g_buzzActor.posAngles.pos.x > 47554;

						if (g_buzzActor.posAngles.pos.z > -270441)
							v5 += 2;

						if (v5 == g_unk4F1508[v1])
							g_unk52F6B0 = ++v1;

						v4 = v1 == 8;
					}

					if (v4 && g_buzzActor.posAngles.pos.z > -148073)
					{
						g_unk52F6B0 = 0;
						++g_unk52AD64;
					}
				}
				else
				{
					v3 = g_buzzActor.posAngles.pos.x > 152386;

					if (g_buzzActor.posAngles.pos.z > 248855)
						v3 += 2;

					if (v3 == g_unk4F1508[v1])
						g_unk52F6B0 = v1 + 1;
				}
				goto LABEL_40;
			}

			v2 = g_unk52F668;
		LABEL_40:

			if (g_levelActors[29].pos.y > 0x2000)
			{
				g_levelActors[29].pos.y = 0x2000;
				v6 = 32 * (*g_randDatBufferPtr - 128);

				g_randDatBufferPtr += 2;
				Nu3D::Particles::SpawnFromPreset(g_levelActors[29].pos.x + v6, 0x2000, v6 + g_levelActors[29].pos.z, 53, 4);
				v2 = g_unk52F668;
			}

			v7 = Levels::g_recordData[1]->data[v2].x - (g_levelActors[29].pos.x >> 5);
			p_z = &Levels::g_recordData[1]->data[v2].z;

			if (v7 * v7 + (*p_z - (g_levelActors[29].pos.z >> 5)) * (*p_z - (g_levelActors[29].pos.z >> 5)) < 360000)
			{
				g_levelActors[29].boundary.x = 32 * Levels::g_recordData[1]->data[v2].x;
				g_levelActors[29].boundary.z = 32 * *p_z;

				if (g_unk52F620 >= 3)
				{
					if (g_levelActors[29].pos.z > -148073)
					{
						if (g_unk52F2F8 == 2 && g_unk52AD64 < 3)
							g_unk52F2F8 = 3;

						g_levelActors[29].actorPhase = 1;
					}
				}
				else
				{
					g_unk52F668 = ++v2;
					if (v2 > 64)
					{
						v2 = 0;
						g_unk52F668 = 0;
						++g_unk52F620;
					}
				}
			}

			g_levelActors[29].motionTargetPos.x = 32 * Levels::g_recordData[1]->data[v2].x;
			g_levelActors[29].motionTargetPos.z = 32 * Levels::g_recordData[1]->data[v2].z;

			if (g_unk52AD64 == 3)
			{
				Collectables::Activate(2, 0);
				v0 = 3;
				g_unk52F2F8 = 3;
			}
			else
			{
				v0 = g_unk52F2F8;
			}
		LABEL_54:

			if ((g_buzzActor.posAngles.pos.x < -159744 || g_buzzActor.posAngles.pos.x > 162225 && g_buzzActor.posAngles.pos.z < 0
				 || g_buzzActor.posAngles.pos.y < -99029)
				&& v0 && (g_levelActors[29].actorFlags & 1) == 0 && ! Collectables::g_pickupSlots[2].state)
			{
				g_unk52F2F8 = 0;
				Game::InitActor(&g_levelActors[29], 0);
			}

			if ((g_levelActors[4].actorFlags & 2) != 0 && (g_levelActors[4].actorFlags & 0x20) != 0
				&& ((g_buzzActor.posAngles.pos.x - g_levelActors[4].pos.x) >> 5) * ((g_buzzActor.posAngles.pos.x - g_levelActors[4].pos.x) >> 5)
						+ ((g_buzzActor.posAngles.pos.z - g_levelActors[4].pos.z) >> 5) * ((g_buzzActor.posAngles.pos.z - g_levelActors[4].pos.z) >> 5)
						+ ((g_buzzActor.posAngles.pos.y - g_levelActors[4].pos.y) >> 5) * ((g_buzzActor.posAngles.pos.y - g_levelActors[4].pos.y) >> 5)
					< 589824
				&& g_levelActors[4].motionTargetPos.y != 20480)
			{
				g_levelActors[4].motionTargetPos.y = 20480;
				g_unk52F61C = 100;
				g_unk52F5E8 = g_unk52F5DC;
				AudioManager::PlaySoundEffect(49, &g_levelActors[4].pos);
			}

			v9 = Levels::g_recordData[2];
			v10 = g_unk52F5DC;
			v11 = g_unk52F5E8;

			if (g_levelActors[4].pos.y > 18432)
			{
				v10 = g_unk52F5DC + 1;
				((uint16_t*)&v12)[1] = 0;
				g_unk52F5DC = v10;

				if (v10 >= Levels::g_recordData[2]->recordCount)
				{
					*(uint16_t*)&v12 = Levels::g_recordData[2]->recordCount;
					v10 -= v12;
					g_unk52F5DC = v10;
				}

				if (Levels::g_recordData[2]->data[v10].y == 0x80000000 || v10 == g_unk52F5E8)
				{
					v13 = 0;

					if (Levels::g_recordData[2]->recordCount)
					{
						do
						{
							g_unk52F5DC = ++v10;

							if (v10 >= Levels::g_recordData[2]->recordCount)
							{
								v10 = 0;
								g_unk52F5DC = 0;
							}

							if (Levels::g_recordData[2]->data[v10].y != 0x80000000 && v10 != g_unk52F5E8)
								break;

							++v13;
						} while (v13 < Levels::g_recordData[2]->recordCount);
					}
				}

				g_levelActors[4].motionTargetPos.x = 32 * Levels::g_recordData[2]->data[v10].x;
				g_levelActors[4].motionTargetPos.y = 32 * Levels::g_recordData[2]->data[v10].y;

				z = Levels::g_recordData[2]->data[v10].z;

				g_levelActors[4].boundary.x = g_levelActors[4].motionTargetPos.x;
				g_levelActors[4].motionTargetPos.z = 32 * z;
				g_levelActors[4].boundary.y = g_levelActors[4].motionTargetPos.y;
				g_levelActors[4].boundary.z = 32 * z;
				g_levelActors[4].pos.x = g_levelActors[4].motionTargetPos.x;
				g_levelActors[4].pos.y = 14336;
				g_levelActors[4].pos.z = 32 * z;
				g_levelActors[4].unkVar9 = 416;
			}

			v16 = (int32_t)Renderer::g_frameDelta;
			v75 = 0;

			if ((int32_t)Renderer::g_frameDelta > 0)
			{
				v17 = g_unk52F6D4;

				do
				{
					if (v9->data[v17].y == 0x80000000)
					{
						v19 = 32 * v9->data[v17].z;
						v20 = 32 * v9->data[v17].x;

						if (((Camera::g_activeCameraTransform.pos.x - v20) >> 8) * ((Camera::g_activeCameraTransform.pos.x - v20) >> 8)
								+ ((Camera::g_activeCameraTransform.pos.z - v19) >> 8) * ((Camera::g_activeCameraTransform.pos.z - v19) >> 8)
								+ ((Camera::g_activeCameraTransform.pos.y + 2992) >> 8) * ((Camera::g_activeCameraTransform.pos.y + 2992) >> 8)
							>= 0x40000)
						{
							v11 = g_unk52F5E8;
							v9 = Levels::g_recordData[2];
							v10 = g_unk52F5DC;
						}
						else
						{
							Nu3D::Particles::SpawnFromPreset(v20, -2992, v19, 58, 3);
							v9 = Levels::g_recordData[2];
							v11 = g_unk52F5E8;
							v10 = g_unk52F5DC;
							v16 = (int32_t)Renderer::g_frameDelta;
						}

						v17 = g_unk52F6D4;
					}

					g_unk52F6D4 = ++v17;

					if (v17 >= v9->recordCount)
					{
						v17 = 0;
						g_unk52F6D4 = 0;
					}

					++v75;
				} while (v75 < v16);
			}

			v21 = g_unk52F61C;

			if (g_unk52F61C > 0)
			{
				v21 = g_unk52F61C - v16;
				g_unk52F61C = v21;

				if (v21 <= 0)
				{
					g_unk52F61C = 0;
					AudioManager::ClearSequence7Cursor();
					v9 = Levels::g_recordData[2];
					v21 = g_unk52F61C;
					v11 = g_unk52F5E8;
					v10 = g_unk52F5DC;
					v16 = (int32_t)Renderer::g_frameDelta;
				}

				if (v21 < 80 && v21 + v16 >= 80)
				{
					x = v9->data[v11].x;
					g_unk52F5F8.y = -9136;
					g_unk52F5F8.x = 32 * x;
					g_unk52F5F8.z = 32 * v9->data[v11].z;

					Nu3D::Particles::SpawnFromPreset(32 * x, -9136, g_unk52F5F8.z, 59, 2);
					AudioManager::StartSoundSequenceOnActor(-3, (Actor::Toy2Actor*)&g_unk52F5F8);

					v9 = Levels::g_recordData[2];
					v21 = g_unk52F61C;
					v11 = g_unk52F5E8;
					v10 = g_unk52F5DC;
					v16 = (int32_t)Renderer::g_frameDelta;
				}
			}

			if (g_buzzActor.collisionFlags)
			{
				if (Buzz::g_groundSlamTicker < -20 && v10 != v11)
				{
					if (v21)
					{
						v23 = g_buzzActor.posAngles.pos.x - 32 * v9->data[v11].x;
						v24 = g_buzzActor.posAngles.pos.z - 32 * v9->data[v11].z;

						if ((v23 >> 8) * (v23 >> 8) + (v24 >> 8) * (v24 >> 8)
								+ ((g_buzzActor.posAngles.pos.y + 2992) >> 8) * ((g_buzzActor.posAngles.pos.y + 2992) >> 8)
							< 2304)
						{
							AudioManager::ClearSequence7Cursor();
							Levels::g_recordData[2]->data[g_unk52F5E8].y = 0x80000000;
							g_unk52F61C = 0;

							if (++g_unk52F6D0 >= 6)
							{
								((uint8_t*)&v25)[1] = (uint8_t)((g_levelActors[4].actorFlags) >> 8);
								*(uint8_t*)&v25 = g_levelActors[4].actorFlags & 0x5B | 0x84;
								g_levelActors[4].actorFlags = v25;
							}

							v26 = 0;
							do
							{
								if (Nu3D::Particles::g_particleInstances[v26].typeId == 59 && Nu3D::Particles::g_particleInstances[v26].lifetime > 0)
									Nu3D::Particles::g_particleInstances[v26].lifetime = 1;
								++v26;
							} while (v26 < 64);

							v16 = (int32_t)Renderer::g_frameDelta;
						}
					}
				}
			}

			v27 = g_randDatBufferPtr;

			if (! g_tickState[g_buzzActor.posAngles.pos.y >= -48640 ? 4 : 9]
				|| (v28 = *g_randDatBufferPtr & 7, v27 = g_randDatBufferPtr + 1, ++g_randDatBufferPtr, v28))
			{
				if (! g_unk52F678)
					goto LABEL_120;
			}
			else
			{
				g_unk52F678 = 1;
			}

			v29 = g_unk52F6E4;
			v30 = g_buzzActor.posAngles.pos.y - 32 * Levels::g_recordData[0]->data[g_unk52F6E4].y;
			v76 = 32 * Levels::g_recordData[0]->data[g_unk52F6E4].y;

			if (v30 > 28672 && v30 < 327680)
			{
				v31 = 32 * (Levels::g_recordData[0]->data[g_unk52F6E4].x + 4 * *v27 - 512);
				g_randDatBufferPtr = v27 + 1;

				v32 = 32 * (Levels::g_recordData[0]->data[g_unk52F6E4].z + 4 * v27[1] - 512);
				g_randDatBufferPtr = v27 + 2;

				if (((g_buzzActor.posAngles.pos.x - v31) >> 8) * ((g_buzzActor.posAngles.pos.x - v31) >> 8)
						+ ((g_buzzActor.posAngles.pos.z - v32) >> 8) * ((g_buzzActor.posAngles.pos.z - v32) >> 8)
					< 640000)
				{
					v33 = Nu3D::Particles::SpawnFromPreset(v31, v76, v32, 55, 19);
					v33->pos.y = -16384;
					v33->groundHeightY = Nu3D::Collision::GetGroundHeight((WorldPos*)v33, 0);
					v33->pos.y = v76;
					v33->rotSpeed = *g_randDatBufferPtr++ - 128;

					if (v33->groundHeightY == 0x80000000)
						v33->groundHeightY = 5216;

					v16 = (int32_t)Renderer::g_frameDelta;
					v29 = g_unk52F6E4;
					g_unk52F678 = 0;
				}
			}

			g_unk52F6E4 = v29 + 1;

			if (v29 + 1 >= Levels::g_recordData[0]->recordCount)
				g_unk52F6E4 = 0;
		LABEL_120:

			g_unk52F658 -= v16;

			if (g_unk52F658 < 0)
				g_unk52F658 = 0;

			if (Buzz::g_unk52B81C == 11 && g_buzzActor.unkShort40)
			{
				if (! g_unk52F658 && ! g_unk52F690)
				{
					Levels::Method3(3, 1);
					AudioManager::PlaySoundEffect(56, &g_buzzActor.posAngles.pos);
					v16 = (int32_t)Renderer::g_frameDelta;
					g_unk52F658 = 30;
					g_unk52F690 = 3072;
				}
			}
			else
			{
				g_unk52F690 = 0;
			}

			if (g_unk52F6B4 == 4096)
			{
				if (Buzz::g_groundSlamTicker == -40 && (Platform::GetFlags(7) & 3) == 2 && g_buzzActor.posAngles.pos.y < 17408)
				{
					g_unk52F6C0 = 1448;
				}

				Nu3D::Link::SetPositionRawAndCommit(6, g_unk52F648.x >> 5, g_unk52F648.y >> 5, g_unk52F648.z >> 5);
				v35 = g_unk52F6BC;

				if (g_unk52F6BC > 0)
				{
					g_unk52F6BC -= 4 * (int32_t)Renderer::g_frameDelta;
					v35 = g_unk52F6BC;

					if (g_unk52F6BC < 0)
					{
						v35 = 0;
						g_unk52F6BC = 0;
					}
				}
				if (g_unk52F648.y >= 17408)
				{
					v36 = -48 * (int32_t)Renderer::g_frameDelta + g_unk52F6C0;
				}
				else
				{
					g_unk52F6C0 += 48 * (int32_t)Renderer::g_frameDelta;
					v36 = g_unk52F6C0;

					if (g_unk52F6C0 <= 1536)
						goto LABEL_151;

					v36 = 1536;
				}

				g_unk52F6C0 = v36;
			LABEL_151:
				v37 = (int32_t)Renderer::g_frameDelta * v35 + g_unk52F648.x;
				v38 = g_unk52F648.y;

				g_unk52F648.x = v37;
				g_unk52F648.y += (int32_t)Renderer::g_frameDelta * v36;

				if (g_unk52F648.y >= 17408 && v38 < 17408)
				{
					Nu3D::Particles::SpawnFromPreset(v37, 17408, g_unk52F648.z, 57, 2);

					if (! Buzz::g_groundSlamTicker)
					{
						v39 = 543;

						if (g_unk52F6C0 > 543)
							v39 = g_unk52F6C0 >> 1;

						g_unk52F6C0 = v39;
					}

					AudioManager::PlaySoundEffect(57, &g_unk52F648);
				}

				Platform::GetOrigin(7, &a2);

				Platform::SetVelocity(7, g_unk52F648.x - a2.x, g_unk52F648.y - a2.y - 4096, g_unk52F648.z - a2.z);
				goto LABEL_159;
			}

			if (*g_unk52F60C)
			{
				if (Buzz::g_groundSlamTicker >= 0)
					v34 = 32 * v16 + g_unk52F6B4;
				else
					v34 = 48 * v16 + g_unk52F6B4;

				g_unk52F6B4 = v34;

				if (v34 >= 4096)
				{
					g_unk52F6B4 = 4096;
					g_unk52F6BC = 850;
					g_unk52F6C0 = -1810;
					AudioManager::PlaySoundEffect(11, &g_unk52F648);
					v34 = g_unk52F6B4;
				}
			LABEL_138:

				Nu3D::Link::SetScaleFromFixedOffsets(6, 4096, v34, 4096);
			LABEL_159:

				v16 = (int32_t)Renderer::g_frameDelta;
				goto LABEL_160;
			}

			if (g_unk52F6B4 > 1024)
			{
				g_unk52F6B4 -= 10 * v16;
				v34 = g_unk52F6B4;

				if (g_unk52F6B4 <= 1024)
				{
					v34 = 1024;
					g_unk52F6B4 = 1024;
				}
				goto LABEL_138;
			}
		LABEL_160:

			v40 = *g_unk52F674;
			if (*g_unk52F674)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(7, 4096, 4096 - *g_unk52F674, 4096);
				v16 = (int32_t)Renderer::g_frameDelta;
				v40 = *g_unk52F674;
			}

			*g_unk52F60C = 0;

			if (v40 != g_unk52F690)
			{
				*g_unk52F60C = v40 - g_unk52F690 < -128;
				*g_unk52F674 = v16 * (v40 - g_unk52F690) / -8 + v40;
			}

			v41 = g_unk52F6C8;
			v42 = 0;

			if (g_unk52F6C8 == 4096 && v74 == 8)
			{
				g_unk52F6C8 = 4095;
				v42 = 56;

				Levels::Method3(0, 1);
				Nu3D::Link::SetScaleFromFixedOffsets(32, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(34, 0, 0, 0);

				v16 = (int32_t)Renderer::g_frameDelta;
				v41 = g_unk52F6C8;
			}

			v43 = g_unk52F6CC;
			if (g_unk52F6CC == 4096 && v74 == 9)
			{
				g_unk52F6CC = 4095;
				v42 = 56;

				Levels::Method3(1, 1);
				Nu3D::Link::SetScaleFromFixedOffsets(33, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(35, 0, 0, 0);

				v16 = (int32_t)Renderer::g_frameDelta;
				v41 = g_unk52F6C8;
				v43 = g_unk52F6CC;
			}

			v44 = 0;

			if (v41 != 4096)
			{
				v44 = 1;

				if (v41 > 0)
				{
					v45 = -64 * v16;
					v46 = v45 + v41 < 0;
					v47 = v45 + v41;

					g_unk52F6C8 = v47;

					if (v46)
					{
						v47 = 0;
						g_unk52F6C8 = 0;
					}

					Nu3D::Link::SetScaleFromFixedOffsets(2, 4096, v47, 4096);
					v16 = (int32_t)Renderer::g_frameDelta;
					v43 = g_unk52F6CC;
				}
			}

			if (v43 != 4096)
			{
				v44 += 2;

				if (v43 > 0)
				{
					v48 = -64 * v16;
					v46 = v48 + v43 < 0;
					v49 = v48 + v43;

					g_unk52F6CC = v49;

					if (v46)
					{
						v49 = 0;
						g_unk52F6CC = 0;
					}

					Nu3D::Link::SetScaleFromFixedOffsets(3, 4096, v49, 4096);
					v16 = (int32_t)Renderer::g_frameDelta;
				}
			}

			if (! v44)
				goto LABEL_186;

			if (v44 != 3)
			{
				v50 = (Numerics::g_sinCosLUT[g_unk52F6D8] >> 9) + 32;
				v4 = v44 == 1;
				g_unk52F614 = v50;
				g_unk52F6D8 = (g_unk52F6D8 + 32 * v16) & 0xFFF;

				v51 = v50;

				if (v4)
					v51 = -v50;

				Nu3D::Link::SetRotationRelative8bit(4, v51, 0, v50);
				Nu3D::Link::SetRotationRelative8bit(8, v51, 0, g_unk52F614);
				Nu3D::Link::SetRotationRelative8bit(9, v51, 0, g_unk52F614);
				Nu3D::Link::SetRotationRelative8bit(10, v51, 0, g_unk52F614);

				g_unk52F618 = v51;
				goto LABEL_185;
			}

			if (! g_unk52F610)
			{
				g_unk52F610 = 1;
				Collision::MarkPlatformAsMoving(5);
				Platform::DisableCollision(6);
				Method1();
				v16 = (int32_t)Renderer::g_frameDelta;
			}

			v54 = g_unk52F614;
			v55 = 0;

			if (g_unk52F614 < 128)
			{
				g_unk52F614 += 2 * v16;
				v54 = g_unk52F614;

				if (g_unk52F614 >= 128)
				{
					v54 = 128;
					g_unk52F614 = 128;
				}

				v55 = 1;
			}

			v56 = g_unk52F618;
			v57 = g_unk52F618 < 0;

			if (g_unk52F618 > 0)
			{
				g_unk52F618 -= 2 * v16;
				v56 = g_unk52F618;

				if (g_unk52F618 <= 0)
				{
					v56 = 0;
					g_unk52F618 = 0;
				}

				v55 = 1;
				v57 = v56 < 0;
			}

			if (v57)
			{
				v56 += 2 * v16;
				g_unk52F618 = v56;

				if (v56 >= 0)
				{
					v56 = 0;
					g_unk52F618 = 0;
				}

				goto LABEL_206;
			}

			if (v55)
			{
			LABEL_206:
				Nu3D::Link::SetRotationRelative8bit(4, v56, 0, v54);
				Nu3D::Link::SetRotationRelative8bit(8, g_unk52F618, 0, g_unk52F614);
				Nu3D::Link::SetRotationRelative8bit(9, g_unk52F618, 0, g_unk52F614);
				Nu3D::Link::SetRotationRelative8bit(10, g_unk52F618, 0, g_unk52F614);
			LABEL_185:
				v16 = (int32_t)Renderer::g_frameDelta;
				goto LABEL_186;
			}

			if (g_unk52F610 != 1)
			{
			LABEL_186:
				if (! v42)
					goto LABEL_188;
				goto LABEL_187;
			}

			g_unk52F610 = 2;
			Nu3D::Link::SetScaleFromFixedOffsets(21, 4096, 4096, 4096);
			Nu3D::Link::SetScaleFromFixedOffsets(4, 0, 0, 0);

			v42 = 59;
		LABEL_187:
			a2.x = 365184;
			a2.y = 4096;
			a2.z = -377127;

			AudioManager::PlaySoundEffect(v42, &a2);
			v16 = (int32_t)Renderer::g_frameDelta;
		LABEL_188:
			v52 = g_unk52F66C;

			if (v74 == 10)
			{
				v53 = g_unk52F66C <= 0;

				if (! g_unk52F66C)
				{
					if (! g_unk52F610)
						goto LABEL_220;

					v52 = 2;

					goto LABEL_211;
				}
			}
			else
			{
				v53 = g_unk52F66C <= 0;

				if (! g_unk52F66C)
					goto LABEL_220;
			}

			if (! v53)
			{
			LABEL_211:
				v58 = v16 + v52;
				g_unk52F66C = v58;

				if (v58 <= 32)
				{
					if (v58 > 6 && v58 - v16 <= 6)
					{
						Buzz::g_groundSlamTicker = 0;
						Levels::Method3(2, 1);
						AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
						Levels::Method2(-4224, 2);

						*(uint8_t*)&g_buzzActor.actorFlags |= 0x44u;
						g_buzzActor.velForward = Numerics::g_sinCosLUT[1024];
						v58 = g_unk52F66C;

						g_buzzActor.velX = Numerics::g_sinCosLUT[0];
						g_buzzActor.posAngles.angles.yaw = 0;
						g_buzzActor.facingAngle = 0;
						Camera::g_unk52AD98 = 64;
					}
					goto LABEL_219;
				}
				v58 = -32;
				goto LABEL_218;
			}

			v58 = v16 + g_unk52F66C;
			g_unk52F66C = v58;

			if (v58 > 0)
			{
				v58 = 0;
			LABEL_218:
				g_unk52F66C = v58;
			}
		LABEL_219:

			Nu3D::Link::SetRotationRelative8bit(21, (-32 * abs(v58)) & 0xFFF, 0, 0);
			v16 = (int32_t)Renderer::g_frameDelta;
		LABEL_220:

			if (g_unk52F5E4 != 0x80000000)
			{
				if (g_unk52F5F0 >= 665)
					v59 = Buzz::g_curMoveableObject != 1 ? 0 : 8;
				else
					v59 = v16 + g_unk52F5E4;

				v60 = g_unk52F5F0;
				g_unk52F5E4 = v59;
				v61 = v16 * v59 / -2 + g_unk52F5F0;
				g_unk52F5F0 = v61;

				if (v60 >= 665 && v61 < 665)
				{
					Platform::DisableCollision(2);
					Platform::DisableCollision(3);
					Collision::MarkPlatformAsMoving(4);

					v59 = g_unk52F5E4;
					v61 = g_unk52F5F0;
					Buzz::g_curMoveableObject = 0;
				}

				if (v61 <= 0)
				{
					v61 = 0;
					g_unk52F5F0 = 0;
					g_unk52F5E4 = -(v59 >> 2);

					if (abs(g_unk52F5E4) >= 4)
					{
						Nu3D::Link::GetCurrentPosFixed(5, &a2);
						AudioManager::PlaySoundEffect(52, &a2);
						v61 = g_unk52F5F0;
					}
					else
					{
						g_unk52F5E4 = 0x80000000;
					}
				}

				Nu3D::Link::SetRotationRelative8bit(5, 0, 0, v61);
				Nu3D::Link::SetRotationRelative8bit(11, 0, 0, g_unk52F5F0);
				v16 = (int32_t)Renderer::g_frameDelta;
			}

			if (g_unk52F5EC)
			{
				if (Buzz::g_curMoveableObject == 2)
					Buzz::g_curMoveableObject = 0;

				if (g_unk52F5EC < 581)
				{
					g_unk52F5EC += v16 * g_unk52F640;
					v62 = g_unk52F5EC;

					if (g_unk52F5EC >= 581)
					{
						g_unk52F5EC = 581;
						Nu3D::Link::GetCurrentPosFixed(30, &a2);
						AudioManager::PlaySoundEffect(52, &a2);
						v62 = g_unk52F5EC;
					}

					Nu3D::Link::SetRotationRelative8bit(30, 0, 0, 4095 - v62);
					Nu3D::Link::SetRotationRelative8bit(31, 0, 0, 4095 - g_unk52F5EC);
					v16 = (int32_t)Renderer::g_frameDelta;
					g_unk52F640 += (int32_t)Renderer::g_frameDelta;
				}
			}
			else if (g_buzzActor.posAngles.pos.y >= -66297)
			{
				MoveableObject::g_moveableObjects[1].segProgress = 0;
			}
			else if (MoveableObject::g_moveableObjects[1].segProgress)
			{
				g_unk52F5EC = 1;
				Platform::DisableCollision(11);
				Collision::MarkPlatformAsMoving(12);

				v16 = (int32_t)Renderer::g_frameDelta;
				g_buzzActor.velX = 0;
				g_buzzActor.velForward = 0;
				Buzz::g_curMoveableObject = 0;
			}

			v63 = g_unk52F664;

			if (! g_unk52F664)
			{
				if (! g_buzzActor.collisionFlags || g_buzzActor.posAngles.pos.y >= -466944 || g_buzzActor.posAngles.pos.x <= 134342
					|| g_buzzActor.posAngles.pos.x >= 221126 || g_buzzActor.posAngles.pos.z <= 184692 || g_buzzActor.posAngles.pos.z >= 313332)
				{
					goto LABEL_262;
				}

				g_unk52F664 = 1;
				Dialogue::Begin(26, 9, "ha ha ha ha ... defeat the ^zurg kite^ to get a ^token^!", -1, 0, -1);
				v16 = (int32_t)Renderer::g_frameDelta;
				v63 = g_unk52F664;
			}

			if (v63 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_262;

				g_unk52F664 = 2;
				g_levelActors[26].unkShort1 = 0;
				g_levelActors[26].movementData = Actor::g_movementData[13] + 14;
				g_unk830D44 = 180;
			LABEL_255:

				if (! g_levelActors[26].creatureId)
				{
					v63 = 3;
					g_unk52B7E0 = 0;
				LABEL_260:
					v63 += v16;
					g_unk52F664 = v63;
					goto LABEL_261;
				}

				g_unk52B7E0 = 54 * g_levelActors[26].actorPhase / 10;
			LABEL_262:

				a2.x = 0;
				a2.y = 384;
				a2.z = 0;

				Actor::ResolveBoneAttachmentPos(&a2, &g_levelActors[26], 0);
				g_unk52F628 = a2;

				goto LABEL_274;
			}

			if (v63 == 2)
				goto LABEL_255;

			if (v63 < 3)
				goto LABEL_262;

			if (v63 < 120)
				goto LABEL_260;

			if (v63 != 200)
			{
				Collectables::Activate(4, 0);
				g_unk52F664 = 200;
				goto LABEL_265;
			}
		LABEL_261:

			if (v63 < 3)
				goto LABEL_262;

			if (v63 == 200)
			{
			LABEL_265:
				KiteTail::Deactivate(0);
				goto LABEL_266;
			}

			v77 = (g_buzzActor.posAngles.pos.z - g_unk52F628.z) >> 8;
			v65 = (g_zKiteYawPhase + (v16 << 6)) & 0xFFF;
			v66 = g_unk52F628.y + 192;
			v67 = Numerics::g_sinCosLUT[v65] >> 5;

			v68 = v67 + g_unk52F628.x;
			v69 = v67 + g_unk52F628.z;
			g_zKiteYawPhase = v65;

			v70 = (g_buzzActor.posAngles.pos.x - g_unk52F628.x) >> 8;
			v71 = (g_buzzActor.posAngles.pos.y - g_unk52F628.y) >> 8;

			g_unk52F628.x += v67;
			g_unk52F628.y += 192;
			g_unk52F628.z += v67;

			if (g_tickState[4] && v70 * v70 + v77 * v77 + v71 * v71 < 0x40000)
				Nu3D::Particles::SpawnFromPreset(v68, v66, v69, 58, 3);
		LABEL_274:

			if (g_unk52F664 == 200 || ! Nu3D::Math::IsWithinDistance(&Camera::g_activeCameraTransform.pos, &g_unk52F628, 800))
				goto LABEL_265;

			KiteTail::Simulate(&g_unk52F628, 0);
			KiteTail::Activate(0);
		LABEL_266:

			g_levelActors[0].visibilityDistance = Actor::IsInsideBounds((Actor::Toy2Actor*)&g_buzzActor, -511196, -161372, -481680, -656) ? 3800 : 1800;

			((uint16_t*)&v64)[1] = 0;
			if (g_curSectorCopy1 == 1)
			{
				if (Buzz::g_currentFluidType == 1)
				{
					if (Camera::g_activeCameraTransform.pos.y <= Buzz::g_underwaterYLevel)
						Nu3D::Link::SetScaleFromFixedOffsets(20, 4096, 4096, 4096);
					else
						Nu3D::Link::SetScaleFromFixedOffsets(20, 0, 0, 0);
				}

				*(uint16_t*)&v64 = Numerics::g_sinCosLUT[g_unk52F6C4];

				Renderer::BlitTextureByIndexOffset(5, 128, 192, 64, 64, 0, (v64 >> 9) & 0x3F, 64, 0);
				g_unk52F6C4 = (g_unk52F6C4 + 8 * (int32_t)Renderer::g_frameDelta) & 0xFFF;
			}

			data = Levels::g_recordData[61]->data;

			if (! Buzz::g_unk53C5E4 && ! g_unk52F6A8)
			{
				if (! Buzz::g_unk53C668)
					goto LABEL_293;

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
					data[1].y = data[1].y;
					data[1].x = 0;
					g_unk52F6A8 = -1;
				}

				data->y = g_unk52F680.y;
				Nu3D::Link::SetPositionRawAndCommit(25, g_unk52F680.x >> 5, g_unk52F680.y >> 5, g_unk52F680.z >> 5);
				v73 = ((g_unk52F680.y + 361068) << 8) / 21970;

				if (v73 <= 4096)
				{
					if (v73 < 0)
						v73 = 0;
				}
				else
				{
					v73 = 4096;
				}

				Nu3D::Link::SetScaleFromFixedOffsets(27, 4096, v73, 4096);
			}
		LABEL_293:

			PlayLevelMusic();
		}

		// FUNCTION: TOY2 0x00448080
		void Method1()
		{
			int32_t l_index;
			Levels::Type63* l_data;
			int32_t l_unk;
			WorldPos l_pos;

			l_index = 0;
			l_data = (Levels::Type63*)Levels::g_recordData[63]->data;

			if (Levels::g_recordData[63]->recordCount)
			{
				do
				{
					if (l_data->pos.y != 0x80000000 && l_data->props.linkId < 0x30u)
					{
						l_pos.x = 32 * l_data->pos.x;
						l_pos.z = 32 * l_data->pos.z;
						l_pos.y = 32 * (l_data->pos.y - 10);

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

	}
}

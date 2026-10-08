#include "Numerics.h"
#include "Toy2/Levels/AndysHouse.h"
#include "Toy2/Toy2.h"
#include "Toy2/Buzz.h"
#include "Toy2/Camera.h"
#include "Toy2/Collision.h"
#include "Toy2/Platform.h"
#include "Toy2/MoveableObject.h"
#include "Toy2/Dialogue.h"
#include "Toy2/Level.h"
#include "Toy2/Levels.h"
#include "Toy2/Actor.h"
#include "Nu3D/Math.h"
#include "Nu3D/Link.h"
#include "Nu3D/Particles.h"
#include "Renderer/Renderer.h"
#include "AudioManager/AudioManager.h"
#include "Random.h"

#include <math.h>
#include <stdlib.h>

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

		// GLOBAL: TOY2 0x004F0EB8
		int32_t g_unk4F0EB8[9] = { 754088, 168579, -550789, 733672, 168579, -550789, 754088, 168579, -530053 };

		// the hints of the actor in the kitchen, one for every token
		// GLOBAL: TOY2 0x004F0F7C
		char* g_unk4F0F7C[5] = {
			"^hamm^ is sitting on the sofa in the living room.",
			"^bo peep^ has lost her sheep. she is on the table in the ^kitchen^.",
			"^r.c. car^ is waiting to race you around the garage. he is through the hole in the door downstairs.",
			"you can move the boxes on the shelf in the ^basement^ to get to the token on the shelf. it is easier with the ^cosmic shield^. mr. potato head "
			"will let you use it, if you find his ^ear^ for him.",
			"the ^tin robot^ boss is in the attic above us. you can reach the attic by climbing the plant and then the rope.",
		};

		// GLOBAL: TOY2 0x0052F588
		int16_t g_unk52F588;

		// GLOBAL: TOY2 0x0052F5A0
		int32_t g_unk52F5A0;

		// GLOBAL: TOY2 0x0052F5A4
		int16_t g_unk52F5A4;

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

		// FUNCTION: TOY2 0x00417380
		void Method2(int16_t a1, int32_t linkId, Vector3I* v1, int16_t a4, int32_t a5)
		{
			if (Nu3D::Math::IsWithinDistance(v1, &g_buzzActor.posAngles.pos, 640))
			{
				int32_t phase = a1 & 0x1FFF;
				int16_t angle;

				if (phase >= 0x800)
				{
					if ((a1 & 0x1FFF) >= 0x1000)
					{
						if ((a1 & 0x1FFF) >= 0x1800)
						{
							int32_t i = 0;

							if (g_tickState[2])
							{
								do
								{
									Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnFromPreset(v1->x - 14336, v1->y + 4096, v1->z, 24, 13);
									particle->groundAlignRot = 16 * *g_randDatBufferPtr++;
									particle->rotSpeed = *g_randDatBufferPtr - 128;
									++i;
									++g_randDatBufferPtr;
								} while (i < g_tickState[2]);
							}

							angle = 0;
						}
						else
						{
							if ((phase - (Renderer::g_frameDelta << 6)) < 5120 && (a1 & 0x1FFF) >= 0x1400)
							{
								Collision::MarkPlatformAsMoving(a5);
								AudioManager::PlaySoundEffect(37, v1);
							}

							angle = 6144 - phase;
						}
					}
					else
					{
						angle = 2048;
					}
				}
				else
				{
					if ((phase - (Renderer::g_frameDelta << 6)) < 1024 && (a1 & 0x1FFF) >= 0x400)
						Platform::DisableCollision(a5);

					angle = phase;
				}

				Nu3D::Link::SetRotationRelative8bit(linkId, 0, 0, a4 & 0xFFF);
				Nu3D::Link::SetPositionRawAndCommit(linkId, v1->x >> 5, (v1->y >> 5) - (Numerics::g_sinCosLUT[(angle + 1024) & 0xFFF] >> 6) + 256, v1->z >> 5);
			}
		}

		// FUNCTION: TOY2 0x00417510
		void Method1(int16_t a1, int32_t linkId, Vector3I* v1, int16_t a4)
		{
			if (Nu3D::Math::IsWithinDistance(v1, &g_buzzActor.posAngles.pos, 640) != 0)
			{
				int32_t phase = a1 & 0x1FFF;
				int16_t angle;

				if (phase >= 0x800)
				{
					if ((a1 & 0x1FFF) >= 0x1000)
					{
						if ((a1 & 0x1FFF) >= 0x1800)
						{
							if ((phase - (Renderer::g_frameDelta << 6)) < 6144)
							{
								Nu3D::Particles::SpawnFromPreset(v1->x, v1->y + 18432, v1->z, 25, 2);
								Nu3D::Particles::SpawnFromPreset(v1->x, v1->y + 24576, v1->z, 26, 2);
								AudioManager::PlaySoundEffect(36, v1);
							}

							angle = 0;
							int32_t i = 0;

							if (g_tickState[2] != 0)
							{
								do
								{
									Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnFromPreset(v1->x, v1->y + 24576, v1->z, 24, 9);
									particle->groundAlignRot = 16 * *g_randDatBufferPtr++;
									particle->rotSpeed = *g_randDatBufferPtr - 128;
									++i;
									++g_randDatBufferPtr;
								} while (i < g_tickState[2]);
							}
						}
						else
						{
							angle = 6144 - phase;
						}
					}
					else
					{
						angle = 2048;
					}
				}
				else
				{
					angle = a1 & 0x1FFF;
				}

				Nu3D::Link::SetRotationRelative8bit(linkId, 0, a4 & 0xFFF, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(linkId, 4096, (Numerics::g_sinCosLUT[(angle + 1024) & 0xFFF] >> 3) + 3072, 4096);
			}
		}

		// FUNCTION: TOY2 0x00417680
		void Interactions()
		{
			Vector3I pos;
			int32_t soundTime;
			int32_t racePoint;
			int32_t side;
			int32_t slamTimer;
			bool slamNegative;

			if (g_curSectorCopy1 == 1)
			{
				if (g_levelActors[0].actorPhase != 80)
				{
					*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode = 0;
					g_unk52F574 += 4 * Renderer::g_frameDelta;
					g_levelActors[0].rollAngle += g_unk52F574;

					if (g_levelActors[0].rollAngle > 1792)
					{
						g_levelActors[0].rollAngle = 1792;
						g_unk52F574 /= -2;
						AudioManager::PlaySoundEffect(50, &g_levelActors[0].pos);

						if (g_unk52F574 > -8)
							g_levelActors[0].actorPhase = 80;
					}
				}

				if (g_levelActors[2].actorPhase != 80)
				{
					*(uint8_t*)&g_levelActors[2].creatureRam->defenseMode = 0;
					g_unk52F570 += 4 * Renderer::g_frameDelta;
					g_levelActors[2].rollAngle = (g_levelActors[2].rollAngle - g_unk52F570) & 0xFFF;

					if (g_levelActors[2].rollAngle < 2304)
					{
						g_levelActors[2].rollAngle = 2304;
						g_unk52F570 /= -2;
						AudioManager::PlaySoundEffect(50, &g_levelActors[2].pos);

						if (g_unk52F570 > -8)
							g_levelActors[2].actorPhase = 80;
					}
				}

				RawLoader::CreatureListRam* creatureRam = g_levelActors[2].creatureRam;
				char defenseMode = g_levelActors[0].creatureRam->defenseMode;

				if (defenseMode || (uint8_t)g_levelActors[2].creatureRam->defenseMode)
				{
					if (g_buzzActor.posAngles.pos.x >= 311924)
					{
						if (defenseMode)
						{
							*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode = 5;
							creatureRam = g_levelActors[2].creatureRam;
						}

						if ((uint8_t)creatureRam->defenseMode)
							*(uint8_t*)&creatureRam->defenseMode = 5;
					}
					else
					{
						if (defenseMode)
						{
							*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode = 4;
							creatureRam = g_levelActors[2].creatureRam;
						}

						if ((uint8_t)creatureRam->defenseMode)
							*(uint8_t*)&creatureRam->defenseMode = 4;
					}

					if (g_buzzActor.collisionFlags && Buzz::g_unk52B81C == 14)
						Platform::DisableCollision(9);
					else
						Collision::MarkPlatformAsMoving(9);
				}
				else if (! g_unk52F56C)
				{
					g_unk52F56C = 144;
					Platform::DisableCollision(9);
					g_unk52F594 = 2;
				}

				// the lamp falls
				if (g_unk52F56C > 0)
				{
					g_unk52F57C += 16 * Renderer::g_frameDelta;
					g_unk52F56C -= Renderer::g_frameDelta;

					if (g_unk52F56C <= 0)
						g_unk52F56C = -1;

					Nu3D::Link::GetCurrentPosFixed(19, &pos);
					int32_t speed = g_unk52F57C;

					if (pos.y > 72704 && g_unk52F57C > 0)
					{
						speed = g_unk52F57C / -2;
						g_unk52F57C /= -2;
					}

					pos.y += Renderer::g_frameDelta * speed;
					Nu3D::Link::SetPositionRawAndCommit(19, pos.x >> 5, pos.y >> 5, pos.z >> 5);
					Nu3D::Link::GetCurrentPosFixed(20, &pos);
					pos.y += Renderer::g_frameDelta * g_unk52F57C;

					Nu3D::Link::SetPositionRawAndCommit(20, pos.x >> 5, pos.y >> 5, pos.z >> 5);
					Nu3D::Link::GetCurrentPosFixed(26, &pos);
					pos.y = Renderer::g_frameDelta * g_unk52F57C + 4 * pos.y;
					Nu3D::Link::SetPositionRawAndCommit(26, pos.x >> 5, pos.y >> 7, pos.z >> 5);
				}
			}

			if (MoveableObject::g_moveableObjects[3].pushState == -1 || MoveableObject::g_moveableObjects[3].liftVel)
			{
				int32_t angle = g_unk52F500;

				if (! g_unk52F500)
				{
					Collision::MarkPlatformAsMoving(14);
					Platform::DisableCollision(1);
					angle = g_unk52F500;
				}

				int32_t newAngle = 16 * Renderer::g_frameDelta + angle;
				g_unk52F500 = newAngle;

				if (newAngle > 512)
				{
					newAngle = 512;
					g_unk52F500 = 512;
				}

				Nu3D::Link::SetRotationRelative8bit(4, newAngle, 0, 0);
			}

			if (g_unk52F5A0)
			{
				if (g_unk52F5A0 < 4096)
				{
					g_unk52F5A0 += Renderer::g_frameDelta << 6;
					int32_t scale = g_unk52F5A0;

					if (g_unk52F5A0 >= 4096)
					{
						scale = 4096;
						g_unk52F5A0 = 4096;
					}

					Nu3D::Link::SetScaleFromFixedOffsets(21, scale, 4096, 4096);
					Nu3D::Link::SnapToOtherLinkUsingScale(22, 21);
				}
			}
			else if (
				(uint32_t)g_buzzActor.posAngles.pos.z < (uint32_t)-321858 && (uint32_t)g_buzzActor.posAngles.pos.z > (uint32_t)-361602
				&& g_buzzActor.posAngles.pos.x < 314294 && g_buzzActor.posAngles.pos.x > 287606 && g_buzzActor.posAngles.pos.y < 18432
				&& g_buzzActor.posAngles.pos.y > 0 && g_buzzActor.unkShort40)
			{
				Collision::MarkPlatformAsMoving(8);
				g_unk52F5A0 = 8;
			}

			// the garage
			if (g_curSectorCopy1 == 4)
			{
				pos.x = 736794;
				pos.y = 157472;
				pos.z = -529601;

				if (Nu3D::Math::IsWithinDistance(&pos, &g_buzzActor.posAngles.pos, 640))
				{
					if (g_tickState[11])
					{
						int32_t spawnY = 32 * Levels::g_recordData[22]->data[0].y;
						int32_t spawnX = 32 * Levels::g_recordData[22]->data[0].x;
						int32_t spawnZ = 32 * Levels::g_recordData[22]->data[0].z;
						int32_t deltaZ = (spawnZ - 32 * Levels::g_recordData[22]->data[g_unk52F52C].z) >> 8;
						int32_t deltaX = (spawnX - 32 * Levels::g_recordData[22]->data[g_unk52F52C].x) >> 8;

						int32_t sparkAngle = (Nu3D::Math::CartesianToFixedAngle(deltaX, deltaZ) - 2048) & 0xFFF;
						int32_t distance = (int32_t)sqrt((double)(deltaX * deltaX + deltaZ * deltaZ));
						int32_t randomOffset = (*g_randDatBufferPtr - 128) >> 2;
						int32_t speed = 7 * distance / 2;
						int32_t velZ = speed * Numerics::g_sinCosLUT[(sparkAngle + 1024) & 0xFFF] / 0x2000;

						++g_randDatBufferPtr;

						Nu3D::Particles::SpawnInstance(
							spawnX,
							spawnY,
							spawnZ,
							speed * Numerics::g_sinCosLUT[sparkAngle] / 0x2000,
							-128 * ((distance << 12) / speed) / 64,
							velZ,
							128,
							0,
							randomOffset,
							12);

						int32_t skip = *g_randDatBufferPtr++ & 0xF;
						int32_t nextPoint = g_unk52F52C + skip + 1;
						g_unk52F52C = nextPoint;

						if (nextPoint >= Levels::g_recordData[22]->recordCount)
							g_unk52F52C = 1 - Levels::g_recordData[22]->recordCount + nextPoint;
					}

					int32_t timer = Renderer::g_frameDelta + g_unk52F5B0;
					g_unk52F5B0 += Renderer::g_frameDelta;

					if (g_unk52F5B0 > 120)
					{
						g_unk52F5B0 = timer - 120;
						int32_t sparkTable = g_unk52F598 + 3;
						g_unk52F598 += 3;

						if (g_unk52F598 == 9)
						{
							sparkTable = 0;
							g_unk52F598 = 0;
						}

						Nu3D::Particles::SpawnFromPreset(g_unk4F0EB8[sparkTable], g_unk4F0EB8[sparkTable + 1] - 2048, g_unk4F0EB8[sparkTable + 2], 34, 2);
					}

					int32_t i = 0;

					if (g_tickState[2])
					{
						do
						{
							Nu3D::Particles::SpawnFromPreset(g_unk4F0EB8[g_unk52F598], g_unk4F0EB8[g_unk52F598 + 1], g_unk4F0EB8[g_unk52F598 + 2], 16, 10)
								->lifetime = (*g_randDatBufferPtr & 0xF) + 32;
							++i;
							++g_randDatBufferPtr;
						} while (i < g_tickState[2]);
					}
				}

				pos.x = 750812;
				pos.y = 147262;
				pos.z = -418634;

				int32_t withinDistance = Nu3D::Math::IsWithinDistance(&pos, &g_buzzActor.posAngles.pos, 640);

				if (withinDistance)
				{
					if (withinDistance < 900)
						Buzz::HandleDamage(0, 2u);

					if (g_tickState[8])
					{
						Nu3D::Particles::SpawnFromPreset(pos.x, pos.y, pos.z, 17, 10)->rotSpeed = (*g_randDatBufferPtr - 128) >> 3;
						++g_randDatBufferPtr;
					}
				}

				if (MoveableObject::g_moveableObjects[4].pushState == -1)
					Levels::g_recordData[58]->data[Level::g_unk830E28 + 1].y = g_unk52F59C;

				if (g_buzzActor.collisionFlags && (Platform::GetFlags(3) & 3) == 2 && ((int16_t*)Platform::GetContactFace32(3))[2] < -12288)
				{
					Levels::Method3(1, 1);

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

				Actor::PlayPeriodicHintSound(1, 178);

				// bo peep
				if ((g_levelActors[1].actorFlags & Actor::ACTOR_PLAYER_TRIGGER) != 0)
				{
					g_levelActors[1].actorFlags &= ~Actor::ACTOR_PLAYER_TRIGGER;

					if (g_collectionCount >= 0)
					{
						if (g_collectionCount == 5)
						{
							AudioManager::Preset::PlayOneShotSound2(180, &g_levelActors[1]);
							Dialogue::Begin(1, 28, "you found my ^sheep^ buzz! thank you, here is your pizza planet ^token^.", -1, 0, 1);
							g_collectionCount = -1;
						}
						else
						{
							AudioManager::Preset::PlayOneShotSound2(179, &g_levelActors[1]);
							Dialogue::Begin(
								1,
								28,
								"hi buzz! please find my ^five^ missing ^sheep^. when you find them come back and see me for a pizza planet ^token^.",
								-1,
								0,
								-1);
						}
					}
				}
			}

			if (g_curSectorCopy1 == 2)
			{
				int32_t speed = g_unk52F504;
				bool notMoving;

				if (Buzz::g_unk53C5E4 == 48)
				{
					notMoving = g_unk52F504 <= 0;

					if (g_unk52F504)
						goto LABEL_89;

					g_unk52F504 = 2;
					AudioManager::PlaySoundEffect(33, &g_buzzActor.posAngles.pos);
					speed = g_unk52F504;
				}

				notMoving = speed <= 0;

			LABEL_89:
				if (notMoving)
				{
					soundTime = Renderer::g_frameDelta;
				}
				else
				{
					int32_t height = Renderer::g_frameDelta * speed + g_unk52F5B8.y;
					g_unk52F5B8.y = height;

					if (height > g_unk52F5B4)
					{
						height = g_unk52F5B4;
						g_unk52F504 = -1;
						g_unk52F5B8.y = g_unk52F5B4;
					}

					Levels::g_recordData[61]->data[16].y = height + g_unk52F530;
					Nu3D::Link::SetPositionRawAndCommit(6, g_unk52F5B8.x >> 5, g_unk52F5B8.y >> 5, g_unk52F5B8.z >> 5);
					soundTime = Renderer::g_frameDelta;
					g_unk52F504 += (Renderer::g_frameDelta << 7) / 4;
				}

				uint16_t spin = (g_unk52F4F4 + (soundTime << 8)) & 0xFFF;
				g_unk52F4F4 = spin;
				g_unk52F568 = (g_unk52F568 + (soundTime << 6)) & 0x1FFF;

				Method1(g_unk52F568, 9, (Vector3I*)g_unk52F538, spin);
				Method1(g_unk52F568 + 2730, 17, (Vector3I*)g_unk52F548, g_unk52F4F4 + 512);
				Method1(g_unk52F568 + 5461, 18, (Vector3I*)g_unk52F558, g_unk52F4F4 + 1024);

				uint16_t spinB = (g_unk52F5A4 + (Renderer::g_frameDelta << 7)) & 0xFFF;
				uint16_t phaseB = (g_unk52F588 + (Renderer::g_frameDelta << 6)) & 0x1FFF;
				g_unk52F5A4 = spinB;
				g_unk52F588 = phaseB;

				Method2(phaseB, 10, (Vector3I*)g_unk52F508, spinB, 12);
				Method2(g_unk52F588 + 4096, 16, (Vector3I*)g_unk52F518, g_unk52F5A4 + 512, 13);

				if ((g_levelActors[29].actorFlags & Actor::ACTOR_PLAYER_TRIGGER) == 0
					|| ((g_levelActors[29].actorFlags &= ~Actor::ACTOR_PLAYER_TRIGGER), Collectables::g_pickupSlots[2].state == 2) || g_unk52F2F8)
				{
					if (g_unk52F2F8 != 1)
					{
						if (g_unk52F2F8 < 2)
							goto LABEL_130;

						racePoint = g_unk52F528;

						if (g_unk52F2F8 != 2)
							goto LABEL_119;

						goto LABEL_104;
					}

					racePoint = g_unk52F528;
				}
				else
				{
					Dialogue::Begin(
						29,
						27,
						"hi buzz! i challenge you to a race! we will race ^3^ laps around the car. if you beat me, i will give you a pizza planet ^token.^ get "
						"ready to race 3... 2... 1...",
						0,
						0,
						-1);
					racePoint = 0;

					g_unk52F2F8 = 1;
					g_unk52F528 = 0;
					g_unk52F4F8 = 0;
					g_unk52AD64 = 0;
					g_unk52F584 = 0;
					g_unk52F580 = 1;

					g_levelActors[29].actorPhase = 202;
				}

				if (Renderer::g_enableCinematicBars)
					goto LABEL_130;

				g_levelActors[29].actorFlags |= Actor::ACTOR_LOCKED_MOVE;
				g_unk52F2F8 = 2;

			LABEL_104:
				// which side of the car Buzz is on, to count the laps
				side = (uint32_t)g_buzzActor.posAngles.pos.z < (uint32_t)-106496;

				if (g_buzzActor.posAngles.pos.x > 10496)
					side |= 2u;

				if (g_buzzActor.posAngles.pos.x < 135168)
					side |= 4u;

				if ((uint32_t)g_buzzActor.posAngles.pos.z > (uint32_t)-122880)
					side |= 8u;

				if (side == 14)
				{
					if (g_unk52F584 == 15)
					{
						if (! g_unk52F580)
							++g_unk52AD64;

						g_unk52F580 = 0;
					}
				}
				else if (side == 15 && g_unk52F584 == 14)
				{
					g_unk52F580 = 1;
				}

				g_unk52F584 = side;

			LABEL_119:
				// the RC car drives along the path
				int32_t* pointZ = &Levels::g_recordData[30]->data[racePoint].z;
				int32_t pointDeltaX = Levels::g_recordData[30]->data[racePoint].x - (g_levelActors[29].pos.x >> 5);

				if ((*pointZ - (g_levelActors[29].pos.z >> 5)) * (*pointZ - (g_levelActors[29].pos.z >> 5)) + pointDeltaX * pointDeltaX < 360000)
				{
					g_levelActors[29].boundary.x = 32 * Levels::g_recordData[30]->data[racePoint].x;
					g_levelActors[29].boundary.z = 32 * *pointZ;

					if (g_unk52F4F8 >= 3)
					{
						if ((uint32_t)g_levelActors[29].pos.z > (uint32_t)-106496)
						{
							if (g_unk52F2F8 == 2 && g_unk52AD64 < 3)
								g_unk52F2F8 = 3;

							g_levelActors[29].actorPhase = 1;
						}
					}
					else
					{
						g_unk52F528 = ++racePoint;

						if (racePoint > 44)
						{
							racePoint = 0;
							g_unk52F528 = 0;
							++g_unk52F4F8;
						}
					}
				}

				g_levelActors[29].motionTargetPos.x = 32 * Levels::g_recordData[30]->data[racePoint].x;
				g_levelActors[29].motionTargetPos.z = 32 * Levels::g_recordData[30]->data[racePoint].z;

				if (g_unk52AD64 == 3)
				{
					Collectables::Activate(2, 0);
					g_unk52F2F8 = 3;
				}
			}

		LABEL_130:
			// the basement, the box that is pushed
			if (g_curSectorCopy1 != 3)
				goto LABEL_146;

			Nu3D::Link::SetScaleFromFixedOffsets(23, 4096, abs(g_unk52F4FC) << 7, 4096);
			Nu3D::Link::SetRotationRelative8bit(23, 0, 0, 32 * abs(g_unk52F4FC));

			if (! g_buzzActor.collisionFlags || Buzz::g_unk52B81C != 8)
			{
				slamTimer = g_unk52F4FC;
				slamNegative = g_unk52F4FC <= 0;

				if (! g_unk52F4FC)
					goto LABEL_145;

			LABEL_137:
				if (slamNegative)
				{
					g_unk52F4FC = Renderer::g_frameDelta + slamTimer;

					if (Renderer::g_frameDelta + slamTimer > 0)
						g_unk52F4FC = 0;

					goto LABEL_145;
				}

				goto LABEL_138;
			}

			slamTimer = g_unk52F4FC;
			slamNegative = g_unk52F4FC <= 0;

			if (g_unk52F4FC)
				goto LABEL_137;

			if (Buzz::g_groundSlamTicker)
			{
				slamTimer = 2;

			LABEL_138:
				int32_t newTimer = Renderer::g_frameDelta + slamTimer;
				g_unk52F4FC = newTimer;

				if (newTimer <= 32)
				{
					if (newTimer > 6 && newTimer - Renderer::g_frameDelta <= 6)
					{
						Levels::Method3(0, 1);
						Buzz::g_groundSlamTicker = 0;
						Levels::Method2(-4736, 2);
						AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
						g_buzzActor.velForward = Numerics::g_sinCosLUT[1310];

						g_buzzActor.actorFlags |= 0x44;
						g_buzzActor.velX = Numerics::g_sinCosLUT[286];
						g_buzzActor.posAngles.angles.yaw = 286;
						g_buzzActor.facingAngle = 286;
						Camera::g_unk52AD98 = 64;
					}
				}
				else
				{
					g_unk52F4FC = -32;
				}
			}

		LABEL_145:
			Actor::CollectQuestReward(30, 29, 3600, 1760, 0);

		LABEL_146:
			Actor::RotatingHint(35, 39, g_unk4F0F7C);

			// the attic, the fight against the tin robot
			if (g_curSectorCopy1 == 5)
			{
				g_unk52F58C = Platform::StepTiltPhysics(15, 15, g_unk52F58C, 1, -448, 448, 8);

				if (g_inTinRobotFight == 1 && ! Renderer::g_enableCinematicBars)
				{
					g_inTinRobotFight = 2;
					g_levelActors[8].actorFlags |= Actor::ACTOR_GROUNDED;
					g_levelActors[8].movementData = Actor::g_movementData[5] + 10;
					g_levelActors[8].unkShort1 = 0;
					g_unk830D44 = 180;
				}

				if (g_levelActors[8].pos.y > -187921)
					g_levelActors[8].pos.y = -187921;
			}

			// the room of mr. potato head
			int32_t frameDelta;

			if (g_curSectorCopy1 == 6)
			{
				Actor::ItemReturnReward(
					31,
					25,
					"hey buzz! you need a ^cosmic shield^ to cross the ^green sludge^. find my missing ^ear^ and i will let you use the ^cosmic shield^!",
					"wow! thanks buzz! in return for finding my ^ear^ i will let you use the ^cosmic shield^. it will only protect you from harm for a little "
					"while so make sure you use it wisely.",
					"the ^cosmic shield^ will only protect you from harm for a little while so make sure you use it wisely.",
					512,
					2560);
				Renderer::BlitTextureByIndexOffset(8, 128, 128, 64, 64, 0, (Numerics::g_sinCosLUT[g_unk52F5D0] >> 11) & 0x3F, 64, 0);
				frameDelta = Renderer::g_frameDelta;
				g_unk52F5D0 = (g_unk52F5D0 + 32 * Renderer::g_frameDelta) & 0xFFF;
			}
			else
			{
				frameDelta = Renderer::g_frameDelta;
			}

			// the camera stays between the ceiling and the floor of the stairs
			if (Buzz::g_unk53C5E4 == 60)
			{
				if (g_buzzActor.posAngles.pos.y > -150000)
				{
					if (g_buzzActor.posAngles.pos.y >= -100000)
					{
					LABEL_161:
						if (g_buzzActor.posAngles.pos.y >= -68288)
						{
						LABEL_165:
							if (Camera::g_gameplayCameraState.pos.y < -50000)
							{
								Camera::g_gameplayCameraState.pos.y = -50000;

								Camera::g_gameplayCameraState.targetPos.y = -50000;
							}

							goto LABEL_167;
						}

						if (Camera::g_gameplayCameraState.pos.y < -50000)
						{
							Camera::g_gameplayCameraState.pos.y = -50000;
							Camera::g_gameplayCameraState.targetPos.y = -50000;
							Camera::g_gameplayCameraState.orbitPitch += frameDelta * (Camera::g_gameplayCameraState.orbitPitch + 256) / -8;
						}

					LABEL_164:
						if (g_buzzActor.posAngles.pos.y < -68288)
							goto LABEL_167;

						goto LABEL_165;
					}

					if (Camera::g_gameplayCameraState.pos.y > -150000)
					{
						Camera::g_gameplayCameraState.pos.y = -150000;
						Camera::g_gameplayCameraState.targetPos.y = -150000;
						Camera::g_gameplayCameraState.orbitPitch += frameDelta * (Camera::g_gameplayCameraState.orbitPitch - 768) / -8;
					}
				}

				if (g_buzzActor.posAngles.pos.y < -100000)
					goto LABEL_164;

				goto LABEL_161;
			}

		LABEL_167:
			g_unk52F5CC = (g_unk52F5CC + 32 * frameDelta) & 0xFFF;
			Nu3D::Link::SetRotationRelative8bit(14, 0, g_unk52F5CC, 0);

			if (g_curSectorCopy1 != 2 && g_unk52F2F8 && (g_levelActors[29].actorFlags & Actor::ACTOR_CUTSCENE_MOVE) == 0
				&& ! Collectables::g_pickupSlots[2].state)
			{
				g_unk52F2F8 = 0;
				Game::InitActor(&g_levelActors[29], 0);
			}

			if (g_buzzActor.posAngles.pos.y > -276188 && g_buzzActor.posAngles.pos.z > -291419 && g_buzzActor.posAngles.pos.y < -137750
				&& g_inTinRobotFight == 2)
				g_hudWidgetTimers[7] = 90;

			PlayLevelMusic();
		}
	}
}

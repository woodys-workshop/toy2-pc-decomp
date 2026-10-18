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

		// clang-format off
		// GLOBAL: TOY2 0x004F0EE8
		Collectables::TokenTableEntry g_tutorialHints[10] = {
			// camera modes
			{ 67, 31,
			  "you can change camera mode using the pause menu. active camera will always try and turn to view buzz from behind. "
			  "passive camera will not turn. "
			  "the camera left and right buttons turn the ^camera^ in ^passive^ mode and turn ^buzz^ in ^active^ mode.",
			  2048 },

			// horizontal bars
			{ 62, 32,
			  "if you jump at a horizontal bar you will swing around it. press jump when you have left the bar to jump farther and higher.",
			  2048 },

			// foot stomp
			{ 63, 33,
			  "you can foot ^stomp^ where you see the ^stomp^ icon by pressing jump and then pressing ^spin^ while you are in the air. "
			  "this will activate switches and catapults and will also act as an attack.",
			  2435 },

			// pushing objects
			{ 64, 34,
			  "if you see the ^push^ icon you can run into the object and push it out of the way. ",
			  2048 },

			// extended jump
			{ 68, 35,
			  "if you press the jump button once and then press it again you can do an extended jump. try it to reach the open drawer.",
			  727 },

			// poles
			{ 65, 36,
			  "you can grab onto poles. when you are holding on you can push up and down to move up and down the pole. "
			  "you can press left and right to turn on the pole. press jump to let go of the pole.",
			  226 },

			// zip lines
			{ 69, 37,
			  "you can jump onto zip lines to slide down them. press jump to let go of the line.",
			  48 },

			// laser targeting
			{ 66, 38,
			  "you can target your laser by using your visor view. press the visor toggle button to activate the view. "
			  "then press the target lock button to swap between visible targets, then press fire to use your laser. "
			  "use it to shoot the catches holding up the side of the crib.",
			  3072 },

			// laser and spin attacks
			{ 71, 40,
			  "if you press the fire button you can attack with your laser. if you press the spin button you can attack with your spin attack. "
			  "hold down the button to charge up the attack. your spin attack can deflect objects that are fired at you.",
			  0 },

			// terminator
			{ -1, 0, 0, 0 },
		};
		// clang-format on

		// GLOBAL: TOY2 0x004F0F90
		int16_t g_unk4F0F90[24] = { 1, 5, 1, 2, 6, 2, 3, 7, 3, 4, 1, 4, 7, 3, 7, 8, 4, 8, 0, 0, 0, -1, 0, 0 };

		// GLOBAL: TOY2 0x004F0EB8
		int32_t g_unk4F0EB8[9] = { 754088, 168579, -550789, 733672, 168579, -550789, 754088, 168579, -530053 };

		// GLOBAL: TOY2 0x004F0F7C
		char* g_tokenHints[5] = {
			"^hamm^ is sitting on the sofa in the living room.",
			"^bo peep^ has lost her sheep. she is on the table in the ^kitchen^.",
			"^r.c. car^ is waiting to race you around the garage. he is through the hole in the door downstairs.",
			"you can move the boxes on the shelf in the ^basement^ to get to the token on the shelf. it is easier with the ^cosmic shield^. mr. potato head "
			"will let you use it, if you find his ^ear^ for him.",
			"the ^tin robot^ boss is in the attic above us. you can reach the attic by climbing the plant and then the rope.",
		};

		// GLOBAL: TOY2 0x0052F588
		int32_t g_unk52F588;

		// GLOBAL: TOY2 0x0052F5A0
		int32_t g_link21Scale;

		// GLOBAL: TOY2 0x0052F5A4
		int32_t g_unk52F5A4;

		// GLOBAL: TOY2 0x0052F4F4
		int32_t g_unk52F4F4;

		// GLOBAL: TOY2 0x0052F4F8
		int32_t g_raceCarLaps;

		// GLOBAL: TOY2 0x0052F4FC
		int32_t g_unk52F4FC;

		// GLOBAL: TOY2 0x0052F500
		int32_t g_unk52F500;

		// GLOBAL: TOY2 0x0052F504
		int32_t g_liftVelocity;

		// GLOBAL: TOY2 0x0052F508
		int32_t g_unk52F508[4];

		// GLOBAL: TOY2 0x0052F518
		int32_t g_unk52F518[4];

		// GLOBAL: TOY2 0x0052F528
		int32_t g_raceCarPathIndex;

		// GLOBAL: TOY2 0x0052F52C
		int32_t g_sparkPathIndex;

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
		int32_t g_raceBuzzLastSide;

		// GLOBAL: TOY2 0x0052F58C
		int32_t g_atticTilt;

		// GLOBAL: TOY2 0x0052F590
		int32_t g_tinRobotFlashTimer;

		// GLOBAL: TOY2 0x0052F594
		int32_t g_unk52F594;

		// GLOBAL: TOY2 0x0052F598
		int32_t g_sparkPresetIndex;

		// GLOBAL: TOY2 0x0052F59C
		int32_t g_unk52F59C;

		// GLOBAL: TOY2 0x0052F5A8
		int32_t g_tinRobotLastPhase;

		// GLOBAL: TOY2 0x0052F5AC
		int32_t g_unk52F5AC;

		// GLOBAL: TOY2 0x0052F5B0
		int32_t g_sparkPresetTimer;

		// GLOBAL: TOY2 0x0052F5B4
		int32_t g_liftMaxY;

		// GLOBAL: TOY2 0x0052F5B8
		Vector3I g_liftPos;

		// GLOBAL: TOY2 0x0052F5C8
		int32_t g_tinRobotFlash;

		// GLOBAL: TOY2 0x0052F5CC
		int32_t g_link14Rotation;

		// GLOBAL: TOY2 0x0052F5D0
		int32_t g_potatoHeadBlitAngle;

		// FUNCTION: TOY2 0x00417380
		void AnimateBobbingPlatform(int16_t cycleTime, int32_t linkId, Vector3I* pos, int16_t roll, int32_t platformId)
		{
			if (Nu3D::Math::IsWithinDistance(pos, &g_buzzActor.posAngles.pos, 640))
			{
				int32_t phase = cycleTime & 0x1FFF;
				int16_t angle;

				if (phase >= 0x800)
				{
					if ((cycleTime & 0x1FFF) >= 0x1000)
					{
						if ((cycleTime & 0x1FFF) >= 0x1800)
						{
							int32_t i = 0;

							if (g_tickState[2])
							{
								do
								{
									Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnFromPreset(pos->x - 14336, pos->y + 4096, pos->z, 24, 13);
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
							if ((phase - (Renderer::g_frameDelta << 6)) < 5120 && (cycleTime & 0x1FFF) >= 0x1400)
							{
								Collision::MarkPlatformAsMoving(platformId);
								AudioManager::PlaySoundEffect(37, pos);
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
					if ((phase - (Renderer::g_frameDelta << 6)) < 1024 && (cycleTime & 0x1FFF) >= 0x400)
						Platform::DisableCollision(platformId);

					angle = phase;
				}

				Nu3D::Link::SetRotationRelative8bit(linkId, 0, 0, roll & 0xFFF);
				Nu3D::Link::SetPositionRawAndCommit(linkId, pos->x >> 5, (pos->y >> 5) - (Numerics::g_sinCosLUT[(angle + 1024) & 0xFFF] >> 6) + 256, pos->z >> 5);
			}
		}

		// FUNCTION: TOY2 0x00417510
		void AnimateStretchingObject(int16_t cycleTime, int32_t linkId, Vector3I* pos, int16_t yaw)
		{
			if (Nu3D::Math::IsWithinDistance(pos, &g_buzzActor.posAngles.pos, 640) != 0)
			{
				int32_t phase = cycleTime & 0x1FFF;
				int16_t angle;

				if (phase >= 0x800)
				{
					if ((cycleTime & 0x1FFF) >= 0x1000)
					{
						if ((cycleTime & 0x1FFF) >= 0x1800)
						{
							if ((phase - (Renderer::g_frameDelta << 6)) < 6144)
							{
								Nu3D::Particles::SpawnFromPreset(pos->x, pos->y + 18432, pos->z, 25, 2);
								Nu3D::Particles::SpawnFromPreset(pos->x, pos->y + 24576, pos->z, 26, 2);
								AudioManager::PlaySoundEffect(36, pos);
							}

							angle = 0;
							int32_t i = 0;

							if (g_tickState[2] != 0)
							{
								do
								{
									Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnFromPreset(pos->x, pos->y + 24576, pos->z, 24, 9);
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
					angle = cycleTime & 0x1FFF;
				}

				Nu3D::Link::SetRotationRelative8bit(linkId, 0, yaw & 0xFFF, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(linkId, 4096, (Numerics::g_sinCosLUT[(angle + 1024) & 0xFFF] >> 3) + 3072, 4096);
			}
		}

		// FUNCTION: TOY2 0x00417680 [MATCHED]
		void Interactions()
		{
			// $TODO: the original frame is 4 bytes larger than its locals need (sub esp, 0x1c), the padding member keeps the size
			struct PosBuf : Vector3I
			{
				int32_t w;
			} pos;

			if (g_curSectorCopy1 == 1)
			{
				if (g_levelActors[0].actorPhase != 80)
				{
					g_levelActors[0].creatureRam->defenseMode = 0;
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
					g_levelActors[2].creatureRam->defenseMode = 0;
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

				if (g_levelActors[0].creatureRam->defenseMode || g_levelActors[2].creatureRam->defenseMode)
				{
					if (g_buzzActor.posAngles.pos.x < 311924)
					{
						if (g_levelActors[0].creatureRam->defenseMode)
							g_levelActors[0].creatureRam->defenseMode = 4;

						if (g_levelActors[2].creatureRam->defenseMode)
							g_levelActors[2].creatureRam->defenseMode = 4;
					}
					else
					{
						if (g_levelActors[0].creatureRam->defenseMode)
							g_levelActors[0].creatureRam->defenseMode = 5;

						if (g_levelActors[2].creatureRam->defenseMode)
							g_levelActors[2].creatureRam->defenseMode = 5;
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

				int32_t timer = g_unk52F56C;

				if (timer > 0)
				{
					g_unk52F57C += 16 * Renderer::g_frameDelta;
					timer -= Renderer::g_frameDelta;
					g_unk52F56C = timer;

					if (timer <= 0)
						g_unk52F56C = -1;

					Nu3D::Link::GetCurrentPosFixed(19, &pos);
					if (pos.y > 72704 && g_unk52F57C > 0)
						g_unk52F57C /= -2;

					pos.y += g_unk52F57C * Renderer::g_frameDelta;
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
				if (! g_unk52F500)
				{
					Collision::MarkPlatformAsMoving(14);
					Platform::DisableCollision(1);
				}

				g_unk52F500 += 16 * Renderer::g_frameDelta;

				if (g_unk52F500 > 512)
					g_unk52F500 = 512;

				Nu3D::Link::SetRotationRelative8bit(4, g_unk52F500, 0, 0);
			}

			if (! g_link21Scale)
			{
				if (g_buzzActor.posAngles.pos.z < -321858u && g_buzzActor.posAngles.pos.z > -361602u && g_buzzActor.posAngles.pos.x < 314294
					&& g_buzzActor.posAngles.pos.x > 287606 && g_buzzActor.posAngles.pos.y < 18432 && g_buzzActor.posAngles.pos.y > 0 && g_buzzActor.unkShort40)
				{
					Collision::MarkPlatformAsMoving(8);
					g_link21Scale = 8;
				}
			}
			else if (g_link21Scale < 4096)
			{
				g_link21Scale += Renderer::g_frameDelta << 6;
				int32_t scale = g_link21Scale;

				if (g_link21Scale >= 4096)
				{
					scale = 4096;
					g_link21Scale = 4096;
				}

				Nu3D::Link::SetScaleFromFixedOffsets(21, scale, 4096, 4096);
				Nu3D::Link::SnapToOtherLinkUsingScale(22, 21);
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
						int32_t deltaX = (spawnX - 32 * Levels::g_recordData[22]->data[g_sparkPathIndex].x) >> 8;
						int32_t deltaZ = (spawnZ - 32 * Levels::g_recordData[22]->data[g_sparkPathIndex].z) >> 8;

						int32_t sparkAngle = (Nu3D::Math::CartesianToFixedAngle(deltaX, deltaZ) - 2048) & 0xFFF;
						int32_t distance = (int32_t)sqrt((double)(deltaX * deltaX + deltaZ * deltaZ));
						int32_t speed = 7 * distance / 2;

						Nu3D::Particles::SpawnInstance(
							spawnX,
							spawnY,
							spawnZ,
							speed * Numerics::g_sinCosLUT[sparkAngle] / 0x2000,
							-128 * ((distance << 12) / speed) / 64,
							speed * Numerics::g_sinCosLUT[(sparkAngle + 1024) & 0xFFF] / 0x2000,
							128,
							0,
							(*g_randDatBufferPtr++ - 128) >> 2,
							12);

						g_sparkPathIndex += (*g_randDatBufferPtr++ & 0xF) + 1;

						if (g_sparkPathIndex >= Levels::g_recordData[22]->recordCount)
							g_sparkPathIndex = 1 - Levels::g_recordData[22]->recordCount + g_sparkPathIndex;
					}

					g_sparkPresetTimer += Renderer::g_frameDelta;

					if (g_sparkPresetTimer > 120)
					{
						g_sparkPresetTimer -= 120;
						g_sparkPresetIndex += 3;

						if (g_sparkPresetIndex == 9)
							g_sparkPresetIndex = 0;

						Nu3D::Particles::SpawnFromPreset(
							g_unk4F0EB8[g_sparkPresetIndex], g_unk4F0EB8[g_sparkPresetIndex + 1] - 2048, g_unk4F0EB8[g_sparkPresetIndex + 2], 34, 2);
					}

					int32_t i = 0;

					if (g_tickState[2] > 0)
					{
						do
						{
							Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnFromPreset(
								g_unk4F0EB8[g_sparkPresetIndex], g_unk4F0EB8[g_sparkPresetIndex + 1], g_unk4F0EB8[g_sparkPresetIndex + 2], 16, 10);
							particle->lifetime = (*g_randDatBufferPtr & 0xF) + 32;
							++g_randDatBufferPtr;
							++i;
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
						Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnFromPreset(pos.x, pos.y, pos.z, 17, 10);
						particle->rotSpeed = (*g_randDatBufferPtr - 128) >> 3;
						++g_randDatBufferPtr;
					}
				}

				if (MoveableObject::g_moveableObjects[4].pushState == -1)
					Levels::g_recordData[58]->data[Level::g_unk830E28 + 1].y = g_unk52F59C;

				if (g_buzzActor.collisionFlags && (Platform::GetFlags(3) & 3) == 2 && ((int16_t*)Platform::GetContactFace32(3))[1] < -12288)
				{
					Levels::Method3(1, 1);

					// the sound is in both branches, that way the compiler merges the stack clean up of the two calls
					if (Buzz::g_groundSlamTicker)
					{
						Buzz::g_groundSlamTicker = 0;
						Levels::Method2(-3072, 2);
						AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
					}
					else
					{
						Levels::Method2(-2432, 2);
						AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
					}
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
				if (Buzz::g_unk53C5E4 == 48 && ! g_liftVelocity)
				{
					g_liftVelocity = 2;
					AudioManager::PlaySoundEffect(33, &g_buzzActor.posAngles.pos);
				}

				int32_t soundTime;

				if (g_liftVelocity > 0)
				{
					g_liftPos.y += g_liftVelocity * Renderer::g_frameDelta;

					if (g_liftPos.y > g_liftMaxY)
					{
						g_liftPos.y = g_liftMaxY;
						g_liftVelocity = -1;
					}

					Levels::g_recordData[61]->data[16].y = g_liftPos.y + g_unk52F530;
					Nu3D::Link::SetPositionRawAndCommit(6, g_liftPos.x >> 5, g_liftPos.y >> 5, g_liftPos.z >> 5);
					soundTime = Renderer::g_frameDelta;
					g_liftVelocity += (soundTime << 7) / 4;
				}
				else
				{
					soundTime = Renderer::g_frameDelta;
				}

				g_unk52F4F4 = (g_unk52F4F4 + (soundTime << 8)) & 0xFFF;
				g_unk52F568 = (g_unk52F568 + (soundTime << 6)) & 0x1FFF;

				AnimateStretchingObject(g_unk52F568, 9, (Vector3I*)g_unk52F538, g_unk52F4F4);
				AnimateStretchingObject(g_unk52F568 + 2730, 17, (Vector3I*)g_unk52F548, g_unk52F4F4 + 512);
				AnimateStretchingObject(g_unk52F568 + 5461, 18, (Vector3I*)g_unk52F558, g_unk52F4F4 + 1024);

				AnimateBobbingPlatform(
					g_unk52F588 = (g_unk52F588 + (Renderer::g_frameDelta << 6)) & 0x1FFF,
					10,
					(Vector3I*)g_unk52F508,
					g_unk52F5A4 = (g_unk52F5A4 + (Renderer::g_frameDelta << 7)) & 0xFFF,
					12);
					
				AnimateBobbingPlatform(g_unk52F588 + 4096, 16, (Vector3I*)g_unk52F518, g_unk52F5A4 + 512, 13);

				if ((g_levelActors[29].actorFlags & Actor::ACTOR_PLAYER_TRIGGER) != 0
					&& ((g_levelActors[29].actorFlags &= ~Actor::ACTOR_PLAYER_TRIGGER), Collectables::g_pickupSlots[2].state != 2) && ! g_unk52F2F8)
				{
					Dialogue::Begin(
						29,
						27,
						"hi buzz! i challenge you to a race! we will race ^3^ laps around the car. if you beat me, i will give you a pizza planet ^token.^ get "
						"ready to race 3... 2... 1...",
						0,
						0,
						-1);
					g_unk52F2F8 = 1;
					g_raceCarPathIndex = 0;
					g_raceCarLaps = 0;
					g_unk52AD64 = 0;
					g_raceBuzzLastSide = 0;
					g_unk52F580 = 1;

					g_levelActors[29].actorPhase = 202;
				}

				if (g_unk52F2F8 == 1 && ! Renderer::g_enableCinematicBars)
				{
					g_levelActors[29].actorFlags |= Actor::ACTOR_LOCKED_MOVE;
					g_unk52F2F8 = 2;
				}

				if (g_unk52F2F8 >= 2)
				{
					if (g_unk52F2F8 == 2)
					{
						// which side of the car Buzz is on, to count the laps
						int32_t side = 0;
						int32_t buzzZ = g_buzzActor.posAngles.pos.z;
						int32_t buzzX = g_buzzActor.posAngles.pos.x;

						if (buzzZ < -106496u)
							side = 1;

						if (buzzX > 10496)
							side |= 2;

						if (buzzX < 135168)
							side |= 4;

						if (buzzZ > -122880u)
							side |= 8;

						if (side == 14)
						{
							if (g_raceBuzzLastSide == 15)
							{
								if (! g_unk52F580)
									++g_unk52AD64;

								g_unk52F580 = 0;
							}
						}
						else if (side == 15 && g_raceBuzzLastSide == 14)
						{
							g_unk52F580 = 1;
						}

						g_raceBuzzLastSide = side;
					}

					// the RC car drives along the path
					int32_t pointDeltaX = Levels::g_recordData[30]->data[g_raceCarPathIndex].x - (g_levelActors[29].pos.x >> 5);
					int32_t* pointZ = &Levels::g_recordData[30]->data[g_raceCarPathIndex].z;

					if ((*pointZ - (g_levelActors[29].pos.z >> 5)) * (*pointZ - (g_levelActors[29].pos.z >> 5)) + pointDeltaX * pointDeltaX < 360000)
					{
						g_levelActors[29].boundary.x = 32 * Levels::g_recordData[30]->data[g_raceCarPathIndex].x;
						g_levelActors[29].boundary.z = 32 * *pointZ;

						if (g_raceCarLaps < 3)
						{
							++g_raceCarPathIndex;

							if (g_raceCarPathIndex > 44)
							{
								g_raceCarPathIndex = 0;
								++g_raceCarLaps;
							}
						}
						else if ((uint32_t)g_levelActors[29].pos.z > (uint32_t)-106496)
						{
							if (g_unk52F2F8 == 2 && g_unk52AD64 < 3)
								g_unk52F2F8 = 3;

							g_levelActors[29].actorPhase = 1;
						}
					}

					g_levelActors[29].motionTargetPos.x = 32 * Levels::g_recordData[30]->data[g_raceCarPathIndex].x;
					g_levelActors[29].motionTargetPos.z = 32 * Levels::g_recordData[30]->data[g_raceCarPathIndex].z;

					if (g_unk52AD64 == 3)
					{
						Collectables::Activate(2, 0);
						g_unk52F2F8 = 3;
					}
				}
			}

			if (g_curSectorCopy1 == 3)
			{
				Nu3D::Link::SetScaleFromFixedOffsets(23, 4096, abs(g_unk52F4FC) << 7, 4096);
				Nu3D::Link::SetRotationRelative8bit(23, 0, 0, 32 * abs(g_unk52F4FC));

				int32_t slamTimer;

				if (g_buzzActor.collisionFlags && Buzz::g_unk52B81C == 8)
				{
					slamTimer = g_unk52F4FC;

					if (! slamTimer && Buzz::g_groundSlamTicker)
						slamTimer = 2;
				}
				else
				{
					slamTimer = g_unk52F4FC;
				}

				if (slamTimer)
				{
					if (slamTimer > 0)
					{
						int32_t newTimer = Renderer::g_frameDelta + slamTimer;
						g_unk52F4FC = newTimer;

						if (newTimer > 32)
						{
							g_unk52F4FC = -32;
						}
						else if (newTimer > 6 && newTimer - (int32_t)Renderer::g_frameDelta <= 6)
						{
							Levels::Method3(0, 1);
							Buzz::g_groundSlamTicker = 0;
							Levels::Method2(-4736, 2);
							AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
							g_buzzActor.velX = Numerics::g_sinCosLUT[286];

							g_buzzActor.actorFlags |= 0x44;
							g_buzzActor.velForward = Numerics::g_sinCosLUT[1310];
							g_buzzActor.posAngles.angles.yaw = 286;
							g_buzzActor.facingAngle = 286;
							Camera::g_unk52AD98 = 64;
						}
					}
					else
					{
						g_unk52F4FC += Renderer::g_frameDelta;

						if (g_unk52F4FC > 0)
							g_unk52F4FC = 0;
					}
				}

				Actor::CollectQuestReward(30, 29, 3600, 1760, 0);
			}

			Actor::RotatingHint(35, 39, g_tokenHints);

			// the attic, the fight against the tin robot
			if (g_curSectorCopy1 == 5)
			{
				g_atticTilt = Platform::StepTiltPhysics(15, 15, g_atticTilt, 1, -448, 448, 8);

				if (g_inTinRobotFight == 1 && ! Renderer::g_enableCinematicBars)
				{
					g_inTinRobotFight = 2;
					g_levelActors[8].movementData = Actor::g_movementData[5] + 10;
					g_levelActors[8].actorFlags |= Actor::ACTOR_GROUNDED;
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
				Renderer::BlitTextureByIndexOffset(8, 128, 128, 64, 64, 0, (Numerics::g_sinCosLUT[g_potatoHeadBlitAngle] >> 11) & 0x3F, 64, 0);
				frameDelta = Renderer::g_frameDelta;
				g_potatoHeadBlitAngle = (g_potatoHeadBlitAngle + 32 * frameDelta) & 0xFFF;
			}
			else
			{
				frameDelta = Renderer::g_frameDelta;
			}

			// the camera stays between the ceiling and the floor of the stairs
			if (Buzz::g_unk53C5E4 == 60)
			{
				if (g_buzzActor.posAngles.pos.y > -150000 && g_buzzActor.posAngles.pos.y < -100000 && Camera::g_gameplayCameraState.pos.y > -150000)
				{
					Camera::g_gameplayCameraState.pos.y = -150000;
					Camera::g_gameplayCameraState.targetPos.y = -150000;
					Camera::g_gameplayCameraState.orbitPitch += frameDelta * (Camera::g_gameplayCameraState.orbitPitch - 768) / -8;
				}

				if (g_buzzActor.posAngles.pos.y >= -100000 && g_buzzActor.posAngles.pos.y < -68288 && Camera::g_gameplayCameraState.pos.y < -50000)
				{
					Camera::g_gameplayCameraState.pos.y = -50000;
					Camera::g_gameplayCameraState.targetPos.y = -50000;
					Camera::g_gameplayCameraState.orbitPitch += frameDelta * (Camera::g_gameplayCameraState.orbitPitch + 256) / -8;
				}

				if (g_buzzActor.posAngles.pos.y >= -68288 && Camera::g_gameplayCameraState.pos.y < -50000)
				{
					Camera::g_gameplayCameraState.pos.y = -50000;
					Camera::g_gameplayCameraState.targetPos.y = -50000;
				}
			}

			g_link14Rotation = (g_link14Rotation + 32 * frameDelta) & 0xFFF;
			Nu3D::Link::SetRotationRelative8bit(14, 0, g_link14Rotation, 0);

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

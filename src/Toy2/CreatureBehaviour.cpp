#include "Toy2/CreatureBehaviour.h"
#include "Toy2/Toy2.h"
#include "Toy2/Particles.h"
#include "Toy2/Buzz.h"
#include "Toy2/Camera.h"
#include "Toy2/Collectables.h"
#include "Toy2/Dialogue.h"
#include "Toy2/Lighting.h"
#include "Toy2/Levels/AirportInfiltration.h"
#include "Toy2/Levels/AlleysAndGullies.h"
#include "Toy2/Levels/AlsPenthouse.h"
#include "Toy2/Levels/AlsSpaceLand.h"
#include "Toy2/Levels/AlsToyBarn.h"
#include "Toy2/Levels/AndysHouse.h"
#include "Toy2/Levels/AndysNeighborhood.h"
#include "Toy2/Levels/ConstructionYard.h"
#include "Toy2/Levels/ElevatorHop.h"
#include "Toy2/Levels/FinalShowdown.h"
#include "Toy2/Levels/TarmacTrouble.h"
#include "Nu3D/Math.h"
#include "Nu3D/Collision.h"
#include "Nu3D/Particles.h"
#include "AudioManager/AudioManager.h"
#include "Renderer/Renderer.h"
#include "Renderer/Effects.h"
#include "Renderer/LensFlare.h"
#include "Toy2/Collision.h"
#include "CharacterLoader.h"
#include "Random.h"
#include "Numerics.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace Toy2
{
	namespace CreatureBehaviour
	{
		// GLOBAL: TOY2 0x0050A544
		int32_t g_unk50A544;

		// GLOBAL: TOY2 0x0050A548
		int32_t g_unk50A548;

		// FUNCTION: TOY2 0x00406220
		void Zurg3(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			int32_t localStrafeSpeed = context->localStrafeSpeed;
			Toy2::Actor::Toy2Actor* actor = context->actor;

			if (localStrafeSpeed <= 256)
			{
				if (localStrafeSpeed < -256)
					localStrafeSpeed = -256;
			}
			else
			{
				localStrafeSpeed = 256;
			}

			bool playerSideNegative = actor->unkVar29 < 0;
			actor->rollAngle -= (int32_t)Renderer::g_frameDelta * (localStrafeSpeed + actor->rollAngle) / 16;

			if (! playerSideNegative)
				AudioManager::PlaySoundEffect(44, &actor->pos);

			if ((context->targetFlags & 1) != 0)
			{
				int32_t previousTimer = actor->unkShort4;
				actor->unkShort4 -= Renderer::g_frameDelta;

				if (actor->unkShort4 >= 0)
				{
					if (previousTimer > 200 && actor->unkShort4 <= 200)
					{
						actor->primaryAnimIdx = 1;
						actor->unkVar30 = (int32_t)Actor::g_unk4E0588[8];
						actor->unkVar7 = *Actor::g_unk4E0588[8] << 16;
					}

					if (previousTimer > 182 && actor->unkShort4 <= 182)
					{
						int32_t yawAngle = actor->yawAngle;

						Nu3D::Particles::SpawnInstance(
							actor->pos.x + (Numerics::g_sinCosLUT[(yawAngle + 1024) & 0xFFF] >> 2),
							actor->pos.y,
							actor->pos.z + (Numerics::g_sinCosLUT[(yawAngle - 2048) & 0xFFF] >> 2),
							Numerics::g_sinCosLUT[yawAngle] >> 4,
							-512,
							Numerics::g_sinCosLUT[(yawAngle + 1024) & 0xFFF] >> 4,
							64,
							0,
							128,
							38);
						AudioManager::PlaySoundEffect(13, &actor->pos);
					}

					if (previousTimer > 166 && actor->unkShort4 <= 166)
					{
						int32_t yawAngle = actor->yawAngle;

						Nu3D::Particles::SpawnInstance(
							actor->pos.x + (Numerics::g_sinCosLUT[(yawAngle - 1024) & 0xFFF] >> 2),
							actor->pos.y,
							actor->pos.z + (Numerics::g_sinCosLUT[yawAngle & 0xFFF] >> 2),
							Numerics::g_sinCosLUT[yawAngle] >> 4,
							-512,
							Numerics::g_sinCosLUT[(yawAngle + 1024) & 0xFFF] >> 4,
							64,
							0,
							128,
							38);
						AudioManager::PlaySoundEffect(13, &actor->pos);
					}

					if (previousTimer > 136 && actor->unkShort4 <= 136)
					{
						actor->primaryAnimIdx = 0;
						actor->unkVar30 = (int32_t)Actor::g_unk4E0588[7];
						actor->unkVar7 = *Actor::g_unk4E0588[7] << 16;
					}
				}
				else
				{
					int32_t angle = actor->yawAngle + 512;
					actor->unkShort4 = 360;

					if (*g_randDatBufferPtr++ < 0x80u)
						angle -= 1024;

					actor->velX = Numerics::g_sinCosLUT[angle] >> 4;
					actor->velForward = Numerics::g_sinCosLUT[(angle + 1024) & 0xFFF] >> 4;
				}
			}
			else
			{
				if (actor->primaryAnimIdx == 1)
				{
					actor->primaryAnimIdx = 0;
					actor->unkVar30 = (int32_t)Actor::g_unk4E0588[7];
					actor->unkVar7 = *Actor::g_unk4E0588[7] << 16;
				}

				actor->unkShort4 = 260;
			}
		}

		// FUNCTION: TOY2 0x004064A0
		void ZgCar(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;

			if (! actor->unkShort4)
				actor->unkShort4 = actor->actorPhase;

			int16_t actorPhase = actor->actorPhase;

			if (actor->unkShort4 != actorPhase && actorPhase < 101)
			{
				bool isAnim1 = actor->primaryAnimIdx == 1;
				actor->unkShort4 = actorPhase;

				if (! isAnim1)
				{
					actor->unkShort1 = 0;
					actor->movementData = Actor::g_movementData[12] + 62;
				}
			}

			if (g_tickState[8] && actor->creatureRam->speedNoTarget == 0xFF)
				AudioManager::PlaySoundEffect(66, &actor->pos);

			int16_t localForwardSpeed = context->localForwardSpeed;

			if (localForwardSpeed > 0)
			{
				AudioManager::g_pitchArray[0] = 4 * localForwardSpeed;
				AudioManager::PlaySoundEffect(65, &actor->pos);
			}

			if ((actor->actorFlags & 1) == 1 && actor->unkVar29 >= 0 && context->localForwardSpeed > 256)
			{
				if (g_tickState[4])
				{
					Nu3D::Particles::SpawnFromPreset(
						actor->pos.x + ((3 * Numerics::g_sinCosLUT[(actor->yawAngle + 1952) & 0xFFF]) >> 2),
						actor->pos.y - 4096,
						actor->pos.z + ((3 * Numerics::g_sinCosLUT[(actor->yawAngle - 1120) & 0xFFF]) >> 2),
						39,
						2);
					Nu3D::Particles::SpawnFromPreset(
						actor->pos.x + ((3 * Numerics::g_sinCosLUT[(actor->yawAngle - 1952) & 0xFFF]) >> 2),
						actor->pos.y - 4096,
						actor->pos.z + ((3 * Numerics::g_sinCosLUT[(actor->yawAngle - 928) & 0xFFF]) >> 2),
						39,
						2);
				}
			}
		}

		// FUNCTION: TOY2 0x00406620
		void ZPod(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			int32_t localStrafeSpeed = context->localStrafeSpeed;
			Toy2::Actor::Toy2Actor* actor = context->actor;
			WorldPos worldPos;
			WorldPos displacement;

			if (localStrafeSpeed <= 512)
			{
				if (localStrafeSpeed < -512)
					localStrafeSpeed = -512;
			}
			else
			{
				localStrafeSpeed = 512;
			}

			bool playerSideNegative = actor->unkVar29 < 0;
			actor->rollAngle -= (int32_t)Renderer::g_frameDelta * (localStrafeSpeed + actor->rollAngle) / 16;

			if (! playerSideNegative)
				AudioManager::PlaySoundEffect(91, &actor->pos);

			if ((context->targetFlags & 1) != 0)
			{
				actor->unkShort4 -= Renderer::g_frameDelta;

				if (actor->unkShort4 < 0)
				{
					int32_t angle = actor->yawAngle + 512;
					actor->unkShort4 = 360;

					if (*g_randDatBufferPtr++ < 0x80u)
						angle -= 1024;

					actor->velX = Numerics::g_sinCosLUT[angle] >> 4;
					actor->velForward = Numerics::g_sinCosLUT[(angle + 1024) & 0xFFF] >> 4;
				}

				int16_t timer = actor->unkShort4;

				if (timer <= 280 && timer >= 100)
				{
					worldPos.x = 0;
					worldPos.y = 0;
					worldPos.z = -100;
					Toy2::Actor::ResolveBoneAttachmentPos((Vector3I*)&worldPos, actor, 0);

					displacement.y = 96000;
					displacement.x = 4 * Numerics::g_sinCosLUT[actor->yawAngle & 0xFFF];
					displacement.z = 4 * Numerics::g_sinCosLUT[(actor->yawAngle + 1024) & 0xFFF];
					Nu3D::Collision::SweepAndSlide(&worldPos, &displacement, 0x8000, 0, 256);
					Renderer::Beam::QueueBeam(9, 32, 400, &worldPos, &displacement, 0, 0x80, 0);

					worldPos.x += displacement.x;
					worldPos.y += displacement.y;
					worldPos.z += displacement.z;
					AudioManager::PlaySoundEffect(92, (Vector3I*)&worldPos);

					int32_t i = 0;

					if (g_tickState[2])
					{
						do
						{
							Nu3D::Particles::SpawnFromPreset(worldPos.x, worldPos.y, worldPos.z, 4, 4);
							++i;
						} while (i < g_tickState[2]);
					}

					if (g_tickState[4])
						Nu3D::Particles::SpawnFromPreset(worldPos.x, worldPos.y, worldPos.z, 70, 2);

					if (g_tickState[10])
						Lighting::SpawnLight(worldPos.x, worldPos.y, worldPos.z, 49152, 16, -2);

					Renderer::LensFlare::RegisterLight(worldPos.x, worldPos.y, worldPos.z, 0, 128, 0, 32);

					if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, (Vector3I*)&worldPos, 25))
					{
						int16_t hitAngle =
							Nu3D::Math::CartesianToFixedAngle(g_buzzActor.posAngles.pos.x - worldPos.x, g_buzzActor.posAngles.pos.z - worldPos.z);
						Buzz::HandleDamage(hitAngle, 3);
					}
				}
			}
			else
			{
				actor->unkShort4 = 360;
			}
		}

		// FUNCTION: TOY2 0x004068E0
		void Box(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;

			if (actor->unkShort4)
			{
				bool nextActorPhaseIsZero = actor[1].actorPhase == 0;
				actor->unkShort4 = 0;

				if (nextActorPhaseIsZero)
				{
					actor[1].unkShort2 = 30;
					actor->unkShort1 = 0;
					actor->movementData = Actor::g_movementData[21] + 53;
				}
				else if (actor[2].actorPhase)
				{
					actor->unkShort1 = 0;
					actor->movementData = Actor::g_movementData[21] + 59;
				}
				else
				{
					actor[2].unkShort2 = 30;
					actor->unkShort1 = 0;
					actor->movementData = Actor::g_movementData[21] + 53;
				}
			}
		}

		// FUNCTION: TOY2 0x00406960
		void BPlane(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			Vector3I particlePos;

			if (actor->unkVar29 >= 0)
				AudioManager::PlaySoundEffect(93, &actor->pos);

			if ((actor->actorFlags & Actor::ACTOR_PLAYER_TRIGGER) != 0 && actor->actorPhase == 1)
				Actor::HandleDamage(actor, (actor->yawAngle - 2048) & 0xFFF, 1);

			if (actor->unkShort4)
			{
				actor->unkShort4 = 0;

				Toy2::Actor::Toy2Actor* actorBefore = actor - 1;

				if (actor[-1].creatureId != Actor::CREATURE_3_BOX)
					actorBefore = actor - 2;

				actor->boundary.y = actorBefore->pos.y - 20480;
				actor->pos.x = actorBefore->pos.x;
				actor->pos.y = actorBefore->pos.y;
				actor->pos.z = actorBefore->pos.z;
				actor->motionTargetPos.x = actorBefore->pos.x;
				actor->motionTargetPos.y = actorBefore->pos.y - 20480;
				actor->motionTargetPos.z = actorBefore->pos.z;
			}

			if (g_tickState[8])
			{
				if ((actor->actorFlags & 1) != 0)
				{
					particlePos.x = 0;
					particlePos.y = -200;
					particlePos.z = 400;
					Actor::ResolveBoneAttachmentPos(&particlePos, actor, 0);
					Nu3D::Particles::SpawnFromPreset(particlePos.x, particlePos.y, particlePos.z, 88, 3);
				}
			}
		}

		// FUNCTION: TOY2 0x00406A60
		void RCCar(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			if (g_levelFileIndex == 1)
				RCCar_Level1(context);

			if (g_levelFileIndex == 2)
				RCCar_Level2(context);
		}

		// FUNCTION: TOY2 0x00406A90
		void FatBloke(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			WorldPos position;
			WorldPos displacement;

			if (actor->unkShort4)
			{
				int16_t yawAngle = actor->yawAngle;
				int32_t offsetX = Numerics::g_sinCosLUT[(yawAngle + 224) & 0xFFF];
				int32_t offsetZ = Numerics::g_sinCosLUT[(yawAngle + 1248) & 0xFFF];

				position.x = actor->pos.x;
				position.y = actor->pos.y - 12288;
				position.z = actor->pos.z;

				int32_t angle =
					Nu3D::Math::CartesianToFixedAngle(g_buzzActor.posAngles.pos.x - position.x - offsetX, g_buzzActor.posAngles.pos.z - position.z - offsetZ)
					& 0xFFF;

				if (((angle - actor->yawAngle + 256) & 0xFFFu) > 0x200)
					angle = actor->yawAngle;

				int32_t dirX = Numerics::g_sinCosLUT[angle] >> 3;
				int32_t dirZ = Numerics::g_sinCosLUT[(angle + 1024) & 0xFFF] >> 3;

				displacement.x = 80 * dirX;
				displacement.y = 0;
				displacement.z = 80 * dirZ;
				Nu3D::Collision::SweepAndSlide(&position, &displacement, 0x8000, 0, 256);

				int32_t travelled;

				if (abs(dirX) <= abs(dirZ))
					travelled = displacement.z / dirZ;
				else
					travelled = displacement.x / dirX;

				Nu3D::Particles::ParticleInstance* particle =
					Nu3D::Particles::SpawnInstance(offsetX + position.x, position.y, offsetZ + position.z, dirX, 0, dirZ, 0, 0, 0, 97);

				int32_t lifetime = 2 * abs(travelled) - 15;

				if (lifetime <= 0)
					lifetime = 1;

				particle->lifetime = lifetime;
				AudioManager::PlaySoundEffect(86, &actor->pos);

				if ((actor->actorFlags & 1) != 0)
				{
					int32_t count = 5;

					do
					{
						Nu3D::Particles::SpawnFromPreset(position.x, position.y, position.z, 100, 15)->rotSpeed = *g_randDatBufferPtr - 128;
						--count;
						++g_randDatBufferPtr;
					} while (count);
				}

				actor->unkShort4 = 0;
			}
		}

		// FUNCTION: TOY2 0x00406C70
		void Buzzard(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			int16_t timer = actor->unkShort4;

			if (timer != actor->actorPhase)
			{
				if (timer)
					AudioManager::PlaySoundEffect(88, &actor->pos);

				actor->unkShort4 = actor->actorPhase;
			}

			AudioManager::PlaySoundEffect(87, &actor->pos);
			actor->primaryAnimIdx = (context->targetFlags & 1) != 0;
		}

		// FUNCTION: TOY2 0x00416A60
		void Sheep(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;

			if ((actor->actorFlags & Toy2::Actor::ACTOR_PLAYER_TRIGGER) != 0)
			{
				++g_collectionCount;
				Particles::SpawnCollectSparkle(actor->pos.x, actor->pos.y - 0x2000, actor->pos.z, 50);
				AudioManager::PlaySoundEffect(32, &actor->pos);
				Actor::Kill(actor, 2);
			}
		}

		// FUNCTION: TOY2 0x00416AB0
		void TinMan(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			int32_t flash = (AndysHouse::g_tinRobotFlash - 1) & 1;
			bool timerExpired = AndysHouse::g_tinRobotFlashTimer - (int32_t)Renderer::g_frameDelta < 0;
			int32_t insideBounds;
			int32_t pitch;
			int16_t* unkVar34;
			int32_t spawnX;
			int32_t spawnY;
			int32_t spawnZ;
			int32_t count;

			AndysHouse::g_tinRobotFlash = flash;
			AndysHouse::g_tinRobotFlashTimer -= Renderer::g_frameDelta;

			if (timerExpired)
			{
				AndysHouse::g_tinRobotFlashTimer = 0;
				actor->useTint = 0;
			}
			else if (flash)
			{
				actor->useTint = 1;
				actor->actorTint.r = 0x2000;
				actor->actorTint.g = 0x2000;
				actor->actorTint.b = 0x2000;
			}
			else
			{
				actor->useTint = 0;
			}

			actor->creatureRam->speedNoTarget = 12 * ((uint8_t)actor->actorPhase + 14);

			int16_t primaryAnimIdx = actor->primaryAnimIdx;

			if (primaryAnimIdx == 3 || primaryAnimIdx == 5)
			{
				*(uint8_t*)&actor->creatureRam->defenseMode = 7;

				if (actor->actorPhase != AndysHouse::g_tinRobotLastPhase)
				{
					AndysHouse::g_tinRobotFlashTimer = 60;

					if (actor->actorPhase >= 10)
					{
						actor->movementData = Actor::g_movementData[5] + 90;
					}
					else
					{
						count = 5;
						actor->movementData = Actor::g_movementData[5] + 101;
						*(uint8_t*)&actor->creatureRam->defenseMode = 4;
						unkVar34 = (int16_t*)actor->unkVar34;
						actor->actorFlags &= ~0x100;
						spawnX = actor->pos.x + unkVar34[0];
						spawnY = actor->pos.y + unkVar34[1];
						spawnZ = actor->pos.z + unkVar34[2];

						do
						{
							Nu3D::Particles::SpawnFromPreset(spawnX, spawnY, spawnZ, 35, 14)->rotSpeed = *g_randDatBufferPtr - 128;
							++g_randDatBufferPtr;
							--count;
						} while (count);

						Lighting::SpawnLight(spawnX, spawnY, spawnZ, 0xF08000, 32, (int32_t)actor);
						AudioManager::PlaySoundEffect(-2, &actor->pos);
						AndysHouse::g_inTinRobotFight = 3;
					}

					AndysHouse::g_tinRobotLastPhase = actor->actorPhase;
					actor->unkShort1 = 0;
				}
			}
			else
			{
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
			}

			if ((context->targetFlags & 1) != 0)
			{
				insideBounds = 1;
			}
			else
			{
				RawLoader::CreatureListRam* creatureRam = actor->creatureRam;
				int32_t sinValue = Numerics::g_sinCosLUT[8 * creatureRam->boundAngle] >> 2;
				int32_t cosValue = Numerics::g_sinCosLUT[(8 * creatureRam->boundAngle + 1024) & 0xFFF] >> 2;
				int32_t deltaX = (g_buzzActor.posAngles.pos.x - actor->boundary.x) >> 5;
				int32_t deltaZ = (g_buzzActor.posAngles.pos.z - actor->boundary.z) >> 5;

				if ((creatureRam->boundHalfX << 8) + ((deltaX * cosValue - deltaZ * sinValue) >> 7) >= (creatureRam->boundHalfX << 9)
					|| (insideBounds = 1, (creatureRam->boundHalfZ << 8) + ((deltaX * sinValue + deltaZ * cosValue) >> 7) >= (creatureRam->boundHalfZ << 9)))
				{
					insideBounds = 0;
				}
			}

			if (actor->primaryAnimIdx == 7)
			{
				if ((actor->unkVar7 & 0xFFFF0000) >= 851968 && ! Collectables::g_pickupSlots[4].state)
					Collectables::Activate(4, 0);

				int32_t yawAngle = actor->yawAngle;
				int32_t trailX = actor->pos.x + 2 * Numerics::g_sinCosLUT[yawAngle] / 3;
				int32_t trailY = actor->pos.y - 20480;
				int32_t trailZ = actor->pos.z + 2 * Numerics::g_sinCosLUT[(yawAngle + 1024) & 0xFFF] / 3;

				if (g_tickState[7])
				{
					Nu3D::Particles::SpawnFromPreset(trailX, trailY, trailZ, 17, 10)->rotSpeed = *g_randDatBufferPtr - 128;
					++g_randDatBufferPtr;
				}

				char randomBits = *g_randDatBufferPtr++ & 3;

				if (randomBits && (actor->unkVar7 & 0xFFFF0000) == 0xE0000)
				{
					Nu3D::Particles::SpawnFromPreset(trailX, trailY, trailZ, 4, 4)->lifetime = 2 * (*g_randDatBufferPtr & 0xF) + 24;
					++g_randDatBufferPtr;
				}
			}

			if (! insideBounds || actor->primaryAnimIdx == 7)
			{
				if (actor->primaryAnimIdx == 2)
				{
					actor->unkShort1 = 0;
					actor->movementData = Actor::g_movementData[5] + 10;
				}
			}
			else if ((uint32_t)g_buzzActor.posAngles.pos.y > 0xFFFCDEC6u)
			{
				if (AndysHouse::g_inTinRobotFight)
				{
					if (AndysHouse::g_inTinRobotFight == 2)
						Camera::g_unk50A118 = actor->pos;
				}
				else if (g_buzzActor.unkShort40 && g_buzzActor.posAngles.pos.y < -179566)
				{
					Dialogue::Begin(8, 26, "ha ha ha ha ... defeat the ^tin robot^ to get a ^token^!", 3619, 1792, -1);
					AndysHouse::g_inTinRobotFight = 1;
				}
			}

			g_unk52B7E0 = 54 * (actor->actorPhase - 9) / 11;

			if (context->localForwardSpeed <= 0)
			{
				pitch = AudioManager::g_pitchArray[1];

				if (AudioManager::g_pitchArray[1] <= 512)
					goto LABEL_46;

				pitch = (int16_t)(-32 * Renderer::g_frameDelta + AudioManager::g_pitchArray[1]);
			}
			else
			{
				pitch = (int16_t)(2 * context->localForwardSpeed + 2048);
			}

			AudioManager::g_pitchArray[1] = pitch;

		LABEL_46:
			if (actor->actorPhase >= 10 && pitch > 512)
				AudioManager::PlaySoundEffect(53, &actor->pos);
		}

		// FUNCTION: TOY2 0x00416F30
		void RCCar_Level1(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			int32_t state = 0;
			Toy2::Actor::Toy2Actor* actor = context->actor;
			int32_t animIdx = 2;

			actor->actorFlags |= Actor::ACTOR_UNUSED_1000;

			if (context->localForwardSpeed > 0)
			{
				AudioManager::g_pitchArray[2] = 4 * context->localForwardSpeed + 2048;
				AudioManager::PlaySoundEffect(55, &actor->pos);
			}

			if (g_unk52F2F8 >= 2)
			{
				if (context->localForwardSpeed < 8 * actor->creatureRam->speedTarget)
				{
					state = 1;
					animIdx = 11;
				}

				int16_t localStrafeSpeed = context->localStrafeSpeed;

				if (abs(localStrafeSpeed) > context->localForwardSpeed)
				{
					state = 3;

					if (localStrafeSpeed < 0)
						state = 2;

					animIdx = 12;
				}

				int16_t primaryAnimIdx = actor->primaryAnimIdx;

				if (state != primaryAnimIdx && (state || primaryAnimIdx != 1 || (actor->unkVar7 & 0xFFFF0000) >= 917504))
					Actor::SetAnimation(actor, state, animIdx);
			}

			if ((actor->actorFlags & Actor::ACTOR_CUTSCENE_MOVE) != 0)
			{
				Actor::UpdatePrimaryAnimation(actor);

				// $TODO: the original multiplies by a register whose upper half was never cleared, only the low 16 bits are used here
				int32_t forwardSpeed = context->localForwardSpeed;

				if (forwardSpeed > 0)
				{
					g_unk50A548 = (g_unk50A548 + ((Renderer::g_frameDelta * forwardSpeed) >> 3)) & 0xFFF;

					RCCar_Method1(actor, 0, g_unk50A548, 0, 0);
					RCCar_Method1(actor, 1, g_unk50A548, 0, 0);

					int32_t wheelAngle;

					if (state == 1 || abs(context->localStrafeSpeed) > 0x200)
						wheelAngle = 160 * Renderer::g_frameDelta + g_unk50A544;
					else
						wheelAngle = ((Renderer::g_frameDelta * context->localForwardSpeed) >> 3) + g_unk50A544;

					g_unk50A544 = wheelAngle & 0xFFF;

					RCCar_Method1(actor, 2, wheelAngle & 0xFFF, 0, 0);
					RCCar_Method1(actor, 3, g_unk50A544, 0, 0);
				}

				if (g_tickState[4] && (state == 1 || abs(context->localStrafeSpeed) > 0x200))
				{
					Nu3D::Particles::SpawnFromPreset(
						(Numerics::g_sinCosLUT[(actor->yawAngle + 1664) & 0xFFF] >> 1) + actor->pos.x,
						actor->pos.y - 2048,
						actor->pos.z + (Numerics::g_sinCosLUT[(actor->yawAngle - 1408) & 0xFFF] >> 1),
						42,
						10)
						->rotSpeed = *g_randDatBufferPtr - 128;
					++g_randDatBufferPtr;

					Nu3D::Particles::SpawnFromPreset(
						(Numerics::g_sinCosLUT[(actor->yawAngle - 1664) & 0xFFF] >> 1) + actor->pos.x,
						actor->pos.y - 2048,
						actor->pos.z + (Numerics::g_sinCosLUT[(actor->yawAngle - 640) & 0xFFF] >> 1),
						42,
						10)
						->rotSpeed = *g_randDatBufferPtr - 128;
					++g_randDatBufferPtr;

					AudioManager::PlaySoundEffect(54, &actor->pos);
				}
			}
		}

		// FUNCTION: TOY2 0x00418610
		void Army(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;

			if (actor->actorPhase == 102)
			{
				if (g_tickState[10])
				{
					if (! (*g_randDatBufferPtr++ & 3))
						AudioManager::PlaySoundEffect(72, &actor->pos);
				}

				if ((actor->actorFlags & Actor::ACTOR_PLAYER_TRIGGER) != 0)
				{
					++g_collectionCount;
					Particles::SpawnCollectSparkle(actor->pos.x, actor->pos.y - 0x2000, actor->pos.z, 50);
					Actor::Kill(actor, 2);
					AudioManager::PlaySoundEffect(73, &actor->pos);
				}

				actor->unkShort4 -= Renderer::g_frameDelta;

				if (actor->unkShort4 <= 0)
				{
					actor->unkShort4 = (*g_randDatBufferPtr & 0x7F) + 128;
					++g_randDatBufferPtr;

					if ((actor->actorFlags & Actor::ACTOR_CUTSCENE_MOVE) != 0)
					{
						int32_t velocity = *g_randDatBufferPtr - 128;
						g_randDatBufferPtr += 3;

						Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnInstance(
							actor->pos.x, actor->pos.y - 4096, actor->pos.z, 4 * velocity, -3072, velocity, 96, 0, 4 * velocity, 121);

						AudioManager::PlaySoundEffect(96, &particle->pos);
					}
				}
			}
		}

		// FUNCTION: TOY2 0x00418720
		void RCCar_Level2(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			int32_t state = 0;
			Toy2::Actor::Toy2Actor* actor = context->actor;
			int32_t animIdx = 2;

			actor->actorFlags |= Actor::ACTOR_UNUSED_1000;

			if (context->localForwardSpeed > 0)
			{
				AudioManager::g_pitchArray[2] = 4 * context->localForwardSpeed + 2048;
				AudioManager::PlaySoundEffect(55, &actor->pos);
			}

			if (g_unk52F2F8 >= 2)
			{
				if (context->localForwardSpeed < 8 * actor->creatureRam->speedTarget)
				{
					state = 1;
					animIdx = 11;
				}

				int16_t localStrafeSpeed = context->localStrafeSpeed;

				if (abs(localStrafeSpeed) > context->localForwardSpeed)
				{
					state = 3;

					if (localStrafeSpeed < 0)
						state = 2;

					animIdx = 12;
				}

				int16_t primaryAnimIdx = actor->primaryAnimIdx;

				if (state != primaryAnimIdx && (state || primaryAnimIdx != 1 || (actor->unkVar7 & 0xFFFF0000) >= 917504))
					Actor::SetAnimation(actor, state, animIdx);
			}

			if ((actor->actorFlags & Actor::ACTOR_CUTSCENE_MOVE) != 0)
			{
				Actor::UpdatePrimaryAnimation(actor);

				// $TODO: the original multiplies by a register whose upper half was never cleared, only the low 16 bits are used here
				int32_t forwardSpeed = context->localForwardSpeed;

				if (forwardSpeed > 0)
				{
					g_unk50A548 = (g_unk50A548 + ((Renderer::g_frameDelta * forwardSpeed) >> 3)) & 0xFFF;

					RCCar_Method1(actor, 0, g_unk50A548, 0, 0);
					RCCar_Method1(actor, 1, g_unk50A548, 0, 0);

					int32_t wheelAngle;

					if (state == 1 || abs(context->localStrafeSpeed) > 0x200)
						wheelAngle = 160 * Renderer::g_frameDelta + g_unk50A544;
					else
						wheelAngle = ((Renderer::g_frameDelta * context->localForwardSpeed) >> 3) + g_unk50A544;

					g_unk50A544 = wheelAngle & 0xFFF;

					RCCar_Method1(actor, 2, wheelAngle & 0xFFF, 0, 0);
					RCCar_Method1(actor, 3, g_unk50A544, 0, 0);
				}

				if (g_tickState[4] && (state == 1 || abs(context->localStrafeSpeed) > 0x200))
				{
					Nu3D::Particles::SpawnFromPreset(
						(Numerics::g_sinCosLUT[(actor->yawAngle + 1664) & 0xFFF] >> 1) + actor->pos.x,
						actor->pos.y - 2048,
						actor->pos.z + (Numerics::g_sinCosLUT[(actor->yawAngle - 1408) & 0xFFF] >> 1),
						42,
						10)
						->rotSpeed = *g_randDatBufferPtr - 128;
					++g_randDatBufferPtr;

					Nu3D::Particles::SpawnFromPreset(
						(Numerics::g_sinCosLUT[(actor->yawAngle - 1664) & 0xFFF] >> 1) + actor->pos.x,
						actor->pos.y - 2048,
						actor->pos.z + (Numerics::g_sinCosLUT[(actor->yawAngle - 640) & 0xFFF] >> 1),
						42,
						10)
						->rotSpeed = *g_randDatBufferPtr - 128;
					++g_randDatBufferPtr;

					AudioManager::PlaySoundEffect(54, &actor->pos);
				}
			}
		}

		// FUNCTION: TOY2 0x004189C0
		void ZKite(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			int32_t flashToggle = (AndysNeighborhood::g_unk52F63C - 1) & 1;
			Toy2::Actor::Toy2Actor* actor = context->actor;
			bool timerExpired = AndysNeighborhood::g_zKiteFlashTimer - (int32_t)Renderer::g_frameDelta < 0;
			int32_t buzzYPos;
			int32_t forceReturn;
			int16_t yawSpinSpeed;
			int32_t targetYaw;
			int32_t yawAngle;
			int32_t vel;

			AndysNeighborhood::g_unk52F63C = flashToggle;
			AndysNeighborhood::g_zKiteFlashTimer -= Renderer::g_frameDelta;

			if (timerExpired)
			{
				AndysNeighborhood::g_zKiteFlashTimer = 0;
			LABEL_3:
				actor->useTint = 0;
				goto LABEL_4;
			}

			if (! flashToggle)
				goto LABEL_3;

			actor->useTint = 1;
			actor->actorTint.r = 0x2000;
			actor->actorTint.g = 0x2000;
			actor->actorTint.b = 0x2000;
		LABEL_4:
			AndysNeighborhood::g_zKiteRollPhase = (AndysNeighborhood::g_zKiteRollPhase + 36 * Renderer::g_frameDelta) & 0xFFF;
			actor->rollAngle = Numerics::g_sinCosLUT[AndysNeighborhood::g_zKiteRollPhase] >> 6;

			AndysNeighborhood::g_zKiteYawPhase = (AndysNeighborhood::g_zKiteYawPhase + (Renderer::g_frameDelta << 6)) & 0xFFF;
			actor->targetYaw =
				(Nu3D::Math::CartesianToFixedAngle(actor->pos.x - g_buzzActor.posAngles.pos.x, actor->pos.z - g_buzzActor.posAngles.pos.z) - 2048) & 0xFFF;

			if (AndysNeighborhood::g_unk52F664 == 2)
			{
				buzzYPos = g_buzzActor.posAngles.pos.y;

				if (g_buzzActor.posAngles.pos.y <= -466944)
				{
					g_hudWidgetTimers[7] = 90;
					Camera::g_unk50A118 = actor->pos;
					forceReturn = 0;
				}
				else
				{
					forceReturn = 1;
					actor->actorFlags = (actor->actorFlags & 0xFFF3) | Actor::ACTOR_LOCKED_MOVE;
					actor->motionTargetPos.x = actor->boundary.x;
					actor->motionTargetPos.z = actor->boundary.z;

					*(uint8_t*)&actor->creatureRam->defenseMode = 4;
					actor->unkShort1 = 50;
				}

				if (buzzYPos > -467373)
					buzzYPos = -467373;

				if ((context->targetFlags & 1) != 0 || forceReturn)
				{
					if (AndysNeighborhood::g_zKitePhase != actor->actorPhase)
					{
						AndysNeighborhood::g_zKiteFlashTimer = 60;
						AndysNeighborhood::g_zKitePhase = actor->actorPhase;
						actor->unkShort1 = 0;

						if (AndysNeighborhood::g_zKitePhase > 0)
							AudioManager::PlaySoundEffect(152, &actor->pos);
					}

					int32_t yawAccum = actor->yawAngle;
					actor->motionTargetPos.y = Numerics::g_sinCosLUT[AndysNeighborhood::g_zKiteYawPhase] + buzzYPos - 40960;
					AndysNeighborhood::g_unk52F65C = yawAccum;
					actor->unkShort4 -= Renderer::g_frameDelta;

					if (actor->unkShort4 < 0)
					{
						yawAngle = actor->yawAngle;
						actor->unkShort4 = 120;
						vel = yawAngle + 512;

						if (*g_randDatBufferPtr++ < 0x80u)
							vel -= 1024;

						actor->velX = Numerics::g_sinCosLUT[vel] >> 3;
						actor->velForward = Numerics::g_sinCosLUT[(vel + 1024) & 0xFFF] >> 3;
					}
				}
				else
				{
					actor->unkShort4 = 2;
					actor->motionTargetPos.x = g_buzzActor.posAngles.pos.x;
					actor->motionTargetPos.y = Numerics::g_sinCosLUT[AndysNeighborhood::g_zKiteYawPhase] + buzzYPos - 12288;
					actor->motionTargetPos.z = g_buzzActor.posAngles.pos.z;

					if (actor->motionTargetPos.y > -475565)
						actor->motionTargetPos.y = -475565;

					yawSpinSpeed = 264 - 20 * actor->actorPhase;
					AudioManager::g_pitchArray[0] = 16 * yawSpinSpeed;
					AudioManager::PlaySoundEffect(60, &actor->pos);

					AndysNeighborhood::g_unk52F65C = (AndysNeighborhood::g_unk52F65C + yawSpinSpeed * Renderer::g_frameDelta) & 0xFFF;

					actor->yawAngle = AndysNeighborhood::g_unk52F65C;

					targetYaw = actor->targetYaw;
					actor->velX = Numerics::g_sinCosLUT[targetYaw] >> 5;
					actor->velForward = Numerics::g_sinCosLUT[(targetYaw + 1024) & 0xFFF] >> 5;
				}
			}
			else
			{
				actor->pos.y = (Numerics::g_sinCosLUT[AndysNeighborhood::g_zKiteYawPhase] >> 3) - 507904;
			}

			if (actor->pos.y > -475565)
				actor->pos.y = -475565;
		}

		// FUNCTION: TOY2 0x00418CE0
		void LawnMower(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			int32_t index;
			uint8_t* rand;
			int32_t angle;
			int32_t randByte;
			int32_t spawnX;
			int32_t spawnZ;

			AudioManager::PlaySoundEffect(75, &actor->pos);

			if ((actor->actorFlags & Actor::ACTOR_CUTSCENE_MOVE) != 0)
			{
				if (g_tickState[9])
				{
					Nu3D::Particles::SpawnFromPreset(actor->pos.x, actor->pos.y, actor->pos.z, 49, 2)->groundAlignRot = 16 * *g_randDatBufferPtr;
					++g_randDatBufferPtr;
				}

				int16_t yawAngle = actor->yawAngle;

				index = 0;
				spawnX = actor->pos.x + Numerics::g_sinCosLUT[(yawAngle - 2048) & 0xFFF];
				spawnZ = actor->pos.z + Numerics::g_sinCosLUT[(yawAngle - 1024) & 0xFFF];

				if (g_tickState[2])
				{
					rand = g_randDatBufferPtr;

					do
					{
						angle = (actor->yawAngle + 6 * *rand + 1280) & 0xFFF;

						g_randDatBufferPtr = rand + 1;
						randByte = rand[1];
						g_randDatBufferPtr = rand + 4;

						Nu3D::Particles::SpawnInstance(
							spawnX,
							actor->pos.y,
							spawnZ,
							Numerics::g_sinCosLUT[angle] >> 4,
							randByte - 3072,
							Numerics::g_sinCosLUT[(angle + 1024) & 0xFFF] >> 4,
							256,
							16 * randByte,
							2 * randByte - 256,
							50)
							->colourG = (rand[4] >> 2) + 64;

						rand = g_randDatBufferPtr + 1;

						++index;
						++g_randDatBufferPtr;
					} while (index < g_tickState[2]);
				}
			}
		}

		// FUNCTION: TOY2 0x0041B780
		void Drill(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			Toy2::Actor::Toy2Actor* v1 = actor;
			int32_t x;
			int32_t cell;
			int32_t* v5;
			int32_t* v6;
			int32_t v7;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			int32_t spawnX;
			int32_t spawnY;
			int32_t spawnZ;
			int32_t v16;
			int32_t v17;
			int32_t v18;
			int16_t v19;
			int32_t v20;
			int32_t v21;
			int32_t actorZ;
			Nu3D::Particles::ParticleInstance* particle;

			ConstructionYard::g_unk52F798 = (ConstructionYard::g_unk52F798 - 1) & 1;
			AudioManager::PlaySoundEffect(153, &actor->pos);

			if ((actor->actorFlags & 0x200) != 0)
			{
				actor->actorFlags &= 0xFDFF;
				AudioManager::PlaySoundEffect(154, &actor->pos);
			}

			if (Camera::g_unk50A510 || ! Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &actor->pos, 300))
			{
				if (Camera::g_unk50A510 < 20 && Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &actor->pos, 100))
					Camera::g_unk50A510 = 40;
			}
			else
			{
				Camera::g_unk50A510 = 20;
			}

			if (ConstructionYard::g_unk52F804 > 1)
			{
				ConstructionYard::g_unk52F864 -= Renderer::g_frameDelta;

				if (ConstructionYard::g_unk52F864 < 0)
				{
					ConstructionYard::g_unk52F864 = 0;
					*(uint8_t*)&actor->creatureRam->defenseMode = 7;
				LABEL_12:
					actor->useTint = 0;
					goto LABEL_13;
				}

				if (! ConstructionYard::g_unk52F798)
					goto LABEL_12;

				actor->useTint = 1;
				actor->actorTint.r = 0x2000;
				actor->actorTint.g = 0x2000;
				actor->actorTint.b = 0x2000;
			}
		LABEL_13:
			if (actor->actorPhase != ConstructionYard::g_unk52F884)
			{
				ConstructionYard::g_unk52F884 = actor->actorPhase;
				ConstructionYard::g_unk52F864 = 60;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
			}

			if (g_discLauncherAmmo == 6 || ConstructionYard::g_unk52F804 <= 1)
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
			else
				*(uint8_t*)&actor->creatureRam->defenseMode = 5;

			x = actor->pos.x;
			cell = 0;
			v5 = ConstructionYard::g_unk4F1CCC;

			do
			{
				if (x < *v5)
					break;

				++v5;
				++cell;
			} while (v5 < &ConstructionYard::g_unk4F1CCC[4]);

			v6 = ConstructionYard::g_unk4F1CDC;
			actorZ = actor->pos.z;

			do
			{
				if (actor->pos.z < *v6)
					break;

				++v6;
				cell += 16;
			} while (v6 < &ConstructionYard::g_unk4F1CDC[4]);

			if ((cell & 0x11) == 0x11)
			{
				if ((ConstructionYard::g_unk52F7CC & 1) == 0)
				{
					v7 = (cell & 3) - 1;
					v8 = ConstructionYard::g_unk4F1CEC[v7];

					if (x >= ConstructionYard::g_unk4F1CEC[v7 + 1] - (ConstructionYard::g_unk4F1CEC[v7 + 1] - v8) / 2)
						actor->pos.x = ConstructionYard::g_unk4F1CEC[v7 + 1];
					else
						actor->pos.x = v8;

					cell &= ~1u;
				}

				if ((ConstructionYard::g_unk52F7CC & 0x10) == 0)
				{
					v9 = ((cell >> 4) & 3) - 1;
					v10 = ConstructionYard::g_unk4F1CFC[v9];

					if (actorZ >= ConstructionYard::g_unk4F1CFC[v9 + 1] - (ConstructionYard::g_unk4F1CFC[v9 + 1] - v10) / 2)
						actor->pos.z = ConstructionYard::g_unk4F1CFC[v9 + 1];
					else
						actor->pos.z = v10;

					cell &= ~0x10u;
				}
			}

			ConstructionYard::g_unk52F7CC = cell;

			if ((uint32_t)actor->pos.y > 0xFFF63C20u)
				actor->pos.y = -639968;

			if (ConstructionYard::g_unk52F804 < 3 && g_tickState[4])
				Nu3D::Particles::SpawnFromPreset(actor->pos.x, actor->pos.y, actor->pos.z, 4, 9)->lifetime = 32;

			if (ConstructionYard::g_unk52F804 == 2 && g_buzzActor.posAngles.pos.y < -515621)
			{
				if (g_tickState[10])
				{
					spawnX = actor->pos.x;
					spawnY = actor->pos.y;
					spawnZ = actor->pos.z;

					char randomBits = *g_randDatBufferPtr & 3;
					++g_randDatBufferPtr;

					if (randomBits)
					{
						particle = Nu3D::Particles::SpawnFromPreset(spawnX, spawnY, spawnZ, 84, 14);
					}
					else
					{
						v16 = spawnY - g_buzzActor.posAngles.pos.y;
						v17 = (spawnZ - g_buzzActor.posAngles.pos.z) >> 5;
						v18 = (spawnX - g_buzzActor.posAngles.pos.x) >> 5;
						v19 = Nu3D::Math::CartesianToFixedAngle(v18, v17) & 0xFFF;

						// $TODO: IDA types this as __int64, the disassembly truncates the sqrt with __ftol
						v20 = (int32_t)sqrt((double)(v18 * v18 + v17 * v17));
						v21 = 3 * v20 / 4;

						particle = Nu3D::Particles::SpawnInstance(
							spawnX,
							spawnY,
							spawnZ,
							v21 * Numerics::g_sinCosLUT[(v19 - 2048) & 0xFFF] / 0x4000,
							-128 * ((v20 << 12) / v21) / 256 - (v16 << 7) / ((v20 << 12) / v21),
							v21 * Numerics::g_sinCosLUT[(v19 - 1024) & 0xFFF] / 0x4000,
							128,
							0,
							0,
							84);
						v1 = actor;
					}

					particle->rotSpeed = *g_randDatBufferPtr++ - 128;
					particle->width = 100;
					particle->height = 100;
				}

				g_unk52B7E0 = 54 * v1->actorPhase / 30;
			}
		}

		// FUNCTION: TOY2 0x0041BB80
		void LTyke(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;

			actor->unkShort4 -= Renderer::g_frameDelta;

			if (actor->unkShort4 <= 0)
			{
				actor->unkShort4 = (*g_randDatBufferPtr++ & 0x7F) + 90;
				AudioManager::PlaySoundEffect(107, &actor->pos);
			}

			if ((actor->actorFlags & 0x200) != 0 && actor->actorPhase == 102)
			{
				++g_collectionCount;
				Particles::SpawnCollectSparkle(actor->pos.x, actor->pos.y - 0x2000, actor->pos.z, 50);
				AudioManager::PlaySoundEffect(108, &actor->pos);
				Actor::Kill(actor, 2);
			}
		}

		// FUNCTION: TOY2 0x0041DDB0
		void Clown(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			int32_t flash = (AlleysAndGullies::g_unk52F8FC - 1) & 1;

			AlleysAndGullies::g_unk52F8FC = flash;

			if (actor->actorPhase != AlleysAndGullies::g_unk52F8B8)
			{
				AlleysAndGullies::g_unk52F8B8 = actor->actorPhase;
				AlleysAndGullies::g_unk52F8B0 = 60;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				flash = AlleysAndGullies::g_unk52F8FC;
			}

			if ((actor->actorFlags & 0x200) != 0)
			{
				actor->actorFlags &= 0xFDFF;
				AudioManager::PlaySoundEffect(161, &actor->pos);
				flash = AlleysAndGullies::g_unk52F8FC;
			}

			if (AlleysAndGullies::g_unk52F8E8 <= 1)
			{
				actor->useTint = 0;
			}
			else
			{
				AlleysAndGullies::g_unk52F8B0 -= Renderer::g_frameDelta;

				if (AlleysAndGullies::g_unk52F8B0 < 0)
				{
					AlleysAndGullies::g_unk52F8B0 = 0;
					*(uint8_t*)&actor->creatureRam->defenseMode = 7;
				LABEL_8:
					actor->useTint = 0;
					goto LABEL_12;
				}

				if (! flash)
					goto LABEL_8;

				actor->useTint = 1;
				actor->actorTint.r = 0x2000;
				actor->actorTint.g = 0x2000;
				actor->actorTint.b = 0x2000;
			}
		LABEL_12:
			if (AlleysAndGullies::g_unk52F8E8 == 2)
			{
				g_unk52B7E0 = 54 * actor->actorPhase / 20;

				if (g_curSectorCopy1 == 2)
					g_hudWidgetTimers[7] = 90;
			}
		}

		// FUNCTION: TOY2 0x0041DEC0
		void Ducks(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;

			actor->unkShort4 -= Renderer::g_frameDelta;

			if (actor->actorPhase == 102 && actor->unkShort4 <= 0)
			{
				actor->unkShort4 = (*g_randDatBufferPtr++ & 0x7F) + 60;
				AudioManager::PlaySoundEffect(162, &actor->pos);
			}

			if ((actor->actorFlags & 0x200) != 0 && actor->actorPhase == 102)
			{
				++g_collectionCount;
				Particles::SpawnCollectSparkle(actor->pos.x, actor->pos.y - 0x2000, actor->pos.z, 50);
				AudioManager::PlaySoundEffect(162, &actor->pos);
				Actor::Kill(actor, 2);
			}
		}

		// FUNCTION: TOY2 0x0041DF70
		void ZBoat(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			Vector3I spawnPos;

			actor->unkShort4 -= Renderer::g_frameDelta;

			if (actor->unkShort4 < 0)
			{
				actor->unkShort4 = 200;
				spawnPos.x = 0;
				spawnPos.y = -500;
				spawnPos.z = -300;
				Actor::ResolveBoneAttachmentPos(&spawnPos, actor, 0);
				Nu3D::Particles::SpawnInstance(
					spawnPos.x,
					spawnPos.y,
					spawnPos.z,
					Numerics::g_sinCosLUT[actor->yawAngle] >> 2,
					-3072,
					Numerics::g_sinCosLUT[(actor->yawAngle + 1024) & 0xFFF] >> 2,
					128,
					0,
					0,
					92);
			}
		}

		// FUNCTION: TOY2 0x00420AF0
		void Dino(Toy2::Actor::Toy2Actor::ActorBehaviourContext* l_actor)
		{
			Toy2::Actor::Toy2Actor* actor = l_actor->actor;
			int16_t yawAngle;
			int32_t angle;
			int32_t velX;
			int32_t velZ;
			int32_t randValue;
			Nu3D::Particles::ParticleInstance* particle;
			int16_t* unkVar34;
			int32_t spawnX;
			int32_t spawnY;
			int32_t spawnZ;
			int32_t count;
			int32_t fireX;
			int32_t fireY;
			int32_t fireZ;
			Vector3I bonePos;

			AlsToyBarn::g_unk52FA20 = (AlsToyBarn::g_unk52FA20 - 1) & 1;

			if (actor->actorPhase != AlsToyBarn::g_unk52FA2C)
			{
				AlsToyBarn::g_unk52FA2C = actor->actorPhase;
				AlsToyBarn::g_unk52FA50 = 60;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				AudioManager::PlaySoundEffect(121, &actor->pos);
			}

			if (actor->unkShort4)
			{
				if (g_curSectorCopy1 == 4)
				{
					yawAngle = actor->yawAngle;
					randValue = *g_randDatBufferPtr;
					bonePos.x = 20;
					bonePos.y = -600;
					bonePos.z = 0;
					angle = (yawAngle + randValue - 128) & 0xFFF;
					++g_randDatBufferPtr;
					velX = Numerics::g_sinCosLUT[angle] >> 3;
					velZ = Numerics::g_sinCosLUT[(angle + 1024) & 0xFFF] >> 3;
					Actor::ResolveBoneAttachmentPos(&bonePos, actor, 1);
					AudioManager::PlaySoundEffect(120, &actor->pos);
					randValue = *g_randDatBufferPtr;
					g_randDatBufferPtr += 2;
					particle = Nu3D::Particles::SpawnInstance(bonePos.x, bonePos.y, bonePos.z, velX, randValue + 512, velZ, 0, 0, randValue - 128, 85);

					if (actor->unkShort4 == 30)
					{
						Camera::g_unk50A510 = 40;
						*(uint8_t*)&particle->renderFlags |= 2;
					}
				}

				actor->unkShort4 -= Renderer::g_frameDelta;

				if (actor->unkShort4 <= 0)
					actor->unkShort4 = 0;
			}

			if (AlsToyBarn::g_unk52FA14 > 1)
			{
				AlsToyBarn::g_unk52FA50 -= Renderer::g_frameDelta;

				if (AlsToyBarn::g_unk52FA50 >= 0)
				{
					if (AlsToyBarn::g_unk52FA20)
					{
						actor->useTint = 1;
						actor->actorTint.r = 0x2000;
						actor->actorTint.g = 0x2000;
						actor->actorTint.b = 0x2000;
						goto LABEL_14;
					}
				}
				else
				{
					AlsToyBarn::g_unk52FA50 = 0;

					if (AlsToyBarn::g_unk52FA14 == 2)
						*(uint8_t*)&actor->creatureRam->defenseMode = 7;
				}
			}

			actor->useTint = 0;
		LABEL_14:
			if (actor->actorPhase < 10 && AlsToyBarn::g_unk52FA14 == 2)
			{
				count = 5;
				actor->movementData = Actor::g_movementData[24] + 58;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				unkVar34 = (int16_t*)actor->unkVar34;
				actor->actorFlags &= ~0x100;
				spawnX = actor->pos.x + unkVar34[0];
				spawnY = actor->pos.y + unkVar34[1];
				spawnZ = actor->pos.z + unkVar34[2];

				do
				{
					Nu3D::Particles::SpawnFromPreset(spawnX, spawnY, spawnZ, 35, 14)->rotSpeed = *g_randDatBufferPtr - 128;
					++g_randDatBufferPtr;
					--count;
				} while (count);

				Lighting::SpawnLight(spawnX, spawnY, spawnZ, 15761408, 32, (int32_t)actor);
				AudioManager::PlaySoundEffect(-2, &actor->pos);
				AlsToyBarn::g_unk52FA14 = 3;
				g_unk52B7E0 = 0;
			}
			else if (AlsToyBarn::g_unk52FA14 < 3)
			{
				goto LABEL_31;
			}

			if (g_curSectorCopy1 == 4)
			{
				yawAngle = actor->yawAngle;
				velX = Numerics::g_sinCosLUT[(yawAngle - 1024) & 0xFFF] >> 2;
				velZ = Numerics::g_sinCosLUT[yawAngle & 0xFFF] >> 2;

				if (AlsToyBarn::g_unk52FA20)
				{
					fireX = velX + actor->pos.x;
					fireY = actor->pos.y - 7424;
					fireZ = velZ + actor->pos.z;
				}
				else
				{
					fireX = actor->pos.x - 3 * velX;
					fireY = actor->pos.y - 1024;
					fireZ = actor->pos.z - 3 * velZ;
				}

				if (g_tickState[7])
				{
					particle = Nu3D::Particles::SpawnFromPreset(fireX, fireY, fireZ, 17, 10);
					particle->rotSpeed = *g_randDatBufferPtr++ - 128;
					particle->width = 40;
					particle->height = 40;
				}

				char randomBits = *g_randDatBufferPtr++ & 3;

				if (randomBits && (actor->unkVar7 & 0xFFFF0000) == 0xA0000)
				{
					Nu3D::Particles::SpawnFromPreset(fireX, fireY, fireZ, 4, 4)->lifetime = 2 * (*g_randDatBufferPtr & 0xF) + 24;
					++g_randDatBufferPtr;
				}
			}
		LABEL_31:
			if (AlsToyBarn::g_unk52FA14 == 2)
			{
				g_unk52B7E0 = 54 * (actor->actorPhase - 9) / 20;

				if (g_curSectorCopy1 == 4)
					g_hudWidgetTimers[7] = 90;
			}
		}

		// FUNCTION: TOY2 0x00420ED0
		void Chick(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;

			actor->unkShort4 -= Renderer::g_frameDelta;

			if (actor->unkShort4 <= 0)
			{
				actor->unkShort4 = (*g_randDatBufferPtr++ & 0x7F) + 60;
				AudioManager::PlaySoundEffect(119, &actor->pos);
			}

			if ((actor->actorFlags & Actor::ACTOR_PLAYER_TRIGGER) != 0 && actor->actorPhase == 102)
			{
				++g_collectionCount;
				Particles::SpawnCollectSparkle(actor->pos.x, actor->pos.y - 0x2000, actor->pos.z, 50);
				AudioManager::PlaySoundEffect(119, &actor->pos);
				Actor::Kill(actor, 2);
			}
		}

		// FUNCTION: TOY2 0x00422660
		void BBuggy(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			int16_t localForwardSpeed = context->localForwardSpeed;
			int32_t flash;
			int16_t hitAngle;
			int16_t* unkVar34;
			int32_t spawnX;
			int32_t spawnY;
			int32_t spawnZ;
			int32_t count;
			int32_t wheelState;
			int16_t primaryAnimIdx;
			Nu3D::Particles::ParticleInstance* leftWheel;
			Nu3D::Particles::ParticleInstance* rightWheel;
			int32_t headOffset[4];
			WorldPos origin;

			if (localForwardSpeed > 0)
			{
				AudioManager::g_pitchArray[0] = 4 * localForwardSpeed + 2048;
				AudioManager::PlaySoundEffect(127, &actor->pos);
			}

			if (g_curSectorCopy2 != 5)
			{
				actor->motionTargetPos.x = actor->boundary.x;
				actor->motionTargetPos.y = actor->boundary.y;
				actor->motionTargetPos.z = actor->boundary.z;
			}

			flash = (AlsSpaceLand::g_unk52FA58 - 1) & 1;
			AlsSpaceLand::g_unk52FA58 = flash;

			if (actor->actorPhase != AlsSpaceLand::g_unk52FA88)
			{
				AlsSpaceLand::g_unk52FA88 = actor->actorPhase;
				AlsSpaceLand::g_unk52FA8C = 60;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				flash = AlsSpaceLand::g_unk52FA58;
			}

			if (AlsSpaceLand::g_unk52FAE0 <= 1)
				goto LABEL_11;

			AlsSpaceLand::g_unk52FA8C -= Renderer::g_frameDelta;

			if (AlsSpaceLand::g_unk52FA8C < 0)
			{
				AlsSpaceLand::g_unk52FA8C = 0;

				if (AlsSpaceLand::g_unk52FAE0 == 2)
					*(uint8_t*)&actor->creatureRam->defenseMode = 7;

				goto LABEL_11;
			}

			if (! flash)
			{
			LABEL_11:
				actor->useTint = 0;
				goto LABEL_12;
			}

			actor->useTint = 1;
			actor->actorTint.r = 0x2000;
			actor->actorTint.g = 0x2000;
			actor->actorTint.b = 0x2000;
		LABEL_12:
			if (AlsSpaceLand::g_unk52FAE0 == 2)
			{
				AlsSpaceLand::g_unk52FB9C -= Renderer::g_frameDelta;

				if (AlsSpaceLand::g_unk52FB9C >= 0)
				{
					if (AlsSpaceLand::g_unk52FB9C < 40)
					{
						if (g_tickState[8])
						{
							AudioManager::PlaySoundEffect(2, &actor->pos);
							memset(headOffset, 0, 12);
							origin.x = actor->pos.x + (Numerics::g_sinCosLUT[actor->yawAngle] >> 2);
							origin.y = actor->pos.y - 4096;
							origin.z = actor->pos.z + (Numerics::g_sinCosLUT[(actor->yawAngle + 1024) & 0xFFF] >> 2);
							SpawnBeamShot(0, actor->yawAngle, 0, &origin, (WorldPos*)headOffset, 4, 0);

							if (actor->pos.y - 4096 < g_buzzActor.posAngles.pos.y)
							{
								if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &actor->pos, 512))
								{
									hitAngle = Nu3D::Math::CartesianToFixedAngle(
										g_buzzActor.posAngles.pos.x - actor->pos.x, g_buzzActor.posAngles.pos.z - actor->pos.z);

									if (((hitAngle - actor->yawAngle + 32) & 0xFFFu) < 0x40)
									{
										Buzz::HandleDamage(hitAngle, 2);
										Lighting::SpawnLight(
											g_buzzActor.posAngles.pos.x,
											g_buzzActor.posAngles.pos.y,
											g_buzzActor.posAngles.pos.z,
											15728640,
											16,
											g_buzzActor.posAngles.pos.z);
									}
								}
							}
						}
					}
				}
				else
				{
					AlsSpaceLand::g_unk52FB9C = 240;
				}

				g_unk52B7E0 = 54 * (actor->actorPhase - 9) / 20;

				if (g_curSectorCopy2 == 5)
				{
					g_hudWidgetTimers[7] = 90;
					Camera::g_unk50A118 = actor->pos;
					AlsSpaceLand::g_unk52FB7C -= Renderer::g_frameDelta;

					if (AlsSpaceLand::g_unk52FB7C <= 0)
					{
						AlsSpaceLand::g_unk52FB7C = *g_randDatBufferPtr++ + 1800;
						AudioManager::Preset::PlayOneShotSound(212, (Toy2::Actor::Toy2Actor*)&g_buzzActor);
					}
				}

				actor->unkShort4 -= Renderer::g_frameDelta;

				if (actor->unkShort4 < 0)
				{
					AudioManager::PlaySoundEffect(76, &actor->pos);
					int32_t yawAngle = actor->yawAngle;
					int32_t actorZ = actor->pos.z;
					actor->unkShort4 = 400;
					Nu3D::Particles::SpawnInstance(
						actor->pos.x + (Numerics::g_sinCosLUT[(yawAngle - 2048) & 0xFFF] >> 2),
						actor->pos.y - 0x2000,
						actorZ + (Numerics::g_sinCosLUT[(yawAngle - 1024) & 0xFFF] >> 2),
						0,
						-2,
						0,
						4 * yawAngle,
						0,
						128,
						114)
						->pitchAngle = 1024;
				}
			}

			if (actor->actorPhase < 10 && AlsSpaceLand::g_unk52FAE0 == 2)
			{
				count = 5;
				actor->movementData = Actor::g_movementData[35] + 34;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				unkVar34 = (int16_t*)actor->unkVar34;
				actor->actorFlags &= ~0x100;
				spawnX = actor->pos.x + unkVar34[0];
				spawnY = actor->pos.y + unkVar34[1];
				spawnZ = actor->pos.z + unkVar34[2];

				do
				{
					Nu3D::Particles::SpawnFromPreset(spawnX, spawnY, spawnZ, 35, 14)->rotSpeed = *g_randDatBufferPtr - 128;
					++g_randDatBufferPtr;
					--count;
				} while (count);

				Lighting::SpawnLight(spawnX, spawnY, spawnZ, 15761408, 32, (int32_t)actor);
				AudioManager::PlaySoundEffect(-2, &actor->pos);
				AlsSpaceLand::g_unk52FAE0 = 3;
				g_unk52B7E0 = 0;
			}

			wheelState = 0;

			if (g_tickState[4])
			{
				primaryAnimIdx = actor->primaryAnimIdx;

				if (primaryAnimIdx == 1 && actor->unkVar7 < 0x80000)
					wheelState = 2;

				if (primaryAnimIdx || abs(context->localStrafeSpeed) <= 0x200)
				{
					if (! wheelState)
						goto LABEL_46;
				}
				else
				{
					wheelState = 1;
				}

				leftWheel = Nu3D::Particles::SpawnFromPreset(
					(Numerics::g_sinCosLUT[(actor->yawAngle + 1664) & 0xFFF] >> 1) + actor->pos.x,
					actor->pos.y - 2048,
					(Numerics::g_sinCosLUT[(actor->yawAngle - 1408) & 0xFFF] >> 1) + actor->pos.z,
					42,
					10);
				leftWheel->rotSpeed = *g_randDatBufferPtr++ - 128;

				if (wheelState == 2)
				{
					leftWheel->velX = -128;
					leftWheel->velZ = 256;
				}

				rightWheel = Nu3D::Particles::SpawnFromPreset(
					(Numerics::g_sinCosLUT[(actor->yawAngle - 1664) & 0xFFF] >> 1) + actor->pos.x,
					actor->pos.y - 2048,
					(Numerics::g_sinCosLUT[(actor->yawAngle - 640) & 0xFFF] >> 1) + actor->pos.z,
					42,
					10);
				rightWheel->rotSpeed = *g_randDatBufferPtr++ - 128;

				if (wheelState == 2)
				{
					rightWheel->velX = -128;
					rightWheel->velZ = 256;
				}

				AudioManager::PlaySoundEffect(54, &actor->pos);
			}
		LABEL_46:
			if (actor->primaryAnimIdx == 2)
			{
				if (g_tickState[7])
				{
					Nu3D::Particles::SpawnFromPreset(
						actor->pos.x - Numerics::g_sinCosLUT[actor->yawAngle] / 3,
						actor->pos.y - 0x2000,
						actor->pos.z - Numerics::g_sinCosLUT[(actor->yawAngle + 1024) & 0xFFF] / 3,
						17,
						10)
						->rotSpeed = *g_randDatBufferPtr - 128;
					++g_randDatBufferPtr;
				}
			}
		}

		// FUNCTION: TOY2 0x00422C70
		void Martian(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;

			actor->unkShort4 -= Renderer::g_frameDelta;

			if (actor->unkShort4 <= 0)
			{
				actor->unkShort4 = (*g_randDatBufferPtr++ & 0x7F) + 300;
				AudioManager::PlaySoundEffect(128, &actor->pos);
			}

			if ((actor->actorFlags & 0x200) != 0 && actor->actorPhase == 102)
			{
				++g_collectionCount;
				Particles::SpawnCollectSparkle(actor->pos.x, actor->pos.y - 0x2000, actor->pos.z, 50);
				AudioManager::PlaySoundEffect(184, &actor->pos);
				Actor::Kill(actor, 2);
			}
		}

		// FUNCTION: TOY2 0x00425700
		void GunsP(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			int32_t flash = (ElevatorHop::g_unk52FC20 - 1) & 1;
			int32_t shotIndex;
			int32_t rotSpeed;
			Vector3I spawnPos;

			ElevatorHop::g_unk52FC20 = flash;

			if (actor->actorPhase != ElevatorHop::g_unk52FCF8)
			{
				ElevatorHop::g_unk52FCF8 = actor->actorPhase;
				ElevatorHop::g_unk52FCE0 = 60;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				AudioManager::PlaySoundEffect(138, &actor->pos);
				flash = ElevatorHop::g_unk52FC20;
			}

			if (ElevatorHop::g_unk52FC4C > 1)
			{
				ElevatorHop::g_unk52FCE0 -= Renderer::g_frameDelta;

				if (ElevatorHop::g_unk52FCE0 >= 0)
				{
					if (flash)
					{
						actor->useTint = 1;
						actor->actorTint.r = 0x2000;
						actor->actorTint.g = 0x2000;
						actor->actorTint.b = 0x2000;
						goto LABEL_9;
					}
				}
				else
				{
					ElevatorHop::g_unk52FCE0 = 0;
					*(uint8_t*)&actor->creatureRam->defenseMode = 7;
				}
			}

			actor->useTint = 0;
		LABEL_9:
			if (ElevatorHop::g_unk52FC4C == 2)
			{
				ElevatorHop::g_unk52FC40 -= Renderer::g_frameDelta;

				if (ElevatorHop::g_unk52FC40 < 0)
				{
					shotIndex = ++ElevatorHop::g_unk52FC48;

					if (ElevatorHop::g_unk52FC48 >= 6)
					{
						shotIndex = 0;
						ElevatorHop::g_unk52FC48 = 0;
					}

					ElevatorHop::g_unk52FC40 = ElevatorHop::g_unk4F3780[shotIndex];

					if (g_buzzActor.posAngles.pos.y > -1844489)
					{
						spawnPos.x = 0;
						spawnPos.y = -500;
						spawnPos.z = -200;
						Actor::ResolveBoneAttachmentPos(&spawnPos, actor, 0);

						if (ElevatorHop::g_unk52FC48)
						{
							Nu3D::Particles::SpawnInstance(
								spawnPos.x,
								spawnPos.y,
								spawnPos.z,
								Numerics::g_sinCosLUT[actor->yawAngle] >> 2,
								-1024,
								Numerics::g_sinCosLUT[(actor->yawAngle + 1024) & 0xFFF] >> 2,
								64,
								0,
								0,
								119);
							AudioManager::PlaySoundEffect(137, &actor->pos);
						}
						else
						{
							rotSpeed = *g_randDatBufferPtr++ - 128;
							Nu3D::Particles::SpawnInstance(spawnPos.x, spawnPos.y, spawnPos.z, 0, -2, 0, 4 * actor->yawAngle, 0, rotSpeed, 89)->pitchAngle = 0;
							AudioManager::PlaySoundEffect(139, &actor->pos);
						}
					}
				}

				if (ElevatorHop::g_unk52FC34 > 1024 || Actor::IsInsideBounds(actor, -31232, 31232, -31232, 31232))
				{
					int32_t boundaryY = actor->boundary.y;
					actor->actorFlags |= 0x10;
					bool isAnim1 = actor->primaryAnimIdx == 1;
					actor->motionTargetPos.y = boundaryY - 24576;

					if (isAnim1)
					{
						AudioManager::PlaySoundEffect(136, &actor->pos);
						Actor::SetAnimation(actor, 2, 2);
					}
				}
				else
				{
					actor->actorFlags &= ~0x10;
					bool isAnim2 = actor->primaryAnimIdx == 2;
					actor->motionTargetPos.y = actor->boundary.y;

					if (isAnim2)
					{
						AudioManager::PlaySoundEffect(136, &actor->pos);
						Actor::SetAnimation(actor, 1, 1);
					}
				}

				g_unk52B7E0 = 54 * actor->actorPhase / 30;

				if (g_buzzActor.posAngles.pos.y <= -1844489)
				{
					*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				}
				else
				{
					g_hudWidgetTimers[7] = 90;

					if (! ElevatorHop::g_unk52FCE0)
						*(uint8_t*)&actor->creatureRam->defenseMode = 7;
				}
			}
		}

		// FUNCTION: TOY2 0x004259B0
		void Mouse(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;

			if (actor->actorPhase == 102)
			{
				actor->unkShort4 -= Renderer::g_frameDelta;

				if (actor->unkShort4 <= 0)
				{
					actor->unkShort4 = (*g_randDatBufferPtr++ & 0x7F) + 90;
					AudioManager::PlaySoundEffect(142, &actor->pos);
				}
			}

			if (actor->creatureRam->latSpeedNoTarget <= 0x7Fu)
			{
				actor->pitchAngle = 3072;
				actor->rollAngle = 2048;
			}

			if ((actor->actorFlags & 0x200) != 0 && actor->actorPhase == 102)
			{
				++g_collectionCount;
				Particles::SpawnCollectSparkle(actor->pos.x, actor->pos.y - 0x2000, actor->pos.z, 50);
				AudioManager::PlaySoundEffect(142, &actor->pos);
				Actor::Kill(actor, 2);
			}
		}

		// FUNCTION: TOY2 0x004282D0
		void GunsL_Level11(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			int32_t flash = (AlsPenthouse::g_unk52FD70 - 1) & 1;
			int32_t fired;
			int32_t yawAngle;
			int32_t count;
			Vector3I spawnPos;

			AlsPenthouse::g_unk52FD70 = flash;

			if (actor->actorPhase != AlsPenthouse::g_unk52FD2C)
			{
				AlsPenthouse::g_unk52FD2C = actor->actorPhase;
				AlsPenthouse::g_unk52FD10 = 60;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				AudioManager::PlaySoundEffect(165, &actor->pos);
				flash = AlsPenthouse::g_unk52FD70;
			}

			if (AlsPenthouse::g_unk52FD40 != 2)
				goto LABEL_8;

			AlsPenthouse::g_unk52FD10 -= Renderer::g_frameDelta;

			if (AlsPenthouse::g_unk52FD10 < 0)
			{
				AlsPenthouse::g_unk52FD10 = 0;
				*(uint8_t*)&actor->creatureRam->defenseMode = 7;
			LABEL_8:
				actor->useTint = 0;
				goto LABEL_9;
			}

			if (! flash)
				goto LABEL_8;

			actor->useTint = 1;
			actor->actorTint.r = 0x2000;
			actor->actorTint.g = 0x2000;
			actor->actorTint.b = 0x2000;
		LABEL_9:
			if (g_buzzActor.posAngles.pos.y < 113825 || g_buzzActor.posAngles.pos.y > 140293)
			{
				actor->motionTargetPos.x = actor->boundary.x;
				actor->motionTargetPos.z = actor->boundary.z;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
			}
			else if (! AlsPenthouse::g_unk52FD10)
			{
				*(uint8_t*)&actor->creatureRam->defenseMode = 7;
			}

			int16_t shotTimer = actor->unkShort4;

			if (shotTimer)
			{
				fired = 0;

				if (shotTimer > 20)
				{
					fired = 1;
					memset(&spawnPos, 0, sizeof(spawnPos));
					Actor::ResolveBoneAttachmentPos(&spawnPos, actor, 15);
					actor->unkShort4 = 10;
					AudioManager::PlaySoundEffect(147, &actor->pos);
				}

				actor->unkShort4 -= Renderer::g_frameDelta;

				if (actor->unkShort4 > 0)
				{
					if (! fired)
						goto LABEL_25;
				}
				else
				{
					memset(&spawnPos, 0, sizeof(spawnPos));
					Actor::ResolveBoneAttachmentPos(&spawnPos, actor, 16);
					actor->unkShort4 = 0;
					AudioManager::PlaySoundEffect(148, &actor->pos);
				}

				yawAngle = Nu3D::Math::CartesianToFixedAngle(g_buzzActor.posAngles.pos.x - spawnPos.x, g_buzzActor.posAngles.pos.z - spawnPos.z) & 0xFFF;

				if (((yawAngle - actor->yawAngle + 256) & 0xFFFu) > 0x200)
					yawAngle = actor->yawAngle;

				Nu3D::Particles::SpawnInstance(
					spawnPos.x,
					spawnPos.y,
					spawnPos.z,
					Numerics::g_sinCosLUT[yawAngle] >> 2,
					512,
					Numerics::g_sinCosLUT[(yawAngle + 1024) & 0xFFF] >> 2,
					0,
					0,
					0,
					97);

				if ((actor->actorFlags & 1) != 0)
				{
					count = 5;

					do
					{
						Nu3D::Particles::SpawnFromPreset(spawnPos.x, spawnPos.y, spawnPos.z, 100, 15)->rotSpeed = *g_randDatBufferPtr - 128;
						--count;
						++g_randDatBufferPtr;
					} while (count);
				}
			}
		LABEL_25:
			if (AlsPenthouse::g_unk52FD40 == 2)
			{
				g_unk52B7E0 = 54 * (actor->actorPhase - 9) / 20;

				if (g_curSectorCopy1 == 2 && g_buzzActor.posAngles.pos.y < 140293)
					g_hudWidgetTimers[7] = 90;
			}

			if (actor->actorPhase < 10 && AlsPenthouse::g_unk52FD40 == 2)
			{
				actor->movementData = Actor::g_movementData[33] + 52;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				actor->actorFlags &= ~0x100;
				AudioManager::PlaySoundEffect(-2, &actor->pos);
				AlsPenthouse::g_unk52FD40 = 3;
				g_unk52B7E0 = 0;
				actor->unkShort1 = 0;
			}

			if (AlsPenthouse::g_unk52FD40 >= 2)
			{
				if (actor->pos.z >= -374000)
				{
					actor->boundary.x = -689568;
					actor->boundary.z = -278464;
					actor->creatureRam->boundHalfX = 96;
					actor->creatureRam->boundHalfZ = 382;
				}
				else
				{
					actor->boundary.x = -643104;
					actor->boundary.z = -401472;
					actor->creatureRam->boundHalfX = 299;
					actor->creatureRam->boundHalfZ = 112;
				}
			}

			if (AlsPenthouse::g_unk52FD40 == 200)
			{
				actor->pos.x = actor->boundary.x;
				actor->pos.z = actor->boundary.z;
			}
		}

		// FUNCTION: TOY2 0x00428650
		void Rabid(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;

			actor->unkShort4 -= Renderer::g_frameDelta;

			if (actor->unkShort4 <= 0)
			{
				actor->unkShort4 = (*g_randDatBufferPtr++ & 0x7F) + 60;
				AudioManager::PlaySoundEffect(150, &actor->pos);
			}

			if ((actor->actorFlags & 0x200) != 0 && actor->actorPhase == 102)
			{
				++g_collectionCount;
				Particles::SpawnCollectSparkle(actor->pos.x, actor->pos.y - 0x2000, actor->pos.z, 50);
				AudioManager::PlaySoundEffect(150, &actor->pos);
				Actor::Kill(actor, 2);
			}
		}

		// FUNCTION: TOY2 0x0042BE60
		void ProsP_Level13(Toy2::Actor::Toy2Actor::ActorBehaviourContext* l_actor)
		{
			Toy2::Actor::Toy2Actor* actor = l_actor->actor;
			int32_t flash = (AirportInfiltration::g_unk52FF2C - 1) & 1;
			int32_t soundIndex;
			int32_t randValue;
			int32_t yawAngle;
			int32_t cosValue;

			AirportInfiltration::g_unk52FF2C = flash;

			if (actor->actorPhase != AirportInfiltration::g_unk52FF3C)
			{
				AirportInfiltration::g_unk52FF3C = actor->actorPhase;
				AirportInfiltration::g_unk52FF44 = 60;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				soundIndex = *g_randDatBufferPtr++ & 3;

				if (soundIndex == 3)
					soundIndex = 0;

				AudioManager::Preset::PlayOneShotSound2(soundIndex + 196, actor);
				flash = AirportInfiltration::g_unk52FF2C;
			}

			if (AirportInfiltration::g_unk52FF1C == 2)
			{
				AirportInfiltration::g_unk52FF30 -= Renderer::g_frameDelta;

				if (AirportInfiltration::g_unk52FF30 < 0)
				{
					randValue = *g_randDatBufferPtr++;
					AirportInfiltration::g_unk52FF30 = 2 * randValue + 400;
					AudioManager::Preset::PlayOneShotSound2(195, actor);
					flash = AirportInfiltration::g_unk52FF2C;
				}

				AirportInfiltration::g_unk52FF44 -= Renderer::g_frameDelta;

				if (AirportInfiltration::g_unk52FF44 >= 0)
				{
					if (flash)
					{
						actor->useTint = 1;
						actor->actorTint.r = 0x2000;
						actor->actorTint.g = 0x2000;
						actor->actorTint.b = 0x2000;
						goto LABEL_11;
					}
				}
				else
				{
					AirportInfiltration::g_unk52FF44 = 0;
					*(uint8_t*)&actor->creatureRam->defenseMode = 6;
				}
			}

			actor->useTint = 0;
		LABEL_11:
			if ((l_actor->targetFlags & 1) != 0)
			{
				if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &actor->pos, 200))
				{
					if (actor->primaryAnimIdx == 4)
					{
						((uint8_t*)&actor->areaIndex)[2] = 0xD0;
						((uint8_t*)&actor->areaIndex)[3] = 0xD0;
						Actor::SetAnimation(actor, 3, 9);
						actor->creatureRam->speedTarget = 0;
						AirportInfiltration::g_unk52FEE8 = 44;

						if (! AudioManager::IsActorSoundPlaying((Buzz::Toy2BuzzActor*)actor))
							AudioManager::Preset::PlayOneShotSound2(194, actor);
					}
				}
			}

			if (actor->primaryAnimIdx == 3 && (actor->unkVar7 & 0xFFFF0000) > 1376256)
			{
				actor->unkShort1 = 0;
				actor->movementData = Actor::g_movementData[41] + 16;
				actor->creatureRam->speedTarget = 16;
				actor->actorFlags &= 0xFFF3;
			}

			if (AirportInfiltration::g_unk52FEE8)
			{
				AirportInfiltration::g_unk52FEE8 -= Renderer::g_frameDelta;

				if (AirportInfiltration::g_unk52FEE8 <= 0)
				{
					yawAngle = actor->yawAngle;
					cosValue = Numerics::g_sinCosLUT[(yawAngle + 1024) & 0xFFF];
					Nu3D::Particles::SpawnInstance(
						actor->pos.x + (cosValue >> 3),
						actor->pos.y - 2048,
						actor->pos.z + (Numerics::g_sinCosLUT[(yawAngle - 2048) & 0xFFF] >> 3),
						Numerics::g_sinCosLUT[yawAngle] >> 2,
						0,
						cosValue >> 2,
						0,
						(2047 - yawAngle) & 0xFFF,
						0,
						104);
					AudioManager::PlaySoundEffect(166, &actor->pos);
					AirportInfiltration::g_unk52FEE8 = 0;
				}
			}

			if (AirportInfiltration::g_unk52FF1C == 2)
			{
				g_hudWidgetTimers[7] = 90;
				g_unk52B7E0 = 54 * (actor->actorPhase - 9) / 20;
			}

			if (actor->actorPhase < 10 && AirportInfiltration::g_unk52FF1C == 2)
			{
				actor->movementData = Actor::g_movementData[41] + 45;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				actor->actorFlags &= 0xFEF7;
				actor->unkShort1 = 0;
				AudioManager::PlaySoundEffect(-2, &actor->pos);
				AirportInfiltration::g_unk52FF1C = 3;
				g_unk52B7E0 = 0;
				AirportInfiltration::g_unk52FEE8 = 0;
				actor->creatureRam->speedTarget = 0;
			}
		}

		// FUNCTION: TOY2 0x0042C150
		void Pilot(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;

			actor->unkShort4 -= Renderer::g_frameDelta;

			if (actor->unkShort4 <= 0 && actor->actorPhase == 102)
			{
				actor->unkShort4 = (*g_randDatBufferPtr++ & 0x7F) + 90;
				AudioManager::PlaySoundEffect(107, &actor->pos);
			}

			if ((actor->actorFlags & 0x200) != 0 && actor->actorPhase == 102)
			{
				++g_collectionCount;
				Particles::SpawnCollectSparkle(actor->pos.x, actor->pos.y - 0x2000, actor->pos.z, 50);
				AudioManager::PlaySoundEffect(108, &actor->pos);
				Actor::Kill(actor, 2);
			}
		}

		// FUNCTION: TOY2 0x0042D3E0
		void Smith_Level14(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			int32_t flash = (TarmacTrouble::g_unk52FF4C - 1) & 1;
			int32_t throwTimer;
			Vector3I spawnPos;

			TarmacTrouble::g_unk52FF4C = flash;

			if (actor->actorPhase != TarmacTrouble::g_unk52FF74)
			{
				TarmacTrouble::g_unk52FF74 = actor->actorPhase;
				TarmacTrouble::g_unk52FF78 = 60;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				flash = TarmacTrouble::g_unk52FF4C;
			}

			if (TarmacTrouble::g_unk52FFAC != 2)
				goto LABEL_8;

			TarmacTrouble::g_unk52FF78 -= Renderer::g_frameDelta;

			if (TarmacTrouble::g_unk52FF78 < 0)
			{
				TarmacTrouble::g_unk52FF78 = 0;
				*(uint8_t*)&actor->creatureRam->defenseMode = 6;
			LABEL_8:
				actor->useTint = 0;
				goto LABEL_9;
			}

			if (! flash)
				goto LABEL_8;

			actor->useTint = 1;
			actor->actorTint.r = 0x2000;
			actor->actorTint.g = 0x2000;
			actor->actorTint.b = 0x2000;
		LABEL_9:
			if ((context->targetFlags & 1) != 0 && Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &actor->pos, 200) && actor->primaryAnimIdx == 1)
			{
				Actor::SetAnimation(actor, 3, 24);
				throwTimer = 63;
				actor->creatureRam->speedTarget = 0;
				TarmacTrouble::g_unk52FFE8 = 63;
			}
			else
			{
				throwTimer = TarmacTrouble::g_unk52FFE8;
			}

			if (actor->primaryAnimIdx == 3 && (actor->unkVar7 & 0xFFFF0000) > 3014656)
			{
				actor->unkShort1 = 0;
				actor->movementData = Actor::g_movementData[39] + 16;
				actor->creatureRam->speedTarget = 16;
				actor->actorFlags &= 0xFFF3;
				throwTimer = TarmacTrouble::g_unk52FFE8;
			}

			if (throwTimer)
			{
				TarmacTrouble::g_unk52FFE8 = throwTimer - Renderer::g_frameDelta;

				if (throwTimer - (int32_t)Renderer::g_frameDelta <= 0)
				{
					spawnPos.x = 180;
					spawnPos.y = -150;
					spawnPos.z = -50;
					Actor::ResolveBoneAttachmentPos(&spawnPos, actor, 4);
					Nu3D::Particles::SpawnInstance(spawnPos.x, spawnPos.y, spawnPos.z, 0, -2, 0, 4 * actor->yawAngle, 0, 0, 102)->pitchAngle = -1;
					TarmacTrouble::g_unk52FFE8 = 0;
					AudioManager::PlaySoundEffect(167, &actor->pos);
				}
			}

			if (TarmacTrouble::g_unk52FFAC == 2)
			{
				g_hudWidgetTimers[7] = 90;
				g_unk52B7E0 = 54 * (actor->actorPhase - 9) / 20;
			}

			if (actor->actorPhase < 10 && TarmacTrouble::g_unk52FFAC == 2)
			{
				actor->movementData = Actor::g_movementData[39] + 45;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				actor->actorFlags &= 0xFEF7;
				actor->unkShort1 = 0;
				AudioManager::PlaySoundEffect(-2, &actor->pos);
				TarmacTrouble::g_unk52FFAC = 3;
				g_unk52B7E0 = 0;
				TarmacTrouble::g_unk52FFE8 = 0;
				actor->creatureRam->speedTarget = 16;
			}
		}

		// FUNCTION: TOY2 0x0042D620
		void Luggage(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;

			if ((actor->actorFlags & Actor::ACTOR_PLAYER_TRIGGER) != 0 && actor->actorPhase == 102)
			{
				++g_collectionCount;
				Particles::SpawnCollectSparkle(actor->pos.x, actor->pos.y - 0x2000, actor->pos.z, 50);
				AudioManager::PlaySoundEffect(31, &actor->pos);
				Actor::Kill(actor, 2);
			}
		}

		// FUNCTION: TOY2 0x0042F310
		void Smith(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			int32_t flash = (FinalShowdown::g_unk530054 - 1) & 1;
			Toy2::Actor::Toy2Actor* actor = context->actor;
			int32_t throwTimer;
			Vector3I spawnPos;

			FinalShowdown::g_unk530054 = flash;

			if (actor->actorPhase != FinalShowdown::g_unk530028)
			{
				FinalShowdown::g_unk530028 = actor->actorPhase;
				FinalShowdown::g_unk530018 = 60;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				flash = FinalShowdown::g_unk530054;
			}

			if (FinalShowdown::g_unk530008 != 2)
				goto LABEL_8;

			FinalShowdown::g_unk530018 -= Renderer::g_frameDelta;

			if (FinalShowdown::g_unk530018 < 0)
			{
				FinalShowdown::g_unk530018 = 0;
				*(uint8_t*)&actor->creatureRam->defenseMode = 6;
			LABEL_8:
				actor->useTint = 0;
				goto LABEL_9;
			}

			if (! flash)
				goto LABEL_8;

			actor->useTint = 1;
			actor->actorTint.r = 0x2000;
			actor->actorTint.g = 0x2000;
			actor->actorTint.b = 0x2000;
		LABEL_9:
			if ((context->targetFlags & 1) != 0 && Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &actor->pos, 300) && actor->primaryAnimIdx == 1)
			{
				Actor::SetAnimation(actor, 3, 24);
				throwTimer = 63;
				actor->creatureRam->speedTarget = 0;
				FinalShowdown::g_unk530060 = 63;
			}
			else
			{
				throwTimer = FinalShowdown::g_unk530060;
			}

			if (actor->primaryAnimIdx == 3 && (actor->unkVar7 & 0xFFFF0000) > 3014656)
			{
				actor->unkShort1 = 0;
				actor->movementData = Actor::g_movementData[39] + 16;
				actor->creatureRam->speedTarget = 16;
				actor->actorFlags &= 0xFFF3;
				throwTimer = FinalShowdown::g_unk530060;
			}

			if (throwTimer)
			{
				FinalShowdown::g_unk530060 = throwTimer - Renderer::g_frameDelta;

				if (throwTimer - (int32_t)Renderer::g_frameDelta <= 0)
				{
					spawnPos.x = 180;
					spawnPos.y = -150;
					spawnPos.z = -50;
					Actor::ResolveBoneAttachmentPos(&spawnPos, actor, 4);
					Nu3D::Particles::SpawnInstance(spawnPos.x, spawnPos.y, spawnPos.z, 0, -2, 0, 4 * actor->yawAngle, 0, 0, 102)->pitchAngle = -1;
					FinalShowdown::g_unk530060 = 0;
					AudioManager::PlaySoundEffect(167, &actor->pos);
				}
			}

			if (actor->actorPhase < 10 && FinalShowdown::g_unk530008 == 2)
			{
				actor->movementData = Actor::g_movementData[39] + 45;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				actor->actorFlags &= 0xFEF7;
				actor->unkShort1 = 0;
				AudioManager::PlaySoundEffect(-2, &actor->pos);
				FinalShowdown::g_unk530008 = 3;

				if (++FinalShowdown::g_unk530064 == 3)
					FinalShowdown::g_unk52FFF4 = 0;

				FinalShowdown::g_unk530060 = 0;
				actor->creatureRam->speedTarget = 16;
			}
		}

		// FUNCTION: TOY2 0x0042F530
		void GunsL(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			Toy2::Actor::Toy2Actor* actor = context->actor;
			int32_t flash = (FinalShowdown::g_unk530050 - 1) & 1;
			int32_t fired;
			int32_t yawAngle;
			int32_t count;
			Vector3I spawnPos;

			FinalShowdown::g_unk530050 = flash;

			if (actor->actorPhase != FinalShowdown::g_unk530030)
			{
				FinalShowdown::g_unk530030 = actor->actorPhase;
				FinalShowdown::g_unk530020 = 60;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				AudioManager::PlaySoundEffect(193, &actor->pos);
				flash = FinalShowdown::g_unk530050;
			}

			if (FinalShowdown::g_unk530004 == 2)
			{
				FinalShowdown::g_unk530020 -= Renderer::g_frameDelta;

				if (FinalShowdown::g_unk530020 >= 0)
				{
					if (flash)
					{
						actor->useTint = 1;
						actor->actorTint.r = 0x2000;
						actor->actorTint.g = 0x2000;
						actor->actorTint.b = 0x2000;
						goto LABEL_9;
					}
				}
				else
				{
					FinalShowdown::g_unk530020 = 0;
					*(uint8_t*)&actor->creatureRam->defenseMode = 7;
				}
			}

			actor->useTint = 0;
		LABEL_9:
			int16_t shotTimer = actor->unkShort4;

			if (shotTimer)
			{
				fired = 0;

				if (shotTimer > 20)
				{
					fired = 1;
					memset(&spawnPos, 0, sizeof(spawnPos));
					Actor::ResolveBoneAttachmentPos(&spawnPos, actor, 15);
					actor->unkShort4 = 10;
				}

				actor->unkShort4 -= Renderer::g_frameDelta;

				if (actor->unkShort4 > 0)
				{
					if (! fired)
						goto LABEL_20;
				}
				else
				{
					memset(&spawnPos, 0, sizeof(spawnPos));
					Actor::ResolveBoneAttachmentPos(&spawnPos, actor, 16);
					actor->unkShort4 = 0;
				}

				AudioManager::PlaySoundEffect(86, &actor->pos);
				yawAngle = Nu3D::Math::CartesianToFixedAngle(g_buzzActor.posAngles.pos.x - spawnPos.x, g_buzzActor.posAngles.pos.z - spawnPos.z) & 0xFFF;

				if (((yawAngle - actor->yawAngle + 256) & 0xFFFu) > 0x200)
					yawAngle = actor->yawAngle;

				Nu3D::Particles::SpawnInstance(
					spawnPos.x,
					spawnPos.y,
					spawnPos.z,
					Numerics::g_sinCosLUT[yawAngle] >> 2,
					512,
					Numerics::g_sinCosLUT[(yawAngle + 1024) & 0xFFF] >> 2,
					0,
					0,
					0,
					97);

				if ((actor->actorFlags & 1) != 0)
				{
					count = 5;

					do
					{
						Nu3D::Particles::SpawnFromPreset(spawnPos.x, spawnPos.y, spawnPos.z, 100, 15)->rotSpeed = *g_randDatBufferPtr - 128;
						--count;
						++g_randDatBufferPtr;
					} while (count);
				}
			}
		LABEL_20:
			if (actor->actorPhase < 10 && FinalShowdown::g_unk530004 == 2)
			{
				actor->movementData = Actor::g_movementData[33] + 52;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				actor->actorFlags &= ~0x100;
				AudioManager::PlaySoundEffect(-2, &actor->pos);
				FinalShowdown::g_unk530004 = 3;

				if (++FinalShowdown::g_unk530064 == 3)
					FinalShowdown::g_unk52FFF4 = 1;

				actor->unkShort1 = 0;
			}
		}

		// FUNCTION: TOY2 0x0042F7B0
		void ProsP(Toy2::Actor::Toy2Actor::ActorBehaviourContext* l_actor)
		{
			Toy2::Actor::Toy2Actor* actor = l_actor->actor;
			int32_t flash = (FinalShowdown::g_unk53004C - 1) & 1;
			int32_t soundIndex;
			int32_t randValue;
			int32_t yawAngle;
			int32_t cosValue;

			FinalShowdown::g_unk53004C = flash;

			if (actor->actorPhase != FinalShowdown::g_unk53002C)
			{
				FinalShowdown::g_unk53002C = actor->actorPhase;
				FinalShowdown::g_unk53001C = 60;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				soundIndex = *g_randDatBufferPtr++ & 3;

				if (soundIndex == 3)
					soundIndex = 0;

				AudioManager::Preset::PlayOneShotSound2(soundIndex + 196, actor);
				flash = FinalShowdown::g_unk53004C;
			}

			if (FinalShowdown::g_unk53000C == 2)
			{
				FinalShowdown::g_unk52FFFC -= Renderer::g_frameDelta;

				if (FinalShowdown::g_unk52FFFC < 0)
				{
					randValue = *g_randDatBufferPtr++;
					FinalShowdown::g_unk52FFFC = 2 * randValue + 400;
					soundIndex = *g_randDatBufferPtr++ % 6;

					if (! soundIndex)
						soundIndex = -4;

					AudioManager::Preset::PlayOneShotSound(soundIndex + 199, actor);
					flash = FinalShowdown::g_unk53004C;
				}

				FinalShowdown::g_unk53001C -= Renderer::g_frameDelta;

				if (FinalShowdown::g_unk53001C >= 0)
				{
					if (flash)
					{
						actor->useTint = 1;
						actor->actorTint.r = 0x2000;
						actor->actorTint.g = 0x2000;
						actor->actorTint.b = 0x2000;
						goto LABEL_13;
					}
				}
				else
				{
					FinalShowdown::g_unk53001C = 0;
					*(uint8_t*)&actor->creatureRam->defenseMode = 6;
				}
			}

			actor->useTint = 0;
		LABEL_13:
			if ((l_actor->targetFlags & 1) != 0)
			{
				if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &actor->pos, 300))
				{
					if (actor->primaryAnimIdx == 4)
					{
						((uint8_t*)&actor->areaIndex)[2] = 0xD0;
						((uint8_t*)&actor->areaIndex)[3] = 0xD0;
						Actor::SetAnimation(actor, 3, 9);
						actor->creatureRam->speedTarget = 0;
						FinalShowdown::g_unk53005C = 44;

						if (! AudioManager::IsActorSoundPlaying((Buzz::Toy2BuzzActor*)actor))
							AudioManager::Preset::PlayOneShotSound2(194, actor);
					}
				}
			}

			if (actor->primaryAnimIdx == 3 && (actor->unkVar7 & 0xFFFF0000) > 1376256)
			{
				actor->unkShort1 = 0;
				actor->movementData = Actor::g_movementData[41] + 16;
				actor->creatureRam->speedTarget = 16;
				actor->actorFlags &= 0xFFF3;
			}

			if (FinalShowdown::g_unk53005C)
			{
				FinalShowdown::g_unk53005C -= Renderer::g_frameDelta;

				if (FinalShowdown::g_unk53005C <= 0)
				{
					yawAngle = actor->yawAngle;
					cosValue = Numerics::g_sinCosLUT[(yawAngle + 1024) & 0xFFF];
					Nu3D::Particles::SpawnInstance(
						actor->pos.x + (cosValue >> 3),
						actor->pos.y - 2048,
						actor->pos.z + (Numerics::g_sinCosLUT[(yawAngle - 2048) & 0xFFF] >> 3),
						Numerics::g_sinCosLUT[yawAngle] >> 2,
						0,
						cosValue >> 2,
						0,
						(2047 - yawAngle) & 0xFFF,
						0,
						104);
					FinalShowdown::g_unk53005C = 0;
					AudioManager::PlaySoundEffect(166, &actor->pos);
				}
			}

			if (actor->actorPhase < 10 && FinalShowdown::g_unk53000C == 2)
			{
				actor->movementData = Actor::g_movementData[41] + 45;
				*(uint8_t*)&actor->creatureRam->defenseMode = 4;
				actor->actorFlags &= 0xFEF7;
				actor->unkShort1 = 0;
				AudioManager::PlaySoundEffect(-2, &actor->pos);
				FinalShowdown::g_unk53000C = 3;

				if (++FinalShowdown::g_unk530064 == 3)
					FinalShowdown::g_unk52FFF4 = 2;

				FinalShowdown::g_unk53005C = 0;
				actor->creatureRam->speedTarget = 16;
			}
		}

		// FUNCTION: TOY2 0x0043C070
		void RCCar_Method1(Toy2::Actor::Toy2Actor* a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5)
		{
			int32_t boneIndex = *(int16_t*)(CharacterLoader::g_unk547CD4[a1->creatureId] + 4) + a2;
			AnglesWithRoll* angles = (AnglesWithRoll*)&Collision::g_mathScratch[237 * 4];

			angles->angles.pitch = (uint16_t)a3;
			angles->angles.yaw = (uint16_t)a4;
			angles->roll = (int16_t)a5;
			Nu3D::Math::EulerToRotationMatrix(angles, &CharacterLoader::g_boneTransforms[boneIndex].rotation);
			Actor::SetNodeAngle(a1, a2, a3, a4, a5);
		}
	}
}

#include "Toy2/CreatureBehaviour.h"
#include "Toy2/Toy2.h"
#include "Toy2/Particles.h"
#include "Nu3D/Math.h"
#include "Nu3D/Particles.h"
#include "AudioManager/AudioManager.h"
#include "Renderer/Renderer.h"
#include "Toy2/Collision.h"
#include "CharacterLoader.h"
#include "Random.h"
#include "Numerics.h"
#include <stdlib.h>

namespace Toy2
{
	namespace CreatureBehaviour
	{
		// GLOBAL: TOY2 0x0050A544
		int32_t g_unk50A544;

		// GLOBAL: TOY2 0x0050A548
		int32_t g_unk50A548;

		// STUB: TOY2 0x00406220
		void Zurg3(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x004064A0
		void ZgCar(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x00406620
		void ZPod(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x004068E0
		void Box(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x00406960
		void BPlane(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// FUNCTION: TOY2 0x00406A60
		void RCCar(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context)
		{
			if (g_levelFileIndex == 1)
				RCCar_Level1(context);

			if (g_levelFileIndex == 2)
				RCCar_Level2(context);
		}

		// STUB: TOY2 0x00406A90
		void FatBloke(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x00406C70
		void Buzzard(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

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

		// STUB: TOY2 0x00416AB0
		void TinMan(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

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
						10)->rotSpeed = *g_randDatBufferPtr - 128;
					++g_randDatBufferPtr;

					Nu3D::Particles::SpawnFromPreset(
						(Numerics::g_sinCosLUT[(actor->yawAngle - 1664) & 0xFFF] >> 1) + actor->pos.x,
						actor->pos.y - 2048,
						actor->pos.z + (Numerics::g_sinCosLUT[(actor->yawAngle - 640) & 0xFFF] >> 1),
						42,
						10)->rotSpeed = *g_randDatBufferPtr - 128;
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
					if (!(*g_randDatBufferPtr++ & 3))
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
							actor->pos.x,
							actor->pos.y - 4096,
							actor->pos.z,
							4 * velocity,
							-3072,
							velocity,
							96,
							0,
							4 * velocity,
							121);

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
						10)->rotSpeed = *g_randDatBufferPtr - 128;
					++g_randDatBufferPtr;

					Nu3D::Particles::SpawnFromPreset(
						(Numerics::g_sinCosLUT[(actor->yawAngle - 1664) & 0xFFF] >> 1) + actor->pos.x,
						actor->pos.y - 2048,
						actor->pos.z + (Numerics::g_sinCosLUT[(actor->yawAngle - 640) & 0xFFF] >> 1),
						42,
						10)->rotSpeed = *g_randDatBufferPtr - 128;
					++g_randDatBufferPtr;

					AudioManager::PlaySoundEffect(54, &actor->pos);
				}
			}
		}

		// STUB: TOY2 0x004189C0
		void ZKite(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x00418CE0
		void LawnMower(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x0041B780
		void Drill(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x0041BB80
		void LTyke(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x0041DDB0
		void Clown(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x0041DEC0
		void Ducks(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x0041DF70
		void ZBoat(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x00420AF0
		void Dino(Toy2::Actor::Toy2Actor::ActorBehaviourContext* l_actor) {}

		// STUB: TOY2 0x00420ED0
		void Chick(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x00422660
		void BBuggy(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x00422C70
		void Martian(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x00425700
		void GunsP(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x004259B0
		void Mouse(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x004282D0
		void GunsL_Level11(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x00428650
		void Rabid(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x0042BE60
		void ProsP_Level13(Toy2::Actor::Toy2Actor::ActorBehaviourContext* l_actor) {}

		// STUB: TOY2 0x0042C150
		void Pilot(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x0042D3E0
		void Smith_Level14(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x0042D620
		void Luggage(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x0042F310
		void Smith(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x0042F530
		void GunsL(Toy2::Actor::Toy2Actor::ActorBehaviourContext* context) {}

		// STUB: TOY2 0x0042F7B0
		void ProsP(Toy2::Actor::Toy2Actor::ActorBehaviourContext* l_actor) {}

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

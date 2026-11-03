#include "Toy2/Levels/SlimeTime.h"

#include "Toy2/Toy2.h"
#include "Toy2/Camera.h"
#include "Toy2/Collectables.h"
#include "Toy2/MoveableObject.h"

#include "Nu3D/Math.h"
#include "Nu3D/Collision.h"

#include "Renderer/Renderer.h"
#include "AudioManager/AudioManager.h"
#include "Random.h"
#include "SaveManager.h"

namespace Toy2
{
	namespace SlimeTime
	{
		// GLOBAL: TOY2 0x0052F98C
		int32_t g_lastSlimeManPhase;

		// GLOBAL: TOY2 0x0052F990
		int32_t g_hitTimer;

		// GLOBAL: TOY2 0x0052F994
		int32_t g_outOfBoundsRiseAngle;

		// GLOBAL: TOY2 0x0052F998
		int32_t g_unkSoundTimer;

		// GLOBAL: TOY2 0x0052F99C
		int32_t g_isOutsideBounds;

		// GLOBAL: TOY2 0x0052F9A0
		int32_t g_attackCycleTimer;

		// GLOBAL: TOY2 0x0052F9A4
		int32_t g_slimeManState;

		// GLOBAL: TOY2 0x0052F9A8
		int32_t g_tintBlink;

		// GLOBAL: TOY2 0x0052F9AC
		int32_t g_unused;

		// GLOBAL: TOY2 0x0052F9B0
		int32_t g_soundCooldown;

		// GLOBAL: TOY2 0x0052F9B4
		int32_t g_lastCameraYaw;

		// GLOBAL: TOY2 0x0052F9B8
		int32_t g_retreatTargetX;

		// FUNCTION: TOY2 0x0041FFB0 [MATCHED]
		void Initialize()
		{
			MoveableObject::InitTable(0);
			Collectables::Init(0, 0);

			g_levelActors[0].actorFlags |= Actor::ACTOR_NO_DESPAWN;

			g_lastSlimeManPhase = g_levelActors[0].actorPhase;
			g_slimeManState = 0;

			g_levelActors[0].transform.pos.x += 0x80000;

			g_attackCycleTimer = 300;
			g_retreatTargetX = 0x40000;
			g_isOutsideBounds = 0;
			g_unkSoundTimer = 150;
			g_outOfBoundsRiseAngle = 0;
			g_unused = 0x7FFFFFFF;
			g_lastCameraYaw = 0;
			g_soundCooldown = 0;
			g_tintBlink = 0;
			g_hitTimer = 0;

			g_levelActors[0].transform.angles.yaw = 3072;
			g_levelActors[0].creatureRam->boundHalfX = 4096;
			g_levelActors[0].creatureRam->boundHalfZ = 4096;
		}

		// FUNCTION: TOY2 0x00420060 [MATCHED]
		void Interactions()
		{
			AudioManager::PlaySoundEffect(103, 0);
			
			g_tintBlink = (g_tintBlink - 1) & 1;

			Actor::Toy2Actor* slimeMan = &g_levelActors[0];

			if (slimeMan->actorPhase && slimeMan->actorPhase != g_lastSlimeManPhase)
			{
				g_lastSlimeManPhase = slimeMan->actorPhase;
				slimeMan->creatureRam->defenseMode = 4;
				g_attackCycleTimer = 299;

				if (slimeMan->actorPhase <= 10)
				{
					g_hitTimer = 300;
					Camera::BeginScriptedCutsceneAtPoint(&slimeMan->transform.pos, 300, 16);

					slimeMan->motionTargetPos.x = slimeMan->transform.pos.x;
					slimeMan->motionTargetPos.z = slimeMan->transform.pos.z;

					Camera::g_cutsceneEyePos.x = slimeMan->transform.pos.x;
					Camera::g_cutsceneEyePos.y = slimeMan->transform.pos.y - 20480;
					Camera::g_cutsceneEyePos.z = slimeMan->transform.pos.z;

					g_slimeManState = 3;
					SaveManager::g_save0Data.tokens[g_levelFileIndex] = 128;
				}
				else
				{
					g_hitTimer = 120;
					Camera::BeginScriptedCutsceneAtPoint(&slimeMan->transform.pos, 120, 16);
				}
			}

			g_hitTimer -= (int32_t)Renderer::g_frameDelta;

			if (g_hitTimer < 0)
			{
				g_hitTimer = 0;
				slimeMan->creatureRam->defenseMode = 7;
				slimeMan->useTint = 0;
			}
			else if (slimeMan->creatureId)
			{
				Camera::g_cutsceneFocusPos = slimeMan->transform.pos;

				if (g_hitTimer < 90)
					AudioManager::PlaySoundEffect(102, &Camera::g_cutsceneFocusPos);

				if (g_tintBlink)
				{
					slimeMan->useTint = 1;
					slimeMan->actorTint.r = 0x2000;
					slimeMan->actorTint.g = 0x2000;
					slimeMan->actorTint.b = 0x2000;
				}
				else
				{
					slimeMan->useTint = 0;
				}
			}

			if (! Actor::IsInsideBounds(slimeMan, -233647, 233647, -139166, 139166))
				g_isOutsideBounds = 1;
			else
				g_isOutsideBounds = 0;

			if (g_isOutsideBounds)
			{
				if (g_outOfBoundsRiseAngle < 2048)
				{
					g_outOfBoundsRiseAngle += (int32_t)Renderer::g_frameDelta << 7;

					if (g_outOfBoundsRiseAngle >= 2048)
						g_outOfBoundsRiseAngle = 2048;
				}
			}
			else
			{
				if (g_outOfBoundsRiseAngle > 0)
				{
					g_outOfBoundsRiseAngle += -128 * (int32_t)Renderer::g_frameDelta;

					if (g_outOfBoundsRiseAngle <= 0)
						g_outOfBoundsRiseAngle = 0;
				}
			}

			if (g_buzzActor.posAngles.pos.x < 204702 && ! g_slimeManState)
			{
				AudioManager::PlayMusicLooping(g_levelIndex);
				g_slimeManState = 1;

				Camera::BeginScriptedCutsceneAtPoint(&slimeMan->transform.pos, 300, 16);
				Camera::g_cutsceneEyePos.x = slimeMan->transform.pos.x - 294912;
				Camera::g_cutsceneEyePos.y = slimeMan->transform.pos.y - 0x8000;
				Camera::g_cutsceneEyePos.z = slimeMan->transform.pos.z;
			}

			if (g_slimeManState > 0 && g_slimeManState < 3)
			{
				if (! g_hitTimer)
					AudioManager::PlaySoundEffect(97, &slimeMan->transform.pos);

				if (! g_soundCooldown && Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &slimeMan->transform.pos, 400))
				{
					int32_t cameraYawDelta = (g_lastCameraYaw - Camera::g_activeCameraTransform.angles.yaw) & 0xFFF;

					if (cameraYawDelta > 0x800)
						cameraYawDelta -= 4096;

					if (abs(cameraYawDelta) > 64)
					{
						AudioManager::PlaySoundEffect((*g_randDatBufferPtr++ & 1) + 98, &slimeMan->transform.pos);
						g_soundCooldown = 100;
					}
				}
				else
				{
					g_soundCooldown -= (int32_t)Renderer::g_frameDelta;

					if (g_soundCooldown <= 0)
						g_soundCooldown = 0;
				}

				g_lastCameraYaw = Camera::g_activeCameraTransform.angles.yaw;
			}

			if (g_slimeManState == 1)
			{
				if (Camera::g_cutsceneTimer < 30)
					slimeMan->actorFlags |= 4;

				int32_t wobbleTime = Camera::g_cutsceneTimer - 30;

				if (wobbleTime < 0)
					wobbleTime = 0;

				slimeMan->rollAngle = ((Numerics::g_sinCosLUT[(20 * wobbleTime + 1024) & 0xFFF] >> 3) - 2048) & 0xFFF;
				slimeMan->transform.pos.x += -2048 * (int32_t)Renderer::g_frameDelta;

				if (! Camera::g_cutsceneTimer)
					g_slimeManState = 2;

				Camera::g_cutsceneFocusPos.x = slimeMan->transform.pos.x;
				Camera::g_cutsceneEyePos.y += 304 * (int32_t)Renderer::g_frameDelta;

				if (Camera::g_cutsceneTimer < 180)
					Camera::g_cutsceneEyePos.x += -512 * (int32_t)Renderer::g_frameDelta;
				else
					Camera::g_cutsceneEyePos.z += -128 * (int32_t)Renderer::g_frameDelta;
			}

			if (g_slimeManState == 2)
			{
				g_attackCycleTimer -= (int32_t)Renderer::g_frameDelta;

				if (g_attackCycleTimer <= 0)
				{
					g_attackCycleTimer = 600;
					g_retreatTargetX = -g_retreatTargetX;
				}

				Camera::g_unk50A118 = slimeMan->transform.pos;

				if (g_tickState[11] && ! g_hitTimer)
				{
					if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &slimeMan->transform.pos, 400))
					{
						Nu3D::Particles::SpawnInstance(
							slimeMan->transform.pos.x,
							slimeMan->transform.pos.y,
							slimeMan->transform.pos.z,
							2 * slimeMan->velX,
							0,
							2 * slimeMan->velForward,
							128,
							0,
							0,
							12);
					}
				}

				if (g_attackCycleTimer > 300)
				{
					slimeMan->motionTargetPos.y = 3 * Numerics::g_sinCosLUT[g_outOfBoundsRiseAngle + 1024] - 68371;
					slimeMan->motionTargetPos.x = g_buzzActor.posAngles.pos.x;
					slimeMan->motionTargetPos.z = g_buzzActor.posAngles.pos.z;

					if (g_attackCycleTimer < 550)
					{
						AudioManager::PlaySoundEffect(101, &slimeMan->motionTargetPos);
						g_unkSoundTimer -= (int32_t)Renderer::g_frameDelta;

						if (g_unkSoundTimer < 0)
						{
							g_unkSoundTimer = 400;
							AudioManager::Preset::PlayOneShotSound(208, (Actor::Toy2Actor*)&g_buzzActor);
						}
					}

					if (g_tickState[6] && g_attackCycleTimer < 550)
					{
						Transform effectPos;
						int32_t probeYaw = slimeMan->transform.angles.yaw - 128;
						effectPos.pos.x = slimeMan->transform.pos.x + 4 * Numerics::g_sinCosLUT[probeYaw & 0xFFF];
						effectPos.pos.y = slimeMan->transform.pos.y;

						effectPos.pos.z = slimeMan->transform.pos.z + 4 * Numerics::g_sinCosLUT[(probeYaw + 1024) & 0xFFF];

						effectPos.pos.y = Nu3D::Collision::GetGroundHeight(&effectPos, 0);

						if (effectPos.pos.y != 0x80000000)
						{
							Nu3D::Particles::SpawnFromPreset(effectPos.pos.x, effectPos.pos.y, effectPos.pos.z, 123, 2);
							Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnFromPreset(effectPos.pos.x, effectPos.pos.y, effectPos.pos.z, 124, 1);

							particle->rotSpeed = *g_randDatBufferPtr;
							++g_randDatBufferPtr;
						}

						probeYaw += 256;
						effectPos.pos.x = slimeMan->transform.pos.x + 4 * Numerics::g_sinCosLUT[probeYaw & 0xFFF];
						effectPos.pos.y = slimeMan->transform.pos.y;
						effectPos.pos.z = slimeMan->transform.pos.z + 4 * Numerics::g_sinCosLUT[(probeYaw + 1024) & 0xFFF];

						effectPos.pos.y = Nu3D::Collision::GetGroundHeight(&effectPos, 0);

						if (effectPos.pos.y != 0x80000000)
						{
							Nu3D::Particles::SpawnFromPreset(effectPos.pos.x, effectPos.pos.y, effectPos.pos.z, 123, 2);
							Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnFromPreset(effectPos.pos.x, effectPos.pos.y, effectPos.pos.z, 124, 1);

							particle->rotSpeed = *g_randDatBufferPtr;
							++g_randDatBufferPtr;
						}
					}
				}
				else
				{
					slimeMan->motionTargetPos.x = g_retreatTargetX;
					slimeMan->motionTargetPos.z = 0;
					slimeMan->motionTargetPos.y = Numerics::g_sinCosLUT[g_outOfBoundsRiseAngle + 1024] + slimeMan->boundary.y + 2 * Numerics::g_sinCosLUT[g_outOfBoundsRiseAngle + 1024] + 0x4000;
				}

				g_hudWidgetTimers[7] = 90;
			}

			if (g_slimeManState == 3)
			{
				slimeMan->transform.pos.y -= 640 * (int32_t)Renderer::g_frameDelta;
				slimeMan->motionTargetPos.y -= 640 * (int32_t)Renderer::g_frameDelta;

				Camera::g_cutsceneEyePos.y += -128 * (int32_t)Renderer::g_frameDelta;

				if (slimeMan->actorPhase <= 10 && Camera::g_cutsceneTimer < 120)
					Actor::Kill(slimeMan, 1u);

				if (! Camera::g_cutsceneTimer)
				{
					g_unk830CC4 = 1;
					g_slimeManState = 4;
				}
			}

			if ((slimeMan->actorFlags & 4) != 0)
			{
				if (g_hitTimer)
				{
					slimeMan->rollAngle += (int32_t)Renderer::g_frameDelta << 6;
					slimeMan->rollAngle &= 0xFFF;

					if (g_slimeManState == 3)
					{
						slimeMan->transform.angles.pitch += 36 * (int32_t)Renderer::g_frameDelta;
						slimeMan->transform.angles.pitch &= 0xFFF;
					}
				}
				else
				{
					int32_t yawToTarget = (slimeMan->transform.angles.yaw - slimeMan->targetYaw) & 0xFFF;

					if (yawToTarget > 0x800)
						yawToTarget -= 4096;

					slimeMan->rollAngle -= (slimeMan->rollAngle - yawToTarget / 2) >> 5;
				}
			}

			if ((slimeMan->actorFlags & 1) != 0)
			{
				Transform effectPos;

				if (g_tickState[2])
				{
					effectPos.pos.x = 500;
					effectPos.pos.y = -400;
					effectPos.pos.z = 0;

					Actor::ResolveBoneAttachmentPos(&effectPos.pos, slimeMan, 0);

					Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnFromPreset(effectPos.pos.x, effectPos.pos.y, effectPos.pos.z, 17, 2);
					particle->rotSpeed = (*g_randDatBufferPtr & 0x1F) - 16;

					effectPos.pos.x = -500;
					++g_randDatBufferPtr;
					effectPos.pos.y = -400;
					effectPos.pos.z = 0;

					Actor::ResolveBoneAttachmentPos(&effectPos.pos, slimeMan, 0);

					particle = Nu3D::Particles::SpawnFromPreset(effectPos.pos.x, effectPos.pos.y, effectPos.pos.z, 17, 2);
					particle->rotSpeed = (*g_randDatBufferPtr & 0x1F) - 16;

					++g_randDatBufferPtr;
				}

				if (slimeMan->actorPhase <= 10 && g_tickState[7])
				{
					effectPos.pos.x = 0;
					effectPos.pos.y = 0;
					effectPos.pos.z = -200;

					Actor::ResolveBoneAttachmentPos(&effectPos.pos, slimeMan, 0);

					Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnFromPreset(effectPos.pos.x, effectPos.pos.y, effectPos.pos.z, 35, 14);
					particle->rotSpeed = *g_randDatBufferPtr - 128;

					++g_randDatBufferPtr;
				}
			}

			g_unk52B7E0 = 54 * (slimeMan->actorPhase - 10) / 10;
		}

	}
}

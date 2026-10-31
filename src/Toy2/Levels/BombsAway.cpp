#include "Toy2/Levels/BombsAway.h"

#include "Toy2/Toy2.h"
#include "Toy2/Actor.h"
#include "Toy2/Buzz.h"
#include "Toy2/Camera.h"
#include "Toy2/Levels.h"
#include "Toy2/Lighting.h"
#include "Toy2/Weather.h"

#include "Nu3D/Link.h"
#include "Nu3D/Math.h"
#include "Nu3D/Particles.h"
#include "Nu3D/Camera.h"

#include "Renderer/Renderer.h"
#include "Renderer/LensFlare.h"

#include "AudioManager/AudioManager.h"
#include "Random.h"
#include "Numerics.h"
#include "SaveManager.h"

#include <STDLIB.H>

namespace Toy2
{
	namespace BombsAway
	{
		// GLOBAL: TOY2 0x0052F6F0
		int32_t g_damageLevel;

		// GLOBAL: TOY2 0x0052F6F4
		int32_t g_dropY;

		// GLOBAL: TOY2 0x0052F6F8
		int32_t g_wobblePhase;

		// GLOBAL: TOY2 0x0052F6FC
		int32_t g_dropVelocity;

		// GLOBAL: TOY2 0x0052F700
		int32_t g_flashSoundDelay;

		// GLOBAL: TOY2 0x0052F704
		int32_t g_currentSize;

		// GLOBAL: TOY2 0x0052F708
		int32_t g_flashTimer;

		// GLOBAL: TOY2 0x0052F70C
		int32_t g_prevCutsceneTimer;

		// GLOBAL: TOY2 0x0052F710
		int32_t g_hitFlashTimer;

		// GLOBAL: TOY2 0x0052F714
		int32_t g_unusedInt;

		// GLOBAL: TOY2 0x0052F718
		int32_t g_introSoundTimer;

		// GLOBAL: TOY2 0x0052F71C
		int32_t g_tauntTimer;

		// GLOBAL: TOY2 0x0052F720
		int32_t g_textureScrollV;

		// GLOBAL: TOY2 0x0052F724
		int32_t g_textureScrollU;

		// GLOBAL: TOY2 0x0052F72C
		int32_t g_savedRecordY;

		// GLOBAL: TOY2 0x0052F748
		int32_t g_tintPhaseR;

		// GLOBAL: TOY2 0x0052F74C
		int32_t g_tintPhaseB;

		// GLOBAL: TOY2 0x0052F750
		int32_t g_tintPhaseG;

		// GLOBAL: TOY2 0x0052F754
		int32_t g_bossState;

		// GLOBAL: TOY2 0x0052F758
		int32_t g_hitCutsceneLength;

		// GLOBAL: TOY2 0x0052F75C
		int32_t g_targetSize;

		// GLOBAL: TOY2 0x0052F730
		int32_t g_lastBossPhase;

		// GLOBAL: TOY2 0x0052F738
		Vector3I g_flashSoundPos;

		// FUNCTION: TOY2 0x0041AA10
		void Interactions()
		{
			AudioManager::PlaySoundEffect(111, (Vector3I*)1);
			AudioManager::PlaySoundEffect(103, 0);

			int32_t spawnParticles = 0;

			if (g_introSoundTimer > 0)
			{
				g_introSoundTimer -= Renderer::g_frameDelta;

				if (g_introSoundTimer < 0)
					AudioManager::Preset::PlayOneShotSound(210, (Actor::Toy2Actor*)&g_buzzActor);
			}

			g_tintPhaseR = (g_tintPhaseR + 31 * Renderer::g_frameDelta) & 0xFFF;
			g_tintPhaseG &= 0xFFFu;
			g_tintPhaseB = (g_tintPhaseB + 63 * Renderer::g_frameDelta) & 0xFFF;

			if (! g_bossState && Nu3D::Math::IsWithinDistanceXZ(&g_buzzActor.posAngles.pos, &g_levelActors[0].transform.pos, 380))
			{
				g_bossState = 1;
				g_levelActors[0].transform.pos.y -= 11000;
				Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].transform.pos, 300, 16);
				g_levelActors[0].transform.pos.y += 11000;
				AudioManager::PlayMusicLooping(g_levelIndex);
			}

			if (g_bossState == 1)
			{
				if (Camera::g_cutsceneTimer < 240)
				{
					if (g_prevCutsceneTimer >= 240)
						AudioManager::PlaySoundEffect(70, &g_levelActors[0].transform.pos);

					spawnParticles = 1;
				}

				if (Camera::g_cutsceneTimer < 150)
				{
					if (g_prevCutsceneTimer >= 150)
						AudioManager::PlaySoundEffect(-4, &g_levelActors[0].transform.pos);

					g_currentSize += (int32_t)Renderer::g_frameDelta * (g_currentSize - g_targetSize) / -32;
				}

				if (! Camera::g_cutsceneTimer)
				{
					g_bossState = 999;
					g_levelActors[0].movementData = Actor::g_movementData[14] + 9;
					g_levelActors[0].unkShort1 = 0;
					Nu3D::Link::SetRotationRelative8bit(0, 0, 0, 0);
				}
			}

			switch (g_bossState)
			{
				case 999:
					g_tauntTimer -= (int32_t)Renderer::g_frameDelta;

					if (g_tauntTimer <= 0)
					{
						g_tauntTimer = *g_randDatBufferPtr++ + 1800;
						AudioManager::Preset::PlayOneShotSound(209, (Actor::Toy2Actor*)&g_buzzActor);
					}

					if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &g_levelActors[0].transform.pos, 50))
					{
						int16_t hitAngle = Nu3D::Math::CartesianToFixedAngle(
							g_buzzActor.posAngles.pos.x - g_levelActors[0].transform.pos.x, g_buzzActor.posAngles.pos.z - g_levelActors[0].transform.pos.z);

						Buzz::HandleDamage(hitAngle & 0xFFF, 3u);
					}

					{
						if (g_lastBossPhase != g_levelActors[0].actorPhase)
						{
							g_hitFlashTimer = 4;
							g_levelActors[0].actorPhase = g_lastBossPhase;
							g_targetSize -= 768;

							if (g_targetSize < 512)
							{
								g_targetSize = 512;
								g_damageLevel += 2048;

								for (int32_t i = 0; i < 64; ++i)
								{
									if (Nu3D::Particles::g_particleInstances[i].typeId == 63)
										Nu3D::Particles::g_particleInstances[i].lifetime = 1;
								}

								if (g_damageLevel >= 12288 && g_savedRecordY)
								{
									Nu3D::Link::SetScaleFromFixedOffsets(50, 4096, 4096, 4096);
									Levels::g_recordData[63]->data[0].y = g_savedRecordY;
									g_savedRecordY = 0;
								}

								if (g_damageLevel >= 14336)
								{
									g_levelActors[0].movementData = Actor::g_movementData[14] + 94;

									g_damageLevel = 14336;
									g_currentSize = 1;
									g_targetSize = 1;

									g_levelActors[0].unkShort1 = 0;
									g_bossState = 1000;
									g_levelActors[0].transform.pos.y -= 11000;
									g_buzzActor.posAngles.pos.x += 10000;
									g_buzzActor.posAngles.pos.z += 10000;

									Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].transform.pos, 420, 16);

									g_levelActors[0].actorFlags &= 0xFE7Fu;
									g_buzzActor.posAngles.pos.z -= 10000;
									g_levelActors[0].transform.pos.y += 11000;
									g_buzzActor.posAngles.pos.x -= 10000;

									g_dropY = g_levelActors[0].transform.pos.y - 12000;
									SaveManager::g_save0Data.tokens[g_levelFileIndex] = 128;
									break;
								}

								g_levelActors[0].creatureRam->defenseMode = 4;

								g_levelActors[0].transform.pos.y -= 14000;
								g_levelActors[0].movementData = Actor::g_movementData[14] + 79;
								g_levelActors[0].unkShort1 = 0;

								Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].transform.pos, g_hitCutsceneLength, 16);

								g_levelActors[0].transform.pos.y += 14000;
								g_levelActors[0].velX = 0;
								g_levelActors[0].gravityVel = 0;
								g_levelActors[0].velForward = 0;

								Actor::SetAnimation(g_levelActors, 1, 16);
								g_hitCutsceneLength += 40;
							}
						}

						if (g_levelActors[0].creatureRam->defenseMode == 4)
							Camera::g_cutsceneFocusPos.y = g_levelActors[0].transform.pos.y - (2 * g_currentSize + 10000);

						if (g_targetSize < g_damageLevel)
						{
							if (g_levelActors[0].creatureRam->defenseMode == 4)
							{
								if (Camera::g_cutsceneTimer < 120)
								{
									if (g_prevCutsceneTimer >= 120)
										AudioManager::PlaySoundEffect(-4, &g_levelActors[0].transform.pos);

									g_targetSize += (int32_t)Renderer::g_frameDelta * (g_targetSize - g_damageLevel) / -32;
								}
								else
								{
									g_levelActors[0].unkShort1 = 5;
								}
							}
							else
							{
								g_targetSize += 32 * Renderer::g_frameDelta;
							}

							if (g_targetSize >= g_damageLevel)
								g_targetSize = g_damageLevel;
						}

						g_currentSize += (int32_t)Renderer::g_frameDelta * (g_currentSize - g_targetSize) / -8;

						if (g_levelActors[0].unkShort4)
						{
							if (g_levelActors[0].unkShort4 == 1)
							{
								Nu3D::Particles::SpawnInstance(
									g_levelActors[0].transform.pos.x,
									g_levelActors[0].transform.pos.y - 10000,
									g_levelActors[0].transform.pos.z,
									0,
									-2,
									0,
									4 * g_levelActors[0].transform.angles.yaw,
									0,
									128,
									63)
									->pitchAngle = 0;

								AudioManager::PlaySoundEffect(13, &g_levelActors[0].transform.pos);
							}

							if (g_levelActors[0].unkShort4 == 3)
								Camera::g_unk50A510 = 40;

							g_levelActors[0].unkShort4 = 0;
						}

						spawnParticles = 1;
					}
			}

			if (g_bossState == 1000)
			{
				spawnParticles = 1;
				Camera::g_cutsceneFocusPos.x = g_levelActors[0].transform.pos.x;
				Camera::g_cutsceneFocusPos.y = g_levelActors[0].transform.pos.y - 11000;
				Camera::g_cutsceneFocusPos.z = g_levelActors[0].transform.pos.z;

				if (Camera::g_cutsceneTimer < 180)
				{
					g_bossState = 1001;
					g_dropVelocity = -3072;

					g_levelActors[0].transform.pos.y += 9000;
					Actor::Kill(g_levelActors, 1u);
					g_levelActors[0].transform.pos.y -= 9000;
				}
			}

			Transform transform;

			if (g_bossState > 1000)
			{
				Camera::g_cutsceneFocusPos.y += 128;
				g_dropVelocity += 96 * Renderer::g_frameDelta;
				g_dropY += g_dropVelocity;

				if (g_dropY > g_levelActors[0].transform.pos.y - 2000)
				{
					g_dropVelocity = -(g_dropVelocity >> 1);
					g_dropY = g_levelActors[0].transform.pos.y - 2000;

					if (abs(g_dropVelocity) > 128)
						AudioManager::PlaySoundEffect(69, &g_levelActors[0].transform.pos);
				}

				Nu3D::Link::SetPositionRawAndCommit(0, g_levelActors[0].transform.pos.x >> 5, g_dropY >> 5, g_levelActors[0].transform.pos.z >> 5);
			}
			else
			{
				g_wobblePhase = (g_wobblePhase + 128) & 0x1FFF;

				if (g_currentSize < 1024)
					g_wobblePhase = 0;

				Nu3D::Link::SetRotationRelative8bit(
					0,
					Numerics::g_sinCosLUT[g_wobblePhase & 0xFFF] >> 8,
					g_levelActors[0].transform.angles.yaw,
					Numerics::g_sinCosLUT[(g_wobblePhase >> 1) & 0xFFF] >> 8);

				transform.pos.x = 0;
				transform.pos.y = -700;
				transform.pos.z = 0;

				Actor::ResolveBoneAttachmentPos(&transform.pos, g_levelActors, 10);

				Nu3D::Link::SetPositionRawAndCommit(0, transform.pos.x >> 5, (transform.pos.y >> 5) - 32, transform.pos.z >> 5);
			}

			if (g_bossState == 1001)
			{
				if (! Camera::g_cutsceneTimer)
					g_bossState = 1002;
			}

			if (g_bossState > 1001)
			{
				if (! g_unk830CC4)
				{
					g_bossState += Renderer::g_frameDelta;

					if (g_bossState > 1030)
						g_unk830CC4 = 1;
				}
			}

			if (g_tickState[6] && spawnParticles)
			{
				transform.pos.x = g_levelActors[0].transform.pos.x;
				transform.pos.y = g_levelActors[0].transform.pos.y - 10000;
				transform.pos.z = g_levelActors[0].transform.pos.z;

				Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnFromPreset(transform.pos.x, transform.pos.y, transform.pos.z, 65, 20);
				particle->rotSpeed = (*g_randDatBufferPtr - 128) >> 3;

				++g_randDatBufferPtr;
			}

			if (g_bossState < 1001)
			{
				g_levelActors[0].secondaryAnimIdx = g_levelActors[0].primaryAnimIdx - 1;
				g_levelActors[0].unkVar8 = g_levelActors[0].unkVar7;
				g_levelActors[0].unkVar13_ = -301;

				int32_t unkInt1 = -12000 - g_currentSize;

				g_levelActors[0].unkVar12 = g_currentSize;
				g_levelActors[0].unkVar12_ = g_currentSize;
				g_levelActors[0].unkVar13 = g_currentSize;

				if (! unkInt1)
					unkInt1 = 1;

				*(int16_t*)(g_levelActors[0].unkVar34 + 18) = unkInt1;
				*(int16_t*)(g_levelActors[0].unkVar34 + 50) = unkInt1;
				*(int16_t*)(g_levelActors[0].unkVar34 + 82) = unkInt1;
				*(int16_t*)(g_levelActors[0].unkVar34 + 114) = unkInt1;

				int32_t unkInt2 = g_currentSize / 16;
				if (! unkInt2)
					unkInt2 = 1;

				*(int16_t*)(g_levelActors[0].unkVar34 + 30) = unkInt2;
				*(int16_t*)(g_levelActors[0].unkVar34 + 62) = unkInt2;
				*(int16_t*)(g_levelActors[0].unkVar34 + 94) = unkInt2;
				*(int16_t*)(g_levelActors[0].unkVar34 + 126) = unkInt2;
			}

			g_unk52B7E0 = 54 * ((14336 - g_damageLevel) / 2048) / 5;

			if (g_bossState >= 999)
				g_hudWidgetTimers[7] = 90;

			g_prevCutsceneTimer = Camera::g_cutsceneTimer;
			Renderer::BlitTextureByIndexOffset(24, 0, 64, 64, 64, g_textureScrollU, g_textureScrollV, 0, 64);
			g_textureScrollV = (g_textureScrollV - Renderer::g_frameDelta) & 0x3F;

			if (g_bossState >= 999)
			{
				Camera::g_unk50A118.x = g_levelActors[0].transform.pos.x;
				Camera::g_unk50A118.y = g_levelActors[0].transform.pos.y - 10000;
				Camera::g_unk50A118.z = g_levelActors[0].transform.pos.z;
			}

			transform.pos.x = 13453;
			transform.pos.y = -210651;
			transform.pos.z = -12501;

			Renderer::LensFlare::RegisterLight(14473, -221337, -8124, 128, 128, 128, 128);

			if (g_hitFlashTimer)
			{
				g_hitFlashTimer -= Renderer::g_frameDelta;

				if (g_hitFlashTimer < 0)
					g_hitFlashTimer = 0;

				g_levelActors[0].useTint = 1;
				g_levelActors[0].actorTint.r = 0x2000;
				g_levelActors[0].actorTint.g = 0x2000;
				g_levelActors[0].actorTint.b = 0x2000;
			}
			else
			{
				g_levelActors[0].useTint = 983040;
				g_levelActors[0].actorTint.r = Numerics::g_sinCosLUT[g_tintPhaseR] >> 4;
				g_levelActors[0].actorTint.g = Numerics::g_sinCosLUT[g_tintPhaseG] >> 4;
				g_levelActors[0].actorTint.b = Numerics::g_sinCosLUT[g_tintPhaseB] >> 4;
			}

			for (int32_t i = 0; i < 64; ++i)
			{
				if (Nu3D::Particles::g_particleInstances[i].typeId == 63 && Nu3D::Particles::g_particleInstances[i].lifetime > 0)
				{
					Lighting::g_dynamicLights[1].colorR = 0;
					Lighting::g_dynamicLights[1].colorG = -1;
					Lighting::g_dynamicLights[1].colorB = 0;
					Lighting::g_dynamicLights[1].lifetime = 1;
					Lighting::g_dynamicLights[1].groupId = -3;
					Lighting::g_dynamicLights[1].pos.x = Nu3D::Particles::g_particleInstances[i].pos.x;
					Lighting::g_dynamicLights[1].pos.y = Nu3D::Particles::g_particleInstances[i].pos.y;
					Lighting::g_dynamicLights[1].pos.z = Nu3D::Particles::g_particleInstances[i].pos.z;
					break;
				}

				Lighting::g_dynamicLights[1].lifetime = 0;
			}

			transform.pos.x = (transform.pos.x - g_buzzActor.posAngles.pos.x) >> 7;
			transform.pos.y = (transform.pos.y - g_buzzActor.posAngles.pos.y) >> 7;
			transform.pos.z = (transform.pos.z - g_buzzActor.posAngles.pos.z) >> 7;

			Nu3D::Math::NormalizeToFixedPoint(&transform.pos, &transform.pos);

			int32_t lightIdx = g_levelFileIndex;

			Lighting::g_levelLightDefs[lightIdx].offset.x = transform.pos.x >> 2;
			Lighting::g_levelLightDefs[lightIdx].offset.y = transform.pos.y >> 2;
			Lighting::g_levelLightDefs[lightIdx].offset.z = transform.pos.z >> 2;

			transform.pos.x >>= 2;
			transform.pos.y >>= 2;
			transform.pos.z >>= 2;

			Lighting::g_levelLightDefs[lightIdx].packedColor = 0xC0C000;

			Weather::StepPrecipitation(256, 4096, 50);

			if (g_tickState[8])
			{
				int32_t yawSin = Numerics::g_sinCosLUT[Camera::g_activeCameraTransform.angles.yaw];
				int32_t yawCos = Numerics::g_sinCosLUT[(Camera::g_activeCameraTransform.angles.yaw + 1024) & 0xFFF];

				int32_t randOffset = (*g_randDatBufferPtr - 128) << 8;

				g_randDatBufferPtr += 2;
				Nu3D::Particles::SpawnFromPreset(
					randOffset + Camera::g_activeCameraTransform.pos.x + 2 * yawSin + yawSin,
					0,
					randOffset + Camera::g_activeCameraTransform.pos.z + 2 * yawCos + yawCos,
					27,
					2);
			}

			int32_t prevTimer = g_flashTimer;
			g_flashTimer -= Renderer::g_frameDelta;

			if (g_flashTimer < 32)
			{
				if (prevTimer >= 32)
				{
					int32_t randDist = *g_randDatBufferPtr++;
					int32_t randAngle = *g_randDatBufferPtr;
					g_flashSoundDelay = randDist;

					int32_t angle = (Camera::g_activeCameraTransform.angles.yaw + randAngle - 128) & 0xFFF;
					++g_randDatBufferPtr;

					g_flashSoundPos.x = Camera::g_activeCameraTransform.pos.x + ((randDist * Numerics::g_sinCosLUT[angle]) >> 5);
					g_flashSoundPos.y = Camera::g_activeCameraTransform.pos.y;
					g_flashSoundPos.z = Camera::g_activeCameraTransform.pos.z + ((randDist * Numerics::g_sinCosLUT[(angle + 1024) & 0xFFF]) >> 5);
				}

				if (Nu3D::Camera::g_targetTintFadeSpeed)
				{
					Nu3D::Camera::g_cameraTintBlue = 128;
					Nu3D::Camera::g_cameraTintGreen = 128;
					Nu3D::Camera::g_cameraTintRed = 128;
					g_flashTimer = 10000;
				}
				else if (g_flashTimer < 0)
				{
					g_flashTimer = 2 * *g_randDatBufferPtr++ + 32;
				}
				else if (Nu3D::Camera::g_cameraTintRed > 64)
				{
					Nu3D::Camera::g_cameraTintBlue = 3 * g_flashTimer + 128;
					Nu3D::Camera::g_cameraTintGreen = 3 * g_flashTimer + 128;
					Nu3D::Camera::g_cameraTintRed = 3 * g_flashTimer + 128;
				}
			}

			g_flashSoundDelay -= Renderer::g_frameDelta;

			if (g_flashSoundDelay <= 0)
			{
				g_flashSoundDelay = 20000;
				AudioManager::PlaySoundEffect(112, &g_flashSoundPos);
			}
		}

	}
}

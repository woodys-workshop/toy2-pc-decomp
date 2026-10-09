#include "Toy2/Levels/EvilEmperorZurg.h"
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
#include "Numerics.h"
#include "SaveManager.h"
#include "Nullsub.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace Toy2
{
	namespace EvilEmperorZurg
	{
		// GLOBAL: TOY2 0x0052FE00
		int32_t g_unk52FE00;

		// GLOBAL: TOY2 0x0052FE04
		int32_t g_unk52FE04;

		// GLOBAL: TOY2 0x0052FE08
		int32_t g_unk52FE08;

		// GLOBAL: TOY2 0x0052FE20
		int32_t g_unk52FE20;

		// GLOBAL: TOY2 0x0052FE24
		int32_t g_unk52FE24;

		// GLOBAL: TOY2 0x0052FE28
		int32_t g_unk52FE28;

		// GLOBAL: TOY2 0x0052FE2C
		int32_t g_unk52FE2C;

		// GLOBAL: TOY2 0x0052FE30
		int32_t g_unk52FE30;

		// FUNCTION: TOY2 0x0042B2D0
		void BuzzRespawn()
		{
			g_buzzActor.respawnPos.x = -121468;
			g_buzzActor.respawnPos.y = -76752;
			g_buzzActor.respawnPos.z = 63129;
			g_buzzActor.respawnYawAngle = 1024;
		}

		// FUNCTION: TOY2 0x0042B3A0
		void Interactions()
		{
			int32_t v0;
			Levels::RecordData* v1;
			int32_t v2;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			bool v6;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			int32_t v11;
			int32_t v13;
			int32_t v14;
			int32_t v15;
			int32_t v17;
			int32_t y;
			int32_t v19;
			int32_t v20;
			int32_t v21;
			int32_t v22;
			int32_t v23;
			int32_t v24;
			int16_t v25;
			int32_t v26;
			int32_t v27;
			int32_t v28;
			int32_t v29;
			int32_t v30;
			int32_t v31;
			Vector3I spawnX;

			v0 = 0;
			AudioManager::PlaySoundEffect(103, 0);

			v1 = Levels::g_recordData[0];
			v29 = 0x7FFFFFFF;
			v28 = 0;

			if (Levels::g_recordData[0]->recordCount)
			{
				v2 = 0;
				v30 = 0;
				do
				{
					v3 = 32 * v1->data[v28].y;
					v4 = 32 * v1->data[v28].z;
					v5 = 32 * v1->data[v28].x;

					if (((Camera::g_activeCameraTransform.pos.x - v5) >> 8) * ((Camera::g_activeCameraTransform.pos.x - v5) >> 8)
							+ ((Camera::g_activeCameraTransform.pos.y - v3) >> 8) * ((Camera::g_activeCameraTransform.pos.y - v3) >> 8)
							+ ((Camera::g_activeCameraTransform.pos.z - v4) >> 8) * ((Camera::g_activeCameraTransform.pos.z - v4) >> 8)
						< 1000000)
					{
						Renderer::LensFlare::RegisterLight(v5, v3, v4, 128, 128, 128, 64);
						if (((g_buzzActor.posAngles.pos.y - v3 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v3 - 0x2000) >> 8)
								+ ((g_buzzActor.posAngles.pos.z - v4) >> 8) * ((g_buzzActor.posAngles.pos.z - v4) >> 8)
								+ ((g_buzzActor.posAngles.pos.x - v5) >> 8) * ((g_buzzActor.posAngles.pos.x - v5) >> 8)
							< v29)
						{
							v29 = ((g_buzzActor.posAngles.pos.y - v3 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v3 - 0x2000) >> 8)
								+ ((g_buzzActor.posAngles.pos.z - v4) >> 8) * ((g_buzzActor.posAngles.pos.z - v4) >> 8)
								+ ((g_buzzActor.posAngles.pos.x - v5) >> 8) * ((g_buzzActor.posAngles.pos.x - v5) >> 8);
							v31 = v28;
						}

						v1 = Levels::g_recordData[0];
					}

					v2 = v30 + 12;
					v6 = ++v28 < v1->recordCount;
					v30 += 12;
				} while (v6);

				if (v29 < 0x10000)
				{
					v0 = 0;
					Lighting::g_dynamicLights[1].pos.x = 32 * v1->data[v31].x;
					Lighting::g_dynamicLights[1].pos.y = 32 * v1->data[v31].y;
					Lighting::g_dynamicLights[1].pos.z = 32 * v1->data[v31].z;
					Lighting::g_dynamicLights[1].colorR = -1;
					Lighting::g_dynamicLights[1].colorG = -1;
					Lighting::g_dynamicLights[1].colorB = -1;
					Lighting::g_dynamicLights[1].lifetime = 1;
					Lighting::g_dynamicLights[1].groupId = (int32_t)&v1->data[v31];

					goto LABEL_12;
				}

				v0 = 0;
			}

			Lighting::g_dynamicLights[1].lifetime = 0;
		LABEL_12:

			v8 = (g_unk52FE28 - 1) & 1;
			v9 = (int32_t)Renderer::g_frameDelta;

			g_unk52FE28 = v8;

			if (g_unk52FE24 >= 2)
			{
				g_unk52FE04 -= (int32_t)Renderer::g_frameDelta;

				if (g_unk52FE04 >= 0)
				{
					if (v8)
					{
						g_levelActors[0].useTint = 1;
						g_levelActors[0].actorTint.r = 0x2000;
						g_levelActors[0].actorTint.g = 0x2000;
						g_levelActors[0].actorTint.b = 0x2000;
					}
					else
					{
						g_levelActors[0].useTint = 0;
					}
				}
				else
				{
					g_unk52FE04 = 0;
					*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode = 6;

					v9 = (int32_t)Renderer::g_frameDelta;
					g_levelActors[0].useTint = 0;
				}
			}

			if (! g_unk52FE24)
			{
				if (g_buzzActor.posAngles.pos.x <= -101163)
					goto LABEL_90;

				AudioManager::PlayMusicLooping(g_levelIndex);
				g_unk52FE24 = 1;
				Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].pos, 300, 16);

				Camera::g_cutsceneEyePos.y = g_levelActors[0].pos.y - 20480;
				Camera::g_cutsceneFocusPos.y = g_levelActors[0].pos.y - 0x4000;
				Camera::g_cutsceneEyePos.x = g_levelActors[0].pos.x - 12288;
				Camera::g_cutsceneEyePos.z = g_levelActors[0].pos.z;

				AudioManager::PlaySoundEffect(216, (Vector3I*)1);
				v9 = (int32_t)Renderer::g_frameDelta;
			}

			if (g_unk52FE24 == 1)
			{
				if (g_unk52FE20 > 0)
				{
					g_unk52FE20 -= v9;

					if (g_unk52FE20 <= 0)
					{
						AudioManager::Preset::PlayOneShotSound(185, g_levelActors);
						v9 = (int32_t)Renderer::g_frameDelta;
					}
				}

				if (g_buzzActor.posAngles.pos.y > -70000)
				{
					g_buzzActor.posAngles.pos.x = -121468;
					g_buzzActor.posAngles.pos.y = -76752;
					g_buzzActor.posAngles.pos.z = 63129;
				}
				if (Camera::g_cutsceneTimer)
				{
					g_levelActors[0].pos.y += v9 << 9;

					Camera::g_cutsceneEyePos.y += 544 * v9;
					v10 = -32 * v9 + Camera::g_cutsceneEyePos.x;
					Camera::g_cutsceneEyePos.x = v10;

					if (Camera::g_cutsceneTimer < 180)
						Camera::g_cutsceneEyePos.x = -336 * v9 + v10;

					g_levelActors[0].motionTargetPos.y = g_levelActors[0].pos.y;
					Camera::g_cutsceneFocusPos.y = g_levelActors[0].pos.y - 0x4000;
				}
				else
				{
					*(uint8_t*)&g_levelActors[0].actorFlags |= 0xCu;
					g_unk52FE24 = 2;
					g_levelActors[0].creatureRam->rotSpeed = 10;
					*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode = 6;
					v9 = (int32_t)Renderer::g_frameDelta;
				}
			}

			if (g_unk52FE24 == 2)
			{
				g_unk52FE08 -= v9;

				if (g_unk52FE08 < 0)
				{
					v11 = *g_randDatBufferPtr++;
					g_unk52FE08 = 2 * v11 + 400;
					v26 = (*g_randDatBufferPtr++ & 3) + 186;

					AudioManager::Preset::PlayOneShotSound(v26, g_levelActors);
					v9 = (int32_t)Renderer::g_frameDelta;
				}

				Camera::g_unk50A118 = g_levelActors[0].pos;
				g_hudWidgetTimers[7] = 90;

				if (g_levelActors[0].actorPhase != g_unk52FE00)
				{
					if (! (*g_randDatBufferPtr++ & 3))
						AudioManager::Preset::PlayOneShotSound(217, (Actor::Toy2Actor*)&g_buzzActor);

					g_unk52FE00 = g_levelActors[0].actorPhase;
					g_unk52FE04 = 60;
					*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode = 4;

					if (g_levelActors[0].actorPhase < 10 && g_unk52FE24 == 2)
					{
						g_unk52FE24 = 3;
						Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].pos, 300, 16);

						Camera::g_cutsceneFocusPos.y = g_levelActors[0].pos.y - 0x4000;
						Camera::g_cutsceneEyePos.y = g_levelActors[0].pos.y - 0x4000;
						Camera::g_cutsceneEyePos.x = g_levelActors[0].pos.x + Numerics::g_sinCosLUT[g_levelActors[0].yawAngle];

						Camera::g_cutsceneEyePos.z = g_levelActors[0].pos.z + Numerics::g_sinCosLUT[(g_levelActors[0].yawAngle + 1024) & 0xFFF];

						Actor::SetAnimation(g_levelActors, 2, 25);
						*(uint8_t*)&g_levelActors[0].actorFlags &= 0xF3u;
						g_unk52FE30 = 30;
						((uint16_t*)&g_levelActors[0].areaIndex)[1] = -16192;

						g_levelActors[0].creatureRam->latSpeedNoTarget = 32;
						g_levelActors[0].creatureRam->latSpeedTarget = 32;

						SaveManager::g_save0Data.tokens[g_levelFileIndex] = 0x80;
						AudioManager::Preset::PlayOneShotSound(192, g_levelActors);
					}
					else
					{
						v27 = (*g_randDatBufferPtr++ & 1) + 190;
						AudioManager::Preset::PlayOneShotSound(v27, g_levelActors);
						g_unk52FE30 = 0;
						Actor::g_unk52FE34 = 1;
						Actor::SetAnimation(g_levelActors, 1, 2);
						((uint16_t*)&g_levelActors[0].areaIndex)[1] = -7968;
					}

					v9 = (int32_t)Renderer::g_frameDelta;
				}

				v13 = g_unk52FE30;
				v14 = g_unk52FE30 - v9;
				g_unk52FE30 -= v9;

				if (g_unk52FE30 < 0)
				{
					g_unk52FE30 = 302;
					v15 = g_levelActors[0].yawAngle + 512;

					if (*g_randDatBufferPtr++ < 0x80u)
						v15 = g_levelActors[0].yawAngle - 512;

					v17 = Numerics::g_sinCosLUT[(v15 + 1024) & 0xFFF] >> 3;
					g_levelActors[0].velX = Numerics::g_sinCosLUT[v15] >> 3;
					g_levelActors[0].velForward = v17;
					goto LABEL_69;
				}

				if (v13 > 200 && v14 <= 200)
				{
					Actor::SetAnimation(g_levelActors, 0, 24);
					v14 = g_unk52FE30;
					((uint16_t*)&g_levelActors[0].areaIndex)[1] = -16192;
				}

				if (v13 > 146 && v14 <= 146)
					v0 = 1;

				if (v13 > 138 && v14 <= 138)
					v0 = 2;

				if (v13 <= 126 || v14 > 126)
				{
					if (! v0)
						goto LABEL_66;
				}
				else
				{
					v0 = 1;
				}

				spawnX.x = -250;
				spawnX.y = -250;
				spawnX.z = 0;

				Actor::ResolveBoneAttachmentPos(&spawnX, g_levelActors, 1);

				if (Actor::g_unk52FE34 || g_levelActors[0].actorPhase < 20 && v0 == 2)
					Nu3D::Particles::SpawnInstance(spawnX.x, spawnX.y, spawnX.z, 0, -2, 0, 4 * g_levelActors[0].yawAngle, 0, 0, 109);
				else
					Nu3D::Particles::SpawnInstance(
						spawnX.x,
						spawnX.y,
						spawnX.z,
						Numerics::g_sinCosLUT[(g_levelActors[0].yawAngle - 128) & 0xFFF] >> 2,
						1024,
						Numerics::g_sinCosLUT[g_levelActors[0].yawAngle + 896] >> 2,
						256,
						0,
						0,
						108);

				AudioManager::PlaySoundEffect(157, &g_levelActors[0].pos);
				v14 = g_unk52FE30;
			LABEL_66:

				if (v13 > 104 && v14 <= 104)
				{
					Actor::g_unk52FE34 = 0;
					Actor::SetAnimation(g_levelActors, 1, 2);
					((uint16_t*)&g_levelActors[0].areaIndex)[1] = -7968;
				}
			}
		LABEL_69:

			if (g_unk52FE24 != 3)
				goto LABEL_90;

			if (g_buzzActor.posAngles.pos.y > -70000)
			{
				g_buzzActor.posAngles.pos.x = -121468;
				g_buzzActor.posAngles.pos.y = -76752;
				g_buzzActor.posAngles.pos.z = 63129;
			}

			g_levelActors[0].targetYaw = Nu3D::Math::CartesianToFixedAngle((-48508 - g_levelActors[0].pos.x) >> 5, (153 - g_levelActors[0].pos.z) >> 5) & 0xFFF;

			if (Actor::IsInsideBounds(g_levelActors, -153302, 57514, -111593, 111255))
			{
				v19 = -Numerics::g_sinCosLUT[((uint16_t)(g_levelActors[0].targetYaw) + 1024) & 0xFFF] >> 4;
				g_levelActors[0].velX = -Numerics::g_sinCosLUT[g_levelActors[0].targetYaw] >> 4;
				g_levelActors[0].velForward = v19;
			}
			else
			{
				g_levelActors[0].creatureRam->rotSpeed = 0;
				g_levelActors[0].yawAngle = (16 * (int32_t)Renderer::g_frameDelta + g_levelActors[0].yawAngle) & 0xFFF;

				y = g_levelActors[0].pos.y;

				if (g_levelActors[0].pos.y < 170000)
				{
					g_levelActors[0].pos.y += (int32_t)Renderer::g_frameDelta * g_unk52FE2C;
					y = g_levelActors[0].pos.y;

					if (g_levelActors[0].pos.y >= 170000)
					{
						AudioManager::PlaySoundEffect(159, 0);
						y = g_levelActors[0].pos.y;
					}

					g_levelActors[0].motionTargetPos.y = y;
					g_unk52FE2C += 64;
				}

				if (y <= 70000)
					goto LABEL_84;

				AudioManager::PlaySoundEffect(158, (Vector3I*)1);

				y = g_levelActors[0].pos.y;

				if (g_levelActors[0].pos.y <= 70000 || ! g_tickState[4] || Camera::g_cutsceneTimer <= 50)
					goto LABEL_84;

				v25 = 16 * *g_randDatBufferPtr++;

				Nu3D::Particles::SpawnInstance(g_levelActors[0].pos.x, 80000, g_levelActors[0].pos.z, 0, -9216, 0, 0, v25, 128, 112);
			}

			y = g_levelActors[0].pos.y;
		LABEL_84:

			Camera::g_cutsceneFocusPos.y = y - 0x4000;
			Camera::g_cutsceneFocusPos.x = g_levelActors[0].pos.x;
			Camera::g_cutsceneFocusPos.z = g_levelActors[0].pos.z;

			v20 = g_unk52FE2C / 64;
			if (g_unk52FE2C / 64)
			{
				if (v20 > 8)
					v20 = 8;
			}
			else
			{
				v20 = 1;
			}

			Camera::g_cutsceneEyePos.y = -100576;
			Camera::g_cutsceneEyePos.x = g_levelActors[0].pos.x + Numerics::g_sinCosLUT[g_levelActors[0].yawAngle] / v20;
			Camera::g_cutsceneEyePos.z = g_levelActors[0].pos.z + Numerics::g_sinCosLUT[(g_levelActors[0].yawAngle + 1024) & 0xFFF] / v20;

			if (! Camera::g_cutsceneTimer)
			{
				g_unk830CC4 = 1;
				g_unk52FE24 = 4;
			}
		LABEL_90:

			g_unk52B7E0 = 54 * (g_levelActors[0].actorPhase - 9) / 20;
			v21 = (-48508 - g_levelActors[0].pos.x) >> 5;

			if (v21 * v21 + ((153 - g_levelActors[0].pos.z) >> 5) * ((153 - g_levelActors[0].pos.z) >> 5) < 1638400)
			{
				v22 = Nu3D::Math::CartesianToFixedAngle(v21, (153 - g_levelActors[0].pos.z) >> 5) & 0xFFF;
				v23 = -48508 - ((1280 * Numerics::g_sinCosLUT[v22]) >> 9);
				v24 = (1280 * Numerics::g_sinCosLUT[(v22 + 1024) & 0xFFF]) >> 9;

				g_levelActors[0].pos.x = v23;
				g_levelActors[0].pos.z = 153 - v24;
			}
		}

	}
}

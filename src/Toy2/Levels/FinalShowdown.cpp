#include "Toy2/Levels/FinalShowdown.h"
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
	namespace FinalShowdown
	{
		// GLOBAL: TOY2 0x0052FFF0
		int32_t g_unk52FFF0;

		// GLOBAL: TOY2 0x0052FFF8
		int32_t g_unk52FFF8;

		// GLOBAL: TOY2 0x00530000
		int32_t g_unk530000;

		// GLOBAL: TOY2 0x00530014
		int32_t g_unk530014;

		// GLOBAL: TOY2 0x00530024
		int32_t g_unk530024;

		// GLOBAL: TOY2 0x00530048
		int32_t g_unk530048;

		// GLOBAL: TOY2 0x00530058
		int32_t g_unk530058;

		// GLOBAL: TOY2 0x00530068
		int32_t g_unk530068;

		// GLOBAL: TOY2 0x00530038
		Vector3I g_unk530038;

		// GLOBAL: TOY2 0x0052FFF4
		int32_t g_unk52FFF4;

		// GLOBAL: TOY2 0x0052FFFC
		int32_t g_unk52FFFC;

		// GLOBAL: TOY2 0x00530004
		int32_t g_unk530004;

		// GLOBAL: TOY2 0x00530008
		int32_t g_unk530008;

		// GLOBAL: TOY2 0x0053000C
		int32_t g_unk53000C;

		// GLOBAL: TOY2 0x00530018
		int32_t g_unk530018;

		// GLOBAL: TOY2 0x0053001C
		int32_t g_unk53001C;

		// GLOBAL: TOY2 0x00530020
		int32_t g_unk530020;

		// GLOBAL: TOY2 0x00530028
		int32_t g_unk530028;

		// GLOBAL: TOY2 0x0053002C
		int32_t g_unk53002C;

		// GLOBAL: TOY2 0x00530030
		int32_t g_unk530030;

		// GLOBAL: TOY2 0x0053004C
		int32_t g_unk53004C;

		// GLOBAL: TOY2 0x00530050
		int32_t g_unk530050;

		// GLOBAL: TOY2 0x00530054
		int32_t g_unk530054;

		// GLOBAL: TOY2 0x0053005C
		int32_t g_unk53005C;

		// GLOBAL: TOY2 0x00530060
		int32_t g_unk530060;

		// GLOBAL: TOY2 0x00530064
		int32_t g_unk530064;

		// FUNCTION: TOY2 0x0042FC50
		void Interactions()
		{
			int32_t v0;
			int32_t v1;
			int32_t v2;
			int32_t v3;
			bool v4;
			int32_t z;
			int32_t v6;
			int32_t x;
			int32_t v8;
			int32_t v9;
			int32_t y;
			int32_t v11;
			int32_t v12;
			int32_t v13;
			int32_t v14;
			int32_t v15;
			int32_t v16;
			int32_t v17;
			int32_t v18;
			int32_t v19;
			int32_t v20;
			int16_t v21;
			int32_t v22;
			int16_t v23;
			int16_t v24;
			Levels::RecordData* v25;
			int32_t v26;
			int32_t v27;
			int32_t v28;
			int32_t v29;
			int32_t v30;
			int32_t v31;
			int32_t v32;
			int32_t p_y;
			int32_t p_ya;
			int32_t p_x;
			int32_t p_xa;
			int32_t v38;
			Vector3I v39;

			v0 = g_unk52FFF8;

			if (g_unk52FFF8)
			{
				v1 = g_unk52FFF8;
				g_unk530014 = g_unk52FFF8;

				if (g_unk52FFF8 < 1000)
				{
					v0 = (int32_t)Renderer::g_frameDelta + g_unk52FFF8;
					g_unk52FFF8 += (int32_t)Renderer::g_frameDelta;
				}

				if (v0 > 150 && v1 <= 150)
				{
					AudioManager::Preset::PlayOneShotSound(205, &g_levelActors[3]);
					v1 = g_unk530014;
				}

				if (g_unk52FFF8 > 280 && v1 <= 280)
				{
					g_unk530058 = -3072;
					g_unk530024 = 2048;
					g_unk52FFF0 = 0;
					g_levelActors[3].gravityVel = -768;
					g_levelActors[4].gravityVel = -1024;
					AudioManager::PlaySoundEffect(47, 0);
					v1 = g_unk530014;
				}

				if (g_unk52FFF8 > 335 && v1 <= 335)
				{
					g_unk530058 = -3072;
					g_unk530024 = 2048;
					g_unk52FFF0 = 1;
					g_levelActors[3].gravityVel = -1024;
					g_levelActors[4].gravityVel = -768;
					AudioManager::PlaySoundEffect(47, 0);
					v1 = g_unk530014;
				}

				if (g_unk52FFF8 > 360 && v1 <= 360)
				{
					g_unk530058 = -3072;
					g_unk530024 = 2048;
					g_unk52FFF0 = 2;
					g_levelActors[3].gravityVel = -1792;
					g_levelActors[3].velX = -1024;
					g_levelActors[3].motionTargetPos.y = 0;
					g_levelActors[4].gravityVel = -1920;
					g_levelActors[4].velX = -1024;
					g_levelActors[4].motionTargetPos.y = 0;
					AudioManager::PlaySoundEffect(47, 0);
					v1 = g_unk530014;
				}

				if (g_unk52FFF8 > 430 && v1 <= 430)
				{
					g_unk530058 = -3072;
					g_unk530024 = 2048;
					g_unk52FFF0 = 3;
					g_levelActors[0].gravityVel = -1792;
					g_levelActors[1].gravityVel = -1536;
					g_levelActors[2].gravityVel = -2048;
					g_levelActors[0].actorPhase = 29;
					g_levelActors[1].actorPhase = 29;
					g_levelActors[2].actorPhase = 29;
					g_levelActors[3].actorPhase = 0;
					g_levelActors[4].actorPhase = 0;
					AudioManager::PlaySoundEffect(10, 0);
				}

				g_unk530058 += 512;

				if (g_unk530058 > 3072)
					g_unk530058 = 3072;

				Nu3D::Link::GetCurrentPosFixed(0, &v39);
				v2 = g_unk530058 + v39.y;
				v39.y += g_unk530058;

				if (v39.y > g_unk530048)
				{
					v2 = g_unk530048;
					v39.y = g_unk530048;
				}

				Nu3D::Link::SetPositionRawAndCommit(0, v39.x >> 5, v2 >> 5, v39.z >> 5);
				Nu3D::Link::SetPositionRawAndCommit(1, v39.x >> 7, v39.y >> 7, v39.z >> 7);
				Nu3D::Link::SetPositionRawAndCommit(3, v39.x >> 5, v39.y >> 5, v39.z >> 5);
				Nu3D::Link::SetPositionRawAndCommit(4, v39.x >> 7, v39.y >> 7, v39.z >> 7);

				if (g_unk530024)
				{
					v3 = g_unk530024 - 256;
					g_unk530024 = v3;

					if (v3 < 0)
					{
						v3 = 0;
						g_unk530024 = 0;
					}

					switch (g_unk52FFF0)
					{
						case 0:
							Nu3D::Link::SetRotationAbsolute8bit(0, Numerics::g_sinCosLUT[v3 & 0xFFF] >> 7, 0, 0);
							Nu3D::Link::SetRotationAbsolute8bit(1, Numerics::g_sinCosLUT[g_unk530024 & 0xFFF] >> 7, 0, 0);
							Nu3D::Link::SetRotationAbsolute8bit(2, Numerics::g_sinCosLUT[g_unk530024 & 0xFFF] >> 7, 0, 0);
							Nu3D::Link::SetRotationAbsolute8bit(3, Numerics::g_sinCosLUT[g_unk530024 & 0xFFF] >> 7, 0, 0);
							break;
						case 1:
							Nu3D::Link::SetRotationAbsolute8bit(0, 0, 0, -Numerics::g_sinCosLUT[v3 & 0xFFF] >> 8);
							Nu3D::Link::SetRotationAbsolute8bit(1, 0, 0, -Numerics::g_sinCosLUT[g_unk530024 & 0xFFF] >> 8);
							Nu3D::Link::SetRotationAbsolute8bit(2, 0, 0, -Numerics::g_sinCosLUT[g_unk530024 & 0xFFF] >> 8);
							Nu3D::Link::SetRotationAbsolute8bit(3, 0, 0, -Numerics::g_sinCosLUT[g_unk530024 & 0xFFF] >> 8);
							break;
						case 2:
							Nu3D::Link::SetRotationAbsolute8bit(0, -Numerics::g_sinCosLUT[v3 & 0xFFF] >> 7, 0, 0);
							Nu3D::Link::SetRotationAbsolute8bit(1, -Numerics::g_sinCosLUT[g_unk530024 & 0xFFF] >> 7, 0, 0);
							Nu3D::Link::SetRotationAbsolute8bit(2, -Numerics::g_sinCosLUT[g_unk530024 & 0xFFF] >> 7, 0, 0);
							Nu3D::Link::SetRotationAbsolute8bit(3, -Numerics::g_sinCosLUT[g_unk530024 & 0xFFF] >> 7, 0, 0);
							break;
						case 3:
							Nu3D::Link::SetRotationAbsolute8bit(0, 0, 0, -Numerics::g_sinCosLUT[((v3 >> 1) + 1024) & 0xFFF] >> 4);
							Nu3D::Link::SetRotationAbsolute8bit(1, 0, 0, -Numerics::g_sinCosLUT[((g_unk530024 >> 1) + 1024) & 0xFFF] >> 4);
							break;
						default:
							break;
					}
				}
			}

			if (g_buzzActor.posAngles.pos.x < -93145 && ! g_unk52FFF8)
			{
				g_unk52FFF8 = 80;
				Camera::BeginScriptedCutsceneAtPoint(&g_unk530038, 480, 56);
				Camera::g_cutsceneEyePos.y -= 0x4000;
				Camera::g_cutsceneFocusPos.y -= 0x2000;
				Camera::InitCutsceneCamera(&Camera::g_cutsceneFocusPos, &Camera::g_cutsceneEyePos);
			}

			if (Camera::g_cutsceneTimer > 240 && Camera::g_cutsceneTimer < 360 && g_unk530064 < 3)
				Camera::g_cutsceneEyePos.x += 384 * (int32_t)Renderer::g_frameDelta;

			if (! g_unk52FFF8 || Camera::g_cutsceneTimer >= 30)
				goto LABEL_42;

			v4 = g_unk530008 <= 0;

			if (! g_unk530008)
			{
				AudioManager::PlayMusicLooping(g_levelIndex);
				g_unk530008 = 2;
				g_unk53000C = 2;
				g_unk530004 = 2;

				g_levelActors[0].unkShort1 = 0;
				g_levelActors[0].movementData = Actor::g_movementData[39] + 14;
				g_levelActors[0].creatureRam->initialFacingAngle = 0;
				g_levelActors[1].unkShort1 = 0;
				g_levelActors[1].movementData = Actor::g_movementData[33] + 14;
				g_levelActors[1].creatureRam->initialFacingAngle = 0;
				g_levelActors[2].unkShort1 = 0;
				g_levelActors[2].movementData = Actor::g_movementData[41] + 14;
				g_levelActors[2].creatureRam->initialFacingAngle = 0;
				g_levelActors[0].gravityVel = -1536;
				g_levelActors[1].gravityVel = -2048;
				g_levelActors[2].gravityVel = -1792;
				g_levelActors[0].velX = 1024;
				g_levelActors[1].velX = 1024;
				g_levelActors[2].velX = 1024;

				AudioManager::PlaySoundEffect(14, 0);
				AudioManager::Preset::PlayOneShotSound(199, &g_levelActors[2]);
			LABEL_42:
				v4 = g_unk530008 <= 0;
			}

			if (! v4)
			{
				z = g_levelActors[0].transform.pos.z;
				v6 = g_levelActors[1].transform.pos.z;
				x = g_levelActors[1].transform.pos.x;

				if (((g_levelActors[0].transform.pos.x - g_levelActors[1].transform.pos.x)
					 >> 5) * ((g_levelActors[0].transform.pos.x - g_levelActors[1].transform.pos.x) >> 5)
						+ ((g_levelActors[0].transform.pos.y - g_levelActors[1].transform.pos.y) >> 5)
							* ((g_levelActors[0].transform.pos.y - g_levelActors[1].transform.pos.y) >> 5)
						+ ((g_levelActors[0].transform.pos.z - g_levelActors[1].transform.pos.z) >> 5)
							* ((g_levelActors[0].transform.pos.z - g_levelActors[1].transform.pos.z) >> 5)
					>= 0x40000)
				{
					y = g_levelActors[1].transform.pos.y;
					v9 = g_levelActors[0].transform.pos.x;
				}
				else
				{
					v8 = Nu3D::Math::CartesianToFixedAngle(
							 (g_levelActors[0].transform.pos.x - g_levelActors[1].transform.pos.x) >> 5,
							 (g_levelActors[0].transform.pos.z - g_levelActors[1].transform.pos.z) >> 5)
						& 0xFFF;

					v9 = g_levelActors[0].transform.pos.x;
					z = g_levelActors[0].transform.pos.z;
					y = g_levelActors[1].transform.pos.y;

					x = g_levelActors[0].transform.pos.x - (Numerics::g_sinCosLUT[v8] << 9 >> 9);
					v11 = Numerics::g_sinCosLUT[(v8 + 1024) & 0xFFF] << 9 >> 9;
					g_levelActors[1].transform.pos.x = x;
					v6 = g_levelActors[0].transform.pos.z - v11;

					g_levelActors[1].transform.pos.z = g_levelActors[0].transform.pos.z - v11;
				}

				v12 = g_levelActors[2].transform.pos.x;
				p_x = (x - g_levelActors[2].transform.pos.x) >> 5;
				v13 = (y - g_levelActors[2].transform.pos.y) >> 5;
				v14 = g_levelActors[2].transform.pos.z;
				p_y = (v6 - g_levelActors[2].transform.pos.z) >> 5;

				if (p_x * p_x + v13 * v13 + p_y * p_y >= 0x40000)
				{
					v16 = g_levelActors[0].transform.pos.y;
				}
				else
				{
					v15 = Nu3D::Math::CartesianToFixedAngle(p_x, p_y) & 0xFFF;
					z = g_levelActors[0].transform.pos.z;
					v9 = g_levelActors[0].transform.pos.x;

					v12 = g_levelActors[1].transform.pos.x - (Numerics::g_sinCosLUT[v15] << 9 >> 9);
					v14 = g_levelActors[1].transform.pos.z - (Numerics::g_sinCosLUT[(v15 + 1024) & 0xFFF] << 9 >> 9);
					v16 = g_levelActors[0].transform.pos.y;

					g_levelActors[2].transform.pos.x = v12;
					g_levelActors[2].transform.pos.z = v14;
				}

				v17 = (z - v14) >> 5;
				v18 = (v9 - v12) >> 5;

				if (v18 * v18 + ((v16 - g_levelActors[2].transform.pos.y) >> 5) * ((v16 - g_levelActors[2].transform.pos.y) >> 5) + v17 * v17 < 0x40000)
				{
					v19 = Nu3D::Math::CartesianToFixedAngle(v18, v17) & 0xFFF;
					v20 = g_levelActors[0].transform.pos.z - (Numerics::g_sinCosLUT[(v19 + 1024) & 0xFFF] << 9 >> 9);
					g_levelActors[2].transform.pos.x = g_levelActors[0].transform.pos.x - (Numerics::g_sinCosLUT[v19] << 9 >> 9);
					g_levelActors[2].transform.pos.z = v20;
				}
			}

			v21 = 1024;

			if (g_unk530008 > 0)
			{
				g_unk530000 -= (int32_t)Renderer::g_frameDelta;

				if (g_unk530000 <= 0)
				{
					g_unk530000 = 0;
					g_unk530068 += 8 * (int32_t)Renderer::g_frameDelta;

					v21 = g_unk530068;

					if (g_unk530068 < 12288)
					{
						if (g_unk530068 >= 2048)
						{
							if (g_unk530068 >= 6144)
							{
								if (g_unk530068 >= 0x2000)
									v21 = 0;
								else
									v21 = g_unk530068 - 4096;
							}
							else
							{
								v21 = 2048;
							}
						}
					}
					else
					{
						v21 = 0;
						g_unk530068 = 0;
					}
				}
			}

			v22 = Numerics::g_sinCosLUT[(v21 + 1024) & 0xFFF] >> 6;
			v23 = 384 - v22;

			v24 = v22 & 0xFFF;

			AudioManager::g_pitchArray[0] = 8 * v23;

			AudioManager::PlaySoundEffect(168, 0);

			if (Camera::g_cameraState)
				v24 = 0;

			Camera::g_gameplayCameraState.roll = v24;

			if (g_unk530008)
			{
				g_hudWidgetTimers[7] = 90;
				g_unk52B7E0 = 54 * (g_levelActors[1].actorPhase + g_levelActors[2].actorPhase + g_levelActors[0].actorPhase - 27) / 60;
			}

			if (g_unk530064 == 3)
			{
				g_unk530064 = 4;
				Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[g_unk52FFF4].transform.pos, 120, 32);
				Camera::g_cutsceneFocusPos.y -= 0x2000;
				Camera::g_cutsceneEyePos.y -= 0x8000;
			}

			if (g_unk530064 == 4)
			{
				if (Camera::g_cutsceneTimer)
					goto LABEL_74;

				g_levelActors[3].transform.angles.yaw = 1024;
				g_levelActors[3].targetYaw = 1024;
				g_levelActors[4].transform.angles.yaw = 1024;
				SaveManager::g_save0Data.tokens[g_levelFileIndex] = 0x80;
				g_levelActors[4].targetYaw = 1024;

				g_unk530064 = 5;
				g_levelActors[3].transform.pos.x = -320736;
				g_levelActors[3].transform.pos.y = -48256;
				g_levelActors[3].transform.pos.z = 5344;
				g_levelActors[4].transform.pos.x = -289092;
				g_levelActors[4].transform.pos.y = -46720;
				g_levelActors[4].transform.pos.z = -7146;
				g_levelActors[3].actorPhase = 1;
				g_levelActors[4].actorPhase = 1;
				g_levelActors[3].motionTargetPos.y = -48256;
				g_levelActors[4].motionTargetPos.y = -46720;

				Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[4].transform.pos, 300, 64);

				Camera::g_cutsceneFocusPos.y -= 0x2000;
				Camera::g_cutsceneEyePos.x = Camera::g_cutsceneFocusPos.x + 12288;
				Camera::g_cutsceneEyePos.y = Camera::g_cutsceneFocusPos.y - 4096;
				Camera::g_cutsceneEyePos.z = Camera::g_cutsceneFocusPos.z - 4096;

				Camera::InitCutsceneCamera(&Camera::g_cutsceneFocusPos, &Camera::g_cutsceneEyePos);
				g_levelTransition = 1;
				g_levelTransitionTimer = 240;
			}

			if (g_unk530064 == 5)
				Camera::g_cutsceneEyePos.z += 32 * (int32_t)Renderer::g_frameDelta;
		LABEL_74:

			v25 = Levels::g_recordData[0];
			v26 = 0;
			p_xa = 0x7FFFFFFF;
			p_ya = 0;

			if (Levels::g_recordData[0]->recordCount)
			{
				do
				{
					v27 = 32 * v25->data[v26].z;
					v28 = 32 * v25->data[v26].y;
					v29 = 32 * v25->data[v26].x;

					v30 = p_ya;

					if (((Camera::g_activeCameraTransform.pos.x - v29) >> 8) * ((Camera::g_activeCameraTransform.pos.x - v29) >> 8)
							+ ((Camera::g_activeCameraTransform.pos.y - v28) >> 8) * ((Camera::g_activeCameraTransform.pos.y - v28) >> 8)
							+ ((Camera::g_activeCameraTransform.pos.z - v27) >> 8) * ((Camera::g_activeCameraTransform.pos.z - v27) >> 8)
						< 0x100000)
					{
						if (p_ya == 2)
						{
							v31 = 0;
							v32 = 0;
						}
						else
						{
							v31 = 128;
							v32 = 128;
						}

						Renderer::LensFlare::RegisterLight(v29, v28, v27, 128, v31, v32, 128);
						if (((g_buzzActor.posAngles.pos.y - v28 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v28 - 0x2000) >> 8)
								+ ((g_buzzActor.posAngles.pos.z - v27) >> 8) * ((g_buzzActor.posAngles.pos.z - v27) >> 8)
								+ ((g_buzzActor.posAngles.pos.x - v29) >> 8) * ((g_buzzActor.posAngles.pos.x - v29) >> 8)
							< p_xa)
						{
							p_xa = ((g_buzzActor.posAngles.pos.y - v28 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v28 - 0x2000) >> 8)
								+ ((g_buzzActor.posAngles.pos.z - v27) >> 8) * ((g_buzzActor.posAngles.pos.z - v27) >> 8)
								+ ((g_buzzActor.posAngles.pos.x - v29) >> 8) * ((g_buzzActor.posAngles.pos.x - v29) >> 8);
							v38 = p_ya;
						}
					}

					v25 = Levels::g_recordData[0];
					v26 = ++p_ya;
				} while (v30 + 1 < Levels::g_recordData[0]->recordCount);
			}

			if (g_levelActors[0].transform.pos.x < -179670 && g_levelActors[0].velX < 0)
				g_levelActors[0].transform.pos.x = -179670;

			if (g_levelActors[1].transform.pos.x < -179670 && g_levelActors[1].velX < 0)
				g_levelActors[1].transform.pos.x = -179670;

			if (g_levelActors[2].transform.pos.x < -179670 && g_levelActors[2].velX < 0)
				g_levelActors[2].transform.pos.x = -179670;

			if (p_xa >= 0x10000)
			{
				Lighting::g_dynamicLights[1].lifetime = 0;
			}
			else
			{
				Lighting::g_dynamicLights[1].pos.x = 32 * v25->data[v38].x;
				Lighting::g_dynamicLights[1].pos.y = 32 * v25->data[v38].y;
				Lighting::g_dynamicLights[1].pos.z = 32 * v25->data[v38].z;
				Lighting::g_dynamicLights[1].colorR = -1;

				if (v38 == 2)
				{
					Lighting::g_dynamicLights[1].colorG = 0;
					Lighting::g_dynamicLights[1].colorB = 0;
				}
				else
				{
					Lighting::g_dynamicLights[1].colorG = -1;
					Lighting::g_dynamicLights[1].colorB = -1;
				}

				Lighting::g_dynamicLights[1].lifetime = 1;
				Lighting::g_dynamicLights[1].groupId = (int32_t)&v25->data[v38];
			}
		}

	}
}

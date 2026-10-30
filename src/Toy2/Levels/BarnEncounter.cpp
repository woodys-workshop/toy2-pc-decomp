#include "Toy2/Levels/BarnEncounter.h"
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
	namespace BarnEncounter
	{
		// GLOBAL: TOY2 0x004F2F5C
		int32_t g_unk4F2F5C[13] = { 8, 8, 7, 7, 8, 9, 10, 7, 11, 8, 11, 7, 0 };

		// GLOBAL: TOY2 0x0052FBB8
		int32_t g_unk52FBB8;

		// GLOBAL: TOY2 0x0052FC0C
		int32_t g_unk52FC0C;

		// GLOBAL: TOY2 0x0052FBEC
		int32_t g_unk52FBEC;

		// GLOBAL: TOY2 0x0052FC08
		int32_t g_unk52FC08;

		// GLOBAL: TOY2 0x0052FBCC
		int32_t g_unk52FBCC;

		// GLOBAL: TOY2 0x0052FC04
		int32_t g_unk52FC04;

		// GLOBAL: TOY2 0x0052FBC0
		int32_t g_unk52FBC0;

		// GLOBAL: TOY2 0x0052FBE8
		int32_t g_unk52FBE8;

		// GLOBAL: TOY2 0x0052FBD4
		int32_t g_unk52FBD4;

		// GLOBAL: TOY2 0x0052FBF0
		int32_t g_unk52FBF0;

		// GLOBAL: TOY2 0x0052FC00
		int32_t g_unk52FC00;

		// GLOBAL: TOY2 0x0052FBC8
		int32_t g_unk52FBC8;

		// GLOBAL: TOY2 0x0052FBC4
		int32_t g_unk52FBC4;

		// GLOBAL: TOY2 0x0052FBF4
		int32_t g_unk52FBF4;

		// GLOBAL: TOY2 0x0052FBBC
		int32_t g_unk52FBBC;

		// GLOBAL: TOY2 0x0052FBD0
		int32_t g_unk52FBD0;

		// GLOBAL: TOY2 0x0052FBF8
		int32_t g_unk52FBF8;

		// GLOBAL: TOY2 0x0052FBFC
		int32_t g_unk52FBFC;

		// FUNCTION: TOY2 0x00424490
		void Interactions()
		{
			int32_t x;
			int32_t z;
			int32_t v2;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			int32_t v6;
			int32_t v7;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			int32_t v11;
			int32_t v12;
			int32_t v13;
			int32_t v14;
			int32_t v15;
			int32_t v16;
			int32_t v17;
			int32_t v18;
			int16_t v22;
			int16_t v23;
			int32_t v24;
			uint32_t v26;
			int32_t v27;
			int16_t actorPhase;
			int32_t v29;
			int32_t v30;
			int32_t v31;
			int32_t v32;
			int16_t tick64Acc;
			int32_t v34;
			int32_t v35;
			int32_t v36;
			int16_t yawAngle;
			int32_t v38;
			int32_t v39;
			int32_t v40;
			int32_t v41;
			int32_t v42;
			int32_t v44;
			int32_t v45;
			int32_t v46;
			int32_t v47;
			int32_t v48;
			int32_t v49;
			int32_t i;
			Nu3D::Particles::ParticleInstance* v51;
			int32_t v52;
			int32_t v53;
			int32_t IsWithinDistance;
			int32_t v55;
			uint32_t v56;
			int32_t v57;
			int16_t v58;
			int32_t v59;
			int32_t v60;
			int32_t v61;
			int32_t v62;
			int16_t v63;
			int32_t v64;
			int32_t v65;
			int32_t v66;
			int16_t v67;
			bool v68;
			int32_t v69;
			int32_t v70;
			int32_t v71;
			int32_t v72;
			int32_t v73;
			int32_t v74;
			Transform a2;
			int32_t v76;
			int32_t v77;
			Transform pts[7];

			AudioManager::PlaySoundEffect(103, 0);

			x = g_levelActors[0].transform.pos.x;
			z = g_levelActors[0].transform.pos.z;
			v2 = g_levelActors[0].transform.pos.x >> 5;
			v3 = g_levelActors[0].transform.pos.z >> 5;

			v4 = g_unk52FC04 < 3 ? 4200 : 5000;

			if (v2 * v2 + v3 * v3 > v4 * v4)
			{
				v5 = Nu3D::Math::CartesianToFixedAngle(v2, v3) & 0xFFF;
				x = (v4 * Numerics::g_sinCosLUT[v5]) >> 9;
				z = (v4 * Numerics::g_sinCosLUT[(v5 + 1024) & 0xFFF]) >> 9;
				g_levelActors[0].transform.pos.x = x;
				g_levelActors[0].transform.pos.z = z;
			}

			if (g_unk52FBF4 > 0)
			{
				v6 = g_unk52FBC8;
				v7 = g_unk52FBC4;

				if ((Camera::g_inCutscene & 1) != 0)
				{
					v8 = g_unk52FBC8;
					g_levelActors[v8].velX = 0;
					g_levelActors[v8].velForward = 0;

					v9 = v7;
					g_levelActors[v9].velX = 0;
					g_levelActors[v9].velForward = 0;

					z = g_levelActors[0].transform.pos.z;
					x = g_levelActors[0].transform.pos.x;
				}

				v10 = g_levelActors[v6].transform.pos.x >> 5;
				v11 = g_levelActors[v6].transform.pos.z >> 5;

				if (v10 * v10 + v11 * v11 > 31360000)
				{
					v12 = Nu3D::Math::CartesianToFixedAngle(v10, v11) & 0xFFF;
					v7 = g_unk52FBC4;
					v13 = g_unk52FBC8;

					g_levelActors[v13].transform.pos.x = (5600 * Numerics::g_sinCosLUT[v12]) >> 9;
					g_levelActors[v13].transform.pos.z = (5600 * Numerics::g_sinCosLUT[(v12 + 1024) & 0xFFF]) >> 9;

					z = g_levelActors[0].transform.pos.z;
					x = g_levelActors[0].transform.pos.x;
				}

				v14 = g_levelActors[v7].transform.pos.x >> 5;

				if (v14 * v14 + (g_levelActors[v7].transform.pos.z >> 5) * (g_levelActors[v7].transform.pos.z >> 5) > 31360000)
				{
					v15 = Nu3D::Math::CartesianToFixedAngle(v14, g_levelActors[v7].transform.pos.z >> 5) & 0xFFF;
					v16 = g_unk52FBC4;

					g_levelActors[v16].transform.pos.x = (5600 * Numerics::g_sinCosLUT[v15]) >> 9;
					g_levelActors[v16].transform.pos.z = (5600 * Numerics::g_sinCosLUT[(v15 + 1024) & 0xFFF]) >> 9;

					z = g_levelActors[0].transform.pos.z;
					x = g_levelActors[0].transform.pos.x;
				}
			}

			v17 = g_unk52FC0C;
			v18 = g_unk52FBEC;

			v77 = x;
			v76 = z;
			for (i = 1; i <= 6; ++i)
			{
				g_levelActors[i].transform.pos.x = v77;
				g_levelActors[i].transform.pos.z = v76;
				g_levelActors[i].transform.angles.yaw = (int16_t)v17;
				g_levelActors[i].targetYaw = v17;

				if (g_levelActors[i].primaryAnimIdx == 0)
					g_levelActors[i].unkVar7 = v18;

				v18 += 786432;

				if (v18 >= 1572864)
					v18 -= 0x180000;

				pts[i].pos.x = 0;
				pts[i].pos.y = 150;
				v17 = (v17 + 683) & 0xFFF;
				pts[i].pos.z = 1450;

				Actor::ResolveBoneAttachmentPos((Vector3I*)&pts[i], &g_levelActors[i], 1);

				if (g_levelActors[i].actorPhase == 1)
				{
					Nu3D::Collision::GetGroundHeight(&pts[i], 150);
					if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, (Vector3I*)&pts[i], 75) != 0)
					{
						v22 = Nu3D::Math::CartesianToFixedAngle(g_buzzActor.posAngles.pos.x - pts[i].pos.x, g_buzzActor.posAngles.pos.z - pts[i].pos.z);
						Buzz::HandleDamage(v22, 1u);
					}
				}
			}

			v23 = g_unk52FC08;
			v24 = g_unk52FBCC + (Numerics::g_sinCosLUT[g_unk52FC08] >> 3);

			for (i = 0; i <= 6; ++i)
			{
				g_levelActors[i].transform.pos.y = v24;
				g_levelActors[i].motionTargetPos.y = v24;
			}

			v26 = (int32_t)Renderer::g_frameDelta;
			g_unk52FC0C = (g_unk52FC0C + 4 * (int32_t)Renderer::g_frameDelta) & 0xFFF;
			g_unk52FC08 = (v23 + 32 * (int32_t)Renderer::g_frameDelta) & 0xFFF;
			g_unk52FBEC += (int32_t)Renderer::g_frameDelta << 14;

			if (g_unk52FBEC >= 1572864)
				g_unk52FBEC -= 1572864;

			v27 = (g_unk52FBE8 - 1) & 1;
			g_unk52FBE8 = v27;

			if (g_levelActors[0].actorPhase != 0 && g_levelActors[0].actorPhase != g_unk52FBB8 && g_unk52FC04 < 4)
			{
				AudioManager::PlaySoundEffect(131, &g_levelActors[0].transform.pos);

				if (g_unk52FBD4 >= 6)
				{
					actorPhase = g_levelActors[0].actorPhase;
				}
				else
				{
					actorPhase = 26 - g_unk52FBD4;
					g_levelActors[0].actorPhase = 26 - g_unk52FBD4;
				}

				v29 = g_unk52FBF4;

				if (g_unk52FBF4 < 12)
				{
					v30 = g_unk4F2F5C[v29];
					g_unk52FBF4 += 2;
					g_unk52FBC8 = v30;
					g_unk52FBC4 = g_unk4F2F5C[v29 + 1];
				}

				g_unk52FBB8 = actorPhase;
				*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode = 4;

				if (g_unk52FC04 == 2)
				{
					g_levelActors[0].creatureRam->speedTarget = 0;
					g_unk52FBC0 = 600;
					Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].transform.pos, 360, 16);
				}
				else
				{
					g_unk52FBC0 = 120;
				}

				if (g_levelActors[0].actorPhase <= 10)
				{
					Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].transform.pos, 240, 16);
					v31 = Numerics::g_sinCosLUT[(g_buzzActor.posAngles.angles.yaw + 1280) & 0xFFF];

					Camera::g_cutsceneEyePos.x = Camera::g_cutsceneFocusPos.x + 4 * Numerics::g_sinCosLUT[(g_buzzActor.posAngles.angles.yaw + 256) & 0xFFF];
					Camera::g_cutsceneEyePos.y -= 0x8000;
					Camera::g_cutsceneEyePos.z = Camera::g_cutsceneFocusPos.z + 4 * v31;

					g_levelActors[0].creatureRam->speedTarget = 0;

					g_unk52FC00 = 10000;
					g_unk52FC04 = 4;
					g_unk52FBC0 = 0;

					SaveManager::g_save0Data.tokens[g_levelFileIndex] = 0x80;
					Camera::InitCutsceneCamera(&Camera::g_cutsceneFocusPos, &Camera::g_cutsceneEyePos);
				}

				v26 = (int32_t)Renderer::g_frameDelta;
				v27 = g_unk52FBE8;
			}

			g_unk52FBC0 -= v26;

			if (g_unk52FBC0 > 0)
			{
				if (g_unk52FC04 == 2)
				{
					tick64Acc = g_tickAccumulators.tick64Acc;

					if (g_tickAccumulators.tick64Acc > 0x1Fu)
						tick64Acc = 63 - g_tickAccumulators.tick64Acc;

					g_levelActors[0].useTint = 1;
					g_levelActors[0].actorTint.r = 4096;
					g_levelActors[0].actorTint.g = (tick64Acc + 32) << 7;
					g_levelActors[0].actorTint.b = 4096;
				}
				else if (v27 != 0)
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
				goto LABEL_48;
			}

			g_unk52FBC0 = 0;
			*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode = 7;
			v32 = g_unk52FBD4;

			g_levelActors[0].useTint = 0;

			if (g_unk52FC04 == 2 && g_unk52FBD4 == 7)
			{
				g_unk52FC04 = 3;
				g_levelActors[0].creatureRam->rotSpeed = 2;
				g_levelActors[0].creatureRam->speedTarget = 20;
			LABEL_48:
				v32 = g_unk52FBD4;
			}

			if (g_buzzActor.posAngles.pos.z > -113496 && g_unk52FC04 == 0)
			{
				AudioManager::PlayMusicLooping(g_levelIndex);

				g_unk52FC04 = 1;
				g_unk52FBCC = -229376;

				Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].transform.pos, 360, 16);

				Camera::g_cutsceneEyePos.x = g_buzzActor.posAngles.pos.x;
				Camera::g_cutsceneEyePos.y = g_buzzActor.posAngles.pos.y - 0x8000;
				Camera::g_cutsceneEyePos.z = g_buzzActor.posAngles.pos.z;

				Camera::InitCutsceneCamera(&Camera::g_cutsceneFocusPos, &Camera::g_cutsceneEyePos);

				v32 = g_unk52FBD4;
			}

			if (g_unk52FC04 == 1)
			{
				if (g_unk52FBF8 > 0)
				{
					g_unk52FBF8 -= (int32_t)Renderer::g_frameDelta;

					if (g_unk52FBF8 <= 0)
					{
						AudioManager::PlaySoundEffect(213, (Vector3I*)1);
						v32 = g_unk52FBD4;
					}
				}

				Camera::g_cutsceneFocusPos = g_levelActors[0].transform.pos;
				Camera::g_cutsceneEyePos.y += -128 * (int32_t)Renderer::g_frameDelta;

				if (g_unk52FBCC < -32768)
				{
					g_unk52FBCC += 640 * (int32_t)Renderer::g_frameDelta;

					if (g_unk52FBCC >= -32768)
						g_unk52FBCC = -32768;
				}

				if (Camera::g_cutsceneTimer == 0)
				{
					g_unk52FC04 = 2;
					g_levelActors[0].creatureRam->speedTarget = 16;
					v32 = g_unk52FBD4;
				}
			}

			if (g_unk52FC04 == 2)
			{
				v34 = g_unk52FBC0;

				if (g_unk52FBC0 != 0)
				{
					if (g_unk52FBC0 > 480)
					{
						v34 = 600;
						g_unk52FBC0 = 600;
					}

					if (Camera::g_cutsceneTimer != 0)
					{
						g_unk52FC00 = 200;
						v35 = pts[v32].pos.y;
						v36 = pts[v32].pos.z;

						Camera::g_cutsceneFocusPos.x = pts[v32].pos.x;
						Camera::g_cutsceneFocusPos.y = v35;
						Camera::g_cutsceneFocusPos.z = v36;

						yawAngle = g_levelActors[v32].transform.angles.yaw;

						Camera::g_cutsceneEyePos.y = v35;
						v38 = Numerics::g_sinCosLUT[(yawAngle + 1024) & 0xFFF];
						Camera::g_cutsceneEyePos.x = Camera::g_cutsceneFocusPos.x + 2 * v38 + v38;
						v39 = Numerics::g_sinCosLUT[(yawAngle - 2048) & 0xFFF];
						Camera::g_cutsceneEyePos.z = v36 + 2 * v39 + v39;
						if (Camera::g_cutsceneTimer < 300 && g_levelActors[v32].primaryAnimIdx == 0)
						{
							Actor::SetAnimation(&g_levelActors[v32], 1, 23);

							Game::InitActor(&g_levelActors[g_unk52FBC8], 0);
							Game::InitActor(&g_levelActors[g_unk52FBC4], 0);

							v40 = g_unk52FBC4;
							v41 = g_unk52FBC8;
							v42 = 40 * g_unk52FBC4;

							g_levelActors[v41].visibilityDistance = 2800;
							g_levelActors[v41].unkVar12 = 0;
							g_levelActors[v41].unkVar12_ = 0;

							g_levelActors[v41].unkVar13 = 0;
							g_levelActors[v41].unkVar13_ = (int16_t)0x8000;

							g_levelActors[v40].visibilityDistance = 2800;
							g_levelActors[v40].unkVar12 = 0;
							g_levelActors[v40].unkVar12_ = 0;
							g_levelActors[v40].unkVar13 = 0;
							g_levelActors[v40].unkVar13_ = (int16_t)0x8000;

							AudioManager::PlaySoundEffect(134, &g_levelActors[v41].transform.pos);
							v32 = g_unk52FBD4;
						}

						if (Camera::g_cutsceneTimer > 185 && Camera::g_cutsceneTimer < 300 && g_tickState[2] != 0)
						{
							a2.pos.x = 0;
							a2.pos.y = 400;
							a2.pos.z = 1200;

							Actor::ResolveBoneAttachmentPos((Vector3I*)&a2, &g_levelActors[v32], 4);

							if (g_tickState[4] != 0)
								Nu3D::Particles::SpawnFromPreset(a2.pos.x, a2.pos.y, a2.pos.z, 17, 2);
							else
								Nu3D::Particles::SpawnFromPreset(a2.pos.x, a2.pos.y, a2.pos.z, 4, 4);

							v32 = g_unk52FBD4;
						}

						if (g_levelActors[v32].actorPhase == 1)
						{
							v44 = pts[v32].pos.y;
							v45 = pts[v32].pos.x;
							v46 = pts[v32].pos.z;

							v47 = g_unk52FBC4;
							v48 = g_unk52FBC8;

							g_levelActors[v48].transform.pos.x = v45;
							g_levelActors[v48].transform.pos.y = v44 + 4096;
							g_levelActors[v48].transform.pos.z = v46;
							g_levelActors[v48].velX = 0;
							g_levelActors[v48].gravityVel = 0;
							g_levelActors[v48].velForward = 0;

							v49 = v47;

							g_levelActors[v49].transform.pos.x = v45;
							g_levelActors[v49].transform.pos.y = v44 + 4096;
							g_levelActors[v49].transform.pos.z = v46;
							g_levelActors[v49].velX = 0;
							g_levelActors[v49].gravityVel = 0;
							g_levelActors[v49].velForward = 0;

							if (g_levelActors[v48].creatureId != 14)
								g_levelActors[v48].unkShort4 = 460;

							if (g_levelActors[v47].creatureId != 14)
								g_levelActors[v47].unkShort4 = 460;

							if (Camera::g_cutsceneTimer < 60 && g_levelActors[v32].primaryAnimIdx == 1)
							{
								for (i = 4; i != 0; --i)
								{
									v51 = Nu3D::Particles::SpawnFromPreset(v45, v44, v46, 35, 14);
									v51->rotSpeed = *g_randDatBufferPtr++ - 128;
								}

								AudioManager::PlaySoundEffect(133, &v51->pos);
								AudioManager::Preset::PlayOneShotSound(214, (Actor::Toy2Actor*)&g_buzzActor);
								Lighting::SpawnLight(v45, v44, v46, 15761408, 32, (int32_t)g_levelActors);
								Actor::Kill(&g_levelActors[g_unk52FBD4], 1u);
							}
						}
					}
					else
					{
						v52 = g_unk52FBC4;
						v53 = g_unk52FBC8;

						if (g_unk52FBF0 == 0)
						{
							if (v34 < 420)
								goto LABEL_92;

							g_unk52FBF0 = 1;
							g_levelActors[g_unk52FBC8].unkShort2 = 10000;
							g_levelActors[v52].unkShort2 = 10000;
						}
						if (g_levelActors[v53].actorPhase == 0 && g_levelActors[v52].actorPhase == 0)
						{
							g_unk52FBC0 = 120;
							g_unk52FBF0 = 0;
							g_levelActors[0].creatureRam->speedTarget = 16;
							++g_unk52FBD4;
						}
					}
				}
			}
		LABEL_92:

			IsWithinDistance = Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &g_levelActors[0].transform.pos, 200);

			v55 = g_unk52FC04;

			if (IsWithinDistance != 0 && g_unk52FC04 == 3)
			{
				v56 = (int32_t)Renderer::g_frameDelta;

				if (g_unk52FBCC < g_buzzActor.posAngles.pos.y)
				{
					g_unk52FBCC += (int32_t)Renderer::g_frameDelta << 7;

					if (g_unk52FBCC >= g_buzzActor.posAngles.pos.y)
						g_unk52FBCC = g_buzzActor.posAngles.pos.y;
				}
				goto LABEL_108;
			}

			v56 = (int32_t)Renderer::g_frameDelta;

			if (g_unk52FBCC > -32768)
			{
				g_unk52FBCC += -128 * (int32_t)Renderer::g_frameDelta;

				if (g_unk52FBCC <= -32768)
					g_unk52FBCC = -32768;
			}

			if (g_unk52FC04 == 4)
			{
				Camera::g_cutsceneEyePos.y += (int32_t)Renderer::g_frameDelta << 7;

				if (Camera::g_cutsceneTimer < 120)
				{
					Actor::Kill(g_levelActors, 1u);
					v56 = (int32_t)Renderer::g_frameDelta;
					v55 = 5;
					g_unk52FC04 = 5;
				LABEL_104:

					if (Camera::g_cutsceneTimer == 0 && g_unk830CC4 == 0)
						g_unk830CC4 = 1;
				}
			}
			else
			{
				if (g_unk52FC04 == 5)
					goto LABEL_104;

				if (g_unk52FC04 < 2)
				{
					v57 = g_unk52FC00;
					goto LABEL_112;
				}
			}
		LABEL_108:

			v57 = g_unk52FC00;

			if (g_unk52FC00 > 0)
			{
				v57 = g_unk52FC00 - v56;
				g_unk52FC00 = v57;

				if (v57 <= 0)
				{
					v57 = 0;
					g_unk52FC00 = 0;
				}
			}
		LABEL_112:

			g_unk52FBFC -= v56;

			if (g_unk52FBFC <= 0)
				g_unk52FBFC = 0;

			if (v57 == 0)
			{
				if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &g_levelActors[0].transform.pos, 400) != 0)
				{
					a2.pos.x = 0;
					a2.pos.y = 0;
					a2.pos.z = -400;

					Actor::ResolveBoneAttachmentPos((Vector3I*)&a2, g_levelActors, 0);

					v58 = g_levelActors[0].transform.angles.yaw;
					v59 = (Nu3D::Math::CartesianToFixedAngle(g_buzzActor.posAngles.pos.x - a2.pos.x, g_buzzActor.posAngles.pos.z - a2.pos.z) - v58) & 0xFFF;

					if (v59 > 0x800)
						v59 -= 4095;

					if (v59 <= 512)
					{
						if (v59 < -512)
							*(uint16_t*)&v59 = -512;
					}
					else
					{
						*(uint16_t*)&v59 = 512;
					}

					v60 = (v58 + v59) & 0xFFF;
					pts[0].pos.y = 96000;

					v61 = Numerics::g_sinCosLUT[(v60 + 1024) & 0xFFF];
					pts[0].pos.x = 4 * Numerics::g_sinCosLUT[v60];
					pts[0].pos.z = 4 * v61;

					Nu3D::Collision::SweepAndSlide(&a2, &pts[0], 0x8000, 0, 256);
					Renderer::Beam::QueueBeam(9, 128, 400, &a2, &pts[0], 0, 0x80u, 0);

					a2.pos.x += pts[0].pos.x;
					a2.pos.y += pts[0].pos.y;
					a2.pos.z += pts[0].pos.z;

					AudioManager::PlaySoundEffect(135, (Vector3I*)&a2);

					v62 = 0;

					if (g_tickState[2] != 0)
					{
						do
						{
							Nu3D::Particles::SpawnFromPreset(a2.pos.x, a2.pos.y, a2.pos.z, 4, 4);
							++v62;
						} while (v62 < g_tickState[2]);
					}

					if (g_tickState[4] != 0)
						Nu3D::Particles::SpawnFromPreset(a2.pos.x, a2.pos.y, a2.pos.z, 70, 2);

					if (g_tickState[10] != 0)
						Lighting::SpawnLight(a2.pos.x, a2.pos.y, a2.pos.z, 49152, 16, -2);

					Renderer::LensFlare::RegisterLight(a2.pos.x, a2.pos.y, a2.pos.z, 0, 128, 0, 64);
					if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, (Vector3I*)&a2, 30) != 0)
					{
						if (g_buzzActor.stunTimer == 0)
						{
							AudioManager::PlaySoundEffect(132, &g_levelActors[0].transform.pos);

							if (g_unk52FBFC == 0)
								g_unk52FBFC = 1200;

							AudioManager::Preset::PlayOneShotSound(215, (Actor::Toy2Actor*)&g_buzzActor);
						}

						v63 = Nu3D::Math::CartesianToFixedAngle(g_buzzActor.posAngles.pos.x - a2.pos.x, g_buzzActor.posAngles.pos.z - a2.pos.z);

						Buzz::HandleDamage(v63, 3u);
					}
				}

				v55 = g_unk52FC04;
				v56 = (int32_t)Renderer::g_frameDelta;
			}

			v64 = g_unk52FBC8;
			v65 = g_unk52FBC8;

			if ((uint16_t)g_levelActors[g_unk52FBC8].unkVar13_ == 0x8000)
			{
				g_levelActors[g_unk52FBC8].unkVar12 += 12 * v56;

				if (g_levelActors[v65].unkVar12 >= 4096)
				{
					g_levelActors[v65].unkVar12 = 4096;
					g_levelActors[v65].unkVar13_ = 0;
				}

				g_levelActors[v65].unkVar12_ = g_levelActors[v65].unkVar12;
				g_levelActors[v65].unkVar13 = g_levelActors[v65].unkVar12;
			}

			v66 = g_unk52FBC4;

			if (v64 != g_unk52FBC4 && (uint16_t)g_levelActors[g_unk52FBC4].unkVar13_ == 0x8000)
			{
				g_levelActors[g_unk52FBC4].unkVar12 += 12 * v56;

				if (g_levelActors[v66].unkVar12 >= 4096)
				{
					g_levelActors[v66].unkVar12 = 4096;
					g_levelActors[v66].unkVar13_ = 0;
				}

				v55 = g_unk52FC04;
				g_levelActors[v66].unkVar12_ = g_levelActors[v66].unkVar12;
				g_levelActors[v66].unkVar13 = g_levelActors[v66].unkVar12;
			}

			if (v55 > 1 && v55 < 4)
			{
				g_unk52FBD0 -= v56;

				if (g_unk52FBD0 > 0)
				{
					v73 = g_unk52FBBC;
				}
				else
				{
					v67 = g_levelActors[v65].actorPhase;
					g_unk52FBD0 = 30;
					v68 = v67 <= 0;

					if (v67 <= 0)
					{
						if (g_levelActors[v66].actorPhase <= 0)
						{
							g_unk52FBBC = 0;
							goto LABEL_150;
						}

						v68 = v67 <= 0;
					}

					if (v68)
						v70 = 0x7FFFFFFF;
					else
						v70 = ((Camera::g_activeCameraTransform.pos.x - g_levelActors[v65].transform.pos.x) >> 8)
								* ((Camera::g_activeCameraTransform.pos.x - g_levelActors[v65].transform.pos.x) >> 8)
							+ ((Camera::g_activeCameraTransform.pos.z - g_levelActors[v65].transform.pos.z) >> 8)
								* ((Camera::g_activeCameraTransform.pos.z - g_levelActors[v65].transform.pos.z) >> 8);

					if (g_levelActors[v66].actorPhase > 0)
					{
						v71 = (Camera::g_activeCameraTransform.pos.x - g_levelActors[v66].transform.pos.x) >> 8;
						v72 = (Camera::g_activeCameraTransform.pos.z - g_levelActors[v66].transform.pos.z) >> 8;
					}
					else
					{
						v71 = 64;
						v70 = 0;
						v72 = 64;
					}

					if (v70 >= v71 * v71 + v72 * v72)
					{
						v55 = g_unk52FC04;
						v73 = v66;
						g_unk52FBBC = v66;
					}
					else
					{
						v55 = g_unk52FC04;
						v73 = v64;
						g_unk52FBBC = v64;
					}
				}

				if (v73 != 0)
				{
					v69 = g_levelActors[v73].transform.pos.z;
					v74 = g_levelActors[v73].transform.pos.y - 0x2000;
					Camera::g_unk50A118.x = g_levelActors[v73].transform.pos.x;
					Camera::g_unk50A118.y = v74;
					goto LABEL_151;
				}
			LABEL_150:

				v69 = g_levelActors[0].transform.pos.z;
				Camera::g_unk50A118.x = g_levelActors[0].transform.pos.x;
				Camera::g_unk50A118.y = g_levelActors[0].transform.pos.y - 0x4000;
			LABEL_151:
				Camera::g_unk50A118.z = v69;
			}

			if (v55 == 2 || v55 == 3)
				g_hudWidgetTimers[7] = 90;

			g_unk52B7E0 = 54 * (g_levelActors[0].actorPhase - 10) / 16;
		}

	}
}

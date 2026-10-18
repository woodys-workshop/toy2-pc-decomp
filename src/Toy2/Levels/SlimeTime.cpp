#include "Toy2/Levels/SlimeTime.h"
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
	namespace SlimeTime
	{
		// GLOBAL: TOY2 0x0052F98C
		int32_t g_unk52F98C;

		// GLOBAL: TOY2 0x0052F990
		int32_t g_unk52F990;

		// GLOBAL: TOY2 0x0052F994
		int32_t g_unk52F994;

		// GLOBAL: TOY2 0x0052F998
		int32_t g_unk52F998;

		// GLOBAL: TOY2 0x0052F99C
		int32_t g_unk52F99C;

		// GLOBAL: TOY2 0x0052F9A0
		int32_t g_unk52F9A0;

		// GLOBAL: TOY2 0x0052F9A4
		int32_t g_unk52F9A4;

		// GLOBAL: TOY2 0x0052F9A8
		int32_t g_unk52F9A8;

		// GLOBAL: TOY2 0x0052F9B0
		int32_t g_unk52F9B0;

		// GLOBAL: TOY2 0x0052F9B4
		int32_t g_unk52F9B4;

		// GLOBAL: TOY2 0x0052F9B8
		int32_t g_unk52F9B8;

		// FUNCTION: TOY2 0x00420060
		void Interactions()
		{
			int32_t v0;
			BOOL IsInsideBounds;
			int32_t v2;
			int32_t v3;
			uint32_t v4;
			int32_t v5;
			int16_t v6;
			int32_t v7;
			int16_t v8;
			int32_t GroundHeight;
			int32_t v10;
			int32_t v11;
			int32_t v12;
			int32_t v13;
			Transform a1;

			AudioManager::PlaySoundEffect(103, 0);
			g_unk52F9A8 = (g_unk52F9A8 - 1) & 1;

			if (g_levelActors[0].actorPhase && g_levelActors[0].actorPhase != g_unk52F98C)
			{
				g_unk52F98C = g_levelActors[0].actorPhase;
				*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode = 4;
				g_unk52F9A0 = 299;

				if (g_levelActors[0].actorPhase > 10)
				{
					g_unk52F990 = 120;
					Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].pos, 120, 16);
				}
				else
				{
					g_unk52F990 = 300;
					Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].pos, 300, 16);

					g_levelActors[0].motionTargetPos.z = g_levelActors[0].pos.z;
					Camera::g_cutsceneEyePos.z = g_levelActors[0].pos.z;
					g_levelActors[0].motionTargetPos.x = g_levelActors[0].pos.x;

					Camera::g_cutsceneEyePos.x = g_levelActors[0].pos.x;
					Camera::g_cutsceneEyePos.y = g_levelActors[0].pos.y - 20480;
					g_unk52F9A4 = 3;
					SaveManager::g_save0Data.tokens[g_levelFileIndex] = 128;
				}
			}

			v0 = g_unk52F990 - (int32_t)Renderer::g_frameDelta;
			g_unk52F990 -= (int32_t)Renderer::g_frameDelta;

			if (g_unk52F990 < 0)
			{
				g_unk52F990 = 0;
				*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode = 7;
			LABEL_8:
				g_levelActors[0].useTint = 0;
				goto LABEL_9;
			}

			if (g_levelActors[0].creatureId)
			{
				Camera::g_cutsceneFocusPos = g_levelActors[0].pos;

				if (v0 < 90)
					AudioManager::PlaySoundEffect(102, &Camera::g_cutsceneFocusPos);

				if (! g_unk52F9A8)
					goto LABEL_8;

				g_levelActors[0].useTint = 1;
				g_levelActors[0].actorTint.r = 0x2000;
				g_levelActors[0].actorTint.g = 0x2000;
				g_levelActors[0].actorTint.b = 0x2000;
			}
		LABEL_9:

			IsInsideBounds = Actor::IsInsideBounds(g_levelActors, -233647, 233647, -139166, 139166);
			v2 = (int32_t)Renderer::g_frameDelta;

			if (IsInsideBounds)
			{
				g_unk52F99C = 0;

				if (g_unk52F994 > 0)
				{
					g_unk52F994 += -128 * (int32_t)Renderer::g_frameDelta;

					if (g_unk52F994 <= 0)
						g_unk52F994 = 0;
				}
			}
			else
			{
				g_unk52F99C = 1;

				if (g_unk52F994 < 2048)
				{
					g_unk52F994 += (int32_t)Renderer::g_frameDelta << 7;

					if (g_unk52F994 >= 2048)
						g_unk52F994 = 2048;
				}
			}

			if (g_buzzActor.posAngles.pos.x < 204702 && ! g_unk52F9A4)
			{
				AudioManager::PlayMusicLooping(g_levelIndex);
				g_unk52F9A4 = 1;

				Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].pos, 300, 16);
				Camera::g_cutsceneEyePos.x = g_levelActors[0].pos.x - 294912;

				v2 = (int32_t)Renderer::g_frameDelta;
				Camera::g_cutsceneEyePos.y = g_levelActors[0].pos.y - 0x8000;
				Camera::g_cutsceneEyePos.z = g_levelActors[0].pos.z;
			}

			v3 = g_unk52F9A4;

			if (g_unk52F9A4 > 0 && g_unk52F9A4 < 3)
			{
				if (! g_unk52F990)
				{
					AudioManager::PlaySoundEffect(97, &g_levelActors[0].pos);
					v2 = (int32_t)Renderer::g_frameDelta;
				}

				if (! g_unk52F9B0)
				{
					if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &g_levelActors[0].pos, 400))
					{
						v4 = (g_unk52F9B4 - Camera::g_activeCameraTransform.angles.yaw) & 0xFFF;

						if (v4 > 0x800)
							v4 -= 4096;

						if (abs((int32_t)v4) > 64)
						{
							v13 = (*g_randDatBufferPtr++ & 1) + 98;
							AudioManager::PlaySoundEffect(v13, &g_levelActors[0].pos);
							g_unk52F9B0 = 100;
						}

						v2 = (int32_t)Renderer::g_frameDelta;
					LABEL_39:

						g_unk52F9B4 = Camera::g_activeCameraTransform.angles.yaw;
						v3 = g_unk52F9A4;
						goto LABEL_40;
					}
					v2 = (int32_t)Renderer::g_frameDelta;
				}

				g_unk52F9B0 -= v2;

				if (g_unk52F9B0 <= 0)
					g_unk52F9B0 = 0;

				goto LABEL_39;
			}
		LABEL_40:

			v5 = Camera::g_cutsceneTimer;

			if (v3 == 1)
			{
				if (Camera::g_cutsceneTimer < 30)
					*(uint8_t*)&g_levelActors[0].actorFlags |= 4u;

				v6 = Camera::g_cutsceneTimer - 30;

				if (Camera::g_cutsceneTimer - 30 < 0)
					v6 = 0;

				v7 = (20 * v6 + 1024) & 0xFFF;
				*(uint16_t*)&v7 = Numerics::g_sinCosLUT[v7];
				g_levelActors[0].rollAngle = ((v7 >> 3) - 2048) & 0xFFF;
				g_levelActors[0].pos.x += -2048 * v2;

				if (! Camera::g_cutsceneTimer)
				{
					v3 = 2;
					g_unk52F9A4 = 2;
				}

				Camera::g_cutsceneFocusPos.x = g_levelActors[0].pos.x;
				Camera::g_cutsceneEyePos.y += 304 * v2;

				if (Camera::g_cutsceneTimer >= 180)
					Camera::g_cutsceneEyePos.z += -128 * v2;
				else
					Camera::g_cutsceneEyePos.x += -512 * v2;
			}

			if (v3 == 2)
			{
				g_unk52F9A0 -= v2;

				if (g_unk52F9A0 <= 0)
				{
					g_unk52F9A0 = 600;
					g_unk52F9B8 = -g_unk52F9B8;
				}

				Camera::g_unk50A118 = g_levelActors[0].pos;

				if (g_tickState[11] && ! g_unk52F990)
				{
					if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &g_levelActors[0].pos, 400))
						Nu3D::Particles::SpawnInstance(
							g_levelActors[0].pos.x,
							g_levelActors[0].pos.y,
							g_levelActors[0].pos.z,
							2 * g_levelActors[0].velX,
							0,
							2 * g_levelActors[0].velForward,
							128,
							0,
							0,
							12);
					v2 = (int32_t)Renderer::g_frameDelta;
				}

				if (g_unk52F9A0 <= 300)
				{
					g_levelActors[0].motionTargetPos.x = g_unk52F9B8;
					g_levelActors[0].motionTargetPos.z = 0;
					g_levelActors[0].motionTargetPos.y =
						Numerics::g_sinCosLUT[g_unk52F994 + 1024] + g_levelActors[0].boundary.y + 2 * Numerics::g_sinCosLUT[g_unk52F994 + 1024] + 0x4000;
				}
				else
				{
					g_levelActors[0].motionTargetPos.y = 3 * Numerics::g_sinCosLUT[g_unk52F994 + 1024] - 68371;
					g_levelActors[0].motionTargetPos.x = g_buzzActor.posAngles.pos.x;
					g_levelActors[0].motionTargetPos.z = g_buzzActor.posAngles.pos.z;

					if (g_unk52F9A0 < 550)
					{
						AudioManager::PlaySoundEffect(101, &g_levelActors[0].motionTargetPos);
						v2 = (int32_t)Renderer::g_frameDelta;
						g_unk52F998 -= (int32_t)Renderer::g_frameDelta;

						if (g_unk52F998 < 0)
						{
							g_unk52F998 = 400;
							AudioManager::Preset::PlayOneShotSound(208, (Actor::Toy2Actor*)&g_buzzActor);
							v2 = (int32_t)Renderer::g_frameDelta;
						}
					}

					if (g_tickState[6] && g_unk52F9A0 < 550)
					{
						v8 = g_levelActors[0].yawAngle - 128;
						a1.pos.x = g_levelActors[0].pos.x + 4 * Numerics::g_sinCosLUT[(g_levelActors[0].yawAngle - 128) & 0xFFF];
						a1.pos.y = g_levelActors[0].pos.y;

						a1.pos.z = g_levelActors[0].pos.z + 4 * Numerics::g_sinCosLUT[(g_levelActors[0].yawAngle - 128 + 1024) & 0xFFF];

						GroundHeight = Nu3D::Collision::GetGroundHeight(&a1, 0);

						a1.pos.y = GroundHeight;

						if (GroundHeight != 0x80000000)
						{
							Nu3D::Particles::SpawnFromPreset(a1.pos.x, GroundHeight, a1.pos.z, 123, 2);
							Nu3D::Particles::SpawnFromPreset(a1.pos.x, a1.pos.y, a1.pos.z, 124, 1)->rotSpeed = *g_randDatBufferPtr;
							++g_randDatBufferPtr;
						}

						v10 = Numerics::g_sinCosLUT[(v8 + 1280) & 0xFFF];

						a1.pos.x = g_levelActors[0].pos.x + 4 * Numerics::g_sinCosLUT[(v8 + 256) & 0xFFF];
						a1.pos.y = g_levelActors[0].pos.y;
						a1.pos.z = g_levelActors[0].pos.z + 4 * v10;

						v11 = Nu3D::Collision::GetGroundHeight(&a1, 0);
						a1.pos.y = v11;

						if (v11 != 0x80000000)
						{
							Nu3D::Particles::SpawnFromPreset(a1.pos.x, v11, a1.pos.z, 123, 2);
							Nu3D::Particles::SpawnFromPreset(a1.pos.x, a1.pos.y, a1.pos.z, 124, 1)->rotSpeed = *g_randDatBufferPtr;
							++g_randDatBufferPtr;
						}

						v2 = (int32_t)Renderer::g_frameDelta;
					}
				}

				v3 = g_unk52F9A4;
				v5 = Camera::g_cutsceneTimer;
				g_hudWidgetTimers[7] = 90;
			}

			if (v3 == 3)
			{
				g_levelActors[0].motionTargetPos.y -= 640 * v2;
				g_levelActors[0].pos.y -= 640 * v2;
				Camera::g_cutsceneEyePos.y += -128 * v2;

				if (g_levelActors[0].actorPhase <= 10 && v5 < 120)
				{
					Actor::Kill(g_levelActors, 1u);
					*(uint16_t*)&v2 = (int32_t)Renderer::g_frameDelta;
				}

				if (Camera::g_cutsceneTimer)
				{
					v3 = g_unk52F9A4;
				}
				else
				{
					v3 = 4;
					g_unk830CC4 = 1;
					g_unk52F9A4 = 4;
				}
			}

			if ((g_levelActors[0].actorFlags & 4) != 0)
			{
				if (g_unk52F990)
				{
					g_levelActors[0].rollAngle = ((v2 << 6) + g_levelActors[0].rollAngle) & 0xFFF;

					if (v3 == 3)
						g_levelActors[0].pitchAngle = (36 * v2 + g_levelActors[0].pitchAngle) & 0xFFF;
				}
				else
				{
					v12 = (g_levelActors[0].yawAngle - (uint16_t)(g_levelActors[0].targetYaw)) & 0xFFF;

					if (v12 > 0x800)
						v12 -= 4096;

					g_levelActors[0].rollAngle -= (g_levelActors[0].rollAngle - v12 / 2) >> 5;
				}
			}

			if ((g_levelActors[0].actorFlags & 1) != 0)
			{
				if (g_tickState[2])
				{
					a1.pos.x = 500;
					a1.pos.y = -400;
					a1.pos.z = 0;

					Actor::ResolveBoneAttachmentPos((Vector3I*)&a1, g_levelActors, 0);
					Nu3D::Particles::SpawnFromPreset(a1.pos.x, a1.pos.y, a1.pos.z, 17, 2)->rotSpeed = (*g_randDatBufferPtr & 0x1F) - 16;

					a1.pos.x = -500;
					++g_randDatBufferPtr;
					a1.pos.y = -400;
					a1.pos.z = 0;

					Actor::ResolveBoneAttachmentPos((Vector3I*)&a1, g_levelActors, 0);
					Nu3D::Particles::SpawnFromPreset(a1.pos.x, a1.pos.y, a1.pos.z, 17, 2)->rotSpeed = (*g_randDatBufferPtr & 0x1F) - 16;
					++g_randDatBufferPtr;
				}

				if (g_levelActors[0].actorPhase <= 10 && g_tickState[7])
				{
					a1.pos.x = 0;
					a1.pos.y = 0;
					a1.pos.z = -200;

					Actor::ResolveBoneAttachmentPos((Vector3I*)&a1, g_levelActors, 0);
					Nu3D::Particles::SpawnFromPreset(a1.pos.x, a1.pos.y, a1.pos.z, 35, 14)->rotSpeed = *g_randDatBufferPtr - 128;
					++g_randDatBufferPtr;
				}
			}

			g_unk52B7E0 = 54 * (g_levelActors[0].actorPhase - 10) / 10;
		}

	}
}

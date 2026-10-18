#include "Toy2/Levels/BombsAway.h"
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
	namespace BombsAway
	{
		// GLOBAL: TOY2 0x0052F6F0
		int32_t g_unk52F6F0;

		// GLOBAL: TOY2 0x0052F6F4
		int32_t g_unk52F6F4;

		// GLOBAL: TOY2 0x0052F6F8
		int32_t g_unk52F6F8;

		// GLOBAL: TOY2 0x0052F6FC
		int32_t g_unk52F6FC;

		// GLOBAL: TOY2 0x0052F700
		int32_t g_unk52F700;

		// GLOBAL: TOY2 0x0052F704
		int32_t g_unk52F704;

		// GLOBAL: TOY2 0x0052F708
		int32_t g_unk52F708;

		// GLOBAL: TOY2 0x0052F70C
		int32_t g_unk52F70C;

		// GLOBAL: TOY2 0x0052F710
		int32_t g_unk52F710;

		// GLOBAL: TOY2 0x0052F718
		int32_t g_unk52F718;

		// GLOBAL: TOY2 0x0052F71C
		int32_t g_unk52F71C;

		// GLOBAL: TOY2 0x0052F720
		int32_t g_unk52F720;

		// GLOBAL: TOY2 0x0052F724
		int32_t g_unk52F724;

		// GLOBAL: TOY2 0x0052F72C
		int32_t g_unk52F72C;

		// GLOBAL: TOY2 0x0052F748
		int32_t g_unk52F748;

		// GLOBAL: TOY2 0x0052F74C
		int32_t g_unk52F74C;

		// GLOBAL: TOY2 0x0052F750
		int32_t g_unk52F750;

		// GLOBAL: TOY2 0x0052F754
		int32_t g_unk52F754;

		// GLOBAL: TOY2 0x0052F758
		int32_t g_unk52F758;

		// GLOBAL: TOY2 0x0052F75C
		int32_t g_unk52F75C;

		// GLOBAL: TOY2 0x0052F730
		int32_t g_unk52F730;

		// GLOBAL: TOY2 0x0052F738
		Vector3I g_unk52F738;

		// FUNCTION: TOY2 0x0041AA10
		void Interactions()
		{
			int32_t v0;
			int16_t v1;
			int32_t v2;
			int32_t v4;
			int32_t v5;
			int16_t unkShort4;
			bool v7;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			int16_t v11;
			int32_t v12;
			int16_t v13;
			int16_t v14;
			int32_t v15;
			int32_t v17;
			int32_t x;
			int32_t y;
			int32_t z;
			int32_t v21;
			int32_t v22;
			int32_t v23;
			int32_t v24;
			int32_t v25;
			int32_t v26;
			int32_t v27;
			int32_t v28;
			int32_t v29;
			int32_t v30;
			uint8_t v31;
			int32_t v32;
			uint8_t* v33;
			int32_t v34;
			int32_t v35;
			int32_t v36;
			Vector3I a1;

			AudioManager::PlaySoundEffect(111, (Vector3I*)1);
			AudioManager::PlaySoundEffect(103, 0);

			v0 = 0;

			if (g_unk52F718 > 0)
			{
				g_unk52F718 -= (int32_t)Renderer::g_frameDelta;

				if (g_unk52F718 < 0)
					AudioManager::Preset::PlayOneShotSound(210, (Actor::Toy2Actor*)&g_buzzActor);
			}

			g_unk52F748 = (g_unk52F748 + 31 * (int32_t)Renderer::g_frameDelta) & 0xFFF;
			g_unk52F750 &= 0xFFFu;
			g_unk52F74C = (g_unk52F74C + 63 * (int32_t)Renderer::g_frameDelta) & 0xFFF;

			if (! g_unk52F754 && Nu3D::Math::IsWithinDistanceXZ(&g_buzzActor.posAngles.pos, &g_levelActors[0].transform.pos, 380))
			{
				g_unk52F754 = 1;
				g_levelActors[0].transform.pos.y -= 11000;
				Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].transform.pos, 300, 16);
				g_levelActors[0].transform.pos.y += 11000;
				AudioManager::PlayMusicLooping(g_levelIndex);
			}

			if (g_unk52F754 == 1)
			{
				if (Camera::g_cutsceneTimer < 240)
				{
					if (g_unk52F70C >= 240)
						AudioManager::PlaySoundEffect(70, &g_levelActors[0].transform.pos);

					v0 = 1;
				}

				if (Camera::g_cutsceneTimer < 150)
				{
					if (g_unk52F70C >= 150)
						AudioManager::PlaySoundEffect(-4, &g_levelActors[0].transform.pos);

					g_unk52F704 += (int32_t)Renderer::g_frameDelta * (g_unk52F704 - g_unk52F75C) / -32;
				}

				if (! Camera::g_cutsceneTimer)
				{
					g_unk52F754 = 999;
					g_levelActors[0].movementData = Actor::g_movementData[14] + 9;
					g_levelActors[0].unkShort1 = 0;
					Nu3D::Link::SetRotationRelative8bit(0, 0, 0, 0);
				}
			}

			if (g_unk52F754 == 999)
			{
				g_unk52F71C -= (int32_t)Renderer::g_frameDelta;

				if (g_unk52F71C <= 0)
				{
					g_unk52F71C = *g_randDatBufferPtr++ + 1800;
					AudioManager::Preset::PlayOneShotSound(209, (Actor::Toy2Actor*)&g_buzzActor);
				}

				if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, &g_levelActors[0].transform.pos, 50))
				{
					v1 = Nu3D::Math::CartesianToFixedAngle(
						g_buzzActor.posAngles.pos.x - g_levelActors[0].transform.pos.x, g_buzzActor.posAngles.pos.z - g_levelActors[0].transform.pos.z);

					Buzz::HandleDamage(v1 & 0xFFF, 3u);
				}

				if (g_unk52F730 != g_levelActors[0].actorPhase)
				{
					g_unk52F710 = 4;
					v2 = g_unk52F75C - 768;
					g_levelActors[0].actorPhase = g_unk52F730;
					g_unk52F75C -= 768;

					if (g_unk52F75C >= 512)
						goto LABEL_36;

					g_unk52F75C = 512;
					g_unk52F6F0 += 2048;

					v15 = 0;

					do
					{
						if (Nu3D::Particles::g_particleInstances[v15].typeId == 63)
							Nu3D::Particles::g_particleInstances[v15].lifetime = 1;
						++v15;
					} while (v15 < 64);

					if (g_unk52F6F0 >= 12288 && g_unk52F72C)
					{
						Nu3D::Link::SetScaleFromFixedOffsets(50, 4096, 4096, 4096);
						Levels::g_recordData[63]->data[0].y = g_unk52F72C;
						g_unk52F72C = 0;
					}

					if (g_unk52F6F0 >= 14336)
					{
						g_levelActors[0].movementData = Actor::g_movementData[14] + 94;

						g_unk52F6F0 = 14336;
						g_unk52F704 = 1;
						g_unk52F75C = 1;

						g_levelActors[0].unkShort1 = 0;
						g_unk52F754 = 1000;
						g_levelActors[0].transform.pos.y -= 11000;
						g_buzzActor.posAngles.pos.x += 10000;
						g_buzzActor.posAngles.pos.z += 10000;

						Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].transform.pos, 420, 16);

						g_levelActors[0].actorFlags &= 0xFE7Fu;
						g_buzzActor.posAngles.pos.z -= 10000;
						g_levelActors[0].transform.pos.y += 11000;
						g_buzzActor.posAngles.pos.x -= 10000;

						g_unk52F6F4 = g_levelActors[0].transform.pos.y - 12000;
						SaveManager::g_save0Data.tokens[g_levelFileIndex] = 128;
						goto LABEL_56;
					}

					*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode = 4;

					g_levelActors[0].transform.pos.y -= 14000;
					g_levelActors[0].movementData = Actor::g_movementData[14] + 79;
					g_levelActors[0].unkShort1 = 0;

					Camera::BeginScriptedCutsceneAtPoint(&g_levelActors[0].transform.pos, g_unk52F758, 16);

					g_levelActors[0].transform.pos.y += 14000;
					g_levelActors[0].velX = 0;
					g_levelActors[0].gravityVel = 0;
					g_levelActors[0].velForward = 0;

					Actor::SetAnimation(g_levelActors, 1, 16);
					g_unk52F758 += 40;
				}

				v2 = g_unk52F75C;
			LABEL_36:

				if (*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode == 4)
					Camera::g_cutsceneFocusPos.y = g_levelActors[0].transform.pos.y - (2 * g_unk52F704 + 10000);

				v4 = g_unk52F6F0;

				if (v2 >= g_unk52F6F0)
					goto LABEL_49;

				if (*(uint8_t*)&g_levelActors[0].creatureRam->defenseMode == 4)
				{
					if (Camera::g_cutsceneTimer >= 120)
					{
						g_levelActors[0].unkShort1 = 5;
						goto LABEL_47;
					}

					if (g_unk52F70C >= 120)
					{
						AudioManager::PlaySoundEffect(-4, &g_levelActors[0].transform.pos);
						v2 = g_unk52F75C;
						v4 = g_unk52F6F0;
					}

					v5 = (int32_t)Renderer::g_frameDelta * (v2 - v4) / -32;
				}
				else
				{
					v5 = 32 * (int32_t)Renderer::g_frameDelta;
				}

				v2 += v5;

				g_unk52F75C = v2;
			LABEL_47:
				if (v2 >= v4)
				{
					v2 = v4;
					g_unk52F75C = v4;
				}
			LABEL_49:

				unkShort4 = g_levelActors[0].unkShort4;

				g_unk52F704 += (int32_t)Renderer::g_frameDelta * (g_unk52F704 - v2) / -8;

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
						unkShort4 = g_levelActors[0].unkShort4;
					}

					if (unkShort4 == 3)
						Camera::g_unk50A510 = 40;

					g_levelActors[0].unkShort4 = 0;
				}

				v0 = 1;
			}
		LABEL_56:

			v7 = g_unk52F754 <= 1000;

			if (g_unk52F754 == 1000)
			{
				Camera::g_cutsceneFocusPos.y = g_levelActors[0].transform.pos.y - 11000;
				v0 = 1;
				Camera::g_cutsceneFocusPos.x = g_levelActors[0].transform.pos.x;
				Camera::g_cutsceneFocusPos.z = g_levelActors[0].transform.pos.z;

				if (Camera::g_cutsceneTimer >= 180)
					goto LABEL_64;

				g_unk52F754 = 1001;
				g_unk52F6FC = -3072;

				g_levelActors[0].transform.pos.y += 9000;
				Actor::Kill(g_levelActors, 1u);
				g_levelActors[0].transform.pos.y -= 9000;

				v7 = g_unk52F754 <= 1000;
			}

			if (! v7)
			{
				v8 = 96 * (int32_t)Renderer::g_frameDelta + g_unk52F6FC;
				v7 = v8 + g_unk52F6F4 <= g_levelActors[0].transform.pos.y - 2000;

				Camera::g_cutsceneFocusPos.y += 128;
				g_unk52F6FC = v8;
				g_unk52F6F4 += v8;

				if (! v7)
				{
					g_unk52F6FC = -(v8 >> 1);
					g_unk52F6F4 = g_levelActors[0].transform.pos.y - 2000;

					if (abs(g_unk52F6FC) > 128)
						AudioManager::PlaySoundEffect(69, &g_levelActors[0].transform.pos);
				}

				Nu3D::Link::SetPositionRawAndCommit(0, g_levelActors[0].transform.pos.x >> 5, g_unk52F6F4 >> 5, g_levelActors[0].transform.pos.z >> 5);

				goto LABEL_67;
			}
		LABEL_64:

			v9 = (g_unk52F6F8 + 128) & 0x1FFF;
			g_unk52F6F8 = v9;

			if (g_unk52F704 < 1024)
			{
				v9 = 0;
				g_unk52F6F8 = 0;
			}

			Nu3D::Link::SetRotationRelative8bit(
				0, Numerics::g_sinCosLUT[v9 & 0xFFF] >> 8, g_levelActors[0].transform.angles.yaw, Numerics::g_sinCosLUT[(v9 >> 1) & 0xFFF] >> 8);

			a1.x = 0;
			a1.y = -700;
			a1.z = 0;

			Actor::ResolveBoneAttachmentPos(&a1, g_levelActors, 10);

			Nu3D::Link::SetPositionRawAndCommit(0, a1.x >> 5, (a1.y >> 5) - 32, a1.z >> 5);
		LABEL_67:
			v10 = g_unk52F754;

			if (g_unk52F754 == 1001)
			{
				if (! Camera::g_cutsceneTimer)
				{
					v10 = 1002;
					g_unk52F754 = 1002;
					goto LABEL_71;
				}
			}
			else if (g_unk52F754 > 1001)
			{
			LABEL_71:
				if (! g_unk830CC4)
				{
					v10 += (int32_t)Renderer::g_frameDelta;
					g_unk52F754 = v10;
					g_unk830CC4 = v10 > 1030;
				}
			}

			if (g_tickState[6] && v0)
			{
				a1.x = g_levelActors[0].transform.pos.x;
				a1.y = g_levelActors[0].transform.pos.y - 10000;
				a1.z = g_levelActors[0].transform.pos.z;

				Nu3D::Particles::SpawnFromPreset(

					g_levelActors[0].transform.pos.x, g_levelActors[0].transform.pos.y - 10000, g_levelActors[0].transform.pos.z, 65, 20)
					->rotSpeed = (*g_randDatBufferPtr - 128) >> 3;

				v10 = g_unk52F754;
				++g_randDatBufferPtr;
			}

			if (v10 < 1001)
			{
				g_levelActors[0].unkVar8 = g_levelActors[0].unkVar7;
				g_levelActors[0].secondaryAnimIdx = g_levelActors[0].primaryAnimIdx - 1;
				g_levelActors[0].unkVar13_ = -301;

				v11 = -12000 - g_unk52F704;

				g_levelActors[0].unkVar12 = g_unk52F704;
				g_levelActors[0].unkVar12_ = g_unk52F704;
				g_levelActors[0].unkVar13 = g_unk52F704;

				if (g_unk52F704 == -12000)
					v11 = 1;

				*(int16_t*)(g_levelActors[0].unkVar34 + 18) = v11;
				*(int16_t*)(g_levelActors[0].unkVar34 + 50) = v11;
				*(int16_t*)(g_levelActors[0].unkVar34 + 82) = v11;
				*(int16_t*)(g_levelActors[0].unkVar34 + 114) = v11;

				v12 = g_unk52F704 / 16;
				if (! (g_unk52F704 / 16))
					*(uint16_t*)&v12 = 1;

				*(int16_t*)(g_levelActors[0].unkVar34 + 30) = v12;
				*(int16_t*)(g_levelActors[0].unkVar34 + 62) = v12;
				*(int16_t*)(g_levelActors[0].unkVar34 + 94) = v12;
				*(int16_t*)(g_levelActors[0].unkVar34 + 126) = v12;

				v10 = g_unk52F754;
			}

			g_unk52B7E0 = 54 * ((14336 - g_unk52F6F0) / 2048) / 5;

			if (v10 >= 999)
				g_hudWidgetTimers[7] = 90;

			g_unk52F70C = Camera::g_cutsceneTimer;
			Renderer::BlitTextureByIndexOffset(24, 0, 64, 64, 64, g_unk52F724, g_unk52F720, 0, 64);
			g_unk52F720 = (g_unk52F720 - (int32_t)Renderer::g_frameDelta) & 0x3F;

			if (g_unk52F754 >= 999)
			{
				Camera::g_unk50A118.x = g_levelActors[0].transform.pos.x;
				Camera::g_unk50A118.y = g_levelActors[0].transform.pos.y - 10000;
				Camera::g_unk50A118.z = g_levelActors[0].transform.pos.z;
			}

			a1.x = 13453;
			a1.y = -210651;
			a1.z = -12501;

			Renderer::LensFlare::RegisterLight(14473, -221337, -8124, 128, 128, 128, 128);

			if (g_unk52F710)
			{
				g_unk52F710 -= (int32_t)Renderer::g_frameDelta;

				if (g_unk52F710 < 0)
					g_unk52F710 = 0;

				g_levelActors[0].useTint = 1;
				g_levelActors[0].actorTint.r = 0x2000;
				g_levelActors[0].actorTint.g = 0x2000;
				g_levelActors[0].actorTint.b = 0x2000;
			}
			else
			{
				g_levelActors[0].useTint = 983040;
				v13 = Numerics::g_sinCosLUT[g_unk52F750];
				g_levelActors[0].actorTint.r = Numerics::g_sinCosLUT[g_unk52F748] >> 4;
				v14 = Numerics::g_sinCosLUT[g_unk52F74C];
				g_levelActors[0].actorTint.g = v13 >> 4;
				g_levelActors[0].actorTint.b = v14 >> 4;
			}

			v15 = 0;

			while (Nu3D::Particles::g_particleInstances[v15].typeId != 63 || Nu3D::Particles::g_particleInstances[v15].lifetime <= 0)
			{
				++v15;

				Lighting::g_dynamicLights[1].lifetime = 0;

				if (v15 >= 64)
					goto LABEL_97;
			}

			Lighting::g_dynamicLights[1].colorR = 0;
			Lighting::g_dynamicLights[1].colorG = -1;
			Lighting::g_dynamicLights[1].colorB = 0;
			Lighting::g_dynamicLights[1].lifetime = 1;

			v17 = v15;

			Lighting::g_dynamicLights[1].groupId = -3;
			x = Nu3D::Particles::g_particleInstances[v15].pos.x;
			y = Nu3D::Particles::g_particleInstances[v15].pos.y;
			Lighting::g_dynamicLights[1].pos.x = x;
			z = Nu3D::Particles::g_particleInstances[v17].pos.z;
			Lighting::g_dynamicLights[1].pos.y = y;
			Lighting::g_dynamicLights[1].pos.z = z;
		LABEL_97:

			a1.y = (a1.y - g_buzzActor.posAngles.pos.y) >> 7;
			a1.x = (a1.x - g_buzzActor.posAngles.pos.x) >> 7;
			a1.z = (a1.z - g_buzzActor.posAngles.pos.z) >> 7;

			Nu3D::Math::NormalizeToFixedPoint(&a1, &a1);

			v21 = g_levelFileIndex;
			v22 = a1.x >> 2;
			v23 = a1.y >> 2;
			v24 = a1.z >> 2;

			Lighting::g_levelLightDefs[v21].offset.x = a1.x >> 2;
			Lighting::g_levelLightDefs[v21].offset.y = v23;
			Lighting::g_levelLightDefs[v21].offset.z = v24;

			a1.x = v22;
			a1.y = v23;
			a1.z = v24;

			Lighting::g_levelLightDefs[v21].packedColor = 0xC0C000;
			Weather::StepPrecipitation(256, 4096, 50);

			if (g_tickState[8])
			{
				v25 = Numerics::g_sinCosLUT[Camera::g_activeCameraTransform.angles.yaw];
				v26 = Numerics::g_sinCosLUT[(Camera::g_activeCameraTransform.angles.yaw + 1024) & 0xFFF];

				v27 = (*g_randDatBufferPtr - 128) << 8;

				g_randDatBufferPtr += 2;
				Nu3D::Particles::SpawnFromPreset(
					v27 + Camera::g_activeCameraTransform.pos.x + 2 * v25 + v25, 0, v27 + Camera::g_activeCameraTransform.pos.z + 2 * v26 + v26, 27, 2);
			}

			v28 = g_unk52F708;
			v29 = g_unk52F708 - (int32_t)Renderer::g_frameDelta;
			g_unk52F708 -= (int32_t)Renderer::g_frameDelta;

			if (g_unk52F708 < 32)
			{
				if (v28 < 32)
				{
					v33 = g_randDatBufferPtr;
				}
				else
				{
					v30 = *g_randDatBufferPtr++;
					v31 = *g_randDatBufferPtr;
					g_unk52F700 = v30;

					v32 = (Camera::g_activeCameraTransform.angles.yaw + v31 - 128) & 0xFFF;
					v33 = ++g_randDatBufferPtr;
					v34 = v30 * Numerics::g_sinCosLUT[v32];
					v35 = v30 * Numerics::g_sinCosLUT[(v32 + 1024) & 0xFFF];

					g_unk52F738.x = Camera::g_activeCameraTransform.pos.x + (v34 >> 5);
					g_unk52F738.y = Camera::g_activeCameraTransform.pos.y;
					g_unk52F738.z = Camera::g_activeCameraTransform.pos.z + (v35 >> 5);
				}

				if (Nu3D::Camera::g_targetTintFadeSpeed)
				{
					g_unk52F708 = 10000;
					Nu3D::Camera::g_cameraTintBlue = 128;
					Nu3D::Camera::g_cameraTintGreen = 128;
					Nu3D::Camera::g_cameraTintRed = 128;
				}
				else if (v29 >= 0)
				{
					if (Nu3D::Camera::g_cameraTintRed > 64)
					{
						Nu3D::Camera::g_cameraTintBlue = 3 * v29 + 128;
						Nu3D::Camera::g_cameraTintGreen = 3 * v29 + 128;
						Nu3D::Camera::g_cameraTintRed = 3 * v29 + 128;
					}
				}
				else
				{
					v36 = *v33;
					g_randDatBufferPtr = v33 + 1;
					g_unk52F708 = 2 * v36 + 32;
				}
			}

			g_unk52F700 -= (int32_t)Renderer::g_frameDelta;

			if (g_unk52F700 <= 0)
			{
				g_unk52F700 = 20000;
				AudioManager::PlaySoundEffect(112, &g_unk52F738);
			}
		}

	}
}

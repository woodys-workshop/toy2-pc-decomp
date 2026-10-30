#include "Toy2/Levels/AlsSpaceLand.h"
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
#include "Renderer/Shadows.h"
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
	namespace AlsSpaceLand
	{
		// GLOBAL: TOY2 0x004F2F0C
		int16_t g_unk4F2F0C[6] = { 49, 51, 50, 53, 52, 0 };

		// GLOBAL: TOY2 0x004F2F18
		RGB16 g_unk4F2F18[4] = { { 128, 0, 0 }, { 0, 128, 0 }, { 128, 0, 0 }, { 0, 64, 128 } };

		// GLOBAL: TOY2 0x004F2F30
		char* g_unk4F2F30[5] = {
			"i saw ^hamm^ at the end of the ^laser battle zone^.",
			"the ^mothership^ has returned for the ^aliens^. she is waiting near a pile of boxes.",
			"there is a ^flying saucer^ that wants to challenge you to a ^race^. it is waiting at the start of the ^zipline^ course.",
			"there is a ^token^ in the claw machine next to you. stomp the button to grab the ^token^.",
			"the ^buzz lightyear buggy^ boss is in a room that can only be reached by climbing the space ^mobile^ in the central area."
		};

		// GLOBAL: TOY2 0x004F2F44
		int16_t g_unk4F2F44[10] = { 0, 0, 1, 15, 3, 2, 19, 4, 3, -1 };

		// GLOBAL: TOY2 0x0052FA60
		Transform g_laserPositions[2];

		// GLOBAL: TOY2 0x0052FA80
		Levels::Type63* g_unk52FA80;

		// GLOBAL: TOY2 0x0052FA84
		int32_t g_unk52FA84;

		// GLOBAL: TOY2 0x0052FA90
		int32_t g_unk52FA90;

		// GLOBAL: TOY2 0x0052FA98
		Vector3I g_unk52FA98;

		// GLOBAL: TOY2 0x0052FAA8
		Vector3I g_unk52FAA8;

		// GLOBAL: TOY2 0x0052FAB8
		Vector3I g_unk52FAB8;

		// GLOBAL: TOY2 0x0052FAC8
		Vector3I g_unk52FAC8;

		// GLOBAL: TOY2 0x0052FAD8
		int32_t g_unk52FAD8;

		// GLOBAL: TOY2 0x0052FADC
		int32_t g_unk52FADC;

		// GLOBAL: TOY2 0x0052FAE4
		int32_t g_unk52FAE4;

		// GLOBAL: TOY2 0x0052FAE8
		Vector3I g_clawHomePos;

		// GLOBAL: TOY2 0x0052FAF8
		int32_t g_unk52FAF8;

		// GLOBAL: TOY2 0x0052FAFC
		int32_t g_laserTotalTicks[2];

		// GLOBAL: TOY2 0x0052FB08
		Vector3I g_tokenPos;

		// GLOBAL: TOY2 0x0052FB18
		int32_t g_clawInitialized;

		// GLOBAL: TOY2 0x0052FB1C
		int32_t g_unk52FB1C;

		// GLOBAL: TOY2 0x0052FB20
		int32_t g_unk52FB20[18];

		// GLOBAL: TOY2 0x0052FB60
		int32_t g_unk52FB60;

		// GLOBAL: TOY2 0x0052FB64
		int32_t g_unk52FB64;

		// GLOBAL: TOY2 0x0052FB68
		int32_t g_unk52FB68;

		// GLOBAL: TOY2 0x0052FB6C
		int32_t g_unk52FB6C;

		// GLOBAL: TOY2 0x0052FB70
		int32_t g_unk52FB70;

		// GLOBAL: TOY2 0x0052FB74
		int32_t g_clawCatchResult;

		// GLOBAL: TOY2 0x0052FB78
		int32_t g_unk52FB78;

		// GLOBAL: TOY2 0x0052FB80
		int32_t g_unk52FB80;

		// GLOBAL: TOY2 0x0052FB84
		int32_t g_clawHoldTimer;

		// GLOBAL: TOY2 0x0052FB88
		int32_t g_clawState;

		// GLOBAL: TOY2 0x0052FB8C
		int32_t g_unk52FB8C;

		// GLOBAL: TOY2 0x0052FB90
		int32_t g_laserTimer[2];

		// GLOBAL: TOY2 0x0052FB98
		int32_t g_unk52FB98;

		// GLOBAL: TOY2 0x0052FBA0
		int32_t g_clawSwingX;

		// GLOBAL: TOY2 0x0052FBA4
		int32_t g_clawSwingZ;

		// GLOBAL: TOY2 0x0052FBA8
		int32_t g_clawDropDepth;

		// GLOBAL: TOY2 0x0052FBAC
		int32_t g_laserWaypointIdx[2];

		// GLOBAL: TOY2 0x0052FA58
		int32_t g_unk52FA58;

		// GLOBAL: TOY2 0x0052FA88
		int32_t g_unk52FA88;

		// GLOBAL: TOY2 0x0052FA8C
		int32_t g_unk52FA8C;

		// GLOBAL: TOY2 0x0052FAE0
		int32_t g_unk52FAE0;

		// GLOBAL: TOY2 0x0052FB7C
		int32_t g_unk52FB7C;

		// GLOBAL: TOY2 0x0052FB9C
		int32_t g_unk52FB9C;

		// FUNCTION: TOY2 0x00422D20
		void FireLaser(int32_t laserIndex)
		{
			int32_t l_newTimer;
			int32_t l_randTimerVal;
			Levels::RecordData* l_waypointTablePtr;
			int32_t l_waypointIndex;
			uint8_t* l_nextRandByte;
			int32_t l_buzzPosY;
			Levels::RecordData* l_waypointData;
			int32_t l_posXIndex;
			int32_t l_posX;
			int32_t l_targetPosZ;
			int32_t v12;
			Levels::RecordData* v13;
			int32_t v14;
			int32_t y;
			int32_t z;
			int32_t v18;
			int32_t v19;
			int32_t v20;
			Transform* v21;
			int16_t v22;
			Transform a5;

			l_newTimer = g_laserTimer[laserIndex] - (int32_t)Renderer::g_frameDelta;
			g_laserTimer[laserIndex] = l_newTimer;

			if (l_newTimer <= 0)
			{
				l_nextRandByte = g_randDatBufferPtr + 1;
				l_randTimerVal = (*g_randDatBufferPtr & 7) + 8;

				g_laserTimer[laserIndex] = l_randTimerVal;
				g_randDatBufferPtr = l_nextRandByte;

				g_laserTotalTicks[laserIndex] = l_randTimerVal;
				g_laserWaypointIdx[laserIndex] += *l_nextRandByte & 7;

				l_waypointTablePtr = Levels::g_recordData[laserIndex + 14];
				l_waypointIndex = g_laserWaypointIdx[laserIndex];

				g_randDatBufferPtr = l_nextRandByte + 1;

				if (l_waypointIndex >= l_waypointTablePtr->recordCount)
					g_laserWaypointIdx[laserIndex] = l_waypointIndex - l_waypointTablePtr->recordCount;

				l_buzzPosY = g_buzzActor.posAngles.pos.y;
				l_waypointData = l_waypointTablePtr;

				l_posXIndex = laserIndex;
				g_laserPositions[l_posXIndex].pos.x = 32 * l_waypointData->data[g_laserWaypointIdx[laserIndex]].x;
				g_laserPositions[l_posXIndex].pos.y = 32 * l_waypointData->data[g_laserWaypointIdx[laserIndex]].y;
				l_posX = 32 * l_waypointData->data[g_laserWaypointIdx[laserIndex]].z;
				g_laserPositions[l_posXIndex].pos.z = l_posX;

				if (l_buzzPosY > -5391
					&& ((g_laserPositions[laserIndex].pos.x - g_buzzActor.posAngles.pos.x)
						>> 8) * ((g_laserPositions[laserIndex].pos.x - g_buzzActor.posAngles.pos.x) >> 8)
							+ ((g_laserPositions[laserIndex].pos.y - l_buzzPosY) >> 8) * ((g_laserPositions[laserIndex].pos.y - l_buzzPosY) >> 8)
							+ ((l_posX - g_buzzActor.posAngles.pos.z) >> 8) * ((l_posX - g_buzzActor.posAngles.pos.z) >> 8)
						< 250000)
				{
					l_targetPosZ = g_buzzActor.posAngles.pos.z;
					g_laserPositions[laserIndex].pos.x = g_buzzActor.posAngles.pos.x;
					g_laserPositions[laserIndex].pos.y = l_buzzPosY - 2048;
					g_laserPositions[laserIndex].pos.z = l_targetPosZ;
				}
			}

			v12 = g_laserWaypointIdx[laserIndex];
			v13 = Levels::g_recordData[-laserIndex + 15];
			v14 = laserIndex;
			y = g_laserPositions[laserIndex].pos.y;

			a5.pos.x = 32 * v13->data[v12].x - g_laserPositions[laserIndex].pos.x;
			a5.pos.y = 32 * v13->data[v12].y - y;
			z = v13->data[v12].z;
			v18 = g_laserTotalTicks[laserIndex];
			v19 = 32 * z - g_laserPositions[laserIndex].pos.z;
			a5.pos.x = a5.pos.x * g_laserTimer[laserIndex] / v18;
			a5.pos.y = a5.pos.y * g_laserTimer[laserIndex] / v18;
			a5.pos.z = v19 * g_laserTimer[laserIndex] / v18;

			if (laserIndex)
				Renderer::Beam::QueueBeam(9, 64, 400, &g_laserPositions[v14], &a5, 0, 0x40u, 0x80u);
			else
				Renderer::Beam::QueueBeam(9, 64, 400, g_laserPositions, &a5, 0x80u, 0, 0);

			if (g_laserTimer[laserIndex] == g_laserTotalTicks[laserIndex])
			{
				v20 = 5;
				do
				{
					v21 = &g_laserPositions[v14];
					Nu3D::Particles::SpawnFromPreset(
						g_laserPositions[laserIndex].pos.x, g_laserPositions[laserIndex].pos.y, g_laserPositions[laserIndex].pos.z, 4, 4)
						->lifetime = 2 * (*g_randDatBufferPtr & 0xF) + 24;
					--v20;
					++g_randDatBufferPtr;
				} while (v20);

				if (laserIndex)
					Lighting::SpawnLight(v21->pos.x, g_laserPositions[laserIndex].pos.y, g_laserPositions[laserIndex].pos.z, 30960, 16, v21->pos.x);
				else
					Lighting::SpawnLight(
						g_laserPositions[0].pos.x, g_laserPositions[0].pos.y, g_laserPositions[0].pos.z, 15728640, 16, g_laserPositions[0].pos.z);

				AudioManager::PlaySoundEffect(7, (Vector3I*)&g_laserPositions[v14]);

				if (Nu3D::Math::IsWithinDistance(&g_buzzActor.posAngles.pos, (Vector3I*)&g_laserPositions[v14], 25))
				{
					v22 = Nu3D::Math::CartesianToFixedAngle(
						g_buzzActor.posAngles.pos.x - v21->pos.x, g_buzzActor.posAngles.pos.z - g_laserPositions[laserIndex].pos.z);

					Buzz::HandleDamage(v22, 2u);
				}
			}
		}

		// FUNCTION: TOY2 0x00423200
		void Interactions()
		{
			int32_t v0;
			int32_t* v1;
			int16_t v2;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			int16_t v6;
			Levels::RecordData* v7;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			int32_t z;
			int32_t v14;
			int32_t l_randByte1;
			int32_t l_randByte2;
			int32_t v17;
			int32_t v18;
			int32_t v19;
			int16_t v20;
			int32_t v21;
			int32_t v22;
			int32_t v23;
			int16_t v24;
			int32_t v25;
			int32_t v26;
			int32_t v27;
			int16_t v28;
			int32_t v29;
			uint8_t* v30;
			int32_t v31;
			int32_t v32;
			int32_t v33;
			Nu3D::Particles::ParticleInstance* v34;
			int32_t v35;
			int32_t v36;
			int32_t v37;
			int32_t v38;
			int32_t v39;
			int16_t v40;
			int32_t v41;
			int32_t v42;
			int32_t v43;
			int32_t v44;
			int32_t v45;
			int32_t v46;
			int32_t v47;
			int32_t v48;
			Vector3I l_clawPos;

			v0 = 2;
			v1 = g_unk52FB20;

			do
			{
				v2 = (int32_t)Renderer::g_frameDelta;
				v3 = (v1[1] + (int32_t)Renderer::g_frameDelta * v0) & 0xFFF;

				v1[1] = v3;
				v45 = (*v1 + v2 * (Numerics::g_sinCosLUT[v3] >> 10)) & 0xFFF;
				*v1 = v45;

				Nu3D::Link::SetRotationRelative8bit(v0, 0, v45, 0);
				v1 += 2;
				++v0;
			} while (v0 - 2 < 8);

			Actor::CollectQuestReward(0, 10, -1, 0, 0);
			Actor::RotatingHint(41, 5, g_unk4F2F30);

			if ((g_levelActors[2].actorFlags & 0x200) != 0)
			{
				g_levelActors[2].actorFlags &= ~0x200u;

				if (g_collectionCount >= 0)
				{
					if (g_collectionCount == 5)
					{
						Dialogue::Begin(2, 11, "thanks for finding my ^aliens^ buzz! here is a pizza planet ^token^!", -1, 0, 1);
						g_collectionCount = -1;
					}
					else
					{
						Dialogue::Begin(
							2,
							11,
							"hi buzz! if you find my ^five^ missing ^aliens^ and come back and find me, i will give you a pizza planet ^token^.",
							-1,
							0,
							-1);
					}
				}
			}

			if ((g_levelActors[1].actorFlags & 0x200) != 0)
			{
				g_levelActors[1].actorFlags &= ~0x200u;

				if (! g_unk52F2F8)
				{
					g_unk52F2F8 = 1;
					g_unk52AD64 = 2;
					Dialogue::Begin(1, 12, "hi buzz! if you can beat me to the end of the ^zipline^ course i will give you a pizza planet ^token^!", -1, 0, -1);
					*(uint8_t*)&g_levelActors[1].actorFlags &= 127u;
					g_unk52FADC = 0;
					g_unk52FAD8 = 0;
				}
			}

			if (g_unk52F2F8 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_21;
				g_unk52F2F8 = 2;
				g_unk52FAD8 = 1;
			}
			else if (g_unk52F2F8 != 2)
			{
				goto LABEL_21;
			}

			if (g_buzzActor.unkShort40)
			{
				if (g_buzzActor.posAngles.pos.y >= -82330)
					goto LABEL_20;

				if (Actor::IsInsideBounds((Actor::Toy2Actor*)&g_buzzActor, -468572, -393436, 76232, 102728))
				{
					g_unk52F2F8 = 3;
					g_unk52AD64 = 3;
					Collectables::Activate(2, 0);
					goto LABEL_21;
				}

				if (! Actor::IsInsideBounds((Actor::Toy2Actor*)&g_buzzActor, -37452, -8268, -392596, -252820))
				LABEL_20:
					g_unk52F2F8 = 4;
			}
		LABEL_21:

			v4 = g_unk52FAD8;
			v5 = (int32_t)Renderer::g_frameDelta;

			if (g_unk52FAD8)
			{
				g_unk52FADC += 16 * (int32_t)Renderer::g_frameDelta;
				v6 = g_unk52FADC;

				if (g_unk52FADC > 3000)
				{
					v6 = 3000;
					g_unk52FADC = 3000;
				}

				if (g_unk52F2F8 == 2)
				{
					AudioManager::g_pitchArray[0] = v6 + 2048;
					AudioManager::PlaySoundEffect(126, &g_levelActors[1].transform.pos);
					v5 = (int32_t)Renderer::g_frameDelta;
				}

				v7 = Levels::g_recordData[13];
				v4 = v5 * g_unk52FADC + g_unk52FAD8;
				((uint16_t*)&v8)[1] = 0;
				g_unk52FAD8 = v4;
				*(uint16_t*)&v8 = Levels::g_recordData[13]->recordCount;

				if (v4 >> 16 > Levels::g_recordData[13]->recordCount - 2)
				{
					*(uint8_t*)&g_levelActors[1].actorFlags |= 0x80u;
					v4 = (v8 << 16) - 65537;
					g_unk52FAD8 = v4;

					if (g_unk52F2F8 != 3)
						g_unk52F2F8 = 4;
				}
			}
			else
			{
				v7 = Levels::g_recordData[13];
			}

			v9 = v4 >> 16;
			v10 = v7->data[v9].z;
			z = v7->data[v9 + 1].z;

			v14 = (Numerics::g_sinCosLUT[g_unk52FA84] >> 2) + (((v4 & 0xFFFF) * (v7->data[v9 + 1].y - v7->data[v9].y)) >> 11);

			g_levelActors[1].transform.pos.x = 32 * v7->data[v9].x + (((v4 & 0xFFFF) * (v7->data[v9 + 1].x - v7->data[v9].x)) >> 11);
			g_levelActors[1].transform.pos.y = v14 + 32 * v7->data[v9].y;
			g_levelActors[1].transform.pos.z = (((v4 & 0xFFFF) * (z - v10)) >> 11) + 32 * v7->data[v9].z;

			g_levelActors[1].motionTargetPos = g_levelActors[1].transform.pos;
			g_levelActors[1].boundary = g_levelActors[1].transform.pos;

			g_unk52FA84 = (g_unk52FA84 + 32 * v5) & 0xFFF;

			if (g_unk52F2F8 == 4 && (g_levelActors[1].actorFlags & 2) == 0)
			{
				g_levelActors[1].actorPhase = 0;
				g_unk52FAD8 = 0;
				g_unk52FADC = 0;
				g_unk52F2F8 = 0;
			}
			if (Actor::IsInsideBounds((Actor::Toy2Actor*)&g_buzzActor, 171683, 633635, -455667, -25203))
			{
				FireLaser(0);
				FireLaser(1);
			}

			if (g_curSectorCopy2 != 4)
				goto LABEL_114;

			if (g_clawInitialized || ! Nu3D::Math::IsWithinDistanceXZ(&g_tokenPos, &g_buzzActor.posAngles.pos, 1000))
			{
				l_randByte1 = g_clawSwingZ;
			}
			else
			{
				g_tokenPos.x += 96 * (128 - *g_randDatBufferPtr++);
				g_tokenPos.z += 96 * (128 - *g_randDatBufferPtr++);

				l_randByte1 = *g_randDatBufferPtr << 8;
				g_clawSwingZ = l_randByte1;
				l_randByte2 = *++g_randDatBufferPtr;

				g_clawInitialized = 1;
				g_clawSwingX = l_randByte2 << 8;
				++g_randDatBufferPtr;
			}

			if (g_buzzActor.collisionFlags && Buzz::g_groundSlamTicker && Buzz::g_unk52B81C == 8)
			{
				v17 = g_unk52FAE4;

				if (g_unk52FAE4)
					goto LABEL_48;

				g_unk52FAE4 = 60;

				if (g_clawState < 3)
					++g_clawState;

				Nu3D::Link::SetScaleFromFixedOffsets(13, 0, 0, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(14, 4096, 4096, 4096);
				Levels::Method3(0, 1);
				l_randByte1 = g_clawSwingZ;
			}

			v17 = g_unk52FAE4;

			if (! g_unk52FAE4)
			{
				v18 = (int32_t)Renderer::g_frameDelta;
				goto LABEL_51;
			}
		LABEL_48:

			v18 = (int32_t)Renderer::g_frameDelta;
			g_unk52FAE4 = v17 - (int32_t)Renderer::g_frameDelta;

			if (v17 - (int32_t)Renderer::g_frameDelta <= 0)
			{
				g_unk52FAE4 = 0;
				Nu3D::Link::SetScaleFromFixedOffsets(13, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(14, 0, 0, 0);
				v18 = (int32_t)Renderer::g_frameDelta;
				l_randByte1 = g_clawSwingZ;
			}
		LABEL_51:

			switch (g_clawState)
			{
				case 1:
					v19 = (v18 << 7) + g_clawSwingX;
					g_clawSwingX = v19;

					if (v19 >= 0x10000)
						g_clawSwingX = v19 - 0x10000;

					v20 = 6144;
					goto LABEL_61;
				case 2:
					v21 = (v18 << 7) + l_randByte1;
					g_clawSwingZ = v21;

					if (v21 >= 0x10000)
						g_clawSwingZ = v21 - 0x10000;

					v20 = 6656;
					goto LABEL_61;
				case 3:
					g_clawDropDepth += v18 << 7;

					if (g_clawDropDepth >= 20480)
					{
						g_clawState = 4;
						g_clawHoldTimer = 0;
						g_clawDropDepth = 20480;
					}
					goto LABEL_60;
				case 4:
					v25 = v18 + g_clawHoldTimer;
					g_clawHoldTimer += v18;

					if (g_clawHoldTimer >= 60 && v25 - v18 < 60)
					{
						Nu3D::Link::SetScaleFromFixedOffsets(11, 4096, 4096, 4096);
						Nu3D::Link::SetScaleFromFixedOffsets(10, 0, 0, 0);
						AudioManager::PlaySoundEffect(129, &g_clawHomePos);
						l_randByte1 = g_clawSwingZ;
						v25 = g_clawHoldTimer;
					}

					if (v25 < 120)
						break;

					g_clawState = 5;
					Nu3D::Link::GetCurrentPosFixed(10, &l_clawPos);
					g_clawCatchResult = Nu3D::Math::IsWithinDistanceXZ(&l_clawPos, &g_tokenPos, 15);

					goto LABEL_62;
				case 5:
					v20 = 5632;
					g_clawDropDepth += -128 * v18;

					if (g_clawDropDepth < 0)
					{
						if (g_clawCatchResult == 1)
						{
							g_clawState = 6;
							g_clawDropDepth = 0;

							if (g_clawSwingX >= 0x8000)
								g_clawSwingX = 0x10000 - g_clawSwingX;

							if (l_randByte1 >= 0x8000)
								g_clawSwingZ = 0x10000 - l_randByte1;
						}
						else
						{
							g_clawState = 0;
							g_clawDropDepth = 0;
							Nu3D::Link::SetScaleFromFixedOffsets(10, 4096, 4096, 4096);
							Nu3D::Link::SetScaleFromFixedOffsets(11, 0, 0, 0);
							AudioManager::PlaySoundEffect(129, &g_clawHomePos);
						}
					}

					goto LABEL_61;
				case 6:
					v26 = v18 << 7;
					g_clawSwingX -= v26;

					if (g_clawSwingX < 0)
						g_clawSwingX = 0;

					v27 = v26 + l_randByte1;
					g_clawSwingZ = v27;

					if (v27 > 0x8000)
					{
						v27 = 0x8000;
						g_clawSwingZ = 0x8000;
					}

					if (g_clawSwingX || v27 != 0x8000)
					{
					LABEL_60:
						v20 = 5632;
					LABEL_61:
						AudioManager::g_pitchArray[0] = 2 * v20;
						AudioManager::PlaySoundEffect(130, &g_clawHomePos);
					}
					else
					{
						Nu3D::Link::SetScaleFromFixedOffsets(10, 4096, 4096, 4096);
						Nu3D::Link::SetScaleFromFixedOffsets(11, 0, 0, 0);

						g_clawState = 7;
						g_clawCatchResult = 2;
						g_unk52FB1C = 0;
						AudioManager::PlaySoundEffect(129, &g_clawHomePos);
					}
				LABEL_62:

					l_randByte1 = g_clawSwingZ;
					break;
				default:
					break;
			}

			v46 = g_clawSwingX;

			if (g_clawSwingX >= 0x8000)
				v46 = 0x10000 - g_clawSwingX;

			if (l_randByte1 >= 0x8000)
				l_randByte1 = 0x10000 - l_randByte1;

			v22 = g_clawHomePos.z + l_randByte1;
			v48 = g_clawHomePos.y + g_clawDropDepth;
			v47 = g_clawHomePos.x + v46;

			Nu3D::Link::SetPositionRawAndCommit(10, v47 >> 5, (g_clawHomePos.y + g_clawDropDepth) >> 5, v22 >> 5);
			Nu3D::Link::SetPositionRawAndCommit(11, v47 >> 5, v48 >> 5, v22 >> 5);
			Nu3D::Link::SetPositionRawAndCommit(12, v47 >> 5, (v48 >> 5) - 1056, v22 >> 5);
			Nu3D::Link::SetPositionRawAndCommit(16, v47 >> 5, (g_clawHomePos.y >> 5) - 1056, v22 >> 5);

			if (Renderer::Shadows::g_shadowCount < 47)
			{
				v23 = Renderer::Shadows::g_shadowCount;
				v24 = Renderer::Shadows::g_shadowCount + 1;
				Renderer::Shadows::g_shadowRecords[v23].worldX = v47;
				Renderer::Shadows::g_shadowRecords[v23].groundY = -64000;
				Renderer::Shadows::g_shadowRecords[v23].worldZ = v22;
				Renderer::Shadows::g_shadowRecords[v23].size = 256;
				Renderer::Shadows::g_shadowCount = v24;
			}

			if (g_clawCatchResult == 1)
			{
				g_tokenPos.x = v47;
				g_tokenPos.y = v48 + 4096;
				g_tokenPos.z = v22;
			}
			else if (g_clawCatchResult == 2)
			{
				v28 = (int32_t)Renderer::g_frameDelta;
				g_unk52FB1C += 32 * (int32_t)Renderer::g_frameDelta;
				g_tokenPos.y += (int32_t)Renderer::g_frameDelta * g_unk52FB1C;

				if (g_tokenPos.y > -27969)
				{
					g_unk52FA80->props.flags |= 0x28u;
					v28 = (int32_t)Renderer::g_frameDelta;
					g_tokenPos.x -= (int32_t)Renderer::g_frameDelta << 8;
					g_tokenPos.z += (int32_t)Renderer::g_frameDelta << 8;
				}

				if (g_tokenPos.y > -6144)
				{
					g_tokenPos.y = -6144;
					g_unk52FB1C /= -2;

					if (abs(g_unk52FB1C) < 256)
						g_clawCatchResult = 3;
				}

				goto LABEL_97;
			}

			v28 = (int32_t)Renderer::g_frameDelta;
		LABEL_97:
			g_unk52FB64 = (g_unk52FB64 + 8 * v28) & 0xFFF;
			g_unk52FB60 = (g_unk52FB60 + v28 * (Numerics::g_sinCosLUT[g_unk52FB64] >> 11)) & 0xFFF;

			Nu3D::Link::SetRotationRelative8bit(10, 0, g_unk52FB60, 0);
			Nu3D::Link::SetRotationRelative8bit(11, 0, g_unk52FB60, 0);

			if (g_unk52FA80->pos.y != 0x80000000)
			{
				Nu3D::Link::SetPositionRawAndCommit(53, g_tokenPos.x >> 5, g_tokenPos.y >> 5, g_tokenPos.z >> 5);

				g_unk52FA80->pos.x = g_tokenPos.x >> 5;
				g_unk52FA80->pos.y = g_tokenPos.y >> 5;
				g_unk52FA80->pos.z = g_tokenPos.z >> 5;

				if (g_clawCatchResult < 2)
					g_unk52FA80->props.floorPos = -2000;
				else
					g_unk52FA80->props.floorPos = 0;
			}

			Renderer::BlitTextureByIndexOffset(5, 0, 0, 64, 64, 0, g_unk52FA90 / 2, 0, 64);
			g_unk52FA90 = ((int32_t)Renderer::g_frameDelta + g_unk52FA90) & 0x7F;
			g_unk52FB6C = (g_unk52FB6C + 7 * (int32_t)Renderer::g_frameDelta) & 0xFFF;
			g_unk52FB68 = (g_unk52FB68 + 6 * (int32_t)Renderer::g_frameDelta) & 0xFFF;
			g_unk52FB78 = (g_unk52FB78 + 8 * (int32_t)Renderer::g_frameDelta) & 0xFFF;
			g_unk52FB70 = ((int32_t)Renderer::g_frameDelta + g_unk52FB70 + 10 * (int32_t)Renderer::g_frameDelta) & 0xFFF;

			Nu3D::Link::SetPositionRawAndCommit(20, g_unk52FAA8.x >> 5, (g_unk52FAA8.y >> 5) + (Numerics::g_sinCosLUT[g_unk52FB78] >> 6), g_unk52FAA8.z >> 5);

			Nu3D::Link::SetPositionRawAndCommit(21, (g_unk52FA98.x >> 5) + (Numerics::g_sinCosLUT[g_unk52FB6C] >> 6), g_unk52FA98.y >> 5, g_unk52FA98.z >> 5);

			Nu3D::Link::SetPositionRawAndCommit(22, (g_unk52FAC8.x >> 5) + (Numerics::g_sinCosLUT[g_unk52FB68] >> 6), g_unk52FAC8.y >> 5, g_unk52FAC8.z >> 5);

			Nu3D::Link::SetPositionRawAndCommit(23, g_unk52FAB8.x >> 5, (g_unk52FAB8.y >> 5) + (Numerics::g_sinCosLUT[g_unk52FB70] >> 6), g_unk52FAB8.z >> 5);
			g_unk52FB80 -= (int32_t)Renderer::g_frameDelta;

			if (g_unk52FB80 <= 0)
			{
				g_unk52FB80 = (*g_randDatBufferPtr++ & 0x7F) + 10;
				v29 = *g_randDatBufferPtr & 3;
				v30 = ++g_randDatBufferPtr;

				if (v29 == 3)
					v29 = 0;

				v31 = v29 + 123;
				v32 = *v30 & 3;
				g_randDatBufferPtr = v30 + 1;

				switch (v32)
				{
					case 0:
						AudioManager::PlaySoundEffect(v31, &g_unk52FAA8);
						break;
					case 1:
						AudioManager::PlaySoundEffect(v31, &g_unk52FA98);
						break;
					case 2:
						AudioManager::PlaySoundEffect(v31, &g_unk52FAC8);
						break;
					case 3:
						AudioManager::PlaySoundEffect(v31, &g_unk52FAB8);
						break;
					default:
						break;
				}
			}

			if (! g_unk52FB8C)
			{
				Nu3D::Link::SetRotationRelative8bit(19, Numerics::g_sinCosLUT[64 * g_tickAccumulators.tick64Acc] >> 7, 0, 0);
				if (g_buzzActor.posAngles.pos.y < -109317 && g_buzzActor.collisionFlags
					&& Actor::IsInsideBounds((Actor::Toy2Actor*)&g_buzzActor, 431100, 480252, 307948, 341740))
				{
					MoveableObject::g_moveableObjects[2].pushState = -1;
					g_unk52FB8C = 1;
					Nu3D::Link::SetRotationRelative8bit(19, 0, 0, 0);
				}
			}
		LABEL_114:

			if (Actor::IsInsideBounds((Actor::Toy2Actor*)&g_buzzActor, 209422, 366094, 158154, 302474))
			{
				v33 = -33280;
				Buzz::g_currentFluidType = 3;
				Buzz::g_underwaterYLevel = -33280;
			}
			else
			{
				v33 = 0;
				Buzz::g_currentFluidType = 0;
				Buzz::g_underwaterYLevel = 0;
			}

			if ((g_buzzActor.actorFlags & 0x100) != 0 && g_tickState[4] && (g_buzzActor.unkVar30 > 32 || abs(g_buzzActor.gravityVel) > 32))
			{
				v34 = Nu3D::Particles::SpawnFromPreset(g_buzzActor.posAngles.pos.x, v33, g_buzzActor.posAngles.pos.z, 95, 4);
				v35 = *g_randDatBufferPtr++ & 3;
				v34->colourR = (uint8_t)g_unk4F2F18[v35].r;
				v34->colourG = (uint8_t)g_unk4F2F18[v35].g;
				v34->colourB = (uint8_t)g_unk4F2F18[v35].b;
			}

			if (g_curSectorCopy2 == 2 && g_buzzActor.posAngles.pos.x < 111719)
			{
				g_unk52FB98 += (int32_t)Renderer::g_frameDelta;

				if (g_unk52FB98 > 200)
				{
					if (g_tickState[9])
					{
						l_clawPos.x = 32 * Levels::g_recordData[16]->data[g_unk52FAF8].x;
						l_clawPos.y = 32 * Levels::g_recordData[16]->data[g_unk52FAF8].y;
						v36 = Levels::g_recordData[16]->data[++g_unk52FAF8 - 1].z;
						l_clawPos.z = 32 * v36;

						if (g_unk52FAF8 >= Levels::g_recordData[16]->recordCount)
						{
							g_unk52FAF8 = 0;
							g_unk52FB98 = 0;
						}

						if (Nu3D::Math::IsWithinDistance(&l_clawPos, &g_buzzActor.posAngles.pos, 768))
						{
							v37 = (l_clawPos.z - g_buzzActor.posAngles.pos.z) >> 5;
							v38 = (l_clawPos.x - g_buzzActor.posAngles.pos.x) >> 5;
							v39 = l_clawPos.y - g_buzzActor.posAngles.pos.y;

							v40 = Nu3D::Math::CartesianToFixedAngle(v38, v37) & 0xFFF;
							v41 = (int32_t)sqrt((double)(v38 * v38 + v37 * v37));
							v42 = 2 * v41 / 3;

							Nu3D::Particles::SpawnInstance(
								l_clawPos.x,
								l_clawPos.y,
								l_clawPos.z,
								v42 * Numerics::g_sinCosLUT[(v40 - 2048) & 0xFFF] / 0x4000,
								-128 * ((v41 << 12) / v42) / 256 - (v39 << 7) / ((v41 << 12) / v42),
								v42 * Numerics::g_sinCosLUT[(v40 - 1024) & 0xFFF] / 0x4000,
								128,
								0,
								0,
								96);

							AudioManager::PlaySoundEffect(13, &l_clawPos);
						}
					}
				}
			}

			Nu3D::Link::GetCurrentPosFixed(0, &l_clawPos);
			Nu3D::Link::SetPositionRawAndCommit(17, l_clawPos.x >> 7, l_clawPos.y >> 7, l_clawPos.z >> 7);
			Nu3D::Link::GetCurrentPosFixed(15, &l_clawPos);
			Nu3D::Link::SetPositionRawAndCommit(18, l_clawPos.x >> 7, l_clawPos.y >> 7, l_clawPos.z >> 7);

			if ((g_levelActors[2].actorFlags & 1) != 0)
				Renderer::BlitTextureByIndexOffset(18, 64, 0, 20, 64, 0, g_tickAccumulators.tick64Acc, 20, 0);

			v43 = 0;

			while (Nu3D::Particles::g_particleInstances[v43].typeId != 114 || ! Nu3D::Particles::g_particleInstances[v43].lifetime)
			{
				++v43;
				if (v43 >= 64)
				{
					Nu3D::Link::SetPositionRawAndCommit(25, -10861, -18786, 9697);
					break;
				}
			}

			v44 = g_unk52FAE0;

			if (! g_unk52FAE0)
			{
				if (! g_buzzActor.collisionFlags || g_curSectorCopy2 != 5 || g_buzzActor.posAngles.pos.y <= -162432)
					goto LABEL_150;

				g_unk52FAE0 = 1;
				Dialogue::Begin(
					40,
					17,
					"hey space ranger! return to your ship immediately!... defeat the ^buzz lightyear buggy^ boss to get a pizza planet ^token^!",
					-1,
					0,
					-1);
				v44 = g_unk52FAE0;
			}

			if (v44 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_150;

				g_unk830D44 = 180;
				g_unk52FAE0 = 2;
				g_levelActors[40].movementData = Actor::g_movementData[35] + 14;
				g_levelActors[40].unkShort1 = 0;

				g_levelActors[40].creatureRam->initialFacingAngle = 0;
				v44 = g_unk52FAE0;
				g_levelActors[40].unkShort4 = 400;
				g_unk52FB9C = 600;
			}

			if (v44 >= 3)
			{
				if (v44 < 120)
				{
					g_unk52FAE0 = (int32_t)Renderer::g_frameDelta + v44;
					PlayLevelMusic();
					return;
				}

				if (v44 != 200)
				{
					Collectables::Activate(4, 0);
					g_unk52FAE0 = 200;
				}
			}
		LABEL_150:

			PlayLevelMusic();
		}

	}
}

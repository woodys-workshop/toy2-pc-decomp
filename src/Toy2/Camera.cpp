#include "Toy2/Camera.h"
#include "Nu3D/Math.h"
#include "InputManager.h"
#include "Toy2/Toy2.h"
#include "Toy2/Buzz.h"
#include "Toy2/Collision.h"
#include "SaveManager.h"
#include "Toy2/Actor.h"
#include "Nu3D/Collision.h"
#include "Nu3D/Link.h"
#include "Nu3D/Particles.h"
#include "Renderer/Renderer.h"
#include "Numerics.h"
#include "Nu3D/Viewport.h"
#include "Nu3D/Portal.h"
#include "Toy2/Portal.h"
#include "Toy2/Platform.h"
#include "AudioManager/AudioManager.h"
#include "CharacterLoader.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

namespace Toy2
{
	namespace Camera
	{
		// GLOBAL: TOY2 0x0052ADC0
		Nu3D::Camera::ActiveCameraTransform g_activeCameraTransform;

		// GLOBAL: TOY2 0x0052F3A0
		GameplayCameraState g_gameplayCameraState;

		// GLOBAL: TOY2 0x0052B7E8
		ScriptedCameraState g_scriptedCameraState;

		// GLOBAL: TOY2 0x0052B810
		int16_t g_scriptedCameraRoll;

		// GLOBAL: TOY2 0x0052B814
		int16_t g_cameraFadeState;

		// GLOBAL: TOY2 0x0052B816
		int16_t g_inCutscene;

		// GLOBAL: TOY2 0x0050A13C
		int32_t g_cameraState;

		// GLOBAL: TOY2 0x0050A1F4
		int32_t g_cutsceneTimer;

		// GLOBAL: TOY2 0x0050A148
		int32_t g_cutsceneFrameTimer;

		// GLOBAL: TOY2 0x0050A140
		int32_t g_cameraBlendTimer;

		// GLOBAL: TOY2 0x0050A128
		int32_t g_buzzCameraDistance;

		// GLOBAL: TOY2 0x0050A118
		Vector3I g_unk50A118;

		// GLOBAL: TOY2 0x0052F1CC
		int32_t g_freezeBuzz;

		// GLOBAL: TOY2 0x00529E4C
		int32_t g_unk529E4C;

		// GLOBAL: TOY2 0x0052F118
		int32_t g_unk52F118;

		// GLOBAL: TOY2 0x0050A4E8
		int32_t g_unk50A4E8;

		// GLOBAL: TOY2 0x0052AD98
		int32_t g_unk52AD98;

		// GLOBAL: TOY2 0x0050A4B4
		int32_t g_unk50A4B4;

		// GLOBAL: TOY2 0x0050A4B8
		int32_t g_unk50A4B8;

		// GLOBAL: TOY2 0x0050A12C
		int32_t g_unk50A12C;

		// GLOBAL: TOY2 0x0050A4E4
		int32_t g_unk50A4E4;

		// GLOBAL: TOY2 0x0050A534
		int32_t g_unk50A534;

		// GLOBAL: TOY2 0x0050A510
		int32_t g_unk50A510;

		// GLOBAL: TOY2 0x0050A4FC
		int32_t g_lockOnActorIdx;

		// GLOBAL: TOY2 0x0050A540
		int32_t g_unk50A540;

		// GLOBAL: TOY2 0x0050A4E0
		int32_t g_unk50A4E0;

		// GLOBAL: TOY2 0x0050A294
		int32_t g_unk50A294;

		// GLOBAL: TOY2 0x0050A134
		int32_t g_unk50A134;

		// GLOBAL: TOY2 0x0050A53C
		int32_t g_unk50A53C;

		// GLOBAL: TOY2 0x0050A144
		int32_t g_unk50A144;

		// GLOBAL: TOY2 0x0050A0CC
		int32_t g_unk50A0CC;

		// GLOBAL: TOY2 0x0050A514
		int32_t g_unk50A514;

		// GLOBAL: TOY2 0x0050A4BC
		int32_t g_unk50A4BC;

		// GLOBAL: TOY2 0x004F5AB4
		int16_t g_cameraAngleOffsetTable[10] = { 0, 512, 1024, 1536, 2048, 2560, 3072, 3584, 4096, 0 };

		// GLOBAL: TOY2 0x004F5AC8
		int16_t g_autoRotationSequence[16] = { 0, 0, 1024, 512, 2048, 0, 1536, 0, 3072, 3584, 0, 0, 2560, 0, 0, 0 };

		// STUB: TOY2 0x00402030
		void InitCutsceneCamera(Vector3I* focusPos, Vector3I* pos) {}

		// STUB: TOY2 0x004020F0
		void BeginScriptedCutsceneAtPoint(Vector3I* target, int32_t duration, int32_t orbitRadius) {}

		// STUB: TOY2 0x00402290
		void BeginScriptedCutsceneOnBuzz(int32_t duration) {}

		// FUNCTION: TOY2 0x00403450
		void InitGameplayCamera(Toy2::Camera::GameplayCameraState* cameraState, Toy2::Buzz::Toy2BuzzActor* buzzActor)
		{
			g_activeCameraTransform.pos.x = 0;
			g_activeCameraTransform.pos.y = 0;
			g_activeCameraTransform.pos.z = 0;
			g_activeCameraTransform.angles.pitch = 0;
			g_activeCameraTransform.angles.yaw = 0;

			g_activeCameraTransform.roll = 0;
			g_activeCameraTransform.unkShort = 0;

			memset(cameraState, 0, sizeof(GameplayCameraState));

			// the original clears 48 bytes here, the 8 bytes after the scripted camera state are the roll, fade state and cutscene flags
			memset(&g_scriptedCameraState, 0, sizeof(ScriptedCameraState));
			g_scriptedCameraRoll = 0;
			g_cameraFadeState = 0;
			g_inCutscene = 0;

			cameraState->distance = 1200;

			uint16_t yaw = buzzActor->posAngles.angles.yaw;

			cameraState->angles.yaw = yaw;
			cameraState->orbitYaw = yaw;

			cameraState->angles.pitch = 0;
			cameraState->roll = 0;
			cameraState->pos.x = buzzActor->posAngles.pos.x + ((1200 * Numerics::g_sinCosLUT[(yaw - 2048) & 0xFFF]) >> 9);
			cameraState->pos.z = buzzActor->posAngles.pos.z + ((1200 * Numerics::g_sinCosLUT[(yaw - 1024) & 0xFFF]) >> 9);
			cameraState->pos.y = buzzActor->posAngles.pos.y;

			int32_t groundHeight = Nu3D::Collision::GetGroundHeight((WorldPos*)cameraState, 0);
			cameraState->pos.y = groundHeight;

			if (groundHeight == 0x80000000)
				cameraState->pos.y = buzzActor->posAngles.pos.y;

			int32_t buzzY = buzzActor->posAngles.pos.y;

			if (cameraState->pos.y > buzzY - 0x4000)
				cameraState->pos.y = buzzY - 0x4000;

			int32_t cameraY = cameraState->pos.y - 12800;

			cameraState->targetPos.x = cameraState->pos.x;
			cameraState->pos.y = cameraY;
			cameraState->targetPos.y = cameraY;

			int32_t cameraZ = cameraState->pos.z;

			cameraState->targetPitch = 0;
			cameraState->targetPos.z = cameraZ;
			cameraState->unkShort6 = 0;
			cameraState->orbitPitch = 64;

			g_unk529E4C = 0;
			g_unk52F118 = 0;
			g_freezeBuzz = 0;

			cameraState->collisionFlags = 0;
			cameraState->followY = g_buzzActor.posAngles.pos.y;
			cameraState->smoothedY = g_buzzActor.posAngles.pos.y;
			cameraState->smoothedYaw = 0;

			g_cameraState = 0;
			g_unk50A118.x = 0x80000000;
			g_cutsceneTimer = 0;
			g_unk50A4E8 = 0;
			g_unk52AD98 = 16;
			g_unk50A4B4 = 0;
			g_unk50A4B8 = 0;
			g_unk50A12C = 0;
			g_unk50A4E4 = 0;

			g_buzzCameraDistance = 1200;

			g_unk50A534 = 0;
			g_unk50A510 = 0;
			g_unk50A4E0 = 0;
			g_cameraBlendTimer = 0;
			Renderer::g_enableCinematicBars = 0;
			g_unk50A294 = 0;
			g_unk50A134 = 0;
			g_unk50A53C = 0;
			g_unk50A144 = 0;
			g_unk50A0CC = 0;
			g_cutsceneFrameTimer = 0;
			g_unk50A514 = 0;
			g_unk50A4BC = 0;

			Nu3D::Link::SetScaleFromFixedOffsets(45, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(46, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(47, 0, 0, 0);
		}

		// FUNCTION: TOY2 0x0048A4C0
		int32_t PrepareCameraCollisionCandidates(int32_t* position, int32_t displacementX, int32_t displacementY, int32_t displacementZ, int32_t radius)
		{
			// the triangles around the camera are gathered in an extra cache entry, after the 8 of the ground queries
			Collision::CollisionCacheEntry* entry = (Collision::CollisionCacheEntry*)Collision::g_unk72871C + 8;
			Collision::CollisionMeshInstance* mesh;
			Collision::CollisionTreeNode* node;
			Collision::CollisionTriangle* triangle;
			int16_t* meshList = (int16_t*)Collision::g_mathScratch;
			int32_t minX;
			int32_t minY;
			int32_t minZ;
			int32_t extX;
			int32_t extY;
			int32_t extZ;
			int32_t pad;
			int32_t widePad;
			int32_t extXWide;
			int32_t extYWide;
			int32_t extZWide;
			int32_t cellExtX;
			int32_t cellExtZ;
			int32_t meshCount = 0;
			int32_t cellX;
			int32_t cellY;
			int32_t cellZ;
			int32_t meshIdx;
			int32_t result;
			int32_t i;
			int32_t j;
			int32_t count;

			Collision::g_unk72D2AC = 0;
			Collision::g_unk7291D8 = 0;
			entry->count = 0;

			if (displacementX >= 0)
			{
				minX = position[0] + displacementX + 25600;
				extX = displacementX + 51200;
			}
			else
			{
				minX = position[0] + 25600;
				extX = 51200 - displacementX;
			}

			if (displacementY >= 0)
			{
				minY = position[1] + displacementY + 25600;
				extY = displacementY + 51200;
			}
			else
			{
				minY = position[1] + 25600;
				extY = 51200 - displacementY;
			}

			if (displacementZ >= 0)
			{
				minZ = position[2] + displacementZ + 25600;
				extZ = displacementZ + 51200;
			}
			else
			{
				minZ = position[2] + 25600;
				extZ = 51200 - displacementZ;
			}

			pad = 5700 * radius / 4096;
			minX = pad + minX;
			minY = pad + minY;
			minZ = pad + minZ;
			widePad = 5700 * radius / 2048;
			extYWide = widePad + extY;
			extZWide = widePad + extZ;
			extXWide = widePad + extX;

			entry->x = minX;
			entry->y = minY;
			entry->z = minZ;
			entry->unkC = minX - extXWide;
			entry->unk10 = minY - extYWide;
			entry->unk14 = minZ - extZWide;

			cellExtX = ((uint32_t)extXWide + 31) >> 5;
			cellExtZ = ((uint32_t)extZWide + 31) >> 5;

			result = Collision::g_unk729120;

			for (i = 0; i < Collision::g_unk729120; i++)
			{
				Collision::CollisionGridCell* cell = &Collision::g_collisionGrid[i];

				if ((uint32_t)(minX - cell->minX) < (uint32_t)(extXWide + cell->extX) && (uint32_t)(minZ - cell->minZ) < (uint32_t)(extZWide + cell->extZ))
				{
					for (j = 0; j < cell->count; j++)
						meshList[meshCount + j] = Collision::g_collisionMeshList[cell->start + j];

					meshCount += cell->count;
				}

				result = Collision::g_unk729120 - i - 1;
			}

			for (i = 0; i < meshCount; i++)
			{
				meshIdx = meshList[i];
				mesh = &Collision::g_collisionMeshInstances[meshIdx];

				if ((uint32_t)(minX - mesh->boundsMin.x) < (uint32_t)(extXWide + mesh->boundsExt.x)
					&& (uint32_t)(minY - mesh->boundsMin.y) < (uint32_t)(extYWide + mesh->boundsExt.y)
					&& (uint32_t)(minZ - mesh->boundsMin.z) < (uint32_t)(extZWide + mesh->boundsExt.z) && mesh->typeFlags && (mesh->unk & 0x200) == 0)
				{
					cellY = (minY - mesh->origin.y) / 32;
					node = (Collision::CollisionTreeNode*)mesh->collisionTree;
					cellX = (minX - mesh->origin.x) / 32;
					cellZ = (minZ - mesh->origin.z) / 32;

					while (node->flag >= 0)
					{
						count = node->count;
						triangle = (Collision::CollisionTriangle*)(node + 1);

						if ((uint32_t)(cellX - node->minX) >= (uint32_t)(cellExtX + node->extX)
							|| (uint32_t)(cellZ - node->minZ) >= (uint32_t)(cellExtZ + node->extZ))
						{
							node = (Collision::CollisionTreeNode*)(triangle + count);
						}
						else
						{
							for (j = 0; j < count; j++)
							{
								if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(cellExtX + triangle->extX)
									&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z) < (uint32_t)(cellExtZ + (triangle->extZByte << 6))
									&& (uint32_t)(cellY - (triangle->minYByte << 6) - triangle->v0y)
										< (uint32_t)(((extYWide + 31) >> 5) + (triangle->extYByte << 6)))
								{
									if (entry->count < 240)
									{
										entry->triangles[entry->count] = triangle;
										entry->meshIdx[entry->count] = (int16_t)meshIdx;
										entry->count++;
									}
								}

								triangle++;
							}

							node = (Collision::CollisionTreeNode*)triangle;
						}
					}

					result = 0;
				}
			}

			return result;
		}

		// FUNCTION: TOY2 0x00403640
		void SmoothToTarget(Toy2::Camera::ScriptedCameraState* cameraState)
		{
			int32_t eyeX = cameraState->eyePos.x;
			int32_t focusY = cameraState->focusPos.y;
			int32_t eyeY = cameraState->eyePos.y;
			int32_t eyeZ;
			int32_t focusX;
			int32_t deltaX;
			int32_t deltaY;
			int32_t deltaZ;
			int32_t yawDelta;
			int32_t pitchDelta;
			int16_t pitchRaw;

			cameraState->pos.x += (eyeX - cameraState->pos.x) >> 3;
			eyeZ = cameraState->eyePos.z;

			cameraState->pos.y += (eyeY - cameraState->pos.y) >> 3;
			focusX = cameraState->focusPos.x;

			cameraState->pos.z += (eyeZ - cameraState->pos.z) >> 3;
			deltaZ = (cameraState->focusPos.z - eyeZ) >> 5;

			deltaX = (focusX - eyeX) >> 5;
			deltaY = (focusY - eyeY) >> 5;

			yawDelta = (Nu3D::Math::CartesianToFixedAngle(deltaX, deltaZ) - cameraState->angles.yaw) & 0xFFF;

			if (yawDelta >= 2048)
				yawDelta -= 4096;

			cameraState->angles.yaw = (cameraState->angles.yaw + (yawDelta >> 2)) & 0xFFF;

			if (deltaY >= 0)
				pitchRaw = Nu3D::Math::CartesianToFixedAngle(-(deltaY * deltaY), deltaX * deltaX + deltaZ * deltaZ);
			else
				pitchRaw = Nu3D::Math::CartesianToFixedAngle(deltaY * deltaY, deltaX * deltaX + deltaZ * deltaZ);

			pitchDelta = (-pitchRaw - cameraState->angles.pitch) & 0xFFF;

			if (pitchDelta >= 2048)
				pitchDelta -= 4096;

			cameraState->angles.pitch = (cameraState->angles.pitch + (pitchDelta >> 2)) & 0xFFF;
		}

		// STUB: TOY2 0x00403730
		void SnapBehindBuzz(Toy2::Camera::GameplayCameraState* cameraState) {}

		// STUB: TOY2 0x004038E0
		void VisorMode(Toy2::Camera::GameplayCameraState* state) {}

		// FUNCTION: TOY2 0x004045E0
		void GameplayMode(Toy2::Camera::GameplayCameraState* state)
		{
			int32_t frameDelta = Renderer::g_frameDelta;
			int32_t snapToTarget = 0;
			int32_t moving = 0;

			int32_t speedMultiplier;
			int32_t newDistance;
			int32_t standing;
			int32_t v62;
			int32_t v84;
			int32_t v94;
			int32_t cameraPos[4];
			WorldPos startPos;
			WorldPos probePos;
			WorldPos displacement;

			if (Collision::IsMeshMoving() && g_buzzActor.collisionFlags)
			{
				speedMultiplier = frameDelta;
				g_unk50A4E0 += 2 * frameDelta;

				if (g_unk50A4E0 > 192)
					g_unk50A4E0 = 192;
			}
			else
			{
				speedMultiplier = frameDelta;
				g_unk50A4E0 -= frameDelta;

				if (g_unk50A4E0 < 0)
					g_unk50A4E0 = 0;
			}

			int32_t buzzZ = g_buzzActor.posAngles.pos.z;
			int32_t stateDistance = state->distance;
			int32_t buzzY = g_buzzActor.posAngles.pos.y;
			int32_t buzzX = g_buzzActor.posAngles.pos.x;
			int32_t cameraDistance = g_buzzCameraDistance;

			if (stateDistance > g_buzzCameraDistance)
			{
				newDistance = stateDistance - 32 * speedMultiplier;
				state->distance = newDistance;

				cameraDistance = g_buzzCameraDistance;

				if (newDistance >= g_buzzCameraDistance)
				{
					speedMultiplier = frameDelta;
				}
				else
				{
					state->distance = g_buzzCameraDistance;
					speedMultiplier = frameDelta;
					cameraDistance = g_buzzCameraDistance;
				}
			}

			stateDistance = state->distance;

			if (stateDistance < cameraDistance)
			{
				newDistance = 32 * speedMultiplier + stateDistance;
				state->distance = newDistance;

				if (newDistance > g_buzzCameraDistance)
					state->distance = g_buzzCameraDistance;

				speedMultiplier = frameDelta;
			}

			standing = (abs(g_buzzActor.velX) < 4 && abs(g_buzzActor.velForward) < 4 && ! Buzz::g_unk53C660 && g_buzzActor.unkShort40)
				|| (Buzz::g_unk53C668 > 0 && g_buzzActor.unkVar37 == 15);

			if (standing)
			{
				int32_t v13;

				// Buzz stands still, the camera can be turned around him
				int32_t v7 = 0;
				moving = 0;
				g_unk50A534 += speedMultiplier;
				int32_t v8 = g_unk50A534;

				if (g_unk50A534 - speedMultiplier < 66 && g_unk50A534 >= 66)
				{
					v8 = g_unk50A534 + 66;
					g_unk50A534 += 66;
				}

				if (v8 >= 240)
				{
					v8 = 240;
					v7 = 1;
					g_unk50A534 = 240;
				}

				if ((SaveManager::g_save0Data.cameraType & 0x40) != 0)
					goto LABEL_32;

				if ((InputManager::g_curButtonsPressed & INPUT_CANCEL) != 0 && (InputManager::g_prevButtonsPressed & INPUT_CANCEL) == 0
					&& (InputManager::g_directionInputState & (DIR_UP | DIR_RIGHT | DIR_DOWN | DIR_LEFT)) == 0)
				{
					snapToTarget = 1;
					state->orbitYaw = g_buzzActor.posAngles.angles.yaw;
					speedMultiplier = frameDelta;
					v8 = g_unk50A534;
				}

				if ((SaveManager::g_save0Data.cameraType & SaveManager::CAMERA_ACTIVE) != 0)
				{
					int32_t v9;
					int32_t v11;

				LABEL_32:
					if ((InputManager::g_directionInputState & (DIR_CAMERA_LEFT | DIR_CAMERA_RIGHT)) != 0)
					{
						int32_t v10;

						v9 = 640;

						if ((InputManager::g_directionInputState & DIR_CAMERA_RIGHT) != 0)
							v10 = 32 * speedMultiplier / 2 + g_buzzActor.facingAngle;
						else
							v10 = -32 * speedMultiplier / 2 + g_buzzActor.facingAngle;

						g_buzzActor.facingAngle = v10 & 0xFFF;
						g_buzzActor.posAngles.angles.yaw = v10 & 0xFFF;
					}
					else
					{
						v9 = 320;
					}

					if (v8 >= 100)
					{
						v11 = g_unk50A4E4;
					}
					else
					{
						v8 = 100;
						g_unk50A534 = 100;
						v11 = (g_buzzActor.facingAngle - state->orbitYaw) & 0xFFF;
						g_unk50A4E4 = v11;

						if (v11 > 0x800)
						{
							v11 -= 4095;
							g_unk50A4E4 = v11;
						}
					}

					if (abs(v11) < v9)
					{
						v11 = v9;
						g_unk50A4E4 = v9;
					}

					if (v8 > 164)
					{
						v11 = v9;
						g_unk50A4E4 = v9;
					}

					int32_t v12 = v8 - 100;

					if (v8 - 100 > 8)
						v12 = 8;

					v7 = 1;
					v13 = abs(v12 * v11) / 384;
				}
				else
				{
					v13 = 10;

					if (! v7)
						goto LABEL_58;
				}

				int32_t v15;
				v15 = (state->orbitYaw - g_buzzActor.posAngles.angles.yaw) & 0xFFF;

				if (v15 >= 0x800)
					v15 -= 4096;

				if (v15 >= 0)
				{
					if (v15 <= 0 || (state->orbitYaw = state->orbitYaw - speedMultiplier * v13, speedMultiplier = frameDelta, v15 - v13 * frameDelta > 0))
					{
					LABEL_58:
						if ((SaveManager::g_save0Data.cameraType & 0x40) == 0)
						{
							int32_t v16 = 0;

							if ((InputManager::g_directionInputState & 0x100) != 0)
								v16 = 32 * speedMultiplier / 2;

							if ((InputManager::g_directionInputState & 0x200) != 0)
								v16 = -32 * speedMultiplier / 2;

							if (v16)
							{
								g_unk50A534 = 66;
								state->orbitYaw += v16;
								v7 = 1;
							}
						}

						state->orbitYaw &= 0xFFF;

						if (! v7)
							goto LABEL_72;

						int32_t v18;
						v18 = state->orbitPitch;

						if (v18 <= 64)
						{
							if (Buzz::g_unk53C668 > 0)
								goto LABEL_72;

							int32_t v19;
							v19 = v18 + 8 * frameDelta;
							state->orbitPitch = v19;

							if (v19 < 64)
								goto LABEL_72;
						}
						else
						{
							v18 = v18 - 8 * frameDelta;
							state->orbitPitch = v18;

							if (v18 > 64)
							{
								int32_t v34;

							LABEL_72:
								int32_t v21 = state->followY - ((state->followY - buzzY + 4096) >> 5);
								state->followY = v21;

								int32_t v22 = state->smoothedY - frameDelta * (state->smoothedY - v21) / 16;
								state->smoothedY = v22;

								if (g_unk50A4E0)
									state->smoothedY = buzzY - (256 - g_unk50A4E0) * ((buzzY - v22 - 4096) / 256) - 4096;

								if (g_unk50A118.x != 0x80000000)
								{
									int32_t v24;

									v24 = (state->orbitYaw
										   - Nu3D::Math::CartesianToFixedAngle(
											   g_unk50A118.x - g_buzzActor.posAngles.pos.x, g_unk50A118.z - g_buzzActor.posAngles.pos.z))
										& 0xFFF;

									if (v24 >= 2048)
										v24 -= 4096;

									state->orbitYaw = state->orbitYaw - (v24 >> 3);
									g_unk50A118.x = 0x80000000;
								}

								int32_t v25 = state->orbitPitch;
								int32_t v26 = state->orbitYaw;

								int32_t v27 = g_buzzCameraDistance * Numerics::g_sinCosLUT[(v25 - 2048) & 0xFFF];
								int32_t v28 = (g_buzzCameraDistance * Numerics::g_sinCosLUT[(v25 + 1024) & 0xFFF]) >> 14;

								int32_t v29 = v28 * Numerics::g_sinCosLUT[(v26 + 1024) & 0xFFF];
								int32_t v30 = v28 * Numerics::g_sinCosLUT[v26 & 0xFFF];

								cameraPos[0] = buzzX;
								cameraPos[1] = buzzY - 7936;
								v27 >>= 9;

								int32_t v31 = -(v29 >> 9);
								int32_t v32 = -(v30 >> 9);
								int32_t v100 = v27 + state->smoothedY - buzzY;

								cameraPos[2] = buzzZ;
								PrepareCameraCollisionCandidates(cameraPos, v32, v100, v31, 2000);

								startPos.x = cameraPos[0];
								startPos.z = cameraPos[2];
								startPos.y = cameraPos[1];
								displacement.x = v32;
								displacement.y = v27 + state->smoothedY - buzzY;
								displacement.z = v31;

								if (g_unk50A534 >= 66)
									v34 = 2840;
								else
									v34 = 80 * (g_unk50A534 / 2) + 200;

								if (Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 1, v34))
								{
									int32_t v35;

									v35 = (int32_t)sqrt((
										double)((displacement.y >> 5) * (displacement.y >> 5) + (displacement.x >> 5) * (displacement.x >> 5)
												+ (displacement.z >> 5) * (displacement.z >> 5)));
									state->distance = v35;

									if (v35 < 10)
										state->distance = 10;
								}

								goto LABEL_194;
							}
						}

						state->orbitPitch = 64;
						goto LABEL_72;
					}
				}
				else
				{
					state->orbitYaw = state->orbitYaw + speedMultiplier * v13;
					speedMultiplier = frameDelta;

					if (v15 + v13 * frameDelta < 0)
						goto LABEL_58;
				}

				state->orbitYaw = g_buzzActor.posAngles.angles.yaw;

				speedMultiplier = frameDelta;
				goto LABEL_58;
			}

			// Buzz moves, the camera follows him
			g_unk50A534 = 0;
			moving = 1;

			if (g_buzzActor.posAngles.pos.y > state->followY + 0x4000)
			{
				state->followY = g_buzzActor.posAngles.pos.y - 0x4000;
				speedMultiplier = frameDelta;
			}

			if (g_buzzActor.posAngles.pos.y < state->followY - 0x4000)
			{
				state->followY = g_buzzActor.posAngles.pos.y + 0x4000;
				speedMultiplier = frameDelta;
			}

			if ((g_buzzActor.unkShort40 && g_buzzActor.unkVar30) || g_buzzActor.posAngles.pos.y < state->followY + 4096)
			{
				state->followY -= (state->followY - g_buzzActor.posAngles.pos.y + 4096) >> 5;
				speedMultiplier = frameDelta;
			}

			int32_t v36;
			v36 = state->smoothedY - speedMultiplier * (state->smoothedY - state->followY) / 16;
			state->smoothedY = v36;

			if (g_unk50A4E0)
				state->smoothedY = g_buzzActor.posAngles.pos.y - (256 - g_unk50A4E0) * ((g_buzzActor.posAngles.pos.y - v36 - 4096) / 256) - 4096;

			if (g_gravityBootsDuration && state->smoothedY > g_buzzActor.posAngles.pos.y - 4096)
				state->smoothedY = g_buzzActor.posAngles.pos.y - 4096;

			if ((SaveManager::g_save0Data.cameraType & 64) != 0)
			{
				int32_t v38;

				v38 = 42;
				int32_t v39;
				v39 = (state->orbitYaw - g_buzzActor.posAngles.angles.yaw) & 0xFFF;

				if (v39 >= 0x800)
					v39 -= 4096;

				if (abs(v39) > 1536)
				{
					int32_t v40;

					v40 = 2048;

					if (v39 <= 0)
						v40 = -2048;

					v39 = 3 * (v40 - v39);
				}

				if (Buzz::g_buzzSlipTimer)
					v38 = 96;

				state->orbitYaw -= v39 * frameDelta / (2 * v38);
			}
			else
			{
				int32_t v37;

				v37 = 0;

				if ((InputManager::g_directionInputState & 0x100) != 0 && (g_unk50A12C & 9) == 0)
					v37 = 32 * frameDelta / 2;

				if ((InputManager::g_directionInputState & 0x200) != 0 && (g_unk50A12C & 0xA) == 0)
					v37 = -32 * frameDelta / 2;

				state->orbitYaw += v37;

				if ((InputManager::g_curButtonsPressed & 0x1000) != 0 && (InputManager::g_prevButtonsPressed & 0x1000) == 0)
					state->orbitYaw = g_buzzActor.posAngles.angles.yaw;
			}

			if (Buzz::g_unk53C618 || g_rocketBootsDuration || Buzz::g_unk53C818 || g_grappleState == 2 || (g_buzzActor.actorFlags & 0x40) != 0)
			{
				int32_t v41;

				v41 = (state->orbitYaw - g_buzzActor.posAngles.angles.yaw) & 0xFFF;

				if (v41 >= 0x800)
					v41 -= 4096;

				if (abs(v41) > 512)
					v41 = v41 >= 0 ? 512 : -512;

				state->orbitYaw -= v41 * frameDelta / 32;
			}

			int32_t v42;
			v42 = state->orbitPitch;
			state->orbitYaw &= 0xFFF;
			stateDistance = state->distance;
			int32_t v43;
			v43 = state->orbitYaw;

			int32_t probeFlags;
			probeFlags = 0;

			int32_t v44;
			v44 = stateDistance * Numerics::g_sinCosLUT[(v42 - 2048) & 0xFFF];
			int32_t v45;
			v45 = (stateDistance * Numerics::g_sinCosLUT[(v42 + 1024) & 0xFFF]) >> 14;
			int32_t v46;
			v46 = v45 * Numerics::g_sinCosLUT[v43 & 0xFFF];
			int32_t v47;
			v47 = v45 * Numerics::g_sinCosLUT[(v43 + 1024) & 0xFFF];

			cameraPos[0] = buzzX;
			cameraPos[1] = buzzY - 4096;
			int32_t v48;
			v48 = v44 >> 9;
			int32_t v49;
			v49 = -(v47 >> 9);
			int32_t v50;
			v50 = -(v46 >> 9);
			int32_t v102;
			v102 = v48 + state->smoothedY - buzzY;
			cameraPos[2] = buzzZ;

			PrepareCameraCollisionCandidates(cameraPos, v50, v102, v49, 2000);

			int32_t v51;
			v51 = Numerics::g_sinCosLUT[(v43 - 2048) & 0xFFF];
			int32_t v103;
			v103 = Numerics::g_sinCosLUT[(v43 + 1024) & 0xFFF] >> 3;
			startPos.x = cameraPos[0] - v103;
			startPos.y = cameraPos[1] - 3840;
			int32_t v104;
			v104 = v51 >> 3;

			startPos.z = cameraPos[2] - (v51 >> 3);
			displacement.x = v50 - v103;
			displacement.y = state->smoothedY - buzzY + v48 - 2048;
			displacement.z = v49 - (v51 >> 3);

			if (Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 1, 200))
			{
				probeFlags = 1;

				if (Collision::g_floorSlope.y > 12000)
					probeFlags = 9;

				startPos.x += displacement.x;
				startPos.z += displacement.z;
			}

			probePos.x = v103 + cameraPos[0];
			probePos.y = cameraPos[1] - 3840;

			displacement.x = v50 + v103;
			probePos.z = v104 + cameraPos[2];
			int32_t v53;
			v53 = state->smoothedY - buzzY;
			displacement.z = v49 + v104;
			displacement.y = v53 + v48 - 2048;

			if (Nu3D::Collision::SweepAndSlide(&probePos, &displacement, 0x8000, 1, 200))
			{
				probeFlags |= 2;

				if (Collision::g_floorSlope.y > 12000)
					probeFlags |= 8;

				probePos.x += displacement.x;
				probePos.z += displacement.z;
			}

			startPos.x = cameraPos[0];
			startPos.y = cameraPos[1] + 3072;
			startPos.z = cameraPos[2];
			displacement.x = v50;
			displacement.y = v48 + state->smoothedY - buzzY;
			displacement.z = v49;

			int32_t v58;
			v58 = probeFlags;

			if (Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 1, 200))
				v58 = (v58 & ~0xFF) | (probeFlags | 4);

			g_unk50A12C = v58;

			if ((v58 & 8) != 0)
				v58 = (v58 & ~0xFF) | 3;

			switch (v58 & 7)
			{
				case 0:
					state->collisionFlags = 0;
					break;

				case 1:
					goto LABEL_153;

				case 2:
					goto LABEL_140;

				case 3:
					goto LABEL_155;

				case 4:
					goto LABEL_156;

				case 5:
					if ((state->collisionFlags & 1) == 0)
						state->collisionFlags = 8;

					goto LABEL_143;

				case 6:
				LABEL_143:
					if ((state->collisionFlags & 2) == 0)
						state->collisionFlags = 8;

					goto LABEL_145;

				case 7:
				LABEL_145:
					int32_t v59;
					v59 = (state->orbitYaw - g_buzzActor.posAngles.angles.yaw) & 0xFFF;

					if (v59 >= 0x800)
						v59 -= 4096;

					int32_t v60;
					v60 = abs(v59);

					if (v60 <= 1024)
					{
						if (v60 <= 512)
						{
							if (g_buzzActor.gravityVel >= -128)
							LABEL_156:
								state->collisionFlags = 8;
							else
							LABEL_155:
								state->collisionFlags = 4;
						}
						else if (v59 <= 0)
						{
						LABEL_140:
							state->collisionFlags = 2;
						}
						else
						{
						LABEL_153:
							state->collisionFlags = 1;
						}
					}
					else if (v59 <= 0)
					{
						state->collisionFlags = 10;
					}
					else
					{
						state->collisionFlags = 9;
					}

					break;

				default:
					break;
			}

			if ((state->collisionFlags & 3) != 0)
			{
				int32_t v61;

				v61 = g_unk50A4B4;

				if ((state->collisionFlags & 3) == 1)
				{
					if (g_unk50A4B4 > 0)
						v61 = 0;

					v61 -= 8 * frameDelta;
					g_unk50A4B4 = v61;

					if (v61 <= -96)
					{
						v61 = -96;
						g_unk50A4B4 = -96;
					}
				}

				if ((state->collisionFlags & 3) == 2)
				{
					if (v61 < 0)
						v61 = 0;

					v61 += 8 * frameDelta;
					g_unk50A4B4 = v61;

					if (v61 >= 96)
					{
						v61 = 96;
						g_unk50A4B4 = 96;
					}
				}

				state->orbitYaw = (state->orbitYaw + frameDelta * v61 / 2) & 0xFFF;
				goto LABEL_169;
			}

			if (g_unk50A4B4 <= 0)
			{
				if (g_unk50A4B4 >= 0)
				{
				LABEL_169:
					v62 = frameDelta;
					goto LABEL_170;
				}

				v62 = frameDelta;
				g_unk50A4B4 += 16 * frameDelta;

				if (g_unk50A4B4 > 0)
					g_unk50A4B4 = 0;
			}
			else
			{
				v62 = frameDelta;
				g_unk50A4B4 += -16 * frameDelta;

				if (g_unk50A4B4 < 0)
					g_unk50A4B4 = 0;
			}

		LABEL_170:
			int32_t collisionFlags;
			collisionFlags = state->collisionFlags;

			if ((collisionFlags & 4) != 0)
			{
				state->orbitPitch += -8 * v62;

				if (state->orbitPitch < -512)
					state->orbitPitch = -512;

				v62 = frameDelta;
			}

			if ((collisionFlags & 8) != 0)
			{
				state->orbitPitch += 8 * v62;

				if (state->orbitPitch > 768)
					state->orbitPitch = 768;

				v62 = frameDelta;
			}

			if ((collisionFlags & 0xC) != 0)
				goto LABEL_190;

			int32_t v64;
			v64 = state->orbitPitch;

			if (v64 <= 64)
			{
				int32_t v65;

				v65 = v64 + 8 * v62;
				state->orbitPitch = v65;

				if (v65 < 64)
					goto LABEL_190;
			}
			else
			{
				int32_t v66;

				v66 = v64 - 8 * v62;
				state->orbitPitch = v66;

				if (v66 > 64)
					goto LABEL_190;
			}

			state->orbitPitch = 64;

		LABEL_190:
			if (g_unk50A118.x != 0x80000000)
			{
				int32_t v67;

				v67 = state->orbitYaw;
				int32_t v68;
				v68 =
					(v67 - Nu3D::Math::CartesianToFixedAngle(g_unk50A118.x - g_buzzActor.posAngles.pos.x, g_unk50A118.z - g_buzzActor.posAngles.pos.z)) & 0xFFF;

				if (v68 >= 2048)
					v68 -= 4096;

				state->orbitYaw = v67 - (v68 >> 3);
				g_unk50A118.x = 0x80000000;
			}

		LABEL_194:
			int32_t v69 = state->orbitPitch;
			int32_t v70 = state->distance;
			int32_t v71 = (v70 * Numerics::g_sinCosLUT[(v69 + 1024) & 0xFFF]) >> 14;
			int32_t v72 = v70 * Numerics::g_sinCosLUT[(v69 - 2048) & 0xFFF];
			int32_t v73 = v71 * Numerics::g_sinCosLUT[(state->orbitYaw + 1024) & 0xFFF];
			displacement.x = (v71 * Numerics::g_sinCosLUT[state->orbitYaw & 0xFFF]) >> 9;

			int32_t v74 = state->smoothedY;
			v72 >>= 9;
			displacement.y = v72;
			state->targetPos.x = buzzX - displacement.x;
			state->targetPos.y = v74 + v72 - 0x2000;
			displacement.z = v73 >> 9;
			state->targetPos.z = buzzZ - (v73 >> 9);

			if (! g_unk50A4E8)
			{
				int32_t v75 = g_unk52AD98;

				if (g_unk52AD98 > 16)
				{
					g_unk52AD98 -= 2 * frameDelta;
					v75 = g_unk52AD98;

					if (g_unk52AD98 < 16)
					{
						v75 = 16;
						g_unk52AD98 = 16;
					}
				}

				if (snapToTarget)
				{
					state->pos.x = state->targetPos.x;
					state->pos.y = state->targetPos.y;
					state->pos.z = state->targetPos.z;
				}
				else
				{
					int32_t v80 = state->targetPos.y;
					state->pos.x -= frameDelta * (state->pos.x - state->targetPos.x) / (2 * v75 / 4);
					int32_t v81 = frameDelta * (state->pos.y - v80);

					int32_t v82 = state->targetPos.z;
					state->pos.y -= v81 / (2 * g_unk52AD98 / 4);
					state->pos.z -= frameDelta * (state->pos.z - v82) / (2 * g_unk52AD98 / 4);
				}
			}

			int32_t v83 = state->distance;

			if (v83 >= 400)
				v84 = 0;
			else
				v84 = 2 * (400 - v83);

			int32_t v85 = Nu3D::Math::CartesianToFixedAngle((buzzX - state->pos.x) >> 5, (buzzZ - state->pos.z) >> 5);
			int32_t v86 = (g_buzzCameraDistance - v84) * Numerics::g_sinCosLUT[(v85 + 1024) & 0xFFF];
			int32_t v87 = ((g_buzzCameraDistance - v84) * Numerics::g_sinCosLUT[v85 & 0xFFF]) >> 14;

			int32_t v88 = v86 >> 14;
			int32_t v89 = (buzzY - state->pos.y - 6144) >> 5;
			int32_t v90 = v89;

			if (v89 >= 0)
				v90 = -v89;

			int32_t v91 = 0;

			if (g_unk50A510 > 0)
			{
				v91 = Numerics::g_sinCosLUT[(-384 * g_unk50A510) & 0xFFF] / (16 * (g_unk50A510 - 50));
				g_unk50A510 -= frameDelta;

				if (g_unk50A510 < 0)
					g_unk50A510 = 0;
			}

			int32_t v92 = (v91 - state->angles.pitch - Nu3D::Math::CartesianToFixedAngle(v89 * v90, v87 * v87 + v88 * v88)) & 0xFFF;

			if (v92 >= 2048)
				v92 -= 4096;

			if (Buzz::g_unk53C660)
			{
				if (v92 <= 32)
				{
					if (v92 < -32)
						v92 = -32;
				}
				else
				{
					v92 = 32;
				}
			}

			state->angles.pitch = (state->angles.pitch + v92 * frameDelta / (g_unk52AD98 / 2)) & 0xFFF;
			int32_t v93 = Nu3D::Math::CartesianToFixedAngle(v87, v88);

			if (! snapToTarget)
			{
				v94 = (v93 - state->angles.yaw) & 0xFFF;

				if (v94 >= 2048)
					v94 -= 4096;

				v93 = state->angles.yaw + v94 * frameDelta / (g_unk52AD98 / 2);
			}

			state->angles.yaw = v93 & 0xFFF;

			if (moving)
			{
				v91 = buzzX;
				v92 = buzzY;
				v94 = buzzZ;

				cameraPos[0] = state->pos.x;
				cameraPos[1] = state->pos.y;
				cameraPos[2] = state->pos.z;
				displacement.x = buzzX - state->pos.x;
				displacement.y = buzzY - state->pos.y - 6144;
				displacement.z = buzzZ - cameraPos[2];

				if (Nu3D::Collision::SweepQuery(cameraPos, &displacement.x, 1, 1) == 1)
				{
					int32_t v97;

					v97 = (int32_t)sqrt((
						double)((displacement.y >> 5) * (displacement.y >> 5) + (displacement.x >> 5) * (displacement.x >> 5)
								+ (displacement.z >> 5) * (displacement.z >> 5)));
					state->distance -= v97;

					if (state->distance < 10)
						state->distance = 10;

					if (v97 < 300)
					{
						state->pos.x += displacement.x;
						state->pos.y = displacement.y + state->pos.y;
						state->pos.z = displacement.z + state->pos.z;
					}

					v92 = buzzY;
				}
			}
			else
			{
				v94 = buzzZ;
				v92 = buzzY;
				v91 = buzzX;
			}

			v102 = (v92 - state->pos.y - 4096) >> 5;
			v103 = ((v91 - state->pos.x) >> 5) * ((v91 - state->pos.x) >> 5);
			v104 = ((v94 - state->pos.z) >> 5) * ((v94 - state->pos.z) >> 5);

			if (v104 + v103 + v102 * v102 < 160000)
				state->pos.y = v92 - 32 * (int32_t)sqrt((double)(160000 - v103 - v104)) - 4096;
		}

		// FUNCTION: TOY2 0x00405860
		void UpdateActiveTransform()
		{
			switch (g_cameraFadeState)
			{
				case 1:
					if (! Nu3D::Camera::g_cameraTintBlue)
					{
						Nu3D::Camera::SetTint(128, 128, 128, 16);
						g_inCutscene |= 1;
						g_cameraFadeState = 0;
					}

					break;

				case 2:
					Nu3D::Camera::SetTint(0, 0, 0, 16);
					g_cameraFadeState = 1;

					if (g_inCutscene & 2)
						g_buzzActor.actorFlags |= Actor::ACTOR_LOCKED_MOVE;

					break;

				case 3:
					if (! Nu3D::Camera::g_cameraTintBlue)
					{
						Nu3D::Camera::SetTint(128, 128, 128, 16);
						g_inCutscene &= ~1;
						g_cameraFadeState = 0;

						if (g_inCutscene & 2)
							g_buzzActor.actorFlags &= ~Actor::ACTOR_LOCKED_MOVE;
					}

					break;

				case 4:
					Nu3D::Camera::SetTint(0, 0, 0, 16);
					g_cameraFadeState = 3;
					break;

				default:
					break;
			}

			// the scripted and the gameplay camera are blended into the active transform the same way
			int32_t blendTimer;
			int32_t pitchDelta;
			int32_t yawDelta;
			int32_t rollDelta;
			int16_t roll;
			int16_t yaw;
			uint16_t pitch;

			if (g_inCutscene & 1)
			{
				if (g_cutsceneFrameTimer < 24)
				{
					g_cutsceneFrameTimer += Renderer::g_frameDelta;

					if (g_cutsceneFrameTimer >= 24)
						g_cutsceneFrameTimer = 24;
				}

				GameplayMode(&g_gameplayCameraState);
				SmoothToTarget(&g_scriptedCameraState);

				blendTimer = g_cameraBlendTimer;

				if (g_cameraBlendTimer > 0)
				{
					g_activeCameraTransform.pos.x =
						g_scriptedCameraState.pos.x + ((g_cameraBlendTimer * (g_activeCameraTransform.pos.x - g_scriptedCameraState.pos.x)) >> 6);
					g_activeCameraTransform.pos.y =
						g_scriptedCameraState.pos.y + ((g_cameraBlendTimer * (g_activeCameraTransform.pos.y - g_scriptedCameraState.pos.y)) >> 6);
					g_activeCameraTransform.pos.z =
						g_scriptedCameraState.pos.z + ((g_cameraBlendTimer * (g_activeCameraTransform.pos.z - g_scriptedCameraState.pos.z)) >> 6);

					pitchDelta = (g_activeCameraTransform.angles.pitch - g_scriptedCameraState.angles.pitch) & 0xFFF;

					if (pitchDelta >= 0x800)
						pitchDelta -= 4096;

					yawDelta = (g_activeCameraTransform.angles.yaw - g_scriptedCameraState.angles.yaw) & 0xFFF;

					if (yawDelta >= 0x800)
						yawDelta -= 4096;

					roll = g_scriptedCameraRoll;
					rollDelta = (g_activeCameraTransform.roll - g_scriptedCameraRoll) & 0xFFF;

					if (rollDelta >= 0x800)
						rollDelta -= 4096;

					pitch = (g_scriptedCameraState.angles.pitch + ((pitchDelta * g_cameraBlendTimer) >> 6)) & 0xFFF;
					yaw = g_scriptedCameraState.angles.yaw + ((yawDelta * g_cameraBlendTimer) >> 6);

					g_activeCameraTransform.angles.pitch = pitch;
					goto LABEL_25;
				}

				g_activeCameraTransform.pos.x = g_scriptedCameraState.pos.x;
				g_activeCameraTransform.pos.y = g_scriptedCameraState.pos.y;
				g_activeCameraTransform.pos.z = g_scriptedCameraState.pos.z;
				g_activeCameraTransform.angles = g_scriptedCameraState.angles;
				g_activeCameraTransform.roll = g_scriptedCameraRoll;
			}
			else
			{
				if (g_cutsceneFrameTimer > 0)
				{
					g_cutsceneFrameTimer -= Renderer::g_frameDelta;

					if (g_cutsceneFrameTimer <= 0)
						g_cutsceneFrameTimer = 0;
				}

				if (g_cameraState)
					VisorMode(&g_gameplayCameraState);
				else
					GameplayMode(&g_gameplayCameraState);

				blendTimer = g_cameraBlendTimer;

				if (g_cameraBlendTimer > 0)
				{
					g_activeCameraTransform.pos.x =
						g_gameplayCameraState.pos.x + ((g_cameraBlendTimer * (g_activeCameraTransform.pos.x - g_gameplayCameraState.pos.x)) >> 6);
					g_activeCameraTransform.pos.y =
						g_gameplayCameraState.pos.y + ((g_cameraBlendTimer * (g_activeCameraTransform.pos.y - g_gameplayCameraState.pos.y)) >> 6);
					g_activeCameraTransform.pos.z =
						g_gameplayCameraState.pos.z + ((g_cameraBlendTimer * (g_activeCameraTransform.pos.z - g_gameplayCameraState.pos.z)) >> 6);

					pitchDelta = (g_activeCameraTransform.angles.pitch - g_gameplayCameraState.angles.pitch) & 0xFFF;

					if (pitchDelta >= 0x800)
						pitchDelta -= 4096;

					yawDelta = (g_activeCameraTransform.angles.yaw - g_gameplayCameraState.angles.yaw) & 0xFFF;

					if (yawDelta >= 0x800)
						yawDelta -= 4096;

					roll = g_gameplayCameraState.roll;
					rollDelta = (g_activeCameraTransform.roll - g_gameplayCameraState.roll) & 0xFFF;

					if (rollDelta >= 0x800)
						rollDelta -= 4096;

					pitch = (g_gameplayCameraState.angles.pitch + ((pitchDelta * g_cameraBlendTimer) >> 6)) & 0xFFF;
					yaw = g_gameplayCameraState.angles.yaw + ((yawDelta * g_cameraBlendTimer) >> 6);

					g_activeCameraTransform.angles.pitch = pitch;

				LABEL_25:
					g_activeCameraTransform.angles.yaw = yaw & 0xFFF;
					g_activeCameraTransform.roll = (roll + ((rollDelta * blendTimer) >> 6)) & 0xFFF;
					g_cameraBlendTimer = blendTimer - Renderer::g_frameDelta;

					return;
				}

				g_activeCameraTransform.pos.x = g_gameplayCameraState.pos.x;
				g_activeCameraTransform.pos.y = g_gameplayCameraState.pos.y;
				g_activeCameraTransform.pos.z = g_gameplayCameraState.pos.z;
				g_activeCameraTransform.angles = g_gameplayCameraState.angles;
				g_activeCameraTransform.roll = g_gameplayCameraState.roll;
			}
		}

		// FUNCTION: TOY2 0x00435F30
		int32_t HandleLedgeClimb(Toy2::Buzz::Toy2BuzzActor* buzzActor)
		{
			int32_t y;
			int32_t v4;
			int32_t v5;
			int32_t v7;
			WorldPos displacement;
			WorldPos startPos;
			WorldPos probePos;

			if (buzzActor->gravityVel > 0 && (Buzz::g_buzzActionState & 0xFFF7F) == 0)
			{
				y = buzzActor->posAngles.pos.y;

				if (buzzActor->floorYPos - y > 0x2000 && buzzActor->floorYPos != (int32_t)0x80000000)
				{
					probePos.y = y - 13824;
					v4 = Numerics::g_sinCosLUT[buzzActor->posAngles.angles.yaw & 0xFFF] / 3;
					v5 = Numerics::g_sinCosLUT[(buzzActor->posAngles.angles.yaw + 1024) & 0xFFF] / 3;
					probePos.x = v4 + buzzActor->posAngles.pos.x;
					probePos.z = v5 + buzzActor->posAngles.pos.z;

					v7 = Nu3D::Collision::GetGroundHeight(&probePos, 0) - 200;

					if (Collision::g_floorSlope.y < -15000)
					{
						if (v7 < buzzActor->posAngles.pos.y - 13824 && v7 >= buzzActor->motionTargetPos.y - 13824)
						{
							startPos.y = buzzActor->posAngles.pos.y;
							startPos.x = buzzActor->posAngles.pos.x - v4 / 4;
							startPos.z = buzzActor->posAngles.pos.z - v5 / 4;

							displacement.x = 0;
							displacement.z = 0;
							displacement.y = v7 - buzzActor->posAngles.pos.y;

							if (! Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 0, 4000))
							{
								startPos.y = v7 - 6000;
								displacement.x = v4 + v4 / 3;
								displacement.y = 0;
								displacement.z = v5 + v5 / 3;

								if (! Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 0, 4000))
								{
									AudioManager::PlaySoundEffect(23, &buzzActor->posAngles.pos);
									Toy2::Portal::UpdateActiveSectorAt(buzzActor->posAngles.pos.x, v7, buzzActor->posAngles.pos.z);

									buzzActor->posAngles.pos.x += v4;
									buzzActor->posAngles.pos.y = v7;
									buzzActor->posAngles.pos.z += v5;

									Buzz::g_unk53C660 = 82;
									Platform::g_unk559E80 = Collision::GetGroundContactIdx();

									startPos.y = v7;

									if (Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 0, 4000))
										buzzActor->facingAngle =
											(Nu3D::Math::CartesianToFixedAngle(Collision::g_floorSlope.x, Collision::g_floorSlope.z) - 2048) & 0xFFF;
								}
							}
						}
					}
				}
			}

			if (Buzz::g_unk53C660)
			{
				int32_t turn = (buzzActor->posAngles.angles.yaw - buzzActor->facingAngle) & 0xFFF;

				if (turn > 0x800)
					turn -= 4096;

				buzzActor->posAngles.angles.yaw = (buzzActor->posAngles.angles.yaw - (turn * (int32_t)Renderer::g_frameDelta) / 16) & 0xFFF;
				g_gameplayCameraState.orbitYaw = (g_gameplayCameraState.orbitYaw - (turn * (int32_t)Renderer::g_frameDelta) / 16) & 0xFFF;

				Buzz::g_unk53C660 -= Renderer::g_frameDelta;

				if (Buzz::g_unk53C660 < 0)
					Buzz::g_unk53C660 = 0;

				return 7;
			}

			Platform::g_unk559E80 = -1;

			return 0;
		}

		// FUNCTION: TOY2 0x00447BD0
		void CullActors(Nu3D::Camera::ActiveCameraTransform* cam)
		{
			if (Actor::g_activeActors[0])
			{
				Actor::Toy2Actor** activeActors = Actor::g_activeActors;

				do
				{
					Actor::Toy2Actor* curActor = *activeActors;
					int32_t visibilityDistance = curActor->visibilityDistance;
					int16_t actorFlags = curActor->actorFlags;
					int32_t creatureId = curActor->creatureId;
					int32_t visibilitySquare = 16 * visibilityDistance * visibilityDistance;

					curActor->actorFlags = actorFlags & 0xDFFE;

					int32_t flagAdd = (actorFlags & Actor::ACTOR_CUTSCENE_MOVE) != 0 ? 250000 : 0;

					if (*(int16_t*)CharacterLoader::g_unk547CD4[creatureId])
					{
						int32_t areaIndex = (int8_t)curActor->areaIndex;

						if (areaIndex >= 0 && Nu3D::Portal::IsAreaVisible(areaIndex))
						{
							int32_t zDiff = cam->pos.z - curActor->pos.z;
							int32_t yDiff = cam->pos.y - curActor->pos.y;
							int32_t xDiff = cam->pos.x - curActor->pos.x;

							if ((xDiff >> 5) * (xDiff >> 5) + (yDiff >> 5) * (yDiff >> 5) + (zDiff >> 5) * (zDiff >> 5) < visibilitySquare + flagAdd)
							{
								Vector3F point;

								point.x = (float)(curActor->pos.x / 32);
								point.y = (float)(curActor->pos.y / 32);
								point.z = (float)(curActor->pos.z / 32);
								float distance = (float)curActor->boundingSphereRadius;

								if ((Nu3D::Frustum::TestSphereAllPlanes(&point, distance) & 0x55555555) == 0)
									curActor->actorFlags |= Actor::ACTOR_CUTSCENE_MOVE;
							}
						}
						else
						{
							curActor->actorFlags |= Actor::ACTOR_STUNNED << 8;
						}
					}

					++activeActors;
				} while (*activeActors);
			}
		}

		// FUNCTION: TOY2 0x004A4BB0
		void UpdateGravityBoots(Toy2::Buzz::Toy2BuzzActor* buzzActor)
		{
			int32_t effectOffset;

			Buzz::g_unk53C838 = 0;
			buzzActor->collisionFlags = 0;

			if (buzzActor->posAngles.pos.y >= buzzActor->floorYPos - g_gravityBootsHoverHeight)
			{
				buzzActor->gravityVel = -32 * Renderer::g_frameDelta + buzzActor->gravityVel;

				if (buzzActor->gravityVel < -384)
					buzzActor->gravityVel = -384;
			}
			else
			{
				buzzActor->gravityVel = 32 * Renderer::g_frameDelta + buzzActor->gravityVel;

				if (buzzActor->gravityVel > 0x180)
					buzzActor->gravityVel = 384;
			}

			if ((InputManager::g_directionInputState & DIR_CAM_FLAG2) != 0)
			{
				g_gravityBootsHoverHeight += Renderer::g_frameDelta << 10;

				if (g_gravityBootsHoverHeight >= 49152)
					g_gravityBootsHoverHeight = 49152;

				effectOffset = 1;
			}
			else
			{
				effectOffset = 0;
				g_gravityBootsHoverHeight += -1024 * Renderer::g_frameDelta;

				if (g_gravityBootsHoverHeight <= 0x2000)
					g_gravityBootsHoverHeight = 0x2000;
			}

			// g_tickState[4] and g_tickState[8] are the 4 and 8 tick pulses
			if (g_tickState[4])
			{
				if (g_gravityBootsDuration >= 120)
				{
					if (g_tickState[8] && effectOffset)
						effectOffset = 0;
				}
				else
				{
					effectOffset = 2;

					if (g_tickState[8])
						goto LABEL_20;
				}

				int32_t effectType = effectOffset + 81;

				Nu3D::Particles::SpawnInstance(
					buzzActor->posAngles.pos.x,
					buzzActor->posAngles.pos.y,
					buzzActor->posAngles.pos.z,
					0,
					0,
					0,
					0,
					g_tickAccumulators.tick32Acc << 7,
					128,
					effectOffset + 81);

				Nu3D::Particles::SpawnInstance(
					buzzActor->posAngles.pos.x,
					buzzActor->posAngles.pos.y,
					buzzActor->posAngles.pos.z,
					0,
					0,
					0,
					0,
					(-128 * g_tickAccumulators.tick32Acc) & 0xFFF,
					-128,
					effectType);
			}

		LABEL_20:
			AudioManager::PlaySoundEffect(80, &g_buzzActor.posAngles.pos);
			g_gravityBootsDuration -= Renderer::g_frameDelta;

			if (g_gravityBootsDuration <= 0)
				g_gravityBootsDuration = 0;
		}

		// FUNCTION: TOY2 0x004A4F80
		BOOL UpdateRocketBoots(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movement)
		{
			BOOL result;

			movement->maxSpeed = 2048;
			movement->maxAirSpeed = 2048;
			movement->turnSpeed = 640 * Renderer::g_frameDelta / 4;
			movement->groundAccel = (Renderer::g_frameDelta << 9) / 4;

			int32_t airAccel = (Renderer::g_frameDelta << 7) / 4;
			movement->airAccel = airAccel;

			// decompiler artifact: only the low word of the register is overwritten, the argument of CalculateMaxTurnAngle keeps the upper half
			airAccel = (airAccel & 0xFFFF0000) | (uint16_t)InputManager::g_directionInputState;

			if ((InputManager::g_directionInputState & (DIR_UP | DIR_RIGHT | DIR_DOWN | DIR_LEFT)) != 0)
				CalculateMaxTurnAngle(airAccel);

			uint16_t yawAngle = buzzActor->posAngles.angles.yaw;
			uint32_t angularDiff = (yawAngle - buzzActor->facingAngle) & 0xFFF;

			if (angularDiff < 2048)
			{
				if (angularDiff > movement->maxTurnRate)
					angularDiff = movement->maxTurnRate;

				result = 1;
				buzzActor->posAngles.angles.yaw = yawAngle - (angularDiff * Renderer::g_frameDelta) / 8;
				buzzActor->posAngles.angles.yaw &= 0xFFF;
			}
			else
			{
				int32_t yawAdd = 4096 - angularDiff;

				if ((4096 - angularDiff) > movement->maxTurnRate)
					yawAdd = movement->maxTurnRate;

				buzzActor->posAngles.angles.yaw = yawAngle + yawAdd * Renderer::g_frameDelta / 8;
				buzzActor->posAngles.angles.yaw &= 0xFFF;

				return 1;
			}

			return result;
		}

		// FUNCTION: TOY2 0x00433F40
		int32_t CalculateMaxTurnAngle(int32_t directionButtonState)
		{
			int32_t cameraSpeed = 0;
			int32_t analogX;
			int32_t analogY;
			int16_t analogStickAngle;
			int16_t cameraAngleFromPos;

			cameraAngleFromPos = Nu3D::Math::CartesianToFixedAngle(
				(g_buzzActor.posAngles.pos.x - g_gameplayCameraState.targetPos.x) >> 5, (g_buzzActor.posAngles.pos.z - g_gameplayCameraState.targetPos.z) >> 5);

			if ((InputManager::g_directionalInputCount == 1 || InputManager::g_directionalInputCount == 2) && ! g_demoMode)
			{
				analogX = InputManager::g_analogInputX;

				if (abs(InputManager::g_analogInputX) < 6144)
				{
					analogX = 0;
					InputManager::g_analogInputX = 0;
				}

				analogY = InputManager::g_analogInputY;

				if (abs(InputManager::g_analogInputY) < 6144)
				{
					analogY = 0;
					InputManager::g_analogInputY = 0;
				}

				analogStickAngle = Nu3D::Math::CartesianToFixedAngle(analogX, analogY);

				g_buzzActor.facingAngle =
					(cameraAngleFromPos + g_cameraAngleOffsetTable[(analogStickAngle & 0xFFF) >> 9]
					 + (((analogStickAngle & 0x1FF)
						 * (g_cameraAngleOffsetTable[((analogStickAngle & 0xFFF) >> 9) + 1] - g_cameraAngleOffsetTable[(analogStickAngle & 0xFFF) >> 9]))
						>> 9))
					& 0xFFF;

				cameraSpeed = (int32_t)sqrt((
					double)(InputManager::g_analogInputY / 2 * (InputManager::g_analogInputY / 2)
							+ InputManager::g_analogInputX / 2 * (InputManager::g_analogInputX / 2)));

				if (cameraSpeed > 16384)
					cameraSpeed = 16384;
			}

			if (InputManager::g_directionalInputCount && ! g_demoMode)
				return cameraSpeed;

			g_buzzActor.facingAngle = (cameraAngleFromPos + g_autoRotationSequence[(directionButtonState >> 4) & 0xF]) & 0xFFF;

			return 16384;
		}

		// STUB: TOY2 0x004037E0
		BOOL GetActorLockOnPoint(Vector3I* pos, Toy2::Actor::Toy2Actor* actor, int32_t flag) { return 0; }

		// FUNCTION: TOY2 0x004A5870
		int32_t SweepSegmentVsActors(WorldPos* arg0, WorldPos* a2, int32_t a3)
		{
			Toy2::Actor::Toy2Actor* actor = Actor::g_activeActors[0];
			Toy2::Actor::Toy2Actor* hitActor = 0;
			int32_t hit = 0;
			Vector3I dir;

			if (! actor)
				return hit;

			Toy2::Actor::Toy2Actor** nextActor = Actor::g_activeActors;

			do
			{
				if ((actor->creatureRam->defenseMode & 4) != 0 && actor->damageCooldownTimer >= 0 && actor->unkVar29 >= 0)
				{
					int32_t endX = a2->x + arg0->x;
					int32_t endY = a2->y + arg0->y;
					int32_t endZ = a2->z + arg0->z;

					int16_t* attach = (int16_t*)(actor->unkVar34 + 16 * actor->primaryAnimIdx);

					int32_t cosYaw = Numerics::g_sinCosLUT[(actor->yawAngle - 1024) & 0xFFF] >> 2;
					int32_t sinYaw = Numerics::g_sinCosLUT[(actor->yawAngle - 2048) & 0xFFF] >> 2;

					int32_t centerX = actor->pos.x + ((sinYaw * attach[2] + cosYaw * attach[0]) >> 12);
					int32_t centerY = actor->pos.y + attach[1];
					int32_t centerZ = actor->pos.z + ((cosYaw * attach[2] - sinYaw * attach[0]) >> 12);

					int32_t startToCenterZ = centerZ - arg0->z;
					int32_t startToCenterX = centerX - arg0->x;

					int32_t scaleX = attach[4];
					int32_t scaleY = attach[5];
					int32_t scaleZ = attach[6];

					int32_t startScaledY = (scaleY * (centerY - arg0->y)) >> 8;
					int32_t startScaledX = (scaleX * ((sinYaw * startToCenterZ + cosYaw * startToCenterX) >> 12)) >> 8;
					int32_t startScaledZ = (scaleZ * ((cosYaw * startToCenterZ - sinYaw * startToCenterX) >> 12)) >> 8;

					int32_t endToCenterX = centerX - endX;
					int32_t endScaledXTerm = scaleX * ((sinYaw * (centerZ - endZ) + cosYaw * endToCenterX) >> 12);
					int32_t endScaledZ = (scaleZ * ((cosYaw * (centerZ - endZ) - sinYaw * endToCenterX) >> 12)) >> 8;

					int32_t rayX = (startScaledX - (endScaledXTerm >> 8)) >> 5;
					int32_t rayY = (startScaledY - ((scaleY * (centerY - endY)) >> 8)) >> 5;
					int32_t rayZ = (startScaledZ - endScaledZ) >> 5;

					int32_t dirX = startScaledX >> 5;
					int32_t dirY = startScaledY >> 5;
					int32_t dirZ = startScaledZ >> 5;

					dir.x = rayX;
					dir.y = rayY;
					dir.z = rayZ;

					Nu3D::Math::NormalizeToFixedPoint(&dir, &dir);

					int32_t closest = (dirX * dir.x + dirY * dir.y + dirZ * dir.z) >> 12;

					if (closest > 0 && closest <= (rayX * dir.x + rayZ * dir.z + rayY * dir.y) >> 12)
					{
						int32_t radiusSq = attach[7] * attach[7];
						int32_t distSq = (dirX - ((dir.x * closest) >> 12)) * (dirX - ((dir.x * closest) >> 12))
							+ (dirZ - ((dir.z * closest) >> 12)) * (dirZ - ((dir.z * closest) >> 12))
							+ (dirY - ((dir.y * closest) >> 12)) * (dirY - ((dir.y * closest) >> 12));

						if (distSq < radiusSq)
						{
							int32_t travel = (int32_t)(closest - sqrt((double)(radiusSq - distSq)));
							int32_t hitX = ((dir.x * travel) >> 4) / scaleX;
							int32_t hitY = 2 * dir.y * travel / scaleY;
							int32_t hitZ = ((dir.z * travel) >> 4) / scaleZ;

							hitActor = actor;
							hit = 2;

							a2->y = hitY;
							a2->x = (cosYaw * hitX - sinYaw * hitZ) >> 7;
							a2->z = (sinYaw * hitX + cosYaw * hitZ) >> 7;
						}
					}
				}

				actor = *++nextActor;
			} while (actor);

			if (! hit || ! a3)
				return hit;

			if (a3 == 1)
				Actor::HandleDamage(hitActor, g_buzzActor.posAngles.angles.yaw, 2);
			else
				Actor::HandleDamage(hitActor, g_buzzActor.posAngles.angles.yaw, 3);

			if ((hitActor->creatureRam->defenseMode & 1) != 0)
				return hit;

			return 4;
		}
	}
}

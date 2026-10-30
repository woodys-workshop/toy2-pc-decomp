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
#include "Toy2/Levels.h"
#include "AudioManager/AudioManager.h"
#include "CharacterLoader.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>

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

		union ScriptedCameraRollStorage
		{
			struct
			{
				int16_t roll;
				uint16_t opaque;
			} words;
			uint32_t raw;
		};

		struct ScriptedCameraRollAlignment
		{
			uint8_t byte;
			ScriptedCameraRollStorage storage;
		};

		// GLOBAL: TOY2 0x0052B810
		ScriptedCameraRollStorage g_scriptedCameraRollStorage;

		STATIC_ASSERT(sizeof(ScriptedCameraRollStorage) == 4);
		STATIC_ASSERT(sizeof(g_scriptedCameraRollStorage.words.roll) == 2);
		STATIC_ASSERT(offsetof(ScriptedCameraRollStorage, words.roll) == 0);
		STATIC_ASSERT(offsetof(ScriptedCameraRollStorage, words.opaque) == 2);
		STATIC_ASSERT(offsetof(ScriptedCameraRollAlignment, storage) == 4);

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

		// the cutscene script that is running
		// GLOBAL: TOY2 0x0050A294
		int32_t* g_unk50A294;

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

		// the particles of the visor mode, -1 for none
		// GLOBAL: TOY2 0x0050A538
		Nu3D::Particles::ParticleInstance* g_unk50A538;

		// GLOBAL: TOY2 0x0050A4DC
		Nu3D::Particles::ParticleInstance* g_unk50A4DC;

		// where the scripted camera looks at
		// GLOBAL: TOY2 0x0050A500
		Vector3I g_cutsceneFocusPos;

		// where the scripted camera is
		// GLOBAL: TOY2 0x0050A520
		Vector3I g_cutsceneEyePos;

		// how long the dialogue cutscene runs
		// GLOBAL: TOY2 0x0050A4D0
		int32_t g_unk50A4D0;

		// the length of the camera segment that is travelled
		// GLOBAL: TOY2 0x0050A1F0
		int32_t g_unk50A1F0;

		// the record the cutscene script uses
		// GLOBAL: TOY2 0x0050A0C8
		int32_t g_unk50A0C8;

		// the yaw and pitch of the visor rings, they follow the aim with a delay
		// GLOBAL: TOY2 0x0050A290
		int32_t g_unk50A290;

		// GLOBAL: TOY2 0x0050A28C
		int32_t g_unk50A28C;

		// the turn speed of the visor
		// GLOBAL: TOY2 0x0050A130
		int32_t g_unk50A130;

		// GLOBAL: TOY2 0x0050A138
		int32_t g_unk50A138;

		// the phase of the spin of the visor rings
		// GLOBAL: TOY2 0x0050A4C8
		int32_t g_unk50A4C8;

		// GLOBAL: TOY2 0x0050A14C
		int32_t g_unk50A14C;

		// the actors (and the grapple points + 1000) that can be locked on to, ends with -1
		// GLOBAL: TOY2 0x0050A0D0
		int32_t g_lockOnCandidates[16];

		// GLOBAL: TOY2 0x004F5AB4
		int16_t g_cameraAngleOffsetTable[10] = { 0, 512, 1024, 1536, 2048, 2560, 3072, 3584, 4096, 0 };

		// GLOBAL: TOY2 0x004F5AC8
		int16_t g_autoRotationSequence[16] = { 0, 0, 1024, 512, 2048, 0, 1536, 0, 3072, 3584, 0, 0, 2560, 0, 0, 0 };

		// FUNCTION: TOY2 0x00402030
		void InitCutsceneCamera(Vector3I* focusPos, Vector3I* pos)
		{
			g_scriptedCameraState.pos = *pos;
			g_scriptedCameraState.focusPos = *focusPos;
			g_scriptedCameraState.eyePos = g_scriptedCameraState.pos;

			int32_t z = (g_scriptedCameraState.focusPos.z - g_scriptedCameraState.pos.z) >> 5;
			int32_t x = (g_scriptedCameraState.focusPos.x - g_scriptedCameraState.pos.x) >> 5;
			int32_t y = (g_scriptedCameraState.focusPos.y - g_scriptedCameraState.pos.y) >> 5;

			g_scriptedCameraState.angles.yaw = Nu3D::Math::CartesianToFixedAngle(x, z);

			int16_t angle;

			if (y >= 0)
				angle = Nu3D::Math::CartesianToFixedAngle(-(y * y), x * x + z * z);
			else
				angle = Nu3D::Math::CartesianToFixedAngle(y * y, x * x + z * z);

			g_scriptedCameraRollStorage.words.roll = 0;

			g_scriptedCameraState.angles.pitch = -angle;
		}

		// FUNCTION: TOY2 0x004020F0
		void BeginScriptedCutsceneAtPoint(Vector3I* target, int32_t duration, int32_t orbitRadius)
		{
			if (g_cameraState)
			{
				if (g_unk50A538 != (Nu3D::Particles::ParticleInstance*)-1)
				{
					g_unk50A538->lifetime = 1;
					g_unk50A538 = (Nu3D::Particles::ParticleInstance*)-1;
				}

				if (g_unk50A4DC != (Nu3D::Particles::ParticleInstance*)-1)
				{
					g_unk50A4DC->lifetime = 1;
					g_unk50A4DC = (Nu3D::Particles::ParticleInstance*)-1;
				}

				g_buzzActor.actorFlags |= Actor::ACTOR_CUTSCENE_MOVE;
				g_gameplayCameraState.orbitYaw = g_buzzActor.posAngles.angles.yaw;
				g_buzzActor.facingAngle = g_buzzActor.posAngles.angles.yaw;

				g_gameplayCameraState.distance = 1200;
				g_gameplayCameraState.pos.y = g_buzzActor.posAngles.pos.y - 12288;
				g_gameplayCameraState.angles.pitch = 0;
				g_gameplayCameraState.targetPos.x = g_gameplayCameraState.pos.x;
				g_gameplayCameraState.unkShort6 = 0;

				g_cameraState = 0;

				Nu3D::Link::SetScaleFromFixedOffsets(45, 0, 0, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(46, 0, 0, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(47, 0, 0, 0);
			}

			g_inCutscene |= 1;
			g_buzzActor.actorFlags |= Actor::ACTOR_LOCKED_MOVE;
			InputManager::g_directionInputState &= 0x309;
			g_cameraBlendTimer = 64;

			g_cutsceneFocusPos = *target;

			int16_t angle =
				Nu3D::Math::CartesianToFixedAngle(g_cutsceneFocusPos.x - g_buzzActor.posAngles.pos.x, g_cutsceneFocusPos.z - g_buzzActor.posAngles.pos.z)
				& 0xFFF;

			int32_t sinValue = Numerics::g_sinCosLUT[(angle - 2048) & 0xFFF];
			int32_t cosValue = orbitRadius * (Numerics::g_sinCosLUT[(angle - 1024) & 0xFFF] >> 4);

			g_cutsceneEyePos.x = g_cutsceneFocusPos.x + orbitRadius * (sinValue >> 4);
			g_cutsceneEyePos.y = g_cutsceneFocusPos.y;
			g_cutsceneEyePos.z = g_cutsceneFocusPos.z + cosValue;

			g_cutsceneTimer = duration;

			InitCutsceneCamera(&g_cutsceneFocusPos, &g_cutsceneEyePos);
		}

		// FUNCTION: TOY2 0x00402290
		void BeginScriptedCutsceneOnBuzz(int32_t duration)
		{
			if (g_cameraState)
			{
				if (g_unk50A538 != (Nu3D::Particles::ParticleInstance*)-1)
				{
					g_unk50A538->lifetime = 1;
					g_unk50A538 = (Nu3D::Particles::ParticleInstance*)-1;
				}

				if (g_unk50A4DC != (Nu3D::Particles::ParticleInstance*)-1)
				{
					g_unk50A4DC->lifetime = 1;
					g_unk50A4DC = (Nu3D::Particles::ParticleInstance*)-1;
				}

				g_buzzActor.actorFlags |= Actor::ACTOR_CUTSCENE_MOVE;
				g_gameplayCameraState.orbitYaw = g_buzzActor.posAngles.angles.yaw;
				g_buzzActor.facingAngle = g_buzzActor.posAngles.angles.yaw;

				g_gameplayCameraState.distance = 1200;
				g_gameplayCameraState.pos.y = g_buzzActor.posAngles.pos.y - 12288;
				g_gameplayCameraState.angles.pitch = 0;
				g_gameplayCameraState.targetPos.x = g_gameplayCameraState.pos.x;
				g_gameplayCameraState.unkShort6 = 0;

				g_cameraState = 0;
				Nu3D::Link::SetScaleFromFixedOffsets(45, 0, 0, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(46, 0, 0, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(47, 0, 0, 0);
			}

			g_inCutscene |= 1;
			g_buzzActor.actorFlags |= Actor::ACTOR_LOCKED_MOVE;
			InputManager::g_directionInputState &= DIR_SECRET_MENU | DIR_MENU | DIR_CAMERA_LEFT | DIR_CAMERA_RIGHT;

			g_cutsceneFocusPos.z = g_buzzActor.posAngles.pos.z;
			g_cutsceneFocusPos.x = g_buzzActor.posAngles.pos.x;
			g_cameraBlendTimer = 64;
			g_cutsceneFocusPos.y = g_buzzActor.posAngles.pos.y - 12288;

			int16_t angle =
				Nu3D::Math::CartesianToFixedAngle(
					g_buzzActor.posAngles.pos.x - g_activeCameraTransform.pos.x, g_buzzActor.posAngles.pos.z - g_activeCameraTransform.pos.z)
				& 0xFFF;

			g_cutsceneEyePos.x = g_cutsceneFocusPos.x + Numerics::g_sinCosLUT[(angle - 2048) & 0xFFF];
			int32_t sinCosZ = Numerics::g_sinCosLUT[(angle - 1024) & 0xFFF];

			g_cutsceneEyePos.y = g_cutsceneFocusPos.y;
			g_cutsceneEyePos.z = g_cutsceneFocusPos.z + sinCosZ;
			g_cutsceneTimer = duration;

			InitCutsceneCamera(&g_cutsceneFocusPos, &g_cutsceneEyePos);
		}

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

			// Clear the state, roll slot, fade word and cutscene word.
			memset(&g_scriptedCameraState, 0, sizeof(ScriptedCameraState));
			memset(&g_scriptedCameraRollStorage, 0, sizeof(g_scriptedCameraRollStorage));
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

			int32_t groundHeight = Nu3D::Collision::GetGroundHeight((Transform*)cameraState, 0);
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

		// FUNCTION: TOY2 0x00403730
		void SnapBehindBuzz(Toy2::Camera::GameplayCameraState* cameraState)
		{
			if (reinterpret_cast<int32_t>(g_unk50A538) != -1)
			{
				g_unk50A538->lifetime = 1;
				g_unk50A538 = reinterpret_cast<Nu3D::Particles::ParticleInstance*>(-1);
			}

			if (reinterpret_cast<int32_t>(g_unk50A4DC) != -1)
			{
				g_unk50A4DC->lifetime = 1;
				g_unk50A4DC = reinterpret_cast<Nu3D::Particles::ParticleInstance*>(-1);
			}

			cameraState->orbitYaw = Toy2::g_buzzActor.posAngles.angles.yaw;
			Toy2::g_buzzActor.facingAngle = Toy2::g_buzzActor.posAngles.angles.yaw;
			cameraState->distance = 1200;

			int32_t yPos = Toy2::g_buzzActor.posAngles.pos.y - 12288;

			cameraState->angles.pitch = 0;
			cameraState->pos.y = yPos;
			cameraState->targetPos.x = cameraState->pos.x;
			cameraState->unkShort6 = 0;

			Toy2::g_buzzActor.actorFlags |= 1u;

			g_cameraState = 0;

			Nu3D::Link::SetScaleFromFixedOffsets(45, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(46, 0, 0, 0);
			Nu3D::Link::SetScaleFromFixedOffsets(47, 0, 0, 0);
		}

		// FUNCTION: TOY2 0x004037E0
		BOOL GetActorLockOnPoint(Vector3I* pos, Toy2::Actor::Toy2Actor* actor, int32_t flag)
		{
			int16_t yawAngle = actor->transform.angles.yaw;

			int16_t angleOffset = yawAngle - 2048;
			int32_t cosYaw = Numerics::g_sinCosLUT[(yawAngle - 1024) & 0xFFF];
			Vector3I16* lockOnOffset = (Vector3I16*)(actor->unkVar34 + 16 * actor->primaryAnimIdx);

			int32_t sinYaw = Numerics::g_sinCosLUT[angleOffset & 0xFFF] >> 2;
			cosYaw >>= 2;
			int32_t offsetX = lockOnOffset->x;

			int32_t rotatedX = (sinYaw * lockOnOffset->z + cosYaw * offsetX) >> 12;
			int32_t rotatedZ = (cosYaw * lockOnOffset->z - sinYaw * offsetX) >> 12;

			if (flag == -1)
			{
				pos->x = rotatedX + actor->transform.pos.x;
				pos->y = actor->transform.pos.y + lockOnOffset->y;
				pos->z = rotatedZ + actor->transform.pos.z;

				return 0;
			}

			pos->x = (rotatedX + actor->transform.pos.x - g_gameplayCameraState.pos.x) >> 5;
			int32_t y = (actor->transform.pos.y + lockOnOffset->y - g_gameplayCameraState.pos.y) >> 5;
			pos->y = y;
			int32_t z = (rotatedZ + actor->transform.pos.z - g_gameplayCameraState.pos.z) >> 5;
			pos->z = z;

			if (flag == 0)
				return 0;

			return z * z + y * y + pos->x * pos->x < flag;
		}

		// FUNCTION: TOY2 0x004038E0
		void VisorMode(Toy2::Camera::GameplayCameraState* state)
		{
			Vector3I lockOnPoint;
			Transform startPos;
			Transform displacement;
			int32_t targetIdx;
			int32_t candidateCount;
			int32_t firstCandidate;
			int32_t yawTarget;
			int32_t pitchTarget;
			int32_t yawInput;
			int32_t pitchInput;
			int32_t frameDelta;
			int32_t yawVel;
			int32_t pitchVel;
			int32_t lockX;
			int32_t lockZ;
			int32_t x;
			int32_t y;
			int32_t z;
			int32_t yawAccel;
			int32_t pitchAccel;
			int32_t pitchCap;
			Nu3D::Particles::ParticleInstance* particle;

			int32_t cameraState = g_cameraState;

			// from the third state on, the camera sits at Buzz
			if (g_cameraState >= 3)
			{
				state->pos.x = g_buzzActor.posAngles.pos.x;
				state->pos.y = g_buzzActor.posAngles.pos.y - 12288;
				state->pos.z = g_buzzActor.posAngles.pos.z;
				cameraState = g_cameraState;
			}

			yawTarget = 0;

			// entering the visor
			if (cameraState == 3)
			{
				AudioManager::PlaySoundEffect(42, &g_buzzActor.posAngles.pos);
				state->angles.pitch = 0;
				state->angles.yaw = g_buzzActor.posAngles.angles.yaw;
				state->targetPos.x = 0;
				g_unk50A290 = g_buzzActor.posAngles.angles.yaw;
				targetIdx = -1;
				g_buzzActor.actorFlags &= ~Actor::ACTOR_CUTSCENE_MOVE;
				g_unk50A28C = 0;
				g_unk50A130 = 0;
				g_unk50A138 = 0;
				g_unk50A4C8 = 2040;
				g_unk50A538 = (Nu3D::Particles::ParticleInstance*)-1;
				g_unk50A4DC = (Nu3D::Particles::ParticleInstance*)-1;
				g_cameraState = 4;
				g_lockOnActorIdx = -1;
				g_unk50A14C = 0;
				g_unk50A540 = 0;
			}
			else
			{
				if (cameraState != 4)
					goto LBL_CHECK_VISOR_EXIT;

				targetIdx = g_lockOnActorIdx;
			}

			// collect the actors (and grapple points) that can be locked on to
			{
				Actor::Toy2Actor* actor = Actor::g_activeActors[0];
				candidateCount = 0;

				if (Actor::g_activeActors[0])
				{
					Actor::Toy2Actor** activeActor = Actor::g_activeActors;
					int32_t* candidate = g_lockOnCandidates;

					do
					{
						if ((uint8_t)actor->creatureRam->defenseMode && actor->unkVar29 >= 0 && (actor->actorFlags & 3) == 3)
						{
							if (GetActorLockOnPoint(&lockOnPoint, actor, 0x1000000))
							{
								*candidate = actor - g_levelActors;
								++candidateCount;
								++candidate;
							}

							targetIdx = g_lockOnActorIdx;
						}

						actor = activeActor[1];
						++activeActor;
					} while (actor);

					yawTarget = 0;
				}
			}

			if (g_grappleTimer)
			{
				int32_t grapplePoint = g_type59VisibleIndices[0];

				if (g_type59VisibleIndices[0] != 0xFF)
				{
					uint8_t* visibleIndex = g_type59VisibleIndices;
					int32_t* candidate = &g_lockOnCandidates[candidateCount];

					do
					{
						++candidate;
						++candidateCount;
						*(candidate - 1) = grapplePoint + 1000;
						grapplePoint = *++visibleIndex;
					} while (grapplePoint != 0xFF);
				}
			}

			g_lockOnCandidates[candidateCount] = -1;
			firstCandidate = g_lockOnCandidates[0];

			if (targetIdx == -1 || g_lockOnCandidates[0] == -1)
			{
			LBL_CLEAR_LOCK_TARGET:
				targetIdx = -1;
				g_unk50A14C = 1;
				g_unk50A540 = 0;
				g_lockOnActorIdx = -1;
			}
			else
			{
				int32_t candidateIdx = g_lockOnCandidates[0];
				int32_t* candidate = g_lockOnCandidates;

				while (candidateIdx != targetIdx)
				{
					candidateIdx = candidate[1];
					++candidate;

					if (candidateIdx == -1)
						goto LBL_CLEAR_LOCK_TARGET;
				}

				if ((InputManager::g_directionInputState & 0x800) == 0 || (InputManager::g_prevDirectionInputState & 0x800) != 0 || ! g_unk50A540)
					goto LBL_UPDATE_TARGET_MARKER;
			}

			// switch to the next target
			if (g_unk50A4DC != (Nu3D::Particles::ParticleInstance*)-1)
			{
				g_unk50A4DC->width = 8;
				targetIdx = g_lockOnActorIdx;
				firstCandidate = g_lockOnCandidates[0];
			}

			if (targetIdx == -1)
			{
				if (! candidateCount)
					goto LBL_CLEAR_TARGET_MARKER;

				g_lockOnActorIdx = firstCandidate;
			}
			else
			{
				int32_t current = 0;

				if (firstCandidate != targetIdx)
				{
					int32_t* candidate = g_lockOnCandidates;

					do
					{
						++candidate;
						++current;
					} while (*candidate != targetIdx);
				}

				if (candidateCount == current)
					g_lockOnActorIdx = firstCandidate;
				else
					g_lockOnActorIdx = g_lockOnCandidates[current + 1];
			}

			AudioManager::PlaySoundEffect(41, &g_buzzActor.posAngles.pos);
			targetIdx = g_lockOnActorIdx;
			g_unk50A14C = 0;

		LBL_UPDATE_TARGET_MARKER:
			// the marker on the target
			if (targetIdx != -1)
			{
				if (targetIdx < 1000)
				{
					GetActorLockOnPoint(&lockOnPoint, &g_levelActors[targetIdx], -1);
					z = lockOnPoint.z;
					y = lockOnPoint.y;
					x = lockOnPoint.x;
				}
				else
				{
					int32_t pointOffset = 3 * targetIdx - 3000;

					x = 32 * *(&Levels::g_recordData[59]->data[0].x + pointOffset);
					lockOnPoint.x = x;
					y = 32 * (*(&Levels::g_recordData[59]->data[0].y + pointOffset) - 208);
					lockOnPoint.y = y;
					z = 32 * *(&Levels::g_recordData[59]->data[0].z + pointOffset);
					lockOnPoint.z = z;
				}

				particle = g_unk50A4DC;

				if (g_unk50A4DC == (Nu3D::Particles::ParticleInstance*)-1)
				{
					particle = Nu3D::Particles::SpawnFromPreset(x, y, z, 48, 2);
					x = lockOnPoint.x;
					g_unk50A4DC = particle;
				}

				particle->pos.x = x;
				g_unk50A4DC->pos.y = lockOnPoint.y;
				g_unk50A4DC->pos.z = lockOnPoint.z;
				g_unk50A4DC->lifetime = 10000;
				goto LBL_REFRESH_LOCK_TARGET;
			}

		LBL_CLEAR_TARGET_MARKER:
			if (g_unk50A4DC == (Nu3D::Particles::ParticleInstance*)-1)
				goto LBL_READ_AIM_INPUT;

			g_unk50A4DC->lifetime = 1;
			g_unk50A4DC = (Nu3D::Particles::ParticleInstance*)-1;

		LBL_REFRESH_LOCK_TARGET:
			targetIdx = g_lockOnActorIdx;

		LBL_READ_AIM_INPUT:
			// the analog stick (or the keys at full speed)
			if ((InputManager::g_directionalInputCount == 1 || InputManager::g_directionalInputCount == 2) && ! g_demoMode)
			{
				yawInput = (InputManager::g_analogInputX * InputManager::g_analogInputX) >> 22;

				if (yawInput < 9)
					yawInput = 0;

				pitchInput = (InputManager::g_analogInputY * InputManager::g_analogInputY) >> 22;

				if (pitchInput < 9)
					pitchInput = 0;
			}
			else
			{
				yawInput = 256;
				pitchInput = 256;
			}

			yawAccel = 0;

			if ((InputManager::g_directionInputState & 0x80u) == 0)
			{
				if ((InputManager::g_directionInputState & 0x20) != 0)
				{
					frameDelta = Renderer::g_frameDelta;
					yawTarget = 2 * yawInput;
					yawAccel = Renderer::g_frameDelta;
				}
				else
				{
					frameDelta = Renderer::g_frameDelta;

					if (g_unk50A130 > 0)
						yawAccel = -(int32_t)Renderer::g_frameDelta;

					if (g_unk50A130 < 0)
						yawAccel = Renderer::g_frameDelta;
				}
			}
			else
			{
				frameDelta = Renderer::g_frameDelta;
				yawAccel = -(int32_t)Renderer::g_frameDelta;
				yawTarget = -2 * yawInput;
			}

			{
				int32_t newYawVel = (yawAccel << 6) / 2 + g_unk50A130;
				g_unk50A130 = newYawVel;

				if (yawAccel >= 0)
				{
					if (newYawVel <= yawTarget)
						goto LBL_UPDATE_PITCH_ACCELERATION;
				}
				else if (newYawVel >= yawTarget)
				{
					goto LBL_UPDATE_PITCH_ACCELERATION;
				}

				g_unk50A130 = yawTarget;
			}

		LBL_UPDATE_PITCH_ACCELERATION:
			pitchAccel = 0;
			pitchCap = 0;

			if ((InputManager::g_directionInputState & 0x40) != 0)
			{
				pitchAccel = -frameDelta;
				pitchCap = -2 * pitchInput;
			}
			else if ((InputManager::g_directionInputState & 0x10) != 0)
			{
				pitchAccel = frameDelta;
				pitchCap = 2 * pitchInput;
			}
			else
			{
				bool negative = g_unk50A138 < 0;

				if (g_unk50A138 > 0)
				{
					pitchAccel = -frameDelta;
					negative = g_unk50A138 < 0;
				}

				if (negative)
					pitchAccel = frameDelta;
			}

			{
				int32_t newPitchVel = (pitchAccel << 6) / 2 + g_unk50A138;
				g_unk50A138 = newPitchVel;

				if (pitchAccel >= 0)
				{
					if (newPitchVel > pitchCap)
					LBL_CLAMP_PITCH_VELOCITY:
						g_unk50A138 = pitchCap;
				}
				else if (newPitchVel < pitchCap)
				{
					goto LBL_CLAMP_PITCH_VELOCITY;
				}
			}

			{
				int32_t lockHeld = g_unk50A540;

				if (! g_unk50A540 && (InputManager::g_directionInputState & 0x800) != 0)
				{
					lockHeld = 1;
					g_unk50A540 = 1;
				}

				if ((InputManager::g_directionInputState & 0xF0) != 0)
				{
					g_unk50A540 = 0;
				LBL_USE_MANUAL_AIM:
					if (g_unk50A4DC != (Nu3D::Particles::ParticleInstance*)-1)
					{
						if (targetIdx < 1000)
						{
							g_unk50A4DC->groundAlignRot = 0;
							g_unk50A4DC->colourR = 0;
						}
						else
						{
							g_unk50A4DC->groundAlignRot = 512;
							g_unk50A4DC->colourR = -1;
						}

						g_unk50A4DC->colourG = -1;
						frameDelta = Renderer::g_frameDelta;
					}

					yawVel = g_unk50A130;
					pitchTarget = g_unk50A138;
					g_unk50A14C = 0;
					goto LBL_APPLY_AIM_VELOCITY;
				}

				if (! lockHeld)
					goto LBL_USE_MANUAL_AIM;
			}

			if (targetIdx == -1 || g_unk50A14C)
			{
				yawVel = pitchAccel;
				pitchTarget = pitchCap;
				goto LBL_APPLY_AIM_VELOCITY;
			}

			// the camera turns towards the target
			{
				Nu3D::Particles::ParticleInstance* marker;

				if (g_tickState[8])
				{
					AudioManager::PlaySoundEffect(40, &g_buzzActor.posAngles.pos);
					marker = g_unk50A4DC;
					targetIdx = g_lockOnActorIdx;
				}
				else
				{
					marker = g_unk50A4DC;
				}

				if (marker != (Nu3D::Particles::ParticleInstance*)-1)
				{
					if (targetIdx < 1000)
						marker->groundAlignRot = 0;
					else
						marker->groundAlignRot = 512;

					if (g_tickAccumulators.tick8Acc >= 4u)
					{
						g_unk50A4DC->colourR = -1;
						g_unk50A4DC->colourG = 0;
					}
					else
					{
						if (g_lockOnActorIdx < 1000)
							g_unk50A4DC->colourR = 0;
						else
							g_unk50A4DC->colourR = -1;

						g_unk50A4DC->colourG = -1;
					}

					targetIdx = g_lockOnActorIdx;
				}
			}

			if (targetIdx < 1000)
			{
				GetActorLockOnPoint(&lockOnPoint, &g_levelActors[targetIdx], 0);
				lockZ = lockOnPoint.z;
				lockX = lockOnPoint.x;
			}
			else
			{
				int32_t pointOffset = 3 * targetIdx - 3000;

				lockX = *(&Levels::g_recordData[59]->data[0].x + pointOffset) - (g_gameplayCameraState.pos.x >> 5);
				lockOnPoint.x = lockX;
				lockOnPoint.y = *(&Levels::g_recordData[59]->data[0].y + pointOffset) - (g_gameplayCameraState.pos.y >> 5) - 208;
				lockZ = *(&Levels::g_recordData[59]->data[0].z + pointOffset) - (g_gameplayCameraState.pos.z >> 5);
				lockOnPoint.z = lockZ;
			}

			{
				int32_t yawDelta = (Nu3D::Math::CartesianToFixedAngle(lockX, lockZ) - g_buzzActor.posAngles.angles.yaw) & 0xFFF;

				if (yawDelta >= 2048)
					yawDelta -= 4096;

				yawVel = 4 * yawDelta;
				g_unk50A130 = 0;

				int32_t pitchDelta =
					(Nu3D::Math::CartesianToFixedAngle((int32_t)sqrt((double)(lockOnPoint.x * lockOnPoint.x + lockOnPoint.z * lockOnPoint.z)), -lockOnPoint.y)
					 - (int16_t)state->targetPos.x - 1024)
					& 0xFFF;

				if (pitchDelta >= 2048)
					pitchDelta -= 4096;

				frameDelta = Renderer::g_frameDelta;
				pitchTarget = 4 * pitchDelta;
				g_unk50A138 = 0;
			}

		LBL_APPLY_AIM_VELOCITY:
			g_buzzActor.posAngles.angles.yaw = (g_buzzActor.posAngles.angles.yaw + yawVel * frameDelta / 16) & 0xFFF;

			{
				int32_t pitch = ((state->targetPos.x + 1024) & 0xFFF) + pitchTarget * frameDelta / 16;

				if (pitch < 1824)
				{
					if (pitch > 200)
						goto LBL_STORE_TARGET_PITCH;

					pitch = 200;
				}
				else
				{
					pitch = 1824;
				}

				g_unk50A138 = 0;

			LBL_STORE_TARGET_PITCH:
				state->targetPos.x = (pitch - 1024) & 0xFFF;
			}

			{
				int32_t pitchKick = 0;

				if (g_unk50A510 > 0)
				{
					pitchKick = Numerics::g_sinCosLUT[(-384 * g_unk50A510) & 0xFFF] / (16 * (g_unk50A510 - 50));
					g_unk50A510 -= Renderer::g_frameDelta;

					if (g_unk50A510 < 0)
						g_unk50A510 = 0;
				}

				int32_t pitchDelta = (pitchKick + state->targetPos.x - state->angles.pitch) & 0xFFF;

				if (pitchDelta >= 0x800)
					pitchDelta -= 4096;

				state->angles.pitch = (state->angles.pitch + (pitchDelta * (int32_t)Renderer::g_frameDelta) / 8) & 0xFFF;

				int32_t yawDelta = (g_buzzActor.posAngles.angles.yaw - state->angles.yaw) & 0xFFF;

				if (yawDelta >= 0x800)
					yawDelta -= 4096;

				state->angles.yaw = (state->angles.yaw + (yawDelta * (int32_t)Renderer::g_frameDelta) / 8) & 0xFFF;
			}

			g_buzzActor.actorFlags &= ~Actor::ACTOR_CUTSCENE_MOVE;

			// the aim of the visor, a ray from Buzz
			{
				uint16_t pitch = state->angles.pitch;
				int32_t cosPitch = Numerics::g_sinCosLUT[(1024 - pitch) & 0xFFF];

				startPos.pos.x = g_buzzActor.posAngles.pos.x;
				startPos.pos.y = g_buzzActor.posAngles.pos.y - 11264;
				startPos.pos.z = g_buzzActor.posAngles.pos.z;

				uint16_t yaw = state->angles.yaw;

				cosPitch *= 4;

				int32_t zScaled = cosPitch * Numerics::g_sinCosLUT[(yaw + 1024) & 0xFFF];

				displacement.pos.x = (cosPitch * Numerics::g_sinCosLUT[yaw & 0xFFF]) >> 13;
				displacement.pos.y = 8 * Numerics::g_sinCosLUT[(-2048 - pitch) & 0xFFF];
				displacement.pos.z = zScaled >> 13;
			}

			{
				int32_t hitWorld = Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 0, 256);
				int32_t hit = SweepSegmentVsActors(&startPos, &displacement, 0) | hitWorld;

				particle = g_unk50A538;

				if (g_unk50A538 == (Nu3D::Particles::ParticleInstance*)-1)
				{
					particle = Nu3D::Particles::SpawnFromPreset(
						startPos.pos.x + displacement.pos.x, startPos.pos.y + displacement.pos.y, startPos.pos.z + displacement.pos.z, 8, 5);
					g_unk50A538 = particle;
				}

				particle->pos.x = startPos.pos.x + displacement.pos.x;
				g_unk50A538->pos.y = startPos.pos.y + displacement.pos.y;
				g_unk50A538->pos.z = startPos.pos.z + displacement.pos.z;
				g_unk50A538->lifetime = 10000;

				if (hit > 1)
				{
					g_unk50A538->colourR = -1;
					g_unk50A538->colourG = 0;
				}
				else
				{
					g_unk50A538->colourR = 0;
					g_unk50A538->colourG = -1;
				}
			}

			g_hudWidgetTimers[0] = 4;
			g_hudWidgetTimers[1] = 4;
			g_hudWidgetTimers[2] = 4;

			Nu3D::Link::SetScaleFromFixedOffsets(45, 2048, 2048, 2048);
			Nu3D::Link::SetScaleFromFixedOffsets(46, 2048, 2048, 2048);
			Nu3D::Link::SetScaleFromFixedOffsets(47, 2048, 2048, 2048);

			{
				// the visor rings follow the aim with a delay
				int32_t pitchLag = (g_unk50A28C - state->targetPos.x) & 0xFFF;

				if (pitchLag >= 0x800)
					pitchLag -= 4096;

				g_unk50A28C = (g_unk50A28C - ((pitchLag * (int32_t)Renderer::g_frameDelta) / 6)) & 0xFFF;

				int32_t yawLag = (g_unk50A290 - g_buzzActor.posAngles.angles.yaw) & 0xFFF;

				if (yawLag >= 0x800)
					yawLag -= 4096;

				g_unk50A290 = (g_unk50A290 - (yawLag * (int32_t)Renderer::g_frameDelta) / 6) & 0xFFF;

				int32_t ringSpin = Numerics::g_sinCosLUT[g_unk50A4C8] >> 10;

				Nu3D::Link::SetPositionRawAndCommit(
					45, g_buzzActor.posAngles.pos.x >> 5, (g_buzzActor.posAngles.pos.y - 12288) >> 5, g_buzzActor.posAngles.pos.z >> 5);
				Nu3D::Link::SetPositionRawAndCommit(
					46, g_buzzActor.posAngles.pos.x >> 5, (g_buzzActor.posAngles.pos.y - 12288) >> 5, g_buzzActor.posAngles.pos.z >> 5);
				Nu3D::Link::SetPositionRawAndCommit(
					47, g_buzzActor.posAngles.pos.x >> 5, (g_buzzActor.posAngles.pos.y - 12288) >> 5, g_buzzActor.posAngles.pos.z >> 5);

				Nu3D::Link::SetRotationRelative8bit(45, 0, (g_buzzActor.posAngles.angles.yaw - 1024) & 0xFFF, ((ringSpin / 2) + state->targetPos.x) & 0xFFF);
				Nu3D::Link::SetRotationRelative8bit(46, 0, (g_unk50A290 - 1024) & 0xFFF, (ringSpin + g_unk50A28C) & 0xFFF);
				Nu3D::Link::SetRotationRelative8bit(47, 0, (g_unk50A290 - 1024) & 0xFFF, (g_unk50A28C + (ringSpin / 2)) & 0xFFF);
			}

			{
				int32_t oldPhase = g_unk50A4C8;
				int32_t newPhase;

				if (g_buzzActor.health > 1)
					newPhase = 32 * Renderer::g_frameDelta + g_unk50A4C8;
				else
					newPhase = (Renderer::g_frameDelta << 6) + g_unk50A4C8;

				g_unk50A4C8 = newPhase;

				if (oldPhase < 2048 && newPhase >= 2048)
					AudioManager::PlaySoundEffect(19, 0);

				if (oldPhase < 4096 && g_unk50A4C8 >= 4096)
					AudioManager::PlaySoundEffect(20, 0);

				g_unk50A4C8 &= 0xFFFu;
				cameraState = g_cameraState;
			}

		LBL_CHECK_VISOR_EXIT:
			// the visor button leaves the visor
			if ((InputManager::g_directionInputState & 0x400) != 0 && (InputManager::g_prevDirectionInputState & 0x400) == 0 && cameraState == 4)
			{
				g_cameraState = 5;
				AudioManager::PlaySoundEffect(43, &g_buzzActor.posAngles.pos);
				cameraState = g_cameraState;
			}

			if (cameraState == 5)
			{
				if (g_unk50A538 != (Nu3D::Particles::ParticleInstance*)-1)
				{
					g_unk50A538->lifetime = 1;
					g_unk50A538 = (Nu3D::Particles::ParticleInstance*)-1;
				}

				if (g_unk50A4DC != (Nu3D::Particles::ParticleInstance*)-1)
				{
					g_unk50A4DC->lifetime = 1;
					g_unk50A4DC = (Nu3D::Particles::ParticleInstance*)-1;
				}

				state->orbitYaw = g_buzzActor.posAngles.angles.yaw;
				g_buzzActor.facingAngle = g_buzzActor.posAngles.angles.yaw;
				int32_t posX = state->pos.x;
				state->distance = 1200;
				state->pos.y = g_buzzActor.posAngles.pos.y - 12288;
				state->angles.pitch = 0;
				state->targetPos.x = posX;
				state->unkShort6 = 0;
				g_buzzActor.actorFlags |= Actor::ACTOR_CUTSCENE_MOVE;
				g_cameraState = 0;
				Nu3D::Link::SetScaleFromFixedOffsets(45, 0, 0, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(46, 0, 0, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(47, 0, 0, 0);
			}
		}

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
			Transform startPos;
			Transform probePos;
			Transform displacement;

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

								startPos.pos.x = cameraPos[0];
								startPos.pos.z = cameraPos[2];
								startPos.pos.y = cameraPos[1];
								displacement.pos.x = v32;
								displacement.pos.y = v27 + state->smoothedY - buzzY;
								displacement.pos.z = v31;

								if (g_unk50A534 >= 66)
									v34 = 2840;
								else
									v34 = 80 * (g_unk50A534 / 2) + 200;

								if (Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 1, v34))
								{
									int32_t v35;

									v35 = (int32_t)sqrt((
										double)((displacement.pos.y >> 5) * (displacement.pos.y >> 5) + (displacement.pos.x >> 5) * (displacement.pos.x >> 5)
												+ (displacement.pos.z >> 5) * (displacement.pos.z >> 5)));
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
			startPos.pos.x = cameraPos[0] - v103;
			startPos.pos.y = cameraPos[1] - 3840;
			int32_t v104;
			v104 = v51 >> 3;

			startPos.pos.z = cameraPos[2] - (v51 >> 3);
			displacement.pos.x = v50 - v103;
			displacement.pos.y = state->smoothedY - buzzY + v48 - 2048;
			displacement.pos.z = v49 - (v51 >> 3);

			if (Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 1, 200))
			{
				probeFlags = 1;

				if (Collision::g_floorSlope.y > 12000)
					probeFlags = 9;

				startPos.pos.x += displacement.pos.x;
				startPos.pos.z += displacement.pos.z;
			}

			probePos.pos.x = v103 + cameraPos[0];
			probePos.pos.y = cameraPos[1] - 3840;

			displacement.pos.x = v50 + v103;
			probePos.pos.z = v104 + cameraPos[2];
			int32_t v53;
			v53 = state->smoothedY - buzzY;
			displacement.pos.z = v49 + v104;
			displacement.pos.y = v53 + v48 - 2048;

			if (Nu3D::Collision::SweepAndSlide(&probePos, &displacement, 0x8000, 1, 200))
			{
				probeFlags |= 2;

				if (Collision::g_floorSlope.y > 12000)
					probeFlags |= 8;

				probePos.pos.x += displacement.pos.x;
				probePos.pos.z += displacement.pos.z;
			}

			startPos.pos.x = cameraPos[0];
			startPos.pos.y = cameraPos[1] + 3072;
			startPos.pos.z = cameraPos[2];
			displacement.pos.x = v50;
			displacement.pos.y = v48 + state->smoothedY - buzzY;
			displacement.pos.z = v49;

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
			displacement.pos.x = (v71 * Numerics::g_sinCosLUT[state->orbitYaw & 0xFFF]) >> 9;

			int32_t v74 = state->smoothedY;
			v72 >>= 9;
			displacement.pos.y = v72;
			state->targetPos.x = buzzX - displacement.pos.x;
			state->targetPos.y = v74 + v72 - 0x2000;
			displacement.pos.z = v73 >> 9;
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
				displacement.pos.x = buzzX - state->pos.x;
				displacement.pos.y = buzzY - state->pos.y - 6144;
				displacement.pos.z = buzzZ - cameraPos[2];

				if (Nu3D::Collision::SweepQuery(cameraPos, &displacement.pos.x, 1, 1) == 1)
				{
					int32_t v97;

					v97 = (int32_t)sqrt((
						double)((displacement.pos.y >> 5) * (displacement.pos.y >> 5) + (displacement.pos.x >> 5) * (displacement.pos.x >> 5)
								+ (displacement.pos.z >> 5) * (displacement.pos.z >> 5)));
					state->distance -= v97;

					if (state->distance < 10)
						state->distance = 10;

					if (v97 < 300)
					{
						state->pos.x += displacement.pos.x;
						state->pos.y = displacement.pos.y + state->pos.y;
						state->pos.z = displacement.pos.z + state->pos.z;
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

					uint32_t scriptedRoll;
					memcpy(&scriptedRoll, &g_scriptedCameraRollStorage, sizeof(scriptedRoll));
					roll = (int16_t)scriptedRoll;
					rollDelta = ((uint32_t)g_activeCameraTransform.roll - scriptedRoll) & 0xFFF;

					if (rollDelta >= 0x800)
						rollDelta -= 4096;

					pitch = (g_scriptedCameraState.angles.pitch + ((pitchDelta * g_cameraBlendTimer) >> 6)) & 0xFFF;
					yaw = g_scriptedCameraState.angles.yaw + ((yawDelta * g_cameraBlendTimer) >> 6);

					g_activeCameraTransform.angles.pitch = pitch;
					goto LBL_FINISH_CAMERA_BLEND;
				}

				g_activeCameraTransform.pos.x = g_scriptedCameraState.pos.x;
				g_activeCameraTransform.pos.y = g_scriptedCameraState.pos.y;
				g_activeCameraTransform.pos.z = g_scriptedCameraState.pos.z;
				g_activeCameraTransform.angles = g_scriptedCameraState.angles;
				g_activeCameraTransform.roll = g_scriptedCameraRollStorage.words.roll;
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

				LBL_FINISH_CAMERA_BLEND:
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
			Transform displacement;
			Transform startPos;
			Transform probePos;

			if (buzzActor->gravityVel > 0 && (Buzz::g_buzzActionState & 0xFFF7F) == 0)
			{
				y = buzzActor->posAngles.pos.y;

				if (buzzActor->floorYPos - y > 0x2000 && buzzActor->floorYPos != (int32_t)0x80000000)
				{
					probePos.pos.y = y - 13824;
					v4 = Numerics::g_sinCosLUT[buzzActor->posAngles.angles.yaw & 0xFFF] / 3;
					v5 = Numerics::g_sinCosLUT[(buzzActor->posAngles.angles.yaw + 1024) & 0xFFF] / 3;
					probePos.pos.x = v4 + buzzActor->posAngles.pos.x;
					probePos.pos.z = v5 + buzzActor->posAngles.pos.z;

					v7 = Nu3D::Collision::GetGroundHeight(&probePos, 0) - 200;

					if (Collision::g_floorSlope.y < -15000)
					{
						if (v7 < buzzActor->posAngles.pos.y - 13824 && v7 >= buzzActor->motionTargetPos.y - 13824)
						{
							startPos.pos.y = buzzActor->posAngles.pos.y;
							startPos.pos.x = buzzActor->posAngles.pos.x - v4 / 4;
							startPos.pos.z = buzzActor->posAngles.pos.z - v5 / 4;

							displacement.pos.x = 0;
							displacement.pos.z = 0;
							displacement.pos.y = v7 - buzzActor->posAngles.pos.y;

							if (! Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 0, 4000))
							{
								startPos.pos.y = v7 - 6000;
								displacement.pos.x = v4 + v4 / 3;
								displacement.pos.y = 0;
								displacement.pos.z = v5 + v5 / 3;

								if (! Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 0, 4000))
								{
									AudioManager::PlaySoundEffect(23, &buzzActor->posAngles.pos);
									Toy2::Portal::UpdateActiveSectorAt(buzzActor->posAngles.pos.x, v7, buzzActor->posAngles.pos.z);

									buzzActor->posAngles.pos.x += v4;
									buzzActor->posAngles.pos.y = v7;
									buzzActor->posAngles.pos.z += v5;

									Buzz::g_unk53C660 = 82;
									Platform::g_unk559E80 = Collision::GetGroundContactIdx();

									startPos.pos.y = v7;

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
							int32_t zDiff = cam->pos.z - curActor->transform.pos.z;
							int32_t yDiff = cam->pos.y - curActor->transform.pos.y;
							int32_t xDiff = cam->pos.x - curActor->transform.pos.x;

							if ((xDiff >> 5) * (xDiff >> 5) + (yDiff >> 5) * (yDiff >> 5) + (zDiff >> 5) * (zDiff >> 5) < visibilitySquare + flagAdd)
							{
								Vector3F point;

								point.x = (float)(curActor->transform.pos.x / 32);
								point.y = (float)(curActor->transform.pos.y / 32);
								point.z = (float)(curActor->transform.pos.z / 32);
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

		// FUNCTION: TOY2 0x004A5870
		int32_t SweepSegmentVsActors(Transform* startPos, Transform* displacement, int32_t damageMode)
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
					int32_t endX = displacement->pos.x + startPos->pos.x;
					int32_t endY = displacement->pos.y + startPos->pos.y;
					int32_t endZ = displacement->pos.z + startPos->pos.z;

					int16_t* attach = (int16_t*)(actor->unkVar34 + 16 * actor->primaryAnimIdx);

					int32_t cosYaw = Numerics::g_sinCosLUT[(actor->transform.angles.yaw - 1024) & 0xFFF] >> 2;
					int32_t sinYaw = Numerics::g_sinCosLUT[(actor->transform.angles.yaw - 2048) & 0xFFF] >> 2;

					int32_t centerX = actor->transform.pos.x + ((sinYaw * attach[2] + cosYaw * attach[0]) >> 12);
					int32_t centerY = actor->transform.pos.y + attach[1];
					int32_t centerZ = actor->transform.pos.z + ((cosYaw * attach[2] - sinYaw * attach[0]) >> 12);

					int32_t startToCenterZ = centerZ - startPos->pos.z;
					int32_t startToCenterX = centerX - startPos->pos.x;

					int32_t scaleX = attach[4];
					int32_t scaleY = attach[5];
					int32_t scaleZ = attach[6];

					int32_t startScaledY = (scaleY * (centerY - startPos->pos.y)) >> 8;
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
							int32_t travel = closest - (int32_t)sqrt((double)(radiusSq - distSq));
							int32_t hitX = ((dir.x * travel) >> 4) / scaleX;
							int32_t hitY = 2 * dir.y * travel / scaleY;
							int32_t hitZ = ((dir.z * travel) >> 4) / scaleZ;

							hitActor = actor;
							hit = 2;

							displacement->pos.y = hitY;
							displacement->pos.x = (cosYaw * hitX - sinYaw * hitZ) >> 7;
							displacement->pos.z = (sinYaw * hitX + cosYaw * hitZ) >> 7;
						}
					}
				}

				actor = *++nextActor;
			} while (actor);

			if (! hit || ! damageMode)
				return hit;

			if (damageMode == 1)
				Actor::HandleDamage(hitActor, g_buzzActor.posAngles.angles.yaw, 2);
			else
				Actor::HandleDamage(hitActor, g_buzzActor.posAngles.angles.yaw, 3);

			if ((hitActor->creatureRam->defenseMode & 1) != 0)
				return hit;

			return 4;
		}
	}
}

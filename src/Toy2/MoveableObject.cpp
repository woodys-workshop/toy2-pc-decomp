#include "Toy2/MoveableObject.h"
#include "Toy2/Toy2.h"
#include "Toy2/Levels.h"
#include "Toy2/Platform.h"
#include "Nu3D/Link.h"
#include "Nu3D/Math.h"
#include "Nu3D/Particles.h"
#include "Renderer/Renderer.h"
#include "AudioManager/AudioManager.h"
#include "InputManager.h"
#include "InputTypes.h"
#include "Random.h"

#include <string.h>
#include <math.h>

namespace Toy2
{
	namespace MoveableObject
	{
		// GLOBAL: TOY2 0x0053C680
		MoveableObject g_moveableObjects[10];

		// GLOBAL: TOY2 0x0053C670
		int32_t g_moveableGrabSoundCooldown;

		// GLOBAL: TOY2 0x0053C65C
		int32_t g_moveableObjectCount;

		// the offset of Buzz to the moveable object he grabbed
		// GLOBAL: TOY2 0x0053C81C
		int32_t g_grabOffsetX;

		// GLOBAL: TOY2 0x0053C820
		int32_t g_grabOffsetZ;

		// GLOBAL: TOY2 0x0052FD24
		int32_t g_unk52FD24;

		// FUNCTION: TOY2 0x004334D0
		void ComputeSegment(int32_t recordType, Toy2::MoveableObject::MoveableObject* l_moveableObject)
		{
			Levels::RecordData* record = Levels::g_recordData[recordType];

			int32_t waypointIdx = l_moveableObject->waypointIdx;
			int32_t x = record->data[waypointIdx + 1].x - record->data[waypointIdx].x;
			int32_t z = record->data[waypointIdx + 1].z - record->data[waypointIdx].z;

			l_moveableObject->segHeading = Nu3D::Math::CartesianToFixedAngle(x, z) & 0xFFF;

			Vector3I normalised;
			normalised.x = x;
			normalised.y = 0;
			normalised.z = z;

			Nu3D::Math::NormalizeToFixedPoint(&normalised, &normalised);

			int32_t length = (int32_t)sqrt((double)(x * x + z * z));

			l_moveableObject->segDeltaX = normalised.x;
			l_moveableObject->segLength = length;
			l_moveableObject->segDeltaZ = normalised.z;
			l_moveableObject->pushState = 0;

			if (l_moveableObject->minWaypointIdx < Levels::g_recordData[l_moveableObject->pathIdx]->recordCount - 2)
			{
				int32_t nextIdx = l_moveableObject->waypointIdx;
				Levels::RecordData* nextRecord = Levels::g_recordData[recordType];

				if (nextRecord->data[nextIdx + 1].x == nextRecord->data[nextIdx + 2].x && nextRecord->data[nextIdx + 1].z == nextRecord->data[nextIdx + 2].z)
					l_moveableObject->pushState = l_moveableObject->segLength / 2;
			}
		}

		// FUNCTION: TOY2 0x004335D0
		void InitTable(int16_t* array)
		{
			memset(g_moveableObjects, 0, sizeof(g_moveableObjects));

			g_moveableGrabSoundCooldown = 0;
			g_moveableObjectCount = 0;

			if (array)
			{
				int16_t modelNode = *array;

				if (*array != -1)
				{
					MoveableObject* moveableObject = g_moveableObjects;

					do
					{
						int32_t waypointIdx;

						if (g_levelFileIndex == 11 && modelNode == 29)
							waypointIdx = 2;
						else
							waypointIdx = 0;

						int16_t* entry = array + 1;

						moveableObject->modelNode = modelNode;
						moveableObject->platformIdx = *entry++;

						int16_t pathIdx = *entry;
						array = entry + 1;
						moveableObject->pathIdx = pathIdx;

						Levels::RecordData* path = Levels::g_recordData[pathIdx];

						Vector3I* pos = &moveableObject->pos;

						pos->x = 32 * path->data[waypointIdx].x;
						pos->y = 32 * path->data[waypointIdx].y;
						pos->z = 32 * path->data[waypointIdx].z;
						moveableObject->waypointIdx = waypointIdx;
						moveableObject->minWaypointIdx = 0;

						ComputeSegment(pathIdx, moveableObject);

						bool hasNoModel = moveableObject->modelNode < 0;
						moveableObject->segProgress = 0;

						if (! hasNoModel)
							Nu3D::Link::SetPositionRawAndCommit(moveableObject->modelNode, pos->x >> 5, pos->y >> 5, pos->z >> 5);

						Platform::SetOrigin(moveableObject->platformIdx, pos->x, pos->y, pos->z);

						++moveableObject;
						++g_moveableObjectCount;

						modelNode = *array;
					} while (*array != -1);
				}
			}
		}

		// FUNCTION: TOY2 0x00433700
		void Update(Toy2::Buzz::Toy2BuzzActor* buzzActor)
		{
			int32_t frameDelta = Renderer::g_frameDelta;

			g_moveableGrabSoundCooldown -= Renderer::g_frameDelta;

			if (g_moveableGrabSoundCooldown < 0)
				g_moveableGrabSoundCooldown = 0;

			if ((InputManager::g_directionInputState & (INPUT_UP | INPUT_RIGHT | INPUT_DOWN | INPUT_LEFT)) == 0 || (Buzz::g_buzzActionState & 0xFFDFE) != 0)
			{
				Buzz::g_curMoveableObject = 0;
			}
			else if (! Buzz::g_curMoveableObject)
			{
				if (buzzActor->collisionFlags)
				{
					MoveableObject* object = g_moveableObjects;

					for (int32_t index = 0; index < g_moveableObjectCount; index++)
					{
						if (! object->liftVel && object->pushState >= 0)
						{
							if ((Platform::GetFlags(object->platformIdx) & 3) == 1)
							{
								int16_t* contactFace = (int16_t*)Platform::GetContactFace32(object->platformIdx);
								int32_t grabAngle = (Nu3D::Math::CartesianToFixedAngle(contactFace[0], contactFace[2]) - 2048) & 0xFFF;

								if (((grabAngle - buzzActor->posAngles.angles.yaw + 384) & 0xFFF) < 0x300)
								{
									Buzz::g_unk53C624 = grabAngle;

									Buzz::g_curMoveableObject = index + 1;
									g_grabOffsetX = (object->pos.x - buzzActor->posAngles.pos.x) >> 5;
									g_grabOffsetZ = (object->pos.z - buzzActor->posAngles.pos.z) >> 5;

									if (! g_moveableGrabSoundCooldown)
									{
										AudioManager::Preset::PlayOneShotSound2(45, (Actor::Toy2Actor*)buzzActor);
										g_moveableGrabSoundCooldown = 40;
									}

									frameDelta = Renderer::g_frameDelta;

									break;
								}
							}

							frameDelta = Renderer::g_frameDelta;
						}

						++object;
					}
				}
			}

			for (int32_t counter = 0; counter < g_moveableObjectCount; counter++)
			{
				MoveableObject* object = &g_moveableObjects[counter];

				if (object->liftVel)
				{
					int32_t liftVel = object->liftVel + (frameDelta << 8) / 4;

					object->liftVel = liftVel;

					if (liftVel > 2048)
						object->liftVel = 2048;

					int32_t posY = 32 * frameDelta * (object->liftVel >> 5) + object->pos.y;

					object->pos.y = posY;

					if (posY >= 32 * Levels::g_recordData[object->pathIdx]->data[object->waypointIdx].y)
					{
						int16_t waypointIdx = object->waypointIdx;
						int32_t pathIdx = object->pathIdx;

						object->pos.y = 32 * Levels::g_recordData[pathIdx]->data[waypointIdx].y;
						object->liftVel = 0;
						object->minWaypointIdx = waypointIdx;

						ComputeSegment(pathIdx, object);
						AudioManager::PlaySoundEffect(47, &object->pos);
					}

					if (object->modelNode >= 0)
						Nu3D::Link::SetPositionRawAndCommit(object->modelNode, object->pos.x >> 5, object->pos.y >> 5, object->pos.z >> 5);

					Platform::SetOrigin(object->platformIdx, object->pos.x, object->pos.y, object->pos.z);

					frameDelta = Renderer::g_frameDelta;
				}

				if (object->pushState < 0)
				{
					object->segProgress += 24 * frameDelta;

					if (object->segProgress > object->segLength)
					{
						object->waypointIdx += 2;
						object->liftVel = 2;
						object->segHeading = 0;
						object->segLength = 0;
						object->segDeltaX = 0;
						object->segDeltaZ = 0;
						object->pushState = 0;
						object->segProgress = 0;
					}

					Levels::RecordData* record = Levels::g_recordData[object->pathIdx];

					int32_t segProgress = object->segProgress;
					int32_t posX = 32 * record->data[object->waypointIdx].x + (((segProgress * object->segDeltaX) >> 7) & 0xFFFFFFE0);

					object->pos.x = posX;

					int32_t posZ = (((segProgress * object->segDeltaZ) >> 7) & 0xFFFFFFE0) + 32 * record->data[object->waypointIdx].z;

					object->pos.z = posZ;

					if (object->modelNode >= 0)
						Nu3D::Link::SetPositionRawAndCommit(object->modelNode, posX >> 5, object->pos.y >> 5, posZ >> 5);

					Platform::SetOrigin(object->platformIdx, object->pos.x, object->pos.y, object->pos.z);

					frameDelta = Renderer::g_frameDelta;
				}
			}

			if (Buzz::g_curMoveableObject)
			{
				int32_t pushing = 0;
				int32_t pushedForward = 0;
				MoveableObject* object = &g_moveableObjects[Buzz::g_curMoveableObject - 1];
				int32_t pathIdx;
				int16_t waypointIdx;
				int16_t segProgress;
				int16_t segLength;
				bool notPastEnd;

				if (! object->liftVel && object->pushState >= 0)
				{
					if (object->minWaypointIdx >= Levels::g_recordData[object->pathIdx]->recordCount - 1)
						goto LABEL_67;

					if (((Buzz::g_unk53C624 - object->segHeading + 8) & 0xFFF) < 0x10)
					{
						if (Levels::g_recordData[58] && Levels::g_recordData[58]->data[Buzz::g_curMoveableObject - 1].y != 0x80000000)
						{
							Levels::Method3(Buzz::g_curMoveableObject - 1, 0);
							frameDelta = Renderer::g_frameDelta;
						}

						if (Toy2::g_levelFileIndex != 11 || Buzz::g_curMoveableObject != 1 || g_unk52FD24 <= 0)
						{
							pushing = 1;
							object->segProgress += 12 * frameDelta;
							pushedForward = 1;

							if (object->pushState > 0 && object->segProgress > object->pushState)
							{
								object->pushState = -1;
								AudioManager::PlaySoundEffect(49, &object->pos);
								Buzz::g_curMoveableObject = 0;
								buzzActor->velX = 0;
								buzzActor->velForward = 0;
								Buzz::g_unk53C5DC = 10;
							}

							frameDelta = Renderer::g_frameDelta;
						}
					}

					if (((Buzz::g_unk53C624 - object->segHeading - 2040) & 0xFFF) < 0x10)
					{
						pushing = 1;
						pushedForward = 1;
						object->segProgress += -12 * frameDelta;
					}

					segProgress = object->segProgress;

					if (! segProgress)
					{
						waypointIdx = object->waypointIdx;

						if (waypointIdx > object->minWaypointIdx)
							goto LABEL_65;
					}

					segLength = object->segLength;
					notPastEnd = segProgress <= segLength;

					if (segProgress == segLength)
					{
						pathIdx = object->pathIdx;
						waypointIdx = object->waypointIdx;

						if (waypointIdx < Levels::g_recordData[pathIdx]->recordCount - 2)
							goto LABEL_61;

						pushing = pushedForward;
						notPastEnd = segProgress <= segLength;
					}

					if (notPastEnd)
						goto LABEL_62;

					pathIdx = object->pathIdx;
					waypointIdx = object->waypointIdx;

					if (waypointIdx >= Levels::g_recordData[pathIdx]->recordCount - 2)
					{
						object->segProgress = segLength;
						goto LABEL_62;
					}

				LABEL_61:
					object->waypointIdx = waypointIdx + 1;
					ComputeSegment(pathIdx, object);
					pushing = 0;
					object->segProgress = 0;

				LABEL_62:
					if (object->segProgress < 0)
					{
						waypointIdx = object->waypointIdx;

						if (waypointIdx <= object->minWaypointIdx)
						{
							object->segProgress = 0;

						LABEL_66:
							pushing = 0;
							goto LABEL_67;
						}

					LABEL_65:
						object->waypointIdx = waypointIdx - 1;
						ComputeSegment(object->pathIdx, object);

						object->segProgress = object->segLength;

						goto LABEL_66;
					}

				LABEL_67:
					int32_t pathIndex = object->pathIdx;
					int32_t progress = object->segProgress;

					object->pos.x = 32 * Levels::g_recordData[pathIndex]->data[object->waypointIdx].x + (((progress * object->segDeltaX) >> 7) & 0xFFFFFFE0);

					int32_t deltaZ = (progress * object->segDeltaZ) >> 7;
					int32_t posZ = (deltaZ & 0xFFFFFFE0) + 32 * Levels::g_recordData[pathIndex]->data[object->waypointIdx].z;

					object->pos.z = posZ;

					if (object->modelNode >= 0)
						Nu3D::Link::SetPositionRawAndCommit(object->modelNode, object->pos.x >> 5, object->pos.y >> 5, posZ >> 5);

					Platform::SetOrigin(object->platformIdx & 0x7FFF, object->pos.x, object->pos.y, object->pos.z);
					buzzActor->velX = object->pos.x - 32 * g_grabOffsetX - buzzActor->posAngles.pos.x;
					buzzActor->velForward = object->pos.z - 32 * g_grabOffsetZ - buzzActor->posAngles.pos.z;

					if (pushing)
					{
						AudioManager::PlaySoundEffect(46, &object->pos);

						if (g_tickState[4])
						{
							int32_t randOffset = *g_randDatBufferPtr++ - 128;

							Nu3D::Particles::SpawnFromPreset(
								buzzActor->posAngles.pos.x + ((randOffset * buzzActor->velForward) >> 4) + 6 * buzzActor->velX,
								buzzActor->posAngles.pos.y,
								buzzActor->posAngles.pos.z + ((randOffset * buzzActor->velX) >> 4) + 6 * buzzActor->velForward,
								2,
								1);
						}
					}
				}
			}
		}
	}
}

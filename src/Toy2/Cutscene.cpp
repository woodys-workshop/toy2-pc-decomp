#include "Toy2/Cutscene.h"
#include "Toy2/Camera.h"
#include "Toy2/Dialogue.h"
#include "Toy2/Collectables.h"
#include "Toy2/Toy2.h"
#include "Toy2/Levels.h"
#include "Nu3D/Collision.h"
#include "Nu3D/Math.h"
#include "Renderer/Renderer.h"
#include "AudioManager/AudioManager.h"
#include "InputManager.h"

#include <math.h>

namespace Toy2
{
	namespace Cutscene
	{
		// FUNCTION: TOY2 0x00402A10
		void Update()
		{
			int32_t finishedBars = 0;

			if (Camera::g_cutsceneTimer > 0)
			{
				Camera::g_scriptedCameraState.focusPos = Camera::g_cutsceneFocusPos;
				Camera::g_scriptedCameraState.eyePos = Camera::g_cutsceneEyePos;
				Camera::g_cutsceneTimer -= Renderer::g_frameDelta;

				if (Camera::g_cutsceneTimer <= 0)
				{
					Camera::g_inCutscene &= ~1;
					g_buzzActor.actorFlags &= ~Actor::ACTOR_LOCKED_MOVE;
					Camera::g_cutsceneTimer = 0;
					Camera::g_cameraBlendTimer = 64;
				}
			}

			if (Renderer::g_enableCinematicBars != 0)
			{
				if (Dialogue::g_unk50A518 != 0)
					Dialogue::UpdateSubtitleBox();

				Levels::RecordData* record = Levels::g_recordData[Camera::g_unk50A0C8];
				Camera::g_unk50A4D0 += Renderer::g_frameDelta;

				if (Dialogue::g_unk50A518 != 0)
				{
					int32_t* script = Camera::g_unk50A294;
					int32_t eyeIdx = Camera::g_unk50A4BC;
					int32_t focusIdx = Camera::g_unk50A514;
					bool moving;
					int32_t nextEyeIdx;
					int32_t segmentSpeed;

					if (Camera::g_unk50A134 != 0)
					{
						moving = Camera::g_unk50A134 < 0;
					}
					else
					{
						do
						{
							while (1)
							{
								switch (*script)
								{
									// stop the script, it waits here until the dialogue is over
									case -1:
										script -= 1;
										Camera::g_unk50A134 = 1;
										Camera::g_unk50A294 = script;

										goto LABEL_47;

									// use a record
									case 0:
										Camera::g_unk50A0C8 = script[1];
										script += 2;
										Camera::g_unk50A294 = script;
										record = Levels::g_recordData[Camera::g_unk50A0C8];

										continue;

									// place an actor (or Buzz for -1) at a point of the record
									case 1:
										if (script[1] >= 0)
										{
											g_levelActors[script[1]].pos.x = 32 * record->data[script[2]].x;
											g_levelActors[script[1]].pos.y = 32 * record->data[script[2]].y;
											g_levelActors[script[1]].pos.z = 32 * record->data[script[2]].z;
											g_levelActors[script[1]].pos.y = Nu3D::Collision::GetGroundHeight((WorldPos*)&g_levelActors[script[1]].pos, 0);
											g_levelActors[script[1]].velX = 0;
											g_levelActors[script[1]].gravityVel = 0;
											g_levelActors[script[1]].velForward = 0;
										}
										else
										{
											g_buzzActor.posAngles.pos.x = 32 * record->data[script[2]].x;
											g_buzzActor.posAngles.pos.y = 32 * record->data[script[2]].y;
											g_buzzActor.posAngles.pos.z = 32 * record->data[script[2]].z;
											g_buzzActor.posAngles.pos.y = Nu3D::Collision::GetGroundHeight((WorldPos*)&g_buzzActor.posAngles.pos, 0);
											g_buzzActor.velX = 0;
											g_buzzActor.gravityVel = 0;
											g_buzzActor.velForward = 0;
											g_buzzActor.unkVar30 = 0;
											g_buzzActor.unkVar31 = 0;
											g_buzzActor.unkVar37 = 1;
											g_buzzActor.motionTargetPos = g_buzzActor.posAngles.pos;

											ResetBuzzState();

											g_buzzActor.actorFlags |= Actor::ACTOR_LOCKED_MOVE;
											InputManager::g_directionInputState &= 0x309;
										}

										eyeIdx = Camera::g_unk50A4BC;
										focusIdx = Camera::g_unk50A514;
										script = Camera::g_unk50A294 + 3;
										Camera::g_unk50A294 += 3;

										break;

									// turn an actor (or Buzz for -1)
									case 2:
										if (script[1] >= 0)
										{
											g_levelActors[script[1]].creatureRam->initialFacingAngle = script[2] >> 4;

											eyeIdx = Camera::g_unk50A4BC;
											g_levelActors[Camera::g_unk50A294[1]].yawAngle = Camera::g_unk50A294[2];

											int32_t actorIdx = Camera::g_unk50A294[1];
											int32_t yaw = Camera::g_unk50A294[2];

											script = Camera::g_unk50A294 + 3;
											Camera::g_unk50A294 = script;
											g_levelActors[actorIdx].targetYaw = yaw;
											focusIdx = Camera::g_unk50A514;
										}
										else
										{
											uint16_t yaw = script[2];

											script += 3;
											g_buzzActor.posAngles.angles.yaw = yaw;
											g_buzzActor.facingAngle = script[-1];
											Camera::g_gameplayCameraState.orbitYaw = script[-1];
											Camera::g_unk50A294 = script;
										}

										break;

									// the point the camera looks at, -1 is half way through the record
									case 3:
										focusIdx = script[1];

										if (focusIdx < 0)
											focusIdx = eyeIdx + (record->recordCount - eyeIdx) / 2;

										script += 2;
										Camera::g_unk50A514 = focusIdx;
										Camera::g_unk50A294 = script;

										continue;

									// the point the camera starts at
									case 4:
										eyeIdx = script[1];
										script += 2;
										Camera::g_unk50A4BC = eyeIdx;
										Camera::g_unk50A294 = script;

										continue;

									// wait, or move along the path while it is negative
									case 7:
										Camera::g_unk50A134 = script[1];
										script += 2;
										Camera::g_unk50A294 = script;

										break;

									// put the camera on the first point of the path
									case 10:
										Camera::g_scriptedCameraState.focusPos.x = 32 * record->data[focusIdx].x;
										Camera::g_scriptedCameraState.focusPos.y = 32 * record->data[focusIdx].y;
										Camera::g_scriptedCameraState.focusPos.z = 32 * record->data[focusIdx].z;
										Camera::g_scriptedCameraState.eyePos.x = 32 * record->data[eyeIdx].x;
										Camera::g_scriptedCameraState.eyePos.y = 32 * record->data[eyeIdx].y;
										Camera::g_scriptedCameraState.eyePos.z = 32 * record->data[eyeIdx].z;
										Camera::g_scriptedCameraState.pos = Camera::g_scriptedCameraState.eyePos;

										{
											int32_t deltaX = record->data[eyeIdx + 1].x - record->data[eyeIdx].x;
											int32_t deltaY = record->data[eyeIdx + 1].y - record->data[eyeIdx].y;
											int32_t deltaZ = record->data[eyeIdx + 1].z - record->data[eyeIdx].z;

											Camera::g_unk50A1F0 = (int32_t)sqrt((double)(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ));
											Camera::g_unk50A144 = 0;

											deltaX = record->data[eyeIdx + 2].x - record->data[eyeIdx + 1].x;
											deltaY = record->data[eyeIdx + 2].y - record->data[eyeIdx + 1].y;
											deltaZ = record->data[eyeIdx + 2].z - record->data[eyeIdx + 1].z;

											Camera::g_unk50A0CC = (int32_t)sqrt((double)(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ));
										}

										{
											int32_t y = (Camera::g_scriptedCameraState.focusPos.y - Camera::g_scriptedCameraState.eyePos.y) >> 5;
											int32_t z = (Camera::g_scriptedCameraState.focusPos.z - Camera::g_scriptedCameraState.eyePos.z) >> 5;
											int32_t x = (Camera::g_scriptedCameraState.focusPos.x - Camera::g_scriptedCameraState.eyePos.x) >> 5;
											int16_t pitch;

											Camera::g_scriptedCameraState.angles.yaw = Nu3D::Math::CartesianToFixedAngle(x, z);

											if (y >= 0)
												pitch = Nu3D::Math::CartesianToFixedAngle(-(y * y), x * x + z * z);
											else
												pitch = Nu3D::Math::CartesianToFixedAngle(y * y, x * x + z * z);

											eyeIdx = Camera::g_unk50A4BC;
											focusIdx = Camera::g_unk50A514;
											script = Camera::g_unk50A294 + 1;
											Camera::g_scriptedCameraState.angles.pitch = -pitch;
											Camera::g_unk50A294 += 1;
										}

										break;

									default:
										continue;
								}

								break;
							}

							moving = Camera::g_unk50A134 < 0;
						} while (Camera::g_unk50A134 == 0);
					}

					// the camera moves along the path
					if (moving)
					{
						int32_t progress = Camera::g_unk50A53C;
						int32_t focusY = progress * (record->data[focusIdx + 1].y - record->data[focusIdx].y);
						int32_t focusZ = progress * (record->data[focusIdx + 1].z - record->data[focusIdx].z) / Camera::g_unk50A1F0;

						Camera::g_scriptedCameraState.focusPos.x =
							32 * (record->data[focusIdx].x + progress * (record->data[focusIdx + 1].x - record->data[focusIdx].x) / Camera::g_unk50A1F0);
						Camera::g_scriptedCameraState.focusPos.y = 32 * (record->data[focusIdx].y + focusY / Camera::g_unk50A1F0);
						Camera::g_scriptedCameraState.focusPos.z = 32 * (focusZ + record->data[focusIdx].z);

						int32_t eyeY = progress * (record->data[eyeIdx + 1].y - record->data[eyeIdx].y) / Camera::g_unk50A1F0;

						nextEyeIdx = eyeIdx + 1;

						int32_t eyeZ = record->data[eyeIdx + 1].z - record->data[eyeIdx].z;

						Camera::g_scriptedCameraState.eyePos.x =
							32 * (record->data[eyeIdx].x + progress * (record->data[eyeIdx + 1].x - record->data[eyeIdx].x) / Camera::g_unk50A1F0);
						Camera::g_scriptedCameraState.eyePos.y = 32 * (record->data[eyeIdx].y + eyeY);
						Camera::g_scriptedCameraState.eyePos.z = 32 * (progress * eyeZ / Camera::g_unk50A1F0 + record->data[eyeIdx].z);

						// the camera speeds up towards the speed of the next segment
						Camera::g_unk50A144 += ((int32_t)Renderer::g_frameDelta * (Camera::g_unk50A144 - Camera::g_unk50A0CC)) / -16;

						segmentSpeed = Camera::g_unk50A144;

						if (Camera::g_unk50A144 < 128)
						{
							segmentSpeed = 128;
							Camera::g_unk50A144 = 128;
						}

						Camera::g_unk50A53C += ((int32_t)Renderer::g_frameDelta * segmentSpeed) / 32;

						if (Camera::g_unk50A53C >= Camera::g_unk50A1F0)
						{
							if (focusIdx >= record->recordCount - 2)
							{
								Camera::g_unk50A53C = Camera::g_unk50A1F0 - 1;
								Camera::g_unk50A294 = script + 1;
								Camera::g_unk50A0CC = 0;
								Camera::g_unk50A134 = 0;

								return;
							}

							Camera::g_unk50A53C -= Camera::g_unk50A1F0;
							Camera::g_unk50A1F0 = Camera::g_unk50A0CC;
							Camera::g_unk50A4BC = nextEyeIdx;
							Camera::g_unk50A514 = focusIdx + 1;

							if (focusIdx + 1 == record->recordCount - 2)
							{
								Camera::g_unk50A0CC = 0;
							}
							else
							{
								int32_t deltaY = record->data[nextEyeIdx + 2].y - record->data[nextEyeIdx + 1].y;
								int32_t deltaZ = record->data[nextEyeIdx + 2].z - record->data[nextEyeIdx + 1].z;
								int32_t deltaX = record->data[nextEyeIdx + 2].x - record->data[nextEyeIdx + 1].x;

								Camera::g_unk50A0CC = (int32_t)sqrt((double)(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ));
							}
						}
					}

					// the script waits
					if (Camera::g_unk50A134 > 0)
					{
					LABEL_47:
						Camera::g_unk50A134 -= Renderer::g_frameDelta;

						if (Camera::g_unk50A134 <= 0)
						{
							Camera::g_unk50A134 = 0;
							Camera::g_unk50A294 = script + 1;
						}
					}
				}
				else
				{
					if (Renderer::g_enableCinematicBars >= 10)
						finishedBars = Renderer::g_enableCinematicBars - 9;

					Camera::g_inCutscene &= ~1;
					g_buzzActor.actorFlags &= ~Actor::ACTOR_LOCKED_MOVE;
					Renderer::g_enableCinematicBars = 0;
					Camera::g_cameraBlendTimer = 64;
					g_buzzActor.collisionFlags = 0;

					if (g_buzzActor.unkWord15 == 0)
						g_buzzActor.unkWord15 = 5;

					AudioManager::PlaySoundEffect(30, 0);

					if (finishedBars != 0)
						Collectables::Activate(finishedBars - 1, 0);
				}
			}
		}
	}
}

#include "Toy2/Camera.h"
#include "Toy2/Toy2.h"
#include "Toy2/Actor.h"
#include "Nu3D/Collision.h"
#include "Nu3D/Link.h"
#include "Renderer/Renderer.h"
#include "Numerics.h"

#include <string.h>

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

		// STUB: TOY2 0x00403640
		void SmoothToTarget(Toy2::Camera::ScriptedCameraState* cameraState) {}

		// STUB: TOY2 0x00403730
		void SnapBehindBuzz(Toy2::Camera::GameplayCameraState* cameraState) {}

		// STUB: TOY2 0x004038E0
		void VisorMode(Toy2::Camera::GameplayCameraState* state) {}

		// STUB: TOY2 0x004045E0
		void GameplayMode(Toy2::Camera::GameplayCameraState* state) {}

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
					g_activeCameraTransform.pos.x = g_scriptedCameraState.pos.x + ((g_cameraBlendTimer * (g_activeCameraTransform.pos.x - g_scriptedCameraState.pos.x)) >> 6);
					g_activeCameraTransform.pos.y = g_scriptedCameraState.pos.y + ((g_cameraBlendTimer * (g_activeCameraTransform.pos.y - g_scriptedCameraState.pos.y)) >> 6);
					g_activeCameraTransform.pos.z = g_scriptedCameraState.pos.z + ((g_cameraBlendTimer * (g_activeCameraTransform.pos.z - g_scriptedCameraState.pos.z)) >> 6);

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
					g_activeCameraTransform.pos.x = g_gameplayCameraState.pos.x + ((g_cameraBlendTimer * (g_activeCameraTransform.pos.x - g_gameplayCameraState.pos.x)) >> 6);
					g_activeCameraTransform.pos.y = g_gameplayCameraState.pos.y + ((g_cameraBlendTimer * (g_activeCameraTransform.pos.y - g_gameplayCameraState.pos.y)) >> 6);
					g_activeCameraTransform.pos.z = g_gameplayCameraState.pos.z + ((g_cameraBlendTimer * (g_activeCameraTransform.pos.z - g_gameplayCameraState.pos.z)) >> 6);

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

		// STUB: TOY2 0x00435F30
		int32_t HandleLedgeClimb(Toy2::Buzz::Toy2BuzzActor* buzzActor) { return 0; }

		// STUB: TOY2 0x00447BD0
		void CullActors(Nu3D::Camera::ActiveCameraTransform* cam) {}

		// STUB: TOY2 0x0048A4C0
		int32_t PrepareCameraCollisionCandidates(uint32_t* a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5) { return 0; }

		// STUB: TOY2 0x004A4BB0
		void UpdateGravityBoots(Toy2::Buzz::Toy2BuzzActor* buzzActor) {}

		// STUB: TOY2 0x004A4F80
		BOOL UpdateRocketBoots(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movement) { return 0; }

		// STUB: TOY2 0x00433F40
		int32_t CalculateMaxTurnAngle(int32_t directionButtonState) { return 0; }

		// STUB: TOY2 0x004037E0
		BOOL GetActorLockOnPoint(Vector3I* pos, Toy2::Actor::Toy2Actor* actor, int32_t flag) { return 0; }

		// STUB: TOY2 0x004A5870
		int32_t SweepSegmentVsActors(WorldPos* arg0, WorldPos* a2, int32_t a3) { return 0; }
	}
}

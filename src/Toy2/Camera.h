#pragma once

#include "Common.h"
#include "Nu3D/Camera.h"
#include "Numerics.h"
#include "Toy2/Buzz.h"
#include <windows.h>

namespace Toy2
{
	namespace Camera
	{
		struct GameplayCameraState
		{
			Vector3I pos;
			Vector3I targetPos;
			int32_t followY;
			int32_t smoothedY;
			Angles angles;
			int16_t roll;
			int16_t distance;
			int16_t orbitYaw;
			int16_t smoothedYaw;
			int16_t targetPitch;
			int16_t orbitPitch;
			int16_t unkShort6;
			int16_t collisionFlags;
		};

		struct ScriptedCameraState
		{
			Vector3I pos;
			Vector3I focusPos;
			Vector3I eyePos;
			Angles angles;
		};

		extern Nu3D::Camera::ActiveCameraTransform g_activeCameraTransform;
		extern GameplayCameraState g_gameplayCameraState;
		extern int32_t g_cameraState;

		void InitCutsceneCamera(Vector3I* focusPos, Vector3I* pos);
		void BeginScriptedCutsceneAtPoint(Vector3I* target, int32_t duration, int32_t orbitRadius);
		void BeginScriptedCutsceneOnBuzz(int32_t duration);
		void InitGameplayCamera(Toy2::Camera::GameplayCameraState* cameraState, Toy2::Buzz::Toy2BuzzActor* buzzActor);
		void SnapBehindBuzz(Toy2::Camera::GameplayCameraState* cameraState);
		void UpdateActiveTransform();
		int32_t CalculateMaxTurnAngle(int32_t directionButtonState);
		int32_t HandleLedgeClimb(Toy2::Buzz::Toy2BuzzActor* buzzActor);
		void CullActors(Nu3D::Camera::ActiveCameraTransform* cam);
		void UpdateGravityBoots(Toy2::Buzz::Toy2BuzzActor* buzzActor);
		BOOL UpdateRocketBoots(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movement);
		int32_t SweepSegmentVsActors(WorldPos* arg0, WorldPos* a2, int32_t a3);

		STATIC_ASSERT(sizeof(GameplayCameraState) == 0x34);
		STATIC_ASSERT(sizeof(ScriptedCameraState) == 0x28);
	}
}

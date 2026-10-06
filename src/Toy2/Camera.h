#pragma once

#include "Common.h"
#include "Numerics.h"
#include <windows.h>

struct WorldPos;


namespace Toy2
{
	namespace Actor
	{
		struct Toy2Actor;
	}
}


struct Vector3I;

namespace Toy2
{
	namespace Buzz
	{
		struct Toy2BuzzActor;
		struct MovementParams;
	}
}

namespace Nu3D
{
	namespace Camera
	{
		struct ActiveCameraTransform;
	}
}

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

		STATIC_ASSERT(sizeof(GameplayCameraState) == 0x34);

		struct ScriptedCameraState
		{
			Vector3I pos;
			Vector3I focusPos;
			Vector3I eyePos;
			Angles angles;
		};

		STATIC_ASSERT(sizeof(ScriptedCameraState) == 0x28);

		void InitCutsceneCamera(Vector3I* focusPos, Vector3I* pos);
		void BeginScriptedCutsceneAtPoint(Vector3I* target, int32_t duration, int32_t orbitRadius);
		void BeginScriptedCutsceneOnBuzz(int32_t duration);
		void InitGameplayCamera(Toy2::Camera::GameplayCameraState* cameraState, Toy2::Buzz::Toy2BuzzActor* buzzActor);
		void SmoothToTarget(Toy2::Camera::ScriptedCameraState* cameraState);
		void SnapBehindBuzz(Toy2::Camera::GameplayCameraState* cameraState);
		void VisorMode(Toy2::Camera::GameplayCameraState* state);
		void GameplayMode(Toy2::Camera::GameplayCameraState* state);
		void UpdateActiveTransform();
		int32_t HandleLedgeClimb(Toy2::Buzz::Toy2BuzzActor* buzzActor);
		void CullActors(Nu3D::Camera::ActiveCameraTransform* cam);
		int32_t PrepareCameraCollisionCandidates(uint32_t* a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5);
		void UpdateGravityBoots(Toy2::Buzz::Toy2BuzzActor* buzzActor);
		BOOL UpdateRocketBoots(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movement);

		int32_t CalculateMaxTurnAngle(int32_t directionButtonState);

		BOOL GetActorLockOnPoint(Vector3I* pos, Toy2::Actor::Toy2Actor* actor, int32_t flag);

		int32_t SweepSegmentVsActors(WorldPos* arg0, WorldPos* a2, int32_t a3);
	}
}

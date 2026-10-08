#pragma once

#include "Common.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Particles.h"
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
		extern int16_t g_inCutscene;
		extern int32_t g_cameraState;
		extern int32_t g_freezeBuzz;
		extern int32_t g_cutsceneFrameTimer;
		extern int32_t g_cutsceneTimer;
		extern int32_t g_cameraBlendTimer;
		extern int32_t g_unk50A510;
		extern int32_t g_lockOnActorIdx;
		extern int32_t g_unk50A540;
		extern int32_t g_unk52AD98;
		extern ScriptedCameraState g_scriptedCameraState;
		extern Nu3D::Particles::ParticleInstance* g_unk50A538;
		extern Nu3D::Particles::ParticleInstance* g_unk50A4DC;
		extern Vector3I g_cutsceneFocusPos;
		extern Vector3I g_cutsceneEyePos;
		extern int32_t g_unk50A4D0;
		extern int32_t g_unk50A1F0;
		extern int32_t g_unk50A0C8;
		extern int32_t* g_unk50A294;
		extern int32_t g_unk50A134;
		extern int32_t g_unk50A53C;
		extern int32_t g_unk50A144;
		extern int32_t g_unk50A0CC;
		extern int32_t g_unk50A514;
		extern int32_t g_unk50A4BC;

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

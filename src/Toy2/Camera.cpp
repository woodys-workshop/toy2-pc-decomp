#include "Toy2/Camera.h"

namespace Toy2
{
	namespace Camera
	{
		// STUB: TOY2 0x00402030
		void InitCutsceneCamera(Vector3I* focusPos, Vector3I* pos) {}

		// STUB: TOY2 0x004020F0
		void BeginScriptedCutsceneAtPoint(Vector3I* target, int32_t duration, int32_t orbitRadius) {}

		// STUB: TOY2 0x00402290
		void BeginScriptedCutsceneOnBuzz(int32_t duration) {}

		// STUB: TOY2 0x00403450
		void InitGameplayCamera(Toy2::Camera::GameplayCameraState* cameraState, Toy2::Buzz::Toy2BuzzActor* buzzActor) {}

		// STUB: TOY2 0x00403640
		void SmoothToTarget(Toy2::Camera::ScriptedCameraState* cameraState) {}

		// STUB: TOY2 0x00403730
		void SnapBehindBuzz(Toy2::Camera::GameplayCameraState* cameraState) {}

		// STUB: TOY2 0x004038E0
		void VisorMode(Toy2::Camera::GameplayCameraState* state) {}

		// STUB: TOY2 0x004045E0
		void GameplayMode(Toy2::Camera::GameplayCameraState* state) {}

		// STUB: TOY2 0x00405860
		void UpdateActiveTransform() {}

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

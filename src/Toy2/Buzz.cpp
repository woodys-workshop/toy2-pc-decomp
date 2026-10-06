#include "Toy2/Buzz.h"
#include "Toy2/Actor.h"

namespace Toy2
{
	namespace Buzz
	{
		// STUB: TOY2 0x00414110
		void Respawn() {}

		// STUB: TOY2 0x004011D0
		void UpdateAnimationState() {}

		// STUB: TOY2 0x004071E0
		void HandleDamage(int16_t hitAngle, uint8_t hitType) {}

		// STUB: TOY2 0x004100F0
		void CheckParticleCollisions() {}

		// STUB: TOY2 0x00433D50
		void Init(Toy2::Buzz::Toy2BuzzActor* buzzActor, int32_t levelIndex) {}

		// STUB: TOY2 0x004340D0
		void UpdateJumpAndGravity(Toy2::Buzz::Toy2BuzzActor* buzzActor, int32_t initialJumpVelocity, int32_t forceJump) {}

		// STUB: TOY2 0x004343D0
		int32_t UpdateHorizontalMovement(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movementParams, int32_t flag) { return 0; }

		// STUB: TOY2 0x00434550
		void ResolveFooting(Toy2::Buzz::Toy2BuzzActor* buzzActor) {}

		// STUB: TOY2 0x004346C0
		int32_t UpdateFacing(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movementParams) { return 0; }

		// STUB: TOY2 0x00434990
		void TickGunFire(PosAndAngles* muzzlePos) {}

		// STUB: TOY2 0x00434D20
		int32_t TickGroundSlam(Toy2::Actor::Toy2Actor* actor) { return 0; }

		// STUB: TOY2 0x00434EB0
		void TickSpinHover(Toy2::Buzz::Toy2BuzzActor* buzz) {}

		// STUB: TOY2 0x00435100
		int32_t HandleSwing(Toy2::Buzz::Toy2BuzzActor* buzz) { return 0; }

		// STUB: TOY2 0x004354E0
		int32_t HandlePoleClimb(Toy2::Buzz::Toy2BuzzActor* buzz) { return 0; }

		// STUB: TOY2 0x004359D0
		int32_t HandleZipline(Toy2::Buzz::Toy2BuzzActor* actor) { return 0; }

		// STUB: TOY2 0x00436220
		void HandleGameplay(Toy2::Buzz::Toy2BuzzActor* buzzActor) {}

		// STUB: TOY2 0x00484380
		void ResolveCollisions(Toy2::Buzz::Toy2BuzzActor* buzzActor, Vector3I* velocity, uint8_t* flags, int32_t index, int32_t halfStep) {}

		// STUB: TOY2 0x004855F0
		void HandleCollisions(Toy2::Buzz::Toy2BuzzActor* buzzActor, Vector3I* velocity, int16_t* flags, int32_t index) {}

		// STUB: TOY2 0x004863A0
		int32_t UpdateFloorHeight(Toy2::Buzz::Toy2BuzzActor* buzzActor) { return 0; }

		// STUB: TOY2 0x004A28F0
		void UpdateRespawnAnchor() {}

		// STUB: TOY2 0x004A2D80
		void UpdateContactEffects() {}

		// STUB: TOY2 0x004A4910
		void RefreshDiscAmm() {}

		// STUB: TOY2 0x004A4960
		void FireDiscLauncher(int16_t pitch) {}

		// STUB: TOY2 0x004A4B90
		void ResetGravityBoots() {}

		// STUB: TOY2 0x004A4D60
		void ActivateRocketBoots(Toy2::Levels::Type63* a1) {}

		// STUB: TOY2 0x004A4E60
		void DeactivateRocketBoots() {}

		// STUB: TOY2 0x004A5170
		void TickCosmicShield() {}

		// STUB: TOY2 0x004A5340
		void CancelGrapple() {}

		// STUB: TOY2 0x004A5370
		uint32_t FireGrapple() { return 0; }

		// STUB: TOY2 0x004A5540
		void TickGrapple() {}

		// STUB: TOY2 0x004A5C40
		void TickBeamShots() {}

		// STUB: TOY2 0x004A62A0
		void TickGadgets() {}
	}
}
#include "Toy2/Buzz.h"
#include "Toy2/Actor.h"
#include "Toy2/MoveableObject.h"
#include "Toy2/Toy2.h"
#include "InputManager.h"

#include <string.h>

namespace Toy2
{
	namespace Buzz
	{
		// GLOBAL: TOY2 0x0053C660
		int32_t g_unk53C660;

		// GLOBAL: TOY2 0x0053C838
		int32_t g_unk53C838;

		// GLOBAL: TOY2 0x0053C668
		int32_t g_unk53C668;

		// GLOBAL: TOY2 0x0053C658
		int32_t g_unk53C658;

		// GLOBAL: TOY2 0x0053C5E4
		int32_t g_unk53C5E4;

		// GLOBAL: TOY2 0x0053C5D4
		int32_t g_buzzSlipTimer;

		// GLOBAL: TOY2 0x0053C618
		int32_t g_unk53C618;

		// GLOBAL: TOY2 0x0053C600
		int32_t g_unk53C600;

		// GLOBAL: TOY2 0x0053C610
		int32_t g_unk53C610;

		// GLOBAL: TOY2 0x0053C83C
		int32_t g_unk53C83C;

		// GLOBAL: TOY2 0x0053C650
		int32_t g_unk53C650;

		// GLOBAL: TOY2 0x0053C64C
		int32_t g_groundSlamTicker;

		// GLOBAL: TOY2 0x0053C828
		int32_t g_buzzActionState;

		// GLOBAL: TOY2 0x0053C620
		int32_t g_fireChargeTimer;

		// GLOBAL: TOY2 0x0053C840
		int32_t g_fireHoldTimer;

		// GLOBAL: TOY2 0x0053C648
		MoveableObject::MoveableObject* g_curMoveableObject;

		// GLOBAL: TOY2 0x0053C624
		int32_t g_unk53C624;

		// GLOBAL: TOY2 0x0053C66C
		int32_t g_unk53C66C;

		// GLOBAL: TOY2 0x0053C834
		int32_t g_unk53C834;

		// GLOBAL: TOY2 0x0053C830
		int32_t g_unk53C830;

		// GLOBAL: TOY2 0x0052B81C
		int32_t g_unk52B81C;

		// GLOBAL: TOY2 0x005281A4
		int32_t g_unk5281A4;

		// GLOBAL: TOY2 0x0053C634
		int32_t g_unk53C634;

		// GLOBAL: TOY2 0x0053C60C
		int32_t g_unk53C60C;

		// GLOBAL: TOY2 0x0053C810
		int32_t g_unk53C810;

		// GLOBAL: TOY2 0x0053C628
		int32_t g_underwaterYLevel;

		// GLOBAL: TOY2 0x0053C5E8
		int32_t g_currentFluidType;

		// GLOBAL: TOY2 0x0053C62C
		int32_t g_unk53C62C;

		// GLOBAL: TOY2 0x0053C824
		int32_t g_greenGunCharge;

		// GLOBAL: TOY2 0x0053C608
		int32_t g_unk53C608;

		// GLOBAL: TOY2 0x0053C818
		int32_t g_unk53C818;

		// GLOBAL: TOY2 0x0050A098
		int32_t g_unk50A098;

		// GLOBAL: TOY2 0x0053C678
		int32_t g_unk53C678;

		// GLOBAL: TOY2 0x0053C5D0
		int32_t g_unk53C5D0;

		// GLOBAL: TOY2 0x0053C674
		int32_t g_unk53C674;

		// GLOBAL: TOY2 0x0053C61C
		int32_t g_unk53C61C;

		// GLOBAL: TOY2 0x0053C664
		int32_t g_unk53C664;

		// GLOBAL: TOY2 0x0053C5DC
		int32_t g_unk53C5DC;

		// STUB: TOY2 0x00414110
		void Respawn() {}

		// STUB: TOY2 0x004011D0
		void UpdateAnimationState() {}

		// STUB: TOY2 0x004071E0
		void HandleDamage(int16_t hitAngle, uint8_t hitType) {}

		// STUB: TOY2 0x004100F0
		void CheckParticleCollisions() {}

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

		// FUNCTION: TOY2 0x00433D50
		void Init(Toy2::Buzz::Toy2BuzzActor* buzzActor, int32_t levelIndex)
		{
			int16_t lives = buzzActor->lives;
			int16_t health = buzzActor->health;

			memset(buzzActor, 0, sizeof(Toy2BuzzActor));

			SpawnLocation* spawnLocation = &g_spawnLocations[levelIndex];

			buzzActor->posAngles.pos.x = spawnLocation->x;
			buzzActor->posAngles.pos.y = spawnLocation->y;
			buzzActor->posAngles.pos.z = spawnLocation->z;

			int32_t floorY = UpdateFloorHeight(buzzActor);

			floorY -= 256;

			buzzActor->motionTargetPos.x = buzzActor->posAngles.pos.x;
			buzzActor->posAngles.pos.y = floorY;
			buzzActor->motionTargetPos.y = floorY;
			buzzActor->motionTargetPos.z = buzzActor->posAngles.pos.z;

			buzzActor->respawnPos.y = floorY;
			buzzActor->respawnPos.x = buzzActor->posAngles.pos.x;
			buzzActor->respawnYawAngle = (int16_t)spawnLocation->rotation;

			buzzActor->posAngles.angles.yaw = (uint16_t)spawnLocation->rotation;
			buzzActor->facingAngle = (int16_t)spawnLocation->rotation;
			buzzActor->surfaceClampY = 0x80000000;
			buzzActor->lives = lives;
			buzzActor->health = health;
			buzzActor->respawnPos.z = buzzActor->posAngles.pos.z;
			buzzActor->isOnWalkableFloor = 0;
			buzzActor->primaryAnimIdx = -1;
			buzzActor->secondaryAnimIdx = -1;
			buzzActor->visibilityDistance = 1280;

			g_unk53C660 = 0;
			g_unk53C838 = 0;
			g_unk53C668 = 0;
			g_unk53C658 = 0;
			g_unk53C5E4 = 0;
			g_buzzSlipTimer = 0;
			g_unk53C618 = 0;
			g_unk53C600 = 0;
			g_unk53C610 = 0;
			g_unk53C83C = 0;
			g_unk53C650 = 0;
			g_groundSlamTicker = 0;
			g_buzzActionState = 0;
			g_fireChargeTimer = 0;
			g_fireHoldTimer = 0;
			g_curMoveableObject = 0;
			g_unk53C624 = 0;
			g_unk53C66C = 0;
			g_unk53C834 = 0;
			g_unk53C830 = 0;
			g_unk52B81C = -1;
			g_unk5281A4 = -1;
			g_unk53C634 = 0;
			g_unk53C60C = 0;
			g_unk53C810 = 0;
			g_underwaterYLevel = 0;
			g_currentFluidType = 0;
			g_unk53C62C = 0;
			g_greenGunCharge = 0;
			g_unk53C608 = 0;
			g_unk53C818 = 0;
			InputManager::g_directionInputState2Frames = 0;
			InputManager::g_directionInputState3Frames = 0;
			g_unk50A098 = 0;
			g_unk53C678 = 0;
			g_unk53C5D0 = 0;
			g_unk53C674 = buzzActor->posAngles.pos.y;
			g_unk53C61C = 0;
			g_unk53C664 = 0;
			g_unk53C5DC = 0;
		}

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
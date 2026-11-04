#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Toy2/Levels.h"

namespace Toy2
{
	struct BuzzActor
	{
		// common variables
		Transform transform;
		int16_t rollAngle;
		int16_t primaryAnimIdx;
		int16_t creatureId;
		int16_t secondaryAnimIdx;
		int32_t primaryAnimTime;
		int32_t secondaryAnimTime;
		int32_t surfaceClampY;

		// --- buzz actor ---

		// lighting
		Vector3I16 lightDirection;
		int16_t unusedLightParam;
		Vector3I16 scale;
		int16_t scaleControl;
		RGBA lightColor;

		// bounding / flags
		Vector3I16 boundingOffset;
		int16_t boundingSphereRadius;
		int16_t actorFlags;
		int16_t visibilityDistance;

		// extra angles
		int32_t unusedVar1;
		int16_t facingAngle;
		int16_t unusedVar2;

		// respawn / target
		Vector3I respawnPos;
		int32_t respawnYawAngle;
		Vector3I motionTargetPos;

		// velocity
		int32_t velX;
		int32_t gravityVel;
		int32_t velForward;
		int32_t localForwardSpeed;
		int32_t localStrafeSpeed;
		
		// animation position
		int32_t currentAnimTime;
		int32_t animTrackPos;

		// y positioning
		int32_t floorYPos;
		int32_t isOnWalkableFloor;
		int16_t jumpState;
		int16_t collisionFlags; // TODO: correctly type/name

		// animation states
		int16_t animState;
		int16_t prevAnimState;

		// buzz timers / health tracking
		int16_t stunTimer;
		int16_t health;
		int16_t inhibitMovementTimer;
		int16_t lives;
		int16_t supportGraceTimer;
		int16_t coinsCollected;
	};

	namespace Buzz
	{
		struct MovementParams
		{
			int32_t groundAccel;
			int32_t airAccel;
			int32_t turnSpeed;
			int32_t initialJumpVelocity;
			int32_t maxSpeed;
			int32_t maxAirSpeed;
			int32_t maxTurnRate;
		};

		void Respawn();
		void UpdateAnimationState();
		void HandleDamage(int32_t hitAngle, int32_t hitType);
		void CheckParticleCollisions();
		void Init(Toy2::BuzzActor* buzzActor, int32_t levelIndex);
		void HandleGameplay(Toy2::BuzzActor* buzzActor);
		void UpdateRespawnAnchor();
		void UpdateContactEffects();
		void ResetGravityBoots();
		void ActivateRocketBoots(Toy2::Levels::Type63* a1);
		void DeactivateRocketBoots();
		void CancelGrapple();
		void TickGadgets();

		extern int32_t g_fireChargeTimer;
		extern int32_t g_unk53C660;
		extern int32_t g_unk53C838;
		extern int32_t g_unk53C668;
		extern int32_t g_unk53C5E4;
		extern int32_t g_unk52B81C;
		extern int32_t g_unk53C618;
		extern int32_t g_buzzSlipTimer;
		extern int32_t g_unk53C61C;
		extern int32_t g_unk53C83C;
		extern int32_t g_unk53C650;
		extern int32_t g_groundSlamTicker;
		extern int32_t g_fireHoldTimer;
		extern int32_t g_unk53C66C;
		extern int32_t g_unk53C608;
		extern int32_t g_unk53C818;
		extern int32_t g_buzzActionState;
		extern int32_t g_curMoveableObject;
		extern int32_t g_unk53C810;
		extern int32_t g_unk53C624;
		extern int32_t g_unk53C5DC;
		extern int32_t g_unk53C634;
		extern int32_t g_unk50A098;
		extern int32_t g_greenGunCharge;
		extern int32_t g_unk53C658;
		extern int32_t g_underwaterYLevel;
		extern int32_t g_unk830D30;
		extern int32_t g_unk53C60C;
		extern int32_t g_currentFluidType;

		STATIC_ASSERT(sizeof(BuzzActor) == 0xA0);
		STATIC_ASSERT(sizeof(MovementParams) == 0x1C);
	}
}
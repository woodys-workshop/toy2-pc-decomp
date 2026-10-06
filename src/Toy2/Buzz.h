#pragma once

#include "Common.h"
#include "Numerics.h"

struct PosAndAngles;
struct Vector3I;

namespace Toy2
{
	namespace Actor
	{
		struct Toy2Actor;
	}
}

namespace Toy2
{
	namespace Levels
	{
		struct Type63;
	}
}


namespace Toy2
{
	namespace Buzz
	{
		struct Toy2BuzzActor
		{
			PosAndAngles posAngles;
			int16_t rollAngle;
			int16_t primaryAnimIdx;
			int16_t creatureId;
			int16_t secondaryAnimIdx;
			int32_t unkVar7;
			int32_t unkVar8;
			int32_t surfaceClampY;
			Vector3I16 lightDirection;
			int16_t unkVar51;
			int32_t unkVar12;
			int32_t unkVar13;
			RGBA color;
			int16_t unkWord1;
			int16_t unkWord2;
			int16_t unkWord3;
			int16_t unkWord4;
			int16_t actorFlags;
			int16_t visibilityDistance;
			int32_t unkVar22;
			int16_t facingAngle;
			int16_t unkVar23;
			Vector3I respawnPos;
			int32_t respawnYawAngle;
			Vector3I motionTargetPos;
			int32_t velX;
			int32_t gravityVel;
			int32_t velForward;
			int32_t unkVar30;
			int32_t unkVar31;
			int16_t unkWord13;
			int16_t actorHealth;
			int32_t unkVar33;
			int32_t floorYPos;
			int32_t isOnWalkableFloor;
			int16_t unkWord15;
			int16_t collisionFlags;
			int16_t unkVar37;
			int16_t unkVar38;
			int16_t stunTimer;
			int16_t health;
			int16_t inhibitMovementTimer;
			int16_t lives;
			int16_t unkShort40;
			int16_t coinsCollected;
		};

		void Respawn();

		STATIC_ASSERT(sizeof(Toy2BuzzActor) == 0xA0);

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

		STATIC_ASSERT(sizeof(MovementParams) == 0x1C);

		void UpdateAnimationState();
		void HandleDamage(int16_t hitAngle, uint8_t hitType);
		void CheckParticleCollisions();
		void Init(Toy2::Buzz::Toy2BuzzActor* buzzActor, int32_t levelIndex);
		void UpdateJumpAndGravity(Toy2::Buzz::Toy2BuzzActor* buzzActor, int32_t initialJumpVelocity, int32_t forceJump);
		int32_t UpdateHorizontalMovement(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movementParams, int32_t flag);
		void ResolveFooting(Toy2::Buzz::Toy2BuzzActor* buzzActor);
		int32_t UpdateFacing(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movementParams);
		void TickGunFire(PosAndAngles* muzzlePos);
		int32_t TickGroundSlam(Toy2::Actor::Toy2Actor* actor);
		void TickSpinHover(Toy2::Buzz::Toy2BuzzActor* buzz);
		int32_t HandleSwing(Toy2::Buzz::Toy2BuzzActor* buzz);
		int32_t HandlePoleClimb(Toy2::Buzz::Toy2BuzzActor* buzz);
		int32_t HandleZipline(Toy2::Buzz::Toy2BuzzActor* actor);
		void HandleGameplay(Toy2::Buzz::Toy2BuzzActor* buzzActor);
		void ResolveCollisions(Toy2::Buzz::Toy2BuzzActor* buzzActor, Vector3I* velocity, uint8_t* flags, int32_t index, int32_t halfStep);
		void HandleCollisions(Toy2::Buzz::Toy2BuzzActor* buzzActor, Vector3I* velocity, int16_t* flags, int32_t index);
		int32_t UpdateFloorHeight(Toy2::Buzz::Toy2BuzzActor* buzzActor);
		void UpdateRespawnAnchor();
		void UpdateContactEffects();
		void RefreshDiscAmm();
		void FireDiscLauncher(int16_t pitch);
		void ResetGravityBoots();
		void ActivateRocketBoots(Toy2::Levels::Type63* a1);
		void DeactivateRocketBoots();
		void TickCosmicShield();
		void CancelGrapple();
		uint32_t FireGrapple();
		void TickGrapple();
		void TickBeamShots();
		void TickGadgets();
	}
}
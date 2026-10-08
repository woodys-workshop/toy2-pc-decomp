#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Toy2/Levels.h"

namespace Toy2
{
	namespace MoveableObject
	{
		struct MoveableObject;
	}

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
		void HandleDamage(int16_t hitAngle, uint8_t hitType);
		void CheckParticleCollisions();
		void Init(Toy2::Buzz::Toy2BuzzActor* buzzActor, int32_t levelIndex);
		void HandleGameplay(Toy2::Buzz::Toy2BuzzActor* buzzActor);
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
		extern int32_t g_unk53C618;
		extern int32_t g_buzzSlipTimer;
		extern int32_t g_unk53C83C;
		extern int32_t g_unk53C650;
		extern int32_t g_groundSlamTicker;
		extern int32_t g_fireHoldTimer;
		extern int32_t g_unk53C66C;
		extern int32_t g_unk53C608;
		extern int32_t g_unk53C818;
		extern int32_t g_buzzActionState;
		extern MoveableObject::MoveableObject* g_curMoveableObject;
		extern int32_t g_unk53C634;
		extern int32_t g_unk50A098;
		extern int32_t g_underwaterYLevel;
		extern int32_t g_currentFluidType;

		STATIC_ASSERT(sizeof(Toy2BuzzActor) == 0xA0);
		STATIC_ASSERT(sizeof(MovementParams) == 0x1C);
	}
}
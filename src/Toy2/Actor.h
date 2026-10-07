#pragma once

#include "Common.h"
#include "RawLoader.h"
#include "Numerics.h"
#include <windows.h>

namespace Toy2
{
	namespace Actor
	{
		enum ActorFlags
		{
			ACTOR_CUTSCENE_MOVE = 0x1,
			ACTOR_VISIBLE_ACTIVE = 0x2,
			ACTOR_LOCKED_MOVE = 0x4,
			ACTOR_GROUNDED = 0x8,
			ACTOR_FLOOR_SNAP = 0x10,
			ACTOR_STUNNED = 0x20,
			ACTOR_OOB_SPECIAL_MOVE = 0x40,
			ACTOR_DEATH_DAMAGE = 0x80,
			ACTOR_AIRBORNE_SPECIAL = 0x100,
			ACTOR_PLAYER_TRIGGER = 0x200,
			ACTOR_VEL_CLAMP_OVERRIDE = 0x400,
			ACTOR_NO_DESPAWN = 0x800,
			ACTOR_UNUSED_1000 = 0x1000,
			ACTOR_FORCE_REMOVE = 0x2000,
			ACTOR_CAMERA_HOVER = 0x4000,
			ACTOR_UNKNOWN_8000 = 0x8000,
		};

		struct Toy2Actor
		{
			struct ActorBehaviourContext
			{
				Toy2Actor* actor;
				int16_t updateFlags;
				int16_t targetFlags;
				int16_t localStrafeSpeed;
				int16_t localForwardSpeed;
			};

			typedef void (*ActorBehaviourFn)(ActorBehaviourContext* ctx);

			Vector3I pos;
			int16_t pitchAngle;
			int16_t yawAngle;
			int16_t rollAngle;
			int16_t primaryAnimIdx;
			int16_t creatureId;
			int16_t secondaryAnimIdx;
			int32_t unkVar7;
			int32_t unkVar8;
			int32_t unkVar9;
			RGB16 actorTint;
			int16_t unkVar10;
			int16_t unkVar12;
			int16_t unkVar12_;
			int16_t unkVar13;
			int16_t unkVar13_;
			int32_t useTint;
			Vector3I16 boundingOffset;
			int16_t boundingSphereRadius;
			int16_t actorFlags;
			int16_t visibilityDistance;
			int32_t velX;
			int32_t gravityVel;
			int32_t velForward;
			Vector3I boundary;
			Vector3I motionTargetPos;
			int32_t targetYaw;
			int32_t areaIndex;
			int16_t unkVar29;
			int16_t unkVar29_;
			int32_t unkVar30;
			int16_t unkShort1;
			int16_t unkShort2;
			int16_t damageCooldownTimer;
			int16_t actorPhase;
			uint16_t* movementData;
			int32_t unkVar34;
			int16_t unkShort3;
			int16_t unkShort4;
			int16_t unkWord15;
			int16_t unkWord16;
			int32_t lastValidYPosition;
			ActorBehaviourFn actorBehaviour;
			RawLoader::CreatureListRam* creatureRam;
		};

		extern Toy2Actor* g_activeActors[65];
		extern Toy2Actor* g_activeActorArray[64];

		void GetCreatureList(uint8_t* creatureIdList);
		void UpdatePrimaryAnimation(Toy2::Actor::Toy2Actor* actor);
		void SetAnimation(Toy2::Actor::Toy2Actor* actor, int16_t state, int32_t animIdx);
		void Kill(Toy2::Actor::Toy2Actor* actor, uint8_t phase);
		void UpdateAIMovement(Toy2::Actor::Toy2Actor* actor);
		void HandleDamage(Toy2::Actor::Toy2Actor* actor, int32_t angle, int32_t hitType);
		void ResolveBoneAttachmentPos(Vector3I* localOffset, Toy2::Actor::Toy2Actor* actor, int32_t boneIndex);
		BOOL IsInsideBounds(Toy2::Actor::Toy2Actor* actor, int32_t minX, int32_t maxX, int32_t minZ, int32_t maxZ);
		void CollectQuestReward(int32_t actorIdx, int32_t recordIdx, int32_t camAngle, int32_t camAngleValue, int32_t pickupSlotIdx);
		void RotatingHint(int32_t actorIdx, int32_t recordIdx, char** hintTable);
		void ItemReturnReward(
			int32_t actorIdx,
			int32_t recordIdx,
			char* textInProgress,
			char* textFoundPiece,
			char* textComplete,
			int32_t camAngleValue,
			int32_t camAngle);
		void PlayPeriodicHintSound(int32_t actorId, int32_t soundIdx);
		void PopulateActiveActors();
		void SetNodeAngle(Toy2::Actor::Toy2Actor* actor, int32_t index, int32_t x, int32_t y, int32_t z);

		STATIC_ASSERT(sizeof(Toy2Actor) == 0x9C);
		STATIC_ASSERT(sizeof(Toy2Actor::ActorBehaviourContext) == 0xC);
	}
}
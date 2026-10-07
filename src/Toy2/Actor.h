#pragma once

#include "Common.h"
#include "RawLoader.h"
#include "Numerics.h"
#include <windows.h>

namespace Toy2
{
	namespace Actor
	{
		enum CreatureId
		{
			CREATURE_BUZZ = 0x0,
			CREATURE_WOODY = 0x1,
			CREATURE_BUZZINT = 0x2,
			CREATURE_ZURG1 = 0x3,
			CREATURE_ZURG3 = 0x4,
			CREATURE_TINMAN = 0x5,
			CREATURE_SHEEP = 0x6,
			CREATURE_BOPEEP = 0x7,
			CREATURE_RC = 0x8,
			CREATURE_MRPOT = 0x9,
			CREATURE_HAMM = 0xA,
			CREATURE_COTBIT = 0xB,
			CREATURE_2_LAWN = 0xC,
			CREATURE_ARMY = 0xD,
			CREATURE_2_ZGCAR = 0xE,
			CREATURE_2_ZKITE = 0xF,
			CREATURE_2_SLIME = 0x10,
			CREATURE_2_PAINT = 0x11,
			CREATURE_2_FTYKE = 0x12,
			CREATURE_2_LTYKE = 0x13,
			CREATURE_2_ZPOD = 0x14,
			CREATURE_2_SHINY = 0x15,
			CREATURE_2_DRILL = 0x16,
			CREATURE_2_MOUSE = 0x17,
			CREATURE_3_BPLANE = 0x18,
			CREATURE_3_BOX = 0x19,
			CREATURE_3_DINO = 0x1A,
			CREATURE_2_ZBOAT = 0x1B,
			CREATURE_3_CHICK = 0x1C,
			CREATURE_3_HOOLA = 0x1D,
			CREATURE_3_LOCK = 0x1E,
			CREATURE_3_GUNSP = 0x1F,
			CREATURE_3_CLOWN = 0x20,
			CREATURE_3_RATTLE = 0x21,
			CREATURE_3_MUM = 0x22,
			CREATURE_3_ROOST = 0x23,
			CREATURE_4_MOTHER = 0x24,
			CREATURE_4_FSAUCE = 0x25,
			CREATURE_4_MARTIAN = 0x26,
			CREATURE_4_JESSIE = 0x27,
			CREATURE_6_RABID = 0x28,
			CREATURE_3_BUZZARD = 0x29,
			CREATURE_4_BULLSEYE = 0x2A,
			CREATURE_4_DUCKS = 0x2B,
			CREATURE_4_SLINKY = 0x2C,
			CREATURE_4_GUNSL = 0x2D,
			CREATURE_4_FATBLOKE = 0x2E,
			CREATURE_5_BBUGGY = 0x2F,
			CREATURE_5_BI = 0x30,
			CREATURE_4_ROCKY = 0x31,
			CREATURE_4_GEORGE = 0x32,
			CREATURE_4_MILDRED = 0x33,
			CREATURE_4_KAREN = 0x34,
			CREATURE_4_PILOT = 0x35,
			CREATURE_5_ZPOD = 0x36,
			CREATURE_5_BUB = 0x37,
			CREATURE_5_BUZL = 0x38,
			CREATURE_5_ZEMP = 0x39,
			CREATURE_5_SMITH = 0x3A,
			CREATURE_5_LUGGAGE = 0x3B,
			CREATURE_6_LUGMAN = 0x3C,
			CREATURE_6_PROSP = 0x3D,
			CREATURE_6_REX = 0x3E,
			CREATURE_INVALID = 0xFF,
		};

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

		enum ActorUpdateFlags
		{
			ACTOR_UPDATE_MOVING = 0x1,
			ACTOR_UPDATE_GROUNDED = 0x2,
			ACTOR_UPDATE_ANIM_TICK = 0x4,
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
		extern uint16_t* g_movementData[44];
		extern uint8_t* g_unk4E0588[26];
		extern int32_t g_unk50A54C;

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
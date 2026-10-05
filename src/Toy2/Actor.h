#pragma once

#include "Common.h"
#include "RawLoader.h"

namespace Toy2
{
	namespace Actor
	{
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

		STATIC_ASSERT(sizeof(Toy2Actor) == 0x9C);
		STATIC_ASSERT(sizeof(Toy2Actor::ActorBehaviourContext) == 0xC);
	}
}
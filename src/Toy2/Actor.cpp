#include "Toy2/Actor.h"

namespace Toy2
{
	namespace Actor
	{
		// GLOBAL: TOY2 0x0052F1D0
		Toy2Actor* g_activeActors[65];

		// GLOBAL: TOY2 0x00529D48
		Toy2Actor* g_activeActorArray[64];

		// STUB: TOY2 0x00414A80
		void GetCreatureList(uint8_t* creatureIdList) {}

		// STUB: TOY2 0x004019D0
		void UpdatePrimaryAnimation(Toy2::Actor::Toy2Actor* actor) {}

		// STUB: TOY2 0x00405C80
		void StepCreatureAnimFrame(Toy2::Actor::Toy2Actor* actor) {}

		// STUB: TOY2 0x00405CF0
		void SetAnimation(Toy2::Actor::Toy2Actor* actor, int16_t state, int32_t animIdx) {}

		// STUB: TOY2 0x00405D20
		void Kill(Toy2::Actor::Toy2Actor* actor, uint8_t phase) {}

		// STUB: TOY2 0x00407150
		void InitCreatureRam() {}

		// STUB: TOY2 0x004076F0
		void UpdateAIMovement(Toy2::Actor::Toy2Actor* actor) {}

		// STUB: TOY2 0x00408A60
		void HandleDamage(Toy2::Actor::Toy2Actor* actor, int32_t angle, int32_t hitType) {}

		// STUB: TOY2 0x004104D0
		void HitType1Particles(int32_t a1, int32_t a2, int32_t a3) {}

		// STUB: TOY2 0x0043C1C0
		void ResolveBoneAttachmentPos(Vector3I* localOffset, Toy2::Actor::Toy2Actor* actor, int32_t boneIndex) {}

		// STUB: TOY2 0x0049F460
		BOOL IsInsideBounds(Toy2::Actor::Toy2Actor* actor, int32_t minX, int32_t maxX, int32_t minZ, int32_t maxZ) { return 0; }

		// STUB: TOY2 0x004A1CE0
		void CollectQuestReward(int32_t actorIdx, int32_t recordIdx, int32_t camAngle, int32_t camAngleValue, int32_t pickupSlotIdx) {}

		// STUB: TOY2 0x004A1E60
		void RotatingHint(int32_t actorIdx, int32_t recordIdx, char** hintTable) {}

		// STUB: TOY2 0x004A2480
		void ItemReturnReward(
			int32_t actorIdx,
			int32_t recordIdx,
			char* textInProgress,
			char* textFoundPiece,
			char* textComplete,
			int32_t camAngleValue,
			int32_t camAngle)
		{}

		// STUB: TOY2 0x004A26F0
		void PlayPeriodicHintSound(int32_t actorId, int32_t soundIdx) {}

		// STUB: TOY2 0x004A28B0
		void PopulateActiveActors() {}

		// STUB: TOY2 0x004CDBB0
		void SetNodeAngle(Toy2::Actor::Toy2Actor* actor, int32_t index, int32_t x, int32_t y, int32_t z) {}

		// STUB: TOY2 0x004CDBF0
		int32_t FindInActorList(Toy2::Actor::Toy2Actor* actor) { return 0; }
	}
}
#pragma once

#include "Common.h"
#include "Toy2/Levels.h"

namespace Toy2
{
	namespace Collectables
	{
		struct TokenTableEntry
		{
			int32_t linkId;
			int32_t recordType;
			const char* text;
			int32_t yaw;
		};

		// the y position of a hidden pickup and the one it is reset to when it shows up
		struct HiddenCollectible
		{
			int32_t* yPtr;
			int32_t savedY;
		};

		struct PickupSlot
		{
			int32_t linkId;
			int32_t state;
			int32_t* yPosPtr;
			int32_t collectTimer;
		};

		struct PickupTable
		{
			uint16_t recordCount;
			uint16_t recordType;
			Toy2::Levels::Type63 data[112];
		};

		extern PickupSlot g_pickupSlots[5];

		void BuildPickupTable();
		int32_t* LoadTokenTable(int32_t* a1);
		void Init(int16_t* linkTable, int32_t linkId);
		void Activate(int32_t slotIdx, int32_t skipIntro);
		void Deactivate(int32_t slotIdx);
		void Interactions();

		STATIC_ASSERT(sizeof(TokenTableEntry) == 0x10);
		STATIC_ASSERT(sizeof(PickupSlot) == 0x10);
		STATIC_ASSERT(sizeof(PickupTable) == 0x704);
		STATIC_ASSERT(sizeof(HiddenCollectible) == 0x8);
	}
}

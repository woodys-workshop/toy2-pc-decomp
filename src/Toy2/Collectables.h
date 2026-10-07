#pragma once

#include "Common.h"

namespace Toy2
{
	namespace Collectables
	{
		struct TokenTableEntry
		{
			int32_t unkVar0;
			int32_t unkVar1;
			const char* text;
			int32_t unkVar3;
		};

		struct PickupSlot
		{
			int32_t linkId;
			int32_t state;
			int32_t* yPosPtr;
			int32_t collectTimer;
		};

		void BuildPickupTable();
		int32_t* LoadTokenTable(int32_t* a1);
		void Init(int16_t* linkTable, int32_t linkId);
		void Activate(int32_t slotIdx, int32_t skipIntro);
		void Deactivate(int32_t slotIdx);
		void Interactions();

		STATIC_ASSERT(sizeof(TokenTableEntry) == 0x10);
		STATIC_ASSERT(sizeof(PickupSlot) == 0x10);
	}
}

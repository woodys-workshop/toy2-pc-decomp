#pragma once

#include "Common.h"


namespace Toy2
{
    namespace Collectables
    {
        void BuildPickupTable();
        int32_t* LoadTokenTable(int32_t* a1);
        void Init(int16_t* linkTable, int32_t linkId);
        void Activate(int32_t slotIdx, int32_t skipIntro);
        void Deactivate(int32_t slotIdx);
        void Interactions();
    }
}
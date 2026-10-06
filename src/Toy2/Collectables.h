#pragma once

#include "Common.h"
#include <windows.h>

namespace Toy2
{
    namespace Levels
    {
        struct Type63;
    }
}


namespace Toy2
{
    namespace Collectables
    {
        void BuildPickupTable();

        int32_t* LoadTokenTable(int32_t* a1);
        void Token(int32_t linkId);
        void Init(int16_t* linkTable, int32_t linkId);
        void Activate(int32_t slotIdx, int32_t skipIntro);
        void Deactivate(int32_t slotIdx);
        void CosmicShield(Toy2::Levels::Type63* object);
        void Interactions();
        int32_t Classify(int32_t a1);
    }
}
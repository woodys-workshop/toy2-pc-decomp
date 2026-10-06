#pragma once

#include "Toy2/Actor.h"
#include "Common.h"

namespace Toy2
{
    namespace Animation
    {
        void ResetNodeAngles();
        void AnimateActors(Actor::Toy2Actor** actorList);
        uint32_t StepEventTrack(uint8_t* animStream);
        void EvaluateClip(int16_t* clipHeader, int32_t time, int32_t baseBone, uint8_t track);
    }
}
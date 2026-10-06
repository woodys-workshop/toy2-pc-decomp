#pragma once

#include "Toy2/Actor.h"
#include "Common.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include <windows.h>

struct Vector3F;
struct Vector3I;

namespace Toy2
{
    namespace Actor
    {
        struct Toy2Actor;
    }
}

namespace Nu3D
{
    struct Creature;
}

namespace Toy2
{
    namespace Animation
    {
        void ResetNodeAngles();
        void AnimateActors(Actor::Toy2Actor** actorList);

        uint32_t StepEventTrack(uint8_t* animStream);
        void EvaluateClip(int16_t* clipHeader, int32_t time, int32_t baseBone, uint8_t track);
        void TransformByBone(Vector3I* point, Toy2::Actor::Toy2Actor* actor, int32_t boneIndex);
        void ParseHeader(int32_t animData);
        int32_t SampleNodeTransform(int32_t curNode, uint16_t* animData, uint32_t unk, D3DMATRIX* matrix);
        void EvaluateClipToMatrices(int32_t index, Nu3D::Creature* creature, D3DMATRIX* targetMatrix, int16_t* animData, uint32_t unk, int32_t flag);
        void GetRootWorldPos(Vector3F* out, int32_t index);
    }
}
#pragma once

#include "Toy2/Actor.h"
#include "Common.h"

namespace Toy2
{
	namespace Animation
	{
		extern Actor::Toy2Actor** g_animActorList;
		extern Vector3I g_nodeAngles[2048];
		extern int16_t g_unk546D70;

		void ResetNodeAngles();
		void AnimateActors(Actor::Toy2Actor** actorList);
		uint32_t StepEventTrack(uint8_t* animStream);
		void EvaluateClip(int16_t* clipHeader, int32_t time, int32_t baseBone, uint8_t track);
		void TransformByBone(Vector3I* point, Toy2::Actor::Toy2Actor* actor, int32_t boneIndex);
	}
}
#pragma once

#include "Toy2/Actor.h"
#include "Common.h"

namespace Toy2
{
	namespace Animation
	{
		extern CreatureActor** g_animActorList;
		extern Vector3I g_nodeAngles[2048];
		extern int16_t g_unk546D70;

		void ResetNodeAngles();
		void AnimateActors(CreatureActor** actorList);
		uint32_t StepEventTrack(uint8_t* animStream);
		void EvaluateClip(int16_t* clipHeader, int32_t time, int32_t baseBone, uint8_t track);
		void TransformByBone(Vector3I* point, Toy2::CreatureActor* actor, int32_t boneIndex);
	}
}
#pragma once

#include "Common.h"
#include "Numerics.h"

namespace Toy2
{
	namespace Buzz
	{
		struct Toy2BuzzActor;
	}
}

namespace Toy2
{
	namespace MoveableObject
	{
		struct MoveableObject
		{
			Vector3I pos;
			int32_t segDeltaX;
			int32_t segDeltaZ;
			int16_t liftVel;
			int16_t pushState;
			int16_t segProgress;
			int16_t segLength;
			int16_t segHeading;
			int16_t waypointIdx;
			int16_t minWaypointIdx;
			int16_t pathIdx;
			int16_t platformIdx;
			int16_t modelNode;
		};

		STATIC_ASSERT(sizeof(MoveableObject) == 0x28);

		void ComputeSegment(int32_t recordType, Toy2::MoveableObject::MoveableObject* l_moveableObject);
		void InitTable(int16_t* array);
		void Update(Toy2::Buzz::Toy2BuzzActor* buzzActor);
	}
}

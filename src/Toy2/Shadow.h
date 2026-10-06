#pragma once

#include "Common.h"

namespace Toy2
{
	namespace Shadow
	{
		void ResetShadowCount();
		void QueueStretched(int32_t x, int32_t baseY, int32_t z, int32_t size, int32_t lightY);
		void QueueStretchedForBuzz(int32_t x, int32_t baseY, int32_t z, int32_t size);
	}
}

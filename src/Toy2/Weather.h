#pragma once

#include "Common.h"

namespace Toy2
{
	namespace Weather
	{
		void Init();
		void StepPrecipitation(int32_t spawnRate, int32_t fallSpeed, int32_t particleType);

		extern int32_t g_spawnAccumulator;
		extern int32_t g_weatherBasePointer;
		extern int32_t g_weatherSpriteSheet;
	}
}
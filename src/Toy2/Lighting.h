#pragma once

#include "Common.h"

namespace Toy2
{
	namespace Lighting
	{
		void SpawnLight(int32_t x, int32_t y, int32_t z, int32_t color, int32_t lifetime, int32_t groupId);
		void UpdateBuzzLight();
		void InitBuzzLight();
	}
}

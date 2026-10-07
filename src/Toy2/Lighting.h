#pragma once

#include "Common.h"
#include "Numerics.h"

namespace Toy2
{
	namespace Lighting
	{
		struct DynamicLight
		{
			Vector3I pos;
			int32_t lifetime;
			int32_t groupId;
			uint8_t colorR;
			uint8_t colorG;
			uint8_t colorB;
			uint8_t pad;
		};

		struct LevelLightDef
		{
			Vector3I offset;
			int32_t range;
			uint32_t packedColor;
		};

		void SpawnLight(int32_t x, int32_t y, int32_t z, int32_t color, int32_t lifetime, int32_t groupId);
		void UpdateBuzzLight();
		void InitBuzzLight();

		STATIC_ASSERT(sizeof(DynamicLight) == 0x18);
		STATIC_ASSERT(sizeof(LevelLightDef) == 0x14);
	}
}

#pragma once

#include "Common.h"

namespace Renderer
{
	namespace LensFlare
	{
		extern int32_t g_activeLensFlareIdx;
		extern int16_t g_flareActive[2];
		extern int32_t g_flareSlotCounts[2];
		extern int32_t g_lensFlareCount;

		void RegisterLight(int32_t worldX, int32_t worldY, int32_t worldZ, int32_t colorR, int32_t colorG, int32_t colorB, int32_t baseSize);
		void CullAndQueue();
		void RenderSlot(int32_t slotIndex);
	}
}

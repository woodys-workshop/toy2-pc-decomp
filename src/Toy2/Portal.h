#pragma once

#include "Common.h"
#include "Toy2/Levels.h"

namespace Toy2
{
	namespace Portal
	{
		struct BackdropRect
		{
			int16_t x0;
			int16_t y0;
			int16_t x1;
			int16_t y1;
		};

		void FloodVisibility(Toy2::Levels::PortalEntry* a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6);
		void UpdateActiveSector();
		void UpdateActiveSectorAt(int32_t x, int32_t y, int32_t z);

		STATIC_ASSERT(sizeof(BackdropRect) == 0x8);
	}
}

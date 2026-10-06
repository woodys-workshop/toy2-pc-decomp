#pragma once

#include "Common.h"

namespace Toy2
{
	namespace Levels
	{
		struct PortalEntry;
	}
}

namespace Toy2
{
	namespace Portal
	{
		void FloodVisibility(Toy2::Levels::PortalEntry* a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6);
		void UpdateActiveSector();
		void UpdateActiveSectorAt(int32_t x, int32_t y, int32_t z);
	}
}

#pragma once

#include "Common.h"
#include <windows.h>

namespace Toy2
{
	namespace AlleysAndGullies
	{
		void UpdateMovingWood(int32_t* pathProgress, int32_t platformIndex, int32_t recordIndex, int32_t renderLinkId, int32_t renderLinkId2, int32_t speed);
		void InitHiddenCollectibles();
		void Interactions();

		void Init();
	}
}

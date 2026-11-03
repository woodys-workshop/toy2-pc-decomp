#pragma once

#include "Common.h"

namespace Toy2
{
	namespace AndysHouse
	{
		extern int32_t g_tinRobotLastPhase;
		extern int32_t g_inTinRobotFight;
		extern int32_t g_tinRobotFlashTimer;
		extern int32_t g_tinRobotFlash;

		void Initialize();
		void Interactions();
	}
}

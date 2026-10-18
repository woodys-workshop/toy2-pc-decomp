#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Toy2/Levels.h"

namespace Toy2
{
	namespace AlsSpaceLand
	{
		void Interactions();

		extern int32_t g_unk52FA58;
		extern int32_t g_unk52FA88;
		extern int32_t g_unk52FA8C;
		extern int32_t g_unk52FAE0;
		extern int32_t g_unk52FB7C;
		extern int32_t g_unk52FB9C;

		extern RGB16 g_unk4F2F18[4];
		extern char* g_unk4F2F30[5];
		extern Transform g_laserPositions[2];
		extern Levels::Type63* g_unk52FA80;
		extern int32_t g_unk52FA84;
		extern int32_t g_unk52FA90;
		extern Vector3I g_unk52FA98;
		extern Vector3I g_unk52FAA8;
		extern Vector3I g_unk52FAB8;
		extern Vector3I g_unk52FAC8;
		extern int32_t g_unk52FAD8;
		extern int32_t g_unk52FADC;
		extern int32_t g_unk52FAE4;
		extern Vector3I g_clawHomePos;
		extern int32_t g_unk52FAF8;
		extern int32_t g_laserTotalTicks[2];
		extern Vector3I g_tokenPos;
		extern int32_t g_clawInitialized;
		extern int32_t g_unk52FB1C;
		extern int32_t g_unk52FB20[16];
		extern int32_t g_unk52FB60;
		extern int32_t g_unk52FB64;
		extern int32_t g_unk52FB68;
		extern int32_t g_unk52FB6C;
		extern int32_t g_unk52FB70;
		extern int32_t g_clawCatchResult;
		extern int32_t g_unk52FB78;
		extern int32_t g_unk52FB80;
		extern int32_t g_clawHoldTimer;
		extern int32_t g_clawState;
		extern int32_t g_unk52FB8C;
		extern int32_t g_laserTimer[2];
		extern int32_t g_unk52FB98;
		extern int32_t g_clawSwingX;
		extern int32_t g_clawSwingZ;
		extern int32_t g_clawDropDepth;
		extern int32_t g_laserWaypointIdx[2];
	}
}

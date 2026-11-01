#pragma once

#include "Common.h"
#include "Numerics.h"

namespace Toy2
{
	namespace LevelSelect
	{
		struct LevelSelectCamera
		{
			Vector3I pos;
			Vector3I lookDir;
			int32_t unused1;
			int32_t unused2;
			int32_t unused3;
			Angles angles;
		};

		struct LevelLinkPair
		{
			int16_t* linksToHide;
			int16_t* linksToShow;
		};

		struct LevelSelectPathData
		{
			int32_t pathProgress[31];
		};

		void ResetCursor();
		int32_t Tick();

		extern char g_jumpToSelectTxt[15];

		STATIC_ASSERT(sizeof(LevelSelectCamera) == 0x28);
		STATIC_ASSERT(sizeof(LevelLinkPair) == 0x8);
		STATIC_ASSERT(sizeof(LevelSelectPathData) == 0x7C);
	}
}

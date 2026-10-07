#pragma once

#include "Common.h"

namespace Renderer
{
	namespace Shadows
	{
		struct ShadowRecord
		{
			int32_t worldX;
			int32_t groundY;
			int32_t worldZ;
			int16_t size;
			int16_t heightBias;
		};

		struct ShadowStretch
		{
			int16_t skewX0;
			int16_t skewX1;
			int16_t skewZ;
			int16_t alpha;
		};

		extern ShadowRecord g_shadowRecords[48];
		extern ShadowStretch g_shadowStretch[48];
		extern int16_t g_shadowCount;
		extern int32_t g_unusedShadowVar;

		void DrawAll();

		STATIC_ASSERT(sizeof(ShadowRecord) == 0x10);
		STATIC_ASSERT(sizeof(ShadowStretch) == 0x8);
	}
}

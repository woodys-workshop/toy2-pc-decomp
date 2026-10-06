#pragma once

#include "Common.h"

namespace Toy2
{
	namespace Dialogue
	{
		int32_t WrapSubtitleText(char* text);
		void UpdateSubtitleBox();
		void Begin(int32_t actorIdx, int32_t recordIdx, char* text, int32_t camAngle, int32_t camAngleValue, int32_t cinematicState);
	}
}

#pragma once

#include "Common.h"

namespace Toy2
{
	namespace Dialogue
	{
		extern int32_t g_unk50A518;
		extern int32_t g_dialogueScript[23];

		int32_t WrapSubtitleText(char* text);
		void UpdateSubtitleBox();
		void Begin(int32_t actorIdx, int32_t recordIdx, char* text, int32_t camAngle, int32_t camAngleValue, int32_t cinematicState);
	}
}

#pragma once

#include "Common.h"

namespace Toy2
{
	namespace Dialogue
	{
		extern int16_t g_subtitleBuffer[72];
		extern uint8_t g_subtitleWrapBuffer[540];
		extern uint8_t* g_subtitleText;
		extern int32_t g_subtitleBoxAnim;
		extern int32_t g_subtitleMode;
		extern int32_t g_subtitleTypeTimer;
		extern int32_t g_subtitleColumn;
		extern int32_t g_subtitleRowStart;
		extern int32_t g_subtitleFontPage;
		extern int32_t g_subtitleScroll;
		extern int32_t g_unk50A110;
		extern int32_t g_unk50A518;
		extern int32_t g_dialogueScript[23];

		int32_t WrapSubtitleText(char* text);
		void UpdateSubtitleBox();
		void Begin(int32_t actorIdx, int32_t recordIdx, char* text, int32_t camAngle, int32_t camAngleValue, int32_t cinematicState);
	}
}

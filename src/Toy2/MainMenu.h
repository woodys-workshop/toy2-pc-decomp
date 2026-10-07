#pragma once

#include <STDIO.H>
#include "Common.h"

namespace Toy2
{
	namespace MainMenu
	{
		void RenderMenu();
		int32_t Tick();
		void ShowSettings();

		extern int32_t g_fadeTimer;
		extern int32_t g_nextScreen;
		extern RGB32 g_menuClearColor;
	}
}
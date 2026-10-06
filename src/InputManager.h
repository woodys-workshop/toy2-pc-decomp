#pragma once

#include "Common.h"
#include "InputTypes.h"

namespace InputManager
{
	struct InputMapping
	{
		char* name;
		int32_t id;
	};

	extern int16_t g_curButtonsPressed;
	extern int16_t g_prevButtonsPressed;
	extern int16_t g_directionInputState;
	extern int16_t g_prevDirectionInputState;
	extern int16_t g_directionInputState2Frames;
	extern int16_t g_directionInputState3Frames;
	extern uint8_t g_previousInputStates[256];

	void Init();
	uint8_t IsKeyPressed(int32_t inputCode);
	void UpdateButtonStates();
	int32_t GetPressedInput();
	char* GetGameControlName(ToyInputCodes index);
	void Cleanup();
	int32_t FindKeyReleased();
	int32_t GetCurButtonsPressed();
	void UpdateDirectionInputState();

	STATIC_ASSERT(sizeof(InputMapping) == 0x8);
}

#include "InputManager.h"
#include "Toy2/D3DApp.h"
#include "Toy2/Toy2.h"
#include "SaveManager.h"
#include "Nu3D/Nu3D.h"

#include <stddef.h>
#include <DINPUT.H>

namespace InputManager
{
	// GLOBAL: TOY2 0x00882794
	int16_t g_prevButtonsPressed;

	// GLOBAL: TOY2 0x0088279C
	int16_t g_curButtonsPressed;

	// GLOBAL: TOY2 0x00529B00
	int16_t g_buttonsPressed;

	// GLOBAL: TOY2 0x0052AD88
	int16_t g_directionInputState;

	// GLOBAL: TOY2 0x0052F2FE
	int16_t g_prevDirectionInputState;

	// GLOBAL: TOY2 0x0053C630
	int16_t g_directionInputState2Frames;

	// GLOBAL: TOY2 0x0053C632
	int16_t g_directionInputState3Frames;

	// GLOBAL: TOY2 0x00529C98
	int32_t g_inputManagerInit;

	// GLOBAL: TOY2 0x00529D3C
	int32_t g_directInputSuccess;

	// GLOBAL: TOY2 0x00529CA0
	int32_t g_isInputDeviceValid[4];

	// GLOBAL: TOY2 0x00529D28
	int32_t g_unused1;

	// GLOBAL: TOY2 0x00529D30
	int32_t g_unused2;

	// GLOBAL: TOY2 0x00529C90
	int32_t g_dInputDeviceCount;

	// GLOBAL: TOY2 0x00529C94
	LPDIRECTINPUTA g_directInput;

	// GLOBAL: TOY2 0x00529C9C
	LPDIRECTINPUTDEVICEA g_directInputDevice;

	// GLOBAL: TOY2 0x005298D0
	LPDIRECTINPUTDEVICE2A g_directInputDevices[4];

	// GLOBAL: TOY2 0x005297D0
	GUID g_dInputGuids[16];

	// GLOBAL: TOY2 0x00529A00
	uint8_t g_inputStates[256];

	// GLOBAL: TOY2 0x005298E0
	uint8_t g_previousInputStates[256];

	// GLOBAL: TOY2 0x00529B02
	int16_t g_joystickUnused1;

	// GLOBAL: TOY2 0x00529D34
	int16_t g_joystickUnused2;

	// GLOBAL: TOY2 0x00529D36
	int16_t g_joystickUnused3;

	// GLOBAL: TOY2 0x00529D38
	int16_t g_joystickUnused4;

	// GLOBAL: TOY2 0x00529D3A
	int16_t g_joystickUnused5;

	// GLOBAL: TOY2 0x00529CD0
	int16_t g_joystickUnused6;

	// GLOBAL: TOY2 0x00731CD8
	int32_t g_analogInputX;

	// GLOBAL: TOY2 0x00731CDC
	int32_t g_analogInputY;

	// GLOBAL: TOY2 0x00529CD8
	DIJOYSTATE g_joystickState;

	STATIC_ASSERT(sizeof(DIJOYSTATE) == 80);
	STATIC_ASSERT(offsetof(DIJOYSTATE, rgbButtons) == 48);

	// GLOBAL: TOY2 0x00529CB4
	LPDIRECTINPUTDEVICE2A g_dInputUnused[4];

	// GLOBAL: TOY2 0x00529CC4
	int32_t g_joystickRawX;

	// GLOBAL: TOY2 0x00529CC8
	int32_t g_joystickRawY;

	// GLOBAL: TOY2 0x005299F8
	int32_t g_joystickDeadzoneThreshold1;

	// GLOBAL: TOY2 0x00529CCC
	int32_t g_joystickDeadzoneThreshold2;

	// GLOBAL: TOY2 0x00529D2C
	int32_t g_joystickDirectionLockState;

	// GLOBAL: TOY2 0x00830C34
	int32_t g_directionalInputCount;

	// GLOBAL: TOY2 0x00529CB0
	int16_t g_joystickConnected;

	// GLOBAL: TOY2 0x00529D40
	int32_t g_joystickDirectionFlags;

	// GLOBAL: TOY2 0x00503860
	int16_t g_camMasks[14] = {
		INPUT_FIRE, INPUT_JUMP,	  INPUT_SPIN, INPUT_JUMP, INPUT_FIRE,	INPUT_CANCEL, INPUT_JUMP,
		INPUT_SPIN, INPUT_CANCEL, INPUT_FIRE, INPUT_JUMP, INPUT_CANCEL, INPUT_SPIN,	  INPUT_JUMP
	};

	// clang-format off
	// GLOBAL: TOY2 0x004ED398
	InputMapping g_inputMapping[] =
	{
		{ "esc", 1 },          { "1", 2 },            { "2", 3 },            { "3", 4 },
		{ "4", 5 },            { "5", 6 },            { "6", 7 },            { "7", 8 },
		{ "8", 9 },            { "9", 10 },           { "0", 11 },           { "minus", 12 },
		{ "equals", 13 },      { "back", 14 },        { "tab", 15 },         { "q", 16 },
		{ "w", 17 },           { "e", 18 },           { "r", 19 },           { "t", 20 },
		{ "y", 21 },           { "u", 22 },           { "i", 23 },           { "o", 24 },
		{ "p", 25 },           { "(", 26 },           { ")", 27 },           { "return", 28 },
		{ "lcontrol", 29 },    { "a", 30 },           { "s", 31 },           { "d", 32 },
		{ "f", 33 },           { "g", 34 },           { "h", 35 },           { "j", 36 },
		{ "k", 37 },           { "l", 38 },           { ";", 39 },           { "apostrophe", 40 },
		{ "grave", 41 },       { "lshift", 42 },      { "\\", 43 },          { "z", 44 },
		{ "x", 45 },           { "c", 46 },           { "v", 47 },           { "b", 48 },
		{ "n", 49 },           { "m", 50 },           { ",", 51 },           { ".", 52 },
		{ "/", 53 },           { "rshift", 54 },      { "multiply", 55 },    { "lmenu", 56 },
		{ "space", 57 },       { "capital", 58 },     { "f1", 59 },          { "f2", 60 },
		{ "f3", 61 },          { "f4", 62 },          { "f5", 63 },          { "f6", 64 },
		{ "f7", 65 },          { "f8", 66 },          { "f9", 67 },          { "f10", 68 },
		{ "numlock", 69 },     { "scroll", 70 },      { "pad 7", 71 },       { "pad 8", 72 },
		{ "pad 9", 73 },       { "subtract", 74 },    { "pad 4", 75 },       { "pad 5", 76 },
		{ "pad 6", 77 },       { "add", 78 },         { "pad 1", 79 },       { "pad 2", 80 },
		{ "pad 3", 81 },       { "pad 0", 82 },       { "pad .", 83 },       { "f11", 87 },
		{ "f12", 88 },         { "f13", 100 },        { "f14", 101 },        { "f15", 102 },
		{ "kana", 112 },       { "convert", 121 },    { "noconvert", 123 },  { "yen", 125 },
		{ "numpadequals", 141 }, { "circumflex", 144 }, { "at", 145 },       { "colon", 146 },
		{ "underline", 147 },  { "kanji", 148 },      { "stop", 149 },       { "ax", 150 },
		{ "unlabeled", 151 },  { "numpadenter", 156 }, { "rcontrol", 157 },  { "pad,", 179 },
		{ "divide", 181 },     { "sysrq", 183 },      { "rmenu", 184 },      { "home", 199 },
		{ "A", 200 },          { "prior", 201 },      { "C", 203 },          { "D", 205 },
		{ "end", 207 },        { "B", 208 },          { "next", 209 },       { "ins", 210 },
		{ "del", 211 },        { "lwin", 219 },       { "rwin", 220 },       { "apps", 221 },
		{ "joy 1", 1024 },     { "joy 2", 1025 },     { "joy 3", 1026 },     { "joy 4", 1027 },
		{ "joy 5", 1028 },     { "joy 6", 1029 },     { "joy 7", 1030 },     { "joy 8", 1031 },
		{ "joy 9", 1032 },     { "joy 10", 1033 },    { "joy 11", 1034 },    { "joy 12", 1035 },
		{ "joy 13", 1036 },    { "joy 14", 1037 },    { "joy 15", 1038 },    { "joy 16", 1039 },
		{ "joy 17", 1040 },    { "joy 18", 1041 },    { "joy 19", 1042 },    { "joy 20", 1043 },
		{ "joy 22", 1044 },    { "joy 23", 1045 },    { "joy 24", 1046 },    { "joy 25", 1047 },
		{ "joy 26", 1048 },    { "joy 27", 1049 },    { "joy 28", 1050 },    { "joy 29", 1051 },
		{ "pov up", 1052 },    { "pov right", 1053 }, { "pov down", 1054 },  { "pov left", 1055 },
		{ "direction pad", 2048 },
		{ NULL, -1 }
	};
	// clang-format on
}

namespace InputManager
{
	// FUNCTION: TOY2 0x00414EA0 [MATCHED]
	BOOL WINAPI EnumDevices(LPCDIDEVICEINSTANCEA deviceInstance, LPVOID context)
	{
		GUID* lpContext = (GUID*)context;

		lpContext[g_dInputDeviceCount] = deviceInstance->guidInstance;

		++g_dInputDeviceCount;

		return g_dInputDeviceCount != 4;
	}

	// FUNCTION: TOY2 0x004152E0
	void Init()
	{
		LPDIRECTINPUTDEVICE devices[4];

		g_inputManagerInit = 0;
		g_directInputSuccess = 0;

		g_isInputDeviceValid[3] = 0;
		g_isInputDeviceValid[2] = 0;
		g_isInputDeviceValid[1] = 0;
		g_isInputDeviceValid[0] = 0;

		InputManager::g_unused1 = 0;
		InputManager::g_unused2 = 0;

		g_dInputDeviceCount = 0;

		if (! DirectInputCreateA(D3DApp::g_windowData.hInstance, 0x0500, &g_directInput, 0))
		{
			// clang-format off
			if (! g_directInput->CreateDevice(GUID_SysKeyboard, &g_directInputDevice, 0) 
				&& ! g_directInputDevice->SetDataFormat(&c_dfDIKeyboard)
				&& ! g_directInputDevice->SetCooperativeLevel(D3DApp::g_windowData.mainHwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) && ! g_directInputDevice->Acquire())
			{
				g_directInputSuccess = 1;
			}

			if (! g_directInput->EnumDevices(4, EnumDevices, g_dInputGuids, 1))
			{
				int32_t deviceIdx = 0;

				if (g_dInputDeviceCount > 0)
				{
					LPGUID curGuid = g_dInputGuids;

					do
					{
						if (! g_directInput->CreateDevice(*curGuid, &devices[deviceIdx], 0))
						{
							LPDIRECTINPUTDEVICE2A* curDevice = &g_directInputDevices[deviceIdx];

							if (! devices[deviceIdx]->QueryInterface(IID_IDirectInputDevice2A, (LPVOID*)curDevice)
								&& ! (*curDevice)->SetDataFormat(&c_dfDIJoystick) 
								&& ! (*curDevice)->SetCooperativeLevel(D3DApp::g_windowData.mainHwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)
								&& ! (*curDevice)->Acquire())
							{
								g_isInputDeviceValid[deviceIdx] = 1;
							}
						}

						++deviceIdx;
						++curGuid;

					} while (deviceIdx < g_dInputDeviceCount);
				}

				g_inputManagerInit = 1;
			}
			// clang-format on
		}
	}

	// FUNCTION: TOY2 0x004157E0
	uint8_t IsKeyPressed(int32_t inputCode)
	{
		uint8_t currentState = g_inputStates[inputCode];
		uint8_t updated = ~currentState;
		uint8_t result = g_previousInputStates[inputCode] & currentState;

		g_previousInputStates[inputCode] = updated;

		return result;
	}

	// FUNCTION: TOY2 0x00414AF0
	void UpdateInputState()
	{
		int32_t hasDirectionalInput = 0;

		g_buttonsPressed = 0;

		g_joystickUnused1 = 0;
		g_joystickUnused2 = 0;
		g_joystickUnused3 = 0;
		g_joystickUnused4 = 0;
		g_joystickUnused5 = 0;
		g_joystickUnused6 = 0;

		if (g_directInputSuccess)
		{
			Nu3D::MemSet32Util(g_inputStates, 64, 0);

			g_directInputDevice->Acquire();
			g_directInputDevice->GetDeviceState(256, g_inputStates);

			int32_t analogX = 0;
			int32_t analogY = 0;

			if (g_inputStates[DIK_RSHIFT])
				g_inputStates[DIK_LSHIFT] = 1;

			if (g_inputStates[DIK_RCONTROL])
				g_inputStates[DIK_LCONTROL] = 1;

			if (g_inputStates[DIK_RMENU])
				g_inputStates[DIK_LMENU] = 1;

			int16_t curButtonsPressed = g_buttonsPressed;
			SaveManager::SaveControlMapping* saveStructs = SaveManager::g_save99Data.saveStructs;

			do
			{
				int32_t key = saveStructs->dInputCode;

				if (saveStructs->dInputCode >= 0 && key < 1024)
				{
					int32_t keyPressed = g_inputStates[key];

					if (keyPressed)
					{
						curButtonsPressed |= (uint16_t)(saveStructs->gameControlId);
						hasDirectionalInput = 1;
						int32_t tempKeyPressed = keyPressed;
						keyPressed = 0;

						switch (saveStructs->gameControlId)
						{
							case INPUT_UP:
								analogY = 32768;
								break;

							case INPUT_RIGHT:
								analogX = 32768;
								break;

							case INPUT_DOWN:
								analogY = -32768;
								break;

							case INPUT_LEFT:
								analogX = -32768;
								break;

							default:
								keyPressed = tempKeyPressed;
								break;
						}
					}
				}

				++saveStructs;

			} while (saveStructs < SaveManager::g_save99Data.unusedStructs);

			g_buttonsPressed = curButtonsPressed;

			if (hasDirectionalInput)
			{
				g_analogInputX = analogX;
				g_analogInputY = analogY;
			}
		}

		if (g_isInputDeviceValid[0])
		{
			if (g_directInputDevices[0]->Poll())
				g_directInputDevices[0]->Acquire();

			if (g_directInputDevices[0]->GetDeviceState(80, &g_joystickState))
				g_directInputDevices[0]->Acquire();

			int16_t joystickButtons = g_buttonsPressed;

			g_joystickRawX = g_joystickState.lX;
			g_joystickRawY = g_joystickState.lY;

			g_joystickDeadzoneThreshold1 = 11200;
			g_joystickDeadzoneThreshold2 = 11200;

			SaveManager::SaveControlMapping* joystickMapping = SaveManager::g_save99Data.saveStructs;

			do
			{
				int32_t dInputCode = joystickMapping->dInputCode;

				if (joystickMapping->dInputCode >= 0x400 && dInputCode < 0x41F && g_joystickState.rgbButtons[dInputCode - 0x400])
					joystickButtons |= (uint16_t)(joystickMapping->gameControlId);

				++joystickMapping;

			} while (joystickMapping < SaveManager::g_save99Data.unusedStructs);

			uint8_t directionLockState = g_joystickDirectionLockState;

			g_buttonsPressed = joystickButtons;
			g_directionalInputCount = 1;
			g_joystickConnected = 1;

			int32_t joystickYAxis;
			int32_t joystickXAxis;

			if ((g_joystickDirectionLockState & 3) == 2)
			{
				joystickYAxis = (((g_joystickState.lX >> 8) & 0xFF) - 128) << 8;
				if (joystickYAxis <= -11200 || joystickYAxis >= 11200)
				{
					directionLockState = g_joystickDirectionLockState & 12;
					g_joystickDirectionLockState &= 12u;
				}
			}
			else
			{
				joystickYAxis = (((g_joystickState.lX >> 8) & 0xFF) - 128) << 8;
			}

			if ((directionLockState & 0xC) == 8)
			{
				joystickXAxis = (128 - ((g_joystickState.lY >> 8) & 0xFF)) << 8;

				if (joystickXAxis <= -11200 || joystickXAxis >= 11200)
					g_joystickDirectionLockState = directionLockState & 3;
			}
			else
			{
				joystickXAxis = (128 - ((g_joystickState.lY >> 8) & 0xFF)) << 8;
			}

			int32_t dirFlags = 0;
			int32_t anyDirectionActive = 0;
			g_joystickDirectionFlags = 0;

			if (joystickYAxis < -11200)
			{
				dirFlags = 128;
				anyDirectionActive = 1;
				g_joystickDirectionFlags = 128;
			}

			if (joystickYAxis > 11200)
			{
				dirFlags |= 32;
				anyDirectionActive = 1;
				g_joystickDirectionFlags = dirFlags;
			}

			if (joystickXAxis < -11200)
			{
				dirFlags |= 64;
				anyDirectionActive = 1;
				g_joystickDirectionFlags = dirFlags;
			}

			if (joystickXAxis <= 11200)
			{
				if (! anyDirectionActive)
					return;
			}
			else
			{
				dirFlags |= 16;
				g_joystickDirectionFlags = dirFlags;
			}

			g_analogInputX = joystickYAxis;
			g_analogInputY = joystickXAxis;

			g_buttonsPressed = dirFlags | joystickButtons;
		}
	}

	// FUNCTION: TOY2 0x00452180
	void UpdateButtonStates()
	{
		g_prevButtonsPressed = g_curButtonsPressed;
		UpdateInputState();
		g_curButtonsPressed = g_buttonsPressed;
	}

	// FUNCTION: TOY2 0x00415120 [MATCHED]
	int32_t GetPressedInput()
	{
		for (int32_t i = 0; i < 256; i++)
		{
			if (g_inputStates[i])
				return i;
		}

		for (int32_t j = 0; j < 32; j++)
		{
			if (g_joystickState.rgbButtons[j])
				return j + 1024;
		}

		return g_joystickDirectionFlags != 0 ? 2048 : -1;
	}

	// FUNCTION: TOY2 0x004152A0 [MATCHED]
	char* GetGameControlName(ToyInputCodes index)
	{
		for (int32_t i = 0; g_inputMapping[i].name; i++)
		{
			if (g_inputMapping[i].id == index)
				return g_inputMapping[i].name;
		}

		return 0;
	}

	// FUNCTION: TOY2 0x00415460
	void Cleanup()
	{
		if (g_directInput)
		{
			if (g_directInputDevice)
			{
				g_directInputDevice->Unacquire();
				g_directInputDevice->Release();
			}

			for (int32_t i = 0; i < 4; i++)
			{
				if (g_dInputUnused[i])
				{
					g_dInputUnused[i]->Unacquire();
					g_dInputUnused[i]->Release();
				}
			}

			g_directInput->Release();
		}
	}

	// FUNCTION: TOY2 0x00415500
	int16_t __cdecl ScanCodeToGlyph(uint8_t scanCode)
	{
		for (int32_t index = 0; Toy2::Ini::g_keyNameTable[index].scancode != 0xFFFF; ++index)
		{
			if (Toy2::Ini::g_keyNameTable[index].scancode == scanCode)
				return (int16_t)Toy2::Ini::g_keyNameTable[index].unk2;
		}
		return -1;
	}

	// FUNCTION: TOY2 0x00415540
	int16_t __cdecl GlyphToScanCode(uint8_t glyph)
	{
		for (int32_t index = 0; Toy2::Ini::g_keyNameTable[index].scancode != 0xFFFF; ++index)
		{
			if (Toy2::Ini::g_keyNameTable[index].unk2 == glyph)
				return (int16_t)Toy2::Ini::g_keyNameTable[index].scancode;
		}
		return -1;
	}

	// FUNCTION: TOY2 0x00415580
	int16_t __cdecl CharacterToGlyph(uint8_t character)
	{
		for (int32_t index = 0; Toy2::Ini::g_keyNameTable[index].scancode != 0xFFFF; ++index)
		{
			if ((uint8_t)Toy2::Ini::g_keyNameTable[index].keyName == character)
				return (int16_t)Toy2::Ini::g_keyNameTable[index].unk2;
		}
		return -1;
	}

	// FUNCTION: TOY2 0x004155C0
	int16_t __cdecl GlyphToCharacter(uint8_t glyph)
	{
		for (int32_t index = 0; Toy2::Ini::g_keyNameTable[index].scancode != 0xFFFF; ++index)
		{
			if (Toy2::Ini::g_keyNameTable[index].unk2 == glyph)
				return (uint8_t)Toy2::Ini::g_keyNameTable[index].keyName;
		}
		return -1;
	}

	// FUNCTION: TOY2 0x00415600
	int16_t __cdecl ScanCodeToCharacter(uint8_t scanCode)
	{
		for (int32_t index = 0; Toy2::Ini::g_keyNameTable[index].scancode != 0xFFFF; ++index)
		{
			if (Toy2::Ini::g_keyNameTable[index].scancode == scanCode)
				return (uint8_t)Toy2::Ini::g_keyNameTable[index].keyName;
		}
		return -1;
	}

	// FUNCTION: TOY2 0x00415800
	int32_t FindKeyPressed()
	{
		for (int32_t i = 1; i < 256; i++)
		{
			uint8_t currentState = g_inputStates[i];
			uint8_t previousState = g_previousInputStates[i];
			uint8_t result;

			if (result = currentState & previousState)
			{
				g_previousInputStates[i] = ~currentState;
				return i;
			}

			g_previousInputStates[i] = ~currentState;
		}

		return 0;
	}

	// FUNCTION: TOY2 0x00415860 [MATCHED]
	int32_t FindKeyReleased()
	{
		for (int32_t i = 1; i < 256; i++)
		{
			uint8_t currentState = g_inputStates[i];
			uint8_t result = g_previousInputStates[i] & currentState;

			g_previousInputStates[i] = ~currentState;

			if (result)
				return i;
		}

		return 0;
	}

	// FUNCTION: TOY2 0x00498620 [MATCHED]
	int32_t GetCurButtonsPressed() { return g_curButtonsPressed; }

	// FUNCTION: TOY2 0x0049EBA0
	void UpdateDirectionInputState()
	{
		int32_t cameraIndex = 3 * (SaveManager::g_save0Data.cameraType & SaveManager::CAMERA_MASK);

		g_directionInputState = g_curButtonsPressed & 0xFFF;

		if (g_curButtonsPressed & g_camMasks[cameraIndex])
			g_directionInputState |= DIR_CAM_REVERSE;

		if (g_curButtonsPressed & g_camMasks[cameraIndex + 1])
			g_directionInputState |= DIR_CAM_FLAG2;

		if (g_curButtonsPressed & g_camMasks[cameraIndex + 2])
			g_directionInputState |= DIR_CAM_FLAG3;
	}
}

#include "SaveManager.h"
#include "InputManager.h"

#include <MEMORY.H>

namespace SaveManager
{
	// GLOBAL: TOY2 0x0052EF90
	Save0Data g_save0Data;

	// GLOBAL: TOY2 0x00529B08
	Save99Data g_save99Data;

	// GLOBAL: TOY2 0x00830CA8
	uint32_t g_curLevelTokenData;

	// FUNCTION: TOY2 0x00415180
	void AddInputEntry(int32_t inputCode, int32_t controlId)
	{
		int32_t saveStructIndex = 0;

		SaveControlMapping* saveStructs = g_save99Data.saveStructs;

		do
		{
			if (saveStructs->dInputCode != -1)
			{
				if (saveStructIndex != (saveStructs - g_save99Data.saveStructs))
				{
					g_save99Data.saveStructs[saveStructIndex] = *saveStructs;

					saveStructs->dInputCode = -1;
					saveStructs->gameControlId = 0;
				}

				++saveStructIndex;
			}

			++saveStructs;

		} while (saveStructs < g_save99Data.unusedStructs);

		if (saveStructIndex < 38)
		{
			g_save99Data.saveStructs[saveStructIndex].dInputCode = inputCode;
			g_save99Data.saveStructs[saveStructIndex].gameControlId = controlId;
		}
	}

	// FUNCTION: TOY2 0x00414F20
	void Init()
	{
		memset(InputManager::g_previousInputStates, 255, sizeof(InputManager::g_previousInputStates));

		// Clear Save99Data entries
		SaveControlMapping* mapping = g_save99Data.saveStructs;

		do
		{
			mapping->dInputCode = TOY_INPUT_UNKNOWN;
			mapping->gameControlId = 0;
			mapping++;

		} while (mapping < g_save99Data.unusedStructs);

		AddInputEntry(TOY_INPUT_CAPITALA, INPUT_UP);
		AddInputEntry(TOY_INPUT_CAPITALB, INPUT_DOWN);
		AddInputEntry(TOY_INPUT_CAPITALC, INPUT_LEFT);
		AddInputEntry(TOY_INPUT_CAPITALD, INPUT_RIGHT);
		AddInputEntry(TOY_INPUT_X, INPUT_CANCEL);
		AddInputEntry(TOY_INPUT_SPACE, INPUT_JUMP);
		AddInputEntry(TOY_INPUT_LCONTROL, INPUT_FIRE);
		AddInputEntry(TOY_INPUT_LSHIFT, INPUT_SPIN);
		AddInputEntry(TOY_INPUT_NEXT, INPUT_CAMERA_RIGHT);
		AddInputEntry(TOY_INPUT_DEL, INPUT_CAMERA_LEFT);
		AddInputEntry(TOY_INPUT_Q, INPUT_TARGET_LOCK);
		AddInputEntry(TOY_INPUT_TAB, INPUT_VISOR_TOGGLE);
		AddInputEntry(TOY_INPUT_X, INPUT_SECRET_MENU);
		AddInputEntry(TOY_INPUT_RETURNKEY, INPUT_MENU);
		AddInputEntry(TOY_INPUT_JOY10, INPUT_CANCEL);
		AddInputEntry(TOY_INPUT_JOY1, INPUT_JUMP);
		AddInputEntry(TOY_INPUT_JOY2, INPUT_FIRE);
		AddInputEntry(TOY_INPUT_JOY3, INPUT_SPIN);
		AddInputEntry(TOY_INPUT_JOY8, INPUT_CAMERA_RIGHT);
		AddInputEntry(TOY_INPUT_JOY7, INPUT_CAMERA_LEFT);
		AddInputEntry(TOY_INPUT_JOY5, INPUT_TARGET_LOCK);
		AddInputEntry(TOY_INPUT_JOY4, INPUT_VISOR_TOGGLE);
		AddInputEntry(TOY_INPUT_JOY10, INPUT_SECRET_MENU);
		AddInputEntry(TOY_INPUT_JOY9, INPUT_MENU);
		AddInputEntry(TOY_INPUT_CAPITALA, INPUT_UP);
		AddInputEntry(TOY_INPUT_CAPITALB, INPUT_DOWN);
		AddInputEntry(TOY_INPUT_CAPITALC, INPUT_LEFT);
		AddInputEntry(TOY_INPUT_CAPITALD, INPUT_RIGHT);
		AddInputEntry(TOY_INPUT_ESC, INPUT_CANCEL);
		AddInputEntry(TOY_INPUT_F1, INPUT_JUMP);
		AddInputEntry(TOY_INPUT_F2, INPUT_MENU);
	}

	// STUB: TOY2 0x004A2C20
	void InitProgressData(Save0Data* save) {}

	// STUB: TOY2 0x004A2CC0
	void LoadProgressData(Save0Data* save) {}

	// STUB: TOY2 0x0049B830
	void SaveToFile(int32_t saveNum, const char* saveName) {}

	// STUB: TOY2 0x004151E0
	void ClearBindByControlId(GameControlId controlId) {}

	// STUB: TOY2 0x00415210
	void ClearBindByInputCode(ToyInputCodes inputCode) {}

	// STUB: TOY2 0x00415240
	int32_t GetInputCodeByControlId(GameControlId controlId) { return 0; }

	// STUB: TOY2 0x00415270
	int32_t GetControlSettingId(ToyInputCodes inputCode) { return 0; }

	// STUB: TOY2 0x00498470
	void SetLightShadowEffects(uint32_t on) {}

	// STUB: TOY2 0x004A2C80
	void TransferProgressData(SaveManager::Save0Data* saveData) {}
}
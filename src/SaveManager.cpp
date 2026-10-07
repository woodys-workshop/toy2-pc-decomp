#include "SaveManager.h"
#include "InputManager.h"
#include "AudioManager/AudioManager.h"
#include "Toy2/Toy2.h"

#include <MEMORY.H>
#include <STDIO.H>
#include <STRING.H>

namespace SaveManager
{
	// GLOBAL: TOY2 0x0052EF90
	Save0Data g_save0Data;

	// GLOBAL: TOY2 0x00529B08
	Save99Data g_save99Data;

	// GLOBAL: TOY2 0x00830CA8
	uint32_t g_curLevelTokenData;

	// GLOBAL: TOY2 0x00830E24
	uint32_t g_saveUnlocksCache;

	// FUNCTION: TOY2 0x00415180 [MATCHED]
	void AddInputEntry(ToyInputCodes inputCode, GameControlId controlId)
	{
		int32_t saveStructIndex = 0;

		for (int32_t i = 0; i < 38; i++)
		{
			if (g_save99Data.saveStructs[i].dInputCode != -1)
			{
				if (saveStructIndex != i)
				{
					g_save99Data.saveStructs[saveStructIndex].dInputCode = g_save99Data.saveStructs[i].dInputCode;
					g_save99Data.saveStructs[saveStructIndex].gameControlId = g_save99Data.saveStructs[i].gameControlId;

					g_save99Data.saveStructs[i].dInputCode = -1;
					g_save99Data.saveStructs[i].gameControlId = 0;
				}

				++saveStructIndex;
			}
		}

		if (saveStructIndex < 38)
		{
			g_save99Data.saveStructs[saveStructIndex].dInputCode = inputCode;
			g_save99Data.saveStructs[saveStructIndex].gameControlId = controlId;
		}
	}

	// FUNCTION: TOY2 0x00414F20 [MATCHED]
	void Init()
	{
		memset(InputManager::g_previousInputStates, 255, sizeof(InputManager::g_previousInputStates));

		// Clear Save99Data entries
		for (int32_t i = 0; i < 38; i++)
		{
			g_save99Data.saveStructs[i].dInputCode = TOY_INPUT_UNKNOWN;
			g_save99Data.saveStructs[i].gameControlId = 0;
		}

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

	// FUNCTION: TOY2 0x004A2C20 [MATCHED]
	void InitProgressData(Save0Data* save)
	{
		save->lastLevel = 0;
		save->unlocks = 0;
		save->health = 14;
		save->lives = 5;

		for (int32_t i = 1; i < 16; i++)
			save->tokens[i] = 0;

		for (int32_t j = 0; j < 48; j++)
			save->moviesUnlocked[j] = 0;
	}

	// FUNCTION: TOY2 0x004A2CC0 [MATCHED]
	void LoadProgressData(Save0Data* save)
	{
		Toy2::g_buzzActor.lives = save->lives;
		Toy2::g_playerUnlocks = save->unlocks;

		int16_t levelIndex = save->lastLevel;

		Toy2::g_levelIndex = levelIndex;
		Toy2::g_buzzActor.health = save->health;

		if (Toy2::g_levelIndex > 14)
			Toy2::g_levelIndex = 14;
		else if (Toy2::g_levelIndex < 0)
			Toy2::g_levelIndex = 0;

		g_curLevelTokenData = save->tokens[Toy2::g_levelFileConversion[Toy2::g_levelIndex]];

		AudioManager::SetVolumes(2 * Toy2::g_musicSliderToAmplitude[save->musicVolume] / 3, 3 * Toy2::g_soundSliderToAmplitude[save->soundVolume] / 2);
	}

	// FUNCTION: TOY2 0x0049B830 [MATCHED]
	void SaveToFile(int32_t saveNum, const char* saveName)
	{
		const char* emptyStrPtr = saveName;
		char emptyStr = Toy2::g_emptyStr[0];

		if (! saveName)
			emptyStrPtr = &emptyStr;

		char buffer[256];
		sprintf(buffer, "Toy2%02d.sav", saveNum);

		FILE* fileHandle = fopen(buffer, "wb");

		if (fileHandle)
		{
			size_t strCount = strlen(emptyStrPtr);
			fwrite(&strCount, 1, sizeof(size_t), fileHandle);

			if (strCount)
				fwrite(emptyStrPtr, 1, strCount, fileHandle);

			if (saveNum == 99)
				fwrite(&g_save99Data, 1, sizeof(Save99Data), fileHandle);
			else
				fwrite(&g_save0Data, 1, sizeof(Save0Data), fileHandle);

			fclose(fileHandle);
		}
	}

	// FUNCTION: TOY2 0x004151E0 [MATCHED]
	void ClearBindByControlId(GameControlId controlId)
	{
		for (int32_t i = 0; i < 38; i++)
		{
			if (g_save99Data.saveStructs[i].gameControlId == controlId)
			{
				g_save99Data.saveStructs[i].dInputCode = -1;
				g_save99Data.saveStructs[i].gameControlId = 0;
			}
		}
	}

	// FUNCTION: TOY2 0x00415210 [MATCHED]
	void ClearBindByInputCode(ToyInputCodes inputCode)
	{
		for (int32_t i = 0; i < 38; i++)
		{
			if (g_save99Data.saveStructs[i].dInputCode == inputCode)
			{
				g_save99Data.saveStructs[i].dInputCode = -1;
				g_save99Data.saveStructs[i].gameControlId = 0;
			}
		}
	}

	// FUNCTION: TOY2 0x00415240 [MATCHED]
	int32_t GetInputCodeByControlId(GameControlId controlId)
	{
		for (int32_t i = 0; i < 38; i++)
		{
			if (g_save99Data.saveStructs[i].gameControlId == controlId)
				return g_save99Data.saveStructs[i].dInputCode;
		}

		return -1;
	}

	// FUNCTION: TOY2 0x00415270 [MATCHED]
	int32_t GetControlSettingId(ToyInputCodes inputCode)
	{
		for (int32_t i = 0; i < 38; i++)
		{
			if (g_save99Data.saveStructs[i].dInputCode == inputCode)
				return g_save99Data.saveStructs[i].gameControlId;
		}

		return -1;
	}

	// FUNCTION: TOY2 0x00498470 [MATCHED]
	void SetLightShadowEffects(uint32_t on) { g_save0Data.lightShadowEffects = on; }

	// FUNCTION: TOY2 0x004A2C80 [MATCHED]
	void TransferProgressData(Save0Data* saveData)
	{
		saveData->lives = Toy2::g_buzzActor.lives;
		saveData->lastLevel = Toy2::g_levelIndex;
		saveData->unlocks = Toy2::g_playerUnlocks;
		saveData->health = Toy2::g_buzzActor.health;
	}
}
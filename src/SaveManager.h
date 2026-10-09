#pragma once

#include "Common.h"
#include "InputTypes.h"

namespace SaveManager
{
	enum SaveCameraType
	{
		CAMERA_PASSIVE = 0x80,
		CAMERA_ACTIVE = 0x40,
		CAMERA_MASK = 0x3F
	};

	struct SaveControlMapping
	{
		int32_t dInputCode;
		int32_t gameControlId;
	};

	struct Save99Data
	{
		int32_t unused1;
		int32_t unused2;
		SaveControlMapping saveStructs[38];
		SaveControlMapping unusedStructs[10];
	};

#pragma pack(push, 1)
	struct Save0Data
	{
		uint32_t farDraw;
		uint32_t lightShadowEffects;
		uint8_t zeros[304];
		uint8_t lives;
		uint8_t lastLevel;
		uint8_t unlocks;
		uint8_t padByte0;
		uint8_t cameraType;
		uint8_t musicVolume;
		uint8_t soundVolume;
		uint8_t padByte1;
		uint16_t unkData1;
		uint16_t unkData2;
		uint16_t health;
		uint8_t padByte2;
		uint8_t tokens[16];
		uint8_t moviesUnlocked[48];
		uint8_t padByte3;
	};
#pragma pack(pop)

	void Init();
	void InitProgressData(Save0Data* save);
	void LoadProgressData(Save0Data* save);
	void SaveToFile(int32_t saveNum, const char* saveName);
	void AddInputEntry(ToyInputCodes inputCode, GameControlId controlId);
	void ClearBindByControlId(GameControlId controlId);
	void ClearBindByInputCode(ToyInputCodes inputCode);
	int32_t GetInputCodeByControlId(GameControlId controlId);
	int32_t GetControlSettingId(ToyInputCodes inputCode);
	void SetLightShadowEffects(uint32_t on);
	void TransferProgressData(SaveManager::Save0Data* saveData);

	extern Save0Data g_save0Data;
	extern Save99Data g_save99Data;
	extern uint32_t g_curLevelTokenData;
	extern uint32_t g_saveUnlocksCache;

	STATIC_ASSERT(sizeof(Save0Data) == 0x188);
	STATIC_ASSERT(sizeof(Save99Data) == 0x188);
	STATIC_ASSERT(sizeof(SaveControlMapping) == 0x8);
}
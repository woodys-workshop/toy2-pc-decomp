#include "CharacterLoader.h"

namespace CharacterLoader
{
	// GLOBAL: TOY2 0x0053EEE0
	BoneTransform g_boneTransforms[300];

	// GLOBAL: TOY2 0x0054717C
	int16_t g_unk54717C;

	// GLOBAL: TOY2 0x00546D78
	int16_t g_unk546D78;

	// GLOBAL: TOY2 0x00547CD0
	int16_t g_unk547CD0;

	// GLOBAL: TOY2 0x0053E4C8
	int32_t g_alternateAllParse[128];

	// GLOBAL: TOY2 0x0053E8C8
	uint8_t* g_charFileDataCache[128];

	// STUB: TOY2 0x0043B9B0
	void Start(int32_t* value, uint8_t** buffer, uint8_t* creatureList) {}

	// STUB: TOY2 0x0043ABC0
	uint16_t* LoadFirstSection(int32_t a1, char a2) { return 0; }

	// STUB: TOY2 0x0043ACA0
	void Load(const char* fileName, int32_t* a2, uint8_t** levelData, int16_t creatureId, uint32_t* a5, CharacterLoader::CharacterBoundData* boundData) {}

	// STUB: TOY2 0x0043AED0
	void LoadBuzzLight(const char* charName, int16_t* boneCursor, void** buffer, int16_t creatureId, int32_t* attachBone, CharacterLoader::CharacterBoundData* boundData) {}

	// STUB: TOY2 0x0043B0C0
	void LoadCharacterData(int32_t* result, uint8_t** levelData, uint8_t* creatureList) {}

	// STUB: TOY2 0x0043C700
	int32_t InitGlobals() { return 0; }

	// STUB: TOY2 0x0043C750
	void LoadSecondSection(int16_t a1) {}

	// STUB: TOY2 0x0043D6B0
	int32_t AlternateAllParse(int32_t boneIdx, int32_t data) { return 0; }

	// STUB: TOY2 0x0043D820
	int32_t LoadAll(const char* fileName, int32_t index, void** buffer) { return 0; }
}


#pragma once

#include "Common.h"
#include "Numerics.h"

namespace CharacterLoader
{
	struct BoneTransform
	{
		Vector3I translation;
		int32_t unkInt4;
		Vector3I16 unkVec4;
		int16_t unkInt6_;
		int16_t unkInt7;
		int16_t unkInt7_;
		int16_t unkInt8;
		uint8_t unkByte1;
		uint8_t track;
		int32_t unkInt9;
		int32_t unkInt10;
		Vector3I16 unkVec5;
		int16_t unkInt11;
		int32_t unkInt13;
		int16_t unkInt14;
		int16_t unkInt14_;
		int32_t unkInt15;
		int32_t unkInt16;
		int32_t unkInt17;
		int32_t unkInt18;
		int32_t unkInt19;
		int32_t unkInt20;
		int32_t unkInt21;
		int32_t unkInt22;
		Matrix3x3I16 rotation;
		int16_t unkInt27_;
	};

	struct CharacterBoundData
	{
		Vector3I16 boundingOffset;
		int16_t boundingSphereRadius;
	};

	// an entry of the table in a .all file
	struct CharacterAllEntry
	{
		int32_t dataSize;
		int32_t x;
		int32_t y;
		int32_t z;
		int32_t type;
		uint8_t pad14[6];
		int16_t trackKind;
		uint8_t pad1C[16];
		int16_t boundX;
		int16_t boundY;
		int16_t boundZ;
		int16_t unk32;
		uint8_t pad34[6];
		int16_t unk3A;
		uint8_t pad3C[6];
		int16_t unk42;
		uint8_t pad44[2];
		uint8_t flags46;
		uint8_t pad47[5];
	};

	extern BoneTransform g_boneTransforms[300];
	extern int32_t g_unk53EECC;
	extern int16_t g_unk53E4C0;
	extern int16_t g_unk53EED8;
	extern uint8_t* g_unk53EEC8;
	extern uint8_t* g_unk53E4C4;
	extern uint8_t* g_unk547CD4[128];
	extern int16_t g_unk54717C;
	extern int16_t g_unk546D78;
	extern int16_t g_unk547CD0;
	extern int32_t g_alternateAllParse[128];
	extern uint8_t* g_charFileDataCache[128];

	void Start(int32_t* value, uint8_t** buffer, uint8_t* creatureList);

	STATIC_ASSERT(sizeof(BoneTransform) == 0x6C);
	STATIC_ASSERT(sizeof(CharacterBoundData) == 0x8);
	STATIC_ASSERT(sizeof(CharacterAllEntry) == 0x4C);
}

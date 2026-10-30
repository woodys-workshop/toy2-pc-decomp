#pragma once

#include <string>
#include <cstdint>
#include <cstddef>
#include <iosfwd>

namespace SaveViewer
{
	struct [[gnu::packed]] SaveControlMapping
	{
		int32_t dInputCode;
		int32_t gameControlId;
	};

	struct [[gnu::packed]] Save99Data
	{
		int32_t unused1;
		int32_t unused2;
		SaveControlMapping mappings[38];
		uint8_t unusedPad[80];
	};

	struct [[gnu::packed]] Save0Data
	{
		uint32_t unkInt1;
		uint32_t unkInt2;
		uint8_t zeros[304];
		uint8_t lives;
		uint8_t lastLevel;
		uint8_t unlocks;
		uint8_t unkByte2_2;
		uint8_t cameraType;
		uint8_t musicVolume;
		uint8_t soundVolume;
		uint8_t unkByte4_2;
		uint16_t unkShort3;
		uint16_t unkShort4;
		uint16_t health;
		uint8_t unkByte5;
		uint8_t tokens[16];
		uint8_t moviesUnlocked[48];
		uint8_t unkByte8;
	};

	static_assert(sizeof(SaveControlMapping) == 8);
	static_assert(sizeof(Save99Data) == 392);
	static_assert(offsetof(Save99Data, mappings) == 8);
	static_assert(offsetof(Save99Data, unusedPad) == 0x138);
	static_assert(sizeof(Save0Data) == 392);
	static_assert(offsetof(Save0Data, lives) == 0x138);
	static_assert(offsetof(Save0Data, tokens) == 0x147);
	static_assert(offsetof(Save0Data, moviesUnlocked) == 0x157);
	static_assert(offsetof(Save0Data, unkByte8) == 0x187);

	enum class SaveType
	{
		ControlData,
		ProgressionData,
	};

	// The caller selects the payload type; the file has no type tag.
	bool readSave(const std::string& savePath, SaveType type);
	// Payload readers require 392 bytes at the current stream position.
	bool readControlData(std::ifstream& file);
	bool readProgressionData(std::ifstream& file);
}

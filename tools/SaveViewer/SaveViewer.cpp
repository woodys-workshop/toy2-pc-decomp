#include "SaveViewer.hpp"
#include "Common/InputMappings.hpp"

#include <algorithm>
#include <bit>
#include <cstdio>
#include <fstream>
#include <filesystem>

static_assert(std::endian::native == std::endian::little);

namespace SaveViewer
{
	bool readSave(const std::string& savePath, SaveType type)
	{
		if (type != SaveType::ControlData && type != SaveType::ProgressionData)
		{
			printf("[SaveViewer] Invalid save type.\n");
			return false;
		}

		const auto filePath = std::filesystem::path(savePath);
		const auto fileName = filePath.filename().string();
		const auto fileExtension = filePath.extension().string();

		if (fileExtension != ".sav")
		{
			printf("[SaveViewer] Not a valid .sav file.\n");
			return false;
		}

		printf("[SaveViewer] Reading sav file -> %s\n", fileName.c_str());

		std::ifstream file(savePath, std::ios::binary | std::ios::ate);
		if (! file.is_open())
		{
			printf("[SaveViewer] Could not open file %s\n", savePath.c_str());
			return false;
		}

		const std::streamsize size = file.tellg();
		const std::streamsize minimumSize = sizeof(int32_t) + sizeof(Save0Data);
		if (size < minimumSize || ! file.seekg(0, std::ios::beg))
		{
			printf("[SaveViewer] Missing header or payload.\n");
			return false;
		}

		int32_t strSize { 0 };
		if (! file.read(reinterpret_cast<char*>(&strSize), sizeof(strSize)))
		{
			printf("[SaveViewer] Failed to read file.\n");
			return false;
		}

		if (strSize < 0 || strSize > size - minimumSize)
		{
			printf("[SaveViewer] Invalid name length or short payload.\n");
			return false;
		}

		std::string saveName(static_cast<size_t>(strSize), '\0');
		if (strSize != 0 && ! file.read(saveName.data(), strSize))
		{
			printf("[SaveViewer] Failed to read save name.\n");
			return false;
		}

		printf("[SaveViewer] String size is %d\n", strSize);
		printf("Save Name: ");
		fwrite(saveName.data(), 1, saveName.size(), stdout);
		printf("\n");

		const bool success = type == SaveType::ControlData ? readControlData(file) : readProgressionData(file);
		if (success && size - minimumSize != strSize)
			printf("[SaveViewer] Trailing file bytes: %lld (not decoded)\n", static_cast<long long>(size - minimumSize - strSize));
		return success;
	}

	bool readProgressionData(std::ifstream& file)
	{
		Save0Data progData {};

		if (! file.read(reinterpret_cast<char*>(&progData), sizeof(Save0Data)))
		{
			printf("[SaveViewer] Failed to read progression data.\n");
			return false;
		}

		printf("------------------ Progression Data ------------------\n");
		printf("unkInt1: %u\n", progData.unkInt1);
		printf("unkInt2: %u\n", progData.unkInt2);
		printf("lives: %u\n", progData.lives);
		printf("lastLevel: %u\n", progData.lastLevel);
		printf("unlocks: %u\n", progData.unlocks);
		printf("unkByte2_2: %u\n", progData.unkByte2_2);
		printf("cameraType: %u\n", progData.cameraType);
		printf("musicVolume: %u\n", progData.musicVolume);
		printf("soundVolume: %u\n", progData.soundVolume);
		printf("unkByte4_2: %u\n", progData.unkByte4_2);
		printf("unkShort3: %u\n", progData.unkShort3);
		printf("unkShort4: %u\n", progData.unkShort4);
		printf("health: %u\n", progData.health);
		printf("unkByte5: %u\n", progData.unkByte5);

		printf("zeros (opaque): [ ");
		for (uint8_t byte : progData.zeros)
			printf("%u ", byte);
		printf("]\n");

		printf("tokens: [ ");
		for (uint8_t token : progData.tokens)
		{
			printf("%u ", token);
		}
		printf("]\n");

		printf("moviesUnlocked: [ ");
		for (uint8_t movie : progData.moviesUnlocked)
			printf("%u ", movie);
		printf("]\n");

		printf("unkByte8: %u\n", progData.unkByte8);
		return true;
	}

	bool readControlData(std::ifstream& file)
	{
		Save99Data controlData {};

		if (! file.read(reinterpret_cast<char*>(&controlData), sizeof(Save99Data)))
		{
			printf("[SaveViewer] Failed to read control data.\n");
			return false;
		}

		printf("------------------ Control Data ------------------\n");
		printf("unused1: %d\n", controlData.unused1);
		printf("unused2: %d\n", controlData.unused2);

		int32_t index = 1;
		int32_t unusedControls = 0;

		for (SaveControlMapping mapping : controlData.mappings)
		{
			if (mapping.dInputCode == -1)
			{
				printf("Mapping %d: input -1 (unused), control %d\n", index, mapping.gameControlId);
				unusedControls++;
				index++;
				continue;
			}

			auto inputMapping = std::find_if(g_inputMapping, g_inputMapping + g_inputMappingCount, [&mapping](const InputMapping& item) {
				return item.id == mapping.dInputCode;
			});

			auto gameControl = std::find_if(g_gameControls, g_gameControls + g_gameControlsCount, [&mapping](const GameControl& item) {
				return item.id == mapping.gameControlId;
			});

			const char* inputName = inputMapping != g_inputMapping + g_inputMappingCount && inputMapping->name ? inputMapping->name : "unknown";
			const char* controlName = gameControl != g_gameControls + g_gameControlsCount && gameControl->name ? gameControl->name : "unknown";
			printf("Mapping %d: input %d (\"%s\") mapped to control %d (\"%s\")\n", index, mapping.dInputCode, inputName, mapping.gameControlId, controlName);
			index++;
		}

		printf("%d Unused Mappings\n", unusedControls);
		printf("unusedPad (opaque): [ ");
		for (uint8_t byte : controlData.unusedPad)
			printf("%u ", byte);
		printf("]\n");
		return true;
	}
}

int32_t main(int32_t argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Usage: SaveViewer <progress|controls> <file.sav>\n");
		return 1;
	}

	SaveViewer::SaveType type;
	const std::string typeName(argv[1]);
	if (typeName == "progress")
		type = SaveViewer::SaveType::ProgressionData;
	else if (typeName == "controls")
		type = SaveViewer::SaveType::ControlData;
	else
	{
		printf("[SaveViewer] Expected save type progress or controls.\n");
		return 1;
	}

	return SaveViewer::readSave(std::string(argv[2]), type) ? 0 : 1;
}

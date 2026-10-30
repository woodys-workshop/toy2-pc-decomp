/*
	----------------------------------------------------
	Toy Story 2 .raw decompressor written by Danny
	3/05/2026
	Decompiled from original game source at 0x0047B170
	----------------------------------------------------
*/

#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <print>
#include <format>

#include "Common/CSVWriter.hpp"
#include "Common/GameStructs.hpp"

namespace fs = std::filesystem;

#define LOBYTE_RD(x) (*((uint8_t*)&(x)))
#define HIBYTE_RD(x) (*((uint8_t*)&(x) + 1))

#define LOWORD_RD(x) (*((uint16_t*)&(x)))
#define HIWORD_RD(x) (*((uint16_t*)&(x) + 1))

// Original function -> Toy2.exe -> 0x0047B170
void DecompressBuffer(uint8_t* p_inBuffer, uint8_t* p_outBuffer)
{
	uint32_t copyLength;
	uint32_t exitFlag;
	uint32_t lengthCode;
	uint32_t distanceHighOrLiteralCount;
	uint32_t extendedLengthBit;
	uint32_t lengthFollowupBit;
	uint32_t hasDistanceHighBits;

	uint8_t* afterHeader = p_inBuffer + 15;
	uint32_t bitAccum = 4 * p_inBuffer[14] + 2;

	do
	{
		while ( true )
		{
			while ( true )
			{
				while ( true )
				{
					bitAccum *= 2;
					uint32_t bitTest = (bitAccum >> 8) & 1;

					if ( ! bitTest )
					{
						bitAccum *= 2;

						*p_outBuffer++ = *afterHeader++;

						bitTest = (bitAccum >> 8) & 1;
					}

					if ( bitTest )
					{
						bitAccum &= 0xFF;

						if ( bitAccum )
							break;

						bitAccum = bitTest + 2 * (*afterHeader++);

						if ( (bitAccum & 0x100) != 0 )
							break;
					}

					*p_outBuffer++ = *afterHeader++;
				}

				LOBYTE_RD(lengthCode) = 2 * bitAccum;
				distanceHighOrLiteralCount = 0;
				copyLength = 2;

				uint32_t lengthCodeBit = ((2 * bitAccum) >> 8) & 1;
				lengthCode &= 0xFF;

				if ( ! lengthCode )
				{
					lengthCode = lengthCodeBit + 2 * (*afterHeader++);
					lengthCodeBit = (lengthCode >> 8) & 1;
				}

				if ( lengthCodeBit )
					break;

				uint32_t shiftedLengthCode = 2 * lengthCode;
				uint32_t shiftedLengthBit = (shiftedLengthCode >> 8) & 1;
				shiftedLengthCode &= 0xFF;

				if ( ! shiftedLengthCode )
				{
					LOBYTE_RD(lengthCodeBit) = (*afterHeader++);
					shiftedLengthCode = shiftedLengthBit + 2 * lengthCodeBit;
					shiftedLengthBit = (shiftedLengthCode >> 8) & 1;
				}

				LOBYTE_RD(bitAccum) = 2 * shiftedLengthCode;
				copyLength = shiftedLengthBit + 4;

				uint32_t extraLengthBit = ((2 * shiftedLengthCode) >> 8) & 1;

				bitAccum &= 0xFF;

				if ( ! bitAccum )
				{
					bitAccum = extraLengthBit + 2 * (*afterHeader++);
					extraLengthBit = (bitAccum >> 8) & 1;
				}

				if ( ! extraLengthBit )
					goto LBL_DECODE_DISTANCE;

				bitAccum *= 2;
				uint32_t finalLengthBit = (bitAccum >> 8) & 1;
				bitAccum &= 0xFF;

				if ( ! bitAccum )
				{
					bitAccum = finalLengthBit + 2 * (*afterHeader++);
					finalLengthBit = (bitAccum >> 8) & 1;
				}

				copyLength = finalLengthBit + 2 * (copyLength - 1);

				if ( copyLength != 9 )
					goto LBL_DECODE_DISTANCE;

				for ( int32_t bitIndex = 3; bitIndex >= 0; --bitIndex )
				{
					bitAccum *= 2;
					uint32_t literalCountBit = (bitAccum >> 8) & 1;
					bitAccum &= 0xFF;

					if ( ! bitAccum )
					{
						bitAccum = literalCountBit + 2 * (*afterHeader++);
						literalCountBit = (bitAccum >> 8) & 1;
					}

					distanceHighOrLiteralCount = literalCountBit + 2 * distanceHighOrLiteralCount;
				}

				*p_outBuffer = *afterHeader;
				p_outBuffer[1] = afterHeader[1];

				p_outBuffer += 4;

				*(p_outBuffer - 2) = afterHeader[2];
				*(p_outBuffer - 1) = afterHeader[3];
				afterHeader += 4;

				uint32_t literalBlockCount = distanceHighOrLiteralCount + 1;

				if ( literalBlockCount >= 0 )
				{
					uint32_t literalBlocksRemaining = literalBlockCount + 1;

					do
					{
						*p_outBuffer = *afterHeader;
						p_outBuffer[1] = afterHeader[1];
						p_outBuffer += 4;

						*(p_outBuffer - 2) = afterHeader[2];
						*(p_outBuffer - 1) = afterHeader[3];

						afterHeader += 4;
						--literalBlocksRemaining;
					}
					while ( literalBlocksRemaining );
				}
			}

			LOBYTE_RD(bitAccum) = 2 * lengthCode;
			lengthFollowupBit = ((2 * lengthCode) >> 8) & 1;
			bitAccum &= 0xFF;

			if ( ! bitAccum )
			{
				bitAccum = lengthFollowupBit + 2 * (*afterHeader++);
				lengthFollowupBit = (bitAccum >> 8) & 1;
			}

			if ( ! lengthFollowupBit )
				goto LBL_COPY_MATCH;

			bitAccum *= 2;
			copyLength = 3;

			extendedLengthBit = (bitAccum >> 8) & 1;
			bitAccum &= 0xFF;

			if ( ! bitAccum )
			{
				bitAccum = extendedLengthBit + 2 * (*afterHeader++);
				extendedLengthBit = (bitAccum >> 8) & 1;
			}

			if ( extendedLengthBit )
				break;

		LBL_DECODE_DISTANCE:

			bitAccum *= 2;
			hasDistanceHighBits = (bitAccum >> 8) & 1;
			bitAccum &= 0xFF;

			if ( ! bitAccum )
			{
				bitAccum = hasDistanceHighBits + 2 * (*afterHeader++);
				hasDistanceHighBits = (bitAccum >> 8) & 1;
			}

			if ( hasDistanceHighBits )
			{
				uint32_t distancePrefixAccum = 2 * bitAccum;
				uint32_t distancePrefixBit = (distancePrefixAccum >> 8) & 1;

				distancePrefixAccum &= 0xFF;

				if ( ! distancePrefixAccum )
				{
					distancePrefixAccum = distancePrefixBit + 2 * (*afterHeader++);
					distancePrefixBit = (distancePrefixAccum >> 8) & 1;
				}

				bitAccum = 2 * distancePrefixAccum;
				uint32_t distanceHighBits = distancePrefixBit;

				uint32_t hasMoreDistanceBits = (bitAccum >> 8) & 1;
				bitAccum &= 0xFF;

				if ( ! bitAccum )
				{
					bitAccum = hasMoreDistanceBits + 2 * (*afterHeader++);
					hasMoreDistanceBits = (bitAccum >> 8) & 1;
				}

				if ( hasMoreDistanceBits )
				{
					uint32_t shifted = 2 * bitAccum;
					uint32_t distanceExtraBit = (shifted >> 8) & 1;
					uint32_t distanceExtraAccum = shifted & 0xFF;

					if ( ! distanceExtraAccum )
					{
						distanceExtraAccum = distanceExtraBit + 2 * (*afterHeader++);
						distanceExtraBit = (distanceExtraAccum >> 8) & 1;
					}

					distanceHighBits = (distanceExtraBit + 2 * distanceHighBits) | 4;

					shifted = 2 * distanceExtraAccum;
					uint32_t distanceTerminatorBit = (shifted >> 8) & 1;
					bitAccum = shifted & 0xFF;

					if ( ! bitAccum )
					{
						bitAccum = distanceTerminatorBit + 2 * (*afterHeader++);
						distanceTerminatorBit = (bitAccum >> 8) & 1;
					}

					if ( distanceTerminatorBit )
						goto LBL_COMMIT_DISTANCE;
				}
				else
				{
					if ( distanceHighBits )
					{
					LBL_COMMIT_DISTANCE:

						int16_t distanceHighWord = 0;
						HIBYTE_RD(distanceHighWord) = distanceHighBits;
						LOWORD_RD(distanceHighOrLiteralCount) = distanceHighWord | (distanceHighBits >> 8);

						goto LBL_COPY_MATCH;
					}

					distanceHighBits = 1;
				}

				bitAccum *= 2;
				uint32_t distanceTailBit = (bitAccum >> 8) & 1;
				bitAccum &= 0xFF;

				if ( ! bitAccum )
				{
					bitAccum = distanceTailBit + 2 * (*afterHeader++);
					distanceTailBit = (bitAccum >> 8) & 1;
				}

				distanceHighBits = distanceTailBit + 2 * distanceHighBits;
				goto LBL_COMMIT_DISTANCE;
			}

		LBL_COPY_MATCH:

			int32_t matchDistance = distanceHighOrLiteralCount & 0xFF00 | (*afterHeader++);
			uint8_t* matchSource = &p_outBuffer[-matchDistance - 1];

			if ( copyLength & 1 )
			{
				*p_outBuffer++ = *matchSource++;
			}

			int32_t pairCopyCount = (copyLength >> 1) - 1;

			if ( matchDistance )
			{
				p_outBuffer += 2;
				uint16_t* matchSourceWords = (uint16_t*)(matchSource + 2);

				*(p_outBuffer - 1) = *((uint8_t*)matchSourceWords - 1);
				*(p_outBuffer - 2) = *matchSource;

				int32_t wordCopyLoopCount = pairCopyCount - 1;

				if ( wordCopyLoopCount >= 0 )
				{
					uint32_t wordCopiesRemaining = wordCopyLoopCount + 1;

					do
					{
						*(uint16_t*)p_outBuffer = *matchSourceWords++;
						p_outBuffer += 2;
						--wordCopiesRemaining;
					}
					while ( wordCopiesRemaining );
				}
			}
			else
			{
				p_outBuffer += 2;
				uint8_t repeatedByte = *matchSource;
				int32_t repeatPairLoopCount = pairCopyCount - 1;

				*(p_outBuffer - 2) = repeatedByte;
				*(p_outBuffer - 1) = repeatedByte;

				if ( repeatPairLoopCount >= 0 )
				{
					int32_t repeatPairsRemaining = repeatPairLoopCount + 1;

					do
					{
						*p_outBuffer = repeatedByte;
						p_outBuffer[1] = repeatedByte;
						p_outBuffer += 2;
						--repeatPairsRemaining;
					}
					while ( repeatPairsRemaining );
				}
			}
		}

		int32_t extendedLengthByte = (*afterHeader++);

		if ( extendedLengthByte )
		{
			copyLength = extendedLengthByte + 8;
			goto LBL_DECODE_DISTANCE;
		}

		bitAccum *= 2;
		exitFlag = (bitAccum >> 8) & 1;
		bitAccum &= 0xFF;

		if ( ! bitAccum )
		{
			bitAccum = exitFlag + 2 * (*afterHeader++);
			exitFlag = (bitAccum >> 8) & 1;
		}
	}
	while ( exitFlag );
}

bool dumpFile(const fs::path& inputFilePath, bool writeType35Dump, bool writePacket)
{
	auto toBigEndian = [](uint32_t num) -> uint32_t {
		return ((num & 0x000000ff) << 24) | ((num & 0x0000ff00) << 8) | ((num & 0x00ff0000) >> 8) | ((num & 0xff000000) >> 24);
	};

	std::ifstream inFile(inputFilePath, std::ios::binary);
	if ( ! inFile )
	{
		std::println("Failed to open {}", inputFilePath.string());
		return false;
	}

	uint32_t beUncompressedSize = 0;
	uint32_t beCompressedSize = 0;
	uint32_t counter = 0;

	inFile.read(reinterpret_cast<char*>(&beUncompressedSize), sizeof(beUncompressedSize));

	while ( inFile && beUncompressedSize != 0xFFFFFFFF )
	{
		uint32_t uncompressedSize = toBigEndian(beUncompressedSize);

		inFile.read(reinterpret_cast<char*>(&beCompressedSize), sizeof(beCompressedSize));
		if ( ! inFile )
			break;

		uint32_t compressedSize = toBigEndian(beCompressedSize);

		std::vector<uint8_t> compressedBuffer(compressedSize + 14);
		std::vector<uint8_t> decompressedBuffer(uncompressedSize);

		std::memcpy(compressedBuffer.data(), &beUncompressedSize, 4);
		std::memcpy(compressedBuffer.data() + 4, &beCompressedSize, 4);

		inFile.read(reinterpret_cast<char*>(compressedBuffer.data() + 8), 6 + compressedSize);

		if ( ! inFile )
		{
			std::println("Failed to read compressed packet {}", counter);
			return false;
		}

		std::println("Inflating {}->{}", compressedSize, uncompressedSize);

		DecompressBuffer(compressedBuffer.data(), decompressedBuffer.data());

		if ( decompressedBuffer[0] == 35 )
		{
			std::string parentName = inputFilePath.parent_path().stem().string();
			std::string csvName = parentName + "_" + inputFilePath.stem().string() + "_type35_dump.csv";

			CSVWriter writer(csvName);

			if ( ! writer.open() )
			{
				throw std::runtime_error("Could not open CSV file for writing");
				return false;
			}

			CreatureRam* creatureRamList = reinterpret_cast<CreatureRam*>(&decompressedBuffer[4]);

			// clang-format off
			writer.writeLine<std::string>(
			    "CreatureIndex",
			    "PosX",
			    "PosY",
			    "PosZ",
				"CreatureId",
				"MovCtrl",
				"RotSpeed",
				"InitialFacingAngle",
				"ActorPhase",
				"UnkByte",
				"ActorFlags",
				"BoundHalfX",
				"BoundHalfZ",
				"BoundAngle",
				"DefenseMode",
				"LatSpeedNoTarget",
				"LatSpeedTarget",
				"SpeedNoTarget",
				"SpeedTarget"
			);

			auto writeCreatureRam = [&writer](const CreatureRam& t, uint64_t index) {
				writer.writeLine(
					(index + 1),
					t.pos.x,
					t.pos.y,
					t.pos.z,
					static_cast<int32_t>(t.creatureId),
					static_cast<int32_t>(t.movCtrl),
					static_cast<int32_t>(t.rotSpeed),
					static_cast<int32_t>(t.initialFacingAngle),
					static_cast<int32_t>(t.entCtrl.actorPhase),
					static_cast<int32_t>(t.entCtrl.unkByte),
					t.entCtrl.actorFlags,
					t.boundHalfX,
					t.boundHalfZ,
					t.boundAngle,
					t.defenseMode,
					static_cast<int32_t>(t.latSpeedNoTarget),
					static_cast<int32_t>(t.latSpeedTarget),
					static_cast<int32_t>(t.speedNoTarget),
					static_cast<int32_t>(t.speedTarget)
				);
			};
			// clang-format on

			for ( int32_t idx = 0; idx < kMaxActors; idx++ )
				writeCreatureRam(creatureRamList[idx], idx);
		}

		if ( writePacket )
		{
			fs::path outputDir = inputFilePath.parent_path() / inputFilePath.stem();
			fs::path outPath = outputDir / std::format("packet_{:03}.bin", ++counter);

			if ( ! fs::exists(outputDir) && ! fs::create_directories(outputDir) )
			{
				std::println("Failed to make directory {}", outputDir.string());
				return false;
			}

			std::ofstream outFile(outPath, std::ios::binary);
			if ( ! outFile )
			{
				std::println("Failed to write {}", outPath.string());
				return false;
			}

			outFile.write(reinterpret_cast<const char*>(decompressedBuffer.data()), decompressedBuffer.size());
		}


		inFile.read(reinterpret_cast<char*>(&beUncompressedSize), sizeof(beUncompressedSize));
	}

	return true;
}

int main(int argc, char* argv[])
{
	if ( argc < 2 )
	{
		std::println("Usage: {} <input_file>", argv[0]);
		return 1;
	}

	fs::path inputFilePath(argv[1]);

	if ( fs::is_directory(inputFilePath) )
	{
		std::println("Running directory code!\n");

		for ( const auto& entry : fs::recursive_directory_iterator(inputFilePath) )
		{
			if ( ! entry.is_regular_file() )
				continue;

			if ( entry.path().extension() != ".raw" )
				continue;

			std::println("Found: {}", entry.path().string());

			if ( ! dumpFile(entry.path(), true, false) )
				break;
		}
	}
	else
	{
		std::println("Running file code!\n");
		dumpFile(inputFilePath, false, true);
	}

	return 0;
}

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
#include <algorithm>
#include <stdexcept>

namespace fs = std::filesystem;

// Original function -> Toy2.exe -> 0x0047B170
void DecompressBuffer(uint8_t* inBuffer, uint8_t* outBuffer)
{
	uint8_t* src;
	uint8_t* dst;
	uint32_t scratch;
	uint32_t carry;
	uint32_t length;
	uint32_t distance;
	uint32_t accum;
	uint8_t* matchPtr;
	int32_t bitIndex;
	uint32_t tmpAccum;
	uint32_t distHigh;
	uint8_t byteA;
	uint8_t byteB;

	src = inBuffer + 14;
	dst = outBuffer;
	accum = *src++;
	accum = 4 * accum + 2;

L1A0:
	while (1)
	{
		accum *= 2;
		carry = (accum >> 8) & 1;

		if (carry || (accum *= 2, carry = *src++, *dst++ = (uint8_t)carry, carry = (accum >> 8) & 1, carry))
		{
			accum &= 0xFF;

			if (accum)
				break;

			accum = *src++;
			accum = carry + 2 * accum;

			if (accum & 0x100)
				break;
		}

		carry = *src++;
		*dst++ = (uint8_t)carry;
	}

	carry = 2 * accum;
	distance = 0;
	accum = carry;
	length = 2;
	accum = (accum >> 8) & 1;
	carry &= 0xFF;

	if (! carry)
	{
		carry = *src++;
		carry = accum + 2 * carry;
		scratch = (carry >> 8) & 1;
		accum = scratch;
	}

	if (! accum)
	{
		carry *= 2;
		length = (carry >> 8) & 1;
		carry &= 0xFF;

		if (! carry)
		{
			accum = *src++;
			carry = length + 2 * accum;
			accum = (carry >> 8) & 1;
			length = accum;
		}

		accum = 2 * carry;
		length += 4;
		carry = (accum >> 8) & 1;
		accum &= 0xFF;

		if (! accum)
		{
			accum = *src++;
			accum = carry + 2 * accum;
			carry = (accum >> 8) & 1;
		}

		if (! carry)
			goto L3A1;

		accum *= 2;
		length--;
		carry = (accum >> 8) & 1;
		accum &= 0xFF;

		if (! accum)
		{
			accum = *src++;
			accum = carry + 2 * accum;
			scratch = (accum >> 8) & 1;
			carry = scratch;
		}

		length = carry + 2 * length;

		if (length - 9)
			goto L3A1;

		bitIndex = 3;

	L298:
		accum *= 2;
		carry = (accum >> 8) & 1;
		accum &= 0xFF;

		if (! accum)
		{
			accum = *src++;
			accum = carry + 2 * accum;
			carry = (accum >> 8) & 1;
		}

		bitIndex--;
		distance = carry + 2 * distance;

		if (bitIndex >= 0)
			goto L298;

		byteA = src[0];
		carry = src[1];
		dst[0] = byteA;
		dst[1] = (uint8_t)carry;
		byteA = src[2];
		carry = src[3];
		dst += 4;
		src += 4;
		dst[-2] = byteA;
		dst[-1] = (uint8_t)carry;

		distance++;

		if ((int32_t)distance < 0)
			goto L1A0;

		distance++;

		do
		{
			byteA = src[0];
			carry = src[1];
			dst[0] = byteA;
			dst[1] = (uint8_t)carry;
			byteA = src[2];
			carry = src[3];
			dst += 4;
			dst[-2] = byteA;
			dst[-1] = (uint8_t)carry;
			src += 4;
			distance--;
		} while (distance);

		goto L1A0;
	}

	accum = carry + carry;
	carry = (accum >> 8) & 1;
	accum &= 0xFF;

	if (! accum)
	{
		accum = *src++;
		accum = carry + 2 * accum;
		carry = (accum >> 8) & 1;
	}

	if (! carry)
		goto L4A4;

	accum *= 2;
	length = 3;
	carry = (accum >> 8) & 1;
	accum &= 0xFF;

	if (! accum)
	{
		accum = *src++;
		accum = carry + 2 * accum;
		scratch = (accum >> 8) & 1;
		carry = scratch;
	}

	if (! carry)
		goto L3A1;

	length = *src++;

	if (! length)
	{
		accum *= 2;
		carry = (accum >> 8) & 1;
		accum &= 0xFF;

		if (! accum)
		{
			accum = *src++;
			accum = carry + 2 * accum;
			carry = (accum >> 8) & 1;
		}

		if (carry)
			goto L1A0;

		return;
	}

	length += 8;

L3A1:
	accum *= 2;
	carry = (accum >> 8) & 1;
	accum &= 0xFF;

	if (! accum)
	{
		accum = *src++;
		accum = carry + 2 * accum;
		scratch = (accum >> 8) & 1;
		carry = scratch;
	}

	if (! carry)
		goto L4A4;

	accum *= 2;
	carry = (accum >> 8) & 1;
	accum &= 0xFF;

	if (! accum)
	{
		accum = *src++;
		accum = carry + 2 * accum;
		carry = (accum >> 8) & 1;
	}

	accum *= 2;
	distHigh = carry;
	carry = (accum >> 8) & 1;
	accum &= 0xFF;

	if (! accum)
	{
		accum = *src++;
		accum = carry + 2 * accum;
		carry = (accum >> 8) & 1;
	}

	if (carry)
	{
		tmpAccum = 2 * accum;
		carry = (tmpAccum >> 8) & 1;
		tmpAccum &= 0xFF;

		if (! tmpAccum)
		{
			accum = *src++;
			tmpAccum = carry + 2 * accum;
			accum = (tmpAccum >> 8) & 1;
			carry = accum;
		}

		accum = 2 * tmpAccum;
		distHigh = carry + 2 * distHigh;
		carry = accum;
		distHigh |= 4;
		carry = (carry >> 8) & 1;
		accum &= 0xFF;

		if (! accum)
		{
			accum = *src++;
			accum = carry + 2 * accum;
			carry = (accum >> 8) & 1;
		}

		if (carry)
			goto L499;
	}
	else
	{
		if (distHigh)
			goto L499;

		distHigh = 1;
	}

	accum *= 2;
	tmpAccum = (accum >> 8) & 1;
	accum &= 0xFF;

	if (! accum)
	{
		accum = *src++;
		accum = tmpAccum + 2 * accum;
		carry = (accum >> 8) & 1;
		tmpAccum = carry;
	}

	distHigh = tmpAccum + 2 * distHigh;

L499:
	scratch = distHigh;
	carry = 0;
	carry = (carry & 0xFFFF00FF) | ((scratch & 0xFF) << 8);
	scratch >>= 8;
	scratch |= carry;
	distance = scratch;

L4A4:
	distance = (distance & 0xFF00) | *src++;
	matchPtr = dst - distance - 1;
	scratch = length & 1;
	length >>= 1;

	if (scratch)
	{
		scratch = *matchPtr++;
		*dst++ = (uint8_t)scratch;
	}

	length--;

	if (distance)
	{
		scratch = matchPtr[0];
		dst += 2;
		matchPtr += 2;
		byteA = matchPtr[-1];
		dst[-1] = byteA;
		dst[-2] = (uint8_t)scratch;
		length--;

		if ((int32_t)length < 0)
			goto L1A0;

		distance = length + 1;

		do
		{
			byteB = matchPtr[1];
			scratch = matchPtr[0];
			dst[1] = byteB;
			dst[0] = (uint8_t)scratch;
			matchPtr += 2;
			dst += 2;
			distance--;
		} while (distance);

		goto L1A0;
	}
	else
	{
		scratch = *matchPtr;
		carry = scratch | (distance & 0xFF00);
		dst += 2;
		length--;
		dst[-2] = (uint8_t)carry;
		dst[-1] = (uint8_t)carry;

		if ((int32_t)length < 0)
			goto L1A0;

		scratch = length + 1;

		do
		{
			dst[0] = (uint8_t)carry;
			dst[1] = (uint8_t)carry;
			dst += 2;
			scratch--;
		} while (scratch);

		goto L1A0;
	}
}

uint32_t ReadLE32(const uint8_t* data)
{
	return uint32_t(data[0]) | (uint32_t(data[1]) << 8) | (uint32_t(data[2]) << 16) | (uint32_t(data[3]) << 24);
}

uint32_t ReadBE32(const uint8_t* data)
{
	return uint32_t(data[3]) | (uint32_t(data[2]) << 8) | (uint32_t(data[1]) << 16) | (uint32_t(data[0]) << 24);
}

void WriteType35(const fs::path& path, const std::vector<uint8_t>& payload)
{
	std::ofstream output;
	output.exceptions(std::ios::failbit | std::ios::badbit);
	output.open(path, std::ios::binary);
	output << "CreatureIndex,PosX,PosY,PosZ,CreatureId,MovCtrl,RotSpeed,InitialFacingAngle,ActorPhase,UnkByte,ActorFlags,"
	          "BoundHalfX,BoundHalfZ,BoundAngle,DefenseMode,LatSpeedNoTarget,LatSpeedTarget,SpeedNoTarget,SpeedTarget\n";
	// The file supplies complete 32-byte records; the game's retained arena tail is not file data.
	size_t count = std::min<size_t>(64, (payload.size() - 4) / 32);
	for ( size_t index = 0; index < count; ++index )
	{
		const uint8_t* record = payload.data() + 4 + 32 * index;
		output << index + 1;
		for ( size_t offset = 0; offset < 12; offset += 4 )
			output << ',' << static_cast<int32_t>(ReadLE32(record + offset));
		for ( size_t offset = 12; offset < 18; ++offset )
			output << ',' << unsigned(record[offset]);
		output << ',' << (unsigned(record[18]) | (unsigned(record[19]) << 8));
		for ( size_t offset = 20; offset < 28; offset += 2 )
			output << ',' << static_cast<int16_t>(unsigned(record[offset]) | (unsigned(record[offset + 1]) << 8));
		for ( size_t offset = 28; offset < 32; ++offset )
			output << ',' << unsigned(record[offset]);
		output << '\n';
	}
	output.close();
}

void dumpFile(const fs::path& inputFilePath, bool writeType35Dump, bool writePacket)
{
	std::ifstream inFile(inputFilePath, std::ios::binary);
	if ( ! inFile )
		throw std::runtime_error("Could not open input file");
	uintmax_t remaining = fs::file_size(inputFilePath);
	size_t counter = 0;
	auto read = [&](uint8_t* buffer, size_t size) {
		if ( size > remaining || ! inFile.read(reinterpret_cast<char*>(buffer), size) )
			throw std::runtime_error("Truncated RAW header or packet");
		remaining -= size;
	};

	while ( true )
	{
		uint8_t header[14];
		read(header, 4);
		uint32_t uncompressedSize = ReadBE32(header);
		if ( uncompressedSize == 0xFFFFFFFF )
			return;
		read(header + 4, 10);
		uint32_t compressedSize = ReadBE32(header + 4);
		// Each match emits at most 263 bytes and consumes at least one body byte.
		if ( compressedSize == 0 || compressedSize > remaining || uint64_t(compressedSize) + 14 > SIZE_MAX ||
		     uncompressedSize > uint64_t(compressedSize) * 263 )
			throw std::runtime_error("Invalid RAW packet size");
		std::vector<uint8_t> compressedBuffer(size_t(compressedSize) + 14);
		std::vector<uint8_t> decompressedBuffer(std::max<size_t>(uncompressedSize, 1));
		std::memcpy(compressedBuffer.data(), header, 14);
		read(compressedBuffer.data() + 14, compressedSize);
		++counter;
		std::println("{}: packet {}: inflating {}->{}", inputFilePath.string(), counter, compressedSize, uncompressedSize);
		DecompressBuffer(compressedBuffer.data(), decompressedBuffer.data());
		decompressedBuffer.resize(uncompressedSize);

		if ( writeType35Dump && uncompressedSize >= 4 && ReadLE32(decompressedBuffer.data()) == 35 )
			WriteType35(inputFilePath.parent_path() / std::format("{}_packet_{:03}_type35_dump.csv", inputFilePath.stem().string(), counter),
			            decompressedBuffer);

		if ( writePacket )
		{
			fs::path outputDir = inputFilePath.parent_path() / inputFilePath.stem();
			fs::path outPath = outputDir / std::format("packet_{:03}.bin", counter);
			fs::create_directories(outputDir);
			std::ofstream outFile;
			outFile.exceptions(std::ios::failbit | std::ios::badbit);
			outFile.open(outPath, std::ios::binary);
			outFile.write(reinterpret_cast<const char*>(decompressedBuffer.data()), decompressedBuffer.size());
			outFile.close();
		}
	}
}

int main(int argc, char* argv[])
{
	if ( argc < 2 || std::string(argv[1]) == "--help" )
	{
		std::println("Usage: {} <input_file_or_directory> [--csv] [--packets]", argv[0]);
		std::println("Default: files export packets; directories recursively export type35 CSVs. Explicit flags select outputs.");
		return argc < 2 ? 1 : 0;
	}

	fs::path inputFilePath(argv[1]);
	try
	{
		bool directory = fs::is_directory(inputFilePath);
		bool writeType35Dump = argc == 2 && directory;
		bool writePacket = argc == 2 && ! directory;
		for ( int index = 2; index < argc; ++index )
		{
			std::string option(argv[index]);
			if ( option == "--csv" )
				writeType35Dump = true;
			else if ( option == "--packets" )
				writePacket = true;
			else
				throw std::runtime_error("Unknown option: " + option);
		}
		std::vector<fs::path> inputs;
		if ( directory )
		{
			for ( const auto& entry : fs::recursive_directory_iterator(inputFilePath) )
				if ( entry.is_regular_file() && (entry.path().extension() == ".raw" || entry.path().extension() == ".RAW") )
					inputs.push_back(entry.path());
			std::sort(inputs.begin(), inputs.end());
		}
		else
			inputs.push_back(inputFilePath);
		for ( const auto& input : inputs )
		{
			inputFilePath = input;
			dumpFile(input, writeType35Dump, writePacket);
		}
		return 0;
	}
	catch ( const std::exception& error )
	{
		std::cerr << inputFilePath << ": " << error.what() << '\n';
		return 1;
	}
}

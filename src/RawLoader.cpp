#include "RawLoader.h"
#include "Toy2/Levels.h"
#include "Toy2/Toy2.h"
#include "FileUtils.h"
#include "Logger.h"
#include "Toy2/Actor.h"

#include <WINDOWS.H>

namespace RawLoader
{
	// GLOBAL: TOY2 0x005D2BE0
	uint8_t* g_resumePacketPtr;

	// GLOBAL: TOY2 0x0052AFD8
	CreatureListRam g_creatureListRam[64];

	// GLOBAL: TOY2 0x0055A0F4
	int32_t g_unusedBuffer1[8];

	// GLOBAL: TOY2 0x005D2AB8
	uint8_t g_unusedBuffer2[32];

	// GLOBAL: TOY2 0x00703C04
	int32_t g_unusedBuffer3[129];

	// GLOBAL: TOY2 0x00559EDC
	uint8_t g_unusedBuffer4[516];

	// GLOBAL: TOY2 0x0055A11C
	int32_t g_unused1;

	// GLOBAL: TOY2 0x00703E08
	int16_t g_unused2;

	// GLOBAL: TOY2 0x0055A13C
	int16_t g_unused3;

	// GLOBAL: TOY2 0x00557700
	uint8_t* g_unused5;

	// GLOBAL: TOY2 0x005D2BE4
	uint16_t* g_unused4;

	// GLOBAL: TOY2 0x00703BE8
	uint16_t g_type36Data1[4];

	// GLOBAL: TOY2 0x004F7408
	uint8_t g_unusedBuffer5[12] = { 32, 0, 255, 32, 0, 0, 0, 0, 0, 164, 0, 201 };

	// FUNCTION: TOY2 0x0047B170
	void DecompressBuffer(uint8_t* inBuffer, uint8_t* outBuffer)
	{
		// Decompression is based on RNC ProPack Method 2. Normally these files
		// have a standard 18 byte header, which looks like it has been replaced
		// in Toy 2 with a custom 15 byte header.

		// this method is incredibly hard to instruction match, this current version
		// is no where near what the original looks like, but it works. Its a future
		// me problem to match it for now.

		typedef union
		{
			uint32_t dword;
			uint16_t word[2];
			uint8_t byte[4];
		} RawReg;

		uint32_t copyLength;
		uint32_t exitFlag;
		RawReg lengthCode;
		RawReg distanceHighOrLiteralCount;
		uint32_t extendedLengthBit;
		uint32_t lengthFollowupBit;
		uint32_t hasDistanceHighBits;

		uint8_t* afterHeader = inBuffer + 15;
		RawReg bitAccum;
		bitAccum.dword = 4 * inBuffer[14] + 2;

		do
		{
			while (true)
			{
				while (true)
				{
					while (true)
					{
						bitAccum.dword *= 2;
						uint32_t bitTest = (bitAccum.dword >> 8) & 1;

						if (! bitTest)
						{
							bitAccum.dword *= 2;

							*outBuffer++ = *afterHeader++;

							bitTest = (bitAccum.dword >> 8) & 1;
						}

						if (bitTest)
						{
							bitAccum.dword &= 0xFF;

							if (bitAccum.dword)
								break;

							bitAccum.dword = bitTest + 2 * (*afterHeader++);

							if ((bitAccum.dword & 0x100) != 0)
								break;
						}

						*outBuffer++ = *afterHeader++;
					}

					lengthCode.byte[0] = 2 * bitAccum.dword;
					distanceHighOrLiteralCount.dword = 0;
					copyLength = 2;

					uint32_t lengthCodeBit = ((2 * bitAccum.dword) >> 8) & 1;
					lengthCode.dword &= 0xFF;

					if (! lengthCode.dword)
					{
						lengthCode.dword = lengthCodeBit + 2 * (*afterHeader++);
						lengthCodeBit = (lengthCode.dword >> 8) & 1;
					}

					if (lengthCodeBit)
						break;

					uint32_t shiftedLengthCode = 2 * lengthCode.dword;
					uint32_t shiftedLengthBit = (shiftedLengthCode >> 8) & 1;
					shiftedLengthCode &= 0xFF;

					if (! shiftedLengthCode)
					{
						RawReg lengthCodeBitReg;
						lengthCodeBitReg.byte[0] = (*afterHeader++);
						lengthCodeBit = lengthCodeBitReg.dword; // only low byte was written
						shiftedLengthCode = shiftedLengthBit + 2 * lengthCodeBit;
						shiftedLengthBit = (shiftedLengthCode >> 8) & 1;
					}

					bitAccum.byte[0] = 2 * shiftedLengthCode;
					copyLength = shiftedLengthBit + 4;

					uint32_t extraLengthBit = ((2 * shiftedLengthCode) >> 8) & 1;

					bitAccum.dword &= 0xFF;

					if (! bitAccum.dword)
					{
						bitAccum.dword = extraLengthBit + 2 * (*afterHeader++);
						extraLengthBit = (bitAccum.dword >> 8) & 1;
					}

					if (! extraLengthBit)
						goto LBL_DECODE_DISTANCE;

					bitAccum.dword *= 2;
					uint32_t finalLengthBit = (bitAccum.dword >> 8) & 1;
					bitAccum.dword &= 0xFF;

					if (! bitAccum.dword)
					{
						bitAccum.dword = finalLengthBit + 2 * (*afterHeader++);
						finalLengthBit = (bitAccum.dword >> 8) & 1;
					}

					copyLength = finalLengthBit + 2 * (copyLength - 1);

					if (copyLength != 9)
						goto LBL_DECODE_DISTANCE;

					for (int32_t bitIndex = 3; bitIndex >= 0; --bitIndex)
					{
						bitAccum.dword *= 2;
						uint32_t literalCountBit = (bitAccum.dword >> 8) & 1;
						bitAccum.dword &= 0xFF;

						if (! bitAccum.dword)
						{
							bitAccum.dword = literalCountBit + 2 * (*afterHeader++);
							literalCountBit = (bitAccum.dword >> 8) & 1;
						}

						distanceHighOrLiteralCount.dword = literalCountBit + 2 * distanceHighOrLiteralCount.dword;
					}

					outBuffer[0] = afterHeader[0];
					outBuffer[1] = afterHeader[1];
					outBuffer[2] = afterHeader[2];
					outBuffer[3] = afterHeader[3];

					outBuffer += 4;
					afterHeader += 4;

					uint32_t literalBlockCount = distanceHighOrLiteralCount.dword + 1;
					uint32_t literalBlocksRemaining = literalBlockCount + 1;

					do
					{
						outBuffer[0] = afterHeader[0];
						outBuffer[1] = afterHeader[1];
						outBuffer[2] = afterHeader[2];
						outBuffer[3] = afterHeader[3];

						outBuffer += 4;
						afterHeader += 4;
						--literalBlocksRemaining;
					} while (literalBlocksRemaining);
				}

				bitAccum.byte[0] = 2 * lengthCode.dword;
				lengthFollowupBit = ((2 * lengthCode.dword) >> 8) & 1;
				bitAccum.dword &= 0xFF;

				if (! bitAccum.dword)
				{
					bitAccum.dword = lengthFollowupBit + 2 * (*afterHeader++);
					lengthFollowupBit = (bitAccum.dword >> 8) & 1;
				}

				if (! lengthFollowupBit)
					goto LBL_COPY_MATCH;

				bitAccum.dword *= 2;
				copyLength = 3;

				extendedLengthBit = (bitAccum.dword >> 8) & 1;
				bitAccum.dword &= 0xFF;

				if (! bitAccum.dword)
				{
					bitAccum.dword = extendedLengthBit + 2 * (*afterHeader++);
					extendedLengthBit = (bitAccum.dword >> 8) & 1;
				}

				if (extendedLengthBit)
					break;

			LBL_DECODE_DISTANCE:

				bitAccum.dword *= 2;
				hasDistanceHighBits = (bitAccum.dword >> 8) & 1;
				bitAccum.dword &= 0xFF;

				if (! bitAccum.dword)
				{
					bitAccum.dword = hasDistanceHighBits + 2 * (*afterHeader++);
					hasDistanceHighBits = (bitAccum.dword >> 8) & 1;
				}

				if (hasDistanceHighBits)
				{
					uint32_t distancePrefixAccum = 2 * bitAccum.dword;
					uint32_t distancePrefixBit = (distancePrefixAccum >> 8) & 1;

					distancePrefixAccum &= 0xFF;

					if (! distancePrefixAccum)
					{
						distancePrefixAccum = distancePrefixBit + 2 * (*afterHeader++);
						distancePrefixBit = (distancePrefixAccum >> 8) & 1;
					}

					bitAccum.dword = 2 * distancePrefixAccum;
					uint32_t distanceHighBits = distancePrefixBit;

					uint32_t hasMoreDistanceBits = (bitAccum.dword >> 8) & 1;
					bitAccum.dword &= 0xFF;

					if (! bitAccum.dword)
					{
						bitAccum.dword = hasMoreDistanceBits + 2 * (*afterHeader++);
						hasMoreDistanceBits = (bitAccum.dword >> 8) & 1;
					}

					if (hasMoreDistanceBits)
					{
						uint32_t shifted = 2 * bitAccum.dword;
						uint32_t distanceExtraBit = (shifted >> 8) & 1;
						uint32_t distanceExtraAccum = shifted & 0xFF;

						if (! distanceExtraAccum)
						{
							distanceExtraAccum = distanceExtraBit + 2 * (*afterHeader++);
							distanceExtraBit = (distanceExtraAccum >> 8) & 1;
						}

						distanceHighBits = (distanceExtraBit + 2 * distanceHighBits) | 4;

						shifted = 2 * distanceExtraAccum;
						uint32_t distanceTerminatorBit = (shifted >> 8) & 1;
						bitAccum.dword = shifted & 0xFF;

						if (! bitAccum.dword)
						{
							bitAccum.dword = distanceTerminatorBit + 2 * (*afterHeader++);
							distanceTerminatorBit = (bitAccum.dword >> 8) & 1;
						}

						if (distanceTerminatorBit)
							goto LBL_COMMIT_DISTANCE;
					}
					else
					{
						if (distanceHighBits)
						{
						LBL_COMMIT_DISTANCE:

							RawReg distanceHighWord;
							distanceHighWord.dword = 0;
							distanceHighWord.byte[1] = distanceHighBits;
							distanceHighOrLiteralCount.word[0] = distanceHighWord.word[0] | (distanceHighBits >> 8);

							goto LBL_COPY_MATCH;
						}

						distanceHighBits = 1;
					}

					bitAccum.dword *= 2;
					uint32_t distanceTailBit = (bitAccum.dword >> 8) & 1;
					bitAccum.dword &= 0xFF;

					if (! bitAccum.dword)
					{
						bitAccum.dword = distanceTailBit + 2 * (*afterHeader++);
						distanceTailBit = (bitAccum.dword >> 8) & 1;
					}

					distanceHighBits = distanceTailBit + 2 * distanceHighBits;
					goto LBL_COMMIT_DISTANCE;
				}

			LBL_COPY_MATCH:

				int32_t matchDistance = (distanceHighOrLiteralCount.dword & 0xFF00) | (*afterHeader++);
				uint8_t* matchSource = &outBuffer[-matchDistance - 1];

				if (copyLength & 1)
				{
					*outBuffer++ = *matchSource++;
				}

				int32_t pairCopyCount = (copyLength >> 1) - 1;

				if (matchDistance)
				{
					outBuffer += 2;
					uint16_t* matchSourceWords = (uint16_t*)(matchSource + 2);

					*(outBuffer - 1) = *((uint8_t*)matchSourceWords - 1);
					*(outBuffer - 2) = *matchSource;

					int32_t wordCopyLoopCount = pairCopyCount - 1;

					if (wordCopyLoopCount >= 0)
					{
						uint32_t wordCopiesRemaining = wordCopyLoopCount + 1;

						do
						{
							*(uint16_t*)outBuffer = *matchSourceWords++;
							outBuffer += 2;
							--wordCopiesRemaining;
						} while (wordCopiesRemaining);
					}
				}
				else
				{
					outBuffer += 2;
					uint8_t repeatedByte = *matchSource;
					int32_t repeatPairLoopCount = pairCopyCount - 1;

					*(outBuffer - 2) = repeatedByte;
					*(outBuffer - 1) = repeatedByte;

					if (repeatPairLoopCount >= 0)
					{
						int32_t repeatPairsRemaining = repeatPairLoopCount + 1;

						do
						{
							*outBuffer = repeatedByte;
							outBuffer[1] = repeatedByte;
							outBuffer += 2;
							--repeatPairsRemaining;
						} while (repeatPairsRemaining);
					}
				}
			}

			int32_t extendedLengthByte = (*afterHeader++);

			if (extendedLengthByte)
			{
				copyLength = extendedLengthByte + 8;
				goto LBL_DECODE_DISTANCE;
			}

			bitAccum.dword *= 2;
			exitFlag = (bitAccum.dword >> 8) & 1;
			bitAccum.dword &= 0xFF;

			if (! bitAccum.dword)
			{
				bitAccum.dword = exitFlag + 2 * (*afterHeader++);
				exitFlag = (bitAccum.dword >> 8) & 1;
			}
		} while (exitFlag);
	}

	// FUNCTION: TOY2 0x00452310
	void LoadPacketData(char* fileName)
	{
		uint8_t* buffer;
		uint8_t* resumePtr;
		uint8_t* decompBuffer;
		uint8_t* packetCursor;

		Logger::Log("ROUTINE : Loading packet data %s, level %d.\n", fileName, Toy2::g_levelFileIndex);

		if ((Toy2::Levels::g_levelLoadConfig & 0x800) != 0 && (resumePtr = g_resumePacketPtr) != 0)
		{
			buffer = reinterpret_cast<uint8_t*>(fileName); // ?
		}
		else
		{
			buffer = Toy2::Levels::g_levelDataHeapBase + sizeof(Toy2::Levels::g_levelDataHeapBase) - FileUtils::GetFileSize(fileName);

			FileUtils::LoadFile(fileName, buffer);

			memset(g_unusedBuffer1, 0, sizeof(g_unusedBuffer1));
			memset(g_unusedBuffer2, 0, sizeof(g_unusedBuffer2));

			resumePtr = g_resumePacketPtr;
			packetCursor = buffer;

			memset(g_unusedBuffer3, 0, sizeof(g_unusedBuffer3));

			g_unused1 = 0x80000000;
			g_unused2 = 0;
			g_unused3 = 0;

			Toy2::g_hasBackdrop = 0;

			memset(g_unusedBuffer4, 0, sizeof(g_unusedBuffer4));
		}

		Toy2::Actor::g_activeActors[0] = 0;

		memset(g_creatureListRam, 0, sizeof(g_creatureListRam));

		if ((Toy2::Levels::g_levelLoadConfig & 0x800) != 0 && resumePtr)
		{
			buffer = resumePtr;
			packetCursor = resumePtr;
		}

		decompBuffer = Toy2::Levels::g_levelLoadArena;
		int32_t resumeCounter = 0;

		while ((*buffer != 0xFF || buffer[1] != 0xFF || buffer[2] != 0xFF || buffer[3] != 0xFF) && resumeCounter != 2)
		{
			DecompressBuffer(buffer, decompBuffer);

			if (*reinterpret_cast<uint32_t*>(decompBuffer) == 35)
			{
				memcpy(g_creatureListRam, decompBuffer + 4, sizeof(g_creatureListRam));
				Logger::Log("LOAD: Type 35, CreatListRam.\n");
			}
			else if (*reinterpret_cast<uint32_t*>(decompBuffer) == 36)
			{
				// omitted
				// $TODO: implement this portion, don't know if it actually does anything
			}

			int32_t packetPayloadSize = (buffer[6] + ((buffer[5] + (buffer[4] << 8)) << 8)) << 8;
			uint8_t* packetBodyBase = &buffer[buffer[7]];

			buffer = &packetBodyBase[packetPayloadSize + 14];
			packetCursor = buffer;

			if ((Toy2::Levels::g_levelLoadConfig & 0x800) != 0)
			{
				g_resumePacketPtr = &packetBodyBase[packetPayloadSize + 14];
				++resumeCounter;
			}
		}

		Toy2::Levels::g_levelLoadArena = decompBuffer;

		g_unused4 = g_type36Data1;
		g_unused5 = g_unusedBuffer5;
	}

	// STUB: TOY2 0x00450010
	void LoadRawAndNGN(const char* filePath) {}
}
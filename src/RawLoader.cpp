#include "RawLoader.h"
#include "Toy2/Levels.h"
#include "Toy2/Toy2.h"
#include "FileUtils.h"
#include "Logger.h"
#include "Toy2/Actor.h"
#include "Renderer/SpriteSheets.h"
#include "NGNLoader/NGNLoader.h"

#include <WINDOWS.H>
#include <STDIO.H>

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

	// GLOBAL: TOY2 0x005D2BE4
	uint16_t* g_unused4;

	// GLOBAL: TOY2 0x00703BE8
	uint16_t g_type36Data1[4];

	// FUNCTION: TOY2 0x0047B170
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

		if (!carry)
		{
			carry = *src++;
			carry = accum + 2 * carry;
			scratch = (carry >> 8) & 1;
			accum = scratch;
		}

		if (!accum)
		{
			carry *= 2;
			length = (carry >> 8) & 1;
			carry &= 0xFF;

			if (!carry)
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

			if (!accum)
			{
				accum = *src++;
				accum = carry + 2 * accum;
				carry = (accum >> 8) & 1;
			}

			if (!carry)
				goto L3A1;

			accum *= 2;
			length--;
			carry = (accum >> 8) & 1;
			accum &= 0xFF;

			if (!accum)
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

			if (!accum)
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

		if (!accum)
		{
			accum = *src++;
			accum = carry + 2 * accum;
			carry = (accum >> 8) & 1;
		}

		if (!carry)
			goto L4A4;

		accum *= 2;
		length = 3;
		carry = (accum >> 8) & 1;
		accum &= 0xFF;

		if (!accum)
		{
			accum = *src++;
			accum = carry + 2 * accum;
			scratch = (accum >> 8) & 1;
			carry = scratch;
		}

		if (!carry)
			goto L3A1;

		length = *src++;

		if (!length)
		{
			accum *= 2;
			carry = (accum >> 8) & 1;
			accum &= 0xFF;

			if (!accum)
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

		if (!accum)
		{
			accum = *src++;
			accum = carry + 2 * accum;
			scratch = (accum >> 8) & 1;
			carry = scratch;
		}

		if (!carry)
			goto L4A4;

		accum *= 2;
		carry = (accum >> 8) & 1;
		accum &= 0xFF;

		if (!accum)
		{
			accum = *src++;
			accum = carry + 2 * accum;
			carry = (accum >> 8) & 1;
		}

		accum *= 2;
		distHigh = carry;
		carry = (accum >> 8) & 1;
		accum &= 0xFF;

		if (!accum)
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

			if (!tmpAccum)
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

			if (!accum)
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

		if (!accum)
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
		Renderer::g_spriteSheets[128] = &Renderer::g_rawLoaderSpriteSheet;
	}

	// FUNCTION: TOY2 0x00450010
	void LoadRawAndNGN(char* filePath)
	{
		RawLoader::LoadPacketData(filePath);
		FileUtils::AppendRegPathToBuffer();

		char fileName[256];
		sprintf(fileName, "%s%s", FileUtils::g_fileNameBuffer, filePath);

		char* rawPath = strstr(fileName, ".raw");

		if (rawPath)
		{
			strcpy(rawPath, ".ngn");

			NGNLoader::SetNewImage(fileName);
			NGNLoader::DetectBackdropTextures();
		}
	}
}
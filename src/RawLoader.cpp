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
	// GLOBAL: TOY2 0x004F73A8
	Backdrop g_backdrop = { 256, 128, 32, { 118, 175, 158 }, { 72, 56, 40 }, 64, 0 };

	// GLOBAL: TOY2 0x004F73D4
	Backdrop g_staticBackdrop = { 128, 160, 32, { 118, 175, 158 }, { 72, 56, 40 }, 64, 0 };

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

	// GLOBAL: TOY2 0x00559EDA
	uint16_t g_backdropSourceFlags;

	// GLOBAL: TOY2 0x00559EDC
	uint8_t* g_unusedBuffer4[129];

	// GLOBAL: TOY2 0x0055A11C
	int32_t g_unused1;

	// GLOBAL: TOY2 0x00703E08
	int16_t g_unused2;

	// GLOBAL: TOY2 0x0055A13C
	int32_t g_unused3;

	// GLOBAL: TOY2 0x005D2BE4
	uint16_t* g_unused4;

	// GLOBAL: TOY2 0x00703BE8
	uint16_t g_type36Data1[4];

	// GLOBAL: TOY2 0x005D2ADC
	int16_t g_backdropSourceWidth;

	// GLOBAL: TOY2 0x00559E7C
	int16_t g_backdropSourceHeight;

	// FUNCTION: TOY2 0x0047B170
	void DecompressBuffer(uint8_t* inBuffer, uint8_t* outBuffer)
	{
		uint8_t* input;
		uint8_t* output;
		uint8_t* temp;
		uint32_t len;
		uint32_t pos;
		uint32_t disp;
		uint32_t bitbuf;
		uint32_t carry;
		uint8_t pairByte;

		input = inBuffer + 14;
		output = outBuffer;
		bitbuf = *input++;
		bitbuf = 4 * bitbuf + 2;

	LBL_GET_BITS2:
		while (1)
		{
			bitbuf += bitbuf;
			carry = (bitbuf >> 8) & 1;

			if (! carry)
			{
				carry = *input++;
				*output++ = (uint8_t)carry;
				bitbuf += bitbuf;
				carry = (bitbuf >> 8) & 1;

				if (! carry)
					goto LBL_X_BYTE;
			}

			bitbuf &= 0xFF;

			if (bitbuf)
				break;

			bitbuf = *input++;
			bitbuf = bitbuf * 2 + carry;

			if (bitbuf & 0x100)
				break;

		LBL_X_BYTE:
			carry = *input++;
			*output++ = (uint8_t)carry;
		}

		len = 2;
		pos = 0;
		bitbuf += bitbuf;
		carry = (bitbuf >> 8) & 1;
		bitbuf &= 0xFF;

		if (! bitbuf)
		{
			bitbuf = *input++;
			bitbuf = bitbuf * 2 + carry;
			carry = (bitbuf >> 8) & 1;
		}

		if (! carry)
		{
			bitbuf += bitbuf;
			carry = (bitbuf >> 8) & 1;
			bitbuf &= 0xFF;

			if (! bitbuf)
			{
				bitbuf = *input++;
				bitbuf = bitbuf * 2 + carry;
				carry = (bitbuf >> 8) & 1;
			}

			len = len * 2 + carry;
			bitbuf += bitbuf;
			carry = (bitbuf >> 8) & 1;
			bitbuf &= 0xFF;

			if (! bitbuf)
			{
				bitbuf = *input++;
				bitbuf = bitbuf * 2 + carry;
				carry = (bitbuf >> 8) & 1;
			}

			if (! carry)
				goto LBL_COPY;

			len--;
			bitbuf += bitbuf;
			carry = (bitbuf >> 8) & 1;
			bitbuf &= 0xFF;

			if (! bitbuf)
			{
				bitbuf = *input++;
				bitbuf = bitbuf * 2 + carry;
				carry = (bitbuf >> 8) & 1;
			}

			len = len * 2 + carry;

			if (len - 9)
				goto LBL_COPY;

			len = 3;

		LBL_X4_BITS:
			bitbuf += bitbuf;
			carry = (bitbuf >> 8) & 1;
			bitbuf &= 0xFF;

			if (! bitbuf)
			{
				bitbuf = *input++;
				bitbuf = bitbuf * 2 + carry;
				carry = (bitbuf >> 8) & 1;
			}

			len--;
			pos = pos * 2 + carry;

			if ((int32_t)len >= 0)
				goto LBL_X4_BITS;

			pos += 2;

		LBL_GET_RAW_REP:
			pairByte = input[0];
			carry = input[1];
			output[0] = pairByte;
			output[1] = (uint8_t)carry;
			pairByte = input[2];
			carry = input[3];
			output[2] = pairByte;
			output[3] = (uint8_t)carry;
			input += 4;
			pos--;
			output += 4;

			if ((int32_t)pos >= 0)
				goto LBL_GET_RAW_REP;

			goto LBL_GET_BITS2;
		}

		bitbuf += bitbuf;
		carry = (bitbuf >> 8) & 1;
		bitbuf &= 0xFF;

		if (! bitbuf)
		{
			bitbuf = *input++;
			bitbuf = bitbuf * 2 + carry;
			carry = (bitbuf >> 8) & 1;
		}

		if (! carry)
			goto LBL_BYTE_DISP2;

		len++;
		bitbuf += bitbuf;
		carry = (bitbuf >> 8) & 1;
		bitbuf &= 0xFF;

		if (! bitbuf)
		{
			bitbuf = *input++;
			bitbuf = bitbuf * 2 + carry;
			carry = (bitbuf >> 8) & 1;
		}

		if (! carry)
			goto LBL_COPY;

		len = *input++;

		if (! len)
		{
			bitbuf += bitbuf;
			carry = (bitbuf >> 8) & 1;
			bitbuf &= 0xFF;

			if (! bitbuf)
			{
				bitbuf = *input++;
				bitbuf = bitbuf * 2 + carry;
				carry = (bitbuf >> 8) & 1;
			}

			if (carry)
				goto LBL_GET_BITS2;

			return;
		}

		len += 8;

	LBL_COPY:
		bitbuf += bitbuf;
		carry = (bitbuf >> 8) & 1;
		bitbuf &= 0xFF;

		if (! bitbuf)
		{
			bitbuf = *input++;
			bitbuf = bitbuf * 2 + carry;
			carry = (bitbuf >> 8) & 1;
		}

		if (! carry)
			goto LBL_BYTE_DISP2;

		bitbuf += bitbuf;
		carry = (bitbuf >> 8) & 1;
		bitbuf &= 0xFF;

		if (! bitbuf)
		{
			bitbuf = *input++;
			bitbuf = bitbuf * 2 + carry;
			carry = (bitbuf >> 8) & 1;
		}

		pos = pos * 2 + carry;
		bitbuf += bitbuf;
		carry = (bitbuf >> 8) & 1;
		bitbuf &= 0xFF;

		if (! bitbuf)
		{
			bitbuf = *input++;
			bitbuf = bitbuf * 2 + carry;
			carry = (bitbuf >> 8) & 1;
		}

		if (carry)
		{
			bitbuf += bitbuf;
			carry = (bitbuf >> 8) & 1;
			bitbuf &= 0xFF;

			if (! bitbuf)
			{
				bitbuf = *input++;
				bitbuf = bitbuf * 2 + carry;
				carry = (bitbuf >> 8) & 1;
			}

			pos = pos * 2 + carry;
			pos |= 4;
			bitbuf += bitbuf;
			carry = (bitbuf >> 8) & 1;
			bitbuf &= 0xFF;

			if (! bitbuf)
			{
				bitbuf = *input++;
				bitbuf = bitbuf * 2 + carry;
				carry = (bitbuf >> 8) & 1;
			}

			if (carry)
				goto LBL_BYTE_DISP;
		}
		else
		{
			if (pos)
				goto LBL_BYTE_DISP;

			pos++;
		}

		bitbuf += bitbuf;
		carry = (bitbuf >> 8) & 1;
		bitbuf &= 0xFF;

		if (! bitbuf)
		{
			bitbuf = *input++;
			bitbuf = bitbuf * 2 + carry;
			carry = (bitbuf >> 8) & 1;
		}

		pos = pos * 2 + carry;

	LBL_BYTE_DISP:
		pos = ((pos & 0xFF) << 8) | (pos >> 8);

	LBL_BYTE_DISP2:
		carry = *input++;
		disp = carry | (pos & 0xFF00);
		temp = output - disp - 1;
		carry = len & 1;
		len >>= 1;

		if (carry)
		{
			carry = *temp++;
			*output++ = (uint8_t)carry;
		}

		len--;

		if (disp)
		{
		LBL_BYTE_DISP5:
			carry = temp[0];
			pairByte = temp[1];
			output[1] = pairByte;
			output[0] = (uint8_t)carry;
			temp += 2;
			len--;
			output += 2;

			if ((int32_t)len >= 0)
				goto LBL_BYTE_DISP5;

			goto LBL_GET_BITS2;
		}

		carry = *temp;
		pos = carry | (pos & 0xFF00);

	LBL_BYTE_DISP4:
		output[0] = (uint8_t)pos;
		output[1] = (uint8_t)pos;
		len--;
		output += 2;

		if ((int32_t)len >= 0)
			goto LBL_BYTE_DISP4;

		goto LBL_GET_BITS2;
	}

	// FUNCTION: TOY2 0x00452310
	void LoadPacketData(char* fileName)
	{
		uint8_t* buffer;
		uint8_t* resumePtr;
		uint8_t* decompBuffer;
		int32_t tail24;
		int32_t tail28;

		Logger::Log("ROUTINE : Loading packet data %s, level %d.\n", fileName, Toy2::g_levelFileIndex);

		if ((Toy2::Levels::g_levelLoadConfig & 0x800) != 0 && (resumePtr = g_resumePacketPtr) != 0)
		{
			buffer = (uint8_t*)fileName; // ?
		}
		else
		{
			buffer = Toy2::Levels::g_levelDataHeapBase + sizeof(Toy2::Levels::g_levelDataHeapBase) - FileUtils::GetFileSize(fileName);

			FileUtils::LoadFile(fileName, buffer);

			memset(g_unusedBuffer1, 0, sizeof(g_unusedBuffer1));
			memset(g_unusedBuffer2, 0, sizeof(g_unusedBuffer2));

			resumePtr = g_resumePacketPtr;

			memset(g_unusedBuffer3, 0, sizeof(g_unusedBuffer3));

			g_unused1 = 0x80000000;
			tail24 = 0;
			tail28 = 0;
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
		}

		decompBuffer = Toy2::Levels::g_levelLoadArena;
		int32_t resumeCounter = 0;

		while ((*buffer != 0xFF || buffer[1] != 0xFF || buffer[2] != 0xFF || buffer[3] != 0xFF) && resumeCounter != 2)
		{
			DecompressBuffer(buffer, decompBuffer);

			if (*(uint32_t*)decompBuffer == 35)
			{
				memset(g_creatureListRam, 0, sizeof(g_creatureListRam));
				memcpy(g_creatureListRam, decompBuffer + 4, sizeof(g_creatureListRam));
				Logger::Log("LOAD: Type 35, CreatListRam.\n");
			}
			else if (*(uint32_t*)decompBuffer == 36)
			{
				int16_t width = *(int16_t*)(decompBuffer + 8);
				g_backdropSourceWidth = width;
				int16_t height = *(int16_t*)(decompBuffer + 10);
				g_backdropSourceHeight = height;
				int16_t pixelOffsetBytes = *(int16_t*)(decompBuffer + 12);
				g_type36Data1[0] = pixelOffsetBytes;
				g_backdropSourceFlags = *(uint16_t*)(decompBuffer + 14);

				g_backdrop.width = width;
				g_backdrop.height = height;
				g_backdrop.verticalOffset = *(int32_t*)(decompBuffer + 4);

				int32_t pixelOffsetWords = pixelOffsetBytes / 2;
				uint8_t* firstPixel = decompBuffer + pixelOffsetWords * 2 + 16;
				uint8_t* lastPixel = decompBuffer + pixelOffsetWords * 2 + width * height + 15;
				g_backdrop.topColor.r = decompBuffer[*firstPixel * 3 + 16];
				g_backdrop.topColor.g = decompBuffer[*firstPixel * 3 + 17];
				g_backdrop.topColor.b = decompBuffer[*firstPixel * 3 + 18];
				g_backdrop.bottomColor.r = decompBuffer[*lastPixel * 3 + 16];
				g_backdrop.bottomColor.g = decompBuffer[*lastPixel * 3 + 17];
				g_backdrop.bottomColor.b = decompBuffer[*lastPixel * 3 + 18];
				g_backdrop.unknown24 = (int16_t)tail24;
				g_backdrop.unknown28 = (int16_t)tail28;
			}

			uint32_t packetPayloadSize = (uint32_t)buffer[4] << 24 | (uint32_t)buffer[5] << 16 | (uint32_t)buffer[6] << 8 | buffer[7];
			buffer += packetPayloadSize + 14;

			if ((Toy2::Levels::g_levelLoadConfig & 0x800) != 0)
			{
				g_resumePacketPtr = buffer;
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

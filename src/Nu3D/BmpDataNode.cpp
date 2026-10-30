#include "Nu3D/BmpDataNode.h"
#include "DrawingDevice.h"
#include "Renderer/Renderer.h"
#include "Logger.h"
#include "Nu3D/Creature.h"
#include "Nu3D/Primitive.h"
#include "NGNLoader/NGNLoader.h"
#include "NGNLoader/ObjectLoad.h"
#include "Toy2/Toy2.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

namespace Nu3D
{
	struct AlternateBmpDataNode
	{
		HBITMAP mainBitmap;
		HBITMAP alphaBitmap;
		LPDIRECTDRAWSURFACE4 surface;
		LPDIRECT3DTEXTURE2 texture;
		uint32_t textureStage;
		char name[80];
		int32_t alphaRequested;
		uint32_t flags;
		uint8_t reserved[0x28];
		void* cpuPixels;
		DDSURFACEDESC2 surfaceDesc;
		uint32_t references;
		AlternateBmpDataNode* previous;
		AlternateBmpDataNode* next;
	};

	STATIC_ASSERT(sizeof(AlternateBmpDataNode) == 0x120);
	STATIC_ASSERT(offsetof(AlternateBmpDataNode, mainBitmap) == 0x0);
	STATIC_ASSERT(offsetof(AlternateBmpDataNode, name) == 0x14);
	STATIC_ASSERT(offsetof(AlternateBmpDataNode, alphaRequested) == 0x64);
	STATIC_ASSERT(offsetof(AlternateBmpDataNode, flags) == 0x68);
	STATIC_ASSERT(offsetof(AlternateBmpDataNode, cpuPixels) == 0x94);
	STATIC_ASSERT(offsetof(AlternateBmpDataNode, surfaceDesc) == 0x98);
	STATIC_ASSERT(offsetof(AlternateBmpDataNode, references) == 0x114);
	STATIC_ASSERT(offsetof(AlternateBmpDataNode, previous) == 0x118);
	STATIC_ASSERT(offsetof(AlternateBmpDataNode, next) == 0x11C);

	// GLOBAL: TOY2 0x00508288
	char g_alternateMediaDirectory[512] = "MEDIA\\";

	// GLOBAL: TOY2 0x00508488
	const char* g_alternateSdkValueName = "DX6SDK Samples Path";

	HRESULT ResolveAlternateMediaPath(const char* filename, const char* mediaDirectory, char* outputPath);

	// GLOBAL: TOY2 0x00884044
	uint8_t g_lastBmpPalette[1024];

	// GLOBAL: TOY2 0x00884448
	int32_t g_minTextureSize;

	// GLOBAL: TOY2 0x00508214
	int32_t g_maxTextureSize = 256;

	// GLOBAL: TOY2 0x00508210
	int32_t g_unusedTexShift = 1;

	// GLOBAL: TOY2 0x00884444
	BmpDataNode* g_bmpDataHead;

	// GLOBAL: TOY2 0x0088400C
	BmpDataNode* g_currentBmpDataNode;

	// FUNCTION: TOY2 0x004AFCB0
	void CalculatePixelFormatShifts(PixelFormatInfo* pixelFormatInfo, DDSURFACEDESC2* surfaceDesc)
	{
		uint32_t mask;

		pixelFormatInfo->alphaShift = -8;
		mask = surfaceDesc->ddpfPixelFormat.dwRGBAlphaBitMask;

		for (pixelFormatInfo->alphaMask = mask; mask; ++pixelFormatInfo->alphaShift)
			mask >>= 1;

		pixelFormatInfo->redShift = -8;
		mask = surfaceDesc->ddpfPixelFormat.dwRBitMask;

		for (pixelFormatInfo->redMask = mask; mask; ++pixelFormatInfo->redShift)
			mask >>= 1;

		pixelFormatInfo->greenShift = -8;
		mask = surfaceDesc->ddpfPixelFormat.dwGBitMask;

		for (pixelFormatInfo->greenMask = mask; mask; ++pixelFormatInfo->greenShift)
			mask >>= 1;

		pixelFormatInfo->blueShift = -8;
		mask = surfaceDesc->ddpfPixelFormat.dwBBitMask;

		for (pixelFormatInfo->blueMask = mask; mask; ++pixelFormatInfo->blueShift)
			mask >>= 1;
	}

	// FUNCTION: TOY2 0x004AFF80 [MATCHED]
	void CopyTextureToSurface(BmpDataNode* bmpDataNode)
	{
		DDSURFACEDESC2 surfaceDesc;
		memset(&surfaceDesc, 0, sizeof(surfaceDesc));
		surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);

		HRESULT lockResult;
		do
		{
			lockResult = bmpDataNode->surface->Lock(0, &surfaceDesc, DDLOCK_NOSYSLOCK, 0);
		} while (lockResult == DDERR_WASSTILLDRAWING);

		if (lockResult < 0)
		{
			Logger::LogD3DError(lockResult);
			return;
		}

		PixelFormatInfo pixelFormatInfo;
		CalculatePixelFormatShifts(&pixelFormatInfo, &surfaceDesc);

		for (int32_t row = 0; row < bmpDataNode->textureHeight; ++row)
		{
			int32_t texHeight = bmpDataNode->textureHeight;
			int32_t texWidth = bmpDataNode->textureWidth;

			uint8_t* sourcePixel = (uint8_t*)&bmpDataNode->texData[texWidth * (texHeight - row - 1)];
			uint16_t* destPixel = (uint16_t*)((uint8_t*)surfaceDesc.lpSurface + row * surfaceDesc.lPitch);

			int32_t col = 0;

			if (texWidth > 0)
			{
				do
				{
					int32_t shiftedValue;

					if (pixelFormatInfo.greenShift >= 0)
						shiftedValue = sourcePixel[1] << (int8_t)pixelFormatInfo.greenShift;
					else
						shiftedValue = sourcePixel[1] >> -(int8_t)pixelFormatInfo.greenShift;

					uint16_t pixel = (uint16_t)pixelFormatInfo.greenMask & shiftedValue;

					if (pixelFormatInfo.blueShift >= 0)
						shiftedValue = sourcePixel[0] << (int8_t)pixelFormatInfo.blueShift;
					else
						shiftedValue = sourcePixel[0] >> -(int8_t)pixelFormatInfo.blueShift;

					pixel = ((uint16_t)pixelFormatInfo.blueMask & shiftedValue) | pixel;

					if (pixelFormatInfo.redShift >= 0)
						shiftedValue = sourcePixel[2] << (int8_t)pixelFormatInfo.redShift;
					else
						shiftedValue = sourcePixel[2] >> -(int8_t)pixelFormatInfo.redShift;

					pixel = ((uint16_t)pixelFormatInfo.redMask & shiftedValue) | pixel;

					if ((bmpDataNode->flags & 0xF) != 0)
					{
						if (pixelFormatInfo.alphaShift >= 0)
							shiftedValue = sourcePixel[3] << (int8_t)pixelFormatInfo.alphaShift;
						else
							shiftedValue = sourcePixel[3] >> -(int8_t)pixelFormatInfo.alphaShift;

						pixel |= (uint16_t)pixelFormatInfo.alphaMask & shiftedValue;
					}

					*destPixel++ = pixel;
					sourcePixel += 4;
					++col;

				} while (col < bmpDataNode->textureWidth);
			}
		}

		bmpDataNode->surface->Unlock(0);
	}
	// FUNCTION: TOY2 0x004B0400
	int32_t CountAlphaBits(LPDDPIXELFORMAT pixelFormat)
	{
		DWORD alphaBitMask = pixelFormat->dwRGBAlphaBitMask;

		int32_t result;

		for (result = 0; alphaBitMask; alphaBitMask >>= 1)
		{
			if ((alphaBitMask & 1) != 0)
				++result;
		}

		return result;
	}

	// FUNCTION: TOY2 0x004B0380
	LONG WINAPI FindSuitablePixelFormat(LPDDPIXELFORMAT pixelFormat, void* context)
	{
		if (! pixelFormat)
			return 1;

		if (! context)
			return 1;

		DWORD flags = pixelFormat->dwFlags;

		if ((flags & 0xE0000) != 0)
			return 1;

		DWORD bitCount = pixelFormat->dwRGBBitCount;

		if (bitCount < 16)
			return 1;

		if (pixelFormat->dwFourCC)
			return 1;

		FindPixelFormat* findPixelFormat = (FindPixelFormat*)context;

		uint32_t needAlpha = findPixelFormat->needAlpha;

		if (needAlpha == 1 && (flags & 1) == 0)
			return 1;

		if (! needAlpha && (flags & 1) != 0 || bitCount != findPixelFormat->bpp || needAlpha && CountAlphaBits(pixelFormat) < findPixelFormat->minAlphaBits)
		{
			return 1;
		}

		memcpy(findPixelFormat->out, pixelFormat, sizeof(DDPIXELFORMAT));

		findPixelFormat->valid = 1;

		return 0;
	}

	// FUNCTION: TOY2 0x004AFBA0
	void DestroyBmpDataNode(BmpDataNode* bmpDataNode)
	{
		LPDIRECT3DTEXTURE2 d3dTexture = bmpDataNode->d3dTexture;

		if (d3dTexture)
		{
			d3dTexture->Release();
			bmpDataNode->d3dTexture = 0;
		}

		if (bmpDataNode->surface)
		{
			if ((bmpDataNode->flags & 64) == 0)
			{
				bmpDataNode->surface->Release();
				bmpDataNode->surface = 0;
			}
		}
	}

	// FUNCTION: TOY2 0x004B0200
	int32_t InitialiseTextureSurface(BmpDataNode* bmpDataNode)
	{
		FindPixelFormat findPixelFormat;
		DDSURFACEDESC2 surfaceDesc;
		D3DDEVICEDESC deviceDesc2;
		D3DDEVICEDESC deviceDesc;

		if ((bmpDataNode->flags & 0x40) != 0)
			return 0;

		DestroyBmpDataNode(bmpDataNode);

		LPDIRECTDRAW4 ddraw4 = DrawingDevice::GetDDraw4();

		// yes, this just gets called
		DrawingDevice::GetD3D();

		LPDIRECT3DDEVICE3 d3dDevice = DrawingDevice::GetD3DDevice();

		deviceDesc.dwSize = 252;
		deviceDesc2.dwSize = 252;

		if (d3dDevice->GetCaps(&deviceDesc, &deviceDesc2))
			return 0;

		memset(&surfaceDesc, 0, sizeof(surfaceDesc));
		uint32_t width = bmpDataNode->textureWidth;
		uint32_t height = bmpDataNode->textureHeight;

		surfaceDesc.ddsCaps.dwCaps2 = 16;

		findPixelFormat.bpp = 16;

		surfaceDesc.dwSize = 124;
		findPixelFormat.out = &surfaceDesc.ddpfPixelFormat;

		int32_t flags = bmpDataNode->flags;

		surfaceDesc.ddpfPixelFormat.dwSize = 32;
		surfaceDesc.dwFlags = 0x101007;
		surfaceDesc.ddsCaps.dwCaps = 4096;
		surfaceDesc.dwTextureStage = 0;
		surfaceDesc.dwWidth = width;
		surfaceDesc.dwHeight = height;
		memset(&findPixelFormat.minAlphaBits, 0, 12);

		if ((flags & 0xF) != 0)
		{
			findPixelFormat.needAlpha = 1;
			findPixelFormat.minAlphaBits = (flags & 1) != 0 ? 4 : 1;
		}

		d3dDevice->EnumTextureFormats(FindSuitablePixelFormat, &findPixelFormat);

		if (! findPixelFormat.valid || ddraw4->CreateSurface(&surfaceDesc, &bmpDataNode->surface, 0)
			|| bmpDataNode->surface->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&bmpDataNode->d3dTexture))
		{
			return 0;
		}

		uint32_t* texData = bmpDataNode->texData;

		memcpy(&bmpDataNode->surfaceDesc, &surfaceDesc, sizeof(bmpDataNode->surfaceDesc));

		if (texData)
			CopyTextureToSurface(bmpDataNode);

		return 1;
	}

	// FUNCTION: TOY2 0x004B0760
	int32_t GetBitmapWidth(HANDLE bmpHandle)
	{
		BITMAP bmp;
		GetObjectA(bmpHandle, sizeof(BITMAP), &bmp);
		return bmp.bmWidth;
	}

	// FUNCTION: TOY2 0x004B0780
	int32_t GetBitmapHeight(HANDLE bmpHandle)
	{
		BITMAP bmp;
		GetObjectA(bmpHandle, sizeof(BITMAP), &bmp);
		return bmp.bmHeight;
	}

	// FUNCTION: TOY2 0x004AFC10
	BmpDataNode* AllocateBmpDataNode()
	{
		BmpDataNode* bmpDataStruct = (BmpDataNode*)malloc(sizeof(BmpDataNode));

		if (bmpDataStruct)
		{
			memset(bmpDataStruct, 0, sizeof(BmpDataNode));
			bmpDataStruct->next = g_bmpDataHead;

			if (g_bmpDataHead)
				g_bmpDataHead->prev = bmpDataStruct;

			bmpDataStruct->prev = 0;

			g_bmpDataHead = bmpDataStruct;

			bmpDataStruct->refCount = 0;
		}

		return bmpDataStruct;
	}

	// FUNCTION: TOY2 0x004B0870
	RGBA SampleBitmapPixel(BITMAP* mainBmp, BITMAP* alphaBmp, int32_t x, int32_t y, int32_t textureSize, uint8_t flags)
	{
		RGBA color;
		int32_t bitsPerPixel = mainBmp->bmBitsPixel;

		if (bitsPerPixel == 8)
		{
			int32_t scaledY2 = y * mainBmp->bmHeight / textureSize;

			color.a = -1;

			int32_t paletteIndex = 4 * *((uint8_t*)mainBmp->bmBits + x * mainBmp->bmWidth / textureSize + mainBmp->bmWidthBytes * scaledY2);

			color.b = g_lastBmpPalette[paletteIndex];
			color.g = g_lastBmpPalette[paletteIndex + 1];
			color.r = g_lastBmpPalette[paletteIndex + 2];

			if ((flags & 2) != 0)
			{
				color.a = -(color.value != -1);
				return color;
			}

			if ((flags & 4) != 0)
			{
				color.a = -(color.value != 0xFF000000);
				return color;
			}

			if ((flags & 8) != 0 && color.value == 0xFF00FF00)
			{
				RGBA empty;
				empty.value = 0;

				return empty;
			}
		}
		else if (bitsPerPixel == 24)
		{
			int32_t scaledY = y * mainBmp->bmHeight / textureSize;

			color.a = 0xFF;

			uint8_t* pixelOffset = (uint8_t*)mainBmp->bmBits + 3 * (x * mainBmp->bmWidth / textureSize) + mainBmp->bmWidthBytes * scaledY;

			color.b = pixelOffset[0];
			color.g = pixelOffset[1];
			color.r = pixelOffset[2];

			if ((flags & 2) != 0)
			{
				color.a = -(color.value != -1);
			}
			else if ((flags & 4) != 0)
			{
				color.a = -(color.value != 0xFF000000);
			}
			else if ((flags & 8) != 0 && color.value == 0xFF00FF00)
			{
				color.value = 0;
			}

			if (alphaBmp && alphaBmp->bmBitsPixel == 8)
			{
				color.a = *((uint8_t*)alphaBmp->bmBits + x * alphaBmp->bmWidth / textureSize + alphaBmp->bmWidthBytes * (y * alphaBmp->bmHeight / textureSize));
				return color;
			}
		}

		return color;
	}

	// FUNCTION: TOY2 0x004B0B60
	HBITMAP ProcessBmpInfoFromStream(FILE* stream)
	{
		int32_t offset = ftell(stream);

		BITMAPFILEHEADER bmpHeader;

		struct
		{
			BITMAPINFOHEADER bmiHeader;
			RGBQUAD bmiColors[256];
		} bmpInfo;

		fread(&bmpHeader, 1, sizeof(BITMAPFILEHEADER), stream);
		fread(&bmpInfo, 1, sizeof(BITMAPINFOHEADER), stream);

		size_t pixelDataSize;
		uint32_t calcHeight;

		switch (bmpInfo.bmiHeader.biBitCount)
		{
			case 1:
				fread(bmpInfo.bmiColors, 2, 4, stream);
				calcHeight = bmpInfo.bmiHeader.biHeight;
				break;
			case 4:
				fread(bmpInfo.bmiColors, 16, 4, stream);
				pixelDataSize = bmpInfo.bmiHeader.biWidth * bmpInfo.bmiHeader.biHeight / 2;
				goto READ_BMDATA;
			case 8:
				fread(bmpInfo.bmiColors, 256, 4, stream);

				memcpy(g_lastBmpPalette, bmpInfo.bmiColors, sizeof(g_lastBmpPalette));
				pixelDataSize = bmpInfo.bmiHeader.biWidth * bmpInfo.bmiHeader.biHeight;

				goto READ_BMDATA;
			default:
				calcHeight = bmpInfo.bmiHeader.biHeight * bmpInfo.bmiHeader.biBitCount;
				break;
		}

		pixelDataSize = bmpInfo.bmiHeader.biWidth * calcHeight / 8;

	READ_BMDATA:

		fseek(stream, offset + bmpHeader.bfOffBits, 0);

		void* bitmapBits;
		HBITMAP hBitmap = CreateDIBSection(0, (BITMAPINFO*)&bmpInfo, 0, &bitmapBits, 0, 0);

		if (hBitmap)
			fread(bitmapBits, pixelDataSize, 1, stream);

		return hBitmap;
	}

	// FUNCTION: TOY2 0x004B06D0
	int32_t CalculateTexSize(HANDLE bmp, int32_t flags)
	{
		BITMAP bitmap;
		GetObjectA(bmp, sizeof(BITMAP), &bitmap);

		int32_t height = bitmap.bmHeight;

		if (bitmap.bmWidth > bitmap.bmHeight)
			height = bitmap.bmWidth;

		int32_t textureSize;

		if (g_minTextureSize > Numerics::RoundUpToPowerOf2(height))
			textureSize = g_minTextureSize;
		else
			textureSize = Numerics::RoundUpToPowerOf2(height);

		if (textureSize >= g_maxTextureSize)
			textureSize = g_maxTextureSize;

		if (((flags == 0) & 16) != 0)
			textureSize >>= g_unusedTexShift;

		return textureSize;
	}

	// FUNCTION: TOY2 0x004B07A0
	uint32_t* ProcessBmpPixelData(HANDLE mainBmp, HANDLE alphaBmp, int32_t flags)
	{
		int32_t texSize = CalculateTexSize(mainBmp, flags);

		BITMAP mainBmpObj;
		BITMAP alphaBmpObj;

		GetObjectA(mainBmp, sizeof(BITMAP), &mainBmpObj);

		if (alphaBmp)
			GetObjectA(alphaBmp, sizeof(BITMAP), &alphaBmpObj);

		uint32_t* buffer = (uint32_t*)malloc(4 * texSize * texSize);

		if (! buffer)
			return buffer;

		int32_t row = 0;

		if (texSize > 0)
		{
			int32_t rowOffset = 0;

			do
			{
				for (int32_t col = 0; col < texSize; ++col)
				{
					uint32_t pixelColor;

					if (alphaBmp)
						pixelColor = SampleBitmapPixel(&mainBmpObj, &alphaBmpObj, col, row, texSize, flags).value;
					else
						pixelColor = SampleBitmapPixel(&mainBmpObj, 0, col, row, texSize, flags).value;

					buffer[col + rowOffset] = pixelColor;
				}

				++row;
				rowOffset += texSize;

			} while (row < texSize);
		}

		return buffer;
	}

	// FUNCTION: TOY2 0x004AFAF0
	void SetMinTexSize(int32_t minTexSize) { g_minTextureSize = minTexSize; }

	// FUNCTION: TOY2 0x004B0A30
	BmpDataNode* LoadTextureByStream(FILE* handle, const char* rawTexStr, int32_t flags)
	{
		HBITMAP mainBmp = ProcessBmpInfoFromStream(handle);
		HBITMAP alphaBmp = 0;

		if (! mainBmp)
			return 0;

		if ((flags & 1) != 0)
			alphaBmp = ProcessBmpInfoFromStream(handle);

		uint32_t* texDataBuffer = ProcessBmpPixelData(mainBmp, alphaBmp, flags);

		if (! texDataBuffer)
		{
			if (alphaBmp)
				DeleteObject(alphaBmp);

			DeleteObject(mainBmp);
			return 0;
		}

		BmpDataNode* bmpDataNode = AllocateBmpDataNode();

		if (! bmpDataNode)
		{
			free(texDataBuffer);

			if (alphaBmp)
				DeleteObject(alphaBmp);

			DeleteObject(mainBmp);
			return 0;
		}

		bmpDataNode->texData = texDataBuffer;

		int32_t texSize = CalculateTexSize(mainBmp, flags);

		bmpDataNode->textureHeight = texSize;
		bmpDataNode->textureWidth = texSize;
		bmpDataNode->bitmapWidth = GetBitmapWidth(mainBmp);
		bmpDataNode->bitmapHeight = GetBitmapHeight(mainBmp);
		bmpDataNode->flags = flags;

		strcpy(bmpDataNode->texName, rawTexStr);

		if (alphaBmp)
			DeleteObject(alphaBmp);

		if ((flags & 32) != 0)
			bmpDataNode->bitmapHandle = mainBmp;
		else
			DeleteObject(mainBmp);

		InitialiseTextureSurface(bmpDataNode);

		return bmpDataNode;
	}

	// FUNCTION: TOY2 0x004AFC70
	BmpDataNode* GetBmpDataNodeByName(const char* texName)
	{
		BmpDataNode* nodeIter;

		for (nodeIter = g_bmpDataHead; nodeIter; nodeIter = nodeIter->next)
		{
			if (! strcmpi(texName, nodeIter->texName))
				break;
		}

		return nodeIter;
	}

	// FUNCTION: TOY2 0x004B0CF0
	void AddRef(BmpDataNode* bmpDataNode) { ++bmpDataNode->refCount; }

	// FUNCTION: TOY2 0x004B0C90
	BmpDataNode* LoadLocalBmpTexture(const char* rawTexStr, int32_t flags)
	{
		BmpDataNode* dataNode = GetBmpDataNodeByName(rawTexStr);

		if (dataNode)
		{
			AddRef(dataNode);
			return dataNode;
		}
		else
		{
			FILE* fileHandle = fopen(rawTexStr, "rb");

			if (fileHandle)
			{
				BmpDataNode* loadedNode = LoadTextureByStream(fileHandle, rawTexStr, flags);
				fclose(fileHandle);

				return loadedNode;
			}
			else
			{
				return 0;
			}
		}
	}

	// FUNCTION: TOY2 0x004AC1A0 [MATCHED]
	HRESULT SetTexture(int32_t stageIndex, BmpDataNode* bmpDataNode)
	{
		g_currentBmpDataNode = bmpDataNode;

		if (! Renderer::g_isSoftwareRendering)
		{
			if (bmpDataNode)
				return DrawingDevice::g_drawingDevice->m_pd3dDevice->SetTexture(stageIndex, bmpDataNode->d3dTexture);

			return DrawingDevice::g_drawingDevice->m_pd3dDevice->SetTexture(stageIndex, 0);
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004B0D90
	void GetDIBPixelColor(RGBA* color, DIBSECTION* dibSection, RGBQUAD* colorTables, int32_t* rowBasePtr, int32_t xOffset)
	{
		color->a = 255;

		switch ((int32_t)dibSection->dsBmih.biBitCount)
		{
			case 24: {
				uint8_t* pixelPtr = (uint8_t*)rowBasePtr + xOffset + 2 * xOffset;

				color->b = *pixelPtr++;
				color->g = *pixelPtr;
				color->r = pixelPtr[1];
				break;
			}
			case 8: {
				uint8_t* pixelPtr = (uint8_t*)rowBasePtr + xOffset;

				color->b = colorTables[*pixelPtr].rgbBlue;
				color->g = colorTables[*pixelPtr].rgbGreen;
				color->r = colorTables[*pixelPtr].rgbRed;
				break;
			}
		}
	}

	// FUNCTION: TOY2 0x004B0E00
	int32_t CopyToDDSurface(BmpDataNode* bmpDataNode, LPDIRECTDRAWSURFACE4 ddSurface)
	{
		int32_t result = 0;

		if (! bmpDataNode)
			return result;

		HBITMAP bmpHandle = bmpDataNode->bitmapHandle;
		if (! bmpHandle)
			return result;

		DIBSECTION dibSection;
		if (! GetObjectA(bmpHandle, sizeof(DIBSECTION), &dibSection))
			return result;

		DDSURFACEDESC2 surfaceDesc;
		memset(&surfaceDesc, 0, sizeof(surfaceDesc));
		surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);

		HRESULT lockResult;
		do
		{
			lockResult = ddSurface->Lock(0, &surfaceDesc, DDLOCK_NOSYSLOCK, 0);
		} while (lockResult == DDERR_WASSTILLDRAWING);

		if (lockResult < 0)
		{
			Logger::LogD3DError(lockResult);
			return 0;
		}

		HDC dc = CreateCompatibleDC(0);

		if (dc)
		{
			RGBQUAD colorTables[256];
			HGDIOBJ oldObject = SelectObject(dc, bmpDataNode->bitmapHandle);

			if (dibSection.dsBmih.biBitCount == 8)
				GetDIBColorTable(dc, 0, 256, colorTables);

			PixelFormatInfo pixelFormat;
			CalculatePixelFormatShifts(&pixelFormat, &surfaceDesc);

			for (uint32_t currentRow = 0; currentRow < surfaceDesc.dwHeight; ++currentRow)
			{
				uint32_t width = surfaceDesc.dwWidth;
				uint32_t bitCount = surfaceDesc.ddpfPixelFormat.dwRGBBitCount;

				int32_t* srcRowPtr =
					(int32_t*)((uint8_t*)dibSection.dsBm.bmBits
							   + dibSection.dsBm.bmWidthBytes * (dibSection.dsBm.bmHeight - currentRow * dibSection.dsBm.bmHeight / surfaceDesc.dwHeight - 1));

				int32_t* destRowPtr = (int32_t*)((uint8_t*)surfaceDesc.lpSurface + currentRow * surfaceDesc.lPitch);

				if (bitCount >= 15)
				{
					if (bitCount > 16)
					{
						if (bitCount == 32)
						{
							for (uint32_t xPixel = 0; xPixel < surfaceDesc.dwWidth; ++xPixel)
							{
								RGBA color;
								GetDIBPixelColor(&color, &dibSection, colorTables, srcRowPtr, xPixel * dibSection.dsBm.bmWidth / width);

								int32_t shiftedGreen;
								if (pixelFormat.greenShift < 0)
									shiftedGreen = color.g >> -(int8_t)pixelFormat.greenShift;
								else
									shiftedGreen = color.g << (int8_t)pixelFormat.greenShift;

								int32_t maskedGreen = pixelFormat.greenMask & shiftedGreen;

								int32_t shiftedBlue;
								if (pixelFormat.blueShift < 0)
									shiftedBlue = color.b >> -(int8_t)pixelFormat.blueShift;
								else
									shiftedBlue = color.b << (int8_t)pixelFormat.blueShift;

								int32_t maskedBlue = (pixelFormat.blueMask & shiftedBlue) | maskedGreen;

								int32_t shiftedRed;
								if (pixelFormat.redShift < 0)
									shiftedRed = color.r >> -(int8_t)pixelFormat.redShift;
								else
									shiftedRed = color.r << (int8_t)pixelFormat.redShift;

								*destRowPtr++ = maskedBlue | (pixelFormat.redMask & shiftedRed);

								width = surfaceDesc.dwWidth;
							}
						}
					}
					else
					{
						// 15 or 16 bit
						for (uint32_t xPixel = 0; xPixel < surfaceDesc.dwWidth; ++xPixel)
						{
							RGBA color;
							GetDIBPixelColor(&color, &dibSection, colorTables, srcRowPtr, xPixel * dibSection.dsBm.bmWidth / width);

							int32_t shiftedGreen;
							if (pixelFormat.greenShift < 0)
								shiftedGreen = color.g >> -(int8_t)pixelFormat.greenShift;
							else
								shiftedGreen = color.g << (int8_t)pixelFormat.greenShift;

							int16_t maskedGreen = (uint16_t)pixelFormat.greenMask & shiftedGreen;

							int32_t shiftedBlue;
							if (pixelFormat.blueShift < 0)
								shiftedBlue = color.b >> -(int8_t)pixelFormat.blueShift;
							else
								shiftedBlue = color.b << (int8_t)pixelFormat.blueShift;

							int16_t maskedBlue = ((uint16_t)pixelFormat.blueMask & shiftedBlue) | maskedGreen;

							int32_t shiftedRed;
							if (pixelFormat.redShift < 0)
								shiftedRed = color.r >> -(int8_t)pixelFormat.redShift;
							else
								shiftedRed = color.r << (int8_t)pixelFormat.redShift;

							*(uint16_t*)destRowPtr = maskedBlue | ((uint16_t)pixelFormat.redMask & shiftedRed);
							destRowPtr = (int32_t*)((uint8_t*)destRowPtr + 2);

							width = surfaceDesc.dwWidth;
						}

						bitCount = surfaceDesc.ddpfPixelFormat.dwRGBBitCount;
					}
				}

				result = 1;
			}

			SelectObject(dc, oldObject);
			DeleteDC(dc);
		}

		ddSurface->Unlock(0);

		return result;
	}

	// FUNCTION: TOY2 0x004AFBE0
	void FreeTexData(BmpDataNode* bmpDataNode)
	{
		if (bmpDataNode->texData)
			free(bmpDataNode->texData);

		bmpDataNode->texData = 0;
	}

	// FUNCTION: TOY2 0x004AFB20
	void ReleaseBmpDataNode(BmpDataNode* bmpDataNode)
	{
		int32_t refCount = bmpDataNode->refCount;

		if (refCount)
		{
			bmpDataNode->refCount = refCount - 1;
		}
		else
		{
			BmpDataNode* next = bmpDataNode->next;

			if (next)
				next->prev = bmpDataNode->prev;

			BmpDataNode* prev = bmpDataNode->prev;

			if (prev)
				prev->next = bmpDataNode->next;
			else
				g_bmpDataHead = bmpDataNode->next;

			DestroyBmpDataNode(bmpDataNode);
			FreeTexData(bmpDataNode);

			if (bmpDataNode->bitmapHandle)
				DeleteObject(bmpDataNode->bitmapHandle);

			free(bmpDataNode);
		}
	}

	// FUNCTION: TOY2 0x004B1150
	void FreeAllBmpDataNodes()
	{
		for (BmpDataNode* curNode = g_bmpDataHead; g_bmpDataHead; curNode = g_bmpDataHead)
		{
			curNode->refCount = 0;
			ReleaseBmpDataNode(g_bmpDataHead);
		}
	}

	// FUNCTION: TOY2 0x004B1180
	void FreeAllBmpDataNodes_T() { FreeAllBmpDataNodes(); }

	// FUNCTION: TOY2 0x004AC260
	int32_t ReleaseBmpDataNode_T(Nu3D::BmpDataNode* Block)
	{
		Nu3D::ReleaseBmpDataNode(Block);
		return 0;
	}

	// FUNCTION: TOY2 0x004B0620
	Nu3D::BmpDataNode* BuildRawBmpNode(HANDLE mainBmp, HANDLE alphaBmp, const char* texName, int32_t flags)
	{
		uint32_t* pixelData = Nu3D::ProcessBmpPixelData(mainBmp, alphaBmp, flags);

		if (pixelData)
		{
			Nu3D::BmpDataNode* bmpDataNode = Nu3D::AllocateBmpDataNode();

			if (bmpDataNode)
			{
				bmpDataNode->texData = pixelData;

				int32_t texSize = Nu3D::CalculateTexSize(mainBmp, flags);

				bmpDataNode->textureHeight = texSize;
				bmpDataNode->textureWidth = texSize;
				bmpDataNode->bitmapWidth = Nu3D::GetBitmapWidth(mainBmp);
				bmpDataNode->bitmapHeight = Nu3D::GetBitmapHeight(mainBmp);
				bmpDataNode->flags = flags;

				strcpy(bmpDataNode->texName, texName);

				Nu3D::InitialiseTextureSurface(bmpDataNode);

				return bmpDataNode;
			}

			free(pixelData);
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004AD060
	HRESULT BuildBmpNode(HANDLE mainBmp, const char* texName, int32_t unused, int32_t flags)
	{
		return Nu3D::BuildRawBmpNode(mainBmp, 0, texName, flags) != 0 ? 0 : DDERR_OUTOFMEMORY;
	}

	// FUNCTION: TOY2 0x004AD090
	HRESULT BuildBmpNodeWithAlpha(HANDLE mainBmp, const char* texName, int32_t unused, int32_t flags, HANDLE alphaBmp)
	{
		return Nu3D::BuildRawBmpNode(mainBmp, alphaBmp, texName, flags) != 0 ? 0 : DDERR_OUTOFMEMORY;
	}

	// FUNCTION: TOY2 0x004AD0C0
	Nu3D::BmpDataNode* GetBmpDataNodeByName_T(char* texName) { return Nu3D::GetBmpDataNodeByName(texName); }

	// FUNCTION: TOY2 0x004AD1C0 [MATCHED]
	int32_t InitBmpNodeSurface(Nu3D::BmpDataNode* bmpNode)
	{
		if (bmpNode)
		{
			Nu3D::InitialiseTextureSurface(bmpNode);
			return 0;
		}

		return -1;
	}

	// FUNCTION: TOY2 0x004BB270
	int32_t CreateTextureResource(HANDLE bmpHandle, char* texName, int32_t flags)
	{
		NGNLoader::NGNTextureData* textureData = NGNLoader::AllocateTextureData();

		if (textureData)
		{
			textureData->isTex14 = 0;
			textureData->color.r = 0;
			textureData->color.g = 0;
			textureData->color.b = 0;
			textureData->bmpDataNode = 0;

			if (DrawingDevice::GetD3DDevice())
			{
				if (! Nu3D::BuildBmpNode(bmpHandle, texName, 0, flags))
				{
					Nu3D::BmpDataNode* bmpDataNode = Nu3D::GetBmpDataNodeByName_T(texName);
					textureData->bmpDataNode = bmpDataNode;

					if (bmpDataNode)
					{
						Nu3D::InitBmpNodeSurface(bmpDataNode);
						return textureData->textureIndex;
					}
				}
			}

			NGNLoader::ReleaseTextureData(textureData);
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004CA0C0
	void CopyNormalsFromNearestVertex(Nu3D::Creature* creature, int32_t vertexIndex, Nu3D::Vertex* targetVertex)
	{
		double minDistanceSquared = 3.4028235e38f;

		Nu3D::Primitive* currentPrim = creature->primitives[vertexIndex];

		if (currentPrim)
		{
			do
			{
				int32_t vertexId = 0;

				if (currentPrim->patchVerts.vertexCount > 0)
				{
					int32_t vertOffset = 0;

					do
					{
						Nu3D::Vertex* candidateVertex = &currentPrim->patchVerts.data.vertices[vertOffset];

						double deltaY = candidateVertex->position.y - targetVertex->position.y;
						double deltaX = candidateVertex->position.x - targetVertex->position.x;
						double deltaZ = candidateVertex->position.z - targetVertex->position.z;

						float distanceSquared = (float)(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);

						if (distanceSquared < minDistanceSquared && distanceSquared < 1024.0)
						{
							minDistanceSquared = distanceSquared;

							targetVertex->normals = candidateVertex->normals;

							if (distanceSquared < 1.0)
								return;
						}

						++vertexId;
						++vertOffset;
					} while (vertexId < currentPrim->patchVerts.vertexCount);
				}

				currentPrim = currentPrim->next;
			} while (currentPrim);

			if (minDistanceSquared != 3.4028235e38f)
				return;
		}

		Logger::DebugLog("warning - unable to find similar vertex in limb\r\n");
	}

	// FUNCTION: TOY2 0x004CBC40 [MATCHED]
	int32_t CopyShapeVertex(int32_t vertexIndex, Nu3D::Vertex* vertex)
	{
		if (vertexIndex < NGNLoader::ObjectLoad::g_curVertexCount && vertexIndex >= 0)
		{
			if (vertex)
			{
				memcpy(vertex, &NGNLoader::ObjectLoad::g_shapeVertices[vertexIndex], sizeof(Nu3D::Vertex));
				return 1;
			}
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004AD130
	LPDIRECT3DTEXTURE2 BmpDataNode::GetTexture(Nu3D::BmpDataNode* bmpDataNode)
	{
		if (bmpDataNode)
			return bmpDataNode->d3dTexture;
		else
			return 0;
	}

	// FUNCTION: TOY2 0x004B1A60
	void SetAlternateMediaDirectory(const char* directory)
	{
		strcpy(g_alternateMediaDirectory, directory ? directory : Toy2::g_emptyStr);
	}

	// FUNCTION: TOY2 0x004B1B60
	HRESULT LoadAlternateBitmap(AlternateBmpDataNode* node)
	{
		char* extension = strrchr(node->name, '.');
		char resolvedPath[256];

		if (! extension)
			return E_NOTIMPL;

		node->mainBitmap = (HBITMAP)LoadImageA(GetModuleHandleA(NULL), node->name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		if (node->mainBitmap)
			return S_OK;

		if (FAILED(ResolveAlternateMediaPath(node->name, g_alternateMediaDirectory, resolvedPath)))
			return DDERR_NOTFOUND;

		if (lstrcmpiA(extension, ".bmp") != 0)
			return E_NOTIMPL;

		node->mainBitmap = (HBITMAP)LoadImageA(GetModuleHandleA(NULL), resolvedPath, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		if (! node->mainBitmap)
			node->mainBitmap = (HBITMAP)LoadImageA(NULL, resolvedPath, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

		return node->mainBitmap ? S_OK : DDERR_NOTFOUND;
	}

	// FUNCTION: TOY2 0x004B1C50
	HRESULT ResolveAlternateMediaPath(const char* filename, const char* mediaDirectory, char* outputPath)
	{
		HKEY key;
		DWORD pathBytes;
		DWORD valueType;
		char sdkRoot[512];
		FILE* file;

		strcpy(outputPath, filename);
		file = fopen(outputPath, "rb");
		if (file && fclose(file) == 0)
			return S_OK;

		char* d3dPath = getenv("D3DPATH");
		if (d3dPath)
		{
			sprintf(outputPath, "%s\\%s", d3dPath, filename);
			file = fopen(outputPath, "rb");
			if (file && fclose(file) == 0)
				return S_OK;
		}

		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\DirectX", 0, KEY_READ, &key) != ERROR_SUCCESS)
			return DDERR_NOTFOUND;

		pathBytes = 512;
		LONG queryResult = RegQueryValueExA(key, g_alternateSdkValueName, NULL, &valueType, (LPBYTE)sdkRoot, &pathBytes);
		RegCloseKey(key);
		if (queryResult != ERROR_SUCCESS)
			return DDERR_NOTFOUND;

		sprintf(outputPath, "%s\\D3DIM\\Media\\%s", sdkRoot, filename);
		file = fopen(outputPath, "rb");
		if (file && fclose(file) == 0)
			return S_OK;

		sprintf(outputPath, "%s\\%s%s", sdkRoot, mediaDirectory, filename);
		file = fopen(outputPath, "rb");
		if (file && fclose(file) == 0)
			return S_OK;

		return DDERR_NOTFOUND;
	}
}

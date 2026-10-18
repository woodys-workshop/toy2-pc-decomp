#pragma once

#include "NGNLoader/NGNTypes.h"
#include <windows.h>
#include <cstdio>
#include "Common.h"

namespace NGNLoader
{
	extern NGNImage* g_ngnImage;
	extern NGNTextureData g_textureDataFreeList[2000];
	extern Vector3F g_vertexScaleVector;

	void SetNewImage(char* fileName);
	void Init();
	void DetectBackdropTextures();
	int32_t GetTextureDataIndex(int32_t textureIndex);
	uint32_t GetTextureDataIndexByName(char* texName);
	NGNTextureData* GetTextureDataByIndex(uint32_t texDataIndex);
	void RetrieveTextureData(
		int32_t texDataIndex,
		uint32_t* bitmapWidthOut,
		uint32_t* bitmapHeightOut,
		uint32_t* textureWidth,
		uint32_t* textureHeight,
		uint32_t** textureData);
	HBITMAP GetBmpHandle(int32_t index);
	int32_t CopyToDDSurfaceByIndex(int32_t texIndex, LPDIRECTDRAWSURFACE4 ddSurface);
	uint32_t GetOrAllocateTexture(NGNTextureParams* texParams);
	NGNLoader::NGNTextureData* ReleaseTextureData(NGNLoader::NGNTextureData* list);
	void ReleaseAllTextures();
	NGNTextureData* AllocateTextureData();
}

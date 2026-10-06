#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Nu3D/Creature.h"
#include "Nu3D/Nu3D.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include <windows.h>
#include <stdio.h>

namespace Nu3D
{
	struct BmpDataNode
	{
		LPDIRECTDRAWSURFACE4 surface;
		LPDIRECT3DTEXTURE2 d3dTexture;
		DDSURFACEDESC2 surfaceDesc;
		uint32_t* texData;
		int32_t textureWidth;
		int32_t textureHeight;
		uint32_t bitmapWidth;
		uint32_t bitmapHeight;
		HBITMAP bitmapHandle;
		int32_t unkVar1;
		int32_t unkVar2;
		int32_t unkVar3;
		int32_t unkVar4;
		char texName[80];
		int32_t unkVar5;
		int32_t flags;
		int32_t refCount;
		BmpDataNode* next;
		BmpDataNode* prev;

		static LPDIRECT3DTEXTURE2 GetTexture(BmpDataNode* bmpDataNode);
	};

	struct PixelFormatInfo
	{
		int32_t alphaShift;
		int32_t redShift;
		int32_t greenShift;
		int32_t blueShift;
		int32_t alphaMask;
		int32_t redMask;
		int32_t greenMask;
		int32_t blueMask;
	};

	struct FindPixelFormat
	{
		uint32_t bpp;
		int32_t minAlphaBits;
		uint32_t needAlpha;
		uint32_t valid;
		DDPIXELFORMAT* out;
	};

	extern BmpDataNode* g_currentBmpDataNode;

	void SetMinTexSize(int32_t minTexSize);
	BmpDataNode* LoadTextureByStream(FILE* handle, const char* rawTexStr, int32_t flags);
	Nu3D::BmpDataNode* LoadLocalBmpTexture(const char* rawTexStr, int32_t flags);
	HRESULT SetTexture(int32_t stageIndex, BmpDataNode* bmpDataNode);
	int32_t CopyToDDSurface(BmpDataNode* bmpDataNode, LPDIRECTDRAWSURFACE4 ddSurface);
	void FreeAllBmpDataNodes_T();
	int32_t ReleaseBmpDataNode_T(Nu3D::BmpDataNode* Block);
	Nu3D::BmpDataNode* GetBmpDataNodeByName_T(char* texName);
	int32_t CreateTextureResource(HANDLE bmpHandle, char* texName, int32_t flags);
	void CopyNormalsFromNearestVertex(Nu3D::Creature* creature, int32_t vertexIndex, Nu3D::Vertex* targetVertex);
	void CopyShapeVertex(int32_t vertexIndex, Nu3D::Vertex* vertex);

	STATIC_ASSERT(sizeof(BmpDataNode) == 0x110);
}
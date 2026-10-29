#pragma once

#include "Common.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>

namespace Renderer
{
	struct RenderEntry;
}

namespace Nu3D
{
	struct MaterialFile
	{
		RGBColor diffuseColor;
		RGBColor ambientColor;
		RGBColor specularColor;
		float emissiveIntensity;
		float power;
		float opacity;
		int32_t metadata;
		uint16_t texDataIndex;
	};

	struct Material
	{
		Material* next;
		Material* prev;
		Material* alwaysZero;
		int32_t id;
		D3DMATERIAL d3dMaterial;
		D3DMATERIALHANDLE d3dMaterialHandle;
		LPDIRECT3DMATERIAL3 direct3DMat3;
		uint32_t texDataIndex;
		float opacity;
		Renderer::RenderEntry* renderEntryHead;
		int32_t metadata;
		int32_t originalMetadata;
		float horzOffset;
		float vertOffset;

		static void Init();
		static Material* Allocate();
		static Material* GetFreeByIndex(int32_t index);
		static Material* GetHead();
		static void InsertSorted(Material* material);
		static ULONG Release(LPDIRECT3DMATERIAL3 matHandle);
		static void ReleaseFromList(Material* material);
		static void ConvertFileToMaterial(Material* material, MaterialFile* materialFile);
		static Material* TryCache(MaterialFile* materialFile);
		static void Unlink(Material* material);
		static HRESULT SetMaterial(LPDIRECT3DMATERIAL3 direct3DMaterial3, LPD3DMATERIAL d3dMaterial);
		static HRESULT GetHandle(LPDIRECT3DMATERIAL3 direct3DMaterial3, LPD3DMATERIALHANDLE d3dMaterialHandle);
		static void AttachTexture(Material* material, uint32_t texDataIndex);
		static Material* CreateFromColor(RGBColor* rgbColor);
		static Material* CreateFromFile(MaterialFile* materialFile);
		static float SetOpacity(Material* material, float alpha);
	};

	// the environment mapped (reflection) versions of the material currently being drawn
	extern Material* g_tex14Version1;
	extern Material* g_tex14Version2;
	extern Material* g_tex14Version3;

	STATIC_ASSERT(sizeof(Material) == 0x84);
	STATIC_ASSERT(sizeof(MaterialFile) == 0x38);
}
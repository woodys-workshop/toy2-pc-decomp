#pragma once

#include "Common.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>

namespace Nu3D
{
	struct Light
	{
		D3DMATRIX matrix;
		BGRA_F color;
		int32_t lightType; // 0 = ambient, otherwise a Direct3D light
		float range;
		int32_t innerConeAngle;
		int32_t outerConeAngle;
		int32_t active;
		Light* next;
		Light* prev;
		LPDIRECT3DLIGHT direct3dLight;
		D3DLIGHT2 d3dLightData;

		static int32_t InitPool(int32_t poolSize);

		static Light* Alloc();
		static Light* Free(Light* light);
		static Light* BuildLight(D3DMATRIX* matrix, BGRA_F* color, int32_t active);
		static Light* BuildAmbient(BGRA_F* color, int32_t active);

		static void BuildGlobalLights();
		static void Destroy(Light* light);
		static void DestroyAllLights();
		static void Cleanup();
		static void Update(Light* light);
		static void SetDirectionalLight(int32_t dirX, int32_t dirY, int32_t dirZ, int32_t colorR, int32_t colorG, int32_t colorB);
	};

	STATIC_ASSERT(sizeof(Light) == 0xC0);
}

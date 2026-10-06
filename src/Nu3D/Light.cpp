#include "Nu3D/Light.h"
#include "Nu3D/Math.h"
#include "DrawingDevice.h"

#include <MATH.H>
#include <STDLIB.H>
#include <STRING.H>
#include <FLOAT.H>

namespace Nu3D
{
	// GLOBAL: TOY2 0x00AAD79C
	Light* g_lightPool;

	// GLOBAL: TOY2 0x00AAD7A0
	Light* g_lightsFreeList;

	// GLOBAL: TOY2 0x00AAD7A4
	Light* g_lightsUsedList;

	// GLOBAL: TOY2 0x00B62428
	Light* g_globalLight1;

	// GLOBAL: TOY2 0x00B62418
	Light* g_globalLight2;

	// GLOBAL: TOY2 0x00B6241C
	Light* g_globalLight3;

	// GLOBAL: TOY2 0x00B62420
	Light* g_globalLight4;

	// GLOBAL: TOY2 0x00B62424
	Light* g_globalLight5;

	// FUNCTION: TOY2 0x004C2FF0
	int32_t Light::InitPool(int32_t poolSize)
	{
		if (g_lightPool)
			DestroyAllLights();

		g_lightPool = (Light*)malloc(sizeof(Light) * poolSize);

		if (! g_lightPool)
			return 0;

		// chain every light in the pool together to form the free list
		for (int32_t i = 1; i < poolSize; i++)
		{
			g_lightPool[i].prev = &g_lightPool[i - 1];
			g_lightPool[i - 1].next = &g_lightPool[i];
		}

		g_lightPool[0].prev = 0;
		g_lightPool[poolSize - 1].next = 0;

		g_lightsFreeList = g_lightPool;
		g_lightsUsedList = 0;

		return 1;
	}

	// FUNCTION: TOY2 0x004C2A70
	Light* Light::Alloc()
	{
		Light* light = g_lightsFreeList;

		if (g_lightsFreeList)
		{
			g_lightsFreeList = g_lightsFreeList->next;

			light->next = g_lightsUsedList;

			if (g_lightsUsedList)
				g_lightsUsedList->prev = light;

			light->prev = 0;

			g_lightsUsedList = light;

			light->direct3dLight = 0;
		}

		return light;
	}

	// FUNCTION: TOY2 0x004C29D0
	Light* Light::Free(Light* light)
	{
		if (light->next)
			light->next->prev = light->prev;

		if (light->prev)
			light->prev->next = light->next;
		else
			g_lightsUsedList = light->next;

		light->next = g_lightsFreeList;
		g_lightsFreeList = light;

		return light;
	}

	// FUNCTION: TOY2 0x004C2BE0
	Light* Light::BuildLight(D3DMATRIX* matrix, BGRA_F32* color, int32_t active)
	{
		Light* light = Alloc();

		if (! light)
			return 0;

		if (DrawingDevice::CreateLight(&light->direct3dLight))
		{
			Free(light);
			return 0;
		}

		memcpy(&light->matrix, matrix, sizeof(D3DMATRIX));

		light->innerConeAngle = 0x8000;
		light->outerConeAngle = 0x8000;
		light->lightType = 2;

		float range = sqrt(FLT_MAX);

		light->range = range;
		light->color.b = color->b;
		light->color.g = color->g;
		light->color.r = color->r;
		light->color.a = color->a;

		light->d3dLightData.dwSize = sizeof(D3DLIGHT2);

		light->active = active != 0;

		light->d3dLightData.dcvColor.r = light->color.r;
		light->d3dLightData.dcvColor.g = light->color.g;
		light->d3dLightData.dcvColor.b = light->color.b;
		light->d3dLightData.dcvColor.a = light->color.a;
		light->d3dLightData.dltType = D3DLIGHT_DIRECTIONAL;

		Math::GetPositionVector(&light->matrix, (Vector3F*)&light->d3dLightData.dvPosition);
		Math::GetForwardVector(&light->matrix, (Vector3F*)&light->d3dLightData.dvDirection);

		light->d3dLightData.dvFalloff = 1.0;
		light->d3dLightData.dvAttenuation0 = 1.0;
		light->d3dLightData.dvAttenuation1 = 1.0;
		light->d3dLightData.dvAttenuation2 = 1.0;
		light->d3dLightData.dvRange = range;
		light->d3dLightData.dvTheta = 0.0;
		light->d3dLightData.dvPhi = 0.0;
		light->d3dLightData.dwFlags = light->active;

		DrawingDevice::SetLight(light->direct3dLight, (LPD3DLIGHT)&light->d3dLightData);
		DrawingDevice::AddLight(light->direct3dLight);

		return light;
	}

	// FUNCTION: TOY2 0x004C2A20
	Light* Light::BuildAmbient(BGRA_F32* color, int32_t active)
	{
		Light* light = Alloc();

		if (light)
		{
			light->lightType = 0;
			light->color = *color;
			light->active = active != 0;

			Update(light);
		}

		return light;
	}

	// FUNCTION: TOY2 0x004CE910
	void Light::BuildGlobalLights()
	{
		BGRA_F32 color;
		D3DMATRIX matrix;

		InitPool(8);

		// ambient light, starts off black
		color.a = 1.0;
		memset(&color, 0, 12);

		g_globalLight1 = BuildAmbient(&color, 1);

		// three white directional lights, 120 degrees apart
		color.a = 1.0;
		color.b = 1.0;
		color.g = 1.0;
		color.r = 1.0;

		Math::SetRotationYFromU16AngleLUT(&matrix, 0);
		g_globalLight2 = BuildLight(&matrix, &color, 1);

		Math::SetRotationYFromU16AngleLUT(&matrix, 21845);
		g_globalLight3 = BuildLight(&matrix, &color, 1);

		Math::SetRotationYFromU16AngleLUT(&matrix, 43690);
		g_globalLight4 = BuildLight(&matrix, &color, 1);

		// the directional light that gets driven by SetDirectionalLight, starts off black
		color.a = 1.0;
		memset(&color, 0, 12);

		g_globalLight5 = BuildLight(&matrix, &color, 1);
	}

	// FUNCTION: TOY2 0x004C29A0
	Light* Light::Destroy(Light* light)
	{
		if (light)
		{
			if (light->direct3dLight)
			{
				DrawingDevice::DeleteLight(light->direct3dLight);
				DrawingDevice::ReleaseLight(light->direct3dLight);
			}

			return Free(light);
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004C3090
	void Light::DestroyAllLights()
	{
		if (g_lightPool)
		{
			while (g_lightsUsedList)
				Destroy(g_lightsUsedList);

			free(g_lightPool);
			g_lightPool = 0;
		}
	}

	// FUNCTION: TOY2 0x004CEA50
	void Light::Cleanup()
	{
		if (g_globalLight2)
			Destroy(g_globalLight2);

		if (g_globalLight3)
			Destroy(g_globalLight3);

		if (g_globalLight4)
			Destroy(g_globalLight4);

		if (g_globalLight5)
			Destroy(g_globalLight5);

		if (g_globalLight1)
			Destroy(g_globalLight1);

		DestroyAllLights();

		g_globalLight1 = 0;
		g_globalLight5 = 0;
		g_globalLight4 = 0;
		g_globalLight3 = 0;
		g_globalLight2 = 0;
	}

	// FUNCTION: TOY2 0x004C2E70
	void Light::Update(Light* light)
	{
		if (light->lightType)
		{
			Math::GetPositionVector(&light->matrix, (Vector3F*)&light->d3dLightData.dvPosition);
			Math::GetForwardVector(&light->matrix, (Vector3F*)&light->d3dLightData.dvDirection);

			light->d3dLightData.dcvColor.r = light->color.r;
			light->d3dLightData.dcvColor.g = light->color.g;
			light->d3dLightData.dcvColor.b = light->color.b;
			light->d3dLightData.dcvColor.a = light->color.a;

			// cone angles are 16-bit angles, convert them to radians
			light->d3dLightData.dvTheta = light->innerConeAngle * 0.000095873802;
			light->d3dLightData.dvRange = light->range;
			light->d3dLightData.dwFlags = light->active & 1;
			light->d3dLightData.dvPhi = light->outerConeAngle * 0.000095873802;

			DrawingDevice::SetLight(light->direct3dLight, (LPD3DLIGHT)&light->d3dLightData);
		}
		else if ((light->active & 1) != 0)
		{
			// the ambient light is not a real D3D light, it sets the device's ambient light state instead
			RGBA ambient;

			int64_t channel = (int64_t)(light->color.a * 255.0);

			if (channel > 0xFF)
				ambient.a = 0xFF;
			else if (channel >= 0)
				ambient.a = (uint8_t)(light->color.a * 255.0);
			else
				ambient.a = 0;

			channel = (int64_t)(light->color.r * 255.0);

			if (channel > 0xFF)
				ambient.r = 0xFF;
			else if (channel >= 0)
				ambient.r = (uint8_t)(light->color.r * 255.0);
			else
				ambient.r = 0;

			channel = (int64_t)(light->color.g * 255.0);

			if (channel > 0xFF)
				ambient.g = 0xFF;
			else if (channel >= 0)
				ambient.g = (uint8_t)(light->color.g * 255.0);
			else
				ambient.g = 0;

			channel = (int64_t)(light->color.b * 255.0);

			if (channel > 0xFF)
				ambient.b = 0xFF;
			else if (channel >= 0)
				ambient.b = (uint8_t)(light->color.b * 255.0);
			else
				ambient.b = 0;

			DrawingDevice::SetLightState(D3DLIGHTSTATE_AMBIENT, ambient.value);
		}
	}

	// FUNCTION: TOY2 0x004CE6C0
	void Light::SetDirectionalLight(int32_t dirX, int32_t dirY, int32_t dirZ, int32_t colorR, int32_t colorG, int32_t colorB)
	{
		if (dirX || dirY || dirZ)
		{
			Vector3F direction;

			direction.x = dirX;
			direction.y = dirY;
			direction.z = dirZ;

			Math::BuildMatrixFromDirection(&g_globalLight5->matrix, &direction);
		}
		else
		{
			Math::BuildIdentityMatrix(&g_globalLight5->matrix);
		}

		g_globalLight5->color.r = colorR * 0.0078125;
		g_globalLight5->color.g = colorG * 0.0078125;
		g_globalLight5->color.b = colorB * 0.0078125;

		Update(g_globalLight5);
	}
}

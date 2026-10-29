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

	// FUNCTION: TOY2 0x004C2FF0 [MATCHED]
	int32_t Light::InitPool(int32_t poolSize)
	{
		if (g_lightPool)
			DestroyAllLights();

		g_lightPool = (Light*)malloc(sizeof(Light) * poolSize);

		if (g_lightPool)
		{
			// chain every light in the pool together to form the free list
			int32_t lightIndex;

			for (lightIndex = 1; lightIndex < poolSize; lightIndex++)
			{
				g_lightPool[lightIndex].prev = &g_lightPool[lightIndex - 1];
				g_lightPool[lightIndex - 1].next = &g_lightPool[lightIndex];
			}

			g_lightPool[0].prev = 0;
			g_lightPool[lightIndex - 1].next = 0;

			g_lightsFreeList = g_lightPool;
			g_lightsUsedList = 0;

			return 1;
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004C2A70 [MATCHED]
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

	// FUNCTION: TOY2 0x004C29D0 [MATCHED]
	Light* Light::Free(Light* light)
	{
		if (light->next)
			light->next->prev = light->prev;

		Light* prev = light->prev;
		Light* next = light->next;

		if (prev)
			prev->next = next;
		else
			g_lightsUsedList = next;

		light->next = g_lightsFreeList;
		g_lightsFreeList = light;

		return light;
	}

	// FUNCTION: TOY2 0x004C2BE0 [MATCHED]
	Light* Light::BuildLight(D3DMATRIX* matrix, BGRA_F* color, int32_t active)
	{
		Light* light = Alloc();

		if (light)
		{
			if (! DrawingDevice::CreateLight(&light->direct3dLight))
			{
				memcpy(&light->matrix, matrix, sizeof(D3DMATRIX));

				light->innerConeAngle = 0x8000;
				light->outerConeAngle = 0x8000;
				light->lightType = 2;

				float range = sqrt(FLT_MAX);

				light->range = range;
				light->color = *color;

				BOOL isActive = active != 0;

				light->d3dLightData.dwSize = sizeof(D3DLIGHT2);
				light->active = isActive;

				light->d3dLightData.dltType = D3DLIGHT_DIRECTIONAL;
				light->d3dLightData.dcvColor.r = light->color.r;
				light->d3dLightData.dcvColor.g = light->color.g;
				light->d3dLightData.dcvColor.b = light->color.b;
				light->d3dLightData.dcvColor.a = light->color.a;

				Math::GetPositionVector(&light->matrix, (Vector3F*)&light->d3dLightData.dvPosition);
				Math::GetForwardVector(&light->matrix, (Vector3F*)&light->d3dLightData.dvDirection);

				light->d3dLightData.dvFalloff = 1.0;
				light->d3dLightData.dvAttenuation0 = 1.0;
				light->d3dLightData.dvAttenuation1 = 1.0;
				light->d3dLightData.dvAttenuation2 = 1.0;
				light->d3dLightData.dvRange = range;
				light->d3dLightData.dvTheta = 0.0;
				light->d3dLightData.dvPhi = 0.0;
				light->d3dLightData.dwFlags = isActive;

				DrawingDevice::SetLight(light->direct3dLight, &light->d3dLightData);
				DrawingDevice::AddLight(light->direct3dLight);
			}
			else
			{
				Free(light);
				light = 0;
			}
		}

		return light;
	}

	// FUNCTION: TOY2 0x004C2A20 [MATCHED]
	Light* Light::BuildAmbient(BGRA_F* color, int32_t active)
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

	// FUNCTION: TOY2 0x004CE910 [MATCHED]
	void Light::BuildGlobalLights()
	{
		BGRA_F color;
		D3DMATRIX matrix;

		InitPool(8);

		// ambient light, starts off black
		color.a = 1.0;
		color.b = 0.0;
		color.g = 0.0;
		color.r = 0.0;

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
		color.b = 0.0;
		color.g = 0.0;
		color.r = 0.0;

		g_globalLight5 = BuildLight(&matrix, &color, 1);
	}

	// FUNCTION: TOY2 0x004C29A0 [MATCHED]
	void Light::Destroy(Light* light)
	{
		if (light)
		{
			if (light->direct3dLight)
			{
				DrawingDevice::DeleteLight(light->direct3dLight);
				DrawingDevice::ReleaseLight(light->direct3dLight);
			}

			Free(light);
		}
	}

	// FUNCTION: TOY2 0x004C3090 [MATCHED]
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

	// FUNCTION: TOY2 0x004CEA50 [MATCHED]
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

	// FUNCTION: TOY2 0x004C2E70 [MATCHED]
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
			light->d3dLightData.dvTheta = light->innerConeAngle * 0.000095873802f;
			light->d3dLightData.dvRange = light->range;
			light->d3dLightData.dwFlags = light->active & 1;
			light->d3dLightData.dvPhi = light->outerConeAngle * 0.000095873802f;

			DrawingDevice::SetLight(light->direct3dLight, &light->d3dLightData);
		}
		else if ((light->active & 1) != 0)
		{
			RGBA ambient;

			int32_t channel = light->color.a * 255.0f;

			if (channel > 0xFF || channel >= 0)
			{
				if (0xFF < channel)
					ambient.a = 0xFF;
				else
					ambient.a = channel;
			}
			else
			{
				ambient.a = 0;
			}

			channel = light->color.r * 255.0f;

			if (channel > 0xFF || channel >= 0)
			{
				if (0xFF < channel)
					ambient.r = 0xFF;
				else
					ambient.r = channel;
			}
			else
			{
				ambient.r = 0;
			}

			channel = light->color.g * 255.0f;

			if (channel > 0xFF || channel >= 0)
			{
				if (0xFF < channel)
					ambient.g = 0xFF;
				else
					ambient.g = channel;
			}
			else
			{
				ambient.g = 0;
			}

			channel = light->color.b * 255.0f;

			if (channel > 0xFF || channel >= 0)
			{
				if (0xFF < channel)
					ambient.b = 0xFF;
				else
					ambient.b = channel;
			}
			else
			{
				ambient.b = 0;
			}

			DrawingDevice::SetLightState(D3DLIGHTSTATE_AMBIENT, ambient.value);
		}
	}

	// FUNCTION: TOY2 0x004CE6C0 [MATCHED]
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

		float colorScale = 0.0078125;

		g_globalLight5->color.r = colorR * colorScale;
		g_globalLight5->color.g = colorG * colorScale;
		g_globalLight5->color.b = colorB * colorScale;

		Update(g_globalLight5);
	}
}

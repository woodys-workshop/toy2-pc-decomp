#include "Toy2/Lighting.h"
#include "Toy2/Toy2.h"
#include "Renderer/Renderer.h"

#include <string.h>

namespace Toy2
{
	namespace Lighting
	{
		// GLOBAL: TOY2 0x00830D60
		DynamicLight g_dynamicLights[8];

		// GLOBAL: TOY2 0x00830D54
		int32_t g_lightLerpProgress;

		// GLOBAL: TOY2 0x00830D3C
		int32_t g_activeLightIdx;

		// GLOBAL: TOY2 0x0050387C
		LevelLightDef g_levelLightDefs[16] = {
			{ { 0x40001000, 0x20008000, 0x20004000 }, 0x20008000, 0x80004000 },
			{ { 0x400, -0x400, 0x400 }, 0x960, 0x908060 },
			{ { 0x400, -0x400, 0x400 }, 0x960, 0xA02000 },
			{ { 0x0, -0x400, 0x0 }, 0x960, 0xC0C000 },
			{ { 0x200, -0x400, 0x200 }, 0x960, 0x204080 },
			{ { 0x200, -0x400, 0x200 }, 0x960, 0x204060 },
			{ { 0x400, -0x400, 0x400 }, 0x960, 0xA02000 },
			{ { 0x400, -0x400, 0x400 }, 0x960, 0x608090 },
			{ { 0x400, -0x400, 0x400 }, 0x960, 0x608090 },
			{ { 0x400, -0x400, 0x400 }, 0x960, 0x608090 },
			{ { 0x400, -0x400, 0x400 }, 0x960, 0x204080 },
			{ { 0x0, -0x400, -0x400 }, 0x960, 0x204080 },
			{ { 0x400, -0x400, 0x400 }, 0x960, 0x204080 },
			{ { 0x400, -0x400, 0x400 }, 0x960, 0x908060 },
			{ { 0x200, -0x400, 0x200 }, 0x960, 0x204080 },
			{ { 0x400, -0x400, 0x400 }, 0x960, 0x406080 },
		};

		// STUB: TOY2 0x0049EE50
		void SpawnLight(int32_t x, int32_t y, int32_t z, int32_t color, int32_t lifetime, int32_t groupId) {}

		// FUNCTION: TOY2 0x0049EEE0
		void UpdateBuzzLight()
		{
			int32_t buzzLightY = g_buzzActor.posAngles.pos.y - 0x2000;

			g_dynamicLights[0].pos.x = g_buzzActor.posAngles.pos.x + 16 * g_levelLightDefs[g_levelFileIndex].offset.x;
			g_dynamicLights[0].pos.z = g_buzzActor.posAngles.pos.z + 16 * g_levelLightDefs[g_levelFileIndex].offset.z;
			g_dynamicLights[0].pos.y = buzzLightY + 16 * g_levelLightDefs[g_levelFileIndex].offset.y;

			int32_t closestDistance;
			int32_t closestIdx;

			// light 1 is the persistent one, the others are timed lights, find the one closest to Buzz
			if (g_dynamicLights[1].lifetime)
			{
				closestIdx = 1;
				closestDistance = ((g_dynamicLights[1].pos.x - g_buzzActor.posAngles.pos.x) >> 8) * ((g_dynamicLights[1].pos.x - g_buzzActor.posAngles.pos.x) >> 8) +
								  ((g_dynamicLights[1].pos.y - buzzLightY) >> 8) * ((g_dynamicLights[1].pos.y - buzzLightY) >> 8) +
								  ((g_dynamicLights[1].pos.z - g_buzzActor.posAngles.pos.z) >> 8) * ((g_dynamicLights[1].pos.z - g_buzzActor.posAngles.pos.z) >> 8);
			}
			else
			{
				closestIdx = 0;
				closestDistance = 0x7FFFFFFF;
			}

			int32_t lightIdx = 2;

			for (DynamicLight* light = &g_dynamicLights[2]; light < &g_dynamicLights[6]; light++, lightIdx++)
			{
				if (light->lifetime > 0)
				{
					light->lifetime -= Renderer::g_frameDelta;

					if (light->lifetime > 0)
					{
						int32_t distanceYZ = ((light->pos.y - buzzLightY) >> 8) * ((light->pos.y - buzzLightY) >> 8) +
											 ((light->pos.z - g_buzzActor.posAngles.pos.z) >> 8) * ((light->pos.z - g_buzzActor.posAngles.pos.z) >> 8);
						int32_t distanceX = ((light->pos.x - g_buzzActor.posAngles.pos.x) >> 8) * ((light->pos.x - g_buzzActor.posAngles.pos.x) >> 8);

						if (distanceX + distanceYZ <= 0x8000)
						{
							if (distanceX + distanceYZ < closestDistance)
							{
								closestDistance = distanceX + distanceYZ;
								closestIdx = lightIdx;
							}
						}
						else
						{
							light->lifetime = 0;
						}
					}
					else
					{
						light->lifetime = 0;
					}
				}
			}

			int32_t activeIdx = g_activeLightIdx;
			int32_t progress;
			int32_t x;
			int32_t y;
			int32_t z;
			int32_t colorR;
			int32_t colorG;
			int32_t colorB;

			if (g_dynamicLights[g_activeLightIdx].groupId == g_dynamicLights[closestIdx].groupId)
			{
				progress = g_lightLerpProgress;

				if (g_lightLerpProgress)
				{
					z = g_dynamicLights[6].pos.z;
					y = g_dynamicLights[6].pos.y;
					goto LABEL_20;
				}

				goto LABEL_17;
			}

			// a different light took over, blend towards it using slot 6 as the target
			g_dynamicLights[6].pos.x = g_dynamicLights[g_activeLightIdx].pos.x + g_lightLerpProgress * (g_dynamicLights[6].pos.x - g_dynamicLights[g_activeLightIdx].pos.x) / 64;

			y = g_dynamicLights[g_activeLightIdx].pos.y + g_lightLerpProgress * (g_dynamicLights[6].pos.y - g_dynamicLights[g_activeLightIdx].pos.y) / 64;
			g_dynamicLights[6].pos.y = y;

			z = g_dynamicLights[g_activeLightIdx].pos.z + g_lightLerpProgress * (g_dynamicLights[6].pos.z - g_dynamicLights[g_activeLightIdx].pos.z) / 64;
			g_dynamicLights[6].pos.z = z;

			g_dynamicLights[6].colorR = g_dynamicLights[g_activeLightIdx].colorR + g_lightLerpProgress * (g_dynamicLights[6].colorR - g_dynamicLights[g_activeLightIdx].colorR) / 64;
			g_dynamicLights[6].colorG = g_dynamicLights[g_activeLightIdx].colorG + g_lightLerpProgress * (g_dynamicLights[6].colorG - g_dynamicLights[g_activeLightIdx].colorG) / 64;
			g_dynamicLights[6].colorB = g_dynamicLights[g_activeLightIdx].colorB + g_lightLerpProgress * (g_dynamicLights[6].colorB - g_dynamicLights[g_activeLightIdx].colorB) / 64;

			activeIdx = closestIdx;
			g_activeLightIdx = closestIdx;

			if (closestIdx > 1)
			{
				g_lightLerpProgress = 0;
				goto LABEL_17;
			}

			progress = 64;

		LABEL_20:
			x = g_dynamicLights[activeIdx].pos.x + progress * (g_dynamicLights[6].pos.x - g_dynamicLights[activeIdx].pos.x) / 64;
			y = g_dynamicLights[activeIdx].pos.y + progress * (y - g_dynamicLights[activeIdx].pos.y) / 64;
			z = g_dynamicLights[activeIdx].pos.z + progress * (z - g_dynamicLights[activeIdx].pos.z) / 64;

			colorR = g_dynamicLights[activeIdx].colorR + progress * (g_dynamicLights[6].colorR - g_dynamicLights[activeIdx].colorR) / 64;
			colorG = g_dynamicLights[activeIdx].colorG + progress * (g_dynamicLights[6].colorG - g_dynamicLights[activeIdx].colorG) / 64;
			colorB = g_dynamicLights[activeIdx].colorB + progress * (g_dynamicLights[6].colorB - g_dynamicLights[activeIdx].colorB) / 64;

			g_lightLerpProgress = progress - Renderer::g_frameDelta;

			if (progress - (int32_t)Renderer::g_frameDelta <= 0)
				g_lightLerpProgress = 0;

			goto LABEL_23;

		LABEL_17:
			x = g_dynamicLights[activeIdx].pos.x;
			y = g_dynamicLights[activeIdx].pos.y;
			z = g_dynamicLights[activeIdx].pos.z;

			colorR = g_dynamicLights[activeIdx].colorR;
			colorG = g_dynamicLights[activeIdx].colorG;
			colorB = g_dynamicLights[activeIdx].colorB;

		LABEL_23:
			int32_t dirY = (y - buzzLightY) >> 8;
			int32_t dirZ = (z - g_buzzActor.posAngles.pos.z) >> 8;
			int32_t dirX = (x - g_buzzActor.posAngles.pos.x) >> 8;

			int32_t distanceSquared = dirX * dirX + dirY * dirY + dirZ * dirZ;

			if (distanceSquared > 0x10000)
				distanceSquared = 0x10000;

			g_buzzActor.lightDirection.x = dirX;
			g_buzzActor.lightDirection.y = dirY;
			g_buzzActor.lightDirection.z = dirZ;

			g_buzzActor.color.value = ((colorR * (0x10000 - distanceSquared)) >> 16) +
									  ((((colorG * (0x10000 - distanceSquared)) >> 16) + (((colorB * (0x10000 - distanceSquared)) >> 16) << 8)) << 8);
		}

		// FUNCTION: TOY2 0x0049F350
		void InitBuzzLight()
		{
			memset(g_dynamicLights, 0, sizeof(g_dynamicLights));

			g_lightLerpProgress = 0;
			g_activeLightIdx = 0;

			g_dynamicLights[0].groupId = 0;

			g_buzzActor.unkVar51 = 2400;

			uint32_t packedColor = g_levelLightDefs[g_levelFileIndex].packedColor;

			g_dynamicLights[0].colorB = (uint8_t)packedColor;
			g_dynamicLights[0].colorR = (uint8_t)(packedColor >> 16);
			g_dynamicLights[0].colorG = (uint8_t)(packedColor >> 8);

			UpdateBuzzLight();
		}
	}
}

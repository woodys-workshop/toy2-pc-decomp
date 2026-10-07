#include "Toy2/Shadow.h"
#include "Toy2/Toy2.h"
#include "Toy2/Collision.h"
#include "Renderer/Shadows.h"

namespace Toy2
{
	namespace Shadow
	{
		// FUNCTION: TOY2 0x00447D30
		void ResetShadowCount()
		{
			Renderer::Shadows::g_shadowCount = 0;
			Renderer::Shadows::g_unusedShadowVar = 0;
		}

		// FUNCTION: TOY2 0x00485680
		void QueueStretched(int32_t x, int32_t baseY, int32_t z, int32_t size, int32_t lightY)
		{
			int32_t heightBias = (lightY - baseY) / 1024 + 64;

			if (heightBias >= 0)
			{
				if (heightBias > 32)
					heightBias = 32;

				int16_t slopeY = Collision::g_floorSlope.y;

				if (slopeY < -4096 && Renderer::Shadows::g_shadowCount < 48)
				{
					int32_t idx = Renderer::Shadows::g_shadowCount;
					int32_t stretch;

					Renderer::Shadows::g_shadowRecords[idx].heightBias = (int16_t)heightBias;
					Renderer::Shadows::g_shadowRecords[idx].worldX = x;
					Renderer::Shadows::g_shadowRecords[idx].worldZ = z;

					if (slopeY <= -12288)
						stretch = size;
					else
						stretch = size * (-2048 - slopeY) / 10240;

					Renderer::Shadows::g_shadowRecords[idx].size = (int16_t)-stretch;
					int32_t offset = stretch * (Collision::g_floorSlope.z + Collision::g_floorSlope.x) / slopeY;
					Renderer::Shadows::g_shadowRecords[idx].groundY = baseY + 32 * offset;

					int32_t slopeZ = Collision::g_floorSlope.z;
					Renderer::Shadows::g_shadowStretch[idx].skewX0 = (int16_t)(stretch * (slopeZ - Collision::g_floorSlope.x) / slopeY - offset);
					Renderer::Shadows::g_shadowStretch[idx].skewX1 = (int16_t)(stretch * (Collision::g_floorSlope.x - slopeZ) / slopeY - offset);
					Renderer::Shadows::g_shadowStretch[idx].skewZ = (int16_t)(-2 * offset);

					if (Collision::g_floorSlope.y < -8192)
					{
						++Renderer::Shadows::g_shadowCount;
						Renderer::Shadows::g_shadowStretch[idx].alpha = 48;
					}
					else
					{
						++Renderer::Shadows::g_shadowCount;
						Renderer::Shadows::g_shadowStretch[idx].alpha = (int16_t)((3 * (-4096 - slopeY)) >> 8);
					}
				}
			}
		}

		// FUNCTION: TOY2 0x004857E0
		void QueueStretchedForBuzz(int32_t x, int32_t baseY, int32_t z, int32_t size)
		{
			int32_t heightBias = (g_buzzActor.posAngles.pos.y - baseY) / 1024 + 64;

			if (heightBias >= 0)
			{
				if (heightBias > 32)
					heightBias = 32;

				int16_t slopeY = Collision::g_floorSlopePrev.y;

				if (slopeY < -4096 && Renderer::Shadows::g_shadowCount < 48)
				{
					int32_t idx = Renderer::Shadows::g_shadowCount;
					int32_t stretch;

					Renderer::Shadows::g_shadowRecords[idx].heightBias = (int16_t)heightBias;
					Renderer::Shadows::g_shadowRecords[idx].worldX = x;
					Renderer::Shadows::g_shadowRecords[idx].worldZ = z;

					if (slopeY <= -12288)
						stretch = size;
					else
						stretch = size * (-2048 - slopeY) / 10240;

					int32_t slopeZ = Collision::g_floorSlopePrev.z;
					Renderer::Shadows::g_shadowRecords[idx].size = (int16_t)-stretch;

					int32_t offset = stretch * (Collision::g_floorSlopePrev.x + slopeZ) / slopeY;
					Renderer::Shadows::g_shadowRecords[idx].groundY = baseY + 32 * offset;

					Renderer::Shadows::g_shadowStretch[idx].skewX0 = (int16_t)(stretch * (slopeZ - Collision::g_floorSlopePrev.x) / slopeY - offset);
					Renderer::Shadows::g_shadowStretch[idx].skewX1 = (int16_t)(stretch * (Collision::g_floorSlopePrev.x - slopeZ) / slopeY - offset);
					Renderer::Shadows::g_shadowStretch[idx].skewZ = (int16_t)(-2 * offset);

					if (Collision::g_floorSlopePrev.y < -8192)
					{
						++Renderer::Shadows::g_shadowCount;
						Renderer::Shadows::g_shadowStretch[idx].alpha = 48;
					}
					else
					{
						++Renderer::Shadows::g_shadowCount;
						Renderer::Shadows::g_shadowStretch[idx].alpha = (int16_t)((3 * (-4096 - slopeY)) >> 8);
					}
				}
			}
		}
	}
}

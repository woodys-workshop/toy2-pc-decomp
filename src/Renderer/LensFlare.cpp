#include "Renderer/LensFlare.h"
#include "Renderer/Renderer.h"
#include "Renderer/Sprite.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Collision.h"
#include "Toy2/Camera.h"
#include "DrawingDevice.h"

namespace Renderer
{
	namespace LensFlare
	{
		struct FlareElement
		{
			int16_t sheetIndex;
			int16_t offsetX;
			int16_t offsetY;
			int16_t redScale;
			int16_t greenScale;
			int16_t blueScale;
		};

		struct LensFlareCandidate
		{
			Vector3I worldPos;
			int32_t screenX;
			int32_t screenY;
			int32_t depth;
			RGB32 color;
			int32_t baseSize;
		};

		struct LensFlareSlot
		{
			Vector3I worldPos;
			int32_t screenX8;
			int32_t screenY8;
			int32_t dxFromCenter;
			int32_t dyFromCenter;
			RGB32 color;
			int32_t baseSize;
		};

		STATIC_ASSERT(sizeof(FlareElement) == 0xC);
		STATIC_ASSERT(sizeof(LensFlareCandidate) == 0x28);
		STATIC_ASSERT(sizeof(LensFlareSlot) == 0x2C);

		// GLOBAL: TOY2 0x004F72D8
		FlareElement g_flareElements[17] = {
			{ 5, 72, 48, 96, 96, 96 }, { 0, 0, 0, 0, 0, 0 },	  { 0, 0, 0, 0, 0, 0 },		 { 0, 0, 0, 0, 0, 0 },		{ 0, 0, 0, 0, 0, 0 },
			{ 7, 42, 28, 4, 4, 56 },   { 4, 36, 24, 24, 24, 72 }, { 4, 24, 16, 64, 24, 24 }, { 0, 0, 0, 0, 0, 0 },		{ 4, 6, 4, 64, 64, 64 },
			{ 4, 12, 8, 64, 64, 64 },  { 4, 36, 24, 64, 64, 24 }, { 7, 66, 44, 48, 16, 16 }, { 4, 36, 24, 32, 32, 72 }, { 0, 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0, 0 },	   { 7, 96, 64, 12, 32, 12 },
		};

		// GLOBAL: TOY2 0x00554DC8
		LensFlareCandidate g_lensFlareCandidates[8];

		// GLOBAL: TOY2 0x005543E0
		LensFlareSlot g_lensFlareSlots[16];

		// GLOBAL: TOY2 0x0054E050
		int16_t g_flareActive[2];

		// GLOBAL: TOY2 0x0054DD50
		int32_t g_flareSlotCounts[2];

		// GLOBAL: TOY2 0x00557710
		int32_t g_activeLensFlareIdx;

		// GLOBAL: TOY2 0x00559C64
		int32_t g_lensFlareCount;

		// FUNCTION: TOY2 0x0044F200
		void RegisterLight(int32_t worldX, int32_t worldY, int32_t worldZ, int32_t colorR, int32_t colorG, int32_t colorB, int32_t baseSize)
		{
			Vector3F outPoints;
			Vector3F inPoints;
			WorldPos posDiff;
			WorldPos camPos;

			if (g_lensFlareCount < 8)
			{
				camPos.x = Toy2::Camera::g_activeCameraTransform.pos.x;
				posDiff.x = 9 * (worldX - Toy2::Camera::g_activeCameraTransform.pos.x) / 10;
				camPos.y = Toy2::Camera::g_activeCameraTransform.pos.y;
				posDiff.y = 9 * (worldY - Toy2::Camera::g_activeCameraTransform.pos.y) / 10;
				posDiff.z = 9 * (worldZ - Toy2::Camera::g_activeCameraTransform.pos.z) / 10;
				camPos.z = Toy2::Camera::g_activeCameraTransform.pos.z;

				if (! Nu3D::Collision::SweepAndSlide(&camPos, &posDiff, 0x8000, 0, 1))
				{
					inPoints.x = worldX * 0.03125;
					inPoints.y = worldY * 0.03125;
					inPoints.z = worldZ * 0.03125;

					Nu3D::Camera::TransformPointProjective(&outPoints, &inPoints, 1, 0);

					float widthDiv = outPoints.x * g_virtualScreenWidth;
					int32_t screenX = (int32_t)(widthDiv / DrawingDevice::GetDestWidth());

					float heightDiv = outPoints.y * g_virtualScreenHeight;
					int32_t screenY = (int32_t)(heightDiv / DrawingDevice::GetDestHeight());

					int32_t depth =
						(int32_t)((Nu3D::Camera::g_primaryCamera->farZ - Nu3D::Camera::g_primaryCamera->nearZ) * outPoints.z
								  + Nu3D::Camera::g_primaryCamera->nearZ);

					if (screenX <= 0x200 && screenX >= 0 && screenY <= 0x100 && screenY >= 0 && depth > 128 && outPoints.z > 0.0 && outPoints.z < 1.0)
					{
						int32_t count = g_lensFlareCount + 1;

						g_lensFlareCandidates[g_lensFlareCount].worldPos.x = worldX;
						g_lensFlareCandidates[g_lensFlareCount].worldPos.y = worldY;
						g_lensFlareCandidates[g_lensFlareCount].worldPos.z = worldZ;
						g_lensFlareCandidates[g_lensFlareCount].screenX = screenX;
						g_lensFlareCandidates[g_lensFlareCount].screenY = screenY;
						g_lensFlareCandidates[g_lensFlareCount].depth = depth;
						g_lensFlareCandidates[g_lensFlareCount].color.r = colorR;
						g_lensFlareCandidates[g_lensFlareCount].color.g = colorG;
						g_lensFlareCandidates[g_lensFlareCount].color.b = colorB;
						g_lensFlareCandidates[g_lensFlareCount].baseSize = baseSize;

						g_lensFlareCount = count;
					}
				}
			}
		}

		// FUNCTION: TOY2 0x0044F420
		void CullAndQueue()
		{
			int32_t screenX;
			int32_t screenY;

			if (g_lensFlareCount > 0)
			{
				int32_t active = g_activeLensFlareIdx;
				LensFlareCandidate* candidate = &g_lensFlareCandidates[0];
				int32_t remaining = g_lensFlareCount;

				do
				{
					screenX = candidate->screenX;
					screenY = candidate->screenY;

					if (screenX <= 0x200 && screenX >= 0 && screenY <= 0x100 && screenY >= 0 && candidate->depth > 128)
					{
						int32_t slotX = 8 * screenX;
						int32_t slotY = 8 * screenY;
						int32_t dx = 256 - screenX;
						int32_t dy = 128 - screenY;

						int32_t distSq = dx * dx + dy * dy;

						if (distSq > 0xFFFF)
							distSq = 0xFFFF;

						int32_t counts = g_flareSlotCounts[active];

						if (counts < 8 && 66559 - distSq > 0 && candidate->depth - 96 > 256)
						{
							g_flareActive[active] = 1;
							int32_t slotIdx = counts + 8 * active;

							g_lensFlareSlots[slotIdx].worldPos.x = candidate->worldPos.x;
							g_lensFlareSlots[slotIdx].worldPos.y = candidate->worldPos.y;
							g_lensFlareSlots[slotIdx].worldPos.z = candidate->worldPos.z;
							g_lensFlareSlots[slotIdx].screenX8 = slotX;
							g_lensFlareSlots[slotIdx].screenY8 = slotY;
							g_lensFlareSlots[slotIdx].dxFromCenter = dx;

							g_lensFlareSlots[slotIdx].dyFromCenter = dy;
							g_lensFlareSlots[slotIdx].color.r = candidate->color.r;
							g_lensFlareSlots[slotIdx].color.g = candidate->color.g;
							g_lensFlareSlots[slotIdx].color.b = candidate->color.b;
							g_lensFlareSlots[slotIdx].baseSize = candidate->baseSize;

							g_flareSlotCounts[active] = counts + 1;
						}
					}

					++candidate;
					--remaining;
				} while (remaining);
			}
		}

		// FUNCTION: TOY2 0x0044F580
		void RenderSlot(int32_t slotIndex)
		{
			bool lastFlare;
			Vector3F inPoints;
			Vector3F outPoints;

			int32_t slotIdx = slotIndex + 8 * g_activeLensFlareIdx;

			inPoints.x = g_lensFlareSlots[slotIdx].worldPos.x * 0.03125;
			inPoints.y = g_lensFlareSlots[slotIdx].worldPos.y * 0.03125;
			inPoints.z = g_lensFlareSlots[slotIdx].worldPos.z * 0.03125;

			Nu3D::Camera::TransformPointProjective(&outPoints, &inPoints, 1, 0);

			float widthDiv = g_virtualScreenWidth * outPoints.x;
			int32_t screenX = (int32_t)(widthDiv / DrawingDevice::GetDestWidth());

			float heightDiv = g_virtualScreenHeight * outPoints.y;
			int32_t screenY = (int32_t)(heightDiv / DrawingDevice::GetDestHeight());

			int32_t depth = (int32_t)(outPoints.z * 47950.0 + 50.0);
			int32_t dx = 256 - screenX;
			int32_t dy = 128 - screenY;
			int32_t x8 = 8 * screenX;
			int32_t y8 = 8 * screenY;
			int32_t distSq = dx * dx + dy * dy;
			int32_t dyCopy = 128 - screenY;

			if (distSq > 0xFFFF)
				distSq = 0xFFFF;

			int32_t intensity = 66559 - distSq;
			int32_t maxFlares = 17;
			slotIdx = slotIndex + 8 * g_activeLensFlareIdx;
			int32_t size = g_lensFlareSlots[slotIdx].baseSize + intensity / (depth / 2 + 1);

			int32_t red = intensity * g_lensFlareSlots[slotIdx].color.r;
			int32_t blue = intensity * g_lensFlareSlots[slotIdx].color.b;
			int32_t green = intensity * g_lensFlareSlots[slotIdx].color.g;

			int32_t blueCopy = blue;

			FlareElement* flare = &g_flareElements[0];

			do
			{
				if (flare->sheetIndex)
				{
					int32_t blueScaled = blue * flare->blueScale;

					Renderer::Sprite::DrawScaled(
						(x8 >> 3) - size * flare->offsetX / 256,
						(y8 >> 3) - size * flare->offsetY / 256,
						flare->sheetIndex,
						0,
						(red * flare->redScale) >> 23,
						(green * flare->greenScale) >> 23,
						blueScaled >> 23,
						32,
						size * flare->offsetX,
						size * flare->offsetY);

					dy = dyCopy;
					blue = blueCopy;
				}

				y8 += dy;

				++flare;

				lastFlare = maxFlares == 1;
				x8 += dx;

				--maxFlares;
			} while (! lastFlare);
		}
	}
}

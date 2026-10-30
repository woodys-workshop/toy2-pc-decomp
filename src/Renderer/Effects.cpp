#include "Renderer/Effects.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderType.h"
#include "Renderer/Sprite.h"
#include "Renderer/SpriteSheets.h"
#include "Renderer/LensFlare.h"
#include "NGNLoader/NGNLoader.h"
#include "Nu3D/Math.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Particles.h"
#include "Toy2/Toy2.h"
#include "Toy2/Weather.h"

namespace Renderer
{
	namespace Beam
	{
		// GLOBAL: TOY2 0x004F72D4
		int32_t g_beamRequestCount = 100;

		// GLOBAL: TOY2 0x0054F640
		BeamRequest g_beamRequests[100];

		// FUNCTION: TOY2 0x0044E1A0
		void DrawSegmentedBeam(
			int32_t spriteSheet,
			int32_t halfWidth,
			int32_t segmentLength,
			Transform* origin,
			Transform* direction,
			uint8_t tintB,
			uint8_t tintG,
			uint8_t tintR)
		{
			RGBA color;
			Vector3F vec5;
			Vector3F vec4;
			Vector3F vec3;
			Vector3F vec1;
			Vector2F bottomRightUV;
			Vector2F topLeftUV;
			Vector3F drawVerts[4];
			Vector3F vec2;

			SpriteSheet* sheet = g_spriteSheets[spriteSheet];

			uint32_t bitmapWidth = 0xFF;
			uint32_t bitmapHeight = 0xFF;

			int32_t texData = NGNLoader::GetTextureDataIndex(sheet->texIndex);

			if (texData != 0)
				NGNLoader::RetrieveTextureData(texData, &bitmapWidth, &bitmapHeight, 0, 0, 0);

			// NOTE: the IDB's RGBA has the red channel in the first byte, ours has the blue channel there. The first byte gets tintR
			color.b = tintR;
			color.g = tintG;
			color.r = tintB;
			color.a = 0xFF;

			Nu3D::Math::GetPositionVector(&Nu3D::Camera::g_secondaryCamera.matrix, &vec2);

			vec1.x = origin->pos.x * 0.03125;
			vec1.y = origin->pos.y * 0.03125;
			vec1.z = origin->pos.z * 0.03125;

			vec3.x = direction->pos.x * 0.03125;
			vec3.y = direction->pos.y * 0.03125;
			vec3.z = direction->pos.z * 0.03125;

			Nu3D::Math::VertexSubtract(&vec2, &vec2, &vec1);
			Nu3D::Math::VertexCrossProduct(&vec4, &vec2, &vec3);
			Nu3D::Math::VectorNormalize(&vec4, &vec4);

			float scale = (float)halfWidth;
			Nu3D::Math::ScaleVector(&vec4, &vec4, scale);
			// the length is truncated to a whole number first (the original calls __ftol here)
			float length = (float)(int32_t)Nu3D::Math::GetLength(&vec3);
			float newLength = 1.0 / length;

			Nu3D::Math::ScaleVector(&vec3, &vec3, newLength);
			Nu3D::Math::ScaleVector(&vec5, &vec3, length);
			Nu3D::Math::VertexAdd(&vec5, &vec5, &vec1);
			Nu3D::Math::VertexSubtract(&drawVerts[2], &vec5, &vec4);
			Nu3D::Math::VertexAdd(&drawVerts[3], &vec5, &vec4);

			RGBA colorCopy = color;
			int32_t tileIndex = 2;
			float segLength = (float)segmentLength;

			do
			{
				double lengthDiff = length - segLength;

				if (lengthDiff >= 0.0)
				{
					length = lengthDiff;
					Nu3D::Math::ScaleVector(&vec5, &vec3, length);
				}
				else
				{
					length = 0.0;
					Nu3D::Math::ScaleVector(&vec5, &vec3, 0.0);
				}

				Nu3D::Math::VertexAdd(&vec5, &vec5, &vec1);
				Nu3D::Math::VertexSubtract(drawVerts, &vec5, &vec4);
				Nu3D::Math::VertexAdd(&drawVerts[1], &vec5, &vec4);

				double width = bitmapWidth;
				topLeftUV.x = sheet->tiles[tileIndex].x / width;

				float height = (float)bitmapHeight;
				topLeftUV.y = sheet->tiles[tileIndex].y / height;

				bottomRightUV.x = (sheet->tileWidth + sheet->tiles[tileIndex].x) / width;
				bottomRightUV.y = (sheet->tiles[tileIndex].y + sheet->tileHeight) / height;

				Renderer::Sprite::QueueQuadSpriteFromVerts(
					drawVerts, &topLeftUV, &bottomRightUV, texData, colorCopy, RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_CUSTOM);

				drawVerts[3] = drawVerts[1];
				drawVerts[2] = drawVerts[0];

				tileIndex = 1;
			} while (length > 0.0);
		}

		// FUNCTION: TOY2 0x0044E100
		void QueueBeam(
			int32_t spriteSheet,
			int32_t halfWidth,
			int32_t segementLength,
			Transform* origin,
			Transform* direction,
			int32_t tintB,
			int32_t tintG,
			int32_t tintR)
		{
			int32_t count = g_beamRequestCount;

			if (g_beamRequestCount)
			{
				--g_beamRequestCount;

				int32_t index = count - 1;

				g_beamRequests[index].spriteSheetId = spriteSheet;
				g_beamRequests[index].halfWidth = halfWidth;
				g_beamRequests[index].segmentLength = segementLength;
				g_beamRequests[index].origin = *origin;
				g_beamRequests[index].direction = *direction;
				g_beamRequests[index].tintB = tintB;
				g_beamRequests[index].tintG = tintG;
				g_beamRequests[index].tintR = tintR;
			}
		}
	}

	namespace Particles
	{
		// FUNCTION: TOY2 0x00445980
		void DrawParticles()
		{
			// quirk: this is a leftover of the stack, it is only used when no render flag matches below
			int32_t renderFlags;
			int32_t tileIndex;
			int32_t x;
			int32_t y;
			RGBA color;
			Vector2F uvBottomRight;
			Vector2F uvTopLeft;
			Vector3F position;

			uint32_t bitmapWidth = 255;
			uint32_t bitmapHeight = 255;
			Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::g_particleInstances;
			int32_t remaining = 64;

			do
			{
				if (particle->lifetime && particle->spriteSheet)
				{
					particle->renderFlags &= ~0x200;
					SpriteSheet* sheet = g_spriteSheets[particle->spriteSheet];

					int32_t texDataIndex = NGNLoader::GetTextureDataIndex(sheet->texIndex);

					if (texDataIndex)
						NGNLoader::RetrieveTextureData(texDataIndex, &bitmapWidth, &bitmapHeight, 0, 0, 0);

					tileIndex = particle->tileIndex;
					color.a = -1;
					uvTopLeft.x = sheet->tiles[tileIndex].x / (double)bitmapWidth;

					float bitmapHeightF = (float)bitmapHeight;
					uvTopLeft.y = sheet->tiles[tileIndex].y / bitmapHeightF;
					x = sheet->tiles[tileIndex].x;
					uvBottomRight.x = (sheet->tileWidth + x) / (double)bitmapWidth;
					y = sheet->tiles[tileIndex].y;

					// NOTE: the IDB's RGBA has the red channel in the first byte, ours has the blue channel there
					color.b = particle->colourB;
					color.r = particle->colourR;
					color.g = particle->colourG;

					int16_t particleRenderFlags = particle->renderFlags;
					int32_t rotation = 16 * particle->groundAlignRot;

					uvBottomRight.y = (sheet->tileHeight + y) / bitmapHeightF;
					position.x = particle->pos.x * 0.03125;
					position.y = particle->pos.y * 0.03125;
					position.z = particle->pos.z * 0.03125;

					switch (particleRenderFlags & 0x60)
					{
						case 0:
							color.a = 64;
							renderFlags = RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT;

							break;

						case 0x20:
							color.a = -1;
							renderFlags = RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_CUSTOM;

							break;

						case 0x40:
							color.a = -1;
							renderFlags = RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_ALT;

							break;

						case 0x60:
							color.a = -1;
							renderFlags = RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT;

							break;

						default:
							break;
					}

					if ((particleRenderFlags & RENDER_CULL_FRONT) != 0)
					{
						position.y = position.y - 10.0;
						particle->renderFlags = particleRenderFlags + 512;

						Renderer::Sprite::QueueGroundAlignedSprite(
							&position,
							-rotation,
							(float)particle->width,
							(float)particle->height,
							&uvTopLeft,
							&uvBottomRight,
							texDataIndex,
							color,
							renderFlags);
					}
					else
					{
						particle->renderFlags = particleRenderFlags + 512;

						Renderer::Sprite::QueueQuadSprite(
							&position, rotation, (float)particle->width, (float)particle->height, &uvTopLeft, &uvBottomRight, texDataIndex, color, renderFlags);
					}
				}

				++particle;
				--remaining;
			} while (remaining);
		}
	}

	// FUNCTION: TOY2 0x0044F010
	void DrawFallingParticles()
	{
		Vector2F uvBottomRight;
		Vector2F uvTopLeft;
		Vector3F position;
		RGBA color;

		uint32_t bitmapWidth = 255;
		uint32_t bitmapHeight = 255;

		if (Toy2::Weather::g_weatherBasePointer)
		{
			SpriteSheet* sheet = g_spriteSheets[Toy2::Weather::g_weatherSpriteSheet];

			if (sheet)
			{
				int32_t texDataIndex = NGNLoader::GetTextureDataIndex(sheet->texIndex);

				if (texDataIndex)
					NGNLoader::RetrieveTextureData(texDataIndex, &bitmapWidth, &bitmapHeight, 0, 0, 0);

				uvTopLeft.x = sheet->tiles[0].x / (double)bitmapWidth;
				float bitmapHeightF = (float)bitmapHeight;
				uvTopLeft.y = sheet->tiles[0].y / bitmapHeightF;
				uvBottomRight.x = (sheet->tileWidth + sheet->tiles[0].x) / (double)bitmapWidth;
				int32_t base = Toy2::Weather::g_weatherBasePointer;
				int32_t offset = 0;

				uvBottomRight.y = (sheet->tileHeight + sheet->tiles[0].y) / bitmapHeightF;

				color.value = 0xFF404040;

				do
				{
					if (*(int32_t*)(offset + base + 12))
					{
						position.x = *(int32_t*)(offset + base) >> 5;
						position.y = *(int32_t*)(offset + base + 4) >> 5;
						position.z = *(int32_t*)(offset + base + 8) >> 5;

						Renderer::Sprite::QueueBillboardSprite(
							&position, 0, 30.0, 200.0, &uvTopLeft, &uvBottomRight, texDataIndex, color, RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_CUSTOM);
						base = Toy2::Weather::g_weatherBasePointer;
					}

					offset += 16;
				} while (offset < 1024);
			}
		}
	}

	// FUNCTION: TOY2 0x0044F190 [MATCHED]
	void DrawLensFlares()
	{
		// lens flares are enabled in the graphics options
		if ((Toy2::g_toyCfgData.flags & 1) != 0)
		{
			LensFlare::CullAndQueue();

			LensFlare::g_activeLensFlareIdx = 0;

			if (LensFlare::g_flareActive[0])
			{
				int32_t index = 0;

				if (LensFlare::g_flareSlotCounts[0] > 0)
				{
					do
					{
						LensFlare::RenderSlot(index);

						++index;
					} while (index < LensFlare::g_flareSlotCounts[LensFlare::g_activeLensFlareIdx]);
				}
			}
		}

		int32_t activeIdx = LensFlare::g_activeLensFlareIdx;
		LensFlare::g_flareActive[activeIdx] = 0;
		LensFlare::g_flareSlotCounts[activeIdx] = 0;
		LensFlare::g_lensFlareCount = 0;
	}
}

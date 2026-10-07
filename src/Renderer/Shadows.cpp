#include "Renderer/Shadows.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderType.h"
#include "Renderer/Sprite.h"
#include "NGNLoader/NGNLoader.h"

namespace Renderer
{
	namespace Shadows
	{
		// GLOBAL: TOY2 0x0054F098
		ShadowRecord g_shadowRecords[48];

		// GLOBAL: TOY2 0x0054DEC8
		ShadowStretch g_shadowStretch[48];

		// GLOBAL: TOY2 0x0054DEB0
		int16_t g_shadowCount;

		// GLOBAL: TOY2 0x0054DD64
		int32_t g_unusedShadowVar;

		// FUNCTION: TOY2 0x00445720
		void DrawAll()
		{
			uint32_t bitmapWidthOut = 255;
			uint32_t bitmapHeightOut = 255;
			RGBA color;
			Vector2F uvTopLeft;
			Vector2F uvBottomRight;
			Vector3F verts[4];

			color.value = (g_deviceBlendShadeCapsCpy & 4) != 0 ? 0xFFFFFF : 0x88000000;

			int32_t texDataIdx = NGNLoader::GetTextureDataIndex(31);

			if (texDataIdx)
				NGNLoader::RetrieveTextureData(texDataIdx, &bitmapWidthOut, &bitmapHeightOut, 0, 0, 0);

			int32_t i = 0;

			if (g_shadowCount > 0)
			{
				ShadowStretch* stretch = &g_shadowStretch[0];
				ShadowRecord* record = &g_shadowRecords[0];

				do
				{
					int32_t groundY = record->groundY;
					int32_t worldZ = record->worldZ;
					int16_t size = record->size;
					float x = (float)(record->worldX >> 5);

					float y = (float)((groundY >> 5) - 10.0);
					float z = (float)(worldZ >> 5);
					float sizeF = (float)size;
					float minX = x - sizeF;
					verts[1].x = minX;

					if (size <= 0)
					{
						int32_t skewX1 = stretch->skewX1;
						int32_t skewZ = stretch->skewZ;
						verts[1].y = stretch->skewX0 + y;
						verts[1].z = sizeF + z;
						float maxX = sizeF + x;
						verts[3].x = maxX;
						verts[2].x = maxX;
						color.a = (uint8_t)stretch->alpha;
						verts[3].y = y;
						verts[3].z = verts[1].z;
						verts[2].y = skewX1 + y;
						float minZ = z - sizeF;
						verts[2].z = minZ;
						verts[0].x = minX;
						verts[0].z = minZ;
						verts[0].y = skewZ + y;
					}
					else
					{
						color.a = 32;
						verts[1].y = y;
						verts[2].y = y;
						verts[1].z = z - sizeF;
						float maxX = sizeF + x;
						verts[3].x = maxX;
						verts[3].y = y;
						verts[2].x = maxX;
						verts[3].z = verts[1].z;
						verts[0].x = minX;
						verts[0].y = y;
						verts[0].z = sizeF + z;
						verts[2].z = verts[0].z;
					}

					double width = bitmapWidthOut;
					uvTopLeft.x = (float)(96.0 / width);
					float height = (float)bitmapHeightOut;
					uvTopLeft.y = (float)(96.0 / height);
					uvBottomRight.x = (float)(127.0 / width);
					uvBottomRight.y = (float)(127.0 / height);

					if ((g_deviceBlendShadeCapsCpy & 4) != 0)
						Sprite::QueueQuadSpriteFromVerts(
							verts, &uvTopLeft, &uvBottomRight, texDataIdx, color, RENDER_ZWRITE | RENDER_ZBIAS_2 | RENDER_CULL_NONE | RENDER_ALPHA_ALT);
					else
						Sprite::QueueQuadSpriteFromVerts(
							verts, &uvTopLeft, &uvBottomRight, texDataIdx, color, RENDER_ZWRITE | RENDER_ZBIAS_2 | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT);

					++i;
					++record;
					++stretch;
				} while (i < g_shadowCount);
			}
		}
	}
}

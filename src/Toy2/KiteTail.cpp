#include "Toy2/KiteTail.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderType.h"
#include "Renderer/Sprite.h"
#include "Renderer/SpriteSheets.h"
#include "NGNLoader/NGNLoader.h"

namespace Toy2
{
	namespace KiteTail
	{
		// one rope of the kite tail, allocated from the level load arena by Init
		struct Rope
		{
			WorldPos points[96];
			int32_t pointCount;
			int32_t spacing;
			int32_t unk608;
			int32_t flags;
		};

		STATIC_ASSERT(sizeof(Rope) == 0x610);

		// GLOBAL: TOY2 0x00559C20
		Rope* g_ropes[8];

		// FUNCTION: TOY2 0x0044EB90
		void Draw()
		{
			Vector2F uvBottomRight;
			Vector2F uvTopLeft;
			Vector3F position;
			Vector3I delta;
			RGBA color;

			uint32_t bitmapWidth = 255;
			uint32_t bitmapHeight = 255;
			Rope** ropeSlot = (Rope**)g_ropes;

			do
			{
				Rope* rope = *ropeSlot;

				if (*ropeSlot && (rope->flags & 1) != 0)
				{
					Vector3I* point = (Vector3I*)rope;
					int32_t i = 0;

					if (rope->pointCount - 1 > 0)
					{
						uint8_t shade = 0x80;

						do
						{
							delta.x = ((Vector3I*)((char*)point + 16))->x - point->x;
							delta.y = ((Vector3I*)((char*)point + 16))->y - point->y;
							delta.z = ((Vector3I*)((char*)point + 16))->z - point->z;
							Renderer::Sprite::QueueSegment(point, &delta, shade, 0, 0);
							point = (Vector3I*)((char*)point + 16);
							++i;
							shade -= 4;
						} while (i < (*ropeSlot)->pointCount - 1);
					}

					Renderer::SpriteSheet* sheet = Renderer::g_spriteSheets[51];
					int32_t texDataIndex = NGNLoader::GetTextureDataIndex(sheet->texIndex);

					if (texDataIndex)
						NGNLoader::RetrieveTextureData(texDataIndex, &bitmapWidth, &bitmapHeight, 0, 0, 0);

					uvTopLeft.x = sheet->tiles[0].x / (double)bitmapWidth;
					float bitmapHeightF = (float)bitmapHeight;
					uvTopLeft.y = sheet->tiles[0].y / bitmapHeightF;
					uvBottomRight.x = (sheet->tileWidth + sheet->tiles[0].x) / (double)bitmapWidth;
					i = 0;
					uvBottomRight.y = (sheet->tileHeight + sheet->tiles[0].y) / bitmapHeightF;

					color.value = 0xFFFFFFFF;

					point = (Vector3I*)*ropeSlot;
					int32_t count = ((Rope*)point)->pointCount;

					if (count > 0)
					{
						do
						{
							position.x = point->x >> 5;
							position.y = point->y >> 5;
							position.z = point->z >> 5;

							Renderer::Sprite::QueueQuadSprite(
								&position,
								0,
								140.0,
								140.0,
								&uvTopLeft,
								&uvBottomRight,
								texDataIndex,
								color,
								Renderer::RENDER_ZWRITE | Renderer::RENDER_CULL_NONE | Renderer::RENDER_ALPHA_DEFAULT);

							point = (Vector3I*)((char*)point + 16);
							++i;
						} while (i < (*ropeSlot)->pointCount);
					}
				}

				++ropeSlot;
			} while (ropeSlot < &g_ropes[8]);
		}

		// STUB: TOY2 0x0044E620
		uint32_t* Init(Vector3I* a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5) { return 0; }

		// STUB: TOY2 0x0044E6D0
		void Activate(int32_t a1) {}

		// STUB: TOY2 0x0044E6F0
		void Deactivate(int32_t a1) {}

		// STUB: TOY2 0x0044E710
		void Simulate(Vector3I* anchor, int32_t slot) {}
	}
}

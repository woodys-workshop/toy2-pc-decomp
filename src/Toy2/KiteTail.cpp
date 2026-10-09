#include "Toy2/KiteTail.h"
#include "Toy2/Collision.h"
#include "Toy2/Levels.h"
#include "Nu3D/Math.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderType.h"
#include "Renderer/Sprite.h"
#include "Renderer/SpriteSheets.h"
#include "NGNLoader/NGNLoader.h"

#include <stdlib.h>

namespace Toy2
{
	namespace KiteTail
	{
		// the velocity of one rope point, the last word is set when the point hit the floor of the arena
		struct RopeVelocity
		{
			int16_t x;
			int16_t y;
			int16_t z;
			int16_t grounded;
		};

		// one rope of the kite tail, allocated from the level load arena by Init
		struct Rope
		{
			WorldPos points[64];
			RopeVelocity velocities[64];
			int32_t pointCount;
			int32_t spacing;
			int32_t gravity;
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

		// FUNCTION: TOY2 0x0044E620
		uint32_t* Init(Vector3I* a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5)
		{
			Rope* rope = (Rope*)Levels::g_levelLoadArena;
			g_ropes[a4] = rope;
			Levels::g_levelLoadArena += sizeof(Rope);

			g_ropes[a4]->pointCount = a2;
			g_ropes[a4]->spacing = a3;
			g_ropes[a4]->gravity = a5;

			rope = g_ropes[a4];
			uint32_t* result = (uint32_t*)rope->velocities;

			if (a2 > 0)
			{
				int32_t offsetY = 0;
				int32_t i = 0;
				int32_t remaining = a2;

				do
				{
					result += 2;
					rope->points[i].x = a1->x;
					rope->points[i].y = a1->y + offsetY;
					rope->points[i].z = a1->z;
					rope->velocities[i].x = 0;
					rope->velocities[i].y = 0;
					rope->velocities[i].z = 0;
					rope->velocities[i].grounded = 0;
					++i;
					offsetY += a3;
					--remaining;
				} while (remaining);
			}

			return result;
		}

		// FUNCTION: TOY2 0x0044E6D0
		void Activate(int32_t a1) { g_ropes[a1]->flags |= 1; }

		// FUNCTION: TOY2 0x0044E6F0
		void Deactivate(int32_t a1) { g_ropes[a1]->flags &= ~1; }

		// FUNCTION: TOY2 0x0044E710
		void Simulate(Vector3I* anchor, int32_t slot)
		{
			WorldPos* scratch = (WorldPos*)Collision::g_mathScratch;
			Rope* rope = g_ropes[slot];
			int32_t i;

			rope->points[0].x = anchor->x;
			rope->points[0].y = anchor->y;
			rope->points[0].z = anchor->z;

			int32_t spacing = rope->spacing;
			int32_t gravity = rope->gravity;

			// move every point along its velocity, then place it at the rope spacing from the point before it
			i = 1;

			if (rope->pointCount > 1)
			{
				do
				{
					rope->points[i].x += rope->velocities[i].x;
					rope->points[i].y += rope->velocities[i].y;
					rope->points[i].z += rope->velocities[i].z;

					scratch[i - 1].x = (rope->points[i].x - rope->points[i - 1].x) >> 3;
					scratch[i - 1].y = (rope->points[i].y - rope->points[i - 1].y) >> 3;
					scratch[i - 1].z = (rope->points[i].z - rope->points[i - 1].z) >> 3;

					Nu3D::Math::NormalizeToFixedPoint((Vector3I*)&scratch[i - 1], (Vector3I*)&scratch[i - 1]);

					scratch[i - 1].x = rope->points[i - 1].x + ((spacing * scratch[i - 1].x) >> 12);
					scratch[i - 1].y = rope->points[i - 1].y + ((spacing * scratch[i - 1].y) >> 12);
					scratch[i - 1].z = rope->points[i - 1].z + ((spacing * scratch[i - 1].z) >> 12);

					++i;
				} while (i < rope->pointCount);
			}

			// pull every velocity towards the neighbouring points, damp it and add the gravity
			i = 1;

			if (rope->pointCount - 1 > 1)
			{
				do
				{
					rope->velocities[i].x = ((rope->points[i + 1].x - scratch[i].x) >> 5) + ((scratch[i - 1].x - rope->points[i].x) >> 4) + 30 * rope->velocities[i].x / 32;
					rope->velocities[i].y = gravity * Renderer::g_frameDelta + 30 * rope->velocities[i].y / 32 + ((scratch[i - 1].y - rope->points[i].y) >> 4)
						+ ((rope->points[i + 1].y - scratch[i].y) >> 5);
					rope->velocities[i].z = 30 * rope->velocities[i].z / 32 + ((scratch[i - 1].z - rope->points[i].z) >> 4) + ((rope->points[i + 1].z - scratch[i].z) >> 5);

					++i;
				} while (i < rope->pointCount - 1);
			}

			// the last point has no neighbour after it
			rope->velocities[i].x = ((scratch[i - 1].x - rope->points[i].x) >> 4) + 30 * rope->velocities[i].x / 32;
			rope->velocities[i].y = gravity * Renderer::g_frameDelta + 30 * rope->velocities[i].y / 32 + ((scratch[i - 1].y - rope->points[i].y) >> 4);
			rope->velocities[i].z = 30 * rope->velocities[i].z / 32 + ((scratch[i - 1].z - rope->points[i].z) >> 4);

			// keep the points out of the arena walls, then store the new positions
			i = 1;

			if (rope->pointCount > 1)
			{
				do
				{
					WorldPos* point = &scratch[i - 1];
					RopeVelocity* velocity = &rope->velocities[i];

					if ((uint32_t)(point->x - 118212) < 0x1558C)
					{
						if ((uint32_t)(point->z - 183960) < 0x1FC38 && point->y >= -467398)
						{
							if ((uint32_t)(point->x - 117188) >= 0x14D8C || (uint32_t)(point->z - 182936) >= 0x1F438)
							{
								int32_t distX = abs(point->x - 118212);
								int32_t distX2 = abs(point->x - 205648);

								if (distX2 < distX)
									distX = -distX2;

								int32_t distZ = abs(point->z - 183960);
								int32_t distZ2 = abs(point->z - 314064);

								if (distZ2 < distZ)
									distZ = -distZ2;

								int32_t depth = point->y + 467398;
								velocity->grounded = 0;
								int32_t absDistX = abs(distX);

								if (absDistX >= depth)
								{
									if (abs(distZ) >= depth)
									{
										point->y = -467654;
										velocity->grounded = 1;
									}
									else
									{
										point->z = distZ <= 0 ? 314320 : 183704;
										velocity->y += (-6 * gravity * (int32_t)Renderer::g_frameDelta) / 4;
									}
								}
								else
								{
									if (absDistX >= abs(distZ))
										point->z = distZ <= 0 ? 314320 : 183704;
									else
										point->x = distX <= 0 ? 205904 : 117956;

									velocity->y += (-6 * gravity * (int32_t)Renderer::g_frameDelta) / 4;
								}
							}
							else
							{
								point->y = -467654;
								velocity->grounded = 1;
							}
						}
					}

					rope->points[i].x = point->x;
					rope->points[i].y = point->y;
					rope->points[i].z = point->z;

					++i;
				} while (i < rope->pointCount);
			}
		}
	}
}

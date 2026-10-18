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
			Transform points[64];
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
					rope->points[i].pos.x = a1->x;
					rope->points[i].pos.y = a1->y + offsetY;
					rope->points[i].pos.z = a1->z;
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
			Transform* scratch = (Transform*)Collision::g_mathScratch;
			Rope* rope = g_ropes[slot];
			int32_t i;

			rope->points[0].pos.x = anchor->x;
			rope->points[0].pos.y = anchor->y;
			rope->points[0].pos.z = anchor->z;

			int32_t spacing = rope->spacing;
			int32_t gravity = rope->gravity;

			// move every point along its velocity, then place it at the rope spacing from the point before it
			i = 1;

			if (rope->pointCount > 1)
			{
				do
				{
					rope->points[i].pos.x += rope->velocities[i].x;
					rope->points[i].pos.y += rope->velocities[i].y;
					rope->points[i].pos.z += rope->velocities[i].z;

					scratch[i - 1].pos.x = (rope->points[i].pos.x - rope->points[i - 1].pos.x) >> 3;
					scratch[i - 1].pos.y = (rope->points[i].pos.y - rope->points[i - 1].pos.y) >> 3;
					scratch[i - 1].pos.z = (rope->points[i].pos.z - rope->points[i - 1].pos.z) >> 3;

					Nu3D::Math::NormalizeToFixedPoint((Vector3I*)&scratch[i - 1], (Vector3I*)&scratch[i - 1]);

					scratch[i - 1].pos.x = rope->points[i - 1].pos.x + ((spacing * scratch[i - 1].pos.x) >> 12);
					scratch[i - 1].pos.y = rope->points[i - 1].pos.y + ((spacing * scratch[i - 1].pos.y) >> 12);
					scratch[i - 1].pos.z = rope->points[i - 1].pos.z + ((spacing * scratch[i - 1].pos.z) >> 12);

					++i;
				} while (i < rope->pointCount);
			}

			// pull every velocity towards the neighbouring points, damp it and add the gravity
			i = 1;

			if (rope->pointCount - 1 > 1)
			{
				do
				{
					rope->velocities[i].x = ((rope->points[i + 1].pos.x - scratch[i].pos.x) >> 5) + ((scratch[i - 1].pos.x - rope->points[i].pos.x) >> 4)
						+ 30 * rope->velocities[i].x / 32;
					rope->velocities[i].y = gravity * Renderer::g_frameDelta + 30 * rope->velocities[i].y / 32
						+ ((scratch[i - 1].pos.y - rope->points[i].pos.y) >> 4) + ((rope->points[i + 1].pos.y - scratch[i].pos.y) >> 5);
					rope->velocities[i].z = 30 * rope->velocities[i].z / 32 + ((scratch[i - 1].pos.z - rope->points[i].pos.z) >> 4)
						+ ((rope->points[i + 1].pos.z - scratch[i].pos.z) >> 5);

					++i;
				} while (i < rope->pointCount - 1);
			}

			// the last point has no neighbour after it
			rope->velocities[i].x = ((scratch[i - 1].pos.x - rope->points[i].pos.x) >> 4) + 30 * rope->velocities[i].x / 32;
			rope->velocities[i].y = gravity * Renderer::g_frameDelta + 30 * rope->velocities[i].y / 32 + ((scratch[i - 1].pos.y - rope->points[i].pos.y) >> 4);
			rope->velocities[i].z = 30 * rope->velocities[i].z / 32 + ((scratch[i - 1].pos.z - rope->points[i].pos.z) >> 4);

			// keep the points out of the arena walls, then store the new positions
			i = 1;

			if (rope->pointCount > 1)
			{
				do
				{
					Transform* point = &scratch[i - 1];
					RopeVelocity* velocity = &rope->velocities[i];

					if ((uint32_t)(point->pos.x - 118212) < 0x1558C)
					{
						if ((uint32_t)(point->pos.z - 183960) < 0x1FC38 && point->pos.y >= -467398)
						{
							if ((uint32_t)(point->pos.x - 117188) >= 0x14D8C || (uint32_t)(point->pos.z - 182936) >= 0x1F438)
							{
								int32_t distX = abs(point->pos.x - 118212);
								int32_t distX2 = abs(point->pos.x - 205648);

								if (distX2 < distX)
									distX = -distX2;

								int32_t distZ = abs(point->pos.z - 183960);
								int32_t distZ2 = abs(point->pos.z - 314064);

								if (distZ2 < distZ)
									distZ = -distZ2;

								int32_t depth = point->pos.y + 467398;
								velocity->grounded = 0;
								int32_t absDistX = abs(distX);

								if (absDistX >= depth)
								{
									if (abs(distZ) >= depth)
									{
										point->pos.y = -467654;
										velocity->grounded = 1;
									}
									else
									{
										point->pos.z = distZ <= 0 ? 314320 : 183704;
										velocity->y += (-6 * gravity * (int32_t)Renderer::g_frameDelta) / 4;
									}
								}
								else
								{
									if (absDistX >= abs(distZ))
										point->pos.z = distZ <= 0 ? 314320 : 183704;
									else
										point->pos.x = distX <= 0 ? 205904 : 117956;

									velocity->y += (-6 * gravity * (int32_t)Renderer::g_frameDelta) / 4;
								}
							}
							else
							{
								point->pos.y = -467654;
								velocity->grounded = 1;
							}
						}
					}

					rope->points[i].pos.x = point->pos.x;
					rope->points[i].pos.y = point->pos.y;
					rope->points[i].pos.z = point->pos.z;

					++i;
				} while (i < rope->pointCount);
			}
		}
	}
}

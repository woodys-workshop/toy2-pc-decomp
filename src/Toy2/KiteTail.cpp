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
			Rope* rope = g_ropes[slot];
			int32_t i;

			rope->points[0].pos.x = anchor->x;
			rope->points[0].pos.y = anchor->y;
			rope->points[0].pos.z = anchor->z;

			rope = g_ropes[slot];
			int32_t spacing = rope->spacing;
			int32_t gravity = rope->gravity;
			Transform* point = &rope->points[1];
			RopeVelocity* velocity = &rope->velocities[1];
			Transform* target = (Transform*)Collision::g_mathScratch;

			// Integrate points before constraining their positions in the shared scratch.
			i = 1;

			if (rope->pointCount > 1)
			{
				do
				{
					point->pos.x += velocity->x;
					point->pos.y += velocity->y;
					point->pos.z += velocity->z;

					target->pos.x = (point->pos.x - point[-1].pos.x) >> 3;
					target->pos.y = (point->pos.y - point[-1].pos.y) >> 3;
					target->pos.z = (point->pos.z - point[-1].pos.z) >> 3;

					Nu3D::Math::NormalizeToFixedPoint(&target->pos, &target->pos);

					target->pos.x = point[-1].pos.x + ((spacing * target->pos.x) >> 12);
					target->pos.y = point[-1].pos.y + ((spacing * target->pos.y) >> 12);
					target->pos.z = point[-1].pos.z + ((spacing * target->pos.z) >> 12);

					++point;
					++velocity;
					++target;
					++i;
				} while (i < g_ropes[slot]->pointCount);
			}

			// Both neighbours contribute before any constrained position is published.
			rope = g_ropes[slot];
			point = &rope->points[1];
			velocity = &rope->velocities[1];
			target = (Transform*)Collision::g_mathScratch;
			i = 1;

			if (rope->pointCount - 1 > 1)
			{
				do
				{
					velocity->x = 30 * velocity->x / 32 + ((target->pos.x - point->pos.x) >> 4) + ((point[1].pos.x - target[1].pos.x) >> 5);
					velocity->y = ((point[1].pos.y - target[1].pos.y) >> 5) + ((target->pos.y - point->pos.y) >> 4) + 30 * velocity->y / 32
						+ gravity * Renderer::g_frameDelta;
					velocity->z = ((point[1].pos.z - target[1].pos.z) >> 5) + ((target->pos.z - point->pos.z) >> 4) + 30 * velocity->z / 32;

					++point;
					++velocity;
					++target;
					++i;
				} while (i < g_ropes[slot]->pointCount - 1);
			}

			velocity->x = 30 * velocity->x / 32 + ((target->pos.x - point->pos.x) >> 4);
			velocity->y = ((target->pos.y - point->pos.y) >> 4) + 30 * velocity->y / 32 + gravity * Renderer::g_frameDelta;
			velocity->z = ((target->pos.z - point->pos.z) >> 4) + 30 * velocity->z / 32;

			rope = g_ropes[slot];
			point = &rope->points[1];
			velocity = &rope->velocities[1];
			target = (Transform*)Collision::g_mathScratch;
			i = 1;

			if (rope->pointCount > 1)
			{
				do
				{
					if ((uint32_t)(target->pos.x - 118212) < 0x1558C)
					{
						if ((uint32_t)(target->pos.z - 183960) < 0x1FC38 && target->pos.y >= -467398)
						{
							if ((uint32_t)(target->pos.x - 117188) >= 0x14D8C || (uint32_t)(target->pos.z - 182936) >= 0x1F438)
							{
								int32_t distX = abs(target->pos.x - 118212);
								int32_t distX2 = abs(target->pos.x - 205648);

								if (distX2 < distX)
									distX = -distX2;

								int32_t distZ = abs(target->pos.z - 183960);
								int32_t distZ2 = abs(target->pos.z - 314064);

								if (distZ2 < distZ)
									distZ = -distZ2;

								int32_t depth = target->pos.y + 467398;
								velocity->grounded = 0;
								int32_t absDistX = abs(distX);

								if (absDistX >= depth)
								{
									if (abs(distZ) >= depth)
									{
										target->pos.y = -467654;
										velocity->grounded = 1;
									}
									else
									{
										target->pos.z = distZ <= 0 ? 314320 : 183704;
										velocity->y += (-6 * gravity * (int32_t)Renderer::g_frameDelta) / 4;
									}
								}
								else
								{
									if (absDistX >= abs(distZ))
										target->pos.z = distZ <= 0 ? 314320 : 183704;
									else
										target->pos.x = distX <= 0 ? 205904 : 117956;

									velocity->y += (-6 * gravity * (int32_t)Renderer::g_frameDelta) / 4;
								}
							}
							else
							{
								target->pos.y = -467654;
								velocity->grounded = 1;
							}
						}
					}

					point->pos.x = target->pos.x;
					point->pos.y = target->pos.y;
					point->pos.z = target->pos.z;

					++point;
					++velocity;
					++target;
					++i;
				} while (i < g_ropes[slot]->pointCount);
			}
		}
	}
}

#pragma once

#include "Common.h"

namespace Renderer
{
	struct SpriteSheet
	{
		struct Tile
		{
			uint8_t x;
			uint8_t y;
		};

		int16_t texIndex;
		uint8_t tileWidth;
		uint8_t tileHeight;
		int32_t zero;
		Tile tiles[];
	};

	extern SpriteSheet* g_spriteSheets[130];
	extern SpriteSheet g_rawLoaderSpriteSheet;

	void InitSpriteSheets();

	STATIC_ASSERT(sizeof(SpriteSheet) == 8);
	STATIC_ASSERT(sizeof(SpriteSheet::Tile) == 2);
}
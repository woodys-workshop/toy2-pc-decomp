#include "Renderer/SpriteSheets.h"
#include "Toy2/Toy2.h"

#include <WINDOWS.H>
#include <STDIO.H>

#define SPRITE_SHEET_END ((SpriteSheet*)-1)

namespace Renderer
{
	// this was way too big to put inside of Renderer/Sprite.h
	// $TODO: Add the global addresses to these

	// 0-127 are the sheets of the level, 128 is g_rawLoaderSpriteSheet (the loading / press jump text) and 129 is the fallback sheet
	// GLOBAL: TOY2 0x00557500
	SpriteSheet* g_spriteSheets[130];

	// GLOBAL: TOY2 0x004F7408
	SpriteSheet g_rawLoaderSpriteSheet = {
		32,
		255,
		32,
		0,
		{
			{ 0, 164 },
			{ 0, 201 },
		},
	};

	// GLOBAL: TOY2 0x004F7268
	SpriteSheet g_fallbackSpriteSheetObj = {
		-1,
		1,
		1,
		-1,
		{
			{ 0, 0 },
			{ 0, 0 },
			{ 255, 255 },
			{ 255, 255 },
			{ 117, 0 },
			{ 0, 0 },
			{ 16, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet1 = {
		31,
		32,
		32,
		0,
		{
			{ 0, 0 },	 { 32, 0 },	  { 64, 0 },   { 96, 0 },	{ 128, 0 },	  { 160, 0 },	{ 192, 0 },	  { 224, 0 },	{ 0, 32 },	  { 32, 32 },
			{ 64, 32 },	 { 96, 32 },  { 128, 32 }, { 160, 32 }, { 192, 32 },  { 224, 32 },	{ 0, 64 },	  { 32, 64 },	{ 64, 64 },	  { 96, 64 },
			{ 128, 64 }, { 160, 64 }, { 192, 64 }, { 224, 64 }, { 0, 96 },	  { 32, 96 },	{ 64, 96 },	  { 96, 96 },	{ 128, 96 },  { 160, 96 },
			{ 192, 96 }, { 224, 96 }, { 0, 128 },  { 32, 128 }, { 64, 128 },  { 96, 128 },	{ 128, 128 }, { 160, 128 }, { 192, 128 }, { 224, 128 },
			{ 0, 160 },	 { 32, 160 }, { 64, 160 }, { 96, 160 }, { 128, 160 }, { 160, 160 }, { 192, 160 }, { 224, 160 },
		},
	};

	SpriteSheet g_level0Sheet2 = {
		5,
		255,
		128,
		0,
		{
			{ 0, 0 },
			{ 0, 0 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet3 = {
		4,
		32,
		32,
		0,
		{
			{ 0, 32 }, { 32, 32 }, { 64, 32 }, { 96, 32 }, { 128, 32 }, { 160, 32 }, { 192, 32 }, { 224, 32 },
			{ 0, 64 }, { 32, 64 }, { 64, 64 }, { 96, 64 }, { 128, 64 }, { 160, 64 }, { 192, 64 }, { 224, 64 },
			{ 0, 0 },  { 32, 0 },  { 64, 0 },  { 96, 0 },  { 128, 0 },	{ 160, 0 },	 { 192, 0 },  { 224, 0 },
		},
	};

	SpriteSheet g_level0Sheet4 = {
		6,
		84,
		84,
		0,
		{
			{ 0, 0 },
			{ 84, 0 },
			{ 168, 0 },
			{ 0, 84 },
			{ 84, 84 },
			{ 168, 84 },
			{ 0, 168 },
			{ 84, 168 },
			{ 168, 168 },
			{ 0, 0 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet5 = {
		7,
		84,
		84,
		0,
		{
			{ 0, 0 },
			{ 84, 0 },
			{ 168, 0 },
			{ 0, 84 },
			{ 84, 84 },
			{ 168, 84 },
			{ 0, 168 },
			{ 84, 168 },
			{ 168, 168 },
			{ 0, 0 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet6 = {
		4,
		80,
		16,
		0,
		{
			{ 0, 96 },
			{ 80, 96 },
			{ 160, 96 },
			{ 0, 112 },
			{ 80, 112 },
			{ 160, 112 },
			{ 0, 128 },
			{ 80, 128 },
			{ 160, 128 },
			{ 0, 144 },
			{ 80, 144 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet7 = {
		4,
		32,
		32,
		0,
		{
			{ 0, 160 },
			{ 32, 160 },
			{ 64, 160 },
			{ 96, 160 },
		},
	};

	SpriteSheet g_level0Sheet8 = {
		8,
		84,
		84,
		0,
		{
			{ 0, 0 },
			{ 84, 0 },
			{ 168, 0 },
			{ 0, 84 },
			{ 84, 84 },
			{ 168, 84 },
			{ 0, 168 },
			{ 84, 168 },
			{ 168, 168 },
			{ 0, 0 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet9 = {
		9,
		84,
		84,
		0,
		{
			{ 0, 0 },
			{ 84, 0 },
			{ 168, 0 },
			{ 0, 84 },
			{ 84, 84 },
			{ 168, 84 },
			{ 0, 168 },
			{ 84, 168 },
			{ 168, 168 },
			{ 0, 0 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet10 = {
		0,
		74,
		96,
		0,
		{
			{ 0, 0 },
			{ 74, 0 },
			{ 148, 0 },
			{ 0, 96 },
			{ 74, 96 },
			{ 148, 96 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet11 = {
		1,
		74,
		96,
		0,
		{
			{ 0, 0 },
			{ 74, 0 },
			{ 148, 0 },
			{ 0, 96 },
			{ 74, 96 },
			{ 148, 96 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet12 = {
		2,
		78,
		96,
		0,
		{
			{ 0, 0 },
			{ 78, 0 },
			{ 156, 0 },
			{ 0, 96 },
			{ 78, 96 },
			{ 156, 96 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet13 = {
		31,
		32,
		32,
		0,
		{
			{ 0, 192 },
			{ 32, 192 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet14 = {
		0,
		16,
		16,
		0,
		{
			{ 0, 64 }, { 16, 64 }, { 32, 64 }, { 48, 64 }, { 64, 64 }, { 80, 64 }, { 96, 64 }, { 112, 64 }, { 128, 64 }, { 144, 64 }, { 160, 64 }, { 176, 64 },
			{ 0, 80 }, { 16, 80 }, { 32, 80 }, { 48, 80 }, { 64, 80 }, { 80, 80 }, { 96, 80 }, { 112, 80 }, { 128, 80 }, { 144, 80 }, { 160, 80 }, { 176, 80 },
			{ 0, 96 }, { 16, 96 }, { 32, 96 }, { 48, 96 }, { 64, 96 }, { 80, 96 }, { 96, 96 }, { 112, 96 }, { 128, 96 }, { 144, 96 }, { 160, 96 }, { 176, 96 },
		},
	};

	SpriteSheet g_level0Sheet15 = {
		0,
		15,
		15,
		0,
		{
			{ 0, 128 },
			{ 0, 0 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet16 = {
		0,
		31,
		31,
		0,
		{
			{ 16, 128 },
			{ 0, 0 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet17 = {
		0,
		44,
		74,
		0,
		{
			{ 64, 128 },
			{ 0, 0 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet18 = {
		10,
		16,
		24,
		0,
		{
			{ 0, 0 },	 { 16, 0 },	  { 32, 0 },   { 48, 0 },	{ 64, 0 },	 { 80, 0 },	  { 96, 0 },   { 112, 0 },	{ 128, 0 },	 { 144, 0 },  { 160, 0 },
			{ 176, 0 },	 { 192, 0 },  { 208, 0 },  { 224, 0 },	{ 240, 0 },	 { 0, 24 },	  { 16, 24 },  { 32, 24 },	{ 48, 24 },	 { 64, 24 },  { 80, 24 },
			{ 96, 24 },	 { 112, 24 }, { 128, 24 }, { 144, 24 }, { 160, 24 }, { 176, 24 }, { 192, 24 }, { 208, 24 }, { 224, 24 }, { 240, 24 }, { 0, 48 },
			{ 16, 48 },	 { 32, 48 },  { 48, 48 },  { 64, 48 },	{ 80, 48 },	 { 96, 48 },  { 112, 48 }, { 128, 48 }, { 144, 48 }, { 160, 48 }, { 176, 48 },
			{ 192, 48 }, { 208, 48 }, { 224, 48 }, { 240, 48 }, { 0, 72 },	 { 16, 72 },  { 32, 72 },  { 48, 72 },	{ 64, 72 },	 { 80, 72 },  { 96, 72 },
			{ 112, 72 }, { 128, 72 }, { 144, 72 }, { 160, 72 }, { 176, 72 }, { 192, 72 }, { 208, 72 }, { 224, 72 }, { 240, 72 },
		},
	};

	SpriteSheet g_level0Sheet19 = {
		5,
		16,
		16,
		0,
		{
			{ 0, 0 },
			{ 16, 0 },
			{ 32, 0 },
			{ 48, 0 },
		},
	};

	SpriteSheet g_level0Sheet20 = {
		17,
		16,
		16,
		0,
		{
			{ 64, 64 },
			{ 80, 64 },
			{ 96, 64 },
			{ 112, 64 },
		},
	};

	SpriteSheet g_level0Sheet21 = {
		0,
		48,
		32,
		0,
		{
			{ 0, 0 },	 { 48, 0 },	   { 96, 0 },	 { 144, 0 },  { 192, 0 },  { 0, 32 },	{ 48, 32 },	  { 96, 32 },	{ 144, 32 }, { 192, 32 }, { 0, 64 },
			{ 48, 64 },	 { 96, 64 },   { 144, 64 },	 { 192, 64 }, { 0, 96 },   { 48, 96 },	{ 96, 96 },	  { 144, 96 },	{ 192, 96 }, { 0, 128 },  { 48, 128 },
			{ 96, 128 }, { 144, 128 }, { 192, 128 }, { 0, 160 },  { 48, 160 }, { 96, 160 }, { 144, 160 }, { 192, 160 }, { 0, 192 },	 { 48, 192 },
		},
	};

	SpriteSheet g_level0Sheet22 = {
		0,
		48,
		32,
		0,
		{
			{ 0, 0 },
			{ 48, 0 },
			{ 96, 0 },
			{ 144, 0 },
			{ 192, 0 },
			{ 0, 32 },
			{ 48, 32 },
			{ 96, 32 },
			{ 144, 32 },
			{ 192, 32 },
			{ 0, 64 },
			{ 48, 64 },
			{ 96, 64 },
			{ 144, 64 },
			{ 192, 64 },
			{ 0, 96 },
		},
	};

	SpriteSheet g_level0Sheet23 = {
		1,
		32,
		32,
		0,
		{
			{ 0, 0 },
			{ 32, 0 },
			{ 64, 0 },
			{ 96, 0 },
			{ 128, 0 },
			{ 160, 0 },
			{ 192, 0 },
			{ 224, 0 },
			{ 0, 32 },
			{ 32, 32 },
			{ 64, 32 },
			{ 96, 32 },
			{ 128, 32 },
			{ 160, 32 },
			{ 192, 32 },
			{ 224, 32 },
		},
	};

	SpriteSheet g_level0Sheet24 = {
		2,
		24,
		22,
		0,
		{
			{ 24, 1 },
			{ 48, 1 },
			{ 72, 1 },
			{ 0, 25 },
			{ 24, 25 },
			{ 48, 25 },
			{ 72, 25 },
			{ 0, 49 },
			{ 24, 49 },
			{ 48, 49 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet25 = {
		17,
		16,
		16,
		0,
		{
			{ 240, 64 },
			{ 224, 64 },
			{ 208, 64 },
			{ 192, 64 },
		},
	};

	SpriteSheet g_level0Sheet26 = {
		3,
		64,
		64,
		0,
		{
			{ 128, 0 },
			{ 0, 0 },
			{ 192, 0 },
			{ 0, 64 },
			{ 64, 0 },
			{ 0, 0 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet27 = {
		18,
		64,
		64,
		0,
		{
			{ 0, 0 },	 { 64, 0 },	  { 64, 0 },  { 128, 0 },  { 192, 0 },	{ 192, 0 },	  { 0, 64 },	{ 64, 64 }, { 64, 64 }, { 128, 64 },
			{ 192, 64 }, { 192, 64 }, { 0, 128 }, { 64, 128 }, { 64, 128 }, { 128, 128 }, { 192, 128 }, { 0, 192 }, { 0, 0 },	{ 0, 0 },
		},
	};

	SpriteSheet g_level0Sheet28 = {
		19,
		64,
		64,
		0,
		{
			{ 0, 0 },
			{ 64, 0 },
			{ 64, 0 },
			{ 128, 0 },
			{ 192, 0 },
			{ 192, 0 },
			{ 0, 64 },
			{ 64, 64 },
			{ 64, 64 },
			{ 128, 64 },
			{ 192, 64 },
			{ 192, 64 },
			{ 0, 128 },
			{ 64, 128 },
			{ 64, 128 },
			{ 0, 0 },
		},
	};

	SpriteSheet* g_level0Sheets[35] = {
		&g_level0Sheet1,  &g_level0Sheet2,	&g_level0Sheet3,  &g_level0Sheet4,	&g_level0Sheet5,  &g_level0Sheet6,	&g_level0Sheet7,
		&g_level0Sheet8,  &g_level0Sheet9,	&g_level0Sheet10, &g_level0Sheet11, &g_level0Sheet12, &g_level0Sheet13, &g_level0Sheet14,
		&g_level0Sheet15, &g_level0Sheet16, &g_level0Sheet17, &g_level0Sheet18, &g_level0Sheet19, &g_level0Sheet17, &g_level0Sheet17,
		&g_level0Sheet17, &g_level0Sheet17, &g_level0Sheet17, &g_level0Sheet17, &g_level0Sheet20, &g_level0Sheet21, &g_level0Sheet22,
		&g_level0Sheet23, &g_level0Sheet24, &g_level0Sheet25, &g_level0Sheet26, &g_level0Sheet27, &g_level0Sheet28, SPRITE_SHEET_END,
	};

	SpriteSheet g_level1Sheet1 = { 8, 32, 32, 0, { { 128, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level1Sheet2 = { 8, 40, 32, 0, { { 64, 0 }, { 0, 0 } } };
	SpriteSheet* g_level1Sheets[5] = { &g_level1Sheet1, &g_level1Sheet1, &g_level1Sheet1, &g_level1Sheet2, SPRITE_SHEET_END };

	SpriteSheet g_level2Sheet1 = { 8, 32, 32, 0, { { 96, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level2Sheet2 = { 8, 31, 23, 0, { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level2Sheet3 = { 8, 31, 31, 0, { { 64, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level2Sheet4 = { 8, 31, 31, 0, { { 128, 0 }, { 0, 0 } } };
	SpriteSheet* g_level2Sheets[5] = { &g_level2Sheet1, &g_level2Sheet2, &g_level2Sheet3, &g_level2Sheet4, SPRITE_SHEET_END };

	SpriteSheet g_level3Sheet1 = { 8, 8, 32, 0, { { 0, 0 }, { 0, 0 } } };
	SpriteSheet* g_level3Sheets[2] = { &g_level3Sheet1, SPRITE_SHEET_END };

	SpriteSheet g_level4Sheet1 = { 8, 32, 32, 0, { { 64, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level4Sheet2 = { 8, 32, 32, 0, { { 128, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level4Sheet3 = { 8, 31, 31, 0, { { 0, 0 }, { 0, 0 } } };
	SpriteSheet* g_level4Sheets[4] = { &g_level4Sheet1, &g_level4Sheet2, &g_level4Sheet3, SPRITE_SHEET_END };

	SpriteSheet g_level5Sheet2 = { 8, 32, 32, 0, { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level5Sheet1 = { 8, 32, 32, 0, { { 64, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level5Sheet3 = { 8, 8, 32, 0, { { 128, 0 }, { 0, 0 } } };
	SpriteSheet* g_level5Sheets[4] = { &g_level5Sheet1, &g_level5Sheet2, &g_level5Sheet3, SPRITE_SHEET_END };

	SpriteSheet g_level6Sheet1 = { 31, 31, 31, 0, { { 96, 96 }, { 0, 0 } } };
	SpriteSheet* g_level6Sheets[5] = { &g_level6Sheet1, &g_level6Sheet1, &g_level6Sheet1, &g_level6Sheet1, SPRITE_SHEET_END };

	SpriteSheet g_level7Sheet2 = { 1, 63, 63, 0, { { 192, 192 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level7Sheet1 = { 7, 32, 32, 0, { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level7Sheet3 = { 7, 31, 31, 0, { { 64, 0 }, { 0, 0 } } };
	SpriteSheet* g_level7Sheets[4] = { &g_level7Sheet1, &g_level7Sheet2, &g_level7Sheet3, SPRITE_SHEET_END };

	SpriteSheet g_level8Sheet3 = { 8, 31, 31, 0, { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level8Sheet2 = { 8, 31, 31, 0, { { 64, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level8Sheet1 = { 8, 32, 32, 0, { { 192, 0 }, { 0, 0 } } };
	SpriteSheet* g_level8Sheets[4] = { &g_level8Sheet1, &g_level8Sheet2, &g_level8Sheet3, SPRITE_SHEET_END };

	SpriteSheet* g_level9Sheets[1] = { SPRITE_SHEET_END };

	SpriteSheet g_level10Sheet1 = { 8, 32, 32, 0, { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level10Sheet2 = { 8, 63, 63, 0, { { 64, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level10Sheet3 = { 8, 31, 31, 0, { { 128, 0 }, { 0, 0 } } };

	SpriteSheet* g_level10Sheets[7] = {
		&g_level10Sheet1, &g_level10Sheet1, &g_level10Sheet1, &g_level10Sheet2, &g_level10Sheet1, &g_level10Sheet3, SPRITE_SHEET_END,
	};

	SpriteSheet g_level11Sheet1 = { 7, 32, 32, 0, { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level11Sheet2 = { 7, 32, 32, 0, { { 192, 196 }, { 0, 0 } } };
	SpriteSheet* g_level11Sheets[3] = { &g_level11Sheet1, &g_level11Sheet2, SPRITE_SHEET_END };

	SpriteSheet g_level12Sheet1 = { 8, 31, 31, 0, { { 0, 0 }, { 0, 0 } } };
	SpriteSheet* g_level12Sheets[4] = { &g_level12Sheet1, &g_level12Sheet1, &g_level12Sheet1, SPRITE_SHEET_END };

	SpriteSheet g_level13Sheet1 = { 23, 32, 32, 0, { { 0, 128 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level13Sheet2 = { 23, 32, 32, 0, { { 64, 128 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level13Sheet4 = { 31, 31, 16, 0, { { 160, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level13Sheet5 = { 31, 31, 31, 0, { { 128, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level13Sheet6 = { 23, 31, 31, 0, { { 160, 64 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level13Sheet3 = { 23, 31, 47, 0, { { 128, 64 }, { 0, 0 } } };
	SpriteSheet* g_level13Sheets[7] = {
		&g_level13Sheet1, &g_level13Sheet2, &g_level13Sheet3, &g_level13Sheet4, &g_level13Sheet5, &g_level13Sheet6, SPRITE_SHEET_END,
	};

	SpriteSheet g_level14Sheet1 = { 23, 32, 32, 0, { { 64, 64 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level14Sheet3 = { 31, 31, 16, 0, { { 160, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level14Sheet2 = { 23, 8, 31, 0, { { 128, 64 }, { 0, 0 } } };
	SpriteSheet* g_level14Sheets[5] = {
		&g_level14Sheet1, &g_level14Sheet2, &g_level14Sheet3, &g_level14Sheet3, SPRITE_SHEET_END,
	};

	SpriteSheet g_level15Sheet1 = { 31, 31, 16, 0, { { 160, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level15Sheet3 = { 31, 31, 31, 0, { { 128, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level15Sheet4 = { 23, 31, 31, 0, { { 160, 64 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level15Sheet2 = { 23, 31, 47, 0, { { 128, 64 }, { 0, 0 } } };
	SpriteSheet* g_level15Sheets[7] = {
		&g_level15Sheet1, &g_level15Sheet1, &g_level15Sheet2, &g_level15Sheet1, &g_level15Sheet3, &g_level15Sheet4, SPRITE_SHEET_END,
	};

	SpriteSheet g_level16Sheet2 = {
		1,
		255,
		20,
		0,
		{
			{ 0, 0 },
			{ 0, 20 },
			{ 0, 40 },
			{ 0, 60 },
			{ 0, 80 },
			{ 0, 100 },
			{ 0, 120 },
			{ 0, 140 },
			{ 0, 160 },
			{ 0, 180 },
			{ 0, 200 },
			{ 0, 220 },
		},
	};

	SpriteSheet g_level16Sheet3 = {
		2,
		255,
		20,
		0,
		{
			{ 0, 0 },
			{ 0, 20 },
			{ 0, 40 },
			{ 0, 60 },
			{ 0, 80 },
			{ 0, 100 },
			{ 0, 120 },
			{ 0, 140 },
			{ 0, 160 },
			{ 0, 180 },
			{ 0, 200 },
			{ 0, 220 },
		},
	};

	SpriteSheet g_level16Sheet4 = {
		3,
		255,
		20,
		0,
		{
			{ 0, 0 },
			{ 0, 20 },
			{ 0, 40 },
			{ 0, 60 },
			{ 0, 80 },
			{ 0, 100 },
			{ 0, 120 },
			{ 0, 140 },
			{ 0, 160 },
			{ 0, 180 },
			{ 0, 200 },
			{ 0, 220 },
		},
	};

	SpriteSheet g_level16Sheet1 = {
		4,
		16,
		16,
		0,
		{
			{ 0, 128 },	  { 16, 128 },	{ 32, 128 },  { 48, 128 },	{ 64, 128 },  { 80, 128 },	{ 96, 128 },  { 112, 128 },
			{ 128, 128 }, { 144, 128 }, { 160, 128 }, { 176, 128 }, { 192, 128 }, { 208, 128 }, { 224, 128 }, { 240, 128 },
			{ 0, 144 },	  { 16, 144 },	{ 32, 144 },  { 48, 144 },	{ 64, 144 },  { 80, 144 },	{ 96, 144 },  { 112, 144 },
			{ 128, 144 }, { 144, 144 }, { 160, 144 }, { 176, 144 }, { 192, 144 }, { 208, 144 }, { 224, 144 }, { 240, 144 },
		},
	};

	SpriteSheet g_level16Sheet6 = { 4, 96, 64, 0, { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level16Sheet5 = {
		0,
		48,
		32,
		0,
		{
			{ 0, 0 },	 { 48, 0 },	   { 96, 0 },	 { 144, 0 },  { 192, 0 },  { 0, 32 },	{ 48, 32 },	  { 96, 32 },	{ 144, 32 }, { 192, 32 }, { 0, 64 },
			{ 48, 64 },	 { 96, 64 },   { 144, 64 },	 { 192, 64 }, { 0, 96 },   { 48, 96 },	{ 96, 96 },	  { 144, 96 },	{ 192, 96 }, { 0, 128 },  { 48, 128 },
			{ 96, 128 }, { 144, 128 }, { 192, 128 }, { 0, 160 },  { 48, 160 }, { 96, 160 }, { 144, 160 }, { 192, 160 }, { 0, 192 },	 { 48, 192 },
		},
	};
	SpriteSheet g_level16Sheet7 = { 4, 32, 58, 0, { { 96, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level16Sheet8 = { 17, 32, 32, 0, { { 128, 64 }, { 128, 96 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level16Sheet9 = {
		17,
		16,
		16,
		0,
		{
			{ 96, 32 },
			{ 112, 32 },
			{ 128, 32 },
			{ 144, 32 },
			{ 160, 32 },
			{ 176, 32 },
			{ 192, 32 },
			{ 208, 32 },
			{ 224, 32 },
			{ 240, 32 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_level16Sheet11 = { 1, 128, 255, 0, { { 0, 0 }, { 128, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level16Sheet12 = { 2, 64, 255, 0, { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level16Sheet13 = { 3, 128, 172, 0, { { 0, 0 }, { 128, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level16Sheet14 = { 4, 64, 172, 0, { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level16Sheet15 = { 4, 64, 64, 0, { { 64, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_level16Sheet10 = { 1, 128, 128, 0, { { 0, 0 }, { 0, 128 }, { 128, 0 }, { 128, 128 } } };
	SpriteSheet g_level16Sheet21 = { 2, 128, 128, 0, { { 0, 0 }, { 0, 128 }, { 128, 0 }, { 128, 128 } } };
	SpriteSheet g_level16Sheet16 = {
		0,
		64,
		64,
		0,
		{
			{ 0, 0 },
			{ 64, 0 },
			{ 0, 64 },
			{ 64, 64 },
			{ 0, 128 },
			{ 64, 128 },
			{ 0, 192 },
			{ 64, 192 },
		},
	};
	SpriteSheet g_level16Sheet17 = {
		5,
		64,
		64,
		0,
		{
			{ 0, 0 },
			{ 64, 0 },
			{ 0, 64 },
			{ 64, 64 },
			{ 0, 128 },
			{ 64, 128 },
			{ 0, 192 },
			{ 64, 192 },
		},
	};
	SpriteSheet g_level16Sheet18 = {
		6,
		64,
		64,
		0,
		{
			{ 0, 0 },
			{ 64, 0 },
			{ 0, 64 },
			{ 64, 64 },
			{ 0, 128 },
			{ 64, 128 },
			{ 0, 192 },
			{ 64, 192 },
		},
	};
	SpriteSheet g_level16Sheet19 = {
		7,
		64,
		64,
		0,
		{
			{ 0, 0 },
			{ 64, 0 },
			{ 0, 64 },
			{ 64, 64 },
			{ 0, 128 },
			{ 64, 128 },
			{ 0, 192 },
			{ 64, 192 },
		},
	};
	SpriteSheet g_level16Sheet20 = {
		8,
		64,
		64,
		0,
		{
			{ 0, 0 },
			{ 64, 0 },
			{ 0, 64 },
			{ 64, 64 },
			{ 0, 128 },
			{ 64, 128 },
			{ 0, 192 },
			{ 64, 192 },
		},
	};

	SpriteSheet* g_level16Sheets[22] = {
		&g_level16Sheet1,  &g_level16Sheet2,  &g_level16Sheet3,	 &g_level16Sheet4,	&g_level16Sheet5,  &g_level16Sheet6,  &g_level16Sheet7,	 &g_level16Sheet8,
		&g_level16Sheet9,  &g_level16Sheet10, &g_level16Sheet11, &g_level16Sheet12, &g_level16Sheet13, &g_level16Sheet14, &g_level16Sheet15, &g_level16Sheet16,
		&g_level16Sheet17, &g_level16Sheet18, &g_level16Sheet19, &g_level16Sheet20, &g_level16Sheet21, SPRITE_SHEET_END,
	};

	SpriteSheet* g_level17Sheets[1] = { SPRITE_SHEET_END };
	SpriteSheet* g_level18Sheets[1] = { SPRITE_SHEET_END };
	SpriteSheet* g_level19Sheets[1] = { SPRITE_SHEET_END };

	// GLOBAL: TOY2 0x004F7284
	SpriteSheet** g_levelSpriteSheets[20] = {
		g_level0Sheets,	 g_level1Sheets,  g_level2Sheets,  g_level3Sheets,	g_level4Sheets,	 g_level5Sheets,  g_level6Sheets,
		g_level7Sheets,	 g_level8Sheets,  g_level9Sheets,  g_level10Sheets, g_level11Sheets, g_level12Sheets, g_level13Sheets,
		g_level14Sheets, g_level15Sheets, g_level16Sheets, g_level17Sheets, g_level18Sheets, g_level19Sheets,
	};

	SpriteSheet g_commonSheet1 = {
		31,
		15,
		15,
		0,
		{
			{ 0, 0 },
			{ 0, 0 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_commonSheet2 = {
		31,
		15,
		15,
		0,
		{
			{ 0, 0 },
			{ 16, 0 },
			{ 32, 0 },
			{ 48, 0 },
			{ 0, 16 },
			{ 16, 16 },
			{ 32, 16 },
			{ 48, 16 },
		},
	};

	SpriteSheet g_commonSheet25 = {
		31,
		15,
		15,
		0,
		{
			{ 0, 32 },
			{ 16, 32 },
			{ 32, 32 },
			{ 48, 32 },
			{ 0, 48 },
			{ 16, 48 },
			{ 0, 0 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_commonSheet3 = {
		31,
		15,
		15,
		0,
		{
			{ 0, 96 },
			{ 16, 96 },
			{ 32, 96 },
			{ 48, 96 },
		},

	};
	SpriteSheet g_commonSheet4 = {
		31,
		16,
		16,
		0,
		{
			{ 64, 0 },
			{ 80, 0 },
			{ 96, 0 },
			{ 112, 0 },
			{ 64, 16 },
			{ 80, 16 },
			{ 96, 16 },
			{ 112, 16 },
			{ 64, 32 },
			{ 80, 32 },
			{ 96, 32 },
			{ 0, 0 },
		},
	};

	SpriteSheet g_commonSheet5 = { 31, 31, 31, 0, { { 96, 96 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet6 = { 31, 31, 31, 0, { { 128, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet7 = { 31, 1, 1, 0, { { 112, 112 }, { 240, 80 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet8 = { 31, 32, 32, 0, { { 160, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet9 = { 31, 31, 31, 0, { { 160, 32 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet10 = { 31, 31, 8, 0, { { 128, 32 }, { 128, 41 }, { 128, 50 }, { 0, 0 } } };
	SpriteSheet g_commonSheet35 = { 31, 31, 15, 0, { { 96, 48 }, { 96, 48 }, { 96, 48 }, { 0, 0 } } };
	SpriteSheet g_commonSheet13 = { 31, 7, 32, 0, { { 216, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet14 = { 31, 31, 7, 0, { { 224, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet17 = {
		31,
		31,
		31,
		0,
		{
			{ 0, 192 },
			{ 32, 192 },
			{ 0, 224 },
			{ 32, 224 },
			{ 128, 192 },
			{ 160, 192 },
			{ 128, 224 },
			{ 160, 224 },
			{ 192, 192 },
			{ 224, 192 },
			{ 192, 224 },
			{ 224, 224 },
		},
	};

	SpriteSheet g_commonSheet18 = { 31, 31, 31, 0, { { 64, 64 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet19 = { 31, 16, 8, 0, { { 32, 88 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet20 = { 31, 31, 31, 0, { { 192, 96 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet22 = { 31, 31, 31, 0, { { 192, 64 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet23 = { 31, 31, 31, 0, { { 224, 64 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet24 = { 31, 31, 31, 0, { { 64, 96 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet26 = { 31, 11, 11, 0, { { 96, 64 }, { 108, 64 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet27 = { 31, 40, 32, 0, { { 0, 128 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet28 = { 31, 32, 32, 0, { { 64, 128 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet29 = { 31, 31, 19, 0, { { 224, 96 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet30 = { 31, 8, 8, 0, { { 64, 128 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet31 = { 31, 31, 39, 0, { { 128, 128 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet32 = { 31, 15, 15, 0, { { 80, 48 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet33 = { 31, 32, 32, 0, { { 192, 128 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet34 = { 31, 15, 16, 0, { { 112, 32 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet36 = { 31, 31, 16, 0, { { 160, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet39 = { 31, 16, 16, 0, { { 64, 224 }, { 80, 224 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet40 = { 31, 31, 31, 0, { { 0, 160 }, { 0, 0 } } };

	SpriteSheet g_commonSheet21 = {
		32,
		16,
		16,
		0,
		{
			{ 0, 0 },	{ 16, 0 },	{ 32, 0 },	{ 48, 0 },	 { 64, 0 },	 { 80, 0 },	 { 96, 0 },	 { 112, 0 },  { 0, 16 },  { 16, 16 }, { 32, 16 }, { 48, 16 },
			{ 64, 16 }, { 80, 16 }, { 96, 16 }, { 112, 16 }, { 0, 32 },	 { 16, 32 }, { 32, 32 }, { 48, 32 },  { 64, 32 }, { 80, 32 }, { 96, 32 }, { 112, 32 },
			{ 0, 48 },	{ 16, 48 }, { 32, 48 }, { 48, 48 },	 { 64, 48 }, { 80, 48 }, { 96, 48 }, { 112, 48 }, { 0, 64 },  { 16, 64 }, { 32, 64 }, { 48, 64 },
			{ 64, 64 }, { 80, 64 }, { 96, 64 }, { 112, 64 }, { 0, 80 },	 { 16, 80 }, { 32, 80 }, { 48, 80 },  { 64, 80 }, { 80, 80 }, { 96, 80 }, { 112, 80 },
			{ 0, 96 },	{ 16, 96 }, { 32, 96 }, { 48, 96 },	 { 64, 96 }, { 80, 96 }, { 96, 96 }, { 112, 96 },
		},
	};

	SpriteSheet g_commonSheet11 = { 32, 64, 64, 0, { { 128, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet12 = { 32, 48, 64, 0, { { 192, 32 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet15 = { 32, 64, 32, 0, { { 192, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet16 = { 32, 64, 32, 0, { { 128, 64 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet37 = { 32, 64, 62, 0, { { 128, 96 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };
	SpriteSheet g_commonSheet38 = { 32, 64, 62, 0, { { 192, 100 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };

	// GLOBAL: TOY2 0x004EC0A4
	SpriteSheet* g_commonSpriteSheets[50] = {
		&g_commonSheet1,
		&g_commonSheet2,
		&g_commonSheet3,
		&g_commonSheet4,
		&g_commonSheet5,
		&g_commonSheet6,
		&g_commonSheet7,
		&g_commonSheet8,
		&g_commonSheet9,
		&g_commonSheet10,
		&g_commonSheet11,
		&g_commonSheet12,
		&g_commonSheet13,
		&g_commonSheet14,
		&g_commonSheet15,
		&g_commonSheet16,
		&g_commonSheet17,
		&g_commonSheet18,
		&g_commonSheet19,
		&g_commonSheet20,
		&g_commonSheet21,
		&g_commonSheet22,
		&g_commonSheet23,
		&g_commonSheet24,
		&g_commonSheet25,
		&g_commonSheet26,
		&g_commonSheet27,
		&g_commonSheet28,
		&g_commonSheet29,
		&g_commonSheet30,
		&g_commonSheet31,
		&g_commonSheet32,
		&g_commonSheet33,
		&g_commonSheet34,
		&g_commonSheet35,
		&g_commonSheet36,
		&g_commonSheet37,
		&g_commonSheet38,
		&g_commonSheet39,
		&g_commonSheet40,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
	};

	// FUNCTION: TOY2 0x00447D40
	void InitSpriteSheets()
	{
		uint32_t levelSheetBytes = 0;

		SpriteSheet** sheet = g_levelSpriteSheets[Toy2::g_levelFileIndex];
		SpriteSheet** curSheet = sheet + 1;
		SpriteSheet* nextSheet;

		if (*sheet != SPRITE_SHEET_END)
		{
			do
			{
				nextSheet = *curSheet;
				levelSheetBytes += 4;
				++curSheet;

			} while (nextSheet != SPRITE_SHEET_END);
		}

		memset(g_spriteSheets, 0, 128 * sizeof(SpriteSheet*));
		g_spriteSheets[129] = &g_fallbackSpriteSheetObj;

		memcpy(g_spriteSheets, g_commonSpriteSheets, sizeof(SpriteSheet*) * 50);
		memcpy(&g_spriteSheets[50], sheet, levelSheetBytes);

		DECOMP_PRINT(("[Renderer::Sprite]: Initialised sprite sheets!\n"));
	}
}
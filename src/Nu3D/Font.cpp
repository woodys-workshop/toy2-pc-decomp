#include "Nu3D/Font.h"

namespace Nu3D
{
	// --------- Nu3D::Font ---------

	// GLOBAL: TOY2 0x00508674
	float g_scaledFontAscent = 1.0;

	// GLOBAL: TOY2 0x00884580
	int32_t g_textCursorX;

	// GLOBAL: TOY2 0x00884584
	int32_t g_textCursorY;

	// STUB: TOY2 0x004B3AD0
	int32_t Font::Init() { return 0; }

	// STUB: TOY2 0x004B4110
	Font* Font::Build(const char* fontName, int32_t fontSize, const char* charSet) { return 0; }

	// STUB: TOY2 0x004B3C60
	void Font::SetFont(Font* font) {}

	// STUB: TOY2 0x004B44D0
	void Font::SetTextColor(int32_t color) {}

	// STUB: TOY2 0x004B3980
	void Font::SetTextClipRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {}

	// STUB: TOY2 0x004B3C20
	void Font::BuildFontTextures() {}

	// FUNCTION: TOY2 0x004B4450
	void Font::SetTextCursor(int32_t x, int32_t y)
	{
		g_textCursorX = x;
		g_textCursorY = y;
	}

	// STUB: TOY2 0x004B38C0
	void Font::SetRenderFlags(int32_t option) {}

	// STUB: TOY2 0x004B38E0
	void Font::SetFontScale(float scaleX, float scaleY) {}

	// STUB: TOY2 0x004B39D0
	void Font::Destroy(Nu3D::Font* font) {}

	// STUB: TOY2 0x004B3A20
	Nu3D::Font* Font::BuildObject(int32_t charSetLength) { return 0; }

	// STUB: TOY2 0x004B3A90
	void Font::ClearList() {}

	// STUB: TOY2 0x004B3CC0
	BOOL Font::BuildTexResource(Nu3D::Font* font) { return 0; }

	// STUB: TOY2 0x004B3FD0
	void Font::ResetContext() {}

	// STUB: TOY2 0x004B4010
	HDC Font::CreateDC() { return 0; }

	// STUB: TOY2 0x004B4080
	HBITMAP Font::CreateAtlasBmp(int32_t width, int32_t height) { return 0; }

	// STUB: TOY2 0x004B45A0
	char* Font::DrawScaledTextString(char* string) { return 0; }

	// STUB: TOY2 0x004B46B0
	int32_t Font::DrawScaledGlyph(char charValue) { return 0; }

	// STUB: TOY2 0x004B4880
	int32_t Font::DrawClippedScaledGlyph(char charValue) { return 0; }

	// STUB: TOY2 0x004B4C10
	int32_t Font::ComputeScaledCharClip(char charValue) { return 0; }

	// STUB: TOY2 0x004B4CD0
	char* Font::DrawTextString(char* string) { return 0; }

	// STUB: TOY2 0x004B4DE0
	int32_t Font::DrawUnscaledGlyph(char charValue) { return 0; }

	// STUB: TOY2 0x004B4FA0
	int32_t Font::DrawClippedUnscaledGlyph(char charValue) { return 0; }

	// STUB: TOY2 0x004B5310
	int32_t Font::ComputeUnscaledCharClip(char charValue) { return 0; }

	// STUB: TOY2 0x004B53D0
	int32_t Font::CalculateScaledTextSize(char* string) { return 0; }

	// STUB: TOY2 0x004B5480
	int32_t Font::CalculateUnscaledTextSize(char* string) { return 0; }
}
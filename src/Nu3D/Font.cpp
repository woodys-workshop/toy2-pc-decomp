#include "Nu3D/Font.h"
#include "Nu3D/Nu3D.h"
#include "Nu3D/BmpDataNode.h"
#include "DrawingDevice.h"
#include "Renderer/Renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace Nu3D
{
	// --------- Nu3D::Font ---------

	// GLOBAL: TOY2 0x00508674
	float g_scaledFontAscent = 1.0;

	// GLOBAL: TOY2 0x00884580
	int32_t g_textCursorX;

	// GLOBAL: TOY2 0x00884584
	int32_t g_textCursorY;

	// GLOBAL: TOY2 0x00884568
	int32_t g_unusedFontList;

	// GLOBAL: TOY2 0x00884574
	Font* g_selectedFont;

	// GLOBAL: TOY2 0x00884560
	int32_t g_selectedFontTexIndex;

	// GLOBAL: TOY2 0x00884588
	int32_t g_textOffsetX;

	// the two triangles of one glyph
	// GLOBAL: TOY2 0x00884490
	VertexTL g_upperFontVerts[3];

	// GLOBAL: TOY2 0x008844F0
	VertexTL g_lowerFontVerts[3];

	// GLOBAL: TOY2 0x00884570
	Font* g_fontListHead;

	// GLOBAL: TOY2 0x0050866C
	float g_fontScaleFactor = 0.5;

	// GLOBAL: TOY2 0x00508668
	float g_fontWidthScale = 0.5;

	// GLOBAL: TOY2 0x00508670
	float g_scaledFontHeight = 1.0;

	// GLOBAL: TOY2 0x0088458C
	int32_t g_lineStartX;

	// GLOBAL: TOY2 0x00508664
	int32_t g_tabWidth = 20;

	// how far the next glyph sticks out of the clip rect, positive means it is clipped
	// GLOBAL: TOY2 0x00884550
	int32_t g_fontClipLeftAmount;

	// GLOBAL: TOY2 0x00884554
	int32_t g_fontClipRightAmount;

	// GLOBAL: TOY2 0x0088455C
	int32_t g_fontClipTopAmount;

	// GLOBAL: TOY2 0x00884564
	int32_t g_fontClipBottomAmount;

	// GLOBAL: TOY2 0x00884578
	HDC g_fontDC;

	// GLOBAL: TOY2 0x0088457C
	HBITMAP g_fontBitmap;

	// GLOBAL: TOY2 0x00884558
	HGDIOBJ g_prevFontDC;

	// GLOBAL: TOY2 0x0088456C
	int16_t g_fontIdCount;

	// GLOBAL: TOY2 0x00884590
	int32_t Font::g_clipRectTop;

	// GLOBAL: TOY2 0x0050865C
	int32_t Font::g_clipRectRight = 1000;

	// GLOBAL: TOY2 0x00508660
	int32_t Font::g_clipRectBottom = 1000;

	// GLOBAL: TOY2 0x00884598
	int32_t Font::g_renderFlags;

	// GLOBAL: TOY2 0x00884594
	int32_t Font::g_measuredTextHeight;

	// GLOBAL: TOY2 0x005086DC
	char* (*Font::DrawText)(char* string) = Font::DrawTextString;

	// GLOBAL: TOY2 0x005086E0
	int32_t (*Font::CalculateTextSize)(char* string) = Font::CalculateUnscaledTextSize;

	// FUNCTION: TOY2 0x004B3AD0
	int32_t Font::Init()
	{
		if (g_unusedFontList)
			ClearList();

		g_selectedFont = 0;
		g_selectedFontTexIndex = 0;

		g_textOffsetX = 0;
		g_textCursorX = 0;
		g_textCursorY = 0;

		g_upperFontVerts[0].position.z = 0.5;
		g_upperFontVerts[0].rhw = 2.0;
		g_upperFontVerts[0].diffuse.value = 0xFFFFFFFF;
		g_upperFontVerts[0].specular.value = 0xFFFFFFFF;
		g_upperFontVerts[1].position.z = 0.5;
		g_upperFontVerts[1].rhw = 2.0;
		g_upperFontVerts[1].diffuse.value = 0xFFFFFFFF;
		g_upperFontVerts[1].specular.value = 0xFFFFFFFF;
		g_upperFontVerts[2].position.z = 0.5;
		g_upperFontVerts[2].rhw = 2.0;
		g_upperFontVerts[2].diffuse.value = 0xFFFFFFFF;
		g_upperFontVerts[2].specular.value = 0xFFFFFFFF;

		g_lowerFontVerts[0].position.z = 0.5;
		g_lowerFontVerts[0].rhw = 2.0;
		g_lowerFontVerts[0].diffuse.value = 0xFFFFFFFF;
		g_lowerFontVerts[0].specular.value = 0xFFFFFFFF;
		g_lowerFontVerts[1].position.z = 0.5;
		g_lowerFontVerts[1].rhw = 2.0;
		g_lowerFontVerts[1].diffuse.value = 0xFFFFFFFF;
		g_lowerFontVerts[1].specular.value = 0xFFFFFFFF;
		g_lowerFontVerts[2].position.z = 0.5;
		g_lowerFontVerts[2].rhw = 2.0;
		g_lowerFontVerts[2].diffuse.value = 0xFFFFFFFF;
		g_lowerFontVerts[2].specular.value = 0xFFFFFFFF;

		SetFontScale(1.0, 1.0);
		SetRenderFlags(0);

		return 1;
	}

	// FUNCTION: TOY2 0x004B4110
	Font* Font::Build(const char* fontName, int32_t fontSize, const char* charSet)
	{
		uint32_t charSetLen;
		HFONT fontHandle;
		HDC dc;
		Font* font;
		int32_t testAtlasWidth;
		int32_t currentRowHeight;
		int32_t charIndex;
		int32_t currentX;
		int32_t currentY;
		int32_t atlasSize;
		HBITMAP fontBmp;
		Font* fontPtr;
		int32_t numChars;
		int32_t xPos;
		GlyphInfo* glyph;
		int32_t index;
		char glyphStr[2];
		Font* fontResult;
		int32_t charSetLenMinusOne;
		int32_t glyphRight;
		int32_t charCounter;
		HGDIOBJ oldObject;
		COLORREF textColor;
		COLORREF bkColor;
		HFONT fontHandleCopy;
		SIZE glyphSize;
		TEXTMETRIC textMetrics;
		int32_t testWidth;
		int32_t renderY;
		float atlasSizeFloat;

		strcpy(glyphStr, "?");

		fontResult = 0;

		if (! charSet)
			charSet = " 01234567890-=!\"\xA3$%^&*()_+ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz[]{};'#:@~,./<>?\\|";

		charSetLen = strlen(charSet) + 1;
		charSetLenMinusOne = charSetLen - 1;

		fontHandle = CreateFontA(fontSize, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, fontName);

		fontHandleCopy = fontHandle;

		if (! fontHandle)
			return 0;

		dc = CreateDC();

		if (dc)
		{
			font = BuildObject(charSetLen - 1);
			fontResult = font;

			if (font)
			{
				oldObject = SelectObject(dc, fontHandle);
				textColor = ::SetTextColor(dc, 0xFFFFFFu);
				bkColor = SetBkColor(dc, 0);

				GetTextMetricsA(dc, &textMetrics);

				strcpy(font->fontName, fontName);

				font->fontHeight = textMetrics.tmHeight;
				font->fontAscent = textMetrics.tmAscent;

				testAtlasWidth = 512;

				do
				{
					currentRowHeight = 1;
					testAtlasWidth >>= 1;
					charIndex = 0;
					testWidth = testAtlasWidth;
					currentX = 1;

					if (charSetLenMinusOne > 0)
					{
						do
						{
							glyphStr[0] = charSet[charIndex];
							GetTextExtentPoint32A(dc, glyphStr, 1, &glyphSize);

							if (glyphSize.cx + currentX + 1 >= testWidth)
							{
								currentX = 1;
								currentRowHeight += fontResult->fontHeight + 1;
							}

							currentX += glyphSize.cx + 1;
							++charIndex;
						} while (charIndex < charSetLenMinusOne);

						testAtlasWidth = testWidth;
					}

					currentY = fontResult->fontHeight + currentRowHeight + 1;

					for (atlasSize = 1; atlasSize < currentY; atlasSize *= 2)
						;
				} while (atlasSize < testAtlasWidth);

				fontBmp = CreateAtlasBmp(atlasSize, atlasSize);
				fontPtr = fontResult;
				numChars = charSetLenMinusOne;
				xPos = 1;

				g_fontBitmap = fontBmp;

				fontResult->bmpHandle = fontBmp;
				fontPtr->atlasWidth = atlasSize;
				fontPtr->atlasHeight = atlasSize;
				fontPtr->numGlyphs = numChars;

				renderY = 1;
				glyphRight = 1;
				charCounter = 0;

				if (numChars > 0)
				{
					glyph = &fontPtr->glyphs[0];
					atlasSizeFloat = (float)(atlasSize - 1);

					do
					{
						glyphStr[0] = charSet[charCounter];
						GetTextExtentPoint32A(dc, glyphStr, 1, &glyphSize);

						if (glyphSize.cx + xPos + 1 >= atlasSize)
						{
							xPos = 1;
							glyphRight = 1;
							renderY += fontResult->fontHeight + 1;
						}

						TextOutA(dc, xPos, renderY, glyphStr, 1);

						glyph->uvMinX = glyphRight / atlasSizeFloat;
						glyphRight = glyphSize.cx + xPos - 1;
						glyph->uvMaxX = glyphRight / atlasSizeFloat;
						glyph->uvMinY = renderY / atlasSizeFloat;
						glyphRight = glyphSize.cy + renderY - 1;
						glyph->uvMaxY = glyphRight / atlasSizeFloat;
						glyph->width = glyphSize.cx;
						glyph->height = glyphSize.cy;
						++glyph;

						xPos += glyphSize.cx + 1;
						numChars = charSetLenMinusOne;
						glyphRight = xPos;

						++charCounter;
					} while (charCounter < charSetLenMinusOne);

					fontPtr = fontResult;
				}

				for (index = 0; index < numChars; ++index)
					fontPtr->charToGlyphIndex[(uint8_t)charSet[index]] = index;

				SelectObject(dc, oldObject);
				::SetTextColor(dc, textColor);
				SetBkColor(dc, bkColor);

				fontHandle = fontHandleCopy;
				font = fontResult;
			}

			ResetContext();
			BuildTexResource(font);
		}

		DeleteObject(fontHandle);

		return fontResult;
	}

	// FUNCTION: TOY2 0x004B3C60
	void Font::SetFont(Font* font)
	{
		if (DrawingDevice::GetD3DDevice())
		{
			g_selectedFont = font;

			if (font)
			{
				g_selectedFontTexIndex = font->texIndex;
				g_scaledFontHeight = font->fontHeight * g_fontScaleFactor;
				g_scaledFontAscent = font->fontAscent * g_fontScaleFactor;
			}
			else
			{
				g_selectedFontTexIndex = 0;
			}
		}
	}

	// FUNCTION: TOY2 0x004B44D0
	void Font::SetTextColor(int32_t color)
	{
		g_upperFontVerts[0].diffuse.value = (color | 0xFF000000);
		g_upperFontVerts[0].specular.value = (color | 0xFF000000);
		g_upperFontVerts[1].diffuse.value = (color | 0xFF000000);
		g_upperFontVerts[1].specular.value = (color | 0xFF000000);
		g_upperFontVerts[2].diffuse.value = (color | 0xFF000000);
		g_upperFontVerts[2].specular.value = (color | 0xFF000000);

		g_lowerFontVerts[0].diffuse.value = (color | 0xFF000000);
		g_lowerFontVerts[0].specular.value = (color | 0xFF000000);
		g_lowerFontVerts[1].diffuse.value = (color | 0xFF000000);
		g_lowerFontVerts[1].specular.value = (color | 0xFF000000);
		g_lowerFontVerts[2].diffuse.value = (color | 0xFF000000);
		g_lowerFontVerts[2].specular.value = (color | 0xFF000000);
	}

	// FUNCTION: TOY2 0x004B3980
	void Font::SetTextClipRect(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
	{
		g_lineStartX = x1;
		g_clipRectTop = y1;
		g_clipRectRight = x2;
		g_clipRectBottom = y2;
	}

	// FUNCTION: TOY2 0x004B3C20
	void Font::BuildFontTextures()
	{
		Font* font;

		if (DrawingDevice::GetD3DDevice())
		{
			for (font = g_fontListHead; font; font = font->next)
				BuildTexResource(font);

			SetFont(g_selectedFont);
		}
	}

	// FUNCTION: TOY2 0x004B4450
	void Font::SetTextCursor(int32_t x, int32_t y)
	{
		g_textCursorX = x;
		g_textCursorY = y;
	}

	// FUNCTION: TOY2 0x004B38C0
	void Font::SetRenderFlags(int32_t option) { g_renderFlags = option != 0 ? 0x200 : 0; }

	// FUNCTION: TOY2 0x004B38E0
	void Font::SetFontScale(float scaleX, float scaleY)
	{
		g_fontWidthScale = scaleX;
		g_fontScaleFactor = scaleY;

		if (scaleX == 1.0 && scaleY == 1.0)
		{
			DrawText = DrawTextString;
			CalculateTextSize = CalculateUnscaledTextSize;
		}
		else
		{
			DrawText = DrawScaledTextString;
			CalculateTextSize = CalculateScaledTextSize;
		}

		if (g_selectedFont)
		{
			g_scaledFontHeight = g_selectedFont->fontHeight * scaleY;
			g_scaledFontAscent = g_selectedFont->fontAscent * scaleY;
		}
	}

	// FUNCTION: TOY2 0x004B39D0
	void Font::Destroy(Font* font)
	{
		Font* prev;

		if (font->texIndex)
			ReleaseBmpDataNode_T((BmpDataNode*)font->texIndex); // Doesn't make sense

		if (font->next)
			font->next->prev = font->prev;

		prev = font->prev;

		if (prev)
			prev->next = font->next;
		else
			g_fontListHead = font->next;

		free(font);
	}

	// FUNCTION: TOY2 0x004B3A20
	Font* Font::BuildObject(int32_t charSetLength)
	{
		Font* font;

		font = (Font*)malloc(20 * charSetLength + 304);

		if (font)
		{
			memset(font, 0, 20 * charSetLength + 304);

			font->next = g_fontListHead;

			if (g_fontListHead)
				g_fontListHead->prev = font;

			g_fontListHead = font;

			font->unkInt1 = 0;
			font->fontId = g_fontIdCount++;
		}

		return font;
	}

	// FUNCTION: TOY2 0x004B3A90
	void Font::ClearList()
	{
		Font* head;

		if (g_unusedFontList)
		{
			head = g_fontListHead;

			for (g_unusedFontList = 0; g_fontListHead; g_selectedFontTexIndex = 0)
			{
				Destroy(head);
				head = g_fontListHead;
				g_selectedFont = 0;
			}
		}
	}

	// FUNCTION: TOY2 0x004B3CC0
	BOOL Font::BuildTexResource(Font* font)
	{
		HBITMAP bmpHandle;
		int32_t texIndex;
		BOOL result;
		char texName[20];

		bmpHandle = font->bmpHandle;
		font->texIndex = 0;

		result = 0;

		if (bmpHandle)
		{
			sprintf(texName, "FONT_%04d", font->fontId);
			texIndex = CreateTextureResource(font->bmpHandle, texName, 4);

			font->texIndex = texIndex;

			if (texIndex)
				return 1;
		}

		return result;
	}

	// FUNCTION: TOY2 0x004B3FD0
	void Font::ResetContext()
	{
		HDC dc;

		dc = g_fontDC;

		if (g_fontDC)
		{
			if (g_fontBitmap)
			{
				SelectObject(g_fontDC, g_prevFontDC);
				dc = g_fontDC;
			}
			DeleteDC(dc);
		}

		g_fontDC = 0;
		g_fontBitmap = 0;
	}

	// FUNCTION: TOY2 0x004B4010
	HDC Font::CreateDC()
	{
		HDC hdc;
		LPDIRECTDRAWSURFACE4 surface;

		if (g_fontDC || g_fontBitmap)
			ResetContext();

		DrawingDevice::GetSlotSurfaceByIndex(0, &surface);

		if (! DrawingDevice::GetDC(surface, &hdc))
		{
			g_fontDC = CreateCompatibleDC(hdc);
			DrawingDevice::ReleaseDC(surface, hdc);
		}

		return g_fontDC;
	}

	// FUNCTION: TOY2 0x004B4080
	HBITMAP Font::CreateAtlasBmp(int32_t width, int32_t height)
	{
		HBITMAP result;
		HBITMAP bmpHandle;
		BITMAPINFO bmpInfo;
		void* bits;

		result = 0;

		if (g_fontDC)
		{
			bmpInfo.bmiHeader.biWidth = width;
			bmpInfo.bmiHeader.biPlanes = 1;
			bmpInfo.bmiHeader.biXPelsPerMeter = 1;
			bmpInfo.bmiHeader.biYPelsPerMeter = 1;
			bmpInfo.bmiHeader.biSize = 40;
			bmpInfo.bmiHeader.biHeight = height;
			bmpInfo.bmiHeader.biBitCount = 24;
			bmpInfo.bmiHeader.biCompression = 0;
			bmpInfo.bmiHeader.biSizeImage = 0;
			bmpInfo.bmiHeader.biClrUsed = 0;
			bmpInfo.bmiHeader.biClrImportant = 0;

			bmpHandle = CreateDIBSection(0, &bmpInfo, 0, &bits, 0, 0);

			if (bmpHandle)
				g_prevFontDC = SelectObject(g_fontDC, bmpHandle);

			return bmpHandle;
		}

		return result;
	}

	// FUNCTION: TOY2 0x004B45A0
	char* Font::DrawScaledTextString(char* string)
	{
		char* textPtr;
		int32_t textCursorX;
		int32_t endPos;
		int32_t test;
		char ch;
		int32_t drawResult;
		int32_t previousCursorX;
		int32_t startCursorX;

		if (g_selectedFont)
		{
			textPtr = string;
			textCursorX = g_textCursorX;

			endPos = 0;
			test = 0;

			ch = *string;
			startCursorX = g_textCursorX;

			if (*string)
			{
				while (1)
				{
					switch (ch)
					{
						case '\t':

							previousCursorX = textCursorX;
							textCursorX = g_tabWidth + textCursorX - (g_tabWidth + textCursorX) % g_tabWidth;
							g_textCursorX = textCursorX;
							endPos += textCursorX - previousCursorX;
							break;
						case '\n':
							g_textCursorY += (int32_t)g_scaledFontHeight;
							textCursorX = startCursorX;
							goto LBL_FINISH_LINE;
						case '\r':
							g_textCursorY += (int32_t)g_scaledFontHeight;
							textCursorX = g_lineStartX;
						LBL_FINISH_LINE:
							g_textCursorX = textCursorX;
							if (test <= endPos)
								test = endPos;

							endPos = 0;
							break;
						default:
							if (ComputeScaledCharClip(ch))
								drawResult = DrawScaledGlyph(*textPtr);
							else
								drawResult = DrawClippedScaledGlyph(*textPtr);

							endPos += drawResult;
							textCursorX = drawResult + g_textCursorX;
							g_textCursorX += drawResult;
							break;
					}

					ch = *++textPtr;

					if (! ch)
						goto LBL_FINISH_TEXT;
				}
			}

			return (char*)endPos;
		}

		// $TODO: the original returns the string pointer when there is no font selected
		test = (int32_t)string;
		endPos = (int32_t)string;
	LBL_FINISH_TEXT:

		if (test <= endPos)
			return (char*)endPos;

		return (char*)test;
	}

	// FUNCTION: TOY2 0x004B46B0
	int32_t Font::DrawScaledGlyph(char charValue)
	{
		LPDIRECT3DDEVICE3 device;
		GlyphInfo* glyphInfo;
		int32_t height;
		double posY;
		float width;

		device = DrawingDevice::GetD3DDevice();

		if (! g_selectedFont || ! g_selectedFontTexIndex || ! device)
			return 0;

		glyphInfo = &g_selectedFont->glyphs[g_selectedFont->charToGlyphIndex[(uint8_t)charValue]];
		height = g_selectedFont->glyphs[g_selectedFont->charToGlyphIndex[(uint8_t)charValue]].height;
		width = g_selectedFont->glyphs[g_selectedFont->charToGlyphIndex[(uint8_t)charValue]].width * g_fontWidthScale;

		posY = g_textCursorY - g_scaledFontAscent;

		g_upperFontVerts[0].position.x = (float)(g_textCursorX + g_textOffsetX);
		g_upperFontVerts[0].position.y = posY;
		g_upperFontVerts[0].uv.x = glyphInfo->uvMinX;
		g_upperFontVerts[0].uv.y = glyphInfo->uvMinY;
		g_upperFontVerts[1].position.x = (g_textCursorX + g_textOffsetX - 1) + width;
		g_upperFontVerts[1].position.y = posY;
		g_upperFontVerts[1].uv.x = glyphInfo->uvMaxX;
		g_upperFontVerts[1].uv.y = glyphInfo->uvMinY;
		g_upperFontVerts[2].position.x = (g_textCursorX - 1) + width;
		g_upperFontVerts[2].position.y = posY - 1.0 + height * g_fontScaleFactor;
		g_upperFontVerts[2].uv.x = glyphInfo->uvMaxX;
		g_upperFontVerts[2].uv.y = glyphInfo->uvMaxY;

		g_lowerFontVerts[0].position.x = g_upperFontVerts[0].position.x;
		g_lowerFontVerts[0].position.z = g_upperFontVerts[0].position.z;
		g_lowerFontVerts[0].position.y = g_upperFontVerts[0].position.y;
		g_lowerFontVerts[0].uv = g_upperFontVerts[0].uv;

		memcpy(&g_lowerFontVerts[1], &g_upperFontVerts[2], sizeof(VertexTL));

		g_lowerFontVerts[2].position.x = (float)g_textCursorX;
		g_lowerFontVerts[2].position.y = g_upperFontVerts[2].position.y;
		g_lowerFontVerts[2].uv.x = glyphInfo->uvMinX;
		g_lowerFontVerts[2].uv.y = glyphInfo->uvMaxY;

		Renderer::DrawSingleTexturedTriangle(g_upperFontVerts, g_selectedFontTexIndex, g_renderFlags | 0x444);

		Renderer::DrawSingleTexturedTriangle(g_lowerFontVerts, g_selectedFontTexIndex, g_renderFlags | 0x444);

		return (int32_t)width;
	}

	// FUNCTION: TOY2 0x004B4880
	int32_t Font::DrawClippedScaledGlyph(char charValue)
	{
		LPDIRECT3DDEVICE3 device;
		int32_t glyphIndex;
		GlyphInfo* glyphInfo;
		int32_t clipTop;
		int32_t posXOffset;
		int32_t fontClipLeft;
		int32_t clipTopMax;
		float posYOffset;
		float height;
		float width;
		double clipTopAmount;
		double uvY;
		double clipBottom;

		device = DrawingDevice::GetD3DDevice();

		if (! g_selectedFont || ! g_selectedFontTexIndex || ! device)
			return 0;

		glyphIndex = g_selectedFont->charToGlyphIndex[(uint8_t)charValue];
		fontClipLeft = g_fontClipLeftAmount;
		glyphInfo = &g_selectedFont->glyphs[glyphIndex];
		width = g_selectedFont->glyphs[glyphIndex].width * g_fontWidthScale;
		height = g_selectedFont->glyphs[glyphIndex].height * g_fontScaleFactor;

		if (g_fontClipLeftAmount <= g_fontClipRightAmount)
			fontClipLeft = g_fontClipRightAmount;

		if (fontClipLeft < width)
		{
			clipTop = g_fontClipTopAmount;
			clipTopMax = g_fontClipTopAmount;

			if (g_fontClipTopAmount <= g_fontClipBottomAmount)
				clipTopMax = g_fontClipBottomAmount;

			if (clipTopMax < height)
			{
				posXOffset = g_textOffsetX + g_textCursorX;
				posYOffset = g_textCursorY - g_scaledFontAscent;

				if (g_fontClipLeftAmount > 0)
				{
					g_upperFontVerts[0].position.x = (float)(posXOffset + g_fontClipLeftAmount);
					g_upperFontVerts[0].uv.x = (g_selectedFont->glyphs[glyphIndex].uvMaxX - g_selectedFont->glyphs[glyphIndex].uvMinX)
							* (g_fontClipLeftAmount + g_fontWidthScale) / width
						+ g_selectedFont->glyphs[glyphIndex].uvMinX;

					g_lowerFontVerts[0].position.x = g_upperFontVerts[0].position.x;
					g_lowerFontVerts[0].uv.x = g_upperFontVerts[0].uv.x;
					clipTop = g_fontClipTopAmount;
					g_lowerFontVerts[2].uv.x = g_upperFontVerts[0].uv.x;
					g_lowerFontVerts[2].position.x = (float)(g_textCursorX + g_fontClipLeftAmount);
				}
				else
				{
					g_upperFontVerts[0].position.x = (float)posXOffset;
					g_lowerFontVerts[0].position.x = g_upperFontVerts[0].position.x;
					g_upperFontVerts[0].uv.x = glyphInfo->uvMinX;
					g_lowerFontVerts[0].uv.x = glyphInfo->uvMinX;
					g_lowerFontVerts[2].position.x = (float)g_textCursorX;
					g_lowerFontVerts[2].uv.x = glyphInfo->uvMinX;
				}

				if (g_fontClipRightAmount > 0)
				{
					g_upperFontVerts[1].position.x = (g_textOffsetX - g_fontClipRightAmount + g_textCursorX - 1) + width;

					g_upperFontVerts[1].uv.x = g_selectedFont->glyphs[glyphIndex].uvMaxX
						- (g_selectedFont->glyphs[glyphIndex].uvMaxX - g_selectedFont->glyphs[glyphIndex].uvMinX) * g_fontClipRightAmount / width;

					g_upperFontVerts[2].position.x = (g_textCursorX - g_fontClipRightAmount - 1) + width;
					g_upperFontVerts[2].uv.x = g_upperFontVerts[1].uv.x;
					g_lowerFontVerts[1].position.x = g_upperFontVerts[2].position.x;
					g_lowerFontVerts[1].uv.x = g_upperFontVerts[1].uv.x;
				}
				else
				{
					g_upperFontVerts[1].position.x = (posXOffset - 1) + width;
					g_upperFontVerts[1].uv.x = g_selectedFont->glyphs[glyphIndex].uvMaxX;
					g_upperFontVerts[2].position.x = (g_textCursorX - 1) + width;

					g_lowerFontVerts[1].position.x = g_upperFontVerts[2].position.x;
					g_upperFontVerts[2].uv.x = g_selectedFont->glyphs[glyphIndex].uvMaxX;
					g_lowerFontVerts[1].uv.x = g_selectedFont->glyphs[glyphIndex].uvMaxX;
				}
				if (clipTop > 0)
				{
					clipTopAmount = g_fontClipTopAmount;

					g_upperFontVerts[0].position.y = clipTopAmount + posYOffset;

					uvY = (g_selectedFont->glyphs[glyphIndex].uvMaxY - g_selectedFont->glyphs[glyphIndex].uvMinY) * (clipTopAmount + g_fontScaleFactor) / height
						+ g_selectedFont->glyphs[glyphIndex].uvMinY;

					g_upperFontVerts[1].position.y = g_upperFontVerts[0].position.y;
					g_lowerFontVerts[0].position.y = g_upperFontVerts[0].position.y;
					g_upperFontVerts[0].uv.y = uvY;
					g_upperFontVerts[1].uv.y = uvY;
					g_lowerFontVerts[0].uv.y = uvY;
				}
				else
				{
					g_upperFontVerts[0].position.y = g_textCursorY - g_scaledFontAscent;
					g_upperFontVerts[0].uv.y = g_selectedFont->glyphs[glyphIndex].uvMinY;
					g_upperFontVerts[1].position.y = g_upperFontVerts[0].position.y;

					g_lowerFontVerts[0].position.y = g_upperFontVerts[0].position.y;
					g_upperFontVerts[1].uv.y = g_selectedFont->glyphs[glyphIndex].uvMinY;
					g_lowerFontVerts[0].uv.y = glyphInfo->uvMinY;
				}
				if (g_fontClipBottomAmount > 0)
				{
					clipBottom = g_fontClipBottomAmount;

					g_upperFontVerts[2].position.y = posYOffset - 1.0 - clipBottom + g_scaledFontHeight;
					uvY = glyphInfo->uvMaxY - clipBottom * (glyphInfo->uvMaxY - glyphInfo->uvMinY) / height;

					g_lowerFontVerts[1].position.y = g_upperFontVerts[2].position.y;
					g_upperFontVerts[2].uv.y = uvY;
					g_lowerFontVerts[1].uv.y = uvY;
					g_lowerFontVerts[2].uv.y = uvY;
					g_lowerFontVerts[2].position.y = g_upperFontVerts[2].position.y;
				}
				else
				{
					g_upperFontVerts[2].position.y = posYOffset - 1.0 + g_scaledFontHeight;

					g_lowerFontVerts[1].position.y = g_upperFontVerts[2].position.y;
					g_upperFontVerts[2].uv.y = glyphInfo->uvMaxY;

					g_lowerFontVerts[2].position.y = g_upperFontVerts[2].position.y;
					g_lowerFontVerts[1].uv.y = glyphInfo->uvMaxY;
					g_lowerFontVerts[2].uv.y = glyphInfo->uvMaxY;
				}
				Renderer::DrawSingleTexturedTriangle(g_upperFontVerts, g_selectedFontTexIndex, 1092);
				Renderer::DrawSingleTexturedTriangle(g_lowerFontVerts, g_selectedFontTexIndex, 1092);
			}
		}

		return (int32_t)width;
	}

	// FUNCTION: TOY2 0x004B4C10
	int32_t Font::ComputeScaledCharClip(char charValue)
	{
		int32_t glyphIndex;

		glyphIndex = g_selectedFont->charToGlyphIndex[(uint8_t)charValue];

		g_fontClipLeftAmount = g_lineStartX - g_textCursorX;
		g_fontClipRightAmount = g_textCursorX + (int32_t)(g_selectedFont->glyphs[glyphIndex].width * g_fontWidthScale) - g_clipRectRight;

		g_fontClipTopAmount = g_clipRectTop - (int32_t)(g_textCursorY - g_scaledFontAscent);
		g_fontClipBottomAmount = g_textCursorY + (int32_t)(g_scaledFontHeight - g_scaledFontAscent) - g_clipRectBottom;

		return (g_lineStartX - g_textCursorX) & g_fontClipRightAmount & g_fontClipTopAmount & g_fontClipBottomAmount & 0x80000000;
	}

	// FUNCTION: TOY2 0x004B4CD0
	char* Font::DrawTextString(char* string)
	{
		char* textPtr;
		int32_t textCursorX;
		int32_t endPos;
		int32_t test;
		char ch;
		int32_t drawResult;
		int32_t previousCursorX;
		int32_t startCursorX;

		if (g_selectedFont)
		{
			textPtr = string;
			textCursorX = g_textCursorX;
			endPos = 0;
			test = 0;
			ch = *string;
			startCursorX = g_textCursorX;

			if (*string)
			{
				while (1)
				{
					switch (ch)
					{
						case 9:
							previousCursorX = textCursorX;
							textCursorX = g_tabWidth + textCursorX - (g_tabWidth + textCursorX) % g_tabWidth;
							g_textCursorX = textCursorX;
							endPos += textCursorX - previousCursorX;
							break;
						case 10:
							g_textCursorY += g_selectedFont->fontHeight;
							textCursorX = startCursorX;
							goto LBL_FINISH_LINE;
						case 13:
							g_textCursorY += (int32_t)g_scaledFontHeight;
							textCursorX = g_lineStartX;
						LBL_FINISH_LINE:
							g_textCursorX = textCursorX;
							if (test <= endPos)
								test = endPos;
							endPos = 0;
							break;
						default:
							if (ComputeUnscaledCharClip(ch))
								drawResult = DrawUnscaledGlyph(*textPtr);
							else
								drawResult = DrawClippedUnscaledGlyph(*textPtr);
							endPos += drawResult;
							textCursorX = drawResult + g_textCursorX;
							g_textCursorX += drawResult;
							break;
					}
					ch = *++textPtr;
					if (! ch)
						goto LBL_FINISH_TEXT;
				}
			}
			return (char*)endPos;
		}

		// $TODO: the original returns the string pointer when there is no font selected
		test = (int32_t)string;
		endPos = (int32_t)string;
	LBL_FINISH_TEXT:
		if (test <= endPos)
			return (char*)endPos;
		return (char*)test;
	}

	// FUNCTION: TOY2 0x004B4DE0
	int32_t Font::DrawUnscaledGlyph(char charValue)
	{
		LPDIRECT3DDEVICE3 device;
		GlyphInfo* glyphInfo;
		double posY;

		device = DrawingDevice::GetD3DDevice();
		if (! g_selectedFont || ! g_selectedFontTexIndex || ! device)
			return 0;
		glyphInfo = &g_selectedFont->glyphs[g_selectedFont->charToGlyphIndex[(uint8_t)charValue]];
		posY = (g_textCursorY - g_selectedFont->fontAscent);
		g_upperFontVerts[0].position.x = (float)(g_textOffsetX + g_textCursorX);
		g_upperFontVerts[0].position.y = posY;
		g_upperFontVerts[0].uv.x = glyphInfo->uvMinX;
		g_upperFontVerts[0].uv.y = glyphInfo->uvMinY;
		g_upperFontVerts[1].position.x = (float)(g_textOffsetX + glyphInfo->width + g_textCursorX - 1);
		g_upperFontVerts[1].position.y = posY;
		g_upperFontVerts[1].uv.x = glyphInfo->uvMaxX;
		g_upperFontVerts[1].uv.y = glyphInfo->uvMinY;
		g_upperFontVerts[2].position.x = (float)(glyphInfo->width + g_textCursorX - 1);
		g_upperFontVerts[2].position.y = glyphInfo->height + posY - 1.0;
		g_upperFontVerts[2].uv.x = glyphInfo->uvMaxX;
		g_upperFontVerts[2].uv.y = glyphInfo->uvMaxY;
		g_lowerFontVerts[0].position.x = g_upperFontVerts[0].position.x;
		g_lowerFontVerts[0].position.y = g_upperFontVerts[0].position.y;
		g_lowerFontVerts[0].position.z = g_upperFontVerts[0].position.z;
		g_lowerFontVerts[0].uv = g_upperFontVerts[0].uv;
		memcpy(&g_lowerFontVerts[1], &g_upperFontVerts[2], sizeof(VertexTL));
		g_lowerFontVerts[2].position.x = (float)g_textCursorX;
		g_lowerFontVerts[2].position.y = g_upperFontVerts[2].position.y;
		g_lowerFontVerts[2].uv.x = glyphInfo->uvMinX;
		g_lowerFontVerts[2].uv.y = glyphInfo->uvMaxY;
		Renderer::DrawSingleTexturedTriangle(g_upperFontVerts, g_selectedFontTexIndex, g_renderFlags | 0x444);
		Renderer::DrawSingleTexturedTriangle(g_lowerFontVerts, g_selectedFontTexIndex, g_renderFlags | 0x444);
		return glyphInfo->width;
	}

	// FUNCTION: TOY2 0x004B4FA0
	int32_t Font::DrawClippedUnscaledGlyph(char charValue)
	{
		LPDIRECT3DDEVICE3 device;
		GlyphInfo* glyphInfo;
		int32_t clipMax;
		float posY;
		double clipTopAmount;
		double uvY;
		double clipBottom;

		device = DrawingDevice::GetD3DDevice();
		if (! g_selectedFont || ! g_selectedFontTexIndex || ! device)
			return 0;
		glyphInfo = &g_selectedFont->glyphs[g_selectedFont->charToGlyphIndex[(uint8_t)charValue]];
		clipMax = g_fontClipLeftAmount;
		if (g_fontClipLeftAmount <= g_fontClipRightAmount)
			clipMax = g_fontClipRightAmount;
		if (clipMax < g_selectedFont->glyphs[g_selectedFont->charToGlyphIndex[(uint8_t)charValue]].width)
		{
			clipMax = g_fontClipBottomAmount;
			if (g_fontClipTopAmount > g_fontClipBottomAmount)
				clipMax = g_fontClipTopAmount;
			if (clipMax < g_selectedFont->glyphs[g_selectedFont->charToGlyphIndex[(uint8_t)charValue]].height)
			{
				posY = (float)(g_textCursorY - g_selectedFont->fontAscent);
				if (g_fontClipLeftAmount > 0)
				{
					g_upperFontVerts[0].position.x = (float)(g_fontClipLeftAmount + g_textOffsetX + g_textCursorX);
					g_upperFontVerts[0].uv.x = (glyphInfo->uvMaxX - glyphInfo->uvMinX) * g_fontClipLeftAmount / glyphInfo->width + glyphInfo->uvMinX;
					g_lowerFontVerts[0].position.x = g_upperFontVerts[0].position.x;
					g_lowerFontVerts[0].uv.x = g_upperFontVerts[0].uv.x;
					g_lowerFontVerts[2].position.x = (float)(g_textCursorX + g_fontClipLeftAmount);
					g_lowerFontVerts[2].uv.x = g_upperFontVerts[0].uv.x;
				}
				else
				{
					g_upperFontVerts[0].position.x = (float)(g_textOffsetX + g_textCursorX);
					g_lowerFontVerts[0].position.x = g_upperFontVerts[0].position.x;
					g_upperFontVerts[0].uv.x = glyphInfo->uvMinX;
					g_lowerFontVerts[0].uv.x = glyphInfo->uvMinX;
					g_lowerFontVerts[2].position.x = (float)g_textCursorX;
					g_lowerFontVerts[2].uv.x = glyphInfo->uvMinX;
				}
				if (g_fontClipRightAmount > 0)
				{
					g_upperFontVerts[1].position.x = (float)(g_textOffsetX + glyphInfo->width - g_fontClipRightAmount + g_textCursorX - 1);
					g_upperFontVerts[1].uv.x = glyphInfo->uvMaxX - (glyphInfo->uvMaxX - glyphInfo->uvMinX) * g_fontClipRightAmount / glyphInfo->width;
					g_upperFontVerts[2].position.x = (float)(glyphInfo->width - g_fontClipRightAmount + g_textCursorX - 1);
					g_upperFontVerts[2].uv.x = g_upperFontVerts[1].uv.x;
					g_lowerFontVerts[1].position.x = g_upperFontVerts[2].position.x;
					g_lowerFontVerts[1].uv.x = g_upperFontVerts[1].uv.x;
				}
				else
				{
					g_upperFontVerts[1].position.x = (float)(g_textOffsetX + glyphInfo->width + g_textCursorX - 1);
					g_upperFontVerts[1].uv.x = glyphInfo->uvMaxX;
					g_upperFontVerts[2].position.x = (float)(glyphInfo->width + g_textCursorX - 1);
					g_lowerFontVerts[1].position.x = g_upperFontVerts[2].position.x;
					g_upperFontVerts[2].uv.x = glyphInfo->uvMaxX;
					g_lowerFontVerts[1].uv.x = glyphInfo->uvMaxX;
				}
				if (g_fontClipTopAmount > 0)
				{
					clipTopAmount = g_fontClipTopAmount;
					g_upperFontVerts[0].position.y = clipTopAmount + posY;
					uvY = clipTopAmount * (glyphInfo->uvMaxY - glyphInfo->uvMinY) / glyphInfo->height + glyphInfo->uvMinY;
					g_upperFontVerts[1].position.y = g_upperFontVerts[0].position.y;
					g_lowerFontVerts[0].position.y = g_upperFontVerts[0].position.y;
					g_upperFontVerts[0].uv.y = uvY;
					g_upperFontVerts[1].uv.y = uvY;
					g_lowerFontVerts[0].uv.y = uvY;
				}
				else
				{
					g_upperFontVerts[0].position.y = posY;
					g_upperFontVerts[0].uv.y = glyphInfo->uvMinY;
					g_upperFontVerts[1].position.y = posY;
					g_upperFontVerts[1].uv.y = glyphInfo->uvMinY;
					g_lowerFontVerts[0].position.y = posY;
					g_lowerFontVerts[0].uv.y = glyphInfo->uvMinY;
				}
				if (g_fontClipBottomAmount > 0)
				{
					clipBottom = g_fontClipBottomAmount;
					g_upperFontVerts[2].position.y = glyphInfo->height + posY - 1.0 - clipBottom;
					uvY = glyphInfo->uvMaxY - clipBottom * (glyphInfo->uvMaxY - glyphInfo->uvMinY) / glyphInfo->height;
					g_lowerFontVerts[1].position.y = g_upperFontVerts[2].position.y;
					g_lowerFontVerts[2].position.y = g_upperFontVerts[2].position.y;
					g_upperFontVerts[2].uv.y = uvY;
					g_lowerFontVerts[1].uv.y = uvY;
					g_lowerFontVerts[2].uv.y = uvY;
				}
				else
				{
					g_upperFontVerts[2].position.y = glyphInfo->height + posY - 1.0;
					g_lowerFontVerts[1].position.y = g_upperFontVerts[2].position.y;
					g_upperFontVerts[2].uv.y = glyphInfo->uvMaxY;
					g_lowerFontVerts[2].position.y = g_upperFontVerts[2].position.y;
					g_lowerFontVerts[1].uv.y = glyphInfo->uvMaxY;
					g_lowerFontVerts[2].uv.y = glyphInfo->uvMaxY;
				}
				Renderer::DrawSingleTexturedTriangle(g_upperFontVerts, g_selectedFontTexIndex, 1092);
				Renderer::DrawSingleTexturedTriangle(g_lowerFontVerts, g_selectedFontTexIndex, 1092);
			}
		}
		return glyphInfo->width;
	}

	// FUNCTION: TOY2 0x004B5310
	int32_t Font::ComputeUnscaledCharClip(char charValue)
	{
		int32_t glyphIndex;

		glyphIndex = g_selectedFont->charToGlyphIndex[(uint8_t)charValue];
		g_fontClipLeftAmount = g_lineStartX - g_textCursorX;
		g_fontClipRightAmount = g_textCursorX + g_selectedFont->glyphs[glyphIndex].width - g_clipRectRight;
		g_fontClipTopAmount = g_clipRectTop + g_selectedFont->fontAscent - g_textCursorY;
		g_fontClipBottomAmount = g_textCursorY + g_selectedFont->fontHeight - g_selectedFont->fontAscent - g_clipRectBottom;
		return (g_lineStartX - g_textCursorX) & g_fontClipRightAmount & g_fontClipTopAmount & g_fontClipBottomAmount & 0x80000000;
	}

	// FUNCTION: TOY2 0x004B53D0
	int32_t Font::CalculateScaledTextSize(char* string)
	{
		int32_t longest;
		int32_t width;
		char* textPtr;
		uint8_t ch;

		if (! g_selectedFont)
		{
			// $TODO: the original returns the string pointer when there is no font selected
			longest = (int32_t)string;
			width = (int32_t)string;
			goto LABEL_13;
		}
		longest = 0;
		width = 0;
		textPtr = string;
		g_measuredTextHeight = (int32_t)g_scaledFontHeight;
		ch = *string;
		if (*string)
		{
			do
			{
				if (ch != 9)
				{
					if (ch == 10 || ch == 13)
					{
						if (longest <= width)
							longest = width;
						g_measuredTextHeight = (int32_t)(g_measuredTextHeight + g_scaledFontHeight);
						width = 0;
					}
					else
					{
						width += (int32_t)(g_selectedFont->glyphs[g_selectedFont->charToGlyphIndex[ch]].width * g_fontWidthScale);
					}
				}
				ch = *++textPtr;
			} while (ch);
		LABEL_13:
			if (longest > width)
				return longest;
		}
		return width;
	}

	// FUNCTION: TOY2 0x004B5480
	int32_t Font::CalculateUnscaledTextSize(char* string)
	{
		int32_t longest;
		int32_t width;
		char* textPtr;
		uint8_t ch;

		if (! g_selectedFont)
		{
			// $TODO: the original returns the string pointer when there is no font selected
			longest = (int32_t)string;
			width = (int32_t)string;
			goto LABEL_13;
		}
		longest = 0;
		width = 0;
		textPtr = string;
		g_measuredTextHeight = (int32_t)g_scaledFontHeight;
		ch = *string;
		if (*string)
		{
			do
			{
				if (ch != 9)
				{
					if (ch == 10 || ch == 13)
					{
						if (longest <= width)
							longest = width;
						width = 0;
						g_measuredTextHeight = (int32_t)(g_measuredTextHeight + g_scaledFontHeight);
					}
					else
					{
						width += g_selectedFont->glyphs[g_selectedFont->charToGlyphIndex[ch]].width;
					}
				}
				ch = *++textPtr;
			} while (ch);
		LABEL_13:
			if (longest > width)
				return longest;
		}
		return width;
	}
}

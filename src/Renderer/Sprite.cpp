#include "Renderer/Sprite.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderQueue.h"
#include "Renderer/SpriteSheets.h"
#include "Nu3D/Sprite.h"
#include "SoftwareRenderer.h"
#include "NGNLoader/NGNLoader.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Math.h"
#include "Nu3D/Viewport.h"
#include "DrawingDevice.h"
#include "Logger.h"

#include <STDIO.H>

namespace Renderer
{
	namespace Sprite
	{
		// GLOBAL: TOY2 0x005087E8
		float g_parallaxDepthZPos = 0.99989998;

		// GLOBAL: TOY2 0x00884AC0
		Nu3D::Sprite* g_queued2DSprite;

		// GLOBAL: TOY2 0x005086FC
		int32_t g_spriteBuffer2DCount = 2000;

		// GLOBAL: TOY2 0x00971F9C
		Nu3D::Sprite g_spriteBuffer2D[2000];

		// GLOBAL: TOY2 0x005087EC
		WORD g_2DSpriteIndices[4] = { 0, 1, 2, 3 };

		// FUNCTION: TOY2 0x004B68B0
		HRESULT Render2DSprite(Nu3D::Sprite* sprite)
		{
			float xPos = DrawingDevice::GetDestWidth() * sprite->position.x;
			float xOffset = DrawingDevice::GetDestWidth() * (sprite->width + sprite->position.x);
			float yPos = DrawingDevice::GetDestHeight() * sprite->position.y;
			float yOffset = DrawingDevice::GetDestHeight() * (sprite->height + sprite->position.y);

			int32_t renderFlags = sprite->renderFlags;

			int32_t flags;
			float zPos;

			if (renderFlags == RENDER_PARALLAX_BG)
			{
				zPos = g_parallaxDepthZPos;
				flags = RENDER_TEXTURE_WRAP_UV | RENDER_Z | RENDER_ZWRITE | RENDER_BILINEAR_FILTER | RENDER_CULL_NONE;
			}
			else
			{
				zPos = 0.0;
				flags = renderFlags | RENDER_Z | RENDER_ZWRITE | RENDER_CULL_NONE;
			}

			Nu3D::VertexTL vertexData[4];

			vertexData[3].position.x = xOffset;
			vertexData[3].position.y = yPos;
			vertexData[1].position.z = zPos;
			vertexData[3].position.z = zPos;
			vertexData[3].uv.y = sprite->uvTopRight.y;
			vertexData[3].uv.x = sprite->uvTopRight.x;
			vertexData[0].position.x = xPos;
			vertexData[0].uv.x = sprite->uvBottomLeft.x;
			vertexData[0].uv.y = sprite->uvBottomLeft.y;
			vertexData[0].position.y = yOffset;
			vertexData[0].position.z = zPos;
			vertexData[1].position.x = xPos;
			vertexData[1].position.y = yPos;
			vertexData[2].position.y = yOffset;
			vertexData[2].position.x = xOffset;
			vertexData[2].position.z = zPos;
			vertexData[1].uv.x = sprite->uvTopLeft.x;
			vertexData[1].uv.y = sprite->uvTopLeft.y;

			RGBA color = sprite->color;
			vertexData[2].uv.x = sprite->uvBottomRight.x;
			vertexData[1].diffuse = color;
			vertexData[1].rhw = 1.0;
			vertexData[3].diffuse = color;
			vertexData[3].rhw = 1.0;
			vertexData[0].diffuse = color;
			vertexData[0].rhw = 1.0;
			vertexData[2].uv.y = sprite->uvBottomRight.y;
			vertexData[2].diffuse = color;
			vertexData[2].rhw = 1.0;

			Renderer::InitRenderState(flags);
			SoftwareRenderer::g_unkE4D950 = 5;

			Renderer::BindTexture(sprite->textureIndex);

			SoftwareRenderer::g_viewportRect = &sprite->viewportRect;
			SoftwareRenderer::g_unk9F6008 = 1;

			return DrawingAPI::DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_0x1C4, vertexData, 4, g_2DSpriteIndices, 4, 24);
		}

		// FUNCTION: TOY2 0x004B8DD0
		void UpdateQueued2DRender(Nu3D::Sprite* sprite)
		{
			Nu3D::Sprite* queuedSprite = g_queued2DSprite;

			if ((sprite->renderFlags & (RENDER_PRESET_COLOR_OVERLAY | RENDER_PRESET_FADE_OVERLAY)) != 0 && g_queued2DSprite)
			{
				for (Nu3D::Sprite* idx = g_queued2DSprite->next; idx; idx = idx->next)
					queuedSprite = idx;

				sprite->next = 0;
				queuedSprite->next = sprite;
			}
			else
			{
				sprite->next = g_queued2DSprite;
				g_queued2DSprite = sprite;
			}

			int32_t renderFlags = sprite->renderFlags;

			if (renderFlags == RENDER_PRESET_COLOR_OVERLAY)
			{
				sprite->renderFlags = RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_CUSTOM;
			}
			else if (renderFlags == RENDER_PRESET_FADE_OVERLAY)
			{
				uint8_t blue = sprite->color.b;
				uint8_t green = sprite->color.g;

				if (blue == green && blue == sprite->color.r)
				{
					sprite->renderFlags = RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT;
					sprite->color.a = green;
					sprite->color.r = 0;
					sprite->color.g = 0;
					sprite->color.b = 0;
				}
				else
				{
					sprite->renderFlags = RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_ALT;
				}
			}
		}

		// FUNCTION: TOY2 0x004B8CC0
		void Queue2DSprite(
			float xPosition,
			float yPosition,
			float width,
			float height,
			Vector2F* uvTopLeft,
			Vector2F* uvBottomRight,
			int32_t textureIndex,
			RGBA color,
			int32_t flags)
		{
			if (g_spriteBuffer2DCount)
			{
				RGBA modulatedColor = ModulateColorByAlpha(color, flags);

				Nu3D::Sprite* sprite;

				if (flags == RENDER_PARALLAX_BG)
					sprite = &g_spriteBuffer2D[g_spriteBuffer2DCount];
				else
					sprite = &g_spriteBuffer2D[g_spriteBuffer2DCount--];

				sprite->position.x = xPosition;
				sprite->position.y = yPosition;
				sprite->width = width;
				sprite->type = RENDER_2D_SPRITE;
				sprite->height = height;
				sprite->uvTopLeft = *uvTopLeft;
				sprite->uvBottomLeft.x = uvTopLeft->x;
				sprite->uvBottomLeft.y = uvBottomRight->y;
				sprite->uvTopRight.x = uvBottomRight->x;
				sprite->uvTopRight.y = uvTopLeft->y;
				sprite->uvBottomRight = *uvBottomRight;
				sprite->textureIndex = textureIndex;

				if (flags != RENDER_PARALLAX_BG)
					modulatedColor = ApplyGammaCorrection(modulatedColor);

				sprite->color = modulatedColor;
				sprite->renderFlags = flags;

				Nu3D::Viewport::GetViewClipRect(&sprite->viewportRect);

				if (flags == RENDER_PARALLAX_BG)
					Render2DSprite(sprite);
				else
					UpdateQueued2DRender(sprite);
			}
			else
			{
				Logger::DebugLog("sprite buffer underrun");
			}
		}

		// FUNCTION: TOY2 0x004946A0
		int16_t DrawTiledFixed(int16_t xPos, int16_t yPos, int16_t sheetIndex, int16_t tileIndex)
		{
			SpriteSheet* sheet = g_spriteSheets[sheetIndex];

			if (! sheet)
				return 1;

			int32_t textureDataIndex = NGNLoader::GetTextureDataIndex(sheet->texIndex);

			Vector2F uvTopLeft;
			Vector2F uvBottomRight;

			if (textureDataIndex)
			{
				uint32_t bitmapWidth;
				uint32_t bitmapHeight;
				NGNLoader::RetrieveTextureData(textureDataIndex, &bitmapWidth, &bitmapHeight, 0, 0, 0);

				double width = bitmapWidth;
				float height = bitmapHeight;

				uvTopLeft.x = sheet->tiles[tileIndex].x / width;
				uvTopLeft.y = sheet->tiles[tileIndex].y / height;
				uvBottomRight.x = (sheet->tileWidth + sheet->tiles[tileIndex].x) / width;
				uvBottomRight.y = (sheet->tileHeight + sheet->tiles[tileIndex].y) / height;
			}

			double yScale = 1.0 / g_virtualScreenHeight;

			RGBA color;
			color.a = 0xFF;
			color.b = Nu3D::Camera::g_cameraTintBlue;
			color.g = Nu3D::Camera::g_cameraTintGreen;
			color.r = Nu3D::Camera::g_cameraTintRed;

			Queue2DSprite(
				xPos * 0.003125,
				yPos * yScale,
				sheet->tileWidth * 0.003125,
				sheet->tileHeight * yScale,
				&uvTopLeft,
				&uvBottomRight,
				textureDataIndex,
				color,
				RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT);

			return 1;
		}

		// FUNCTION: TOY2 0x00494820
		int16_t DrawTile(int16_t xPos, int16_t yPos, int16_t sheetIndex, int16_t tileIndex)
		{
			SpriteSheet* sheet = g_spriteSheets[sheetIndex];

			if (! sheet)
				return 1;

			int32_t textureDataIndex = NGNLoader::GetTextureDataIndex(sheet->texIndex);

			Vector2F uvTopLeft;
			Vector2F uvBottomRight;

			if (textureDataIndex)
			{
				uint32_t bitmapWidth;
				uint32_t bitmapHeight;
				NGNLoader::RetrieveTextureData(textureDataIndex, &bitmapWidth, &bitmapHeight, 0, 0, 0);

				double width = bitmapWidth;
				float height = bitmapHeight;

				uvTopLeft.x = sheet->tiles[tileIndex].x / width;
				uvTopLeft.y = sheet->tiles[tileIndex].y / height;
				uvBottomRight.x = (sheet->tileWidth + sheet->tiles[tileIndex].x) / width;
				uvBottomRight.y = (sheet->tileHeight + sheet->tiles[tileIndex].y) / height;
			}

			double yScale = 1.0 / g_virtualScreenHeight;

			RGBA color;
			color.a = 0xFF;
			color.b = Nu3D::Camera::g_cameraTintBlue;
			color.g = Nu3D::Camera::g_cameraTintGreen;
			color.r = Nu3D::Camera::g_cameraTintRed;
			float xScale = 1.0 / g_virtualScreenWidth;

			Queue2DSprite(
				xPos * xScale,
				yPos * yScale,
				sheet->tileWidth * xScale,
				sheet->tileHeight * yScale,
				&uvTopLeft,
				&uvBottomRight,
				textureDataIndex,
				color,
				RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT);

			return 1;
		}

		// FUNCTION: TOY2 0x00493DC0
		int16_t DrawColouredFixed(int16_t xPos, int16_t yPos, int16_t sheetIndex, int16_t tileIndex, uint8_t red, uint8_t green, uint8_t blue)
		{
			SpriteSheet* sheet = g_spriteSheets[sheetIndex];

			if (! sheet)
				return 1;

			int32_t textureDataIndex = NGNLoader::GetTextureDataIndex(sheet->texIndex);

			Vector2F uvTopLeft;
			Vector2F uvBottomRight;

			if (textureDataIndex)
			{
				uint32_t bitmapWidth;
				uint32_t bitmapHeight;
				NGNLoader::RetrieveTextureData(textureDataIndex, &bitmapWidth, &bitmapHeight, 0, 0, 0);

				double width = bitmapWidth;
				float height = bitmapHeight;

				uvTopLeft.x = sheet->tiles[tileIndex].x / width;
				uvTopLeft.y = sheet->tiles[tileIndex].y / height;
				uvBottomRight.x = (sheet->tileWidth + sheet->tiles[tileIndex].x) / width;
				uvBottomRight.y = (sheet->tileHeight + sheet->tiles[tileIndex].y) / height;
			}

			double yScale = 1.0 / g_virtualScreenHeight;

			// (the original really does swap red and blue)
			RGBA color;
			color.a = 0xFF;
			color.b = red;
			color.g = green;
			color.r = blue;

			Queue2DSprite(
				xPos * 0.003125,
				yPos * yScale,
				sheet->tileWidth * 0.003125,
				sheet->tileHeight * yScale,
				&uvTopLeft,
				&uvBottomRight,
				textureDataIndex,
				color,
				RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT);

			return 1;
		}

		// FUNCTION: TOY2 0x00493C30
		int16_t DrawColoured(int16_t xPos, int16_t yPos, int16_t sheetIndex, int16_t tileIndex, uint8_t blue, uint8_t green, uint8_t red)
		{
			SpriteSheet* sheet = g_spriteSheets[sheetIndex];

			if (! sheet)
				return 1;

			int32_t textureDataIndex = NGNLoader::GetTextureDataIndex(sheet->texIndex);

			Vector2F uvTopLeft;
			Vector2F uvBottomRight;

			if (textureDataIndex)
			{
				uint32_t bitmapWidth;
				uint32_t bitmapHeight;
				NGNLoader::RetrieveTextureData(textureDataIndex, &bitmapWidth, &bitmapHeight, 0, 0, 0);

				double width = bitmapWidth;
				float height = bitmapHeight;

				uvTopLeft.x = sheet->tiles[tileIndex].x / width;
				uvTopLeft.y = sheet->tiles[tileIndex].y / height;
				uvBottomRight.x = (sheet->tileWidth + sheet->tiles[tileIndex].x) / width;
				uvBottomRight.y = (sheet->tileHeight + sheet->tiles[tileIndex].y) / height;
			}

			double yScale = 1.0 / g_virtualScreenHeight;

			RGBA color;
			color.a = 0xFF;
			color.b = blue;
			color.g = green;
			color.r = red;
			float xScale = 1.0 / g_virtualScreenWidth;

			Queue2DSprite(
				xPos * xScale,
				yPos * yScale,
				sheet->tileWidth * xScale,
				sheet->tileHeight * yScale,
				&uvTopLeft,
				&uvBottomRight,
				textureDataIndex,
				color,
				RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT);

			return 1;
		}

		// FUNCTION: TOY2 0x0049D390
		void DrawBitmapText(char* text, int32_t screenY, int32_t screenX, uint8_t colorB, uint8_t colorG, uint8_t colorR)
		{
			// Draws centered text from the UI glyph texture. The glyphs are on an 8 wide grid of 32x32 tiles, the first
			// 26 are the letters, followed by the digits, a few symbols are placed by hand.

			int32_t textLength = strlen(text);
			int32_t texelX = screenX - 13 * textLength / 2;

			for (int32_t i = 0; i < textLength; i++, texelX += 13)
			{
				uint8_t character = text[i];
				int32_t glyphX;
				int32_t glyphY;

				switch (character)
				{
					case '!':
						glyphX = 96;
						glyphY = 160;
						break;

					case '\'':
						glyphX = 224;
						glyphY = 160;
						break;

					case '(':
						glyphX = 160;
						glyphY = 160;
						break;

					case ')':
						glyphX = 192;
						glyphY = 160;
						break;

					case ',':
						glyphX = 160;
						glyphY = 128;
						break;

					case '.':
						glyphX = 128;
						glyphY = 128;
						break;

					case '/':
						glyphX = 32;
						glyphY = 160;
						break;

					case ':':
						glyphX = 224;
						glyphY = 128;
						break;

					case ';':
						glyphX = 192;
						glyphY = 128;
						break;

					case '?':
						glyphX = 128;
						glyphY = 160;
						break;

					case '\\':
						glyphX = 0;
						glyphY = 160;
						break;

					case 0xB2:
						glyphX = 96;
						glyphY = 64;
						break;

					case 0xB3:
						glyphX = 112;
						glyphY = 64;
						break;

					case 0xB9:
						glyphX = 80;
						glyphY = 64;
						break;

					case 0xBA:
						glyphX = 64;
						glyphY = 64;
						break;

					default: {
						int32_t glyphIndex;

						if (character >= 'a' && character <= 'z')
							glyphIndex = character - 'a';
						else if (character >= 'A' && character <= 'Z')
							glyphIndex = character - 'A';
						else if (character >= '0' && character <= '9')
							glyphIndex = character - 22;
						else
							continue;

						glyphX = 32 * (glyphIndex % 8);
						glyphY = 32 * (glyphIndex / 8);
					}
				}

				Vector2F uvMin;
				Vector2F uvMax;

				uvMin.x = glyphX * 0.0039215689;
				uvMin.y = glyphY * 0.0039215689;
				uvMax.x = (glyphX + 31.0) * 0.0039215689;
				uvMax.y = (glyphY + 31.0) * 0.0039215689;

				int32_t fontTextureIndex = NGNLoader::GetTextureDataIndex(31);

				RGBA color;
				color.b = colorB;
				color.a = 0xFF;
				color.g = colorG;
				color.r = colorR;

				double invHeight = 1.0 / g_virtualScreenHeight;
				float invWidth = 1.0 / g_virtualScreenWidth;

				Queue2DSprite(
					texelX * invWidth,
					screenY * invHeight,
					invWidth * 16.0,
					invHeight * 16.0,
					&uvMin,
					&uvMax,
					fontTextureIndex,
					color,
					RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT);
			}
		}

		// FUNCTION: TOY2 0x0049D750
		void DrawWhiteText(char* text, int32_t screenY, int32_t screenX) { DrawBitmapText(text, screenY, screenX, 255, 255, 255); }

		// FUNCTION: TOY2 0x00493F40
		int16_t DrawScaled(
			int16_t xPos,
			int16_t yPos,
			int32_t sheetIndex,
			int32_t tileIndex,
			uint32_t red,
			uint32_t green,
			uint32_t blue,
			uint32_t flags,
			int32_t scaleX,
			int32_t scaleY)
		{
			SpriteSheet* sheet;

			if ((sheetIndex & 0x8000) == 0)
				sheet = g_spriteSheets[(int16_t)sheetIndex];
			else
				sheet = g_spriteSheets[129];

			if (! sheet)
				return 1;

			int32_t textureDataIndex = NGNLoader::GetTextureDataIndex(sheet->texIndex);

			Vector2F uvMin;
			Vector2F uvMax;

			if (textureDataIndex)
			{
				uint32_t bitmapWidth;
				uint32_t bitmapHeight;
				NGNLoader::RetrieveTextureData(textureDataIndex, &bitmapWidth, &bitmapHeight, 0, 0, 0);

				int16_t ti = (int16_t)tileIndex;

				double dW = (double)bitmapWidth;
				double dH = (double)bitmapHeight;

				uvMin.x = sheet->tiles[ti].x / dW;
				uvMin.y = sheet->tiles[ti].y / dH;

				uvMax.x = ((double)sheet->tileWidth + (double)sheet->tiles[ti].x) / dW;
				uvMax.y = ((double)sheet->tileHeight + (double)sheet->tiles[ti].y) / dH;
			}

			RGBA packedColor;
			packedColor.g = (uint8_t)green;
			packedColor.r = (uint8_t)red;
			packedColor.b = (uint8_t)blue;

			uint8_t* alphaPtr = &packedColor.a;

			if (! alphaPtr)
				alphaPtr = (uint8_t*)&red;

			int32_t blendMode = flags & 96;
			int32_t renderFlags;

			if (blendMode != 0)
			{
				if (blendMode == 32)
				{
					*alphaPtr = 255;
					renderFlags = RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_CUSTOM;
				}
				else if (blendMode == 64)
				{
					*alphaPtr = 255;
					renderFlags = RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_ALT;
				}
				else
				{
					*alphaPtr = 255 - (uint8_t)((flags >> 8) & 0xFF);
					renderFlags = RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT;
				}
			}
			else
			{
				*alphaPtr = 128;
				renderFlags = RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT;
			}

			float invHeight = 1.0 / g_virtualScreenHeight;
			float invWidth = 1.0 / g_virtualScreenWidth;

			Queue2DSprite(
				xPos * invWidth,
				yPos * invHeight,
				((scaleX * sheet->tileWidth) >> 12) * invWidth,
				((scaleY * sheet->tileHeight) >> 12) * invHeight,
				&uvMin,
				&uvMax,
				textureDataIndex,
				packedColor,
				renderFlags);

			return 1;
		}

		// FUNCTION: TOY2 0x004B6300
		void ResetQueue()
		{
			g_queued2DSprite = 0;
			g_spriteBuffer2DCount = 2000;
		}

		// FUNCTION: TOY2 0x004B8B10
		void InsertIntoBucket(Nu3D::Sprite* sprite)
		{
			Vector3F position;

			Nu3D::Math::GetPositionVector(&Nu3D::Camera::g_secondaryCamera.matrix, &position);
			Nu3D::Math::VertexSubtract(&position, &position, &sprite->position);

			position.x = position.x * position.x;
			position.y = position.y * position.y;
			position.z = position.z * position.z;

			float distanceSquared = position.z + position.y + position.x;

			if (distanceSquared < 1.0)
				distanceSquared = 1.0;

			Nu3D::Sprite* previous = 0;
			int32_t depth = 0;

			// the exponent of the float is the bucket, they get exponentially further apart
			int32_t bucket = ((*(uint32_t*)&distanceSquared >> 20) + 8) & 0xFF;

			sprite->distanceSquared = distanceSquared;

			Nu3D::Sprite* current = g_spriteBuckets[bucket];

			// sorted furthest first inside of a bucket
			while (current)
			{
				if (current->distanceSquared <= distanceSquared)
					break;

				previous = current;
				current = current->next;
				depth++;
			}

			sprite->next = current;

			if (previous)
				previous->next = sprite;
			else
				g_spriteBuckets[bucket] = sprite;

			if (g_maxBucketDepth <= depth)
				g_maxBucketDepth = depth;
		}

		// FUNCTION: TOY2 0x004B9100
		void QueueQuadSpriteFromVerts(Vector3F* verts, Vector2F* uvTopLeft, Vector2F* uvBottomRight, int32_t textureIndex, RGBA color, int32_t renderFlags)
		{
			if (g_spriteBufferCount)
			{
				RGBA modulatedColor = ModulateColorByAlpha(color, renderFlags);

				Nu3D::Sprite* sprite = &g_spriteBuffer[--g_spriteBufferCount];

				sprite->type = RENDER_QUAD_SPRITE_FROM_VERTS;
				sprite->position = verts[0];
				sprite->triVerts[0] = verts[1];
				sprite->triVerts[1] = verts[2];
				sprite->triVerts[2] = verts[3];
				sprite->uvBottomLeft.x = uvTopLeft->x;
				sprite->uvBottomLeft.y = uvBottomRight->y;
				sprite->uvTopLeft = *uvTopLeft;
				sprite->uvBottomRight = *uvBottomRight;
				sprite->uvTopRight.x = uvBottomRight->x;
				sprite->uvTopRight.y = uvTopLeft->y;
				sprite->textureIndex = textureIndex;
				sprite->color = ApplyGammaCorrection(modulatedColor);
				sprite->renderFlags = renderFlags;

				Nu3D::Viewport::GetViewClipRect(&sprite->viewportRect);

				InsertIntoBucket(sprite);
			}
			else
			{
				Logger::DebugLog("sprite buffer underrun");
			}
		}

		// FUNCTION: TOY2 0x004B8E60
		void QueueQuadSprite(
			Vector3F* position,
			int32_t trigIndex,
			float width,
			float height,
			Vector2F* uvTopLeft,
			Vector2F* uvBottomRight,
			int32_t textureIndex,
			RGBA color,
			int32_t renderFlags)
		{
			if (g_spriteBufferCount)
			{
				RGBA modulatedColor = ModulateColorByAlpha(color, renderFlags);

				Nu3D::Sprite* sprite = &g_spriteBuffer[--g_spriteBufferCount];

				sprite->type = RENDER_QUADSPRITE;
				sprite->position = *position;
				sprite->trigIndex = trigIndex;
				sprite->width = width;
				sprite->height = height;
				sprite->uvTopLeft = *uvTopLeft;
				sprite->uvBottomLeft.x = uvTopLeft->x;
				sprite->uvBottomLeft.y = uvBottomRight->y;
				sprite->uvTopRight.x = uvBottomRight->x;
				sprite->uvTopRight.y = uvTopLeft->y;
				sprite->uvBottomRight = *uvBottomRight;
				sprite->textureIndex = textureIndex;
				sprite->color = ApplyGammaCorrection(modulatedColor);
				sprite->renderFlags = renderFlags;

				Nu3D::Viewport::GetViewClipRect(&sprite->viewportRect);

				InsertIntoBucket(sprite);
			}
			else
			{
				Logger::DebugLog("sprite buffer underrun");
			}
		}

		// FUNCTION: TOY2 0x004B8A30
		void QueueGroundAlignedSprite(
			Vector3F* position,
			int32_t trigIndex,
			float width,
			float height,
			Vector2F* uvTopLeft,
			Vector2F* uvBottomRight,
			int32_t textureIndex,
			RGBA color,
			int32_t renderFlags)
		{
			if (g_spriteBufferCount)
			{
				RGBA modulatedColor = ModulateColorByAlpha(color, renderFlags);

				Nu3D::Sprite* sprite = &g_spriteBuffer[--g_spriteBufferCount];

				sprite->type = RENDER_GROUND_ALIGNED_SPRITE;
				sprite->position = *position;
				sprite->trigIndex = trigIndex;
				sprite->width = width;
				sprite->height = height;
				sprite->uvTopLeft = *uvTopLeft;
				sprite->uvBottomLeft.x = uvTopLeft->x;
				sprite->uvBottomLeft.y = uvBottomRight->y;
				sprite->uvTopRight.x = uvBottomRight->x;
				sprite->uvTopRight.y = uvTopLeft->y;
				sprite->uvBottomRight = *uvBottomRight;
				sprite->textureIndex = textureIndex;
				sprite->color = ApplyGammaCorrection(modulatedColor);
				sprite->renderFlags = renderFlags;

				Nu3D::Viewport::GetViewClipRect(&sprite->viewportRect);

				InsertIntoBucket(sprite);
			}
			else
			{
				Logger::DebugLog("sprite buffer underrun");
			}
		}

		// FUNCTION: TOY2 0x004B8460
		void DrawQueuedSprite()
		{
			DispatchCommand(g_queued2DSprite);
			ResetQueue();

			if (g_isSoftwareRendering)
			{
				SoftwareRenderer::UnkFunc31();
				SoftwareRenderer::UnkFunc32();
			}
		}

		// STUB: TOY2 0x00493A60
		int16_t DrawTinted(int16_t xPos, int16_t yPos, int32_t sheetIndex, int32_t tileIndex, int32_t red, int32_t green, int32_t blue, int16_t flags)
		{
			return 0;
		}

		// STUB: TOY2 0x004942D0
		int16_t DrawScaledFixed(
			int16_t xPos,
			int16_t yPos,
			int32_t sheetIndex,
			int32_t tileIndex,
			uint8_t red,
			uint8_t green,
			uint8_t blue,
			int16_t flags,
			int32_t scaleX,
			int32_t scaleY)
		{
			return 0;
		}

		// STUB: TOY2 0x004949B0
		void DrawClipped(int16_t xPos, int16_t yPos, int32_t clipLeft, int32_t clipRight, int16_t sheetIndex, int16_t tileIndex) {}

		// STUB: TOY2 0x0049D2D0
		void QueueSegment(Vector3I* start, Vector3I* delta, uint8_t b, uint8_t g, uint8_t r) {}

		// STUB: TOY2 0x0049D7A0
		void DrawBackdropTransition(int32_t* timer, int32_t* backdropIndex, int32_t cycleDuration) {}

		// FUNCTION: TOY2 0x004B8F40
		void QueueBillboardSprite(
			Vector3F* position,
			int32_t trigIndex,
			float width,
			float height,
			Vector2F* uvTopLeft,
			Vector2F* uvBottomRight,
			int32_t textureIndex,
			RGBA color,
			int32_t renderFlags)
		{
			if (g_spriteBufferCount)
			{
				RGBA modulatedColor = ModulateColorByAlpha(color, renderFlags);

				Nu3D::Sprite* sprite = &g_spriteBuffer[--g_spriteBufferCount];

				sprite->type = RENDER_BILLBOARD_SPRITE;
				sprite->position = *position;
				sprite->trigIndex = trigIndex;
				sprite->width = width;
				sprite->height = height;
				sprite->uvTopLeft = *uvTopLeft;
				sprite->uvBottomLeft.x = uvTopLeft->x;
				sprite->uvBottomLeft.y = uvBottomRight->y;
				sprite->uvTopRight.x = uvBottomRight->x;
				sprite->uvTopRight.y = uvTopLeft->y;
				sprite->uvBottomRight = *uvBottomRight;
				sprite->textureIndex = textureIndex;
				sprite->color = ApplyGammaCorrection(modulatedColor);
				sprite->renderFlags = renderFlags;

				Nu3D::Viewport::GetViewClipRect(&sprite->viewportRect);

				InsertIntoBucket(sprite);
			}
			else
			{
				Logger::DebugLog("sprite buffer underrun");
			}
		}

		// STUB: TOY2 0x004B9210
		void QueueType10(Vector3F* position, Vector3F* triVerts, RGBA color) {}

	}
}

#pragma once

#include "Common.h"
#include "Numerics.h"

namespace Renderer
{
	namespace Sprite
	{
		extern float g_parallaxDepthZPos;

		int16_t DrawScaled(int16_t xPos,
			int16_t yPos,
			int32_t sheetIndex,
			int32_t tileIndex,
			uint32_t red,
			uint32_t green,
			uint32_t blue,
			uint32_t flags,
			int32_t scaleX,
			int32_t scaleY);

		void Queue2DSprite(float xPosition,
			float yPosition,
			float width,
			float height,
			Vector2F* uvTopLeft,
			Vector2F* uvBottomRight,
			int32_t textureIndex,
			RGBA color,
			int32_t flags);

		int16_t DrawTiledFixed(int16_t xPos, int16_t yPos, int16_t sheetIndex, int16_t tileIndex);
		int16_t DrawTile(int16_t xPos, int16_t yPos, int16_t sheetIndex, int16_t tileIndex);
		int16_t DrawColouredFixed(int16_t xPos, int16_t yPos, int16_t sheetIndex, int16_t tileIndex, uint8_t red, uint8_t green, uint8_t blue);
		int16_t DrawColoured(int16_t xPos, int16_t yPos, int16_t sheetIndex, int16_t tileIndex, uint8_t blue, uint8_t green, uint8_t red);
		void DrawWhiteText(char *text, int32_t screenY, int32_t screenX);
		void DrawBitmapText(char* text, int32_t screenY, int32_t screenX, uint8_t colorB, uint8_t colorG, uint8_t colorR);

		void QueueQuadSprite(Vector3F* position,
			int32_t trigIndex,
			float width,
			float height,
			Vector2F* uvTopLeft,
			Vector2F* uvBottomRight,
			int32_t textureIndex,
			RGBA color,
			int32_t renderFlags);

		void QueueGroundAlignedSprite(Vector3F* position,
			int32_t trigIndex,
			float width,
			float height,
			Vector2F* uvTopLeft,
			Vector2F* uvBottomRight,
			int32_t textureIndex,
			RGBA color,
			int32_t renderFlags);

		void ResetQueue();
		void DrawQueuedSprite();
	}
}
#include "Toy2/Dialogue.h"
#include "Toy2/Toy2.h"
#include "Toy2/Camera.h"
#include "Toy2/Levels.h"
#include "Nu3D/Link.h"
#include "Nu3D/Math.h"
#include "Nu3D/Particles.h"
#include "Renderer/Renderer.h"
#include "AudioManager/AudioManager.h"
#include "InputManager.h"
#include "Numerics.h"

#include <string.h>
#include <stdlib.h>

namespace Toy2
{
	namespace Dialogue
	{
		// the colour of the subtitle text
		// GLOBAL: TOY2 0x0050A110
		int32_t g_unk50A110;

		// the text after WrapSubtitleText, rows of 36 characters with the spaces filled in
		// GLOBAL: TOY2 0x0050A298
		uint8_t g_subtitleWrapBuffer[540];

		// The script Cutscene::Update runs for a dialogue, Begin fills in the operands (the record, the actors and the camera angle).
		// 0: use record [1], 1: place Buzz at point 0, 1: place actor [6] at point 1, 2: Buzz faces [10], 2: actor [12] faces [13],
		// 4: eye point 2, 3: focus point -1 (half way), 10: start the camera, 7: move along the path until the end
		// GLOBAL: TOY2 0x004DF6CC
		int32_t g_dialogueScript[23] = { 0, 0, 1, -1, 0, 1, 1, 1, 2, -1, 0, 2, 1, 0, 4, 2, 3, -1, 10, 7, -1, 8, -1 };

		// the box opens with 0 to 4096 and closes with -4096 to 0
		// GLOBAL: TOY2 0x0050A4EC
		int32_t g_subtitleBoxAnim;

		// 0 is typing, 1 is a full page that waits for the jump button, 2 is the end of the text
		// GLOBAL: TOY2 0x0050A4C0
		int32_t g_subtitleMode;

		// GLOBAL: TOY2 0x0050A4CC
		int32_t g_subtitleTypeTimer;

		// GLOBAL: TOY2 0x0050A4C4
		int32_t g_subtitleColumn;

		// GLOBAL: TOY2 0x0050A114
		int32_t g_subtitleRowStart;

		// GLOBAL: TOY2 0x0050A530
		uint8_t* g_subtitleText;

		// GLOBAL: TOY2 0x0050A4D4
		int32_t g_subtitleFontPage;

		// GLOBAL: TOY2 0x0050A4D8
		int32_t g_subtitleScroll;

		// GLOBAL: TOY2 0x0050A518
		int32_t g_unk50A518;

		// two rows of 36 characters, the second byte is the font page
		// GLOBAL: TOY2 0x0050A1FC
		int16_t g_subtitleBuffer[72];

		// FUNCTION: TOY2 0x00401A00
		int32_t WrapSubtitleText(char* text)
		{
			uint8_t* out;
			char* src;
			char* cursor;
			char* scan;
			int32_t count;
			char ch;
			char* back;
			int32_t padding;
			int32_t column;
			int32_t result;
			uint8_t* padEnd;

			out = g_subtitleWrapBuffer;
			memset(g_subtitleWrapBuffer, 32, sizeof(g_subtitleWrapBuffer));
			src = text;

			for (cursor = text;; src = cursor)
			{
				scan = src;
				count = 0;

				// find where the 36 characters of the row end, a ^ does not take up a column
				do
				{
					ch = *scan;

					if (! *scan)
						goto LABEL_18;

					++scan;

					if (ch != 94)
						++count;
				} while (count < 36);

				if (*scan == 32)
				{
					cursor = scan + 1;

				LABEL_18:
					padding = 0;
					goto LABEL_19;
				}

				if (! *scan)
				{
					cursor = scan;
					goto LABEL_18;
				}

				// the row ends inside of a word, go back to the last space
				ch = *(scan - 1);

				for (back = scan - 1; ch != 32; --back)
				{
					if (ch != 94)
						--count;

					ch = *(back - 1);
				}

				cursor = back + 1;

				if (*back == 32)
				{
					do
					{
						ch = *(back - 1);
						--count;
						--back;
					} while (ch == 32);
				}

				padding = 35 - count;

			LABEL_19:
				column = 0;

				if (36 - padding > 0)
					break;

			LABEL_24:
				if (padding > 0)
				{
					memset(out, 0x20, 4 * (padding >> 2));
					padEnd = &out[4 * (padding >> 2)];
					out += padding;
					memset(padEnd, 32, padding & 3);
				}
			}

			while (1)
			{
				result = *src++;

				if (result == 94)
					--column;

				*out++ = result;

				if (! result)
					return result;

				if (++column >= 36 - padding)
					goto LABEL_24;
			}
		}

		// FUNCTION: TOY2 0x00401C30
		void UpdateSubtitleBox()
		{
			int32_t boxSize;
			int32_t sum;

			if (g_subtitleBoxAnim == 4096)
			{
				int32_t action = 1;
				int16_t buttons = InputManager::g_curButtonsPressed;

				if ((InputManager::g_curButtonsPressed & 0x8000) != 0 && (InputManager::g_prevButtonsPressed & 0x8000) == 0)
				{
					g_subtitleBoxAnim = -4096;
					AudioManager::PlaySoundEffect(62, 0);
					buttons = InputManager::g_curButtonsPressed;
				}

				if (g_subtitleMode)
				{
					if ((buttons & 0x4000) != 0 && (InputManager::g_prevButtonsPressed & 0x4000) == 0)
					{
						if (g_subtitleMode == 2)
						{
							g_subtitleBoxAnim = -4096;
							AudioManager::PlaySoundEffect(62, 0);
						}
						else
						{
							g_subtitleMode = 0;
							AudioManager::PlaySoundEffect(61, 0);
						}
					}
				}
				else
				{
					g_subtitleTypeTimer -= Renderer::g_frameDelta;

					if (g_subtitleTypeTimer <= 0)
					{
						// pressing jump types out the whole page
						if ((buttons & 0x4000) != 0 && (InputManager::g_prevButtonsPressed & 0x4000) == 0)
							action = 2;

						int32_t column = g_subtitleColumn;
						int32_t row = g_subtitleRowStart;
						uint8_t* text = g_subtitleText;
						int32_t fontPage = g_subtitleFontPage;
						g_subtitleTypeTimer = 2;

						while (1)
						{
							if (action == 1)
								action = 0;

							if (! column)
							{
								// a new line, clear it and scroll the box
								for (int32_t i = 0; i < 36; i++)
									g_subtitleBuffer[row + i] = 0x20;

								g_subtitleScroll += 36;

								if (g_subtitleScroll >= 72)
									g_subtitleScroll = 0;
							}

							int32_t nextChar = -1;

							// a ^ switches to the other font page
							do
							{
								int32_t curChar = *text++;
								g_subtitleText = text;

								if (curChar == 94)
								{
									fontPage ^= 0x100;
									g_subtitleFontPage = fontPage;
								}
								else
								{
									nextChar = curChar;
								}
							} while (nextChar < 0);

							g_subtitleBuffer[row + column++] = fontPage + nextChar;
							g_subtitleColumn = column;

							if (! *text)
								break;

							if (column >= 36)
							{
								row += 36;
								column = 0;
								g_subtitleColumn = 0;
								g_subtitleRowStart = row;

								if (row >= 72)
								{
									g_subtitleRowStart = 0;
									g_subtitleMode = 1;

									goto LABEL_33;
								}
							}

							if (! action)
								goto LABEL_33;
						}

						g_subtitleMode = 2;
					}
				}

			LABEL_33:
				if (Toy2::g_tickAccumulators.tick32Acc < 0x10 && g_subtitleMode)
				{
					int32_t length = 0;
					char* prompt = "press jump to continue";

					if (prompt[0])
					{
						while (prompt[++length])
							;
					}

					int32_t xPos = 4 * (40 - length);

					if (length > 0)
					{
						int32_t remainingChars = length;

						do
						{
							char promptChar = *prompt++;

							Renderer::DrawChar(xPos, 48, promptChar, 0xFF, 0xFF, 0xFF, 0);
							xPos += 8;
							--remainingChars;
						} while (remainingChars);
					}
				}

				int32_t index = g_subtitleScroll;
				int32_t charX = 16;
				int32_t charY = 32;

				if (g_subtitleScroll < 0)
					index = 0;

				int32_t remaining = 72;

				do
				{
					uint8_t red;

					if (g_subtitleBuffer[index] >> 8 == 1)
						red = 0;
					else
						red = 0x80;

					Renderer::DrawChar(charX, charY, (char)g_subtitleBuffer[index++], red, 0x80, 0, 0);

					if (index >= 72)
						index = 0;

					charX += 8;

					if (charX >= 304)
					{
						charX = 16;
						charY += 8;
					}

					--remaining;
				} while (remaining);

				boxSize = g_subtitleBoxAnim;

				goto LABEL_52;
			}

			if (g_subtitleBoxAnim < 0)
			{
				sum = (Renderer::g_frameDelta << 8) + g_subtitleBoxAnim;
				boxSize = sum;
				g_subtitleBoxAnim = boxSize;

				if (sum >= 0)
				{
					g_subtitleBoxAnim = 0;
					g_unk50A518 = 0;

					return;
				}

			LABEL_52:
				if (! boxSize)
					return;

				goto LABEL_53;
			}

			g_subtitleBoxAnim += Renderer::g_frameDelta << 8;
			boxSize = g_subtitleBoxAnim;

			if (g_subtitleBoxAnim < 4096)
				goto LABEL_52;

			boxSize = 4096;
			g_subtitleBoxAnim = 4096;

		LABEL_53:
			int32_t width = abs(474 * boxSize);
			int32_t height = abs(32 * boxSize);

			if (width < 0x4000)
				width = 0x4000;

			if (height < 0x2000)
				height = 0x2000;

			Renderer::DrawBlackBorderBox(257 - (width >> 13), 44 - (height >> 13), width, height, 0x80, 0, 0);
		}

		// FUNCTION: TOY2 0x004027F0
		void Begin(int32_t actorIdx, int32_t recordIdx, char* text, int32_t camAngle, int32_t camAngleValue, int32_t cinematicState)
		{
			int32_t angle = camAngle;
			int32_t angleValue;

			if (camAngle == -1)
			{
				angleValue =
					Nu3D::Math::CartesianToFixedAngle(
						32 * Levels::g_recordData[recordIdx]->data[0].x - 32 * Levels::g_recordData[recordIdx]->data[1].x,
						32 * Levels::g_recordData[recordIdx]->data[0].z - 32 * Levels::g_recordData[recordIdx]->data[1].z)
					& 0xFFF;
				angle = (angleValue - 2048) & 0xFFF;
			}
			else
			{
				angleValue = camAngleValue;
			}

			// the operands of the cutscene script
			g_dialogueScript[1] = recordIdx;
			g_dialogueScript[13] = angleValue;
			g_dialogueScript[6] = actorIdx;
			g_dialogueScript[12] = actorIdx;
			g_dialogueScript[10] = angle;

			if (! Renderer::g_enableCinematicBars)
			{
				if (Camera::g_cameraState)
				{
					if (Camera::g_unk50A538 != (Nu3D::Particles::ParticleInstance*)-1)
					{
						Camera::g_unk50A538->lifetime = 1;
						Camera::g_unk50A538 = (Nu3D::Particles::ParticleInstance*)-1;
					}

					if (Camera::g_unk50A4DC != (Nu3D::Particles::ParticleInstance*)-1)
					{
						Camera::g_unk50A4DC->lifetime = 1;
						Camera::g_unk50A4DC = (Nu3D::Particles::ParticleInstance*)-1;
					}

					g_buzzActor.actorFlags |= Actor::ACTOR_CUTSCENE_MOVE;
					Camera::g_gameplayCameraState.orbitYaw = g_buzzActor.posAngles.angles.yaw;
					g_buzzActor.facingAngle = g_buzzActor.posAngles.angles.yaw;
					Camera::g_gameplayCameraState.distance = 1200;
					Camera::g_gameplayCameraState.pos.y = g_buzzActor.posAngles.pos.y - 12288;
					Camera::g_gameplayCameraState.angles.pitch = 0;
					Camera::g_gameplayCameraState.targetPos.x = Camera::g_gameplayCameraState.pos.x;
					Camera::g_gameplayCameraState.unkShort6 = 0;

					Camera::g_cameraState = 0;

					Nu3D::Link::SetScaleFromFixedOffsets(45, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(46, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(47, 0, 0, 0);
				}

				if (cinematicState < 0)
					Renderer::g_enableCinematicBars = 1;
				else
					Renderer::g_enableCinematicBars = cinematicState + 10;

				Camera::g_inCutscene |= 1;
				Camera::g_unk50A4D0 = 0;
				Camera::g_unk50A294 = g_dialogueScript;
				Camera::g_unk50A134 = 0;
				Camera::g_unk50A53C = 0;
				Camera::g_unk50A144 = 0;
				Camera::g_unk50A0CC = 0;
				Camera::g_unk50A1F0 = 0;
				Camera::g_unk50A514 = 0;
				Camera::g_unk50A4BC = 0;
				Camera::g_unk50A0C8 = 0;

				if (text)
				{
					WrapSubtitleText(text);

					g_unk50A518 = 1;

					for (int32_t i = 0; i < 36; i++)
						((int32_t*)g_subtitleBuffer)[i] = 0x200020;

					g_subtitleText = g_subtitleWrapBuffer;
					g_subtitleColumn = 0;
					g_subtitleRowStart = 0;
					g_subtitleScroll = -72;
					g_unk50A110 = 0x808080;
					g_subtitleTypeTimer = 2;
					g_subtitleFontPage = 0;
					g_subtitleBoxAnim = 0;
					g_subtitleMode = 0;

					AudioManager::PlaySoundEffect(29, 0);
				}
				else
				{
					g_unk50A518 = 0;
				}
			}
		}
	}
}

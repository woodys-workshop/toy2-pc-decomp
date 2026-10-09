#include "Toy2/MainMenu.h"
#include "Toy2/Toy2.h"
#include "AudioManager/AudioManager.h"
#include "InputManager.h"
#include "Nu3D/Camera.h"
#include "SoftwareRenderer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Sprite.h"
#include "Nullsub.h"
#include "InputTypes.h"
#include "SaveManager.h"

#include <string.h>

namespace Toy2
{
	namespace MainMenu
	{
		// GLOBAL: TOY2 0x0053CA60
		int32_t g_fadeTimer;

		// GLOBAL: TOY2 0x0053E4A8
		int32_t g_nextScreen;

		// GLOBAL: TOY2 0x00559E84
		RGB32 g_menuClearColor;

		// FUNCTION: TOY2 0x00441980
		void RenderMenu()
		{
			Toy2::ProcessMiscEventsEx();

			RGBA color;
			color.a = -1;
			color.b = g_menuClearColor.r;
			color.g = g_menuClearColor.g;
			color.r = g_menuClearColor.b;

			Toy2::g_showBlackFrames = 1;
			Renderer::Sprite::g_parallaxDepthZPos = 0.80000001;
			Renderer::ClearScreen(color, 2);

			if (Renderer::BeginScene())
			{
				Renderer::ResetParallax();
				Renderer::RenderParallaxBackground(0);

				Renderer::DrawTintOverlay();

				Renderer::FlushRenderQueues();
				Renderer::Sprite::DrawQueuedSprite();
				DevDraw::DrawSlots();

				Renderer::DoFrameDelay(0);
				Renderer::EndScene(1);
			}
		}

		// FUNCTION: TOY2 0x00437C40
		int32_t Draw()
		{
			InputManager::g_curButtonsPressed = 0;
			InputManager::g_prevButtonsPressed = 0;

			g_fadeTimer = 0;
			g_nextScreen = 0;

			Nu3D::Camera::g_cameraTintBlue = 0;
			Nu3D::Camera::g_cameraTintGreen = 0;
			Nu3D::Camera::g_cameraTintRed = 0;

			Nu3D::Camera::SetTint(128, 128, 128, 12);
			SoftwareRenderer::UnkFunc67(0, 0);
			Renderer::g_frameDelta = 1;
			Toy2::SetBackdropByIndex(4);

			int32_t selectedY = 120;
			int32_t idleTimer = 0;
			int32_t cursorY = 120;
			bool cursorBelowTarget;

			AudioManager::PlayMusicOneShot(19);

			while (! g_nextScreen)
			{
			LBL_FRAME_START:

				Nu3D::Camera::FadeToTargetTint();

				uint32_t frameDelta = Renderer::g_frameDelta;

				if (g_fadeTimer > 0)
				{
					g_fadeTimer -= Renderer::g_frameDelta;

					if (g_fadeTimer <= 0)
						g_fadeTimer = 0;
				}

				idleTimer += Renderer::g_frameDelta;

				if (g_nextScreen)
				{
					cursorBelowTarget = cursorY < selectedY;
					goto LBL_ANIMATE_CURSOR;
				}

				cursorBelowTarget = cursorY < selectedY;

				if (cursorY != selectedY)
					goto LBL_ANIMATE_CURSOR;

				if ((InputManager::g_curButtonsPressed & INPUT_JUMP) == 0 || (InputManager::g_prevButtonsPressed & INPUT_JUMP) != 0 || idleTimer <= 30)
				{
					if ((InputManager::g_curButtonsPressed & INPUT_DOWN) != 0 && (InputManager::g_prevButtonsPressed & INPUT_DOWN) == 0 && selectedY < 200)
					{
						selectedY += 20;
						AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);

						frameDelta = Renderer::g_frameDelta;
					}

					if ((InputManager::g_curButtonsPressed & INPUT_UP) == 0 || (InputManager::g_prevButtonsPressed & INPUT_UP) != 0 || selectedY <= 120)
					{
						cursorBelowTarget = cursorY < selectedY;
						goto LBL_ANIMATE_CURSOR;
					}

					selectedY -= 20;

					AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
				}
				else
				{
					switch (cursorY)
					{
						case 120:
							g_nextScreen = 2;
							break;

						case 140:
							g_nextScreen = 3;
							break;

						case 160:
							g_nextScreen = 4;
							break;
						case 180:

							g_nextScreen = 5;
							break;

						case 200:
							exit(-1);
					}

					g_fadeTimer = 23;
					Nu3D::Camera::SetTint(0, 0, 0, 12);
					AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);
				}

				frameDelta = Renderer::g_frameDelta;
				cursorBelowTarget = cursorY < selectedY;

			LBL_ANIMATE_CURSOR:

				if (cursorBelowTarget)
				{
					cursorY += 2 * frameDelta;

					if (cursorY < selectedY)
						goto LBL_DRAW_FRAME;
				}
				else
				{
					cursorY -= 2 * frameDelta;

					if (cursorY > selectedY)
						goto LBL_DRAW_FRAME;
				}

				cursorY = selectedY;

			LBL_DRAW_FRAME:

				uint32_t bounce = idleTimer & 63;

				if (bounce > 31)
					bounce = 63 - bounce;

				uint32_t spriteScale = 4 * bounce;

				Renderer::Sprite::DrawScaled(64, cursorY, 62, 0, 4 * bounce, 4 * bounce, 128, 255, 2048, 2048);
				Renderer::Sprite::DrawScaled(240, cursorY, 62, 1, spriteScale, spriteScale, 128, 255, 2048, 2048);

				if (Toy2::g_saveLoaded)
					Renderer::DrawMainMenuText(120, "continue game", 128);
				else
					Renderer::DrawMainMenuText(120, "start game", 128);

				Renderer::DrawMainMenuText(140, "options", 128);
				Renderer::DrawMainMenuText(160, "load game", 128);
				Renderer::DrawMainMenuText(180, "movie viewer", 128);
				Renderer::DrawMainMenuText(200, "exit", 128);

				Nullsub3();

				RenderMenu();

				if (g_attractModeTimer >= 0 && ((InputManager::g_curButtonsPressed & INPUT_SECRET_MENU) != 0 || idleTimer >= g_attractModeTimer))
				{
					if (g_nextScreen)
						break;

					g_nextScreen = 1;
					g_fadeTimer = 23;

					Nu3D::Camera::SetTint(0, 0, 0, 12);
				}
			}

			if (g_fadeTimer)
				goto LBL_FRAME_START;

			AudioManager::StopAndWait();

			return g_nextScreen;
		}

		// FUNCTION: TOY2 0x00437FB0
		int32_t Tick()
		{
			if (g_mainMenuState != -1)
				goto LBL_INIT_TITLE_SCREEN;

			int32_t nextScreenResult;

			do
			{
				nextScreenResult = Draw();
				g_nextScreen = nextScreenResult;

				if (nextScreenResult != 1)
					break;

			LBL_INIT_TITLE_SCREEN:

				InputManager::g_curButtonsPressed = 0;
				InputManager::g_prevButtonsPressed = 0;

				g_fadeTimer = 0;
				g_nextScreen = 0;

				Nu3D::Camera::g_cameraTintBlue = 0;
				Nu3D::Camera::g_cameraTintGreen = 0;
				Nu3D::Camera::g_cameraTintRed = 0;

				Nu3D::Camera::SetTint(128, 128, 128, 12);
				SoftwareRenderer::UnkFunc67(0, 0);

				int32_t showPressJumpPrompt = 1;
				Renderer::g_frameDelta = 1;

				int32_t timeoutTicks;

				if (g_returnedToTitle == 1)
				{
					timeoutTicks = 300;
					showPressJumpPrompt = 0;
				}
				else
				{
					timeoutTicks = 900;
				}

				SetBackdropByIndex(0);

				int32_t elapsedTicks = 0;

				AudioManager::PlayMusicOneShot(20);

				while (! g_nextScreen)
				{
				LBL_TITLE_FRAME:

					Nu3D::Camera::FadeToTargetTint();

					if (g_fadeTimer > 0)
					{
						g_fadeTimer -= Renderer::g_frameDelta;

						if (g_fadeTimer <= 0)
							g_fadeTimer = 0;
					}

					elapsedTicks += Renderer::g_frameDelta;
					int32_t pulsePhase = elapsedTicks & 63;

					if (pulsePhase > 31)
						pulsePhase = 63 - pulsePhase;

					if (showPressJumpPrompt)
						Renderer::DrawMainMenuText(204, "press jump", 4 * pulsePhase);

					Nullsub3();
					RenderMenu();

					if (g_attractModeTimer < 0)
					{
						if (elapsedTicks > timeoutTicks && ! g_nextScreen)
						{
							g_nextScreen = 1;

						LBL_BEGIN_FADE_OUT:

							g_fadeTimer = 23;
							Nu3D::Camera::SetTint(0, 0, 0, 12);
						}
					}
					else if (((InputManager::g_curButtonsPressed & INPUT_SECRET_MENU) != 0 || elapsedTicks > timeoutTicks) && ! g_nextScreen)
					{
						if (showPressJumpPrompt)
							g_nextScreen = 10;
						else
							g_nextScreen = (InputManager::g_curButtonsPressed & INPUT_SECRET_MENU) != 0 ? 10 : 1;

						goto LBL_BEGIN_FADE_OUT;
					}

					if ((InputManager::g_curButtonsPressed & INPUT_JUMP) != 0 && (InputManager::g_prevButtonsPressed & INPUT_JUMP) == 0)
					{
						if (g_nextScreen)
							break;

						if (showPressJumpPrompt)
						{
							if (elapsedTicks > 30)
							{
								g_nextScreen = 2;
								g_fadeTimer = 23;

								Nu3D::Camera::SetTint(0, 0, 0, 12);
								AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);
							}
						}
					}
				}

				if (g_fadeTimer)
					goto LBL_TITLE_FRAME;

				AudioManager::StopAndWait();

				if (g_attractModeTimer >= 0)
					return g_nextScreen - 1;

				nextScreenResult = g_nextScreen;

			} while (g_nextScreen == 2);

			return nextScreenResult - 1;
		}

		// the y position of the four entries: configure controller, music volume, sfx volume and configure gfx
		// GLOBAL: TOY2 0x004F6BD4
		int32_t g_unk4F6BD4[4] = { 65, 85, 105, 125 };

		// GLOBAL: TOY2 0x0055A0E8
		int32_t g_unk55A0E8;

		// GLOBAL: TOY2 0x0055A0EC
		int32_t g_unk55A0EC;

		// FUNCTION: TOY2 0x004371B0
		void ShowSettings()
		{
			int32_t arrowOffset = 0;
			int32_t childResult = 0;
			int32_t scrollPos = g_unk4F6BD4[0] << 8;
			int32_t selected = 0;
			int32_t arrowNext = 0;
			int32_t editing = 0;
			int32_t blink = 0;
			int32_t soundAnim = 0;
			int32_t musicAnimAcc = 0;
			int32_t soundAnimAcc = 0;
			int32_t musicAnim = 0;
			int32_t pulse = 0;
			int32_t animCounter = 0;
			int32_t entryY;
			int32_t newPos;
			int32_t tile;
			int32_t musicFrame;
			int32_t steps;
			uint8_t musicVolume;
			uint8_t soundVolume;
			int16_t unkData1;
			int16_t unkData2;
			uint8_t cameraType = 0;
			SaveManager::Save99Data save99Data;

			InputManager::g_curButtonsPressed = 0;
			InputManager::g_prevButtonsPressed = 0;

			g_fadeTimer = 0;
			g_nextScreen = 0;
			Nu3D::Camera::g_cameraTintBlue = 0;
			Nu3D::Camera::g_cameraTintGreen = 0;
			Nu3D::Camera::g_cameraTintRed = 0;

			Nu3D::Camera::SetTint(128, 128, 128, 12);
			SoftwareRenderer::UnkFunc67(0, 0);

			Renderer::g_frameDelta = 1;

			AudioManager::PlayMusicLooping(19);

			while (! g_nextScreen || g_fadeTimer)
			{
				Nu3D::Camera::FadeToTargetTint();

				if (g_fadeTimer > 0)
				{
					g_fadeTimer -= Renderer::g_frameDelta;

					if (g_fadeTimer <= 0)
						g_fadeTimer = 0;
				}

				blink = (blink - 1) & 1;
				pulse += Renderer::g_frameDelta;

				if (pulse >= 40)
					pulse -= 40;

				if (arrowOffset > 0)
				{
					arrowOffset += Renderer::g_frameDelta;
					arrowNext = arrowOffset;

					if (arrowOffset >= 32)
					{
						arrowOffset -= 32;
						arrowNext = arrowOffset;
					}
				}

				if (arrowOffset < 0)
				{
					newPos = arrowOffset - Renderer::g_frameDelta;
					arrowNext = newPos;

					if (newPos <= -32)
						arrowNext = newPos + 32;
				}

				animCounter = (Renderer::g_frameDelta + animCounter) & 0xF;

				if (! editing)
				{
					entryY = g_unk4F6BD4[selected];

					if (scrollPos == entryY << 8)
					{
						if ((InputManager::g_curButtonsPressed & INPUT_JUMP) == 0 || (InputManager::g_prevButtonsPressed & INPUT_JUMP) != 0 || g_nextScreen)
						{
							if ((InputManager::g_curButtonsPressed & INPUT_UP) != 0 && selected > 0)
							{
								--selected;
								arrowNext = -1;
								AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
							}

							if ((InputManager::g_curButtonsPressed & INPUT_DOWN) != 0 && selected < 3)
							{
								++selected;
								arrowNext = 1;
								AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
								animCounter = -1;
							}
						}
						else
						{
							InputManager::g_prevButtonsPressed = InputManager::g_curButtonsPressed;
							editing = 1;
							AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);
							soundAnimAcc = 0;
							musicVolume = SaveManager::g_save0Data.musicVolume;
							soundAnim = 0;
							pulse = 0;

							soundVolume = SaveManager::g_save0Data.soundVolume;
							unkData2 = SaveManager::g_save0Data.unkData2;
							cameraType = SaveManager::g_save0Data.cameraType;

							memcpy(&save99Data, &SaveManager::g_save99Data, sizeof(save99Data));

							unkData1 = SaveManager::g_save0Data.unkData1;
						}
					}
					else
					{
						animCounter = -1;

						if (scrollPos < entryY << 8)
						{
							scrollPos += 8 * Renderer::g_frameDelta * (entryY - g_unk4F6BD4[selected - 1]);

							if (scrollPos > entryY << 8)
								scrollPos = entryY << 8;
						}

						if (scrollPos > entryY << 8)
						{
							scrollPos -= 8 * Renderer::g_frameDelta * (g_unk4F6BD4[selected + 1] - entryY);

							if (scrollPos < entryY << 8)
								scrollPos = entryY << 8;
						}
					}
				}

				if ((InputManager::g_curButtonsPressed & INPUT_CANCEL) != 0 && (InputManager::g_prevButtonsPressed & INPUT_CANCEL) == 0 && ! g_nextScreen
					&& ! editing)
				{
					g_nextScreen = 1;
					g_fadeTimer = 44;
					Nu3D::Camera::SetTint(0, 0, 0, 6);
					AudioManager::PlayOneShotSoundGlobal(2, 4608, 80, 80);
				}

				if ((((InputManager::g_curButtonsPressed & INPUT_CANCEL) != 0 && (InputManager::g_prevButtonsPressed & INPUT_CANCEL) == 0) || childResult == 1)
					&& editing == 1)
				{
					editing = 0;
					AudioManager::PlayOneShotSoundGlobal(2, 4608, 80, 80);
					SaveManager::g_save0Data.musicVolume = musicVolume;
					SaveManager::g_save0Data.soundVolume = soundVolume;
					SaveManager::g_save0Data.unkData1 = unkData1;
					SaveManager::g_save0Data.unkData2 = unkData2;

					memcpy(&SaveManager::g_save99Data, &save99Data, sizeof(SaveManager::g_save99Data));
					SaveManager::g_save0Data.cameraType = cameraType;
					g_unk55A0E8 = unkData1;
					g_unk55A0EC = unkData2;
					childResult = 0;
				}

				if (((InputManager::g_curButtonsPressed & INPUT_JUMP) != 0 && (InputManager::g_prevButtonsPressed & INPUT_JUMP) == 0) || childResult == 2)
				{
					if (editing == 1)
					{
						editing = 0;
						AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);
						childResult = 0;
						goto LABEL_52;
					}
				}

				if (! editing)
				{
				LABEL_52:
					if (animCounter < 0)
					{
						tile = arrowNext;

						if (arrowNext < 0)
							tile = arrowNext + 31;

						Renderer::Sprite::DrawTile(48, scrollPos >> 8, 52, tile / 2);
						Renderer::Sprite::DrawTile(240, scrollPos >> 8, 52, tile / 2);
					}
					else
					{
						Renderer::Sprite::DrawTile(48, scrollPos >> 8, 52, animCounter / 2 + 16);
						Renderer::Sprite::DrawTile(240, scrollPos >> 8, 52, (((animCounter / 2) + 2) & 7) + 16);
					}

					Renderer::DrawMenuText(50, "options", 0);
					Renderer::DrawMenuText(g_unk4F6BD4[0] + 14, "configure controller", 0);
					Renderer::DrawMenuText(g_unk4F6BD4[1] + 14, "music volume", 0);
					Renderer::DrawMenuText(g_unk4F6BD4[2] + 14, "sfx volume", 0);
					Renderer::DrawMenuText(g_unk4F6BD4[3] + 14, "configure gfx", 0);

					Renderer::DrawMenuTextScaled(160, 190, "jump:select  cancel:go back", 0, 1, 1536);
				}
				else if (editing == 1)
				{
					if (scrollPos != g_unk4F6BD4[0] << 8 && scrollPos != g_unk4F6BD4[3] << 8)
						Renderer::DrawMenuTextScaled(160, 190, "jump:accept  cancel:go back", 0, 1, 1536);

					if (scrollPos == g_unk4F6BD4[0] << 8)
						childResult = ShowControlConfig();

					if (scrollPos == g_unk4F6BD4[1] << 8)
					{
						Renderer::DrawMenuText(50, "music volume", 0);

						if ((InputManager::g_curButtonsPressed & INPUT_RIGHT) != 0 && (InputManager::g_prevButtonsPressed & INPUT_RIGHT) == 0
							&& SaveManager::g_save0Data.musicVolume < 10)
						{
							++SaveManager::g_save0Data.musicVolume;
							AudioManager::PlayOneShotSoundGlobal(1, 4608, 48, 80);
						}

						if ((InputManager::g_curButtonsPressed & INPUT_LEFT) != 0 && (InputManager::g_prevButtonsPressed & INPUT_LEFT) == 0
							&& SaveManager::g_save0Data.musicVolume)
						{
							--SaveManager::g_save0Data.musicVolume;
							AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 48);
						}

						musicFrame = musicAnim;

						if (musicAnim >= 9)
							Renderer::Sprite::DrawTile(117, 76, 54, musicAnim - 9);
						else
							Renderer::Sprite::DrawTile(117, 76, 53, musicAnim);

						Renderer::Sprite::DrawTile(119, 166, 55, SaveManager::g_save0Data.musicVolume);
						musicAnimAcc += 8 * Renderer::g_frameDelta;

						if (musicAnimAcc >= 16)
						{
							++musicAnim;

							if (musicFrame + 1 >= 18)
								musicAnim = 0;

							musicAnimAcc &= 0xF;
						}

						if (pulse < 20)
						{
							if (SaveManager::g_save0Data.musicVolume)
								Renderer::Sprite::DrawTile(50, 158, 56, 3);

							if (SaveManager::g_save0Data.musicVolume < 10)
								Renderer::Sprite::DrawTile(239, 158, 56, 2);
						}
					}

					if (scrollPos == g_unk4F6BD4[2] << 8)
					{
						Renderer::DrawMenuText(50, "sfx volume", 0);

						if ((InputManager::g_curButtonsPressed & INPUT_RIGHT) != 0 && (InputManager::g_prevButtonsPressed & INPUT_RIGHT) == 0
							&& SaveManager::g_save0Data.soundVolume < 10)
						{
							++SaveManager::g_save0Data.soundVolume;
							AudioManager::PlayOneShotSoundGlobal(1, 4608, 48, 80);
						}

						if ((InputManager::g_curButtonsPressed & INPUT_LEFT) != 0 && (InputManager::g_prevButtonsPressed & INPUT_LEFT) == 0
							&& SaveManager::g_save0Data.soundVolume)
						{
							--SaveManager::g_save0Data.soundVolume;
							AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 48);
						}

						if (soundAnim >= 9)
							Renderer::Sprite::DrawTile(117, 76, 58, soundAnim - 9);
						else
							Renderer::Sprite::DrawTile(117, 76, 57, soundAnim);

						Renderer::Sprite::DrawTile(119, 166, 55, SaveManager::g_save0Data.soundVolume);
						soundAnimAcc += Renderer::g_frameDelta * ((SaveManager::g_save0Data.soundVolume >> 1) + 5);

						if (soundAnimAcc >= 16)
						{
							steps = (uint32_t)soundAnimAcc >> 4;
							soundAnimAcc = -16 * steps + soundAnimAcc;

							do
							{
								if (++soundAnim >= 18)
									soundAnim = 0;

								--steps;
							} while (steps);
						}

						if (pulse < 20)
						{
							if (SaveManager::g_save0Data.soundVolume)
								Renderer::Sprite::DrawTile(50, 158, 56, 3);

							if (SaveManager::g_save0Data.soundVolume < 10)
								Renderer::Sprite::DrawTile(239, 158, 56, 2);
						}
					}

					if (scrollPos == g_unk4F6BD4[3] << 8)
						ShowGraphicsConfig();
				}

				Nullsub3();
				RenderMenu();

				SaveManager::g_save0Data.unkData1 = g_unk55A0E8;
				SaveManager::g_save0Data.unkData2 = g_unk55A0EC;

				AudioManager::SetVolumesProcessed(SaveManager::g_save0Data.musicVolume, SaveManager::g_save0Data.soundVolume);
				arrowOffset = arrowNext;
			}

			AudioManager::FlushSoundVoices();
			AudioManager::StopAndWait();
		}
	}
}
#include "Toy2/Toy2.h"
#include "Toy2/D3DApp.h"
#include "Logger.h"
#include "FileUtils.h"
#include "InputManager.h"
#include "DrawingDevice.h"
#include "ModeSelect.h"
#include "Nullsub.h"
#include "SoftwareRenderer.h"
#include "SaveManager.h"
#include "Random.h"
#include "Toy2/LevelSelect.h"
#include "Toy2/Buzz.h"
#include "Toy2/Levels.h"
#include "Toy2/MainMenu.h"
#include "Toy2/Actor.h"
#include "Toy2/Animation.h"
#include "Toy2/Collision.h"
#include "Toy2/KiteTail.h"
#include "Toy2/Sector.h"

#include "Nu3D/Font.h"
#include "Nu3D/Viewport.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Light.h"
#include "Nu3D/Link.h"
#include "Nu3D/Math.h"
#include "Nu3D/Scene.h"
#include "Renderer/Renderer.h"
#include "Renderer/Effects.h"
#include "Renderer/Glue.h"
#include "Renderer/Shadows.h"
#include "Renderer/Sprite.h"
#include "Renderer/SpriteSheets.h"
#include "AudioManager/AudioManager.h"
#include "NGNLoader/NGNLoader.h"

#include <WINDOWS.H>
#include <STDIO.H>
#include <DINPUT.H>

#include <Numerics.h>

namespace Toy2
{
	// GLOBAL: TOY2 0x00508D70
	ToyCfg g_toyCfgData = {
		7, /* flags */
		1, /* detail */
		2.0, /* gamma correction */
		-1, /* driver index */
		-1, /* device index */
		-1, /* display mode index */
	};

	// GLOBAL: TOY2 0x0088278C
	int32_t g_levelFileIndex;

	// GLOBAL: TOY2 0x0052AD9C
	int32_t g_returnedToTitle;

	// GLOBAL: TOY2 0x0052ADA0
	int32_t g_attractModeTimer;

	// GLOBAL: TOY2 0x00A4C454
	int32_t g_isElevatorHopLevel;

	// GLOBAL: TOY2 0x005D2A90
	int32_t g_hasBackdrop;

	// GLOBAL: TOY2 0x004F7280
	int32_t g_showBlackFrames = 1;

	// GLOBAL: TOY2 0x004FCDB4
	int32_t g_cdBaseTrack = 2;

	// GLOBAL: TOY2 0x005281C8
	int32_t g_modeSelectFinished;

	// GLOBAL: TOY2 0x0052ADA4
	int32_t g_unused1;

	// GLOBAL: TOY2 0x0052ADA8
	int32_t g_unused2;

	// GLOBAL: TOY2 0x0072EFD8
	int32_t g_pastInitialBoot;

	// GLOBAL: TOY2 0x0052ADB4
	int32_t g_attractModeInputTimer;

	// GLOBAL: TOY2 0x0052C83C
	int32_t g_curDemoLevel;

	// GLOBAL: TOY2 0x0052B7DC
	int32_t g_levelTransition;

	// GLOBAL: TOY2 0x0052AD94
	int32_t g_demoMode;

	// GLOBAL: TOY2 0x0055A0E0
	int32_t g_hasStaticBackdrop;

	// GLOBAL: TOY2 0x00500A50
	int32_t g_nextBackdropId = 36;

	// GLOBAL: TOY2 0x00830C88
	int32_t g_mainMenuState;

	// GLOBAL: TOY2 0x00830D58
	int32_t g_saveLoaded;

	// GLOBAL: TOY2 0x0052AD8A
	int16_t g_levelIndex;

	// GLOBAL: TOY2 0x0050268C
	int32_t g_levelFileConversion[16] = { 1, 2, 6, 4, 5, 3, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1 };

	// GLOBAL: TOY2 0x00503840
	int16_t g_tokenProgressTable[16] = { 0, 0, 3, 0, 0, 10, 0, 0, 18, 0, 0, 28, 0, 0, 40, 0 };

	// GLOBAL: TOY2 0x0052AD7C
	int32_t g_demoPathWriteIdx;

	// GLOBAL: TOY2 0x0052EF40
	int32_t g_demoInputRunLength;

	// GLOBAL: TOY2 0x0052B818
	int16_t g_isPaused;

	// GLOBAL: TOY2 0x00529E48
	int16_t g_pauseMenuBlinkTimer;

	// GLOBAL: TOY2 0x0052F2DC
	int16_t g_levelTransitionTimer;

	// GLOBAL: TOY2 0x00830D50
	int32_t g_quitToTitleFlag;

	// GLOBAL: TOY2 0x0052F300
	Buzz::Toy2BuzzActor g_buzzActor;

	// GLOBAL: TOY2 0x00529388
	int32_t g_wndIsExitingUnused;

	// GLOBAL: TOY2 0x0072EFC8
	int32_t g_clearScreenSaveResult;

	// GLOBAL: TOY2 0x00500AA0
	int32_t g_setScreenSaveRunning = 1;

	// GLOBAL: TOY2 0x00830C64
	int32_t g_extraControlsUnused;

	// GLOBAL: TOY2 0x00830C1C
	int32_t g_inputSuppressFrames;

	// GLOBAL: TOY2 0x00500A58
	SectorBackdropTexIdTable g_sectorBackdropTexTable = {
		{ 36, 40, 41, 42, 43, 44, 45, 46, 47, 32 },
		{ 88, 89, 90, 91, 92, 93, 94, 95 },
	};

	// GLOBAL: TOY2 0x00534550
	int32_t g_unused0;

	// GLOBAL: TOY2 0x00559C5C
	int32_t g_renderGame;

	// GLOBAL: TOY2 0x005546A0
	int32_t g_curSector;

	// GLOBAL: TOY2 0x0054DEA0
	int32_t g_curSectorCopy1;

	// GLOBAL: TOY2 0x005D2A8C
	int32_t g_curSectorCopy2;

	// GLOBAL: TOY2 0x0054E04E
	int16_t g_pickupAnimTimer;

	// GLOBAL: TOY2 0x005D2A94
	uint8_t g_type59VisibleIndices[36];

	// GLOBAL: TOY2 0x00508D28
	ViewportPreset g_viewportPresets[3] = {
		{ 3000.0, 3500.0, 50.0, 55.0, 10000.0, 0.0 },
		{ 5000.0, 6500.0, 40.0, 60.0, 10000.0, 0.0 },
		{ 10000.0, 12000.0, 40.0, 60.0, 12000.0, 1000.0 },
	};
}

namespace Toy2
{
	namespace Graphics
	{
		// FUNCTION: TOY2 0x004CDD90
		int32_t AddDetailLevel()
		{
			int32_t detail = g_toyCfgData.detail + 1;

			if ((g_toyCfgData.detail + 1) >= 2)
				detail = 2;

			g_toyCfgData.detail = detail;

			return detail;
		}

		// FUNCTION: TOY2 0x004CDDB0
		int32_t RemoveDetailLevel()
		{
			int32_t detail = (g_toyCfgData.detail - 1) <= 0 ? 0 : g_toyCfgData.detail - 1;
			g_toyCfgData.detail = detail;

			return detail;
		}
	}

	namespace Game
	{
		// STUB: TOY2 0x0049E330
		void PauseLoop() {}

		// STUB: TOY2 0x0049DFE0
		void MainLoop() {}

		// STUB: TOY2 0x00406CD0
		void InitActor(Toy2::Actor::Toy2Actor* actor, int32_t initialSpawn) {}

		// STUB: TOY2 0x00407440
		void ActorCollisionCheck() {}

		// STUB: TOY2 0x004086F0
		void UpdateActors() {}

		// STUB: TOY2 0x0049F4B0
		void MenuLoop() {}
	}

	namespace PostGameRecap
	{
		// STUB: TOY2 0x004398B0
		void Tick() {}
	}

	namespace GameOver
	{
		// STUB: TOY2 0x00437B20
		void Tick() {}
	}

	// FUNCTION: TOY2 0x00440F70
	void RenderGame(int32_t fullRender)
	{
		// the camera data lives in the shared math scratch buffer, treat it as dwords like the original does
		uint32_t* mathScratch = (uint32_t*)Collision::g_mathScratch;

		// never set to anything but 0
		if (g_renderGame)
			return;

		if (g_showBlackFrames)
		{
			g_showBlackFrames = 0;
			Renderer::ShowBlackFrames();
		}

		Renderer::Sprite::g_parallaxDepthZPos = 0.9999f;

		// TODO:  (the original swaps red and blue here)
		RGBA clearColor;
		clearColor.a = 0xFF;
		clearColor.b = MainMenu::g_menuClearColor.r >> 1;
		clearColor.g = MainMenu::g_menuClearColor.g >> 1;
		clearColor.r = MainMenu::g_menuClearColor.b >> 1;

		Renderer::Glue::ReleaseBackdrop();

		g_curSector = Levels::GetSectorAtPosition((Vector3I*)&mathScratch[221]);
		g_curSectorCopy1 = g_curSector;
		g_curSectorCopy2 = g_curSector;

		int32_t sectorIndex = g_curSector;
		Sector::UpdateLighting();

		Renderer::SetViewportPreset();

		Nu3D::Scene::RenderLayerFlags layerFlags = (Nu3D::Scene::RenderLayerFlags)0;

		// some levels need to override the sector being rendered or the viewport distance
		switch (g_levelFileIndex)
		{
			case 2:
				if (g_curSectorCopy1 == 3)
				{
					if ((int32_t)mathScratch[222] < -104000)
					{
						layerFlags = Nu3D::Scene::RENDERLAYER_FAR_FORCE_SCAN;
						sectorIndex = -1;
					}
				}
				else if ((int32_t)mathScratch[222] < -74000)
				{
					layerFlags = Nu3D::Scene::RENDERLAYER_FAR_FORCE_SCAN;
					sectorIndex = -1;
				}
				break;

			case 3:
			case 9:
				Renderer::SetViewportPresetByDetail(0x7FFFFFFF);
				layerFlags = Nu3D::Scene::RENDERLAYER_INFINITE_VIEWPORT;
				break;

			case 4: {
				int32_t sector = g_curSectorCopy2;

				if (g_curSectorCopy2 == 2 && Nu3D::Camera::g_cameraWorldPos.y < -7900.0)
				{
					sector = 1;
					g_curSectorCopy1 = 1;
					g_curSector = 1;
					g_curSectorCopy2 = 1;
					sectorIndex = 1;
				}

				Vector3I checkPos;
				checkPos.x = 334743;
				checkPos.y = -66813;
				checkPos.z = 386446;

				if (sector == 2 || Nu3D::Math::IsWithinDistance(&checkPos, &g_buzzActor.posAngles.pos, 250))
					Renderer::SetViewportPresetByDetail(0x7FFFFFFF);

				break;
			}

			case 10:
				if (g_curSector == 4 && Nu3D::Camera::g_cameraWorldPos.y < -40800.0 && Nu3D::Camera::g_cameraWorldPos.x > -800.0
					&& Nu3D::Camera::g_cameraWorldPos.x < 800.0 && Nu3D::Camera::g_cameraWorldPos.z > -5500.0 && Nu3D::Camera::g_cameraWorldPos.z < -3900.0)
				{
					sectorIndex = 5;
					g_curSectorCopy1 = 5;
					g_curSector = 5;
				}

				break;

			case 11:
				if (g_curSector == 5 || g_curSector == 7)
					Renderer::SetViewportPresetByDetail(0x7FFFFFFF);

				break;

			case 14:
				Renderer::ConfigureFog(24000.0, 46000.0, clearColor);
				break;
		}

		Animation::AnimateActors(Actor::g_activeActorArray);
		ProcessMiscEventsEx();

		if (g_hasBackdrop || g_hasStaticBackdrop)
		{
			Renderer::ClearScreen(clearColor, 2);
		}
		else
		{
			Renderer::ClearScreen(clearColor, 3);
			Renderer::ConfigureFog(24000.0, 48000.0, clearColor);
		}

		Nu3D::Light::SetDirectionalLight(
			-g_buzzActor.lightDirection.x,
			-g_buzzActor.lightDirection.y,
			-g_buzzActor.lightDirection.z,
			g_buzzActor.color.r,
			g_buzzActor.color.g,
			g_buzzActor.color.b);

		if (Renderer::BeginScene())
		{
			Renderer::UpdateBackgroundScroll(Nu3D::Camera::g_cameraPitch, -Nu3D::Camera::g_cameraYaw);

			Renderer::SetFogEnable(0);
			Renderer::RenderParallaxBackground(1);
			Renderer::SetFogEnable(1);

			// TODO: NOTE: the original hands this an uninitialised stack variable, which can't be intentional.
			// Sector::UpdateLighting sets it to the camera view matrix, so do the same here.
			Nu3D::Camera::SetObjectViewMatrix((Matrix3x3I16*)&mathScratch[229]);

			Nu3D::Scene::RenderWorldGeometry(sectorIndex, layerFlags);
			Nu3D::Scene::RenderActors(Actor::g_activeActorArray);

			if (fullRender)
			{
				Renderer::Shadows::DrawAll();

				uint32_t bitmapWidth = 255;
				uint32_t bitmapHeight = 255;

				int16_t animTimer = g_pickupAnimTimer;

				if (! g_isPaused)
				{
					animTimer = Renderer::g_frameDelta + g_pickupAnimTimer;
					g_pickupAnimTimer = animTimer;

					if (animTimer > 19)
					{
						animTimer -= 20;
						g_pickupAnimTimer = animTimer;
					}
				}

				int32_t tileFrame = animTimer >> 1;

				Vector3F cameraPos;
				Nu3D::Math::GetPositionVector(&Nu3D::Camera::g_secondaryCamera.matrix, &cameraPos);

				int32_t cullDistance = Levels::g_type63CullDistance / 4;
				int32_t cullDistanceSquared = cullDistance * cullDistance;

				// type 63 records are the collectables, they are either a sprite (animated through a 10 frame sprite sheet),
				// or a link to a 3D object that gets spun around
				if (Levels::g_recordData[63])
				{
					Levels::Type63* pickup = (Levels::Type63*)Levels::g_recordData[63]->data;
					uint16_t recordCount = Levels::g_recordData[63]->recordCount;

					for (int32_t i = 0; i < recordCount; i++, pickup++)
					{
						// every pickup is offset 2 frames further along the animation than the last one
						tileFrame += 2;

						if (tileFrame > 9)
							tileFrame -= 10;

						pickup->props.flags &= ~0x80;

						if (pickup->pos.y == 0x80000000)
							continue;

						int32_t deltaZ = ((int32_t)cameraPos.z - pickup->pos.z) >> 4;
						int32_t deltaX = ((int32_t)cameraPos.x - pickup->pos.x) >> 4;
						int32_t distanceSquared = deltaX * deltaX + deltaZ * deltaZ - 100;

						if (distanceSquared >= cullDistanceSquared)
							continue;

						// fade the pickup in as the camera gets closer
						int32_t alpha;

						if (((cullDistanceSquared - distanceSquared) & 0xFFFFFFC0) <= 16320)
							alpha = (cullDistanceSquared - distanceSquared) >> 6;
						else
							alpha = 255;

						if (pickup->props.linkId < 0x30)
						{
							Vector3F position;
							position.x = pickup->pos.x;
							position.y = pickup->pos.y;
							position.z = pickup->pos.z;

							Renderer::SpriteSheet* sheet = Renderer::g_spriteSheets[pickup->props.linkId];
							int32_t textureDataIndex = NGNLoader::GetTextureDataIndex(sheet->texIndex);

							if (textureDataIndex)
								NGNLoader::RetrieveTextureData(textureDataIndex, &bitmapWidth, &bitmapHeight, 0, 0, 0);

							Vector2F uvTopLeft;
							Vector2F uvBottomRight;

							uvTopLeft.x = sheet->tiles[tileFrame].x / (double)(int32_t)bitmapWidth;
							uvTopLeft.y = sheet->tiles[tileFrame].y / (float)(int32_t)bitmapHeight;
							uvBottomRight.x = (sheet->tileWidth + sheet->tiles[tileFrame].x) / (double)(int32_t)bitmapWidth;
							uvBottomRight.y = (sheet->tileHeight + sheet->tiles[tileFrame].y) / (float)(int32_t)bitmapHeight;

							pickup->props.flags |= 0x80;

							RGBA color;
							color.value = (alpha << 24) + 0xFFFFFF;

							Renderer::Sprite::QueueQuadSprite(
								&position,
								0,
								100.0,
								100.0,
								&uvTopLeft,
								&uvBottomRight,
								textureDataIndex,
								color,
								Renderer::RENDER_ZWRITE | Renderer::RENDER_CULL_NONE | Renderer::RENDER_ALPHA_DEFAULT);

							// a blob shadow on the floor underneath it
							position.x = pickup->pos.x;
							position.y = pickup->props.floorPos - 10.0;
							position.z = pickup->pos.z;

							int32_t shadowTexture = NGNLoader::GetTextureDataIndex(31);

							if (shadowTexture)
								NGNLoader::RetrieveTextureData(shadowTexture, &bitmapWidth, &bitmapHeight, 0, 0, 0);

							uvTopLeft.x = 96.0 / (double)(int32_t)bitmapWidth;
							uvTopLeft.y = 96.0 / (float)(int32_t)bitmapHeight;
							uvBottomRight.x = 127.0 / (double)(int32_t)bitmapWidth;
							uvBottomRight.y = 127.0 / (float)(int32_t)bitmapHeight;

							if ((Renderer::g_deviceBlendShadeCapsCpy & 4) != 0)
							{
								color.value = 0xFFAAAAAA;

								Renderer::Sprite::QueueGroundAlignedSprite(
									&position,
									0,
									80.0,
									80.0,
									&uvTopLeft,
									&uvBottomRight,
									shadowTexture,
									color,
									Renderer::RENDER_ZWRITE | Renderer::RENDER_CULL_NONE | Renderer::RENDER_ALPHA_ALT);
							}
							else
							{
								color.value = 0x88000000;

								Renderer::Sprite::QueueGroundAlignedSprite(
									&position,
									0,
									80.0,
									80.0,
									&uvTopLeft,
									&uvBottomRight,
									shadowTexture,
									color,
									Renderer::RENDER_ZWRITE | Renderer::RENDER_CULL_NONE | Renderer::RENDER_ALPHA_DEFAULT);
							}
						}
						else
						{
							if (! g_isPaused)
							{
								Vector3I rotation = { 0, 0, 0 };

								Nu3D::Link::GetRotation8Bit(pickup->props.linkId, &rotation);
								Nu3D::Link::SetRotationAbsolute8bit(
									pickup->props.linkId,
									rotation.x + 10 * Renderer::g_frameDelta,
									rotation.y + 2 * Renderer::g_frameDelta * (3 * ((i >> 2) & 3) + 7),
									rotation.z + 2 * Renderer::g_frameDelta * ((i & 7) + 4));
							}

							pickup->props.flags |= 0x80;
						}
					}
				}

				// type 59 records are light sprites, remember which ones are visible
				int32_t visibleCount = 0;
				uint32_t lightBitmapWidth = 255;
				uint32_t lightBitmapHeight = 255;
				float cullDistanceSquaredF = (float)cullDistanceSquared;

				Nu3D::Math::GetPositionVector(&Nu3D::Camera::g_secondaryCamera.matrix, &cameraPos);

				if (Levels::g_recordData[59])
				{
					Vector3I* record = Levels::g_recordData[59]->data;
					uint16_t recordCount = Levels::g_recordData[59]->recordCount;

					for (int32_t i = 0; i < recordCount; i++, record++)
					{
						Vector3F position;
						position.x = record->x;
						position.y = record->y - 250;
						position.z = record->z;

						Vector3F offset;
						Nu3D::Math::VertexSubtract(&offset, &position, &cameraPos);

						double distanceSquared = (offset.z * offset.z + offset.y * offset.y + offset.x * offset.x) * 0.0009765625;

						if (distanceSquared < cullDistanceSquaredF && record->y != 0x80000000)
						{
							if (distanceSquared < 65536.0 && (Nu3D::Frustum::TestSphereAllPlanes(&position, 250.0) & 0x55555575) == 0)
								g_type59VisibleIndices[visibleCount++] = i;

							Renderer::SpriteSheet* sheet = Renderer::g_spriteSheets[30];
							int32_t textureDataIndex = NGNLoader::GetTextureDataIndex(sheet->texIndex);

							if (textureDataIndex)
								NGNLoader::RetrieveTextureData(textureDataIndex, &lightBitmapWidth, &lightBitmapHeight, 0, 0, 0);

							Vector2F uvTopLeft;
							Vector2F uvBottomRight;

							uvTopLeft.x = sheet->tiles[0].x / (double)(int32_t)lightBitmapWidth;
							uvTopLeft.y = sheet->tiles[0].y / (float)(int32_t)lightBitmapHeight;
							uvBottomRight.x = (sheet->tileWidth + sheet->tiles[0].x) / (double)(int32_t)lightBitmapWidth;
							uvBottomRight.y = (sheet->tileHeight + sheet->tiles[0].y) / (float)(int32_t)lightBitmapHeight;

							RGBA white;
							white.value = 0xFFFFFFFF;

							Renderer::Sprite::QueueQuadSprite(
								&position,
								0,
								150.0,
								250.0,
								&uvTopLeft,
								&uvBottomRight,
								textureDataIndex,
								white,
								Renderer::RENDER_ZWRITE | Renderer::RENDER_CULL_NONE | Renderer::RENDER_ALPHA_DEFAULT);
						}
					}
				}

				g_type59VisibleIndices[visibleCount] = 0xFF;

				Renderer::Particles::DrawParticles();

				// the beam queue is filled from the end, so everything from the current count onwards is pending
				for (int32_t i = Renderer::Beam::g_beamRequestCount; Renderer::Beam::g_beamRequestCount < 100; ++Renderer::Beam::g_beamRequestCount)
				{
					Renderer::Beam::BeamRequest* beam = &Renderer::Beam::g_beamRequests[i];

					Renderer::Beam::DrawSegmentedBeam(
						beam->spriteSheetId, beam->halfWidth, beam->segmentLength, &beam->origin, &beam->direction, beam->tintB, beam->tintG, beam->tintR);

					i = Renderer::Beam::g_beamRequestCount + 1;
				}

				Renderer::DrawFallingParticles();
				Renderer::DrawLensFlares();

				if (g_levelFileIndex == 2)
					KiteTail::Draw();
			}

			if (! Renderer::GetIsSoftwareRendering())
				Renderer::DrawCinematicBars();

			Renderer::DrawTintOverlay();
			Renderer::FlushRenderQueues();
			Renderer::SetFogEnable(0);
			Renderer::Sprite::DrawQueuedSprite();
			DevDraw::DrawSlots();
			Renderer::EndScene(1);
		}

		Renderer::DoFrameDelay(1);
		Nu3D::Camera::ToggleCameraSkew(0);
		Nu3D::Camera::SetSkewPhase((Renderer::g_frameDelta << 16) / 360);
	}

	// STUB: TOY2 0x00454020
	void ShowPostGameSaveMenu() {}

	// STUB: TOY2 0x00453D90
	void ShowActClearScreen() {}

	// FUNCTION: TOY2 0x0049EB50 [MATCHED]
	uint32_t ComputeTokenProgress()
	{
		int32_t times = 0;
		int32_t idx = 0;

		for (idx = 0; idx < 15; idx++)
		{
			int32_t tokenData = SaveManager::g_save0Data.tokens[g_levelFileConversion[idx]];

			if (! tokenData)
				break;

			int32_t innerIdx = 5;

			do
			{
				if (tokenData & 1)
					++times;

				tokenData >>= 1;
				--innerIdx;
			} while (innerIdx);
		}

		return idx + (((times << 8) + g_tokenProgressTable[idx]) << 8);
	}

	// STUB: TOY2 0x00414720
	int32_t EnterLevel(int32_t levelIndex) { return 0; }

	// STUB: TOY2 0x004A3770
	void LoadPathBin() {}

	// FUNCTION: TOY2 0x00453CF0
	int32_t ShowLevelSelect()
	{
		int32_t prevLevelFileIdx = Toy2::g_levelFileIndex;

		g_levelFileIndex = 16;
		Levels::g_levelLoadConfig = 56;

		g_hasStaticBackdrop = 0;
		Renderer::g_virtualScreenWidth = 512.0;
		Renderer::g_virtualScreenHeight = 256.0;
		g_nextBackdropId = 36;
		Levels::InitLevelPlay(16);

		Renderer::g_frameDelta = 2;
		g_hasBackdrop = 0;
		MainMenu::g_menuClearColor.b = 140;
		MainMenu::g_menuClearColor.g = 140;
		MainMenu::g_menuClearColor.r = 140;

		int32_t newState = LevelSelect::Tick();

		g_levelFileIndex = prevLevelFileIdx;

		MainMenu::g_menuClearColor.b = 32;
		MainMenu::g_menuClearColor.g = 32;
		MainMenu::g_menuClearColor.r = 32;

		return newState;
	}

	// STUB: TOY2 0x00453FA0
	void ShowMovieViewer() {}

	// STUB: TOY2 0x00453F20
	int32_t ShowSaveScreen() { return 0; }

	// STUB: TOY2 0x0049EB20
	void UnlockAndPlayMovie(int32_t movieId, int32_t backgroundId, int32_t forcePlay) {}

	// STUB: TOY2 0x0049AB90
	int32_t PlayMovieWithTransition(int32_t movieId, int32_t backgroundId) { return 1; }

	// FUNCTION: TOY2 0x0048F1B0
	void SetBackdropByIndex(int32_t index)
	{
		int32_t sectorIdx = index + 1;
		Renderer::g_parallaxCurHorizScroll = 0.0;

		if (index + 1 >= 0 && sectorIdx < 9)
		{
			if (NGNLoader::GetTextureDataIndex(g_sectorBackdropTexTable.primary[sectorIdx]))
			{
				g_nextBackdropId = g_sectorBackdropTexTable.primary[sectorIdx];
				g_hasBackdrop = 1;
			}
			else
			{
				uint32_t l_textureId = g_sectorBackdropTexTable.secondary[sectorIdx - 1];
				int32_t* l_id = &g_sectorBackdropTexTable.secondary[sectorIdx - 1];

				if (NGNLoader::GetTextureDataIndex(l_textureId))
					g_nextBackdropId = *l_id;

				g_hasBackdrop = 1;
			}
		}
	}

	// STUB: TOY2 0x00438520
	int32_t ShowStaticScreen(int32_t backdropIndex) { return 0; }

	// STUB: TOY2 0x0043A380
	int32_t ShowCredits() { return 0; }

	// FUNCTION: TOY2 0x004381F0
	int32_t ScreenDispatcher(int32_t index)
	{
		int32_t defaultFadeFramesRemaining;

		switch (index)
		{
			case 1: {
				int32_t caseOneFadeFrames = 160;
				InputManager::g_curButtonsPressed = 0;
				InputManager::g_prevButtonsPressed = 0;

				MainMenu::g_fadeTimer = 0;
				MainMenu::g_nextScreen = 0;

				Nu3D::Camera::g_cameraTintBlue = 0;
				Nu3D::Camera::g_cameraTintGreen = 0;
				Nu3D::Camera::g_cameraTintRed = 0;

				Nu3D::Camera::SetTint(128, 128, 128, 12);
				SoftwareRenderer::UnkFunc67(0, 0);
				Renderer::g_frameDelta = 1;

				SetBackdropByIndex(1);

				while (true)
				{
					Nu3D::Camera::FadeToTargetTint();
					caseOneFadeFrames -= Renderer::g_frameDelta;

					if (caseOneFadeFrames <= 0)
						break;

					if (caseOneFadeFrames <= 23)
					{
						if ((caseOneFadeFrames + Renderer::g_frameDelta) > 23)
							Nu3D::Camera::SetTint(0, 0, 0, 12);
					}

				LBL_CHECK_FADE_COMPLETE:

					if (! caseOneFadeFrames)
						return 1;
				}

				caseOneFadeFrames = 0;

				if ((caseOneFadeFrames + Renderer::g_frameDelta) > 23)
					Nu3D::Camera::SetTint(0, 0, 0, 12);

				goto LBL_CHECK_FADE_COMPLETE;
			}

			case 2:
				g_mainMenuState = MainMenu::Tick();
				return 1;

			case 4:
				PostGameRecap::Tick();
				return 1;

			case 5:
				GameOver::Tick();
				return 1;

			case 6: {
				if (! g_returnedToTitle && g_attractModeTimer >= 0)
				{
					int32_t caseSixFadeFrames = 2 * g_attractModeTimer;
					InputManager::g_curButtonsPressed = 0;
					InputManager::g_prevButtonsPressed = 0;

					MainMenu::g_fadeTimer = 0;
					MainMenu::g_nextScreen = 0;

					Nu3D::Camera::g_cameraTintBlue = 0;
					Nu3D::Camera::g_cameraTintGreen = 0;
					Nu3D::Camera::g_cameraTintRed = 0;

					Nu3D::Camera::SetTint(128, 128, 128, 12);
					SoftwareRenderer::UnkFunc67(0, 0);
					Renderer::g_frameDelta = 1;
					SetBackdropByIndex(0);

					int32_t skipInputThreshold = caseSixFadeFrames - 120;

					if (! caseSixFadeFrames)
						return 1;

					while (true)
					{
						Nu3D::Camera::FadeToTargetTint();

						if (caseSixFadeFrames > 0)
						{
							caseSixFadeFrames -= Renderer::g_frameDelta;

							if (caseSixFadeFrames <= 0)
								break;
						}

						if (caseSixFadeFrames)
						{
							if ((caseSixFadeFrames + Renderer::g_frameDelta) > 23)
								Nu3D::Camera::SetTint(0, 0, 0, 12);
						}

					LBL_CHECK_OR_COMPLETE:

						if ((InputManager::g_curButtonsPressed & (INPUT_CANCEL | INPUT_SPIN | INPUT_JUMP | INPUT_FIRE)) != 0
							&& caseSixFadeFrames < skipInputThreshold && caseSixFadeFrames > 23)
						{
							caseSixFadeFrames = 24;
						}
						else if (! caseSixFadeFrames)
						{
							return 1;
						}
					}

					caseSixFadeFrames = 0;

					if ((caseSixFadeFrames + Renderer::g_frameDelta) > 23)
						Nu3D::Camera::SetTint(0, 0, 0, 12);

					goto LBL_CHECK_OR_COMPLETE;
				}

				defaultFadeFramesRemaining = 600;

				InputManager::g_curButtonsPressed = 0;
				InputManager::g_prevButtonsPressed = 0;

				MainMenu::g_fadeTimer = 0;
				MainMenu::g_nextScreen = 0;

				Nu3D::Camera::g_cameraTintBlue = 0;
				Nu3D::Camera::g_cameraTintGreen = 0;
				Nu3D::Camera::g_cameraTintRed = 0;

				Nu3D::Camera::SetTint(128, 128, 128, 12);
				SoftwareRenderer::UnkFunc67(0, 0);
				Renderer::g_frameDelta = 1;

				SetBackdropByIndex(0);
				break;
			}

			case 8:
				g_mainMenuState = MainMenu::g_nextScreen - 1;
				return 1;

			case 9:
				MainMenu::ShowSettings();
				return 1;

			case 10:
				ShowStaticScreen(2);
				ShowStaticScreen(3);
				return 1;

			case 11:
				ShowCredits();
				return 1;

			default:
				return 1;
		}

		do
		{
			Nu3D::Camera::FadeToTargetTint();

			defaultFadeFramesRemaining -= Renderer::g_frameDelta;

			if (defaultFadeFramesRemaining > 0)
			{
				if (defaultFadeFramesRemaining > 23)
					continue;
			}
			else
			{
				defaultFadeFramesRemaining = 0;
			}

			if ((defaultFadeFramesRemaining + Renderer::g_frameDelta) > 23)
				Nu3D::Camera::SetTint(0, 0, 0, 12);

		} while (defaultFadeFramesRemaining);

		return 1;
	}

	// STUB: TOY2 0x0048E730
	void OneInit()
	{
		g_randDatBufferPtr = g_randDatBuffer;
		SaveManager::Init();
	}

	// STUB: TOY2 0x00490730
	void CheckForQuit() {}

	// FUNCTION: TOY2 0x004CE760
	void InitCfg()
	{
		memset(&g_toyCfgData, 0, sizeof(g_toyCfgData));

		g_toyCfgData.driverIndex = -1;
		g_toyCfgData.detail = 1;
		g_toyCfgData.flags |= 7;
		g_toyCfgData.gammaCorrection = 2.0;
	}

	// FUNCTION: TOY2 0x004CE810
	int32_t ReadCfg()
	{
		InitCfg();

		FILE* fileHandle = fopen("toy2.cfg", "rb");

		if (fileHandle)
		{
			fread(&g_toyCfgData, 1, sizeof(ToyCfg), fileHandle);
			fclose(fileHandle);
			return 1;
		}

		return 0;
	}

	// FUNCTION: TOY2 0x00412B50
	void RunModeSelect()
	{
		if (! g_modeSelectFinished)
		{
			atexit(DrawingDevice::Quit);
			ModeSelect::SetForceFullscreen_T(0);

			if (ModeSelect::EnumerateDrivers_T(ModeSelect::DeviceFilterCallback) < 0)
				Logger::GetErrorHandler("C:\\projects\\toy2\\direct6.cpp", 103)("Unable to enumerate a suitable device");

			ModeSelect::Show();

			DrawingDevice::DDAppDevice* primaryDevice;
			DrawingDevice::DDAppDevice::App* ddApp;

			if (DrawingDevice::GetChosenDevice_T(&ddApp, &primaryDevice))
				Logger::GetErrorHandler("C:\\projects\\toy2\\direct6.cpp", 111)("Unable to create D3D device\r\n try a lower resolution or screen depth");

			int32_t canDoWindowed = primaryDevice->canRenderWindowedOnPrimary;
			int32_t fullscreenExclusive = (ModeSelect::g_unusedFlag1 != 0 ? 2 : 0) | (canDoWindowed == 0) | (ModeSelect::g_unusedFlag2 != 0 ? 4 : 0);

			if (! canDoWindowed)
				Logger::g_showMsgBoxOnThrow = 1;

			if (primaryDevice->isHardwareAccelerated)
			{
				Renderer::SetIsSoftwareRendering(0);
			}
			else
			{
				Renderer::SetIsSoftwareRendering(1);
				while (Toy2::Graphics::RemoveDetailLevel()) {};
			}

			if (! primaryDevice->isHardwareAccelerated && primaryDevice->canRenderWindowedOnPrimary)
			{
				RECT adjustedRect;
				adjustedRect.top = 0;
				adjustedRect.left = 0;
				adjustedRect.right = 320;
				adjustedRect.bottom = 240;

				AdjustWindowRect(&adjustedRect, 0, 0);
				SetWindowPos(D3DApp::g_windowData.mainHwnd, 0, 0, 0, adjustedRect.right, adjustedRect.bottom, 2);
			}

			ShowWindow(D3DApp::g_windowData.mainHwnd, SW_SHOWMAXIMIZED);

			if (DrawingDevice::CD3DFramework::Build(
					D3DApp::g_windowData.mainHwnd, &ddApp->guid, primaryDevice, primaryDevice->primaryDisplayMode, fullscreenExclusive)
				>= 0)
				g_modeSelectFinished = 1;
		}
	}

	// FUNCTION: TOY2 0x0047D8D0
	void UnusedInit()
	{
		// GLOBAL: TOY2 0x00725F20
		static int32_t g_unusedInit;

		g_unusedInit = 2;
	}

	// FUNCTION: TOY2 0x00412D70
	int32_t ShowModeSelect()
	{
		char cdFileName[8];
		char cdTrackBuffer[1024];

		strcpy(cdFileName, "cd.txt");
		memset(cdTrackBuffer, 0, sizeof(cdTrackBuffer));

		int32_t baseCDTrack;

		if (FileUtils::LoadFile(cdFileName, cdTrackBuffer))
			baseCDTrack = atoi(cdTrackBuffer);
		else
			baseCDTrack = 2;

		g_cdBaseTrack = baseCDTrack;
		Logger::Log("CONFIG : Base CD track is %d.\n", baseCDTrack);

		AudioManager::Init();
		UnusedInit();
		Numerics::InitTrigLut();

		RunModeSelect();

		InputManager::Init();
		Renderer::Init();
		Nu3D::Viewport::Init();

		RECT* destRect = DrawingDevice::GetDestRect();

		Nu3D::Font::Init();
		Nu3D::Font* font = Nu3D::Font::Build("ariel", 20, 0);

		if (font)
		{
			Nu3D::Font::SetFont(font);
			Nu3D::Font::SetTextColor(0xFFFFFFFF);
			Nu3D::Font::SetTextClipRect(destRect->left, destRect->top, destRect->right - 1, destRect->bottom - 1);
		}

		while (ShowCursor(FALSE) >= 0) {};

		return 1;
	}

	// FUNCTION: TOY2 0x0049D910
	int32_t Run(int32_t argCount, char** argList)
	{
		g_returnedToTitle = 0;

		g_unused1 = 2;
		g_unused2 = 0;

		g_attractModeTimer = -25;
		g_attractModeInputTimer = -50;

		SoftwareRenderer::SwapRenderBuffer();

		g_pastInitialBoot = 0;
		g_curDemoLevel = 0;
		g_levelTransition = 0;

		SaveManager::g_save0Data.unkData1 = 1;
		SaveManager::g_save0Data.unkData2 = 23;
		SaveManager::g_save0Data.cameraType = SaveManager::CAMERA_PASSIVE | SaveManager::CAMERA_ACTIVE;
		SaveManager::g_save0Data.musicVolume = 8;
		SaveManager::g_save0Data.soundVolume = 8;

		AudioManager::SetVolumesProcessed(8, 8);

		int32_t enteredLevelIdx;
		int32_t levelIdxCache = g_levelFileIndex;

	LBL_RESTART_GAME:

		g_demoMode = 0;
		g_mainMenuState = 0;
		g_levelFileIndex = 0;

		Levels::g_levelLoadConfig = 1084;

		Renderer::SetVirtualRatioTo54();
		Levels::InitLevelPlay(0);
		ScreenDispatcher(10);

		g_levelFileIndex = levelIdxCache;

		if (! PlayMovieWithTransition(2, 0) && ! PlayMovieWithTransition(0, 0) && ! PlayMovieWithTransition(1, 0))
			UnlockAndPlayMovie(0, 0, 1);

		g_pastInitialBoot = 1;

	LBL_RESTART_MENU_STATE:

		SaveManager::InitProgressData(&SaveManager::g_save0Data);
		SaveManager::LoadProgressData(&SaveManager::g_save0Data);

		g_mainMenuState = 0;
		g_saveLoaded = 0;

		LevelSelect::ResetCursor();

	LBL_REDO_MENU_LOOP:

		while (true)
		{
			int32_t savedLevelFileIndex = g_levelFileIndex;
			g_levelFileIndex = 0;
			Levels::g_levelLoadConfig = 1084;

			Renderer::SetVirtualRatioTo54();
			Levels::InitLevelPlay(0);
			ScreenDispatcher(2);

			g_levelFileIndex = savedLevelFileIndex;

			switch (g_mainMenuState)
			{
				case 0:
					g_demoMode = 1;
					goto LBL_DEMO_MODE;

				case 1:
					g_demoMode = 0;
					goto LBL_DEMO_MODE;

				case 2: // Options Screen
					g_levelFileIndex = 0;
					Levels::g_levelLoadConfig = 1212;

					Renderer::SetVirtualRatioTo54();
					Levels::InitLevelPlay(0);
					ScreenDispatcher(9);

					g_levelFileIndex = enteredLevelIdx;
					g_mainMenuState = -1;
					continue;

				case 3: // Save Screen
					if (ShowSaveScreen())
						g_saveLoaded = 1;

					g_mainMenuState = -1;
					continue;

				case 4: // Movie Viewer
					ShowMovieViewer();
					g_mainMenuState = -1;
					continue;

				case 8:
					g_levelFileIndex = 0;
					Levels::g_levelLoadConfig = 1084;

					Renderer::SetVirtualRatioTo54();
					Levels::InitLevelPlay(0);
					ScreenDispatcher(8);

					g_levelFileIndex = enteredLevelIdx;
					g_mainMenuState = 0;
					goto LBL_RESTART_MENU_STATE;

				case 9:
					Nullsub5();
					return 0;

				default:

				LBL_DEMO_MODE:
					g_mainMenuState = 0;

					if (g_demoMode == 1)
					{
						switch (g_curDemoLevel)
						{
							case 0:
								g_levelIndex = 0;
								break;
							case 1:
								g_levelIndex = 3;
								break;
							case 2:
								g_levelIndex = 7;
								break;
							case 3:
								g_levelIndex = 10;
								break;
							case 4:
								g_levelIndex = 13;
								break;
							default:
								break;
						}

						if (++g_curDemoLevel > 4)
							g_curDemoLevel = 0;

						g_levelFileIndex = g_levelFileConversion[g_levelIndex];

						LoadPathBin();

						int32_t levelIdxCache = g_levelIndex;

						g_demoPathWriteIdx = -2;
						g_demoInputRunLength = 0;

						SaveManager::InitProgressData(&SaveManager::g_save0Data);
						SaveManager::LoadProgressData(&SaveManager::g_save0Data);

						g_levelIndex = levelIdxCache;
					}

					if (! g_demoMode && g_attractModeTimer < 0)
						goto LBL_SHOW_LEVEL_SELECT;

					break;
			}

			break;
		}

		while (true)
		{
			g_levelFileIndex = g_levelFileConversion[g_levelIndex];

			if (EnterLevel(g_levelFileIndex))
				break;

			while (true)
			{
				g_attractModeInputTimer = 2 * g_attractModeTimer;

				InputManager::g_curButtonsPressed = 0;
				InputManager::g_prevButtonsPressed = 0;

				g_isPaused = 0;
				g_pauseMenuBlinkTimer = 0;

				InputManager::g_directionInputState = 0;
				InputManager::g_prevDirectionInputState = 0;
				InputManager::g_directionInputState2Frames = 0;
				InputManager::g_directionInputState3Frames = 0;

				Renderer::g_frameDelta = 1;
				g_levelTransition = 0;
				g_levelTransitionTimer = 90;

				Nu3D::Camera::SetTint(128, 128, 128, 6);

				while (! g_levelTransition || g_levelTransitionTimer)
				{
					if (g_demoMode)
						Renderer::g_frameDelta = 2; // Half frame rate in demo mode

					if (g_isPaused)
						Game::PauseLoop();
					else
						Game::MainLoop();
				}

				AudioManager::FlushSoundVoices();

				if (g_levelTransition != 2)
					AudioManager::StopAndWait();

				if (g_quitToTitleFlag)
				{
					if (g_levelTransition == 2)
						AudioManager::StopAndWait();

					goto LBL_RESTART_MENU_STATE;
				}

				if (g_levelTransition != 2)
					break;

				if (! g_buzzActor.lives)
				{
					AudioManager::StopAndWait();
					goto LBL_GAME_OVER;
				}

				Buzz::Respawn();
			}

			if (g_levelTransition == 3 || g_levelTransition == 4)
			{
				if (g_attractModeTimer >= 0)
					break;

				if (g_levelTransition != 3)
					goto LBL_RESTART_MENU_STATE;

				levelIdxCache = g_levelFileIndex;

				goto LBL_RESTART_GAME;
			}

			enteredLevelIdx = g_levelFileIndex;

			if (g_levelTransition != 5)
			{
				if (g_levelTransition != 1)
				{
					if (g_attractModeTimer >= 0)
						break;

					goto LBL_SHOW_LEVEL_SELECT;
				}

				goto LBL_SHOW_LEVEL_RESULTS;
			}

			if (g_buzzActor.health < 0)
			{
				if (! g_buzzActor.lives)
				{
				LBL_GAME_OVER:

					levelIdxCache = g_levelFileIndex;

					g_levelFileIndex = 0;
					Levels::g_levelLoadConfig = 1148;

					Renderer::SetVirtualRatioTo54();

					Levels::InitLevelPlay(0);
					ScreenDispatcher(5);

					g_levelFileIndex = levelIdxCache;

					if (g_attractModeTimer >= 0)
						break;

					goto LBL_RESTART_GAME;
				}

				--g_buzzActor.lives;
			}

			if (g_attractModeTimer >= 0)
				break;

			if (g_levelFileIndex % 3)
			{
				g_levelTransition = 1;

			LBL_SHOW_LEVEL_RESULTS:

				if (g_levelFileIndex % 3)
				{
					g_levelFileIndex = 0;
					Levels::g_levelLoadConfig = 1148;

					Renderer::SetVirtualRatioTo54();
					Levels::InitLevelPlay(0);
					ScreenDispatcher(4);
					g_levelFileIndex = enteredLevelIdx;

					if (g_attractModeTimer >= 0)
						break;

					int32_t movie17Status = SaveManager::g_save0Data.moviesUnlocked[17];

					if (SaveManager::g_curLevelTokenData != SaveManager::g_save0Data.tokens[g_levelFileConversion[g_levelIndex]]
						&& (ComputeTokenProgress() & 0xFF0000) == 0x320000)
					{
						SaveManager::g_save0Data.moviesUnlocked[17] = 1;
					}

					ShowPostGameSaveMenu();

					if (! movie17Status && SaveManager::g_save0Data.moviesUnlocked[17])
						UnlockAndPlayMovie(17, 16, 1);
				}
				else
				{
					ShowActClearScreen();

					int32_t levelMovieStatus = SaveManager::g_save0Data.moviesUnlocked[g_levelIndex + 1];

					SaveManager::g_save0Data.moviesUnlocked[g_levelIndex + 1] = 1;

					if (g_levelFileIndex == 15)
						SaveManager::g_save0Data.moviesUnlocked[18] = 1;

					ShowPostGameSaveMenu();

					if (! levelMovieStatus)
						UnlockAndPlayMovie(g_levelIndex + 1, 30, 1);

					if (g_levelFileIndex == 15)
					{
						UnlockAndPlayMovie(18, 31, 1);
						levelIdxCache = g_levelFileIndex;

						g_levelFileIndex = 0;
						Levels::g_levelLoadConfig = 1276;

						Renderer::SetVirtualRatioTo54();
						Levels::InitLevelPlay(0);
						ScreenDispatcher(11); // Show credits

						goto LBL_RESTART_GAME;
					}
				}
			}

			if (g_attractModeTimer >= 0)
				break;

		LBL_SHOW_LEVEL_SELECT:

			g_mainMenuState = 0;
			g_saveLoaded = 1;

			if (ShowLevelSelect())
			{
				g_mainMenuState = -1;
				goto LBL_REDO_MENU_LOOP;
			}

			if ((g_levelIndex + 1) % 3)
			{
				UnlockAndPlayMovie(g_levelIndex + 1, 0, 0);
			}
			else if (g_levelIndex == 11)
			{
				UnlockAndPlayMovie(16, 0, 0);
			}
		}

		Nullsub5();

		return 0;
	}

	// STUB: TOY2 0x00412E80
	int32_t CleanupManagers() { return 0; }

	// STUB: TOY2 0x0047D7C0
	void UpdateAudioChannels() {}

	// STUB: TOY2 0x00490BF0
	int16_t UpdateD3DState() { return 0; }

	// FUNCTION: TOY2 0x004909E0
	void ProcessMiscEventsEx()
	{
		Nu3D::Font::SetTextCursor(0, Nu3D::g_scaledFontAscent);
		DevDraw::g_vertexCount = 0;

		D3DApp::g_windowData.wndIsExiting = g_wndIsExitingUnused;

		if (g_wndIsExitingUnused)
		{
			Logger::Log("CheckForQuit : Starting shutdown now...\n");

			DestroyWindow(D3DApp::g_windowData.mainHwnd);

			if (D3DApp::g_renderMode == RENDERMODE_SOFTWARE)
			{
				SoftwareRenderer::Destroy();
			}
			else if (D3DApp::g_renderMode == RENDERMODE_D3D)
			{
				Logger::Log("QUIT : Destroying Direct3D renderer.\n");
			}

			CleanupManagers();
			D3DApp::PostQuitMessage();

			CoUninitialize();

			D3DApp::g_windowData.mainHwnd = 0;

			Logger::Log("CheckForQuit : Code shutdown.\n");

			g_clearScreenSaveResult = SystemParametersInfoA(SPI_SCREENSAVERRUNNING, 0, &g_setScreenSaveRunning, 0);

			if (g_clearScreenSaveResult)
				Logger::Log("Managed to clear SCREENSAVERRUNNING\n");
			else
				Logger::Log("Failed to clear SCREENSAVERRUNNING\n");

			exit(D3DApp::g_windowData.wndEventMsg.wParam);
		}

		D3DApp::ProcessWndEvents();

		if (D3DApp::g_windowData.wndIsExiting)
		{
			DrawingDevice::g_drawingDevice->RestoreToGDISurface(1);
			Logger::GetErrorHandler("C:\\projects\\toy2\\toy2.cpp", 4670)("");
		}

		if (InputManager::IsKeyPressed(DIK_F3))
		{
			SoftwareRenderer::ZoomOut();
			g_extraControlsUnused = 15;
		}

		if (InputManager::IsKeyPressed(DIK_F4))
		{
			SoftwareRenderer::ZoomIn();
			g_extraControlsUnused = 15;
		}

		if (InputManager::IsKeyPressed(DIK_F5))
		{
			Graphics::RemoveDetailLevel();
			g_extraControlsUnused = 15;
		}

		if (InputManager::IsKeyPressed(DIK_F6))
		{
			Graphics::AddDetailLevel();
			g_extraControlsUnused = 15;
		}

		InputManager::UpdateButtonStates();

		if (g_inputSuppressFrames)
		{
			InputManager::g_curButtonsPressed = 0;
			--g_inputSuppressFrames;
		}

		UpdateAudioChannels();

		SoftwareRenderer::g_unk830C60 = 0;

		if (D3DApp::g_renderMode == RENDERMODE_SOFTWARE)
		{
			SoftwareRenderer::UnkFunc2();
			SoftwareRenderer::UnkFunc3();
		}
		else if (D3DApp::g_renderMode == RENDERMODE_D3D)
		{
			UpdateD3DState();
		}
	}

	// STUB: TOY2 0x0047CC90
	void InitSoftwareRenderer()
	{
		// Some back story on this, the game has two rendering modes. Hardware accelerated (DirectX) or full software based rendering.
		// This is normal for the time, considering it was uncommon for people to have dedicated graphics hardware as it was expensive.
		//
		// The software renderer will be decompiled last, as it is extremely math heavy and optimized, making it difficult to produce
		// code that can be read by human eyes.
		return;
	}

	// STUB: TOY2 0x00499950
	void InitDirect3DRenderer()
	{
		// Weird method, a good portion of these variables are never even used in the game
	}

	// STUB: TOY2 0x00414270
	void LoadLevelWithFadeIn(int32_t levelId, int32_t flag) {}

	// STUB: TOY2 0x00414320
	int32_t ShowLevelIntroScreen(int32_t levelId, int32_t demo) { return 0; }

	// STUB: TOY2 0x0041E020
	void InterpolatePathPoint(int32_t index, int32_t path, Vector3I* vec) {}

	// STUB: TOY2 0x00421090
	void InitialiseLevel7() {}

	// STUB: TOY2 0x00424390
	void InitialiseLevel9() {}

	// STUB: TOY2 0x00425B60
	int32_t InitialiseLevel10() { return 0; }

	// STUB: TOY2 0x0042E600
	int32_t InitialiseLevel14() { return 0; }

	// STUB: TOY2 0x00430E90
	void ReadIniFile() {}

	// STUB: TOY2 0x00433ED0
	void ResetBuzzState() {}

	// STUB: TOY2 0x0043E5A0
	void InitZoneData() {}

	// STUB: TOY2 0x0044F840
	void ShowModelNode(int32_t creatureIndex, int32_t nodeIdx) {}

	// STUB: TOY2 0x0044F860
	void HideModelNode(int32_t creatureIndex, int32_t nodeIdx) {}

	// STUB: TOY2 0x004500A0
	void LoadLevelGraphics(int32_t level) {}

	// STUB: TOY2 0x00453CA0
	void ResetBackdropState() {}

	// STUB: TOY2 0x0047D4E0
	int32_t* BuildPerspectiveDivideTable(int32_t centerX) { return 0; }

	// STUB: TOY2 0x0047EF80
	void ProfileCPU() {}

	// STUB: TOY2 0x00498550
	void ProcessMiscEvents() {}

	// STUB: TOY2 0x0049A930
	int32_t PlayMovie(int32_t movieId) { return 0; }

	// STUB: TOY2 0x0049B9E0
	int32_t TickSaveMenuMachine(int32_t showSaveMenu) { return 0; }

	// STUB: TOY2 0x0049C420
	int32_t ShowControlConfig() { return 0; }

	// STUB: TOY2 0x0049CAA0
	void ShowGraphicsConfig() {}

	// STUB: TOY2 0x0049EAC0
	void PlayLevelMusic() {}

	// STUB: TOY2 0x0049F490
	void UnusedVarUpdate() {}

	// STUB: TOY2 0x0049FD40
	void RenderHUD() {}

	// STUB: TOY2 0x004A1A50
	void InitialiseLevelVariables(int32_t levelIndex) {}

	// STUB: TOY2 0x004A1B00
	void HandleLevelInteractions(int32_t levelId) {}

	// STUB: TOY2 0x004A2960
	void UpdateFrameTimers() {}

	// STUB: TOY2 0x004A48B0
	void ResetGadgets() {}

	// STUB: TOY2 0x004A4B70
	void ActivateGravityBoots() {}

	// STUB: TOY2 0x004A5070
	void RespawnCosmicShield() {}

	// STUB: TOY2 0x004A5D30
	Toy2::BeamShot* SpawnBeamShot(
		int32_t beamType,
		int16_t aimYaw,
		int16_t aimPitch,
		WorldPos* origin,
		WorldPos* headOffset,
		int32_t range,
		Toy2::Actor::Toy2Actor* autoAimActor)
	{
		return 0;
	}

	// STUB: TOY2 0x004CD110
	int32_t ShowTokenSparkle() { return 0; }

	namespace PostGameSaveMenu
	{
		// STUB: TOY2 0x0043A130
		int32_t Tick() { return 0; }
	}

	namespace MovieViewer
	{
		// STUB: TOY2 0x0043A600
		int32_t Tick(int32_t a1) { return 0; }
	}

	namespace Ini
	{
		// STUB: TOY2 0x00430BF0
		void ParseDefText() {}

		// STUB: TOY2 0x00415640
		int16_t KeyNameToScancode(char keyName) { return 0; }
	}

	// STUB: TOY2 0x0049FC60
	int32_t TickSlide(int32_t widgetIndex) { return 0; }
}

// $FUNC DEBUG
void AllocateConsole()
{
	AllocConsole();

	FILE* fp;

	// redirect STDOUT
	fp = freopen("CONOUT$", "w", stdout);
	fp = freopen("CONOUT$", "w", stderr);

	// redirect STDIN
	fp = freopen("CONIN$", "r", stdin);

	printf("[Debug Console Allocated!]\n");
}

// FUNCTION: TOY2 0x004316C0
int32_t WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, char* cmdLine, int32_t cmdShow)
{
#ifdef APPLY_FIXES
	AllocateConsole();
#endif

	Toy2::g_unused0 = 0;

	memset(&D3DApp::g_d3dAppI, 0, sizeof(D3DApp::g_d3dAppI));

	D3DApp::g_no32bitColors = 1;

	FileUtils::ValidateInstall();

	Toy2::g_levelFileIndex = 1;

	memset(&D3DApp::g_windowData, 0, sizeof(D3DApp::g_windowData));

	D3DApp::g_windowData.hInstance = hInstance;
	D3DApp::g_windowData.hPrev = hPrev;
	D3DApp::g_windowData.lpCmdLine = cmdLine;
	D3DApp::g_windowData.nShowCmd = cmdShow;

	D3DApp::g_windowData.unkInt8 = 0;
	D3DApp::g_windowData.unkInt3 = 1;
	D3DApp::g_windowData.unkInt4 = 1;
	D3DApp::g_windowData.unkInt5 = 1;
	D3DApp::g_windowData.wndIsExiting = 0;

	Toy2::OneInit();

	Toy2::g_returnedToTitle = 0;
	Toy2::g_attractModeTimer = -1;

	Toy2::g_unused1 = 2;
	Toy2::g_unused2 = 0;

	Toy2::ReadCfg();
	D3DApp::BuildProfileMachine();
	D3DApp::BuildWindow();
	Toy2::ShowModeSelect();

	switch (D3DApp::g_renderMode)
	{
		case RENDERMODE_SOFTWARE:
			Toy2::InitSoftwareRenderer();
			break;
		case RENDERMODE_D3D:
			Toy2::InitDirect3DRenderer();
			break;
	}

	int32_t tokenCount = 0;
	char* currentToken;
	char* tokenEntries[8];

	currentToken = strtok(cmdLine, " ");

	if (currentToken)
	{
		tokenCount = 1;
		char* nextToken = strtok(0, " ");

		if (nextToken)
		{
			char** tokenArrayPtr = tokenEntries;

			do
			{
				*tokenArrayPtr = nextToken;
				++tokenCount;
				++tokenArrayPtr;

				nextToken = strtok(0, " ");
			} while (nextToken);
		}
	}

	Toy2::Run(tokenCount, &currentToken);

	D3DApp::g_windowData.wndIsExiting = 1;

	Toy2::CheckForQuit();

	return 0;
}
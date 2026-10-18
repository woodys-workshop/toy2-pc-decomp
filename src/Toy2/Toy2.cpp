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
#include "Numerics.h"

#include "Toy2/LevelSelect.h"
#include "Toy2/Buzz.h"
#include "Toy2/Levels.h"
#include "Toy2/MainMenu.h"
#include "Toy2/Actor.h"
#include "Toy2/CreatureBehaviour.h"
#include "Toy2/Animation.h"
#include "Toy2/Collision.h"
#include "Toy2/KiteTail.h"
#include "Toy2/Sector.h"
#include "Toy2/Camera.h"
#include "Toy2/Cutscene.h"
#include "Toy2/Shadow.h"
#include "Toy2/Lighting.h"
#include "Toy2/Gadget.h"
#include "Toy2/Level.h"
#include "Toy2/Collectables.h"
#include "Toy2/MoveableObject.h"
#include "Toy2/Platform.h"
#include "Toy2/Weather.h"

// Level Includes
#include "Toy2/Levels/AndysHouse.h"
#include "Toy2/Levels/AndysNeighborhood.h"
#include "Toy2/Levels/BombsAway.h"
#include "Toy2/Levels/ConstructionYard.h"
#include "Toy2/Levels/AlleysAndGullies.h"
#include "Toy2/Levels/SlimeTime.h"
#include "Toy2/Levels/AlsToyBarn.h"
#include "Toy2/Levels/AlsSpaceLand.h"
#include "Toy2/Levels/BarnEncounter.h"
#include "Toy2/Levels/ElevatorHop.h"
#include "Toy2/Levels/AlsPenthouse.h"
#include "Toy2/Levels/EvilEmperorZurg.h"
#include "Toy2/Levels/AirportInfiltration.h"
#include "Toy2/Levels/TarmacTrouble.h"
#include "Toy2/Levels/FinalShowdown.h"

#include "Nu3D/Font.h"
#include "Nu3D/FMV.h"
#include "Nu3D/Viewport.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Light.h"
#include "Nu3D/Link.h"
#include "Nu3D/Collision.h"
#include "Nu3D/Math.h"
#include "Nu3D/Scene.h"
#include "Nu3D/Particles.h"
#include "Renderer/Renderer.h"
#include "Renderer/Effects.h"
#include "Renderer/Glue.h"
#include "Renderer/Shadows.h"
#include "Renderer/Sprite.h"
#include "Renderer/SpriteSheets.h"
#include "AudioManager/AudioManager.h"
#include "NGNLoader/NGNLoader.h"
#include "RawLoader.h"

#include <WINDOWS.H>
#include <STDIO.H>
#include <STRING.H>
#include <STDLIB.H>
#include <MATH.H>
#include <DINPUT.H>

namespace Toy2
{
	// GLOBAL: TOY2 0x0072E34C
	int32_t g_mpegPlay;

	// GLOBAL: TOY2 0x0072EFB0
	uint32_t g_cpuTime;

	// GLOBAL: TOY2 0x0072EF94
	int32_t g_cpuTicks;

	// GLOBAL: TOY2 0x00731F0C
	uint32_t g_cpuSpeed;

	// GLOBAL: TOY2 0x0055A120
	int32_t g_unk55A120;

	// GLOBAL: TOY2 0x00534078
	int32_t g_cheatsEnabled;

	// GLOBAL: TOY2 0x00830C20
	int32_t g_highQualityMpegEnabled;

	// GLOBAL: TOY2 0x00534560
	int32_t g_isDemo;

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
	int16_t g_levelTransition;

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

	// GLOBAL: TOY2 0x005028A8
	int16_t g_musicSliderToAmplitude[16] = { 0, 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126, 128 };

	// GLOBAL: TOY2 0x005028C8
	int16_t g_soundSliderToAmplitude[16] = { 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120 };

	// GLOBAL: TOY2 0x00528160
	char g_emptyStr[8];

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

	// GLOBAL: TOY2 0x0052F2D8
	uint16_t g_playerUnlocks;

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

	// GLOBAL: TOY2 0x0072E354
	SaveNameBuffer g_saveNameBuffers[8];

	// GLOBAL: TOY2 0x00731F18
	int32_t g_savePromptPending;

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

	// GLOBAL: TOY2 0x0052C840
	Actor::Toy2Actor g_levelActors[64];

	// GLOBAL: TOY2 0x004F59A4
	SpawnLocation g_spawnLocations[17] = {
		{ 0, 0, 0, 0 },
		{ 194774, 60044, -361401, 0 },
		{ -466652, 433, -32144, 1280 },
		{ -47074, 97, -185901, 0 },
		{ 411295, 11, 30702, 3154 },
		{ 204615, 75, 114100, 2946 },
		{ 264357, -1082, 2748, 3072 },
		{ -128033, 76, -332419, 0 },
		{ -4082, 40, 315274, 2047 },
		{ 16532, 70, -141912, 4066 },
		{ -291, -32978, -646, 1024 },
		{ -262581, 174040, 253369, 1751 },
		{ -121468, -76752, 63129, 1048 },
		{ 27126, -94, 240599, 3068 },
		{ -320338, 76, 983296, 1937 },
		{ -36601, 69, -5416, 3066 },
		{ 0, -561600, -844800, 0 },
	};

	// GLOBAL: TOY2 0x0052F1C0
	uint8_t g_tickState[12];

	// GLOBAL: TOY2 0x0052AD58
	TickAccumulators g_tickAccumulators;

	// GLOBAL: TOY2 0x0052F120
	SoundSequencePlayer g_soundSequencePlayers[8];

	// GLOBAL: TOY2 0x0052AD80
	int32_t g_curSequencePlayer;

	// GLOBAL: TOY2 0x0052F2E0
	int16_t g_hudWidgetPositions[12];

	// GLOBAL: TOY2 0x0052C824
	int16_t g_hudWidgetTimers[11];

	// GLOBAL: TOY2 0x0052B7E4
	int16_t g_gameMenuScrollIdx;

	// GLOBAL: TOY2 0x0052ADB0
	int16_t g_pauseMenuType;

	// GLOBAL: TOY2 0x0052EF44
	int16_t g_feedbackTimerA;

	// GLOBAL: TOY2 0x0052EF46
	int16_t g_feedbackTimerB;

	// GLOBAL: TOY2 0x0052F2DA
	int16_t g_feedbackTimerA_sub;

	// GLOBAL: TOY2 0x0052B7E0
	int32_t g_unk52B7E0;

	// GLOBAL: TOY2 0x0052B7D8
	int32_t g_collectionCount;

	// GLOBAL: TOY2 0x00830E38
	int32_t g_unk830E38;

	// GLOBAL: TOY2 0x00830D4C
	int16_t g_unk830D4C;

	// GLOBAL: TOY2 0x0050A0BC
	int32_t g_unk50A0BC;

	// GLOBAL: TOY2 0x0050A0B0
	int32_t g_unk50A0B0;

	// GLOBAL: TOY2 0x00830D2D
	uint8_t g_underwaterTintBlue;

	// GLOBAL: TOY2 0x00830D20
	uint8_t g_underwaterTintGreen;

	// GLOBAL: TOY2 0x00830D2C
	uint8_t g_underwaterTintRed;

	// GLOBAL: TOY2 0x0052F2F8
	int32_t g_unk52F2F8;

	// GLOBAL: TOY2 0x0052AD64
	int32_t g_unk52AD64;

	// GLOBAL: TOY2 0x0052AD68
	int16_t g_unused3;

	// GLOBAL: TOY2 0x0052AD6A
	int16_t g_unk52AD6A;

	// GLOBAL: TOY2 0x0052AD6C
	int16_t g_unk52AD6C;

	// GLOBAL: TOY2 0x0052AD6E
	int16_t g_unk52AD6E;

	// GLOBAL: TOY2 0x0052AD70
	int16_t g_unk52AD70;

	// GLOBAL: TOY2 0x0052AD72
	int16_t g_unk52AD72;

	// GLOBAL: TOY2 0x0052AD74
	int16_t g_unk52AD74;

	// GLOBAL: TOY2 0x0052AD76
	int16_t g_unk52AD76;

	// GLOBAL: TOY2 0x0052AD78
	int16_t g_unk52AD78;

	// GLOBAL: TOY2 0x0052AD7A
	int16_t g_unk52AD7A;

	// GLOBAL: TOY2 0x00830D44
	int32_t g_unk830D44;

	// GLOBAL: TOY2 0x00830D38
	int32_t g_unk830D38;

	// GLOBAL: TOY2 0x00830C90
	int32_t g_unk830C90;

	// GLOBAL: TOY2 0x00830E2C
	Nu3D::Particles::ParticleInstance* g_unk830E2C;

	// GLOBAL: TOY2 0x0052B820
	int16_t g_demoPathBuffer[2048];

	// GLOBAL: TOY2 0x0052AD84
	int32_t g_unk52AD84;

	// GLOBAL: TOY2 0x00830CC4
	int32_t g_unk830CC4;

	// GLOBAL: TOY2 0x00830CB0
	Nu3D::Camera::ActiveCameraTransform g_unk830CB0;

	// GLOBAL: TOY2 0x00830CC8
	int32_t g_devCamState;

	// GLOBAL: TOY2 0x00882970
	BeamShot g_beamShots[4];

	// GLOBAL: TOY2 0x00882934
	Levels::Type63* g_cosmicShieldPickup;

	// GLOBAL: TOY2 0x00882958
	int32_t g_cosmicShieldPickupYPos;

	// GLOBAL: TOY2 0x0088295C
	int32_t g_cosmicShieldRotYaw;

	// GLOBAL: TOY2 0x00882960
	int32_t g_cosmicShieldSpin;

	// GLOBAL: TOY2 0x00882924
	Levels::Type63* g_rocketBootsPickup;

	// GLOBAL: TOY2 0x0053C5E0
	int32_t g_rocketBootsDuration;

	// GLOBAL: TOY2 0x00882938
	int32_t g_grappleTimer;

	// GLOBAL: TOY2 0x00882950
	int32_t g_grappleState;

	// GLOBAL: TOY2 0x0088292C
	int32_t g_grappleDistance;

	// GLOBAL: TOY2 0x00882930
	int32_t g_grappleProgress;

	// GLOBAL: TOY2 0x00882964
	int32_t g_discLauncherTimer;

	// GLOBAL: TOY2 0x00882968
	int32_t g_discLauncherAmmo;

	// GLOBAL: TOY2 0x00882920
	int32_t g_gravityBootsDuration;

	// GLOBAL: TOY2 0x00882928
	int32_t g_gravityBootsHoverHeight;

	// GLOBAL: TOY2 0x00882954
	int32_t g_rocketBootsPickupY;

	// GLOBAL: TOY2 0x00882940
	Transform g_grappleTargetPos;

	// GLOBAL: TOY2 0x00830C94
	int32_t g_unk830C94;

	// GLOBAL: TOY2 0x00830CA0
	int32_t g_unk830CA0;

	// GLOBAL: TOY2 0x00830E20
	int32_t g_bgmVolumeUI;

	// GLOBAL: TOY2 0x00830E30
	int32_t g_sfxVolumeUI;

	// the number of entries of every pause menu
	// GLOBAL: TOY2 0x005039BC
	uint8_t g_scrollCountsPerMenu[10] = { 4, 2, 2, 2, 2, 0, 0, 0, 0, 0 };

	// GLOBAL: TOY2 0x00502750
	char g_sfxVolumeStr[15] = "sfx **********";

	// GLOBAL: TOY2 0x00502760
	char g_bgmVolumeStr[15] = "bgm **********";
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
		// FUNCTION: TOY2 0x0049F4B0
		void MenuLoop()
		{
			int16_t scrollIdx;

			int16_t buttons = InputManager::g_curButtonsPressed;
			int32_t closeMenu = 0;

			if ((InputManager::g_curButtonsPressed & 1) == 0 || (InputManager::g_prevButtonsPressed & 1) != 0)
			{
				scrollIdx = g_gameMenuScrollIdx;
			}
			else
			{
				AudioManager::PlaySoundEffect(61, 0);
				buttons = InputManager::g_curButtonsPressed;
				scrollIdx = 0;
				g_pauseMenuType = MENU_SHORTENEDPAUSE;
				g_gameMenuScrollIdx = 0;
			}

			if ((buttons & INPUT_DOWN) != 0 && (InputManager::g_prevButtonsPressed & INPUT_DOWN) == 0 && scrollIdx < g_scrollCountsPerMenu[g_pauseMenuType] - 1)
			{
				AudioManager::PlaySoundEffect(63, 0);
				buttons = InputManager::g_curButtonsPressed;
				scrollIdx = ++g_gameMenuScrollIdx;
				g_devCamState = 3780;
			}

			if ((buttons & INPUT_UP) != 0 && (InputManager::g_prevButtonsPressed & INPUT_UP) == 0 && scrollIdx > 0)
			{
				AudioManager::PlaySoundEffect(63, 0);
				buttons = InputManager::g_curButtonsPressed;
				scrollIdx = --g_gameMenuScrollIdx;
				g_devCamState = 3780;
			}

			BOOL cancelPressed = (buttons & INPUT_CANCEL) != 0 && (InputManager::g_prevButtonsPressed & INPUT_CANCEL) == 0;

			if ((buttons & INPUT_JUMP) != 0 && (InputManager::g_prevButtonsPressed & INPUT_JUMP) == 0 || cancelPressed)
			{
				switch (g_pauseMenuType)
				{
					case MENU_MAIN:
						if (cancelPressed)
							goto LBL_TEST_VOL_SELECT;

						if (! scrollIdx)
						{
							AudioManager::PlaySoundEffect(62, 0);
							g_isPaused = 0;

							AudioManager::PlayMusicLooping(AudioManager::g_curTrackIndex);

							if (! g_buzzActor.unkWord15)
								g_buzzActor.unkWord15 = 5;

							scrollIdx = g_gameMenuScrollIdx;
							Buzz::g_unk53C658 = 10;

							closeMenu = 1;
						}

						if (scrollIdx == 1)
						{
							AudioManager::PlaySoundEffect(61, 0);
							g_pauseMenuType = 1;
							g_gameMenuScrollIdx = 0;

							break;
						}

						if (scrollIdx == 2)
						{
							g_bgmVolumeUI = SaveManager::g_save0Data.musicVolume;
							g_sfxVolumeUI = SaveManager::g_save0Data.soundVolume;
							AudioManager::PlaySoundEffect(61, 0);
							scrollIdx = 0;
							g_pauseMenuType = MENU_VOLUME_CONTROL;
							g_gameMenuScrollIdx = 0;

							goto LBL_VOL_SELECT;
						}

						if (scrollIdx != 3)
							goto LBL_TEST_VOL_SELECT;

						AudioManager::PlaySoundEffect(61, 0);
						g_pauseMenuType = MENU_AREYOUSURE;
						g_gameMenuScrollIdx = 0;

						break;

					case MENU_CAMERA:
						if (scrollIdx)
						{
							if (scrollIdx == 1)
							{
								if (! cancelPressed)
									SaveManager::g_save0Data.cameraType |= SaveManager::CAMERA_ACTIVE;

								g_pauseMenuType = MENU_MAIN;
								g_gameMenuScrollIdx = 0;
							}
						}
						else
						{
							if (! cancelPressed)
								SaveManager::g_save0Data.cameraType &= SaveManager::CAMERA_PASSIVE | SaveManager::CAMERA_MASK;

							g_pauseMenuType = MENU_MAIN;
						}

						AudioManager::PlaySoundEffect(62, 0);

						goto LABEL_41;

					case MENU_VOLUME_CONTROL:
						if (scrollIdx)
						{
							if (scrollIdx == 1)
							{
								g_pauseMenuType = MENU_MAIN;
								g_gameMenuScrollIdx = 0;
							}
						}
						else
						{
							g_pauseMenuType = MENU_MAIN;
						}

						if (! cancelPressed)
						{
							SaveManager::g_save0Data.soundVolume = g_sfxVolumeUI;
							SaveManager::g_save0Data.musicVolume = g_bgmVolumeUI;
						}

						AudioManager::SetVolumes(
							2 * g_musicSliderToAmplitude[SaveManager::g_save0Data.musicVolume] / 3,
							3 * g_soundSliderToAmplitude[SaveManager::g_save0Data.soundVolume] / 2);

						AudioManager::PlaySoundEffect(62, 0);

						goto LABEL_41;

					case MENU_AREYOUSURE:
						if (cancelPressed)
						{
							g_gameMenuScrollIdx = 0;
						}
						else if (scrollIdx)
						{
							goto LABEL_72;
						}

						g_pauseMenuType = MENU_MAIN;
						AudioManager::PlaySoundEffect(62, 0);

						scrollIdx = g_gameMenuScrollIdx;
						g_quitToTitleFlag = 0;

					LABEL_72:
						if (scrollIdx == 1)
						{
							g_isPaused = 0;
							AudioManager::PlaySoundEffect(61, 0);
							closeMenu = 1;

							if (g_levelTransition != 1)
								g_levelTransition = 5;

							g_buzzActor.actorFlags |= Actor::ACTOR_LOCKED_MOVE;

							g_levelTransitionTimer = 46;
							Nu3D::Camera::g_targetTintBlue = 0;
							Nu3D::Camera::g_targetTintGreen = 0;
							Nu3D::Camera::g_targetTintRed = 0;
							Nu3D::Camera::g_targetTintFadeSpeed = 6;
							Nu3D::Camera::g_tintBlend = -1;

						LABEL_41:
							scrollIdx = g_gameMenuScrollIdx;
						}

						goto LBL_TEST_VOL_SELECT;

					case MENU_SHORTENEDPAUSE:
						if (cancelPressed)
						{
							g_gameMenuScrollIdx = 0;
						}
						else if (scrollIdx)
						{
							goto LABEL_82;
						}

						AudioManager::PlaySoundEffect(62, 0);
						g_isPaused = 0;
						AudioManager::PlayMusicLooping(AudioManager::g_curTrackIndex);

						if (! g_buzzActor.unkWord15)
							g_buzzActor.unkWord15 = 5;

						scrollIdx = g_gameMenuScrollIdx;
						Buzz::g_unk53C658 = 10;
						closeMenu = 1;

					LABEL_82:
						if (scrollIdx != 1)
							goto LBL_TEST_VOL_SELECT;

						AudioManager::PlaySoundEffect(61, 0);
						g_pauseMenuType = MENU_AREYOUSURE;
						g_gameMenuScrollIdx = 0;
						g_quitToTitleFlag = 1;

						break;

					default:
						goto LBL_TEST_VOL_SELECT;
				}
			}
			else
			{
			LBL_TEST_VOL_SELECT:
				if (g_pauseMenuType == MENU_VOLUME_CONTROL)
				{
				LBL_VOL_SELECT:
					int32_t sfxVolume = g_sfxVolumeUI;
					int32_t sfxCharIdx = 0;

					// the bars of the volume sliders are made out of stars
					if (g_sfxVolumeUI <= 0 || (memset(&g_sfxVolumeStr[4], '*', g_sfxVolumeUI), sfxCharIdx = sfxVolume, sfxVolume < 10))
						memset(&g_sfxVolumeStr[sfxCharIdx + 4], ' ', 10 - sfxCharIdx);

					int32_t bgmVolume = g_bgmVolumeUI;
					int32_t bgmCharIdx = 0;

					if (g_bgmVolumeUI <= 0 || (memset(&g_bgmVolumeStr[4], '*', g_bgmVolumeUI), bgmCharIdx = bgmVolume, bgmVolume < 10))
						memset(&g_bgmVolumeStr[bgmCharIdx + 4], ' ', 10 - bgmCharIdx);

					int32_t targetVolume = sfxVolume;

					if (scrollIdx)
						targetVolume = bgmVolume;

					if ((InputManager::g_curButtonsPressed & INPUT_RIGHT) != 0 && (InputManager::g_prevButtonsPressed & INPUT_RIGHT) == 0 && targetVolume < 10)
					{
						++targetVolume;
						AudioManager::PlaySoundEffect(61, 0);
						scrollIdx = g_gameMenuScrollIdx;
						bgmVolume = g_bgmVolumeUI;
						sfxVolume = g_sfxVolumeUI;
					}

					if ((InputManager::g_curButtonsPressed & INPUT_LEFT) != 0 && (InputManager::g_prevButtonsPressed & INPUT_LEFT) == 0 && targetVolume > 0)
					{
						--targetVolume;
						AudioManager::PlaySoundEffect(61, 0);
						scrollIdx = g_gameMenuScrollIdx;
						bgmVolume = g_bgmVolumeUI;
						sfxVolume = g_sfxVolumeUI;
					}

					if (scrollIdx)
					{
						bgmVolume = targetVolume;
						g_bgmVolumeUI = targetVolume;
					}
					else
					{
						sfxVolume = targetVolume;
						g_sfxVolumeUI = targetVolume;
					}

					AudioManager::SetVolumes(2 * g_musicSliderToAmplitude[bgmVolume] / 3, 3 * g_soundSliderToAmplitude[sfxVolume] / 2);
				}
			}

			if ((InputManager::g_curButtonsPressed & INPUT_MENU) == 0 || (InputManager::g_prevButtonsPressed & INPUT_MENU) != 0)
			{
				if (! closeMenu)
					return;
			}
			else
			{
				AudioManager::PlaySoundEffect(62, 0);
				g_isPaused = 0;
				AudioManager::PlayMusicLooping(AudioManager::g_curTrackIndex);

				if (! g_buzzActor.unkWord15)
					g_buzzActor.unkWord15 = 5;

				Buzz::g_unk53C658 = 10;
			}

			memcpy(&Camera::g_activeCameraTransform, &g_unk830CB0, sizeof(Camera::g_activeCameraTransform));
		}

		// FUNCTION: TOY2 0x0049E330
		void PauseLoop()
		{
			int32_t devCamState;
			int32_t x;
			int32_t y;
			int32_t z;
			int32_t angleDiff;
			Transform startPos;
			Transform displacement;

			int32_t frameDelta = Renderer::g_frameDelta;

			if (g_pauseMenuBlinkTimer < 100)
				g_pauseMenuBlinkTimer = (g_pauseMenuBlinkTimer + Renderer::g_frameDelta) & 0x3F;

			if (g_attractModeTimer >= 0)
			{
				int32_t timer = g_returnedToTitle || ! InputManager::g_curButtonsPressed ? g_attractModeInputTimer : 2 * g_attractModeTimer;
				g_attractModeInputTimer = timer - Renderer::g_frameDelta;

				if (timer - Renderer::g_frameDelta <= 0 || (InputManager::g_curButtonsPressed & INPUT_SECRET_MENU) != 0)
				{
					g_isPaused = 0;
					g_levelTransition = 4;
					g_levelTransitionTimer = 46;
					Nu3D::Camera::SetTint(0, 0, 0, 6);
					frameDelta = Renderer::g_frameDelta;
				}
			}

			if (g_devCamState <= 3600)
			{
				devCamState = g_devCamState - frameDelta;

				g_devCamState -= frameDelta;
				goto LABEL_13;
			}

			// holding the camera right and visor buttons for a while is a cheat
			if (InputManager::g_curButtonsPressed != (INPUT_CAMERA_RIGHT | INPUT_VISOR_TOGGLE))
			{
				g_devCamState = 3780;

				goto LABEL_25;
			}

			devCamState = g_devCamState - frameDelta;
			g_devCamState = devCamState;

			if (devCamState > 3600)
			{
			LABEL_13:
				if (devCamState > 0)
					goto LABEL_25;

				goto LABEL_14;
			}

			if (g_buzzActor.coinsCollected == 7)
			{
				g_buzzActor.lives = 9;
				g_devCamState = 0;
			}
			else
			{
				if (g_buzzActor.coinsCollected == 5)
					g_buzzActor.health = 14;

				g_devCamState = 0;
			}

		LABEL_14:
			if ((Camera::g_inCutscene & 1) == 0 && ! Camera::g_cameraState)
			{
				startPos.pos.x = g_buzzActor.posAngles.pos.x;
				Camera::g_activeCameraTransform.angles.yaw = (Camera::g_activeCameraTransform.angles.yaw + 8 * frameDelta) & 0xFFF;
				startPos.pos.y = g_buzzActor.posAngles.pos.y - 0x4000;
				startPos.pos.z = g_buzzActor.posAngles.pos.z;

				displacement.pos.x = -2 * Numerics::g_sinCosLUT[Camera::g_activeCameraTransform.angles.yaw];
				displacement.pos.z = -2 * Numerics::g_sinCosLUT[(Camera::g_activeCameraTransform.angles.yaw + 1024) & 0xFFF];
				g_devCamState = 0;
				displacement.pos.y = 0;

				Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 0, 256);
				frameDelta = Renderer::g_frameDelta;

				x = displacement.pos.x + startPos.pos.x;
				y = displacement.pos.y + startPos.pos.y;
				z = displacement.pos.z + startPos.pos.z;

				startPos.pos.x += displacement.pos.x;
				startPos.pos.y += displacement.pos.y;
				startPos.pos.z += displacement.pos.z;

				goto LABEL_28;
			}

		LABEL_25:
			x = g_unk830CB0.pos.x;
			y = g_unk830CB0.pos.y;
			z = g_unk830CB0.pos.z;

			Camera::g_activeCameraTransform.angles.yaw &= 0xFFF;
			angleDiff = (Camera::g_activeCameraTransform.angles.yaw - g_unk830CB0.angles.yaw) & 0xFFF;

			startPos.pos.x = g_unk830CB0.pos.x;
			startPos.pos.y = g_unk830CB0.pos.y;
			startPos.pos.z = g_unk830CB0.pos.z;

			if (angleDiff > 0x800)
				angleDiff -= 4096;

			Camera::g_activeCameraTransform.angles.yaw += (angleDiff * frameDelta) / -16;

		LABEL_28:
			Camera::g_activeCameraTransform.pos.x += frameDelta * (Camera::g_activeCameraTransform.pos.x - x) / -16;
			Camera::g_activeCameraTransform.pos.y += frameDelta * (Camera::g_activeCameraTransform.pos.y - y) / -16;
			Camera::g_activeCameraTransform.pos.z += frameDelta * (Camera::g_activeCameraTransform.pos.z - z) / -16;

			Nu3D::Camera::ApplyTransformToCamera(&Camera::g_activeCameraTransform);

			RenderHUD();
			Nullsub3();
			RenderGame(1);

			MenuLoop();
		}

		// FUNCTION: TOY2 0x00406CD0
		void InitActor(Toy2::Actor::Toy2Actor* actor, int32_t initialSpawn)
		{
			RawLoader::CreatureListRam* creatureRam = actor->creatureRam;
			int16_t initActorFlags;

			actor->creatureId = creatureRam->creatureId;
			actor->transform.pos.x = 32 * creatureRam->pos.x;
			actor->transform.pos.y = 32 * creatureRam->pos.y;
			actor->transform.pos.z = 32 * creatureRam->pos.z;
			actor->movementData = Actor::g_movementData[creatureRam->movCtrl];
			actor->actorPhase = creatureRam->entCtrl.actorPhase;

			if (initialSpawn)
			{
				initActorFlags = creatureRam->entCtrl.actorFlags;
			}
			else
			{
				initActorFlags = creatureRam->entCtrl.actorFlags;

				if ((actor->actorFlags & Actor::ACTOR_NO_DESPAWN) != 0)
					initActorFlags += Actor::ACTOR_NO_DESPAWN;
			}

			int32_t actorPosX = actor->transform.pos.x;
			int32_t actorPosZ = actor->transform.pos.z;
			actor->actorFlags = initActorFlags;
			actor->unkShort2 = creatureRam->entCtrl.unkByte;

			int32_t actorPosY = actor->transform.pos.y;

			bool flag = actor->unkShort2 == 100;

			actor->boundary.x = actorPosX;
			actor->motionTargetPos.x = actorPosX;
			actor->boundary.y = actorPosY;
			actor->boundary.z = actorPosZ;
			actor->motionTargetPos.y = actorPosY;
			actor->motionTargetPos.z = actorPosZ;

			if (flag)
				actor->unkShort2 = 1800;

			actor->transform.angles.pitch = 0;

			int16_t initialFacingAngle = 16 * creatureRam->initialFacingAngle;

			actor->transform.angles.yaw = initialFacingAngle;
			actor->rollAngle = 0;
			actor->targetYaw = initialFacingAngle;
			actor->velX = 0;
			actor->gravityVel = 0;
			actor->velForward = 0;
			actor->unkVar7 = 0;
			actor->primaryAnimIdx = 0;
			actor->secondaryAnimIdx = -1;
			actor->unkVar30 = (int32_t)Actor::g_unk4E0588[1];

			actor->unkShort4 = 0;
			actor->unkWord15 = 0;
			actor->unkShort1 = 0;
			actor->damageCooldownTimer = 0;
			actor->unkVar9 = 0x80000000;
			actor->lastValidYPosition = actorPosY;

			if (initialSpawn)
			{
				actor->visibilityDistance = 1280;

				switch (creatureRam->creatureId)
				{
					case Actor::CREATURE_WOODY:
					case Actor::CREATURE_MRPOT:
					case Actor::CREATURE_2_SLIME:
					case Actor::CREATURE_2_SHINY:
					case Actor::CREATURE_3_HOOLA:
					case Actor::CREATURE_3_LOCK:
					case Actor::CREATURE_3_MUM:
					case Actor::CREATURE_3_ROOST:
					case Actor::CREATURE_4_MOTHER:
					case Actor::CREATURE_4_FSAUCE:
					case Actor::CREATURE_4_JESSIE:
					case Actor::CREATURE_4_BULLSEYE:
					case Actor::CREATURE_4_ROCKY:
					case Actor::CREATURE_5_ZPOD:
					case Actor::CREATURE_5_BUB:
					case Actor::CREATURE_5_ZEMP:
					case Actor::CREATURE_6_REX:
						actor->visibilityDistance = 1800;
						break;
					case Actor::CREATURE_ZURG3:
						actor->actorBehaviour = CreatureBehaviour::Zurg3;
						break;
					case Actor::CREATURE_TINMAN:
						actor->visibilityDistance = 1800;
						actor->actorBehaviour = CreatureBehaviour::TinMan;
						break;
					case Actor::CREATURE_SHEEP:
						actor->visibilityDistance = 1800;
						actor->actorBehaviour = CreatureBehaviour::Sheep;
						break;
					case Actor::CREATURE_RC:
						actor->visibilityDistance = 1800;
						actor->actorBehaviour = CreatureBehaviour::RCCar;
						break;
					case Actor::CREATURE_2_LAWN:
						actor->visibilityDistance = 1800;
						actor->actorBehaviour = CreatureBehaviour::LawnMower;
						break;
					case Actor::CREATURE_ARMY:
						actor->actorBehaviour = CreatureBehaviour::Army;
						break;
					case Actor::CREATURE_2_ZGCAR:
						actor->visibilityDistance = 1800;
						actor->actorBehaviour = CreatureBehaviour::ZgCar;
						break;
					case Actor::CREATURE_2_ZKITE:
						actor->actorBehaviour = CreatureBehaviour::ZKite;
						break;
					case Actor::CREATURE_2_LTYKE:
						actor->actorBehaviour = CreatureBehaviour::LTyke;
						break;
					case Actor::CREATURE_2_ZPOD:
						actor->actorBehaviour = CreatureBehaviour::ZPod;
						break;
					case Actor::CREATURE_2_DRILL:
						actor->visibilityDistance = 1800;
						actor->actorBehaviour = CreatureBehaviour::Drill;
						break;
					case Actor::CREATURE_2_MOUSE:
						actor->actorBehaviour = CreatureBehaviour::Mouse;
						break;
					case Actor::CREATURE_3_BPLANE:
						actor->actorPhase = 0;
						actor->actorBehaviour = CreatureBehaviour::BPlane;
						actor->unkShort2 = 10000;
						break;
					case Actor::CREATURE_3_BOX:
						actor->actorBehaviour = CreatureBehaviour::Box;
						break;
					case Actor::CREATURE_3_DINO:
						actor->visibilityDistance = 1800;
						actor->actorBehaviour = CreatureBehaviour::Dino;
						break;
					case Actor::CREATURE_2_ZBOAT:
						actor->visibilityDistance = 1800;
						actor->actorBehaviour = CreatureBehaviour::ZBoat;
						break;
					case Actor::CREATURE_3_CHICK:
						actor->actorBehaviour = CreatureBehaviour::Chick;
						break;
					case Actor::CREATURE_3_GUNSP:
						actor->visibilityDistance = 1800;
						actor->actorBehaviour = CreatureBehaviour::GunsP;
						break;
					case Actor::CREATURE_3_CLOWN:
						actor->visibilityDistance = 1800;
						actor->actorBehaviour = CreatureBehaviour::Clown;
						break;
					case Actor::CREATURE_4_MARTIAN:
						actor->actorBehaviour = CreatureBehaviour::Martian;
						break;
					case Actor::CREATURE_6_RABID:
						actor->actorBehaviour = CreatureBehaviour::Rabid;
						break;
					case Actor::CREATURE_3_BUZZARD:
						actor->actorBehaviour = CreatureBehaviour::Buzzard;
						break;
					case Actor::CREATURE_4_DUCKS:
						actor->actorBehaviour = CreatureBehaviour::Ducks;
						break;
					case Actor::CREATURE_4_GUNSL:
						if (g_levelFileIndex == 11)
						{
							actor->visibilityDistance = 1800;
							actor->actorBehaviour = CreatureBehaviour::GunsL_Level11;
						}
						else
						{
							actor->visibilityDistance = 1800;
							actor->actorBehaviour = CreatureBehaviour::GunsL;
						}
						break;
					case Actor::CREATURE_4_FATBLOKE:
						actor->actorBehaviour = CreatureBehaviour::FatBloke;
						break;
					case Actor::CREATURE_5_BBUGGY:
						actor->visibilityDistance = 1800;
						actor->actorBehaviour = CreatureBehaviour::BBuggy;
						break;
					case Actor::CREATURE_5_BI:
						actor->visibilityDistance = 3800;
						break;
					case Actor::CREATURE_4_GEORGE:
					case Actor::CREATURE_4_MILDRED:
					case Actor::CREATURE_4_KAREN:
					case Actor::CREATURE_4_PILOT:
						actor->actorBehaviour = CreatureBehaviour::Pilot;
						break;
					case Actor::CREATURE_5_SMITH:
						if (g_levelFileIndex == 14)
							actor->actorBehaviour = CreatureBehaviour::Smith_Level14;
						else
							actor->actorBehaviour = CreatureBehaviour::Smith;
						break;
					case Actor::CREATURE_5_LUGGAGE:
						actor->actorBehaviour = CreatureBehaviour::Luggage;
						break;
					case Actor::CREATURE_6_PROSP:
						if (g_levelFileIndex == 13)
						{
							actor->visibilityDistance = 1800;
							actor->actorBehaviour = CreatureBehaviour::ProsP_Level13;
						}
						else
						{
							actor->actorBehaviour = CreatureBehaviour::ProsP;
							actor->visibilityDistance = 1800;
						}
						break;
					default:
						return;
				}
			}
		}

		// FUNCTION: TOY2 0x00407440
		void ActorCollisionCheck()
		{
			int32_t sphereRadius = 150;
			int32_t hitType = 0;
			int32_t ellipsoidRadius = 150;

			// spinning
			if (Buzz::g_unk53C650 > 20 || Buzz::g_unk53C83C <= -120)
			{
				sphereRadius = 400;
				hitType = 1;
				ellipsoidRadius = 400;
			}

			// ground slam
			if (Buzz::g_groundSlamTicker > 0)
				hitType = 5;

			if (Buzz::g_groundSlamTicker < -30)
			{
				sphereRadius = 400;
				hitType = 5;
				ellipsoidRadius = 400;
			}

			Actor::Toy2Actor* actor = Actor::g_activeActors[0];

			int32_t buzzX = g_buzzActor.posAngles.pos.x;
			int32_t buzzY = g_buzzActor.posAngles.pos.y - 7360;
			int32_t buzzZ = g_buzzActor.posAngles.pos.z;

			if (Actor::g_activeActors[0])
			{
				Actor::Toy2Actor** nextActor = Actor::g_activeActors;

				do
				{
					int32_t deltaZ = (actor->transform.pos.z + actor->boundingOffset.z - buzzZ) >> 5;
					int32_t deltaY = (actor->transform.pos.y + actor->boundingOffset.y - buzzY) >> 5;
					int32_t deltaX = (actor->transform.pos.x + actor->boundingOffset.x - buzzX) >> 5;

					// rough test with the bounding sphere of the actor
					if (deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ
							< (sphereRadius + actor->boundingSphereRadius) * (sphereRadius + actor->boundingSphereRadius)
						&& actor->damageCooldownTimer >= 0)
					{
						int16_t* attach = (int16_t*)(actor->unkVar34 + 16 * actor->primaryAnimIdx);
						int32_t sinYaw = Numerics::g_sinCosLUT[(actor->transform.angles.yaw - 2048) & 0xFFF] >> 2;
						int32_t cosYaw = Numerics::g_sinCosLUT[(actor->transform.angles.yaw - 1024) & 0xFFF] >> 2;

						int32_t attachX = attach[0];
						int32_t relX = actor->transform.pos.x + ((sinYaw * attach[2] + cosYaw * attachX) >> 12) - buzzX;
						int32_t relZ = actor->transform.pos.z + ((cosYaw * attach[2] - sinYaw * attachX) >> 12) - buzzZ;

						// the ellipsoid of the actor, Buzz must be inside of it
						int32_t scaledX = attach[4] * ((relX * cosYaw + relZ * sinYaw) >> 12);
						int32_t scaledZ = (attach[6] * ((relZ * cosYaw - relX * sinYaw) >> 12)) >> 13;
						int32_t scaledY = (attach[5] * (actor->transform.pos.y + attach[1] - buzzY)) >> 13;

						if (scaledY * scaledY + (scaledX >> 13) * (scaledX >> 13) + scaledZ * scaledZ
								< (ellipsoidRadius + attach[7]) * (ellipsoidRadius + attach[7])
							&& (actor->actorFlags & Actor::ACTOR_DEATH_DAMAGE) != 0)
						{
							int32_t angle = Nu3D::Math::CartesianToFixedAngle(
								g_buzzActor.posAngles.pos.x - actor->transform.pos.x, g_buzzActor.posAngles.pos.z - actor->transform.pos.z);

							actor->actorFlags |= 0x200;

							int32_t actorHitType = hitType;

							if (! hitType || (actor->creatureRam->defenseMode & 2) == 0 || (hitType == 5 && scaledY < 0))
								actorHitType = 0;

							uint8_t buzzHitType;

							if ((actor->actorFlags & Actor::ACTOR_AIRBORNE_SPECIAL) == 0 || hitType)
								buzzHitType = actor->actorPhase != 102 && ! actorHitType;
							else
								buzzHitType = 3;

							Actor::HandleDamage(actor, angle + 2048, actorHitType);
							Buzz::HandleDamage((int16_t)angle & 0xFFF, buzzHitType);
						}
					}

					actor = *++nextActor;
				} while (*nextActor);
			}
		}

		// FUNCTION: TOY2 0x004086F0
		void UpdateActors()
		{
			int32_t activeCount = 0;
			int32_t distances[64];
			int32_t i;

			for (i = 0; i < 64; i++)
			{
				Actor::Toy2Actor* actor = &g_levelActors[i];

				actor->actorFlags = (actor->actorFlags & 0xFF00)
					| (actor->actorFlags
					   & (Actor::ACTOR_CUTSCENE_MOVE | Actor::ACTOR_LOCKED_MOVE | Actor::ACTOR_GROUNDED | Actor::ACTOR_FLOOR_SNAP | Actor::ACTOR_STUNNED
						  | Actor::ACTOR_OOB_SPECIAL_MOVE | Actor::ACTOR_DEATH_DAMAGE));

				if (actor->creatureId > 0)
				{
					if (actor->actorPhase <= 0)
					{
						int16_t unkShort2 = actor->unkShort2;

						if (unkShort2)
						{
							if (unkShort2 < 5000 && Actor::g_unk50A54C != (int32_t)actor)
							{
								int16_t newValue = unkShort2 - Renderer::g_frameDelta;
								actor->unkShort2 = newValue;

								if (newValue < 0)
									actor->unkShort2 = 0;
							}
						}
						else if (
							(actor->actorFlags & Actor::ACTOR_OOB_SPECIAL_MOVE) != 0
							|| ! Nu3D::Camera::LineOfSightCheck(&Camera::g_activeCameraTransform, actor))
						{
							InitActor(actor, 0);
						}
					}
					else
					{
						int32_t distSq;

						if (g_levelFileIndex == 6 || actor->actorPhase == 202)
						{
							distSq = 10;
						}
						else
						{
							int32_t dz = (Camera::g_activeCameraTransform.pos.z - actor->boundingOffset.z - actor->transform.pos.z) >> 8;
							int32_t dx = (Camera::g_activeCameraTransform.pos.x - actor->boundingOffset.x - actor->transform.pos.x) >> 8;
							int32_t dy = (Camera::g_activeCameraTransform.pos.y - actor->boundingOffset.y - actor->transform.pos.y) >> 8;

							distSq = dx * dx + dy * dy + dz * dz;
						}

						int32_t radius = (actor->boundingSphereRadius >> 3) + ((362 * actor->visibilityDistance) >> 9);

						if (distSq < radius * radius)
						{
							Actor::g_activeActors[activeCount] = actor;

							distances[activeCount] = distSq;
							++activeCount;
						}
					}
				}
			}

			Actor::g_activeActors[activeCount] = 0;
			distances[activeCount] = -1;
			Camera::CullActors(&Camera::g_activeCameraTransform);

			if (g_levelFileIndex == 10)
				ElevatorHop::InitMouse();

			Actor::Toy2Actor* curActor = Actor::g_activeActors[0];

			if (Actor::g_activeActors[0])
			{
				int32_t index = 0;

				do
				{
					int16_t actorFlags = curActor->actorFlags;
					int32_t radius = (curActor->boundingSphereRadius >> 3) + 400;

					if (((actorFlags & 1) == 0 && distances[index] >= radius * radius) || (actorFlags & Actor::ACTOR_FORCE_REMOVE) != 0)
					{
						Actor::g_activeActors[index] = (Actor::Toy2Actor*)-1;
					}
					else
					{
						curActor->actorFlags = actorFlags | Actor::ACTOR_VISIBLE_ACTIVE;
					}

					curActor = Actor::g_activeActors[++index];
				} while (curActor);
			}

			int32_t lastActorIdx = 0;

			if (Actor::g_activeActors[0])
			{
				Actor::Toy2Actor** testActor = Actor::g_activeActors;
				Actor::Toy2Actor** leadingActor = Actor::g_activeActors;
				Actor::Toy2Actor** destActor = Actor::g_activeActors;
				Actor::Toy2Actor* nextActor;

				do
				{
					if (*testActor != (Actor::Toy2Actor*)-1)
					{
						++lastActorIdx;
						*destActor++ = *leadingActor;
					}

					nextActor = leadingActor[1];
					testActor = ++leadingActor;
				} while (nextActor);
			}

			Actor::g_activeActors[lastActorIdx] = 0;
			curActor = Actor::g_activeActors[0];

			if (Actor::g_activeActors[0])
			{
				Actor::Toy2Actor** nextActorPtr = Actor::g_activeActors;

				do
				{
					Actor::UpdateAIMovement(curActor);

					int16_t unkVar29 = curActor->unkVar29;

					if (unkVar29 > 0)
					{
						int16_t newValue = unkVar29 - Renderer::g_frameDelta;
						curActor->unkVar29 = newValue;

						if (newValue <= 0)
						{
							curActor->unkVar29 = 0;
							Actor::Kill(curActor, 1);
						}
					}

					int16_t curUnkVar29 = curActor->unkVar29;

					if (curUnkVar29 < 0)
					{
						if (g_tickState[4] && curUnkVar29 < -1)
						{
							int16_t* attachOffset = (int16_t*)curActor->unkVar34;
							int32_t spawnX = curActor->transform.pos.x + attachOffset[0] + 32 * (*g_randDatBufferPtr++ - 128);
							int32_t spawnY = curActor->transform.pos.y + attachOffset[1] + 32 * (*g_randDatBufferPtr++ - 128);
							int32_t spawnZ = curActor->transform.pos.z + attachOffset[2] + 32 * (*g_randDatBufferPtr++ - 128);

							Nu3D::Particles::SpawnFromPreset(spawnX, spawnY, spawnZ, 37, 1)->rotSpeed = *g_randDatBufferPtr - 128;
							++g_randDatBufferPtr;
						}

						curActor->unkVar29 += Renderer::g_frameDelta;

						if (curActor->unkVar29 >= 0)
						{
							curActor->unkVar29 = 0;
							Actor::Kill(curActor, 2);
							--nextActorPtr;
						}
					}

					curActor = nextActorPtr[1];
					++nextActorPtr;
				} while (curActor);
			}

			if (g_levelFileIndex == 10)
				ElevatorHop::InitMouse();
		}
	}

	namespace PostGameRecap
	{
		// one sparkle of the recap screen, spawned when a token piece is counted
		struct Sparkle
		{
			int32_t x;
			int32_t y;
			int16_t velX;
			int16_t velY;
			int16_t unk12;
			int16_t lifetime;
		};

		STATIC_ASSERT(sizeof(Sparkle) == 0x10);

		// FUNCTION: TOY2 0x004398B0
		int32_t Tick()
		{
			int32_t unlockMasks[5];
			int32_t currentTokens[5];
			int32_t savedTokens[5];
			Sparkle sparkles[64];
			int32_t attractTimer;
			int32_t timer;
			int32_t tokenTimer;
			int32_t coinTimer;
			int32_t coinCountdown;
			int32_t tokenStep;
			int32_t coinsShown;
			int32_t blink;
			int32_t spawnFlag;
			int32_t tokenIndex;
			int32_t tintedOut;
			int32_t frame;
			int32_t newTokens;
			int32_t i;
			int32_t j;
			int32_t savedBits;
			int32_t remaining;
			uint8_t* rand;
			int32_t velX;
			int32_t velY;
			int32_t offsetX;
			int32_t offsetY;
			int32_t rand0;
			int32_t rand1;
			int32_t rand2;
			int32_t frameDelta;

			unlockMasks[0] = 1;
			unlockMasks[1] = 2;
			unlockMasks[2] = 4;
			unlockMasks[3] = 16;
			unlockMasks[4] = 8;
			attractTimer = g_attractModeTimer < 0 ? -1 : 900;
			SetBackdropByIndex(0);

			for (i = 0; i < 64; ++i)
				sparkles[i].lifetime = 0;

			InputManager::g_curButtonsPressed = 0;
			InputManager::g_prevButtonsPressed = 0;
			MainMenu::g_fadeTimer = 0;
			MainMenu::g_nextScreen = 0;
			Nu3D::Camera::g_cameraTintBlue = 0;
			Nu3D::Camera::g_cameraTintGreen = 0;
			Nu3D::Camera::g_cameraTintRed = 0;
			Nu3D::Camera::SetTint(128, 128, 128, 12);
			SoftwareRenderer::UnkFunc67(0, 0);
			*(uint16_t*)((char*)&Renderer::g_level0Sheet22.zero + 2) |= 8;

			currentTokens[0] = SaveManager::g_curLevelTokenData & 1;
			timer = 360;
			currentTokens[1] = (SaveManager::g_curLevelTokenData & 2) >> 1;
			currentTokens[2] = (SaveManager::g_curLevelTokenData & 4) >> 2;
			currentTokens[3] = (SaveManager::g_curLevelTokenData & 8u) >> 3;
			currentTokens[4] = (SaveManager::g_curLevelTokenData & 16) >> 4;
			savedBits = SaveManager::g_save0Data.tokens[g_levelFileConversion[g_levelIndex]];
			Renderer::g_frameDelta = 1;
			savedTokens[0] = savedBits & 1;
			savedTokens[1] = (savedBits & 2) >> 1;
			savedTokens[2] = (savedBits & 4) >> 2;
			savedTokens[4] = (savedBits & 0x10) >> 4;
			newTokens = 0;
			coinsShown = 0;
			blink = 0;
			spawnFlag = 0;
			tokenIndex = 0;
			tintedOut = 0;
			// $TODO: the frame counter is never initialised in the original, it starts as whatever was on the stack
			frame = 0;
			g_randDatBufferPtr = g_randDatBuffer;
			savedTokens[3] = (savedBits & 8u) >> 3;

			for (i = 0; i < 5; ++i)
			{
				if (savedTokens[i] && ! currentTokens[i])
					++newTokens;
			}

			tokenTimer = 60 * newTokens;

			if (tokenTimer)
			{
				tokenTimer += 60;

				if (tokenTimer)
					g_savePromptPending = 1;
			}

			if (SaveManager::g_saveUnlocksCache != SaveManager::g_save0Data.unlocks)
				g_savePromptPending = 1;

			coinTimer = 3 * g_buzzActor.coinsCollected + 16;
			AudioManager::PlayMusicOneShot(21);
			tintedOut = 0;
			g_unk55A120 = 1;
			Nu3D::Camera::SetTint(128, 128, 128, 6);
			tokenStep = tokenTimer - 30;
			coinCountdown = coinTimer;

			do
			{
				while (1)
				{
					Nu3D::Camera::FadeToTargetTint();

					if (g_randDatBufferPtr > &g_randDatBuffer[1500])
						g_randDatBufferPtr -= 1500;

					SoftwareRenderer::UnkFunc67(0, 0);
					frameDelta = Renderer::g_frameDelta;

					for (i = 0; i < 64; ++i)
					{
						if (sparkles[i].lifetime > 0)
						{
							sparkles[i].x += frameDelta * sparkles[i].velX;
							sparkles[i].y += frameDelta * sparkles[i].velY;
							Renderer::Sprite::DrawTinted(sparkles[i].x >> 8, sparkles[i].y >> 8, 80, sparkles[i].lifetime >> 3, 64, 64, 0, 32);
							frameDelta = Renderer::g_frameDelta;
							sparkles[i].velY += 24 * frameDelta;
							sparkles[i].lifetime -= frameDelta;
						}
					}

					Renderer::Sprite::DrawScaledFixed(44, 60, 82, 15, 255, 255, 255, 255, 2048, 2048);
					Renderer::Sprite::DrawScaledFixed(96, 60, 82, g_levelIndex, 255, 255, 255, 255, 2048, 2048);
					Renderer::Sprite::DrawScaledFixed(148, 60, 82, 16, 255, 255, 255, 255, 2048, 2048);
					Renderer::Sprite::DrawScaledFixed(200, 60, 82, 17, 255, 255, 255, 255, 2048, 2048);
					Renderer::Sprite::DrawScaledFixed(252, 60, 83, g_levelIndex, 255, 255, 255, 255, 2048, 2048);

					for (i = 0; i < 5; ++i)
					{
						if (currentTokens[i])
							Renderer::Sprite::DrawTile(52 * i + 36, 32, 76, (4 * i + (frame / 2)) & 0x1F);
						else
							Renderer::Sprite::DrawTinted(52 * i + 36, 32, 76, (4 * i + (frame / 2)) & 0x1F, 0, 0, 0, 0);
					}

					for (i = 0; i < 5; ++i)
					{
						if ((g_playerUnlocks & unlockMasks[i]) != 0)
							Renderer::Sprite::DrawScaledFixed(40 * i + 68, 173, 81, i, 255, 255, 255, 255, 2048, 2048);
						else
							Renderer::Sprite::DrawScaledFixed(40 * i + 68, 173, 81, i, 0, 0, 0, 0, 2048, 2048);
					}

					Renderer::Sprite::DrawTile(92, 118, 78, (frame + 2 * i) & 0xF);
					Renderer::Sprite::DrawTile(152, 122, 79, 10);
					Renderer::Sprite::DrawTile(190, 122, 79, coinsShown / 10 % 10);
					Renderer::Sprite::DrawTile(208, 122, 79, coinsShown % 10);

					if (timer < 300)
					{
						if (tokenTimer <= 0)
						{
							if (coinTimer <= 0)
							{
								if (coinsShown < g_buzzActor.coinsCollected)
									++coinsShown;
							}
							else
							{
								if (coinCountdown - coinTimer > 3)
								{
									if (coinsShown < g_buzzActor.coinsCollected)
										AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);

									if (coinCountdown - coinTimer > 3)
									{
										do
										{
											coinCountdown -= 3;

											if (coinsShown < g_buzzActor.coinsCollected)
												++coinsShown;
										} while (coinCountdown - coinTimer > 3);
									}
								}

								coinTimer -= Renderer::g_frameDelta;
							}
						}
						else
						{
							if (tokenStep - tokenTimer >= 30)
							{
								j = 0;
								tokenStep -= 30;

								while (currentTokens[j] || ! savedTokens[j])
								{
									if (++j >= 5)
									{
										tokenTimer -= Renderer::g_frameDelta;
										goto LABEL_52;
									}
								}

								AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
								currentTokens[j] = 1;
								tokenIndex = j;
								spawnFlag = 20;
							}

							tokenTimer -= Renderer::g_frameDelta;
						}
					}
				LABEL_52:
					if (spawnFlag)
					{
						rand = g_randDatBufferPtr;
						spawnFlag = 0;
						remaining = 15;

						do
						{
							rand0 = *rand;
							offsetX = 16 * (rand0 - 128);
							g_randDatBufferPtr = rand + 1;
							rand1 = rand[1];
							g_randDatBufferPtr = rand + 2;
							offsetY = 8 * rand1 - 1024;
							rand2 = rand[2];
							velY = offsetY / 7 + (rand2 >> 1) - 384;
							rand += 4;
							velX = offsetX / 7 + (rand2 >> 4) - 16;
							g_randDatBufferPtr = rand;

							for (j = 0; j < 64; ++j)
							{
								if (! sparkles[j].lifetime)
									break;
							}

							if (j != 64)
							{
								sparkles[j].x = offsetX + ((tokenIndex + 4 * (3 * tokenIndex + 3) + 1) << 10);
								sparkles[j].y = offsetY + 10240;
								sparkles[j].velX = velX;
								sparkles[j].velY = velY;
								sparkles[j].lifetime = 31;
							}

							--remaining;
						} while (remaining);
					}

					Nullsub3();
					MainMenu::RenderMenu();

					if (timer > 120 && timer < 240)
					{
						blink = (Renderer::g_frameDelta + blink) & 0x3F;

						if (blink < 0x30)
							Renderer::Sprite::DrawWhiteText("press jump to exit", 220, 160);
					}

					Nullsub6();
					frameDelta = Renderer::g_frameDelta;

					if ((timer <= 180 && timer > 120) || (timer -= frameDelta, timer > 0))
					{
						if (timer > 23)
							goto LABEL_72;
					}
					else
					{
						timer = 0;
					}

					if (! tintedOut)
					{
						Nu3D::Camera::SetTint(0, 0, 0, 12);
						frameDelta = Renderer::g_frameDelta;
						tintedOut = 1;
					}
				LABEL_72:
					frame += frameDelta;

					if (g_attractModeTimer >= 0)
					{
						if (attractTimer >= 0)
						{
							attractTimer -= frameDelta;

							if (attractTimer <= 0)
								attractTimer = -1;
						}

						if ((InputManager::g_curButtonsPressed & 1) != 0 || attractTimer < 0)
						{
							InputManager::g_curButtonsPressed |= 0x4000;
							InputManager::g_prevButtonsPressed &= ~0x4000;
						}
					}

					if ((InputManager::g_curButtonsPressed & 0x4000) == 0 || (InputManager::g_prevButtonsPressed & 0x4000) != 0 || timer >= 240 || timer <= 120)
						break;

					AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);

					if (tokenTimer || coinTimer)
					{
						timer = 120;
						coinsShown = g_buzzActor.coinsCollected;
						memcpy(currentTokens, savedTokens, sizeof(currentTokens));
					}
					else
					{
						timer = 24;
					}
				}
			} while (timer > 0);

			return AudioManager::StopAndWait();
		}

	}

	namespace GameOver
	{
		// FUNCTION: TOY2 0x00437B20
		int32_t Tick()
		{
			int32_t timer;

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
			timer = 1200;
			AudioManager::PlayMusicOneShot(17);

			do
			{
				while (1)
				{
					Nu3D::Camera::FadeToTargetTint();
					MainMenu::RenderMenu();
					timer -= Renderer::g_frameDelta;

					if (timer <= 0)
						timer = 0;

					if (AudioManager::IsStreamActive())
					{
						if (timer > 23)
							goto LABEL_11;
					}
					else if (timer > 23)
					{
						timer = 23;
					}

					if ((int32_t)(timer + Renderer::g_frameDelta) > 23)
						Nu3D::Camera::SetTint(0, 0, 0, 12);
				LABEL_11:
					if (g_attractModeTimer >= 0 && (InputManager::g_curButtonsPressed & 1) != 0)
						InputManager::g_curButtonsPressed |= 0x4000;

					if ((InputManager::g_curButtonsPressed & 0xF000) == 0 || (InputManager::g_prevButtonsPressed & 0xF000) != 0 || timer >= 1140 || timer <= 23)
						break;

					timer = 24;
				}
			} while (timer);

			return AudioManager::StopAndWait();
		}

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

	// the path file of the current level, the two digits are replaced with the level number
	// GLOBAL: TOY2 0x00503AD4
	char g_padPathFile[15] = "PAD\\PATH00.BIN";

	// FUNCTION: TOY2 0x004A3770
	void LoadPathBin()
	{
		g_padPathFile[8] = g_levelFileIndex / 10 + 48;
		g_padPathFile[9] = g_levelFileIndex % 10 + 48;
		FileUtils::LoadFile(g_padPathFile, g_demoPathBuffer);
	}

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

	// FUNCTION: TOY2 0x0047D7C0
	void UpdateAudioChannels() { AudioManager::UpdateChannels(); }

	// FUNCTION: TOY2 0x0049B9E0
	int32_t TickSaveMenuMachine(int32_t showSaveMenu)
	{
		// The load / save menu. The menu type is 0 = main menu, 1 = load slots, 2 = save slots and 3 = fading out to leave
		char slotFileName[256];
		char slotName[256];
		char saveName[256];
		char xOffsets[32];

		size_t slotNameSize;

		int32_t done = 0;
		int32_t saveMenuType = 0;
		int32_t menuCursor = 0;
		int32_t loadedSave = 0;

		Nu3D::Camera::g_cameraTintBlue = 0;
		Nu3D::Camera::g_cameraTintGreen = 0;
		Nu3D::Camera::g_cameraTintRed = 0;

		Nu3D::Camera::SetTint(128, 128, 128, 12);

		int32_t inputDelay = 30;
		int32_t blinkTimer = 20;
		int32_t slotCursor = 0;

		if (showSaveMenu)
			saveMenuType = 2;

		// read the name of every save slot, and work out how far each one is pushed along the curve
		size_t saveIndex = 0;
		char* xOffset = xOffsets;
		SaveNameBuffer* saveNameBuffer = g_saveNameBuffers;

		do
		{
			size_t nextSaveIndex = saveIndex + 1;

			sprintf(slotFileName, "Toy2%02d.sav", saveIndex + 1);

			FILE* saveFile = fopen(slotFileName, "rb");

			if (saveFile)
			{
				fread(&slotNameSize, 1, 4, saveFile);

				if (slotNameSize && saveNameBuffer)
				{
					fread(saveNameBuffer, 1, slotNameSize, saveFile);

					saveNameBuffer->name[slotNameSize] = 0;
				}

				fclose(saveFile);
			}
			else if (saveNameBuffer)
			{
				saveNameBuffer->name[0] = '\0';
			}

			*xOffset = (cos(saveIndex * 0.64269906 - 1.0) * 80.0);

			++saveNameBuffer;

			saveIndex = nextSaveIndex;

			xOffset += 4;
		} while (saveNameBuffer < &g_saveNameBuffers[8]);

		do
		{
			D3DApp::ProcessWndEvents();

			if (D3DApp::g_windowData.wndIsExiting)
			{
				DrawingDevice::RestoreToGDISurface(1);
				Logger::GetErrorHandler("C:\\projects\\toy2\\toy2.cpp", 4670)(g_emptyStr);
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

			Renderer::DoFrameDelay(0);
			Nu3D::Camera::FadeToTargetTint();

			RGBA clearColor;
			clearColor.value = 0;
			Renderer::ClearScreen(clearColor, 2);

			if (Renderer::BeginScene())
			{
				Renderer::g_parallaxCurHorizScroll = 0.0;
				Renderer::g_parallaxHorizOffset = 0.0;
				Renderer::g_parallaxTexHeightRatio = 1.0;
				Renderer::g_parallaxTexWidthRatio = 1.0;
				Renderer::RenderParallaxBackground(0);

				switch (saveMenuType)
				{
					case 0:
						Renderer::g_parallaxCurHorizScroll = 0.0;

						if (NGNLoader::GetTextureDataIndex(g_sectorBackdropTexTable.primary[1]))
							g_nextBackdropId = g_sectorBackdropTexTable.primary[1];
						else if (NGNLoader::GetTextureDataIndex(g_sectorBackdropTexTable.secondary[0]))
							g_nextBackdropId = g_sectorBackdropTexTable.secondary[0];

						g_hasBackdrop = 1;

						Renderer::DrawFormattedText(256, 25, "load / save options");

						// the selected entry blinks, so it is skipped for part of the blink timer
						if ((menuCursor || blinkTimer < 10) && (Renderer::DrawFormattedText(256, 75, "load game"), menuCursor == 1) && blinkTimer >= 10
							|| (Renderer::DrawFormattedText(256, 100, "save game"), menuCursor != 2) || blinkTimer < 10)
						{
							Renderer::DrawFormattedText(256, 125, "main menu");
						}

						if (inputDelay <= 0)
						{
							if (menuCursor && (InputManager::g_curButtonsPressed & INPUT_UP)
								&& InputManager::g_curButtonsPressed != (InputManager::g_prevButtonsPressed & INPUT_UP))
							{
								--menuCursor;
								AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
							}

							if (menuCursor < 2 && (InputManager::g_curButtonsPressed & INPUT_DOWN)
								&& InputManager::g_curButtonsPressed != (InputManager::g_prevButtonsPressed & INPUT_DOWN))
							{
								++menuCursor;
								AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
							}

							if (InputManager::g_curButtonsPressed & INPUT_JUMP)
							{
								if (menuCursor == 0 || menuCursor == 1)
								{
									saveMenuType = menuCursor == 0 ? 1 : 2;
									slotCursor = 0;
									inputDelay = 30;

									AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);
								}
								else
								{
									AudioManager::PlayOneShotSoundGlobal(2, 4608, 80, 80);
									saveMenuType = 3;
									Nu3D::Camera::SetTint(0, 0, 0, 6);
								}
							}

							if (InputManager::g_curButtonsPressed & INPUT_CANCEL)
							{
								AudioManager::PlayOneShotSoundGlobal(2, 4608, 80, 80);
								saveMenuType = 3;
								Nu3D::Camera::SetTint(0, 0, 0, 6);
							}
						}
						break;

					case 1: {
						Renderer::g_parallaxCurHorizScroll = 0.0;

						if (NGNLoader::GetTextureDataIndex(g_sectorBackdropTexTable.primary[1]))
							g_nextBackdropId = g_sectorBackdropTexTable.primary[1];
						else if (NGNLoader::GetTextureDataIndex(g_sectorBackdropTexTable.secondary[0]))
							g_nextBackdropId = g_sectorBackdropTexTable.secondary[0];

						g_hasBackdrop = 1;

						Renderer::DrawFormattedText(256, 3, "select slot to load");

						int32_t slot = 0;
						int32_t slotY = 33;
						char* slotXOffset = xOffsets;
						SaveNameBuffer* nameBuffer = g_saveNameBuffers;

						do
						{
							if (slot != slotCursor || blinkTimer < 10)
							{
								if (nameBuffer->name[0])
									Renderer::DrawFormattedText(*slotXOffset + 256, slotY, "%s", nameBuffer->name);
								else
									Renderer::DrawFormattedText(*slotXOffset + 256, slotY, "empty slot");
							}

							++nameBuffer;
							++slot;
							slotY += 23;
							slotXOffset += 4;
						} while (nameBuffer < &g_saveNameBuffers[8]);

						Renderer::DrawFormattedText(256, 25 * slot + 28, "jump:select  cancel:menu");

						int16_t buttons = InputManager::g_curButtonsPressed;

						if ((InputManager::g_curButtonsPressed & INPUT_UP)
							&& InputManager::g_curButtonsPressed != (InputManager::g_prevButtonsPressed & INPUT_UP) && slotCursor)
						{
							--slotCursor;
							AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
							buttons = InputManager::g_curButtonsPressed;
						}

						if ((buttons & INPUT_DOWN) && buttons != (InputManager::g_prevButtonsPressed & INPUT_DOWN) && slotCursor < 7)
						{
							++slotCursor;
							AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
							buttons = InputManager::g_curButtonsPressed;
						}

						if (inputDelay <= 0)
						{
							if ((buttons & INPUT_JUMP) && g_saveNameBuffers[slotCursor].name[0])
							{
								sprintf(slotFileName, "Toy2%02d.sav", slotCursor + 1);

								FILE* saveFile = fopen(slotFileName, "rb");

								if (saveFile)
								{
									fread(&slotNameSize, 1, 4, saveFile);

									if (slotNameSize)
									{
										fread(slotName, 1, slotNameSize, saveFile);
										slotName[slotNameSize] = 0;
									}

									if (slotCursor == 98)
										fread(&SaveManager::g_save99Data, 1, 392, saveFile);
									else
										fread(&SaveManager::g_save0Data, 1, 392, saveFile);

									fclose(saveFile);
								}

								saveMenuType = 3;
								Nu3D::Camera::SetTint(0, 0, 0, 6);
								AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);

								buttons = InputManager::g_curButtonsPressed;
								loadedSave = 1;
							}

							if (buttons & INPUT_CANCEL)
							{
								inputDelay = 30;
								saveMenuType = 0;
								AudioManager::PlayOneShotSoundGlobal(2, 4608, 80, 80);
							}
						}
						break;
					}

					case 2: {
						Renderer::g_parallaxCurHorizScroll = 0.0;

						if (NGNLoader::GetTextureDataIndex(g_sectorBackdropTexTable.primary[1]))
							g_nextBackdropId = g_sectorBackdropTexTable.primary[1];
						else if (NGNLoader::GetTextureDataIndex(g_sectorBackdropTexTable.secondary[0]))
							g_nextBackdropId = g_sectorBackdropTexTable.secondary[0];

						g_hasBackdrop = 1;

						Renderer::DrawFormattedText(256, 3, "select slot to save");

						int32_t slot = 0;
						int32_t slotY = 33;
						char* slotXOffset = xOffsets;
						SaveNameBuffer* nameBuffer = g_saveNameBuffers;

						do
						{
							if (slot != slotCursor || blinkTimer < 10)
							{
								if (nameBuffer->name[0])
									Renderer::DrawFormattedText(*slotXOffset + 256, slotY, "%s", nameBuffer->name);
								else
									Renderer::DrawFormattedText(*slotXOffset + 256, slotY, "empty slot");
							}

							++nameBuffer;
							++slot;
							slotY += 23;
							slotXOffset += 4;
						} while (nameBuffer < &g_saveNameBuffers[8]);

						if (showSaveMenu)
							Renderer::DrawFormattedText(256, 25 * slot + 28, "jump:save  cancel:continue");
						else
							Renderer::DrawFormattedText(256, 25 * slot + 28, "jump:select  cancel:menu");

						int16_t buttons = InputManager::g_curButtonsPressed;

						if ((InputManager::g_curButtonsPressed & INPUT_UP)
							&& InputManager::g_curButtonsPressed != (InputManager::g_prevButtonsPressed & INPUT_UP) && slotCursor)
						{
							--slotCursor;
							AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
							buttons = InputManager::g_curButtonsPressed;
						}

						if ((buttons & INPUT_DOWN) && buttons != (InputManager::g_prevButtonsPressed & INPUT_DOWN) && slotCursor < 7)
						{
							++slotCursor;
							AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
							buttons = InputManager::g_curButtonsPressed;
						}

						if (inputDelay <= 0)
						{
							if (buttons & INPUT_JUMP)
							{
								int32_t tokenProgress = ComputeTokenProgress();

								sprintf(saveName, "TOK %d LEV %d", (tokenProgress >> 16) & 0xFF, tokenProgress);
								SaveManager::SaveToFile(slotCursor + 1, saveName);

								saveMenuType = 3;
								Nu3D::Camera::SetTint(0, 0, 0, 6);

								// read the slot names again, so the new save shows up
								int32_t refreshIndex = 0;
								SaveNameBuffer* refreshBuffer = g_saveNameBuffers;

								do
								{
									int32_t nextRefreshIndex = refreshIndex + 1;

									sprintf(slotFileName, "Toy2%02d.sav", refreshIndex + 1);

									FILE* saveFile = fopen(slotFileName, "rb");

									if (saveFile)
									{
										fread(&slotNameSize, 1, 4, saveFile);

										if (slotNameSize && refreshBuffer)
										{
											fread(refreshBuffer, 1, slotNameSize, saveFile);
											refreshBuffer->name[slotNameSize] = 0;
										}

										fclose(saveFile);
									}
									else if (refreshBuffer)
									{
										refreshBuffer->name[0] = 0;
									}

									++refreshBuffer;
									refreshIndex = nextRefreshIndex;
								} while (refreshBuffer < &g_saveNameBuffers[8]);

								AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);
								buttons = InputManager::g_curButtonsPressed;
							}

							if (buttons & INPUT_CANCEL)
							{
								if (showSaveMenu)
								{
									AudioManager::PlayOneShotSoundGlobal(2, 4608, 80, 80);
									saveMenuType = 3;
									Nu3D::Camera::SetTint(0, 0, 0, 6);
								}
								else
								{
									inputDelay = 30;
									saveMenuType = 0;
									AudioManager::PlayOneShotSoundGlobal(2, 4608, 80, 80);
								}
							}
						}
						break;
					}

					case 3:
						if (! Nu3D::Camera::g_cameraTintBlue)
							done = 1;
						break;
				}

				Renderer::DrawTintOverlay();
				Renderer::Sprite::DrawQueuedSprite();
				Renderer::EndScene(1);
			}

			if (inputDelay > 0)
				inputDelay -= Renderer::g_frameDelta;

			blinkTimer -= Renderer::g_frameDelta;

			if (blinkTimer < 0)
				blinkTimer += 20;

		} while (! done);

		return loadedSave;
	}

	// FUNCTION: TOY2 0x00453F20
	int32_t ShowSaveScreen()
	{
		int32_t prevLevelFileIdx = g_levelFileIndex;

		g_levelFileIndex = 16;
		Levels::g_levelLoadConfig = 248;

		g_hasStaticBackdrop = 0;
		Renderer::g_virtualScreenWidth = 512.0;
		Renderer::g_virtualScreenHeight = 256.0;
		g_nextBackdropId = 36;
		Levels::InitLevelPlay(16);

		g_savePromptPending = 0;
		SaveManager::TransferProgressData(&SaveManager::g_save0Data);

		int32_t result = TickSaveMenuMachine(0);

		SaveManager::LoadProgressData(&SaveManager::g_save0Data);

		g_levelFileIndex = prevLevelFileIdx;

		return result;
	}

	// FUNCTION: TOY2 0x00453CA0
	void ResetBackdropState()
	{
		g_hasStaticBackdrop = 0;
		Renderer::g_virtualScreenWidth = 512.0;
		Renderer::g_virtualScreenHeight = 256.0;
		g_nextBackdropId = 36;
	}

	// FUNCTION: TOY2 0x00414320
	int32_t ShowLevelIntroScreen(int32_t levelId, int32_t demo)
	{
		int32_t skippedByAttractMode = 0;

		if (! demo || demo == 123)
		{
			int32_t attractTimer;

			if (g_attractModeTimer < 0)
				attractTimer = -1;
			else
				attractTimer = 2 * g_attractModeTimer;

			int32_t fadingOut = 0;
			int32_t titleSlidePos = 0;
			int32_t blinkTimer = 0;

			SoftwareRenderer::g_unk4F7400.x = 0;
			SoftwareRenderer::g_unk4F7400.y = 0;

			int32_t fadeFrames = 28;

			do
			{
				if (fadingOut)
				{
					fadeFrames -= Renderer::g_frameDelta;

					if (fadeFrames <= 0)
						fadeFrames = 0;
				}

				if (attractTimer > 0)
				{
					attractTimer -= Renderer::g_frameDelta;

					if (attractTimer <= 0)
					{
						InputManager::g_curButtonsPressed |= 1;
						attractTimer = 0;
					}
				}

				if ((InputManager::g_curButtonsPressed & INPUT_JUMP) && ! (InputManager::g_prevButtonsPressed & INPUT_JUMP) && ! fadingOut)
				{
					Nu3D::Camera::SetTint(0, 0, 0, 12);
					fadingOut = 1;
					AudioManager::PlaySoundEffect(7, 0);
				}

				if ((InputManager::g_curButtonsPressed & 1) && ! fadingOut && g_attractModeTimer >= 0)
				{
					Nu3D::Camera::SetTint(0, 0, 0, 12);
					fadingOut = 1;
					skippedByAttractMode = 1;
				}

				Nu3D::Camera::FadeToTargetTint();

				uint32_t frameDelta = Renderer::g_frameDelta;
				blinkTimer = (Renderer::g_frameDelta + blinkTimer) & 0x3F;

				if (blinkTimer > 30)
				{
					Renderer::Sprite::DrawScaled(96, 200, 128, 0, 255, 255, 255, 255, 2048, 2048);
					frameDelta = Renderer::g_frameDelta;
				}

				if (titleSlidePos < 1024 && demo != 123)
				{
					titleSlidePos += 32 * frameDelta;
					Renderer::Sprite::DrawScaled(96, 264 - (Numerics::g_sinCosLUT[titleSlidePos + 1024] >> 8), 128, 1, 255, 255, 255, 255, 2048, 2048);
				}

				Nullsub3();
				MainMenu::RenderMenu();
			} while (fadeFrames);
		}
		else
		{
			Nu3D::Camera::SetTint(0, 0, 0, 12);
			Nu3D::Camera::g_cameraTintBlue = Nu3D::Camera::g_targetTintBlue;
			Nu3D::Camera::g_cameraTintGreen = Nu3D::Camera::g_targetTintGreen;
			Nu3D::Camera::g_cameraTintRed = Nu3D::Camera::g_targetTintRed;
			Nu3D::Camera::g_targetTintFadeSpeed = 0;
		}

		ResetBackdropState();

		SoftwareRenderer::g_unk4F7400.x = 0xFFFF8000;
		SoftwareRenderer::g_unk4F7400.y = 0xFFFF8000;

		return skippedByAttractMode;
	}

	// FUNCTION: TOY2 0x004500A0
	void LoadLevelGraphics(int32_t level)
	{
		char levelName[256];
		char* fileName;

		Levels::FlushRenderer();
		Levels::InitLevelDefaults();

		Levels::g_levelLoadArena = Levels::g_levelDataHeapBase;

		switch (level)
		{
			case -1:
				RawLoader::LoadPacketData("gfx\\film.raw");
				FileUtils::AppendRegPathToBuffer();
				sprintf(levelName, "%s%s", FileUtils::g_fileNameBuffer, "gfx\\film.raw");
				fileName = strstr(levelName, ".raw");

				if (fileName)
					goto LBL_MAKENGN_IMAGE;

				break;

			case 0:
				RawLoader::LoadPacketData("gfx\\loading.raw");
				FileUtils::AppendRegPathToBuffer();
				sprintf(levelName, "%s%s", FileUtils::g_fileNameBuffer, "gfx\\loading.raw");
				fileName = strstr(levelName, ".raw");

				if (fileName)
					goto LBL_MAKENGN_IMAGE;

				break;

			case 1:
				RawLoader::LoadPacketData("gfx\\level1a.raw");
				FileUtils::AppendRegPathToBuffer();
				sprintf(levelName, "%s%s", FileUtils::g_fileNameBuffer, "gfx\\level1a.raw");
				fileName = strstr(levelName, ".raw");

				if (fileName)
					goto LBL_MAKENGN_IMAGE;

				break;

			case 2:
				RawLoader::LoadPacketData("gfx\\level1b.raw");
				FileUtils::AppendRegPathToBuffer();
				sprintf(levelName, "%s%s", FileUtils::g_fileNameBuffer, "gfx\\level1b.raw");
				fileName = strstr(levelName, ".raw");

				if (fileName)
					goto LBL_MAKENGN_IMAGE;

				break;

			case 3:
				RawLoader::LoadPacketData("gfx\\level1c.raw");
				FileUtils::AppendRegPathToBuffer();
				sprintf(levelName, "%s%s", FileUtils::g_fileNameBuffer, "gfx\\level1c.raw");
				fileName = strstr(levelName, ".raw");

				if (fileName)
					goto LBL_MAKENGN_IMAGE;

				break;

			case 4:
				RawLoader::LoadPacketData("gfx\\level2a.raw");
				FileUtils::AppendRegPathToBuffer();
				sprintf(levelName, "%s%s", FileUtils::g_fileNameBuffer, "gfx\\level2a.raw");
				fileName = strstr(levelName, ".raw");

				if (fileName)
					goto LBL_MAKENGN_IMAGE;

				break;

			case 5:
				RawLoader::LoadPacketData("gfx\\level2b.raw");
				FileUtils::AppendRegPathToBuffer();
				sprintf(levelName, "%s%s", FileUtils::g_fileNameBuffer, "gfx\\level2b.raw");
				fileName = strstr(levelName, ".raw");

				if (fileName)
					goto LBL_MAKENGN_IMAGE;

				break;

			case 6:
				RawLoader::LoadPacketData("gfx\\level2c.raw");
				FileUtils::AppendRegPathToBuffer();
				sprintf(levelName, "%s%s", FileUtils::g_fileNameBuffer, "gfx\\level2c.raw");
				fileName = strstr(levelName, ".raw");

				if (fileName)
					goto LBL_MAKENGN_IMAGE;

				break;

			case 7:
				RawLoader::LoadPacketData("gfx\\level3a.raw");
				FileUtils::AppendRegPathToBuffer();
				sprintf(levelName, "%s%s", FileUtils::g_fileNameBuffer, "gfx\\level3a.raw");
				fileName = strstr(levelName, ".raw");

				if (fileName)
					goto LBL_MAKENGN_IMAGE;

				break;

			case 8:
				RawLoader::LoadPacketData("gfx\\level3b.raw");
				FileUtils::AppendRegPathToBuffer();
				sprintf(levelName, "%s%s", FileUtils::g_fileNameBuffer, "gfx\\level3b.raw");
				fileName = strstr(levelName, ".raw");

				if (fileName)
					goto LBL_MAKENGN_IMAGE;

				break;

			case 9:
				RawLoader::LoadPacketData("gfx\\level3c.raw");
				FileUtils::AppendRegPathToBuffer();
				sprintf(levelName, "%s%s", FileUtils::g_fileNameBuffer, "gfx\\level3c.raw");
				fileName = strstr(levelName, ".raw");

				if (fileName)
				{
				LBL_MAKENGN_IMAGE:
					strcpy(fileName, ".ngn");
					NGNLoader::SetNewImage(levelName);
					NGNLoader::DetectBackdropTextures();
				}

				break;

			case 10:
				RawLoader::LoadRawAndNGN("gfx\\level4a.raw");
				break;

			case 11:
				RawLoader::LoadRawAndNGN("gfx\\level4b.raw");
				break;

			case 12:
				RawLoader::LoadRawAndNGN("gfx\\level4c.raw");
				break;

			case 13:
				RawLoader::LoadRawAndNGN("gfx\\level5a.raw");
				break;

			case 14:
				RawLoader::LoadRawAndNGN("gfx\\level5b.raw");
				break;

			case 15:
				RawLoader::LoadRawAndNGN("gfx\\level5c.raw");
				break;

			case 16:
			case 17:
				RawLoader::LoadRawAndNGN("gfx\\congrats.raw");
				break;

			case 30:
				RawLoader::LoadRawAndNGN("gfx\\bonus.raw");
				break;

			case 31:
				RawLoader::LoadRawAndNGN("gfx\\gamewin.raw");
				break;

			case 35:
			case 38:
			case 41:
			case 44:
			case 47:
				RawLoader::LoadRawAndNGN("gfx\\boss.raw");
				break;

			default:
				RawLoader::LoadRawAndNGN("gfx\\level1a.raw");
				break;
		}
	}

	// FUNCTION: TOY2 0x00414270
	void LoadLevelWithFadeIn(int32_t levelId, int32_t flag)
	{
		if (! flag || flag == 123)
			Toy2::LoadLevelGraphics(levelId);
		else
			Toy2::LoadLevelGraphics(0);

		SoftwareRenderer::g_unk4F7400.x = 0;
		SoftwareRenderer::g_unk4F7400.y = 0;

		Nu3D::Camera::SetTint(128, 128, 128, 12);

		int32_t delta = 28;

		do
		{
			delta -= Renderer::g_frameDelta;

			if (delta <= 0)
				delta = 0;

			Nu3D::Camera::FadeToTargetTint();

			if (! flag)
				Renderer::Sprite::DrawScaled(96, 200, 128, 1, 255u, 255u, 255u, 255, 2048, 2048);

			Nullsub3();

			Toy2::MainMenu::RenderMenu();
		} while (delta);
	}

	// FUNCTION: TOY2 0x0049A930
	int32_t PlayMovie(int32_t movieId)
	{
		const char* movieName;
		int32_t exited;
		int32_t result;
		CHAR filePath[256];

		FileUtils::AppendCDPath(filePath);

		strcat(filePath, "rtlibs\\");

		switch (movieId)
		{
			case 0:
				movieName = "dlogo";
				goto LABEL_24;
			case 1:
				movieName = "tt";
				goto LABEL_24;
			case 2:
				movieName = "acti";
				goto LABEL_24;
			case 10:
				movieName = "1st trailer";
				goto LABEL_24;
			case 11:
				movieName = "l 01 in";
				goto LABEL_24;
			case 12:
				movieName = "l 02 in";
				goto LABEL_24;
			case 13:
				movieName = "l 03 bo";
				goto LABEL_24;
			case 14:
				movieName = "l 04 in";
				goto LABEL_24;
			case 15:
				movieName = "l 05 in";
				goto LABEL_24;
			case 16:
				movieName = "l 06 bo";
				goto LABEL_24;
			case 17:
				movieName = "l 07 in";
				goto LABEL_24;
			case 18:
				movieName = "l 08 in";
				goto LABEL_24;
			case 19:
				movieName = "l 09 bo";
				goto LABEL_24;
			case 20:
				movieName = "l 10 in";
				goto LABEL_24;
			case 21:
				movieName = "l 11 in";
				goto LABEL_24;
			case 22:
				movieName = "l 12 bo";
				goto LABEL_24;
			case 23:
				movieName = "l 13 in";
				goto LABEL_24;
			case 24:
				movieName = "l 14 in";
				goto LABEL_24;
			case 25:
				movieName = "l 15 bo 1";
				goto LABEL_24;
			case 26:
				movieName = "l 12 in";
				goto LABEL_24;
			case 27:
				movieName = "l 15 bo 2";
				goto LABEL_24;
			case 28:
				movieName = "end 01";
			LABEL_24:
				strcat(filePath, movieName);
				strcat(filePath, ".dll");

				AudioManager::ReleaseBuffers();
				exited = Nu3D::FMV::PlayMovie(filePath);
				AudioManager::Init();

				result = exited;
				break;
			default:
				result = 0;
				break;
		}

		return result;
	}

	// FUNCTION: TOY2 0x0049AB90
	int32_t PlayMovieWithTransition(int32_t movieId, int32_t backgroundId)
	{
		int32_t counter;

		Renderer::SetVirtualRatioTo54();

		if (backgroundId)
			LoadLevelWithFadeIn(backgroundId, 0);

		counter = 60;

		do
		{
			g_cpuTime = timeGetTime();
			g_cpuTicks = g_cpuSpeed / (abs((int32_t)(timeGetTime() - g_cpuTime)) + 1);

			do
				g_cpuTicks = 1000 / (abs((int32_t)(timeGetTime() - g_cpuTime)) + 1);
			while (g_cpuTicks > 60);

			--counter;
		} while (counter);

		if (backgroundId)
			ShowLevelIntroScreen(backgroundId, 0);

		if (g_mpegPlay)
			return 0;
		else
			return PlayMovie(movieId);
	}

	// FUNCTION: TOY2 0x0049EB20
	void UnlockAndPlayMovie(int32_t movieId, int32_t backgroundId, int32_t forcePlay)
	{
		if (! SaveManager::g_save0Data.moviesUnlocked[movieId] || forcePlay)
		{
			SaveManager::g_save0Data.moviesUnlocked[movieId] = 1;
			Toy2::PlayMovieWithTransition(movieId + 10, backgroundId);
		}
	}

	namespace MovieViewer
	{
		// GLOBAL: TOY2 0x004F6E3C
		uint8_t g_unk4F6E3C[80] = {
			66, 1, 65, 3, 67, 4, 65, 3, 67, 5, 65, 3, 67, 3, 65, 3, 67, 2, 65, 3, 65, 3, 65, 3, 66, 3, 65, 3, 66, 0, 65, 3, 67, 1, 65, 3, 65,  2,	65,	 3,
			65, 1, 65, 3, 65, 0, 65, 3, 68, 2, 65, 3, 68, 0, 65, 3, 68, 1, 65, 3, 68, 3, 65, 3, 67, 0, 65, 3, 66, 6, 65, 3, 66, 2, 65, 3, 255, 255, 255, 255
		};

		// GLOBAL: TOY2 0x004F6E8C
		uint8_t g_unk4F6E8C[20] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 16, 12, 13, 14, 15, 17, 18, 255 };

		// FUNCTION: TOY2 0x0043A600
		int32_t Tick(int32_t a1)
		{
			int32_t unlockedBefore;
			uint8_t* idPtr;
			int32_t selected;
			int32_t menuIdx;
			int32_t idIdx;
			uint8_t movieId;
			uint8_t nextEntry;
			int32_t slotPos;
			int32_t curPos;
			int32_t frame;
			int32_t nextFrame;
			int32_t timer;
			int32_t timerCopy;
			int32_t velocity;
			int32_t minPos;
			int32_t maxPos;
			int32_t targetPos;
			int32_t result;
			int32_t speed;
			int32_t scrollX;
			int16_t tile;
			int16_t sheet;
			uint8_t menu[192];

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
			unlockedBefore = 0;
			SaveManager::g_save0Data.moviesUnlocked[0] = 1;

			if (g_unk4F6E8C[0] != a1)
			{
				idPtr = g_unk4F6E8C;

				do
				{
					if (idPtr[1] == 0xFF)
						break;

					if (SaveManager::g_save0Data.moviesUnlocked[*idPtr])
						++unlockedBefore;

					++idPtr;
				} while (*idPtr != a1);
			}

			idIdx = 0;
			selected = g_unk4F6E8C[unlockedBefore] != 0xFF ? unlockedBefore : 0;
			menuIdx = 0;

			if (g_unk4F6E3C[0] != 0xFF)
			{
				do
				{
					movieId = g_unk4F6E8C[idIdx / 4];

					if (SaveManager::g_save0Data.moviesUnlocked[movieId])
					{
						menu[menuIdx] = g_unk4F6E3C[4 * movieId];
						menu[menuIdx + 1] = g_unk4F6E3C[4 * movieId + 1];
						menu[menuIdx + 2] = movieId;
						menuIdx += 4;
					}

					nextEntry = g_unk4F6E3C[4 + idIdx];
					idIdx += 4;
				} while (nextEntry != 0xFF);
			}

			menu[menuIdx] = 0xFF;
			menu[menuIdx + 1] = 0xFF;
			slotPos = selected << 12;
			menu[menuIdx + 2] = 0xFF;
			menu[menuIdx + 3] = 0xFF;
			timer = 4000;
			velocity = 0;
			targetPos = slotPos;
			curPos = slotPos;
			frame = 0;
			AudioManager::PlayMusicLooping(19);
			g_unk55A120 = 1;
			result = -1;
			Nu3D::Camera::SetTint(128, 128, 128, 6);
			maxPos = slotPos + 1024;
			minPos = slotPos - 1024;

			while (1)
			{
				Nu3D::Camera::FadeToTargetTint();
				nextFrame = Renderer::g_frameDelta + frame;

				if (minPos > -1024 && (nextFrame & 0x3Fu) < 0x30)
					Renderer::Sprite::DrawTile(16, 112, 57, 0);

				if (menu[4 * (slotPos >> 12) + 4] != 0xFF && (nextFrame & 0x3Fu) < 0x30)
					Renderer::Sprite::DrawTile(272, 112, 57, 1);

				Renderer::Sprite::DrawTile(0, 0, 60, 0);
				Renderer::Sprite::DrawTile(128, 0, 60, 1);
				Renderer::Sprite::DrawTile(256, 0, 61, 0);
				Renderer::Sprite::DrawClipped(268 - ((curPos >> 5) & 0x7F), 80, 208, 264, 64, 0);
				Renderer::Sprite::DrawClipped(114 - ((curPos >> 5) & 0x7F), 80, 56, 112, 64, 0);

				tile = menu[4 * ((curPos + 2048) >> 12) + 1];
				sheet = menu[4 * ((curPos + 2048) >> 12)];
				Renderer::Sprite::DrawClipped(268 - (((curPos - 2048) >> 5) & 0x7F), 80, 208, 264, sheet, tile);
				Renderer::Sprite::DrawClipped(114 - (((curPos - 2048) >> 5) & 0x7F), 80, 56, 112, sheet, tile);

				scrollX = -(curPos >> 6) % 320;
				Renderer::Sprite::DrawTile(scrollX, 0, 62, 0);
				Renderer::Sprite::DrawTile(scrollX + 128, 0, 62, 1);
				Renderer::Sprite::DrawTile(scrollX + 256, 0, 63, 0);
				Renderer::Sprite::DrawTile(320 - (curPos >> 6) % 320, 0, 62, 0);
				Renderer::Sprite::DrawTile(448 - (curPos >> 6) % 320, 0, 62, 1);
				Renderer::Sprite::DrawTile(576 - (curPos >> 6) % 320, 0, 63, 0);
				Nullsub3();
				MainMenu::RenderMenu();
				timerCopy = timer;

				if (timer > 1000)
				{
					if ((InputManager::g_curButtonsPressed & INPUT_LEFT) != 0 && (InputManager::g_prevButtonsPressed & INPUT_LEFT) == 0 && minPos > -1024
						&& abs(curPos - targetPos) < 2048)
					{
						targetPos -= 4096;
						minPos -= 4096;
						maxPos -= 4096;
						AudioManager::PlayOneShotSoundGlobal(1, 4608, 64, 96);
					}

					if ((InputManager::g_curButtonsPressed & INPUT_RIGHT) != 0 && (InputManager::g_prevButtonsPressed & INPUT_RIGHT) == 0
						&& menu[4 * (targetPos >> 12) + 4] != 0xFF && abs(curPos - targetPos) < 2048)
					{
						targetPos += 4096;
						minPos += 4096;
						maxPos += 4096;
						AudioManager::PlayOneShotSoundGlobal(1, 4608, 96, 64);
					}

					speed = velocity;

					if (velocity >= 0)
					{
						if (velocity <= 0)
						{
						LABEL_37:
							if (curPos < minPos)
							{
								speed += 16 * Renderer::g_frameDelta;
								velocity = speed;
							}

							if (curPos > maxPos)
							{
								speed += -16 * Renderer::g_frameDelta;
								velocity = speed;
							}

							if (speed <= 128)
							{
								if (speed < -128)
								{
									velocity = -128;
									speed = -128;
								}

								curPos += speed;
							}
							else
							{
								velocity = 128;
								curPos += 128;
							}

							goto LABEL_46;
						}

						velocity -= 8 * Renderer::g_frameDelta;

						if (velocity < 0)
							goto LABEL_35;
					}
					else
					{
						velocity += 8 * Renderer::g_frameDelta;

						if (velocity > 0)
						{
						LABEL_35:
							velocity = 0;
						}
					}

					speed = velocity;
					goto LABEL_37;
				}
			LABEL_46:
				Renderer::Sprite::DrawWhiteText("press jump to select", 212, 160);
				Nullsub6();

				if (timer > 0 && timer < 1000)
				{
					timerCopy = timer - Renderer::g_frameDelta;
					timer -= Renderer::g_frameDelta;
				}

				if ((InputManager::g_curButtonsPressed & INPUT_CANCEL) != 0 && (InputManager::g_prevButtonsPressed & INPUT_CANCEL) == 0 && timerCopy > 23
					&& Nu3D::Camera::g_cameraTintBlue == 128)
				{
					timer = 53;
					AudioManager::PlayOneShotSoundGlobal(2, 4608, 96, 96);
					result = -1;
					Nu3D::Camera::SetTint(0, 0, 0, 6);
					timerCopy = 53;
				}

				if ((InputManager::g_curButtonsPressed & INPUT_JUMP) != 0 && (InputManager::g_prevButtonsPressed & INPUT_JUMP) == 0 && timerCopy > 23
					&& Nu3D::Camera::g_cameraTintBlue == 128)
				{
					timer = 53;
					AudioManager::PlayOneShotSoundGlobal(0, 4608, 96, 96);
					result = menu[4 * ((curPos + 2048) >> 12) + 2];
					Nu3D::Camera::SetTint(0, 0, 0, 6);
					goto LABEL_12;
				}

				if (timerCopy <= 0)
					break;
			LABEL_12:
				slotPos = targetPos;
				frame = nextFrame;
			}

			AudioManager::StopAndWait();
			return result;
		}
	}

	// FUNCTION: TOY2 0x00453FA0
	void ShowMovieViewer()
	{
		int32_t oldLevelSave = g_levelFileIndex;
		int32_t selectedMovie;

		g_levelFileIndex = 16;

		selectedMovie = 0;

		while (1)
		{
			Levels::g_levelLoadConfig = 184;

			Renderer::g_virtualScreenWidth = 320.0;
			Renderer::g_virtualScreenHeight = 256.0;

			Levels::InitLevelPlay(g_levelFileIndex);

			MainMenu::g_menuClearColor.b = 0;
			MainMenu::g_menuClearColor.g = 0;
			MainMenu::g_menuClearColor.r = 0;

			selectedMovie = MovieViewer::Tick(selectedMovie);

			if (selectedMovie < 0)
				break;

			PlayMovieWithTransition(selectedMovie + 10, 0);
		}

		g_levelFileIndex = oldLevelSave;
	}

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

	// FUNCTION: TOY2 0x00438520
	int32_t ShowStaticScreen(int32_t backdropIndex)
	{
		int32_t result;
		int32_t timer;
		int32_t maxTimer;

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
		SetBackdropByIndex(backdropIndex);
		result = 0;
		timer = 600;
		maxTimer = g_pastInitialBoot != 0 ? 600 : 300;

		do
		{
			while (1)
			{
				Nu3D::Camera::FadeToTargetTint();
				SoftwareRenderer::UnkFunc67(0, 0);
				Nullsub3();
				MainMenu::RenderMenu();
				timer -= Renderer::g_frameDelta;

				if (timer > 0)
				{
					if (timer > 23)
						goto LABEL_8;
				}
				else
				{
					timer = 0;
				}

				if ((int32_t)(timer + Renderer::g_frameDelta) > 23)
					Nu3D::Camera::SetTint(0, 0, 0, 12);
			LABEL_8:
				if (g_attractModeTimer >= 0 && (InputManager::g_curButtonsPressed & 1) != 0 && timer > 23)
				{
					timer = 24;
					result = 1;
				}

				if ((InputManager::g_curButtonsPressed & (INPUT_CANCEL | INPUT_SPIN | INPUT_JUMP | INPUT_FIRE)) == 0
					|| (InputManager::g_prevButtonsPressed & (INPUT_CANCEL | INPUT_SPIN | INPUT_JUMP | INPUT_FIRE)) != 0 || timer >= maxTimer || timer <= 23)
				{
					break;
				}

				timer = 24;
			}
		} while (timer > 0);

		return result;
	}

	// the credits, the lines are separated with ~ and a ^ in front of a line makes it a heading
	// GLOBAL: TOY2 0x004F5F54
	char g_creditsText[2189] =
		"Congratulations!~~~~You have completed~~~~Toy Story 2!~~~~~~~~~~Traveller's Tales~~"
		"Credits~~~~~~~Game Design and~~Programming~~~^Jon Burton~~~~~3d engine programming~~~"
		"^dave dootson~~~~~pc conversion~~~^steve monks~~~~~software renderer~~~^andy holdroyd~~~~~"
		"character animation~~~^jeremy pardon~~~~~character artwork~~~^neil allen~~^dave burton~~"
		"^jeremy pardon~~^will thompson~~~~~background artwork~~~^neil allen~~^dave burton~~"
		"^leon warren~~^jeremy pardon~~^barry thompson~~^james cunliffe~~^bev bush~~~~~"
		"terrain design~~~^barry thompson~~~~~utility programming~~~^andy holdroyd~~^dave dootson~~"
		"^gary ireland~~~~~qa by~~~^arthur parsons~~~~~directed by~~~^Jon Burton~~~~~~~~~~"
		"ACTIVISION credits~~~~~~Senior Producer~~~^Rob Letts~~~~~Associate Producer~~~"
		"^William Oertel~~~~~VP European Studios~~~^Julian Lynn-Evans~~~~~Executive VP~~"
		"Activision Studios~~~^Mitch Lasky~~~~~QA Manager~~~^Marilena Morini~~~~~"
		"Senior Test Lead~~~^Marietta Pashayan~~~~~Project Lead~~~^Nadine Theuzillot~~~~~"
		"PC Compatibility lead~~~^John Fritts~~~~~Test Team~~~^Richard Kurnadi~~^Kragen Lum~~"
		"^Russell Shirley~~^Daniel Ramirez~~^Christian Biermann~~^David Silverman~~^David Hakim~~"
		"^Keith Harris~~^Brian Ulmer~~^Josh Horowitz~~^Nicole Dodd~~^Eric Zimmerman~~"
		"^Jenn Spencier~~^Peter Muravez~~^Todd D. Jones~~^Chad Bordwell~~^Hector Garcia~~~~~~~~~~"
		"Disney Interactive~~Credits~~~~~~Senior Producer~~~^Dan Winters~~~~~Producer~~~"
		"^Peter Wyse~~~~~Lead Designer~~~^Joel Goodsell~~~~~Original Character~~Design~~~"
		"^Jeff Berting~~^Tom Barlow~~~~~Additional Art~~~^Jeff Berting~~^Tom Barlow~~~~~"
		"Assistant Producer~~~^Renee Johnson~~~~~U.K. Production Lead~~~^Nick Bridger~~~~~"
		"Game Dialogue~~~^Peter Wyse~~~~~Additional Dialogue~~~^Renee Johnson~~~~"
		"Senior Lead Tester~~~^Carlos Schulte~~~~~Lead Tester~~~^Kevin Cope~~~~~Test Team~~~"
		"^Patrick Larkin~~^Andre Aguilar~~^Bryan Martinez~~^Amir Firozkar~~~~~~~~~~~"
		"Music Credits~~~~~~original music score~~~^swallow studios~~~~~sound effects~~~"
		"^p.c. music~~~~~tt logo music~~~^aaron szpakowski~~~~~~~~~~Special Thanks~~~"
		"^Helen Burton~~^John Lasseter~~^Ash Brannon~~^Helene Plotkin~~^Karen Robert Jackson~~"
		"^Katherine Sarafian~~^Kathleen Handy~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

	// FUNCTION: TOY2 0x0043A380
	int32_t ShowCredits()
	{
		int32_t linesDone;
		char* textPtr;
		int32_t lineIdx;
		int32_t i;
		int32_t j;
		int32_t newLines;
		int32_t scroll;
		int32_t exitTimer;
		int32_t exitTimerCopy;
		int32_t linesTotal;
		int32_t backdropIndex;
		int32_t backdropTimer;
		char* line;
		char lineBuffer[2560];
		char ch;

		linesDone = 0;

		textPtr = g_creditsText;

		scroll = 0;
		linesTotal = 0;

		for (i = 0; i < 40; ++i)
			lineBuffer[64 * i] = 0;

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
		MainMenu::g_menuClearColor.b = 0;
		MainMenu::g_menuClearColor.g = 0;
		MainMenu::g_menuClearColor.r = 0;
		exitTimer = 4000;
		AudioManager::PlayMusicLooping(18);
		g_unk55A120 = 1;
		Nu3D::Camera::SetTint(128, 128, 128, 6);

		backdropIndex = 0;
		backdropTimer = 600;

		while (1)
		{
			Nu3D::Camera::FadeToTargetTint();

			if (scroll / 128 > linesDone)
			{
				lineIdx = linesDone + 2;
				newLines = scroll / 128 - linesDone;
				linesTotal = scroll / 128;

				do
				{
					ch = *textPtr;

					for (i = 0; *textPtr != 126; ch = *textPtr)
					{
						if (! ch)
							break;

						j = i + ((lineIdx % 40) << 6);
						++i;
						++textPtr;
						lineBuffer[j] = ch;
					}

					if (*textPtr)
						++textPtr;

					lineBuffer[64 * (lineIdx % 40) + i] = 0;
					++lineIdx;
					--newLines;
				} while (newLines);
			}

			SoftwareRenderer::UnkFunc67(0, 0);
			line = lineBuffer;

			for (j = 320; j < 640; j += 8)
			{
				Renderer::Sprite::DrawWhiteText(line, (j - scroll / 16 % 320) % 320 - 33, 160);
				line += 64;
			}

			Renderer::Sprite::DrawBackdropTransition(&backdropTimer, &backdropIndex, 600);

			Nullsub6();
			exitTimerCopy = exitTimer;
			scroll += 8 * Renderer::g_frameDelta;

			if (exitTimer > 0 && exitTimer < 1000)
			{
				exitTimerCopy = exitTimer - Renderer::g_frameDelta;
				exitTimer -= Renderer::g_frameDelta;
			}

			if ((((InputManager::g_curButtonsPressed & 0xF000) != 0 && (InputManager::g_prevButtonsPressed & 0xF000) == 0) || ! *textPtr) && exitTimerCopy > 53
				&& Nu3D::Camera::g_cameraTintBlue == 128)
			{
				exitTimer = 53;
				Nu3D::Camera::SetTint(0, 0, 0, 6);
				exitTimerCopy = 53;
			}

			MainMenu::RenderMenu();

			if (exitTimerCopy <= 0)
				break;

			linesDone = linesTotal;
		}

		AudioManager::StopAndWait();
		MainMenu::g_menuClearColor.b = 32;
		MainMenu::g_menuClearColor.g = 32;
		MainMenu::g_menuClearColor.r = 32;
		return 32;
	}

	namespace PostGameSaveMenu
	{
		// FUNCTION: TOY2 0x0043A130
		int32_t Tick()
		{
			int32_t timer;
			int32_t choice;

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
			timer = 4000;
			choice = 1;
			AudioManager::PlayMusicLooping(19);
			g_unk55A120 = 1;
			Nu3D::Camera::SetTint(128, 128, 128, 6);

			do
			{
				while (1)
				{
					Nu3D::Camera::FadeToTargetTint();
					SoftwareRenderer::UnkFunc67(0, 0);

					if (timer > 1000)
					{
						if ((InputManager::g_curButtonsPressed & INPUT_LEFT) != 0 && ! choice)
						{
							choice = 1;
							AudioManager::PlayOneShotSoundGlobal(1, 4608, 64, 96);
						}

						if ((InputManager::g_curButtonsPressed & INPUT_RIGHT) != 0 && choice == 1)
						{
							choice = 0;
							AudioManager::PlayOneShotSoundGlobal(1, 4608, 96, 64);
						LABEL_9:
							Renderer::Sprite::DrawTile(32, 112, 57, 0);
							goto LABEL_10;
						}
					}

					if (! choice)
						goto LABEL_9;

					Renderer::Sprite::DrawTile(448, 112, 57, 1);
				LABEL_10:
					Nullsub3();
					Renderer::Sprite::DrawWhiteText(LevelSelect::g_jumpToSelectTxt, 200, 256);
					Renderer::Sprite::DrawWhiteText("SAVE GAME?", 48, 256);
					Renderer::Sprite::DrawWhiteText("   /   ", 128, 256);

					if (choice)
					{
						Renderer::Sprite::DrawWhiteText("YES    ", 128, 256);
						Renderer::Sprite::DrawBitmapText("    NO ", 128, 256, 64, 64, 64);
					}
					else
					{
						Renderer::Sprite::DrawWhiteText("    NO ", 128, 256);
						Renderer::Sprite::DrawBitmapText("YES    ", 128, 256, 64, 64, 64);
					}

					Nullsub6();
					MainMenu::RenderMenu();

					if (timer < 1000)
						timer -= Renderer::g_frameDelta;

					if ((InputManager::g_curButtonsPressed & 0x4000) == 0 || (InputManager::g_prevButtonsPressed & 0x4000) != 0 || timer <= 23
						|| Nu3D::Camera::g_cameraTintBlue != 128)
					{
						break;
					}

					Nu3D::Camera::SetTint(0, 0, 0, 6);
					timer = 53;
					AudioManager::PlayOneShotSoundGlobal(0, 4608, 96, 96);
				}
			} while (timer > 0);

			AudioManager::StopAndWait();
			return choice;
		}
	}

	// FUNCTION: TOY2 0x00454020
	void ShowPostGameSaveMenu()
	{
		int32_t previousLevel = g_levelFileIndex;
		g_levelFileIndex = 16;

		if (g_savePromptPending)
		{
			Levels::g_levelLoadConfig = 248;
			g_hasStaticBackdrop = 0;

			Renderer::g_virtualScreenWidth = 512.0;
			Renderer::g_virtualScreenHeight = 256.0;
			g_nextBackdropId = 36;

			Levels::InitLevelPlay(16);

			g_savePromptPending = PostGameSaveMenu::Tick();

			if (g_savePromptPending)
			{
				g_savePromptPending = 1;

				SaveManager::TransferProgressData(&SaveManager::g_save0Data);
				TickSaveMenuMachine(1);
				SaveManager::LoadProgressData(&SaveManager::g_save0Data);

				g_savePromptPending = 0;
			}
		}

		g_levelFileIndex = previousLevel;
	}

	// FUNCTION: TOY2 0x00453D90
	void ShowActClearScreen()
	{
		int32_t timer;
		int32_t pressed;
		int32_t blink;
		int32_t exitTimer;
		int32_t previousLevel = g_levelFileIndex;

		g_levelFileIndex += 32;
		Renderer::g_virtualScreenWidth = 320.0;
		Renderer::g_virtualScreenHeight = 256.0;
		LoadLevelGraphics(g_levelFileIndex);
		SoftwareRenderer::g_unk4F7400.x = 0;
		SoftwareRenderer::g_unk4F7400.y = 0;
		Nu3D::Camera::SetTint(128, 128, 128, 12);
		timer = 28;
		AudioManager::PlayMusicOneShot(21);

		do
		{
			timer -= Renderer::g_frameDelta;

			if (timer <= 0)
				timer = 0;

			Nu3D::Camera::FadeToTargetTint();
			MainMenu::RenderMenu();
		} while (timer);

		pressed = 0;
		blink = 0;
		SoftwareRenderer::g_unk4F7400.x = 0;
		SoftwareRenderer::g_unk4F7400.y = 0;
		exitTimer = 28;

		do
		{
			Nu3D::Camera::FadeToTargetTint();

			if (pressed)
			{
				exitTimer -= Renderer::g_frameDelta;

				if (exitTimer <= 0)
					exitTimer = 0;
			}

			if ((InputManager::g_curButtonsPressed & 0x4000) != 0 && (InputManager::g_prevButtonsPressed & 0x4000) == 0 && ! pressed)
			{
				Nu3D::Camera::SetTint(0, 0, 0, 12);
				pressed = 1;
				AudioManager::PlaySoundEffect(7, 0);
			}

			blink = (Renderer::g_frameDelta + blink) & 0x3F;

			if (blink > 30)
				Renderer::Sprite::DrawScaledFixed(96, 200, 128, 0, 255, 255, 255, 255, 2048, 2048);

			MainMenu::RenderMenu();
		} while (exitTimer);

		AudioManager::StopAndWait();
		g_hasStaticBackdrop = 0;
		SoftwareRenderer::g_unk4F7400.x = -32768;
		SoftwareRenderer::g_unk4F7400.y = -32768;
		Renderer::g_virtualScreenWidth = 512.0;
		Renderer::g_virtualScreenHeight = 256.0;
		g_nextBackdropId = 36;
		g_savePromptPending = 1;
		g_levelFileIndex = previousLevel;
	}

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

	// one word of the TOY2.INI, the table ends with a token value of -1
	struct IniToken
	{
		char* tokenName;
		int32_t tokenValue;
	};

	// GLOBAL: TOY2 0x004F4F60
	IniToken g_iniTokens[47] = {
		{ "US", 1 },		  { "DE", 2 },		 { "FR", 3 },		 { "NL", 10 },		  { "DK", 4 },		 { "SW", 5 },		{ "IT", 6 },
		{ "CT", 7 },		  { "FN", 8 },		 { "NW", 9 },		 { "LANGUAGE", 12 },  { "DEFTEXT", 13 }, { "KEY", 14 },		{ "JOY", 15 },
		{ "MESSAGE", 16 },	  { "QUIET", 17 },	 { "FINISHED", 47 }, { "CHEAT", 18 },	  { "HIMPEG", 19 },	 { "NOMPEG", 20 },	{ "LOG", 22 },
		{ "IF", 21 },		  { "ENDIF", 23 },	 { "DEFKEY", 24 },	 { "FIRE", 25 },	  { "JUMP", 26 },	 { "CAMLEFT", 27 }, { "CAMRIGHT", 28 },
		{ "HELMETVIEW", 29 }, { "PAUSE", 30 },	 { "SELECT", 31 },	 { "DERVISH", 32 },	  { "PAD_LL", 33 },	 { "PAD_LU", 34 },	{ "PAD_LR", 36 },
		{ "PAD_LD", 35 },	  { "PAD_RL", 37 },	 { "PAD_RU", 38 },	 { "PAD_RR", 40 },	  { "PAD_RD", 39 },	 { "PAD_FLT", 41 }, { "PAD_FLB", 42 },
		{ "PAD_FRT", 43 },	  { "PAD_FRB", 44 }, { "PAD_SEL", 45 },	 { "PAD_START", 46 }, { 0, -1 },
	};

	// GLOBAL: TOY2 0x0053407C
	char g_iniTokenBuffer[1024];

	// GLOBAL: TOY2 0x00534070
	FILE* g_iniFileFd;

	// the number of DEFTEXT lines that were read, they are stored in g_defTextStrings
	// GLOBAL: TOY2 0x0053406C
	int32_t g_defTextCount;

	// GLOBAL: TOY2 0x00534480
	int32_t g_languageToken;

	// GLOBAL: TOY2 0x0053006C
	char g_defTextStrings[16384];

	// GLOBAL: TOY2 0x00882C24
	int32_t g_useRelativePathForIni;

	// GLOBAL: TOY2 0x00882A20
	char g_empty512Chars[512];

	// GLOBAL: TOY2 0x00882C2C
	char g_empty512Chars_2[512];

	// GLOBAL: TOY2 0x00504D54
	char g_iniCdPath[256] = "cd\\";

	namespace Ini
	{
		// a key name of the INI and the scancode it stands for
		struct KeyNameEntry
		{
			uint16_t scancode;
			uint16_t unk2;
			uint16_t keyName;
		};

		// the position and the text of one entry of the key and gamepad configuration, ParseDefText replaces both
		struct DefTextEntry
		{
			int32_t x;
			int32_t y;
			int32_t unk8;
			char* text;
			int32_t unk10;
			int32_t unk14;
		};

		// the position and the text of one message, ParseDefText replaces both
		struct DefTextMessage
		{
			int32_t x;
			int32_t y;
			char* text;
			int32_t unkC;
		};

		// GLOBAL: TOY2 0x004EFCE0
		KeyNameEntry g_keyNameTable[123] = {
			{ 200, 39, 94 },   { 208, 40, 36 },	  { 203, 41, 40 },	 { 205, 42, 41 },		{ 1, 57, 32 },		{ 2, 44, 49 },		{ 3, 45, 50 },
			{ 4, 46, 51 },	   { 5, 47, 52 },	  { 6, 48, 53 },	 { 7, 49, 54 },			{ 8, 50, 55 },		{ 9, 51, 56 },		{ 10, 52, 57 },
			{ 11, 43, 48 },	   { 12, 34, 45 },	  { 14, 65535, 32 }, { 15, 59, 32 },		{ 16, 16, 113 },	{ 17, 22, 119 },	{ 18, 4, 101 },
			{ 19, 17, 114 },   { 20, 19, 116 },	  { 21, 25, 122 },	 { 22, 20, 117 },		{ 23, 8, 105 },		{ 24, 14, 111 },	{ 25, 15, 112 },
			{ 26, 30, 91 },	   { 27, 31, 93 },	  { 28, 56, 32 },	 { 29, 53, 32 },		{ 30, 0, 97 },		{ 31, 18, 115 },	{ 32, 3, 100 },
			{ 33, 5, 102 },	   { 34, 6, 103 },	  { 35, 7, 104 },	 { 36, 9, 106 },		{ 37, 10, 107 },	{ 38, 11, 108 },	{ 39, 33, 59 },
			{ 41, 65535, 32 }, { 42, 58, 32 },	  { 43, 32, 47 },	 { 44, 24, 121 },		{ 45, 23, 120 },	{ 46, 2, 99 },		{ 47, 21, 118 },
			{ 48, 1, 98 },	   { 49, 13, 110 },	  { 50, 12, 109 },	 { 51, 28, 44 },		{ 52, 29, 46 },		{ 53, 36, 32 },		{ 54, 58, 32 },
			{ 56, 54, 32 },	   { 184, 54, 32 },	  { 57, 55, 32 },	 { 58, 65535, 32 },		{ 59, 65535, 32 },	{ 60, 65535, 32 },	{ 61, 65535, 32 },
			{ 62, 65535, 32 }, { 63, 65535, 32 }, { 64, 65535, 32 }, { 65, 65535, 32 },		{ 66, 65535, 32 },	{ 67, 65535, 32 },	{ 68, 65535, 32 },
			{ 69, 65535, 32 }, { 70, 65535, 32 }, { 71, 65535, 55 }, { 72, 65535, 32 },		{ 73, 65535, 57 },	{ 74, 65535, 45 },	{ 75, 65535, 52 },
			{ 76, 65535, 53 }, { 77, 65535, 54 }, { 78, 65535, 43 }, { 79, 65535, 49 },		{ 80, 65535, 50 },	{ 81, 65535, 51 },	{ 82, 65535, 48 },
			{ 83, 29, 46 },	   { 87, 65535, 32 }, { 88, 65535, 32 }, { 100, 65535, 32 },	{ 101, 65535, 32 }, { 102, 65535, 32 }, { 41, 27, 35 },
			{ 0, 69, 65 },	   { 0, 70, 66 },	  { 0, 71, 67 },	 { 0, 72, 68 },			{ 0, 73, 69 },		{ 0, 74, 70 },		{ 0, 75, 71 },
			{ 0, 76, 72 },	   { 0, 77, 73 },	  { 0, 78, 74 },	 { 0, 79, 75 },			{ 0, 80, 76 },		{ 0, 81, 77 },		{ 0, 82, 78 },
			{ 0, 83, 79 },	   { 0, 84, 80 },	  { 0, 85, 81 },	 { 0, 86, 82 },			{ 0, 87, 83 },		{ 0, 88, 84 },		{ 0, 89, 85 },
			{ 0, 90, 86 },	   { 0, 91, 87 },	  { 0, 92, 88 },	 { 0, 93, 89 },			{ 0, 94, 90 },		{ 0, 64, 163 },		{ 0, 96, 37 },
			{ 0, 97, 38 },	   { 0, 60, 42 },	  { 0, 95, 96 },	 { 65535, 65535, 255 },
		};

		// GLOBAL: TOY2 0x004EFFC8
		DefTextEntry g_unk4EFFC8[14] = {
			{ 60, 40, 0, "forward", 0 },		   { 60, 60, 0, "back", 0 },		 { 60, 80, 0, "left", 0 },		  { 60, 100, 0, "right", 0 },
			{ 60, 120, 0, "fire/get/drop", 0 },	   { 60, 140, 0, "jump/change", 0 }, { 60, 160, 0, "seed color", 0 }, { 200, 40, 0, "camera", 0 },
			{ 200, 60, 0, "lock camera", 0 },	   { 240, 80, 0, "walk", 0 },		 { 200, 100, 0, "kick", 0 },	  { 80, 180, 0, "game pad", 0 },
			{ 80, 200, 0, "restore defaults", 0 }, { 80, 220, 0, "accept", 0 },
		};

		// GLOBAL: TOY2 0x004F0114
		DefTextEntry* g_unk4F0114[15] = {
			&g_unk4EFFC8[0], &g_unk4EFFC8[1], &g_unk4EFFC8[2],	&g_unk4EFFC8[3],  &g_unk4EFFC8[4],	&g_unk4EFFC8[5],  &g_unk4EFFC8[6],	&g_unk4EFFC8[7],
			&g_unk4EFFC8[8], &g_unk4EFFC8[9], &g_unk4EFFC8[10], &g_unk4EFFC8[11], &g_unk4EFFC8[12], &g_unk4EFFC8[13], (DefTextEntry*)-1
		};

		// GLOBAL: TOY2 0x004F0150
		DefTextEntry g_unk4F0150[10] = {
			{ 40, 40, 0, "fire/get/drop", 0 },	   { 40, 60, 0, "jump/change", 256 }, { 40, 80, 0, "seed color", 512 }, { 40, 100, 0, "camera", 768 },
			{ 200, 40, 0, "lock camera", 1024 },   { 200, 60, 0, "walk", 1280 },	  { 200, 80, 0, "kick", 1536 },		{ 80, 180, 0, "keyboard", 0 },
			{ 80, 200, 0, "restore defaults", 0 }, { 80, 220, 0, "accept", 0 },
		};

		// GLOBAL: TOY2 0x004F023C
		DefTextEntry* g_unk4F023C[11] = {
			&g_unk4F0150[0], &g_unk4F0150[1], &g_unk4F0150[2], &g_unk4F0150[3], &g_unk4F0150[4],  &g_unk4F0150[5],
			&g_unk4F0150[6], &g_unk4F0150[7], &g_unk4F0150[8], &g_unk4F0150[9], (DefTextEntry*)-1
		};

		// GLOBAL: TOY2 0x004F5398
		DefTextMessage g_unk4F5398[25] = {
			{ 20, 200, "select option and press enter", 0 },
			{ 90, 200, "() - to change", 0 },
			{ 20, 220, "press enter to accept changes", 0 },
			{ 50, 240, "press esc to go back", 0 },
			{ 70, 120, "do you want to quit", 0 },
			{ 90, 4, "video options", 0 },
			{ 100, 4, "define keys", 0 },
			{ 130, 4, "save game", 0 },
			{ 10, 200, "select file", 0 },
			{ 10, 220, "press esc to go back", 0 },
			{ 130, 4, "confirm", 0 },
			{ 80, 180, "do you wish to", 0 },
			{ 70, 200, "save over this game", 0 },
			{ 130, 4, "enter name", 0 },
			{ 130, 4, "load game", 0 },
			{ 10, 200, "select game and press enter", 0 },
			{ 10, 220, "press esc to go back", 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		};

		// GLOBAL: TOY2 0x004F5524
		DefTextMessage* g_unk4F5524[25] = {
			&g_unk4F5398[0],  &g_unk4F5398[1],	&g_unk4F5398[2],  &g_unk4F5398[3],	&g_unk4F5398[4],  &g_unk4F5398[5],	&g_unk4F5398[6],
			&g_unk4F5398[7],  &g_unk4F5398[8],	&g_unk4F5398[9],  &g_unk4F5398[10], &g_unk4F5398[11], &g_unk4F5398[12], &g_unk4F5398[13],
			&g_unk4F5398[14], &g_unk4F5398[15], &g_unk4F5398[16], &g_unk4F5398[17], &g_unk4F5398[18], &g_unk4F5398[19], &g_unk4F5398[20],
			&g_unk4F5398[21], &g_unk4F5398[22], &g_unk4F5398[23], &g_unk4F5398[24]
		};

		// FUNCTION: TOY2 0x00430BF0
		void ParseDefText()
		{
			IniToken* curToken;
			int32_t tokenType;
			int32_t tokenValue;
			char* buffer;
			FILE* file;
			int32_t ch;
			int32_t index;
			uint8_t byteCh;
			char* source;
			int32_t x;
			int32_t y;
			DefTextEntry** table = 0;
			DefTextEntry* entry;
			DefTextMessage* message;
			char text[256];

			if (fscanf(g_iniFileFd, "%s", g_iniTokenBuffer) == -1)
				Logger::Log("EOF reached .\n");

			curToken = g_iniTokens;

			if (g_iniTokenBuffer[0] == '#')
			{
				tokenType = 11;
			}
			else if (g_iniTokens[0].tokenValue == -1)
			{
			LABEL_8:
				tokenType = -1;
			}
			else
			{
				while (strcmp(curToken->tokenName, g_iniTokenBuffer))
				{
					tokenValue = curToken[1].tokenValue;
					++curToken;

					if (tokenValue == -1)
						goto LABEL_8;
				}

				tokenType = curToken->tokenValue;
			}

			switch (tokenType)
			{
				case 14:
					table = g_unk4F0114;
					break;
				case 15:
					table = g_unk4F023C;
					break;
				case 16:
					break;
				default:
					buffer = g_iniTokenBuffer;
					memset(g_iniTokenBuffer, 0, sizeof(g_iniTokenBuffer));
					file = g_iniFileFd;

					do
					{
						ch = fgetc(file);

						if (buffer)
							*buffer++ = ch;
					} while (ch != -1 && ch != 10);

					Logger::Log("UNKNOWN DefText : %s", g_iniTokenBuffer);
					return;
			}

			if (fscanf(g_iniFileFd, "%s", g_iniTokenBuffer) == -1)
				Logger::Log("EOF reached .\n");

			index = atoi(g_iniTokenBuffer);
			file = g_iniFileFd;

			do
				byteCh = fgetc(file);
			while (byteCh != '"' && byteCh != -1);

			file = g_iniFileFd;
			source = text;

			do
			{
				byteCh = fgetc(file);
				*source++ = byteCh;
			} while (byteCh != '"' && byteCh != -1);

			*(source - 1) = 0;

			if (fscanf(g_iniFileFd, "%s", g_iniTokenBuffer) == -1)
				Logger::Log("EOF reached .\n");

			x = atoi(g_iniTokenBuffer);

			if (fscanf(g_iniFileFd, "%s", g_iniTokenBuffer) == -1)
				Logger::Log("EOF reached .\n");

			y = atoi(g_iniTokenBuffer);

			if (g_defTextCount < 64)
			{
				strncpy(&g_defTextStrings[256 * g_defTextCount], text, 256);

				if (tokenType <= 15)
				{
					entry = table[index];
					entry->text = &g_defTextStrings[256 * g_defTextCount];
					entry->x = x;
					entry->y = y;
				}
				else
				{
					message = g_unk4F5524[index];
					message->text = &g_defTextStrings[256 * g_defTextCount];
					message->x = x;
					message->y = y;
				}

				++g_defTextCount;
			}
		}

		// FUNCTION: TOY2 0x00415640
		int16_t KeyNameToScancode(char keyName)
		{
			int32_t index = 0;
			int16_t result;
			KeyNameEntry* entry;

			switch (keyName)
			{
				case '#':
					result = 0x39;
					break;
				case '%':
					result = 0xF;
					break;
				case '+':
					result = 28;
					break;
				case '<':
					result = 203;
					break;
				case '>':
					result = 205;
					break;
				case '@':
					result = 29;
					break;
				case '[':
					result = 42;
					break;
				case '^':
					result = 200;
					break;
				case 'v':
					result = 208;
					break;
				default:
					if (g_keyNameTable[0].scancode == 0xFFFF)
					{
					LABEL_15:
						result = -1;
					}
					else
					{
						entry = g_keyNameTable;

						while ((char)entry->keyName != keyName)
						{
							++entry;
							++index;

							if (entry->scancode == 0xFFFF)
								goto LABEL_15;
						}

						result = g_keyNameTable[index].scancode;
					}
					break;
			}

			return result;
		}
	}

	// FUNCTION: TOY2 0x00430E90
	void ReadIniFile()
	{
		FILE* cDriveFd;
		char* rootPath;
		IniToken* curTokenLookup;
		int32_t tokenValue;
		int32_t tokenValueEnd;
		int32_t inputCode;
		int32_t keyTokenValue;
		char* buffer;
		FILE* file;
		int32_t ch;
		char filePath[1024];

		memset(filePath, 0, sizeof(filePath));

		g_defTextCount = 0;

		cDriveFd = fopen("c:\\toy2.ini", "rb");

		if (cDriveFd) // Check for the INI on the c drive first
		{
			fclose(cDriveFd);
			rootPath = "c:\\";
		}
		else
		{
			if (! FileUtils::GetFileSize("toy2.ini"))
			{
				Logger::Log("ReadIniFile : No TOY2.INI file found.\n");
				return;
			}

			if (g_useRelativePathForIni)
			{
				strcat(filePath, g_empty512Chars);
				rootPath = g_iniCdPath;
			}
			else
			{
				strcat(filePath, g_empty512Chars_2);
				rootPath = "\\cd\\";
			}
		}

		strcat(filePath, rootPath);
		strcat(filePath, "toy2.ini");

		g_iniFileFd = fopen(filePath, "rt");

		if (g_iniFileFd)
		{
			while (1)
			{
			LABEL_10:
				if (fscanf(g_iniFileFd, "%s", g_iniTokenBuffer) == -1)
					Logger::Log("EOF reached .\n");

				curTokenLookup = g_iniTokens;

				if (g_iniTokenBuffer[0] == '#')
				{
					tokenValue = 11;
				}
				else if (g_iniTokens[0].tokenValue == -1)
				{
				LABEL_17:
					tokenValue = -1;
				}
				else
				{
					while (strcmp(curTokenLookup->tokenName, g_iniTokenBuffer))
					{
						tokenValueEnd = curTokenLookup[1].tokenValue;
						++curTokenLookup;

						if (tokenValueEnd == -1)
							goto LABEL_17;
					}

					tokenValue = curTokenLookup->tokenValue;
				}

				switch (tokenValue)
				{
					case 11:
						buffer = g_iniTokenBuffer;
						memset(g_iniTokenBuffer, 0, sizeof(g_iniTokenBuffer));
						file = g_iniFileFd;

						do
						{
							ch = fgetc(file);

							if (buffer)
								*buffer++ = ch;
						} while (ch != -1 && ch != 10);
						continue;
					case 12:
						file = g_iniFileFd;

						do
						{
							if (fscanf(file, "%s", g_iniTokenBuffer) == -1)
								Logger::Log("EOF reached .\n");
						} while (strcmp("=", g_iniTokenBuffer));

						if (fscanf(g_iniFileFd, "%s", g_iniTokenBuffer) == -1)
							Logger::Log("EOF reached .\n");

						curTokenLookup = g_iniTokens;

						if (g_iniTokenBuffer[0] == '#')
							goto LABEL_80;

						if (g_iniTokens[0].tokenValue == -1)
							continue;

						while (strcmp(curTokenLookup->tokenName, g_iniTokenBuffer))
						{
							tokenValue = curTokenLookup[1].tokenValue;
							++curTokenLookup;

							if (tokenValue == -1)
								goto LABEL_10;
						}

						if (curTokenLookup->tokenValue >= 0)
						{
						LABEL_80:
							Logger::Log("ReadIniFile : LANGUAGE set to %s.\n", g_iniTokenBuffer);
							strncpy(AudioManager::g_langPrefix, g_iniTokenBuffer, 2);
							curTokenLookup = g_iniTokens;

							if (AudioManager::g_langPrefix[0] == '#')
							{
								g_languageToken = 11;
							}
							else if (g_iniTokens[0].tokenValue == -1)
							{
							LABEL_85:
								g_languageToken = -1;
							}
							else
							{
								while (strcmp(curTokenLookup->tokenName, AudioManager::g_langPrefix))
								{
									tokenValue = curTokenLookup[1].tokenValue;
									++curTokenLookup;

									if (tokenValue == -1)
										goto LABEL_85;
								}

								g_languageToken = curTokenLookup->tokenValue;
							}
						}

						continue;
					case 13:
						Ini::ParseDefText();

						continue;
					case 17:
						AudioManager::g_quiet = 1;
						Logger::Log("ReadIniFile : QUIET set TRUE\n");

						continue;
					case 18:
						g_cheatsEnabled = g_cheatsEnabled == 0;

						continue;
					case 19:
						g_highQualityMpegEnabled = g_highQualityMpegEnabled == 0;

						continue;
					case 20:
						g_mpegPlay = 1;
						Logger::Log("ReadIniFile : Mpeg play OFF.\n");

						continue;
					case 21:
						if (fscanf(g_iniFileFd, "%s", g_iniTokenBuffer) == -1)
							Logger::Log("EOF reached .\n");

						if (strcmp(g_iniTokenBuffer, AudioManager::g_langPrefix))
						{
							file = g_iniFileFd;

							do
							{
								if (fscanf(file, "%s", g_iniTokenBuffer) == -1)
									Logger::Log("EOF reached .\n");
							} while (strcmp("ENDIF", g_iniTokenBuffer));
						}
						continue;
					case 22:
						Logger::g_logsEnabled = 1;

						continue;
					case 24:
						if (fscanf(g_iniFileFd, "%s", g_iniTokenBuffer) == -1)
							Logger::Log("EOF reached .\n");

						curTokenLookup = g_iniTokens;

						if (g_iniTokenBuffer[0] == '#')
						{
							keyTokenValue = 11;
						}
						else if (g_iniTokens[0].tokenValue == -1)
						{
						LABEL_27:
							keyTokenValue = -1;
						}
						else
						{
							while (strcmp(curTokenLookup->tokenName, g_iniTokenBuffer))
							{
								tokenValueEnd = curTokenLookup[1].tokenValue;
								++curTokenLookup;

								if (tokenValueEnd == -1)
									goto LABEL_27;
							}

							keyTokenValue = curTokenLookup->tokenValue;
						}

						if (fscanf(g_iniFileFd, "%s", g_iniTokenBuffer) == -1)
							Logger::Log("EOF reached .\n");

						_strlwr(g_iniTokenBuffer);
						inputCode = Ini::KeyNameToScancode(g_iniTokenBuffer[0]);

						switch (keyTokenValue)
						{
							case 25:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[6].dInputCode = inputCode;

								break;
							case 26:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[5].dInputCode = inputCode;

								break;
							case 27:
							case 44:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[8].dInputCode = inputCode;

								break;
							case 28:
							case 42:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[9].dInputCode = inputCode;

								break;
							case 29:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[11].dInputCode = inputCode;

								break;
							case 30:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[13].dInputCode = inputCode;

								break;
							case 31:
							case 45:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[12].dInputCode = inputCode;

								break;
							case 32:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[7].dInputCode = inputCode;

								break;
							case 37:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[6].dInputCode = inputCode;

								break;
							case 38:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[4].dInputCode = inputCode;

								break;
							case 39:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[5].dInputCode = inputCode;

								break;
							case 40:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[7].dInputCode = inputCode;

								break;
							case 41:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[11].dInputCode = inputCode;

								break;
							case 43:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[10].dInputCode = inputCode;

								break;
							case 46:
								if (inputCode >= 0)
									SaveManager::g_save99Data.saveStructs[13].dInputCode = inputCode;

								break;
							default:
								continue;
						}
						break;
					case 47:
						fclose(g_iniFileFd);
						return;
					default:
						continue;
				}
			}
		}

		Logger::Log("ReadIniFile : Failed to open TOY2.INI.\n");
	}

	// the result of the CPU speed test
	struct CpuProfile
	{
		int32_t avgTime;
		int32_t testIteration;
		int32_t startTime;
		int32_t loopCounter;
		int32_t time[10];
		float cpuSpeedInMhz;
	};

	STATIC_ASSERT(sizeof(CpuProfile) == 0x3C);

	// GLOBAL: TOY2 0x00726F44
	CpuProfile g_cpuProfile;

	// FUNCTION: TOY2 0x0047EF80
	void ProfileCPU()
	{
		DWORD time;
		double speedInMhz;

		g_cpuProfile.avgTime = 0;

		for (g_cpuProfile.testIteration = 0; g_cpuProfile.testIteration < 10; g_cpuProfile.avgTime += g_cpuProfile.time[g_cpuProfile.testIteration++])
		{
			g_cpuProfile.startTime = timeGetTime();

			for (g_cpuProfile.loopCounter = 0; g_cpuProfile.loopCounter < 1000000; ++g_cpuProfile.loopCounter)
				g_cpuProfile.cpuSpeedInMhz = g_cpuProfile.loopCounter / 50000.0 * 50000.0;

			time = timeGetTime();

			g_cpuProfile.time[g_cpuProfile.testIteration] = abs((int32_t)(time - g_cpuProfile.startTime));
		}

		g_cpuProfile.avgTime /= 10;

#ifdef APPLY_FIXES
		// On modern CPUs the 1M-iteration loop finishes within one timer tick, so every sample is 0 and the
		// original divides by zero (0xC0000094 at 0x411E5F).
		if (g_cpuProfile.avgTime == 0)
			g_cpuProfile.avgTime = 1;
#endif

		g_cpuSpeed = 1000000 * (56000 / g_cpuProfile.avgTime);

		Logger::Log("CalculateClockSpeed->time[0]=%i\n", g_cpuProfile.time[0]);
		Logger::Log("                   ->time[1]=%i\n", g_cpuProfile.time[1]);
		Logger::Log("                   ->time[2]=%i\n", g_cpuProfile.time[2]);
		Logger::Log("                   ->time[3]=%i\n", g_cpuProfile.time[3]);
		Logger::Log("                   ->time[4]=%i\n", g_cpuProfile.time[4]);
		Logger::Log("                   ->time[5]=%i\n", g_cpuProfile.time[5]);
		Logger::Log("                   ->time[6]=%i\n", g_cpuProfile.time[6]);
		Logger::Log("                   ->time[7]=%i\n", g_cpuProfile.time[7]);
		Logger::Log("                   ->time[8]=%i\n", g_cpuProfile.time[8]);
		Logger::Log("                   ->time[9]=%i\n", g_cpuProfile.time[9]);
		Logger::Log("                   ->avgtime=%i\n", g_cpuProfile.avgTime);
		Logger::Log("                   ->CPUSPEED=%u\n", g_cpuSpeed);

		speedInMhz = g_cpuSpeed / 1000000.0;
		g_cpuProfile.cpuSpeedInMhz = speedInMhz;

		Logger::Log("INIT : CPU clock speed is %f MHZ\n", speedInMhz);
	}

	// FUNCTION: TOY2 0x0048E730
	void OneInit()
	{
		char* arg;
		const char* version;
		FILE* saveFile;
		FILE* secondSaveFile;
		size_t configStrLength;
		char saveFilePath[256];
		char secondSaveFileName[256];

		Logger::g_logsEnabled = 0;
		g_mpegPlay = 0;
		g_cheatsEnabled = 0;
		g_highQualityMpegEnabled = 0;

		for (arg = strtok(D3DApp::g_windowData.lpCmdLine, " -"); arg; arg = strtok(0, " -"))
		{
			if (! lstrcmpA(arg, "cheat"))
				g_cheatsEnabled = 1;

			if (! lstrcmpA(arg, "log"))
				Logger::g_logsEnabled = 1;

			if (! lstrcmpA(arg, "mpeg"))
			{
				g_mpegPlay = 1;
				Logger::Log("ONEINIT : Mpeg play OFF.\n");
			}

			if (! lstrcmpA(arg, "high"))
			{
				g_highQualityMpegEnabled = 1;
				Logger::Log("ONEINIT : Hi quality mpeg playing ON.\n");
			}

			if (! lstrcmpA(arg, "demo"))
			{
				g_isDemo = g_isDemo == 0;
				version = "demo";

				if (g_isDemo != 1)
					version = "full";

				Logger::Log("ONEINIT : Switched to %s version.\n", version);
			}
		}

		SaveManager::Init();
		Nullsub1();

		SaveManager::g_save0Data.musicVolume = 128;
		SaveManager::g_save0Data.soundVolume = 255;

		AudioManager::SetVolumesProcessed(128, 255);

		// slot 0 is the settings of the game, slot 99 the controls
		sprintf(saveFilePath, "Toy2%02d.sav", 0);
		saveFile = fopen(saveFilePath, "rb");

		if (saveFile)
		{
			fread(&configStrLength, 1, 4, saveFile);

			if (configStrLength)
			{
				fread(secondSaveFileName, 1, configStrLength, saveFile);
				secondSaveFileName[configStrLength] = 0;
			}

			fread(&SaveManager::g_save0Data, 1, 392, saveFile);
			fclose(saveFile);
		}

		sprintf(secondSaveFileName, "Toy2%02d.sav", 99);
		secondSaveFile = fopen(secondSaveFileName, "rb");

		if (secondSaveFile)
		{
			fread(&configStrLength, 1, 4, secondSaveFile);

			if (configStrLength)
			{
				fread(saveFilePath, 1, configStrLength, secondSaveFile);
				saveFilePath[configStrLength] = 0;
			}

			fread(&SaveManager::g_save99Data, 1, 392, secondSaveFile);
			fclose(secondSaveFile);
		}

		SaveManager::InitProgressData(&SaveManager::g_save0Data);
		SaveManager::LoadProgressData(&SaveManager::g_save0Data);

		g_levelIndex = 0;
		g_randDatBufferPtr = g_randDatBuffer;

		ProfileCPU();
		Nullsub7();
		ReadIniFile();
	}

	// FUNCTION: TOY2 0x00412E80
	int32_t CleanupManagers()
	{
		InputManager::Cleanup();
		AudioManager::StopAndFlush();
		AudioManager::ReleaseBuffers();
		Renderer::Cleanup();
		DrawingDevice::Quit();
		return 1;
	}

	// FUNCTION: TOY2 0x00490730
	void CheckForQuit()
	{
		if (D3DApp::g_windowData.wndIsExiting)
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

			Toy2::CleanupManagers();
			D3DApp::PostQuitMessage();

			CoUninitialize();

			D3DApp::g_windowData.mainHwnd = 0;
			Logger::Log("CheckForQuit : Code shutdown.\n");

			g_clearScreenSaveResult = SystemParametersInfoA(SPI_SETSCREENSAVERRUNNING, 0, &g_setScreenSaveRunning, 0);

			if (g_clearScreenSaveResult)
				Logger::Log("Managed to clear SCREENSAVERRUNNING\n");
			else
				Logger::Log("Failed to clear SCREENSAVERRUNNING\n");

			exit(D3DApp::g_windowData.wndEventMsg.wParam);
		}
	}

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

	// GLOBAL: TOY2 0x0072E340
	int32_t g_unused72E340;

	// FUNCTION: TOY2 0x00490BF0
	int16_t UpdateD3DState()
	{
		g_unused72E340 = 0;

		RECT* destRect = DrawingDevice::GetDestRect();

		Nu3D::Camera::g_screenWidth = destRect->right - destRect->left;
		int32_t height = destRect->bottom - destRect->top;

		Nu3D::Camera::g_viewportWidth = Nu3D::Camera::g_screenWidth;
		Nu3D::Camera::g_viewportRight = height - 1;
		Nu3D::Camera::g_viewportBottom = Nu3D::Camera::g_screenWidth - 1;
		Nu3D::Camera::g_screenHeight = height;
		Nu3D::Camera::g_scaleFactorX = (Nu3D::Camera::g_screenWidth << 8) / 320;
		Nu3D::Camera::g_viewportLeft = 0;
		Nu3D::Camera::g_viewportTop = 0;
		Nu3D::Camera::g_screenCenterX = Nu3D::Camera::g_screenWidth / 2;
		Nu3D::Camera::g_viewportRightFixed10 = (Nu3D::Camera::g_screenWidth << 10) - 1;
		Nu3D::Camera::g_viewportCenterX = Nu3D::Camera::g_screenWidth / 2;
		Nu3D::Camera::g_viewportCenterY = height / 2;
		Nu3D::Camera::g_viewportHeight = height;
		Nu3D::Camera::g_viewportLeftFixed10 = 0;

		SoftwareRenderer::g_unk504D34 = SoftwareRenderer::g_unk87E50C;
		SoftwareRenderer::g_unk839278 = 1023;

		// omitted: clears a short in the draw buffer of DevDraw, which is not decompiled yet

		SoftwareRenderer::g_unk731CBC = timeGetTime();

		return 1;
	}

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

	// STUB: TOY2 0x0047D4E0
	int32_t* BuildPerspectiveDivideTable(int32_t centerX) { return 0; }

	// FUNCTION: TOY2 0x00499950
	void InitDirect3DRenderer()
	{
		// Weird method, a good portion of these variables are never even used in the game
		Logger::Log("INIT : Starting Direct3D renderer.\n");

		RECT* destRect = DrawingDevice::GetDestRect();

		int32_t screenWidth = destRect->right - destRect->left;
		Nu3D::Camera::g_screenWidth = screenWidth;

		int32_t screenHeight = destRect->bottom - destRect->top;
		Nu3D::Camera::g_screenHeight = screenHeight;

		if (Nu3D::Camera::g_screenCenterX != Nu3D::Camera::g_cachedScreenCenterX)
		{
			Nu3D::Camera::g_cachedScreenCenterX = Nu3D::Camera::g_screenCenterX;
			BuildPerspectiveDivideTable(Nu3D::Camera::g_screenCenterX);

			screenWidth = Nu3D::Camera::g_screenWidth;
			screenHeight = Nu3D::Camera::g_screenHeight;
		}

		Nu3D::Camera::g_viewportWidth = screenWidth;
		Nu3D::Camera::g_viewportRight = screenHeight - 1;
		Nu3D::Camera::g_viewportBottom = screenWidth - 1;
		Nu3D::Camera::g_scaleFactorX = (screenWidth << 8) / 320;
		Nu3D::Camera::g_viewportHeight = screenHeight;
		Nu3D::Camera::g_viewportLeft = 0;
		Nu3D::Camera::g_screenCenterX = screenWidth / 2;
		Nu3D::Camera::g_viewportCenterX = screenWidth / 2;
		Nu3D::Camera::g_viewportCenterY = screenHeight / 2;
		Nu3D::Camera::g_viewportRightFixed10 = (screenWidth << 10) - 1;

		memset(SoftwareRenderer::g_bltQueue, 0, sizeof(SoftwareRenderer::g_bltQueue));

		Nu3D::Camera::g_viewportTop = 0;
		Nu3D::Camera::g_viewportLeftFixed10 = 0;
		SoftwareRenderer::g_bltQueueCount = 0;
	}

	// FUNCTION: TOY2 0x0041E020
	void InterpolatePathPoint(int32_t index, int32_t path, Vector3I* vec)
	{
		int32_t vecId = path / 4096;
		Levels::RecordData* data;
		int32_t zPos;

		vec->x = Levels::g_recordData[index]->data[vecId].x
			+ (path & 0xFFF) * (Levels::g_recordData[index]->data[vecId + 1].x - Levels::g_recordData[index]->data[vecId].x) / 4096;

		vec->y = Levels::g_recordData[index]->data[vecId].y
			+ (path & 0xFFF) * (Levels::g_recordData[index]->data[vecId + 1].y - Levels::g_recordData[index]->data[vecId].y) / 4096;

		data = Levels::g_recordData[index];

		zPos = data->data[path / 4096].z;

		vec->z = zPos + (path & 0xFFF) * (data->data[path / 4096 + 1].z - zPos) / 4096;
	}

	// GLOBAL: TOY2 0x004F73B4
	RGB32 g_type36Color = { 118, 175, 158 };

	// FUNCTION: TOY2 0x00421090
	void InitialiseLevel7()
	{
		Vector3I origin;

		Collectables::Init(AlsToyBarn::g_unk4F2A48, 65);
		Collectables::LoadTokenTable((int32_t*)AlsToyBarn::g_unk4F2A54);
		MoveableObject::InitTable(AlsToyBarn::g_unk4F2A24);
		Collectables::Activate(3, 1);
		Nullsub2(19, 18);
		AlsToyBarn::g_unk52F9E4 = 0;
		AlsToyBarn::g_unk52FA24 = 0;
		AlsToyBarn::g_unk52FA30 = 0;
		AlsToyBarn::g_unk52FA28 = 0;
		Platform::AddFlags(0, 256);
		g_levelActors[6].actorFlags &= 0xFF7E;
		AlsToyBarn::g_unk52FA0C = 0;
		AlsToyBarn::g_unk52FA14 = 0;
		AlsToyBarn::g_unk52FA20 = 0;
		AlsToyBarn::g_unk52FA50 = 0;
		AlsToyBarn::g_unk52FA2C = g_levelActors[0].actorPhase;
		AlsToyBarn::g_unk52F9DC = 0;
		AlsToyBarn::g_unk52F9E0 = 0;
		AlsToyBarn::g_unk52FA18 = 0;
		AlsToyBarn::g_unk52F9C0 = 0;
		Nu3D::Link::SetScaleFromFixedOffsets(30, 4096, 0, 4096);
		AlsToyBarn::g_unk52F9EC = 0;
		Nu3D::Link::SetScaleFromFixedOffsets(3, 4096, 0, 4096);
		AlsToyBarn::g_unk52F9E8 = 0;
		AlsToyBarn::g_unk52F9BC = 0;
		AlsToyBarn::g_unk52FA4C = 0;
		Platform::AddFlags(10, 256);
		Platform::AddFlags(14, 256);
		AlsToyBarn::g_unk52FA08 = 0;
		AlsToyBarn::g_unk52FA1C = 0;
		AlsToyBarn::g_unk52FA10 = 0;
		g_unk52F2F8 = 0;
		g_unk52AD64 = 0;
		AlsToyBarn::g_unk52FA00 = AlsToyBarn::g_unk4F294C;
		AlsToyBarn::g_unk52FA44 = 0;
		AlsToyBarn::g_unk52FA04 = AlsToyBarn::g_unk4F2970;
		AlsToyBarn::g_unk52FA48 = 0;
		AlsToyBarn::g_unk52F9F8 = AlsToyBarn::g_unk4F2994;
		AlsToyBarn::g_unk52FA3C = 0;
		Platform::AddFlags(7, 256);
		Platform::AddFlags(8, 256);
		Platform::AddFlags(9, 256);
		AlsToyBarn::g_unk52F9FC = AlsToyBarn::g_unk4F29B8;
		AlsToyBarn::g_unk52FA40 = 0;
		AlsToyBarn::g_unk52F9F0 = AlsToyBarn::g_unk4F29DC;
		AlsToyBarn::g_unk52FA34 = 0;
		AlsToyBarn::g_unk52F9F4 = AlsToyBarn::g_unk4F2A00;
		AlsToyBarn::g_unk52FA38 = 0;
		Platform::GetOrigin(13, &origin);
		Platform::SetOrigin(13, origin.x + 25280, origin.y, origin.z);
		Nu3D::Link::SetPositionRawAndCommit(12, (origin.x + 25280) >> 5, origin.y >> 5, origin.z >> 5);
		Platform::GetOrigin(12, &origin);
		Platform::SetOrigin(12, origin.x + 25280, origin.y, origin.z);
		Nu3D::Link::SetPositionRawAndCommit(13, (origin.x + 25280) >> 5, origin.y >> 5, origin.z >> 5);
		Platform::GetOrigin(11, &origin);
		Platform::SetOrigin(11, origin.x + 25280, origin.y, origin.z);
		Nu3D::Link::SetPositionRawAndCommit(14, (origin.x + 25280) >> 5, origin.y >> 5, origin.z >> 5);
		MainMenu::g_menuClearColor = g_type36Color;
	}

	// FUNCTION: TOY2 0x00424390
	void InitialiseLevel9()
	{
		Actor::Toy2Actor* actor;

		MoveableObject::InitTable(0);
		Collectables::Init(0, 0);
		BarnEncounter::g_unk52FBB8 = g_levelActors[0].actorPhase;
		BarnEncounter::g_unk52FC0C = 0;
		BarnEncounter::g_unk52FBEC = 0;
		BarnEncounter::g_unk52FC08 = 0;
		BarnEncounter::g_unk52FBCC = -360448;
		BarnEncounter::g_unk52FC04 = 0;
		BarnEncounter::g_unk52FBC0 = 0;
		BarnEncounter::g_unk52FBE8 = 0;
		BarnEncounter::g_unk52FBD4 = 1;
		BarnEncounter::g_unk52FBF0 = 0;
		BarnEncounter::g_unk52FC00 = 200;
		BarnEncounter::g_unk52FBC8 = 7;
		BarnEncounter::g_unk52FBC4 = 7;
		BarnEncounter::g_unk52FBF4 = 0;
		BarnEncounter::g_unk52FBBC = 0;
		BarnEncounter::g_unk52FBD0 = 0;
		BarnEncounter::g_unk52FBF8 = 120;
		BarnEncounter::g_unk52FBFC = 0;
		g_levelActors[0].creatureRam->speedTarget = 0;

		actor = &g_levelActors[7];

		do
		{
			actor->unkShort2 = 10000;
			actor->actorPhase = 0;
			actor->unkVar13_ = 0;

			actor->creatureRam->boundHalfX = g_levelActors[6].creatureRam->boundHalfX;
			actor->creatureRam->boundHalfZ = g_levelActors[6].creatureRam->boundHalfZ;

			++actor;
		} while (actor < &g_levelActors[12]);

		actor = &g_levelActors[0];

		do
		{
			actor->actorFlags |= 0x800;
			++actor;
		} while (actor < &g_levelActors[12]);
	}

	// FUNCTION: TOY2 0x00425B60
	int32_t InitialiseLevel10()
	{
		int16_t* platformIdx;
		int32_t* originY;
		int16_t platform;
		Vector3I origin;

		Levels::g_recordData[9]->data[2].y = -27220;
		Levels::g_recordData[9]->data[5].y = -27220;
		Levels::g_recordData[9]->data[8].y = -27220;
		Nu3D::Link::SetPositionRawAndCommit(110, Levels::g_recordData[14]->data[0].x, Levels::g_recordData[14]->data[0].y, Levels::g_recordData[14]->data[0].z);
		MoveableObject::InitTable(0);
		Collectables::Init(ElevatorHop::g_unk4F3798, 115);
		Collectables::Activate(3, 1);
		Collectables::LoadTokenTable((int32_t*)ElevatorHop::g_unk4F37A4);
		ElevatorHop::g_unk52FC10 = (int32_t)Collectables::g_pickupSlots[2].yPosPtr - 4;
		ElevatorHop::g_unk52FCDC = 0;
		ElevatorHop::g_unk52FC44 = 0;
		Platform::AddFlags(0, 256);
		Platform::AddFlags(1, 256);
		Platform::AddFlags(2, 256);
		Platform::AddFlags(3, 256);
		Platform::AddFlags(6, 256);
		Platform::AddFlags(7, 256);
		Platform::AddFlags(8, 256);
		Platform::AddFlags(9, 256);
		Platform::AddFlags(10, 256);
		Platform::AddFlags(11, 256);
		Platform::AddFlags(12, 256);
		Platform::AddFlags(13, 256);
		Platform::AddFlags(14, 256);
		Platform::AddFlags(15, 256);
		Platform::AddFlags(16, 256);
		Platform::AddFlags(17, 256);

		platform = ElevatorHop::g_unk4F3614[0];

		if (platform != -1)
		{
			platformIdx = ElevatorHop::g_unk4F3614;
			originY = ElevatorHop::g_unk52FCA0;

			do
			{
				Platform::GetOrigin(platform, &origin);
				++platformIdx;
				*originY = origin.y;
				platform = *platformIdx;
				++originY;
			} while (platform != -1);
		}

		platform = ElevatorHop::g_unk4F3628[0];

		if (platform != -1)
		{
			platformIdx = ElevatorHop::g_unk4F3628;
			originY = ElevatorHop::g_unk52FC78;

			do
			{
				Platform::GetOrigin(platform, &origin);
				++platformIdx;
				*originY = origin.y;
				platform = *platformIdx;
				++originY;
			} while (platform != -1);
		}

		ElevatorHop::g_unk52FCC8 = 0;
		ElevatorHop::g_unk52FC1C = 0;
		ElevatorHop::g_unk52FC28 = 0;
		ElevatorHop::g_unk52FC38 = 0;
		ElevatorHop::g_unk52FD00 = 0;
		ElevatorHop::g_unk52FC54 = 0;
		ElevatorHop::g_unk52FC6C = 0;
		ElevatorHop::g_unk52FC34 = 0;
		ElevatorHop::g_unk52FC2C = 0;
		ElevatorHop::g_unk52FC58 = 0;
		ElevatorHop::g_unk52FC64 = 0;
		ElevatorHop::g_unk52FC74 = 0;
		ElevatorHop::g_unk52FC60 = 0;
		ElevatorHop::g_unk52FC70 = 0;
		ElevatorHop::g_unk52FC4C = 0;
		ElevatorHop::g_unk52FCFC = 12288;
		ElevatorHop::g_unk52FC18 = 0x7FFF;
		ElevatorHop::g_unk52FC40 = 400;
		ElevatorHop::g_unk52FC48 = 5;
		ElevatorHop::g_unk52FC20 = 0;
		ElevatorHop::g_unk52FCE0 = 0;
		ElevatorHop::g_unk52FCF8 = g_levelActors[8].actorPhase;
		ElevatorHop::g_unk52FC68 = 0;
		ElevatorHop::g_unk52FCEC = 0;
		ElevatorHop::g_unk52FCD0 = 0;
		ElevatorHop::g_unk52FC14 = 0;
		ElevatorHop::g_unk52FC50 = 0;
		ElevatorHop::g_unk52FC30 = 2;
		ElevatorHop::g_unk52FCD8 = 0;
		ElevatorHop::g_unk52FC24 = 1;
		ElevatorHop::Method2();
		Nu3D::Link::SetScaleFromFixedOffsets(71, 4096, 4096, 4096);
		Nu3D::Link::SetScaleFromFixedOffsets(70, 0, 0, 0);
		Nu3D::Link::SetScaleFromFixedOffsets(73, 4096, 4096, 4096);
		Nu3D::Link::SetScaleFromFixedOffsets(72, 0, 0, 0);
		Nu3D::Link::SetScaleFromFixedOffsets(79, 4096, 4096, 4096);
		Nu3D::Link::SetScaleFromFixedOffsets(78, 0, 0, 0);
		ElevatorHop::g_unk52FCCC = 0;
		ElevatorHop::g_unk52FCE8 = 0;
		ElevatorHop::g_unk52FCD4 = 0;
		ElevatorHop::g_unk52FCE4 = 0;
		ElevatorHop::g_unk52FC3C = 4096;
		ElevatorHop::g_unk52FCF4 = ElevatorHop::g_unk4F3688;
		ElevatorHop::g_unk52FCF0 = ElevatorHop::g_unk4F36DC;
		g_levelActors[8].transform.pos.x += 0x10000;
		return g_levelActors[8].transform.pos.x;
	}

	// FUNCTION: TOY2 0x0042E600
	int32_t InitialiseLevel14()
	{
		Actor::Toy2Actor* actor;
		int32_t i;
		int32_t recordCount;
		Levels::Type63* entry;
		Vector3I pos1;
		Vector3I pos2;

		MoveableObject::InitTable(0);
		Collectables::Init(TarmacTrouble::g_unk4F4C68, 120);
		Collectables::LoadTokenTable((int32_t*)TarmacTrouble::g_unk4F4C40);
		Collectables::Activate(3, 1);
		Nu3D::Link::GetCurrentPosFixed(10, &pos1);
		Nu3D::Link::GetCurrentPosFixed(11, &pos2);
		pos1.x = (pos1.x + pos2.x) >> 1;
		pos1.y = (pos1.y + pos2.y) >> 1;
		pos1.z = (pos1.z + pos2.z) >> 1;
		Nu3D::Link::GetCurrentPosFixed(13, &pos2);
		TarmacTrouble::g_unk52FF60 = (pos2.x + pos1.x) >> 1;
		TarmacTrouble::g_unk52FFB8 = (pos2.x + pos1.x) >> 1;
		TarmacTrouble::g_unk52FF64 = (pos2.y + pos1.y) >> 1;
		TarmacTrouble::g_unk52FFBC = (pos2.y + pos1.y) >> 1;
		TarmacTrouble::g_unk52FF80.z = 0;
		TarmacTrouble::g_unk52FF68 = (pos2.z + pos1.z) >> 1;
		TarmacTrouble::g_unk52FFC0 = (pos2.z + pos1.z) >> 1;
		TarmacTrouble::g_unk52FF80.y = 0;
		TarmacTrouble::g_unk52FF80.x = 0;
		Platform::AddFlags(0, 256);
		TarmacTrouble::Method2();
		TarmacTrouble::g_unk52FFB4 = 0;
		TarmacTrouble::g_unk52FFEC = 0;
		TarmacTrouble::g_unk52FF7C = 0;
		TarmacTrouble::g_unk52FFD4 = 0;

		for (i = 0; i < 64; ++i)
		{
			if (g_levelActors[i].creatureId)
				g_levelActors[i].visibilityDistance = 1800;
		}

		TarmacTrouble::g_unk52FF74 = g_levelActors[46].actorPhase;
		TarmacTrouble::g_unk52FFAC = 0;
		TarmacTrouble::g_unk52FF78 = 0;
		TarmacTrouble::g_unk52FF4C = 0;
		TarmacTrouble::g_unk52FFE8 = 0;
		TarmacTrouble::g_unk52FFCC = -1;
		i = 0;
		recordCount = Levels::g_recordData[63]->recordCount;

		if (recordCount > 0)
		{
			entry = (Levels::Type63*)Levels::g_recordData[63]->data;

			while (entry->props.linkId != 116)
			{
				++entry;
				++i;

				if (i >= recordCount)
					goto LABEL_11;
			}

			TarmacTrouble::g_unk52FFCC = i;
		}
	LABEL_11:
		TarmacTrouble::g_unk52FFA8 = 200;
		Weather::Init();

		// $TODO: the original returns whatever Weather::Init leaves in eax
		return 0;
	}

	// FUNCTION: TOY2 0x00433ED0
	void ResetBuzzState()
	{
		Buzz::g_unk53C660 = 0;

		Buzz::g_unk53C838 = 0;
		Buzz::g_unk53C668 = 0;
		Buzz::g_unk53C5E4 = 0;
		Buzz::g_unk53C618 = 0;
		Buzz::g_buzzSlipTimer = 0;
		Buzz::g_unk53C83C = 0;
		Buzz::g_unk53C650 = 0;
		Buzz::g_groundSlamTicker = 0;
		Buzz::g_fireChargeTimer = 0;
		Buzz::g_fireHoldTimer = 0;
		Buzz::g_curMoveableObject = 0;
		Buzz::g_unk53C66C = 0;
		Buzz::g_unk53C608 = 0;

		Buzz::DeactivateRocketBoots();
		Buzz::CancelGrapple();
		Buzz::ResetGravityBoots();

		g_buzzActor.actorFlags &= 0xFFBB;
	}

	// FUNCTION: TOY2 0x0043E5A0
	void InitZoneData()
	{
		int32_t entryCount = 20;
		int32_t i;

		for (i = 0; i < 20; i++)
		{
			Levels::g_datContents[i].unkByte1 = 0;
			Levels::g_datContents[i].unkByte2 = 0;
			Levels::g_datContents[i].unkShort8 = 0;
			Levels::g_datContents[i].unkShort7 = 0;
		}

		int32_t category = -1;
		int32_t highestCategory;

		// every category of the first instance list gets the size in bytes of its entries (unkShort7)
		if (Levels::g_instanceSection->flags)
		{
			Levels::InstanceSection* section = Levels::g_instanceSection;
			int8_t nextFlags;

			do
			{
				if ((uint8_t)section->category != category)
				{
					if (category != -1)
						Levels::g_datContents[category].unkShort7 = 20 * entryCount;

					entryCount = 0;
					category = (uint8_t)section->category;
				}

				nextFlags = section[1].flags;
				section++;

				++entryCount;
			} while (nextFlags);

			if (category == -1)
			{
				highestCategory = Levels::g_unk5574FC;
			}
			else
			{
				highestCategory = category;
				Levels::g_unk5574FC = category;
				Levels::g_datContents[category].unkShort7 = 20 * entryCount;
			}
		}
		else
		{
			highestCategory = Levels::g_unk5574FC;
		}

		category = -1;

		// and the second list the same way (unkShort8)
		if (Levels::g_secondInstanceSection->flags)
		{
			Levels::InstanceSection* section = Levels::g_secondInstanceSection;
			int8_t nextFlags;

			do
			{
				if ((uint8_t)section->category != category)
				{
					if (category != -1)
						Levels::g_datContents[category].unkShort8 = 20 * entryCount;

					entryCount = 0;
					category = (uint8_t)section->category;
				}

				nextFlags = section[1].flags;
				section++;

				++entryCount;
			} while (nextFlags);

			if (category != -1)
				Levels::g_datContents[category].unkShort8 = 20 * entryCount;
		}

		if (highestCategory < category)
			highestCategory = category;

		int32_t totalEntries = 0;

		for (i = 0; i < 20; i++)
		{
			uint32_t firstCount = (uint32_t)(int32_t)(int16_t)Levels::g_datContents[i].unkShort7 / 20;
			uint32_t secondCount = (uint32_t)(int32_t)(int16_t)Levels::g_datContents[i].unkShort8 / 20;

			totalEntries = totalEntries + secondCount + firstCount;
		}

		Levels::g_unk547ED8 = totalEntries;
		Levels::g_unk5574FC = highestCategory + 1;
	}

	// FUNCTION: TOY2 0x0044F840
	void ShowModelNode(int32_t creatureIndex, int32_t nodeIdx) { Nu3D::Creature::SetNodeVisibleByIndex(creatureIndex, nodeIdx, 1); }

	// FUNCTION: TOY2 0x0044F860
	void HideModelNode(int32_t creatureIndex, int32_t nodeIdx) { Nu3D::Creature::SetNodeVisibleByIndex(creatureIndex, nodeIdx, 0); }

	// FUNCTION: TOY2 0x00498550
	void ProcessMiscEvents()
	{
		D3DApp::ProcessWndEvents();

		if (D3DApp::g_windowData.wndIsExiting)
		{
			DrawingDevice::g_drawingDevice->RestoreToGDISurface(1);
			Logger::GetErrorHandler("C:\projects\toy2\toy2.cpp", 4670)("");
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
	}

	// one control of the configuration screen, the list ends with an id of 0
	struct GameControl
	{
		int32_t id;
		char* name;
	};

	// GLOBAL: TOY2 0x00500E58
	GameControl g_gameControls[14] = {
		{ 16, "up" },
		{ 64, "down" },
		{ 128, "left" },
		{ 32, "right" },
		{ 16384, "jump" },
		{ 32768, "fire" },
		{ 8192, "spin" },
		{ 256, "camera left" },
		{ 512, "camera right" },
		{ 1024, "visor toggle" },
		{ 2048, "target lock" },
		{ 8, "menu" },
		{ 4096, "cancel" },
		{ 0, 0 },
	};

	// GLOBAL: TOY2 0x00830C6C
	int32_t g_controlSettingsScrollIndex;

	// GLOBAL: TOY2 0x00830C70
	int32_t g_controlListTop;

	// GLOBAL: TOY2 0x00830C74
	int32_t g_controlEditing;

	// GLOBAL: TOY2 0x00500ECC
	int32_t g_controlBlink = 20;

	// GLOBAL: TOY2 0x00500EC8
	int32_t g_controlDelay = 30;

	// FUNCTION: TOY2 0x0049C420
	int32_t ShowControlConfig()
	{
		int32_t listTop;
		int32_t rowIndex;
		GameControl* control;
		int32_t y;
		int32_t controlName;
		char* controlNameStr;
		int32_t id;
		int32_t nextId;
		int32_t pressedInput;
		int32_t scrollIdx;
		int32_t result = 0;
		int32_t moreBelow = 0;

		g_inputSuppressFrames = 30 / Renderer::g_frameDelta;

		if (SaveManager::GetControlSettingId(TOY_INPUT_ESC) != -1)
		{
			SaveManager::ClearBindByInputCode(TOY_INPUT_ESC);
			SaveManager::ClearBindByInputCode(TOY_INPUT_CAPITALA);
			SaveManager::ClearBindByInputCode(TOY_INPUT_CAPITALB);
			SaveManager::ClearBindByInputCode(TOY_INPUT_CAPITALC);
			SaveManager::ClearBindByInputCode(TOY_INPUT_CAPITALD);
			SaveManager::ClearBindByInputCode(TOY_INPUT_F1);
			SaveManager::ClearBindByInputCode(TOY_INPUT_F2);
		}

		Renderer::DrawMenuTextScaled(160, 46, "control configuration", 0, 1, 2048);

		listTop = g_controlListTop;
		rowIndex = g_controlListTop;

		if (g_gameControls[g_controlListTop].id)
		{
			control = &g_gameControls[g_controlListTop];

			do
			{
				y = 16 * (rowIndex - listTop + 5);

				if (y > 144)
				{
					moreBelow = 1;
					break;
				}

				controlName = SaveManager::GetInputCodeByControlId((GameControlId)control->id);
				controlNameStr = InputManager::GetGameControlName((ToyInputCodes)controlName);

				if (rowIndex != g_controlSettingsScrollIndex || g_controlBlink > 10 || g_controlEditing)
					Renderer::DrawMenuTextScaled(150, y, control->name, 0, 2, 2048);

				if (rowIndex != g_controlSettingsScrollIndex || g_controlBlink > 10 || ! g_controlEditing)
				{
					if (controlNameStr)
					{
						Renderer::DrawMenuTextScaled(160, y, controlNameStr, 0, 0, 2048);
					}
					else
					{
						id = control->id;

						if (control->id > INPUT_DOWN)
						{
							switch (id)
							{
								case INPUT_LEFT:
									Renderer::DrawMenuTextScaled(160, y, "C", 0, 0, 2048);
									goto LABEL_31;
								case INPUT_CANCEL:
									controlNameStr = InputManager::GetGameControlName(TOY_INPUT_ESC);
									Renderer::DrawMenuTextScaled(160, y, controlNameStr, 0, 0, 2048);
									goto LABEL_31;
								case INPUT_JUMP:
									controlNameStr = InputManager::GetGameControlName(TOY_INPUT_F2);
									Renderer::DrawMenuTextScaled(160, y, controlNameStr, 0, 0, 2048);
									goto LABEL_31;
							}
						LABEL_27:
							Renderer::DrawMenuTextScaled(160, y, "???", 0, 0, 2048);
							goto LABEL_31;
						}

						if (control->id == INPUT_DOWN)
						{
							Renderer::DrawMenuTextScaled(160, y, "B", 0, 0, 2048);
						}
						else
						{
							switch (id)
							{
								case INPUT_MENU:
									controlNameStr = InputManager::GetGameControlName(TOY_INPUT_F1);
									Renderer::DrawMenuTextScaled(160, y, controlNameStr, 0, 0, 2048);
									break;
								case INPUT_UP:
									Renderer::DrawMenuTextScaled(160, y, "A", 0, 0, 2048);
									break;
								case INPUT_RIGHT:
									Renderer::DrawMenuTextScaled(160, y, "D", 0, 0, 2048);
									break;
								default:
									goto LABEL_27;
							}
						}
					}
				}
			LABEL_31:

				nextId = control[1].id;
				++control;
				listTop = g_controlListTop;
				rowIndex = rowIndex + 1;
			} while (nextId != 0);
		}

		if (g_controlBlink > 10)
		{
			if (listTop)
				Renderer::Sprite::DrawScaledFixed(152, 64, 67, 45, 255, 255, 255, 255, 4096, 4096);

			if (moreBelow)
				Renderer::Sprite::DrawScaledFixed(152, 150, 67, 46, 255, 255, 255, 255, 4096, 4096);
		}

		if (g_controlEditing)
		{
			Renderer::DrawMenuTextScaled(160, 182, "press desired key / button", 0, 1, 1536);
		}
		else
		{
			Renderer::DrawMenuTextScaled(160, 178, "ABto select, space to change", 0, 1, 1536);
			Renderer::DrawMenuTextScaled(160, 186, "return to accept, esc to cancel", 0, 1, 1536);
		}

		if (g_controlDelay <= 0)
		{
			if (g_controlEditing)
			{
				pressedInput = InputManager::GetPressedInput();

				if (pressedInput != TOY_INPUT_UNKNOWN && pressedInput != TOY_INPUT_DIRECTIONPAD)
				{
					if (pressedInput > TOY_INPUT_CAPITALA)
					{
						if (pressedInput > TOY_INPUT_CAPITALB
							|| (pressedInput != TOY_INPUT_CAPITALB && pressedInput != TOY_INPUT_CAPITALC && pressedInput != TOY_INPUT_CAPITALD))
						{
						LABEL_75:
							SaveManager::ClearBindByControlId((GameControlId)g_gameControls[g_controlSettingsScrollIndex].id);
							SaveManager::ClearBindByInputCode((ToyInputCodes)pressedInput);
							g_controlDelay = 30;

							SaveManager::AddInputEntry((ToyInputCodes)pressedInput, (GameControlId)g_gameControls[g_controlSettingsScrollIndex].id);
							g_controlEditing = 0;
							AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);
						}
					}
					else if (pressedInput != TOY_INPUT_CAPITALA)
					{
						switch (pressedInput)
						{
							case TOY_INPUT_ESC:
								g_controlEditing = 0;
								InputManager::IsKeyPressed(DIK_ESCAPE);
								AudioManager::PlayOneShotSoundGlobal(2, 4608, 80, 80);
								break;
							case TOY_INPUT_F1:
							case TOY_INPUT_F2:
							case TOY_INPUT_F3:
							case TOY_INPUT_F4:
							case TOY_INPUT_F5:
							case TOY_INPUT_F6:
							case TOY_INPUT_F7:
							case TOY_INPUT_F8:
							case TOY_INPUT_F9:
							case TOY_INPUT_F10:
							case TOY_INPUT_F11:
							case TOY_INPUT_F12:
							case TOY_INPUT_F13:
							case TOY_INPUT_F14:
							case TOY_INPUT_F15:
								break;
							default:
								goto LABEL_75;
						}
					}
				}
			}
			else
			{
				if (InputManager::IsKeyPressed(DIK_UP))
				{
					if (g_controlSettingsScrollIndex)
					{
						AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
						--g_controlSettingsScrollIndex;
					}

					if (g_controlListTop && g_controlListTop >= g_controlSettingsScrollIndex)
						--g_controlListTop;
				}

				if (InputManager::IsKeyPressed(DIK_DOWN))
				{
					scrollIdx = g_controlSettingsScrollIndex;

					if ((g_gameControls[rowIndex].id && g_controlSettingsScrollIndex < rowIndex) || g_controlSettingsScrollIndex < rowIndex - 1)
					{
						AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
						scrollIdx = ++g_controlSettingsScrollIndex;
					}

					if (moreBelow && scrollIdx >= rowIndex)
						++g_controlListTop;
				}

				if (InputManager::IsKeyPressed(DIK_ESCAPE))
				{
					result = 1;
					g_controlEditing = 0;
					g_controlListTop = 0;
					g_controlSettingsScrollIndex = 0;
					g_controlDelay = 30;
					InputManager::g_curButtonsPressed = 0;
					AudioManager::PlayOneShotSoundGlobal(2, 4608, 80, 80);
				}

				if (InputManager::IsKeyPressed(DIK_RETURN))
				{
					result = 2;
					g_controlEditing = 0;
					g_controlListTop = 0;
					g_controlSettingsScrollIndex = 0;
					g_controlDelay = 30;
					SaveManager::AddInputEntry(TOY_INPUT_ESC, INPUT_CANCEL);
					SaveManager::AddInputEntry(TOY_INPUT_CAPITALA, INPUT_UP);
					SaveManager::AddInputEntry(TOY_INPUT_CAPITALB, INPUT_DOWN);
					SaveManager::AddInputEntry(TOY_INPUT_CAPITALC, INPUT_LEFT);
					SaveManager::AddInputEntry(TOY_INPUT_CAPITALD, INPUT_RIGHT);
					SaveManager::AddInputEntry(TOY_INPUT_F1, INPUT_JUMP);
					SaveManager::AddInputEntry(TOY_INPUT_F2, INPUT_MENU);
					SaveManager::SaveToFile(99, 0);
					AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);
				}

				if (InputManager::IsKeyPressed(DIK_SPACE))
				{
					g_controlEditing = 1;
					g_controlDelay = 30;
					AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);
				}
			}
		}
		else
		{
			g_controlDelay -= Renderer::g_frameDelta;
		}

		g_controlBlink = g_controlBlink - Renderer::g_frameDelta + ((g_controlBlink - (int32_t)Renderer::g_frameDelta) < 0 ? 20 : 0);

		return result;
	}

	namespace GraphicsConfig
	{
		// GLOBAL: TOY2 0x00830C78
		int32_t g_scrollerIdx;

		// GLOBAL: TOY2 0x00500ED0
		int32_t g_flashState = 20;

		// GLOBAL: TOY2 0x00830C7C
		int32_t g_scrollDelay;

		// GLOBAL: TOY2 0x00500ED4
		int32_t g_arrowYPos = 72;
	}

	// GLOBAL: TOY2 0x005009F4
	char* g_detailOptionsStr[3] = { "low detail", "medium detail", "high detail" };

	// GLOBAL: TOY2 0x00500A00
	char* g_gammaCorrectionOptionsStr[3] = { "gamma correction normal", "gamma correction medium", "gamma correction high" };

	// FUNCTION: TOY2 0x0049CAA0
	void ShowGraphicsConfig()
	{
		int32_t showLeftArrow;
		int32_t scrollIdx1;
		int32_t scrollerWrap;
		int32_t scrollIdx2;
		int32_t frameDelta;
		int32_t yPosAdd;
		int32_t yPosDiv;
		int32_t check;
		int16_t arrowYPos;
		uint8_t curButtons;
		int32_t showRightArrow;

		showLeftArrow = 0;

		Renderer::DrawMenuText(50, "graphics configuration", 0);

		if (! GraphicsConfig::g_scrollerIdx && GraphicsConfig::g_flashState >= 10)
			goto LBL_DRAW_DETAIL;

		if ((g_toyCfgData.flags & 1) != 0)
			Renderer::DrawMenuText(75, "lens flare on", 0);
		else
			Renderer::DrawMenuText(75, "lens flare off", 0);

		if (GraphicsConfig::g_scrollerIdx != 1 || GraphicsConfig::g_flashState < 10)
		{
		LBL_DRAW_DETAIL:
			Renderer::DrawMenuText(100, g_detailOptionsStr[g_toyCfgData.detail], 0);

			if (GraphicsConfig::g_scrollerIdx == 2 && GraphicsConfig::g_flashState >= 10)
				goto LBL_DRAW_ANIMTEX;
		}

		Renderer::DrawMenuText(125, g_gammaCorrectionOptionsStr[(int32_t)(-((g_toyCfgData.gammaCorrection - 2.0) * -2.0))], 0);

		if (GraphicsConfig::g_scrollerIdx != 3 || GraphicsConfig::g_flashState < 10)
		{
		LBL_DRAW_ANIMTEX:
			if ((g_toyCfgData.flags & 4) != 0)
				Renderer::DrawMenuText(150, "animated textures on", 0);
			else
				Renderer::DrawMenuText(150, "animated textures off", 0);
		}

		Renderer::DrawMenuTextScaled(160, 178, "AB to select, CD to change", 0, 1, 1536);
		Renderer::DrawMenuTextScaled(160, 186, "jump:accept  cancel:go back", 0, 1, 1536);

		if (GraphicsConfig::g_scrollDelay <= 0)
		{
			if ((InputManager::g_curButtonsPressed & INPUT_UP) != 0)
			{
				GraphicsConfig::g_scrollDelay = 15;
				GraphicsConfig::g_scrollerIdx = (GraphicsConfig::g_scrollerIdx - 1) & ((GraphicsConfig::g_scrollerIdx - 1 < 0) - 1);
				AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);
			}

			if ((InputManager::g_curButtonsPressed & INPUT_DOWN) != 0)
			{
				scrollIdx1 = GraphicsConfig::g_scrollerIdx;
				GraphicsConfig::g_scrollerIdx = 3;
				scrollerWrap = scrollIdx1 + 1;

				if (scrollerWrap <= 3)
					GraphicsConfig::g_scrollerIdx = scrollerWrap;

				GraphicsConfig::g_scrollDelay = 15;
				AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);
			}
		}

		scrollIdx2 = GraphicsConfig::g_scrollerIdx;
		showRightArrow = 0;

		switch (GraphicsConfig::g_scrollerIdx)
		{
			case 0:
			case 3:
				showRightArrow = 1;
				showLeftArrow = 1;
				break;
			case 1:
				if (g_toyCfgData.detail)
					showLeftArrow = 1;

				if (g_toyCfgData.detail < 2)
					showRightArrow = 1;

				break;
			case 2:
				if (g_toyCfgData.gammaCorrection > 2.0)
					showLeftArrow = 1;

				if (g_toyCfgData.gammaCorrection < 3.0)
					showRightArrow = 1;

				break;
			default:
				break;
		}

		frameDelta = Renderer::g_frameDelta;
		yPosAdd = 25 * GraphicsConfig::g_scrollerIdx - GraphicsConfig::g_arrowYPos + 72;

		if (yPosAdd <= 0)
		{
			if (yPosAdd < 0)
			{
				yPosDiv = yPosAdd * (int32_t)Renderer::g_frameDelta / 5;
				check = yPosDiv <= -1 ? yPosAdd * (int32_t)Renderer::g_frameDelta / 5 : -1;

				if (yPosAdd <= check)
				{
					if (yPosDiv > -1)
					{
						yPosAdd = -1;
						goto LBL_AFTER_CALC;
					}
					goto LBL_SET_YPOS;
				}
			}
		}
		else
		{
			yPosDiv = yPosAdd * (int32_t)Renderer::g_frameDelta / 5;
			check = 1;

			if (yPosDiv >= 1)
				check = yPosAdd * (int32_t)Renderer::g_frameDelta / 5;

			if (yPosAdd >= check)
			{
				if (yPosDiv < 1)
				{
					yPosAdd = 1;
					goto LBL_AFTER_CALC;
				}
			LBL_SET_YPOS:

				yPosAdd = yPosDiv;
			}
		}
	LBL_AFTER_CALC:

		arrowYPos = yPosAdd + GraphicsConfig::g_arrowYPos;
		GraphicsConfig::g_arrowYPos += yPosAdd;

		if (showLeftArrow) // Left Arrow
		{
			Renderer::Sprite::DrawScaledFixed(50, arrowYPos, 56, 3, 255, 255, 255, 255, 2048, 2048);

			scrollIdx2 = GraphicsConfig::g_scrollerIdx;
			arrowYPos = GraphicsConfig::g_arrowYPos;
			frameDelta = Renderer::g_frameDelta;
		}

		if (showRightArrow) // Right Arrow
		{
			Renderer::Sprite::DrawScaledFixed(255, arrowYPos, 56, 2, 255, 255, 255, 255, 2048, 2048);

			scrollIdx2 = GraphicsConfig::g_scrollerIdx;
			frameDelta = Renderer::g_frameDelta;
		}

		GraphicsConfig::g_flashState = GraphicsConfig::g_flashState - frameDelta + (GraphicsConfig::g_flashState - frameDelta < 0 ? 20 : 0);

		if (GraphicsConfig::g_scrollDelay <= 0)
		{
			curButtons = InputManager::g_curButtonsPressed;

			if ((InputManager::g_curButtonsPressed & (INPUT_RIGHT | INPUT_LEFT)) != 0)
			{
				if ((InputManager::g_curButtonsPressed & INPUT_LEFT) != 0 && showLeftArrow)
				{
					AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
					scrollIdx2 = GraphicsConfig::g_scrollerIdx;
				}

				curButtons = InputManager::g_curButtonsPressed;
				GraphicsConfig::g_scrollDelay = 15;
			}

			switch (scrollIdx2)
			{
				case 0:
					if ((curButtons & (INPUT_RIGHT | INPUT_LEFT)) != 0)
						g_toyCfgData.flags ^= ((uint8_t)g_toyCfgData.flags ^ (-1 - (g_toyCfgData.flags & 1))) & 1;

					break;
				case 1:
					if (showLeftArrow && (int8_t)curButtons < 0)
						--g_toyCfgData.detail;

					if (showRightArrow && (curButtons & INPUT_RIGHT) != 0)
						++g_toyCfgData.detail;
					break;
				case 2:
					if (showLeftArrow && (int8_t)curButtons < 0)
						g_toyCfgData.gammaCorrection = g_toyCfgData.gammaCorrection - 0.5;

					if (showRightArrow && (curButtons & INPUT_RIGHT) != 0)
						g_toyCfgData.gammaCorrection = g_toyCfgData.gammaCorrection + 0.5;
					break;
				case 3:
					if ((curButtons & (INPUT_RIGHT | INPUT_LEFT)) != 0)
						g_toyCfgData.flags ^= ((uint8_t)g_toyCfgData.flags ^ (4 * -(((g_toyCfgData.flags >> 2) & 1) + 1))) & 4;
					break;
				default:
					return;
			}
		}
		else
		{
			GraphicsConfig::g_scrollDelay -= frameDelta;
		}
	}

	// FUNCTION: TOY2 0x0049EAC0
	void PlayLevelMusic()
	{
		if (g_hudWidgetTimers[7])
		{
			if (AudioManager::g_curLoopingTrack != 15)
				AudioManager::PlayMusicLooping(15);
		}
		else if (g_hudWidgetTimers[6] && g_unk52F2F8 > 1)
		{
			if (AudioManager::g_curLoopingTrack != 16)
				AudioManager::PlayMusicLooping(16);
		}
		else if (AudioManager::g_curLoopingTrack != g_levelIndex)
		{
			AudioManager::PlayMusicLooping(g_levelIndex);
		}
	}

	// FUNCTION: TOY2 0x0049F490
	void UnusedVarUpdate() { g_unused3 = (g_unused3 + 1) & 0xF; }

	// FUNCTION: TOY2 0x0049FC60
	int32_t TickSlide(int32_t widgetIndex)
	{
		if (! g_isPaused)
		{
			int16_t timer = g_hudWidgetTimers[widgetIndex];

			if (! timer || (Camera::g_inCutscene & 1) != 0)
			{
				int16_t position = g_hudWidgetPositions[widgetIndex];

				if (position > 0)
				{
					int16_t newPosition = position - 32 * Renderer::g_frameDelta;
					g_hudWidgetPositions[widgetIndex] = newPosition;

					if (newPosition <= 0)
						g_hudWidgetPositions[widgetIndex] = 0;
				}
			}
			else
			{
				int16_t position = g_hudWidgetPositions[widgetIndex];
				int16_t frameDelta = Renderer::g_frameDelta;

				if (position < 1024)
				{
					int16_t newPosition = 32 * Renderer::g_frameDelta + position;
					g_hudWidgetPositions[widgetIndex] = newPosition;

					if (newPosition >= 1024)
						g_hudWidgetPositions[widgetIndex] = 1024;
				}

				if (timer < 1000)
				{
					int16_t newTimer = timer - frameDelta;
					g_hudWidgetTimers[widgetIndex] = newTimer;

					if (newTimer <= 0)
						g_hudWidgetTimers[widgetIndex] = 0;
				}
			}
		}

		int16_t position = g_hudWidgetPositions[widgetIndex];

		if (position)
			return (Numerics::g_sinCosLUT[position] >> 8) - 64;
		else
			return 1;
	}

	// FUNCTION: TOY2 0x0049FD40
	void RenderHUD()
	{
		int32_t tick64Acc;
		int32_t pulse;
		int32_t colors[12];
		int32_t tokenColor;
		int32_t digits;
		int32_t digitsLow;
		int32_t health;
		int32_t spinGreen;
		int32_t slotIcon;
		int32_t flicker;

		if ((InputManager::g_directionInputState & DIR_TARGET_LOCK) != 0)
		{
			g_hudWidgetTimers[0] = 90;
			g_hudWidgetTimers[1] = 90;
			g_hudWidgetTimers[2] = 90;
		}

		g_hudWidgetTimers[10] = 5;

		if (Buzz::g_unk53C83C)
			g_hudWidgetTimers[3] = 120;

		if (Buzz::g_fireChargeTimer || Buzz::g_greenGunCharge || g_grappleTimer || g_discLauncherTimer)
			g_hudWidgetTimers[4] = 60;

		if (g_collectionCount > 0)
			g_hudWidgetTimers[5] = 5;

		if (g_unk52F2F8 > 0 && g_unk52F2F8 < 3)
			g_hudWidgetTimers[6] = 5;

		if (Gadget::g_unk830D48 < 0)
			g_hudWidgetTimers[8] = 5;

		if (g_buzzActor.coinsCollected >= 50 && ! Gadget::g_unk830D1C)
			g_hudWidgetTimers[9] = 5;

		if (g_unk830CC4 == 9 && ! g_isPaused)
		{
			// the token that was just collected
			Renderer::DrawString(94, "you have collected a token!", 0x80, 0x80, 0, 0);

			tick64Acc = g_tickAccumulators.tick64Acc;

			if (tick64Acc > 0x1F)
				tick64Acc = 63 - tick64Acc;

			pulse = 2 * tick64Acc + 64;

			if ((InputManager::g_curButtonsPressed & 0x40) != 0 && (InputManager::g_prevButtonsPressed & 0x40) == 0 && ! g_unk830C94)
			{
				g_unk830C94 = 1;
				AudioManager::PlaySoundEffect(63, 0);
			}

			if ((InputManager::g_curButtonsPressed & 0x10) != 0 && (InputManager::g_prevButtonsPressed & 0x10) == 0 && g_unk830C94 == 1)
			{
				g_unk830C94 = 0;
				AudioManager::PlaySoundEffect(63, 0);
			}

			if (g_unk830C94)
			{
				Renderer::DrawString(106, "keep on playing", 0x80, 0x80, 0x80, 0);
				Renderer::DrawString(114, "exit level?", pulse, pulse, pulse, 0);
			}
			else
			{
				Renderer::DrawString(106, "keep on playing", pulse, pulse, pulse, 0);
				Renderer::DrawString(114, "exit level?", 0x80, 0x80, 0x80, 0);
			}

			Renderer::DrawString(124, "jump to select", 0x80, 0x80, 0x80, 0);
			Renderer::DrawBlackBorderBox(77, 91, 0x166000, 0x2D000, 0, 0, 0x80);

			if ((InputManager::g_curButtonsPressed & INPUT_JUMP) != 0 && (InputManager::g_prevButtonsPressed & INPUT_JUMP) == 0)
			{
				AudioManager::PlaySoundEffect(62, 0);

				if (g_unk830C94 == 1)
				{
					g_unk830CC4 = 3;
					g_levelTransition = 1;
					g_levelTransitionTimer = 1000;
					AudioManager::PlaySoundEffect(169, 0);
				}
				else
				{
					Camera::g_inCutscene &= 0xFE;
					g_buzzActor.actorFlags &= ~4;
					g_unk830CC4 = 0;
					Camera::g_cutsceneTimer = 0;
					Camera::g_cameraBlendTimer = 64;
					g_buzzActor.unkWord15 = 5;
				}
			}
		}

		if (g_isPaused && g_devCamState)
		{
			g_pauseMenuBlinkTimer &= 0x3F;
			int32_t pulseColor = g_pauseMenuBlinkTimer;

			if (g_pauseMenuBlinkTimer > 31)
				pulseColor = 63 - g_pauseMenuBlinkTimer;

			pulseColor = 2 * pulseColor + 64;

			// the colour of the four menu entries: elements 8 to 11 are the red channel, 4 to 7 the green channel and 0 to 3 the blue channel
			colors[4] = 128;
			colors[5] = 128;
			memset(colors, 0, 16);
			colors[6] = 128;
			colors[7] = 128;
			colors[8] = 128;
			colors[9] = 128;
			colors[10] = 128;
			colors[11] = 128;

			if (InputManager::g_directionalInputCount == 3 && pulseColor > 96)
				Renderer::DrawString(200, "insert controller", 0x80, 0x80, 0, 0);

			int32_t selection = g_gameMenuScrollIdx;
			colors[selection + 8] = pulseColor;
			colors[selection + 4] = pulseColor;
			colors[selection] = 0;

			if (g_pauseMenuType == MENU_MAIN)
			{
				Renderer::DrawString(84, "pause menu", 0x80, 0x80, 0, 0);
				Renderer::DrawString(96, "continue", (uint8_t)colors[8], (uint8_t)colors[4], (uint8_t)colors[0], 0);
				Renderer::DrawString(104, "camera mode", (uint8_t)colors[9], (uint8_t)colors[5], (uint8_t)colors[1], 0);
				Renderer::DrawString(112, "volume control", (uint8_t)colors[10], (uint8_t)colors[6], (uint8_t)colors[2], 0);
				Renderer::DrawString(120, "exit level", (uint8_t)colors[11], (uint8_t)colors[7], (uint8_t)colors[3], 0);
			}

			if (g_pauseMenuType == MENU_CAMERA)
			{
				Renderer::DrawString(84, "camera mode", 128, 0x80, 0, 0);
				Renderer::DrawString(100, "passive camera", (uint8_t)colors[8], (uint8_t)colors[4], (uint8_t)colors[0], 0);
				Renderer::DrawString(108, "active camera", (uint8_t)colors[9], (uint8_t)colors[5], (uint8_t)colors[1], 0);
			}

			if (g_pauseMenuType == MENU_VOLUME_CONTROL)
			{
				Renderer::DrawString(84, "volume control", 0x80, 0x80, 0, 0);
				Renderer::DrawString(100, g_sfxVolumeStr, (uint8_t)colors[8], (uint8_t)colors[4], (uint8_t)colors[0], 0);
				Renderer::DrawString(108, g_bgmVolumeStr, (uint8_t)colors[9], (uint8_t)colors[5], (uint8_t)colors[1], 0);
			}

			if (g_pauseMenuType == MENU_AREYOUSURE)
			{
				Renderer::DrawString(84, "are you sure?", 0x80, 0x80, 0, 0);
				Renderer::DrawString(100, "no", (uint8_t)colors[8], (uint8_t)colors[4], (uint8_t)colors[0], 0);
				Renderer::DrawString(108, "yes", (uint8_t)colors[9], (uint8_t)colors[5], (uint8_t)colors[1], 0);
			}

			if (g_pauseMenuType == MENU_SHORTENEDPAUSE)
			{
				Renderer::DrawString(84, "pause menu", 0x80, 0x80, 0, 0);
				Renderer::DrawString(100, "continue", (uint8_t)colors[8], (uint8_t)colors[4], (uint8_t)colors[0], 0);
				Renderer::DrawString(108, "exit game", (uint8_t)colors[9], (uint8_t)colors[5], (uint8_t)colors[1], 0);
			}

			Renderer::DrawString(134, "jump to select", 0x80, 0x80, 0x80, 0);
			Renderer::DrawBlackBorderBox(0xA0, 0x51, 0xC0000, 0x41000, 128, 0, 0x80);
		}

		// lives
		int32_t slide0 = TickSlide(0);
		int16_t slide0Short = slide0;

		if (slide0 <= 0)
		{
			Renderer::Sprite::DrawScaledFixed(16, slide0 + 16, 10, 0, 0x80, 0x80, 0x80, 255, 2048, 2048);
			Renderer::Sprite::DrawScaled(60, slide0Short + 32, 20, g_buzzActor.lives + 26, 0xFF, 0xFF, 0xFF, 96, 4096, 4096);
		}

		// the token pieces
		int32_t slide6 = TickSlide(6);

		if (slide6 <= 0)
		{
			int32_t offsetX = 0;
			int32_t offsetY = 0;

			if (g_unk52AD64 < 100)
			{
				Renderer::Sprite::DrawTile(250, slide6 + 24, 3, 3 - g_unk52AD64);

				if (g_unk52AD64 == 2)
				{
					tick64Acc = g_tickAccumulators.tick64Acc;

					if (tick64Acc > 31)
						tick64Acc = 63 - tick64Acc;

					tokenColor = 4 * tick64Acc + 20;
				}
				else
				{
					tokenColor = 0x80;
				}

				goto LABEL_78;
			}

			if (! g_isPaused)
				AudioManager::PlaySoundEffect(94, 0);

			if ((int16_t)g_unk830D4C < 0)
			{
			LABEL_71:
				digits = g_unk52AD64 - 100;
				digitsLow = (g_unk52AD64 - 100) / 10;
				Renderer::Sprite::DrawTile(offsetY + 243, slide6 + 24, 3, (g_unk52AD64 - 100) / 10);
				Renderer::Sprite::DrawTile(offsetY + 257, slide6 + 24, 3, digits - 10 * digitsLow);
				tokenColor = 0x80;

			LABEL_78:
				Renderer::Sprite::DrawColouredFixed(offsetX + 140, slide6 + 16, 26, 0, tokenColor, tokenColor, tokenColor);
			}
			else
			{
				offsetX = -20;
				offsetY = -32;

				if ((int16_t)g_unk830D4C == 5)
				{
					pulse = g_tickAccumulators.tick32Acc & 0x1F;

					if (pulse > 0xF)
						pulse = 31 - pulse;

					tokenColor = 4 * pulse + 80;

					if (g_tickAccumulators.tick32Acc < 0x10)
						goto LABEL_70;
				}
				else
				{
					tokenColor = 0x80;
				}

				Renderer::Sprite::DrawTile(302, slide6 + 32, 3, (int16_t)g_unk830D4C);

			LABEL_70:
				Renderer::Sprite::DrawColouredFixed(160, slide6 + 16, 51, 0, tokenColor, tokenColor, tokenColor);

				goto LABEL_71;
			}
		}

		// health
		int32_t slide1 = TickSlide(1);

		if (slide1 <= 0)
		{
			Renderer::Sprite::DrawScaledFixed(272, slide1 + 16, 11, 0, 0x80, 0x80, 0x80, 255, 2048, 2048);

			health = 2 * g_buzzActor.health + 2;

			if (health <= 4 && g_tickAccumulators.tick16Acc < 8)
				health = 0;

			Renderer::Sprite::DrawScaled(476, slide1 - health + 47, 6, 0, 0x80, (uint16_t)(6 * health), 0, 96, 20480, health << 12);
			Renderer::Sprite::DrawTiledFixed(296, slide1 + 16, 12, 0);
		}

		// the collected pieces
		int32_t slide5 = TickSlide(5);

		if (slide1 == 1)
			slide1 = -64;

		int32_t offset5 = (slide1 + 64) / 2;

		if (slide5 <= 0)
		{
			int32_t shown = g_collectionCount;

			if (g_collectionCount == 5)
			{
				if (g_unk830E38 != 5)
				{
					AudioManager::StartSoundSequenceOnActor(-5, (Actor::Toy2Actor*)&g_buzzActor);
					shown = g_collectionCount;
				}

				pulse = (g_tickAccumulators.tick32Acc - 16) & 0x1F;

				if (pulse > 0xF)
					pulse = 31 - pulse;

				flicker = 4 * pulse + 80;
			}
			else
			{
				flicker = 0x80;
			}

			if (shown != 5 || g_tickAccumulators.tick32Acc >= 0x10)
				Renderer::Sprite::DrawScaled(474 - (offset5 << 9) / 320, slide5 + 32, 20, shown + 26, 0xFF, 0xFF, 0xFF, 96, 4096, 4096);

			Renderer::Sprite::DrawColouredFixed(272 - offset5, slide5 + 16, 50, 0, flicker, flicker, flicker);
		}

		g_unk830E38 = g_collectionCount;
		int32_t slide8 = TickSlide(8);

		if (slide5 == 1)
			slide5 = -64;

		int32_t offset8 = (slide5 + 64) / 2 + offset5;

		if (slide8 <= 0)
		{
			pulse = (g_tickAccumulators.tick32Acc + 8) & 0x1F;

			if (pulse > 0xF)
				pulse = 31 - pulse;

			Renderer::Sprite::DrawColouredFixed(272 - offset8, slide8 + 16, 27, 0, 4 * pulse + 80, 4 * pulse + 80, 4 * pulse + 80);
		}

		int32_t barValue = slide8;
		int32_t slide9 = TickSlide(9);

		if (slide8 == 1)
			barValue = -64;

		int32_t offset9 = (barValue + 64) / 2 + offset8;

		if (slide9 <= 0)
		{
			pulse = (g_tickAccumulators.tick32Acc - 8) & 0x1F;

			if (pulse > 0xF)
				pulse = 31 - pulse;

			Renderer::Sprite::DrawColouredFixed(272 - offset9, slide9 + 16, 32, 0, 4 * pulse + 80, 4 * pulse + 80, 4 * pulse + 80);
		}

		// the coins
		int32_t slide2 = TickSlide(2);

		if (slide2 <= 0)
		{
			int32_t coinFrame = Renderer::g_frameDelta + g_unk830CA0;
			g_unk830CA0 += Renderer::g_frameDelta;

			if (g_unk830CA0 >= 24)
			{
				coinFrame = 0;
				g_unk830CA0 = 0;
			}

			int16_t coinY = 224 - slide2;
			Renderer::Sprite::DrawScaled(26, 224 - slide2, 16, coinFrame / 2, 0x80, 0x80, 0x80, 96, 3277, 2048);
			Renderer::Sprite::DrawScaled(51, coinY, 20, g_buzzActor.coinsCollected / 10 + 26, 0xFF, 0xFF, 0xFF, 96, 4096, 4096);
			Renderer::Sprite::DrawScaled(63, coinY, 20, g_buzzActor.coinsCollected % 10 + 26, 0xFF, 0xFF, 0xFF, 96, 4096, 4096);
		}

		// the spin
		int32_t slide3 = TickSlide(3);

		if (slide3 <= 0)
		{
			Renderer::Sprite::DrawScaledFixed(272, 222 - slide3, 14, 0, 0x80, 0x80, 0x80, 255, 2048, 2048);

			int32_t spinCharge = 0;

			if (Buzz::g_unk53C83C > 0)
				spinCharge = Buzz::g_unk53C83C / 2;

			int32_t spinFlashing = 0;
			int32_t spinRed = 128;

			if (Buzz::g_unk53C83C <= -120)
			{
				spinCharge = (Buzz::g_unk53C83C + 120) / -6;
				spinFlashing = 1;
			}

			if (Buzz::g_unk53C83C >= 60)
				spinFlashing = 1;

			if (g_tickAccumulators.tick8Acc < 4 && spinFlashing)
			{
				spinRed = 0;
				spinGreen = 0;
			}
			else
			{
				spinGreen = 128;
			}

			Renderer::Sprite::DrawScaled(438, 218 - slide3, 6, 0, spinRed, spinGreen, 0, 96, 6200 * spinCharge, 12288);
			Renderer::Sprite::DrawTiledFixed(272, 216 - slide3, 13, 0);
		}

		// the gun
		int32_t slide3Check = slide3;
		int32_t slide4 = TickSlide(4);

		if (slide4 <= 0)
		{
			if (slide3 == 1)
				slide3Check = -64;

			int32_t gunY = -slide4;
			int32_t offset4 = (slide3Check + 64) / 2;
			Renderer::Sprite::DrawScaledFixed(272 - offset4, 224 - slide4, 15, 0, 0x80, 0x80, 0x80, 255, 2048, 2048);

			int32_t slotCount = 0;
			int32_t slotOffsetY = -7;

			if (g_discLauncherTimer)
			{
				slotIcon = 28;
				slotCount = g_discLauncherTimer;
				slotOffsetY = 0;
			}

			if (g_grappleTimer)
			{
				slotIcon = 29;
				slotCount = g_grappleTimer;
			}

			if (slotCount)
			{
				int32_t slotY = slotOffsetY - slide4;
				int32_t slotDrawY = slotOffsetY - slide4 + 216;

				Renderer::Sprite::DrawScaledFixed(290 - offset4, slotDrawY, 20, slotCount / 10 + 26, 0xFF, 0xFF, 0xFF, 96, 2048, 2048);
				Renderer::Sprite::DrawScaledFixed(297 - offset4, slotDrawY, 20, slotCount % 10 + 26, 0xFF, 0xFF, 0xFF, 96, 2048, 2048);

				if (slotIcon == 28)
					Renderer::Sprite::DrawScaled(438 - (offset4 << 9) / 320, slotY + 214, 28, 0, 0x80, 0x80, 0x80, 96, 3072, 2048);
				else
					Renderer::Sprite::DrawColouredFixed(274 - offset4, slotY + 206, 33, 0, 0x80, 0x60, 0x40);
			}

			if (slotOffsetY)
			{
				if (Buzz::g_greenGunCharge)
				{
					int32_t gunGreen = Buzz::g_greenGunCharge / 16 + 64;
					Renderer::Sprite::DrawScaled(
						438 - (offset4 << 9) / 320, gunY + 218, 6, 0, 0, (uint8_t)gunGreen, 0, 96, 152 * Buzz::g_greenGunCharge, 12288);
				}
				else
				{
					int32_t fireRed;

					if (Buzz::g_fireHoldTimer == 64 && g_tickAccumulators.tick8Acc < 4)
						fireRed = 0x80;
					else
						fireRed = 0;

					uint32_t fireColor = (uint8_t)(2 * Buzz::g_fireHoldTimer - fireRed);

					Renderer::Sprite::DrawScaled(
						438 - (offset4 << 9) / 320, gunY + 218, 6, 0, (uint8_t)(0x80 - fireRed), fireColor, 0, 96, 5700 * Buzz::g_fireHoldTimer / 2, 12288);
				}

				Renderer::Sprite::DrawTiledFixed(272 - offset4, gunY + 216, 13, 0);
			}
		}

		// the boss bar
		int32_t slide7 = TickSlide(7);
		int16_t slide7Short = slide7;

		if (slide7 <= 0)
		{
			int32_t bossBar = g_unk52B7E0;

			if (g_unk52B7E0 >= 0)
			{
				if (g_unk52B7E0 > 54)
				{
					bossBar = 54;
					g_unk52B7E0 = 54;

					goto LABEL_153;
				}

				if (g_unk52B7E0 > 10)
					goto LABEL_153;
			}
			else
			{
				bossBar = 0;
				g_unk52B7E0 = 0;
			}

			if (g_tickAccumulators.tick16Acc >= 8)
			{
			LABEL_154:
				Renderer::Sprite::DrawScaled(224, slide7Short + 16, 13, 0, 0x80, 0x80, 0x80, 96, 0x2000, 0x2000);

				goto LABEL_155;
			}

		LABEL_153:
			Renderer::Sprite::DrawScaled(228, slide7Short + 20, 6, 0, 0x80, (uint8_t)(2 * bossBar), 0, 96, bossBar << 12, 0x4000);

			goto LABEL_154;
		}

	LABEL_155:
		if (g_demoMode == 1 && g_tickAccumulators.tick64Acc < 32)
			Renderer::DrawString(112, "demo mode", 0x80, 0x80, 0, 0);
	}

	// FUNCTION: TOY2 0x004A1A50
	void InitialiseLevelVariables(int32_t levelIndex)
	{
		// FUNCTION IS WIP: only the setup for level 1 (Andy's House) is ported, the other levels do nothing yet
		switch (levelIndex)
		{
			case 1:
				Collectables::Init(AndysHouse::g_unk4F0EDC, 72);
				Collectables::LoadTokenTable((int32_t*)AndysHouse::g_tutorialHints);
				Collectables::Activate(3, 1);
				MoveableObject::InitTable(AndysHouse::g_unk4F0F90);

				AndysHouse::g_sparkPathIndex = 1;
				AndysHouse::g_sparkPresetIndex = 0;
				AndysHouse::g_sparkPresetTimer = 0;
				AndysHouse::g_unk52F4F4 = 0;
				AndysHouse::g_unk52F568 = 0;
				AndysHouse::g_link14Rotation = 0;
				AndysHouse::g_liftVelocity = 0;
				AndysHouse::g_atticTilt = 0;

				Platform::SetRotationAngles(15, 0, 0, 0);

				AndysHouse::g_tinRobotLastPhase = g_levelActors[8].actorPhase;
				AndysHouse::g_unk52F4FC = 0;
				AndysHouse::g_unk52F5AC = 1024;
				AndysHouse::g_inTinRobotFight = 0;
				AndysHouse::g_raceCarPathIndex = 0;
				g_unk52F2F8 = 0;
				AndysHouse::g_raceCarLaps = 0;
				g_unk52AD64 = 0;
				AndysHouse::g_raceBuzzLastSide = 0;
				AndysHouse::g_unk52F580 = 0;
				AndysHouse::g_unk52F574 = 0;
				AndysHouse::g_unk52F570 = 0;
				AndysHouse::g_unk52F56C = 0;
				AndysHouse::g_unk52F57C = 0;
				AndysHouse::g_unk52F594 = 0;
				AndysHouse::g_potatoHeadBlitAngle = 0;
				AndysHouse::g_unk52F500 = 0;
				AndysHouse::g_unk52F59C = Levels::g_recordData[58]->data[Level::g_unk830E28 + 1].y;

				Levels::Method3(1, 1);

				Nu3D::Link::GetCurrentPosFixed(6, &AndysHouse::g_liftPos);

				AndysHouse::g_liftMaxY = AndysHouse::g_liftPos.y + 47104;
				AndysHouse::g_unk52F530 = Levels::g_recordData[61]->data[16].y - AndysHouse::g_liftPos.y;

				Nu3D::Link::GetCurrentPosFixed(9, &AndysHouse::g_link9Pos.pos);
				Nu3D::Link::GetCurrentPosFixed(17, &AndysHouse::g_link17Pos.pos);
				Nu3D::Link::GetCurrentPosFixed(18, &AndysHouse::g_link18Pos.pos);
				Nu3D::Link::GetCurrentPosFixed(10, &AndysHouse::g_link10Pos.pos);
				Nu3D::Link::GetCurrentPosFixed(16, &AndysHouse::g_link16Pos.pos);
				Nu3D::Link::SetScaleFromFixedOffsets(21, 0, 4096, 4096);
				Nu3D::Link::SnapToOtherLinkUsingScale(22, 21);

				Platform::DisableCollision(8);
				Platform::DisableCollision(14);

				AndysHouse::g_tinRobotFlashTimer = 0;
				AndysHouse::g_tinRobotFlash = 0;
				break;

			default:
				break;
		}
	}

	// FUNCTION: TOY2 0x004A1B00
	void HandleLevelInteractions(int32_t levelId)
	{
		switch (levelId)
		{
			case 1:
				AndysHouse::Interactions();
				break;

			case 2:
				AndysNeighborhood::Interactions();
				break;

			case 3:
				BombsAway::Interactions();
				break;

			case 4:
				ConstructionYard::Interactions();
				break;

			case 5:
				AlleysAndGullies::Interactions();
				break;

			case 6:
				SlimeTime::Interactions();
				break;

			case 7:
				AlsToyBarn::Interactions();
				break;

			case 8:
				AlsSpaceLand::Interactions();
				break;

			case 9:
				BarnEncounter::Interactions();
				break;

			case 10:
				ElevatorHop::Interactions();
				break;

			case 11:
				AlsPenthouse::Interactions();
				break;

			case 12:
				EvilEmperorZurg::Interactions();
				break;

			case 13:
				AirportInfiltration::Interactions();
				break;

			case 14:
				TarmacTrouble::Interactions();
				break;

			case 15:
				FinalShowdown::Interactions();
				break;

			case 16:
				Nullsub11();
				break;

			case 17:
				Nullsub12();
				break;

			default:
				return;
		}
	}

	// FUNCTION: TOY2 0x004A2960
	void UpdateFrameTimers()
	{
		uint8_t tickAcc = Renderer::g_frameDelta + g_tickAccumulators.tick2Acc;
		uint8_t tickCount = 0;
		bool lessThanPeriod = (Renderer::g_frameDelta + g_tickAccumulators.tick2Acc) < 2u;

		g_tickState[1] = Renderer::g_frameDelta;
		g_tickState[2] = 0;

		g_tickAccumulators.tick2Acc += Renderer::g_frameDelta;

		if (! lessThanPeriod)
		{
			do
			{
				tickAcc -= 2;
				++tickCount;
			} while (tickAcc >= 2u);

			g_tickState[2] = tickCount;
			g_tickAccumulators.tick2Acc = tickAcc;
		}

		tickCount = 0;
		tickAcc = Renderer::g_frameDelta + g_tickAccumulators.tick3Acc;

		g_tickState[3] = 0;
		g_tickAccumulators.tick3Acc += Renderer::g_frameDelta;

		if (g_tickAccumulators.tick3Acc >= 3u)
		{
			do
			{
				tickAcc -= 3;
				++tickCount;
			} while (tickAcc >= 3u);

			g_tickState[3] = tickCount;
			g_tickAccumulators.tick3Acc = tickAcc;
		}

		g_tickState[4] = 0;
		tickAcc = Renderer::g_frameDelta + g_tickAccumulators.tick4Acc;
		g_tickAccumulators.tick4Acc += Renderer::g_frameDelta;

		if (g_tickAccumulators.tick4Acc >= 4u)
		{
			g_tickState[4] = 1;
			g_tickAccumulators.tick4Acc = tickAcc - 4;
		}

		g_tickState[5] = 0;
		tickAcc = Renderer::g_frameDelta + g_tickAccumulators.tick5Acc;
		g_tickAccumulators.tick5Acc += Renderer::g_frameDelta;

		if (g_tickAccumulators.tick5Acc >= 5u)
		{
			g_tickState[5] = 1;
			g_tickAccumulators.tick5Acc = tickAcc - 5;
		}

		g_tickState[6] = 0;
		tickAcc = Renderer::g_frameDelta + g_tickAccumulators.tick6Acc;
		g_tickAccumulators.tick6Acc += Renderer::g_frameDelta;

		if (g_tickAccumulators.tick6Acc >= 6u)
		{
			g_tickState[6] = 1;
			g_tickAccumulators.tick6Acc = tickAcc - 6;
		}

		g_tickState[7] = 0;
		tickAcc = Renderer::g_frameDelta + g_tickAccumulators.tick7Acc;
		g_tickAccumulators.tick7Acc += Renderer::g_frameDelta;

		if (g_tickAccumulators.tick7Acc >= 7u)
		{
			g_tickState[7] = 1;
			g_tickAccumulators.tick7Acc = tickAcc - 7;
		}

		g_tickState[8] = 0;
		tickAcc = Renderer::g_frameDelta + g_tickAccumulators.tick8Acc;
		g_tickAccumulators.tick8Acc += Renderer::g_frameDelta;

		if (g_tickAccumulators.tick8Acc >= 8u)
		{
			g_tickState[8] = 1;
			g_tickAccumulators.tick8Acc = tickAcc - 8;
		}

		g_tickState[9] = 0;
		tickAcc = Renderer::g_frameDelta + g_tickAccumulators.tick16Acc;
		g_tickAccumulators.tick16Acc += Renderer::g_frameDelta;

		if (g_tickAccumulators.tick16Acc >= 0x10u)
		{
			g_tickState[9] = 1;
			g_tickAccumulators.tick16Acc = tickAcc - 16;
		}

		g_tickState[10] = 0;
		tickAcc = Renderer::g_frameDelta + g_tickAccumulators.tick32Acc;
		g_tickAccumulators.tick32Acc += Renderer::g_frameDelta;

		if (g_tickAccumulators.tick32Acc >= 0x20u)
		{
			g_tickState[10] = 1;
			g_tickAccumulators.tick32Acc = tickAcc - 32;
		}

		g_tickState[11] = 0;
		tickAcc = Renderer::g_frameDelta + g_tickAccumulators.tick64Acc;
		g_tickAccumulators.tick64Acc += Renderer::g_frameDelta;

		if (g_tickAccumulators.tick64Acc >= 0x40u)
		{
			g_tickState[11] = 1;
			g_tickAccumulators.tick64Acc = tickAcc - 64;
		}

		int32_t feedbackActive = 0;

		if (g_feedbackTimerA > 0)
		{
			g_feedbackTimerA -= Renderer::g_frameDelta;
			feedbackActive = 1;
		}

		if (g_feedbackTimerB <= 0)
		{
			g_feedbackTimerA_sub = 0;
		}
		else
		{
			g_feedbackTimerB -= Renderer::g_frameDelta;
			feedbackActive |= g_feedbackTimerA_sub & 0xFE;
		}

		if (g_demoMode != 1 && (SaveManager::g_save0Data.cameraType & SaveManager::CAMERA_PASSIVE))
		{
			if (feedbackActive)
			{
				g_unk830D38 += Renderer::g_frameDelta;

				if (g_unk830D38 > 360)
					g_unk830D38 = 300;
			}
			else
			{
				g_unk830D38 = 0;
			}
		}

		AudioManager::UpdateSoundSequences();

		if (g_randDatBufferPtr > &g_randDatBuffer[1500])
			g_randDatBufferPtr -= 1500;

		int16_t transitionTimer = g_levelTransitionTimer;

		if (g_levelTransitionTimer <= 0
			|| (transitionTimer = g_levelTransitionTimer - Renderer::g_frameDelta, g_levelTransitionTimer = transitionTimer, transitionTimer >= 0))
		{
			if (transitionTimer >= 46)
				goto LABEL_43;
		}
		else
		{
			transitionTimer = 0;
			g_levelTransitionTimer = 0;
		}

		if (Renderer::g_frameDelta + transitionTimer >= 46 && g_levelTransition > 0)
		{
			Nu3D::Camera::g_targetTintBlue = 0;
			Nu3D::Camera::g_targetTintGreen = 0;
			Nu3D::Camera::g_targetTintRed = 0;
			Nu3D::Camera::g_targetTintFadeSpeed = 6;
			Nu3D::Camera::g_tintBlend = -1;
		}

	LABEL_43:
		Nu3D::Camera::FadeToTargetTint();
	}

	// FUNCTION: TOY2 0x004A48B0
	void ResetGadgets()
	{
		memset(g_beamShots, 0, sizeof(g_beamShots));

		g_cosmicShieldPickup = 0;
		g_cosmicShieldPickupYPos = 0;

		g_cosmicShieldRotYaw = 0;
		g_cosmicShieldSpin = 0;

		g_rocketBootsPickup = 0;
		g_rocketBootsDuration = 0;

		g_grappleTimer = 0;
		g_grappleState = 0;
		g_grappleDistance = 0;
		g_grappleProgress = 0;

		g_discLauncherTimer = 0;
		g_discLauncherAmmo = 6;

		g_gravityBootsDuration = 0;
	}

	// FUNCTION: TOY2 0x00414720
	int32_t EnterLevel(int32_t levelIndex)
	{
		Nu3D::Camera::g_cameraTintBlue = 0;
		Nu3D::Camera::g_cameraTintGreen = 0;
		Nu3D::Camera::g_cameraTintRed = 0;

		Renderer::SetVirtualRatioTo54();

		int32_t demoMode = g_demoMode;

		if (! g_demoMode || g_demoMode == 123)
			LoadLevelGraphics(g_levelFileIndex);
		else
			LoadLevelGraphics(0);

		SoftwareRenderer::g_unk4F7400.x = 0;
		SoftwareRenderer::g_unk4F7400.y = 0;

		Nu3D::Camera::SetTint(128, 128, 128, 12);

		// fade in on the loading screen
		int32_t frameCount = 28;

		do
		{
			frameCount -= Renderer::g_frameDelta;

			if (frameCount <= 0)
				frameCount = 0;

			Nu3D::Camera::FadeToTargetTint();

			if (! demoMode)
				Renderer::Sprite::DrawScaled(96, 200, 128, 1, 255, 255, 255, 255, 2048, 2048);

			Nullsub3();
			MainMenu::RenderMenu();
		} while (frameCount);

		Levels::InitLevelPlay(levelIndex);

		int32_t result = ShowLevelIntroScreen(g_levelFileIndex, g_demoMode);

		Level::FixupRecordCoordinates();

		memset(g_tickState, 0, sizeof(g_tickState));
		memset(&g_tickAccumulators, 0, sizeof(g_tickAccumulators));
		memset(g_soundSequencePlayers, 0, sizeof(g_soundSequencePlayers));

		g_hudWidgetPositions[0] = 1024;
		g_hudWidgetPositions[1] = 1024;
		g_hudWidgetPositions[2] = 1024;
		g_levelTransition = 0;
		g_levelTransitionTimer = 0;
		g_isPaused = 0;
		g_pauseMenuBlinkTimer = 0;
		g_gameMenuScrollIdx = 0;
		g_pauseMenuType = MENU_MAIN;
		g_feedbackTimerA = 0;
		g_feedbackTimerB = 0;
		g_feedbackTimerA_sub = 0;
		Nu3D::Camera::g_targetTintBlue = 0;
		Nu3D::Camera::g_targetTintGreen = 0;
		Nu3D::Camera::g_targetTintRed = 0;
		Nu3D::Camera::g_targetTintFadeSpeed = 0;
		g_randDatBufferPtr = g_randDatBuffer;
		g_unk52B7E0 = 54;
		g_hudWidgetPositions[3] = 0;
		g_hudWidgetPositions[4] = 0;
		g_hudWidgetPositions[5] = 0;
		g_hudWidgetPositions[6] = 0;
		g_hudWidgetPositions[7] = 0;
		g_hudWidgetPositions[8] = 0;
		g_hudWidgetPositions[9] = 0;
		g_hudWidgetPositions[10] = 0;
		g_hudWidgetTimers[0] = 180;
		g_hudWidgetTimers[1] = 180;
		g_hudWidgetTimers[2] = 180;
		g_hudWidgetTimers[3] = 0;
		g_hudWidgetTimers[4] = 0;
		g_hudWidgetTimers[5] = 0;
		g_hudWidgetTimers[6] = 0;
		g_hudWidgetTimers[7] = 0;
		g_hudWidgetTimers[8] = 0;
		g_hudWidgetTimers[9] = 0;
		g_hudWidgetTimers[10] = 0;

		Buzz::Init(&g_buzzActor, levelIndex);
		Lighting::InitBuzzLight();
		Camera::InitGameplayCamera(&Camera::g_gameplayCameraState, &g_buzzActor);
		Nu3D::Particles::Init();
		ResetGadgets();

		Actor::g_activeActorArray[0] = 0;
		g_collectionCount = 0;
		g_unk830E38 = 0;
		g_unk830D4C = -1;
		g_unk50A0BC = 900;
		g_unk50A0B0 = 206;
		g_underwaterTintBlue = 128;
		g_underwaterTintGreen = 128;
		g_underwaterTintRed = 128;
		g_unk52F2F8 = 0;
		g_unk52AD64 = 0;

		InitialiseLevelVariables(levelIndex);
		Gadget::InitLevelUnlockGeometry();

		uint32_t levelTokenData = SaveManager::g_save0Data.tokens[g_levelFileIndex];

		g_unused3 = 0;
		g_unk52AD6A = 0;
		g_unk52AD6C = 0;
		g_unk52AD6E = 0;
		g_unk52AD70 = 0;
		g_unk52AD72 = 0;
		g_unk52AD74 = 0;
		g_unk52AD76 = 0;
		g_unk52AD78 = 0;
		g_unk52AD7A = 0;
		SaveManager::g_curLevelTokenData = levelTokenData;
		SaveManager::g_saveUnlocksCache = (uint8_t)SaveManager::g_save0Data.unlocks;
		g_unk830D44 = 0;
		g_unk830D38 = 0;
		g_unk830C90 = 0;
		g_unk830E2C = (Nu3D::Particles::ParticleInstance*)-1;

		// the level music, every third level is a boss and has its own
		if (g_levelFileIndex > 0 && g_levelFileIndex < 16 && g_levelIndex < 15 && g_levelFileIndex % 3)
			AudioManager::PlayMusicLooping(g_levelIndex);

		return result;
	}

	namespace Game
	{
		// FUNCTION: TOY2 0x0049DFE0
		void MainLoop()
		{
			int16_t curButtons = InputManager::g_curButtonsPressed;
			int32_t demoMode = g_demoMode;
			int32_t writeIdx;

			// demo recording, the buttons are stored as run length pairs
			if (g_demoMode == 2 && InputManager::g_curButtonsPressed != InputManager::g_prevButtonsPressed)
			{
				int32_t pathIdx = g_demoPathWriteIdx;

				g_demoPathBuffer[pathIdx] = InputManager::g_prevButtonsPressed;
				g_demoPathBuffer[pathIdx + 1] = (int16_t)g_demoInputRunLength;

				writeIdx = pathIdx + 2;
				g_demoInputRunLength = 0;
				g_demoPathWriteIdx = writeIdx;

				if (writeIdx >= 2048)
				{
					writeIdx = 2046;
					g_demoPathWriteIdx = 2046;
				}
			}
			else
			{
				if (g_demoMode == 2)
					++g_demoInputRunLength;

				writeIdx = g_demoPathWriteIdx;
			}

			if (g_attractModeTimer >= 0)
			{
				int32_t attractTimer = (g_returnedToTitle || ! curButtons) ? g_attractModeInputTimer : 2 * g_attractModeTimer;

				g_attractModeInputTimer = attractTimer - Renderer::g_frameDelta;

				if (attractTimer - (int32_t)Renderer::g_frameDelta <= 0)
				{
					curButtons |= 1;
					InputManager::g_curButtonsPressed = curButtons;
				}
			}

			// demo playback
			if (demoMode == 1)
			{
				g_unk52AD84 = curButtons;

				bool runLengthEnded = g_demoInputRunLength - 1 < 0;
				--g_demoInputRunLength;

				if (runLengthEnded)
				{
					int32_t runLength = g_demoPathBuffer[writeIdx + 3];

					writeIdx += 2;
					g_demoInputRunLength = runLength;
					g_demoPathWriteIdx = writeIdx;
				}

				curButtons = g_demoPathBuffer[writeIdx];
				InputManager::g_curButtonsPressed = curButtons;
			}

			InputManager::g_directionInputState3Frames = InputManager::g_directionInputState2Frames;
			InputManager::g_directionInputState2Frames = InputManager::g_prevDirectionInputState;
			InputManager::g_prevDirectionInputState = InputManager::g_directionInputState;

			if (demoMode == 1)
				InputManager::g_directionInputState = curButtons;
			else
				InputManager::UpdateDirectionInputState();

			if ((g_buzzActor.actorFlags & Actor::ACTOR_LOCKED_MOVE) || (! g_levelTransition && g_levelTransitionTimer > 60))
				InputManager::g_directionInputState &= DIR_SECRET_MENU | DIR_MENU | DIR_CAMERA_LEFT | DIR_CAMERA_RIGHT;

			Shadow::ResetShadowCount();
			Cutscene::Update();
			Buzz::HandleGameplay(&g_buzzActor);

			Camera::UpdateActiveTransform();
			Nu3D::Camera::ApplyTransformToCamera(&Camera::g_activeCameraTransform);

			UpdateActors();
			Buzz::UpdateRespawnAnchor();
			Collectables::Interactions();
			Buzz::TickGadgets();
			Buzz::CheckParticleCollisions();

			if (g_buzzActor.health >= 0)
				ActorCollisionCheck();

			HandleLevelInteractions(g_levelFileIndex);
			Lighting::UpdateBuzzLight();

			g_buzzActor.unkVar23 = 0;

			Buzz::UpdateAnimationState();
			UnusedVarUpdate();
			Buzz::UpdateContactEffects();
			Level::UpdateAmbientEmitters();
			Nu3D::Particles::Update();

			demoMode = g_demoMode;

			if (g_demoMode
				|| (g_attractModeTimer >= 0 && (InputManager::g_curButtonsPressed & 1) && ! g_levelTransition
					&& (g_levelTransition = 4, g_levelTransitionTimer = 46, Nu3D::Camera::SetTint(0, 0, 0, 6), (demoMode = g_demoMode) != 0)))
			{
				if ((InputManager::g_curButtonsPressed & 8) && ! g_levelTransition)
				{
					g_levelTransition = 3;
					g_levelTransitionTimer = 46;
					Nu3D::Camera::SetTint(0, 0, 0, 6);
					demoMode = g_demoMode;
				}

				if (demoMode && g_unk52AD84 && ! g_levelTransition)
				{
					g_levelTransition = 4;
					g_levelTransitionTimer = 46;
					Nu3D::Camera::SetTint(0, 0, 0, 6);
					demoMode = g_demoMode;
				}
			}

			if ((InputManager::g_curButtonsPressed & 8) == 0 || (InputManager::g_prevButtonsPressed & 8) != 0)
			{
				if (demoMode)
					goto LABEL_49;

				goto LABEL_45;
			}

			if (demoMode)
				goto LABEL_49;

			if (g_unk830CC4 == 9)
				goto LABEL_45;

			goto LABEL_46;

		LABEL_45:
			if (InputManager::g_directionalInputCount != 3)
				goto LABEL_49;

		LABEL_46:
			// the menu button was pressed, pause the game as soon as the screen is not fading
			if (! Nu3D::Camera::g_targetTintFadeSpeed && Nu3D::Camera::g_cameraTintRed)
			{
				AudioManager::StopAndWait();
				AudioManager::FlushSoundVoices();

				g_pauseMenuBlinkTimer = 0;
				g_isPaused = 1;
				g_gameMenuScrollIdx = 0;

				g_pauseMenuType = MENU_MAIN;
				g_quitToTitleFlag = 0;
				AudioManager::PlaySoundEffect(61, 0);

				memcpy(&g_unk830CB0, &Camera::g_activeCameraTransform, sizeof(g_unk830CB0));
				g_devCamState = 3780;
			}

		LABEL_49:
			UpdateFrameTimers();
			RenderHUD();
			Actor::PopulateActiveActors();
			Nullsub3();
			RenderGame(1);
		}
	}

	// FUNCTION: TOY2 0x004A4B70
	void ActivateGravityBoots()
	{
		ResetBuzzState();

		g_gravityBootsDuration = 600;
		g_gravityBootsHoverHeight = 0x2000;
	}

	// FUNCTION: TOY2 0x004A5070
	void RespawnCosmicShield()
	{
		if (g_cosmicShieldPickup)
		{
			g_cosmicShieldPickup->pos.y = g_cosmicShieldPickupYPos;

			Nu3D::Link::SetScaleFromFixedOffsets(g_cosmicShieldPickup->props.linkId, 0x2000, 0x2000, 0x2000);
			Nu3D::Link::SetPositionRawAndCommit(
				g_cosmicShieldPickup->props.linkId, g_cosmicShieldPickup->pos.x, g_cosmicShieldPickup->pos.y, g_cosmicShieldPickup->pos.z);

			g_cosmicShieldPickup = 0;
		}
	}

	// FUNCTION: TOY2 0x004A5D30
	Toy2::BeamShot* SpawnBeamShot(
		int32_t beamType,
		int16_t aimYaw,
		int16_t aimPitch,
		Transform* origin,
		Transform* headOffset,
		int32_t range,
		Toy2::Actor::Toy2Actor* autoAimActor)
	{
		int32_t oldestLife = 1000;
		BeamShot* beamShot = 0;
		int32_t index = 0;

		// reuse the first free slot, otherwise the one that is closest to expiring
		while (true)
		{
			int16_t lifeTimer = g_beamShots[index].lifeTimer;

			if (! lifeTimer)
			{
				beamShot = &g_beamShots[index];

				break;
			}

			if (lifeTimer < oldestLife)
			{
				oldestLife = lifeTimer;
				beamShot = &g_beamShots[index];
			}

			++index;

			if (index >= 4)
				break;
		}

		Transform startPos;
		Transform displacement;

		if (autoAimActor)
		{
			Actor::Toy2Actor* actor = Actor::g_activeActors[0];
			int32_t bestAngleDiff = 0x7FFFFFFF;

			if (actor)
			{
				Actor::Toy2Actor** nextActor = Actor::g_activeActors;

				do
				{
					if ((actor->actorFlags & 1) != 0 && (actor->creatureRam->defenseMode & 4) != 0 && actor->unkVar29 >= 0)
					{
						int32_t deltaZ = (actor->transform.pos.z - origin->pos.z) >> 5;
						int32_t deltaY = (actor->transform.pos.y - origin->pos.y) >> 5;
						int32_t deltaX = (actor->transform.pos.x - origin->pos.x) >> 5;

						if (deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ < 0x1000000 && abs(deltaY) < 768)
						{
							int32_t angleDiff = (Nu3D::Math::CartesianToFixedAngle(deltaX, deltaZ) - aimYaw) & 0xFFF;

							// quirk: 4095, not 4096
							if (angleDiff > 0x800)
								angleDiff -= 4095;

							if (abs(angleDiff) < abs(bestAngleDiff))
							{
								bestAngleDiff = angleDiff;
								autoAimActor = actor;
							}
						}
					}

					actor = *++nextActor;
				} while (*nextActor);

				if (bestAngleDiff != 0x7FFFFFFF)
				{
					int16_t targetYaw = autoAimActor->transform.angles.yaw;
					int32_t sinFacing = Numerics::g_sinCosLUT[(targetYaw - 2048) & 0xFFF] >> 2;
					int16_t* attachOffset = (int16_t*)(autoAimActor->unkVar34 + 16 * autoAimActor->primaryAnimIdx);
					int32_t cosFacing = Numerics::g_sinCosLUT[(targetYaw - 1024) & 0xFFF] >> 2;
					int32_t attachZ = attachOffset[2];

					int32_t aimDeltaX = (autoAimActor->transform.pos.x + ((sinFacing * attachZ + cosFacing * attachOffset[0]) >> 12) - origin->pos.x) >> 5;
					int32_t aimDeltaY = (autoAimActor->transform.pos.y + attachOffset[1] - origin->pos.y) >> 5;
					int32_t aimDeltaZ = (autoAimActor->transform.pos.z + ((cosFacing * attachZ - sinFacing * attachOffset[0]) >> 12) - origin->pos.z) >> 5;

					int32_t targetAngle = Nu3D::Math::CartesianToFixedAngle(aimDeltaX, aimDeltaZ);

					if (((targetAngle - aimYaw + 128) & 0xFFF) < 0x100)
					{
						aimYaw = targetAngle;

						int32_t pitch =
							(Nu3D::Math::CartesianToFixedAngle((int32_t)sqrt((double)(aimDeltaX * aimDeltaX + aimDeltaZ * aimDeltaZ)), aimDeltaY) - 1024)
							& 0xFFF;

						if (pitch > 2048)
							pitch -= 4096;

						if (pitch <= 64)
						{
							if (pitch < -64)
								pitch = -64;
						}
						else
						{
							pitch = 64;
						}

						aimPitch = (int16_t)pitch;
					}
					else
					{
						g_buzzActor.facingAngle = targetAngle;
					}
				}
			}
		}

		int32_t rangeCos = range * Numerics::g_sinCosLUT[(aimPitch + 1024) & 0xFFF];
		int32_t offsetX = rangeCos * Numerics::g_sinCosLUT[aimYaw & 0xFFF];
		int32_t offsetZ = rangeCos * Numerics::g_sinCosLUT[(aimYaw + 1024) & 0xFFF];
		int32_t offsetY = 2 * range * Numerics::g_sinCosLUT[(aimPitch - 2048) & 0xFFF];

		beamShot->headPos.x = headOffset->pos.x + origin->pos.x;
		beamShot->headPos.y = origin->pos.y + headOffset->pos.y;
		beamShot->headPos.z = origin->pos.z + headOffset->pos.z;

		beamShot->originPos.x = (offsetX >> 13) + origin->pos.x;
		beamShot->originPos.y = offsetY + origin->pos.y;
		beamShot->originPos.z = (offsetZ >> 13) + origin->pos.z;
		beamShot->lifeTimer = 32;

		startPos.pos.x = origin->pos.x;
		startPos.pos.y = origin->pos.y;
		startPos.pos.z = origin->pos.z;

		displacement.pos.x = beamShot->originPos.x - startPos.pos.x;
		displacement.pos.y = beamShot->originPos.y - startPos.pos.y;
		displacement.pos.z = beamShot->originPos.z - startPos.pos.z;

		if (beamType == 0)
		{
			beamShot->tintB = 0x80;
			beamShot->tintG = 0;
			beamShot->tintR = 0;
			beamShot->halfWidth = 32;
		}
		else if (beamType == 1)
		{
			beamShot->tintB = 0x80;
			beamShot->tintG = 0x80;
			beamShot->tintR = 0;
			beamShot->halfWidth = 64;
		}
		else if (beamType == 2)
		{
			beamShot->tintB = 0;
			beamShot->tintG = 0x80;
			beamShot->tintR = 0;
			beamShot->halfWidth = 32;
		}

		int32_t hitWall = Nu3D::Collision::SweepAndSlide(&startPos, &displacement, 0x8000, 0, 256);
		int32_t hit = Camera::SweepSegmentVsActors(&startPos, &displacement, beamType + 1) | hitWall;

		if (hit)
		{
			beamShot->originPos.x = startPos.pos.x + displacement.pos.x;
			beamShot->originPos.y = startPos.pos.y + displacement.pos.y;
			beamShot->originPos.z = startPos.pos.z + displacement.pos.z;

			if (beamType)
			{
				if (beamType == 1)
				{
					Nu3D::Particles::SpawnFromPreset(beamShot->originPos.x, beamShot->originPos.y, beamShot->originPos.z, 10, 5);
				}
				else if (beamType == 2)
				{
					if (Camera::g_cameraState == 4)
						Nu3D::Particles::SpawnFromPreset(beamShot->originPos.x, beamShot->originPos.y, beamShot->originPos.z, 74, 5);
					else
						Nu3D::Particles::SpawnFromPreset(beamShot->originPos.x, beamShot->originPos.y, beamShot->originPos.z, 73, 2);
				}
			}
			else if (Camera::g_cameraState == 4)
			{
				Nu3D::Particles::SpawnFromPreset(beamShot->originPos.x, beamShot->originPos.y, beamShot->originPos.z, 11, 5);
			}
			else
			{
				Nu3D::Particles::SpawnFromPreset(beamShot->originPos.x, beamShot->originPos.y, beamShot->originPos.z, 1, 2);
			}

			if (hit == 1)
			{
				Nu3D::Particles::SpawnFromPreset(beamShot->originPos.x, beamShot->originPos.y, beamShot->originPos.z, 5, 2);
				AudioManager::PlaySoundEffect(8, &beamShot->originPos);
			}

			int32_t sparkCount = 5;

			do
			{
				Nu3D::Particles::SpawnFromPreset(beamShot->originPos.x, beamShot->originPos.y, beamShot->originPos.z, 4, 4)->lifetime =
					2 * (*g_randDatBufferPtr & 0xF) + 24;

				--sparkCount;
				++g_randDatBufferPtr;
			} while (sparkCount);
		}

		startPos.pos.x = (beamShot->originPos.x - beamShot->headPos.x) >> 2;
		startPos.pos.y = (beamShot->originPos.y - beamShot->headPos.y) >> 2;
		startPos.pos.z = (beamShot->originPos.z - beamShot->headPos.z) >> 2;

		Nu3D::Math::NormalizeToFixedPoint((Vector3I*)&startPos, (Vector3I*)&startPos);

		beamShot->velocity.y = startPos.pos.y >> 1;
		beamShot->velocity.z = startPos.pos.z >> 1;
		beamShot->velocity.x = startPos.pos.x >> 1;

		int32_t dominantVel = beamShot->velocity.x;
		int32_t dominantDist;

		if (abs(dominantVel) <= abs(beamShot->velocity.y))
		{
			dominantVel = beamShot->velocity.y;
			dominantDist = beamShot->originPos.y - beamShot->headPos.y;
		}
		else
		{
			dominantDist = beamShot->originPos.x - beamShot->headPos.x;
		}

		if (abs(dominantVel) <= abs(beamShot->velocity.z))
		{
			dominantVel = beamShot->velocity.z;
			dominantDist = beamShot->originPos.z - beamShot->headPos.z;
		}

		if (! dominantVel)
			dominantVel = 1;

		beamShot->moveTimer = dominantDist / dominantVel;

		return (hit & 4) != 0 ? beamShot : 0;
	}

	// FUNCTION: TOY2 0x004CD110
	int32_t ShowTokenSparkle() { return 1; }
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
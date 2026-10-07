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
#include "Nu3D/Viewport.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Light.h"
#include "Nu3D/Link.h"
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
#include <MATH.H>
#include <DINPUT.H>

namespace Toy2
{
	// GLOBAL: TOY2 0x0072E34C
	int32_t g_mpegPlay;

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
	SoundSequencePlayer* g_curSequencePlayer;

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
	int32_t g_unk830D4C;

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
	int32_t g_unk830E2C;

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

		// FUNCTION: TOY2 0x00406CD0
		void InitActor(Toy2::Actor::Toy2Actor* actor, int32_t initialSpawn)
		{
			RawLoader::CreatureListRam* creatureRam = actor->creatureRam;
			int16_t initActorFlags;

			actor->creatureId = creatureRam->creatureId;
			actor->pos.x = 32 * creatureRam->pos.x;
			actor->pos.y = 32 * creatureRam->pos.y;
			actor->pos.z = 32 * creatureRam->pos.z;
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

			int32_t actorPosX = actor->pos.x;
			int32_t actorPosZ = actor->pos.z;
			actor->actorFlags = initActorFlags;
			actor->unkShort2 = creatureRam->entCtrl.unkByte;

			int32_t actorPosY = actor->pos.y;

			bool flag = actor->unkShort2 == 100;

			actor->boundary.x = actorPosX;
			actor->motionTargetPos.x = actorPosX;
			actor->boundary.y = actorPosY;
			actor->boundary.z = actorPosZ;
			actor->motionTargetPos.y = actorPosY;
			actor->motionTargetPos.z = actorPosZ;

			if (flag)
				actor->unkShort2 = 1800;

			actor->pitchAngle = 0;

			int16_t initialFacingAngle = 16 * creatureRam->initialFacingAngle;

			actor->yawAngle = initialFacingAngle;
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

		// STUB: TOY2 0x00407440
		void ActorCollisionCheck() {}

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
						& (Actor::ACTOR_CUTSCENE_MOVE | Actor::ACTOR_LOCKED_MOVE | Actor::ACTOR_GROUNDED | Actor::ACTOR_FLOOR_SNAP
							| Actor::ACTOR_STUNNED | Actor::ACTOR_OOB_SPECIAL_MOVE | Actor::ACTOR_DEATH_DAMAGE));

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
						else if ((actor->actorFlags & Actor::ACTOR_OOB_SPECIAL_MOVE) != 0
							|| !Nu3D::Camera::LineOfSightCheck(&Camera::g_activeCameraTransform, actor))
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
							int32_t dz = (Camera::g_activeCameraTransform.pos.z - actor->boundingOffset.z - actor->pos.z) >> 8;
							int32_t dx = (Camera::g_activeCameraTransform.pos.x - actor->boundingOffset.x - actor->pos.x) >> 8;
							int32_t dy = (Camera::g_activeCameraTransform.pos.y - actor->boundingOffset.y - actor->pos.y) >> 8;

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
							int32_t spawnX = curActor->pos.x + attachOffset[0] + 32 * (*g_randDatBufferPtr++ - 128);
							int32_t spawnY = curActor->pos.y + attachOffset[1] + 32 * (*g_randDatBufferPtr++ - 128);
							int32_t spawnZ = curActor->pos.z + attachOffset[2] + 32 * (*g_randDatBufferPtr++ - 128);

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

	// STUB: TOY2 0x00430E90
	void ReadIniFile() {}

	// STUB: TOY2 0x0047EF80
	void ProfileCPU() {}

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
			if (!lstrcmpA(arg, "cheat"))
				g_cheatsEnabled = 1;

			if (!lstrcmpA(arg, "log"))
				Logger::g_logsEnabled = 1;

			if (!lstrcmpA(arg, "mpeg"))
			{
				g_mpegPlay = 1;
				Logger::Log("ONEINIT : Mpeg play OFF.\n");
			}

			if (!lstrcmpA(arg, "high"))
			{
				g_highQualityMpegEnabled = 1;
				Logger::Log("ONEINIT : Hi quality mpeg playing ON.\n");
			}

			if (!lstrcmpA(arg, "demo"))
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

	// STUB: TOY2 0x00414270
	void LoadLevelWithFadeIn(int32_t levelId, int32_t flag) {}

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

	// STUB: TOY2 0x0044F840
	void ShowModelNode(int32_t creatureIndex, int32_t nodeIdx) {}

	// STUB: TOY2 0x0044F860
	void HideModelNode(int32_t creatureIndex, int32_t nodeIdx) {}

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

	// STUB: TOY2 0x00498550
	void ProcessMiscEvents() {}

	// STUB: TOY2 0x0049A930
	int32_t PlayMovie(int32_t movieId) { return 0; }

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

	// FUNCTION: TOY2 0x004A1A50
	void InitialiseLevelVariables(int32_t levelIndex)
	{
		// FUNCTION IS WIP: only the setup for level 1 (Andy's House) is ported, the other levels do nothing yet
		switch (levelIndex)
		{
			case 1:
				Collectables::Init(AndysHouse::g_unk4F0EDC, 72);
				Collectables::LoadTokenTable((int32_t*)AndysHouse::g_unk4F0EE8);
				Collectables::Activate(3, 1);
				MoveableObject::InitTable(AndysHouse::g_unk4F0F90);

				AndysHouse::g_unk52F52C = 1;
				AndysHouse::g_unk52F598 = 0;
				AndysHouse::g_unk52F5B0 = 0;
				AndysHouse::g_unk52F4F4 = 0;
				AndysHouse::g_unk52F568 = 0;
				AndysHouse::g_unk52F5CC = 0;
				AndysHouse::g_unk52F504 = 0;
				AndysHouse::g_unk52F58C = 0;

				Platform::SetRotationAngles(15, 0, 0, 0);

				AndysHouse::g_unk52F5A8 = g_levelActors[8].actorPhase;
				AndysHouse::g_unk52F4FC = 0;
				AndysHouse::g_unk52F5AC = 1024;
				AndysHouse::g_inTinRobotFight = 0;
				AndysHouse::g_unk52F528 = 0;
				g_unk52F2F8 = 0;
				AndysHouse::g_unk52F4F8 = 0;
				g_unk52AD64 = 0;
				AndysHouse::g_unk52F584 = 0;
				AndysHouse::g_unk52F580 = 0;
				AndysHouse::g_unk52F574 = 0;
				AndysHouse::g_unk52F570 = 0;
				AndysHouse::g_unk52F56C = 0;
				AndysHouse::g_unk52F57C = 0;
				AndysHouse::g_unk52F594 = 0;
				AndysHouse::g_unk52F5D0 = 0;
				AndysHouse::g_unk52F500 = 0;
				AndysHouse::g_unk52F59C = Levels::g_recordData[58]->data[Level::g_unk830E28 + 1].y;

				Levels::Method3(1, 1);

				Nu3D::Link::GetCurrentPosFixed(6, &AndysHouse::g_unk52F5B8);

				AndysHouse::g_unk52F5B4 = AndysHouse::g_unk52F5B8.y + 47104;
				AndysHouse::g_unk52F530 = Levels::g_recordData[61]->data[16].y - AndysHouse::g_unk52F5B8.y;

				Nu3D::Link::GetCurrentPosFixed(9, (Vector3I*)AndysHouse::g_unk52F538);
				Nu3D::Link::GetCurrentPosFixed(17, (Vector3I*)AndysHouse::g_unk52F548);
				Nu3D::Link::GetCurrentPosFixed(18, (Vector3I*)AndysHouse::g_unk52F558);
				Nu3D::Link::GetCurrentPosFixed(10, (Vector3I*)AndysHouse::g_unk52F508);
				Nu3D::Link::GetCurrentPosFixed(16, (Vector3I*)AndysHouse::g_unk52F518);
				Nu3D::Link::SetScaleFromFixedOffsets(21, 0, 4096, 4096);
				Nu3D::Link::SnapToOtherLinkUsingScale(22, 21);

				Platform::DisableCollision(8);
				Platform::DisableCollision(14);

				AndysHouse::g_unk52F590 = 0;
				AndysHouse::g_unk52F5C8 = 0;
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

		if (g_levelTransitionTimer <= 0 || (transitionTimer = g_levelTransitionTimer - Renderer::g_frameDelta, g_levelTransitionTimer = transitionTimer, transitionTimer >= 0))
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
		g_unk830E2C = -1;

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

			if (g_demoMode ||
				(g_attractModeTimer >= 0 && (InputManager::g_curButtonsPressed & 1) && ! g_levelTransition &&
				 (g_levelTransition = 4, g_levelTransitionTimer = 46, Nu3D::Camera::SetTint(0, 0, 0, 6), (demoMode = g_demoMode) != 0)))
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
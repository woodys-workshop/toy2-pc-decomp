#include "Toy2/LevelSelect.h"
#include "Toy2/Toy2.h"
#include "Toy2/Levels.h"
#include "Renderer/Renderer.h"
#include "Renderer/Sprite.h"
#include "InputManager.h"
#include "Random.h"
#include "Nu3D/Link.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Math.h"
#include "NGNLoader/NGNLoader.h"
#include "Nullsub.h"
#include "SaveManager.h"
#include "AudioManager/AudioManager.h"

#include <MATH.H>
#include <STDIO.H>

namespace Toy2
{
	namespace LevelSelect
	{
		// GLOBAL: TOY2 0x0055A0E4
		int32_t g_levelSelectCursor;

		// TODO: Label these
		int16_t g_hideLinks1[6] = { 1, 2, 3, 4, -1, 0 };

		int16_t g_showLinks1[6] = { 100, 101, 102, 103, 104, -1 };

		int16_t g_hideLinks2[6] = { 5, 6, 7, 8, -1, 0 };

		int16_t g_showLinks2[6] = { 105, 106, 107, 108, -1, 0 };

		int16_t g_hideLinks3[6] = { 9, 10, 11, 12, 0, -1 };

		int16_t g_showLinks3[6] = { 109, 110, 111, 112, -1, 0 };

		int16_t g_hideLinks4[18] = { 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, -1 };

		int16_t g_showLinks4[18] = { 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, -1 };

		int16_t g_hideLinks5[14] = { 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, -1, 0 };

		int16_t g_showLinks5[14] = { 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, -1, 0 };

		int16_t g_hideLinks6[8] = { 43, 44, 45, 46, 47, 131, 132, -1 };

		int16_t g_showLinks6[8] = { 143, 144, 145, 146, 147, 231, 232, -1 };

		int16_t g_hideLinks7[6] = { 48, 49, 50, 51, -1, 0 };

		int16_t g_showLinks7[6] = { 148, 149, 150, 151, -1, 0 };

		int16_t g_hideLinks8[6] = { 52, 53, 54, 55, -1, 0 };

		int16_t g_showLinks8[6] = { 152, 153, 154, 155, -1, 0 };

		int16_t g_hideLinks9[4] = { 56, 57, 153, -1 };

		int16_t g_showLinks9[4] = { 156, 157, 253, -1 };

		int16_t g_hideLinks10[6] = { 58, 59, 60, 61, -1, 0 };

		int16_t g_showLinks10[6] = { 158, 159, 160, 161, -1, 0 };

		int16_t g_hideLinks11[4] = { 62, 63, 158, -1 };

		int16_t g_showLinks11[4] = { 162, 163, 258, -1 };

		int16_t g_hideLinks12[4] = { 64, 65, 258, -1 };

		int16_t g_showLinks12[4] = { 164, 165, 358, -1 };

		int16_t g_hideLinks13[6] = { 66, 67, 68, 69, 70, -1 };

		int16_t g_showLinks13[6] = { 166, 167, 168, 169, 170, -1 };

		int16_t g_hideLinks14[4] = { 71, 166, -1, 0 };

		int16_t g_showLinks14[4] = { 171, 266, -1, 0 };

		int16_t g_hideLinks15[6] = { 72, 73, 74, 266, -1, 0 };

		int16_t g_showLinks15[6] = { 172, 173, 174, 366, -1, 0 };

		// GLOBAL: TOY2 0x004F6DC4
		LevelLinkPair g_levelSelectLinks[15] = {
			{ g_hideLinks1, g_showLinks1 },	  { g_hideLinks2, g_showLinks2 },	{ g_hideLinks3, g_showLinks3 },	  { g_hideLinks4, g_showLinks4 },
			{ g_hideLinks5, g_showLinks5 },	  { g_hideLinks6, g_showLinks6 },	{ g_hideLinks7, g_showLinks7 },	  { g_hideLinks8, g_showLinks8 },
			{ g_hideLinks9, g_showLinks9 },	  { g_hideLinks10, g_showLinks10 }, { g_hideLinks11, g_showLinks11 }, { g_hideLinks12, g_showLinks12 },
			{ g_hideLinks13, g_showLinks13 }, { g_hideLinks14, g_showLinks14 }, { g_hideLinks15, g_showLinks15 },
		};

		// GLOBAL: TOY2 0x004F6BE4
		char* g_levelNames[16] = {
			reinterpret_cast<char*>(145), // not sure?
			"andy's house",
			"andy's neighborhood",
			"bombs away",
			"construction yard",
			"alleys and gullies",
			"slime time",
			"al's toy barn",
			"al's space land",
			"toy barn encounter",
			"elevator hop",
			"al's penthouse",
			"the evil emperor zurg",
			"airport infiltration",
			"tarmac trouble",
			"final showdown",
		};

		// GLOBAL: TOY2 0x0053E4AC
		RGB32 g_unusedColour;

		// GLOBAL: TOY2 0x00830C5C
		float g_arrowZoomProgress;

		// GLOBAL: TOY2 0x004F6878
		char* g_jumpToSelectTxt = "jump to select";

		// FUNCTION: TOY2 0x00452170
		void ResetCursor() { g_levelSelectCursor = 0; }

		// GLOBAL: TOY2 0x00500B20
		float g_arrowRects[56] = {
			0.0,   0.0,	  255.0, 64.0,	0.0,   64.0,  72.0,	 146.0, 0.0,   146.0, 72.0,	 226.0, 0.0,   226.0, 72.0,	 305.0, 0.0,   306.0, 255.0,
			382.0, 72.0,  64.0,	 78.0,	70.0,  72.0,  299.0, 78.0,	305.0, 256.0, 0.0,	 511.0, 64.0,  440.0, 64.0,	 511.0, 146.0, 440.0, 146.0,
			511.0, 226.0, 440.0, 226.0, 511.0, 305.0, 256.0, 306.0, 511.0, 382.0, 434.0, 64.0,	440.0, 70.0,  434.0, 299.0, 440.0, 305.0,
		};

		// GLOBAL: TOY2 0x00500AB0
		Vector2F g_arrowUVs[14] = {
			{ 0.0, 0.0 },
			{ 0.99609375, 0.25 },
			{ 0.0, 0.578125 },
			{ 0.28515625, 0.89453125 },
			{ 0.296875, 0.578125 },
			{ 0.58203125, 0.88671875 },
			{ 0.6015625, 0.578125 },
			{ 0.88671875, 0.88671875 },
			{ 0.0, 0.265625 },
			{ 0.99609375, 0.5625 },
			{ 0.90625, 0.578125 },
			{ 0.9296875, 0.6015625 },
			{ 0.90625, 0.62109375 },
			{ 0.9296875, 0.64453125 },
		};

		// FUNCTION: TOY2 0x00438650
		void TurnTowardLookDir(LevelSelectCamera* levelSelectCam, int32_t turnRate)
		{
			int32_t z = levelSelectCam->lookDir.z;
			int32_t x = levelSelectCam->lookDir.x;
			int32_t y = levelSelectCam->lookDir.y;

			int32_t maxTurn = turnRate * Renderer::g_frameDelta;

			// yaw
			int32_t yaw = levelSelectCam->angles.yaw;
			int32_t yawDelta = (Nu3D::Math::CartesianToFixedAngle(x, z) - yaw) & 0xFFF;

			if (yawDelta >= 2048)
			{
				int32_t step = (4096 - yawDelta) >> 3;

				if (step >= maxTurn)
					yaw -= maxTurn;
				else
					yaw -= step;
			}
			else
			{
				int32_t step = yawDelta >> 3;

				if (step >= maxTurn)
					yaw += maxTurn;
				else
					yaw += step;
			}

			levelSelectCam->angles.yaw = yaw;

			// pitch
			int32_t pitch = levelSelectCam->angles.pitch;
			int32_t pitchAngle;

			if (y >= 0)
				pitchAngle = Nu3D::Math::CartesianToFixedAngle(-(y * y), x * x + z * z);
			else
				pitchAngle = Nu3D::Math::CartesianToFixedAngle(y * y, x * x + z * z);

			int32_t pitchDelta = -(pitch + pitchAngle) & 0xFFF;

			if (pitchDelta >= 2048)
			{
				int32_t step = (4096 - pitchDelta) >> 3;

				if (step >= maxTurn)
					levelSelectCam->angles.pitch = pitch - maxTurn;
				else
					levelSelectCam->angles.pitch = pitch - step;
			}
			else
			{
				int32_t step = pitchDelta >> 3;

				if (step >= maxTurn)
					levelSelectCam->angles.pitch = pitch + maxTurn;
				else
					levelSelectCam->angles.pitch = step + pitch;
			}
		}

		// FUNCTION: TOY2 0x00438790
		void ApplyWallRepulsion(LevelSelectCamera* levelSelectCam, Vector3I* velocity, int32_t recordType)
		{
			// Pushes the camera away from walls, every pair of points in the record is a wall the camera can't go through

			Levels::RecordData* record = Levels::g_recordData[recordType];

			Vector3I localVec;
			localVec.y = 0;

			int32_t index = 0;
			int32_t processed = 0;

			if (! record->recordCount)
				return;

			do
			{
				int32_t camY = levelSelectCam->pos.y / 32;

				// only walls that are around the height of the camera
				if (camY < record->data[index].y && camY > record->data[index + 1].y)
				{
					int32_t wallZ = record->data[index].z;
					int32_t wallDeltaZ = (record->data[index + 1].z - wallZ) >> 2;
					int32_t wallDeltaX = (record->data[index + 1].x - record->data[index].x) >> 2;

					int32_t wallLengthSq = wallDeltaX * wallDeltaX + wallDeltaZ * wallDeltaZ;

					int32_t camZ = levelSelectCam->pos.z / 32;
					int32_t camDeltaX = (levelSelectCam->pos.x / 32 - record->data[index].x) >> 2;
					int32_t camDeltaZ = (camZ - wallZ) >> 2;

					int32_t distanceSq = camDeltaX * camDeltaX + camDeltaZ * camDeltaZ;

					if (distanceSq < wallLengthSq)
					{
						int32_t offsetX = levelSelectCam->pos.x / 32 - record->data[index].x;
						int32_t offsetZ = camZ - record->data[index].z;

						// scale it down until it fits in the fixed point normalise
						while (abs(offsetX) > 0x4000 || abs(offsetZ) > 0x4000)
						{
							offsetX >>= 1;
							offsetZ >>= 1;
						}

						localVec.x = offsetX;
						localVec.z = offsetZ;

						Nu3D::Math::NormalizeToFixedPoint(&localVec, &localVec);

						// the closer to the wall, the harder it pushes (up to a limit)
						int32_t strength = 2048 - (distanceSq << 11) / wallLengthSq;

						if (strength > 384)
							strength = 384;

						velocity->x = ((strength * localVec.x) >> 12) + velocity->x;
						velocity->z = ((strength * localVec.z) >> 12) + velocity->z;
					}
				}

				record = Levels::g_recordData[recordType];
				index += 2;
				processed += 2;
			} while (processed < record->recordCount);
		}

		// FUNCTION: TOY2 0x00494130
		void DrawArrows()
		{
			// the arrows zoom in from the edges of the screen, over 55 frames
			double progress = Renderer::g_frameDelta + g_arrowZoomProgress;

			if (progress <= 55.0)
				g_arrowZoomProgress = progress;
			else
				g_arrowZoomProgress = 55.0;

			float doubleProgress = g_arrowZoomProgress + g_arrowZoomProgress;
			float invHeight = 1.0 / (384.0 - doubleProgress);
			float invWidth = 1.0 / (512.0 - doubleProgress);

			RGBA white;
			white.value = 0xFFFFFFFF;

			// 7 rects for the left arrows (texture 32), then 7 for the right arrows (texture 33)
			for (int32_t texture = 0; texture < 2; texture++)
			{
				int32_t textureDataIndex = NGNLoader::GetTextureDataIndex(32 + texture);

				for (int32_t i = 0; i < 7; i++)
				{
					float* rect = &g_arrowRects[(texture * 7 + i) * 4];

					float width = invWidth * (rect[2] - rect[0] + 1.0);
					float height = invHeight * (rect[3] - rect[1] + 1.0);

					Renderer::Sprite::Queue2DSprite(
						(rect[0] - g_arrowZoomProgress) * invWidth,
						(rect[1] - g_arrowZoomProgress) * invHeight,
						width,
						height,
						&g_arrowUVs[i * 2],
						&g_arrowUVs[i * 2 + 1],
						textureDataIndex,
						white,
						Renderer::RENDER_ZWRITE | Renderer::RENDER_CULL_NONE | Renderer::RENDER_ALPHA_DEFAULT);
				}
			}
		}

		// FUNCTION: TOY2 0x00438A50
		int32_t Tick()
		{
			g_unusedColour.r = 128;
			g_unusedColour.g = 128;
			g_unusedColour.b = 128;

			for (int32_t showLinkIdx = 0; showLinkIdx < 100; ++showLinkIdx)
				Nu3D::Link::SetScaleFromFixedOffsets(showLinkIdx, 4096, 4096, 4096);

			for (int32_t hideLinkIdx = 100; hideLinkIdx < 367; ++hideLinkIdx)
				Nu3D::Link::SetScaleFromFixedOffsets(hideLinkIdx, 0, 0, 0);

			LevelSelect::g_arrowZoomProgress = 0.0;

			LevelSelectPathData pathData;
			memset(&pathData, 0, sizeof(pathData));

			pathData.pathProgress[0] = 12800;
			pathData.pathProgress[1] = 25600;
			pathData.pathProgress[2] = 36864;
			pathData.pathProgress[3] = 23808;
			pathData.pathProgress[4] = 19712;
			pathData.pathProgress[5] = 17408;
			pathData.pathProgress[6] = 11264;
			pathData.pathProgress[7] = 4096;
			pathData.pathProgress[8] = 2560;
			pathData.pathProgress[9] = 35584;
			pathData.pathProgress[10] = 28160;
			pathData.pathProgress[11] = 14336;
			pathData.pathProgress[12] = 4096;

			Vector3I velocity;
			memset(&velocity, 0, sizeof(velocity));

			g_randDatBufferPtr = g_randDatBuffer;

			int32_t animTimer = 0;
			uint32_t tokenProgress = Toy2::ComputeTokenProgress();

			int32_t maxUnlockedLevel = tokenProgress & 0xFF;
			int32_t tokensHeld = (tokenProgress >> 16) & 0xFF;
			int32_t tokensNeeded = (tokenProgress >> 8) & 0xFF;

			if (tokenProgress > 14u)
				maxUnlockedLevel = 14;

			int32_t sceneTimer = 0;
			int32_t subPointTimer = 3;

			LevelLinkPair* links = &g_levelSelectLinks[0];
			uint32_t linkPairCount = (2 * maxUnlockedLevel + 2) >> 1;

			do
			{
				int16_t curHideLink = *links->linksToHide;

				if (curHideLink != -1)
				{
					int32_t hideIdx = 0;

					do
					{
						Nu3D::Link::SetScaleFromFixedOffsets(curHideLink, 0, 0, 0);

						curHideLink = links->linksToHide[++hideIdx];
					} while (curHideLink != -1);
				}

				int16_t curShowLink = *links->linksToShow;

				if (curShowLink != -1)
				{
					int32_t showIdx = 0;

					do
					{
						Nu3D::Link::SetScaleFromFixedOffsets(curShowLink, 4096, 4096, 4096);

						curShowLink = links->linksToShow[++showIdx];
					} while (curShowLink != -1);
				}

				++links;
				--linkPairCount;
			} while (linkPairCount);

			int32_t startCursor = 0;
			int32_t curLevel = Toy2::g_levelIndex + 1;
			int32_t maxLevel = maxUnlockedLevel + 1;

			LevelSelectCamera levelSelectCam;
			levelSelectCam.angles.pitch = 0;
			levelSelectCam.angles.yaw = 0;

			int16_t cameraRoll = 0;

			int32_t pendingLevel = curLevel;
			int32_t maxLevelCursor = maxLevel;

			if (curLevel <= maxLevel)
			{
				startCursor = g_levelSelectCursor;
			}
			else
			{
				curLevel = maxLevel;
				g_levelSelectCursor = 0;
				pendingLevel = maxLevel;
			}

			int32_t startX = Toy2::Levels::g_recordData[2]->data[3 * startCursor + 1].x;
			Toy2::Levels::RecordData* startRecord = Toy2::Levels::g_recordData[2] + 9 * startCursor;

			Nu3D::Camera::ActiveCameraTransform cameraTransform;

			levelSelectCam.pos.x = 32 * startX;
			cameraTransform.pos.x = 32 * startX;

			levelSelectCam.pos.y = 32 * startRecord->data[1].y;
			cameraTransform.pos.y = levelSelectCam.pos.y;

			levelSelectCam.pos.z = 32 * startRecord->data[1].z;
			cameraTransform.pos.z = levelSelectCam.pos.z;

			int32_t initDirX = Toy2::Levels::g_recordData[1]->data[3 * curLevel + 1].x - Toy2::Levels::g_recordData[2]->data[3 * curLevel + 1].x;
			levelSelectCam.lookDir.x = initDirX;

			int32_t initDirY = Toy2::Levels::g_recordData[1]->data[3 * curLevel + 1].y - Toy2::Levels::g_recordData[2]->data[3 * curLevel + 1].y;
			levelSelectCam.lookDir.y = initDirY;

			int32_t initDirZ = Toy2::Levels::g_recordData[1]->data[3 * curLevel + 1].z - Toy2::Levels::g_recordData[2]->data[3 * curLevel + 1].z;
			levelSelectCam.lookDir.z = initDirZ;

			if (curLevel < maxLevel && ! SaveManager::g_curLevelTokenData && SaveManager::g_save0Data.tokens[Toy2::g_levelFileConversion[Toy2::g_levelIndex]])
				pendingLevel = ++curLevel;

			int32_t smoothDirZ = initDirZ;
			int32_t smoothDirX = initDirX;
			int32_t smoothDirY = initDirY;

			int32_t approachBlend = 0;
			int32_t needTokensTimer = 0;
			int32_t wasCancelled = 0;
			int32_t transitionTimer = 0;
			uint32_t frameCounter20 = 0;
			int32_t subPointIdx = 1;
			int32_t inputHoldTimer = 0;
			int32_t atTarget = 0;

			AudioManager::PlayMusicLooping(19);
			Renderer::g_frameDelta = 1;

			Toy2::LevelSelect::TurnTowardLookDir(&levelSelectCam, 16);
			Nu3D::Camera::SetTint(128, 128, 128, 6);

			do
			{
				Toy2::Levels::RecordData* soundRecord = Toy2::Levels::g_recordData[1] + 9 * curLevel;

				Vector3I soundPos;
				soundPos.x = 32 * soundRecord->data[1].x;
				soundPos.y = 32 * soundRecord->data[1].y;
				soundPos.z = 32 * soundRecord->data[1].z;

				AudioManager::PlayLoopingSound3DPositional(&soundPos, curLevel + 6, 5120, 96, &soundPos, 0);

				uint8_t tokenBits = SaveManager::g_save0Data.tokens[curLevel];

				sceneTimer += Renderer::g_frameDelta;
				animTimer += Renderer::g_frameDelta;

				int32_t tokenIconCount = (tokenBits & 1) != 0;

				if ((tokenBits & 2) != 0)
					++tokenIconCount;

				if ((tokenBits & 4) != 0)
					++tokenIconCount;

				if ((tokenBits & 8) != 0)
					++tokenIconCount;

				if ((tokenBits & 16) != 0)
					++tokenIconCount;

				int32_t pathObj1 = 87;
				int32_t* pathDataPtr = &pathData.pathProgress[0];

				do
				{
					Nu3D::Link::FollowWaypointPath(pathObj1++, 7, pathDataPtr++);
				} while (pathObj1 < 89);

				int32_t pathObj2 = 89;
				int32_t* pathProgress2 = &pathData.pathProgress[2];

				do
				{
					Nu3D::Link::FollowWaypointPath(pathObj2++, 8, pathProgress2++);
				} while (pathObj2 < 96);

				int32_t pathObj3 = 96;
				int32_t* pathProgres3 = &pathData.pathProgress[9];

				do
				{
					Nu3D::Link::FollowWaypointPath(pathObj3++, 9, pathProgres3++);
				} while (pathObj3 < 100);

				int32_t levelPointBase = 3 * curLevel;
				int32_t targetPointIdx = subPointIdx + 3 * curLevel;

				Toy2::Levels::RecordData* targetRecord = Toy2::Levels::g_recordData[2] + 3 * targetPointIdx;

				Vector3I targetDelta;

				int32_t deltaX = 32 * targetRecord->data[0].x + smoothDirX * approachBlend / 1024 - levelSelectCam.pos.x;
				targetDelta.x = deltaX;

				int32_t deltaY = 32 * targetRecord->data[0].y + smoothDirY * approachBlend / 1024 - levelSelectCam.pos.y;
				targetDelta.y = deltaY;

				int32_t deltaZ = smoothDirZ * approachBlend / 1024 + 32 * Toy2::Levels::g_recordData[2]->data[targetPointIdx].z - levelSelectCam.pos.z;
				targetDelta.z = deltaZ;

				int32_t distSquared = deltaZ / 128 * (deltaZ / 128) + deltaY / 128 * (deltaY / 128) + deltaX / 128 * (deltaX / 128);

				while (abs(deltaX) > 0x4000 || abs(deltaY) > 0x4000 || abs(deltaZ) > 0x4000)
				{
					deltaX >>= 1;
					deltaY >>= 1;
					deltaZ >>= 1;
				}

				targetDelta.z = deltaZ;
				targetDelta.y = deltaY;
				targetDelta.x = deltaX;

				if (deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ <= 0x4000000)
				{
					atTarget = 1;
					subPointTimer += Renderer::g_frameDelta;

					if (subPointTimer < 3u)
					{
						subPointTimer |= 3u;
						int32_t randSubPoint = *g_randDatBufferPtr++ % 3;
						subPointIdx = randSubPoint;
					}

					targetDelta.x = (Numerics::g_sinCosLUT[(19 * sceneTimer) & 0xFFF] >> 5) + deltaX;
					targetDelta.y = (Numerics::g_sinCosLUT[(23 * sceneTimer) & 0xFFF] >> 4) + deltaY;

					int32_t wobbleZ = Numerics::g_sinCosLUT[(29 * sceneTimer) & 0xFFF] >> 5;

					velocity.x = ((int32_t)Renderer::g_frameDelta * targetDelta.x) >> 4;
					targetDelta.z = wobbleZ + deltaZ;
					velocity.y = ((int32_t)Renderer::g_frameDelta * targetDelta.y) >> 4;
					velocity.z = ((int32_t)Renderer::g_frameDelta * (wobbleZ + deltaZ)) >> 4;
				}
				else
				{
					Nu3D::Math::NormalizeToFixedPoint(&targetDelta, &targetDelta);

					subPointTimer = 3;
					velocity.x += ((int32_t)Renderer::g_frameDelta * ((targetDelta.x >> 1) - velocity.x)) >> 5;
					velocity.y += ((int32_t)Renderer::g_frameDelta * ((targetDelta.y >> 1) - velocity.y)) >> 4;
					velocity.z += ((int32_t)Renderer::g_frameDelta * ((targetDelta.z >> 1) - velocity.z)) >> 5;
				}

				Toy2::LevelSelect::ApplyWallRepulsion(&levelSelectCam, &velocity, 3);

				levelSelectCam.pos.x += velocity.x;
				levelSelectCam.pos.y += velocity.y;
				levelSelectCam.pos.z += velocity.z;

				int32_t lookPointIdx = levelPointBase + subPointIdx;

				levelSelectCam.lookDir.x = Toy2::Levels::g_recordData[1]->data[lookPointIdx].x - Toy2::Levels::g_recordData[2]->data[lookPointIdx].x;
				levelSelectCam.lookDir.y = Toy2::Levels::g_recordData[1]->data[lookPointIdx].y - Toy2::Levels::g_recordData[2]->data[lookPointIdx].y;
				levelSelectCam.lookDir.z =
					Toy2::Levels::g_recordData[1]->data[levelPointBase + subPointIdx].z - Toy2::Levels::g_recordData[2]->data[levelPointBase + subPointIdx].z;

				smoothDirX += (levelSelectCam.lookDir.x - smoothDirX) >> 4;
				smoothDirY += (levelSelectCam.lookDir.y - smoothDirY) >> 4;
				smoothDirZ += (levelSelectCam.lookDir.z - smoothDirZ) >> 4;

				if (atTarget)
				{
					approachBlend += 96 * Renderer::g_frameDelta;

					if (approachBlend > 14000)
						approachBlend = 14000;
				}
				else
				{
					approachBlend += -256 * Renderer::g_frameDelta;

					if (approachBlend < 0)
						approachBlend = 0;
				}

				Toy2::LevelSelect::TurnTowardLookDir(&levelSelectCam, 0x2000 / (sqrt(distSquared) + 256));

				cameraTransform.pos.x += (levelSelectCam.pos.x - cameraTransform.pos.x) >> 1;
				cameraTransform.pos.y += (levelSelectCam.pos.y - cameraTransform.pos.y) >> 1;
				cameraTransform.angles = levelSelectCam.angles;
				cameraTransform.pos.z += (levelSelectCam.pos.z - cameraTransform.pos.z) >> 1;
				cameraTransform.roll = cameraRoll;

				Nu3D::Camera::ApplyTransformToCamera(&cameraTransform);
				Nu3D::Camera::FadeToTargetTint();

				if (g_randDatBufferPtr > &g_randDatBuffer[1500])
					g_randDatBufferPtr -= 1500;

				int16_t curButtons = InputManager::g_curButtonsPressed;
				int16_t prevButtons = InputManager::g_prevButtonsPressed;

				if (InputManager::g_curButtonsPressed == InputManager::g_prevButtonsPressed)
				{
					int32_t repeatTimer = Renderer::g_frameDelta + inputHoldTimer;
					inputHoldTimer += Renderer::g_frameDelta;

					if (inputHoldTimer > 32 && (repeatTimer & 15) < 3)
					{
						prevButtons = 0;
						repeatTimer |= 3;

						InputManager::g_prevButtonsPressed = 0;
						inputHoldTimer = repeatTimer;
					}
				}
				else
				{
					inputHoldTimer = 0;
				}

				if (++frameCounter20 > 20)
					frameCounter20 = 0;

				if ((InputManager::g_curButtonsPressed & INPUT_JUMP) == 0 || (prevButtons & INPUT_JUMP) != 0 || transitionTimer)
				{
					curLevel = pendingLevel;
				}
				else
				{
					curLevel = pendingLevel;

					if (sceneTimer > 60)
					{
						if (pendingLevel == maxLevelCursor && tokensHeld < tokensNeeded)
						{
							AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);

							curButtons = InputManager::g_curButtonsPressed;
							prevButtons = (prevButtons & ~0xFF) | (InputManager::g_prevButtonsPressed & 0xFF);
							needTokensTimer = 192;
						}
						else
						{
							transitionTimer = 1;

							Nu3D::Camera::SetTint(0, 0, 0, 6u);
							AudioManager::PlayOneShotSoundGlobal(0, 4608, 80, 80);

							curButtons = InputManager::g_curButtonsPressed;
							prevButtons = (prevButtons & ~0xFF) | (InputManager::g_prevButtonsPressed & 0xFF);
						}
					}
				}

				int32_t nextTransition;
				bool fadeOutwards;

				if ((curButtons & INPUT_CANCEL) != 0)
				{
					fadeOutwards = transitionTimer <= 0;

					if (transitionTimer)
						goto LBL_ADVANCE_TRANSITION;

					if (sceneTimer > 60)
					{
						Nu3D::Camera::SetTint(0, 0, 0, 6u);
						AudioManager::PlayOneShotSoundGlobal(2, 4608, 80, 80);

						wasCancelled = 1;
						nextTransition = 2;
					LBL_STORE_TRANSITION:

						transitionTimer = nextTransition;
						goto LBL_DRAW_FRAME;
					}
				}
				else
				{
					fadeOutwards = transitionTimer <= 0;
					if (transitionTimer)
					{
					LBL_ADVANCE_TRANSITION:
						if (fadeOutwards)
							nextTransition = transitionTimer - 1;
						else
							nextTransition = transitionTimer + 1;

						goto LBL_STORE_TRANSITION;
					}
				}

				if ((curButtons & INPUT_RIGHT) != 0 && (prevButtons & INPUT_RIGHT) == 0 && curLevel < maxLevelCursor)
				{
					++curLevel;
				LBL_SELECTION_CHANGED:

					pendingLevel = curLevel;
					atTarget = 0;

					AudioManager::PlayOneShotSoundGlobal(1, 4608, 80, 80);
					goto LBL_DRAW_FRAME;
				}

				if ((curButtons & INPUT_LEFT) != 0 && (prevButtons & INPUT_LEFT) == 0 && curLevel > 1)
				{
					--curLevel;
					goto LBL_SELECTION_CHANGED;
				}
			LBL_DRAW_FRAME:

				Toy2::RenderGame(1);

				for (int32_t tokenIconIdx = 0; tokenIconIdx < tokenIconCount; ++tokenIconIdx)
					Renderer::Sprite::DrawScaled(
						19 * (2 * tokenIconIdx - tokenIconCount) + 256, 50, 54, ((animTimer / 2) + 4 * tokenIconIdx + 4) & 31, 128, 128, 128, 96, 3276, 2048);

				int32_t warnTimercopy = needTokensTimer;
				int32_t tokensHeldCopy;

				if (needTokensTimer <= 0)
					goto LBL_DRAW_TOKEN_COUNTER;

				if ((needTokensTimer & 63) <= 0x18)
				{
					warnTimercopy = needTokensTimer - Renderer::g_frameDelta;
					needTokensTimer -= Renderer::g_frameDelta;
				LBL_DRAW_TOKEN_COUNTER:

					Renderer::Sprite::DrawTiledFixed(14, 192, 54, (animTimer / 2) & 31);

					tokensHeldCopy = tokensHeld;

					Renderer::Sprite::DrawTile(102, 208, 58, tokensHeld / 10 % 10);
					Renderer::Sprite::DrawTile(115, 208, 58, tokensHeld % 10);
					goto LBL_DRAW_LEVEL_GATE;
				}

				Renderer::Sprite::DrawColouredFixed(14, 192, 54, (animTimer / 2) & 31, 128, 32, 32);

				tokensHeldCopy = tokensHeld;

				Renderer::Sprite::DrawColoured(102, 208, 58, tokensHeld / 10 % 10, 128, 32, 32);
				Renderer::Sprite::DrawColoured(115, 208, 58, tokensHeld % 10, 128, 32, 32);

				warnTimercopy = needTokensTimer - Renderer::g_frameDelta;
				needTokensTimer -= Renderer::g_frameDelta;
			LBL_DRAW_LEVEL_GATE:

				uint8_t blinkTimer;
				int32_t tokensNeededCopy;

				if (curLevel == maxLevelCursor && (tokensNeededCopy = tokensNeeded, tokensHeldCopy < tokensNeeded))
				{
					blinkTimer = sceneTimer;

					if ((sceneTimer & 63u) < 48)
					{
						if (tokensNeeded >= 10)
						{
							Renderer::Sprite::DrawScaled(232, 109, 58, tokensNeeded / 10 % 10, 128, 128, 128, 96, 6000, 5500);
							Renderer::Sprite::DrawScaled(256, 109, 58, tokensNeededCopy % 10, 128, 128, 128, 96, 6000, 5500);
						}
						else
						{
							Renderer::Sprite::DrawScaled(244, 109, 58, tokensNeeded % 10, 128, 128, 128, 96, 6172, 6172);
						}
					}
					Renderer::Sprite::DrawTiledFixed(112, 88, 55, 0);
				}
				else
				{
					blinkTimer = sceneTimer;
				}

				if (curLevel > 1 && (blinkTimer & 63) < 48)
					Renderer::Sprite::DrawTile(32, 112, 57, 0);

				if (curLevel < maxLevelCursor && (blinkTimer & 63) < 48)
					Renderer::Sprite::DrawTile(448, 112, 57, 1);

				Toy2::LevelSelect::DrawArrows();

				Nullsub3();

				if (warnTimercopy > 0 && (warnTimercopy & 63) > 24)
					Renderer::Sprite::DrawWhiteText("you need more tokens", 154, 256);

				Renderer::Sprite::DrawWhiteText(g_levelNames[curLevel], 32, 256);

				int32_t textLen1 = 0;

				if (g_jumpToSelectTxt[0])
				{
					while (g_jumpToSelectTxt[++textLen1])
						;
				}

				int32_t textLen2 = 0;

				if (g_jumpToSelectTxt[0])
				{
					while (g_jumpToSelectTxt[++textLen2])
						;
				}

				if (textLen2 > textLen1)
					textLen1 = textLen2;

				int32_t textXPos = 13 * textLen1 - 475;

				Renderer::Sprite::DrawWhiteText(g_jumpToSelectTxt, 190, textXPos);
				Renderer::Sprite::DrawWhiteText("cancel to go back", 208, textXPos);

				Nullsub6();

			} while (abs(transitionTimer) < 28);

			g_levelSelectCursor = curLevel;
			Toy2::g_levelIndex = curLevel - 1;

			AudioManager::FlushSoundVoices();
			AudioManager::StopAndWait();

			return wasCancelled;
		}

	}
}

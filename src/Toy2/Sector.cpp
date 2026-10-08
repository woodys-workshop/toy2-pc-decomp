#include "Toy2/Sector.h"
#include "Toy2/Toy2.h"
#include "Toy2/Levels.h"
#include "Toy2/Portal.h"
#include "Toy2/Collision.h"
#include "Toy2/Camera.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Math.h"
#include "Numerics.h"

namespace Toy2
{
	namespace Sector
	{
		// GLOBAL: TOY2 0x0054D920
		Vector3I g_unk54D920;

		// GLOBAL: TOY2 0x0054DD74
		int32_t g_unk54DD74;

		// FUNCTION: TOY2 0x004402B0
		void UpdateLighting()
		{
			uint32_t* mathScratch = (uint32_t*)Collision::g_mathScratch;

			// the 3x3 matrices that are stored in the scratch buffer, the camera matrix and the one of the object view
			int16_t* viewMatrix = (int16_t*)&mathScratch[229];
			int16_t* cameraMatrix = (int16_t*)&mathScratch[245];

			g_buzzActor.posAngles.pos.y -= 0x2000;

			if (g_curSectorCopy2 == -1 || g_levelFileIndex == 2)
				g_curSectorCopy2 = Levels::GetSectorAtPosition(&g_buzzActor.posAngles.pos);
			else
				Portal::UpdateActiveSector();

			g_unk54D920.x = g_buzzActor.posAngles.pos.x;
			g_unk54D920.y = g_buzzActor.posAngles.pos.y;
			g_unk54D920.z = g_buzzActor.posAngles.pos.z;

			g_buzzActor.posAngles.pos.y += 0x2000;

			int32_t movedToSectorOnly = 1;
			int32_t sectorAtCamera = Levels::GetSectorAtPosition((Vector3I*)&mathScratch[221]);
			int32_t sector;

			if (g_curSectorCopy2 == sectorAtCamera || Camera::g_cutsceneFrameTimer)
			{
				sector = sectorAtCamera;
				goto LABEL_16;
			}

			if (sectorAtCamera < 0 || g_curSectorCopy2 < 0)
				goto LABEL_15;

			// only change to the sector of the camera when the sector of Buzz has a portal to it
			{
				Levels::PortalEntry* portal = Levels::g_portalEntries[g_curSectorCopy2].entries;

				if (portal->recordIdx == 0xFF)
					goto LABEL_15;

				sector = g_curSectorCopy1;

				do
				{
					if (portal->categoryIdx == sectorAtCamera)
					{
						sector = sectorAtCamera;
						movedToSectorOnly = 0;
						g_curSectorCopy1 = sectorAtCamera;
					}

					uint8_t nextRecordIdx = portal[1].recordIdx;
					++portal;

					if (nextRecordIdx == 0xFF)
						break;
				} while (1);
			}

			if (movedToSectorOnly)
			{
			LABEL_15:
				sector = g_curSectorCopy2;
			LABEL_16:
				g_curSectorCopy1 = sector;
			}

			if (sector == 255)
			{
				sector = 1;
				g_curSectorCopy1 = 1;
			}

			if (g_levelFileIndex != 2)
				goto LABEL_29;

			if (sector == 3)
			{
				if ((int32_t)mathScratch[222] >= -104000)
					goto LABEL_29;
			}
			else if ((int32_t)mathScratch[222] >= -74000)
			{
				goto LABEL_29;
			}

			// the camera is below the level, everything in the sector is visible
			{
				int32_t count = Levels::g_unk5574FC;

				if (count > 0)
				{
					g_unk54DD74 = 0;

					for (int32_t i = 0; i < count; i++)
					{
						Levels::g_datContents[sector].unkByte1 = 0xFF;
						Levels::g_datContents[i].unkByte1 = 0xFF;
						Levels::g_datContents[sector].unkShort1 = 0;
						Levels::g_datContents[i].unkShort1 = 0;
						Levels::g_datContents[sector].unkShort2 = 0;
						Levels::g_datContents[i].unkShort2 = 0;
						Levels::g_datContents[sector].unkShort3 = 512;
						Levels::g_datContents[i].unkShort3 = 512;
						Levels::g_datContents[sector].unkShort4 = 256;
						Levels::g_datContents[i].unkShort4 = 256;
					}
				}
			}

			goto LABEL_33;

		LABEL_29: {
			int32_t count = Levels::g_unk5574FC;

			for (int32_t i = 0; i < count; i++)
			{
				Levels::g_datContents[i].unkByte1 = 0;
				Levels::g_datContents[i].unkByte2 = 0;
			}

			int32_t cameraX = (int32_t)mathScratch[225];
			int32_t cameraY = (int32_t)mathScratch[226];
			int32_t cameraZ = (int32_t)mathScratch[227];

			Levels::g_datContents[sector].unkByte1 = 0xFF;
			Levels::g_datContents[0].unkByte1 = 0xFF;
			Levels::g_datContents[sector].unkShort1 = 0;
			Levels::g_datContents[0].unkShort1 = 0;
			Levels::g_datContents[sector].unkShort2 = 0;
			Levels::g_datContents[0].unkShort2 = 0;
			Levels::g_datContents[sector].unkShort3 = 512;
			Levels::g_datContents[0].unkShort3 = 512;
			Levels::g_datContents[sector].unkShort4 = 256;
			Levels::g_datContents[0].unkShort4 = 256;

			g_unk54DD74 = 0;

			mathScratch[234] = -((viewMatrix[0] * cameraX + viewMatrix[1] * cameraY + viewMatrix[2] * cameraZ) / 16384);
			mathScratch[235] = -((viewMatrix[3] * cameraX + viewMatrix[4] * cameraY + viewMatrix[5] * cameraZ) / 16384);
			mathScratch[236] = -((viewMatrix[6] * cameraX + viewMatrix[7] * cameraY + viewMatrix[8] * cameraZ) / 16384);

			Nu3D::Camera::SetObjectViewPosition((Nu3D::Camera::NineWords*)&mathScratch[229]);
			Nu3D::Camera::SetObjectViewMatrix((Matrix3x3I16*)&mathScratch[229]);

			Portal::FloodVisibility(Levels::g_portalEntries[g_curSectorCopy1].entries, 0, 512, 0, 256, 254);
		}

		LABEL_33:
			// the original passes the camera angles and the roll copy that follows them in memory
			AnglesWithRoll cameraAngles;

			cameraAngles.angles = Nu3D::Camera::g_cameraAngles;
			cameraAngles.roll = Nu3D::Camera::g_cameraRollCopy;
			cameraAngles.unkRoll = 0;

			Nu3D::Math::EulerToRotationMatrix(&cameraAngles, (Matrix3x3I16*)cameraMatrix);

			int32_t index = 0;

			if (Levels::g_unk5574FC > 0)
			{
				Levels::DatContents* contents = Levels::g_datContents;

				do
				{
					if (contents->unkByte1)
					{
						if ((int16_t)contents->unkShort1 >= 3 || (int16_t)contents->unkShort3 <= 509 || (int16_t)contents->unkShort2 >= 3
							|| (int16_t)contents->unkShort4 <= 253)
						{
							int16_t* scratchShort = (int16_t*)mathScratch;

							mathScratch[129] = (int16_t)contents->unkShort2 - 128;
							mathScratch[128] = 320 * ((int16_t)contents->unkShort1 - 256) / 512;

							mathScratch[130] = 160;
							mathScratch[134] = 160;

							mathScratch[132] = 320 * ((int16_t)contents->unkShort3 - 256) / 512;
							mathScratch[133] = (int16_t)contents->unkShort4 - 128;

							Nu3D::Math::NormalizeToFixedPoint((Vector3I*)&mathScratch[128], (Vector3I*)&mathScratch[136]);
							Nu3D::Math::NormalizeToFixedPoint((Vector3I*)&mathScratch[132], (Vector3I*)&mathScratch[140]);

							int32_t sumX = (int32_t)mathScratch[136] + (int32_t)mathScratch[140];
							int32_t sumY = (int32_t)mathScratch[137] + (int32_t)mathScratch[141];
							int32_t sumZ = (int32_t)mathScratch[138] + (int32_t)mathScratch[142];

							mathScratch[136] = sumX;
							mathScratch[137] = sumY;
							mathScratch[138] = sumZ;

							int32_t rotatedX = (cameraMatrix[6] * sumZ + cameraMatrix[3] * sumY + cameraMatrix[0] * sumX) / 4096;
							int32_t rotatedY = (cameraMatrix[4] * sumY + cameraMatrix[7] * sumZ + cameraMatrix[1] * sumX) / 4096;
							int32_t rotatedZ = (cameraMatrix[5] * sumY + cameraMatrix[8] * sumZ + cameraMatrix[2] * sumX) / 4096;

							mathScratch[251] = rotatedY;
							mathScratch[250] = rotatedX;
							mathScratch[242] = rotatedX;
							mathScratch[252] = rotatedZ;

							mathScratch[243] = rotatedY;
							mathScratch[244] = rotatedZ;

							scratchShort[141 * 2] = 0;

							int16_t yaw = -Nu3D::Math::CartesianToFixedAngle(rotatedX, rotatedZ);
							scratchShort[140 * 2 + 1] = yaw;

							int32_t sine = Numerics::g_sinCosLUT[yaw & 0xFFF];
							int32_t cosine = Numerics::g_sinCosLUT[(yaw + 1024) & 0xFFF];

							int32_t rotated = cosine * (int32_t)mathScratch[242] + sine * (int32_t)mathScratch[244];
							mathScratch[244] = (cosine * (int32_t)mathScratch[244] - sine * (int32_t)mathScratch[242]) >> 14;
							mathScratch[242] = rotated >> 14;

							scratchShort[140 * 2] = Nu3D::Math::CartesianToFixedAngle(mathScratch[243], mathScratch[244]);

							int16_t angleA;
							int16_t angleB;

							angleA = Nu3D::Math::CartesianToFixedAngle(mathScratch[132], 160);
							angleB = Nu3D::Math::CartesianToFixedAngle(mathScratch[128], 160);
							contents->unkShort5 = (3 * Numerics::g_sinCosLUT[(angleA - angleB) & 0xFFF]) >> 2;

							angleA = Nu3D::Math::CartesianToFixedAngle(mathScratch[133], 160);
							angleB = Nu3D::Math::CartesianToFixedAngle(mathScratch[129], 160);
							contents->unkShort6 = (3 * Numerics::g_sinCosLUT[(angleA - angleB) & 0xFFF]) >> 2;

							angleA = Nu3D::Math::CartesianToFixedAngle(mathScratch[132], 160);
							contents->unkShort5 += angleA - Nu3D::Math::CartesianToFixedAngle(mathScratch[128], 160);

							angleA = Nu3D::Math::CartesianToFixedAngle(mathScratch[133], 160);
							contents->unkShort6 += angleA - Nu3D::Math::CartesianToFixedAngle(mathScratch[129], 160);

							contents->unkShort5 = (160 * (int16_t)contents->unkShort5) / 160;
							contents->unkShort6 = ((int16_t)contents->unkShort6 << 7) / 160;
							contents->unkByte4 = 1;

							Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&mathScratch[140], &contents->unkVec);
						}
						else
						{
							contents->unkByte4 = 0;
						}
					}

					++contents;
					++index;
				} while (index < Levels::g_unk5574FC);
			}

			if (Levels::g_hasZoneData)
			{
				Levels::g_datContents[g_curSectorCopy1].unkShort1 = (int16_t)Nu3D::Camera::g_cameraUnk_554DC0;
				Levels::g_datContents[g_curSectorCopy1].unkShort2 = (int16_t)Nu3D::Camera::g_cameraUnk_553028;
				Levels::g_datContents[g_curSectorCopy1].unkShort3 = (int16_t)(Nu3D::Camera::g_cameraUnk_55302C + 512);
				Levels::g_datContents[g_curSectorCopy1].unkShort4 = (int16_t)(Nu3D::Camera::g_cameraUnk_5546B4 + 256);
			}
		}
	}
}

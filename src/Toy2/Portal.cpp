#include "Toy2/Portal.h"
#include "Toy2/Toy2.h"
#include "Toy2/Levels.h"
#include "Toy2/Sector.h"
#include "Toy2/Collision.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Math.h"
#include "Renderer/Shadows.h"

#include <stdlib.h>

namespace Toy2
{
	namespace Portal
	{
		// the screen rectangles of the portals to the backdrop sector
		// GLOBAL: TOY2 0x00557BB0
		BackdropRect g_backdropRects[12];

		// FUNCTION: TOY2 0x0043F3D0
		void FloodVisibility(Toy2::Levels::PortalEntry* a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6)
		{
			int32_t* scratch = (int32_t*)Collision::g_mathScratch;
			int16_t* scratchShort = (int16_t*)Collision::g_mathScratch;

			Levels::PortalEntry* portal = a1;
			uint8_t recordIdx = a1->recordIdx;

			while (recordIdx != 0xFF)
			{
				Levels::PortalQuad* quad = (Levels::PortalQuad*)Levels::g_recordData[recordIdx];

				if (quad && Levels::g_datContents[portal->categoryIdx].unkByte1 < a6)
				{
					// the original passes two more pointers to ProjectQuad, one of them is read back as the flags below and it is never written
					int32_t flags;

					Nu3D::Camera::ProjectQuad(
						(Vector3I*)&quad->p0x,
						(Vector3I*)&quad->p1x,
						(Vector3I*)&quad->p3x,
						(Vector3I*)&quad->p2x,
						(int32_t)&scratch[0],
						(int32_t)&scratch[1],
						(int32_t)&scratch[2],
						(int32_t)&scratch[3]);

#ifdef APPLY_FIXES
					// $TODO: the original tests an uninitialised stack slot here, read it out of the running original
					flags = 0;
#endif

					// is any corner of the quad inside of the rectangle that is visible through the portal
					if ((scratchShort[0] <= a3 || scratchShort[2] <= a3 || scratchShort[4] <= a3 || scratchShort[6] <= a3)
						&& (scratchShort[0] >= a2 || scratchShort[2] >= a2 || scratchShort[4] >= a2 || scratchShort[6] >= a2)
						&& (scratchShort[1] <= a5 || scratchShort[3] <= a5 || scratchShort[5] <= a5 || scratchShort[7] <= a5)
						&& (scratchShort[1] >= a4 || scratchShort[3] >= a4 || scratchShort[5] >= a4 || scratchShort[7] >= a4))
					{
						int32_t clipCount = 0;

						if ((flags & 0x20000) != 0 && (g_levelFileIndex != 7 || portal->categoryIdx != 15))
						{
							Levels::g_datContents[portal->categoryIdx].unkByte2 = 1;

							Nu3D::Camera::WorldToView((Vector3I*)&quad->p0x, (Vector3I*)&scratch[4]);
							Nu3D::Camera::WorldToView((Vector3I*)&quad->p1x, (Vector3I*)&scratch[8]);
							Nu3D::Camera::WorldToView((Vector3I*)&quad->p2x, (Vector3I*)&scratch[12]);
							Nu3D::Camera::WorldToView((Vector3I*)&quad->p3x, (Vector3I*)&scratch[16]);

							int32_t z3 = scratch[18];
							int32_t z0;
							int32_t clipX;
							int32_t clipY;

							// every corner is projected, a corner behind the camera is moved along the edges to the corners that are in front of it
							if (scratch[6] <= 10)
							{
								z0 = scratch[6];

								if (scratch[6] < 0)
									clipCount = 8;

								if (scratch[10] <= 10 || scratch[10] <= scratch[18])
								{
									if (scratch[18] <= 10)
									{
										clipX = scratch[4];
										clipY = scratch[5];
									}
									else
									{
										clipX = scratch[16] + (scratch[18] * (scratch[4] - scratch[16])) / (scratch[18] - scratch[6]);
										clipY = scratch[17] + (scratch[18] * (scratch[5] - scratch[17])) / (scratch[18] - scratch[6]);
									}
								}
								else
								{
									clipX = scratch[8] + (scratch[10] * (scratch[4] - scratch[8])) / (scratch[10] - scratch[6]);
									clipY = scratch[9] + (scratch[10] * (scratch[5] - scratch[9])) / (scratch[10] - scratch[6]);
								}

								if (abs(clipX) < 512 || abs(clipY) < 512)
									++clipCount;

								scratchShort[0] = clipX >= 0 ? 512 : 0;
								scratchShort[1] = clipY >= 0 ? 512 : 0;
							}
							else
							{
								z0 = scratch[6];
								scratchShort[0] = (160 * scratch[4]) / scratch[6] + 256;
								scratchShort[1] = (160 * scratch[5]) / scratch[6] + 128;
							}

							int32_t y2 = scratch[13];

							if (scratch[10] <= 10)
							{
								if (scratch[10] < 0)
									clipCount += 8;

								if (scratch[14] <= 10 || scratch[14] <= z0)
								{
									if (z0 <= 10)
									{
										clipX = scratch[8];
										clipY = scratch[9];
									}
									else
									{
										clipX = scratch[4] + (z0 * (scratch[8] - scratch[4])) / (z0 - scratch[10]);
										y2 = scratch[13];
										clipY = scratch[5] + (z0 * (scratch[9] - scratch[5])) / (z0 - scratch[10]);
									}
								}
								else
								{
									clipX = scratch[12] + (scratch[14] * (scratch[8] - scratch[12])) / (scratch[14] - scratch[10]);
									y2 = scratch[13];
									clipY = scratch[13] + (scratch[14] * (scratch[9] - scratch[13])) / (scratch[14] - scratch[10]);
								}

								if (abs(clipX) < 512 || abs(clipY) < 512)
									++clipCount;

								z3 = scratch[18];
								scratchShort[2] = clipX >= 0 ? 512 : 0;
								scratchShort[3] = clipY >= 0 ? 512 : 0;
							}
							else
							{
								scratchShort[2] = (160 * scratch[8]) / scratch[10] + 256;
								scratchShort[3] = (160 * scratch[9]) / scratch[10] + 128;
							}

							if (scratch[14] <= 10)
							{
								if (scratch[14] < 0)
									clipCount += 8;

								if (z3 <= 10 || z3 <= scratch[10])
								{
									if (scratch[10] <= 10)
									{
										clipX = scratch[12];
										clipY = y2;
									}
									else
									{
										clipX = scratch[8] + (scratch[10] * (scratch[12] - scratch[8])) / (scratch[10] - scratch[14]);
										clipY = scratch[9] + (scratch[10] * (y2 - scratch[9])) / (scratch[10] - scratch[14]);
									}
								}
								else
								{
									clipX = scratch[16] + (scratch[18] * (scratch[12] - scratch[16])) / (scratch[18] - scratch[14]);
									clipY = scratch[17] + (scratch[18] * (y2 - scratch[17])) / (scratch[18] - scratch[14]);
								}

								if (abs(clipX) < 512 || abs(clipY) < 512)
									++clipCount;

								z3 = scratch[18];
								scratchShort[6] = (clipX >= 0 ? 0 : 0xFE00) + 512;
								scratchShort[7] = clipY >= 0 ? 512 : 0;
							}
							else
							{
								scratchShort[6] = (160 * scratch[12]) / scratch[14] + 256;
								scratchShort[7] = (160 * y2) / scratch[14] + 128;
							}

							if (scratch[18] <= 10)
							{
								if (z3 < 0)
									clipCount += 8;

								if (z0 <= 10 || z0 <= scratch[14])
								{
									if (scratch[14] <= 10)
									{
										clipX = scratch[16];
										clipY = scratch[17];
									}
									else
									{
										clipX = scratch[12] + (scratch[14] * (scratch[16] - scratch[12])) / (scratch[14] - z3);
										clipY = y2 + (scratch[14] * (scratch[17] - y2)) / (scratch[14] - z3);
									}
								}
								else
								{
									clipX = scratch[4] + (z0 * (scratch[16] - scratch[4])) / (z0 - z3);
									clipY = scratch[5] + (z0 * (scratch[17] - scratch[5])) / (z0 - z3);
								}

								if (abs(clipX) < 512 || abs(clipY) < 512)
									++clipCount;

								scratchShort[4] = clipX >= 0 ? 512 : 0;
								scratchShort[5] = clipY >= 0 ? 512 : 0;
							}
							else
							{
								scratchShort[4] = (160 * scratch[16]) / z3 + 256;
								scratchShort[5] = (160 * scratch[17]) / z3 + 128;
							}

							portal = a1;
						}

						if (Nu3D::Math::Cross2D(((PointI16*)scratch)[0], ((PointI16*)scratch)[1], ((PointI16*)scratch)[2]) >= 0
							|| Nu3D::Math::Cross2D(((PointI16*)scratch)[1], ((PointI16*)scratch)[3], ((PointI16*)scratch)[2]) >= 0 || clipCount > 0)
						{
							if (clipCount >= 32)
							{
								Levels::g_datContents[portal->categoryIdx].unkByte1 = 0;
							}
							else
							{
								Levels::g_datContents[portal->categoryIdx].unkByte1 = a6;
								Levels::g_datContents[portal->categoryIdx].unkByte3 = portal->recordIdx;

								if ((clipCount & 3) != 0)
								{
									Levels::g_datContents[portal->categoryIdx].unkShort1 = a2;
									Levels::g_datContents[portal->categoryIdx].unkShort2 = a4;
									Levels::g_datContents[portal->categoryIdx].unkShort3 = a3;
									Levels::g_datContents[portal->categoryIdx].unkShort4 = a5;
								}
								else
								{
									// the rectangle that holds all the corners, clamped to the rectangle of the portal before
									int32_t minX = scratchShort[0];
									int32_t minY = scratchShort[1];

									if (scratchShort[0] > scratchShort[2])
										minX = scratchShort[2];

									if (minX > scratchShort[4])
										minX = scratchShort[4];

									if (minX > scratchShort[6])
										minX = scratchShort[6];

									int32_t maxX = scratchShort[2];

									if (scratchShort[0] >= scratchShort[2])
										maxX = scratchShort[0];

									if (maxX < scratchShort[4])
										maxX = scratchShort[4];

									if (maxX < scratchShort[6])
										maxX = scratchShort[6];

									int32_t maxY = scratchShort[3];

									if (scratchShort[1] > scratchShort[3])
										minY = scratchShort[3];

									if (minY > scratchShort[5])
										minY = scratchShort[5];

									if (minY > scratchShort[7])
										minY = scratchShort[7];

									if (scratchShort[1] >= scratchShort[3])
										maxY = scratchShort[1];

									if (maxY < scratchShort[5])
										maxY = scratchShort[5];

									if (maxY < scratchShort[7])
										maxY = scratchShort[7];

									if (minX >= a2)
										Levels::g_datContents[portal->categoryIdx].unkShort1 = minX;
									else
										Levels::g_datContents[portal->categoryIdx].unkShort1 = a2;

									if (minY >= a4)
										Levels::g_datContents[portal->categoryIdx].unkShort2 = minY;
									else
										Levels::g_datContents[portal->categoryIdx].unkShort2 = a4;

									if (maxX <= a3)
										Levels::g_datContents[portal->categoryIdx].unkShort3 = maxX;
									else
										Levels::g_datContents[portal->categoryIdx].unkShort3 = a3;

									if (maxY <= a5)
										Levels::g_datContents[portal->categoryIdx].unkShort4 = maxY;
									else
										Levels::g_datContents[portal->categoryIdx].unkShort4 = a5;
								}

								if (portal->categoryIdx == 15 && g_hasBackdrop && Sector::g_unk54DD74 < 12)
								{
									int32_t rectIdx = Sector::g_unk54DD74;

									*(uint8_t*)&Renderer::Shadows::g_unusedShadowVar = 0;

									g_backdropRects[rectIdx].x0 = Levels::g_datContents[portal->categoryIdx].unkShort1;
									g_backdropRects[rectIdx].x1 = Levels::g_datContents[portal->categoryIdx].unkShort3;
									g_backdropRects[rectIdx].y0 = Levels::g_datContents[portal->categoryIdx].unkShort2;

									int32_t categoryIdx = portal->categoryIdx;

									Sector::g_unk54DD74 = rectIdx + 1;
									g_backdropRects[rectIdx].y1 = Levels::g_datContents[categoryIdx].unkShort4;
								}
								else
								{
									int32_t categoryIdx = portal->categoryIdx;

									FloodVisibility(
										Levels::g_portalEntries[categoryIdx].entries,
										Levels::g_datContents[categoryIdx].unkShort1,
										Levels::g_datContents[categoryIdx].unkShort3,
										Levels::g_datContents[categoryIdx].unkShort2,
										Levels::g_datContents[categoryIdx].unkShort4,
										a6 - 1);
								}
							}
						}
					}
					else
					{
						Levels::g_datContents[portal->categoryIdx].unkByte1 = 0;
					}
				}

				recordIdx = portal[1].recordIdx;
				a1 = ++portal;
			}
		}

		// FUNCTION: TOY2 0x0043FEF0
		void UpdateActiveSector()
		{
			uint32_t* mathScratch = (uint32_t*)Collision::g_mathScratch;

			Levels::PortalEntry* portal = Levels::g_portalEntries[g_curSectorCopy2].entries;
			uint8_t recordIdx = portal->recordIdx;
			Levels::PortalQuad* quad;
			int32_t x;
			int32_t y;
			int32_t z;

			while (recordIdx != 0xFF)
			{
				if (portal->categoryIdx == 15 && g_hasBackdrop)
					goto NEXT_PORTAL;

				x = g_buzzActor.posAngles.pos.x >> 7;

				quad = (Levels::PortalQuad*)Levels::g_recordData[recordIdx];

				// Buzz has to be near the first corner of the quad, or near the third one
				if (abs(x - quad->p0x) < 0x4000)
				{
					y = g_buzzActor.posAngles.pos.y >> 7;

					if (abs(y - quad->p0y) < 0x4000)
					{
						z = g_buzzActor.posAngles.pos.z >> 7;

						if (abs(z - quad->p0z) < 0x4000)
							goto LABEL_10;
					}
				}

				if (abs(x - quad->p2x) >= 0x4000)
					goto NEXT_PORTAL;

				y = g_buzzActor.posAngles.pos.y >> 7;

				if (abs(y - quad->p2y) >= 0x4000)
					goto NEXT_PORTAL;

				z = g_buzzActor.posAngles.pos.z >> 7;

				if (abs(z - quad->p2z) >= 0x4000)
					goto NEXT_PORTAL;

			LABEL_10:
				mathScratch[144] = quad->nx;
				mathScratch[145] = quad->ny;
				mathScratch[146] = quad->nz;

				{
					quad = (Levels::PortalQuad*)Levels::g_recordData[portal->recordIdx];

					int32_t quadZ = quad->p0z;
					int32_t quadY = quad->p0y;
					int32_t quadX = quad->p0x;

					// the side of the plane of the quad that Buzz is on now and was on before
					int32_t sideNow =
						(z - quadZ) * (int32_t)mathScratch[146] + (y - quadY) * (int32_t)mathScratch[145] + (x - quadX) * (int32_t)mathScratch[144];

					if (sideNow >= 0)
						goto NEXT_PORTAL;

					int32_t sidePrevious = ((Sector::g_unk54D920.z >> 7) - quadZ) * (int32_t)mathScratch[146]
						+ ((Sector::g_unk54D920.y >> 7) - quadY) * (int32_t)mathScratch[145]
						+ ((Sector::g_unk54D920.x >> 7) - quadX) * (int32_t)mathScratch[144];

					if (sidePrevious < 0)
						goto NEXT_PORTAL;

					// the normal is packed into 3 shorts
					((int16_t*)&mathScratch[132])[0] = (int16_t)mathScratch[144];
					((int16_t*)&mathScratch[132])[1] = (int16_t)mathScratch[145];
					((int16_t*)&mathScratch[132])[2] = (int16_t)mathScratch[146];

					// where the path of Buzz crosses the plane
					mathScratch[242] =
						((Sector::g_unk54D920.x + sidePrevious * (g_buzzActor.posAngles.pos.x - Sector::g_unk54D920.x) / (sidePrevious - sideNow)) >> 7)
						- ((Levels::PortalQuad*)Levels::g_recordData[portal->recordIdx])->p0x;

					mathScratch[243] =
						((Sector::g_unk54D920.y + sidePrevious * (g_buzzActor.posAngles.pos.y - Sector::g_unk54D920.y) / (sidePrevious - sideNow)) >> 7)
						- ((Levels::PortalQuad*)Levels::g_recordData[portal->recordIdx])->p0y;

					mathScratch[244] =
						((Sector::g_unk54D920.z + sidePrevious * (g_buzzActor.posAngles.pos.z - Sector::g_unk54D920.z) / (sidePrevious - sideNow)) >> 7)
						- ((Levels::PortalQuad*)Levels::g_recordData[portal->recordIdx])->p0z;

					quad = (Levels::PortalQuad*)Levels::g_recordData[portal->recordIdx];

					if (Nu3D::Math::PointIntersectsTriangle(
							mathScratch[242],
							mathScratch[243],
							mathScratch[244],
							quad->p1x - quad->p0x,
							quad->p1y - quad->p0y,
							quad->p1z - quad->p0z,
							quad->p2x - quad->p0x,
							quad->p2y - quad->p0y,
							quad->p2z - quad->p0z,
							(Vector3I16*)&mathScratch[132],
							400)
						|| (quad = (Levels::PortalQuad*)Levels::g_recordData[portal->recordIdx],
							Nu3D::Math::PointIntersectsTriangle(
								mathScratch[242],
								mathScratch[243],
								mathScratch[244],
								quad->p2x - quad->p0x,
								quad->p2y - quad->p0y,
								quad->p2z - quad->p0z,
								quad->p3x - quad->p0x,
								quad->p3y - quad->p0y,
								quad->p3z - quad->p0z,
								(Vector3I16*)&mathScratch[132],
								400)))
					{
						g_curSectorCopy2 = portal->categoryIdx;
					}
				}

			NEXT_PORTAL:
				recordIdx = portal[1].recordIdx;
				++portal;
			}
		}

		// FUNCTION: TOY2 0x00440260
		void UpdateActiveSectorAt(int32_t x, int32_t y, int32_t z)
		{
			int32_t previousZ = g_buzzActor.posAngles.pos.z;
			int32_t previousX = g_buzzActor.posAngles.pos.x;
			int32_t previousY = g_buzzActor.posAngles.pos.y;

			g_buzzActor.posAngles.pos.x = x;
			g_buzzActor.posAngles.pos.y = y;
			g_buzzActor.posAngles.pos.z = z;

			UpdateActiveSector();

			g_buzzActor.posAngles.pos.y = previousY;
			g_buzzActor.posAngles.pos.x = previousX;
			g_buzzActor.posAngles.pos.z = previousZ;
		}
	}
}

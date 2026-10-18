#include "Toy2/Level.h"

#include "Toy2/Collision.h"
#include "Toy2/Camera.h"
#include "Toy2/Toy2.h"
#include "Nu3D/Particles.h"

#include <stdlib.h>
#include <string.h>

namespace Toy2
{
	// GLOBAL: TOY2 0x0052B7DC
	int16_t g_levelTransition;

	namespace Level
	{
		// GLOBAL: TOY2 0x00830E28
		int32_t g_unk830E28;

		// GLOBAL: TOY2 0x00830C9C
		int32_t g_unk830C9C;

		// FUNCTION: TOY2 0x00414550
		void FixupRecordCoordinates()
		{
			g_unk830E28 = 0;
			g_unk830C9C = 0;

			Levels::RecordData* record = Levels::g_recordData[58];

			if (record)
			{
				Vector3I* write = record->data;
				Vector3I* read = write;
				int32_t recordCount = record->recordCount;
				int32_t idx = 0;

				if (recordCount > 0)
				{
					do
					{
						record = Levels::g_recordData[58];

						// an all zero entry is dropped from the list, remember where it was
						if (write->x == 0 && write->y == 0 && write->z == 0)
						{
							g_unk830E28 = idx;
							read++;
							record->recordCount--;
							idx++;
						}

						write->x = 32 * read->x;
						write->y = 32 * read->y;
						write->z = 32 * read->z;

						write++;
						read++;
						idx++;
					} while (idx < recordCount);
				}
			}

			if (Levels::g_recordData[60])
			{
				int32_t* value = (int32_t*)Levels::g_recordData[60]->data;

				for (int32_t i = 0; i < 3 * Levels::g_recordData[60]->recordCount; i++)
				{
					*value = 32 * *value;
					value++;
				}
			}

			record = Levels::g_recordData[61];

			if (record)
			{
				Vector3I* read = record->data;
				Vector3I* write = record->data;
				uint16_t newRecordCount = record->recordCount;
				int32_t spacing = 0;
				int32_t pairIdx = 0;

				if ((record->recordCount >> 1) > 0)
				{
					do
					{
						int32_t absY = abs(read->y);

						// entries where all three components are equal are markers, they set the spacing for the pairs after them
						for (int32_t absX = abs(read->x); absX == absY; absX = abs(read->x))
						{
							if (absY != abs(read->z))
								break;

							read++;
							spacing = absX / 50;
							newRecordCount--;
							absY = abs(read->y);
						}

						write[0].x = 32 * read[0].x;
						write[0].y = 32 * read[0].y;
						write[0].z = 32 * read[0].z;
						write[1].x = spacing;
						write[1].y = write[0].y - 32 * read[1].y;

						read += 2;
						write += 2;

						record = Levels::g_recordData[61];
						pairIdx++;
					} while (pairIdx < (record->recordCount >> 1));
				}

				record->recordCount = newRecordCount;
			}

			if (Levels::g_recordData[62])
			{
				int32_t* value = (int32_t*)Levels::g_recordData[62]->data;

				for (int32_t i = 0; i < 3 * Levels::g_recordData[62]->recordCount; i++)
				{
					*value = 32 * *value;
					value++;
				}
			}
		}

		// FUNCTION: TOY2 0x0043E070
		void LoaderHelper()
		{
			uint32_t* mathScratch = (uint32_t*)Collision::g_mathScratch;

			int32_t foundIndex = Levels::g_currentType64Region;
			int32_t index = 0;
			int32_t previousRegion = Levels::g_currentType64Region;
			int32_t bestVar6 = 0x7FFFFFFF;

			// find the region that holds the camera position, the smallest one wins
			if (Levels::g_type64Count > 0)
			{
				Levels::Type64* region = Levels::g_type64Structs;

				do
				{
					if (region->unkVar6 < bestVar6 && (int32_t)mathScratch[225] >= region->unkVar0.x && (int32_t)mathScratch[226] >= region->unkVar0.y
						&& (int32_t)mathScratch[227] >= region->unkVar0.z && (int32_t)mathScratch[225] <= region->unkVar3.x
						&& (int32_t)mathScratch[226] <= region->unkVar3.y && (int32_t)mathScratch[227] <= region->unkVar3.z)
					{
						foundIndex = index;
						bestVar6 = region->unkVar6;
					}

					++index;
					++region;
				} while (index < Levels::g_type64Count);

				Levels::g_currentType64Region = foundIndex;
			}

			if (foundIndex == -1)
			{
				foundIndex = 0;
				Levels::g_currentType64Region = 0;
			}

			if (foundIndex != previousRegion)
			{
				uint8_t* visData = (uint8_t*)Levels::g_type64Structs[foundIndex].unkVar8;

				memcpy(Levels::g_activeZoneVisTable, visData + 4, 0x100);
				Levels::g_unk54E054 = 2;
				Levels::g_unk550C20 = visData + 260;
			}
		}

		// FUNCTION: TOY2 0x0043E2D0
		int32_t PolyCountHelper(int32_t* a1, Toy2::Levels::InstanceSection* a2)
		{
			a2->x = 0;

			int16_t* cursor;

			if (*a1 >= 0)
			{
				cursor = (int16_t*)((uint8_t*)a1 + 8 * *a1 + 4);
			}
			else
			{
				int32_t count = -*a1;

				cursor = (int16_t*)((uint8_t*)a1 + 12 * count + 8);
				*a1 = -count;
			}

			int32_t result = 0;
			int32_t count;

			while ((uint16_t)*cursor != 0xFFFF)
			{
				switch (*cursor & 0x1F)
				{
					case 0:
					case 1:
					case 2:
					case 3:
					case 4:
					case 6:
						count = cursor[1];
						cursor += 2;

						if (count > 0)
						{
							cursor += 6 * count;
							result += count;
						}

						break;

					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
					case 14:
						count = cursor[1];
						cursor += 2;

						if (count > 0)
						{
							result += 2 * count;
							cursor += 6 * count;
						}

						break;

					case 16:
					case 17:
					case 18:
					case 19:
					case 20:
					case 22:
					case 24:
					case 25:
					case 26:
					case 27:
					case 28:
					case 30:
						count = cursor[1];
						*(uint8_t*)cursor &= ~8;
						cursor += 2;

						if (count > 0)
						{
							cursor += 2 * count;
							result += count;
						}

						break;

					default:
						break;
				}
			}

			return result;
		}

		// FUNCTION: TOY2 0x0043E430
		int32_t CalculatePolyCount(Toy2::Levels::InstanceSection* secondData)
		{
			uint8_t* modelData = (uint8_t*)secondData->unk6->modelPtr;
			int32_t* meshData;
			int32_t scratch;
			int32_t result;
			int32_t lod;

			switch ((uint8_t)secondData->flags & 0x6F)
			{
				case 1:
				case 4:
				case 0x41:
				case 0x44:
					result = PolyCountHelper(*(int32_t**)(modelData + 0x14), (Toy2::Levels::InstanceSection*)&scratch);

					lod = modelData[0x13] >> 3;

					if (lod > 0x12)
						lod = 0x12;

					modelData[0x13] = (modelData[0x13] & 7) + 8 * lod;
					break;

				case 2:
				case 3:
				case 0x42:
				case 0x43:
					meshData = *(int32_t**)(modelData + 0x14);

					goto COUNT_MESHES;

				case 9:
				case 0xC:
				case 0x49:
				case 0x4C:
					result = PolyCountHelper(*(int32_t**)(modelData + 0x1C), (Toy2::Levels::InstanceSection*)&scratch);

					lod = modelData[0x19] >> 3;

					if (lod > 0x12)
						lod = 0x12;

					modelData[0x19] = (modelData[0x19] & 7) + 8 * lod;
					break;

				case 0xA:
				case 0xB:
				case 0x4A:
				case 0x4B:
					meshData = *(int32_t**)(modelData + 0x1C);

				COUNT_MESHES:
					result = 0;

					if (*meshData > 0)
					{
						int32_t meshCount = *meshData;
						int32_t* mesh = &meshData[3 * *meshData + 1];

						do
						{
							int32_t* next = mesh + 1;
							int32_t count = *mesh;

							if (count > 0)
							{
								result += count;
								next += 11 * count;
							}

							--meshCount;
							mesh = next;
						} while (meshCount);
					}

					break;

				default:
					result = 0;
					break;
			}

			return result;
		}

		// FUNCTION: TOY2 0x0049FB40
		void UpdateAmbientEmitters()
		{
			int32_t y;
			int32_t x;
			int32_t z;

			if (g_tickState[9])
			{
				Levels::RecordData* record = Levels::g_recordData[58];

				if (Levels::g_recordData[58])
				{
					int32_t idx = g_unk830C9C;
					int32_t savedIdx = g_unk830C9C;
					Vector3I* emitter = &Levels::g_recordData[58]->data[g_unk830C9C];
					Vector3I* savedEmitter = emitter;

					if (g_unk830C9C < Levels::g_recordData[58]->recordCount)
					{
						do
						{
							y = emitter->y;

							if (y != 0x80000000)
							{
								x = emitter->x;
								z = emitter->z;
								int32_t distYZ = ((Camera::g_activeCameraTransform.pos.y - y) >> 8) * ((Camera::g_activeCameraTransform.pos.y - y) >> 8)
									+ ((Camera::g_activeCameraTransform.pos.z - z) >> 8) * ((Camera::g_activeCameraTransform.pos.z - z) >> 8);
								int32_t distX =
									((Camera::g_activeCameraTransform.pos.x - emitter->x) >> 8) * ((Camera::g_activeCameraTransform.pos.x - emitter->x) >> 8);

								if (distX + distYZ >= 360000 || distX + distYZ == -1)
								{
									idx = savedIdx;
								}
								else
								{
									idx = savedIdx;

									if (savedIdx >= g_unk830E28)
										Nu3D::Particles::SpawnFromPreset(x, y, z, 115, 2);
									else
										Nu3D::Particles::SpawnFromPreset(x, y, z, 113, 2);
								}
							}

							record = Levels::g_recordData[58];
							idx += 4;
							emitter = savedEmitter + 4;
							savedEmitter += 4;
							savedIdx = idx;
						} while (idx < Levels::g_recordData[58]->recordCount);

						idx = g_unk830C9C;
					}

					idx = idx + 1;
					g_unk830C9C = idx;

					if (idx >= record->recordCount || idx >= 4)
						g_unk830C9C = 0;
				}
			}
		}
	}
}

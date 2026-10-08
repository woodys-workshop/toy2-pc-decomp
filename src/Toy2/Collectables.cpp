#include "Toy2/Collectables.h"
#include "Toy2/Levels.h"
#include "Toy2/Camera.h"
#include "Toy2/Toy2.h"
#include "Nu3D/Link.h"
#include "Nu3D/Collision.h"
#include "SaveManager.h"

#include <string.h>

namespace Toy2
{
	namespace Collectables
	{
		// GLOBAL: TOY2 0x0050A150
		TokenTableEntry g_tokenTable[10];

		// GLOBAL: TOY2 0x00830CCC
		PickupSlot g_pickupSlots[5];

		// GLOBAL: TOY2 0x00830CAC
		int32_t g_unk830CAC;

		// GLOBAL: TOY2 0x005546B8
		PickupTable g_pickupTable;

		// GLOBAL: TOY2 0x00556FAC
		Levels::RecordData* g_unk556FAC;

		// FUNCTION: TOY2 0x00447DB0
		void BuildPickupTable()
		{
			int32_t levelIdx = g_levelFileIndex;
			int32_t firstObject = 48;

			switch (g_levelFileIndex)
			{
				case 4:
					firstObject = 96;
					break;

				case 5:
					firstObject = 80;
					break;

				case 10:
					firstObject = 96;
					break;

				case 11:
					firstObject = 96;
					break;

				case 14:
					firstObject = 96;
					break;

				case 16:
					firstObject = 400;
					break;
			}

			Levels::RecordData* lightRecord = Levels::g_recordData[59];
			Levels::RecordData* pickupRecord = Levels::g_recordData[63];

			g_unk556FAC = Levels::g_recordData[63];

			Levels::Type63* table = g_pickupTable.data;

			// the light sprites sit on the ground
			if (lightRecord)
			{
				int32_t i = 0;

				if (Levels::g_recordData[59]->recordCount)
				{
					do
					{
						WorldPos position;

						position.x = 32 * lightRecord->data[i].x;
						position.z = 32 * lightRecord->data[i].z;
						position.y = 32 * (lightRecord->data[i].y - 10);

						Levels::g_recordData[59]->data[i].y = Nu3D::Collision::GetGroundHeightEx(&position, 0, 0) >> 5;
						lightRecord = Levels::g_recordData[59];

						++i;
					} while (i < Levels::g_recordData[59]->recordCount);

					levelIdx = g_levelFileIndex;
					pickupRecord = Levels::g_recordData[63];
				}
			}

			uint16_t recordCount = 0;

			if (pickupRecord)
			{
				int32_t i = 0;
				Levels::Type63* pickup = (Levels::Type63*)pickupRecord->data;

				if (pickupRecord->recordCount)
				{
					do
					{
						WorldPos position;

						position.x = 32 * pickup->pos.x;
						position.z = 32 * pickup->pos.z;
						position.y = 32 * (pickup->pos.y - 10);

						int32_t floorPos = Nu3D::Collision::GetGroundHeightEx(&position, 0, 0) >> 5;

						pickup->props.floorPos = floorPos;

						if (floorPos == 0x7FFF)
							pickup->props.floorPos = 32766;

						if (pickup->props.floorPos - pickup->pos.y > 6144)
							pickup->props.floorPos = 32767;

						Levels::Type63* current = pickup;

						pickup->props.flags = 17;
						Levels::Type63* entry = table;

						++pickup;
						++table;

						*entry = *current;
						++i;
					} while (i < Levels::g_recordData[63]->recordCount);

					levelIdx = g_levelFileIndex;
				}

				g_pickupTable.recordType = g_unk556FAC->recordType;
				recordCount = g_unk556FAC->recordCount;
			}
			else
			{
				g_pickupTable.recordType = 0;
			}

			g_pickupTable.recordCount = recordCount;
			Levels::g_recordData[63] = (Levels::RecordData*)&g_pickupTable;

			if (levelIdx == 5)
			{
				Vector3I position;

				Levels::g_objectListBase->entries[89]->z_or_coord = -18000;
				Nu3D::Link::GetCurrentPosFixed(89, &position);
				Nu3D::Link::SetPositionRawAndCommit(89, position.x >> 5, position.y >> 5, -18000);
			}

			// the objects of the level are added as pickups after the ones of the record
			Levels::ObjectList* objectList = Levels::g_objectListBase;

			if (objectList->count > 47)
			{
				int32_t objectIdx = firstObject;

				if (firstObject <= objectList->count)
				{
					do
					{
						// $TODO: the entries are read as a position and a type, which is not what Levels::Object has
						int32_t* object = (int32_t*)objectList->entries[objectIdx];

						if (object)
						{
							WorldPos position;

							table->pos.x = object[0];
							table->pos.y = object[1];
							table->pos.z = object[2];
							table->props.linkId = objectIdx;
							table->props.flags = (uint8_t)(*(int16_t*)(object + 3) >> 3);

							position.x = 32 * table->pos.x;
							position.z = 32 * table->pos.z;
							position.y = 32 * (table->pos.y - 10);

							int32_t floorPos = Nu3D::Collision::GetGroundHeightEx(&position, 0, 0) >> 5;

							table->props.floorPos = floorPos;

							if (floorPos == 0x7FFF)
								table->props.floorPos = 32766;

							if (table->props.floorPos - table->pos.y > 6144)
								table->props.floorPos = 0x7FFF;

							objectList = Levels::g_objectListBase;
							++table;
							++g_pickupTable.recordCount;
						}

						++objectIdx;
					} while (objectIdx <= objectList->count);
				}
			}
		}

		// FUNCTION: TOY2 0x004025C0
		int32_t* LoadTokenTable(int32_t* a1)
		{
			memset(g_tokenTable, 0, sizeof(g_tokenTable));

			int32_t* result = a1;
			TokenTableEntry* entry = g_tokenTable;

			do
			{
				int32_t firstValue = *result++;

				if (firstValue < 0)
					break;

				entry->unkVar0 = firstValue;
				entry->unkVar1 = *result;
				result++;
				entry->text = (const char*)*result;
				result++;
				entry->unkVar3 = *result;
				result++;

				entry++;
			} while (entry < &g_tokenTable[10]);

			return result;
		}

		// STUB: TOY2 0x00402610
		void Token(int32_t linkId) {}

		// FUNCTION: TOY2 0x004A0C80
		void Init(int16_t* linkTable, int32_t linkId)
		{
			int16_t* link = linkTable;
			int32_t objectIdx = 0;

			uint8_t tokenBits = SaveManager::g_save0Data.tokens[g_levelFileIndex];
			int32_t remainingBits = tokenBits;

			g_unk830CC4 = 0;

			if (linkTable)
			{
				int32_t currentLinkId = linkId;

				g_unk830CAC = 0;

				PickupSlot* slot = g_pickupSlots;

				while (1)
				{
					// a token that was collected already shows the collected shape
					if (tokenBits & 1)
						Nu3D::Link::CopyShapeId(*link, currentLinkId);

					slot->linkId = *link;
					slot->state = 0;

					int16_t* nextLink = link + 1;

					Nu3D::Link::SetScaleFromFixedOffsets(slot->linkId, 0, 0, 0);

					Levels::RecordData* record = Levels::g_recordData[63];
					Levels::Type63* object = (Levels::Type63*)Levels::g_recordData[63]->data;

					if (Levels::g_recordData[63]->recordCount)
					{
						while (object->props.linkId != slot->linkId)
						{
							++object;
							++objectIdx;

							if (objectIdx >= Levels::g_recordData[63]->recordCount)
								goto LABEL_14;
						}

						object->pos.y = 0x80000000;
						record = Levels::g_recordData[63];
						slot->yPosPtr = &object->pos.y;

					LABEL_14:
						currentLinkId = linkId;
					}

					remainingBits >>= 1;

					int32_t searchIdx = 0;

					if (record->recordCount)
					{
						while (object->props.linkId != currentLinkId)
						{
							++object;
							++searchIdx;

							if (searchIdx >= record->recordCount)
								goto LABEL_20;
						}

						object->pos.y = 0x80000000;
					}

				LABEL_20:
					Nu3D::Link::SetScaleFromFixedOffsets(currentLinkId, 0, 0, 0);

					++slot;
					++currentLinkId;
					linkId = currentLinkId;

					if (slot >= &g_pickupSlots[5])
						break;

					tokenBits = remainingBits;
					link = nextLink;
					objectIdx = 0;
				}
			}
			else
			{
				g_unk830CAC = 1;

				PickupSlot* slot = g_pickupSlots;

				do
					(slot++)->state = 0;
				while (slot < &g_pickupSlots[5]);
			}
		}

		// FUNCTION: TOY2 0x004A0DB0
		void Activate(int32_t slotIdx, int32_t skipIntro)
		{
			if (! g_pickupSlots[slotIdx].state)
			{
				g_pickupSlots[slotIdx].state = 1;

				int32_t* yPosPtr = g_pickupSlots[slotIdx].yPosPtr;
				Vector3I pos;

				Nu3D::Link::GetCurrentPosFixed(g_pickupSlots[slotIdx].linkId, &pos);
				*yPosPtr = pos.y >> 5;

				if (skipIntro)
				{
					Nu3D::Link::SetScaleFromFixedOffsets(g_pickupSlots[slotIdx].linkId, 4096, 4096, 4096);
				}
				else
				{
					g_pickupSlots[slotIdx].collectTimer = 132;
					Camera::BeginScriptedCutsceneAtPoint(&pos, 180, 16);

					if (Camera::g_cameraState)
						Camera::g_cameraState = 5;
				}
			}
		}

		// STUB: TOY2 0x004A0E60
		void Deactivate(int32_t slotIdx) {}

		// FUNCTION: TOY2 0x004A50D0
		void CosmicShield(Toy2::Levels::Type63* object)
		{
			if (g_cosmicShieldPickup)
			{
				g_cosmicShieldPickup->pos.y = g_cosmicShieldPickupYPos;

				Nu3D::Link::SetScaleFromFixedOffsets(g_cosmicShieldPickup->props.linkId, 0x2000, 0x2000, 0x2000);
				Nu3D::Link::SetPositionRawAndCommit(
					g_cosmicShieldPickup->props.linkId, g_cosmicShieldPickup->pos.x, g_cosmicShieldPickup->pos.y, g_cosmicShieldPickup->pos.z);

				g_cosmicShieldPickup = 0;
			}

			g_cosmicShieldPickup = object;
			g_cosmicShieldPickupYPos = object->pos.y;

			g_buzzActor.inhibitMovementTimer = 900;

			Nu3D::Link::SetScaleFromFixedOffsets(object->props.linkId, 12288, 12288, 12288);
		}

		// STUB: TOY2 0x004A0F80
		void Interactions() {}

		// STUB: TOY2 0x0044E520
		int32_t Classify(int32_t a1) { return 0; }
	}
}
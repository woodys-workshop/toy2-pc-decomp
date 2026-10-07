#include "Toy2/Collectables.h"
#include "Toy2/Levels.h"
#include "Toy2/Camera.h"
#include "Toy2/Toy2.h"
#include "Nu3D/Link.h"
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

		// STUB: TOY2 0x00447DB0
		void BuildPickupTable() {}

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

		// STUB: TOY2 0x004A50D0
		void CosmicShield(Toy2::Levels::Type63* object) {}

		// STUB: TOY2 0x004A0F80
		void Interactions() {}

		// STUB: TOY2 0x0044E520
		int32_t Classify(int32_t a1) { return 0; }
	}
}
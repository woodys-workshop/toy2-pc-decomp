#include "Toy2/Gadget.h"
#include "Toy2/Toy2.h"
#include "Toy2/Levels.h"
#include "Nu3D/Link.h"

namespace Toy2
{
	namespace Gadget
	{
		// GLOBAL: TOY2 0x00503A1E
		int16_t g_unk503A1E[19] = {
			(int16_t)0xF4B0, 0x0000, (int16_t)0xF4B0, 0x0108, 0x0000, 0x0000, 0x0409, 0x0000, 0x0000, 0x0201,
			0x0000,			 0x0000, 0x1004,		  0x0000, 0x0000, 0x0805, 0x0000, 0x0000, 0x0000,
		};

		// each table is a list of 4 byte entries (level, unknown, link id, unknown) ended by 0xFF

		// GLOBAL: TOY2 0x00503A44
		uint8_t g_shieldUnlockTable[16] = {
			0x01, 0x19, 0x31, 0x0A, 0x08, 0x01, 0x30, 0x02, 0x0B, 0x52, 0x6A, 0x18, 0xFF, 0x00, 0x00, 0x00,
		};

		// GLOBAL: TOY2 0x00503A54
		uint8_t g_rocketUnlockTable[80] = {
			0x02, 0x11, 0x3B, 0x08, 0x02, 0x12, 0x3C, 0x09, 0x02, 0x13, 0x3D, 0x0A, 0x05, 0x25, 0x50, 0x14, 0x05, 0x30, 0x66, 0x15,
			0x07, 0x20, 0x3C, 0x10, 0x0E, 0x49, 0x63, 0x05, 0x0E, 0x4B, 0x62, 0x0A, 0x0E, 0x4A, 0x61, 0x07, 0x0E, 0x47, 0x60, 0x0B,
			0x0E, 0x41, 0x71, 0x0C, 0x0E, 0x40, 0x70, 0x09, 0x0E, 0x3F, 0x6E, 0x03, 0x0E, 0x3E, 0x6D, 0x06, 0x0E, 0x3D, 0x68, 0x08,
			0x0E, 0x3C, 0x67, 0x02, 0x0E, 0x3B, 0x66, 0x01, 0x0E, 0x3A, 0x65, 0x04, 0x0E, 0x35, 0x64, 0x0D, 0xFF, 0x00, 0x00, 0x00,
		};

		// GLOBAL: TOY2 0x00503AA4
		uint8_t g_discUnlockTable[20] = {
			0x04, 0x46, 0x6B, 0x0D, 0x04, 0x45, 0x6C, 0x19, 0x05, 0x2B, 0x5A, 0x18, 0x07, 0x23, 0x40, 0x13, 0xFF, 0x00, 0x00, 0x00,
		};

		// GLOBAL: TOY2 0x00503AB8
		uint8_t g_hoverUnlockTable[16] = {
			0x07, 0x19, 0x3E, 0x11, 0x0D, 0x1A, 0x3D, 0x0D, 0x0D, 0x1B, 0x3E, 0x0C, 0xFF, 0x00, 0x00, 0x00,
		};

		// GLOBAL: TOY2 0x00503AC8
		uint8_t g_grappleUnlockTable[12] = {
			0x05, 0x26, 0x51, 0x19, 0x0A, 0x2C, 0x60, 0x12, 0xFF, 0x00, 0x00, 0x00,
		};

		// GLOBAL: TOY2 0x00830D1C
		int32_t g_unk830D1C;

		// GLOBAL: TOY2 0x00830C98
		int32_t g_unk830C98;

		// GLOBAL: TOY2 0x00830D40
		int32_t g_periodicHintSoundTimer;

		// GLOBAL: TOY2 0x00830D34
		int32_t g_unk830D34;

		// GLOBAL: TOY2 0x00830D24
		int32_t g_unk830D24;

		// GLOBAL: TOY2 0x00830CA4
		int32_t g_unk830CA4;

		// GLOBAL: TOY2 0x00830D48
		int32_t g_unk830D48;

		// FUNCTION: TOY2 0x004A2080
		void InitLevelUnlockGeometry()
		{
			uint8_t unlockMask = (uint8_t)(g_unk503A1E[g_levelFileIndex + 2] >> 8);

			g_unk830D1C = 0;
			g_unk830C98 = 200;
			g_periodicHintSoundTimer = 250;
			g_unk830D34 = 150;
			g_unk830D24 = -1;
			g_unk830CA4 = 100;

			Levels::RecordData* recordData;
			Levels::Type63* object;
			int32_t linkId;
			int32_t objectIdx;
			uint8_t* entry;

			if (unlockMask & g_playerUnlocks)
			{
				switch (g_levelFileIndex)
				{
					case 1:
						linkId = 70;
						break;

					case 4:
						linkId = 109;
						break;

					case 7:
						linkId = 63;
						break;

					case 10:
						linkId = 111;
						break;

					default:
						linkId = 63;

						// $TODO: the original reads an uninitialised local for every other level, this can not happen as the unlock table is only set for
						// levels 1, 4, 7, 10 and 13
						if (g_levelFileIndex != 13)
							linkId = 0;

						break;
				}

				Nu3D::Link::SetScaleFromFixedOffsets(linkId, 0, 0, 0);

				recordData = Levels::g_recordData[63];
				objectIdx = 0;
				object = (Levels::Type63*)Levels::g_recordData[63]->data;

				if (Levels::g_recordData[63]->recordCount)
				{
					while (object->props.linkId != linkId)
					{
						object++;
						objectIdx++;

						if (objectIdx >= Levels::g_recordData[63]->recordCount)
						{
							g_unk830D48 = 0;
							goto LABEL_6;
						}
					}

					object->pos.y = 0x80000000;
					recordData = Levels::g_recordData[63];
				}

				g_unk830D48 = 0;
			}
			else
			{
				int32_t nodeIdx = (uint8_t)g_unk503A1E[g_levelFileIndex + 2];

				g_unk830D48 = nodeIdx;

				if (nodeIdx)
				{
					HideModelNode(9, nodeIdx);

					if (g_unk830D48 == 1)
					{
						HideModelNode(9, 11);
						HideModelNode(9, 12);
						HideModelNode(9, 15);
					}
				}

				recordData = Levels::g_recordData[63];
			}

		LABEL_6:
			// a gadget the player does not own yet hides the pickups that would give it
			if (g_playerUnlocks & UNLOCK_SHIELD)
			{
				ApplyUnlockToGeometry(g_shieldUnlockTable, 8);
				recordData = Levels::g_recordData[63];
			}
			else
			{
				entry = g_shieldUnlockTable;

				if (*entry != 0xFF)
				{
					do
					{
						if (*entry == g_levelFileIndex)
						{
							objectIdx = 0;
							object = (Levels::Type63*)recordData->data;

							if (recordData->recordCount)
							{
								while (object->props.linkId != entry[2])
								{
									object++;
									objectIdx++;

									if (objectIdx >= recordData->recordCount)
										goto SHIELD_NEXT;
								}

								Nu3D::Link::SetScaleFromFixedOffsets(entry[2], 4096, 4096, 4096);
								object->props.flags = 0;
								recordData = Levels::g_recordData[63];
							}
						}

					SHIELD_NEXT:
						entry += 4;
					} while (*entry != 0xFF);
				}
			}

			if (g_playerUnlocks & UNLOCK_ROCKET)
			{
				ApplyUnlockToGeometry(g_rocketUnlockTable, 4);
				recordData = Levels::g_recordData[63];
			}
			else
			{
				entry = g_rocketUnlockTable;

				if (*entry != 0xFF)
				{
					do
					{
						if (*entry == g_levelFileIndex)
						{
							objectIdx = 0;
							object = (Levels::Type63*)recordData->data;

							if (recordData->recordCount)
							{
								while (object->props.linkId != entry[2])
								{
									object++;
									objectIdx++;

									if (objectIdx >= recordData->recordCount)
										goto ROCKET_NEXT;
								}

								Nu3D::Link::SetScaleFromFixedOffsets(entry[2], 4096, 4096, 4096);
								object->props.flags = 0;
								recordData = Levels::g_recordData[63];
							}
						}

					ROCKET_NEXT:
						entry += 4;
					} while (*entry != 0xFF);
				}
			}

			if (g_playerUnlocks & UNLOCK_DISC)
			{
				ApplyUnlockToGeometry(g_discUnlockTable, 4);
				recordData = Levels::g_recordData[63];
			}
			else
			{
				entry = g_discUnlockTable;

				if (*entry != 0xFF)
				{
					do
					{
						if (*entry == g_levelFileIndex)
						{
							objectIdx = 0;
							object = (Levels::Type63*)recordData->data;

							if (recordData->recordCount)
							{
								while (object->props.linkId != entry[2])
								{
									object++;
									objectIdx++;

									if (objectIdx >= recordData->recordCount)
										goto DISC_NEXT;
								}

								Nu3D::Link::SetScaleFromFixedOffsets(entry[2], 4096, 4096, 4096);
								object->props.flags = 0;
								recordData = Levels::g_recordData[63];
							}
						}

					DISC_NEXT:
						entry += 4;
					} while (*entry != 0xFF);
				}
			}

			if (g_playerUnlocks & UNLOCK_HOVER)
			{
				ApplyUnlockToGeometry(g_hoverUnlockTable, 4);
				recordData = Levels::g_recordData[63];
			}
			else
			{
				entry = g_hoverUnlockTable;

				if (*entry != 0xFF)
				{
					do
					{
						if (*entry == g_levelFileIndex)
						{
							objectIdx = 0;
							object = (Levels::Type63*)recordData->data;

							if (recordData->recordCount)
							{
								while (object->props.linkId != entry[2])
								{
									object++;
									objectIdx++;

									if (objectIdx >= recordData->recordCount)
										goto HOVER_NEXT;
								}

								Nu3D::Link::SetScaleFromFixedOffsets(entry[2], 4096, 4096, 4096);
								object->props.flags = 0;
								recordData = Levels::g_recordData[63];
							}
						}

					HOVER_NEXT:
						entry += 4;
					} while (*entry != 0xFF);
				}
			}

			if (g_playerUnlocks & UNLOCK_GRAPPLE)
			{
				ApplyUnlockToGeometry(g_grappleUnlockTable, 4);
			}
			else
			{
				entry = g_grappleUnlockTable;

				if (*entry != 0xFF)
				{
					do
					{
						if (*entry == g_levelFileIndex)
						{
							objectIdx = 0;
							object = (Levels::Type63*)recordData->data;

							if (recordData->recordCount)
							{
								while (object->props.linkId != entry[2])
								{
									object++;
									objectIdx++;

									if (objectIdx >= recordData->recordCount)
										goto GRAPPLE_NEXT;
								}

								Nu3D::Link::SetScaleFromFixedOffsets(entry[2], 3072, 3072, 3072);
								object->props.flags = 0;
								recordData = Levels::g_recordData[63];
							}
						}

					GRAPPLE_NEXT:
						entry += 4;
					} while (*entry != 0xFF);
				}
			}
		}

		// STUB: TOY2 0x004A27A0
		void ApplyUnlockToGeometry(uint8_t* table, int32_t showScale) {}
	}
}

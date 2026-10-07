#include "Toy2/Level.h"

#include <stdlib.h>

namespace Toy2
{
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

		// STUB: TOY2 0x0043E070
		void LoaderHelper() {}

		// STUB: TOY2 0x0043E2D0
		int32_t PolyCountHelper(int32_t* a1, Toy2::Levels::InstanceSection* a2) { return 0; }

		// STUB: TOY2 0x0043E430
		int32_t CalculatePolyCount(Toy2::Levels::InstanceSection* secondData) { return 0; }

		// STUB: TOY2 0x0049FB40
		void UpdateAmbientEmitters() {}
	}
}

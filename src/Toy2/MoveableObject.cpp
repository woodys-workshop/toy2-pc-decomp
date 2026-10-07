#include "Toy2/MoveableObject.h"
#include "Toy2/Toy2.h"
#include "Toy2/Levels.h"
#include "Toy2/Platform.h"
#include "Nu3D/Link.h"

#include <string.h>

namespace Toy2
{
	namespace MoveableObject
	{
		// GLOBAL: TOY2 0x0053C680
		MoveableObject g_moveableObjects[10];

		// GLOBAL: TOY2 0x0053C670
		int32_t g_moveableGrabSoundCooldown;

		// GLOBAL: TOY2 0x0053C65C
		int32_t g_moveableObjectCount;

		// STUB: TOY2 0x004334D0
		void ComputeSegment(int32_t recordType, Toy2::MoveableObject::MoveableObject* l_moveableObject) {}

		// FUNCTION: TOY2 0x004335D0
		void InitTable(int16_t* array)
		{
			memset(g_moveableObjects, 0, sizeof(g_moveableObjects));

			g_moveableGrabSoundCooldown = 0;
			g_moveableObjectCount = 0;

			if (array)
			{
				int16_t modelNode = *array;

				if (*array != -1)
				{
					MoveableObject* moveableObject = g_moveableObjects;

					do
					{
						int32_t waypointIdx;

						if (g_levelFileIndex == 11 && modelNode == 29)
							waypointIdx = 2;
						else
							waypointIdx = 0;

						int16_t* entry = array + 1;

						moveableObject->modelNode = modelNode;
						moveableObject->platformIdx = *entry++;

						int16_t pathIdx = *entry;
						array = entry + 1;
						moveableObject->pathIdx = pathIdx;

						Levels::RecordData* path = Levels::g_recordData[pathIdx];

						Vector3I* pos = &moveableObject->pos;

						pos->x = 32 * path->data[waypointIdx].x;
						pos->y = 32 * path->data[waypointIdx].y;
						pos->z = 32 * path->data[waypointIdx].z;
						moveableObject->waypointIdx = waypointIdx;
						moveableObject->minWaypointIdx = 0;

						ComputeSegment(pathIdx, moveableObject);

						bool hasNoModel = moveableObject->modelNode < 0;
						moveableObject->segProgress = 0;

						if (! hasNoModel)
							Nu3D::Link::SetPositionRawAndCommit(moveableObject->modelNode, pos->x >> 5, pos->y >> 5, pos->z >> 5);

						Platform::SetOrigin(moveableObject->platformIdx, pos->x, pos->y, pos->z);

						++moveableObject;
						++g_moveableObjectCount;

						modelNode = *array;
					} while (*array != -1);
				}
			}
		}

		// STUB: TOY2 0x00433700
		void Update(Toy2::Buzz::Toy2BuzzActor* buzzActor) {}
	}
}

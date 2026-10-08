#include "Nu3D/Creature.h"
#include "NGNLoader/NGNLoader.h"

namespace Nu3D
{
	// GLOBAL: TOY2 0x00B423F0
	NodeMatricesInner g_nodeMatrices[64];

	// FUNCTION: TOY2 0x004C9F50
	void Creature::Destroy(Nu3D::Creature* creature)
	{
		if (creature->matrixList1 != 0)
			free(creature->matrixList1);

		if (creature->matrixList2 != 0)
			free(creature->matrixList2);

		if (creature->matrixList3 != 0)
			free(creature->matrixList3);

		if (creature->nodeNames != 0)
		{
			for (int32_t nodeNameIdx = 0; nodeNameIdx < creature->nodeCount; ++nodeNameIdx)
				free(creature->nodeNames[nodeNameIdx]);

			free(creature->nodeNames);
		}

		if (creature->primitives != 0)
		{
			for (int32_t primIdx = 0; primIdx < creature->nodeCount; ++primIdx)
			{
				if (creature->primitives[primIdx] != 0)
					Nu3D::Primitive::Destroy(creature->primitives[primIdx]);
			}
		}

		int32_t animCount = creature->animCount;

		if (animCount != 0)
		{
			int32_t animIndex = 0;

			if (animCount > 0)
			{
				do
				{
					free(creature->animData[animIndex++]);

				} while (animIndex < creature->animCount);
			}

			free(creature->animData);
		}

		if (creature->patch != 0)
			Nu3D::Patch::PatchVertices::Destroy(creature->patch);

		if (creature->flagsList != 0)
			free(creature->flagsList);

		free(creature);
	}

	// FUNCTION: TOY2 0x004CAB80
	int32_t Creature::SetNodeVisible(Nu3D::Creature* creature, int32_t nodeIdx, int32_t visible)
	{
		int32_t result = -1;

		if (nodeIdx >= 0 && nodeIdx < creature->nodeCount)
		{
			int32_t** flagsList = creature->flagsList;

			if (flagsList)
			{
				// the flags are stored in the pointer slots, bit 0 means hidden
				int32_t previous = ~(int32_t)flagsList[nodeIdx];
				flagsList[nodeIdx] = (int32_t*)((int32_t)flagsList[nodeIdx] & 0xFFFFFFFE);

				result = previous & 1;

				if (! visible)
					creature->flagsList[nodeIdx] = (int32_t*)((int32_t)creature->flagsList[nodeIdx] | 1);
			}
		}

		return result;
	}

	// FUNCTION: TOY2 0x004CE570
	void Creature::SetNodeVisibleByIndex(int32_t creatureIndex, int32_t nodeIdx, int32_t visible)
	{
		if (creatureIndex >= 0 && creatureIndex < NGNLoader::g_ngnImage->creatureCount)
		{
			Nu3D::Creature* creature = NGNLoader::g_ngnImage->creatureData[creatureIndex];

			if (creature)
				SetNodeVisible(creature, nodeIdx, visible);
		}
	}
}

#include "NGNLoader/NGNTypes.h"
#include "Nu3D/Portal.h"

namespace NGNLoader
{
	// FUNCTION: TOY2 0x004B33A0
	void NGNImage::DestroyPortal(Nu3D::Portal::AreaPortal* portal) { free(portal); }

	// FUNCTION: TOY2 0x004BC1B0
	void NGNImage::DestroyPools(NGNLoader::NGNImage* ngnImage)
	{
		if (ngnImage->scalerEntryPool)
			free(ngnImage->scalerEntryPool);

		if (ngnImage->portalStatePool)
			free(ngnImage->portalStatePool);

		if (ngnImage->portalHashTable)
			free(ngnImage->portalHashTable);

		ngnImage->scalerEntryPool = 0;
		ngnImage->portalStatePool = 0;
		ngnImage->portalHashTable = 0;
		ngnImage->scalerEntryCount = 0;
		ngnImage->maxScalerEntries = 0;
		ngnImage->portalEntryCount = 0;
		ngnImage->areaPortalCount = 0;
		ngnImage->bucketCount = 0;
	}

	// FUNCTION: TOY2 0x004C32B0
	void NGNImage::Destroy(NGNLoader::NGNImage* ngnImage)
	{
		if (ngnImage)
		{
			for (int32_t i = 0; i < 64; i++)
			{
				if (ngnImage->textureEntries[i].auxiliaryData)
					free((void*)ngnImage->textureEntries[i].auxiliaryData);
			}

			if (ngnImage->creatureData)
			{
				for (int32_t creatureIndex = 0; creatureIndex < ngnImage->creatureCount; ++creatureIndex)
				{
					if (ngnImage->creatureData[creatureIndex])
						Nu3D::Creature::Destroy(ngnImage->creatureData[creatureIndex]);
				}

				free(ngnImage->creatureData);
			}

			if (ngnImage->primitives)
			{
				for (int32_t primIndex = 0; primIndex < ngnImage->primCount; ++primIndex)
					Nu3D::Primitive::Destroy(ngnImage->primitives[primIndex]);

				free(ngnImage->primitives);
			}

			for (int32_t gscaleType = 0; gscaleType < ngnImage->gscaleType; ++gscaleType)
			{
				// Dynamic Scalers are free'd here
				if (ngnImage->dynamicScalers[gscaleType])
					free(ngnImage->dynamicScalers[gscaleType]);

				if (ngnImage->spacialGrid[gscaleType])
					free(ngnImage->spacialGrid[gscaleType]);
			}

			if (ngnImage->areaPortals)
			{
				for (int32_t portal = 0; portal < ngnImage->actualPortalCount; ++portal)
					NGNImage::DestroyPortal(ngnImage->areaPortals[portal]);

				free(ngnImage->areaPortals);
			}

			if (ngnImage->links)
				free(ngnImage->links);

			if (ngnImage->portalHashTable)
				NGNImage::DestroyPools(ngnImage);

			free(ngnImage);
		}
	}
}

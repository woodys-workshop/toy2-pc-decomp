#pragma once

#include "Common.h"
#include "Toy2/Actor.h"


namespace NGNLoader
{
	struct NGNImage;
}

namespace Nu3D
{
	namespace Scene
	{
		enum RenderLayerFlags
		{
			RENDERLAYER_FAR_FORCE_SCAN = 0x1,
			RENDERLAYER_NEAR_FORCE_SCAN = 0x2,
			RENDERLAYER_INFINITE_VIEWPORT = 0x3,
		};

		void RenderWorldGeometry(int32_t sectorIndex, RenderLayerFlags layerFlags);
		void RenderActors(Toy2::Actor::Toy2Actor** activeActorList);
		int32_t RenderCellsInRadius(int32_t radius, int32_t layer, NGNLoader::NGNImage* ngnImage);
	}
}

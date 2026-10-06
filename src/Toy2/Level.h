#pragma once

#include "Common.h"

namespace Toy2
{
	namespace Levels
	{
		struct InstanceSection;
	}
}

namespace Toy2
{
	namespace Level
	{
		void FixupRecordCoordinates();
		void LoaderHelper();
		int32_t PolyCountHelper(int32_t* a1, Toy2::Levels::InstanceSection* a2);
		int32_t CalculatePolyCount(Toy2::Levels::InstanceSection* secondData);
		void UpdateAmbientEmitters();
	}
}

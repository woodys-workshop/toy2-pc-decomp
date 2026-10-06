#pragma once

#include "Common.h"
#include "Toy2/Levels.h"

namespace Toy2
{
	namespace Level
	{
		void FixupRecordCoordinates();
		void LoaderHelper();
		int32_t CalculatePolyCount(Toy2::Levels::InstanceSection* secondData);
		void UpdateAmbientEmitters();
	}
}

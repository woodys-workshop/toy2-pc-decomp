#pragma once

#include "Common.h"
#include "Toy2/Levels.h"

namespace Toy2
{
	namespace Level
	{
		extern int32_t g_unk830E28;

		void FixupRecordCoordinates();
		void LoaderHelper();
		int32_t CalculatePolyCount(Toy2::Levels::InstanceSection* secondData);
		void UpdateAmbientEmitters();
	}
}

#pragma once

#include "Common.h"

struct Vector3I;


namespace Toy2
{
	namespace KiteTail
	{
		void Draw();

		uint32_t* Init(Vector3I* a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5);
		void Activate(int32_t a1);
		void Deactivate(int32_t a1);
		void Simulate(Vector3I* anchor, int32_t slot);
	}
}

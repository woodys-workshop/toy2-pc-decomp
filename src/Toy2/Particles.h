#pragma once

#include "Common.h"

namespace Nu3D
{
	namespace Particles
	{
		struct ParticleInstance;
	}
}

struct Vector3I;

namespace Toy2
{
	namespace Actor
	{
		struct Toy2Actor;
	}
}

namespace Toy2
{
	namespace Particles
	{
		void SpawnCollectSparkle(int32_t x, int32_t y, int32_t z, int32_t spread);
		Nu3D::Particles::ParticleInstance* SpawnBurstRingAtPoint(Vector3I* pos, uint8_t r, uint8_t g, uint8_t b, int32_t yOffset);
		void SpawnBurstRingAtActor(Toy2::Actor::Toy2Actor* actor, uint8_t r, uint8_t g, uint8_t b, int32_t yOffset);
	}
}

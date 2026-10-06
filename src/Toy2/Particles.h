#pragma once

#include "Common.h"
#include "Nu3D/Particles.h"
#include "Numerics.h"
#include "Toy2/Actor.h"

namespace Toy2
{
	namespace Particles
	{
		void SpawnCollectSparkle(int32_t x, int32_t y, int32_t z, int32_t spread);
		Nu3D::Particles::ParticleInstance* SpawnBurstRingAtPoint(Vector3I* pos, uint8_t r, uint8_t g, uint8_t b, int32_t yOffset);
		void SpawnBurstRingAtActor(Toy2::Actor::Toy2Actor* actor, uint8_t r, uint8_t g, uint8_t b, int32_t yOffset);
	}
}

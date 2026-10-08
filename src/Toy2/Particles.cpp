#include "Toy2/Particles.h"
#include "Toy2/Camera.h"
#include "Toy2/Lighting.h"
#include "Random.h"
#include "Numerics.h"

namespace Toy2
{
	namespace Particles
	{
		// FUNCTION: TOY2 0x00410410
		void SpawnCollectSparkle(int32_t x, int32_t y, int32_t z, int32_t spread)
		{
			bool lastIteration;

			spread = 2 * spread;
			int32_t iterations = 5;

			do
			{
				int32_t rand = *g_randDatBufferPtr & 0x3F;
				g_randDatBufferPtr += 3;

				Nu3D::Particles::ParticleInstance* particle =
					Nu3D::Particles::SpawnFromPreset(x + spread * (rand - 32), spread * (rand - 32) + y, spread * (rand - 32) + z, 41, 15);

				particle->rotSpeed = *g_randDatBufferPtr++ - 128;
				particle->lifetime = 2 * (*g_randDatBufferPtr & 0xF) + 24;

				lastIteration = iterations == 1;
				++g_randDatBufferPtr;

				--iterations;
			} while (! lastIteration);

			Toy2::Lighting::SpawnLight(x, y, z, 0x604000, 16, x);
		}

		// FUNCTION: TOY2 0x00410540
		Nu3D::Particles::ParticleInstance* SpawnBurstRingAtPoint(Vector3I* pos, uint8_t r, uint8_t g, uint8_t b, int32_t yOffset)
		{
			int32_t zFactor = Numerics::g_sinCosLUT[Camera::g_activeCameraTransform.angles.yaw & 0xFFF];
			int32_t xFactor = Numerics::g_sinCosLUT[(Camera::g_activeCameraTransform.angles.yaw + 1024) & 0xFFF];
			int16_t flags = 0;
			int32_t lutIdx = 0;

			do
			{
				int32_t ringSin = Numerics::g_sinCosLUT[lutIdx];
				int32_t ringCos = Numerics::g_sinCosLUT[(flags + 1024) & 0xFFF] / 4;
				int32_t velY = *g_randDatBufferPtr++ - 128;

				Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnInstance(
					((xFactor * (ringSin / 4)) >> 14) + pos->x,
					ringCos + pos->y - yOffset,
					(-(zFactor * (ringSin / 4)) >> 14) + pos->z,
					(xFactor * (ringSin / 4)) >> 16,
					(ringCos >> 2) - 1024,
					-(zFactor * (ringSin / 4)) >> 16,
					64,
					flags,
					velY,
					125);

				lutIdx += 512;
				flags += 512;

				particle->lifetime = (*g_randDatBufferPtr++ & 0x3F) + 80;
				particle->colourR = r;
				particle->colourG = g;
				particle->colourB = b;
			} while (lutIdx < 4096);

			Nu3D::Particles::SpawnInstance(pos->x, pos->y - yOffset, pos->z, 0, -1024, 0, 64, 0, 0, 126);

			return Nu3D::Particles::SpawnInstance(pos->x, pos->y - yOffset, pos->z, 0, -1024, 0, 64, 0, 0, 127);
		}

		// FUNCTION: TOY2 0x004106C0
		void SpawnBurstRingAtActor(Toy2::Actor::Toy2Actor* actor, uint8_t r, uint8_t g, uint8_t b, int32_t yOffset)
		{
			int32_t zFactor = Numerics::g_sinCosLUT[Camera::g_activeCameraTransform.angles.yaw & 0xFFF];
			int32_t xFactor = Numerics::g_sinCosLUT[(Camera::g_activeCameraTransform.angles.yaw + 1024) & 0xFFF];
			int16_t flags = 0;
			int32_t lutIdx = 0;

			do
			{
				int32_t ringSin = Numerics::g_sinCosLUT[lutIdx];
				int32_t ringCos = Numerics::g_sinCosLUT[(flags + 1024) & 0xFFF];
				int32_t velY = *g_randDatBufferPtr++ - 128;

				Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnInstance(
					((xFactor * (ringSin / 4)) >> 14) + actor->pos.x,
					actor->pos.y + ringCos / 4 - yOffset,
					actor->pos.z + (-(zFactor * (ringSin / 4)) >> 14),
					(xFactor * (ringSin / 4)) >> 17,
					((ringCos / 4) >> 3) - 1024,
					-(zFactor * (ringSin / 4)) >> 17,
					64,
					flags,
					velY,
					125);

				lutIdx += 819;
				flags += 819;

				particle->lifetime = (*g_randDatBufferPtr++ & 0x1F) + 50;
				particle->colourR = r;
				particle->colourG = g;
				particle->colourB = b;
			} while (lutIdx < 4095);

			Nu3D::Particles::SpawnInstance(actor->pos.x, actor->pos.y - yOffset, actor->pos.z, 0, -1024, 0, 64, 0, 0, 126);
			Nu3D::Particles::SpawnInstance(actor->pos.x, actor->pos.y - yOffset, actor->pos.z, 0, -1024, 0, 64, 0, 0, 127);
		}
	}
}

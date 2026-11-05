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
			spread = 2 * spread;
			int32_t iterations = 5;

			do
			{
				int32_t rand = *g_randDatBufferPtr & 0x3F;
				int32_t offset = spread * (rand - 32);
				g_randDatBufferPtr += 3;

				Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnFromPreset(x + offset, y + offset, z + offset, 41, 15);

				particle->rotSpeed = *g_randDatBufferPtr++ - 128;
				particle->lifetime = 2 * (*g_randDatBufferPtr & 0xF) + 24;

				++g_randDatBufferPtr;

				--iterations;
			} while (iterations);

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
				int32_t ringSin = Numerics::g_sinCosLUT[lutIdx] / 4;
				int32_t offsetX = (xFactor * ringSin) >> 14;
				int32_t offsetZ = -(zFactor * ringSin) >> 14;
				int32_t ringCos = Numerics::g_sinCosLUT[(flags + 1024) & 0xFFF] / 4;
				int32_t velY = *g_randDatBufferPtr++ - 128;

				Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnInstance(
					offsetX + pos->x, ringCos + pos->y - yOffset, offsetZ + pos->z, offsetX >> 2, (ringCos >> 2) - 1024, offsetZ >> 2, 64, flags, velY, 125);

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
		void SpawnBurstRingAtActor(Toy2::CreatureActor* actor, uint8_t r, uint8_t g, uint8_t b, int32_t yOffset)
		{
			int32_t zFactor = Numerics::g_sinCosLUT[Camera::g_activeCameraTransform.angles.yaw & 0xFFF];
			int32_t xFactor = Numerics::g_sinCosLUT[(Camera::g_activeCameraTransform.angles.yaw + 1024) & 0xFFF];

			for (int32_t lutIdx = 0; lutIdx < 4095; lutIdx += 819)
			{
				int32_t ringSin = Numerics::g_sinCosLUT[lutIdx] / 4;
				int32_t offsetZ = -(zFactor * ringSin) >> 14;
				int32_t offsetX = (xFactor * ringSin) >> 14;
				int32_t ringCos = Numerics::g_sinCosLUT[(lutIdx + 1024) & 0xFFF] / 4;
				int32_t velY = *g_randDatBufferPtr++ - 128;

				Nu3D::Particles::ParticleInstance* particle = Nu3D::Particles::SpawnInstance(
					offsetX + actor->transform.pos.x,
					actor->transform.pos.y + ringCos - yOffset,
					actor->transform.pos.z + offsetZ,
					offsetX >> 3,
					(ringCos >> 3) - 1024,
					offsetZ >> 3,
					64,
					lutIdx,
					velY,
					125);

				particle->lifetime = (*g_randDatBufferPtr++ & 0x1F) + 50;
				particle->colourR = r;
				particle->colourG = g;
				particle->colourB = b;
			}

			Nu3D::Particles::SpawnInstance(actor->transform.pos.x, actor->transform.pos.y - yOffset, actor->transform.pos.z, 0, -1024, 0, 64, 0, 0, 126);
			Nu3D::Particles::SpawnInstance(actor->transform.pos.x, actor->transform.pos.y - yOffset, actor->transform.pos.z, 0, -1024, 0, 64, 0, 0, 127);
		}
	}
}

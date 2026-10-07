#include "Nu3D/Particles.h"
#include <string.h>

namespace Nu3D
{
	namespace Particles
	{
		// GLOBAL: TOY2 0x00529E58;
		ParticleInstance g_particleInstances[64];

		// GLOBAL: TOY2 0x0052AD90
		int32_t g_nextParticleSlotIndex;

		// FUNCTION: TOY2 0x0040FAC0
		void Init()
		{
			memset(g_particleInstances, 0, sizeof(g_particleInstances));
			g_nextParticleSlotIndex = 0;
		}

		// STUB: TOY2 0x0040FAE0
		Nu3D::Particles::ParticleInstance* SpawnInstance(
			int32_t spawnX,
			int32_t spawnY,
			int32_t spawnZ,
			int32_t velX,
			int32_t velY,
			int32_t velZ,
			int32_t scale,
			int16_t flags,
			int32_t secondaryParam,
			int32_t particleTypeIndex)
		{
			return 0;
		}

		// STUB: TOY2 0x0040FDF0
		Nu3D::Particles::ParticleInstance* SpawnFromPreset(int32_t spawnX, int32_t spawnY, int32_t spawnZ, int32_t particleIndex, int32_t presetIndex)
		{
			return 0;
		}

		// STUB: TOY2 0x00410850
		void SpawnBreakBurst(Nu3D::Particles::ParticleInstance* a2, uint8_t flag) {}

		// STUB: TOY2 0x00410B80
		void DeathEffect(Nu3D::Particles::ParticleInstance* particle) {}

		// STUB: TOY2 0x00410F40
		void Update() {}

		// STUB: TOY2 0x00425AD0
		void ReflectWallsSquareArena(Nu3D::Particles::ParticleInstance* particle) {}

		// STUB: TOY2 0x0042B090
		void UpdateArenaBounce(Nu3D::Particles::ParticleInstance* particle) {}

		// STUB: TOY2 0x0042B250
		void UpdateDrainTrail(Nu3D::Particles::ParticleInstance* a1) {}

		// STUB: TOY2 0x00446FA0
		void UpdateUnkShort6(Nu3D::Particles::ParticleInstance* instance) {}
	}
}
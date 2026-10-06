#pragma once

#include "Numerics.h"
#include "Common.h"

namespace Nu3D
{
	namespace Particles
	{
		struct ParticleInstance
		{
			Vector3I pos;
			int32_t velX;
			int32_t velY;
			int32_t velZ;
			int32_t yawAngle;
			int32_t groundHeightY;
			int32_t pitchAngle;
			int16_t lifetime;
			int16_t width;
			int16_t height;
			uint8_t unkByte1;
			uint8_t unkByte2;
			uint8_t spriteSheet;
			uint8_t typeId;
			uint8_t collisionFlags;
			uint8_t updateMode;
			uint8_t unkByte7;
			uint8_t tileIndex;
			int16_t renderFlags;
			int16_t groundAlignRot;
			int16_t rotSpeed;
			uint8_t colourR;
			uint8_t colourG;
			uint8_t colourB;
			uint8_t unkByte12;
		};

		void Init();
		Nu3D::Particles::ParticleInstance* SpawnInstance(int32_t spawnX, int32_t spawnY, int32_t spawnZ, int32_t velX, int32_t velY, int32_t velZ, int32_t scale, int16_t flags, int32_t secondaryParam, int32_t particleTypeIndex);
		Nu3D::Particles::ParticleInstance* SpawnFromPreset(int32_t spawnX, int32_t spawnY, int32_t spawnZ, int32_t particleIndex, int32_t presetIndex);
		void Update();

        extern ParticleInstance g_particleInstances[64];

	    STATIC_ASSERT(sizeof(ParticleInstance) == 0x3C);
	}
}
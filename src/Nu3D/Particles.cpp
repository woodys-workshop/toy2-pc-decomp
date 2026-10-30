#include "Nu3D/Particles.h"
#include "Toy2/Camera.h"
#include "Toy2/Toy2.h"
#include "Toy2/Lighting.h"
#include "Toy2/Levels.h"
#include "AudioManager/AudioManager.h"
#include "Random.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shadows.h"
#include "Renderer/LensFlare.h"
#include "Nu3D/Collision.h"
#include "Nu3D/Math.h"
#include "Nu3D/Link.h"
#include "Toy2/Actor.h"
#include "Toy2/Buzz.h"
#include "Numerics.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

namespace Nu3D
{
	namespace Particles
	{
		struct ParticleTypeDef
		{
			uint8_t spriteSheet;
			int8_t unkByte2;
			int16_t lifetime;
			uint8_t unkByte3;
			uint8_t collisionFlags;
			int16_t unkByte4;
			int16_t unkByte5;
			int16_t renderFlags;
			uint8_t updateMode;
			uint8_t colorR;
			uint8_t colorG;
			uint8_t colorB;
		};

		struct ParticleSpawnPreset
		{
			int32_t randModeMask;
			int32_t velXRange;
			int32_t velYRange;
			int32_t velZRange;
			int32_t scaleRange;
			int16_t spawnFlags;
			int16_t spawnOffset;
		};

		struct ParticleWrapper
		{
			ParticleTypeDef typeDefs[128];
			ParticleSpawnPreset presets[28];
		};

		STATIC_ASSERT(sizeof(ParticleTypeDef) == 0x10);
		STATIC_ASSERT(sizeof(ParticleSpawnPreset) == 0x18);
		STATIC_ASSERT(sizeof(ParticleWrapper) == 0xAA0);

		// GLOBAL: TOY2 0x00529E58
		ParticleInstance g_particleInstances[64];

		// GLOBAL: TOY2 0x0052AD90
		int32_t g_nextParticleSlotIndex;

		// GLOBAL: TOY2 0x00528168
		ParticleInstance g_emptyParticleStruct;

		// GLOBAL: TOY2 0x004EC160
		ParticleWrapper g_particleWrapper = {
			{
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 0
				{ 4, 100, 16, 1, 0, 100, 100, 32, 4, 128, 32, 0 }, // 1
				{ 1, 2, 16, 8, 0, 100, 100, 32, 2, 48, 32, 16 }, // 2
				{ 1, 2, 16, 8, 0, 50, 50, 64, 2, 64, 96, 128 }, // 3
				{ 6, 100, 8, 1, 0, 10, 10, 32, 5, 255, 128, 0 }, // 4
				{ 4, 120, 128, 1, 0, 30, 30, 192, 6, 32, 48, 64 }, // 5
				{ 5, 100, 200, 1, 0, 200, 200, 32, 2, 96, 32, 0 }, // 6
				{ 5, 100, 200, 1, 0, 150, 150, 32, 2, 96, 96, 64 }, // 7
				{ 8, 100, 200, 1, 0, 150, 150, 416, 0, 0, 255, 0 }, // 8
				{ 5, 100, 200, 1, 0, 75, 75, 416, 0, 128, 0, 0 }, // 9
				{ 5, 100, 16, 1, 0, 200, 200, 32, 7, 128, 128, 0 }, // 10
				{ 5, 100, 16, 1, 0, 200, 200, 32, 7, 128, 32, 0 }, // 11
				{ 21, 100, 200, 1, 1, 150, 150, 419, 8, 0, 64, 128 }, // 12
				{ 24, 2, 12, 6, 0, 70, 70, 32, 2, 0, 64, 128 }, // 13
				{ 22, 94, 50, 1, 0, 250, 250, 178, 2, 0, 64, 128 }, // 14
				{ 19, 100, 24, 1, 0, 110, 110, 32, 9, 48, 56, 64 }, // 15
				{ 1, 2, 16, 8, 0, 200, 200, 32, 10, 64, 32, 255 }, // 16
				{ 19, 100, 32, 1, 0, 110, 110, 32, 9, 64, 80, 96 }, // 17
				{ 17, 6, 16, 1, 0, 110, 110, 48, 11, 64, 0, 96 }, // 18
				{ 17, 5, 18, 1, 0, 10, 10, 48, 11, 16, 96, 32 }, // 19
				{ 17, 100, 12, 1, 0, 270, 270, 48, 12, 32, 128, 48 }, // 20
				{ 17, 100, 12, 1, 0, 270, 270, 48, 12, 128, 0, 170 }, // 21
				{ 17, 100, 8, 1, 0, 270, 270, 48, 13, 48, 192, 72 }, // 22
				{ 17, 100, 8, 1, 0, 270, 270, 48, 13, 192, 0, 255 }, // 23
				{ 2, 4, 16, 4, 0, 100, 50, 32, 2, 128, 72, 32 }, // 24
				{ 0, 100, 16, 1, 0, 256, 256, 386, 0, 255, 128, 0 }, // 25
				{ 5, 100, 24, 1, 0, 200, 200, 432, 14, 128, 64, 0 }, // 26
				{ 7, 2, 16, 1, 0, 70, 70, 48, 11, 0, 24, 24 }, // 27
				{ 7, 4, 16, 1, 0, 70, 70, 48, 11, 0, 48, 48 }, // 28
				{ 24, 2, 12, 6, 0, 70, 70, 0, 0, 48, 32, 64 }, // 29
				{ 24, 2, 12, 6, 0, 70, 70, 0, 0, 0, 128, 32 }, // 30
				{ 18, 100, 32, 1, 0, 60, 30, 48, 2, 0, 64, 0 }, // 31
				{ 18, 100, 32, 1, 0, 60, 30, 48, 2, 0, 64, 64 }, // 32
				{ 18, 100, 32, 1, 0, 60, 30, 80, 2, 64, 64, 64 }, // 33
				{ 0, 100, 60, 1, 0, 180, 180, 386, 0, 255, 128, 0 }, // 34
				{ 5, 100, 60, 1, 3, 180, 180, 161, 16, 255, 128, 0 }, // 35
				{ 19, 100, 16, 1, 0, 120, 120, 32, 2, 192, 64, 0 }, // 36
				{ 19, 100, 16, 1, 0, 200, 200, 32, 5, 192, 192, 0 }, // 37
				{ 5, 100, 60, 1, 3, 120, 120, 419, 17, 0, 32, 255 }, // 38
				{ 7, 1, 16, 1, 0, 80, 80, 32, 15, 0, 32, 128 }, // 39
				{ 7, 8, 16, 1, 0, 50, 50, 48, 46, 0, 32, 128 }, // 40
				{ 5, 100, 16, 1, 0, 100, 100, 32, 2, 255, 128, 0 }, // 41
				{ 19, 100, 16, 1, 0, 110, 110, 32, 9, 80, 64, 48 }, // 42
				{ 5, 100, 60, 1, 0, 80, 80, 160, 18, 128, 96, 0 }, // 43
				{ 5, 5, 20, 1, 0, 50, 50, 32, 5, 192, 128, 32 }, // 44
				{ 21, 100, 200, 1, 229, 50, 50, 32, 19, 32, 48, 64 }, // 45
				{ 19, 2, 16, 1, 0, 60, 60, 32, 20, 192, 192, 0 }, // 46
				{ 5, 100, 100, 1, 0, 110, 110, 416, 21, 128, 128, 0 }, // 47
				{ 23, 60, 200, 1, 0, 8, 8, 416, 22, 0, 255, 0 }, // 48
				{ 17, 100, 64, 1, 0, 500, 500, 208, 2, 32, 32, 32 }, // 49
				{ 6, 100, 24, 1, 0, 10, 60, 96, 0, 0, 128, 0 }, // 50
				{ 7, 1, 48, 1, 0, 70, 70, 48, 11, 24, 22, 20 }, // 51
				{ 7, 1, 24, 1, 0, 70, 70, 48, 11, 48, 44, 40 }, // 52
				{ 24, 2, 12, 6, 0, 70, 70, 32, 2, 96, 88, 80 }, // 53
				{ 18, 100, 32, 1, 0, 60, 30, 48, 2, 96, 88, 80 }, // 54
				{ 52, 100, 200, 1, 2, 150, 150, 480, 23, 128, 128, 128 }, // 55
				{ 22, 90, 80, 1, 0, 250, 250, 178, 2, 0, 128, 32 }, // 56
				{ 7, 6, 16, 1, 0, 70, 70, 48, 11, 0, 48, 48 }, // 57
				{ 19, 100, 24, 1, 0, 110, 110, 32, 9, 96, 80, 64 }, // 58
				{ 37, 100, 40, 1, 0, 150, 150, 480, 0, 128, 128, 128 }, // 59
				{ 6, 2, 12, 1, 0, 96, 5, 32, 24, 0, 64, 128 }, // 60
				{ 16, 1, 200, 12, 0, 70, 70, 482, 25, 128, 128, 128 }, // 61
				{ 24, 2, 12, 6, 0, 70, 70, 32, 2, 0, 64, 0 }, // 62
				{ 5, 100, 60, 1, 6, 250, 250, 426, 26, 0, 128, 0 }, // 63
				{ 21, 1, 24, 1, 0, 60, 60, 32, 33, 0, 128, 0 }, // 64
				{ 19, 100, 24, 1, 0, 110, 110, 32, 9, 0, 32, 0 }, // 65
				{ 19, 100, 16, 1, 0, 200, 200, 32, 2, 64, 64, 64 }, // 66
				{ 52, 100, 200, 1, 7, 150, 150, 482, 28, 128, 128, 128 }, // 67
				{ 1, 2, 32, 8, 0, 400, 400, 32, 31, 96, 32, 0 }, // 68
				{ 0, 100, 60, 1, 0, 300, 300, 386, 0, 255, 128, 0 }, // 69
				{ 4, 32, 64, 1, 0, 75, 75, 208, 3, 48, 48, 48 }, // 70
				{ 28, 100, 47, 1, 0, 75, 40, 488, 29, 128, 128, 128 }, // 71
				{ 28, 100, 40, 1, 0, 75, 40, 480, 30, 128, 128, 128 }, // 72
				{ 4, 100, 16, 1, 0, 100, 100, 32, 4, 64, 128, 64 }, // 73
				{ 5, 100, 16, 1, 0, 200, 200, 32, 7, 0, 128, 0 }, // 74
				{ 28, 1, 16, 1, 0, 80, 40, 32, 37, 0, 48, 0 }, // 75
				{ 5, 100, 15, 1, 0, 200, 200, 418, 32, 96, 112, 128 }, // 76
				{ 5, 4, 12, 1, 0, 150, 150, 32, 33, 96, 112, 128 }, // 77
				{ 17, 2, 20, 1, 0, 550, 550, 32, 34, 48, 48, 48 }, // 78
				{ 17, 4, 20, 1, 0, 550, 550, 48, 34, 48, 48, 48 }, // 79
				{ 51, 100, 210, 1, 9, 250, 250, 482, 35, 64, 64, 64 }, // 80
				{ 17, 100, 12, 1, 0, 40, 40, 48, 36, 32, 128, 64 }, // 81
				{ 17, 100, 12, 1, 0, 40, 40, 48, 36, 128, 0, 170 }, // 82
				{ 17, 100, 12, 1, 0, 40, 40, 48, 36, 170, 0, 0 }, // 83
				{ 52, 99, 100, 1, 7, 150, 150, 482, 28, 128, 128, 128 }, // 84
				{ 19, 4, 16, 1, 0, 20, 20, 32, 20, 192, 192, 0 }, // 85
				{ 52, 100, 60, 1, 169, 100, 100, 482, 38, 64, 64, 64 }, // 86
				{ 1, 2, 16, 8, 0, 150, 150, 32, 2, 64, 64, 64 }, // 87
				{ 21, 100, 32, 1, 0, 75, 75, 32, 2, 60, 70, 80 }, // 88
				{ 0, 100, 60, 1, 0, 200, 200, 426, 39, 254, 128, 0 }, // 89
				{ 19, 1, 32, 1, 0, 200, 200, 32, 40, 192, 192, 0 }, // 90
				{ 23, 60, 2, 1, 0, 100, 100, 97, 0, 255, 255, 255 }, // 91
				{ 4, 100, 60, 1, 8, 100, 100, 483, 16, 64, 64, 64 }, // 92
				{ 5, 100, 60, 1, 0, 200, 200, 419, 32, 96, 112, 128 }, // 93
				{ 5, 100, 16, 1, 0, 100, 100, 32, 2, 128, 128, 128 }, // 94
				{ 51, 100, 32, 1, 0, 100, 100, 96, 0, 60, 70, 80 }, // 95
				{ 52, 100, 60, 1, 169, 100, 100, 483, 0, 128, 128, 0 }, // 96
				{ 4, 100, 80, 1, 169, 30, 30, 483, 41, 64, 64, 64 }, // 97
				{ 1, 2, 16, 8, 0, 30, 30, 32, 2, 64, 64, 64 }, // 98
				{ 31, 64, 100, 1, 0, 100, 100, 224, 42, 128, 128, 128 }, // 99
				{ 19, 100, 16, 1, 0, 50, 50, 32, 9, 64, 80, 96 }, // 100
				{ 1, 2, 32, 8, 0, 250, 250, 32, 31, 96, 32, 0 }, // 101
				{ 53, 100, 60, 1, 3, 350, 175, 442, 43, 0, 28, 112 }, // 102
				{ 53, 2, 16, 1, 0, 250, 125, 32, 27, 0, 32, 128 }, // 103
				{ 54, 100, 40, 1, 0, 200, 200, 418, 44, 128, 32, 0 }, // 104
				{ 55, 1, 24, 1, 0, 75, 200, 80, 24, 64, 64, 64 }, // 105
				{ 52, 1, 24, 1, 0, 200, 300, 80, 2, 64, 64, 64 }, // 106
				{ 53, 4, 24, 1, 0, 400, 200, 48, 45, 0, 32, 128 }, // 107
				{ 52, 100, 200, 1, 169, 50, 50, 482, 47, 128, 128, 0 }, // 108
				{ 52, 98, 100, 1, 169, 50, 50, 490, 48, 0, 128, 0 }, // 109
				{ 52, 1, 16, 1, 0, 50, 50, 32, 2, 32, 32, 0 }, // 110
				{ 52, 1, 16, 1, 0, 50, 50, 32, 2, 0, 32, 0 }, // 111
				{ 17, 4, 20, 1, 0, 550, 550, 48, 34, 0, 48, 0 }, // 112
				{ 36, 100, 16, 1, 0, 150, 150, 96, 0, 128, 128, 128 }, // 113
				{ 5, 98, 100, 1, 3, 100, 100, 426, 49, 128, 32, 0 }, // 114
				{ 37, 100, 16, 1, 0, 150, 150, 96, 0, 128, 128, 128 }, // 115
				{ 7, 100, 40, 1, 0, 220, 220, 178, 50, 128, 0, 0 }, // 116
				{ 5, 118, 120, 1, 0, 100, 100, 416, 51, 128, 64, 0 }, // 117
				{ 19, 100, 16, 1, 0, 50, 50, 32, 2, 128, 0, 0 }, // 118
				{ 55, 100, 200, 1, 10, 100, 100, 419, 52, 128, 128, 128 }, // 119
				{ 53, 94, 800, 1, 0, 250, 250, 434, 2, 128, 128, 128 }, // 120
				{ 5, 122, 40, 1, 11, 100, 100, 416, 51, 128, 128, 0 }, // 121
				{ 19, 100, 16, 1, 0, 75, 75, 64, 2, 64, 96, 128 }, // 122
				{ 53, 100, 32, 1, 0, 200, 200, 464, 2, 48, 48, 48 }, // 123
				{ 1, 2, 16, 8, 0, 200, 200, 162, 2, 64, 48, 32 }, // 124
				{ 39, 32, 100, 1, 0, 256, 256, 224, 53, 128, 128, 128 }, // 125
				{ 7, 12, 24, 1, 0, 50, 50, 160, 11, 0, 32, 128 }, // 126
				{ 7, 12, 24, 1, 0, 50, 50, 176, 11, 0, 32, 128 }, // 127
			},
			{
				{ 0x1010, 63, 0, 63, -16, 0, 0 }, // 1
				{ 0x0, 0, 0, 0, 0, 0, 0 }, // 2
				{ 0x1010, 31, -512, 31, 0, 0, 0 }, // 3
				{ 0x1510, 127, 2175, 127, 64, 0, 0 }, // 4
				{ 0x0, 0, 0, 0, 0, 512, -48 }, // 5
				{ 0x0, 0, 0, 0, 0, 0, -12 }, // 6
				{ 0x1510, 127, 4351, 127, 64, 0, 0 }, // 7
				{ 0x1010, 255, -2048, 255, 128, 0, 0 }, // 8
				{ 0x1510, 127, 3711, 127, 128, 0, 0 }, // 9
				{ 0x1010, 15, -512, 15, -16, 0, 0 }, // 10
				{ 0x0, 0, 0, 0, 0, 0, -128 }, // 11
				{ 0x0, 0, 0, 0, 0, 0, -192 }, // 12
				{ 0x3510, 255, 3711, 127, 128, 0, 0 }, // 13
				{ 0x1510, 255, 5887, 255, 128, 0, 0 }, // 14
				{ 0x1110, 31, 31, 31, 0, 0, 0 }, // 15
				{ 0x0, 0, -512, 0, 32, 0, 0 }, // 16
				{ 0x1510, 255, 3711, 255, 128, 0, 0 }, // 17
				{ 0x0, 0, -256, 0, -8, 0, 0 }, // 18
				{ 0x0, 0, 0, 0, 192, 0, 0 }, // 19
				{ 0x1010, 63, -256, 63, -16, 0, 0 }, // 20
				{ 0x1010, 31, -1280, 31, -64, 0, 0 }, // 21
				{ 0x110, -2560, 31, 31, -8, 0, 0 }, // 22
				{ 0x110, 2560, 31, 31, -8, 0, 0 }, // 23
				{ 0x0, 0, -3072, -1536, 256, 0, 0 }, // 24
				{ 0x0, 4096, -2048, 0, 160, 0, 0 }, // 25
				{ 0x1010, 31, -512, 31, -16, 0, 0 }, // 26
				{ 0x1010, 31, -512, 31, 16, 0, 0 }, // 27
				{ 0x0, 0, 320, 0, 0, 0, 0 }, // 28
			}
		};

		// FUNCTION: TOY2 0x0040FAC0
		void Init()
		{
			memset(g_particleInstances, 0, sizeof(g_particleInstances));
			g_nextParticleSlotIndex = 0;
		}

		// FUNCTION: TOY2 0x00446FA0
		void UpdateUnkShort6(Nu3D::Particles::ParticleInstance* instance)
		{
			int8_t renderFlags = (int8_t)instance->renderFlags;

			instance->unkByte12 = 44;

			if ((renderFlags & 0x60) != 0x60)
				instance->unkByte12 = 46;
		}

		// FUNCTION: TOY2 0x0040FAE0
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
			int16_t lifetime;
			int8_t unkByte2;
			int16_t renderFlags;

			int32_t distanceLimit = 640000;

			if (particleTypeIndex == 55 || particleTypeIndex == 67 || particleTypeIndex == 80)
				distanceLimit = 1638400;

			if (((Toy2::Camera::g_activeCameraTransform.pos.x - spawnX) >> 8) * ((Toy2::Camera::g_activeCameraTransform.pos.x - spawnX) >> 8)
					+ ((Toy2::Camera::g_activeCameraTransform.pos.y - spawnY) >> 8) * ((Toy2::Camera::g_activeCameraTransform.pos.y - spawnY) >> 8)
					+ ((Toy2::Camera::g_activeCameraTransform.pos.z - spawnZ) >> 8) * ((Toy2::Camera::g_activeCameraTransform.pos.z - spawnZ) >> 8)
				>= distanceLimit)
				return &g_emptyParticleStruct;

			int32_t newIndex = -1;
			int32_t testIndex = 0;
			int32_t particleIndex = g_nextParticleSlotIndex + 1;
			bool inRange = g_nextParticleSlotIndex + 1 < 64;
			int32_t lifetimeMax = 9999;

			++g_nextParticleSlotIndex;

			if (! inRange)
			{
				particleIndex = 0;
				g_nextParticleSlotIndex = 0;
			}

			int32_t idx = particleIndex;

			// a slot with bit 7 of the render flags set is in use, look for a free one or the one with the shortest lifetime left
			if ((int8_t)g_particleInstances[particleIndex].renderFlags < 0)
			{
				do
				{
					lifetime = g_particleInstances[idx].lifetime;

					if (! lifetime)
						break;

					if (lifetime < lifetimeMax)
					{
						lifetimeMax = lifetime;
						newIndex = particleIndex;
					}

					if (++testIndex >= 64)
					{
						particleIndex = newIndex;

						break;
					}

					if (++particleIndex >= 64)
						particleIndex = 0;

					idx = particleIndex;
				} while ((int8_t)g_particleInstances[particleIndex].renderFlags < 0);

				g_nextParticleSlotIndex = particleIndex;
			}

			ParticleInstance* instance = &g_particleInstances[particleIndex];

			ParticleTypeDef* typeDef = &g_particleWrapper.typeDefs[particleTypeIndex];

			instance->pos.z = spawnZ;
			instance->pos.x = spawnX;
			instance->pos.y = spawnY;
			instance->velX = velX / 2;
			instance->groundHeightY = 0x80000000;
			instance->velY = velY / 2;
			instance->velZ = velZ / 2;
			instance->yawAngle = scale / 4;
			instance->lifetime = 2 * typeDef->lifetime;
			instance->width = typeDef->unkByte4;
			instance->height = typeDef->unkByte5;

			unkByte2 = typeDef->unkByte2;

			if (unkByte2 >= 0)
			{
				instance->unkByte1 = 2 * unkByte2;
			}
			else
			{
				// quirk: unkByte7 still holds the value of the particle that used this slot before
				instance->unkByte1 = 2 * ((*g_randDatBufferPtr++ & 3) + 127 * unkByte2);
				instance->lifetime = 2 * instance->unkByte1 * instance->unkByte7;
			}

			instance->unkByte2 = instance->unkByte1;
			instance->spriteSheet = typeDef->spriteSheet;
			instance->typeId = particleTypeIndex;
			instance->collisionFlags = typeDef->collisionFlags;
			instance->updateMode = typeDef->updateMode;
			instance->unkByte7 = typeDef->unkByte3;
			instance->tileIndex = 0;

			renderFlags = typeDef->renderFlags;
			renderFlags |= 0x200;

			instance->groundAlignRot = flags;
			instance->renderFlags = renderFlags;
			instance->rotSpeed = secondaryParam / 2;
			instance->colourR = typeDef->colorR;
			instance->colourG = typeDef->colorG;
			instance->colourB = typeDef->colorB;

			UpdateUnkShort6(instance);

			return instance;
		}

		// FUNCTION: TOY2 0x0040FDF0
		Nu3D::Particles::ParticleInstance* SpawnFromPreset(int32_t spawnX, int32_t spawnY, int32_t spawnZ, int32_t particleIndex, int32_t presetIndex)
		{
			int32_t velXRange;
			uint8_t* randByte;
			int32_t velYRange;
			int32_t velYRangeModified;
			int32_t velZRange;
			int32_t velZRangeModified;
			int32_t scaleRange;
			int32_t scaleRangeModified;

			ParticleSpawnPreset* preset = &g_particleWrapper.presets[presetIndex - 1];

			velXRange = preset->velXRange;

			switch ((preset->randModeMask >> 12) & 0xF)
			{
				case 1:
					randByte = g_randDatBufferPtr + 1;
					velXRange = 8 * ((velXRange & *g_randDatBufferPtr++) - (velXRange >> 1));
					break;

				case 2:
					randByte = g_randDatBufferPtr + 1;
					velXRange = 8 * (velXRange & *g_randDatBufferPtr++);
					break;

				case 3:
					randByte = g_randDatBufferPtr + 1;
					velXRange = -8 * (velXRange & *g_randDatBufferPtr++);
					break;

				case 4:
					randByte = g_randDatBufferPtr + 1;
					velXRange = 8 * (((velXRange >> 4) & 0xFF0) + (velXRange & *g_randDatBufferPtr++));
					break;

				case 5:
					randByte = g_randDatBufferPtr + 1;
					velXRange = 8 * ((velXRange & *g_randDatBufferPtr++) - ((velXRange >> 4) & 0xFF0));
					break;

				case 6:
					randByte = g_randDatBufferPtr + 1;
					velXRange = 16 * ((velXRange & *g_randDatBufferPtr++) - (velXRange >> 1));
					break;

				default:
					randByte = g_randDatBufferPtr;
					break;
			}

			velYRange = preset->velYRange;

			switch ((preset->randModeMask >> 8) & 0xF)
			{
				case 1:
					velYRangeModified = 8 * ((velYRange & *randByte) - (velYRange >> 1));
					goto LBL_UPDATE_VELY;

				case 2:
					velYRangeModified = 8 * (velYRange & *randByte);
					goto LBL_UPDATE_VELY;

				case 3:
					velYRangeModified = -8 * (velYRange & *randByte);
					goto LBL_UPDATE_VELY;

				case 4:
					velYRangeModified = 8 * (((velYRange >> 4) & 0xFF0) + (velYRange & *randByte));
					goto LBL_UPDATE_VELY;

				case 5:
					velYRangeModified = 8 * ((velYRange & *randByte) - ((velYRange >> 4) & 0xFF0));
					goto LBL_UPDATE_VELY;

				case 6:
					velYRangeModified = 16 * ((velYRange & *randByte) - (velYRange >> 1));

				LBL_UPDATE_VELY:
					++randByte;
					velYRange = velYRangeModified;
					g_randDatBufferPtr = randByte;

					break;

				default:
					break;
			}

			velZRange = preset->velZRange;

			switch ((preset->randModeMask >> 4) & 0xF)
			{
				case 1:
					velZRangeModified = 8 * ((velZRange & *randByte) - (velZRange >> 1));
					goto LBL_UPDATE_VELZ;

				case 2:
					velZRangeModified = 8 * (velZRange & *randByte);
					goto LBL_UPDATE_VELZ;

				case 3:
					velZRangeModified = -8 * (velZRange & *randByte);
					goto LBL_UPDATE_VELZ;

				case 4:
					velZRangeModified = 8 * (((velZRange >> 4) & 0xFF0) + (velZRange & *randByte));
					goto LBL_UPDATE_VELZ;

				case 5:
					velZRangeModified = 8 * ((velZRange & *randByte) - ((velZRange >> 4) & 0xFF0));
					goto LBL_UPDATE_VELZ;

				case 6:
					velZRangeModified = 16 * ((velZRange & *randByte) - (velZRange >> 1));

				LBL_UPDATE_VELZ:
					++randByte;
					velZRange = velZRangeModified;
					g_randDatBufferPtr = randByte;

					break;

				default:
					break;
			}

			scaleRange = preset->scaleRange;

			switch (preset->randModeMask & 0xF)
			{
				case 1:
					scaleRangeModified = 8 * ((scaleRange & *randByte) - (scaleRange >> 1));
					goto LABEL_32;

				case 2:
					scaleRangeModified = 8 * (scaleRange & *randByte);
					goto LABEL_32;

				case 3:
					scaleRangeModified = -8 * (scaleRange & *randByte);
					goto LABEL_32;

				case 4:
					scaleRangeModified = 8 * (((scaleRange >> 4) & 0xFF0) + (scaleRange & *randByte));
					goto LABEL_32;

				case 5:
					scaleRangeModified = 8 * ((scaleRange & *randByte) - ((scaleRange >> 4) & 0xFF0));
					goto LABEL_32;

				case 6:
					scaleRangeModified = 16 * ((scaleRange & *randByte) - (scaleRange >> 1));

				LABEL_32:
					scaleRange = scaleRangeModified;

					g_randDatBufferPtr = randByte + 1;

					break;

				default:
					break;
			}

			return SpawnInstance(spawnX, spawnY, spawnZ, velXRange, velYRange, velZRange, scaleRange, preset->spawnFlags, preset->spawnOffset, particleIndex);
		}

		// FUNCTION: TOY2 0x00410850
		void SpawnBreakBurst(Nu3D::Particles::ParticleInstance* particle, uint8_t flag)
		{
			int32_t bigPreset;
			int32_t particleIndex;

			int32_t spawnY = particle->pos.y + 32 * particle->height;

			if ((flag & 1) != 0)
			{
				bigPreset = 56;
				particleIndex = 30;
				AudioManager::PlaySoundEffect(74, &particle->pos);
			}
			else
			{
				bigPreset = 14;
				particleIndex = 13;

				if (Toy2::g_levelFileIndex == 1)
					AudioManager::PlaySoundEffect(12, &particle->pos);
				else
					AudioManager::PlaySoundEffect(100, &particle->pos);
			}

			int32_t count = 5;

			do
			{
				ParticleInstance* spawned = SpawnFromPreset(particle->pos.x, spawnY, particle->pos.z, particleIndex, 9);
				spawned->unkByte1 = 2 * ((*g_randDatBufferPtr & 3) + 3);

				--count;
				++g_randDatBufferPtr;

				spawned->lifetime = 5 * spawned->unkByte1;
			} while (count);

			if ((flag & 2) == 0)
				SpawnFromPreset(particle->pos.x, spawnY, particle->pos.z, bigPreset, 2);
		}

		// FUNCTION: TOY2 0x00410B80
		void DeathEffect(Nu3D::Particles::ParticleInstance* particle)
		{
			int8_t collisionFlags;
			int32_t count;
			int32_t spawnY;
			int32_t x;
			int32_t y;
			int32_t z;
			ParticleInstance* spawned;
			bool lastIteration;

			collisionFlags = (int8_t)particle->collisionFlags;

			if (collisionFlags >= 0)
			{
				switch (collisionFlags)
				{
					case 1:
						SpawnBreakBurst(particle, 0);

						break;

					case 2:
						SpawnBreakBurst(particle, 1);

						break;

					case 3:
						count = 5;
						spawnY = particle->pos.y + 32 * particle->height;

						do
						{
							SpawnFromPreset(particle->pos.x, spawnY, particle->pos.z, 37, 17)->lifetime = 2 * (*g_randDatBufferPtr & 7) + 32;

							--count;
							++g_randDatBufferPtr;
						} while (count);

						SpawnFromPreset(particle->pos.x, spawnY, particle->pos.z, 40, 2);
						Toy2::Lighting::SpawnLight(particle->pos.x, spawnY, particle->pos.z, 0xA00000, 24, (int32_t)particle);
						AudioManager::PlaySoundEffect(11, &particle->pos);

						break;

					case 4:
						SpawnBreakBurst(particle, 2);

						break;

					case 5:
						z = particle->pos.z;
						y = particle->pos.y;
						x = particle->pos.x;
						count = 5;

						do
						{
							int32_t offset = *g_randDatBufferPtr & 0x3F;
							g_randDatBufferPtr += 3;

							spawned = SpawnFromPreset(x + 60 * (offset - 32), 60 * (offset - 32) + y, 60 * (offset - 32) + z, 41, 15);
							spawned->rotSpeed = *g_randDatBufferPtr++ - 128;
							spawned->lifetime = 2 * (*g_randDatBufferPtr & 0xF) + 24;

							lastIteration = count == 1;
							++g_randDatBufferPtr;
							--count;
						} while (! lastIteration);

						Toy2::Lighting::SpawnLight(x, y, z, 0x00604000, 16, x);

						break;

					case 6:
						count = 5;

						do
						{
							spawned = SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 62, 9);
							spawned->unkByte1 = 2 * ((*g_randDatBufferPtr & 3) + 3);

							--count;
							++g_randDatBufferPtr;

							spawned->lifetime = 5 * spawned->unkByte1;
						} while (count);

						break;

					case 7:
						count = 3;
						spawnY = particle->pos.y + 32 * particle->height;

						do
						{
							SpawnFromPreset(particle->pos.x, spawnY, particle->pos.z, 37, 17)->lifetime = 2 * (*g_randDatBufferPtr & 7) + 32;

							--count;
							++g_randDatBufferPtr;
						} while (count);

						goto LBL_PLAY_SOUND;

					case 8:
						count = 5;
						spawnY = particle->pos.y + 32 * particle->height;

						do
						{
							SpawnFromPreset(particle->pos.x, spawnY, particle->pos.z, 37, 17)->lifetime = 2 * (*g_randDatBufferPtr & 7) + 32;

							--count;
							++g_randDatBufferPtr;
						} while (count);

						Toy2::Lighting::SpawnLight(particle->pos.x, spawnY, particle->pos.z, 0xA00000, 24, (int32_t)particle);
						AudioManager::PlaySoundEffect(11, &particle->pos);

						break;

					case 9:
						count = 3;

						do
						{
							spawned = SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 17, 4);
							spawned->width = 250;
							spawned->height = 250;
							spawned->rotSpeed = *g_randDatBufferPtr++ - 64;
							spawned->lifetime = 2 * (*g_randDatBufferPtr & 7) + 32;

							--count;
							++g_randDatBufferPtr;
						} while (count);

					LBL_PLAY_SOUND:
						AudioManager::PlaySoundEffect(11, &particle->pos);

						break;

					case 10:
						SpawnFromPreset(particle->pos.x, particle->pos.y + 32 * particle->height, particle->pos.z, 120, 2);

						break;

					case 11:
						SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 117, 28)->rotSpeed = *g_randDatBufferPtr - 128;
						++g_randDatBufferPtr;

						break;

					default:
						return;
				}
			}
			else
			{
				SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, -collisionFlags, 2);
			}
		}

		// FUNCTION: TOY2 0x00425AD0
		void ReflectWallsSquareArena(Nu3D::Particles::ParticleInstance* particle)
		{
			if (particle->pos.x < -92651)
			{
				particle->velX = abs(particle->velX);
				AudioManager::PlaySoundEffect(74, &particle->pos);
			}

			if (particle->pos.x > 92437)
			{
				particle->velX = -abs(particle->velX);
				AudioManager::PlaySoundEffect(74, &particle->pos);
			}

			if (particle->pos.z < -93423)
			{
				particle->velZ = abs(particle->velZ);
				AudioManager::PlaySoundEffect(74, &particle->pos);
			}

			if (particle->pos.z > 92561)
			{
				particle->velZ = -abs(particle->velZ);
				AudioManager::PlaySoundEffect(74, &particle->pos);
			}
		}

		// FUNCTION: TOY2 0x0042B090
		void UpdateArenaBounce(Nu3D::Particles::ParticleInstance* particle)
		{
			int32_t velY;
			int32_t velZ;
			int32_t x;
			int32_t z;

			int32_t playSound = 0;

			if (particle->lifetime > 4 && Toy2::g_tickState[4])
				SpawnFromPreset(
					particle->pos.x - Renderer::g_frameDelta * particle->velX,
					particle->pos.y,
					particle->pos.z - Renderer::g_frameDelta * particle->velZ,
					110,
					2);

			if (Toy2::Actor::IsInsideBounds((Toy2::Actor::Toy2Actor*)particle, -153302, 57514, -111593, 111255))
			{
				if (particle->groundHeightY == 0x80000000)
					particle->groundHeightY = -76755;

				if (particle->pos.y <= -76755)
				{
					particle->groundHeightY = -76755;

					if (Renderer::Shadows::g_shadowCount < 47)
					{
						int32_t shadowIdx = Renderer::Shadows::g_shadowCount;
						int16_t shadowCount = Renderer::Shadows::g_shadowCount + 1;
						Renderer::Shadows::g_shadowRecords[shadowIdx].worldX = particle->pos.x;
						Renderer::Shadows::g_shadowRecords[shadowIdx].groundY = particle->groundHeightY;
						Renderer::Shadows::g_shadowRecords[shadowIdx].worldZ = particle->pos.z;
						Renderer::Shadows::g_shadowRecords[shadowIdx].size = particle->width;
						Renderer::Shadows::g_shadowCount = shadowCount;
					}
				}
				else if (particle->groundHeightY == -76755)
				{
					velY = particle->velY;
					particle->pos.y = -76755;
					playSound = 1;
					particle->velY = -abs((7 * velY) >> 3);
				}
				else
				{
					velZ = -particle->velZ;
					particle->velX = -particle->velX;
					particle->velZ = velZ;
					playSound = 1;
				}
			}
			else
			{
				particle->groundHeightY = 400000;
			}

			x = particle->pos.x;

			if (particle->pos.x < -170838)
			{
				playSound = 1;
				particle->velX = abs(particle->velX);
			}

			if (x > 173610)
			{
				playSound = 1;
				particle->velX = -abs(particle->velX);
			}

			z = particle->pos.z;

			if (z < -143593)
			{
				playSound = 1;
				particle->velZ = abs(particle->velZ);
			}

			if (z > 142231)
			{
				playSound = 1;
				particle->velZ = -abs(particle->velZ);
			}

			if (((-48508 - x) >> 5) * ((-48508 - x) >> 5) + ((153 - z) >> 5) * ((153 - z) >> 5) < 0x100000)
				particle->lifetime = 0;

			if (playSound)
				AudioManager::PlaySoundEffect(67, &particle->pos);
		}

		// FUNCTION: TOY2 0x0042B250
		void UpdateDrainTrail(Nu3D::Particles::ParticleInstance* particle)
		{
			int32_t z;
			int32_t x;

			z = particle->pos.z;
			x = particle->pos.x;

			if (((-48508 - particle->pos.x) >> 5) * ((-48508 - particle->pos.x) >> 5) + ((153 - z) >> 5) * ((153 - z) >> 5) < 0x100000)
				particle->lifetime = 0;

			if (particle->lifetime > 4)
			{
				if (Toy2::g_tickState[4])
					SpawnFromPreset(x - Renderer::g_frameDelta * particle->velX, particle->pos.y, z - Renderer::g_frameDelta * particle->velZ, 111, 2);
			}
		}

		// FUNCTION: TOY2 0x00410F40
		void Update()
		{
			int32_t particleIdx;
			int16_t lifetime;
			int32_t homingScale;
			int32_t homingDivisor;
			Toy2::Actor::Toy2Actor* targetActor;
			int32_t dirX;
			int32_t dirY;
			int32_t dirZ;
			int16_t* attach;
			int32_t pitchAngle;
			int32_t sinPitch;
			int32_t cosPitch;
			int32_t gravity;
			int32_t velYStep;
			int32_t groundHeightY;
			int16_t shadowCount;
			int32_t shadowIdx;
			int32_t clampValue;
			uint8_t tileIndex;
			uint8_t unkByte7;
			int32_t offset;
			int32_t spawnY;
			int32_t value;
			int32_t value2;
			int32_t value3;
			int32_t typeId;
			int32_t colorB;
			uint32_t posX;
			ParticleInstance* spawned;
			int16_t hitAngle;
			int16_t tick16Acc;
			uint16_t yaw;

			ParticleInstance* particle = g_particleInstances;
			int32_t colorRampMode = 0;

			for (particleIdx = 0; particleIdx < 64; ++particleIdx, ++particle)
			{
				lifetime = particle->lifetime;

				if (lifetime <= 0)
					continue;

				int32_t frameDelta = Renderer::g_frameDelta;
				int16_t newLifetime = (int16_t)(lifetime - Renderer::g_frameDelta);

				particle->lifetime = newLifetime;

				if (newLifetime < 0)
					particle->lifetime = 0;

				if ((particle->renderFlags & 8) != 0)
				{
					if (particle->unkByte1 == 196)
					{
						homingScale = 8;
						homingDivisor = particle->lifetime / 16 * (particle->lifetime / 16) + 4;
					}
					else
					{
						homingScale = 16;
						homingDivisor = 8;
					}

					if (particle->lifetime > 0)
					{
						// velY holds a pointer to the actor to home in on, or -1 for Buzz
						targetActor = (Toy2::Actor::Toy2Actor*)particle->velY;

						if ((int32_t)targetActor == -1)
						{
							dirX = (Toy2::g_buzzActor.posAngles.pos.x - particle->pos.x) >> 5;
							dirY = (Toy2::g_buzzActor.posAngles.pos.y - particle->pos.y - 0x2000) >> 5;
							dirZ = (Toy2::g_buzzActor.posAngles.pos.z - particle->pos.z) >> 5;
						}
						else
						{
							attach = (int16_t*)(targetActor->unkVar34 + 16 * targetActor->primaryAnimIdx);

							if (! targetActor->actorPhase || targetActor->unkVar29 < 0)
								particle->lifetime = 1;

							value = targetActor->transform.pos.x + attach[0] - particle->pos.x;
							value2 = attach[1] - particle->pos.y;
							value3 = attach[2];

							lifetime = particle->lifetime;
							dirX = value >> 5;
							dirY = (targetActor->transform.pos.y + value2) >> 5;
							dirZ = (targetActor->transform.pos.z + value3 - particle->pos.z) >> 5;

							if (lifetime >= 32)
								value = 3 * (lifetime & 0xF) + 2;
							else
								value = (lifetime & 0xF) + 2;

							homingDivisor = value;
							homingScale = 6;
						}

						int32_t yawDiff = (Nu3D::Math::CartesianToFixedAngle(dirX, dirZ) - particle->yawAngle) & 0xFFF;

						if (yawDiff >= 0x800)
							yawDiff -= 4096;

						particle->yawAngle = (particle->yawAngle + (yawDiff * (int32_t)Renderer::g_frameDelta) / homingDivisor) & 0xFFF;

						if (particle->pitchAngle != -1)
						{
							int32_t pitchDiff =
								(-1024 - particle->pitchAngle + Nu3D::Math::CartesianToFixedAngle((int32_t)sqrt((double)(dirX * dirX + dirZ * dirZ)), dirY))
								& 0xFFF;

							if (pitchDiff >= 2048)
								pitchDiff -= 4096;

							particle->pitchAngle = (particle->pitchAngle + (pitchDiff * (int32_t)Renderer::g_frameDelta) / 20) & 0xFFF;
						}

						frameDelta = Renderer::g_frameDelta;
					}

					pitchAngle = particle->pitchAngle;

					if (pitchAngle == -1)
					{
						sinPitch = 0;
						cosPitch = 0x4000;
					}
					else
					{
						sinPitch = Numerics::g_sinCosLUT[particle->pitchAngle & 0xFFF];
						cosPitch = Numerics::g_sinCosLUT[(pitchAngle + 1024) & 0xFFF];
					}

					velYStep = -(sinPitch / homingScale);

					// quirk: the x velocity indexes the table with the yaw without masking
					particle->velX = ((cosPitch * Numerics::g_sinCosLUT[particle->yawAngle]) >> 14) / homingScale;
					particle->velZ = ((cosPitch * Numerics::g_sinCosLUT[(particle->yawAngle + 1024) & 0xFFF]) >> 14) / homingScale;
					gravity = 0;
				}
				else
				{
					velYStep = particle->velY;
					gravity = particle->yawAngle;
				}

				value = gravity * frameDelta + particle->velY;
				value2 = frameDelta * particle->velX;
				value3 = frameDelta * particle->velZ;

				particle->pos.y += velYStep * frameDelta;

				int32_t spawnX = value2 + particle->pos.x;
				int32_t spawnZ = value3 + particle->pos.z;

				particle->velY = value;
				particle->pos.x = spawnX;
				particle->pos.z = spawnZ;

				if (particle->unkByte7 > 1)
				{
					particle->unkByte2 -= frameDelta;

					while ((int8_t)particle->unkByte2 <= 0)
					{
						tileIndex = particle->tileIndex + 1;
						particle->unkByte2 += particle->unkByte1;
						unkByte7 = particle->unkByte7;
						particle->tileIndex = tileIndex;

						if (tileIndex >= unkByte7)
							particle->tileIndex = tileIndex - unkByte7;
					}
				}

				if (particle->rotSpeed)
					particle->groundAlignRot = (particle->groundAlignRot + frameDelta * particle->rotSpeed) & 0xFFF;

				if ((particle->renderFlags & 1) == 0 || particle->lifetime <= 0)
					goto LABEL_52;

				if (particle->velX || particle->velZ || particle->groundHeightY == 0x80000000)
				{
					int32_t groundHeight = Nu3D::Collision::GetGroundHeight((Transform*)particle, 0);
					frameDelta = Renderer::g_frameDelta;
					particle->groundHeightY = groundHeight;
				}

				groundHeightY = particle->groundHeightY;

				if (groundHeightY == 0x80000000)
				{
				LABEL_52:
					shadowCount = Renderer::Shadows::g_shadowCount;

					goto LABEL_53;
				}

				if (groundHeightY >= particle->pos.y + 32 * particle->height)
				{
					shadowCount = Renderer::Shadows::g_shadowCount;

					if (Renderer::Shadows::g_shadowCount < 47)
					{
						shadowIdx = Renderer::Shadows::g_shadowCount;
						Renderer::Shadows::g_shadowRecords[shadowIdx].worldX = particle->pos.x;

						goto LABEL_51;
					}
				}
				else
				{
					value = particle->pos.y;
					value2 = particle->groundHeightY;
					value3 = particle->yawAngle - particle->velY;
					particle->lifetime = 0;

					if (value2 <= value + value3)
						goto LABEL_52;

					shadowCount = Renderer::Shadows::g_shadowCount;
					particle->pos.y = particle->groundHeightY - 32 * particle->height;

					if (Renderer::Shadows::g_shadowCount < 47)
					{
						shadowIdx = shadowCount;
						Renderer::Shadows::g_shadowRecords[shadowIdx].worldX = particle->pos.x;
						groundHeightY = particle->groundHeightY;

					LABEL_51:
						Renderer::Shadows::g_shadowRecords[shadowIdx].groundY = groundHeightY;
						Renderer::Shadows::g_shadowRecords[shadowIdx].worldZ = particle->pos.z;
						++shadowCount;
						Renderer::Shadows::g_shadowRecords[shadowIdx].size = particle->width;
						Renderer::Shadows::g_shadowCount = shadowCount;
					}
				}

			LABEL_53:
				if (((Toy2::Camera::g_activeCameraTransform.pos.x - particle->pos.x)
					 >> 8) * ((Toy2::Camera::g_activeCameraTransform.pos.x - particle->pos.x) >> 8)
						+ ((Toy2::Camera::g_activeCameraTransform.pos.y - particle->pos.y) >> 8)
							* ((Toy2::Camera::g_activeCameraTransform.pos.y - particle->pos.y) >> 8)
						+ ((Toy2::Camera::g_activeCameraTransform.pos.z - particle->pos.z) >> 8)
							* ((Toy2::Camera::g_activeCameraTransform.pos.z - particle->pos.z) >> 8)
					> 1638400)
				{
					particle->lifetime = 0;
					particle->collisionFlags = 0;
				}

				// 0x100 and 0x200 are the texture clamp / bilinear filter flags
				if ((particle->renderFlags & 0x300) == 0)
					particle->lifetime = 0;

				switch (particle->updateMode)
				{
					case 1:
						if (shadowCount < 47)
						{
							shadowIdx = shadowCount;
							Renderer::Shadows::g_shadowRecords[shadowIdx].worldX = particle->pos.x;
							Renderer::Shadows::g_shadowRecords[shadowIdx].groundY = particle->pos.y + 32 * particle->height;
							Renderer::Shadows::g_shadowRecords[shadowIdx].worldZ = particle->pos.z;
							Renderer::Shadows::g_shadowRecords[shadowIdx].size = particle->width;
							Renderer::Shadows::g_shadowCount = shadowCount + 1;
						}

						goto LABEL_243;

					case 2:
						goto LABEL_65;

					case 3:
						goto LABEL_233;

					case 4:
						colorRampMode = 1;

						if (particle->lifetime > 16 && Toy2::g_tickState[4])
							SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 3, 3);

						goto LABEL_243;

					case 5:
						goto LABEL_194;

					case 6:
						colorRampMode = 4;

						goto LABEL_243;

					case 7:
						Renderer::LensFlare::RegisterLight(
							particle->pos.x, particle->pos.y, particle->pos.z, particle->colourR, particle->colourG, particle->colourB, 48);

					LABEL_65:
						colorRampMode = 1;

						goto LABEL_243;

					case 8:
						value = Numerics::g_sinCosLUT[(((particle->lifetime & 0x1F) << 7) + 1024) & 0xFFF];
						particle->width = (Numerics::g_sinCosLUT[128 * (particle->lifetime & 0x1F)] >> 10) + 120;
						particle->height = (value >> 10) + 120;

						if (Toy2::g_tickState[7] != 0 && particle->lifetime > 5)
							goto LABEL_68;

						goto LABEL_243;

					case 9:
						value = 2 * frameDelta;
						colorRampMode = 1;
						particle->width += value;
						particle->height += value;

						goto LABEL_243;

					case 0xA:
						particle->width += -4 * frameDelta;
						colorRampMode = 5;

						if (particle->width < 16)
							particle->width = 16;

						particle->height = particle->width;

						goto LABEL_243;

					case 0xB:
						particle->width += frameDelta * particle->unkByte1;
						colorRampMode = 1;
						particle->height = particle->width;

						goto LABEL_243;

					case 0xC:
						value = particle->lifetime;
						colorRampMode = 1;
						particle->pos.x = Toy2::g_buzzActor.posAngles.pos.x;
						value2 = Toy2::g_buzzActor.posAngles.pos.y - 554 * value;
						particle->width += -8 * frameDelta;
						value3 = Toy2::g_buzzActor.posAngles.pos.z;
						particle->pos.y = value2;
						particle->pos.z = value3;
						particle->height = particle->width;

						goto LABEL_243;

					case 0xD:
						colorRampMode = 1;
						particle->pos.x = Toy2::g_buzzActor.posAngles.pos.x;
						value = Toy2::g_buzzActor.posAngles.pos.z;
						particle->pos.y = Toy2::g_buzzActor.posAngles.pos.y - 400 * (particle->lifetime + 16);
						particle->pos.z = value;
						particle->width += -5 * frameDelta;
						particle->height = particle->width;

						goto LABEL_243;

					case 0xE:
						value = 16 * *g_randDatBufferPtr;
						++g_randDatBufferPtr;
						particle->groundAlignRot = value;
						colorRampMode = particle->lifetime < 32 ? 1 : 6;

						Renderer::LensFlare::RegisterLight(
							particle->pos.x, particle->pos.y, particle->pos.z, particle->colourR, particle->colourG, particle->colourB, 48);

						goto LABEL_243;

					case 0xF:
						particle->width -= frameDelta * particle->unkByte1;
						clampValue = 1;

						goto LABEL_149;

					case 0x10:
						if (particle->lifetime > 4 && Toy2::g_tickState[4])
							SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 36, 15);

						goto LABEL_243;

					case 0x11:
						if (particle->lifetime > 4 && Toy2::g_tickState[3])
						{
							int32_t count = 0;

							do
							{
								SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 39, 2);
								++count;
							} while (count < Toy2::g_tickState[3]);
						}

						goto LABEL_243;

					case 0x12:
						if (particle->lifetime > 4 && Toy2::g_tickState[7])
						{
							offset = 2 * *g_randDatBufferPtr - 256;
							spawnZ = offset + particle->pos.z;
							spawnY = particle->pos.y + offset;
							spawnX = particle->pos.x + offset;

							g_randDatBufferPtr += 3;
							SpawnFromPreset(spawnX, spawnY, spawnZ, 44, 2)->rotSpeed = (*g_randDatBufferPtr - 128) >> 1;
							++g_randDatBufferPtr;
						}

						goto LABEL_243;

					case 0x13:
						value = *g_randDatBufferPtr & 7;
						++g_randDatBufferPtr;

						if (! value)
						{
							particle->velX = *g_randDatBufferPtr - 128;
							++g_randDatBufferPtr;
							particle->velZ = *g_randDatBufferPtr - 128;
							++g_randDatBufferPtr;
						}

						value = Toy2::Buzz::g_underwaterYLevel;
						value2 = (Numerics::g_sinCosLUT[(((particle->lifetime & 0x1F) << 7) + 1024) & 0xFFF] >> 11) + 40;

						particle->width = (Numerics::g_sinCosLUT[128 * (particle->lifetime & 0x1F)] >> 11) + 40;
						particle->height = value2;

						if (particle->pos.y < value)
							particle->lifetime = 0;

						goto LABEL_243;

					case 0x14:
						particle->width += frameDelta * particle->unkByte1;
						colorRampMode = 3;
						particle->height = particle->width;

						goto LABEL_243;

					case 0x15:
						value = Toy2::g_buzzActor.posAngles.pos.z;
						particle->pos.x = Toy2::g_buzzActor.posAngles.pos.x;

						yaw = Toy2::g_buzzActor.posAngles.angles.yaw;
						colorRampMode = 1;
						particle->pos.z = value;

						if (particle->unkByte1 == 101)
						{
							spawnX = (Numerics::g_sinCosLUT[(yaw + 1664) & 0xFFF] >> 2) + particle->pos.x;
							offset = (int16_t)(yaw - 1408);
						}
						else
						{
							spawnX = (Numerics::g_sinCosLUT[(yaw - 1664) & 0xFFF] >> 2) + particle->pos.x;
							offset = (int16_t)(yaw - 640);
						}

						particle->pos.x = spawnX;
						particle->pos.z += Numerics::g_sinCosLUT[offset & 0xFFF] >> 2;
						particle->pos.y = Toy2::g_buzzActor.posAngles.pos.y - 1024;

						if (particle->lifetime > 4)
						{
							value = 46;

							if (Toy2::Buzz::g_currentFluidType == 1 && particle->pos.y > Toy2::Buzz::g_underwaterYLevel)
								value = 45;

							value2 = *g_randDatBufferPtr++;

							SpawnInstance(
								particle->pos.x,
								particle->pos.y,
								particle->pos.z,
								Toy2::g_buzzActor.velX / 2,
								0,
								Toy2::g_buzzActor.velForward / 2,
								-16,
								0,
								8 * value2 - 1024,
								value);
						}

						goto LABEL_243;

					case 0x16:
						value = particle->width;

						if (value < 320)
						{
							value2 = 32 * frameDelta + value;
							particle->width = value2;

							if ((int16_t)value2 > 320)
								particle->width = 320;
						}

						particle->height = particle->width;

						goto LABEL_243;

					case 0x17:
						if (shadowCount < 47)
						{
							shadowIdx = shadowCount;

							Renderer::Shadows::g_shadowRecords[shadowIdx].worldX = particle->pos.x;
							Renderer::Shadows::g_shadowRecords[shadowIdx].groundY = particle->groundHeightY;
							Renderer::Shadows::g_shadowRecords[shadowIdx].worldZ = particle->pos.z;
							Renderer::Shadows::g_shadowRecords[shadowIdx].size = particle->width;
							Renderer::Shadows::g_shadowCount = shadowCount + 1;
						}

						if (particle->pos.y + 32 * particle->height > particle->groundHeightY)
						{
							value = particle->height;
							value2 = particle->groundHeightY;
							particle->lifetime = 0;
							particle->pos.y = value2 - 32 * value;
						}

						goto LABEL_243;

					case 0x18:
						particle->width -= frameDelta * particle->unkByte1;
						colorRampMode = 1;

						if (particle->width < 1)
							particle->width = 1;

						goto LABEL_243;

					case 0x19:
						if (shadowCount < 47)
						{
							shadowIdx = shadowCount;
							Renderer::Shadows::g_shadowRecords[shadowIdx].worldX = particle->pos.x;
							Renderer::Shadows::g_shadowRecords[shadowIdx].groundY = particle->groundHeightY;
							Renderer::Shadows::g_shadowRecords[shadowIdx].worldZ = particle->pos.z;
							Renderer::Shadows::g_shadowRecords[shadowIdx].size = particle->width;
							Renderer::Shadows::g_shadowCount = shadowCount + 1;
						}

						if (particle->pos.y + 32 * particle->height > particle->groundHeightY)
						{
							value = particle->velY;
							particle->pos.y = particle->groundHeightY - 32 * particle->height;

							if (value > 0)
								particle->velY = -6 * value / 8;

							if (abs(particle->velY) < 256)
								particle->lifetime = 0;

							AudioManager::PlaySoundEffect(31, &particle->pos);
						}

						goto LABEL_243;

					case 0x1A:
						colorRampMode = 1;
						AudioManager::PlaySoundEffect(68, &particle->pos);

						if (particle->lifetime > 4)
						{
							offset = 32 * (*g_randDatBufferPtr - 128);
							spawnZ = offset + particle->pos.z;
							spawnY = particle->pos.y + offset;
							spawnX = particle->pos.x + offset;
							g_randDatBufferPtr += 3;
							SpawnFromPreset(spawnX, spawnY, spawnZ, 64, 2);
						}

						// quirk: the red channel of the light is the green channel of the particle
						Renderer::LensFlare::RegisterLight(
							particle->pos.x, particle->pos.y, particle->pos.z, particle->colourG, particle->colourG, particle->colourB, 48);

						goto LABEL_243;

					case 0x1B:
						value = frameDelta * particle->unkByte1;
						particle->width -= value;
						particle->height -= value;

						if (particle->width < 1)
							particle->width = 1;

						if (particle->height < 1)
							particle->height = 1;

						goto LABEL_243;

					case 0x1C:
						if (particle->unkByte1 != 0xC6)
						{
							value = Nu3D::Collision::GetGroundHeight((Transform*)particle, 100);
							particle->groundHeightY = value;

							if (value != 0x80000000 && particle->pos.y + 32 * particle->height > value)
							{
								value2 = particle->velY;
								particle->pos.y = particle->groundHeightY - 32 * particle->height;

								if (value2 > 0)
									particle->velY = -3 * value2 / 4;

								if (abs(particle->velY) < 1024)
									particle->lifetime = 0;

								AudioManager::PlaySoundEffect(104, &particle->pos);
							}
						}

						if (Toy2::g_tickState[7] && particle->lifetime > 5 && (particle->renderFlags & 0x200) != 0)
						{
						LABEL_68:
							SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 15, 2)->rotSpeed = (*g_randDatBufferPtr - 128) >> 3;
							++g_randDatBufferPtr;
						}

						goto LABEL_243;

					case 0x1D:
						if (Toy2::g_tickState[8])
							AudioManager::PlaySoundEffect(85, &particle->pos);

						targetActor = (Toy2::Actor::Toy2Actor*)particle->velY;
						attach = (int16_t*)(targetActor->unkVar34 + 16 * targetActor->primaryAnimIdx);
						value = targetActor->transform.pos.z;

						value2 = attach[1];
						value3 = targetActor->transform.pos.x + attach[0] - particle->pos.x;
						dirZ = (value + attach[2] - particle->pos.z) >> 5;
						dirY = (targetActor->transform.pos.y + value2 - particle->pos.y) >> 5;

						dirX = value3 >> 5;

						if (dirX * dirX + dirY * dirY + dirZ * dirZ < 0x4000)
						{
							if ((uint8_t)targetActor->creatureRam->defenseMode == 4)
							{
								hitAngle = Nu3D::Math::CartesianToFixedAngle(targetActor->transform.pos.x - particle->pos.x, value - particle->pos.z);
								((uint8_t*)&particle->renderFlags)[0] &= 0xF5;
								hitAngle += 2048;
								offset = (hitAngle + 1024) & 0xFFF;
								particle->velY = -1024;

								value2 = Numerics::g_sinCosLUT[hitAngle & 0xFFF];
								particle->yawAngle = 48;
								particle->lifetime = 50;
								particle->velX = value2 / 16;
								particle->updateMode = 30;
								particle->velZ = Numerics::g_sinCosLUT[offset] / 16;

								AudioManager::PlaySoundEffect(7, &particle->pos);
							}
							else
							{
								hitAngle = Nu3D::Math::CartesianToFixedAngle(dirX, dirZ);
								Toy2::Actor::HandleDamage(targetActor, hitAngle & 0xFFF, 4);
								particle->lifetime = 0;
								particle->collisionFlags = 8;
							}
						}

						goto LABEL_145;

					case 0x1E:
					LABEL_145:
						if (particle->lifetime > 4 && Toy2::g_tickState[3])
						{
							value = *g_randDatBufferPtr;
							spawnY = value + particle->pos.y - 128;
							spawnZ = particle->pos.z + value - 128;
							spawnX = value + particle->pos.x - 128;

							value2 = particle->velZ;
							g_randDatBufferPtr += 4;
							SpawnInstance(spawnX, spawnY, spawnZ, particle->velX / 3, 0, value2 / 3, 0, 0, value - 128, 44);
						}

						goto LABEL_243;

					case 0x1F:
						particle->width += -4 * frameDelta;
						colorRampMode = 1;
						clampValue = 16;

					LABEL_149:
						if (particle->width < clampValue)
							particle->width = clampValue;

						particle->height = particle->width;

						goto LABEL_243;

					case 0x20:
						tick16Acc = Toy2::g_tickAccumulators.tick16Acc;

						if (Toy2::g_tickAccumulators.tick16Acc >= 8)
							tick16Acc = 15 - Toy2::g_tickAccumulators.tick16Acc;

						value = 8 * tick16Acc + 100;
						particle->width = value;
						particle->height = value;

						if (particle->lifetime > 4 && Toy2::g_tickState[2] && (particle->renderFlags & 0x200) != 0)
						{
							SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 77, 2)->rotSpeed = *g_randDatBufferPtr - 128;
							++g_randDatBufferPtr;
						}

						goto LABEL_243;

					case 0x21:
						particle->width += frameDelta * particle->unkByte1 / -2;

						if (particle->width < 1)
							particle->width = 1;

						colorRampMode = 1;
						particle->height = particle->width;

						goto LABEL_243;

					case 0x22:
						particle->width += frameDelta * particle->unkByte1;
						particle->height = particle->width;
						colorRampMode = 7;

						goto LABEL_243;

					case 0x23:
						value = particle->velY;
						value2 = particle->groundAlignRot & 0x400;
						particle->groundHeightY = 0;
						value3 = value >> 6;

						if (value2)
						{
							particle->width = 250 - value3;
							particle->height = value3 + 250;
						}
						else
						{
							particle->width = value3 + 250;
							particle->height = 250 - value3;
						}

						if (particle->pos.y + 32 * particle->height <= particle->groundHeightY)
							goto LABEL_188;

						value = particle->velY;
						particle->pos.y = particle->groundHeightY - 32 * particle->height;

						if (value > 0)
							particle->velY = -3 * value / 4;

						break;

					case 0x24:
						colorRampMode = 1;
						value = (Toy2::g_buzzActor.posAngles.angles.yaw - 2048) & 0xFFF;
						value2 = Numerics::g_sinCosLUT[(Toy2::g_buzzActor.posAngles.angles.yaw + 1024) & 0xFFF] / 10;
						value3 = Numerics::g_sinCosLUT[value] / 10;

						if (particle->rotSpeed < 0)
						{
							value2 = Numerics::g_sinCosLUT[(Toy2::g_buzzActor.posAngles.angles.yaw + 1024) & 0xFFF] / -10;
							value3 = Numerics::g_sinCosLUT[value] / -10;
						}

						offset = particle->lifetime;
						particle->pos.x = value2 + Toy2::g_buzzActor.posAngles.pos.x;
						particle->pos.y = Toy2::g_buzzActor.posAngles.pos.y + 288 * (24 - offset);
						spawnZ = value3 + Toy2::g_buzzActor.posAngles.pos.z;

						particle->width += 4 * frameDelta;
						particle->pos.z = spawnZ;
						particle->height = particle->width;

						goto LABEL_243;

					case 0x25:
						particle->width -= frameDelta * particle->unkByte1;

						if (particle->width < 1)
							particle->width = 1;

						colorRampMode = 1;
						particle->height = particle->width / 2;

						goto LABEL_243;

					case 0x26:
						posX = particle->pos.x;

						if (particle->velX <= 0)
						{
							if (posX < 0xFFFA55ED)
							{
								value = particle->velX;
								particle->pos.x = 0xFFFA55ED;
								particle->velX = -value;
							}
						}
						else if (posX <= 0xFFFC95A9)
						{
							if (posX > 0xFFFBD8AC && ! particle->groundHeightY)
							{
								if (particle->pos.y >= 0xFFFF5509)
								{
									value = particle->velX;
									particle->pos.x = 0xFFFBD8AC;
									particle->velX = -value;
								}
								else
								{
									particle->groundHeightY = 0xFFFF5509;
								}
							}
						}
						else
						{
							particle->groundHeightY = 1;
						}

						if (particle->pos.y + 32 * particle->height <= particle->groundHeightY)
							goto LABEL_188;

						value = particle->velY;
						particle->pos.y = particle->groundHeightY - 32 * particle->height;

						if (value > 0)
							particle->velY = -value >> 1;

						break;

					case 0x27:
						AudioManager::PlaySoundEffect(64, &particle->pos);

						Renderer::LensFlare::RegisterLight(
							particle->pos.x, particle->pos.y, particle->pos.z, particle->colourR, particle->colourG, particle->colourB, 48);

						if (particle->lifetime > 4 && Toy2::g_tickState[4] && (particle->renderFlags & 0x200) != 0)
						{
							SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 90, 2)->rotSpeed = *g_randDatBufferPtr - 128;
							++g_randDatBufferPtr;
						}

					LABEL_194:
						colorRampMode = 3;

						goto LABEL_243;

					case 0x28:
						particle->width -= frameDelta * particle->unkByte1;
						colorRampMode = 3;
						particle->height = particle->width;

						goto LABEL_243;

					case 0x29:
						if (particle->lifetime > 4 && Toy2::g_tickState[4] && (particle->renderFlags & 0x200) != 0)
							SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 98, 2);

						goto LABEL_243;

					case 0x2A:
						if (particle->velY > 384)
						{
							particle->velY = 384;

							if (Toy2::g_tickState[10])
							{
								value = *g_randDatBufferPtr++;
								particle->velX = 2 * value - 256;
								value2 = *g_randDatBufferPtr;
								++g_randDatBufferPtr;
								particle->velZ = 2 * value2 - 256;
							}
						}

						particle->width = abs(Numerics::g_sinCosLUT[64 * (particle->lifetime & 0x3F)] / particle->unkByte1);

						goto LABEL_243;

					case 0x2B:
						particle->groundAlignRot = (2047 - (int16_t)particle->yawAngle) & 0xFFF;
						value = Nu3D::Collision::GetGroundHeight((Transform*)particle, 0);
						particle->groundHeightY = value;

						if (value != 0x80000000)
							particle->pos.y = value - 2048;

						if (particle->lifetime > 4 && Toy2::g_tickState[2] && (particle->renderFlags & 0x200) != 0 && Toy2::g_tickState[4])
							SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 107, 2)->groundAlignRot =
								(2047 - (int16_t)particle->yawAngle) & 0xFFF;

						goto LABEL_243;

					case 0x2C:
						value = Nu3D::Collision::GetGroundHeight((Transform*)particle, 0);
						value2 = particle->pos.y;
						particle->groundHeightY = value;

						if (value2 >= value - 4096)
						{
							if (particle->lifetime > 4 && (particle->renderFlags & 0x200) != 0)
							{
								if (Toy2::g_tickState[4])
								{
									spawned = SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 44, 4);
									spawned->velX += 2 * particle->velX / 3;
									spawned->velZ += 2 * particle->velZ / 3;
								}

								if (Toy2::g_tickState[6])
									SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 105, 2)->groundAlignRot = particle->groundAlignRot;

								if (Toy2::g_tickState[8])
								{
									spawned = SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 106, 2);

									if (Toy2::g_tickState[9])
										spawned->groundAlignRot = (*g_randDatBufferPtr + particle->groundAlignRot + 384) & 0xFFF;
									else
										spawned->groundAlignRot = (*g_randDatBufferPtr + particle->groundAlignRot - 640) & 0xFFF;

									++g_randDatBufferPtr;
								}
							}
						}
						else
						{
							particle->lifetime = 0;
						}

						goto LABEL_243;

					case 0x2D:
						particle->width -= frameDelta * particle->unkByte1;
						colorRampMode = 2;
						particle->height = particle->width / 2;

						goto LABEL_243;

					case 0x2E:
						particle->width += frameDelta * particle->unkByte1;
						particle->height = particle->width;
						colorRampMode = 1;

						Renderer::LensFlare::RegisterLight(particle->pos.x, particle->pos.y, particle->pos.z, particle->colourB, particle->colourB >> 1, 0, 48);

						goto LABEL_243;

					case 0x2F:
						UpdateArenaBounce(particle);

						goto LABEL_243;

					case 0x30:
						UpdateDrainTrail(particle);

						goto LABEL_243;

					case 0x31:
						AudioManager::PlaySoundEffect(64, &particle->pos);
						Nu3D::Link::SetPositionRawAndCommit(25, particle->pos.x >> 5, particle->pos.y >> 5, particle->pos.z >> 5);
						Nu3D::Link::SetRotationRelative8bit(25, 0, (particle->yawAngle + 1024) & 0xFFF, particle->pitchAngle);

						if (particle->lifetime > 4 && Toy2::g_tickState[2])
						{
							value = *g_randDatBufferPtr - 128;
							value2 = particle->velZ / 2;
							value3 = particle->velX;
							++g_randDatBufferPtr;

							SpawnInstance(particle->pos.x, particle->pos.y, particle->pos.z, value3 / 2, 0, value2, 0, 0, value, 46);
						}

						goto LABEL_243;

					case 0x32:
						if (particle->lifetime >= 64)
						{
							value = 82 - particle->lifetime;
							typeId = particle->typeId;
							particle->colourR = (value * g_particleWrapper.typeDefs[typeId].colorR / 2) >> 3;
							particle->colourG = (value * g_particleWrapper.typeDefs[typeId].colorG / 2) >> 3;
							particle->colourB = (value * g_particleWrapper.typeDefs[typeId].colorB / 2) >> 3;
						}

					LABEL_233:
						colorRampMode = 2;

						goto LABEL_243;

					case 0x33:
						if (particle->unkByte1 == 118)
							colorRampMode = 2;

						if (particle->lifetime > 4 && Toy2::g_tickState[4])
						{
							SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, particle->unkByte1 >> 1, 2)->rotSpeed =
								*g_randDatBufferPtr - 128;
							++g_randDatBufferPtr;
						}

						goto LABEL_243;

					case 0x34:
						ReflectWallsSquareArena(particle);

						goto LABEL_243;

					case 0x35:
						value = particle->lifetime;
						value2 = Numerics::g_sinCosLUT[64 * (value & 0x3F)] / particle->unkByte1;
						particle->width = abs(value2);

						if (value > 4 && Toy2::g_tickState[9])
						{
							spawned = SpawnFromPreset(particle->pos.x, particle->pos.y, particle->pos.z, 46, 2);
							value3 = particle->unkByte1;
							spawned->lifetime = 48;
							spawned->width = 0x2000 / value3;
							spawned->height = 0x2000 / value3;
						}

						goto LABEL_243;

					default:
						goto LABEL_243;
				}

				AudioManager::PlaySoundEffect(113, &particle->pos);

			LABEL_188:
				if (Renderer::Shadows::g_shadowCount < 47)
				{
					shadowIdx = Renderer::Shadows::g_shadowCount;
					Renderer::Shadows::g_shadowRecords[shadowIdx].worldX = particle->pos.x;
					Renderer::Shadows::g_shadowRecords[shadowIdx].groundY = particle->groundHeightY;
					Renderer::Shadows::g_shadowRecords[shadowIdx].worldZ = particle->pos.z;
					Renderer::Shadows::g_shadowRecords[shadowIdx].size = particle->width;
					Renderer::Shadows::g_shadowCount = Renderer::Shadows::g_shadowCount + 1;
				}

			LABEL_243:
				switch (colorRampMode)
				{
					case 1:
						if (particle->lifetime < 32)
							goto LABEL_245;

						break;

					case 2:
						if (particle->lifetime < 64)
						{
							value = particle->lifetime;
							typeId = particle->typeId;

							particle->colourR = (value * g_particleWrapper.typeDefs[typeId].colorR / 2) >> 5;
							particle->colourG = (value * g_particleWrapper.typeDefs[typeId].colorG / 2) >> 5;
							colorB = (value * g_particleWrapper.typeDefs[typeId].colorB / 2) >> 5;

							goto LABEL_246;
						}

						break;

					case 3:
						if (particle->lifetime < 32)
						{
							value = particle->lifetime;
							particle->colourR = (value * g_particleWrapper.typeDefs[particle->typeId].colorR / 2) >> 4;

							if (value > 16)
							{
								value2 = particle->lifetime - 16;
								typeId = particle->typeId;

								particle->colourG = (value2 * g_particleWrapper.typeDefs[typeId].colorG / 2) >> 3;
								colorB = (value2 * g_particleWrapper.typeDefs[typeId].colorB / 2) >> 3;

								goto LABEL_246;
							}

							particle->colourG = 0;
							particle->colourB = 0;
						}

						break;

					case 4:
						if (particle->lifetime < 256)
						{
							value = particle->lifetime;
							typeId = particle->typeId;

							particle->colourR = (value * g_particleWrapper.typeDefs[typeId].colorR / 2) >> 7;
							particle->colourG = (value * g_particleWrapper.typeDefs[typeId].colorG / 2) >> 7;
							colorB = (value * g_particleWrapper.typeDefs[typeId].colorB / 2) >> 7;

							goto LABEL_246;
						}

						break;

					case 5:
						value = particle->lifetime;

						if (value >= 32)
						{
							particle->colourR = 0;
						}
						else
						{
							if (value >= 16)
								particle->colourR = 0;
							else
								particle->colourR = (particle->lifetime * g_particleWrapper.typeDefs[particle->typeId].colorR / 2) >> 4;

							if (particle->lifetime <= 12)
								particle->colourB = 0;
							else
								particle->colourB = ((particle->lifetime - 12) * g_particleWrapper.typeDefs[particle->typeId].colorB / 2) >> 4;

							if (particle->lifetime <= 24)
								particle->colourG = 0;
							else
								particle->colourG = ((particle->lifetime - 24) * g_particleWrapper.typeDefs[particle->typeId].colorG / 2) >> 3;
						}

						break;

					case 6:
						value = (*g_randDatBufferPtr++ & 0x7F) + 128;
						typeId = particle->typeId;

						particle->colourR = (value * g_particleWrapper.typeDefs[typeId].colorR) >> 8;
						particle->colourG = (value * g_particleWrapper.typeDefs[typeId].colorG) >> 8;
						particle->colourB = (value * g_particleWrapper.typeDefs[typeId].colorB) >> 8;

						break;

					case 7:
						if (particle->lifetime < 32)
						{
						LABEL_245:
							value = particle->lifetime;
							typeId = particle->typeId;

							particle->colourR = (value * g_particleWrapper.typeDefs[typeId].colorR / 2) >> 4;
							particle->colourG = (value * g_particleWrapper.typeDefs[typeId].colorG / 2) >> 4;
							colorB = (value * g_particleWrapper.typeDefs[typeId].colorB / 2) >> 4;
						}
						else
						{
							value = 42 - particle->lifetime;
							typeId = particle->typeId;

							particle->colourR = (value * g_particleWrapper.typeDefs[typeId].colorR / 2) >> 2;
							particle->colourG = (value * g_particleWrapper.typeDefs[typeId].colorG / 2) >> 2;
							colorB = (value * g_particleWrapper.typeDefs[typeId].colorB / 2) >> 2;
						}

					LABEL_246:
						particle->colourB = colorB;

						break;

					default:
						break;
				}

				colorRampMode = 0;

				if (! particle->lifetime)
					particle->lifetime = -1;
			}

			// particles that ran out this frame
			ParticleInstance* deadParticle = g_particleInstances;
			int32_t remaining = 64;

			do
			{
				if (deadParticle->lifetime == -1)
				{
					deadParticle->lifetime = 0;
					deadParticle->spriteSheet = 0;

					DeathEffect(deadParticle);
				}

				++deadParticle;
				--remaining;
			} while (remaining);
		}
	}
}

#include "Toy2/Weather.h"
#include "Toy2/Toy2.h"
#include "Toy2/Levels.h"
#include "Toy2/Collision.h"
#include "Nu3D/Camera.h"
#include "Renderer/Renderer.h"
#include "Numerics.h"
#include "Random.h"

namespace Toy2
{
	namespace Weather
	{
		// GLOBAL: TOY2 0x00557708
		int32_t g_spawnAccumulator;

		// 64 drops of 16 bytes (x, y, z, floor y), the floor is 0 for a drop that is not used
		// GLOBAL: TOY2 0x0054F08C
		int32_t g_weatherBasePointer;

		// GLOBAL: TOY2 0x00557AA4
		int32_t g_weatherSpriteSheet;

		// FUNCTION: TOY2 0x0044ED60
		void Init()
		{
			g_weatherBasePointer = (int32_t)Levels::g_levelLoadArena;
			Levels::g_levelLoadArena += 1024;

			for (int32_t i = 0; i < 1024; *(int32_t*)(i + g_weatherBasePointer - 4) = 0)
				i += 16;
		}

		// FUNCTION: TOY2 0x0044ED90
		void StepPrecipitation(int32_t spawnRate, int32_t fallSpeed, int32_t particleType)
		{
			int32_t base = g_weatherBasePointer;

			if (g_weatherBasePointer)
			{
				if (g_buzzActor.gravityVel > 1024)
					fallSpeed += 2 * g_buzzActor.gravityVel;

				int32_t freeCount = 0;
				uint32_t index = 0;
				int32_t offset = 0;

				// the drops that are not used are collected in the scratch memory, 8 at most
				uint32_t* freeList = (uint32_t*)&Collision::g_mathScratch[256];

				do
				{
					if (*(int32_t*)(offset + base + 12))
					{
						*(int32_t*)(offset + base + 4) += fallSpeed * (int32_t)Renderer::g_frameDelta;
						base = g_weatherBasePointer;

						int32_t* floorY = (int32_t*)(offset + g_weatherBasePointer + 12);

						// the drop hit the floor
						if (*(int32_t*)(offset + g_weatherBasePointer + 4) > *floorY)
						{
							*floorY = 0;
							base = g_weatherBasePointer;
						}
					}
					else if ((int32_t)freeList < (int32_t)&Collision::g_mathScratch[288])
					{
						*freeList = index;
						++freeCount;
						++freeList;
					}

					offset += 16;
					++index;
				} while (offset < 1024);

				g_weatherSpriteSheet = particleType;
				g_spawnAccumulator += spawnRate * (int32_t)Renderer::g_frameDelta;

				if (g_spawnAccumulator > 255)
				{
					uint32_t* freeEnd = (uint32_t*)&Collision::g_mathScratch[256] + freeCount;

					while (1)
					{
						if (freeCount <= 0)
							return;

						int32_t remaining = freeCount - 1;

						--freeEnd;

						int32_t random = *g_randDatBufferPtr++;
						int16_t angle = 8 * random - 1024;
						int32_t radius = *g_randDatBufferPtr++ + Numerics::g_sinCosLUT[(3 * Nu3D::Camera::g_cameraAngles.pitch + 1024) & 0xFFF] / 512;

						// the drop is spawned in a ring around the camera, above it
						*(int32_t*)(16 * *freeEnd + base) = *(int32_t*)&Collision::g_mathScratch[884] + 32 * (*g_randDatBufferPtr - 128)
							- ((radius * Numerics::g_sinCosLUT[(angle + Nu3D::Camera::g_cameraAngles.yaw) & 0xFFF]) >> 5);

						int32_t entryOffset = 16 * *freeEnd;

						++g_randDatBufferPtr;
						*(int32_t*)(entryOffset + g_weatherBasePointer + 4) = *(int32_t*)&Collision::g_mathScratch[888] - 81920;

						*(int32_t*)(16 * *freeEnd + g_weatherBasePointer + 8) =
							((radius * Numerics::g_sinCosLUT[(Nu3D::Camera::g_cameraAngles.yaw + angle + 1024) & 0xFFF]) >> 5)
							+ *(int32_t*)&Collision::g_mathScratch[892] + 32 * (*g_randDatBufferPtr++ - 128);

						// the sector 3 has a floor for the drops
						if (g_curSectorCopy1 == 3)
							*(int32_t*)(16 * *freeEnd + g_weatherBasePointer + 12) = 63500;
						else
							*(int32_t*)(16 * *freeEnd + g_weatherBasePointer + 12) = 0;

						base = g_weatherBasePointer;

						int32_t drop = g_weatherBasePointer + 16 * *freeEnd;
						int32_t floorLimit = *(int32_t*)(drop + 4) + 147456;

						if (*(int32_t*)(drop + 12) > floorLimit)
						{
							*(int32_t*)(drop + 12) = floorLimit;
							base = g_weatherBasePointer;
						}

						int32_t* floorY = (int32_t*)(16 * *freeEnd + base + 12);

						if (*floorY)
						{
							if (*floorY != 0x80000000)
								goto LABEL_25;

							*floorY = 0;
						}
						else
						{
							*floorY = -1;
						}

						base = g_weatherBasePointer;

					LABEL_25:
						g_spawnAccumulator -= 256;

						if (g_spawnAccumulator <= 255)
							return;

						freeCount = remaining;
					}
				}
			}
		}
	}
}

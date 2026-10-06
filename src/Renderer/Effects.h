#pragma once

#include "Common.h"
#include "Numerics.h"

namespace Renderer
{
	namespace Beam
	{
		struct BeamRequest
		{
			int32_t spriteSheetId;
			int32_t halfWidth;
			int32_t segmentLength;
			WorldPos origin;
			WorldPos direction;
			int32_t tintB;
			int32_t tintG;
			int32_t tintR;
		};

		extern int32_t g_beamRequestCount;
		extern BeamRequest g_beamRequests[100];

		void DrawSegmentedBeam(
			int32_t spriteSheet,
			int32_t halfWidth,
			int32_t segmentLength,
			WorldPos* origin,
			WorldPos* direction,
			uint8_t tintB,
			uint8_t tintG,
			uint8_t tintR);
		void QueueBeam(
			int32_t spriteSheet,
			int32_t halfWidth,
			int32_t segementLength,
			WorldPos* origin,
			WorldPos* direction,
			uint8_t tintB,
			uint8_t tintG,
			uint8_t tintR);

		STATIC_ASSERT(sizeof(BeamRequest) == 0x38);
	}

	namespace Particles
	{
		void DrawParticles();
	}

	void DrawFallingParticles();
	void DrawLensFlares();
}

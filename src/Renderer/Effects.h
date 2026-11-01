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
			Transform origin;
			Transform direction;
			int32_t tintB; // Red channel.
			int32_t tintG;
			int32_t tintR; // Blue channel.
		};

		extern int32_t g_beamRequestCount;
		extern BeamRequest g_beamRequests[100];

		void DrawSegmentedBeam(
			int32_t spriteSheet,
			int32_t halfWidth,
			int32_t segmentLength,
			Transform* origin,
			Transform* direction,
			uint8_t red,
			uint8_t tintG,
			uint8_t blue);
		void QueueBeam(
			int32_t spriteSheet,
			int32_t halfWidth,
			int32_t segementLength,
			Transform* origin,
			Transform* direction,
			int32_t red,
			int32_t tintG,
			int32_t blue);

		STATIC_ASSERT(sizeof(BeamRequest) == 0x38);
	}

	namespace Particles
	{
		void DrawParticles();
	}

	void DrawFallingParticles();
	void DrawLensFlares();
}

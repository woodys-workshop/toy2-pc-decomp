#include "Renderer/Effects.h"

namespace Renderer
{
	namespace Beam
	{
		// GLOBAL: TOY2 0x004F72D4
		int32_t g_beamRequestCount = 100;

		// GLOBAL: TOY2 0x0054F640
		BeamRequest g_beamRequests[100];

		// STUB: TOY2 0x0044E1A0
		void DrawSegmentedBeam(
			int32_t spriteSheet, int32_t halfWidth, int32_t segmentLength, WorldPos* origin, WorldPos* direction, uint8_t tintB, uint8_t tintG, uint8_t tintR)
		{}
	}

	namespace Particles
	{
		// STUB: TOY2 0x00445980
		void DrawParticles() {}
	}

	// STUB: TOY2 0x0044F010
	void DrawFallingParticles() {}

	// STUB: TOY2 0x0044F190
	void DrawLensFlares() {}
}

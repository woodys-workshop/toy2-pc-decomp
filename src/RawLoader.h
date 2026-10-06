#pragma once

#include "Common.h"
#include "Numerics.h"

namespace RawLoader
{
	struct CreatureListRam
	{
		struct EntityControl
		{
			uint8_t actorPhase;
			uint8_t unkByte;
			uint16_t actorFlags;
		};

		Vector3I pos;
		uint8_t creatureId;
		uint8_t movCtrl;
		uint8_t rotSpeed;
		uint8_t initialFacingAngle;
		EntityControl entCtrl;
		int16_t boundHalfX;
		int16_t boundHalfZ;
		int16_t boundAngle;
		int16_t defenseMode;
		uint8_t latSpeedNoTarget;
		uint8_t latSpeedTarget;
		uint8_t speedNoTarget;
		uint8_t speedTarget;
	};

	extern CreatureListRam g_creatureListRam[64];

	void LoadPacketData(char* fileName);
	void LoadRawAndNGN(const char* filePath);

	STATIC_ASSERT(sizeof(CreatureListRam) == 0x20);
	STATIC_ASSERT(sizeof(CreatureListRam::EntityControl) == 0x4);
}
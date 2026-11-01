#pragma once

#include "Common.h"
#include "Numerics.h"
#include <stddef.h>

namespace Toy2
{
	namespace Levels
	{
		struct RecordData
		{
			uint16_t recordCount;
			uint16_t recordType;
			Vector3I data[];
		};

		struct Type63Properties
		{
			uint8_t linkId;
			uint8_t flags;
			uint16_t floorPos;
		};

		struct Type63
		{
			Vector3I pos;
			Type63Properties props;
		};

		struct ModelTypeData_A
		{
			int32_t unk0;
			int32_t unk4;
			int32_t unk8;
			Vector3I16 rotation;
			int16_t scaleX;
			int16_t scaleY;
			int16_t scaleZ;
			int8_t flags;
			int8_t unk19;
			int8_t unk1A;
			int8_t unk1B;
			int32_t meshPtr;
			int32_t animPtr;
		};

		struct ModelTypeData_B
		{
			int16_t mat_11;
			int16_t mat_12;
			int16_t mat_13;
			int16_t mat_21;
			int16_t mat_22;
			int16_t mat_23;
			Vector3I16 rotation;
			uint8_t flags;
			uint8_t unk;
			void* modelPtr;
			int32_t unk11;
			int16_t polyCount;
			int16_t unk13;
		};

		struct InstanceSection
		{
			int32_t x;
			int32_t y;
			int32_t z;
			int16_t modelId;
			int8_t flags;
			int8_t category;
			ModelTypeData_B* unk6;
		};

		struct Type64
		{
			Vector3I unkVar0;
			Vector3I unkVar3;
			int32_t unkVar6;
			void* recordPtr;
			void* unkVar8;
		};

		struct ObjectDesc
		{
			int16_t unkVar1;
			int16_t unkVar2;
			int16_t id;
			int16_t unkVar3;
			int32_t unkVar4;
		};

		struct Object
		{
			int16_t modelId;
			int8_t flags;
			int8_t _pad3;
			int32_t unk6;
			int32_t z_or_coord;
			int16_t type;
			uint8_t unk14;
			uint8_t unk15;
			ObjectDesc* desc;
		};

		struct ObjectList
		{
			int32_t count;
			Object* entries[];
		};

		struct ObjectDescCache
		{
			int32_t unkInt1;
			int32_t unkInt2;
			int32_t unkInt3;
			int16_t unkInt4;
			int16_t unkInt5;
			int16_t unkInt6;
			int16_t unkInt7;
			int16_t unkInt8;
			int16_t unkInt9;
			int16_t unkInt10;
			int16_t unkInt11;
		};

		struct PortalEntry
		{
			uint8_t recordIdx;
			uint8_t categoryIdx;
		};

		// the records of the portals, the corners and the normal are packed as 3 shorts
		struct PortalQuad
		{
			uint16_t recordCount;
			uint16_t recordType;
			int16_t p0x;
			int16_t p0y;
			int16_t p0z;
			int16_t nx;
			int16_t ny;
			int16_t nz;
			int16_t p1x;
			int16_t p1y;
			int16_t p1z;
			int16_t unk16[3];
			int16_t p2x;
			int16_t p2y;
			int16_t p2z;
			int16_t unk22[3];
			int16_t p3x;
			int16_t p3y;
			int16_t p3z;
		};

		struct PortalZone
		{
			PortalEntry entries[16];
		};

		struct DatContents
		{
			uint8_t unkByte1;
			uint8_t unkByte2;
			uint8_t unkByte3;
			uint8_t unkByte4;
			uint16_t unkShort1;
			uint16_t unkShort2;
			uint16_t unkShort3;
			uint16_t unkShort4;
			uint16_t unkShort5;
			uint16_t unkShort6;
			uint16_t unkShort7;
			uint16_t unkShort8;
			Matrix3x3I16 unkVec;
			uint8_t pad[14];
		};

		extern RecordData* g_recordData[117];
		extern int32_t g_hasZoneData;
		extern ObjectList* g_objectListBase;
		extern PortalZone g_portalEntries[20];
		extern int32_t g_type64Count;
		extern Type64 g_type64Structs[24];
		extern int32_t g_currentType64Region;
		extern char g_activeZoneVisTable[260];
		extern InstanceSection* g_instanceSection;
		extern InstanceSection* g_secondInstanceSection;
		extern DatContents g_datContents[20];
		extern int32_t g_unk5574FC;
		extern int32_t g_unk547ED8;
		extern int32_t g_unk54E054;
		extern uint8_t* g_unk550C20;
		extern int32_t g_type63CullDistance;
		extern uint8_t g_levelDataHeapBase[1249280];
		STATIC_ASSERT(sizeof(g_levelDataHeapBase) == 0x131000);
		extern int32_t g_levelLoadConfig;
		extern uint8_t* g_levelLoadArena;

		void BuildLevelPath(int32_t level, char* output, const char* suffix);
		void FlushRenderer();
		int32_t InitLevelDefaults();
		void InitLevelPlay(int32_t levelId);
		int32_t LoadDAT(int32_t levelId, int32_t fileSize);
		int32_t GetSectorAtPosition(Vector3I* position);
		int32_t Method2(int32_t a1, int16_t a2);
		void Method1(int32_t a1, int32_t a2, int32_t a3, int32_t a4);
		void Method3(int32_t a1, int32_t a2);

		STATIC_ASSERT(sizeof(RecordData) == 0x4);
		STATIC_ASSERT(sizeof(Type63) == 0x10);
		STATIC_ASSERT(sizeof(ModelTypeData_A) == 0x24);
		STATIC_ASSERT(offsetof(ModelTypeData_A, rotation) == 0xC);
		STATIC_ASSERT(offsetof(ModelTypeData_A, scaleX) == 0x12);
		STATIC_ASSERT(offsetof(ModelTypeData_A, flags) == 0x18);
		STATIC_ASSERT(offsetof(ModelTypeData_A, meshPtr) == 0x1C);
		STATIC_ASSERT(offsetof(ModelTypeData_A, animPtr) == 0x20);
		STATIC_ASSERT(sizeof(ModelTypeData_B) == 0x20);
		STATIC_ASSERT(offsetof(ModelTypeData_B, rotation) == 0xC);
		STATIC_ASSERT(offsetof(ModelTypeData_B, flags) == 0x12);
		STATIC_ASSERT(offsetof(ModelTypeData_B, modelPtr) == 0x14);
		STATIC_ASSERT(offsetof(ModelTypeData_B, unk11) == 0x18);
		STATIC_ASSERT(offsetof(ModelTypeData_B, polyCount) == 0x1C);
		STATIC_ASSERT(sizeof(InstanceSection) == 0x14);
		STATIC_ASSERT(offsetof(InstanceSection, modelId) == 0xC);
		STATIC_ASSERT(offsetof(InstanceSection, flags) == 0xE);
		STATIC_ASSERT(offsetof(InstanceSection, unk6) == 0x10);
		STATIC_ASSERT(sizeof(Type64) == 0x24);
		STATIC_ASSERT(offsetof(Type64, recordPtr) == 0x1C);
		STATIC_ASSERT(offsetof(Type64, unkVar8) == 0x20);
		STATIC_ASSERT(sizeof(Object) == 0x14);
		STATIC_ASSERT(sizeof(ObjectDesc) == 0xC);
		STATIC_ASSERT(sizeof(ObjectDescCache) == 0x1C);
		STATIC_ASSERT(offsetof(ObjectDescCache, unkInt4) == 0xC);
		STATIC_ASSERT(offsetof(ObjectDescCache, unkInt7) == 0x12);
		STATIC_ASSERT(offsetof(ObjectDescCache, unkInt8) == 0x14);
		STATIC_ASSERT(sizeof(ObjectList) == 0x4);
		STATIC_ASSERT(sizeof(PortalEntry) == 0x2);
		STATIC_ASSERT(sizeof(PortalQuad) == 0x2E);
		STATIC_ASSERT(sizeof(PortalZone) == 0x20);
		STATIC_ASSERT(sizeof(DatContents) == 0x34);
	}

	namespace Terrain
	{
		// Directory record following the word-sized collision payloads.
		struct TerrainInstance
		{
			int32_t dataSizeWords;
			int32_t x;
			int32_t y;
			int32_t z;
			int32_t type;
			uint8_t pad20[6];
			int16_t platformSlot;
			uint8_t pad28[12];
			int16_t sectorAndCollisionFlags;
			uint8_t pad42[26];
			int16_t copySlot;
			int16_t isCopy;
			uint8_t pad72[4];
		};

		STATIC_ASSERT(sizeof(TerrainInstance) == 0x4C);
		STATIC_ASSERT(offsetof(TerrainInstance, dataSizeWords) == 0);
		STATIC_ASSERT(offsetof(TerrainInstance, type) == 0x10);
		STATIC_ASSERT(offsetof(TerrainInstance, platformSlot) == 0x1A);
		STATIC_ASSERT(offsetof(TerrainInstance, sectorAndCollisionFlags) == 0x28);
		STATIC_ASSERT(offsetof(TerrainInstance, copySlot) == 0x44);
		STATIC_ASSERT(offsetof(TerrainInstance, isCopy) == 0x46);

		int32_t LoadAll(const char* fileName, int32_t index, void** buffer);
	}
}

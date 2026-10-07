#pragma once

#include "Common.h"
#include "Numerics.h"

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

		struct PortalZone
		{
			PortalEntry entries[16];
		};

		extern RecordData* g_recordData[96];
		extern int32_t g_type63CullDistance;
		extern uint8_t g_levelDataHeapBase[1249280];
		extern int32_t g_levelLoadConfig;
		extern uint8_t* g_levelLoadArena;

		void FlushRenderer();
		int32_t InitLevelDefaults();
		void InitLevelPlay(int32_t levelId);
		int32_t GetSectorAtPosition(Vector3I* position);
		int32_t Method2(int32_t a1, int16_t a2);
		void Method1(int32_t a1, int32_t a2, int32_t a3, int32_t a4);
		void Method3(int32_t a1, int32_t a2);

		STATIC_ASSERT(sizeof(RecordData) == 0x4);
		STATIC_ASSERT(sizeof(Type63) == 0x10);
		STATIC_ASSERT(sizeof(ModelTypeData_A) == 0x24);
		STATIC_ASSERT(sizeof(ModelTypeData_B) == 0x20);
		STATIC_ASSERT(sizeof(InstanceSection) == 0x14);
		STATIC_ASSERT(sizeof(Object) == 0x14);
		STATIC_ASSERT(sizeof(ObjectDesc) == 0xC);
		STATIC_ASSERT(sizeof(ObjectDescCache) == 0x1C);
		STATIC_ASSERT(sizeof(ObjectList) == 0x4);
		STATIC_ASSERT(sizeof(PortalEntry) == 0x2);
		STATIC_ASSERT(sizeof(PortalZone) == 0x20);
	}

	namespace Terrain
	{
		int32_t LoadAll(const char* fileName, int32_t index, void** buffer);
	}
}
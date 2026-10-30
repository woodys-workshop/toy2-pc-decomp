#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Nu3D/Viewport.h"
#include "Nu3D/Sprite.h"
#include "Nu3D/Primitive.h"
#include "Nu3D/Material.h"
#include "Nu3D/Link.h"
#include "Nu3D/Portal.h"
#include "Nu3D/Creature.h"
#include "Nu3D/BmpDataNode.h"

#include <directx6/ddraw.h>
#include <directx6/d3d.h>
#include <stddef.h>

namespace Nu3D
{
	namespace Portal
	{
		struct AreaPortal;
	}
}

namespace NGNLoader
{
	struct TextureEntry
	{
		int32_t isBGR;
		int32_t textureDataIndex;
		int32_t auxiliaryData;
		int32_t auxiliaryValue;
	};

	struct Type266Entry
	{
		int16_t count;
		int16_t index;
		Vector3F* points;
	};

	struct NGNTextureParams
	{
		char* rawTexStr;
		uint32_t isTex14;
		RGBA color;
		uint32_t unkVar4;
		uint32_t unkVar5;
		uint32_t unkVar6;
		uint32_t unkVar7;
	};

	struct NGNImage
	{
		Nu3D::Primitive** primitives;
		int32_t primCount;
		Nu3D::Link::DynamicScaler* dynamicScalers[2];
		int32_t shapeCounts[2];
		int32_t gscaleType;
		Nu3D::Link::DynamicScaler** spacialGrid[2];
		float worldMinX;
		float worldMaxX;
		float worldMinZ;
		float worldMaxZ;
		int32_t gridWidth;
		int32_t gridHeight;
		float cellWidthInWorldUnits;
		float cellHeightInWorldUnits;
		Nu3D::Portal::AreaPortal** areaPortals;
		int32_t actualPortalCount;
		Nu3D::Link::Linker* links;
		int32_t maxLinkId;
		Type266Entry* data266[128];
		Nu3D::Portal::PortalHashTable* portalHashTable;
		Nu3D::Portal::ScalerEntry* scalerEntryPool;
		int32_t scalerEntryCount;
		int32_t maxScalerEntries;
		Nu3D::Portal::PortalState* portalStatePool;
		int32_t portalEntryCount;
		int32_t areaPortalCount;
		int32_t bucketCount;
		Nu3D::Creature** creatureData;
		int32_t creatureCount;
		TextureEntry textureEntries[64];

		static void DestroyPortal(Nu3D::Portal::AreaPortal* portal);
		static void DestroyPools(NGNImage* ngnImage);
		static void Destroy(NGNImage* ngnImage);
	};

	struct NGNTextureData
	{
		NGNTextureData* next;
		NGNTextureData* prev;
		uint32_t isTex14;
		uint32_t textureIndex;
		Nu3D::BmpDataNode* bmpDataNode;
		RGBA color;
		uint32_t textureCacheIndex;
	};

	struct NGNTextureDataSentinal
	{
		NGNTextureData* freeList;
		NGNTextureData* activeList;
	};

	struct NGNTextureCache
	{
		NGNTextureCache* next;
		NGNTextureCache* prev;
		char texName[256];
		NGNTextureParams params;
		uint32_t textureIndex;
	};

	struct NGNTextureCacheSentinal
	{
		NGNTextureCache* activeList;
		NGNTextureCache* freeList;
	};

	STATIC_ASSERT(sizeof(NGNImage) == 0x67C);
	STATIC_ASSERT(sizeof(NGNTextureData) == 0x1C);
	STATIC_ASSERT(sizeof(NGNTextureData) == sizeof(Nu3D::Viewport::FustrumInfo));
	STATIC_ASSERT(offsetof(NGNTextureData, isTex14) == offsetof(Nu3D::Viewport::FustrumInfo, unkInt3));
	STATIC_ASSERT(sizeof(NGNTextureDataSentinal) == 0x8);
	STATIC_ASSERT(sizeof(NGNTextureCache) == 0x128);
	STATIC_ASSERT(sizeof(NGNTextureCacheSentinal) == 0x8);

	STATIC_ASSERT(sizeof(TextureEntry) == 0x10);
	STATIC_ASSERT(sizeof(Type266Entry) == 0x8);
	STATIC_ASSERT(offsetof(TextureEntry, auxiliaryData) == 0x8);
	STATIC_ASSERT(offsetof(TextureEntry, auxiliaryValue) == 0xC);
	STATIC_ASSERT(offsetof(Type266Entry, count) == 0x0);
	STATIC_ASSERT(offsetof(Type266Entry, index) == 0x2);
	STATIC_ASSERT(offsetof(Type266Entry, points) == 0x4);
	STATIC_ASSERT(sizeof(Vector3F) == 0xC);
	STATIC_ASSERT(offsetof(NGNImage, data266) == 0x54);
	STATIC_ASSERT(sizeof(((NGNImage*)0)->data266) == 0x200);
	STATIC_ASSERT(offsetof(NGNImage, portalHashTable) == 0x254);
	STATIC_ASSERT(offsetof(NGNImage, textureEntries) == 0x27C);
}

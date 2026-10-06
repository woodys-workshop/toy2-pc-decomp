#include "Toy2/Levels.h"

#include "SoftwareRenderer.h"
#include "FileUtils.h"
#include "Random.h"
#include "Logger.h"
#include "CharacterLoader.h"
#include "RawLoader.h"
#include "Collision.h"

#include "NGNLoader/NGNLoader.h"
#include "Toy2/Toy2.h"
#include "Toy2/Collectables.h"
#include "Toy2/Actor.h"
#include "Toy2/Animation.h"
#include "AudioManager/AudioManager.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shadows.h"
#include "Renderer/Sprite.h"
#include "Renderer/SpriteSheets.h"
#include "Nu3D/Particles.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Light.h"
#include "SaveManager.h"
#include "Toy2/D3DApp.h"
#include "Toy2/Weather.h"

namespace Toy2
{
	namespace Levels
	{
		// GLOBAL: TOY2 0x00559C70
		RecordData* g_recordData[96];

		// GLOBAL: TOY2 0x0055A114
		int32_t g_levelLoadConfig;

		// GLOBAL: TOY2 0x0055A12C
		int32_t g_levelLoadConfigCopy;

		// GLOBAL: TOY2 0x00729128
		void* g_cachedAllBuffer;

		// GLOBAL: TOY2 0x005D2BE8
		uint8_t g_levelDataHeapBase[1249280];

		// GLOBAL: TOY2 0x00703C00
		uint8_t* g_levelDataHeapBasePtr;

		// GLOBAL: TOY2 0x00559DF4
		uint8_t* g_levelLoadArena;

		// GLOBAL: TOY2 0x0054DE98
		void* g_levelDataBase;

		// GLOBAL: TOY2 0x00556FB8
		int32_t g_unused1;

		// GLOBAL: TOY2 0x005571D0
		int32_t g_unused2;

		// GLOBAL: TOY2 0x00556FB4
		int32_t g_unused3;

		// GLOBAL: TOY2 0x005571CC
		int32_t g_unused4;

		// GLOBAL: TOY2 0x0054F090
		int32_t g_unused5;

		// GLOBAL: TOY2 0x00557AA8
		int32_t g_unused6;

		// GLOBAL: TOY2 0x00547EE8
		int32_t g_unused7;

		// GLOBAL: TOY2 0x00557AA0
		int32_t g_unused8;

		// GLOBAL: TOY2 0x0054DEB8
		int32_t g_unused9;

		// GLOBAL: TOY2 0x0054E07C
		int32_t g_unused10;

		// GLOBAL: TOY2 0x00556FA4
		int32_t g_unused11;

		// GLOBAL: TOY2 0x0054F61C
		int32_t g_unused12;

		// GLOBAL: TOY2 0x0054DD58
		int32_t g_hasZoneData;

		// GLOBAL: TOY2 0x0054BEF0
		int32_t g_type63CullDistance;

		// GLOBAL: TOY2 0x00557C20
		int16_t g_layerScaleTable[4095];

		// GLOBAL: TOY2 0x00557AAC
		int32_t g_type64Count;

		// GLOBAL: TOY2 0x00554040
		Type64 g_type64Structs[24];

		// GLOBAL: TOY2 0x00556FA8
		int32_t g_instanceRecordCount;

		// GLOBAL: TOY2 0x0054E048
		InstanceSection* g_instanceSection;

		// GLOBAL: TOY2 0x0054F628
		ObjectList* g_objectListBase;

		// GLOBAL: TOY2 0x0054DD80
		InstanceSection* g_secondInstanceSection;

		// GLOBAL: TOY2 0x00559C4C
		ObjectDescCache* g_objectDescCache;

		// STUB: TOY2 0x004CEA20
		void FlushRenderer() {}

		// STUB: TOY2 0x004885C0
		int32_t GetSectorAtPosition(Vector3I* position)
		{
			// NOTE: only the path for levels without zone data is ported (level select is one of them).
			// Levels with zone data walk the collision triangles to find the sector under the position, that part
			// returns "no sector" (-1) for now, which makes the world render by scanning the cells around the camera.

			if (! g_hasZoneData)
				return 0;

			return -1;
		}

		// FUNCTION: TOY2 0x004CE8B0
		int32_t InitLevelDefaults()
		{
			Renderer::Init();
			Renderer::BuildGammaCorrectionLUT(g_toyCfgData.gammaCorrection);

			Nu3D::SetUseAsDiffuseModulation(0);
			Nu3D::SetDefaultPrimFlags(32);

			Nu3D::Light::BuildGlobalLights();
			Nu3D::Camera::ApplyTransformToCamera(0);

			Toy2::Animation::ResetNodeAngles();

			SoftwareRenderer::InitialisePrimarySurface_T();

			Renderer::GetBlendShadeCaps(&Renderer::g_deviceBlendShadeCapsCpy);

			SaveManager::SaveToFile(0, "default.cfg");

			FILE* fd = fopen("toy2.cfg", "wb");

			if (! fd)
				return 0;

			fwrite(&g_toyCfgData, 1, sizeof(g_toyCfgData), fd);
			fclose(fd);

			return 1;
		}

		// FUNCTION: TOY2 0x0043E6E0
		int32_t LoadDAT(int32_t levelId, int32_t fileSize)
		{
			// FUNCTION IS WIP, very big and confusing
			// the commented out bits are what we don't need for
			// getting into level select thankfully

			int32_t l_cullBandWidth; // eax
			uint32_t l_scaleTableZeroCount; // esi
			int16_t* l_scaleTableCursor; // edx
			int16_t* l_scaleFillPtr; // edi
			int32_t l_oddTailCount; // ecx
			int16_t* l_region0600; // edi
			int16_t* l_region0400; // edx
			int16_t* l_region0512; // edx
			int32_t l_type64Count; // ebp
			int16_t l_type65Idx; // bx
			int16_t l_recordType; // ax
			int32_t l_type64Idx; // eax
			int32_t l_zoneAnchorIdx; // ecx
			int32_t* l_zoneAnchor; // eax
			int32_t l_portalVertIdx; // ecx
			int32_t l_portalVert; // eax
			int32_t l_coordRaw; // edx
			int32_t l_portalByteOffset; // eax
			int16_t l_portalCounts_; // dx
			int32_t l_termOffset; // ecx
			int32_t l_secondIdx; // eax
			InstanceSection* l_secondSection; // eax
			ModelTypeData_B* l_sectionStorage; // esi
			InstanceSection* l_secondSection_; // ebp
			int8_t l_secondSection14Val; // cl
			int8_t* l_secondSection14; // eax
			int16_t l_modelId; // cx
			ModelTypeData_B* l_typeData2; // edi
			ModelTypeData_A* l_typeData; // edi
			int32_t l_mat_21; // ecx
			int32_t l_mat_12; // ecx
			int32_t l_mat_22; // edx
			int32_t l_secRotY; // ecx
			int32_t l_mat_13; // edx
			int32_t l_secRotZ; // edx
			int32_t l_secRotZValue; // eax
			int32_t l_secondSectionCount; // ecx
			int32_t l_secondInstance; // ebp
			int16_t l_modelIdRaw; // ax
			ModelTypeData_B* l_modelB2; // edi
			ModelTypeData_A* l_modelA2; // edi
			int32_t l_secMat21; // ecx
			int32_t l_secMat12; // ecx
			int32_t l_secMat22; // edx
			int32_t l_secRotY2; // ecx
			int32_t l_secMat13; // edx
			int32_t l_secRotZ2; // edx
			int32_t l_secRotZ2Value; // eax
			int8_t l_nextFlags; // al
			int32_t l_lodTrailerSize; // ecx
			Type64* l_record; // ebp
			int32_t* l_unkVar0Y; // edi
			int32_t* l_unkVar0Z; // esi
			int32_t l_recordOffset; // edx
			int32_t l_extentZ; // edx
			int32_t l_extentY; // esi
			int32_t l_extentX; // edi
			ObjectDescCache* l_descCache; // eax
			ObjectList* l_objectList; // ecx
			int32_t l_objIdx; // edi
			int32_t l_cacheByteOffset; // eax
			int32_t l_objEntryOffset; // edx
			uint32_t l_meshPtr; // esi
			int32_t l_objDesc; // ecx
			int32_t l_totalRecordCount; // [esp+10h] [ebp-30h]
			int16_t* l_newLvlBase; // [esp+10h] [ebp-30h]
			int32_t l_type64Count_; // [esp+14h] [ebp-2Ch]
			int16_t l_portalCounts[20]; // [esp+18h] [ebp-28h] BYREF
			int32_t l_bboxVert; // [esp+48h] [ebp+8h]
			int32_t l_bboxVert2; // [esp+48h] [ebp+8h]
			char* l_bboxVert3; // [esp+48h] [ebp+8h]
			char* l_bboxVert4; // [esp+48h] [ebp+8h]

			Levels::g_unused1 = 256;
			Levels::g_unused2 = 256;

			g_hasZoneData = 0;
			Weather::g_spawnAccumulator = 0;
			Weather::g_weatherBasePointer = 0;

			Levels::g_unused3 = 512;
			Levels::g_unused4 = 512;

			if (D3DApp::g_renderMode == RENDERMODE_SOFTWARE || D3DApp::g_renderMode == RENDERMODE_D3D)
			{
				l_cullBandWidth = 1664;
				g_type63CullDistance = 1664;
			}
			else
			{
				l_cullBandWidth = g_type63CullDistance;
			}

			l_scaleTableZeroCount = l_cullBandWidth - 512;

			Levels::g_unused5 = l_cullBandWidth;
			Levels::g_unused6 = l_cullBandWidth - 512;
			Levels::g_unused7 = 3072;

			l_scaleTableCursor = g_layerScaleTable;
			Levels::g_unused8 = 170;

			if (l_cullBandWidth - 512 > 0)
			{
				l_scaleTableCursor = &g_layerScaleTable[l_scaleTableZeroCount];

				memset(g_layerScaleTable, 0, 4 * (l_scaleTableZeroCount >> 1));
				l_scaleFillPtr = &g_layerScaleTable[2 * (l_scaleTableZeroCount >> 1)];

				for (l_oddTailCount = l_cullBandWidth & 1; l_oddTailCount; --l_oddTailCount)
					*l_scaleFillPtr++ = 0;
			}

			l_region0600 = l_scaleTableCursor;
			l_region0400 = l_scaleTableCursor + 170;

			Nu3D::MemSet32Util(l_region0600, 85, 0x6000600);
			Nu3D::MemSet32Util(l_region0400, 85, 0x4000400);

			for (int16_t* p = l_region0400 + 170; p != g_layerScaleTable + (sizeof(g_layerScaleTable) / sizeof(g_layerScaleTable[0])); ++p)
				*p = 512;

			uint8_t* l_base = reinterpret_cast<uint8_t*>(g_levelDataBase);
			l_type64Count = 0;
			Levels::g_unused9 = 0;

			g_type64Count = 0;

			Type64* end = g_type64Structs + (sizeof(g_type64Structs) / sizeof(g_type64Structs[0]));
			Type64* l_type64Init = g_type64Structs;

			do
			{
				(l_type64Init++)->recordPtr = 0;
			} while (l_type64Init < end);

			uint8_t* l_recordPtr = (l_base + 4);
			memset(l_portalCounts, 0, sizeof(l_portalCounts));

			l_type65Idx = 65;

			if (*reinterpret_cast<uint16_t*>(l_recordPtr - 4) > 0)
			{
				l_totalRecordCount = *reinterpret_cast<uint16_t*>(l_recordPtr - 4);

				printf("Total Record Count -> %d\n", l_totalRecordCount);

				do
				{
					l_recordType = *reinterpret_cast<uint16_t*>(l_recordPtr + 2);

					if (l_recordType >= 0)
					{
						if (l_recordType == 64)
						{
							l_type64Idx = l_type64Count++;

							g_type64Count = l_type64Count;
							g_type64Structs[l_type64Idx].recordPtr = l_recordPtr;

							l_recordPtr += 12 * *reinterpret_cast<uint16_t*>(l_recordPtr) + 4;
						}
						else
						{
							if (l_recordType <= 64)
							{
								g_recordData[l_recordType] = reinterpret_cast<RecordData*>(l_recordPtr);
							}
							else
							{
								// g_hasZoneData = 1;
								// g_recordData[l_type65Idx] = reinterpret_cast<RecordData*>(l_recordPtr);

								// l_zoneAnchorIdx = *(l_recordPtr + 12 * *l_recordPtr - 8);
								// l_zoneAnchor = reinterpret_cast<int32_t*>(l_recordPtr + 12 * *l_recordPtr - 8);

								// *(&g_portalEntries[l_zoneAnchorIdx].entries[0].recordIdx + l_portalCounts[l_zoneAnchorIdx]) = l_type65Idx;
								// *(&g_portalEntries[*l_zoneAnchor].entries[0].categoryIdx + l_portalCounts[*l_zoneAnchor]) = *(l_zoneAnchor + 4);

								// l_portalVertIdx = 0;
								// l_portalCounts[*l_zoneAnchor] += 2;

								// l_portalVert = l_recordPtr + 4;

								// if (*l_recordPtr > 0)
								// {
								// 	do
								// 	{
								// 		l_coordRaw = *l_portalVert;
								// 		l_portalVert += 12;

								// 		*(l_portalVert - 12) = l_coordRaw >> 2;
								// 		*(l_portalVert - 10) = *(l_portalVert - 8) >> 2;
								// 		*(l_portalVert - 8) = *(l_portalVert - 4) >> 2;

								// 		++l_portalVertIdx;
								// 	} while (l_portalVertIdx < *l_recordPtr);
								// }

								// l_type64Count = g_type64Count;
								// ++l_type65Idx;
							}

							if (*reinterpret_cast<uint16_t*>(l_recordPtr + 2) == 63)
								l_recordPtr += 16 * *reinterpret_cast<uint16_t*>(l_recordPtr) + 4;
							else
								l_recordPtr += 12 * *reinterpret_cast<uint16_t*>(l_recordPtr) + 4;
						}
					}
					else
					{
						g_recordData[-l_recordType] = reinterpret_cast<RecordData*>(l_recordPtr);
						l_recordPtr += 4 * ((3 * *reinterpret_cast<uint16_t*>(l_recordPtr) + 1) / 2) + 16;
					}

					--l_totalRecordCount;
				} while (l_totalRecordCount);
			}

			/*
			l_portalByteOffset = 0;
			*&l_portalCounts_ = l_portalCounts;
			do
			{
				l_termOffset = l_portalByteOffset + **&l_portalCounts_;
				l_portalByteOffset += 32;
				*&l_portalCounts_ += 2;

				*(&g_portalEntries[0].entries[0].recordIdx + l_termOffset) = -1;
				*(&g_portalEntries[0].entries[0].categoryIdx + l_termOffset) = -1;

			} while (l_portalByteOffset < 640);*/

			l_secondIdx = *l_recordPtr << 7;
			g_instanceRecordCount = 0;

			l_secondSection = (InstanceSection*)(l_recordPtr + 4 * (l_secondIdx / 4) + 4);
			l_sectionStorage = (ModelTypeData_B*)((uint8_t*)g_levelDataBase + fileSize);

			printf("Second Idx %d\n", l_secondIdx);

			g_instanceSection = l_secondSection;

			l_secondSection_ = l_secondSection;
			l_secondSection14Val = l_secondSection->flags;
			l_secondSection14 = &l_secondSection->flags;

			printf("l_secondSection14 %d\n", l_secondSection14Val);

			/*
			if (l_secondSection14Val)
			{
				do
				{
					++g_instanceRecordCount;
					l_modelId = l_secondSection_->modelId;

					if (l_modelId < 0)
						l_secondSection_->modelId = -l_modelId;

					if (*l_secondSection14 < 0)
						*l_secondSection14 = (*l_secondSection14 | 0x50) - 16;

					if ((*l_secondSection14 & 8) != 0)
					{
						l_typeData = (g_levelDataBase + l_secondSection_->unk6);
						l_secondSection_->unk6 = l_typeData;
						l_typeData->meshPtr += g_levelDataBase;

						if ((*l_secondSection14 & 0x20) != 0)
						{
							*l_secondSection14 |= 0x40u;
							l_typeData->animPtr += g_levelDataBase;
						}

						Nu3D::Math::EulerToRotationMatrix(&l_typeData->rotation, l_sectionStorage);

						l_mat_21 = l_sectionStorage->mat_21;
						l_sectionStorage->mat_11 = l_typeData->scaleX * l_sectionStorage->mat_11 / 4096;
						l_sectionStorage->mat_21 = l_mat_21 * l_typeData->scaleX / 4096;
						l_mat_12 = l_sectionStorage->mat_12;
						l_sectionStorage->rotation.x = l_typeData->scaleX * l_sectionStorage->rotation.x / 4096;
						l_mat_22 = l_sectionStorage->mat_22;

						l_sectionStorage->mat_12 = l_mat_12 * l_typeData->scaleY / 4096;
						l_secRotY = l_sectionStorage->rotation.y;
						l_sectionStorage->mat_22 = l_mat_22 * l_typeData->scaleY / 4096;
						l_mat_13 = l_sectionStorage->mat_13;
						l_sectionStorage->rotation.y = l_secRotY * l_typeData->scaleY / 4096;
						l_sectionStorage->mat_13 = l_mat_13 * l_typeData->scaleZ / 4096;
						l_secRotZ = l_sectionStorage->rotation.z;
						l_sectionStorage->mat_23 = l_typeData->scaleZ * l_sectionStorage->mat_23 / 4096;
						l_secRotZValue = l_secRotZ * l_typeData->scaleZ;

						l_sectionStorage->modelPtr = l_typeData;
						l_sectionStorage->unk11 = 0;
						l_sectionStorage->rotation.z = l_secRotZValue / 4096;

						l_secondSection_->unk6 = l_sectionStorage;
						l_sectionStorage->polyCount = Toy2::Level::CalculatePolyCount(l_secondSection_);
						l_typeData->flags &= 0xFu;
					}
					else
					{
						l_typeData2 = (g_levelDataBase + l_secondSection_->unk6);
						l_secondSection_->unk6 = l_typeData2;
						l_typeData2->modelPtr = l_typeData2->modelPtr + g_levelDataBase;

						if ((*l_secondSection14 & 0x20) != 0)
						{
							*l_secondSection14 |= 0x40u;
							l_typeData2->unk11 += g_levelDataBase;
						}

						Nu3D::Math::EulerToRotationMatrix(&l_typeData2->rotation, l_sectionStorage);

						l_sectionStorage->modelPtr = l_typeData2;
						l_sectionStorage->unk11 = 0;
						l_secondSection_->unk6 = l_sectionStorage;

						l_sectionStorage->polyCount = Toy2::Level::CalculatePolyCount(l_secondSection_);
						l_typeData2->flags &= 0xFu;
					}

					++l_secondSection_;
					++l_sectionStorage;

					l_secondSection14 = &l_secondSection_->flags;
				} while (l_secondSection_->flags);
			}*/

			g_objectListBase = (ObjectList*)((uint8_t*)l_secondSection_ + sizeof(InstanceSection));
			l_secondSectionCount = *(int32_t*)((uint8_t*)l_secondSection_ + sizeof(InstanceSection));

			g_secondInstanceSection = (InstanceSection*)((uint8_t*)l_secondSection_ + 20 + 4 * l_secondSectionCount + 8);

			printf("Second section count -> %d\n", l_secondSectionCount);
			printf("Second Instance Flags -> %d\n", g_secondInstanceSection->flags);

			/*
			l_secondInstance = ((uint8_t*)l_secondSection_ + 20 + 4 * l_secondSectionCount + 22);

			if (*(g_secondInstanceSection + 14))
			{
				do
				{
					++g_instanceRecordCount;
					l_modelIdRaw = *(l_secondInstance - 2);

					if (l_modelIdRaw < 0)
						*(l_secondInstance - 2) = -l_modelIdRaw;

					if (*l_secondInstance < 0)
						*l_secondInstance = (*l_secondInstance | 0x50) - 16;

					if ((*l_secondInstance & 8) != 0)
					{
						l_modelA2 = (g_levelDataBase + *(l_secondInstance + 2));
						*(l_secondInstance + 2) = l_modelA2;
						l_modelA2->meshPtr += g_levelDataBase;

						if ((*l_secondInstance & 0x20) != 0)
						{
							*l_secondInstance |= 0x40u;
							l_modelA2->animPtr += g_levelDataBase;
						}

						Nu3D::Math::EulerToRotationMatrix(&l_modelA2->rotation, l_sectionStorage);
						l_secMat21 = l_sectionStorage->mat_21;
						l_sectionStorage->mat_11 = l_modelA2->scaleX * l_sectionStorage->mat_11 / 4096;
						l_sectionStorage->mat_21 = l_secMat21 * l_modelA2->scaleX / 4096;
						l_secMat12 = l_sectionStorage->mat_12;
						l_sectionStorage->rotation.x = l_modelA2->scaleX * l_sectionStorage->rotation.x / 4096;
						l_secMat22 = l_sectionStorage->mat_22;
						l_sectionStorage->mat_12 = l_secMat12 * l_modelA2->scaleY / 4096;
						l_secRotY2 = l_sectionStorage->rotation.y;
						l_sectionStorage->mat_22 = l_secMat22 * l_modelA2->scaleY / 4096;
						l_secMat13 = l_sectionStorage->mat_13;
						l_sectionStorage->rotation.y = l_secRotY2 * l_modelA2->scaleY / 4096;
						l_sectionStorage->mat_13 = l_secMat13 * l_modelA2->scaleZ / 4096;
						l_secRotZ2 = l_sectionStorage->rotation.z;
						l_sectionStorage->mat_23 = l_modelA2->scaleZ * l_sectionStorage->mat_23 / 4096;
						l_secRotZ2Value = l_secRotZ2 * l_modelA2->scaleZ;
						l_sectionStorage->modelPtr = l_modelA2;
						l_sectionStorage->unk11 = 0;
						l_sectionStorage->rotation.z = l_secRotZ2Value / 4096;

						*(l_secondInstance + 2) = l_sectionStorage;
						l_sectionStorage->polyCount = Toy2::Level::CalculatePolyCount((l_secondInstance - 14));
						l_modelA2->flags &= 0xFu;
					}
					else
					{
						l_modelB2 = (g_levelDataBase + *(l_secondInstance + 2));
						*(l_secondInstance + 2) = l_modelB2;
						l_modelB2->modelPtr = l_modelB2->modelPtr + g_levelDataBase;

						if ((*l_secondInstance & 0x20) != 0)
						{
							*l_secondInstance |= 0x40u;
							l_modelB2->unk11 += g_levelDataBase;
						}

						Nu3D::Math::EulerToRotationMatrix(&l_modelB2->rotation, l_sectionStorage);
						l_sectionStorage->modelPtr = l_modelB2;
						l_sectionStorage->unk11 = 0;
						*(l_secondInstance + 2) = l_sectionStorage;
						l_sectionStorage->polyCount = Toy2::Level::CalculatePolyCount((l_secondInstance - 14));

						l_modelB2->flags &= 0xFu;
					}

					l_nextFlags = *(l_secondInstance + 20);
					l_secondInstance += 20;
					++l_sectionStorage;
				} while (l_nextFlags);
			}*/

			l_lodTrailerSize = *(int32_t*)((uint8_t*)g_levelDataBase + fileSize - 4);
			printf("Load trailer size -> %d\n", l_lodTrailerSize);
			printf("Object List Base Count -> %d\n", g_objectListBase->count);

			/*
			if (l_lodTrailerSize == -1)
			{
				Levels::g_unused10 = -1;
				memset(g_activeZoneVisTable, 255u, 256u);
			}
			else
			{
				l_newLvlBase = (g_levelDataBase + p_fileSize - l_lodTrailerSize);

				if (g_type64Count > 0)
				{
					l_type64Records = &g_type64Structs[0].recordPtr;
					l_type64Count_ = g_type64Count;

					do
					{
						l_record = ADJ(l_type64Records);

						l_unkVar0Y = &ADJ(l_type64Records)->unkVar0.y;
						l_unkVar0Z = &ADJ(l_type64Records)->unkVar0.z;

						ADJ(l_type64Records)->unkVar0.x = 0x7FFFFFFF;
						ADJ(l_type64Records)->unkVar0.y = 0x7FFFFFFF;
						ADJ(l_type64Records)->unkVar0.z = 0x7FFFFFFF;

						ADJ(l_type64Records)->unkVar3.x = 0x80000000;
						ADJ(l_type64Records)->unkVar3.y = 0x80000000;
						ADJ(l_type64Records)->unkVar3.z = 0x80000000;

						for (l_recordOffset = 0; l_recordOffset < 24; l_recordOffset += 12)
						{
							l_bboxVert = ADJ(l_type64Records)->recordPtr + l_recordOffset;

							if (*(l_bboxVert + 4) < l_record->unkVar0.x)
								l_record->unkVar0.x = *(l_bboxVert + 4);

							l_bboxVert2 = ADJ(l_type64Records)->recordPtr + l_recordOffset;

							if (*(l_bboxVert2 + 8) < *l_unkVar0Y)
								*l_unkVar0Y = *(l_bboxVert2 + 8);

							if (*(ADJ(l_type64Records)->recordPtr + l_recordOffset + 12) < *l_unkVar0Z)
								*l_unkVar0Z = *(ADJ(l_type64Records)->recordPtr + l_recordOffset + 12);

							l_bboxVert3 = ADJ(l_type64Records)->recordPtr + l_recordOffset;

							if (*(l_bboxVert3 + 1) > ADJ(l_type64Records)->unkVar3.x)
								ADJ(l_type64Records)->unkVar3.x = *(l_bboxVert3 + 1);

							l_bboxVert4 = ADJ(l_type64Records)->recordPtr + l_recordOffset;

							if (*(l_bboxVert4 + 2) > ADJ(l_type64Records)->unkVar3.y)
								ADJ(l_type64Records)->unkVar3.y = *(l_bboxVert4 + 2);

							if (*(ADJ(l_type64Records)->recordPtr + l_recordOffset + 12) > ADJ(l_type64Records)->unkVar3.z)
								ADJ(l_type64Records)->unkVar3.z = *(ADJ(l_type64Records)->recordPtr + l_recordOffset + 12);
						}

						l_extentZ = ADJ(l_type64Records)->unkVar3.z - *l_unkVar0Z;
						l_extentY = ADJ(l_type64Records)->unkVar3.y - *l_unkVar0Y;
						l_extentX = ADJ(l_type64Records++)->unkVar3.x - l_record->unkVar0.x;

						// Current unkVar6 (double check this)
						l_type64Records[-2].unkVar8 =
							((l_extentZ >> 5) * (l_extentZ >> 5) + (l_extentY >> 5) * (l_extentY >> 5) + (l_extentX >> 5) * (l_extentX >> 5));

						// Current unkVar8
						l_type64Records[-1].unkVar0.y = l_newLvlBase;

						l_newLvlBase = (l_newLvlBase + *l_newLvlBase);

						--l_type64Count_;
					} while (l_type64Count_);
				}

				g_currentType64Region = -1;
				Levels::g_unused11 = 1;
				Toy2::Level::LoaderHelper();
				Levels::g_unused10 = 0;
			}

			Levels::g_unused12 = 400;

			if (g_objectDescCache)
				free(g_objectDescCache);

			l_descCache = malloc(28 * (g_objectListBase->count + 1));
			l_objectList = g_objectListBase;
			g_objectDescCache = l_descCache;

			l_objIdx = 0;

			if (g_objectListBase->count >= 0)
			{
				l_cacheByteOffset = 0;
				l_objEntryOffset = 4;

				do
				{
					l_meshPtr = *(&l_objectList->count + l_objEntryOffset);

					if (l_meshPtr)
					{
						*(&l_objectList->count + l_objEntryOffset) = g_levelDataBase + l_meshPtr;

						g_objectDescCache[l_cacheByteOffset].unkInt7 = *(*(&g_objectListBase->count + l_objEntryOffset) + 14);

						l_objDesc = *(*(*(&g_objectListBase->count + l_objEntryOffset) + 16) + 20);

						g_objectDescCache[l_cacheByteOffset].unkInt8 = *(l_objDesc + 12);
						g_objectDescCache[l_cacheByteOffset].unkInt9 = *(l_objDesc + 14);
						g_objectDescCache[l_cacheByteOffset].unkInt10 = *(l_objDesc + 16);
						g_objectDescCache[l_cacheByteOffset].unkInt4 = *(l_objDesc + 18);
						g_objectDescCache[l_cacheByteOffset].unkInt5 = *(l_objDesc + 20);
						g_objectDescCache[l_cacheByteOffset].unkInt6 = *(l_objDesc + 22);

						l_objectList = g_objectListBase;
					}

					++l_objIdx;
					l_objEntryOffset += 4;
					++l_cacheByteOffset;
				} while (l_objIdx <= l_objectList->count);
			}

			if (g_hasZoneData)
				Toy2::InitZoneData();
			*/

			return 32 * g_instanceRecordCount;
		}

		// FUNCTION: TOY2 0x004521A0
		void BuildLevelPath(int32_t level, char* output, const char* suffix)
		{
			if (level >= 10)
			{
				strcpy(output, "level");
				output[5] = level / 10 + 48;
				output[6] = level % 10 + 48;
			}
			else
			{
				strcpy(output, "level0");
				output[6] = level + 48;
			}

			strcpy(output + 7, "\\");
			strcat(output, suffix);
		}

		// FUNCTION: TOY2 0x00452FC0
		void InitLevelPlay(int32_t levelId)
		{
			SoftwareRenderer::SetLevelFileIndex(g_levelFileIndex);
			FlushRenderer();
			InitLevelDefaults();

			FileUtils::LoadFile("rand.dat", g_randDatBuffer);

			int32_t levelIdCpy = levelId;

			AudioManager::g_curTrackIndex = -1;
			g_isElevatorHopLevel = levelId != 10;

			Logger::Log("InitLevelPlay : START.\n");

			AudioManager::LoadSfxPackForLevel(levelId);

			memset(&g_recordData, 0, sizeof(g_recordData));
			memset(CharacterLoader::g_boneTransforms, 0, sizeof(CharacterLoader::g_boneTransforms));
			memset(Collision::g_collisionMeshInstances, 0, sizeof(Collision::g_collisionMeshInstances));
			memset(Collision::g_mathScratch, 0, sizeof(Collision::g_mathScratch));

			SoftwareRenderer::g_unk4F7400.x = -32768;
			SoftwareRenderer::g_unk4F7400.y = -32768;

			g_hasBackdrop = 0;

			CharacterLoader::g_unk54717C = 0;
			CharacterLoader::g_unk546D78 = 0;
			CharacterLoader::g_unk547CD0 = 0;

			SoftwareRenderer::g_unk500A1C = -1;

			Renderer::Shadows::g_shadowCount = 0;
			Renderer::Shadows::g_unusedShadowVar = 0;

			for (int32_t particleIdx = 0; particleIdx < 64; particleIdx++)
				Nu3D::Particles::g_particleInstances[particleIdx].lifetime = 0;

			Nu3D::Camera::InitViewMatrixGlobals();

			memset(CharacterLoader::g_alternateAllParse, 0, sizeof(CharacterLoader::g_alternateAllParse));
			memset(CharacterLoader::g_charFileDataCache, 0, sizeof(CharacterLoader::g_charFileDataCache));

			g_cachedAllBuffer = 0;

			if (levelId > 10)
			{
				g_levelLoadConfig |= 256;

				if ((g_levelLoadConfig & 192) == 0)
					g_levelLoadConfig |= 64;

				levelIdCpy = levelId - 10;
			}

			int32_t loadConfigCpy = g_levelLoadConfig;
			g_levelLoadConfigCopy = g_levelLoadConfig;

			uint8_t* dataBuffer = g_levelDataHeapBase;

			g_levelDataHeapBasePtr = g_levelDataHeapBase;
			g_levelLoadArena = g_levelDataHeapBase;

			char fileNameBuffer[128];
			fileNameBuffer[0] = '\0';

			if ((loadConfigCpy & 1) == 0)
			{
				int32_t rawVariant = ((loadConfigCpy >> 6) & 3) - 1;
				const char* rawFileName;

				if (rawVariant)
				{
					int32_t rawVariantStep = rawVariant - 1;

					if (rawVariantStep)
					{
						if (rawVariantStep == 1)
						{
							if (levelIdCpy >= 10)
							{
								strcpy(fileNameBuffer, "level");
								fileNameBuffer[5] = levelIdCpy / 10 + 48;
								fileNameBuffer[6] = levelIdCpy % 10 + 48;
							}
							else
							{
								strcpy(fileNameBuffer, "level0");
								fileNameBuffer[6] = levelIdCpy + 48;
							}

							strcpy(&fileNameBuffer[7], "\\");
							rawFileName = "level3.raw";
						}
						else
						{
							if (levelIdCpy >= 10)
							{
								strcpy(fileNameBuffer, "level");
								fileNameBuffer[5] = levelIdCpy / 10 + 48;
								fileNameBuffer[6] = levelIdCpy % 10 + 48;
							}
							else
							{
								strcpy(fileNameBuffer, "level0");
								fileNameBuffer[6] = levelIdCpy + 48;
							}

							strcpy(&fileNameBuffer[7], "\\");
							rawFileName = "level.raw";
						}
					}
					else
					{
						if (levelIdCpy >= 10)
						{
							strcpy(fileNameBuffer, "level");
							fileNameBuffer[5] = levelIdCpy / 10 + 48;
							fileNameBuffer[6] = levelIdCpy % 10 + 48;
						}
						else
						{
							strcpy(fileNameBuffer, "level0");
							fileNameBuffer[6] = levelIdCpy + 48;
						}

						strcpy(&fileNameBuffer[7], "\\");
						rawFileName = "level2.raw";
					}
				}
				else
				{
					if (levelIdCpy >= 10)
					{
						strcpy(fileNameBuffer, "level");
						fileNameBuffer[5] = levelIdCpy / 10 + 48;
						fileNameBuffer[6] = levelIdCpy % 10 + 48;
					}
					else
					{
						strcpy(fileNameBuffer, "level0");
						fileNameBuffer[6] = levelIdCpy + 48;
					}

					strcpy(&fileNameBuffer[7], "\\");
					rawFileName = "level1.raw";
				}

				strcat(fileNameBuffer, rawFileName);
				RawLoader::LoadPacketData(fileNameBuffer);
				loadConfigCpy = g_levelLoadConfig;
			}

			dataBuffer = g_levelLoadArena;

			char levelDigits[4];

			if ((loadConfigCpy & 2) == 0)
			{
				int32_t binVariant = ((loadConfigCpy >> 8) & 3) - 1;

				if (binVariant)
				{
					int32_t binVariantStep = binVariant - 1;

					if (binVariantStep)
					{
						if (binVariantStep == 1)
						{
							strcpy(fileNameBuffer, "..\\level");

							if (levelIdCpy + 10 >= 10)
							{
								levelDigits[2] = 0;
								levelDigits[0] = (levelIdCpy + 10) / 10 + 48;
								levelDigits[1] = (levelIdCpy + 10) % 10 + 48;
							}
							else
							{
								levelDigits[0] = '0';
								levelDigits[1] = levelIdCpy + 58;
								levelDigits[2] = 0;
							}
						}
						else
						{
							strcpy(fileNameBuffer, "..\\level");

							if (levelIdCpy >= 10)
							{
								levelDigits[2] = 0;
								levelDigits[0] = levelIdCpy / 10 + 48;
								levelDigits[1] = levelIdCpy % 10 + 48;
							}
							else
							{
								levelDigits[0] = '0';
								levelDigits[2] = 0;
								levelDigits[1] = levelIdCpy + 48;
							}
						}
					}
					else
					{
						strcpy(fileNameBuffer, "..\\level");

						if (levelIdCpy + 10 >= 10)
						{
							levelDigits[2] = 0;
							levelDigits[0] = (levelIdCpy + 10) / 10 + 48;
							levelDigits[1] = (levelIdCpy + 10) % 10 + 48;
						}
						else
						{
							levelDigits[0] = '0';
							levelDigits[1] = levelIdCpy + 58;
							levelDigits[2] = 0;
						}
					}
				}
				else
				{
					strcpy(fileNameBuffer, "..\\level");

					if (levelIdCpy + 10 >= 10)
					{
						levelDigits[0] = (levelIdCpy + 10) / 10 + 48;
						levelDigits[1] = (levelIdCpy + 10) % 10 + 48;
					}
					else
					{
						levelDigits[0] = 48;
						levelDigits[1] = levelIdCpy + 58;
					}

					levelDigits[2] = 0;
				}

				strcat(fileNameBuffer, levelDigits);
				strcat(fileNameBuffer, ".bin");
			}

			Renderer::InitSpriteSheets();

			const char* datFileName;
			void* bufferPtr;
			int32_t charLoaderFlag = 0;

			memset(fileNameBuffer, 0, sizeof(fileNameBuffer));

			if ((g_levelLoadConfig & 4) == 0)
			{
				bufferPtr = dataBuffer;
				g_levelDataBase = dataBuffer;

				switch ((g_levelLoadConfig >> 8) & 3)
				{
					case 1:
						if (levelIdCpy >= 10)
						{
							strcpy(fileNameBuffer, "level");
							fileNameBuffer[5] = levelIdCpy / 10 + 48;
							fileNameBuffer[6] = levelIdCpy % 10 + 48;
						}
						else
						{
							strcpy(fileNameBuffer, "level0");
							fileNameBuffer[6] = levelIdCpy + 48;
						}

						strcpy(&fileNameBuffer[7], "\\");
						datFileName = "level1.dat";
						break;

					case 2:
						if (levelIdCpy >= 10)
						{
							strcpy(fileNameBuffer, "level");
							fileNameBuffer[5] = levelIdCpy / 10 + 48;
							fileNameBuffer[6] = levelIdCpy % 10 + 48;
						}
						else
						{
							strcpy(fileNameBuffer, "level0");
							fileNameBuffer[6] = levelIdCpy + 48;
						}

						strcpy(&fileNameBuffer[7], "\\");
						datFileName = "level2.dat";
						break;

					case 3:
						if (levelIdCpy >= 10)
						{
							strcpy(fileNameBuffer, "level");
							fileNameBuffer[5] = levelIdCpy / 10 + 48;
							fileNameBuffer[6] = levelIdCpy % 10 + 48;
						}
						else
						{
							strcpy(fileNameBuffer, "level0");
							fileNameBuffer[6] = levelIdCpy + 48;
						}

						strcpy(&fileNameBuffer[7], "\\");
						datFileName = "level3.dat";
						break;

					default:
						if (levelIdCpy >= 10)
						{
							strcpy(fileNameBuffer, "level");
							fileNameBuffer[5] = levelIdCpy / 10 + 48;
							fileNameBuffer[6] = levelIdCpy % 10 + 48;
						}
						else
						{
							strcpy(fileNameBuffer, "level0");
							fileNameBuffer[6] = levelIdCpy + 48;
						}

						strcpy(&fileNameBuffer[7], "\\");
						datFileName = "level.dat";
						break;
				}

				strcat(fileNameBuffer, datFileName);

				int32_t fileSize = FileUtils::LoadFile(fileNameBuffer, bufferPtr);
				dataBuffer = dataBuffer + fileSize;

				int32_t offset = LoadDAT(levelIdCpy, fileSize);
				dataBuffer = dataBuffer + offset;
			}

			fileNameBuffer[0] = 0;
			FileUtils::AppendRegPathToBuffer();

			int32_t variant;
			int32_t variantStep;
			const char* ngnBaseName;
			int32_t loadConfigCpy2 = g_levelLoadConfig;

			Logger::Log("Level load config -> %d\n", g_levelLoadConfig);

			if ((g_levelLoadConfig & 4) == 0 || (g_levelLoadConfig & 1) == 0)
			{
				g_levelDataBase = dataBuffer;

				if (((g_levelLoadConfig >> 8) & 3) == 1)
				{
					if (levelIdCpy >= 10)
					{
						strcpy(fileNameBuffer, "level");
						fileNameBuffer[5] = levelIdCpy / 10 + 48;
						fileNameBuffer[6] = levelIdCpy % 10 + 48;
						loadConfigCpy2 = g_levelLoadConfig;
					}
					else
					{
						strcpy(fileNameBuffer, "level0");
						fileNameBuffer[6] = levelIdCpy + 48;
					}

					strcpy(&fileNameBuffer[7], "\\");
					ngnBaseName = "level1";
				}
				else
				{
					if (((g_levelLoadConfig >> 8) & 3) != 2)
					{
						if (((g_levelLoadConfig >> 8) & 3) == 3)
							BuildLevelPath(levelIdCpy, fileNameBuffer, "level3");
						else
							BuildLevelPath(levelIdCpy, fileNameBuffer, "level");

						loadConfigCpy2 = g_levelLoadConfig;

					LBL_BUILD_TEXTURE_SUFFIX:

						if ((loadConfigCpy2 & 4) == 0 && levelIdCpy && levelId != 16)
							goto LBL_LOAD_NGN;

						variant = ((loadConfigCpy2 >> 6) & 3) - 1;

						const char* tSuffix;

						if (variant)
						{
							variantStep = variant - 1;

							if (variantStep)
							{
								if (variantStep != 1)
								{
								LBL_LOAD_NGN:

									strcat(fileNameBuffer, ".ngn");
									strcat(FileUtils::g_fileNameBuffer, fileNameBuffer);

									NGNLoader::SetNewImage(FileUtils::g_fileNameBuffer);
									NGNLoader::DetectBackdropTextures();

									loadConfigCpy2 = g_levelLoadConfig;
									goto LBL_POST_NGN_LOAD;
								}

								tSuffix = "t3";
							}
							else
							{
								tSuffix = "t2";
							}
						}
						else
						{
							tSuffix = "t1";
						}

						strcat(fileNameBuffer, tSuffix);
						goto LBL_LOAD_NGN;
					}

					if (levelIdCpy >= 10)
					{
						strcpy(fileNameBuffer, "level");
						fileNameBuffer[5] = levelIdCpy / 10 + 48;
						fileNameBuffer[6] = levelIdCpy % 10 + 48;
						loadConfigCpy2 = g_levelLoadConfig;
					}
					else
					{
						strcpy(fileNameBuffer, "level0");
						fileNameBuffer[6] = levelIdCpy + 48;
					}

					strcpy(&fileNameBuffer[7], "\\");
					ngnBaseName = "level2";
				}

				strcat(fileNameBuffer, ngnBaseName);
				goto LBL_BUILD_TEXTURE_SUFFIX;
			}

		LBL_POST_NGN_LOAD:

			if ((loadConfigCpy2 & 8) == 0)
			{
				if (! g_cachedAllBuffer)
				{
					Collision::BuildCollisionWorld(levelIdCpy, &dataBuffer, (loadConfigCpy2 >> 8) & 3);
					loadConfigCpy2 = (loadConfigCpy2 & ~0xFF) | (g_levelLoadConfig & 0xFF);
				}

				if ((loadConfigCpy2 & 8) == 0)
				{
					Collectables::BuildPickupTable();
					loadConfigCpy2 = (loadConfigCpy2 & ~0xFF) | (g_levelLoadConfig & 0xFF);
				}
			}

			if ((loadConfigCpy2 & 16) == 0)
			{
				uint8_t creatureIdList[128];
				Actor::GetCreatureList(creatureIdList);
				CharacterLoader::Start(&charLoaderFlag, &dataBuffer, creatureIdList);
			}

			g_levelLoadArena = dataBuffer;
			g_levelLoadConfig = 0;

			Logger::Log("InitLevelPlay : END.\n");
		}

		// STUB: TOY2 0x00434090
		int32_t Method2(int32_t a1, int16_t a2) { return 0; }

		// STUB: TOY2 0x0048B660
		void Method1(int32_t a1, int32_t a2, int32_t a3, int32_t a4) {}

		// STUB: TOY2 0x0049FAB0
		void Method3(int32_t a1, int32_t a2) {}
	}

	namespace Terrain
	{
		// STUB: TOY2 0x00489980
		int32_t LoadAll(const char* fileName, int32_t index, void** buffer) { return 0; }
	}
}
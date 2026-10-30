#include "CharacterLoader.h"
#include "Nullsub.h"
#include "FileUtils.h"
#include "Toy2/Toy2.h"

#include <string.h>
#include <stdlib.h>

namespace CharacterLoader
{
	// GLOBAL: TOY2 0x0053EEE0
	BoneTransform g_boneTransforms[300];

	// GLOBAL: TOY2 0x0054717C
	int16_t g_unk54717C;

	// GLOBAL: TOY2 0x00546D78
	int16_t g_unk546D78;

	// GLOBAL: TOY2 0x00547CD0
	int16_t g_unk547CD0;

	// GLOBAL: TOY2 0x0053E4C8
	int32_t g_alternateAllParse[128];

	// GLOBAL: TOY2 0x0053E8C8
	uint8_t* g_charFileDataCache[128];

	// the bone data that a character gets when it has no bone to attach to
	// GLOBAL: TOY2 0x004F6EA0
	int16_t g_unk4F6EA0[8] = { 0, -250, 0, 1, 512, 256, 512, 250 };
	STATIC_ASSERT(sizeof(g_unk4F6EA0) == 0x10);

	// GLOBAL: TOY2 0x0053E6C8
	void* g_attachBone[128];

	// GLOBAL: TOY2 0x0053EAC8
	CharacterBoundData g_boundingData[128];

	// GLOBAL: TOY2 0x0054DD88
	CharacterBoundData g_unk54DD88;

	// GLOBAL: TOY2 0x00547180
	CharacterBoundData g_unk547180;

	// GLOBAL: TOY2 0x0053EECC
	int32_t g_unk53EECC;

	// the lists of the marker entries of the .all files (types 256 and up), the entries are linked through the data
	// GLOBAL: TOY2 0x00546D7C
	void* g_unk546D7C[256];

	// GLOBAL: TOY2 0x00547188
	void* g_unk547188[512];

	// GLOBAL: TOY2 0x00547988
	void* g_unk547988[140];

	// GLOBAL: TOY2 0x00547BB8
	int16_t g_unk547BB8[140];

	// GLOBAL: TOY2 0x00547CD4
	uint8_t* g_unk547CD4[128];

	// GLOBAL: TOY2 0x0053E4C0
	int16_t g_unk53E4C0;

	// GLOBAL: TOY2 0x0053EED8
	int16_t g_unk53EED8;

	// GLOBAL: TOY2 0x0053EEC8
	uint8_t* g_unk53EEC8;

	// GLOBAL: TOY2 0x0053E4C4
	uint8_t* g_unk53E4C4;

	// GLOBAL: TOY2 0x00559DF0
	int32_t g_unk559DF0;

	// FUNCTION: TOY2 0x0043ABC0
	uint16_t* LoadFirstSection(int32_t a1, char a2)
	{
		uint8_t* file = (uint8_t*)a1;
		int16_t i = 0;
		uint16_t* firstSection = 0;

		if (*(int16_t*)(file + 2) > 0)
		{
			// the offsets of the sections are made into pointers
			do
			{
				uint32_t* entry = (uint32_t*)(file + 8) + i;

				if (*entry)
				{
					*entry = (uint32_t)file + *entry;

					if (! firstSection)
						firstSection = (uint16_t*)*entry;
				}

				++i;
			} while (i < *(int16_t*)(file + 2));

			if (firstSection && a2)
			{
				int32_t headerSize;

				if ((int16_t)firstSection[0] >= 0)
				{
					headerSize = 12;
					g_unk53E4C0 = 0;
					g_unk53EED8 = 12;
				}
				else
				{
					headerSize = -(int16_t)firstSection[0];
					g_unk53EED8 = (int16_t)headerSize;
					g_unk53E4C0 = 1;
				}

				uint8_t* boneList = (uint8_t*)firstSection + headerSize;

				g_unk53EEC8 = boneList;
				g_unk53E4C4 = (uint8_t*)firstSection + headerSize + 2 * firstSection[5];

				int16_t boneIdx = 0;

				if (firstSection[5])
				{
					int16_t trackCount = g_unk54717C;

					do
					{
						if (*(uint16_t*)(boneList + 2 * boneIdx) == 0xFFFE)
						{
							g_unk547BB8[trackCount] = *(int16_t*)(file + 4) + boneIdx;
							++trackCount;
							g_unk54717C = trackCount;
						}

						++boneIdx;
					} while (boneIdx < (int32_t)firstSection[5]);
				}
			}
		}

		return firstSection;
	}

	// FUNCTION: TOY2 0x0043D6B0
	int32_t AlternateAllParse(int32_t boneIdx, int32_t data)
	{
		uint8_t** buffer = (uint8_t**)data;
		int32_t boneCount = 0;

		int32_t* table = (int32_t*)(*buffer + 2 * *(int32_t*)*buffer);
		*buffer += 4;

		g_unk546D7C[31] = 0;
		g_unk546D7C[5] = 0;

		if (*table <= 0)
			return 0;

		BoneTransform* bone = &g_boneTransforms[boneIdx];
		CharacterAllEntry* entry = (CharacterAllEntry*)(table + 1);

		for (int32_t i = 0; i < *table; i++)
		{
			if (entry->type >= 256)
			{
				void* position = *buffer;

				if (*(int32_t*)*buffer == 0x12345678)
				{
					*(int32_t*)*buffer = (int32_t)g_unk546D7C[entry->type - 256];
					position = *buffer + 4;
				}

				g_unk546D7C[entry->type - 256] = position;
			}
			else
			{
				++boneCount;

				bone->translation.x = entry->x;
				bone->translation.y = entry->y;
				bone->translation.z = entry->z;
				bone->unkInt4 = entry->type;

				bone->unkInt9 = (int32_t)*buffer;
				bone->unkVec5.x = entry->unk32;
				bone->unkVec5.y = entry->unk42;
				bone->unkVec5.z = entry->unk3A;

				uint8_t* boneType = (uint8_t*)&bone->unkInt14_;

				*boneType = (uint8_t)entry->type;

				if (*boneType == 9)
				{
					memcpy(&g_unk547180, &entry->boundX, sizeof(CharacterBoundData));
					*boneType = 12;
				}

				if (entry->trackKind)
				{
					if (entry->trackKind == 1)
						*boneType += 4;
					else
						*boneType += 8;

					// walk the data of the bone to the end of its tracks
					uint8_t* trackData = (uint8_t*)bone->unkInt9;
					uint8_t trackFlags = trackData[3];

					while (trackFlags)
					{
						int32_t trackType = trackFlags & 0x7C;

						if (trackType == 0x34)
							trackData += 0x24;
						else if (trackType == 0x3C)
							trackData += 0x30;

						trackFlags = trackData[3];
					}

					bone->unkInt4 = (int32_t)(trackData + 4);
				}

				if ((entry->flags46 & 0x40) == 0x40)
					*boneType += 2;

				++bone;
			}

			*buffer += 2 * entry->dataSize;
			++entry;
		}

		return boneCount;
	}

	// FUNCTION: TOY2 0x0043D820
	int32_t LoadAll(const char* fileName, int32_t index, void** buffer)
	{
		char allFileName[100];

		strcpy(allFileName, fileName);
		strcat(allFileName, ".all");
		FileUtils::LoadFile(allFileName, *buffer);

		int32_t boneCount = 0;
		int32_t* table = (int32_t*)((uint8_t*)*buffer + 2 * *(int32_t*)*buffer);
		*buffer = (uint8_t*)*buffer + 4;

		g_unk546D7C[31] = 0;
		g_unk546D7C[5] = 0;

		if (*table <= 0)
			return 0;

		BoneTransform* bone = &g_boneTransforms[index];
		CharacterAllEntry* entry = (CharacterAllEntry*)(table + 1);

		for (int32_t i = 0; i < *table; i++)
		{
			if (entry->type >= 256)
			{
				// types above 255 are markers, the slot in the data is added to the list of that type
				if (*(int32_t*)*buffer == 0x12345678)
				{
					*(int32_t*)*buffer = (int32_t)g_unk546D7C[entry->type - 256];
					g_unk546D7C[entry->type - 256] = (uint8_t*)*buffer + 4;
				}
				else
				{
					g_unk546D7C[entry->type - 256] = *buffer;
				}
			}
			else
			{
				++boneCount;

				bone->translation.x = entry->x;
				bone->translation.y = entry->y;
				bone->translation.z = entry->z;
				bone->unkInt4 = entry->type;

				bone->unkInt9 = (int32_t)*buffer;
				bone->unkVec5.x = entry->unk32;
				bone->unkVec5.y = entry->unk42;
				bone->unkVec5.z = entry->unk3A;

				uint8_t* boneType = (uint8_t*)&bone->unkInt14_;

				*boneType = (uint8_t)entry->type;

				if (*boneType == 9)
				{
					memcpy(&g_unk547180, &entry->boundX, sizeof(CharacterBoundData));
					*boneType = 12;
				}

				if ((entry->flags46 & 0x40) == 0x40)
					*boneType += 2;

				++bone;
			}

			*buffer = (uint8_t*)*buffer + 2 * entry->dataSize;
			++entry;
		}

		return boneCount;
	}

	// $TODO: the quads of a bone are 48 bytes: a flag byte at +3, 4 vertices of 3 shorts at +4 with a stride of 8, and 4 dwords at +0 / +0x24 / +0x28 / +0x2C
	// that belong to the vertices. The original has the 4 rotations of the quad written out as straight line code, they are loops here.
	// FUNCTION: TOY2 0x0043C750
	void LoadSecondSection(int16_t a1)
	{
		int32_t i;
		int32_t j;
		int32_t r;
		int32_t c;
		int32_t d;
		uint8_t* extra = 0;
		uint8_t* extraBase = 0;
		uint8_t* boneExtra = 0;
		uint8_t* foundExtra = 0;

		if (g_unk547CD0 < 140)
			memset(&g_unk547988[g_unk547CD0], 0, 4 * (140 - g_unk547CD0));

		uint8_t* marker = (uint8_t*)g_unk546D7C[31];
		int32_t lastGroupIdx = g_unk547CD0;

		if (marker)
		{
			do
			{
				uint32_t value = *(uint16_t*)(marker + 2);

				*(int16_t*)(marker + 2) = (int16_t)(value % 100 + 100 * (a1 + value / 100) - 100);

				uint32_t groupOffset = (uint16_t)(value % 100 + 100 * (a1 + value / 100) - 100) % 100;
				int32_t groupIdx = g_unk547CD0 + groupOffset;

				if (groupIdx >= (int16_t)lastGroupIdx)
					lastGroupIdx = g_unk547CD0 + groupOffset + 1;

				if (g_unk547988[groupIdx])
				{
					// the original spins here forever
					while (1)
						;
				}

				g_unk547988[groupIdx] = marker;
				marker = *(uint8_t**)(marker - 4);
			} while (marker);
		}

		for (i = g_unk546D78; i < g_unk54717C; i++)
		{
			BoneTransform* bone = &g_boneTransforms[g_unk547BB8[i]];
			uint8_t* boneType = (uint8_t*)&bone->unkInt14_;

			if (*boneType == 9)
			{
				int16_t* header = (int16_t*)bone->unkInt4;

				*boneType = 10;
				extra = (uint8_t*)header + *header * 8 + 4;
				boneExtra = (uint8_t*)header + 4;
			}
			else
			{
				*boneType = 2;
			}

			int16_t previousFlag = bone->unkVec5.y;

			bone->unkVec5.x = previousFlag;
			bone->unkVec5.z += 257 * g_unk547CD0;

			if (previousFlag >= 2)
			{
				bone->unkVec5.y = 0;
				bone->unkInt14 = g_unk547CD0;
			}
			else
			{
				uint8_t* group = (uint8_t*)g_unk547988[(uint16_t)bone->unkVec5.z >> 8];

				if (group)
				{
					uint8_t* quad = (uint8_t*)bone->unkInt9;

					bone->unkVec5.y = 0;

					if (quad[3])
					{
						int32_t matchResult;

						do
						{
							int32_t count = *(uint16_t*)group;
							int16_t* groupVerts = (int16_t*)(group + 4);
							int16_t* quadVerts = (int16_t*)(quad + 4);

							matchResult = 0;
							j = 0;

							if (count > 0)
							{
								// the edge of the quad has to run the same way as the edge of the group
								do
								{
									for (r = 0; r < 4; r++)
									{
										int32_t matched = 1;

										for (c = 0; c < 3; c++)
										{
											if (abs(quadVerts[4 * r + c] - groupVerts[4 * j + c]) >= 2
												|| abs(quadVerts[4 * ((r + 1) % 4) + c] - groupVerts[4 * (j + 1) + c]) >= 2)
											{
												matched = 0;
												break;
											}
										}

										if (matched)
											break;
									}

									if (r < 4)
									{
										if (r > 0)
										{
											uint32_t old[12];
											uint32_t* rotated = (uint32_t*)quad;
											int32_t perVertex[4];

											perVertex[0] = 0;
											perVertex[1] = 9;
											perVertex[2] = 10;
											perVertex[3] = 11;

											memcpy(old, quad, 48);

											for (d = 1; d <= 8; d++)
												rotated[d] = old[1 + (d - 1 + 2 * r) % 8];

											for (d = 0; d < 4; d++)
												rotated[perVertex[d]] = old[perVertex[(d + r) % 4]];

											quad[3] = ((uint8_t*)old)[3];
											quad[0x2F] = ((uint8_t*)old)[0x2F];

											if (*boneType == 10)
											{
												uint16_t oldExtra[4];

												memcpy(oldExtra, extra, 8);

												for (d = 0; d < 4; d++)
													((uint16_t*)extra)[d] = oldExtra[(d + r) % 4];
											}
										}

										j = count;
										matchResult = count + 1;
									}

									++j;
									count = *(uint16_t*)group;
								} while (j < count);
							}

							if ((int16_t)matchResult == 0)
							{
								count = *(uint16_t*)group;
								j = 0;

								if (count > 0)
								{
									// the edge of the quad runs the other way
									do
									{
										for (r = 0; r < 4; r++)
										{
											int32_t matched = 1;

											for (c = 0; c < 3; c++)
											{
												if (abs(quadVerts[4 * r + c] - groupVerts[4 * (j + 1) + c]) >= 2
													|| abs(quadVerts[4 * ((r + 1) % 4) + c] - groupVerts[4 * j + c]) >= 2)
												{
													matched = 0;
													break;
												}
											}

											if (matched)
												break;
										}

										if (r < 4)
										{
											if (r > 0)
											{
												uint32_t old[12];
												uint32_t* rotated = (uint32_t*)quad;
												int32_t perVertex[4];

												perVertex[0] = 0;
												perVertex[1] = 9;
												perVertex[2] = 10;
												perVertex[3] = 11;

												memcpy(old, quad, 48);

												for (d = 1; d <= 8; d++)
													rotated[d] = old[1 + (d - 1 + 2 * r) % 8];

												for (d = 0; d < 4; d++)
													rotated[perVertex[d]] = old[perVertex[(d + r) % 4]];

												quad[3] = ((uint8_t*)old)[3];
												quad[0x2F] = ((uint8_t*)old)[0x2F];

												if (*boneType == 10)
												{
													uint16_t oldExtra[4];

													memcpy(oldExtra, extra, 8);

													for (d = 0; d < 4; d++)
														((uint16_t*)extra)[d] = oldExtra[(d + r) % 4];
												}
											}

											j = count;
											matchResult = -1 - count;
										}

										++j;
										count = *(uint16_t*)group;
									} while (j < count);
								}

								if ((int16_t)matchResult < 0)
									bone->unkVec5.y = 1;
							}

							uint8_t more = quad[0x33];

							quad += 0x30;
							extra += 8;

							if (! more)
								break;
						} while (1);
					}
				}
			}
		}

		for (i = g_unk546D78; i < g_unk54717C; i++)
		{
			BoneTransform* bone = &g_boneTransforms[g_unk547BB8[i]];
			uint8_t* boneType = (uint8_t*)&bone->unkInt14_;

			if (*boneType == 10)
				boneExtra = (uint8_t*)bone->unkInt4 + 4;

			if (bone->unkVec5.x < 2)
			{
				uint8_t* group = (uint8_t*)g_unk547988[(uint16_t)bone->unkVec5.z >> 8];
				int16_t* groupVerts = (int16_t*)(group + 4);
				int32_t flipped = bone->unkVec5.y;
				int32_t count = *(uint16_t*)group;

				if (count > 0)
				{
					// puts the quads in the order of the vertices of the group, the first vertex of a quad is the vertex of the group
					// (or the one after it when the quad runs the other way)
					j = 0;

					do
					{
						uint8_t* quads = (uint8_t*)bone->unkInt9;
						uint8_t* quad = quads + 48 * j;
						uint8_t* found = 0;

						if (*boneType == 10)
							extra = boneExtra + (*(int16_t*)(boneExtra - 4) + j) * 8;

						if (! quad[3])
						{
							// the original spins here forever
							while (1)
								;
						}

						do
						{
							if (found)
								break;

							if (abs(*(int16_t*)(quad + 4) - groupVerts[4 * (j + (flipped ? 1 : 0)) + 0]) < 2
								&& abs(*(int16_t*)(quad + 6) - groupVerts[4 * (j + (flipped ? 1 : 0)) + 1]) < 2
								&& abs(*(int16_t*)(quad + 8) - groupVerts[4 * (j + (flipped ? 1 : 0)) + 2]) < 2)
							{
								found = quad;
								foundExtra = extra;
							}

							uint8_t more = quad[0x33];

							quad += 0x30;
							extra += 8;

							if (! more)
								break;
						} while (1);

						if (! found)
						{
							// the original spins here forever
							while (1)
								;
						}

						if (boneExtra)
							extra = boneExtra + (*(int16_t*)(boneExtra - 4) + j) * 8;

						uint32_t swap[12];

						memcpy(swap, quads + 48 * j, 48);
						memcpy(quads + 48 * j, found, 48);
						memcpy(found, swap, 48);

						if (*boneType == 10)
						{
							uint32_t swapExtra[2];

							memcpy(swapExtra, extra, 8);
							memcpy(extra, foundExtra, 8);
							memcpy(foundExtra, swapExtra, 8);
						}

						++j;
						count = *(uint16_t*)group;
					} while (j < count);
				}
			}
		}

		g_unk546D78 = g_unk54717C;
		g_unk547CD0 = (int16_t)lastGroupIdx;
	}

	// FUNCTION: TOY2 0x0043ACA0
	void Load(const char* fileName, int32_t* a2, uint8_t** levelData, int16_t creatureId, uint32_t* a5, CharacterBoundData* boundData)
	{
		char fullFilePath[256];

		if (! g_unk547CD4[creatureId])
		{
			uint8_t* fileContents = g_charFileDataCache[creatureId];

			if (! fileContents)
			{
				strcpy(fullFilePath, fileName);
				strcat(fullFilePath, ".anm");

				fileContents = *levelData;
				*levelData += 4 * ((FileUtils::LoadFile(fullFilePath, fileContents) + 3) / 4);
			}

			g_unk547CD4[creatureId] = fileContents;
			*(int16_t*)fileContents = 1;
			*(int16_t*)(fileContents + 4) = (int16_t)*a2;

			LoadFirstSection((int32_t)fileContents, 1);

			if (g_alternateAllParse[creatureId])
			{
				uint8_t* alternateData = (uint8_t*)g_alternateAllParse[creatureId];

				*a2 += AlternateAllParse(*a2, (int32_t)&alternateData);
				*levelData = alternateData;
			}
			else
			{
				strcpy(fullFilePath, fileName);
				*a2 += LoadAll(fullFilePath, *a2, (void**)levelData);
			}

			// a bone that is only there to attach things to is not kept, the characters remember its data instead
			if (*(uint8_t*)&g_boneTransforms[*a2 - 1].unkInt14_ == 12)
			{
				--*a2;
				*a5 = g_boneTransforms[*a2].unkInt9;
				*boundData = g_unk547180;
			}

			*(int16_t*)(fileContents + 6) = (int16_t)*a2;

			void** marker = (void**)g_unk546D7C[5];

			while (marker)
			{
				int32_t slot = *(int32_t*)marker % 4;

				g_unk547188[4 * creatureId + slot] = marker + 1;
				marker = (void**)marker[-1];
			}

			LoadSecondSection(*(int16_t*)(fileContents + 4));
		}
	}

	// FUNCTION: TOY2 0x0043AED0
	void LoadBuzzLight(const char* charName, int32_t* boneCursor, void** buffer, int16_t creatureId, int32_t* attachBone, CharacterBoundData* boundData)
	{
		char fileName[256];

		if (! g_unk547CD4[creatureId])
		{
			// $TODO: the original never sets this, it uses whatever is on the stack
			uint8_t* charInfo;

#ifdef APPLY_FIXES
			static uint8_t s_charInfo[16];
			charInfo = s_charInfo;
#endif

			g_unk547CD4[creatureId] = charInfo;

			*(int16_t*)charInfo = 1;
			*(int16_t*)(charInfo + 4) = (int16_t)*boneCursor;

			if (g_alternateAllParse[creatureId])
			{
				uint8_t* alternateData = (uint8_t*)g_alternateAllParse[creatureId];

				*boneCursor += AlternateAllParse(*boneCursor, (int32_t)&alternateData);
			}
			else
			{
				strcpy(fileName, charName);
				*boneCursor += LoadAll(fileName, *boneCursor, buffer);
			}

			if (*(uint8_t*)&g_boneTransforms[*boneCursor - 1].unkInt14_ == 12)
			{
				--*boneCursor;
				*attachBone = g_boneTransforms[*boneCursor].unkInt9;
				*boundData = g_unk547180;
			}

			*(int16_t*)(charInfo + 6) = (int16_t)*boneCursor;

			void** marker = (void**)g_unk546D7C[5];

			while (marker)
			{
				int32_t slot = *(int32_t*)marker % 4;

				g_unk547188[4 * creatureId + slot] = marker + 1;
				marker = (void**)marker[-1];
			}

			// the light of Buzz does not move on its own
			for (int32_t boneIdx = *(int16_t*)(charInfo + 4); boneIdx < *(int16_t*)(charInfo + 6); boneIdx++)
			{
				g_boneTransforms[boneIdx].translation.x = 0;
				g_boneTransforms[boneIdx].translation.y = 0;
				g_boneTransforms[boneIdx].translation.z = 0;
			}
		}
	}

	// FUNCTION: TOY2 0x0043B0C0
	void LoadCharacterData(int32_t* result, uint8_t** levelData, uint8_t* creatureList)
	{
		if (*creatureList != 255)
		{
			uint8_t* creatureListPtr = creatureList;
			char creatureCode;

			do
			{
				int32_t creatureId = *creatureListPtr;

				switch (creatureId)
				{
					case 0:
						Load("chars\\buzz", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 1:
						Load("chars\\woody", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 2:
						Load("chars\\buzzint", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 3:
						Load("chars\\zurg1", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 4:
						Load("chars\\zurg3", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 5:
						Load("chars\\tinman", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 6:
						Load("chars\\sheep", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 7:
						Load("chars\\bopeep", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 8:
						Load("chars\\rc", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 9:
						Load("chars\\mrpot", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 10:
						Load("chars\\hamm", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 11:
						Load("chars\\cotbit", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 12:
						Load("chars2\\lawn", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 13:
						Load("chars\\army", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 14:
						Load("chars2\\zgcar", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 15:
						Load("chars2\\zkite", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 16:
						Load("chars2\\slime", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 17:
						Load("chars2\\paint", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 18:
						Load("chars2\\ftyke", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 19:
						Load("chars2\\ltyke", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 20:
						Load("chars2\\zpod", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 21:
						Load("chars2\\shiny", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 22:
						Load("chars2\\drill", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 23:
						Load("chars2\\mouse", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 24:
						Load("chars3\\bplane", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 25:
						Load("chars3\\box", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 26:
						Load("chars3\\dino", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 27:
						Load("chars2\\zboat", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 28:
						Load("chars3\\chick", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 29:
						Load("chars3\\hoola", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 30:
						Load("chars3\\lock", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 31:
						Load("chars3\\gunsp", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 32:
						Load("chars3\\clown", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 33:
						Load("chars3\\rattle", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 34:
						Load("chars3\\mum", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 35:
						Load("chars3\\roost", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 36:
						Load("chars4\\mother", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 37:
						Load("chars4\\fsauce", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 38:
						Load("chars4\\martian", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 39:
						Load("chars4\\jessie", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 40:
						Load("chars6\\rabid", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 41:
						Load("chars3\\buzzard", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 42:
						Load("chars4\\bullseye", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 43:
						Load("chars4\\ducks", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 44:
						Load("chars4\\slinky", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 45:
						Load("chars4\\gunsl", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 46:
						Load("chars4\\fatbloke", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 47:
						Load("chars5\\bbuggy", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 48:
						Load("chars5\\bi", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 49:
						Load("chars4\\rocky", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 50:
						Load("chars4\\george", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 51:
						Load("chars4\\mildred", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 52:
						Load("chars4\\karen", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 53:
						Load("chars4\\pilot", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 54:
						Load("chars5\\zpod", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 55:
						Load("chars5\\bub", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 56:
						LoadBuzzLight("chars5\\buzl", result, (void**)levelData, creatureId, (int32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 57:
						Load("chars5\\zemp", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 58:
						Load("chars5\\smith", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 59:
						Load("chars5\\luggage", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 60:
						Load("chars6\\lugman", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 61:
						Load("chars6\\prosp", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					case 62:
						Load("chars6\\rex", result, levelData, creatureId, (uint32_t*)&g_attachBone[creatureId], &g_boundingData[creatureId]);
						break;

					default:
						break;
				}

				creatureCode = *++creatureListPtr;
			} while (creatureCode != -1);
		}

		g_unk54DD88 = g_boundingData[0];

		for (int32_t i = 0; i < 64; i++)
		{
			Toy2::Actor::Toy2Actor* actor = &Toy2::g_levelActors[i];

			if (actor->creatureId > 0)
			{
				actor->unkVar34 = (int32_t)g_attachBone[actor->creatureId];
				actor->boundingOffset.x = g_boundingData[actor->creatureId].boundingOffset.x;
				actor->boundingOffset.y = g_boundingData[actor->creatureId].boundingOffset.y;
				actor->boundingOffset.z = g_boundingData[actor->creatureId].boundingOffset.z;
				actor->boundingSphereRadius = g_boundingData[actor->creatureId].boundingSphereRadius;
			}
		}

		if (*result > 300)
			g_unk559DF0 = 1;
	}

	// FUNCTION: TOY2 0x0043C700
	int32_t InitGlobals()
	{
		g_unk54717C = 0;
		g_unk546D78 = 0;
		g_unk547CD0 = 0;

		return 0;
	}

	// FUNCTION: TOY2 0x0043B9B0
	void Start(int32_t* value, uint8_t** buffer, uint8_t* creatureList)
	{
		g_unk53EECC = -1;

		Nullsub4();

		InitGlobals();

		int32_t i;

		memset(g_boneTransforms, 0, sizeof(g_boneTransforms));

		for (i = 0; i < 128; i++)
			g_attachBone[i] = &g_unk4F6EA0;

		memset(g_unk547188, 0, sizeof(g_unk547188));
		memset(g_unk547CD4, 0, sizeof(g_unk547CD4));

		for (i = 0; i < 128; i++)
		{
			g_boundingData[i].boundingOffset.z = 0;
			g_boundingData[i].boundingOffset.x = 0;
			g_boundingData[i].boundingSphereRadius = 500;
			g_boundingData[i].boundingOffset.y = 500;
		}

		LoadCharacterData(value, buffer, creatureList);
	}
}

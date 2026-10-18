#include "Toy2/Collectables.h"
#include "Toy2/Levels.h"
#include "Toy2/Camera.h"
#include "Toy2/Toy2.h"
#include "Toy2/Dialogue.h"
#include "Toy2/Gadget.h"
#include "Toy2/Buzz.h"
#include "Toy2/Particles.h"
#include "Nu3D/Particles.h"
#include "Renderer/Renderer.h"
#include "AudioManager/AudioManager.h"
#include "InputManager.h"
#include "Random.h"
#include "Nu3D/Link.h"
#include "Nu3D/Collision.h"
#include "SaveManager.h"

#include <string.h>

namespace Toy2
{
	namespace Collectables
	{
		// GLOBAL: TOY2 0x0050A150
		TokenTableEntry g_tokenTable[10];

		// GLOBAL: TOY2 0x00830CCC
		PickupSlot g_pickupSlots[5];

		// the script Token runs, it puts the camera on the token and waits
		// GLOBAL: TOY2 0x004DF69C
		int32_t g_tokenScript[12] = { 1, -1, 0, 4, 2, 3, -1, 10, 7, -1, 8, -1 };

		// the directions the pieces of a collected token fly in
		// GLOBAL: TOY2 0x005039C4
		int16_t g_gameStruct5[48] = {
			0,	  4096, 0, 0,	  -4096, 0, 4096, 0,	 0, -4096, 0,	  0,	2896, 2896, 0,	   2896, -2896, 0,	  -2896, 2896,	0,	   -2896, -2896, 0,
			2896, 2896, 0, -2896, 2896,	 0, 2896, -2896, 0, -2896, -2896, 2896, 0,	  2896, -2896, 0,	 2896,	2896, 0,	 -2896, -2896, 0,	  -2896
		};

		// the gravity of Buzz while the level ends
		// GLOBAL: TOY2 0x00830D28
		int32_t g_unk830D28;

		// the pickup that comes back after a gadget pickup
		// GLOBAL: TOY2 0x00830D5C
		Levels::Type63* g_pendingPickup;

		// GLOBAL: TOY2 0x00830E34
		int32_t g_pendingPickupY;

		// GLOBAL: TOY2 0x00830CAC
		int32_t g_unk830CAC;

		// GLOBAL: TOY2 0x005546B8
		PickupTable g_pickupTable;

		// GLOBAL: TOY2 0x00556FAC
		Levels::RecordData* g_unk556FAC;

		// FUNCTION: TOY2 0x00447DB0
		void BuildPickupTable()
		{
			int32_t levelIdx = g_levelFileIndex;
			int32_t firstObject = 48;

			switch (g_levelFileIndex)
			{
				case 4:
					firstObject = 96;
					break;

				case 5:
					firstObject = 80;
					break;

				case 10:
					firstObject = 96;
					break;

				case 11:
					firstObject = 96;
					break;

				case 14:
					firstObject = 96;
					break;

				case 16:
					firstObject = 400;
					break;
			}

			Levels::RecordData* lightRecord = Levels::g_recordData[59];
			Levels::RecordData* pickupRecord = Levels::g_recordData[63];

			g_unk556FAC = Levels::g_recordData[63];

			Levels::Type63* table = g_pickupTable.data;

			// the light sprites sit on the ground
			if (lightRecord)
			{
				int32_t i = 0;

				if (Levels::g_recordData[59]->recordCount)
				{
					do
					{
						WorldPos position;

						position.x = 32 * lightRecord->data[i].x;
						position.z = 32 * lightRecord->data[i].z;
						position.y = 32 * (lightRecord->data[i].y - 10);

						Levels::g_recordData[59]->data[i].y = Nu3D::Collision::GetGroundHeightEx(&position, 0, 0) >> 5;
						lightRecord = Levels::g_recordData[59];

						++i;
					} while (i < Levels::g_recordData[59]->recordCount);

					levelIdx = g_levelFileIndex;
					pickupRecord = Levels::g_recordData[63];
				}
			}

			uint16_t recordCount = 0;

			if (pickupRecord)
			{
				int32_t i = 0;
				Levels::Type63* pickup = (Levels::Type63*)pickupRecord->data;

				if (pickupRecord->recordCount)
				{
					do
					{
						WorldPos position;

						position.x = 32 * pickup->pos.x;
						position.z = 32 * pickup->pos.z;
						position.y = 32 * (pickup->pos.y - 10);

						int32_t floorPos = Nu3D::Collision::GetGroundHeightEx(&position, 0, 0) >> 5;

						pickup->props.floorPos = floorPos;

						if (floorPos == 0x7FFF)
							pickup->props.floorPos = 32766;

						if (pickup->props.floorPos - pickup->pos.y > 6144)
							pickup->props.floorPos = 32767;

						Levels::Type63* current = pickup;

						pickup->props.flags = 17;
						Levels::Type63* entry = table;

						++pickup;
						++table;

						*entry = *current;
						++i;
					} while (i < Levels::g_recordData[63]->recordCount);

					levelIdx = g_levelFileIndex;
				}

				g_pickupTable.recordType = g_unk556FAC->recordType;
				recordCount = g_unk556FAC->recordCount;
			}
			else
			{
				g_pickupTable.recordType = 0;
			}

			g_pickupTable.recordCount = recordCount;
			Levels::g_recordData[63] = (Levels::RecordData*)&g_pickupTable;

			if (levelIdx == 5)
			{
				Vector3I position;

				Levels::g_objectListBase->entries[89]->z_or_coord = -18000;
				Nu3D::Link::GetCurrentPosFixed(89, &position);
				Nu3D::Link::SetPositionRawAndCommit(89, position.x >> 5, position.y >> 5, -18000);
			}

			// the objects of the level are added as pickups after the ones of the record
			Levels::ObjectList* objectList = Levels::g_objectListBase;

			if (objectList->count > 47)
			{
				int32_t objectIdx = firstObject;

				if (firstObject <= objectList->count)
				{
					do
					{
						// $TODO: the entries are read as a position and a type, which is not what Levels::Object has
						int32_t* object = (int32_t*)objectList->entries[objectIdx];

						if (object)
						{
							WorldPos position;

							table->pos.x = object[0];
							table->pos.y = object[1];
							table->pos.z = object[2];
							table->props.linkId = objectIdx;
							table->props.flags = (uint8_t)(*(int16_t*)(object + 3) >> 3);

							position.x = 32 * table->pos.x;
							position.z = 32 * table->pos.z;
							position.y = 32 * (table->pos.y - 10);

							int32_t floorPos = Nu3D::Collision::GetGroundHeightEx(&position, 0, 0) >> 5;

							table->props.floorPos = floorPos;

							if (floorPos == 0x7FFF)
								table->props.floorPos = 32766;

							if (table->props.floorPos - table->pos.y > 6144)
								table->props.floorPos = 0x7FFF;

							objectList = Levels::g_objectListBase;
							++table;
							++g_pickupTable.recordCount;
						}

						++objectIdx;
					} while (objectIdx <= objectList->count);
				}
			}
		}

		// FUNCTION: TOY2 0x004025C0
		int32_t* LoadTokenTable(int32_t* a1)
		{
			memset(g_tokenTable, 0, sizeof(g_tokenTable));

			int32_t* result = a1;
			TokenTableEntry* entry = g_tokenTable;

			do
			{
				int32_t firstValue = *result++;

				if (firstValue < 0)
					break;

				entry->linkId = firstValue;
				entry->recordType = *result;
				result++;
				entry->text = (const char*)*result;
				result++;
				entry->yaw = *result;
				result++;

				entry++;
			} while (entry < &g_tokenTable[10]);

			return result;
		}

		// FUNCTION: TOY2 0x00402610
		void Token(int32_t linkId)
		{
			if (! Renderer::g_enableCinematicBars)
			{
				if (Camera::g_cameraState)
				{
					if (Camera::g_unk50A538 != (Nu3D::Particles::ParticleInstance*)-1)
					{
						Camera::g_unk50A538->lifetime = 1;
						Camera::g_unk50A538 = (Nu3D::Particles::ParticleInstance*)-1;
					}

					if (Camera::g_unk50A4DC != (Nu3D::Particles::ParticleInstance*)-1)
					{
						Camera::g_unk50A4DC->lifetime = 1;
						Camera::g_unk50A4DC = (Nu3D::Particles::ParticleInstance*)-1;
					}

					g_buzzActor.actorFlags |= Actor::ACTOR_CUTSCENE_MOVE;
					Camera::g_gameplayCameraState.orbitYaw = g_buzzActor.posAngles.angles.yaw;
					g_buzzActor.facingAngle = g_buzzActor.posAngles.angles.yaw;
					Camera::g_gameplayCameraState.distance = 1200;
					Camera::g_gameplayCameraState.pos.y = g_buzzActor.posAngles.pos.y - 12288;
					Camera::g_gameplayCameraState.angles.pitch = 0;
					Camera::g_gameplayCameraState.targetPos.x = Camera::g_gameplayCameraState.pos.x;
					Camera::g_gameplayCameraState.unkShort6 = 0;
					Camera::g_cameraState = 0;
					Nu3D::Link::SetScaleFromFixedOffsets(45, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(46, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(47, 0, 0, 0);
				}

				// the entry of the token in the table
				int32_t index = 0;
				TokenTableEntry* entry = g_tokenTable;

				do
				{
					if (entry->linkId == linkId)
						break;

					entry++;
					++index;
				} while (entry < &g_tokenTable[10]);

				Camera::g_inCutscene |= 1;
				Renderer::g_enableCinematicBars = 1;

				Camera::g_unk50A4D0 = 0;
				Camera::g_unk50A294 = g_tokenScript;
				Camera::g_unk50A134 = 0;
				char* text = (char*)g_tokenTable[index].text;
				int32_t recordIdx = g_tokenTable[index].recordType;

				Camera::g_unk50A53C = 0;
				Camera::g_unk50A144 = 0;
				Camera::g_unk50A0CC = 0;
				Camera::g_unk50A1F0 = 0;
				Camera::g_unk50A514 = 0;
				Camera::g_unk50A4BC = 0;
				Camera::g_unk50A0C8 = recordIdx;

				if (text)
				{
					Dialogue::WrapSubtitleText(text);

					Dialogue::g_unk50A518 = 1;

					for (int32_t i = 0; i < 36; i++)
						((int32_t*)Dialogue::g_subtitleBuffer)[i] = 0x200020;

					Dialogue::g_subtitleText = Dialogue::g_subtitleWrapBuffer;
					Dialogue::g_subtitleColumn = 0;
					Dialogue::g_subtitleRowStart = 0;
					Dialogue::g_subtitleScroll = -72;
					Dialogue::g_unk50A110 = 0x808080;
					Dialogue::g_subtitleTypeTimer = 2;
					Dialogue::g_subtitleFontPage = 0;
					Dialogue::g_subtitleBoxAnim = 0;

					Dialogue::g_subtitleMode = 0;
					AudioManager::PlaySoundEffect(29, 0);
				}
				else
				{
					Dialogue::g_unk50A518 = 0;
				}

				g_buzzActor.posAngles.angles.yaw = (int16_t)g_tokenTable[index].yaw;
				g_buzzActor.facingAngle = g_buzzActor.posAngles.angles.yaw;
				Camera::g_gameplayCameraState.orbitYaw = g_buzzActor.posAngles.angles.yaw;
			}
		}

		// FUNCTION: TOY2 0x004A0C80
		void Init(int16_t* linkTable, int32_t linkId)
		{
			int16_t* link = linkTable;
			int32_t objectIdx = 0;

			uint8_t tokenBits = SaveManager::g_save0Data.tokens[g_levelFileIndex];
			int32_t remainingBits = tokenBits;

			g_unk830CC4 = 0;

			if (linkTable)
			{
				int32_t currentLinkId = linkId;

				g_unk830CAC = 0;

				PickupSlot* slot = g_pickupSlots;

				while (1)
				{
					// a token that was collected already shows the collected shape
					if (tokenBits & 1)
						Nu3D::Link::CopyShapeId(*link, currentLinkId);

					slot->linkId = *link;
					slot->state = 0;

					int16_t* nextLink = link + 1;

					Nu3D::Link::SetScaleFromFixedOffsets(slot->linkId, 0, 0, 0);

					Levels::RecordData* record = Levels::g_recordData[63];
					Levels::Type63* object = (Levels::Type63*)Levels::g_recordData[63]->data;

					if (Levels::g_recordData[63]->recordCount)
					{
						while (object->props.linkId != slot->linkId)
						{
							++object;
							++objectIdx;

							if (objectIdx >= Levels::g_recordData[63]->recordCount)
								goto LABEL_14;
						}

						object->pos.y = 0x80000000;
						record = Levels::g_recordData[63];
						slot->yPosPtr = &object->pos.y;

					LABEL_14:
						currentLinkId = linkId;
					}

					remainingBits >>= 1;

					int32_t searchIdx = 0;

					if (record->recordCount)
					{
						while (object->props.linkId != currentLinkId)
						{
							++object;
							++searchIdx;

							if (searchIdx >= record->recordCount)
								goto LABEL_20;
						}

						object->pos.y = 0x80000000;
					}

				LABEL_20:
					Nu3D::Link::SetScaleFromFixedOffsets(currentLinkId, 0, 0, 0);

					++slot;
					++currentLinkId;
					linkId = currentLinkId;

					if (slot >= &g_pickupSlots[5])
						break;

					tokenBits = remainingBits;
					link = nextLink;
					objectIdx = 0;
				}
			}
			else
			{
				g_unk830CAC = 1;

				PickupSlot* slot = g_pickupSlots;

				do
					(slot++)->state = 0;
				while (slot < &g_pickupSlots[5]);
			}
		}

		// FUNCTION: TOY2 0x004A0DB0
		void Activate(int32_t slotIdx, int32_t skipIntro)
		{
			if (! g_pickupSlots[slotIdx].state)
			{
				g_pickupSlots[slotIdx].state = 1;

				int32_t* yPosPtr = g_pickupSlots[slotIdx].yPosPtr;
				Vector3I pos;

				Nu3D::Link::GetCurrentPosFixed(g_pickupSlots[slotIdx].linkId, &pos);
				*yPosPtr = pos.y >> 5;

				if (skipIntro)
				{
					Nu3D::Link::SetScaleFromFixedOffsets(g_pickupSlots[slotIdx].linkId, 4096, 4096, 4096);
				}
				else
				{
					g_pickupSlots[slotIdx].collectTimer = 132;
					Camera::BeginScriptedCutsceneAtPoint(&pos, 180, 16);

					if (Camera::g_cameraState)
						Camera::g_cameraState = 5;
				}
			}
		}

		// FUNCTION: TOY2 0x004A0E60
		void Deactivate(int32_t slotIdx)
		{
			g_pickupSlots[slotIdx].state = 0;
			*g_pickupSlots[slotIdx].yPosPtr = 0x80000000;
			Nu3D::Link::SetScaleFromFixedOffsets(g_pickupSlots[slotIdx].linkId, 0, 0, 0);
		}

		// FUNCTION: TOY2 0x004A50D0
		void CosmicShield(Toy2::Levels::Type63* object)
		{
			if (g_cosmicShieldPickup)
			{
				g_cosmicShieldPickup->pos.y = g_cosmicShieldPickupYPos;

				Nu3D::Link::SetScaleFromFixedOffsets(g_cosmicShieldPickup->props.linkId, 0x2000, 0x2000, 0x2000);
				Nu3D::Link::SetPositionRawAndCommit(
					g_cosmicShieldPickup->props.linkId, g_cosmicShieldPickup->pos.x, g_cosmicShieldPickup->pos.y, g_cosmicShieldPickup->pos.z);

				g_cosmicShieldPickup = 0;
			}

			g_cosmicShieldPickup = object;
			g_cosmicShieldPickupYPos = object->pos.y;

			g_buzzActor.inhibitMovementTimer = 900;

			Nu3D::Link::SetScaleFromFixedOffsets(object->props.linkId, 12288, 12288, 12288);
		}

		// FUNCTION: TOY2 0x0044E520
		int32_t Classify(int32_t a1)
		{
			int32_t result;

			switch (Levels::g_objectListBase->entries[a1]->desc[2].id)
			{
				case 6:
					result = 4;
					break;
				case 0x12:
					result = 3;
					break;
				case 0x13:
					result = 10;
					break;
				case 0x14:
					result = 8;
					break;
				case 0x1E:
					result = 7;
					break;
				case 0x1F:
				case 0x32:
				case 0x50:
					result = 9;
					break;
				case 0x20:
					result = 0;
					break;
				case 0x24:
				case 0x48:
					result = 2;
					break;
				case 0x27:
					result = 5;
					break;
				case 0x3C:
					result = 1;
					break;
				case 0x64:
					result = 6;
					break;
				default:
					result = -1;
					break;
			}

			return result;
		}

		// FUNCTION: TOY2 0x004A0F80
		void Interactions()
		{
			Vector3I spawnPos;
			Vector3I collectPos;

			// the sparkles of the tokens that are not collected yet
			if (g_tickState[9])
			{
				int32_t slotIdx = 0;
				PickupSlot* slot = g_pickupSlots;

				do
				{
					if (slot->state == 1 && ((1 << slotIdx) & SaveManager::g_save0Data.tokens[g_levelFileIndex]) == 0 && ShowTokenSparkle())
					{
						Nu3D::Link::GetCurrentPosFixed(slot->linkId, &spawnPos);

						int32_t distanceYZ =
							((Camera::g_activeCameraTransform.pos.y - spawnPos.y) >> 8) * ((Camera::g_activeCameraTransform.pos.y - spawnPos.y) >> 8)
							+ ((Camera::g_activeCameraTransform.pos.z - spawnPos.z) >> 8) * ((Camera::g_activeCameraTransform.pos.z - spawnPos.z) >> 8);
						int32_t distanceX =
							((Camera::g_activeCameraTransform.pos.x - spawnPos.x) >> 8) * ((Camera::g_activeCameraTransform.pos.x - spawnPos.x) >> 8);

						if (distanceX + distanceYZ < 589824 && distanceX + distanceYZ != -1)
						{
							Nu3D::Particles::ParticleInstance* sparkle = Nu3D::Particles::SpawnFromPreset(spawnPos.x, spawnPos.y, spawnPos.z, 41, 9);
							sparkle->rotSpeed = *g_randDatBufferPtr++ - 128;
							sparkle->lifetime = 2 * (*g_randDatBufferPtr++ & 0xF) + 24;
						}
					}

					++slot;
					++slotIdx;
				} while (slot < &g_pickupSlots[5]);
			}

			// the pickup of the last gadget comes back
			if (g_unk830C90 > 0)
			{
				g_unk830C90 -= Renderer::g_frameDelta;

				if (g_unk830C90 <= 0)
				{
					Levels::Type63* pickup = g_pendingPickup;
					g_pendingPickup->pos.y = g_pendingPickupY;
					g_unk830C90 = 0;

					Nu3D::Link::SetScaleFromFixedOffsets(pickup->props.linkId, 4096, 4096, 4096);
					int32_t x = 32 * pickup->pos.x;
					spawnPos.x = x;
					int32_t y = 32 * pickup->pos.y;
					spawnPos.y = y;
					int32_t z = 32 * pickup->pos.z;

					int32_t distanceYZ = ((Camera::g_activeCameraTransform.pos.y - y) >> 8) * ((Camera::g_activeCameraTransform.pos.y - y) >> 8)
						+ ((Camera::g_activeCameraTransform.pos.z - z) >> 8) * ((Camera::g_activeCameraTransform.pos.z - z) >> 8);
					int32_t distanceX = ((Camera::g_activeCameraTransform.pos.x - x) >> 8) * ((Camera::g_activeCameraTransform.pos.x - x) >> 8);
					spawnPos.z = z;

					if (distanceX + distanceYZ < 589824 && distanceX + distanceYZ != -1)
						Particles::SpawnCollectSparkle(x, y, z, 50);
				}
			}

			// Buzz picks up the objects of the level
			int32_t soundId = 0;
			Levels::Type63* data = (Levels::Type63*)Levels::g_recordData[63]->data;
			int32_t buzzX = g_buzzActor.posAngles.pos.x >> 5;
			int32_t buzzY = (g_buzzActor.posAngles.pos.y >> 5) - 230;
			int32_t buzzZ = g_buzzActor.posAngles.pos.z >> 5;

			if (Levels::g_recordData[63]->recordCount)
			{
				int32_t remaining = Levels::g_recordData[63]->recordCount;

				while (1)
				{
					if (data->pos.y != 0x80000000)
					{
						int8_t flags = data->props.flags;

						if (flags < 0 && (flags & 0x7F) != 0)
						{
							int32_t radius = (flags & 0x7F) + 14;

							if (((buzzX - data->pos.x) >> 3) * ((buzzX - data->pos.x) >> 3) + ((buzzY - data->pos.y) >> 3) * ((buzzY - data->pos.y) >> 3)
									+ ((buzzZ - data->pos.z) >> 3) * ((buzzZ - data->pos.z) >> 3)
								< radius * radius)
							{
								collectPos.x = 32 * data->pos.x;
								collectPos.y = 32 * data->pos.y;
								collectPos.z = 32 * data->pos.z;
								soundId = -1;

								Particles::SpawnCollectSparkle(32 * data->pos.x, 32 * data->pos.y, 32 * data->pos.z, radius);

								int32_t hide;
								int32_t category;

								if (data->props.linkId < 48u)
								{
									hide = 0;
									category = 16;
								}
								else
								{
									hide = 1;
									category = Classify(data->props.linkId);
								}

								switch (category)
								{
									case -1:
										Gadget::g_unk830D48 = -Gadget::g_unk830D48;
										goto LABEL_63;

									// a heart
									case 0:
										g_hudWidgetTimers[1] = 180;
										g_buzzActor.health += 4;

										if (g_buzzActor.health > 14)
											g_buzzActor.health = 14;

										goto LABEL_63;

									// the cosmic shield
									case 1:
										CosmicShield(data);
										soundId = -1;
										hide = 0;
										goto LABEL_64;

									// a token
									case 2: {
										int32_t slotIdx = 0;
										PickupSlot* slot = g_pickupSlots;

										while (slot->linkId != data->props.linkId)
										{
											++slot;
											++slotIdx;

											if (slot >= &g_pickupSlots[5])
												goto LABEL_63;
										}

										g_pickupSlots[slotIdx].state = 2;
										g_pickupSlots[slotIdx].collectTimer = 0;
										g_unk830CC4 = 1;

										SaveManager::g_save0Data.tokens[g_levelFileIndex] |= 1 << slotIdx;
										goto LABEL_63;
									}

									// an extra life
									case 3:
										g_hudWidgetTimers[0] = 180;

										if (g_buzzActor.lives < 9)
											++g_buzzActor.lives;

										goto LABEL_63;

									// a pizza planet token of the level
									case 4:
										Token(data->props.linkId);
										soundId = -1;
										hide = 2;
										goto LABEL_64;

									// the rocket boots
									case 5:
										Buzz::ActivateRocketBoots(data);
										goto LABEL_63;

									// the grapple
									case 6:
										soundId = -1;
										g_discLauncherTimer = 0;
										g_grappleTimer += 5;

										if (g_grappleTimer > 10)
											g_grappleTimer = 10;

										if (g_unk830C90 > 0)
										{
											Levels::Type63* pickup = g_pendingPickup;
											g_pendingPickup->pos.y = g_pendingPickupY;

											g_unk830C90 = 0;
											Nu3D::Link::SetScaleFromFixedOffsets(pickup->props.linkId, 4096, 4096, 4096);
											int32_t pickupX = pickup->pos.x;
											int32_t pickupY = pickup->pos.y;
											int32_t pickupZ = 32 * pickup->pos.z;
											int32_t pickupY32 = 32 * pickupY;
											int32_t pickupX32 = 32 * pickupX;

											int32_t distanceYZ = ((Camera::g_activeCameraTransform.pos.y - pickupY32) >> 8)
													* ((Camera::g_activeCameraTransform.pos.y - pickupY32) >> 8)
												+ ((Camera::g_activeCameraTransform.pos.z - pickupZ) >> 8)
													* ((Camera::g_activeCameraTransform.pos.z - pickupZ) >> 8);
											int32_t distanceX = ((Camera::g_activeCameraTransform.pos.x - pickupX32) >> 8)
												* ((Camera::g_activeCameraTransform.pos.x - pickupX32) >> 8);

											if (distanceYZ + distanceX < 589824 && distanceYZ + distanceX != -1)
												Particles::SpawnCollectSparkle(pickupX32, pickupY32, pickupZ, 50);
										}

										goto LABEL_55;

									// the disc launcher
									case 7:
										soundId = -1;
										g_grappleTimer = 0;
										g_discLauncherTimer += 10;

										if (g_discLauncherTimer > 30)
											g_discLauncherTimer = 30;

										if (g_unk830C90 > 0)
										{
											Levels::Type63* pickup = g_pendingPickup;
											g_pendingPickup->pos.y = g_pendingPickupY;
											g_unk830C90 = 0;
											Nu3D::Link::SetScaleFromFixedOffsets(pickup->props.linkId, 4096, 4096, 4096);

											int32_t pickupX = pickup->pos.x;
											int32_t pickupY = pickup->pos.y;
											int32_t pickupZ = 32 * pickup->pos.z;
											int32_t pickupY32 = 32 * pickupY;
											int32_t pickupX32 = 32 * pickupX;
											int32_t distanceYZ = ((Camera::g_activeCameraTransform.pos.y - pickupY32) >> 8)
													* ((Camera::g_activeCameraTransform.pos.y - pickupY32) >> 8)
												+ ((Camera::g_activeCameraTransform.pos.z - pickupZ) >> 8)
													* ((Camera::g_activeCameraTransform.pos.z - pickupZ) >> 8);
											int32_t distanceX = ((Camera::g_activeCameraTransform.pos.x - pickupX32) >> 8)
												* ((Camera::g_activeCameraTransform.pos.x - pickupX32) >> 8);

											if (distanceYZ + distanceX < 589824 && distanceYZ + distanceX != -1)
												Particles::SpawnCollectSparkle(pickupX32, pickupY32, pickupZ, 50);
										}

										goto LABEL_55;

									// the gravity boots
									case 8:
										soundId = -1;
										ActivateGravityBoots();

										if (g_unk830C90 > 0)
										{
											Levels::Type63* pickup = g_pendingPickup;
											g_pendingPickup->pos.y = g_pendingPickupY;
											g_unk830C90 = 0;

											Nu3D::Link::SetScaleFromFixedOffsets(pickup->props.linkId, 4096, 4096, 4096);

											int32_t pickupX = pickup->pos.x;
											int32_t pickupY = pickup->pos.y;
											int32_t pickupZ = 32 * pickup->pos.z;
											int32_t pickupY32 = 32 * pickupY;
											int32_t pickupX32 = 32 * pickupX;
											int32_t distanceYZ = ((Camera::g_activeCameraTransform.pos.y - pickupY32) >> 8)
													* ((Camera::g_activeCameraTransform.pos.y - pickupY32) >> 8)
												+ ((Camera::g_activeCameraTransform.pos.z - pickupZ) >> 8)
													* ((Camera::g_activeCameraTransform.pos.z - pickupZ) >> 8);
											int32_t distanceX = ((Camera::g_activeCameraTransform.pos.x - pickupX32) >> 8)
												* ((Camera::g_activeCameraTransform.pos.x - pickupX32) >> 8);

											if (distanceYZ + distanceX < 589824 && distanceYZ + distanceX != -1)
												Particles::SpawnCollectSparkle(pickupX32, pickupY32, pickupZ, 50);
										}

									LABEL_55:
										g_unk830C90 = 400;
										g_pendingPickupY = data->pos.y;
										g_pendingPickup = data;

										Nu3D::Link::SetScaleFromFixedOffsets(data->props.linkId, 0, 0, 0);
										goto LABEL_64;

									case 9:
										++g_unk830D4C;
										goto LABEL_63;

									// the green gun
									case 10:
										Buzz::g_greenGunCharge = 1200;
										goto LABEL_63;

									// a coin
									case 16: {
										int16_t coinsCollected = g_buzzActor.coinsCollected;
										g_hudWidgetTimers[2] = 180;

										if (g_buzzActor.coinsCollected < 99)
											coinsCollected = ++g_buzzActor.coinsCollected;

										if (coinsCollected == 50)
											AudioManager::PlaySoundEffect(79, 0);

										goto LABEL_63;
									}

									default:
										goto LABEL_64;
								}

							LABEL_63:
								soundId = -1;
							LABEL_64:
								if (hide)
								{
									if (hide != 1)
										goto LABEL_68;

									Nu3D::Link::SetScaleFromFixedOffsets(data->props.linkId, 0, 0, 0);
								}

								data->pos.y = 0x80000000;
							}
						}
					}

				LABEL_68:
					++data;

					if (! --remaining)
					{
						if (soundId)
							AudioManager::PlaySoundEffect(soundId, &collectPos);

						break;
					}
				}
			}

			// the tokens that were collected fly up and burst
			PickupSlot* timerSlot = &g_pickupSlots[0];

			do
			{
				int32_t collectTimer = timerSlot->collectTimer;

				if (timerSlot->collectTimer)
				{
					bool expired = collectTimer - (int32_t)Renderer::g_frameDelta <= 0;
					timerSlot->collectTimer = collectTimer - Renderer::g_frameDelta;

					if (expired)
						timerSlot->collectTimer = 0;

					if (collectTimer > 100 && timerSlot->collectTimer <= 100)
					{
						AudioManager::PlaySoundEffect(51, 0);
						Nu3D::Link::GetCurrentPosFixed(timerSlot->linkId, &spawnPos);

						int16_t* direction = &g_gameStruct5[1];

						do
						{
							int32_t random = *g_randDatBufferPtr - 128;
							int32_t directionZ = direction[1];
							++g_randDatBufferPtr;

							Nu3D::Particles::SpawnInstance(
								spawnPos.x, spawnPos.y, spawnPos.z, direction[-1] / 4, direction[0] / 4, directionZ / 4, 32, 0, random, 43);

							direction += 3;
						} while (direction < &g_gameStruct5[49]);
					}

					int32_t scale;

					if (collectTimer >= 88)
					{
						// the index is before the start of the table when the timer is 96, the original reads whatever is there
						if (collectTimer <= 96)
							scale = Numerics::g_sinCosLUT[12160 - 128 * collectTimer] / 4;
						else
							scale = 0;
					}
					else
					{
						scale = (((Numerics::g_sinCosLUT[256 * ((collectTimer - 4) & 0xF)] / 8) + 512) >> (5 - collectTimer / 16)) + 4096;
					}

					Nu3D::Link::SetScaleFromFixedOffsets(timerSlot->linkId, scale, scale, scale);
				}

				++timerSlot;
			} while (timerSlot < &g_pickupSlots[5]);

			// what happens after a token is collected
			int32_t state = g_unk830CC4;

			if (g_unk830CC4)
			{
				if (g_unk830CC4 == 1)
				{
					ResetBuzzState();
					g_buzzActor.actorFlags |= Actor::ACTOR_LOCKED_MOVE;
					InputManager::g_directionInputState &= 0x309;

					g_buzzActor.velX = 0;
					g_buzzActor.velForward = 0;
					g_unk830CC4 = 2;
				}
				else if (g_unk830CC4 != 2)
				{
				LABEL_97:
					if (state == 9)
					{
						Camera::g_cutsceneTimer = 1000;
						uint16_t yaw = g_buzzActor.posAngles.angles.yaw;
						int32_t yawDelta = (Camera::g_activeCameraTransform.angles.yaw - g_buzzActor.posAngles.angles.yaw - 2048) & 0xFFF;

						if (yawDelta >= 0x800)
							yawDelta -= 4096;

						g_buzzActor.posAngles.angles.yaw = (g_buzzActor.posAngles.angles.yaw + (yawDelta * (int32_t)Renderer::g_frameDelta) / 16) & 0xFFF;
						g_buzzActor.facingAngle = (yaw + (yawDelta * (int32_t)Renderer::g_frameDelta) / 16) & 0xFFF;
					}
					else
					{
						if (state == 3)
						{
							if (g_levelTransitionTimer < 890)
							{
								Levels::Method2(-3072, 5);
								g_unk830CC4 = 4;
								g_levelTransitionTimer = 68;
								AudioManager::PlaySoundEffect(51, &g_buzzActor.posAngles.pos);
								g_unk830D28 = g_buzzActor.gravityVel;
							}

							uint16_t yaw = g_buzzActor.posAngles.angles.yaw;
							int32_t yawDelta = (Camera::g_activeCameraTransform.angles.yaw - g_buzzActor.posAngles.angles.yaw - 2048) & 0xFFF;

							if (yawDelta >= 0x800)
								yawDelta -= 4096;

							g_buzzActor.posAngles.angles.yaw = (g_buzzActor.posAngles.angles.yaw + (yawDelta * (int32_t)Renderer::g_frameDelta) / 16) & 0xFFF;
							g_buzzActor.facingAngle = (yaw + (yawDelta * (int32_t)Renderer::g_frameDelta) / 16) & 0xFFF;
							state = g_unk830CC4;
						}

						if (state == 4)
						{
							Camera::g_cutsceneFocusPos.x = g_buzzActor.posAngles.pos.x;
							Camera::g_cutsceneFocusPos.y = g_buzzActor.posAngles.pos.y - 12288;
							Camera::g_cutsceneFocusPos.z = g_buzzActor.posAngles.pos.z;
							g_unk830D28 = g_buzzActor.gravityVel;
						}
					}

					return;
				}

				Buzz::g_unk53C838 = 0;

				if (! g_buzzActor.collisionFlags || g_buzzActor.velX || g_buzzActor.velForward)
					return;

				Camera::BeginScriptedCutsceneOnBuzz(1000);

				if (g_unk830CAC)
				{
					g_unk830CC4 = 3;
					g_levelTransition = 1;
					g_levelTransitionTimer = 1000;
					AudioManager::PlaySoundEffect(169, 0);
				}
				else
				{
					g_unk830CC4 = 9;
				}

				state = g_unk830CC4;

				g_unk830C94 = 0;
				goto LABEL_97;
			}
		}
	}
}
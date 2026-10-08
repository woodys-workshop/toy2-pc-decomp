#include "Toy2/Platform.h"
#include "Toy2/Collision.h"
#include "Toy2/Toy2.h"
#include "Nu3D/Math.h"
#include "Nu3D/Link.h"
#include "Toy2/Buzz.h"
#include "Toy2/Levels.h"
#include "Renderer/Renderer.h"
#include "Random.h"
#include "Numerics.h"

#include <stdlib.h>
#include <math.h>

namespace Toy2
{
	namespace Platform
	{
		// GLOBAL: TOY2 0x00559E80
		int32_t g_unk559E80 = -1;

		// GLOBAL: TOY2 0x00728720
		PlatformDescriptor g_platformDescriptors[32];

		// GLOBAL: TOY2 0x0052FE48
		PathPlatform g_pathPlatforms[5] = {
			{ -1, -1, -1, -1, -1, -1, 0xFFFF, 0xFFFF, -1, -1 }, { -1, -1, -1, -1, -1, -1, 0xFFFF, 0xFFFF, -1, -1 },
			{ -1, -1, -1, -1, -1, -1, 0xFFFF, 0xFFFF, -1, -1 }, { -1, -1, -1, -1, -1, -1, 0xFFFF, 0xFFFF, -1, -1 },
			{ -1, -1, -1, -1, -1, -1, 0xFFFF, 0xFFFF, -1, -1 },
		};

		// FUNCTION: TOY2 0x0042C2B0
		void InitPathPlatform(int32_t index, int32_t platformIdx, int32_t pathIdx, int32_t linkId, int32_t linkId2, int32_t a6, int32_t pathPosition, int32_t y)
		{
			Vector3I pos;

			g_pathPlatforms[index].pathPosition = pathPosition;
			g_pathPlatforms[index].unkInt2 = 0;
			g_pathPlatforms[index].platformIdx = platformIdx;
			g_pathPlatforms[index].pathIdx = pathIdx;

			g_pathPlatforms[index].linkId = linkId;
			g_pathPlatforms[index].linkId2 = linkId2;
			g_pathPlatforms[index].rotationY = y;
			g_pathPlatforms[index].unkShort2 = 1;

			g_pathPlatforms[index].unkInt3 = 0x7FFFFFFF;
			g_pathPlatforms[index].unkInt4 = 0x7FFFFFFF;

			SamplePoint(g_pathPlatforms[index].pathIdx, g_pathPlatforms[index].pathPosition, &pos);
			SetOrigin(g_pathPlatforms[index].platformIdx, 32 * pos.x, 32 * pos.y, 32 * pos.z);

			if (g_pathPlatforms[index].linkId2)
				Nu3D::Link::SetPositionRawAndCommit(g_pathPlatforms[index].linkId2, pos.x, pos.y, pos.z);

			Nu3D::Link::SetPositionRawAndCommit(g_pathPlatforms[index].linkId, pos.x, pos.y, pos.z);
			SetRotationAngles(platformIdx, 0, y, 0);
			Nu3D::Link::SetRotationRelative8bit(linkId, 0, y, 0);

			if (linkId2)
				Nu3D::Link::SetRotationRelative8bit(linkId2, 0, y, 0);
		}

		// FUNCTION: TOY2 0x004878D0
		void DisableCollision(int32_t platformIdx)
		{
			int16_t meshInstanceId = g_platformDescriptors[platformIdx].meshInstanceId;

			if (meshInstanceId)
				Collision::g_collisionMeshInstances[meshInstanceId].typeFlags = 0;
		}

		// FUNCTION: TOY2 0x00487900
		void SetVelocity(int32_t platformIndex, int32_t velX, int32_t velY, int32_t velZ)
		{
			if (g_platformDescriptors[platformIndex].meshInstanceId)
			{
				if (velX || velY || velZ)
				{
					g_platformDescriptors[platformIndex].flags |= 0x80;
					g_platformDescriptors[platformIndex].linVel.x = velX;
					g_platformDescriptors[platformIndex].linVel.y = velY;
					g_platformDescriptors[platformIndex].linVel.z = velZ;
				}
				else
				{
					g_platformDescriptors[platformIndex].flags &= ~0x80;
					g_platformDescriptors[platformIndex].linVel.x = 0;
					g_platformDescriptors[platformIndex].linVel.y = 0;
					g_platformDescriptors[platformIndex].linVel.z = 0;
				}
			}
		}

		// FUNCTION: TOY2 0x00487970
		void SetAngularVelocity(int32_t idx, int16_t x, int16_t y, int16_t z)
		{
			if (g_platformDescriptors[idx].meshInstanceId)
			{
				g_platformDescriptors[idx].flags |= 8;
				g_platformDescriptors[idx].angVel.x = x;
				g_platformDescriptors[idx].angVel.y = y;
				g_platformDescriptors[idx].angVel.z = z;
			}
		}

		// FUNCTION: TOY2 0x004879C0
		void GetOrigin(int32_t platformindex, Vector3I* outOrigin)
		{
			int16_t id = g_platformDescriptors[platformindex].meshInstanceId;

			if (id)
			{
				outOrigin->x = Collision::g_collisionMeshInstances[id].origin.x;
				outOrigin->y = Collision::g_collisionMeshInstances[g_platformDescriptors[platformindex].meshInstanceId].origin.y;
				outOrigin->z = Collision::g_collisionMeshInstances[g_platformDescriptors[platformindex].meshInstanceId].origin.z;
			}
		}

		// FUNCTION: TOY2 0x00487A20
		void GetRotation(int32_t platformIdx, Vector3I* vecOut)
		{
			if (g_platformDescriptors[platformIdx].meshInstanceId)
			{
				vecOut->x = g_platformDescriptors[platformIdx].rotAngle.x;
				vecOut->y = g_platformDescriptors[platformIdx].rotAngle.y;
				vecOut->z = g_platformDescriptors[platformIdx].rotAngle.z;
			}
		}

		// FUNCTION: TOY2 0x00488510
		void SetOrigin(int32_t platformIndex, int32_t x, int32_t y, int32_t z)
		{
			int32_t meshInstanceId = g_platformDescriptors[platformIndex].meshInstanceId;

			Collision::g_collisionMeshInstances[meshInstanceId].boundsMin.x += x - Collision::g_collisionMeshInstances[meshInstanceId].origin.x;
			Collision::g_collisionMeshInstances[meshInstanceId].boundsMin.z += z - Collision::g_collisionMeshInstances[meshInstanceId].origin.z;

			Collision::g_collisionMeshInstances[meshInstanceId].origin.x = x;
			Collision::g_collisionMeshInstances[meshInstanceId].origin.y = y;
			Collision::g_collisionMeshInstances[meshInstanceId].origin.z = z;
		}

		// FUNCTION: TOY2 0x00488580
		int32_t GetFlags(int32_t platformIdx) { return g_platformDescriptors[platformIdx].flags; }

		// FUNCTION: TOY2 0x004885A0
		int32_t GetContactFace32(int32_t index) { return (int32_t)g_platformDescriptors[index].contactFace + 32; }

		// FUNCTION: TOY2 0x00488A60
		void AddFlags(int32_t platformIdx, int16_t flags) { g_platformDescriptors[platformIdx].flags |= flags; }

		// FUNCTION: TOY2 0x00488A80
		void ClearFlags(int32_t platformIdx, int16_t flags) { g_platformDescriptors[platformIdx].flags &= ~flags; }

		// FUNCTION: TOY2 0x00488AA0
		void SetRotationAngles(int32_t platformIdx, int16_t x, int16_t y, int16_t z)
		{
			g_platformDescriptors[platformIdx].rotAngle.x = 4 * x;
			g_platformDescriptors[platformIdx].rotAngle.y = 4 * y;
			g_platformDescriptors[platformIdx].rotAngle.z = 4 * z;

			g_platformDescriptors[platformIdx].flags |= 8;
		}

		// FUNCTION: TOY2 0x00488AF0
		void GetRotationAngles(int32_t platformIdx, Vector3I* vecOut)
		{
			vecOut->x = g_platformDescriptors[platformIdx].rotAngle.x >> 2;
			vecOut->y = g_platformDescriptors[platformIdx].rotAngle.y >> 2;
			vecOut->z = g_platformDescriptors[platformIdx].rotAngle.z >> 2;
		}

		// FUNCTION: TOY2 0x00488BD0
		void CommitRotationToLink(int32_t platformIdx, int32_t linkId)
		{
			Nu3D::Link::SetRotationAbsolute8bit(
				linkId,
				g_platformDescriptors[platformIdx].rotAngle.x >> 2,
				g_platformDescriptors[platformIdx].rotAngle.y >> 2,
				g_platformDescriptors[platformIdx].rotAngle.z >> 2);
		}

		// FUNCTION: TOY2 0x00488C10
		void AdvancePartialMotion(int32_t a1, int32_t a2, int32_t a3)
		{
			int32_t i;
			int32_t platformIdx;
			int32_t meshInstanceId;
			int32_t x;
			int32_t y;
			int32_t z;
			int32_t angVelX;
			int32_t angVelY;
			int32_t angVelZ;

			for (i = 0; i < Collision::g_unk7290E4; i++)
			{
				if (! a3)
				{
					platformIdx = Collision::g_unk7290A0[i];

					if ((g_platformDescriptors[platformIdx].flags & 0x80) != 0)
					{
						x = g_platformDescriptors[platformIdx].prevLinVel.x;
						meshInstanceId = g_platformDescriptors[platformIdx].meshInstanceId;

						x = a1 * x / (a1 - a2);
						y = a1 * g_platformDescriptors[platformIdx].prevLinVel.y;
						Collision::g_collisionMeshInstances[meshInstanceId].origin.x += x;
						Collision::g_collisionMeshInstances[meshInstanceId].origin.y =
							y / (a1 - a2) + Collision::g_collisionMeshInstances[meshInstanceId].origin.y;
						z = a1 * g_platformDescriptors[platformIdx].prevLinVel.z / (a1 - a2);
						Collision::g_collisionMeshInstances[meshInstanceId].origin.z += z;
						Collision::g_collisionMeshInstances[meshInstanceId].boundsMin.x += x;
						Collision::g_collisionMeshInstances[meshInstanceId].boundsMin.z += z;
						g_platformDescriptors[platformIdx].prevLinVel.x = g_platformDescriptors[platformIdx].prevLinVel.x - x;
						g_platformDescriptors[platformIdx].prevLinVel.y -= a1 * g_platformDescriptors[platformIdx].prevLinVel.y / (a1 - a2);
						g_platformDescriptors[platformIdx].prevLinVel.z -= a1 * g_platformDescriptors[platformIdx].prevLinVel.z / (a1 - a2);
					}

					if ((g_platformDescriptors[platformIdx].flags & 8) != 0)
					{
						g_platformDescriptors[platformIdx].rotAngle.x += a1 * g_platformDescriptors[platformIdx].prevAngVel.x / (a1 - a2);
						g_platformDescriptors[platformIdx].rotAngle.y += a1 * g_platformDescriptors[platformIdx].prevAngVel.y / (a1 - a2);
						g_platformDescriptors[platformIdx].rotAngle.z += a1 * g_platformDescriptors[platformIdx].prevAngVel.z / (a1 - a2);

						angVelX = g_platformDescriptors[platformIdx].prevAngVel.x;
						g_platformDescriptors[platformIdx].prevAngVel.x = angVelX - a1 * angVelX / (a1 - a2);
						angVelY = g_platformDescriptors[platformIdx].prevAngVel.y;
						g_platformDescriptors[platformIdx].prevAngVel.y = angVelY - a1 * angVelY / (a1 - a2);
						angVelZ = g_platformDescriptors[platformIdx].prevAngVel.z;
						g_platformDescriptors[platformIdx].prevAngVel.z = angVelZ - a1 * angVelZ / (a1 - a2);
					}
				}
			}
		}

		// FUNCTION: TOY2 0x00488E10
		char UpdateBuzzPlatformMotion(int32_t a1, int32_t a2)
		{
			PlatformDescriptor* descriptor;
			Matrix3x3I16* rotation = (Matrix3x3I16*)&Collision::g_mathScratch[948];
			Matrix3x3I16* nextRotation = (Matrix3x3I16*)&Collision::g_mathScratch[980];
			Vector3I16 angles;
			uint8_t* cache;
			int32_t result;
			int32_t carried = -1;
			int32_t standing;
			int32_t forced;
			int32_t i;
			int32_t index;
			int32_t z;
			int32_t meshInstanceId;
			int32_t x;
			int32_t y;
			int32_t localX;
			int32_t localY;
			int32_t localZ;
			int32_t highX;
			int32_t highY;
			int32_t highZ;
			int32_t lowX;
			int32_t lowY;
			int32_t lowZ;
			int32_t originX;
			int32_t originY;
			int32_t originZ;
			int32_t velZ;
			int32_t velX;
			int32_t velY;
			int16_t flags;

			result = (char)a2;

			if (a2 != 2)
			{
				if (g_unk559E80 < 0 || Toy2::g_levelFileIndex != 10)
					forced = 0;
				else
					forced = 1;

				if (Collision::g_unk729110 < 0 || Toy2::g_buzzActor.unkWord15
					|| (standing = Collision::g_unk729110, (g_platformDescriptors[Collision::g_unk729110].flags & 0x10) == 0))
					standing = -1;

				// the cached ground queries run out
				cache = Collision::g_unk72871C;

				for (i = 0; i < 11776; i += 1472)
				{
					if (*(int16_t*)(cache + i + 24) > 0)
					{
						*(int16_t*)(cache + i + 24) = *(int16_t*)(cache + i + 24) - 1;
						cache = Collision::g_unk72871C;
					}
				}

				z = Toy2::g_buzzActor.posAngles.pos.z;
				index = 0;
				descriptor = &g_platformDescriptors[0];

				do
				{
					flags = descriptor->flags;

					if ((flags & 0x100) != 0 && (flags & 0x12) != 0 && ! Toy2::g_buzzActor.unkWord15)
						standing = index;

					flags = (flags & 0xFF00) | (flags & 0xFC);
					descriptor->flags = flags;

					if ((flags & 0x80) != 0)
					{
						descriptor->prevLinVel.x = descriptor->linVel.x;
						descriptor->prevLinVel.y = descriptor->linVel.y;
						descriptor->prevLinVel.z = descriptor->linVel.z;
					}

					if ((descriptor->flags & 8) != 0)
					{
						descriptor->prevAngVel.x = descriptor->angVel.x;
						descriptor->prevAngVel.y = descriptor->angVel.y;
						descriptor->prevAngVel.z = descriptor->angVel.z;
					}

					if (index == g_unk559E80 || index == standing || forced)
					{
						descriptor->prevLinVel.x = 0;
						descriptor->prevLinVel.y = 0;
						descriptor->prevLinVel.z = 0;
						descriptor->prevAngVel.x = 0;
						descriptor->prevAngVel.y = 0;
						descriptor->prevAngVel.z = 0;

						flags = descriptor->flags;

						if ((flags & 0x100) != 0)
						{
							x = descriptor->linVel.x;
							meshInstanceId = descriptor->meshInstanceId;
							Collision::g_collisionMeshInstances[meshInstanceId].origin.x += x;
							y = descriptor->linVel.y;
							Collision::g_collisionMeshInstances[meshInstanceId].origin.y += y;
							velZ = descriptor->linVel.z;
							Collision::g_collisionMeshInstances[meshInstanceId].origin.z += velZ;
							Collision::g_collisionMeshInstances[meshInstanceId].boundsMin.x += x;
							Collision::g_collisionMeshInstances[meshInstanceId].boundsMin.z += velZ;

							if (index == g_unk559E80 || g_unk559E80 == -1)
							{
								z += velZ;
								carried = index;
								Toy2::g_buzzActor.posAngles.pos.z = z;
								Toy2::g_buzzActor.posAngles.pos.x += x;
								Toy2::g_buzzActor.posAngles.pos.y += y;

								if ((flags & 8) != 0)
								{
									angles.x = descriptor->rotAngle.x >> 2;
									angles.y = descriptor->rotAngle.y >> 2;
									angles.z = descriptor->rotAngle.z >> 2;

									Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, rotation);

									angles.x = (descriptor->angVel.x + descriptor->rotAngle.x) >> 2;
									angles.y = (descriptor->angVel.y + descriptor->rotAngle.y) >> 2;
									angles.z = (descriptor->angVel.z + descriptor->rotAngle.z) >> 2;

									Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, nextRotation);

									originZ = Collision::g_collisionMeshInstances[descriptor->meshInstanceId].origin.z;
									originY = Collision::g_collisionMeshInstances[descriptor->meshInstanceId].origin.y;
									originX = Collision::g_collisionMeshInstances[descriptor->meshInstanceId].origin.x;

									localX = (Toy2::g_buzzActor.posAngles.pos.z - originZ) * rotation->m20
										+ (Toy2::g_buzzActor.posAngles.pos.y - originY) * rotation->m10
										+ (Toy2::g_buzzActor.posAngles.pos.x - originX) * rotation->m00;
									localY = (Toy2::g_buzzActor.posAngles.pos.z - originZ) * rotation->m21
										+ (Toy2::g_buzzActor.posAngles.pos.y - originY) * rotation->m11
										+ (Toy2::g_buzzActor.posAngles.pos.x - originX) * rotation->m01;
									localZ = (Toy2::g_buzzActor.posAngles.pos.z - originZ) * rotation->m22
										+ (Toy2::g_buzzActor.posAngles.pos.y - originY) * rotation->m12
										+ (Toy2::g_buzzActor.posAngles.pos.x - originX) * rotation->m02;

									lowZ = localZ & 0xFFF;
									lowY = localY & 0xFFF;
									lowX = localX & 0xFFF;

									Toy2::g_buzzActor.posAngles.pos.x = originX
										+ (localZ / 4096 * nextRotation->m02 + localY / 4096 * nextRotation->m01 + localX / 4096 * nextRotation->m00
										   + lowZ * nextRotation->m02 / 4096 + lowY * nextRotation->m01 / 4096 + lowX * nextRotation->m00 / 4096)
											/ 4096;
									Toy2::g_buzzActor.posAngles.pos.y = originY
										+ (localZ / 4096 * nextRotation->m12 + localY / 4096 * nextRotation->m11 + localX / 4096 * nextRotation->m10
										   + lowZ * nextRotation->m12 / 4096 + lowY * nextRotation->m11 / 4096 + lowX * nextRotation->m10 / 4096)
											/ 4096;

									velY = localZ / 4096 * nextRotation->m22 + localY / 4096 * nextRotation->m21 + localX / 4096 * nextRotation->m20
										+ lowZ * nextRotation->m22 / 4096 + lowY * nextRotation->m21 / 4096 + lowX * nextRotation->m20 / 4096;

									descriptor->rotAngle.x += descriptor->angVel.x;
									descriptor->rotAngle.y += descriptor->angVel.y;
									z = originZ + velY / 4096;
									descriptor->rotAngle.z += descriptor->angVel.z;
									Toy2::g_buzzActor.posAngles.pos.z = z;
								}
							}

							if (Toy2::g_buzzActor.velX || Toy2::g_buzzActor.velForward)
								Collision::g_buzzCollisionStates[0].pushbackVel.y += 800;
						}
					}
					else if ((descriptor->flags & 0x12) != 0)
					{
						Collision::g_buzzCollisionStates[0].pushbackVel.x += descriptor->linVel.x;
						Collision::g_buzzCollisionStates[0].pushbackVel.y += descriptor->linVel.y;
						Collision::g_buzzCollisionStates[0].pushbackVel.z += descriptor->linVel.z;

						if (descriptor->prevAngVel.x || descriptor->prevAngVel.y || descriptor->prevAngVel.z)
							Collision::g_buzzCollisionStates[0].unkWord20 = 0;

						angles.x = descriptor->rotAngle.x >> 2;
						angles.y = descriptor->rotAngle.y >> 2;
						angles.z = descriptor->rotAngle.z >> 2;

						Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, rotation);

						angles.x = (descriptor->prevAngVel.x + descriptor->rotAngle.x) >> 2;
						angles.y = (descriptor->rotAngle.y + descriptor->prevAngVel.y) >> 2;
						angles.z = (descriptor->rotAngle.z + descriptor->prevAngVel.z) >> 2;

						Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, nextRotation);

						z = Toy2::g_buzzActor.posAngles.pos.z;
						meshInstanceId = descriptor->meshInstanceId;

						localZ = Toy2::g_buzzActor.posAngles.pos.z - Collision::g_collisionMeshInstances[meshInstanceId].origin.z;
						localY = Toy2::g_buzzActor.posAngles.pos.y - Collision::g_collisionMeshInstances[meshInstanceId].origin.y;
						localX = Toy2::g_buzzActor.posAngles.pos.x - Collision::g_collisionMeshInstances[meshInstanceId].origin.x;

						highX = (localZ * rotation->m20 + localY * rotation->m10 + localX * rotation->m00) / 4096;
						highY = (localZ * rotation->m21 + localY * rotation->m11 + localX * rotation->m01) / 4096;
						highZ = (localZ * rotation->m22 + localY * rotation->m12 + localX * rotation->m02) / 4096;

						Collision::g_buzzCollisionStates[0].pushbackVel.x += (uint16_t)Collision::g_collisionMeshInstances[meshInstanceId].origin.x
							+ (highX * nextRotation->m00 + highY * nextRotation->m01 + highZ * nextRotation->m02) / 4096
							- (uint16_t)Toy2::g_buzzActor.posAngles.pos.x;
						Collision::g_buzzCollisionStates[0].pushbackVel.y += (uint16_t)Collision::g_collisionMeshInstances[meshInstanceId].origin.y
							+ (highX * nextRotation->m10 + highY * nextRotation->m11 + highZ * nextRotation->m12) / 4096
							- (uint16_t)Toy2::g_buzzActor.posAngles.pos.y;
						Collision::g_buzzCollisionStates[0].pushbackVel.z += (uint16_t)Collision::g_collisionMeshInstances[meshInstanceId].origin.z
							+ (highX * nextRotation->m20 + highY * nextRotation->m21 + highZ * nextRotation->m22) / 4096
							- (uint16_t)Toy2::g_buzzActor.posAngles.pos.z;
					}

					*(uint8_t*)&descriptor->flags &= ~0x10;
					descriptor++;
					index++;
				} while (descriptor < &g_platformDescriptors[32]);

				result = 48 * a1;
				Collision::g_buzzCollisionStates[a1].pushbackVel.x += Collision::g_buzzCollisionStates[a1].responseVel.x;
				Collision::g_buzzCollisionStates[a1].pushbackVel.y += Collision::g_buzzCollisionStates[a1].responseVel.y;
				Collision::g_buzzCollisionStates[a1].pushbackVel.z += Collision::g_buzzCollisionStates[a1].responseVel.z;
				Collision::g_unk729110 = standing;

				// Buzz is carried by the platform he stands on
				if (Collision::g_unk728718 != -1 && carried == -1 && ! Toy2::g_buzzActor.unkWord15)
				{
					descriptor = &g_platformDescriptors[Collision::g_unk728718];

					velX = descriptor->linVel.x;
					velY = descriptor->linVel.y / 2;
					Toy2::g_buzzActor.posAngles.pos.y += velY;
					velZ = descriptor->linVel.z;
					result = (char)descriptor->flags;
					Toy2::g_buzzActor.posAngles.pos.z = velZ / 2 + z;
					Toy2::g_buzzActor.velForward += velZ / 2;
					x = velX / 2;
					Toy2::g_buzzActor.posAngles.pos.x += x;
					Toy2::g_buzzActor.velX += x;
					Toy2::g_buzzActor.gravityVel += velY;

					if ((result & 8) != 0)
					{
						angles.x = descriptor->rotAngle.x >> 2;
						angles.y = descriptor->rotAngle.y >> 2;
						angles.z = descriptor->rotAngle.z >> 2;

						Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, rotation);

						angles.x = (descriptor->angVel.x + descriptor->rotAngle.x) >> 2;
						angles.y = (descriptor->angVel.y + descriptor->rotAngle.y) >> 2;
						angles.z = (descriptor->angVel.z + descriptor->rotAngle.z) >> 2;

						Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles, nextRotation);

						meshInstanceId = descriptor->meshInstanceId;
						originZ = Collision::g_collisionMeshInstances[meshInstanceId].origin.z;
						originX = Collision::g_collisionMeshInstances[meshInstanceId].origin.x;
						originY = Collision::g_collisionMeshInstances[meshInstanceId].origin.y;

						highX = ((Toy2::g_buzzActor.posAngles.pos.z - originZ) * rotation->m20 + (Toy2::g_buzzActor.posAngles.pos.y - originY) * rotation->m10
								 + (Toy2::g_buzzActor.posAngles.pos.x - originX) * rotation->m00)
							/ 4096;
						highY = ((Toy2::g_buzzActor.posAngles.pos.z - originZ) * rotation->m21 + (Toy2::g_buzzActor.posAngles.pos.y - originY) * rotation->m11
								 + (Toy2::g_buzzActor.posAngles.pos.x - originX) * rotation->m01)
							/ 4096;
						highZ = ((Toy2::g_buzzActor.posAngles.pos.z - originZ) * rotation->m22 + (Toy2::g_buzzActor.posAngles.pos.y - originY) * rotation->m12
								 + (Toy2::g_buzzActor.posAngles.pos.x - originX) * rotation->m02)
							/ 4096;

						localX = originX + (highX * nextRotation->m00 + highY * nextRotation->m01 + highZ * nextRotation->m02) / 4096;
						Toy2::g_buzzActor.posAngles.pos.x += (localX - Toy2::g_buzzActor.posAngles.pos.x) / 2;
						localY = originY + (highX * nextRotation->m10 + highY * nextRotation->m11 + highZ * nextRotation->m12) / 4096;
						Toy2::g_buzzActor.posAngles.pos.y += (localY - Toy2::g_buzzActor.posAngles.pos.y) / 2;
						localZ = (highX * nextRotation->m20 + highY * nextRotation->m21 + highZ * nextRotation->m22) / 4096 + originZ;
						Toy2::g_buzzActor.posAngles.pos.z += (localZ - Toy2::g_buzzActor.posAngles.pos.z) / 2;
						Toy2::g_buzzActor.velX += (localX - Toy2::g_buzzActor.posAngles.pos.x) / 2;
						Toy2::g_buzzActor.gravityVel += (localY - Toy2::g_buzzActor.posAngles.pos.y) / 2;
						result = (localZ - Toy2::g_buzzActor.posAngles.pos.z) / 2;
						Toy2::g_buzzActor.velForward += result;
					}
				}

				Collision::g_unk728718 = carried;
			}

			return (char)result;
		}

		// FUNCTION: TOY2 0x0048ACC0
		void StepMotionScript(int32_t platformIndex, int32_t linkId, int16_t** scriptIP, int32_t* timer, int32_t* accelRamp)
		{
			int16_t* script = *scriptIP;
			Vector3I target;
			int32_t originX;
			int32_t originY;
			int32_t originZ;

			switch (*script)
			{
				// jump back
				case 0:
					*scriptIP = &script[-script[1]];
					*timer = 0;

					break;

				// move relative to the current position of the link
				case 1: {
					Nu3D::Link::GetTargetPosFixed(linkId, &target);

					if (g_platformDescriptors[platformIndex].meshInstanceId != 0)
					{
						int32_t meshInstanceId = g_platformDescriptors[platformIndex].meshInstanceId;

						originX = Collision::g_collisionMeshInstances[meshInstanceId].origin.x;
						originY = Collision::g_collisionMeshInstances[meshInstanceId].origin.y;
						originZ = Collision::g_collisionMeshInstances[meshInstanceId].origin.z;
					}

					target.z >>= 5;
					originZ >>= 5;
					target.x >>= 5;
					target.y >>= 5;
					target.x += (*scriptIP)[1];
					target.y += (*scriptIP)[2];
					target.z += (*scriptIP)[3];

					int32_t posX = originX >> 5;
					originY >>= 5;

					Nu3D::Link::SetPositionRawAndCommit(linkId, posX, originY, originZ);

					int32_t distX = abs(posX - target.x);
					int32_t slowDistance = 4 * (*scriptIP)[4] + 48;

					// close to the target, slow down
					if (! (distX >= slowDistance || abs(originY - target.y) >= slowDistance || abs(originZ - target.z) >= slowDistance))
					{
						if (*accelRamp > 0)
							*accelRamp = -64;
					}

					if (*accelRamp >= 0)
					{
						if (*accelRamp < 64)
							*accelRamp = *accelRamp + 2 * (int32_t)Renderer::g_frameDelta;
					}
					else
					{
						int32_t newRamp = *accelRamp + 2 * (int32_t)Renderer::g_frameDelta;
						*accelRamp = newRamp;

						if (newRamp > 0)
							*accelRamp = 0;
					}

					int32_t ramp = *accelRamp;

					if (ramp == 0)
					{
						*accelRamp = 0;
						*timer = 0;
						*scriptIP = *scriptIP + 5;

						if (g_platformDescriptors[platformIndex].meshInstanceId != 0)
						{
							g_platformDescriptors[platformIndex].flags &= ~0x80;
							g_platformDescriptors[platformIndex].linVel.x = 0;
							g_platformDescriptors[platformIndex].linVel.y = 0;
							g_platformDescriptors[platformIndex].linVel.z = 0;
						}

						break;
					}

					int32_t speed = (*scriptIP)[4];

					// arrived
					if (distX < speed + 2 && abs(originY - target.y) < speed + 2 && abs(originZ - target.z) < speed + 2)
					{
						*accelRamp = 0;
						*timer = 0;
						*scriptIP = *scriptIP + 5;

						if (g_platformDescriptors[platformIndex].meshInstanceId != 0)
						{
							g_platformDescriptors[platformIndex].flags &= ~0x80;
							g_platformDescriptors[platformIndex].linVel.x = 0;
							g_platformDescriptors[platformIndex].linVel.y = 0;
							g_platformDescriptors[platformIndex].linVel.z = 0;
						}

						break;
					}

					int32_t curSpeed;

					if (ramp >= 0)
					{
						if (ramp >= 64)
							curSpeed = (*scriptIP)[4];
						else
							curSpeed = speed * ramp / 64;
					}
					else
					{
						curSpeed = speed * ramp / -64;
					}

					if (curSpeed < 1)
						curSpeed = 1;

					target.x -= posX;
					target.y -= originY;
					target.z -= originZ;
					Nu3D::Math::NormalizeToFixedPoint(&target, &target);

					int32_t velX = (target.x * curSpeed * (int32_t)Renderer::g_frameDelta) >> 7;
					int32_t velY = (target.y * curSpeed * (int32_t)Renderer::g_frameDelta) >> 7;
					int32_t velZ = (target.z * curSpeed * (int32_t)Renderer::g_frameDelta) >> 7;

					if (g_platformDescriptors[platformIndex].meshInstanceId != 0)
					{
						if (velX == 0 && velY == 0 && velZ == 0)
						{
							g_platformDescriptors[platformIndex].flags &= ~0x80;
							g_platformDescriptors[platformIndex].linVel.x = 0;
							g_platformDescriptors[platformIndex].linVel.y = 0;
							g_platformDescriptors[platformIndex].linVel.z = 0;
						}
						else
						{
							g_platformDescriptors[platformIndex].flags |= 0x80;
							g_platformDescriptors[platformIndex].linVel.x = velX;
							g_platformDescriptors[platformIndex].linVel.y = velY;
							g_platformDescriptors[platformIndex].linVel.z = velZ;
						}
					}

					break;
				}

				// wait
				case 2:
					if (*timer != 0)
					{
						int32_t newTimer = *timer - (int32_t)Renderer::g_frameDelta;
						*timer = newTimer;

						if (newTimer <= 0)
						{
							*timer = 0;
							*scriptIP += 2;
						}
					}
					else
					{
						*timer = script[1];
					}

					break;

				// wait for a random time
				case 3:
					if (*timer != 0)
					{
						int32_t newTimer = *timer - (int32_t)Renderer::g_frameDelta;
						*timer = newTimer;

						if (newTimer <= 0)
						{
							*timer = 0;
							*scriptIP += 3;
						}
					}
					else
					{
						*timer = script[2] + (*g_randDatBufferPtr++ & script[1]);
					}

					break;

				// move to a point of a record
				case 4: {
					if (g_platformDescriptors[platformIndex].meshInstanceId != 0)
					{
						int32_t meshInstanceId = g_platformDescriptors[platformIndex].meshInstanceId;

						originX = Collision::g_collisionMeshInstances[meshInstanceId].origin.x;
						originY = Collision::g_collisionMeshInstances[meshInstanceId].origin.y;
						originZ = Collision::g_collisionMeshInstances[meshInstanceId].origin.z;
					}

					int32_t posX = originX >> 5;
					target.x = Levels::g_recordData[script[1]]->data[script[2]].x;
					target.y = Levels::g_recordData[script[1]]->data[script[2]].y;
					target.z = Levels::g_recordData[script[1]]->data[script[2]].z;
					originY >>= 5;
					originZ >>= 5;

					Nu3D::Link::SetPositionRawAndCommit(linkId, posX, originY, originZ);

					int32_t distX = abs(posX - target.x);
					int32_t slowDistance = 4 * (*scriptIP)[3] + 48;

					if (! (distX >= slowDistance || abs(originY - target.y) >= slowDistance || abs(originZ - target.z) >= slowDistance))
					{
						if (*accelRamp > 0)
							*accelRamp = -64;
					}

					if (*accelRamp >= 0)
					{
						if (*accelRamp < 64)
							*accelRamp = *accelRamp + 2 * (int32_t)Renderer::g_frameDelta;
					}
					else
					{
						int32_t newRamp = *accelRamp + 2 * (int32_t)Renderer::g_frameDelta;
						*accelRamp = newRamp;

						if (newRamp > 0)
							*accelRamp = 0;
					}

					int32_t ramp = *accelRamp;

					if (ramp == 0)
					{
						*accelRamp = 0;
						*timer = 0;
						*scriptIP = *scriptIP + 4;

						if (g_platformDescriptors[platformIndex].meshInstanceId != 0)
						{
							g_platformDescriptors[platformIndex].flags &= ~0x80;
							g_platformDescriptors[platformIndex].linVel.x = 0;
							g_platformDescriptors[platformIndex].linVel.y = 0;
							g_platformDescriptors[platformIndex].linVel.z = 0;
						}

						break;
					}

					int32_t speed = (*scriptIP)[3];

					if (distX >= speed + 2 || abs(originY - target.y) >= speed + 2 || abs(originZ - target.z) >= speed + 2)
					{
						int32_t curSpeed;

						if (ramp >= 0)
						{
							if (ramp >= 64)
								curSpeed = (*scriptIP)[3];
							else
								curSpeed = speed * ramp / 64;
						}
						else
						{
							curSpeed = speed * ramp / -64;
						}

						if (curSpeed < 1)
							curSpeed = 1;

						target.x -= posX;
						target.y -= originY;
						target.z -= originZ;
						Nu3D::Math::NormalizeToFixedPoint(&target, &target);

						int32_t velX = (target.x * curSpeed * (int32_t)Renderer::g_frameDelta) >> 7;
						int32_t velY = (target.y * curSpeed * (int32_t)Renderer::g_frameDelta) >> 7;
						int32_t velZ = (target.z * curSpeed * (int32_t)Renderer::g_frameDelta) >> 7;

						if (g_platformDescriptors[platformIndex].meshInstanceId != 0)
						{
							if (velX != 0 || velY != 0 || velZ != 0)
							{
								g_platformDescriptors[platformIndex].flags |= 0x80;
								g_platformDescriptors[platformIndex].linVel.x = velX;
								g_platformDescriptors[platformIndex].linVel.y = velY;
								g_platformDescriptors[platformIndex].linVel.z = velZ;
							}
							else
							{
								g_platformDescriptors[platformIndex].flags &= ~0x80;
								g_platformDescriptors[platformIndex].linVel.x = 0;
								g_platformDescriptors[platformIndex].linVel.y = 0;
								g_platformDescriptors[platformIndex].linVel.z = 0;
							}
						}
					}
					else
					{
						*accelRamp = 0;
						*timer = 0;
						*scriptIP = *scriptIP + 4;

						if (g_platformDescriptors[platformIndex].meshInstanceId != 0)
						{
							g_platformDescriptors[platformIndex].flags &= ~0x80;
							g_platformDescriptors[platformIndex].linVel.x = 0;
							g_platformDescriptors[platformIndex].linVel.y = 0;
							g_platformDescriptors[platformIndex].linVel.z = 0;
						}
					}

					break;
				}

				// skip the next instruction if a flag is set
				case 6:
					if ((Collision::g_unk72D2A4 & script[1]) != 0)
						*scriptIP = script + 2;

					break;

				// skip the next instruction if a flag is not set
				case 7:
					if ((Collision::g_unk72D2A4 & script[1]) == 0)
						*scriptIP = script + 2;

					break;

				case 8:
					Collision::g_unk72D2A4 |= script[1];
					*scriptIP += 2;

					break;

				case 9:
					Collision::g_unk72D2A4 &= -1 - script[1];
					*scriptIP += 2;

					break;

				// move to a point of a record with a constant speed
				case 10: {
					if (g_platformDescriptors[platformIndex].meshInstanceId != 0)
					{
						int32_t meshInstanceId = g_platformDescriptors[platformIndex].meshInstanceId;

						originX = Collision::g_collisionMeshInstances[meshInstanceId].origin.x;
						originY = Collision::g_collisionMeshInstances[meshInstanceId].origin.y;
						originZ = Collision::g_collisionMeshInstances[meshInstanceId].origin.z;
					}

					int32_t posY = originY >> 5;
					target.x = Levels::g_recordData[script[1]]->data[script[2]].x;
					int32_t posX = originX >> 5;
					target.y = Levels::g_recordData[script[1]]->data[script[2]].y;
					target.z = Levels::g_recordData[script[1]]->data[script[2]].z;
					originZ >>= 5;

					Nu3D::Link::SetPositionRawAndCommit(linkId, posX, posY, originZ);

					int32_t arriveDistance = (*scriptIP)[3] + 8;

					if (abs(posX - target.x) >= arriveDistance || abs(posY - target.y) >= arriveDistance || abs(originZ - target.z) >= arriveDistance)
					{
						target.x -= posX;
						target.y -= posY;
						target.z -= originZ;
						Nu3D::Math::NormalizeToFixedPoint(&target, &target);

						int16_t* curScript = *scriptIP;
						int32_t velX = (target.x * (int32_t)Renderer::g_frameDelta * (*scriptIP)[3]) >> 7;
						target.x = velX;
						int32_t velY = (target.y * (int32_t)Renderer::g_frameDelta * curScript[3]) >> 7;
						target.y = velY;
						int32_t velZ = (target.z * (int32_t)Renderer::g_frameDelta * curScript[3]) >> 7;

						if (g_platformDescriptors[platformIndex].meshInstanceId != 0)
						{
							if (velX != 0 || velY != 0 || velZ != 0)
							{
								g_platformDescriptors[platformIndex].flags |= 0x80;
								g_platformDescriptors[platformIndex].linVel.x = velX;
								g_platformDescriptors[platformIndex].linVel.y = velY;
								g_platformDescriptors[platformIndex].linVel.z = velZ;
							}
							else
							{
								g_platformDescriptors[platformIndex].flags &= ~0x80;
								g_platformDescriptors[platformIndex].linVel.x = 0;
								g_platformDescriptors[platformIndex].linVel.y = 0;
								g_platformDescriptors[platformIndex].linVel.z = 0;
							}
						}
					}
					else
					{
						*timer = 0;
						*scriptIP += 4;

						if (g_platformDescriptors[platformIndex].meshInstanceId != 0)
						{
							g_platformDescriptors[platformIndex].flags &= ~0x80;
							g_platformDescriptors[platformIndex].linVel.x = 0;
							g_platformDescriptors[platformIndex].linVel.y = 0;
							g_platformDescriptors[platformIndex].linVel.z = 0;
						}
					}

					break;
				}

				default:
					return;
			}
		}

		// FUNCTION: TOY2 0x0048B640
		BOOL HadBuzzContactThisFrame(int32_t platformIdx) { return (g_platformDescriptors[platformIdx].flags & 0x12) != 0; }

		// FUNCTION: TOY2 0x0048E200
		void CopyVelocity(int32_t srcPlatform, int32_t dstPlatform)
		{
			if (g_platformDescriptors[srcPlatform].meshInstanceId)
			{
				if (g_platformDescriptors[dstPlatform].meshInstanceId)
				{
					g_platformDescriptors[dstPlatform].flags |= 0x80;
					g_platformDescriptors[dstPlatform].linVel.x = g_platformDescriptors[srcPlatform].linVel.x;
					g_platformDescriptors[dstPlatform].linVel.y = g_platformDescriptors[srcPlatform].linVel.y;
					g_platformDescriptors[dstPlatform].linVel.z = g_platformDescriptors[srcPlatform].linVel.z;
				}
			}
		}

		// FUNCTION: TOY2 0x0049EC00
		int32_t StepTiltPhysics(int32_t platformId, int32_t linkId, int32_t angVel, int32_t mode, int32_t minAngle, int32_t maxAngle, int32_t divisor)
		{
			Vector3I vec;
			int32_t newAngVel;
			int32_t frameDelta;

			if (g_unk559E80 == platformId)
				return 0;

			if (Toy2::g_buzzActor.collisionFlags && (GetFlags(platformId) & 3) == 2)
			{
				// Buzz stands on the platform, it tilts to his side
				GetRotationAngles(platformId, &vec);

				int32_t sinYaw = Numerics::g_sinCosLUT[((int16_t)vec.y - 2048) & 0xFFF] >> 2;
				int32_t cosYaw = Numerics::g_sinCosLUT[((int16_t)vec.y + 1024) & 0xFFF] >> 2;

				Nu3D::Link::GetCurrentPosFixed(linkId, &vec);

				int32_t distAcross =
					(cosYaw * ((Toy2::g_buzzActor.posAngles.pos.x - vec.x) >> 5) + sinYaw * ((Toy2::g_buzzActor.posAngles.pos.z - vec.z) >> 5)) / 4096;
				int32_t distance =
					(int32_t)sqrt((
						double)(distAcross * distAcross
								+ ((Toy2::g_buzzActor.posAngles.pos.y - vec.y) >> 5) * ((Toy2::g_buzzActor.posAngles.pos.y - vec.y) >> 5)))
					>> 5;
				int32_t force = distance + ((distance * Buzz::g_unk53C810) >> 9);

				frameDelta = Renderer::g_frameDelta;

				if (distAcross <= 0)
					newAngVel = frameDelta * force / -2;
				else
					newAngVel = frameDelta * force / 2;

				newAngVel += angVel;
			}
			else if (mode == 1)
			{
				frameDelta = Renderer::g_frameDelta;
				newAngVel = angVel + 4 * frameDelta;
			}
			else if (mode == 2 && (GetRotation(platformId, &vec), abs(vec.z) > 8))
			{
				frameDelta = Renderer::g_frameDelta;

				if (vec.z >= 0)
					newAngVel = -3 * frameDelta + angVel;
				else
					newAngVel = frameDelta + angVel + 2 * frameDelta;
			}
			else
			{
				newAngVel = angVel;
				frameDelta = Renderer::g_frameDelta;
			}

			// friction
			if (newAngVel > 0)
			{
				newAngVel -= 2 * frameDelta;

				if (newAngVel >= 0)
					goto LABEL_23;

				goto LABEL_22;
			}

			if (newAngVel < 0)
			{
				newAngVel += 2 * frameDelta;

				if (newAngVel > 0)
				{
				LABEL_22:
					newAngVel = 0;
				}
			}

		LABEL_23:
			GetRotation(platformId, &vec);

			int16_t oldAngle = vec.z;
			int32_t newAngle = newAngVel / divisor + vec.z;
			vec.z = newAngle;

			if (newAngle < 0)
			{
				// hit the end of the tilt, bounce back
				if (newAngle < 4 * minAngle)
				{
					newAngle = 4 * minAngle;
					vec.z = 4 * minAngle;
					newAngVel = abs(3 * newAngVel / 4);
				}
			}
			else if (newAngle > 4 * maxAngle)
			{
				newAngle = 4 * maxAngle;
				vec.z = 4 * maxAngle;
				newAngVel = -abs(3 * newAngVel / 4);
			}

			SetAngularVelocity(platformId, 0, 0, newAngle - oldAngle);
			CommitRotationToLink(platformId, linkId);

			return newAngVel;
		}

		// FUNCTION: TOY2 0x0042C200
		int32_t SamplePoint(int32_t pathIdx, int32_t position, Vector3I* out)
		{
			int32_t index = position / 4096;

			out->x = Levels::g_recordData[pathIdx]->data[index].x
				+ (position & 0xFFF) * (Levels::g_recordData[pathIdx]->data[index + 1].x - Levels::g_recordData[pathIdx]->data[index].x) / 4096;

			out->y = Levels::g_recordData[pathIdx]->data[index].y
				+ (position & 0xFFF) * (Levels::g_recordData[pathIdx]->data[index + 1].y - Levels::g_recordData[pathIdx]->data[index].y) / 4096;

			Levels::RecordData* record = Levels::g_recordData[pathIdx];
			int32_t z = record->data[position / 4096].z;
			int32_t result = z + (position & 0xFFF) * (record->data[position / 4096 + 1].z - z) / 4096;

			out->z = result;

			return result;
		}
	}
}

#include "Toy2/Platform.h"
#include "Toy2/Collision.h"
#include "Toy2/Toy2.h"
#include "Nu3D/Math.h"

namespace Toy2
{
	namespace Platform
	{
		// GLOBAL: TOY2 0x00559E80
		int32_t g_unk559E80 = -1;

		// GLOBAL: TOY2 0x00728720
		PlatformDescriptor g_platformDescriptors[32];

		// STUB: TOY2 0x0042C2B0
		void InitPathPlatform(int32_t a1, int32_t a2, int32_t a3, int32_t linkId, int32_t linkId2, int32_t a6, int32_t a7, int32_t y) {}

		// FUNCTION: TOY2 0x004878D0
		void DisableCollision(int32_t platformIdx)
		{
			int16_t meshInstanceId = g_platformDescriptors[platformIdx].meshInstanceId;

			if (meshInstanceId)
				Collision::g_collisionMeshInstances[meshInstanceId].typeFlags = 0;
		}

		// STUB: TOY2 0x00487900
		void SetVelocity(int32_t platformIndex, int32_t velX, int32_t velY, int32_t velZ) {}

		// STUB: TOY2 0x00487970
		void SetAngularVelocity(int32_t idx, int16_t x, int16_t y, int16_t z) {}

		// STUB: TOY2 0x004879C0
		void GetOrigin(int32_t platformindex, Vector3I* outOrigin) {}

		// STUB: TOY2 0x00487A20
		void GetRotation(int32_t platformIdx, Vector3I* vecOut) {}

		// STUB: TOY2 0x00488510
		void SetOrigin(int32_t platformIndex, int32_t x, int32_t y, int32_t z) {}

		// STUB: TOY2 0x00488580
		int32_t GetFlags(int32_t platformIdx) { return 0; }

		// STUB: TOY2 0x004885A0
		int32_t GetContactFace32(int32_t index) { return 0; }

		// STUB: TOY2 0x00488A60
		void AddFlags(int32_t platformIdx, int16_t flags) {}

		// STUB: TOY2 0x00488A80
		void ClearFlags(int32_t platformIdx, int16_t flags) {}

		// FUNCTION: TOY2 0x00488AA0
		void SetRotationAngles(int32_t platformIdx, int16_t x, int16_t y, int16_t z)
		{
			g_platformDescriptors[platformIdx].rotAngle.x = 4 * x;
			g_platformDescriptors[platformIdx].rotAngle.y = 4 * y;
			g_platformDescriptors[platformIdx].rotAngle.z = 4 * z;

			g_platformDescriptors[platformIdx].flags |= 8;
		}

		// STUB: TOY2 0x00488AF0
		void GetRotationAngles(int32_t platformIdx, Vector3I* vecOut) {}

		// STUB: TOY2 0x00488BD0
		void CommitRotationToLink(int32_t platformIdx, int32_t linkId) {}

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
				if (!a3)
				{
					platformIdx = Collision::g_unk7290A0[i];

					if ((g_platformDescriptors[platformIdx].flags & 0x80) != 0)
					{
						x = g_platformDescriptors[platformIdx].prevLinVel.x;
						meshInstanceId = g_platformDescriptors[platformIdx].meshInstanceId;

						x = a1 * x / (a1 - a2);
						y = a1 * g_platformDescriptors[platformIdx].prevLinVel.y;
						Collision::g_collisionMeshInstances[meshInstanceId].origin.x += x;
						Collision::g_collisionMeshInstances[meshInstanceId].origin.y = y / (a1 - a2) + Collision::g_collisionMeshInstances[meshInstanceId].origin.y;
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

				if (Collision::g_unk729110 < 0 || Toy2::g_buzzActor.unkWord15 || (standing = Collision::g_unk729110, (g_platformDescriptors[Collision::g_unk729110].flags & 0x10) == 0))
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

					if ((flags & 0x100) != 0 && (flags & 0x12) != 0 && !Toy2::g_buzzActor.unkWord15)
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

									localX = (Toy2::g_buzzActor.posAngles.pos.z - originZ) * rotation->m20 + (Toy2::g_buzzActor.posAngles.pos.y - originY) * rotation->m10
											 + (Toy2::g_buzzActor.posAngles.pos.x - originX) * rotation->m00;
									localY = (Toy2::g_buzzActor.posAngles.pos.z - originZ) * rotation->m21 + (Toy2::g_buzzActor.posAngles.pos.y - originY) * rotation->m11
											 + (Toy2::g_buzzActor.posAngles.pos.x - originX) * rotation->m01;
									localZ = (Toy2::g_buzzActor.posAngles.pos.z - originZ) * rotation->m22 + (Toy2::g_buzzActor.posAngles.pos.y - originY) * rotation->m12
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
				if (Collision::g_unk728718 != -1 && carried == -1 && !Toy2::g_buzzActor.unkWord15)
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

		// STUB: TOY2 0x0048ACC0
		void StepMotionScript(int32_t platformIndex, int32_t linkId, int32_t** scriptIP, int32_t** timer, int32_t** accelRamp) {}

		// STUB: TOY2 0x0048B640
		BOOL HadBuzzContactThisFrame(int32_t platformIdx) { return 0; }

		// STUB: TOY2 0x0048E200
		void CopyVelocity(int32_t srcPlatform, int32_t dstPlatform) {}

		// STUB: TOY2 0x0049EC00
		int32_t StepTiltPhysics(int32_t platformId, int32_t linkId, int32_t angVel, int32_t mode, int32_t minAngle, int32_t maxAngle, int32_t divisor)
		{
			return 0;
		}

		// STUB: TOY2 0x0042C200
		int32_t SamplePoint(int32_t a1, int32_t a2, uint32_t* a3) { return 0; }
	}
}

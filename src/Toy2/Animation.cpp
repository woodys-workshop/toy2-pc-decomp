#include "Toy2/Animation.h"
#include "Toy2/Toy2.h"
#include "Toy2/Buzz.h"
#include "Toy2/Collision.h"
#include "Nu3D/Creature.h"
#include "Nu3D/Math.h"
#include "NGNLoader/NGNLoader.h"
#include "AudioManager/AudioManager.h"
#include "CharacterLoader.h"
#include <string.h>
#include <directx6/ddraw.h>
#include <directx6/d3d.h>

namespace Toy2
{
	namespace Animation
	{
		// GLOBAL: TOY2 0x00B223C0
		Actor::Toy2Actor** g_animActorList;

		// GLOBAL: TOY2 0x00E4D98C
		int32_t g_unkE4D98C;

		// GLOBAL: TOY2 0x00E4D990
		int32_t g_unkE4D990;

		// GLOBAL: TOY2 0x00E4D9A4
		int32_t g_unkE4D9A4;

		// GLOBAL: TOY2 0x00508D00
		int32_t g_specialNodeValue = -1;

		// GLOBAL: TOY2 0x00B223E8
		D3DMATRIX g_localBoneMatrices[2048];

		// GLOBAL: TOY2 0x00B62400
		Nu3D::Creature* g_curAnimCreature;

		// GLOBAL: TOY2 0x00B223DC
		int32_t g_curActorAnimSlot;

		// GLOBAL: TOY2 0x00B1C3C0
		Vector3I g_nodeAngles[2048];

		// GLOBAL: TOY2 0x00B223E0
		int16_t* g_unkB223E0;

		// GLOBAL: TOY2 0x00B423E8
		uint8_t* g_unkB423E8;

		// GLOBAL: TOY2 0x00B423EC
		uint8_t* g_unkB423EC;

		// GLOBAL: TOY2 0x00546D70
		int16_t g_unk546D70;

		// GLOBAL: TOY2 0x0053E4B8
		int16_t g_unk53E4B8;

		// GLOBAL: TOY2 0x0053E4BA
		int16_t g_unk53E4BA;

		// GLOBAL: TOY2 0x0053E4BC
		int16_t g_unk53E4BC;

		// GLOBAL: TOY2 0x0053E4BE
		int16_t g_unk53E4BE;

		// FUNCTION: TOY2 0x004CD120
		void ResetNodeAngles()
		{
			memset(g_nodeAngles, 0, sizeof(g_nodeAngles));
			g_animActorList = 0;
		}

		// FUNCTION: TOY2 0x004CD140
		void GetRootWorldPos(Vector3F* out, int32_t index)
		{
			if (g_curAnimCreature != 0)
				Nu3D::Math::TransformPointByMatrix(out, out, &g_localBoneMatrices[index + 32 * g_curActorAnimSlot]);
		}

		// FUNCTION: TOY2 0x004CD170
		void ParseHeader(int16_t* animData)
		{
			int32_t headerSize;

			if (*animData >= 0)
				headerSize = 12;
			else
				headerSize = -*animData;

			g_unkB223E0 = (int16_t*)((uint8_t*)animData + headerSize);
			g_unkB423E8 = (uint8_t*)animData + headerSize + 2 * animData[5];
			g_unkB423EC = g_unkB423E8 + animData[6];
		}

		// FUNCTION: TOY2 0x004CD1B0
		int32_t SampleNodeTransform(int32_t curNode, uint16_t* animData, uint32_t unk, D3DMATRIX* matrix)
		{
			int32_t frac = unk & 0xFFFF;
			float fracScale = (float)frac * 0.000015259022f;
			int32_t frame = unk >> 16;

			if ((uint32_t)frame > (uint32_t)(animData[2] - 2))
				frame = animData[2] - 2;

			int32_t nextFrame = frame + 1;

			if (frame >= animData[2] - 2)
				nextFrame = frame;

			int32_t stride = animData[4];
			int16_t* frameA = (int16_t*)(g_unkB423EC + 2 * frame * stride);
			int32_t boneOffset = g_unkB223E0[curNode];

			if (boneOffset == -3 || boneOffset == -2 || boneOffset == -1)
				return 0;

			int16_t* keyA = frameA + boneOffset;
			int16_t* keyB = (int16_t*)g_unkB423EC + (boneOffset + nextFrame * stride);

			Vector3F offset;
			Vector3F scale;
			D3DMATRIX rotA;
			D3DMATRIX rotB;
			int32_t pitchA, yawA, rollA;
			int32_t pitchB, yawB, rollB;
			int32_t packed;

			if (frac)
			{
				int32_t a = keyA[0] >> 2;
				offset.x = (float)(a + ((frac * ((keyB[0] >> 2) - a)) >> 16));

				a = keyA[1] >> 2;
				offset.y = (float)(a + ((frac * ((keyB[1] >> 2) - a)) >> 16));

				a = keyA[2] >> 2;
				offset.z = (float)(a + ((frac * ((keyB[2] >> 2) - a)) >> 16));

				packed = (uint16_t)keyA[3] + ((uint16_t)keyA[4] << 16);
				pitchA = ((packed >> 14) & 0xFFC0) | (16 * (keyA[0] & 3));
				yawA = ((packed >> 4) & 0xFFC0) | (16 * (keyA[1] & 3));
				rollA = 16 * ((4 * (packed & 0x3FF)) | (keyA[2] & 3));

				packed = (uint16_t)keyB[3] + ((uint16_t)keyB[4] << 16);
				pitchB = ((packed >> 14) & 0xFFC0) | (16 * (keyB[0] & 3));
				yawB = ((packed >> 4) & 0xFFC0) | (16 * (keyB[1] & 3));
				rollB = 16 * ((4 * (packed & 0x3FF)) | (keyB[2] & 3));

				Nu3D::Math::BuildIdentityMatrix(&rotA);
				Nu3D::Math::MatrixRotatePitch(&rotA, pitchA);
				Nu3D::Math::MatrixRotateYaw(&rotA, yawA);
				Nu3D::Math::MatrixRotateRoll(&rotA, rollA);

				Nu3D::Math::BuildIdentityMatrix(&rotB);
				Nu3D::Math::MatrixRotatePitch(&rotB, pitchB);
				Nu3D::Math::MatrixRotateYaw(&rotB, yawB);
				Nu3D::Math::MatrixRotateRoll(&rotB, rollB);

				rotA._11 = (rotB._11 - rotA._11) * fracScale + rotA._11;
				rotA._12 = (rotB._12 - rotA._12) * fracScale + rotA._12;
				rotA._13 = (rotB._13 - rotA._13) * fracScale + rotA._13;
				rotA._21 = (rotB._21 - rotA._21) * fracScale + rotA._21;
				rotA._22 = (rotB._22 - rotA._22) * fracScale + rotA._22;
				rotA._23 = (rotB._23 - rotA._23) * fracScale + rotA._23;
				rotA._31 = (rotB._31 - rotA._31) * fracScale + rotA._31;
				rotA._32 = (rotB._32 - rotA._32) * fracScale + rotA._32;
				rotA._33 = (rotB._33 - rotA._33) * fracScale + rotA._33;
			}
			else
			{
				offset.x = (float)(keyA[0] >> 2);
				offset.y = (float)(keyA[1] >> 2);
				offset.z = (float)(keyA[2] >> 2);

				// NOTE: the original takes the low two bits of the angles from the next key here
				packed = (uint16_t)keyA[3] + ((uint16_t)keyA[4] << 16);
				pitchB = ((packed >> 14) & 0xFFC0) | (16 * (keyB[0] & 3));
				yawB = ((packed >> 4) & 0xFFC0) | (16 * (keyB[1] & 3));
				rollB = 16 * ((4 * (packed & 0x3FF)) | (keyB[2] & 3));

				Nu3D::Math::BuildIdentityMatrix(&rotA);
				Nu3D::Math::MatrixRotatePitch(&rotA, pitchB);
				Nu3D::Math::MatrixRotateYaw(&rotA, yawB);
				Nu3D::Math::MatrixRotateRoll(&rotA, rollB);
			}

			if (((1 << (curNode % 8)) & g_unkB423E8[curNode / 8]) != 0)
			{
				if (frac)
				{
					int32_t a = keyA[5];
					scale.x = (float)(a + ((frac * (keyB[5] - a)) >> 16));

					a = keyA[6];
					scale.y = (float)(a + ((frac * (keyB[6] - a)) >> 16));

					a = keyA[7];
					int32_t z = a + ((frac * (keyB[7] - a)) >> 16);

					scale.x = scale.x * 0.00024414062f;
					scale.y = scale.y * 0.00024414062f;
					scale.z = (float)(z * 0.00024414062);
				}
				else
				{
					scale.x = (float)(keyA[5] * 0.00024414062);
					scale.y = (float)(keyA[6] * 0.00024414062);
					scale.z = (float)(keyA[7] * 0.00024414062);
				}
			}
			else
			{
				scale.x = 1.0f;
				scale.y = 1.0f;
				scale.z = 1.0f;
			}

			int32_t rootNode = (int16_t)animData[7];

			if (rootNode == curNode)
				g_unkE4D9A4 = rootNode;
			else
				rootNode = g_unkE4D9A4;

			if (g_unkE4D990)
			{
				if (rootNode)
				{
					Vector3F rootPos;
					rootPos.x = 0.0f;
					rootPos.y = -200.0f;
					rootPos.z = 0.0f;

					GetRootWorldPos(&rootPos, rootNode);

					offset.x = rootPos.x + offset.x;
					offset.y = rootPos.y + offset.y + 200.0f;
					offset.z = rootPos.z + offset.z;
				}
			}

			Nu3D::Math::MatrixApplyScale(matrix, &scale);
			Nu3D::Math::MultiplyMatrix3x4(matrix, matrix, &rotA);
			Nu3D::Math::AddWorldSpaceTransform(matrix, &offset);

			return 1;
		}

		// FUNCTION: TOY2 0x004CD7B0
		void EvaluateClipToMatrices(int32_t index, Nu3D::Creature* creature, D3DMATRIX* targetMatrix, int16_t* animData, uint32_t unk, int32_t flag)
		{
			int32_t curNode = 0;
			D3DMATRIX localMatrix;

			if (animData)
			{
				if (flag)
				{
					g_unkE4D9A4 = 0;
					g_unkE4D990 = 0;
				}
				else
				{
					g_unkE4D990 = g_unkE4D98C;
				}

				ParseHeader(animData);

				if (creature->nodeCount > 0)
				{
					do
					{
						Nu3D::Math::BuildIdentityMatrix(&localMatrix);

						if (curNode == g_specialNodeValue
							|| SampleNodeTransform(curNode, (uint16_t*)animData, unk, &localMatrix)
							|| flag)
						{
							int32_t matrixIdx = curNode + 32 * index;

							memcpy(&g_localBoneMatrices[matrixIdx], &localMatrix, sizeof(D3DMATRIX));
							Nu3D::Math::MultiplyMatrix3x4(&Nu3D::g_nodeMatrices[0].matrices[matrixIdx], &localMatrix, targetMatrix);
						}

						++curNode;
					}
					while (curNode < creature->nodeCount);
				}
			}
		}

		// FUNCTION: TOY2 0x004CD880
		void AnimateActors(Actor::Toy2Actor** actorList)
		{
			Actor::Toy2Actor** list;
			Actor::Toy2Actor** actorNext;
			Actor::Toy2Actor* selectedActor;
			Nu3D::Creature* creature;
			Vector3F offset;
			Vector3I* nodeAngle;
			Nu3D::NodeMatricesInner* nodeMatrices;
			float v22;
			int32_t slot;
			D3DMATRIX matrix1;
			D3DMATRIX matrix2;

			list = actorList;
			g_animActorList = actorList;

			slot = 0;

			if (NGNLoader::g_ngnImage->creatureCount && *actorList)
			{
				actorNext = actorList;

				nodeAngle = &g_nodeAngles[0];
				nodeMatrices = Nu3D::g_nodeMatrices;

				while (1)
				{
					g_unkE4D98C = actorNext[1] == 0;
					selectedActor = *list;
					creature = NGNLoader::g_ngnImage->creatureData[(*list)->creatureId];

					if (creature)
					{
						g_curAnimCreature = NGNLoader::g_ngnImage->creatureData[(*list)->creatureId];
						g_curActorAnimSlot = slot;

						Nu3D::Math::BuildIdentityMatrix(&matrix1);
						Nu3D::Math::BuildIdentityMatrix(&matrix2);

						v22 = 0.0f;
						offset.x = (float)(selectedActor->unkVar12 * 0.00024414062);
						offset.y = (float)(selectedActor->unkVar12_ * 0.00024414062);
						offset.z = (float)(selectedActor->unkVar13 * 0.00024414062);

						int16_t unkVar13_ = selectedActor->unkVar13_;
						bool applyScale1 = false;

						if (unkVar13_)
						{
							if ((uint16_t)unkVar13_ == 0x8000)
							{
								Nu3D::Math::MatrixApplyScale(&matrix2, &offset);
								applyScale1 = true;
							}
							else if ((unkVar13_ & 1) != 0)
							{
								v22 = (float)((1.0f - offset.y) * unkVar13_);
								applyScale1 = true;
							}

							if (applyScale1)
								Nu3D::Math::MatrixApplyScale(&matrix1, &offset);
						}

						Nu3D::Math::MatrixRotatePitch(&matrix1, 16 * selectedActor->pitchAngle);
						Nu3D::Math::MatrixRotateYaw(&matrix1, 16 * (selectedActor->yawAngle + 2048));
						Nu3D::Math::MatrixRotateRoll(&matrix1, 16 * selectedActor->rollAngle);

						Nu3D::Math::MatrixRotatePitch(&matrix2, 16 * selectedActor->pitchAngle);
						Nu3D::Math::MatrixRotateYaw(&matrix2, 16 * (selectedActor->yawAngle + 2048));
						Nu3D::Math::MatrixRotateRoll(&matrix2, 16 * selectedActor->rollAngle);

						offset.x = (float)(selectedActor->pos.x * 0.03125);
						offset.y = (float)(selectedActor->pos.y * 0.03125);
						offset.z = (float)(selectedActor->pos.z * 0.03125);
						Nu3D::Math::AddWorldSpaceTransform(&matrix2, &offset);
						offset.y = v22 + offset.y;
						Nu3D::Math::AddWorldSpaceTransform(&matrix1, &offset);

						if (selectedActor->secondaryAnimIdx != -1)
							EvaluateClipToMatrices(
								slot,
								creature,
								&matrix1,
								(int16_t*)creature->animData[selectedActor->secondaryAnimIdx],
								selectedActor->unkVar8,
								1);

						if (selectedActor->primaryAnimIdx != -1)
							EvaluateClipToMatrices(
								slot,
								creature,
								&matrix2,
								(int16_t*)creature->animData[selectedActor->primaryAnimIdx],
								selectedActor->unkVar7,
								0);

						int32_t nodeIdx = 0;

						if (creature->nodeCount > 0)
						{
							D3DMATRIX* curMatrix = nodeMatrices->matrices;
							Vector3I* curAngle = nodeAngle;

							do
							{
								if (curAngle->z)
								{
									Nu3D::Math::MatrixRotateRoll(curMatrix, 16 * curAngle->z);
									curAngle->z = 0;
								}

								if (curAngle->y)
								{
									Nu3D::Math::MatrixRotateYaw(curMatrix, 16 * curAngle->y);
									curAngle->y = 0;
								}

								if (curAngle->x)
								{
									Nu3D::Math::MatrixRotatePitch(curMatrix, 16 * curAngle->x);
									curAngle->x = 0;
								}

								++nodeIdx;
								++curMatrix;
								++curAngle;
							}
							while (nodeIdx < creature->nodeCount);
						}
					}

					++actorNext;

					g_curAnimCreature = 0;
					++slot;

					++nodeMatrices;
					nodeAngle += 32;

					list = actorNext;

					if (*actorNext == 0)
						return;
				}
			}
		}

		// FUNCTION: TOY2 0x0043C0E0
		void TransformByBone(Vector3I* point, Toy2::Actor::Toy2Actor* actor, int32_t boneIndex)
		{
			int32_t x = point->x;
			int32_t y = point->y;
			int32_t z = point->z;
			CharacterLoader::BoneTransform* bone =
				&CharacterLoader::g_boneTransforms[boneIndex + *(int16_t*)(CharacterLoader::g_unk547CD4[actor->creatureId] + 4)];

			int32_t newX = (bone->rotation.m00 * x + bone->rotation.m01 * y + bone->rotation.m02 * z) / 4096;
			int32_t newY = (bone->rotation.m10 * x + bone->rotation.m11 * y + bone->rotation.m12 * z) / 4096;
			int32_t newZ = (bone->rotation.m20 * x + bone->rotation.m21 * y + bone->rotation.m22 * z) / 4096;

			point->x = bone->translation.x + newX;
			point->y = bone->translation.y + newY;
			point->z = bone->translation.z + newZ;
		}

		// FUNCTION: TOY2 0x0043BA80
		void EvaluateClip(int16_t* clipHeader, int32_t time, int32_t baseBone, uint8_t track)
		{
			int32_t headerSize;

			if (*clipHeader >= 0)
			{
				headerSize = 12;
				CharacterLoader::g_unk53E4C0 = 0;
				CharacterLoader::g_unk53EED8 = 12;
			}
			else
			{
				headerSize = -*clipHeader;
				CharacterLoader::g_unk53EED8 = (int16_t)headerSize;
				CharacterLoader::g_unk53E4C0 = 1;
			}

			CharacterLoader::g_unk53EEC8 = (uint8_t*)clipHeader + headerSize;
			CharacterLoader::g_unk53E4C4 = (uint8_t*)clipHeader + headerSize + 2 * (uint16_t)clipHeader[5];

			int32_t frame = time >> 16;
			int32_t frac = time & 0xFFFF;

			if (frame <= (clipHeader[2] & 0x7FFF))
			{
				uint8_t* frameBase = (uint8_t*)clipHeader + headerSize + 2 * (uint16_t)clipHeader[5] + (uint16_t)clipHeader[6];
				uint8_t* frameA = frameBase + 2 * frame * (uint16_t)clipHeader[4];
				int32_t boneSlot = CharacterLoader::g_unk53EECC;
				int32_t boneEnd;
				int32_t curBone;

				if (CharacterLoader::g_unk53EECC < 0)
				{
					curBone = 0;
					boneSlot = 0;
					boneEnd = (uint16_t)clipHeader[3];
				}
				else
				{
					curBone = CharacterLoader::g_unk53EECC;
					boneEnd = CharacterLoader::g_unk53EECC + 1;
					baseBone += CharacterLoader::g_unk53EECC;
				}

				for (; boneSlot < boneEnd; curBone = boneSlot)
				{
					int32_t boneValue = *((int16_t*)CharacterLoader::g_unk53EEC8 + boneSlot);

					if (boneValue == -3)
					{
						++baseBone;
					}
					else if (boneValue != -2 && boneValue != -1)
					{
						CharacterLoader::BoneTransform* boneTransform = &CharacterLoader::g_boneTransforms[(int16_t)baseBone];

						boneTransform->track = track;

						int16_t* keyA = (int16_t*)(frameA + 2 * boneValue);
						int16_t* keyB;

						if (frame >= (clipHeader[2] & 0x7FFF) - 1)
							keyB = (int16_t*)(frameBase + 2 * boneValue);
						else
							keyB = keyA + (uint16_t)clipHeader[4];

						if ((int16_t)time)
						{
							int32_t a = keyA[0] >> 2;
							boneTransform->translation.x = a + ((frac * ((keyB[0] >> 2) - a)) >> 16);

							a = keyA[1] >> 2;
							boneTransform->translation.y = a + ((frac * ((keyB[1] >> 2) - a)) >> 16);

							a = keyA[2] >> 2;
							boneTransform->translation.z = a + ((frac * ((keyB[2] >> 2) - a)) >> 16);

							AnglesWithRoll* anglesA = (AnglesWithRoll*)&Collision::g_mathScratch[237 * 4];
							AnglesWithRoll* anglesB = (AnglesWithRoll*)&Collision::g_mathScratch[245 * 4];
							Matrix3x3I16* matA = (Matrix3x3I16*)anglesA;
							Matrix3x3I16* matB = (Matrix3x3I16*)anglesB;

							int32_t packed = (uint16_t)keyA[3] + ((uint16_t)keyA[4] << 16);
							anglesA->angles.pitch = (keyA[0] & 3) | ((packed >> 18) & 0xFFC);
							anglesA->angles.yaw = (keyA[1] & 3) | ((packed >> 8) & 0xFFC);
							anglesA->roll = (keyA[2] & 3) | (4 * (packed & 0x3FF));
							Nu3D::Math::EulerToRotationMatrix(anglesA, matA);

							packed = (uint16_t)keyB[3] + ((uint16_t)keyB[4] << 16);
							anglesB->angles.pitch = (keyB[0] & 3) | ((packed >> 18) & 0xFFC);
							anglesB->angles.yaw = (keyB[1] & 3) | ((packed >> 8) & 0xFFC);
							anglesB->roll = (keyB[2] & 3) | (4 * (packed & 0x3FF));
							Nu3D::Math::EulerToRotationMatrix(anglesB, matB);

							boneTransform->rotation.m00 = matA->m00 + ((frac * (matB->m00 - matA->m00)) >> 16);
							boneTransform->rotation.m01 = matA->m01 + ((frac * (matB->m01 - matA->m01)) >> 16);
							boneTransform->rotation.m02 = matA->m02 + ((frac * (matB->m02 - matA->m02)) >> 16);

							boneTransform->rotation.m10 = matA->m10 + ((frac * (matB->m10 - matA->m10)) >> 16);
							boneTransform->rotation.m11 = matA->m11 + ((frac * (matB->m11 - matA->m11)) >> 16);
							boneTransform->rotation.m12 = matA->m12 + ((frac * (matB->m12 - matA->m12)) >> 16);

							boneTransform->rotation.m20 = matA->m20 + ((frac * (matB->m20 - matA->m20)) >> 16);
							boneTransform->rotation.m21 = matA->m21 + ((frac * (matB->m21 - matA->m21)) >> 16);
							boneTransform->rotation.m22 = matA->m22 + ((frac * (matB->m22 - matA->m22)) >> 16);
						}
						else
						{
							boneTransform->translation.x = keyA[0] >> 2;
							boneTransform->translation.y = keyA[1] >> 2;
							boneTransform->translation.z = keyA[2] >> 2;

							int32_t packed = (uint16_t)keyA[3] + ((uint16_t)keyA[4] << 16);

							boneTransform->unkVec4.x = (keyA[0] & 3) | ((packed >> 18) & 0xFFC);
							boneTransform->unkVec4.y = (keyA[1] & 3) | ((packed >> 8) & 0xFFC);
							boneTransform->unkVec4.z = (keyA[2] & 3) | (4 * (packed & 0x3FF));

							Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&boneTransform->unkVec4, &boneTransform->rotation);
						}

						if (CharacterLoader::g_unk53E4C0 && clipHeader[7] == (int16_t)baseBone)
						{
							Vector3I point;

							point.x = 0;
							point.z = 0;
							point.y = -200;

							TransformByBone(&point, (Actor::Toy2Actor*)&g_buzzActor, baseBone);

							g_unk53E4BC = (int16_t)point.z;
							g_unk53E4B8 = (int16_t)point.x;
							g_unk53E4BA = (int16_t)point.y + 200;
							g_unk53E4BE = 1;
						}

						if (g_unk546D70 && g_unk53E4BE)
						{
							boneTransform->translation.x += g_unk53E4B8;
							boneTransform->translation.y = g_unk53E4BA + boneTransform->translation.y;
							boneTransform->translation.z = g_unk53E4BC + boneTransform->translation.z;
						}

						if (((1 << (curBone % 8)) & CharacterLoader::g_unk53E4C4[curBone / 8]) != 0)
						{
							if ((int16_t)time)
							{
								boneTransform->unkInt7 = keyA[5] + ((frac * (keyB[5] - keyA[5])) >> 16);
								boneTransform->unkInt7_ = keyA[6] + ((frac * (keyB[6] - keyA[6])) >> 16);

								int16_t scaleZ = keyA[7];

								boneTransform->unkByte1 = 1;

								boneTransform->unkInt8 = scaleZ + ((frac * (keyB[7] - scaleZ)) >> 16);
							}
							else
							{
								boneTransform->unkInt7 = keyA[5];
								boneTransform->unkInt7_ = keyA[6];
								boneTransform->unkInt8 = keyA[7];

								boneTransform->unkByte1 = 1;
							}
						}
						else
						{
							boneTransform->unkByte1 = 0;
						}

						boneSlot = curBone;

						++baseBone;
					}

					++boneSlot;
				}

				if (g_unk546D70)
				{
					if (g_unk53E4BE)
						g_unk53E4BE = 0;
				}

				CharacterLoader::g_unk53EECC = -1;
			}
		}

		// FUNCTION: TOY2 0x00401000
		uint32_t StepEventTrack(uint8_t* animStream)
		{
			int32_t unkVar33 = g_buzzActor.unkVar33;
			uint8_t* cur = &animStream[g_buzzActor.unkVar33 >> 16];

			if (cur[1] == 0xFF && cur[2] == 1)
			{
				unkVar33 = g_buzzActor.unkVar33 & 0xFFFF0000;
				g_buzzActor.unkVar33 &= 0xFFFF0000;
			}

			cur = &animStream[unkVar33 >> 16];
			uint32_t result = *cur;

			if (result >= 0x80)
			{
				while (1)
				{
					switch (result)
					{
					case 0x81:
						if (g_buzzActor.unkShort40)
							Buzz::g_unk53C634 |= 1;
						break;

					case 0x82:
						if (g_buzzActor.unkShort40)
							Buzz::g_unk53C634 |= 2;
						break;

					case 0x83:
						AudioManager::PlaySoundEffect(48, &g_buzzActor.posAngles.pos);
						break;

					case 0x84:
						AudioManager::PlaySoundEffect(16, &g_buzzActor.posAngles.pos);
						break;

					case 0x85:
						AudioManager::PlaySoundEffect(23, &g_buzzActor.posAngles.pos);
						break;

					case 0x86:
						AudioManager::PlaySoundEffect(67, &g_buzzActor.posAngles.pos);
						break;

					case 0xFE:
						result = (uint32_t)-1;
						g_buzzActor.unkVar37 = 0;
						g_buzzActor.unkVar38 = -1;
						Buzz::g_unk50A098 = 0;
						return result;

					case 0xFF:
						g_buzzActor.unkVar33 = -65536 - (cur[1] << 16) + unkVar33;
						break;

					default:
						break;
					}

					g_buzzActor.unkVar33 += 0x10000;
					unkVar33 = g_buzzActor.unkVar33;
					result = animStream[g_buzzActor.unkVar33 >> 16];
					cur = &animStream[g_buzzActor.unkVar33 >> 16];

					if (result < 0x80)
						return result;
				}
			}

			return result;
		}
	}
}

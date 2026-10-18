#include "Toy2/Levels/AlsToyBarn.h"
#include "Toy2/Toy2.h"
#include "Toy2/Actor.h"
#include "Toy2/Buzz.h"
#include "Toy2/Camera.h"
#include "Toy2/Collectables.h"
#include "Toy2/Dialogue.h"
#include "Toy2/Gadget.h"
#include "Toy2/Level.h"
#include "Toy2/Levels.h"
#include "Toy2/Lighting.h"
#include "Toy2/MoveableObject.h"
#include "Toy2/Platform.h"
#include "Toy2/Particles.h"
#include "Toy2/Weather.h"
#include "Toy2/Cutscene.h"
#include "Toy2/Collision.h"
#include "Nu3D/Link.h"
#include "Nu3D/Math.h"
#include "Nu3D/Particles.h"
#include "Nu3D/Collision.h"
#include "Nu3D/Camera.h"
#include "Renderer/Renderer.h"
#include "Renderer/Sprite.h"
#include "Renderer/LensFlare.h"
#include "Renderer/Effects.h"
#include "AudioManager/AudioManager.h"
#include "InputManager.h"
#include "Random.h"
#include "Numerics.h"
#include "SaveManager.h"
#include "Nullsub.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

namespace Toy2
{
	namespace AlsToyBarn
	{
		// GLOBAL: TOY2 0x004F2A34
		char* g_unk4F2A34[5] = {
			"i saw ^hamm^ on the table in the ^office^.",
			"the ^chicken^ has lost her ^chicks^. she is just past the ^bike^ section. buzz, one of the ^chicks^ is on the top of the boxes in this room! "
			"you'll need the ^hover boots^ to reach it and ^mr. potato head^ won't give you ^hover boots^ until later.",
			"the ^rooster^ is at the end of the ^hall^ by the office. he has some ^challenges^ for you!",
			"there is a ^token^ on top of the cabinets in the ^office^. you need to open up the ^locked^ drawers to get to it. try using the ^disk launcher^! "
			"if you haven't unlocked it yet, go see ^mr. potato head^ in the ^construction yard^ level.",
			"the ^dinosaur^ boss is through a tunnel to the ^left^ of here."
		};

		// GLOBAL: TOY2 0x0052F9C4
		int32_t g_unk52F9C4;

		// GLOBAL: TOY2 0x0052F9C8
		int32_t g_unk52F9C8;

		// GLOBAL: TOY2 0x0052F9CC
		int32_t g_unk52F9CC;

		// GLOBAL: TOY2 0x0052F9D0
		int32_t g_unk52F9D0;

		// GLOBAL: TOY2 0x0052F9D4
		int32_t g_unk52F9D4;

		// GLOBAL: TOY2 0x0052F9D8
		int32_t g_unk52F9D8;

		// GLOBAL: TOY2 0x004F294C
		int32_t g_unk4F294C[9] = { 1, 63636, 196616, 6291583, 1, 0, 196616, 2097279, 1048576 };

		// GLOBAL: TOY2 0x004F2970
		int32_t g_unk4F2970[9] = { 1, 62836, 196616, 6291583, 1, 0, 196616, 2097279, 1048576 };

		// GLOBAL: TOY2 0x004F2994
		int32_t g_unk4F2994[9] = { 1, 62836, 196616, 6291583, 1, 0, 196616, 2097279, 1048576 };

		// GLOBAL: TOY2 0x004F29B8
		int32_t g_unk4F29B8[9] = { 51773441, 0, 196620, 2097407, 1, 0, 196620, 2097407, 1048576 };

		// GLOBAL: TOY2 0x004F29DC
		int32_t g_unk4F29DC[9] = { 51773441, 0, 196620, 2097407, 1, 0, 196620, 2097407, 1048576 };

		// GLOBAL: TOY2 0x004F2A00
		int32_t g_unk4F2A00[9] = { 51773441, 0, 196620, 2097407, 1, 0, 196620, 2097407, 1048576 };

		// GLOBAL: TOY2 0x004F2A24
		int16_t g_unk4F2A24[8] = { 8, 5, 0, 9, 6, 1, -1, 0 };

		// GLOBAL: TOY2 0x004F2A48
		int16_t g_unk4F2A48[6] = { 49, 51, 52, 50, 48, 0 };

		// GLOBAL: TOY2 0x004F2A54
		Collectables::TokenTableEntry g_unk4F2A54[2] = {
			{ 70,
			  20,
			  "the ^hay bale ride^ will buck you over to the ^chick^ on the gumball machine but it will stop if you leave the area. unlock the ^rocket boots^ "
			  "and jet over to the shopping cart to solve this puzzle.",
			  1024 },
			{ -1, 0, 0, 0 },
		};

		// GLOBAL: TOY2 0x0052F9E4
		int32_t g_unk52F9E4;

		// GLOBAL: TOY2 0x0052FA24
		int32_t g_unk52FA24;

		// GLOBAL: TOY2 0x0052FA30
		int32_t g_unk52FA30;

		// GLOBAL: TOY2 0x0052FA28
		int32_t g_unk52FA28;

		// GLOBAL: TOY2 0x0052FA0C
		int32_t g_unk52FA0C;

		// GLOBAL: TOY2 0x0052F9DC
		int32_t g_unk52F9DC;

		// GLOBAL: TOY2 0x0052F9E0
		int32_t g_unk52F9E0;

		// GLOBAL: TOY2 0x0052FA18
		int32_t g_unk52FA18;

		// GLOBAL: TOY2 0x0052F9C0
		int32_t g_unk52F9C0;

		// GLOBAL: TOY2 0x0052F9EC
		int32_t g_unk52F9EC;

		// GLOBAL: TOY2 0x0052F9E8
		int32_t g_unk52F9E8;

		// GLOBAL: TOY2 0x0052F9BC
		int32_t g_unk52F9BC;

		// GLOBAL: TOY2 0x0052FA4C
		int32_t g_unk52FA4C;

		// GLOBAL: TOY2 0x0052FA08
		int32_t g_unk52FA08;

		// GLOBAL: TOY2 0x0052FA1C
		int32_t g_unk52FA1C;

		// GLOBAL: TOY2 0x0052FA10
		int32_t g_unk52FA10;

		// GLOBAL: TOY2 0x0052FA44
		int32_t g_unk52FA44;

		// GLOBAL: TOY2 0x0052FA48
		int32_t g_unk52FA48;

		// GLOBAL: TOY2 0x0052FA3C
		int32_t g_unk52FA3C;

		// GLOBAL: TOY2 0x0052FA40
		int32_t g_unk52FA40;

		// GLOBAL: TOY2 0x0052FA34
		int32_t g_unk52FA34;

		// GLOBAL: TOY2 0x0052FA38
		int32_t g_unk52FA38;

		// GLOBAL: TOY2 0x0052FA00
		int32_t* g_unk52FA00;

		// GLOBAL: TOY2 0x0052FA04
		int32_t* g_unk52FA04;

		// GLOBAL: TOY2 0x0052F9F8
		int32_t* g_unk52F9F8;

		// GLOBAL: TOY2 0x0052F9FC
		int32_t* g_unk52F9FC;

		// GLOBAL: TOY2 0x0052F9F0
		int32_t* g_unk52F9F0;

		// GLOBAL: TOY2 0x0052F9F4
		int32_t* g_unk52F9F4;

		// GLOBAL: TOY2 0x0052FA14
		int32_t g_unk52FA14;

		// GLOBAL: TOY2 0x0052FA20
		int32_t g_unk52FA20;

		// GLOBAL: TOY2 0x0052FA2C
		int32_t g_unk52FA2C;

		// GLOBAL: TOY2 0x0052FA50
		int32_t g_unk52FA50;

		// FUNCTION: TOY2 0x00420F70
		void Method1(int32_t actorIndex, int32_t objectId)
		{
			int32_t y;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			int32_t v6;
			int32_t v7;

			if ((g_levelActors[actorIndex].actorFlags & 2) != 0)
			{
				y = g_levelActors[actorIndex].transform.pos.y;

				if (y > MoveableObject::g_moveableObjects[1].pos.y - 36864)
				{
					v3 = (MoveableObject::g_moveableObjects[objectId].pos.x - g_levelActors[actorIndex].transform.pos.x) >> 5;
					v4 = (MoveableObject::g_moveableObjects[objectId].pos.z - g_levelActors[actorIndex].transform.pos.z) >> 5;

					if (v3 * v3 + v4 * v4 < 490000)
					{
						v5 = MoveableObject::g_moveableObjects[objectId].pos.y;
						if (y >= v5 - 34816)
						{
							v6 = Nu3D::Math::CartesianToFixedAngle(
									 v3, (MoveableObject::g_moveableObjects[objectId].pos.z - g_levelActors[actorIndex].transform.pos.z) >> 5)
								& 0xFFF;

							v7 = Numerics::g_sinCosLUT[(v6 + 1024) & 0xFFF];

							g_levelActors[actorIndex].transform.pos.x =
								MoveableObject::g_moveableObjects[objectId].pos.x - ((700 * Numerics::g_sinCosLUT[v6]) >> 9);
							g_levelActors[actorIndex].transform.pos.z = MoveableObject::g_moveableObjects[objectId].pos.z - ((700 * v7) >> 9);
						}
						else
						{
							g_levelActors[actorIndex].transform.pos.y = v5 - 36864;
						}
					}
				}
			}
		}

		// FUNCTION: TOY2 0x00421340
		void Interactions()
		{
			int32_t v0;
			int32_t v1;
			int32_t v2;
			int32_t v3;
			bool v4;
			int32_t v5;
			bool v6;
			int32_t v7;
			int32_t v8;
			bool v9;
			int32_t v10;
			bool v11;
			int32_t v12;
			int32_t v13;
			Nu3D::Particles::ParticleInstance* v14;
			int32_t v15;
			int32_t x;
			int32_t v17;
			int32_t z;
			char actorFlags;
			int32_t v20;
			int32_t v21;
			Vector3I a2;
			Vector3I v23;

			v0 = g_unk52FA24;

			if (g_unk52FA24 > 1 && g_buzzActor.posAngles.pos.z > -70147)
			{
				v0 = 1;
				goto LABEL_5;
			}

			if (g_unk52FA24 > 0)
			{
			LABEL_5:
				g_unk52F2F8 = 2;
				g_unk52AD64 = v0 / 60 + 100;

				v0 -= (int32_t)Renderer::g_frameDelta;
				v4 = (int32_t)Renderer::g_frameDelta + g_unk52FA28 <= 512;

				g_unk52FA24 = v0;
				g_unk52FA28 += (int32_t)Renderer::g_frameDelta;

				if (! v4)
					g_unk52FA28 = 512;

				if (v0 <= 0)
				{
					g_unk52FA24 = 0;
					Platform::SetRotationAngles(15, 0, 0, 0);
					Nu3D::Link::SetRotationRelative8bit(33, 0, 0, 0);

					g_unk52F2F8 = 0;
					g_unk52AD64 = 100;
				LABEL_16:

					v0 = g_unk52FA24;
					goto LABEL_17;
				}
				goto LABEL_17;
			}

			if (g_unk52FA28 > 0)
			{
				g_unk52FA28 -= (int32_t)Renderer::g_frameDelta;

				if (g_unk52FA28 < 0)
					g_unk52FA28 = 0;
			}

			if (Buzz::g_groundSlamTicker)
			{
				if (Platform::HadBuzzContactThisFrame(15) && ! g_unk52F2F8)
				{
					Platform::SetRotationAngles(15, 0, 0, -384);
					Nu3D::Link::SetRotationRelative8bit(33, 0, 0, -384);
					g_unk52FA24 = 2700;
					Levels::Method3(0, 1);
				}
				goto LABEL_16;
			}
		LABEL_17:

			if (g_unk52FA28)
			{
				AudioManager::g_pitchArray[0] = 8 * g_unk52FA28 + 1024;
				Nu3D::Link::GetCurrentPosFixed(33, &a2);
				AudioManager::PlaySoundEffect(117, &a2);
				v0 = g_unk52FA24;
			}

			g_unk52FA30 += (int32_t)Renderer::g_frameDelta * g_unk52FA28 / 16;

			if (v0 > 0 && (Platform::GetFlags(0) & 2) == 2)
			{
				g_unk52F9E4 = 1;
				Buzz::g_groundSlamTicker = 0;
				Levels::Method2(-3072, 2);
				AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);

				Camera::g_unk52AD98 = 64;

				g_buzzActor.velX = Numerics::g_sinCosLUT[2960] / 7;
				g_buzzActor.actorFlags |= 0x44u;
				g_buzzActor.velForward = Numerics::g_sinCosLUT[3984] / 7;
				g_buzzActor.posAngles.angles.yaw = 2960;
				g_buzzActor.facingAngle = 2960;
			}

			Nu3D::Link::GetTargetPosFixed(0, &a2);
			Platform::GetOrigin(0, &v23);

			v1 = Numerics::g_sinCosLUT[((7 * g_unk52FA30) / 6) & 0xFFF];
			a2.y += (v1 / 2) - v23.y;

			Platform::SetVelocity(0, 0, a2.y, 0);
			Platform::GetRotation(0, &a2);

			if (g_unk52F9E4 == 1)
			{
				Platform::SetAngularVelocity(0, 0, 0, (-1800 - a2.z) >> 2);

				if (abs((-1800 - a2.z) >> 2) < 8)
					g_unk52F9E4 = 2;
			}
			else if (g_unk52F9E4 == 2)
			{
				Platform::SetAngularVelocity(0, 0, 0, (Numerics::g_sinCosLUT[g_unk52FA30 & 0xFFF] / 32 - a2.z) >> 3);

				if (abs((Numerics::g_sinCosLUT[g_unk52FA30 & 0xFFF] / 32 - a2.z) >> 3) < 8)
					g_unk52F9E4 = 0;
			}
			else
			{
				Platform::SetAngularVelocity(0, 0, 0, (Numerics::g_sinCosLUT[g_unk52FA30 & 0xFFF] / 32 - a2.z) >> 2);
			}

			Platform::CommitRotationToLink(0, 0);

			Nu3D::Link::SetPositionRawAndCommit(0, v23.x >> 5, v23.y >> 5, v23.z >> 5);
			Nu3D::Link::SetPositionRawAndCommit(1, v23.x >> 7, v23.y >> 7, v23.z >> 7);
			Nu3D::Link::GetRotation8Bit(0, &a2);
			Nu3D::Link::SetRotationRelative8bit(1, a2.x, a2.y, a2.z);

			v2 = g_unk52FA08;

			if (g_unk52FA08 >= g_unk52FA10)
			{
				if (g_unk52FA08 <= g_unk52FA10)
					goto LABEL_35;

				g_unk52FA08 += -512 * (int32_t)Renderer::g_frameDelta;
				v2 = g_unk52FA08;

				if (g_unk52FA08 > g_unk52FA10)
					goto LABEL_35;
			}
			else
			{
				g_unk52FA08 += (int32_t)Renderer::g_frameDelta << 9;
				v2 = g_unk52FA08;

				if (g_unk52FA08 < g_unk52FA10)
					goto LABEL_35;
			}

			v2 = g_unk52FA10;
			g_unk52FA08 = g_unk52FA10;
		LABEL_35:

			if (v2)
			{
				Platform::DisableCollision(18);
				Nu3D::Link::GetTargetPosFixed(31, &a2);
				Nu3D::Link::SetPositionRawAndCommit(31, a2.x >> 5, (a2.y - g_unk52FA08) >> 5, a2.z >> 5);
			}
			else
			{
				Collision::MarkPlatformAsMoving(18);
				Nu3D::Link::GetTargetPosFixed(31, &a2);
				Nu3D::Link::SetPositionRawAndCommit(31, a2.x >> 5, a2.y >> 5, a2.z >> 5);
			}

			Platform::SetAngularVelocity(1, 0, 0, 36);
			Platform::CommitRotationToLink(1, 7);

			Platform::SetAngularVelocity(2, 0, 0, 44);
			Platform::CommitRotationToLink(2, 6);

			Platform::SetAngularVelocity(3, 0, 0, 36);
			Platform::CommitRotationToLink(3, 4);

			Platform::SetAngularVelocity(4, 0, 0, 44);
			Platform::CommitRotationToLink(4, 5);

			Nu3D::Link::SetScaleFromFixedOffsets(30, 4096, abs(g_unk52F9C0) << 7, 4096);
			Nu3D::Link::SetRotationRelative8bit(2, 0, 0, 32 * abs(g_unk52F9C0));

			if (g_buzzActor.collisionFlags && Buzz::g_unk52B81C == 8)
			{
				v3 = g_unk52F9C0;
				v4 = g_unk52F9C0 <= 0;

				if (! g_unk52F9C0)
				{
					if (! Buzz::g_groundSlamTicker)
						goto LABEL_52;

					Levels::Method3(1, 1);
					v3 = 2;
					goto LABEL_45;
				}
			}
			else
			{
				v3 = g_unk52F9C0;
				v4 = g_unk52F9C0 <= 0;

				if (! g_unk52F9C0)
					goto LABEL_52;
			}

			if (! v4)
			{
			LABEL_45:
				v5 = (int32_t)Renderer::g_frameDelta + v3;
				g_unk52F9C0 = v5;

				if (v5 <= 32)
				{
					if (v5 > 6 && v5 - (int32_t)Renderer::g_frameDelta <= 6)
					{
						Buzz::g_groundSlamTicker = 0;
						Levels::Method2(-3072, 2);
						AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);

						g_buzzActor.actorFlags |= 0x44u;
						g_buzzActor.velX = Numerics::g_sinCosLUT[2078] >> 3;
						g_buzzActor.velForward = Numerics::g_sinCosLUT[3102] >> 3;
						g_buzzActor.posAngles.angles.yaw = 2078;
						g_buzzActor.facingAngle = 2078;

						Camera::g_unk52AD98 = 128;
					}
				}
				else
				{
					g_unk52F9C0 = -32;
				}

				goto LABEL_52;
			}

			g_unk52F9C0 = (int32_t)Renderer::g_frameDelta + v3;

			if ((int32_t)Renderer::g_frameDelta + v3 > 0)
				g_unk52F9C0 = 0;
		LABEL_52:

			if (g_buzzActor.collisionFlags && Buzz::g_unk52B81C == 9)
			{
				*(uint8_t*)&g_buzzActor.actorFlags &= 0xBBu;
				g_unk52F9EC = 1;

				if (Buzz::g_groundSlamTicker)
				{
					Buzz::g_groundSlamTicker = 0;
					Levels::Method2(-3072, 2);
				}
				else
				{
					Levels::Method2(-2432, 2);
				}

				AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
			}

			if (g_unk52F9EC)
			{
				if (g_unk52F9EC <= 12288)
				{
					Nu3D::Link::SetScaleFromFixedOffsets(3, 4096, Numerics::g_sinCosLUT[g_unk52F9EC & 0xFFF] >> (((g_unk52F9EC >> 11) & 0xE) + 3), 4096);

					g_unk52F9EC += (int32_t)Renderer::g_frameDelta << 8;
				}
				else
				{
					g_unk52F9EC = 0;
					Nu3D::Link::SetScaleFromFixedOffsets(3, 4096, 0, 4096);
				}
			}

			Nu3D::Link::GetCurrentPosFixed(8, &a2);
			Nu3D::Link::SetPositionRawAndCommit(21, a2.x >> 7, a2.y >> 7, a2.z >> 7);

			Nu3D::Link::GetCurrentPosFixed(9, &a2);
			Nu3D::Link::SetPositionRawAndCommit(22, a2.x >> 7, a2.y >> 7, a2.z >> 7);

			v6 = g_unk52FA4C <= 0;

			if (! g_unk52FA4C)
			{
				if (! g_buzzActor.collisionFlags)
					goto LABEL_85;

				if ((Platform::GetFlags(10) & 3) == 2 && ((int16_t*)Platform::GetContactFace32(10))[2] < -12288)
				{
					g_unk52FA4C = 32;
					Levels::Method3(3, 1);
				}

				v6 = g_unk52FA4C <= 0;
			}

			if (! v6)
			{
				Platform::GetOrigin(10, &a2);
				AudioManager::g_pitchArray[0] = g_unk52FA4C + 3400;
				AudioManager::PlaySoundEffect(122, &a2);

				Nu3D::Link::SetPositionRawAndCommit(11, a2.x >> 5, a2.y >> 5, a2.z >> 5);

				if (g_unk52FA4C >= 1700)
					v7 = 1700;
				else
					v7 = 32 * (int32_t)Renderer::g_frameDelta + g_unk52FA4C;

				v8 = g_unk52F9BC;
				g_unk52FA4C = v7;

				if (a2.z < 273504)
				{
					if (a2.y <= -7680 - (int32_t)Renderer::g_frameDelta * g_unk52F9BC || g_unk52F9BC <= 0)
					{
						if (g_unk52F9BC >= 4096)
							v8 = 4096;
						else
							v8 = 96 * (int32_t)Renderer::g_frameDelta + g_unk52F9BC;
					}
					else if (g_unk52F9BC <= 1200)
					{
						v8 = -192;
					}
					else
					{
						v8 = -3 * g_unk52F9BC / 8;
					}

					g_unk52F9BC = v8;
				}

				Platform::SetVelocity(10, 0, (int32_t)Renderer::g_frameDelta * v8, -((int32_t)Renderer::g_frameDelta * v7));

				if (a2.z < 66752)
				{
					g_unk52FA4C = -1;
					Platform::SetVelocity(10, 0, 0, 0);

					if ((Platform::GetFlags(10) & 3) == 2)
					{
						Levels::Method2(-2560, 2);
						AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
						*(uint8_t*)&g_buzzActor.actorFlags |= 0x44u;

						g_buzzActor.velX = Numerics::g_sinCosLUT[2078] >> 5;
						g_buzzActor.velForward = Numerics::g_sinCosLUT[3102] >> 5;
						g_buzzActor.posAngles.angles.yaw = 2078;
						g_buzzActor.facingAngle = 2078;
					}
				}
			}
		LABEL_85:

			v9 = g_unk52F9E8 <= 0;

			if (! g_unk52F9E8)
			{
				if (! g_buzzActor.collisionFlags)
					goto LABEL_105;

				if ((Platform::GetFlags(14) & 3) == 2 && ((int16_t*)Platform::GetContactFace32(14))[2] < -12288)
				{
					g_unk52F9E8 = 32;
					Levels::Method3(2, 1);
				}

				v9 = g_unk52F9E8 <= 0;
			}

			if (! v9)
			{
				Platform::GetOrigin(14, &a2);
				AudioManager::g_pitchArray[0] = g_unk52F9E8 + 3400;
				AudioManager::PlaySoundEffect(122, &a2);

				a2.x >>= 5;
				a2.y >>= 5;
				a2.z >>= 5;

				Nu3D::Link::SetPositionRawAndCommit(24, a2.x, a2.y, a2.z);
				Platform::SetVelocity(14, 0, 0, -((int32_t)Renderer::g_frameDelta * g_unk52F9E8));

				if (g_unk52F9E8 >= 1800)
					v10 = 1800;
				else
					v10 = 32 * (int32_t)Renderer::g_frameDelta + g_unk52F9E8;

				g_unk52F9E8 = v10;

				if (a2.z < 8960)
				{
					g_unk52F9E8 = -7 * v10 / 8;

					if ((Platform::GetFlags(14) & 3) == 2)
					{
						Levels::Method2(-2560, 2);
						AudioManager::PlaySoundEffect(28, &g_buzzActor.posAngles.pos);
						*(uint8_t*)&g_buzzActor.actorFlags |= 0x44u;

						g_buzzActor.velX = Numerics::g_sinCosLUT[2078] >> 4;
						g_buzzActor.velForward = Numerics::g_sinCosLUT[3102] >> 4;
						g_buzzActor.posAngles.angles.yaw = 2078;
						g_buzzActor.facingAngle = 2078;
					}
				}

				v9 = g_unk52F9E8 < 0;
			}

			if (v9)
			{
				Platform::GetOrigin(14, &a2);

				a2.x >>= 5;
				a2.y >>= 5;
				a2.z >>= 5;

				Nu3D::Link::SetPositionRawAndCommit(24, a2.x, a2.y, a2.z);

				if (a2.z <= 14886)
				{
					Platform::SetVelocity(14, 0, 0, -g_unk52F9E8);
				}
				else
				{
					v11 = 32 * (int32_t)Renderer::g_frameDelta + g_unk52F9E8 < 0;
					v12 = 32 * (int32_t)Renderer::g_frameDelta + g_unk52F9E8;
					g_unk52F9E8 = v12;

					if (v11)
					{
						Platform::SetVelocity(14, 0, 0, -v12);
					}
					else
					{
						Platform::SetVelocity(14, 0, 0, 0);
						g_unk52F9E8 = 0;
						Platform::SetVelocity(14, 0, 0, 0);
					}
				}
			}
		LABEL_105:

			Platform::StepMotionScript(7, 15, (int16_t**)&g_unk52FA00, &g_unk52FA44, &g_unk52F9D4);
			Platform::StepMotionScript(8, 16, (int16_t**)&g_unk52FA04, &g_unk52FA48, &g_unk52F9D8);
			Platform::StepMotionScript(9, 17, (int16_t**)&g_unk52F9F8, &g_unk52FA3C, &g_unk52F9CC);

			Nu3D::Link::GetCurrentPosFixed(15, &a2);
			Nu3D::Link::SetPositionRawAndCommit(18, a2.x >> 7, a2.y >> 7, a2.z >> 7);

			Nu3D::Link::GetCurrentPosFixed(16, &a2);
			Nu3D::Link::SetPositionRawAndCommit(19, a2.x >> 7, a2.y >> 7, a2.z >> 7);

			Nu3D::Link::GetCurrentPosFixed(17, &a2);
			Nu3D::Link::SetPositionRawAndCommit(20, a2.x >> 7, a2.y >> 7, a2.z >> 7);

			if (g_discLauncherAmmo == 6)
			{
				*(uint8_t*)&g_levelActors[7].creatureRam->defenseMode = 4;
				*(uint8_t*)&g_levelActors[8].creatureRam->defenseMode = 4;
				*(uint8_t*)&g_levelActors[9].creatureRam->defenseMode = 4;
			}
			else
			{
				*(uint8_t*)&g_levelActors[7].creatureRam->defenseMode = 5;
				*(uint8_t*)&g_levelActors[8].creatureRam->defenseMode = 5;
				*(uint8_t*)&g_levelActors[9].creatureRam->defenseMode = 5;
			}

			if (! g_levelActors[7].actorPhase)
				Platform::StepMotionScript(13, 12, (int16_t**)&g_unk52F9FC, &g_unk52FA40, &g_unk52F9D0);

			if (! g_levelActors[8].actorPhase)
				Platform::StepMotionScript(12, 13, (int16_t**)&g_unk52F9F0, &g_unk52FA34, &g_unk52F9C4);

			if (! g_levelActors[9].actorPhase)
				Platform::StepMotionScript(11, 14, (int16_t**)&g_unk52F9F4, &g_unk52FA38, &g_unk52F9C8);

			if (g_tickState[11])
			{
				if (Actor::IsInsideBounds((Actor::Toy2Actor*)&g_buzzActor, -380759, -223831, 28029, 337021))
				{
					v13 = g_unk52FA0C + 1;
					g_unk52FA0C = v13;

					if (v13 >= Levels::g_recordData[6]->recordCount)
					{
						v13 = 0;
						g_unk52FA0C = 0;
					}

					v14 = Nu3D::Particles::SpawnFromPreset(
						32 * Levels::g_recordData[6]->data[v13].x,
						32 * Levels::g_recordData[6]->data[v13].y,
						32 * Levels::g_recordData[6]->data[v13].z,
						86,
						25);

					v14->groundHeightY = 0;

					AudioManager::PlaySoundEffect(116, &v14->pos);
				}

				if (g_tickState[11] && Actor::IsInsideBounds((Actor::Toy2Actor*)&g_buzzActor, -244538, -112442, -42973, 298915))
				{
					v15 = *g_randDatBufferPtr++ & 7;

					if (v15 > 5)
						v15 -= 6;

					Nu3D::Particles::SpawnFromPreset(
						32 * Levels::g_recordData[7]->data[v15].x, 32 * Levels::g_recordData[7]->data[v15].y, 32 * Levels::g_recordData[7]->data[v15].z, 80, 24)
						->groundAlignRot = (*g_randDatBufferPtr & 3) << 10;

					++g_randDatBufferPtr;
				}
			}

			x = g_levelActors[10].transform.pos.x;
			if (g_levelActors[10].transform.pos.x < -499087)
			{
				x = -499087;
				g_levelActors[10].transform.pos.x = -499087;
			}

			v17 = g_levelActors[11].transform.pos.x;
			if (g_levelActors[11].transform.pos.x < -499087)
			{
				v17 = -499087;
				g_levelActors[11].transform.pos.x = -499087;
			}

			z = g_levelActors[10].transform.pos.z;
			if (g_levelActors[10].transform.pos.z < -14536)
			{
				z = -14536;
				g_levelActors[10].transform.pos.z = -14536;
			}

			if (g_levelActors[11].transform.pos.z < -14536)
				g_levelActors[11].transform.pos.z = -14536;

			if (MoveableObject::g_moveableObjects[0].pos.z >= 44544)
			{
				if (z > MoveableObject::g_moveableObjects[0].pos.z - 38912 && x > MoveableObject::g_moveableObjects[0].pos.x - 18432)
				{
					z = MoveableObject::g_moveableObjects[0].pos.z - 38912;
					g_levelActors[10].velForward = -2304;
					g_levelActors[10].transform.pos.z = MoveableObject::g_moveableObjects[0].pos.z - 38912;
				}

				if (g_levelActors[11].transform.pos.z > MoveableObject::g_moveableObjects[0].pos.z - 38912
					&& v17 > MoveableObject::g_moveableObjects[0].pos.x - 18432)
				{
					g_levelActors[11].transform.pos.z = MoveableObject::g_moveableObjects[0].pos.z - 38912;
					g_levelActors[11].velForward = -2304;
				}
			}
			else
			{
				if (x > MoveableObject::g_moveableObjects[0].pos.x - 18432)
					g_levelActors[10].transform.pos.x = MoveableObject::g_moveableObjects[0].pos.x - 18432;

				if (v17 > MoveableObject::g_moveableObjects[0].pos.x - 18432)
					g_levelActors[11].transform.pos.x = MoveableObject::g_moveableObjects[0].pos.x - 18432;
			}

			if ((g_levelActors[10].actorFlags & 1) != 0)
				g_unk52F9DC = 1;

			actorFlags = (char)g_levelActors[11].actorFlags;

			if ((g_levelActors[11].actorFlags & 1) != 0)
				g_unk52F9E0 = 1;

			if (g_levelActors[10].actorPhase > 0 && g_unk52F9DC && (z > 290816 || (g_levelActors[10].actorFlags & 1) == 0))
			{
				g_unk52F9DC = 0;

				if ((g_levelActors[10].actorFlags & 2) != 0)
				{
					Actor::Kill(&g_levelActors[10], 2);
					actorFlags = (char)g_levelActors[11].actorFlags;
				}
				else
				{
					g_levelActors[10].actorPhase = 0;
				}
			}

			if (g_levelActors[11].actorPhase > 0 && g_unk52F9E0 && (g_levelActors[11].transform.pos.z > 290816 || (actorFlags & 1) == 0))
			{
				g_unk52F9E0 = 0;

				if ((actorFlags & 2) != 0)
					Actor::Kill(&g_levelActors[11], 2);
				else
					g_levelActors[11].actorPhase = 0;
			}

			if ((g_levelActors[12].actorFlags & 0x200) != 0)
			{
				g_levelActors[12].actorFlags &= ~0x200u;

				if (g_collectionCount >= 0)
				{
					AudioManager::PlaySoundEffect(118, &g_levelActors[12].transform.pos);

					if (g_collectionCount == 5)
					{
						Dialogue::Begin(12, 4, "thanks for finding my ^chicks^ buzz! here is a pizza planet ^token^!", -1, 0, 1);
						g_collectionCount = -1;
					}
					else
					{
						Dialogue::Begin(
							12,
							4,
							"hi buzz! if you find my ^five^ missing ^chicks^ and come back and find me, i will give you a pizza planet ^token^.",
							-1,
							0,
							-1);
					}
				}
			}

			if ((g_levelActors[13].actorFlags & 0x200) != 0)
			{
				g_levelActors[13].actorFlags &= ~0x200u;

				if (g_unk52F2F8)
				{
					AudioManager::PlaySoundEffect(114, &g_levelActors[13].transform.pos);
					Dialogue::Begin(13, 8, "hurry up, buzz! the ^egg^ is still hatching!", -1, 0, -1);
				}
				else
				{
					if (g_unk52FA1C == 2)
						goto LABEL_194;

					AudioManager::PlaySoundEffect(114, &g_levelActors[13].transform.pos);
					g_unk52F2F8 = 1;
					g_unk52FA10 = 0x4000;

					if (g_unk52FA1C)
					{
						g_unk52AD64 = 126;
						Dialogue::Begin(
							13,
							8,
							"this time the egg will hatch quicker but there is a ^token^ inside it! get to it in time and you can keep the ^token^!",
							-1,
							0,
							2);
					}
					else
					{
						g_unk52AD64 = 150;
						Dialogue::Begin(
							13,
							8,
							"if you can get to my hatching ^egg^ in time you can keep the ^chick^ that you find in it! come back and see me after you have got "
							"the ^chick^!",
							-1,
							0,
							-1);
					}
				}
			}

			v20 = g_unk52F2F8;

			if (g_unk52F2F8 != 1)
			{
				if (g_unk52F2F8 != 2)
				{
				LABEL_189:
					if (! v20)
						goto LABEL_194;
					goto LABEL_190;
				}
			LABEL_177:

				if (g_unk52FA24)
					goto LABEL_194;

				if (g_unk52FA1C)
				{
					if (Collectables::g_pickupSlots[2].state == 2)
					{
						g_unk52FA1C = 2;
						g_unk52F2F8 = 0;
						goto LABEL_194;
					}
				}
				else if (! g_levelActors[6].creatureId)
				{
					g_unk52FA1C = 1;
					g_unk52F2F8 = 0;
					g_unk52FA10 = 0;

					Levels::Method2(-1536, 2);

					v20 = g_unk52F2F8;
					if (! g_unk52F2F8)
						goto LABEL_194;
				}

				if (g_curSectorCopy1 == 4)
					g_unk52AD64 = 99;

				if (g_tickState[11])
					--g_unk52AD64;

				if (g_unk52AD64 < 100)
				{
					g_unk52AD64 = 100;
					v20 = 0;
					g_unk52F2F8 = 0;

					if (g_unk52FA1C)
					{
						Collectables::Deactivate(2);
						v20 = g_unk52F2F8;
					}

					g_unk52FA10 = 0;
				}
				goto LABEL_189;
			}

			if (! Renderer::g_enableCinematicBars)
			{
				v20 = 2;
				g_unk52F2F8 = 2;
				goto LABEL_177;
			}
		LABEL_190:

			if (! g_unk52FA24)
			{
				*(uint8_t*)&g_levelActors[6].actorFlags |= 0x81u;
				goto LABEL_195;
			}
		LABEL_194:

			*(uint8_t*)&g_levelActors[6].actorFlags &= 0x7Eu;
		LABEL_195:

			if (Gadget::g_unk830D48 <= 0)
				Actor::ItemReturnReward(
					14,
					2,
					"hi buzz! you need ^rocket boots^ to cross over to the shopping cart. find my missing ^arm^, i will let you use the ^rocket boots^!",
					"wow! thanks buzz! thanks for finding my ^arm^ now you can use the ^rocket boots^. they give you great speed for a short amount of time. "
					"they're great for racing around with!",
					"the ^rocket boots^ give you great speed for a short amount of time. they're great for racing around with!",
					1536,
					3584);
			else
				Actor::ItemReturnReward(
					14,
					10,
					"hi buzz! you need ^rocket boots^ to cross over to the shopping cart. find my missing ^arm^, i will let you use the ^rocket boots^!",
					"wow! thanks buzz! thanks for finding my ^arm^ now you can use the ^rocket boots^. they give you great speed for a short amount of time. "
					"they're great for racing around with!",
					"the ^rocket boots^ give you great speed for a short amount of time. they're great for racing around with!",
					1536,
					3584);

			Actor::CollectQuestReward(1, 3, 3600, 1760, 0);
			Actor::RotatingHint(31, 11, g_unk4F2A34);

			v21 = g_unk52FA14;
			if (! g_unk52FA14)
			{
				if (! g_buzzActor.collisionFlags || g_buzzActor.posAngles.pos.y <= -1596 || g_curSectorCopy1 != 4)
					goto LABEL_211;

				g_unk52FA14 = 1;
				Dialogue::Begin(0, 5, "ha ha ha ha ... defeat the ^dinosaur^ boss to get a pizza planet ^token^!", -1, 0, -1);
				v21 = g_unk52FA14;
			}

			if (v21 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_211;

				g_unk830D44 = 180;
				g_unk52FA14 = 2;
				g_levelActors[0].movementData = Actor::g_movementData[24] + 14;
				g_levelActors[0].unkShort1 = 0;
				g_levelActors[0].creatureRam->initialFacingAngle = 0;
				v21 = g_unk52FA14;
			}

			if (v21 >= 3)
			{
				if (v21 >= 120)
				{
					if (v21 != 200)
					{
						Collectables::Activate(4, 0);
						g_unk52FA14 = 200;
					}
				}
				else
				{
					g_unk52FA14 = (int32_t)Renderer::g_frameDelta + v21;
				}
			}
		LABEL_211:

			Method1(19, 1);
			Method1(20, 1);
			Method1(21, 1);

			if (g_levelActors[27].transform.pos.y < g_levelActors[27].boundary.y)
				g_levelActors[27].transform.pos.y = g_levelActors[27].boundary.y;

			if (g_rocketBootsDuration && g_buzzActor.posAngles.pos.y < -50176 && g_buzzActor.posAngles.pos.y > -53248 && g_buzzActor.collisionFlags)
			{
				Levels::Method2(-1920, 2);
			}

			PlayLevelMusic();
		}

	}
}

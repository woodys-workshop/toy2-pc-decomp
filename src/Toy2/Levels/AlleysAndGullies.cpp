#include "Toy2/Levels/AlleysAndGullies.h"
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
#include "Renderer/Shadows.h"
#include "AudioManager/AudioManager.h"
#include "InputManager.h"
#include "Random.h"
#include "SaveManager.h"
#include "Nullsub.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Numerics.h"

namespace Toy2
{
	namespace AlleysAndGullies
	{
		// GLOBAL: TOY2 0x004F215C
		int16_t g_unk4F215C[14] = { 14, 0, 12, 0, 17, 6, -2, 18, 7, 51, 26, 21, -1, 0 };

		// GLOBAL: TOY2 0x004F2178
		int16_t g_unk4F2178[6] = { 89, 87, 86, 88, 85, 0 };

		// GLOBAL: TOY2 0x004F2184
		char* g_unk4F2184[5] = {
			"i saw ^hamm^ in the market place near an ^umbrella^.",
			"the ^mother duck^ has lost her ^ducklings^. she is near the central drain.",
			"^slinky^ has a ^challenge^ for you. he is on the island in the middle of the ^gully^ over this fence.",
			"there is a ^token^ on the table in the market place. you need to float over to it on a ^balloon^.",
			"the ^clown top^ boss is on the roof of the apartment building. you need a ^grappling hook^ to climb up. help ^mr. potato head^ on another level "
			"to open the ^grappling hook^."
		};

		// GLOBAL: TOY2 0x0052F8A8
		int32_t g_unk52F8A8;

		// GLOBAL: TOY2 0x0052F8AC
		int32_t g_unk52F8AC;

		// GLOBAL: TOY2 0x0052F8B4
		int32_t g_unk52F8B4;

		// GLOBAL: TOY2 0x0052F8C0
		int32_t g_unk52F8C0;

		// GLOBAL: TOY2 0x0052F8C4
		int32_t g_unk52F8C4;

		// GLOBAL: TOY2 0x0052F8C8
		int32_t g_unk52F8C8;

		// GLOBAL: TOY2 0x0052F8D0
		int32_t g_unk52F8D0;

		// GLOBAL: TOY2 0x0052F8D4
		int32_t g_unk52F8D4;

		// GLOBAL: TOY2 0x0052F8D8
		int32_t g_unk52F8D8;

		// GLOBAL: TOY2 0x0052F8DC
		int32_t g_unk52F8DC;

		// GLOBAL: TOY2 0x0052F8E0
		int32_t g_unk52F8E0;

		// GLOBAL: TOY2 0x0052F8EC
		int32_t g_unk52F8EC;

		// GLOBAL: TOY2 0x0052F8F0
		int32_t g_unk52F8F0;

		// GLOBAL: TOY2 0x0052F8F4
		int32_t g_unk52F8F4;

		// GLOBAL: TOY2 0x0052F8F8
		int32_t g_unk52F8F8;

		// GLOBAL: TOY2 0x0052F900
		int32_t g_unk52F900;

		// GLOBAL: TOY2 0x0052F904
		int32_t g_unk52F904;

		// GLOBAL: TOY2 0x0052F908
		int32_t g_unk52F908;

		// GLOBAL: TOY2 0x0052F90C
		int32_t g_unk52F90C;

		// GLOBAL: TOY2 0x0052F910
		Vector3I g_unk52F910;

		// GLOBAL: TOY2 0x0052F91C
		int32_t g_unk52F91C;

		// GLOBAL: TOY2 0x0052F920
		Vector3I g_unk52F920;

		// GLOBAL: TOY2 0x0052F92C
		int32_t g_unk52F92C;

		// GLOBAL: TOY2 0x0052F930
		int32_t g_unk52F930;

		// GLOBAL: TOY2 0x0052F934
		int32_t g_unk52F934;

		// GLOBAL: TOY2 0x0052F938
		int32_t g_unk52F938;

		// GLOBAL: TOY2 0x0052F93C
		int32_t g_unk52F93C;

		// GLOBAL: TOY2 0x0052F940
		int32_t g_unk52F940;

		// GLOBAL: TOY2 0x0052F944
		int32_t g_unk52F944;

		// GLOBAL: TOY2 0x0052F948
		int32_t g_unk52F948;

		// GLOBAL: TOY2 0x0052F94C
		int32_t g_unk52F94C;

		// GLOBAL: TOY2 0x0052F950
		int32_t g_unk52F950;

		// GLOBAL: TOY2 0x0052F954
		HiddenCollectible g_unk52F954[5];

		// GLOBAL: TOY2 0x0052F97C
		int32_t g_unk52F97C;

		// GLOBAL: TOY2 0x0052F980
		int32_t g_unk52F980;

		// GLOBAL: TOY2 0x0052F984
		int32_t g_unk52F984;

		// GLOBAL: TOY2 0x0052F988
		int32_t g_unk52F988;

		// GLOBAL: TOY2 0x0052F8B0
		int32_t g_unk52F8B0;

		// GLOBAL: TOY2 0x0052F8B8
		int32_t g_unk52F8B8;

		// GLOBAL: TOY2 0x0052F8E8
		int32_t g_unk52F8E8;

		// GLOBAL: TOY2 0x0052F8FC
		int32_t g_unk52F8FC;

		// FUNCTION: TOY2 0x0041E150
		void UpdateMovingWood(int32_t* pathProgress, int32_t platformIndex, int32_t recordIndex, int32_t renderLinkId, int32_t renderLinkId2, int32_t speed)
		{
			int32_t l_y;
			int32_t l_x;
			int32_t l_z;
			Vector3I l_normalised;
			Vector3I l_origin;
			Vector3I l_interpolatedPoint;

			if (*pathProgress <= (Levels::g_recordData[recordIndex]->recordCount - 2) << 12)
			{
				InterpolatePathPoint(recordIndex, *pathProgress, &l_interpolatedPoint);
				Platform::GetOrigin(platformIndex, &l_origin);

				l_y = 32 * l_interpolatedPoint.y - l_origin.y;
				l_x = 32 * l_interpolatedPoint.x - l_origin.x;
				l_z = 32 * l_interpolatedPoint.z - l_origin.z;

				l_normalised.x = l_x;
				l_normalised.y = l_y;
				l_normalised.z = l_z;

				if (l_x < 4096)
					*pathProgress += 4096;

				while (abs(l_x) > 0x4000 || abs(l_y) > 0x4000 || abs(l_z) > 0x4000)
				{
					l_x >>= 2;
					l_y >>= 2;
					l_z >>= 2;
				}

				l_normalised.x = l_x;
				l_normalised.z = l_z;
				l_normalised.y = l_y;

				Nu3D::Math::NormalizeToFixedPoint(&l_normalised, &l_normalised);

				Platform::SetVelocity(
					platformIndex,
					(speed * l_normalised.x * (int32_t)Renderer::g_frameDelta) >> 10,
					(speed * l_normalised.y * (int32_t)Renderer::g_frameDelta) >> 10,
					(speed * l_normalised.z * (int32_t)Renderer::g_frameDelta) >> 10);
			}
			else
			{
				*pathProgress = 0;

				InterpolatePathPoint(recordIndex, 0, &l_interpolatedPoint);

				Platform::SetOrigin(platformIndex, 32 * l_interpolatedPoint.x, 32 * l_interpolatedPoint.y, 32 * l_interpolatedPoint.z);

				Platform::GetOrigin(platformIndex, &l_origin);
			}

			if (renderLinkId2)
				Nu3D::Link::SetPositionRawAndCommit(renderLinkId2, l_origin.x >> 5, l_origin.y >> 5, l_origin.z >> 5);

			Nu3D::Link::SetPositionRawAndCommit(renderLinkId, l_origin.x >> 5, l_origin.y >> 5, l_origin.z >> 5);
		}

		// FUNCTION: TOY2 0x0041E300
		void InitHiddenCollectibles()
		{
			int32_t i;
			Levels::Type63* entry;

			i = 0;
			if (Levels::g_recordData[63]->recordCount)
			{
				entry = (Levels::Type63*)Levels::g_recordData[63]->data;
				do
				{
					if (entry->props.linkId == 97)
						g_unk52F954[0].yPtr = &entry->pos.y;
					if (entry->props.linkId == 98)
						g_unk52F954[1].yPtr = &entry->pos.y;
					if (entry->props.linkId == 99)
						g_unk52F954[2].yPtr = &entry->pos.y;
					if (entry->props.linkId == 100)
						g_unk52F954[3].yPtr = &entry->pos.y;
					if (entry->props.linkId == 101)
						g_unk52F954[4].yPtr = &entry->pos.y;
					++entry;
					++i;
				} while (i < Levels::g_recordData[63]->recordCount);
			}
			for (i = 0; i < 5; ++i)
			{
				g_unk52F954[i].savedY = *g_unk52F954[i].yPtr;
				*g_unk52F954[i].yPtr = 0x80000000;
				Nu3D::Link::SetScaleFromFixedOffsets(i + 97, 0, 0, 0);
			}
		}

		// FUNCTION: TOY2 0x0041E880
		void Interactions()
		{
			int16_t v0;
			int32_t v1;
			int32_t v2;
			int32_t v3;
			int32_t v4;
			int32_t v5;
			int32_t v6;
			int16_t v7;
			int32_t v8;
			int32_t v9;
			int32_t v10;
			int32_t v11;
			int32_t v12;
			int32_t y;
			int32_t x;
			int32_t z;
			int32_t v16;
			int32_t v17;
			int32_t v18;
			int32_t v19;
			int32_t v20;
			int32_t v21;
			int32_t v22;
			bool v23;
			int32_t v24;
			Nu3D::Particles::ParticleInstance* v25;
			Nu3D::Particles::ParticleInstance* v26;
			int32_t v27;
			int32_t v28;
			int32_t v29;
			Nu3D::Particles::ParticleInstance* v30;
			int32_t v31;
			int32_t v33;
			Levels::RecordData* v35;
			int32_t v37;
			int32_t v38;
			int32_t v39;
			bool v40;
			int32_t v42;
			int32_t v43;
			int32_t v44;
			int32_t l_groundHeight;
			int32_t v46;
			int32_t v47;
			int32_t v49;
			Vector3I a2;

			if (Buzz::g_currentFluidType == 1 && ! Nu3D::Camera::g_targetTintFadeSpeed
				&& ((Buzz::g_underwaterYLevel - Camera::g_activeCameraTransform.pos.y) >> 4) + 128 > 128)
			{
				AudioManager::PlaySoundEffect(111, 0);
			}

			if (Platform::g_unk559E80 == 5 || Platform::g_unk559E80 == 8)
			{
				Platform::SetVelocity(5, 0, 0, 0);
				Platform::SetVelocity(8, 0, 0, 0);
			}
			else
			{
				UpdateMovingWood(&g_unk52F8B4, 8, 0, 29, 33, 96);
				UpdateMovingWood(&g_unk52F8A8, 5, 0, 30, 34, 96);
			}

			if (Platform::g_unk559E80 == 7 || Platform::g_unk559E80 == 6)
			{
				Platform::SetVelocity(7, 0, 0, 0);
				Platform::SetVelocity(6, 0, 0, 0);
			}
			else
			{
				UpdateMovingWood(&g_unk52F8AC, 6, 5, 31, 35, 96);
				UpdateMovingWood(&g_unk52F97C, 7, 5, 32, 36, 96);
			}
			if (Platform::g_unk559E80 == 9 || Platform::g_unk559E80 == 10)
			{
				Platform::SetVelocity(9, 0, 0, 0);
				Platform::SetVelocity(10, 0, 0, 0);
			}
			else
			{
				UpdateMovingWood(&g_unk52F944, 9, 2, 21, 0, 192);
				UpdateMovingWood(&g_unk52F908, 10, 2, 22, 0, 192);
			}

			if (Platform::g_unk559E80 == 11 || Platform::g_unk559E80 == 12)
			{
				Platform::SetVelocity(11, 0, 0, 0);
				Platform::SetVelocity(12, 0, 0, 0);
			}
			else
			{
				UpdateMovingWood(&g_unk52F900, 11, 3, 23, 0, 192);
				UpdateMovingWood(&g_unk52F8F4, 12, 3, 24, 0, 192);
			}

			g_unk52F988 = Platform::StepTiltPhysics(2, 4, g_unk52F988, 2, -448, 448, 1);
			g_unk52F93C = Platform::StepTiltPhysics(1, 6, g_unk52F93C, 2, -448, 448, 1);

			Nu3D::Link::GetRotation8Bit(4, &a2);
			Nu3D::Link::SetRotationRelative8bit(5, a2.x, a2.y, a2.z);
			Nu3D::Link::GetRotation8Bit(6, &a2);
			Nu3D::Link::SetRotationRelative8bit(7, a2.x, a2.y, a2.z);

			if (g_buzzActor.collisionFlags && (Platform::GetFlags(3) & 3) == 2 && ((int16_t*)Platform::GetContactFace32(3))[2] < -12288)
			{
				Levels::Method3(1, 1);

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

			if (g_buzzActor.unkShort40)
			{
				Levels::g_recordData[61]->data[1].x = 0;
				Levels::g_recordData[61]->data[3].x = 0;
			}

			if (g_buzzActor.collisionFlags)
			{
				if ((g_unk52F938 & 1) != 0)
					goto LABEL_37;

				if ((Platform::GetFlags(4) & 3) == 2 && ((int16_t*)Platform::GetContactFace32(4))[2] < -12288 && Buzz::g_groundSlamTicker)
				{
					g_unk52F938 |= 1u;
					Platform::GetOrigin(4, &a2);
					a2.y += 3600;
					Platform::SetOrigin(4, a2.x, a2.y, a2.z);
					Nu3D::Link::SetScaleFromFixedOffsets(16, 4096, 2400, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(17, 4096, 2400, 4096);
					Levels::Method3(0, 1);
				}
			}

			if ((g_unk52F938 & 1) == 0)
			{
			LABEL_61:
				v6 = g_unk52F91C;
				goto LABEL_62;
			}
		LABEL_37:

			v0 = (int32_t)Renderer::g_frameDelta + g_unk52F8D0;
			g_unk52F8D0 += (int32_t)Renderer::g_frameDelta;
			if (g_unk52F8D0 > 128)
			{
				v0 = 128;
				g_unk52F8D0 = 128;
			}

			AudioManager::g_pitchArray[0] = 32 * (v0 + 32);
			Nu3D::Link::GetCurrentPosFixed(10, &a2);
			AudioManager::PlaySoundEffect(163, &a2);
			g_unk52F904 += g_unk52F8D0 * (int32_t)Renderer::g_frameDelta;

			v1 = g_unk52F8D4;

			if ((g_unk52F938 & 2) != 0)
			{
				if (g_unk52F8D4 <= 384)
				{
					v2 = g_unk52F90C - (int32_t)Renderer::g_frameDelta;

					g_unk52F90C = v2;
					if (v2 < 0)
					{
						v2 = 0;
						g_unk52F90C = 0;
						g_unk52F938 &= ~2u;
					}
				}
				else
				{
					v2 = (int32_t)Renderer::g_frameDelta + g_unk52F90C;
					g_unk52F90C = v2;

					if (v2 > 32)
					{
						g_unk52F90C = 32;
						v3 = 32 * (int32_t)Renderer::g_frameDelta / -8;
						goto LABEL_52;
					}
				}

				v3 = (int32_t)Renderer::g_frameDelta * v2 / -8;
				goto LABEL_52;
			}

			if (g_unk52F8D4 >= 512)
			{
				v4 = g_unk52F90C - (int32_t)Renderer::g_frameDelta;
				g_unk52F90C = v4;

				if (v4 < 0)
				{
					v4 = 0;
					g_unk52F90C = 0;
					g_unk52F938 |= 2u;
				}
			}
			else
			{
				v4 = (int32_t)Renderer::g_frameDelta + g_unk52F90C;
				g_unk52F90C = v4;
				if (v4 > 32)
				{
					v4 = 32;
					g_unk52F90C = 32;
				}
			}

			v3 = (int32_t)Renderer::g_frameDelta * v4 / 8;
		LABEL_52:

			g_unk52F8D4 += v3;

			Nu3D::Link::SetRotationRelative8bit(10, 0, -(v3 + v1), 0);
			Nu3D::Link::SetRotationRelative8bit(11, 0, -g_unk52F8D4, 0);
			Nu3D::Link::SetRotationRelative8bit(20, 0, -g_unk52F8D4, -2 * g_unk52F904);

			v5 = (g_unk52F8F8 + (int32_t)Renderer::g_frameDelta) & 0x1FF;
			g_unk52F8F8 = v5;

			if (v5 <= 32 || v5 >= 40)
				goto LABEL_56;

			v6 = g_unk52F91C;
			if (! g_unk52F91C)
			{
				g_unk52F91C = 1;
				AudioManager::PlaySoundEffect(160, &g_unk52F910);
				v5 = g_unk52F8F8;
			LABEL_56:
				v6 = g_unk52F91C;
			}

			if (v5 > 288 && v5 < 296 && ! g_unk52F92C)
			{
				g_unk52F92C = 1;
				AudioManager::PlaySoundEffect(160, &g_unk52F920);
				goto LABEL_61;
			}
		LABEL_62:

			if (v6 > 0)
			{
				v7 = Numerics::g_sinCosLUT[(3 * v6) & 0xFFF];
				v8 = 32 * (int32_t)Renderer::g_frameDelta + v6;
				v9 = v7 / 256 + 4096;
				g_unk52F91C = v8;

				if (v8 < 4096)
				{
					Nu3D::Link::SetScaleFromFixedOffsets(12, v9 * v8 / 4096, v8, v9 * v8 / 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(13, v9 * g_unk52F91C / 4096, g_unk52F91C, v9 * g_unk52F91C / 4096);
					Nu3D::Link::SetRotationRelative8bit(12, 0, Numerics::g_sinCosLUT[g_unk52F91C & 0xFFF] / 128, 0);
					Nu3D::Link::SetRotationRelative8bit(13, 0, Numerics::g_sinCosLUT[g_unk52F91C & 0xFFF] / 128, 0);

					Levels::g_recordData[61]->data[1].x = 3;
				LABEL_81:

					if (Buzz::g_unk53C668 > 0 && ! Buzz::g_unk53C5E4)
					{
						g_buzzActor.posAngles.pos.y += g_unk52F910.y + Levels::g_recordData[61]->data[1].y - Levels::g_recordData[61]->data[0].y;

						g_buzzActor.posAngles.pos.x = g_unk52F910.x;
						g_buzzActor.posAngles.pos.z = g_unk52F910.z;

						Levels::g_recordData[61]->data[1].x = 3;
					}

					Levels::g_recordData[61]->data[0].x = g_unk52F910.x;
					Levels::g_recordData[61]->data[0].y = g_unk52F910.y + Levels::g_recordData[61]->data[1].y;
					Levels::g_recordData[61]->data[0].z = g_unk52F910.z;
					goto LABEL_90;
				}

				if (g_unk52F910.y > -60596)
					Levels::g_recordData[61]->data[1].x = 3;

				Nu3D::Link::SetScaleFromFixedOffsets(12, v7 / 256 + 4096, 4096, v7 / 256 + 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(13, v7 / 256 + 4096, 4096, v7 / 256 + 4096);

				Nu3D::Link::SetRotationRelative8bit(12, 0, Numerics::g_sinCosLUT[g_unk52F91C & 0xFFF] / 128, 0);
				Nu3D::Link::SetRotationRelative8bit(13, 0, Numerics::g_sinCosLUT[g_unk52F91C & 0xFFF] / 128, 0);

				if (g_unk52F910.y <= -120596)
				{
					v10 = 16 * (int32_t)Renderer::g_frameDelta;
					v11 = 16 * (int32_t)Renderer::g_frameDelta + g_unk52F8C4;
					g_unk52F8C4 = v11;

					if (v11 <= 512)
						goto LABEL_73;

					v11 = 512;
				}
				else
				{
					v10 = 16 * (int32_t)Renderer::g_frameDelta;
					v11 = g_unk52F8C4 - 16 * (int32_t)Renderer::g_frameDelta;
					g_unk52F8C4 = v11;

					if (v11 >= -512)
						goto LABEL_73;

					v11 = -512;
				}

				g_unk52F8C4 = v11;
			LABEL_73:

				if (abs(g_unk52F910.y + 118000) < 20000 && (g_unk52F938 & 1) != 0)
				{
					v12 = g_unk52F8C0 - v10;
					g_unk52F8C0 -= v10;

					if (g_unk52F8C0 < -700)
					{
						v12 = -700;
						g_unk52F8C0 = -700;
					}
				}
				else
				{
					v12 = g_unk52F8C0;
				}

				y = (int32_t)Renderer::g_frameDelta * v11 + g_unk52F910.y;
				x = (int32_t)Renderer::g_frameDelta * v12 + g_unk52F910.x;

				g_unk52F910.x = x;
				z = (int32_t)Renderer::g_frameDelta * g_unk52F8C8 + g_unk52F910.z;
				g_unk52F910.y = y;
				g_unk52F910.z = z;

				if (x < -1084753)
				{
					Nu3D::Link::SetScaleFromFixedOffsets(12, 5500, 5500, 5500);
					Nu3D::Link::SetScaleFromFixedOffsets(13, 5500, 5500, 5500);
					g_unk52F91C = -1;
					AudioManager::PlaySoundEffect(11, &g_unk52F910);
					z = g_unk52F910.z;
					x = g_unk52F910.x;
					y = g_unk52F910.y;
				}

				Nu3D::Link::SetPositionRawAndCommit(12, x >> 5, y >> 5, z >> 5);

				Nu3D::Link::SetPositionRawAndCommit(13, g_unk52F910.x >> 7, g_unk52F910.y >> 7, g_unk52F910.z >> 7);
				goto LABEL_81;
			}

			if (v6 >= 0)
			{
				Levels::g_recordData[61]->data[1].x = 3;
			}
			else
			{
				g_unk52F910.x = -934963;
				g_unk52F910.y = -50596;
				g_unk52F910.z = -701229;

				g_unk52F91C = 0;
				g_unk52F8C0 = 0;
				g_unk52F8C4 = 0;
				g_unk52F8C8 = 0;

				Nu3D::Link::SetScaleFromFixedOffsets(12, 0, 0, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(13, 0, 0, 0);
				Nu3D::Link::SetPositionRawAndCommit(12, g_unk52F910.x >> 5, g_unk52F910.y >> 5, g_unk52F910.z >> 5);
				Nu3D::Link::SetPositionRawAndCommit(13, g_unk52F910.x >> 7, g_unk52F910.y >> 7, g_unk52F910.z >> 7);

				Levels::g_recordData[61]->data[1].x = 3;

				if (Buzz::g_unk53C668 > 0 && ! Buzz::g_unk53C5E4)
				{
					Buzz::g_unk53C668 = 0;
					g_buzzActor.unkVar37 = 2;
					g_buzzActor.collisionFlags = 0;
					g_buzzActor.unkShort40 = 0;
				}
			}
		LABEL_90:

			if (g_unk52F92C > 0)
			{
				v16 = Numerics::g_sinCosLUT[(3 * g_unk52F92C) & 0xFFF] / 256 + 4096;
				g_unk52F92C += 32 * (int32_t)Renderer::g_frameDelta;

				if (g_unk52F92C < 4096)
				{
					Nu3D::Link::SetScaleFromFixedOffsets(18, v16 * g_unk52F92C / 4096, g_unk52F92C, v16 * g_unk52F92C / 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(19, v16 * g_unk52F92C / 4096, g_unk52F92C, v16 * g_unk52F92C / 4096);

					Nu3D::Link::SetRotationRelative8bit(18, 0, Numerics::g_sinCosLUT[g_unk52F92C & 0xFFF] / 128, 0);
					Nu3D::Link::SetRotationRelative8bit(19, 0, Numerics::g_sinCosLUT[g_unk52F92C & 0xFFF] / 128, 0);

					Levels::g_recordData[61]->data[3].x = 3;
				LABEL_109:

					if (Buzz::g_unk53C668 > 0 && Buzz::g_unk53C5E4 == 6)
					{
						g_buzzActor.posAngles.pos.y += g_unk52F920.y + Levels::g_recordData[61]->data[3].y - Levels::g_recordData[61]->data[2].y;

						g_buzzActor.posAngles.pos.x = g_unk52F920.x;
						g_buzzActor.posAngles.pos.z = g_unk52F920.z;

						Levels::g_recordData[61]->data[3].x = 3;
					}

					Levels::g_recordData[61]->data[2].x = g_unk52F920.x;
					Levels::g_recordData[61]->data[2].y = g_unk52F920.y + Levels::g_recordData[61]->data[3].y;
					Levels::g_recordData[61]->data[2].z = g_unk52F920.z;

					goto LABEL_118;
				}

				if (g_unk52F920.y > -60596)
					Levels::g_recordData[61]->data[3].x = 3;

				Nu3D::Link::SetScaleFromFixedOffsets(18, v16, 4096, v16);
				Nu3D::Link::SetScaleFromFixedOffsets(19, v16, 4096, v16);

				Nu3D::Link::SetRotationRelative8bit(18, 0, Numerics::g_sinCosLUT[g_unk52F92C & 0xFFF] / 128, 0);
				Nu3D::Link::SetRotationRelative8bit(19, 0, Numerics::g_sinCosLUT[g_unk52F92C & 0xFFF] / 128, 0);

				if (g_unk52F920.y <= -120596)
				{
					v17 = 16 * (int32_t)Renderer::g_frameDelta;
					v18 = 16 * (int32_t)Renderer::g_frameDelta + g_unk52F8DC;
					g_unk52F8DC = v18;

					if (v18 <= 512)
						goto LABEL_101;

					v18 = 512;
				}
				else
				{
					v17 = 16 * (int32_t)Renderer::g_frameDelta;
					v18 = g_unk52F8DC - 16 * (int32_t)Renderer::g_frameDelta;
					g_unk52F8DC = v18;

					if (v18 >= -512)
						goto LABEL_101;

					v18 = -512;
				}

				g_unk52F8DC = v18;
			LABEL_101:

				if (abs(g_unk52F920.y + 118000) < 20000 && (g_unk52F938 & 1) != 0)
				{
					v19 = g_unk52F8D8 - v17;
					g_unk52F8D8 -= v17;

					if (g_unk52F8D8 < -700)
					{
						v19 = -700;
						g_unk52F8D8 = -700;
					}
				}
				else
				{
					v19 = g_unk52F8D8;
				}

				v20 = (int32_t)Renderer::g_frameDelta * v18 + g_unk52F920.y;
				v21 = (int32_t)Renderer::g_frameDelta * v19 + g_unk52F920.x;
				g_unk52F920.x = v21;
				v22 = (int32_t)Renderer::g_frameDelta * g_unk52F8E0 + g_unk52F920.z;

				g_unk52F920.y = v20;
				g_unk52F920.z = v22;

				if (v21 < -1084753)
				{
					Nu3D::Link::SetScaleFromFixedOffsets(18, 5500, 5500, 5500);
					Nu3D::Link::SetScaleFromFixedOffsets(19, 5500, 5500, 5500);
					g_unk52F92C = -1;
					AudioManager::PlaySoundEffect(11, &g_unk52F910);

					v22 = g_unk52F920.z;
					v21 = g_unk52F920.x;
					v20 = g_unk52F920.y;
				}

				Nu3D::Link::SetPositionRawAndCommit(18, v21 >> 5, v20 >> 5, v22 >> 5);

				Nu3D::Link::SetPositionRawAndCommit(19, g_unk52F920.x >> 7, g_unk52F920.y >> 7, g_unk52F920.z >> 7);
				goto LABEL_109;
			}

			if (g_unk52F92C >= 0)
			{
				Levels::g_recordData[61]->data[3].x = 3;
			}
			else
			{
				g_unk52F920.x = -934963;
				g_unk52F920.y = -50596;
				g_unk52F920.z = -701229;

				g_unk52F92C = 0;
				g_unk52F8D8 = 0;
				g_unk52F8DC = 0;
				g_unk52F8E0 = 0;

				Nu3D::Link::SetScaleFromFixedOffsets(18, 0, 0, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(19, 0, 0, 0);
				Nu3D::Link::SetPositionRawAndCommit(18, g_unk52F920.x >> 5, g_unk52F920.y >> 5, g_unk52F920.z >> 5);
				Nu3D::Link::SetPositionRawAndCommit(19, g_unk52F920.x >> 7, g_unk52F920.y >> 7, g_unk52F920.z >> 7);

				Levels::g_recordData[61]->data[3].x = 3;

				if (Buzz::g_unk53C668 > 0 && Buzz::g_unk53C5E4 == 6)
				{
					Buzz::g_unk53C668 = 0;
					g_buzzActor.unkVar37 = 2;
					g_buzzActor.collisionFlags = 0;
					g_buzzActor.unkShort40 = 0;
				}
			}
		LABEL_118:

			Nu3D::Link::GetCurrentPosFixed(14, &a2);
			Nu3D::Link::SetPositionRawAndCommit(15, a2.x >> 7, a2.y >> 7, a2.z >> 7);

			v23 = g_unk52F930 - (int32_t)Renderer::g_frameDelta < 0;
			Buzz::g_underwaterYLevel = g_buzzActor.posAngles.pos.z >= 0xF32A0 ? 0x70000 : 0x10000;
			g_unk52F930 -= (int32_t)Renderer::g_frameDelta;

			if (v23)
			{
				v24 = g_unk52F980 + 2;
				g_unk52F980 = v24;

				if (v24 >= Levels::g_recordData[62]->recordCount)
				{
					v24 = 0;
					g_unk52F980 = 0;
				}

				v25 = Nu3D::Particles::SpawnInstance(
					Levels::g_recordData[62]->data[v24].x,
					Levels::g_recordData[62]->data[v24].y,
					Levels::g_recordData[62]->data[v24].z,
					(Levels::g_recordData[62]->data[v24 + 1].x - Levels::g_recordData[62]->data[v24].x) >> 6,
					(Levels::g_recordData[62]->data[v24 + 1].y - Levels::g_recordData[62]->data[v24].y) >> 6,
					(Levels::g_recordData[62]->data[v24 + 1].z - Levels::g_recordData[62]->data[v24].z) >> 6,
					0,
					0,
					0,
					76);

				v25->lifetime = 128;

				if (v25 != &Nu3D::Particles::g_emptyParticleStruct)
					AudioManager::PlaySoundEffect(164, &v25->pos);

				g_unk52F930 = 20;
			}

			Renderer::BlitTextureByIndexOffset(5, 192, 128, 64, 64, 0, g_tickAccumulators.tick64Acc, 0, 64);

			if (g_curSectorCopy1 == 2 && g_tickState[10])
			{
				if (--g_unk52F934 >= 0)
				{
					if (g_unk52F934 < 5)
					{
						v26 = Nu3D::Particles::SpawnFromPreset(169520, -677472, -191238, 93, 14);

						if (v26 != &Nu3D::Particles::g_emptyParticleStruct)
							AudioManager::PlaySoundEffect(164, &v26->pos);
					}
				}
				else
				{
					g_unk52F934 = 10;
				}
			}

			v27 = g_unk52F8E8;

			if (! g_unk52F8E8)
			{
				if (! g_buzzActor.collisionFlags || g_curSectorCopy1 != 2)
					goto LABEL_146;

				g_unk52F8E8 = 1;
				Dialogue::Begin(3, 13, "ha ha ha ha ... defeat the ^ clown top^ boss to get a pizza planet ^token^!", -1, 0, -1);
				v27 = g_unk52F8E8;
			}

			if (v27 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_146;

				g_unk830D44 = 180;
				g_unk52F8E8 = 2;

				g_levelActors[3].movementData = Actor::g_movementData[28] + 14;
				g_levelActors[3].unkShort1 = 0;
				g_levelActors[3].creatureRam->initialFacingAngle = 0;

				v27 = g_unk52F8E8;
			}

			if (v27 == 2)
			{
				if (! g_levelActors[3].creatureId)
				{
					g_unk52B7E0 = 0;
					g_unk52F8E8 = (int32_t)Renderer::g_frameDelta + 3;
				}
			}
			else if (v27 >= 3)
			{
				if (v27 >= 120)
				{
					if (v27 != 200)
					{
						Collectables::Activate(4, 0);
						g_unk52F8E8 = 200;
					}
				}
				else
				{
					g_unk52F8E8 = (int32_t)Renderer::g_frameDelta + v27;
				}
			}
		LABEL_146:

			if (g_unk52F8EC)
			{
				if (Buzz::g_curMoveableObject == 3)
					Buzz::g_curMoveableObject = 0;

				if (g_unk52F8EC < 1024)
				{
					g_unk52F8EC += (int32_t)Renderer::g_frameDelta * g_unk52F950;
					v28 = g_unk52F8EC;

					if (g_unk52F8EC >= 1024)
					{
						g_unk52F8EC = 1024;

						a2.x = 336692;
						a2.y = -3158;
						a2.z = 368662;

						v29 = 20;

						do
						{
							v30 = Nu3D::Particles::SpawnFromPreset(a2.x, a2.y, a2.z, 13, 14);
							v30->unkByte1 = 2 * ((*g_randDatBufferPtr & 3) + 3);
							--v29;
							++g_randDatBufferPtr;
							v30->lifetime = 5 * v30->unkByte1;
						} while (v29);

						AudioManager::PlaySoundEffect(52, &a2);
						AudioManager::PlaySoundEffect(58, &a2);

						Nu3D::Link::SetScaleFromFixedOffsets(41, 0, 0, 0);
						Nu3D::Link::SetScaleFromFixedOffsets(42, 0, 0, 0);

						v28 = g_unk52F8EC;
					}

					Nu3D::Link::SetRotationRelative8bit(2, 0, 0, v28);
					Nu3D::Link::SetRotationRelative8bit(3, 0, 0, g_unk52F8EC);
					g_unk52F950 += (int32_t)Renderer::g_frameDelta;
				}
			}
			else
			{
				if (MoveableObject::g_moveableObjects[2].segProgress)
				{
					g_unk52F8EC = 1;

					Platform::DisableCollision(18);
					Collision::MarkPlatformAsMoving(19);

					g_buzzActor.velX = 0;
					g_buzzActor.velForward = 0;
					Buzz::g_curMoveableObject = 0;
				}

				MoveableObject::g_moveableObjects[1].segProgress = 0;
			}

			Nu3D::Link::GetCurrentPosFixed(0, &a2);
			Nu3D::Link::SetPositionRawAndCommit(1, a2.x >> 7, a2.y >> 7, a2.z >> 7);

			Nu3D::Link::GetCurrentPosFixed(51, &a2);
			Nu3D::Link::SetPositionRawAndCommit(52, a2.x >> 7, a2.y >> 7, a2.z >> 7);

			Actor::CollectQuestReward(20, 15, -1, 0, 0);
			Actor::RotatingHint(34, 20, g_unk4F2184);

			if ((g_levelActors[19].actorFlags & 0x200) != 0)
			{
				g_levelActors[19].actorFlags &= ~0x200u;

				if (g_collectionCount >= 0)
				{
					if (g_collectionCount == 5)
					{
						Dialogue::Begin(19, 16, "thanks for finding my ^baby ducklings^ buzz! here is a pizza planet ^token^!", -1, 0, 1);
						g_collectionCount = -1;
					}
					else
					{
						Dialogue::Begin(
							19,
							16,
							"hi buzz! i have lost all my ^baby ducklings^! if you can find all ^five^ of them and come and see me, i will give you a pizza "
							"planet ^token^.",
							-1,
							0,
							-1);
					}
				}
			}

			Actor::PlayPeriodicHintSound(18, 181);

			if ((g_levelActors[18].actorFlags & 0x200) != 0)
			{
				g_levelActors[18].actorFlags &= ~0x200u;

				if (g_unk52F2F8)
				{
					if (g_unk830D4C >= 5)
					{
						if (g_unk52F2F8 != 3)
						{
							AudioManager::Preset::PlayOneShotSound2(183, &g_levelActors[18]);
							Dialogue::Begin(18, 14, "well done! you found all of my bones! here is your pizza planet ^token^!", -1, 0, 2);
							g_unk52F2F8 = 3;
							goto LABEL_180;
						}
					}
					else
					{
						Dialogue::Begin(18, 14, "quick! you need to find more bones!", -1, 0, -1);
					}
				}
				else
				{
					AudioManager::Preset::PlayOneShotSound2(182, &g_levelActors[18]);
					Dialogue::Begin(
						18, 14, "howdy buzz! if you can bring me ^five^ bones before you run out of time, i will give you a pizza planet ^token^.", -1, 0, -1);

					g_unk830D4C = 0;
					g_unk52F2F8 = 1;

					v31 = 0;

					do
					{
						*g_unk52F954[v31].yPtr = g_unk52F954[v31].savedY;
						Nu3D::Link::SetScaleFromFixedOffsets(v31 + 97, 4096, 4096, 4096);
						++v31;
					} while (v31 < 5);
				}
			}

			if (g_unk52F2F8 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_180;

				g_unk52F2F8 = 2;
				g_unk52AD64 = 130;
			}
			else if (g_unk52F2F8 != 2)
			{
				goto LABEL_180;
			}

			if (g_tickState[11])
				--g_unk52AD64;

			if (g_unk52AD64 < 100)
			{
				g_unk52AD64 = 100;
				g_unk52F2F8 = 0;

				v33 = 0;

				do
				{
					*g_unk52F954[v33].yPtr = 0x80000000;
					Nu3D::Link::SetScaleFromFixedOffsets(v33 + 97, 0, 0, 0);
					++v33;
				} while (v33 < 5);
			}
		LABEL_180:

			if (Buzz::g_currentFluidType == 1)
			{
				if (Camera::g_activeCameraTransform.pos.y <= Buzz::g_underwaterYLevel)
				{
					Nu3D::Link::SetScaleFromFixedOffsets(39, 4096, 4096, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(40, 4096, 4096, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(44, 4096, 4096, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(49, 4096, 4096, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(50, 4096, 4096, 4096);
				}
				else
				{
					Nu3D::Link::SetScaleFromFixedOffsets(39, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(40, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(44, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(49, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(50, 0, 0, 0);
				}
			}

			Weather::StepPrecipitation(256, 4096, 52);
			v35 = Levels::g_recordData[17];

			v47 = 0x7FFFFFFF;
			v46 = 0;

			if (! Levels::g_recordData[17]->recordCount)
				goto LABEL_192;

			do
			{
				v37 = 32 * v35->data[v46].z;
				v38 = 32 * v35->data[v46].y;
				v39 = 32 * v35->data[v46].x;

				if (((Camera::g_activeCameraTransform.pos.x - v39) >> 8) * ((Camera::g_activeCameraTransform.pos.x - v39) >> 8)
						+ ((Camera::g_activeCameraTransform.pos.y - v38) >> 8) * ((Camera::g_activeCameraTransform.pos.y - v38) >> 8)
						+ ((Camera::g_activeCameraTransform.pos.z - v37) >> 8) * ((Camera::g_activeCameraTransform.pos.z - v37) >> 8)
					< 1000000)
				{
					Renderer::LensFlare::RegisterLight(v39, v38, v37, 64, 48, 32, 128);
					if (((g_buzzActor.posAngles.pos.y - v38 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v38 - 0x2000) >> 8)
							+ ((g_buzzActor.posAngles.pos.z - v37) >> 8) * ((g_buzzActor.posAngles.pos.z - v37) >> 8)
							+ ((g_buzzActor.posAngles.pos.x - v39) >> 8) * ((g_buzzActor.posAngles.pos.x - v39) >> 8)
						< v47)
					{
						v47 = ((g_buzzActor.posAngles.pos.y - v38 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v38 - 0x2000) >> 8)
							+ ((g_buzzActor.posAngles.pos.z - v37) >> 8) * ((g_buzzActor.posAngles.pos.z - v37) >> 8)
							+ ((g_buzzActor.posAngles.pos.x - v39) >> 8) * ((g_buzzActor.posAngles.pos.x - v39) >> 8);
						v49 = v46;
					}
				}

				v35 = Levels::g_recordData[17];
				v40 = ++v46 < Levels::g_recordData[17]->recordCount;
			} while (v40);

			if (v47 < 0x10000)
			{
				Lighting::g_dynamicLights[1].pos.x = 32 * Levels::g_recordData[17]->data[v49].x;
				Lighting::g_dynamicLights[1].pos.y = 32 * Levels::g_recordData[17]->data[v49].y;

				v42 = Levels::g_recordData[17]->data[v49].z;

				Lighting::g_dynamicLights[1].colorR = 0x80;
				Lighting::g_dynamicLights[1].pos.z = 32 * v42;
				Lighting::g_dynamicLights[1].colorG = 96;
				Lighting::g_dynamicLights[1].colorB = 64;
				Lighting::g_dynamicLights[1].lifetime = 1;
				Lighting::g_dynamicLights[1].groupId = (int32_t)&Levels::g_recordData[17]->data[v49];
			}
			else
			{
			LABEL_192:
				Lighting::g_dynamicLights[1].lifetime = 0;
			}

			if (g_tickState[8] && (Buzz::g_currentFluidType != 1 || Buzz::g_underwaterYLevel - Camera::g_activeCameraTransform.pos.y >= 0))
			{
				v43 = Numerics::g_sinCosLUT[Camera::g_activeCameraTransform.angles.yaw];
				a2.x = v43 + Camera::g_activeCameraTransform.pos.x + 2 * v43 + ((*g_randDatBufferPtr - 128) << 8);
				v44 = Numerics::g_sinCosLUT[(Camera::g_activeCameraTransform.angles.yaw + 1024) & 0xFFF];

				a2.y = Camera::g_activeCameraTransform.pos.y - 0x8000;
				a2.z = v44 + Camera::g_activeCameraTransform.pos.z + 2 * v44 + ((*++g_randDatBufferPtr - 128) << 8);
				++g_randDatBufferPtr;

				l_groundHeight = Nu3D::Collision::GetGroundHeight((WorldPos*)&a2, 0);

				if (l_groundHeight != 0x80000000)
				{
					if (l_groundHeight > Buzz::g_underwaterYLevel)
						l_groundHeight = Buzz::g_underwaterYLevel;

					if (l_groundHeight > Camera::g_activeCameraTransform.pos.y)
						Nu3D::Particles::SpawnFromPreset(a2.x, l_groundHeight, a2.z, 27, 2);
				}
			}

			PlayLevelMusic();
		}

		// FUNCTION: TOY2 0x0041E390
		void Init()
		{
			Vector3I l_vec;

			Weather::Init();
			MoveableObject::InitTable(g_unk4F215C);
			Collectables::Init(g_unk4F2178, 65);
			Collectables::Activate(3, 1);
			InitHiddenCollectibles();
			g_unk52F8A8 = 245760;
			g_unk52F97C = 245760;
			g_unk52F948 = 245760;
			g_unk52F940 = 245760;
			g_unk52F908 = 245760;
			g_unk52F8F4 = 245760;
			g_unk52F8B4 = 0;
			g_unk52F8AC = 0;
			g_unk52F984 = 0;
			g_unk52F94C = 0;
			g_unk52F944 = 0;
			g_unk52F900 = 0;
			InterpolatePathPoint(0, 0, &l_vec);
			Platform::SetOrigin(8, 32 * l_vec.x, 32 * l_vec.y, 32 * l_vec.z);
			Nu3D::Link::SetPositionRawAndCommit(33, l_vec.x, l_vec.y, l_vec.z);
			Nu3D::Link::SetPositionRawAndCommit(29, l_vec.x, l_vec.y, l_vec.z);
			InterpolatePathPoint(0, g_unk52F8A8, &l_vec);
			Platform::SetOrigin(5, 32 * l_vec.x, 32 * l_vec.y, 32 * l_vec.z);
			Nu3D::Link::SetPositionRawAndCommit(34, l_vec.x, l_vec.y, l_vec.z);
			Nu3D::Link::SetPositionRawAndCommit(30, l_vec.x, l_vec.y, l_vec.z);
			InterpolatePathPoint(5, g_unk52F8AC, &l_vec);
			Platform::SetOrigin(6, 32 * l_vec.x, 32 * l_vec.y, 32 * l_vec.z);
			Nu3D::Link::SetPositionRawAndCommit(35, l_vec.x, l_vec.y, l_vec.z);
			Nu3D::Link::SetPositionRawAndCommit(31, l_vec.x, l_vec.y, l_vec.z);
			InterpolatePathPoint(5, g_unk52F97C, &l_vec);
			Platform::SetOrigin(7, 32 * l_vec.x, 32 * l_vec.y, 32 * l_vec.z);
			Nu3D::Link::SetPositionRawAndCommit(36, l_vec.x, l_vec.y, l_vec.z);
			Nu3D::Link::SetPositionRawAndCommit(32, l_vec.x, l_vec.y, l_vec.z);
			InterpolatePathPoint(2, g_unk52F944, &l_vec);
			Platform::SetOrigin(9, 32 * l_vec.x, 32 * l_vec.y, 32 * l_vec.z);
			Nu3D::Link::SetPositionRawAndCommit(21, l_vec.x, l_vec.y, l_vec.z);
			InterpolatePathPoint(2, g_unk52F908, &l_vec);
			Platform::SetOrigin(10, 32 * l_vec.x, 32 * l_vec.y, 32 * l_vec.z);
			Nu3D::Link::SetPositionRawAndCommit(22, l_vec.x, l_vec.y, l_vec.z);
			InterpolatePathPoint(3, g_unk52F900, &l_vec);
			Platform::SetOrigin(11, 32 * l_vec.x, 32 * l_vec.y, 32 * l_vec.z);
			Nu3D::Link::SetPositionRawAndCommit(23, l_vec.x, l_vec.y, l_vec.z);
			InterpolatePathPoint(3, g_unk52F8F4, &l_vec);
			Platform::SetOrigin(12, 32 * l_vec.x, 32 * l_vec.y, 32 * l_vec.z);
			Nu3D::Link::SetPositionRawAndCommit(24, l_vec.x, l_vec.y, l_vec.z);
			g_unk52F8EC = 0;
			g_unk52F950 = 0;
			Platform::DisableCollision(19);
			g_unk52F938 = 0;
			g_unk52F93C = 0;
			g_unk52F988 = 0;
			g_unk52F904 = 0;
			g_unk52F8D4 = 0;
			g_unk52F90C = 0;
			g_unk52F8D0 = 0;
			g_unk52F8F8 = 0;
			g_unk52F910.x = -934963;
			g_unk52F910.y = -50596;
			g_unk52F910.z = -701229;
			g_unk52F91C = 1;
			g_unk52F8C0 = 0;
			g_unk52F8C4 = 0;
			g_unk52F8C8 = 0;
			Nu3D::Link::SetPositionRawAndCommit(12, -29218, -1582, -21914);
			Nu3D::Link::SetScaleFromFixedOffsets(12, 0, 0, 0);
			Nu3D::Link::SetPositionRawAndCommit(13, g_unk52F910.x >> 7, g_unk52F910.y >> 7, g_unk52F910.z >> 7);
			Nu3D::Link::SetScaleFromFixedOffsets(13, 0, 0, 0);
			g_unk52F920.x = -934963;
			g_unk52F920.y = -50596;
			g_unk52F920.z = -701229;
			g_unk52F92C = 0;
			g_unk52F8D8 = 0;
			g_unk52F8DC = 0;
			g_unk52F8E0 = 0;
			Nu3D::Link::SetPositionRawAndCommit(18, -29218, -1582, -21914);
			Nu3D::Link::SetScaleFromFixedOffsets(18, 0, 0, 0);
			Nu3D::Link::SetPositionRawAndCommit(19, g_unk52F920.x >> 7, g_unk52F920.y >> 7, g_unk52F920.z >> 7);
			Nu3D::Link::SetScaleFromFixedOffsets(19, 0, 0, 0);
			Buzz::g_underwaterYLevel = 0x10000;
			Buzz::g_unk830D30 = 0x10000;
			g_unk52F930 = 0;
			g_unk52F934 = 0;
			g_unk52F980 = 0;
			g_unk52F8E8 = 0;
			g_unk52F8FC = 0;
			g_unk52F8B0 = 0;
			g_unk52F2F8 = 0;
			g_unk52AD64 = 0;
			Buzz::g_currentFluidType = 1;
			g_underwaterTintBlue = 96;
			g_underwaterTintGreen = 120;
			g_underwaterTintRed = 0x80;
			g_unk52F8B8 = g_levelActors[3].actorPhase;
			g_unk52F8F0 = 200;
		}

	}
}

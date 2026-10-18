#include "Numerics.h"
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
#include "Toy2/Levels/ElevatorHop.h"

namespace Toy2
{
	namespace ElevatorHop
	{
		// GLOBAL: TOY2 0x004F35B4
		int16_t g_unk4F35B4[14] = { 24, 28, 29, 30, 31, 49, 52, 53, 54, 58, 59, 60, 74, -1 };

		// GLOBAL: TOY2 0x004F35D0
		int16_t g_unk4F35D0[14] = { 25, 32, 33, 34, 35, 51, 55, 56, 57, 61, 62, 63, 76, -1 };

		// GLOBAL: TOY2 0x004F35EC
		int16_t g_unk4F35EC[10] = { 26, 36, 37, 38, 39, 48, 64, 65, 66, -1 };

		// GLOBAL: TOY2 0x004F3600
		int16_t g_unk4F3600[10] = { 27, 40, 41, 42, 43, 50, 67, 68, 69, -1 };

		// GLOBAL: TOY2 0x004F37D8
		char* g_unk4F37D8[5] = {
			"^hamm^ is in the ^electrical room^. use the ^pole swings^ to get to him. watch out fot the electricity!",
			"^mother mouse^ has lost her baby mice. she is in the control room farther up the ventilation shafts.",
			"^father mouse^ is at the top of the elevator shaft. he has a ^challenge^ for you!",
			"you can activate the elevators by solving the ^wire puzzle^ in the control room.",
			"the ^spider gunro^ boss is in a room above the elevator shafts."
		};

		// GLOBAL: TOY2 0x004F37EC
		Vector3I g_unk4F37EC[5] = { { 0, 0, 256 }, { 256, 0, 0 }, { 0, 0, -256 }, { -256, 0, 0 }, { 0, -256, 0 } };

		// GLOBAL: TOY2 0x0052FC5C
		int32_t g_unk52FC5C;

		// GLOBAL: TOY2 0x004F3614
		int16_t g_unk4F3614[9] = { 1, 2, 3, 6, 7, 8, 9, 10, -1 };

		// GLOBAL: TOY2 0x004F3628
		int16_t g_unk4F3628[9] = { 0, 11, 12, 13, 14, 15, 16, 17, -1 };

		// GLOBAL: TOY2 0x004F3688
		int32_t g_unk4F3688[21] = {
			65544,	65545, 131080, 196612,	1179648, 131081, 65544, 65545,	 8323075, 524384, 589825,
			524289, 65538, 0,	   1179648, 131081,	 65544,	 65545, 8323075, 32,	  39
		};

		// GLOBAL: TOY2 0x004F36DC
		int32_t g_unk4F36DC[41] = {
			16777224, 16777225, 33554440, 196612,	786433,	 33554441, 16777224, 16777225, 8323075, 524320,	 590080, 524544,  66048, 0,
			786432,	  33554441, 16777224, 16777225, 8323075, 32,	   39,		 -271682,  -64262,	-1024,	 -1536,	 -271682, -1094, -1536,
			1536,	  -384898,	-980742,  -512,		1536,	 -320066,  -64326,	 -1536,	   1024,	-271682, -1094,	 -1024,	  -1024
		};

		// GLOBAL: TOY2 0x004F3798
		int16_t g_unk4F3798[6] = { 107, 108, 110, 106, 109, 0 };

		// GLOBAL: TOY2 0x004F37A4
		Collectables::TokenTableEntry g_unk4F37A4[4] = {
			{ 112, 34, "if you line up the ^wire^ you will open the ^door^ and activate the ^elevators^.", 0 },
			{ 113, 35, "the switch in the control room will activate this ^shortcut^ fan.", 0 },
			{ 114, 36, "the switch at the top of the elevator shaft will activate this ^shortcut^ fan.", 0 },
			{ -1, 0, 0, 0 },
		};

		// GLOBAL: TOY2 0x0052FC78
		int32_t g_unk52FC78[10];

		// GLOBAL: TOY2 0x0052FCA0
		int32_t g_unk52FCA0[10];

		// GLOBAL: TOY2 0x0052FC10
		int32_t g_unk52FC10;

		// GLOBAL: TOY2 0x0052FCDC
		int32_t g_unk52FCDC;

		// GLOBAL: TOY2 0x0052FC44
		int32_t g_unk52FC44;

		// GLOBAL: TOY2 0x0052FCC8
		int32_t g_unk52FCC8;

		// GLOBAL: TOY2 0x0052FC1C
		int32_t g_unk52FC1C;

		// GLOBAL: TOY2 0x0052FC28
		int32_t g_unk52FC28;

		// GLOBAL: TOY2 0x0052FC38
		int32_t g_unk52FC38;

		// GLOBAL: TOY2 0x0052FD00
		int32_t g_unk52FD00;

		// GLOBAL: TOY2 0x0052FC54
		int32_t g_unk52FC54;

		// GLOBAL: TOY2 0x0052FC6C
		int32_t g_unk52FC6C;

		// GLOBAL: TOY2 0x0052FC2C
		int32_t g_unk52FC2C;

		// GLOBAL: TOY2 0x0052FC58
		int32_t g_unk52FC58;

		// GLOBAL: TOY2 0x0052FC64
		int32_t g_unk52FC64;

		// GLOBAL: TOY2 0x0052FC74
		int32_t g_unk52FC74;

		// GLOBAL: TOY2 0x0052FC60
		int32_t g_unk52FC60;

		// GLOBAL: TOY2 0x0052FC70
		int32_t g_unk52FC70;

		// GLOBAL: TOY2 0x0052FCFC
		int32_t g_unk52FCFC;

		// GLOBAL: TOY2 0x0052FC18
		int32_t g_unk52FC18;

		// GLOBAL: TOY2 0x0052FC68
		int32_t g_unk52FC68;

		// GLOBAL: TOY2 0x0052FCEC
		int32_t g_unk52FCEC;

		// GLOBAL: TOY2 0x0052FCD0
		int32_t g_unk52FCD0;

		// GLOBAL: TOY2 0x0052FC14
		int32_t g_unk52FC14;

		// GLOBAL: TOY2 0x0052FC50
		int32_t g_unk52FC50;

		// GLOBAL: TOY2 0x0052FC30
		int32_t g_unk52FC30;

		// GLOBAL: TOY2 0x0052FCD8
		int32_t g_unk52FCD8;

		// GLOBAL: TOY2 0x0052FC24
		int32_t g_unk52FC24;

		// GLOBAL: TOY2 0x0052FCCC
		int32_t g_unk52FCCC;

		// GLOBAL: TOY2 0x0052FCE8
		int32_t g_unk52FCE8;

		// GLOBAL: TOY2 0x0052FCD4
		int32_t g_unk52FCD4;

		// GLOBAL: TOY2 0x0052FCE4
		int32_t g_unk52FCE4;

		// GLOBAL: TOY2 0x0052FC3C
		int32_t g_unk52FC3C;

		// GLOBAL: TOY2 0x0052FCF4
		int32_t* g_unk52FCF4;

		// GLOBAL: TOY2 0x0052FCF0
		int32_t* g_unk52FCF0;

		// GLOBAL: TOY2 0x004F3780
		int32_t g_unk4F3780[6] = { 400, 40, 40, 40, 40, 200 };

		// GLOBAL: TOY2 0x0052FC20
		int32_t g_unk52FC20;

		// GLOBAL: TOY2 0x0052FC34
		int32_t g_unk52FC34;

		// GLOBAL: TOY2 0x0052FC40
		int32_t g_unk52FC40;

		// GLOBAL: TOY2 0x0052FC48
		int32_t g_unk52FC48;

		// GLOBAL: TOY2 0x0052FC4C
		int32_t g_unk52FC4C;

		// GLOBAL: TOY2 0x0052FCE0
		int32_t g_unk52FCE0;

		// GLOBAL: TOY2 0x0052FCF8
		int32_t g_unk52FCF8;

		// FUNCTION: TOY2 0x00425680
		void Method2()
		{
			Nu3D::Link::SetPositionRawAndCommit(
				18, Levels::g_recordData[9]->data[g_unk52FC50].x, Levels::g_recordData[9]->data[g_unk52FC50].y, Levels::g_recordData[9]->data[g_unk52FC50].z);
			Nu3D::Link::SetPositionRawAndCommit(
				19,
				Levels::g_recordData[9]->data[g_unk52FC30 + 3].x,
				Levels::g_recordData[9]->data[g_unk52FC30 + 3].y,
				Levels::g_recordData[9]->data[g_unk52FC30 + 3].z);
			Nu3D::Link::SetPositionRawAndCommit(
				20,
				Levels::g_recordData[9]->data[g_unk52FCD8 + 6].x,
				Levels::g_recordData[9]->data[g_unk52FCD8 + 6].y,
				Levels::g_recordData[9]->data[g_unk52FCD8 + 6].z);
		}

		// FUNCTION: TOY2 0x00425A80
		void InitMouse()
		{
			int32_t i;
			int32_t v1;

			i = 1;

			do
			{
				if (g_levelActors[i].creatureRam->latSpeedNoTarget <= 127u)
				{
					v1 = g_levelActors[i].transform.pos.z - g_levelActors[i].boundary.z;
					g_levelActors[i].transform.pos.z = g_levelActors[i].transform.pos.y + g_levelActors[i].boundary.z - g_levelActors[i].boundary.y;
					g_levelActors[i].transform.pos.y = v1 + g_levelActors[i].boundary.y;
				}
				++i;
			} while (i < 6);

			g_levelActors[3].rollAngle = 2048;
		}

		// FUNCTION: TOY2 0x00425EB0
		void Method1(Vector3I* a2, int32_t arg4, int32_t a3)
		{
			int32_t v3;
			Nu3D::Particles::ParticleInstance* v4;
			int16_t v5;

			if (arg4 == 4)
			{
				v3 = 7;
				AudioManager::g_pitchArray[0] = 5120;
				AudioManager::PlaySoundEffect(140, a2);
			}
			else
			{
				v3 = 9;
			}
			if (g_tickState[v3])
			{
				if (arg4 == 4)
					v4 = Nu3D::Particles::SpawnFromPreset(a2->x, a2->y, a2->z, 79, 2);
				else
					v4 = Nu3D::Particles::SpawnFromPreset(a2->x, a2->y, a2->z, 78, 2);
				v4->velX = a3 * g_unk4F37EC[arg4].x;
				v4->velY = a3 * g_unk4F37EC[arg4].y;
				v4->velZ = a3 * g_unk4F37EC[arg4].z;
				v5 = g_tickAccumulators.tick32Acc << 6;
				v4->rotSpeed = -256;
				v4->groundAlignRot = 4095 - v5;
			}
		}

		// FUNCTION: TOY2 0x00425F60
		void Interactions()
		{
			Levels::RecordData* v0;
			int32_t v2;
			int32_t v3;
			int32_t v4;
			bool v5;
			int32_t z;
			int32_t v8;
			int32_t v9;
			bool v10;
			Levels::RecordData* v11;
			int32_t v12;
			int32_t v13;
			Nu3D::Particles::ParticleInstance* v14;
			int32_t v15;
			Levels::RecordData* v16;
			int32_t v17;
			int32_t v18;
			Nu3D::Particles::ParticleInstance* v19;
			int32_t v20;
			Levels::RecordData* v21;
			int32_t v22;
			int32_t v23;
			Nu3D::Particles::ParticleInstance* v24;
			int32_t v25;
			int32_t v26;
			int32_t v27;
			int32_t v28;
			int32_t v29;
			int32_t i;
			int32_t v31;
			int32_t v32;
			Levels::RecordData* v33;
			int32_t v34;
			int32_t IsWithinDistance;
			int32_t v36;
			Levels::RecordData* v37;
			int32_t v38;
			int32_t v39;
			Levels::RecordData* v40;
			int32_t v41;
			int32_t v42;
			Levels::RecordData* v43;
			int32_t v44;
			int32_t v45;
			Levels::RecordData* v46;
			int32_t v47;
			int32_t v48;
			bool v49;
			int32_t v50;
			int32_t v51;
			int32_t v52;
			int32_t v53;
			int16_t* v54;
			int32_t v55;
			int32_t v56;
			int16_t* v57;
			int16_t* v58;
			int16_t v59;
			int32_t v60;
			int16_t* v61;
			int32_t* v62;
			int16_t v63;
			int16_t* v64;
			int16_t v65;
			int16_t* v66;
			int16_t* v67;
			int16_t v68;
			int16_t* v69;
			int16_t* v70;
			int16_t v71;
			int16_t* v72;
			int32_t v73;
			int16_t* v74;
			int32_t v75;
			int32_t v76;
			int16_t* v77;
			int16_t* v78;
			int16_t v79;
			int32_t v80;
			int16_t* v81;
			int32_t* v82;
			int16_t v83;
			int16_t* v84;
			int16_t v85;
			int16_t* v86;
			int16_t* v87;
			int16_t v88;
			int16_t* v89;
			int16_t* v90;
			int16_t v91;
			int16_t* v92;
			int32_t v93;
			Nu3D::Particles::ParticleInstance* v94;
			Nu3D::Particles::ParticleInstance* v95;
			int32_t v96;
			int32_t v97;
			int32_t v99;
			int32_t v100;
			int32_t v101;
			int32_t v102;
			int32_t v103;
			Nu3D::Particles::ParticleInstance* v104;
			int32_t v105;
			Nu3D::Particles::ParticleInstance* v107;
			int32_t v108;
			int32_t v109;
			Levels::RecordData* v111;
			int32_t v112;
			int32_t v113;
			int32_t v114;
			int32_t v115;
			int32_t v117;
			int32_t* p_z;
			int32_t v119;
			int32_t v120;
			int32_t v121;
			int32_t v122;
			int32_t v123;
			int32_t v124;
			int32_t v125;
			int32_t v126;
			int32_t v127;
			int32_t v128;
			int32_t v129;
			int32_t v130;
			int32_t v131;
			int32_t v132;
			int32_t v134;
			int32_t spawnZ;
			Vector3I p_v1;
			int32_t v137;
			Vector3I a2;
			Vector3I v139;

			v0 = Levels::g_recordData[13];
			v132 = 0x7FFFFFFF;
			v126 = 0;

			if (! Levels::g_recordData[13]->recordCount)
				goto LABEL_9;

			do
			{
				v2 = 32 * v0->data[v126].z;
				v3 = 32 * v0->data[v126].y;
				v4 = 32 * v0->data[v126].x;

				if (((Camera::g_activeCameraTransform.pos.x - v4) >> 8) * ((Camera::g_activeCameraTransform.pos.x - v4) >> 8)
						+ ((Camera::g_activeCameraTransform.pos.y - v3) >> 8) * ((Camera::g_activeCameraTransform.pos.y - v3) >> 8)
						+ ((Camera::g_activeCameraTransform.pos.z - v2) >> 8) * ((Camera::g_activeCameraTransform.pos.z - v2) >> 8)
					< 1000000)
				{
					Renderer::LensFlare::RegisterLight(v4, v3, v2, 128, 128, 128, 64);
					if (((g_buzzActor.posAngles.pos.y - v3 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v3 - 0x2000) >> 8)
							+ ((g_buzzActor.posAngles.pos.z - v2) >> 8) * ((g_buzzActor.posAngles.pos.z - v2) >> 8)
							+ ((g_buzzActor.posAngles.pos.x - v4) >> 8) * ((g_buzzActor.posAngles.pos.x - v4) >> 8)
						< v132)
					{
						v132 = ((g_buzzActor.posAngles.pos.y - v3 - 0x2000) >> 8) * ((g_buzzActor.posAngles.pos.y - v3 - 0x2000) >> 8)
							+ ((g_buzzActor.posAngles.pos.z - v2) >> 8) * ((g_buzzActor.posAngles.pos.z - v2) >> 8)
							+ ((g_buzzActor.posAngles.pos.x - v4) >> 8) * ((g_buzzActor.posAngles.pos.x - v4) >> 8);
						spawnZ = v126;
					}
				}

				v0 = Levels::g_recordData[13];
				v5 = ++v126 < Levels::g_recordData[13]->recordCount;
			} while (v5);

			if (v132 < 0x10000)
			{
				Lighting::g_dynamicLights[1].pos.x = 32 * Levels::g_recordData[13]->data[spawnZ].x;
				Lighting::g_dynamicLights[1].pos.y = 32 * Levels::g_recordData[13]->data[spawnZ].y;
				z = Levels::g_recordData[13]->data[spawnZ].z;
				Lighting::g_dynamicLights[1].colorR = -1;
				Lighting::g_dynamicLights[1].pos.z = 32 * z;
				Lighting::g_dynamicLights[1].colorG = -1;
				Lighting::g_dynamicLights[1].colorB = -1;
				Lighting::g_dynamicLights[1].lifetime = 1;
				Lighting::g_dynamicLights[1].groupId = (int32_t)&Levels::g_recordData[13]->data[spawnZ];
			}
			else
			{
			LABEL_9:
				Lighting::g_dynamicLights[1].lifetime = 0;
			}

			v8 = (int32_t)Renderer::g_frameDelta;
			v9 = (int32_t)Renderer::g_frameDelta + g_unk52FCD0;
			v5 = (int32_t)Renderer::g_frameDelta + g_unk52FCD0 <= 120;

			g_unk52FCD0 += (int32_t)Renderer::g_frameDelta;
			g_unk52FC5C = 0;

			if (! v5)
			{
				v10 = g_unk52FCEC == 2;
				g_unk52FC5C = 1;
				g_unk52FCD0 = v9 - 120;
				++g_unk52FCEC;

				if (v10)
					g_unk52FCEC = 0;
			}

			v11 = Levels::g_recordData[4];
			v12 = g_unk52FCEC;

			if (g_unk52FCEC < Levels::g_recordData[4]->recordCount)
			{
				v13 = g_unk52FCEC;

				do
				{
					p_v1.x = 32 * v11->data[v13].x;
					p_v1.y = 32 * v11->data[v13].y;
					p_v1.z = 32 * v11->data[v13].z;

					if (Nu3D::Math::IsWithinDistance(&p_v1, &g_buzzActor.posAngles.pos, 1152))
					{
						if (g_unk52FC5C)
						{
							Nu3D::Particles::SpawnFromPreset(p_v1.x, p_v1.y - 2048, p_v1.z, 69, 2);
							AudioManager::PlaySoundEffect(139, &p_v1);
						}

						v14 = Nu3D::Particles::SpawnFromPreset(
							p_v1.x + (Numerics::g_sinCosLUT[g_unk52FC68 & 0xFFF] >> 3),
							p_v1.y,
							p_v1.z + (Numerics::g_sinCosLUT[(g_unk52FC68 + 1024) & 0xFFF] >> 3),
							68,
							21);

						v15 = *g_randDatBufferPtr++ & 0x7F;
						g_unk52FC68 += v15 + 256;
						v14->lifetime = (*g_randDatBufferPtr++ & 0xF) + 32;
					}

					v11 = Levels::g_recordData[4];
					v12 += 3;
					v13 += 3;
				} while (v12 < Levels::g_recordData[4]->recordCount);

				v8 = (int32_t)Renderer::g_frameDelta;
			}

			v16 = Levels::g_recordData[5];
			v17 = g_unk52FCEC;

			if (g_unk52FCEC < Levels::g_recordData[5]->recordCount)
			{
				v18 = g_unk52FCEC;

				do
				{
					p_v1.x = 32 * v16->data[v18].x;
					p_v1.y = 32 * v16->data[v18].y;
					p_v1.z = 32 * v16->data[v18].z;

					if (Nu3D::Math::IsWithinDistance(&p_v1, &g_buzzActor.posAngles.pos, 1152))
					{
						if (g_unk52FC5C)
						{
							Nu3D::Particles::SpawnFromPreset(p_v1.x - 2048, p_v1.y, p_v1.z, 69, 2);
							Nu3D::Particles::SpawnFromPreset(p_v1.x - 10240, p_v1.y, p_v1.z, 69, 2);
							AudioManager::PlaySoundEffect(139, &p_v1);
						}

						v19 = Nu3D::Particles::SpawnFromPreset(
							p_v1.x,
							p_v1.y + (Numerics::g_sinCosLUT[g_unk52FC68 & 0xFFF] >> 3),
							p_v1.z + (Numerics::g_sinCosLUT[(g_unk52FC68 + 1024) & 0xFFF] >> 3),
							68,
							22);

						v20 = *g_randDatBufferPtr++ & 0x7F;
						g_unk52FC68 += v20 + 256;
						v19->lifetime = (*g_randDatBufferPtr++ & 0xF) + 32;
					}

					v16 = Levels::g_recordData[5];
					v17 += 3;
					v18 += 3;
				} while (v17 < Levels::g_recordData[5]->recordCount);

				v8 = (int32_t)Renderer::g_frameDelta;
			}

			v21 = Levels::g_recordData[6];
			v22 = g_unk52FCEC;

			if (g_unk52FCEC < Levels::g_recordData[6]->recordCount)
			{
				v23 = g_unk52FCEC;

				do
				{
					p_v1.x = 32 * v21->data[v23].x;
					p_v1.y = 32 * v21->data[v23].y;
					p_v1.z = 32 * v21->data[v23].z;

					if (Nu3D::Math::IsWithinDistance(&p_v1, &g_buzzActor.posAngles.pos, 1152))
					{
						if (g_unk52FC5C)
						{
							Nu3D::Particles::SpawnFromPreset(p_v1.x + 2048, p_v1.y, p_v1.z, 69, 2);
							Nu3D::Particles::SpawnFromPreset(p_v1.x + 10240, p_v1.y, p_v1.z, 69, 2);
							AudioManager::PlaySoundEffect(139, &p_v1);
						}

						v24 = Nu3D::Particles::SpawnFromPreset(
							p_v1.x,
							p_v1.y + (Numerics::g_sinCosLUT[g_unk52FC68 & 0xFFF] >> 3),
							p_v1.z + (Numerics::g_sinCosLUT[(g_unk52FC68 + 1024) & 0xFFF] >> 3),
							68,
							23);

						v25 = *g_randDatBufferPtr++ & 0x7F;
						g_unk52FC68 += v25 + 256;
						v24->lifetime = (*g_randDatBufferPtr++ & 0xF) + 32;
					}

					v21 = Levels::g_recordData[6];
					v22 += 3;
					v23 += 3;
				} while (v22 < Levels::g_recordData[6]->recordCount);

				v8 = (int32_t)Renderer::g_frameDelta;
			}

			g_unk52FD00 += ((v8 * g_unk52FC34) << 7) / 4096;
			g_unk52FC6C = (g_unk52FC6C + (v8 << 7)) & 0xFFF;
			g_unk52FC28 = (g_unk52FC28 + 6 * v8) & 0xFFF;

			v26 = g_unk52FC38;

			if (g_unk52FC28 <= 2560)
			{
				if (g_unk52FC38 < 128)
				{
					v26 = g_unk52FC38 + 2;
					goto LABEL_42;
				}
			}
			else if (g_unk52FC38 > 0)
			{
				v26 = g_unk52FC38 - 2;
			LABEL_42:
				g_unk52FC38 = v26;
			}

			v27 = v8 * v26 + g_unk52FC54;
			v28 = 0;
			g_unk52FC54 = v27;

			do
			{
				Nu3D::Link::SetRotationRelative8bit(v28, v27, 0, 0);
				v27 += 768;
				++v28;
			} while (v28 < 4);

			v29 = g_unk52FD00;

			for (i = 7; i < 16; ++i)
			{
				Nu3D::Link::SetRotationRelative8bit(i, 0, v29, 0);
				v29 += 768;
			}

			v31 = g_unk52FC6C;

			if ((g_unk52FC14 & 1) != 0)
				Nu3D::Link::SetRotationRelative8bit(4, 0, g_unk52FC6C, 0);

			v32 = v31 + 768;

			Nu3D::Link::SetRotationRelative8bit(6, 0, v32, 0);

			if ((g_unk52FC14 & 2) != 0)
				Nu3D::Link::SetRotationRelative8bit(5, 0, v32 + 768, 0);

			v134 = 1;

			if (g_unk52FC34)
			{
				v33 = Levels::g_recordData[8];
				v127 = 0;

				if (Levels::g_recordData[8]->recordCount)
				{
					v34 = 0;

					do
					{
						a2.x = 32 * v33->data[v34].x;
						a2.y = 32 * v33->data[v34].y;
						a2.z = 32 * v33->data[v34].z;

						IsWithinDistance = Nu3D::Math::IsWithinDistance(&a2, &g_buzzActor.posAngles.pos, 1152);
						v33 = Levels::g_recordData[8];

						if (IsWithinDistance)
						{
							if (v134)
							{
								p_v1.x = Levels::g_recordData[8]->data[v34].x / 8 - g_buzzActor.posAngles.pos.x / 256;
								p_v1.y = Levels::g_recordData[8]->data[v34].y / 8 - g_buzzActor.posAngles.pos.y / 256;
								p_v1.z = Levels::g_recordData[8]->data[v34].z / 8 - g_buzzActor.posAngles.pos.z / 256;

								v36 = g_unk52FC34 * (Levels::g_recordData[8]->data[v34].y / 8 - Levels::g_recordData[8]->data[v34 + 1].y / 8 - 192) / 4096;

								v137 = v36;

								if (p_v1.x * p_v1.x + p_v1.z * p_v1.z < 25600 && p_v1.y > 0 && p_v1.y < v36)
								{
									Buzz::g_groundSlamTicker = 0;
									Buzz::g_unk53C608 = 1;
									g_buzzActor.collisionFlags = 0;

									if (g_buzzActor.gravityVel > -2048)
									{
										v134 = 0;
										g_buzzActor.gravityVel -= (int32_t)Renderer::g_frameDelta * ((g_unk52FC34 << 7) / 4096 + 128);
									}
								}
							}
						}

						v34 += 2;
						v127 += 2;
					} while (v127 < Levels::g_recordData[8]->recordCount);
				}
			}

			if ((g_unk52FC14 & 1) != 0)
			{
				v37 = Levels::g_recordData[10];
				v38 = 0;
				v128 = 0;

				if (Levels::g_recordData[10]->recordCount)
				{
					do
					{
						a2.x = 32 * v37->data[v38].x;
						a2.y = 32 * v37->data[v38].y;
						a2.z = 32 * v37->data[v38].z;

						if (Nu3D::Math::IsWithinDistance(&a2, &g_buzzActor.posAngles.pos, 1408))
							Method1(&a2, 4, 5);

						p_v1.x = Levels::g_recordData[10]->data[v38].x / 8 - g_buzzActor.posAngles.pos.x / 256;
						p_v1.y = Levels::g_recordData[10]->data[v38].y / 8 - g_buzzActor.posAngles.pos.y / 256;
						p_v1.z = Levels::g_recordData[10]->data[v38].z / 8 - g_buzzActor.posAngles.pos.z / 256;
						v39 = Levels::g_recordData[10]->data[v38].y / 8 - Levels::g_recordData[10]->data[v38 + 1].y / 8 - 192;

						v137 = v39;

						if (p_v1.x * p_v1.x + p_v1.z * p_v1.z < 9216 && p_v1.y > 0 && p_v1.y < v39)
						{
							Buzz::g_groundSlamTicker = 0;
							Buzz::g_unk53C608 = 1;
							g_buzzActor.collisionFlags = 0;

							if (g_buzzActor.gravityVel > -4096)
								g_buzzActor.gravityVel += -128 * (int32_t)Renderer::g_frameDelta;
						}

						a2.x = 32 * Levels::g_recordData[10]->data[v38 + 1].x;
						a2.y = 32 * (Levels::g_recordData[10]->data[v38 + 1].y + 1500);
						a2.z = 32 * Levels::g_recordData[10]->data[v38 + 1].z;

						if (Nu3D::Math::IsWithinDistance(&a2, &g_buzzActor.posAngles.pos, 1408))
							Method1(&a2, 4, 5);

						v37 = Levels::g_recordData[10];
						v38 += 2;
						v128 += 2;
					} while (v128 < Levels::g_recordData[10]->recordCount);
				}
			}

			if ((g_unk52FC14 & 2) != 0)
			{
				v40 = Levels::g_recordData[1];
				v129 = 0;

				if (Levels::g_recordData[1]->recordCount)
				{
					v41 = 0;

					do
					{
						a2.x = 32 * v40->data[v41].x;
						a2.y = 32 * v40->data[v41].y;
						a2.z = 32 * v40->data[v41].z;

						if (Nu3D::Math::IsWithinDistance(&a2, &g_buzzActor.posAngles.pos, 1408))
							Method1(&a2, 4, 5);

						p_v1.x = Levels::g_recordData[1]->data[v41].x / 8 - g_buzzActor.posAngles.pos.x / 256;
						p_v1.y = Levels::g_recordData[1]->data[v41].y / 8 - g_buzzActor.posAngles.pos.y / 256;
						p_v1.z = Levels::g_recordData[1]->data[v41].z / 8 - g_buzzActor.posAngles.pos.z / 256;
						v42 = Levels::g_recordData[1]->data[v41].y / 8 - Levels::g_recordData[1]->data[v41 + 1].y / 8 - 192;

						v137 = v42;

						if (p_v1.x * p_v1.x + p_v1.z * p_v1.z < 9216 && p_v1.y > 0 && p_v1.y < v42)
						{
							Buzz::g_groundSlamTicker = 0;
							Buzz::g_unk53C608 = 1;
							g_buzzActor.collisionFlags = 0;

							if (g_buzzActor.gravityVel > -4096)
								g_buzzActor.gravityVel += -128 * (int32_t)Renderer::g_frameDelta;
						}

						a2.x = 32 * Levels::g_recordData[1]->data[v41 + 1].x;
						a2.y = 32 * (Levels::g_recordData[1]->data[v41 + 1].y + 1500);
						a2.z = 32 * Levels::g_recordData[1]->data[v41 + 1].z;

						if (Nu3D::Math::IsWithinDistance(&a2, &g_buzzActor.posAngles.pos, 1408))
							Method1(&a2, 4, 5);

						v40 = Levels::g_recordData[1];
						v41 += 2;
						v129 += 2;
					} while (v129 < Levels::g_recordData[1]->recordCount);
				}
			}

			v43 = Levels::g_recordData[0];
			v130 = 0;

			if (Levels::g_recordData[0]->recordCount)
			{
				v44 = 0;
				do
				{
					a2.x = 32 * v43->data[v44].x;
					a2.y = 32 * v43->data[v44].y;
					a2.z = 32 * v43->data[v44].z;

					if (Nu3D::Math::IsWithinDistance(&a2, &g_buzzActor.posAngles.pos, 1408))
						Method1(&a2, 4, 5);

					p_v1.x = Levels::g_recordData[0]->data[v44].x / 8 - g_buzzActor.posAngles.pos.x / 256;
					p_v1.y = Levels::g_recordData[0]->data[v44].y / 8 - g_buzzActor.posAngles.pos.y / 256;
					p_v1.z = Levels::g_recordData[0]->data[v44].z / 8 - g_buzzActor.posAngles.pos.z / 256;
					v45 = Levels::g_recordData[0]->data[v44].y / 8 - Levels::g_recordData[0]->data[v44 + 1].y / 8 - 192;

					v137 = v45;

					if (p_v1.x * p_v1.x + p_v1.z * p_v1.z < 9216 && p_v1.y > 0 && p_v1.y < v45)
					{
						Buzz::g_groundSlamTicker = 0;
						Buzz::g_unk53C608 = 1;
						g_buzzActor.collisionFlags = 0;

						if (g_buzzActor.gravityVel > -4096)
							g_buzzActor.gravityVel += -128 * (int32_t)Renderer::g_frameDelta;
					}

					a2.x = 32 * Levels::g_recordData[0]->data[v44 + 1].x;
					a2.y = 32 * (Levels::g_recordData[0]->data[v44 + 1].y + 1500);
					a2.z = 32 * Levels::g_recordData[0]->data[v44 + 1].z;

					if (Nu3D::Math::IsWithinDistance(&a2, &g_buzzActor.posAngles.pos, 1408))
						Method1(&a2, 4, 5);

					v43 = Levels::g_recordData[0];
					v44 += 2;
					v130 += 2;
				} while (v130 < Levels::g_recordData[0]->recordCount);
			}

			if (g_unk52FC38 > 32)
			{
				v46 = Levels::g_recordData[2];
				v47 = 0;
				v131 = 0;

				if (Levels::g_recordData[2]->recordCount)
				{
					do
					{
						a2.x = 32 * v46->data[v47].x;
						a2.y = 32 * v46->data[v47].y;
						a2.z = 32 * v46->data[v47].z;

						if (Nu3D::Math::IsWithinDistance(&a2, &g_buzzActor.posAngles.pos, 1408))
						{
							AudioManager::g_pitchArray[0] = 40 * g_unk52FC38;
							AudioManager::PlaySoundEffect(140, &a2);

							if (g_unk52FC38 > 64 || g_unk52FC28 < 2560)
							{
								if (v47 < 4)
									Method1(&a2, 1, 5);
								else
									Method1(&a2, 1, 12);
							}

							v46 = Levels::g_recordData[2];
							p_v1.x = Levels::g_recordData[2]->data[v47].x / 8 - g_buzzActor.posAngles.pos.x / 256;
							p_v1.y = Levels::g_recordData[2]->data[v47].y / 8 - g_buzzActor.posAngles.pos.y / 256;
							p_v1.z = Levels::g_recordData[2]->data[v47].z / 8 - g_buzzActor.posAngles.pos.z / 256;
							v48 = Levels::g_recordData[2]->data[v47].x / 8 - Levels::g_recordData[2]->data[v47 + 1].x / 8 - 32;
							v137 = v48;

							if (p_v1.y * p_v1.y + p_v1.z * p_v1.z >= 36864 || p_v1.x >= 0 || p_v1.x <= v48)
								goto LABEL_112;

							Levels::Method1(0, 32 * g_unk52FC38 - 32 * p_v1.x * g_unk52FC38 / v48, 0, 0);
						}

						v46 = Levels::g_recordData[2];
					LABEL_112:
						v47 += 2;
						v131 += 2;
					} while (v131 < v46->recordCount);
				}
			}

			if (g_buzzActor.collisionFlags && Buzz::g_groundSlamTicker && g_unk52FC24)
			{
				if ((g_unk52FC14 & 4) != 0)
					goto LABEL_140;

				switch (Buzz::g_unk52B81C)
				{
					case 32:
						if (g_unk52FC50 > 0)
							--g_unk52FC50;

						if (g_unk52FC30 < 2)
							++g_unk52FC30;
						break;
					case 33:
						if (g_unk52FC30 > 0)
							--g_unk52FC30;

						if (g_unk52FCD8 < 2)
							++g_unk52FCD8;
						break;
					case 34:
						if (g_unk52FCD8 > 0)
							--g_unk52FCD8;

						if (g_unk52FC50 < 2)
							++g_unk52FC50;
						break;
					default:
					LABEL_133:

						if (g_unk52FC50 == 1 && g_unk52FC30 == 1 && g_unk52FCD8 == 1)
						{
							g_unk52FC14 |= 4u;
							Platform::DisableCollision(19);
							AudioManager::StartSoundSequenceOnActor(-5, (Actor::Toy2Actor*)&g_buzzActor);
							Nu3D::Link::SetScaleFromFixedOffsets(70, 4096, 4096, 4096);
							Nu3D::Link::SetScaleFromFixedOffsets(71, 0, 0, 0);
							Nu3D::Link::SetScaleFromFixedOffsets(72, 4096, 4096, 4096);
							Nu3D::Link::SetScaleFromFixedOffsets(73, 0, 0, 0);
							Nu3D::Link::SetScaleFromFixedOffsets(78, 4096, 4096, 4096);

							Nu3D::Link::SetScaleFromFixedOffsets(79, 0, 0, 0);
						}
						goto LABEL_137;
				}

				g_unk52FC24 = 0;
				Levels::Method3(2, 1);
				Levels::Method3(3, 1);
				Levels::Method3(1, 1);
				goto LABEL_133;
			}
		LABEL_137:

			if ((g_unk52FC14 & 4) == 0)
			{
				if (! Buzz::g_groundSlamTicker)
					g_unk52FC24 = 1;
				goto LABEL_150;
			}
		LABEL_140:

			if (g_unk52FC3C)
			{
				v49 = -32 * (int32_t)Renderer::g_frameDelta + g_unk52FC3C < 0;
				v50 = -32 * (int32_t)Renderer::g_frameDelta + g_unk52FC3C;
				g_unk52FC3C = v50;

				if (v49)
				{
					Nu3D::Link::SetScaleFromFixedOffsets(80, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(81, 0, 0, 0);
					g_unk52FC3C = 0;
				}
				else
				{
					Nu3D::Link::SetScaleFromFixedOffsets(80, 4096, v50, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(81, 4096, g_unk52FC3C, 4096);
				}
			}

			v51 = ((int32_t)Renderer::g_frameDelta + g_unk52FC24) & 0x3F;
			g_unk52FC24 = v51;

			if (v51 > 40)
				goto LABEL_148;

			if ((g_unk52FC14 & 8) == 0)
			{
				g_unk52FC14 |= 8u;
				Lighting::SpawnLight(-64120, -918347, 193996, 0xFF0000, 16, -1);
				Nu3D::Link::SetScaleFromFixedOffsets(70, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(71, 0, 0, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(72, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(73, 0, 0, 0);
				Nu3D::Link::SetScaleFromFixedOffsets(78, 4096, 4096, 4096);
				Nu3D::Link::SetScaleFromFixedOffsets(79, 0, 0, 0);
				v51 = g_unk52FC24;
			}

			if (v51 > 40)
			{
			LABEL_148:
				if ((g_unk52FC14 & 8) == 8)
				{
					v52 = g_unk52FC14;
					*(uint8_t*)&v52 = g_unk52FC14 & 0xF7;
					g_unk52FC14 = v52;

					Nu3D::Link::SetScaleFromFixedOffsets(71, 4096, 4096, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(70, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(73, 4096, 4096, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(72, 0, 0, 0);
					Nu3D::Link::SetScaleFromFixedOffsets(79, 4096, 4096, 4096);
					Nu3D::Link::SetScaleFromFixedOffsets(78, 0, 0, 0);
				}
			}
		LABEL_150:

			Method2();

			if ((g_unk52FC14 & 4) != 0)
			{
				v53 = 1;

				if (g_unk4F3614[1] != -1)
				{
					v54 = &g_unk4F3614[1];
					do
					{
						v55 = *v54++;

						if (Platform::g_unk559E80 == v55)
							v53 = 0;
					} while (*v54 != -1);
				}

				if (Platform::g_unk559E80 == 1 || ! v53)
				{
					v71 = g_unk4F3614[0];

					if (g_unk4F3614[0] != -1)
					{
						v72 = g_unk4F3614;

						do
						{
							++v72;
							Platform::SetVelocity(v71, 0, 0, 0);
							v71 = *v72;
						} while (*v72 != -1);
					}

					Platform::SetVelocity(1, 0, 0, 0);
				}
				else
				{
					v56 = 0;

					if (g_unk4F3614[0] != -1)
					{
						v57 = g_unk4F3614;
						v58 = g_unk4F3614;

						do
						{
							if ((Platform::GetFlags(*v57) & 0x12) == 0)
								break;

							++v58;
							++v56;
							v57 = v58;
						} while (*v58 != -1);
					}

					v59 = g_unk4F3614[v56];

					if (v59 != -1)
					{
						v60 = 0;
						Platform::GetOrigin(v59, &v139);
						v139.y -= g_unk52FCA0[v56];

						if (g_unk4F3614[0] != -1)
						{
							v61 = g_unk4F3614;
							v62 = g_unk52FCA0;

							do
							{
								if (v60 != v56)
								{
									Platform::GetOrigin(*v61, &p_v1);
									Platform::SetOrigin(*v61, p_v1.x, *v62 + v139.y, p_v1.z);
								}

								++v61;
								++v60;
								++v62;
							} while (*v61 != -1);
						}
					}

					Platform::StepMotionScript(1, 52, (int16_t**)&g_unk52FCF4, &g_unk52FCCC, &g_unk52FCE8);

					if ((Collision::g_unk72D2A4 & 1) != 0)
					{
						Nu3D::Link::GetCurrentPosFixed(24, &p_v1);
						p_v1.x += 3 * (Camera::g_activeCameraTransform.pos.x - p_v1.x) / 4;
						p_v1.y += 3 * (Camera::g_activeCameraTransform.pos.y - p_v1.y) / 4;
						p_v1.z += 3 * (Camera::g_activeCameraTransform.pos.z - p_v1.z) / 4;
						AudioManager::PlaySoundEffect(144, &p_v1);
					}

					if ((Collision::g_unk72D2A4 & 2) != 0)
					{
						Nu3D::Link::GetCurrentPosFixed(24, &p_v1);
						p_v1.x += 3 * (Camera::g_activeCameraTransform.pos.x - p_v1.x) / 4;
						p_v1.y += 3 * (Camera::g_activeCameraTransform.pos.y - p_v1.y) / 4;
						p_v1.z += 3 * (Camera::g_activeCameraTransform.pos.z - p_v1.z) / 4;
						AudioManager::PlaySoundEffect(143, &p_v1);
					}

					v63 = g_unk4F3614[0];

					if (g_unk4F3614[0] != -1)
					{
						v64 = g_unk4F3614;
						do
						{
							++v64;
							Platform::CopyVelocity(1, v63);
							v63 = *v64;
						} while (*v64 != -1);
					}

					Nu3D::Link::GetTargetPosFixed(52, &v139);
					Nu3D::Link::GetCurrentPosFixed(52, &p_v1);

					v65 = g_unk4F35B4[0];
					p_v1.x -= v139.x;
					p_v1.y -= v139.y;
					p_v1.z -= v139.z;

					if (g_unk4F35B4[0] != -1)
					{
						v66 = g_unk4F35B4;
						v67 = g_unk4F35B4;

						do
						{
							Nu3D::Link::GetTargetPosFixed(v65, &v139);
							v139.y += p_v1.y;
							v139.z += p_v1.z;
							v139.x += p_v1.x;

							Nu3D::Link::SetPositionRawAndCommit(*v66, v139.x >> 5, v139.y >> 5, v139.z >> 5);
							v65 = v67[1];
							++v67;
							v66 = v67;
						} while (v65 != -1);
					}

					v68 = g_unk4F35EC[0];

					if (g_unk4F35EC[0] != -1)
					{
						v69 = g_unk4F35EC;
						v70 = g_unk4F35EC;

						do
						{
							Nu3D::Link::GetTargetPosFixed(v68, &v139);
							v139.y += p_v1.y >> 2;
							v139.z += p_v1.z >> 2;
							v139.x += p_v1.x >> 2;

							Nu3D::Link::SetPositionRawAndCommit(*v69, v139.x >> 5, v139.y >> 5, v139.z >> 5);
							v68 = v70[1];
							++v70;
							v69 = v70;
						} while (v68 != -1);
					}
					Nu3D::Link::SetScaleFromFixedOffsets(75, 4096, ((p_v1.y + 323200) << 12) / 323200, 4096);
				}

				v73 = 1;

				if (g_unk4F3628[1] != -1)
				{
					v74 = &g_unk4F3628[1];

					do
					{
						v75 = *v74++;

						if (Platform::g_unk559E80 == v75)
							v73 = 0;
					} while (*v74 != -1);
				}

				if (Platform::g_unk559E80 && v73)
				{
					v76 = 0;

					if (g_unk4F3628[0] != -1)
					{
						v77 = g_unk4F3628;
						v78 = g_unk4F3628;

						do
						{
							if ((Platform::GetFlags(*v77) & 0x12) == 0)
								break;

							++v78;
							++v76;
							v77 = v78;
						} while (*v78 != -1);
					}

					v79 = g_unk4F3628[v76];

					if (v79 != -1)
					{
						v80 = 0;

						Platform::GetOrigin(v79, &v139);
						v139.y -= g_unk52FC78[v76];

						if (g_unk4F3628[0] != -1)
						{
							v81 = g_unk4F3628;
							v82 = g_unk52FC78;

							do
							{
								if (v80 != v76)
								{
									Platform::GetOrigin(*v81, &p_v1);
									Platform::SetOrigin(*v81, p_v1.x, v139.y + *v82, p_v1.z);
								}

								++v81;
								++v80;
								++v82;
							} while (*v81 != -1);
						}
					}

					Platform::StepMotionScript(0, 55, (int16_t**)&g_unk52FCF0, &g_unk52FCD4, &g_unk52FCE4);

					if ((Collision::g_unk72D2A4 & 0x100) != 0)
					{
						Nu3D::Link::GetCurrentPosFixed(25, &p_v1);
						p_v1.x += 3 * (Camera::g_activeCameraTransform.pos.x - p_v1.x) / 4;
						p_v1.y += 3 * (Camera::g_activeCameraTransform.pos.y - p_v1.y) / 4;
						p_v1.z += 3 * (Camera::g_activeCameraTransform.pos.z - p_v1.z) / 4;
						AudioManager::PlaySoundEffect(144, &p_v1);
					}

					if ((Collision::g_unk72D2A4 & 0x200) != 0)
					{
						Nu3D::Link::GetCurrentPosFixed(25, &v139);
						p_v1.x += 3 * (Camera::g_activeCameraTransform.pos.x - p_v1.x) / 4;
						p_v1.y += 3 * (Camera::g_activeCameraTransform.pos.y - p_v1.y) / 4;
						p_v1.z += 3 * (Camera::g_activeCameraTransform.pos.z - p_v1.z) / 4;
						AudioManager::PlaySoundEffect(143, &v139);
					}

					v83 = g_unk4F3628[0];

					if (g_unk4F3628[0] != -1)
					{
						v84 = g_unk4F3628;

						do
						{
							++v84;
							Platform::CopyVelocity(0, v83);
							v83 = *v84;
						} while (*v84 != -1);
					}

					Nu3D::Link::GetTargetPosFixed(55, &v139);
					Nu3D::Link::GetCurrentPosFixed(55, &p_v1);
					v85 = g_unk4F35D0[0];

					p_v1.x -= v139.x;
					p_v1.y -= v139.y;
					p_v1.z -= v139.z;

					if (g_unk4F35D0[0] != -1)
					{
						v86 = g_unk4F35D0;
						v87 = g_unk4F35D0;

						do
						{
							Nu3D::Link::GetTargetPosFixed(v85, &v139);
							v139.y += p_v1.y;
							v139.z += p_v1.z;
							v139.x += p_v1.x;
							Nu3D::Link::SetPositionRawAndCommit(*v86, v139.x >> 5, v139.y >> 5, v139.z >> 5);
							v85 = v87[1];
							++v87;
							v86 = v87;
						} while (v85 != -1);
					}

					v88 = g_unk4F3600[0];

					if (g_unk4F3600[0] != -1)
					{
						v89 = g_unk4F3600;
						v90 = g_unk4F3600;

						do
						{
							Nu3D::Link::GetTargetPosFixed(v88, &v139);
							v139.y += p_v1.y >> 2;
							v139.z += p_v1.z >> 2;
							v139.x += p_v1.x >> 2;
							Nu3D::Link::SetPositionRawAndCommit(*v89, v139.x >> 5, v139.y >> 5, v139.z >> 5);
							v88 = v90[1];
							v89 = ++v90;
						} while (v88 != -1);
					}

					Nu3D::Link::SetScaleFromFixedOffsets(77, 4096, ((p_v1.y + 323200) << 12) / 323200, 4096);
				}
				else
				{
					v91 = g_unk4F3628[0];

					if (g_unk4F3628[0] != -1)
					{
						v92 = g_unk4F3628;

						do
						{
							++v92;
							Platform::SetVelocity(v91, 0, 0, 0);
							v91 = *v92;
						} while (*v92 != -1);
					}
					Platform::SetVelocity(0, 0, 0, 0);
				}
			}

			if (Buzz::g_groundSlamTicker)
			{
				if ((g_unk52FC14 & 1) == 0 && Platform::HadBuzzContactThisFrame(4))
				{
					Platform::SetRotationAngles(4, 384, 0, 0);
					Nu3D::Link::SetRotationRelative8bit(17, 384, 0, 0);
					g_unk52FC14 |= 1u;
					Levels::Method3(0, 1);
				}

				if (Buzz::g_groundSlamTicker && (g_unk52FC14 & 2) == 0 && Platform::HadBuzzContactThisFrame(5))
				{
					Platform::SetRotationAngles(5, 0, 0, 384);
					Nu3D::Link::SetRotationRelative8bit(16, 0, 0, 384);
					g_unk52FC14 |= 2u;
					Levels::Method3(4, 1);
				}
			}

			if (g_curSectorCopy2 == 7 || g_curSectorCopy2 == 12)
			{
				g_unk52FC2C -= (int32_t)Renderer::g_frameDelta;

				if (g_unk52FC2C < 0)
				{
					v93 = ++g_unk52FC58;

					if (g_unk52FC58 >= 8)
					{
						v93 = 0;
						g_unk52FC58 = 0;
					}

					v94 = Nu3D::Particles::SpawnFromPreset(
						Levels::g_recordData[60]->data[v93].x, Levels::g_recordData[60]->data[v93].y, Levels::g_recordData[60]->data[v93].z, 76, 2);

					v95 = v94;

					if (v94 != &Nu3D::Particles::g_emptyParticleStruct)
						AudioManager::PlaySoundEffect(141, &v94->pos);

					if ((g_unk52FC58 & 1) != 0)
					{
						g_unk52FC2C = 100;
						v95->velZ = 1536;
						v95->rotSpeed = 32;
					}
					else
					{
						g_unk52FC2C = 28;
						v95->velZ = -1536;
						v95->rotSpeed = -32;
					}
				}

				g_unk52FC64 -= (int32_t)Renderer::g_frameDelta;

				if (g_unk52FC64 < 0)
				{
					v96 = g_unk52FC74 + 2;
					g_unk52FC74 = v96;

					if (v96 >= Levels::g_recordData[12]->recordCount)
					{
						v96 = 0;
						g_unk52FC74 = 0;
					}

					v97 = Levels::g_recordData[12]->data[v96].z;

					v99 = 32 * Levels::g_recordData[12]->data[v96].x;
					v100 = v99 - 32 * Levels::g_recordData[12]->data[v96 + 1].x;
					v101 = 32 * Levels::g_recordData[12]->data[v96].y;
					v102 = v101 - 32 * Levels::g_recordData[12]->data[v96 + 1].y;

					v103 = 32 * Levels::g_recordData[12]->data[v96 + 1].z;
					v124 = *g_randDatBufferPtr++ - 128;

					v104 = Nu3D::Particles::SpawnInstance(v99, v101, 32 * v97, -v100 >> 5, -v102 >> 5, (v103 - 32 * v97) >> 5, 0, 0, v124, 76);

					v104->lifetime = 64;

					if (v104 != &Nu3D::Particles::g_emptyParticleStruct)
						AudioManager::PlaySoundEffect(141, &v104->pos);

					g_unk52FC64 = 50;
				}

				g_unk52FC60 -= (int32_t)Renderer::g_frameDelta;

				if (g_unk52FC60 < 0)
				{
					v105 = g_unk52FC70 + 1;
					g_unk52FC70 = v105;

					if (v105 >= Levels::g_recordData[11]->recordCount)
					{
						v105 = 0;
						g_unk52FC70 = 0;
					}

					v125 = *g_randDatBufferPtr - 128;
					v123 = 32 * Levels::g_recordData[11]->data[v105].z;
					v122 = 32 * Levels::g_recordData[11]->data[v105].y;
					v121 = 32 * Levels::g_recordData[11]->data[v105].x;

					++g_randDatBufferPtr;
					v107 = Nu3D::Particles::SpawnInstance(v121, v122, v123, 0, -2560, 0, 0, 0, v125, 76);
					v107->lifetime = 64;

					if (v107 != &Nu3D::Particles::g_emptyParticleStruct)
						AudioManager::PlaySoundEffect(141, &v107->pos);

					g_unk52FC60 = 50;
				}
			}

			v108 = g_unk52FC4C;

			if (! g_unk52FC4C)
			{
				if (! g_buzzActor.collisionFlags || g_buzzActor.posAngles.pos.y <= -1844489 || g_buzzActor.posAngles.pos.y >= -1753545)
				{
					goto LABEL_271;
				}

				g_unk52FC4C = 1;
				Dialogue::Begin(8, 31, "ha ha ha ha ... defeat the ^spider gunro^ boss to get a pizza planet ^token^!", -1, 0, -1);
				v108 = g_unk52FC4C;
			}

			if (v108 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_271;

				g_unk830D44 = 180;
				g_unk52FC4C = 2;

				g_levelActors[8].movementData = Actor::g_movementData[27] + 14;
				g_levelActors[8].unkShort1 = 0;
				g_levelActors[8].creatureRam->initialFacingAngle = 0;
				v108 = g_unk52FC4C;
			}

			if (v108 != 2)
			{
				if (v108 > 2)
				{
					g_unk52FC34 += -32 * (int32_t)Renderer::g_frameDelta;
					v109 = g_unk52FC34;

					if (g_unk52FC34 > 0)
						goto LABEL_272;
					goto LABEL_270;
				}
			LABEL_271:

				v109 = g_unk52FC34;
				goto LABEL_272;
			}

			g_unk52FCFC += 32 * (int32_t)Renderer::g_frameDelta;

			if (g_unk52FCFC <= g_unk52FC18)
			{
				if (g_unk52FCFC < 4096)
				{
					v109 = g_unk52FCFC;
					g_unk52FC34 = g_unk52FCFC;

					goto LABEL_272;
				}

				if (g_unk52FCFC < 0x2000)
				{
					v109 = 4096;
					g_unk52FC34 = 4096;
					goto LABEL_272;
				}

				if (g_unk52FCFC < 12288)
				{
					v109 = 12287 - g_unk52FCFC;
					g_unk52FC34 = 12287 - g_unk52FCFC;
					goto LABEL_272;
				}
			LABEL_270:

				v109 = 0;
				g_unk52FC34 = 0;
				goto LABEL_272;
			}

			g_unk52FCFC = 0;
			g_unk52FC18 = (*g_randDatBufferPtr++ + 320) << 6;

			v109 = 0;
			g_unk52FC34 = 0;
		LABEL_272:

			if (g_hudWidgetTimers[7] && v109)
			{
				AudioManager::g_pitchArray[0] = v109 + 1024;
				AudioManager::PlaySoundEffect(140, (Vector3I*)1);
				v108 = g_unk52FC4C;
			}

			if (v108 == 2)
			{
				if (! g_levelActors[8].creatureId)
				{
					g_unk52B7E0 = 0;

					i = 0;

					do
					{
						if (Nu3D::Particles::g_particleInstances[i].typeId == 89)
							Nu3D::Particles::g_particleInstances[i].lifetime = 1;
						++i;
					} while (i < 64);

					g_unk52FC4C = (int32_t)Renderer::g_frameDelta + 3;
				}
			}
			else if (v108 >= 3)
			{
				if (v108 >= 120)
				{
					if (v108 != 200)
					{
						Collectables::Activate(4, 0);
						g_unk52FC4C = 200;
					}
				}
				else
				{
					g_unk52FC4C = (int32_t)Renderer::g_frameDelta + v108;
				}
			}

			if (Gadget::g_unk830D48 <= 0)
				Actor::ItemReturnReward(
					7,
					37,
					"hi buzz! you need the ^grappling hook^ to climb up higher. if you find my missing ^foot^, i will unlock the ^grappling hook^!",
					"wow! thanks buzz! in return for finding my ^foot^ i will let you use the ^grappling hook^. you can aim at the special targets in visor "
					"view using the  button and you can launch the hook by pressing fire!",
					"with the ^grappling hook^ you can aim at the special targets in visor view using the target lock button and you can launch the hook by "
					"pressing fire!",
					-1,
					3584);
			else
				Actor::ItemReturnReward(
					7,
					32,
					"hi buzz! you need the ^grappling hook^ to climb up higher. if you find my missing ^foot^, i will unlock the ^grappling hook^!",
					"wow! thanks buzz! in return for finding my ^foot^ i will let you use the ^grappling hook^. you can aim at the special targets in visor "
					"view using the  button and you can launch the hook by pressing fire!",
					"with the ^grappling hook^ you can aim at the special targets in visor view using the target lock button and you can launch the hook by "
					"pressing fire!",
					-1,
					3584);

			Actor::CollectQuestReward(6, 29, -1, 0, 0);
			Actor::RotatingHint(23, 38, g_unk4F37D8);

			if ((g_levelActors[0].actorFlags & 0x200) != 0)
			{
				g_levelActors[0].actorFlags &= ~0x200u;

				if (g_collectionCount >= 0)
				{
					if (g_collectionCount == 5)
					{
						Dialogue::Begin(0, 30, "thanks for finding my ^baby mice^ buzz! here is a pizza planet ^token^!", 1214, 3584, 1);
						g_collectionCount = -1;
					}
					else
					{
						Dialogue::Begin(
							0,
							30,
							"hi buzz! if you find my ^five^ missing ^baby mice^ and come back and find me, i will give you a pizza planet ^token^. ",
							1214,
							3584,
							-1);
					}
				}
			}

			if (Collectables::g_pickupSlots[2].state == 2)
			{
				g_unk52AD64 = 100;
				g_unk52F2F8 = 0;
				g_unk52FC44 = 0;
				((Levels::Type63*)g_unk52FC10)->props.flags = 0;
			}

			if ((g_levelActors[22].actorFlags & 0x200) != 0)
			{
				g_levelActors[22].actorFlags &= ~0x200u;

				if (Collectables::g_pickupSlots[2].state != 2)
				{
					if (g_unk52F2F8)
					{
						Dialogue::Begin(22, 33, "hurry up buzz! you may still be able to reach the ^token^!", -1, 0, -1);
					}
					else
					{
						g_unk52F2F8 = 1;
						Dialogue::Begin(22, 33, "hi buzz! if you can beat the ^token^ to the bottom of the shaft you can keep it.", -1, 0, -1);
					}
				}
			}

			if (g_unk52F2F8 == 1)
			{
				if (Renderer::g_enableCinematicBars)
					goto LABEL_312;

				g_unk52AD64 = 140;
				g_unk52FC44 = 1;
				g_unk52FCDC = 0;
				Collectables::Activate(2, 0);
				((Levels::Type63*)g_unk52FC10)->props.flags = 0;
				g_unk52F2F8 = 2;
			}
			else if (g_unk52F2F8 < 2)
			{
				goto LABEL_312;
			}

			if (Actor::IsInsideBounds((Actor::Toy2Actor*)&g_buzzActor, -144736, 149536, -80448, 81600))
				g_unk52AD64 = 99;

			if (g_tickState[11])
				--g_unk52AD64;

			if (g_unk52AD64 < 100)
			{
				g_unk52AD64 = 100;
				g_unk52F2F8 = 0;
				Collectables::Deactivate(2);
				Nu3D::Link::SetPositionRawAndCommit(
					110, Levels::g_recordData[14]->data[0].x, Levels::g_recordData[14]->data[0].y, Levels::g_recordData[14]->data[0].z);
				g_unk52FC44 = 0;
				((Levels::Type63*)g_unk52FC10)->props.flags = 0;
			}
		LABEL_312:

			if (((Levels::Type63*)g_unk52FC10)->pos.y == 0x80000000)
			{
				g_unk52FC44 = 0;
				PlayLevelMusic();
			}
			else
			{
				if (g_unk52FC44)
				{
					g_unk52FC44 += (int32_t)Renderer::g_frameDelta;
					if (g_unk52FC44 > 150)
					{
						g_unk52FC44 = 1000;
						v111 = Levels::g_recordData[14];
						v112 = g_unk52FCDC + 1490 * (int32_t)Renderer::g_frameDelta;

						((uint16_t*)&v113)[1] = 0;
						g_unk52FCDC = v112;

						if (v112 >> 16 > Levels::g_recordData[14]->recordCount - 2)
						{
							*(uint16_t*)&v113 = Levels::g_recordData[14]->recordCount;
							g_unk52FCDC = (v113 << 16) - 65537;
							((Levels::Type63*)g_unk52FC10)->props.flags |= 0x28u;

							v111 = Levels::g_recordData[14];
							v112 = g_unk52FCDC;
						}

						v114 = v112;
						v115 = v112 >> 16;

						v117 = v111->data[v115 + 1].z;
						p_z = &v111->data[v115].z;
						v119 = v111->data[v115 + 1].y - v111->data[v115].y;

						v120 = v114 * (v117 - *p_z);
						a2.x = v111->data[v115].x + ((v114 * (v111->data[v115 + 1].x - v111->data[v115].x)) >> 16);
						a2.y = ((v114 * v119) >> 16) + v111->data[v115].y;
						a2.z = *p_z + (v120 >> 16);

						Nu3D::Link::SetPositionRawAndCommit(110, a2.x, a2.y, a2.z);

						((Levels::Type63*)g_unk52FC10)->pos.x = a2.x;
						((Levels::Type63*)g_unk52FC10)->pos.y = a2.y;
						((Levels::Type63*)g_unk52FC10)->pos.z = a2.z;
					}
				}

				PlayLevelMusic();
			}
		}

	}
}

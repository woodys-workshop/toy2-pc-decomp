#include "Toy2/Buzz.h"
#include "Toy2/Actor.h"
#include "Toy2/Collision.h"
#include "Toy2/Platform.h"
#include "Nu3D/Collision.h"
#include "Nu3D/Math.h"
#include "Toy2/Shadow.h"
#include "Toy2/MoveableObject.h"
#include "Toy2/Toy2.h"
#include "Toy2/Animation.h"
#include "Toy2/Camera.h"
#include "Renderer/Renderer.h"
#include "AudioManager/AudioManager.h"
#include "Nu3D/Link.h"
#include "CharacterLoader.h"
#include "InputManager.h"
#include "Random.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

namespace Toy2
{
	namespace Buzz
	{
		// one entry of g_buzzAnimTable, the animation of a Buzz state
		struct BuzzAnimEntry
		{
			uint8_t* track;
			int32_t primaryAnim;
			int32_t secondaryAnim;
			int32_t speed;
			int32_t unk10;
		};

		// GLOBAL: TOY2 0x0053C660
		int32_t g_unk53C660;

		// GLOBAL: TOY2 0x0053C838
		int32_t g_unk53C838;

		// GLOBAL: TOY2 0x0053C668
		int32_t g_unk53C668;

		// GLOBAL: TOY2 0x0053C658
		int32_t g_unk53C658;

		// GLOBAL: TOY2 0x0053C5E4
		int32_t g_unk53C5E4;

		// GLOBAL: TOY2 0x0053C5D4
		int32_t g_buzzSlipTimer;

		// GLOBAL: TOY2 0x0053C618
		int32_t g_unk53C618;

		// GLOBAL: TOY2 0x0053C600
		int32_t g_unk53C600;

		// GLOBAL: TOY2 0x0053C610
		int32_t g_unk53C610;

		// GLOBAL: TOY2 0x0053C83C
		int32_t g_unk53C83C;

		// GLOBAL: TOY2 0x0053C650
		int32_t g_unk53C650;

		// GLOBAL: TOY2 0x0053C64C
		int32_t g_groundSlamTicker;

		// GLOBAL: TOY2 0x0053C828
		int32_t g_buzzActionState;

		// GLOBAL: TOY2 0x0053C620
		int32_t g_fireChargeTimer;

		// GLOBAL: TOY2 0x0053C840
		int32_t g_fireHoldTimer;

		// GLOBAL: TOY2 0x0053C648
		MoveableObject::MoveableObject* g_curMoveableObject;

		// GLOBAL: TOY2 0x0053C624
		int32_t g_unk53C624;

		// GLOBAL: TOY2 0x0053C66C
		int32_t g_unk53C66C;

		// GLOBAL: TOY2 0x0053C834
		int32_t g_unk53C834;

		// GLOBAL: TOY2 0x0053C830
		int32_t g_unk53C830;

		// GLOBAL: TOY2 0x0052B81C
		int32_t g_unk52B81C;

		// GLOBAL: TOY2 0x005281A4
		int32_t g_unk5281A4;

		// GLOBAL: TOY2 0x0053C634
		int32_t g_unk53C634;

		// GLOBAL: TOY2 0x0053C60C
		int32_t g_unk53C60C;

		// GLOBAL: TOY2 0x0053C810
		int32_t g_unk53C810;

		// GLOBAL: TOY2 0x0053C628
		int32_t g_underwaterYLevel;

		// GLOBAL: TOY2 0x0053C5E8
		int32_t g_currentFluidType;

		// GLOBAL: TOY2 0x0053C62C
		int32_t g_unk53C62C;

		// GLOBAL: TOY2 0x0053C824
		int32_t g_greenGunCharge;

		// GLOBAL: TOY2 0x0053C608
		int32_t g_unk53C608;

		// GLOBAL: TOY2 0x0053C818
		int32_t g_unk53C818;

		// GLOBAL: TOY2 0x0050A098
		int32_t g_unk50A098;

		// GLOBAL: TOY2 0x0053C678
		int32_t g_unk53C678;

		// GLOBAL: TOY2 0x0053C5D0
		int32_t g_unk53C5D0;

		// GLOBAL: TOY2 0x0053C814
		int32_t g_unk53C814;

		// GLOBAL: TOY2 0x0053C674
		int32_t g_unk53C674;

		// GLOBAL: TOY2 0x0053C61C
		int32_t g_unk53C61C;

		// GLOBAL: TOY2 0x0053C664
		int32_t g_unk53C664;

		// GLOBAL: TOY2 0x0053C5DC
		int32_t g_unk53C5DC;

		// GLOBAL: TOY2 0x0050A0A0
		Vector3I g_unk50A0A0;

		// GLOBAL: TOY2 0x004DF040
		// the event tracks of Buzz's animations, read by StepEventTrack
		uint8_t g_buzzAnimTracks[944] = {
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x82, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
			0x0F, 0x10, 0x11, 0x12, 0x81, 0x13, 0x14, 0x15, 0x16, 0x17, 0xFF, 0x1A, 0x00, 0x01, 0x02, 0x03,
			0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
			0x14, 0x15, 0x16, 0x17, 0xFF, 0x18, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xFF, 0x10, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03,
			0x04, 0x05, 0xFF, 0x01, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0x01, 0x0C, 0x0D, 0x0E, 0x0F,
			0xFE, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
			0x0C, 0x0D, 0xFF, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
			0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0xFF, 0x18, 0x00, 0x00,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0xFF, 0x01, 0x00, 0x00, 0x01, 0x02, 0x03,
			0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
			0x14, 0x15, 0x16, 0x17, 0xFF, 0x18, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0xFF, 0x01, 0x00, 0x00,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0x11, 0x12, 0x13, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x86, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
			0x17, 0x86, 0xFF, 0x1A, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
			0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0xFF, 0x18, 0x00, 0x00,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0x0C, 0x00, 0x00,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
			0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0xFF, 0x01, 0x00, 0x01, 0x02, 0x03,
			0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0xFF, 0x01,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0xFF, 0x08, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03,
			0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03,
			0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
			0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0xFF, 0x1E, 0x00, 0x01, 0x02, 0x03,
			0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03,
			0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03,
			0x83, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0xFF, 0x0A, 0x00, 0x00, 0x00,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x83, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
			0x0F, 0x10, 0x11, 0xFF, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
			0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0xFE, 0x00, 0x00,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0xFF, 0x01, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
			0x0C, 0xFF, 0x01, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
			0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
			0x0E, 0x0F, 0x10, 0x11, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			0x08, 0xFF, 0x01, 0x00, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0xFF, 0x01, 0x00,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0xFF, 0x1E,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03,
			0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
			0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0xFF, 0x01, 0x00, 0x00,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x84, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
			0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E,
			0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E,
			0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x85, 0x36, 0x37, 0x38, 0x39, 0xFE, 0x00, 0x00, 0x00,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
			0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
			0xFE, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
			0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0xFF, 0x0B, 0x00, 0x00, 0x00,
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0xFF, 0x18, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03,
			0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0xFF, 0x0A
		};

		// GLOBAL: TOY2 0x004DF3F0
		BuzzAnimEntry g_buzzAnimTable[33] = {
			{&g_buzzAnimTracks[0x0], 0, 1, -48, 0},
			{&g_buzzAnimTracks[0x38], 2, 3, 16384, 1},
			{&g_buzzAnimTracks[0x4C], 4, 5, 16384, 0},
			{&g_buzzAnimTracks[0x54], 4, 5, 16384, 0},
			{&g_buzzAnimTracks[0x5C], 4, 5, 32768, 0},
			{&g_buzzAnimTracks[0x64], 6, 6, 32768, 0},
			{&g_buzzAnimTracks[0x1C], 24, 25, -48, 0},
			{&g_buzzAnimTracks[0x90], 8, 8, 16384, 0},
			{&g_buzzAnimTracks[0x230], 22, 27, 32768, 0},
			{&g_buzzAnimTracks[0x140], 10, 10, 32768, 0},
			{&g_buzzAnimTracks[0x1DC], 17, 17, 32768, 0},
			{&g_buzzAnimTracks[0x1F0], 18, 18, 32768, 0},
			{&g_buzzAnimTracks[0x1CC], 15, 15, 32768, 0},
			{&g_buzzAnimTracks[0x208], 16, 16, 32768, 0},
			{&g_buzzAnimTracks[0xF8], 19, 19, 32768, 0},
			{&g_buzzAnimTracks[0x114], 20, 20, 32768, 0},
			{&g_buzzAnimTracks[0x130], 21, 21, 32768, 0},
			{&g_buzzAnimTracks[0x1BC], 14, 14, 32768, 0},
			{&g_buzzAnimTracks[0x244], 23, 23, 32768, 0},
			{&g_buzzAnimTracks[0x180], 11, 11, 32768, 0},
			{&g_buzzAnimTracks[0x74], 7, 7, 32768, 0},
			{&g_buzzAnimTracks[0xAC], 9, 9, 32768, 0},
			{&g_buzzAnimTracks[0x254], 26, 26, 32768, 0},
			{&g_buzzAnimTracks[0x278], 12, 12, 32768, 0},
			{&g_buzzAnimTracks[0x284], 12, 12, 32768, 0},
			{&g_buzzAnimTracks[0x290], 13, 13, -100, 0},
			{&g_buzzAnimTracks[0x2B0], 28, 28, 21845, 0},
			{&g_buzzAnimTracks[0x2CC], 31, 31, 16384, 0},
			{&g_buzzAnimTracks[0x2F0], 29, 29, 32768, 0},
			{&g_buzzAnimTracks[0x330], 30, 30, 16384, 0},
			{&g_buzzAnimTracks[0x364], 32, 32, 24576, 0},
			{&g_buzzAnimTracks[0x380], 33, 33, 32768, 0},
			{&g_buzzAnimTracks[0x39C], 34, 34, 32768, 0}
		};

		// STUB: TOY2 0x00414110
		void Respawn() {}

		// FUNCTION: TOY2 0x004011D0
		void UpdateAnimationState()
		{
			int32_t v0;
			int32_t v1;
			int32_t v8;
			int32_t v10;
			uint32_t v11;
			uint8_t* v9;
			uint16_t unkVar33;
			int32_t v13;
			int32_t v15;
			int32_t v16;
			int32_t v17;
			int32_t groundHeight;
			WorldPos probePos;

			do
			{
				for (;;)
				{
					v0 = g_unk50A0BC - Renderer::g_frameDelta;
					g_unk50A0BC -= Renderer::g_frameDelta;

					if (g_unk50A0BC <= 0)
					{
						v0 = 0;
						g_unk50A0BC = 0;
					}

					if (!g_buzzActor.unkShort40)
					{
						v1 = g_buzzActor.unkVar37;
						g_unk50A098 = 0;

						switch (g_buzzActor.unkVar37)
						{
							case 0xA:
							case 0xB:
								v1 = 0;
								break;

							case 2:
								if (g_buzzActor.gravityVel <= 0)
									goto LABEL_55;

								v1 = 3;
								break;

							case 5:
								v1 = 4;
								break;

							default:
								goto LABEL_55;
						}

						g_buzzActor.unkVar37 = v1;

					LABEL_55:
						if (g_unk53C838 == 80)
						{
							v1 = 12;
							goto LABEL_57;
						}

						goto LABEL_58;
					}

					v1 = g_buzzActor.unkVar37;

					if (g_buzzActor.unkVar37 == 3)
					{
						v1 = 4;
						g_buzzActor.unkVar37 = 4;
					}

					if (g_buzzActor.unkVar30 < 512 && abs(g_buzzActor.unkVar31) < 512
						&& ((InputManager::g_directionInputState & (DIR_UP | DIR_RIGHT | DIR_DOWN | DIR_LEFT)) == 0 || g_unk53C664 == 30))
					{
						if (v1 == 4)
						{
							g_unk50A098 = 0;
							goto LABEL_58;
						}

						v1 = g_unk50A098;

						if (g_unk50A098)
							goto LABEL_23;

						v1 = 1;
						g_unk50A098 = 1;

						if (((g_buzzActor.actorFlags & 0x100) != 0 && g_currentFluidType >= 2) || g_buzzActor.floorYPos - g_buzzActor.posAngles.pos.y <= 4096
							|| g_buzzActor.floorYPos == 0x80000000)
						{
						LABEL_25:
							if ((g_buzzActor.unkVar33 & 0xFFFF0000) == 0xE0000)
							{
								uint8_t roll = *g_randDatBufferPtr & 3;
								uint8_t* next = ++g_randDatBufferPtr;

								if (!roll)
								{
									v1 = (*next & 1) + 28;
									g_unk50A098 = v1;
									g_randDatBufferPtr = next + 1;

									if (!v0)
									{
										g_unk50A0BC = 1800;

										if (g_fireChargeTimer <= 0 && !g_unk53C83C && !g_unk53C650 && (Camera::g_inCutscene & 1) == 0)
										{
											g_unk50A0B0 = (g_unk50A0B0 != 207) + 206;
											AudioManager::Preset::PlayOneShotSound(g_unk50A0B0, (Actor::Toy2Actor*)&g_buzzActor);
											v1 = g_unk50A098;
										}
									}

									goto LABEL_33;
								}
							}
						}
						else
						{
							probePos.y = g_buzzActor.posAngles.pos.y;
							probePos.x = g_buzzActor.posAngles.pos.x + (Numerics::g_sinCosLUT[g_buzzActor.posAngles.angles.yaw & 0xFFF] >> 2);
							probePos.z = g_buzzActor.posAngles.pos.z + (Numerics::g_sinCosLUT[(g_buzzActor.posAngles.angles.yaw + 1024) & 0xFFF] >> 2);

							groundHeight = Nu3D::Collision::GetGroundHeight(&probePos, 0);

							if (groundHeight - g_buzzActor.posAngles.pos.y > 4096 && groundHeight != 0x80000000)
							{
								v1 = 10;
								goto LABEL_38;
							}

							probePos.x = g_buzzActor.posAngles.pos.x + (Numerics::g_sinCosLUT[(g_buzzActor.posAngles.angles.yaw - 2048) & 0xFFF] >> 2);
							probePos.z = g_buzzActor.posAngles.pos.z + (Numerics::g_sinCosLUT[(g_buzzActor.posAngles.angles.yaw - 1024) & 0xFFF] >> 2);

							groundHeight = Nu3D::Collision::GetGroundHeight(&probePos, 0);

							if (groundHeight - g_buzzActor.posAngles.pos.y > 4096 && groundHeight != 0x80000000)
							{
								v1 = 11;
								goto LABEL_38;
							}

							v1 = g_unk50A098;

						LABEL_23:
							if (v1 == 1)
							{
								v0 = g_unk50A0BC;
								goto LABEL_25;
							}

						LABEL_33:
							if (v1 >= 28 && (g_fireChargeTimer > 0 || g_unk53C83C || g_unk53C650))
							{
								v1 = 1;
							LABEL_38:
								g_unk50A098 = v1;
							}
						}

						if ((Camera::g_inCutscene & 1) != 0)
						{
							v1 = 1;
							g_unk50A098 = 1;
						}

						g_buzzActor.unkVar37 = v1;

						if (g_buzzSlipTimer)
						{
							v1 = 18;
						LABEL_57:
							g_buzzActor.unkVar37 = v1;
							goto LABEL_58;
						}

						goto LABEL_58;
					}

					v1 = 0;
					g_unk50A098 = 0;
					g_buzzActor.unkVar37 = 0;

					if (g_buzzSlipTimer)
					{
						v1 = 18;
						goto LABEL_57;
					}

				LABEL_58:
					if (g_unk53C66C && v1 != 8)
					{
						v1 = 26;
						g_buzzActor.unkVar37 = 26;
					}

					if (g_curMoveableObject)
					{
						v1 = 25;
						g_buzzActor.unkVar37 = 25;
					}

					if (g_unk53C618 == 2)
					{
						v1 = 17;
						g_buzzActor.unkVar37 = 17;
					}

					if (g_grappleState == 2 && g_grappleProgress >= 10)
					{
						v1 = 30;
						g_buzzActor.unkVar37 = 30;
					}

					if (g_gravityBootsDuration)
					{
						v1 = 31;
						g_buzzActor.unkVar37 = 31;
					}

					if (g_unk53C608)
					{
						v1 = 19;
						g_buzzActor.unkVar37 = 19;
					}

					if (g_unk53C818 && (g_buzzActor.unkShort40 || !v1))
					{
						v1 = 32;
						g_buzzActor.unkVar37 = 32;
					}

					if (g_unk53C83C < 0)
					{
						v1 = (g_unk53C83C > -120) + 19;
						g_buzzActor.unkVar37 = v1;
					}

					if (g_unk53C668 > 0)
					{
						v1 = 15;
						g_buzzActor.unkVar37 = 15;

						if ((g_unk53C668 & 2) != 0)
						{
							v1 = 14;
							g_buzzActor.unkVar37 = 14;
						}

						if ((g_unk53C668 & 4) != 0)
						{
							g_buzzActor.unkVar37 = 16;
							AudioManager::PlaySoundEffect(35, &g_buzzActor.posAngles.pos);
							v1 = g_buzzActor.unkVar37;
						}
					}

					if (g_unk53C660)
					{
						v1 = 9;
						g_buzzActor.unkVar37 = 9;
					}

					if (g_groundSlamTicker > 0)
					{
						v1 = 23;
						g_buzzActor.unkVar37 = 23;
					}

					if (g_groundSlamTicker < -14)
					{
						v1 = 24;
						g_buzzActor.unkVar37 = 24;
					}

					if (g_unk53C838 < 0)
					{
						v1 = 13;
						g_buzzActor.unkVar37 = 13;
					}

					if (g_rocketBootsDuration)
					{
						v1 = 10;
						g_buzzActor.unkVar37 = 10;
					}

					if (g_buzzActor.stunTimer > 0)
					{
						if (g_buzzActor.stunTimer <= 67)
						{
							g_buzzActor.actorFlags |= 0x20;
						}
						else
						{
							v1 = 5;
							g_buzzActor.unkVar37 = 5;
						}
					}

					if (g_levelTransition == 2)
					{
						v1 = 7;
					}
					else
					{
						if (g_levelTransition != 1)
							goto LABEL_102;

						v1 = 27;
					}

					g_buzzActor.unkVar37 = v1;

				LABEL_102:
					v8 = v1;
					v9 = g_buzzAnimTable[v1].track;

					if (v1 != g_buzzActor.unkVar38)
					{
						unkVar33 = 0;
						g_buzzActor.unkVar33 = 0;
						g_buzzActor.unkVar38 = v8;
						v11 = *v9;
						goto LABEL_113;
					}

					v10 = Renderer::g_frameDelta * g_buzzAnimTable[v1].speed;

					if (v10 < 0)
					{
						v10 = (int32_t)-(v10 * sqrt((double)(g_buzzActor.unkVar30 * g_buzzActor.unkVar30 + g_buzzActor.unkVar31 * g_buzzActor.unkVar31)));

						if (v10 < 0)
						{
							v10 = 0;
							break;
						}
					}

					if (v10 <= 0xFFFF)
						break;

					for (;;)
					{
						v10 -= 0x10000;
						g_buzzActor.unkVar33 += 0x10000;

						if (Animation::StepEventTrack(v9) == -1)
							break;

						if (v10 <= 0xFFFF)
							goto LABEL_109;
					}
				}

			LABEL_109:
				g_buzzActor.unkVar33 += v10;
				v11 = Animation::StepEventTrack(v9);
			} while (v11 == -1);

			unkVar33 = (uint16_t)g_buzzActor.unkVar33;

		LABEL_113:
			v13 = (v11 << 16) + unkVar33;

			*(int32_t*)&g_buzzActor.unkWord13 = v13;

			v15 = v13;
			v16 = g_buzzAnimTable[v8].secondaryAnim;
			v17 = g_buzzAnimTable[v8].primaryAnim;

			if (g_unk53C650 > 0)
			{
				if (v17 == v16)
				{
					g_unk53C650 = 0;
				}
				else
				{
					v17 = 9;
					v15 = (48 - g_unk53C650) << 15;
				}
			}

			if (g_fireChargeTimer > 0)
			{
				if (v17 != v16 || Camera::g_cameraState == 4)
				{
					v17 = 26;
					v15 = ((g_fireChargeTimer & 1) + 2 * g_buzzAnimTracks[0x254 + g_fireChargeTimer / 2]) << 15;
				}
				else
				{
					g_fireChargeTimer = 0;
				}
			}

			g_buzzActor.primaryAnimIdx = v17;
			g_buzzActor.unkVar7 = v15;
			g_buzzActor.secondaryAnimIdx = v16;
			g_buzzActor.unkVar8 = v13;

			if (v17 == v16)
				g_buzzActor.unkVar8 = v15;
			else
				Animation::EvaluateClip(*(int16_t**)(CharacterLoader::g_unk547CD4[0] + 4 * v16 + 8), v13, 0, 0);

			Animation::g_unk546D70 = 1;

			Animation::EvaluateClip(*(int16_t**)(CharacterLoader::g_unk547CD4[0] + 4 * v17 + 8), v15, 0, 1);

			Animation::g_unk546D70 = 0;

			if (Camera::g_cameraState)
			{
				g_unk50A0A0.x = 20480;
				g_unk50A0A0.y = 5376;
				g_unk50A0A0.z = -14336;

				Nu3D::Link::TransformVectorInt3x3(47, &g_unk50A0A0);

				g_unk50A0A0.x += g_buzzActor.posAngles.pos.x;
				g_unk50A0A0.y = g_unk50A0A0.y + g_buzzActor.posAngles.pos.y - 12288;
				g_unk50A0A0.z += g_buzzActor.posAngles.pos.z;
			}
			else
			{
				g_unk50A0A0.x = -133;
				g_unk50A0A0.y = -211;
				g_unk50A0A0.z = -6;

				Actor::ResolveBoneAttachmentPos(&g_unk50A0A0, (Actor::Toy2Actor*)&g_buzzActor, 15);
			}

			int32_t absVelZ = abs(g_buzzActor.unkVar31);

			if (((absVelZ > g_buzzActor.unkVar30 && absVelZ > 256) || g_buzzActor.unkVar30 < -256) && g_buzzActor.unkShort40 && (g_buzzActor.actorFlags & 4) == 0
				&& !g_unk53C838)
				g_unk53C634 |= 4;

			if (!g_buzzActor.unkVar30 && !g_buzzActor.unkVar31)
				g_unk53C634 |= 8;
		}

		// STUB: TOY2 0x004071E0
		void HandleDamage(int16_t hitAngle, uint8_t hitType) {}

		// STUB: TOY2 0x004100F0
		void CheckParticleCollisions() {}

		// FUNCTION: TOY2 0x004340D0
		void UpdateJumpAndGravity(Toy2::Buzz::Toy2BuzzActor* buzzActor, int32_t initialJumpVelocity, int32_t forceJump)
		{
			int32_t gravityScale;
			int32_t maxFallSpeed;
			int32_t jumpState;
			int32_t groundSlamTimer;
			int32_t fallenHeight;
			int32_t velocity;

			if ((g_buzzActor.actorFlags & 0x100) != 0)
			{
				gravityScale = 4;

				if (g_currentFluidType == 1)
				{
					maxFallSpeed = 1024;
				}
				else
				{
					maxFallSpeed = 64;

					if (buzzActor->gravityVel >= 0)
						buzzActor->unkShort40 = 6;
				}
			}
			else
			{
				gravityScale = 2;
				maxFallSpeed = 2048;
			}

			if ((InputManager::g_directionInputState & DIR_CAM_FLAG2) == 0 || forceJump)
			{
				if (buzzActor->collisionFlags)
					buzzActor->unkWord15 = 0;

				if (buzzActor->unkWord15 == 1)
				{
					velocity = buzzActor->gravityVel;

					if (velocity < 0)
					{
						if (velocity < -800 / gravityScale)
							buzzActor->gravityVel = velocity + 384 / (gravityScale * gravityScale);

						buzzActor->gravityVel = buzzActor->gravityVel / 2 - 256 / (gravityScale * gravityScale);
					}

					buzzActor->unkWord15 = 2;
				}

				if (buzzActor->unkWord15 == 5)
					buzzActor->unkWord15 = 6;
			}
			else
			{
				jumpState = buzzActor->unkWord15;
				groundSlamTimer = g_unk53C838;

				if ((jumpState == 2 && buzzActor->gravityVel > -1024 && g_unk53C83C >= 0 && !buzzActor->unkShort40 && buzzActor->inhibitMovementTimer <= 0
					 && g_unk53C838 >= 0)
					|| (!jumpState && g_unk53C838 == 80))
				{
					g_unk53C838 = 0;
					fallenHeight = g_unk53C674 - buzzActor->posAngles.pos.y;

					if (fallenHeight > 0 && g_unk53C61C)
					{
						velocity = (int32_t)-sqrt((double)(2 * (27264 - fallenHeight) * (256 / (gravityScale * gravityScale))));
						buzzActor->gravityVel = velocity;

						if (velocity < -1868)
							buzzActor->gravityVel = -1868;

						if (buzzActor->gravityVel > -554)
							buzzActor->gravityVel = -554;
					}
					else
					{
						buzzActor->gravityVel = -2304 / gravityScale;
					}

					buzzActor->unkWord15 = 5;
					buzzActor->unkVar37 = 8;
					AudioManager::PlaySoundEffect(16, &g_buzzActor.posAngles.pos);
					groundSlamTimer = g_unk53C838;
				}

				jumpState = buzzActor->unkWord15;

				if (jumpState == 1 || jumpState >= 4)
				{
					if (buzzActor->collisionFlags)
					{
						buzzActor->unkWord15 = 5;
					}
					else if (jumpState == 6)
					{
						buzzActor->unkWord15 = 3;
					}
				}
				else if (!buzzActor->unkShort40 || g_groundSlamTicker || groundSlamTimer == 80)
				{
					if (jumpState == 2)
						buzzActor->unkWord15 = 3;
				}
				else
				{
					if (!AudioManager::IsActorSoundPlaying(buzzActor))
						AudioManager::Preset::PlayOneShotSound2(25, (Actor::Toy2Actor*)buzzActor);

					g_buzzActor.unkWord15 = 1;
					g_buzzActor.collisionFlags = 0;
					g_buzzActor.unkShort40 = 0;
					g_buzzActor.unkVar37 = 2;
					g_buzzActor.gravityVel = initialJumpVelocity;
					g_curMoveableObject = 0;
					g_buzzSlipTimer = 0;
					g_unk53C61C = 0;
					fallenHeight = buzzActor->posAngles.pos.y;
					g_unk53C61C = 1;
					g_unk53C674 = fallenHeight;
				}
			}

			if (!buzzActor->collisionFlags)
				buzzActor->gravityVel += ((int32_t)Renderer::g_frameDelta << 8) / (gravityScale * gravityScale);

			if (buzzActor->gravityVel > maxFallSpeed)
				buzzActor->gravityVel = maxFallSpeed;
		}

		// STUB: TOY2 0x004343D0
		int32_t UpdateHorizontalMovement(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movementParams, int32_t flag) { return 0; }

		// STUB: TOY2 0x004346C0
		int32_t UpdateFacing(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movementParams) { return 0; }

		// STUB: TOY2 0x00434990
		void TickGunFire(PosAndAngles* muzzlePos) {}

		// STUB: TOY2 0x00434D20
		int32_t TickGroundSlam(Toy2::Actor::Toy2Actor* actor) { return 0; }

		// STUB: TOY2 0x00434EB0
		void TickSpinHover(Toy2::Buzz::Toy2BuzzActor* buzz) {}

		// STUB: TOY2 0x00435100
		int32_t HandleSwing(Toy2::Buzz::Toy2BuzzActor* buzz) { return 0; }

		// STUB: TOY2 0x004354E0
		int32_t HandlePoleClimb(Toy2::Buzz::Toy2BuzzActor* buzz) { return 0; }

		// STUB: TOY2 0x004359D0
		int32_t HandleZipline(Toy2::Buzz::Toy2BuzzActor* actor) { return 0; }

		// FUNCTION: TOY2 0x004863A0
		int32_t UpdateFloorHeight(Toy2::Buzz::Toy2BuzzActor* buzzActor)
		{
			int32_t triangleCount = Collision::g_candidateTriangleCount;
			WorldPos worldPos;

			Nu3D::Collision::GatherTrianglesAtXZ((WorldPos*)buzzActor);

			worldPos.x = buzzActor->posAngles.pos.x;
			worldPos.z = buzzActor->posAngles.pos.z;
			worldPos.x2 = *(int32_t*)&buzzActor->posAngles.angles;
			worldPos.y = buzzActor->posAngles.pos.y - 1024;

			Nu3D::Collision::ResolveGroundCeiling(&worldPos, 0x10000);

			int32_t floorY = worldPos.y;

			Collision::g_unk72912C = -1;

			if (Collision::g_collisionMeshInstances[Collision::g_groundContactMeshIdx].typeFlags == 8)
			{
				int32_t platformIdx = Collision::g_collisionMeshInstances[Collision::g_groundContactMeshIdx].platformIdx;
				int16_t flags;
				int32_t buzzY;

				Collision::g_unk72912C = platformIdx;
				flags = Platform::g_platformDescriptors[platformIdx].flags;

				if ((flags & 8) == 8)
				{
					buzzY = Toy2::g_buzzActor.posAngles.pos.y;

					if (buzzY > worldPos.y + 512 && buzzY < worldPos.y + 24576)
					{
						buzzY = worldPos.y - 448;
						Toy2::g_buzzActor.posAngles.pos.y = worldPos.y - 448;

						if (Collision::g_floorSlope.y >= (int16_t)0xE000)
						{
							Toy2::g_buzzActor.velX = 3 * Toy2::g_buzzActor.velX / 4;
							Toy2::g_buzzActor.velForward = 3 * Toy2::g_buzzActor.velForward / 4;
						}
					}
				}
				else
					buzzY = Toy2::g_buzzActor.posAngles.pos.y;

				if (worldPos.y - buzzY < 512 && worldPos.y - buzzY > -1024 && Collision::g_floorSlope.y < (int16_t)0xE000)
				{
					Platform::g_platformDescriptors[platformIdx].flags = flags | 0x12;
					Collision::g_buzzCollisionStates[0].platformIdx = (int16_t)platformIdx;

					if (Toy2::g_buzzActor.unkWord15 == 0)
					{
						Toy2::g_buzzActor.collisionFlags |= 1;
						Toy2::g_buzzActor.gravityVel += 32;
					}
				}
			}

			Collision::g_floorSlopePrev.z = Collision::g_floorSlope.z;
			Collision::g_floorSlopePrev.x = Collision::g_floorSlope.x;
			Collision::g_floorSlopePrev.y = Collision::g_floorSlope.y;
			Collision::g_candidateTriangleCount = triangleCount;

			return floorY;
		}

		// FUNCTION: TOY2 0x00484380
		void ResolveCollisions(Toy2::Buzz::Toy2BuzzActor* buzzActor, Vector3I* velocity, uint8_t* flags, int32_t index, int32_t halfStep)
		{
			int32_t savedIndex;
			int32_t speed;
			int16_t x;
			int16_t y;
			int16_t z;
			int16_t surfaceY;
			Vector3I16 velocity16;
			Vector3I16 angles16;
			int16_t* surfaceData;
			int32_t contactSurfaceIndex;
			int32_t platformIdx;
			int32_t divisor;
			int32_t radius;
			int32_t posX;
			int32_t posY;
			int32_t posZ;
			int32_t doubleRadius;
			int32_t meshCount;
			int32_t listCount;
			int32_t platformMeshCount;
			int32_t cellX;
			int32_t cellY;
			int32_t cellZ;
			int32_t listIdx;
			int32_t i;
			int32_t j;
			int32_t mask;
			int32_t nodeMargin;
			int32_t platformRadius;
			int32_t platformX;
			int32_t platformY;
			int32_t platformZ;
			int32_t localX;
			int32_t localY;
			int32_t localZ;
			int32_t rotatedX;
			int32_t rotatedY;
			int32_t rotatedZ;
			int32_t minCellX;
			int32_t minCellZ;
			int32_t maxCellX;
			int32_t maxCellZ;
			int32_t blockStart;
			int32_t blockEnd;
			int32_t count;
			int32_t steps;
			int32_t substepResult;
			int32_t platformResult;
			int32_t stuck;
			int16_t meshId;
			int16_t* meshList;
			int16_t* platformList;
			uint8_t* edgeNode;
			int32_t* edgeBlock;
			int32_t* edgePoint;
			Platform::PlatformDescriptor* platform;
			Collision::CollisionMeshInstance* mesh;
			Collision::CollisionTreeNode* node;
			Collision::CollisionTriangle* triangle;
			Collision::ResolvedVelocity resolvedVelocity;
			WorldPos vectorAngles;
			int16_t stepCount;
			Matrix3x3I16* rotation = (Matrix3x3I16*)&Collision::g_mathScratch[948];

			savedIndex = index;
			buzzActor->posAngles.pos.y += -192 - Collision::g_buzzCollisionStates[index].floorOffset;

			Collision::g_buzzCollisionStates[index].platformIdx = -1;
			Collision::g_buzzCollisionStates[index].pushbackVel.x = 0;
			Collision::g_buzzCollisionStates[index].pushbackVel.y = 0;
			Collision::g_buzzCollisionStates[index].pushbackVel.z = 0;

			Collision::g_unk72D2AC = 0;
			Collision::g_candidateTriangleCount = 0;
			Collision::g_unk72D2A8 = 0;
			Collision::g_unk7291D8 = 0;

			x = (int16_t)velocity->x;

			if (Collision::g_buzzCollisionStates[index].wasGroundedLastFrame == 1)
			{
				y = (int16_t)velocity->y;
			}
			else
			{
				x += Collision::g_buzzCollisionStates[index].pushbackVel.x;
				y = (int16_t)velocity->y + Collision::g_buzzCollisionStates[index].pushbackVel.y;
			}

			velocity16.x = x;
			z = (int16_t)velocity->z;
			velocity16.y = y;
			velocity16.z = z;

			if (halfStep)
			{
				x /= 2;
				velocity16.x = x;

				y /= 2;
				velocity16.y = y;

				z /= 2;
				velocity16.z = z;
			}

			Collision::g_unk7290E4 = 0;
			Collision::g_buzzCollisionStates[index].unkWord20 = 0;

			speed = (int32_t)sqrt((double)(x * x + y * y + z * z));

			// sliding down the surface Buzz is standing on
			if (flags[1] != 1)
				goto LABEL_28;

			contactSurfaceIndex = Collision::g_buzzCollisionStates[index].contactSurfaceIndex;

			if (Collision::g_collisionMeshInstances[contactSurfaceIndex & 0x7FF].typeFlags != 8
				|| (platformIdx = Collision::g_collisionMeshInstances[contactSurfaceIndex & 0x7FF].platformIdx,
					(Platform::g_platformDescriptors[platformIdx].flags & 8) == 0))
			{
				surfaceData = (int16_t*)Collision::g_buzzCollisionStates[index].contactSurfaceData;

				if ((contactSurfaceIndex & 0x4000) != 0)
				{
					surfaceY = surfaceData[20];

					if (surfaceY <= -12000)
						goto LABEL_28;

					if (surfaceY < -4096)
					{
						velocity->x += -4096 * surfaceData[19] / ((speed + 3200) * surfaceY / 32);
						velocity->z += -4096 * surfaceData[21] / ((speed + 3200) * surfaceData[20] / 32);

						goto LABEL_28;
					}

					divisor = ((speed + 3200) << 12) / 32;
					velocity->x += (surfaceData[19] << 12) / divisor;
					velocity->z += (surfaceData[21] << 12) / divisor;
				}
				else
				{
					surfaceY = surfaceData[17];

					if (surfaceY <= -12000)
						goto LABEL_28;

					if (surfaceY < -4096)
					{
						velocity->x += -4096 * surfaceData[16] / ((speed + 3200) * surfaceY / 32);
						velocity->z += -4096 * surfaceData[18] / ((speed + 3200) * surfaceData[17] / 32);

						goto LABEL_28;
					}

					divisor = ((speed + 3200) << 12) / 32;
					velocity->x += (surfaceData[16] << 12) / divisor;
					velocity->z += (surfaceData[18] << 12) / divisor;
				}

				goto LABEL_28;
			}

			// the surface belongs to a rotated platform, the normal is rotated
			platform = &Platform::g_platformDescriptors[platformIdx];

			angles16.x = platform->rotAngle.x >> 2;
			angles16.y = platform->rotAngle.y >> 2;
			angles16.z = platform->rotAngle.z >> 2;

			Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles16, rotation);

			surfaceData = (int16_t*)Collision::g_buzzCollisionStates[index].contactSurfaceData;

			if ((Collision::g_buzzCollisionStates[index].contactSurfaceIndex & 0x4000) != 0)
			{
				rotatedX = (surfaceData[20] * rotation->m01 + rotation->m00 * surfaceData[19] + rotation->m02 * surfaceData[21]) / 4096;
				rotatedY = (surfaceData[20] * rotation->m11 + rotation->m10 * surfaceData[19] + rotation->m12 * surfaceData[21]) / 4096;
				rotatedZ = (surfaceData[20] * rotation->m21 + rotation->m20 * surfaceData[19] + rotation->m22 * surfaceData[21]) / 4096;
			}
			else
			{
				rotatedX = (rotation->m00 * surfaceData[16] + rotation->m02 * surfaceData[18] + rotation->m01 * surfaceData[17]) / 4096;
				rotatedY = (rotation->m10 * surfaceData[16] + rotation->m12 * surfaceData[18] + rotation->m11 * surfaceData[17]) / 4096;
				rotatedZ = (rotation->m20 * surfaceData[16] + rotation->m22 * surfaceData[18] + rotation->m21 * surfaceData[17]) / 4096;
			}

			if (rotatedY > -12000)
			{
				divisor = speed + 3200;

				if (rotatedY >= -4096)
				{
					divisor = (divisor << 12) / 128;
					velocity->x += (rotatedX << 12) / divisor;
					velocity->z += (rotatedZ << 12) / divisor;
				}
				else
				{
					divisor = rotatedY * divisor / 128;
					velocity->x += -4096 * rotatedX / divisor;
					velocity->z += -4096 * rotatedZ / divisor;
				}
			}

		LABEL_28:
			radius = speed + 8000 * Collision::g_buzzCollisionStates[index].floorOffset / 4096 + 6272;

			if (!index)
				Platform::UpdateBuzzPlatformMotion(0, halfStep);

			meshCount = 0;
			doubleRadius = 2 * radius;
			posX = radius + buzzActor->posAngles.pos.x;
			nodeMargin = (radius + 15) / 16;
			posZ = radius + buzzActor->posAngles.pos.z;
			posY = radius + buzzActor->posAngles.pos.y;

			meshList = (int16_t*)Collision::g_mathScratch;

			// the meshes of the grid cells that overlap the area
			for (i = 0; i < Collision::g_unk729120; i++)
			{
				Collision::CollisionGridCell* cell = &Collision::g_collisionGrid[i];

				if ((uint32_t)(posX - cell->minX) < (uint32_t)(cell->extX + doubleRadius) && (uint32_t)(posZ - cell->minZ) < (uint32_t)(doubleRadius + cell->extZ))
				{
					for (j = 0; j < cell->count; j++)
						meshList[meshCount + j] = Collision::g_collisionMeshList[cell->start + j];

					meshCount += cell->count;
				}
			}

			listCount = 0;

			if (meshCount > 0)
			{
				listCount = meshCount;

				for (i = 0; i < meshCount; i++)
				{
					meshId = meshList[i];
					mesh = &Collision::g_collisionMeshInstances[meshId];

					if ((uint32_t)(posX - mesh->boundsMin.x) < (uint32_t)(doubleRadius + mesh->boundsExt.x)
						&& (uint32_t)(posY - mesh->boundsMin.y) < (uint32_t)(doubleRadius + mesh->boundsExt.y)
						&& (uint32_t)(posZ - mesh->boundsMin.z) < (uint32_t)(doubleRadius + mesh->boundsExt.z))
					{
						if (mesh->typeFlags)
						{
							if ((mesh->unk & 0x100) == 0)
							{
								cellY = (posY - mesh->origin.y) / 32;
								node = (Collision::CollisionTreeNode*)mesh->collisionTree;

								cellX = (posX - mesh->origin.x) / 32;
								cellZ = (posZ - mesh->origin.z) / 32;

								while (node->flag >= 0)
								{
									count = node->count;
									triangle = (Collision::CollisionTriangle*)(node + 1);

									if ((uint32_t)(cellX - node->minX) >= (uint32_t)(nodeMargin + node->extX) || (uint32_t)(cellZ - node->minZ) >= (uint32_t)(nodeMargin + node->extZ))
									{
										node = (Collision::CollisionTreeNode*)(triangle + count);
									}
									else
									{
										for (j = 0; j < count; j++)
										{
											if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(nodeMargin + triangle->extX)
												&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z) < (uint32_t)(nodeMargin + (triangle->extZByte << 6))
												&& (uint32_t)(cellY - (triangle->minYByte << 6) - triangle->v0y) < (uint32_t)(nodeMargin + (triangle->extYByte << 6)))
											{
												if (Collision::g_candidateTriangleCount < 240)
												{
													Collision::g_candidateTris[Collision::g_candidateTriangleCount] = triangle;
													Collision::g_candidateMeshIdx[Collision::g_candidateTriangleCount + 1] = meshId;
													Collision::g_candidateTriangleCount++;
												}
											}

											triangle++;
										}

										node = (Collision::CollisionTreeNode*)triangle;
									}
								}
							}
						}
					}
				}
			}

			// the platforms, only for Buzz himself
			if (!index)
			{
				platformRadius = radius + 1024;
				platformMeshCount = Collision::g_unk7284A2;
				platformList = (int16_t*)Collision::g_mathScratch + listCount;

				for (j = 0; j < platformMeshCount; j++)
					platformList[j] = Collision::g_collisionMeshList[Collision::g_unk7284A0 + j];

				for (i = 0; i < platformMeshCount; i++)
				{
					meshId = platformList[i];
					mesh = &Collision::g_collisionMeshInstances[meshId];
					platform = &Platform::g_platformDescriptors[mesh->platformIdx];

					platformIdx = mesh->platformIdx;

					radius = platformRadius + abs(platform->linVel.x) + abs(platform->linVel.y) + abs(platform->linVel.z);

					doubleRadius = 2 * radius;
					nodeMargin = (radius + 15) / 16;

					if ((platform->flags & 8) != 0)
					{
						angles16.x = platform->rotAngle.x >> 2;
						angles16.y = platform->rotAngle.y >> 2;
						angles16.z = platform->rotAngle.z >> 2;

						Nu3D::Math::EulerToRotationMatrix((AnglesWithRoll*)&angles16, rotation);

						localY = (buzzActor->posAngles.pos.y - mesh->origin.y) >> 5;
						localX = (buzzActor->posAngles.pos.x - mesh->origin.x) >> 5;
						localZ = (buzzActor->posAngles.pos.z - mesh->origin.z) >> 5;

						platformX = radius + mesh->origin.x + (localZ * rotation->m20 + localY * rotation->m10 + localX * rotation->m00) / 128;
						platformY = radius + mesh->origin.y + (localZ * rotation->m21 + localY * rotation->m11 + localX * rotation->m01) / 128;
						platformZ = radius + mesh->origin.z + (localZ * rotation->m22 + localY * rotation->m12 + localX * rotation->m02) / 128;
					}
					else
					{
						platformX = radius + buzzActor->posAngles.pos.x;
						platformY = radius + buzzActor->posAngles.pos.y;
						platformZ = radius + buzzActor->posAngles.pos.z;
					}

					if ((uint32_t)(platformX - mesh->boundsMin.x) >= (uint32_t)(doubleRadius + mesh->boundsExt.x)
						|| (uint32_t)(platformZ - mesh->boundsMin.z) >= (uint32_t)(doubleRadius + mesh->boundsExt.z))
					{
						// out of reach, the platform catches up with the motion of this frame
						mesh->origin.x += platform->prevLinVel.x;
						mesh->origin.y += platform->prevLinVel.y;
						mesh->origin.z += platform->prevLinVel.z;
						mesh->boundsMin.x += platform->prevLinVel.x;
						mesh->boundsMin.z += platform->prevLinVel.z;

						platform->prevLinVel.x = 0;
						Platform::g_platformDescriptors[mesh->platformIdx].prevLinVel.y = 0;
						Platform::g_platformDescriptors[mesh->platformIdx].prevLinVel.z = 0;
						Platform::g_platformDescriptors[mesh->platformIdx].rotAngle.x += Platform::g_platformDescriptors[mesh->platformIdx].prevAngVel.x;
						Platform::g_platformDescriptors[mesh->platformIdx].rotAngle.y += Platform::g_platformDescriptors[mesh->platformIdx].prevAngVel.y;
						Platform::g_platformDescriptors[mesh->platformIdx].rotAngle.z += Platform::g_platformDescriptors[mesh->platformIdx].prevAngVel.z;
						Platform::g_platformDescriptors[mesh->platformIdx].prevAngVel.x = 0;
						Platform::g_platformDescriptors[mesh->platformIdx].prevAngVel.y = 0;
						Platform::g_platformDescriptors[mesh->platformIdx].prevAngVel.z = 0;
					}
					else
					{
						Collision::g_unk7290A0[Collision::g_unk7290E4] = (int16_t)mesh->platformIdx;
						Collision::g_unk7290E4++;

						if (mesh->typeFlags)
						{
							if ((mesh->unk & 0x100) == 0)
							{
								cellY = (platformY - mesh->origin.y) / 32;
								node = (Collision::CollisionTreeNode*)mesh->collisionTree;
								cellX = (platformX - mesh->origin.x) / 32;
								cellZ = (platformZ - mesh->origin.z) / 32;

								while (node->flag >= 0)
								{
									count = node->count;
									triangle = (Collision::CollisionTriangle*)(node + 1);

									if ((uint32_t)(cellX - node->minX) >= (uint32_t)(nodeMargin + node->extX) || (uint32_t)(cellZ - node->minZ) >= (uint32_t)(nodeMargin + node->extZ))
									{
										node = (Collision::CollisionTreeNode*)(triangle + count);
									}
									else
									{
										for (j = 0; j < count; j++)
										{
											if ((uint32_t)(cellX - triangle->minX) < (uint32_t)(nodeMargin + triangle->extX)
												&& (uint32_t)(cellZ - (triangle->minZByte << 6) - triangle->v0z) < (uint32_t)(nodeMargin + (triangle->extZByte << 6))
												&& (uint32_t)(cellY - (triangle->minYByte << 6) - triangle->v0y) < (uint32_t)(nodeMargin + (triangle->extYByte << 6)))
											{
												if (Collision::g_candidateTriangleCount < 240)
												{
													Collision::g_candidateTris[Collision::g_candidateTriangleCount] = triangle;
													Collision::g_candidateTriangleCount++;
													Collision::g_candidateMeshIdx[Collision::g_candidateTriangleCount] = platformList[i];
												}

												if ((platform->flags & 8) != 0)
												{
													Collision::g_unk7286A0[Collision::g_unk72D2A8] = triangle;
													Collision::g_unk729134[Collision::g_unk72D2A8] = platformList[i];
													Collision::g_unk72D2A8++;
												}
											}

											triangle++;
										}

										node = (Collision::CollisionTreeNode*)triangle;
									}
								}
							}
						}
					}
				}

				posZ = radius + buzzActor->posAngles.pos.z;
				posX = radius + buzzActor->posAngles.pos.x;
			}

			// the wall segments, a list of nodes of 16 points each
			minCellX = (posX - doubleRadius) / 32;
			minCellZ = (posZ - doubleRadius) / 32;
			maxCellX = posX / 32;
			maxCellZ = posZ / 32;

			edgeNode = (uint8_t*)Collision::g_unk7290F0[1];

			if (edgeNode)
			{
				count = Collision::g_unk72D2AC;

				do
				{
					blockStart = 0;
					blockEnd = *(uint16_t*)edgeNode;

					if (blockEnd)
					{
						edgeBlock = (int32_t*)(edgeNode + 8);

						do
						{
							if (edgeBlock[0] == 0x80000000)
							{
								mask = blockEnd;
							}
							else if ((uint32_t)(maxCellX - edgeBlock[0]) >= (uint32_t)(nodeMargin + edgeBlock[3])
									 || (uint32_t)(maxCellZ - edgeBlock[6]) >= (uint32_t)(nodeMargin + edgeBlock[9]))
							{
								mask = 0;
							}
							else
							{
								mask = blockStart + 16;
							}

							if (blockStart < mask)
							{
								edgePoint = edgeBlock + 1;

								for (steps = mask - blockStart; steps; steps--)
								{
									if (((edgePoint[-2] >= minCellX && edgePoint[1] <= maxCellX) || (edgePoint[1] >= minCellX && edgePoint[-2] <= maxCellX))
										&& ((edgePoint[0] >= minCellZ && edgePoint[3] <= maxCellZ) || (edgePoint[3] >= minCellZ && edgePoint[0] <= maxCellZ)) && count < 32)
									{
										Collision::g_unk728DA0[3 * count] = edgePoint[-2];
										Collision::g_unk728DA0[3 * count + 1] = edgePoint[-1];
										Collision::g_unk728DA0[3 * count + 2] = edgePoint[0];
										count++;
										Collision::g_unk72D2AC = count;

										Collision::g_unk728DA0[3 * count] = edgePoint[1];
										Collision::g_unk728DA0[3 * count + 1] = edgePoint[2];
										Collision::g_unk728DA0[3 * count + 2] = edgePoint[3];
										count++;
										Collision::g_unk72D2AC = count;
									}

									edgePoint += 3;
								}
							}

							blockStart += 16;
							blockEnd = *(uint16_t*)edgeNode;
							edgeBlock += 48;
						} while (blockStart < blockEnd);
					}

					edgeNode = *(uint8_t**)(edgeNode - 4);
				} while (edgeNode);
			}

			*flags = 0;
			Collision::g_buzzCollisionStates[index].wasGroundedLastFrame = 0;
			Collision::g_buzzCollisionStates[index].collisionQuality = 255;

			stuck = 0;

			if (Collision::g_candidateTriangleCount > 0 || Collision::g_unk72D2AC > 0)
			{
				memcpy(&vectorAngles, &buzzActor->posAngles, sizeof(vectorAngles));
				Collision::g_unk72D298 = 0;

				resolvedVelocity.vel.x = (int16_t)velocity->x;
				resolvedVelocity.vel.y = (int16_t)velocity->y;
				resolvedVelocity.vel.z = (int16_t)velocity->z;

				if (index)
				{
					resolvedVelocity.platformVel.x = 0;
					resolvedVelocity.platformVel.y = 0;
					resolvedVelocity.platformVel.z = 0;
				}
				else
				{
					resolvedVelocity.platformVel.x = Collision::g_buzzCollisionStates[0].pushbackVel.x;
					resolvedVelocity.platformVel.y = Collision::g_buzzCollisionStates[0].pushbackVel.y;
					resolvedVelocity.platformVel.z = Collision::g_buzzCollisionStates[0].pushbackVel.z;
				}

				if (halfStep)
				{
					resolvedVelocity.vel.x = (int16_t)velocity->x / 2;
					resolvedVelocity.vel.y = (int16_t)velocity->y / 2;
					resolvedVelocity.vel.z = (int16_t)velocity->z / 2;
					resolvedVelocity.platformVel.x = resolvedVelocity.platformVel.x / 2;
					resolvedVelocity.platformVel.y = resolvedVelocity.platformVel.y / 2;
					resolvedVelocity.platformVel.z = resolvedVelocity.platformVel.z / 2;
				}

				stepCount = (Collision::g_candidateTriangleCount <= 10) + 3;

				do
				{
					if (stepCount <= 0)
						break;

					if (flags[1] == 1)
					{
						platformResult = Collision::ResolvePlatformFooting(&resolvedVelocity, &vectorAngles, velocity, &Collision::g_buzzCollisionStates[savedIndex]);

						if (platformResult == 1)
							*flags = 1;
					}
					else
					{
						platformResult = 0;
					}

					substepResult = Collision::ResolveSubstep(
						&resolvedVelocity,
						&vectorAngles,
						velocity,
						&Collision::g_buzzCollisionStates[savedIndex],
						(uint16_t*)&Collision::g_candidateMeshIdx[1],
						(int32_t*)Collision::g_candidateTris,
						Collision::g_candidateTriangleCount,
						1);

					if (substepResult == 2)
					{
						flags[1] = 0;
						stuck = 1;
					}
					else if (platformResult == 1 || substepResult == 1)
					{
						*flags = 1;
						flags[1] = 1;
					}
					else
					{
						flags[1] = 0;
					}

					--stepCount;
				} while (substepResult > 0);

				memcpy(&buzzActor->posAngles, &vectorAngles, sizeof(vectorAngles));

				if (!index)
				{
					Collision::g_buzzCollisionStates[0].pushbackVel.x = 3 * resolvedVelocity.platformVel.x / 4;
					Collision::g_buzzCollisionStates[0].pushbackVel.y = 3 * resolvedVelocity.platformVel.y / 4;
					Collision::g_buzzCollisionStates[0].pushbackVel.z = 3 * resolvedVelocity.platformVel.z / 4;
				}
			}
			else
			{
				Platform::AdvancePartialMotion(100, 0, 0);
				Platform::AdvancePartialMotion(100, 0, 1);

				flags[1] = 0;

				if (halfStep)
				{
					buzzActor->posAngles.pos.x += velocity->x / 2;
					buzzActor->posAngles.pos.y += velocity->y / 2;
					buzzActor->posAngles.pos.z += velocity->z / 2;
				}
				else
				{
					buzzActor->posAngles.pos.x += velocity->x;
					buzzActor->posAngles.pos.y = velocity->y + buzzActor->posAngles.pos.y;
					buzzActor->posAngles.pos.z = velocity->z + buzzActor->posAngles.pos.z;
				}

				Collision::g_buzzCollisionStates[index].pushbackVel.x = 0;
				Collision::g_buzzCollisionStates[index].pushbackVel.y = 0;
				Collision::g_buzzCollisionStates[index].pushbackVel.z = 0;
			}

			posY = Collision::g_buzzCollisionStates[index].floorOffset + 192 + buzzActor->posAngles.pos.y;
			posX = buzzActor->posAngles.pos.x;
			buzzActor->posAngles.pos.y = posY;

			if (posX == Toy2::g_buzzActor.motionTargetPos.x && posY == Toy2::g_buzzActor.motionTargetPos.y && buzzActor->posAngles.pos.z == Toy2::g_buzzActor.motionTargetPos.z
				&& stuck > 0)
				Collision::g_buzzCollisionStates[index].stuckTimer += Renderer::g_frameDelta;
			else
				Collision::g_buzzCollisionStates[index].stuckTimer = 0;

			if (Collision::g_buzzCollisionStates[index].stuckTimer > 20)
			{
				Collision::g_buzzCollisionStates[index].stuckTimer = 20;
				*flags = 1;
			}

			Collision::g_buzzCollisionStates[index].responseVel.x = 0;
			Collision::g_buzzCollisionStates[index].responseVel.y = 0;
			Collision::g_buzzCollisionStates[index].responseVel.z = 0;
		}

		// FUNCTION: TOY2 0x004855F0
		void HandleCollisions(Toy2::Buzz::Toy2BuzzActor* buzzActor, Vector3I* velocity, uint8_t* flags, int32_t index)
		{
			if (velocity->z * velocity->z + velocity->y * velocity->y + velocity->x * velocity->x <= 0x400000)
			{
				ResolveCollisions(buzzActor, velocity, flags, index, 0);
			}
			else
			{
				ResolveCollisions(buzzActor, velocity, flags, index, 1);

				uint8_t newFlags = flags[0] | flags[1];

				ResolveCollisions(buzzActor, velocity, flags, index, 2);

				flags[1] |= newFlags;
			}
		}

		// FUNCTION: TOY2 0x00434550
		void ResolveFooting(Toy2::Buzz::Toy2BuzzActor* buzzActor)
		{
			int32_t velX = buzzActor->velX;
			int32_t gravityVel = buzzActor->gravityVel;
			int32_t velForward = buzzActor->velForward;
			int32_t surfaceQuality;
			int32_t previousQuality;

			HandleCollisions(buzzActor, (Vector3I*)&buzzActor->velX, (uint8_t*)&buzzActor->collisionFlags, 0);

			buzzActor->floorYPos = UpdateFloorHeight(buzzActor);
			buzzActor->isOnWalkableFloor = Nu3D::Collision::IsFloorWalkable();

			surfaceQuality = g_unk5281A4;

			if (g_unk5281A4 == -1)
			{
				previousQuality = g_unk52B81C;
				surfaceQuality = Nu3D::Collision::GetSurfaceQuality(0);
				g_unk52B81C = surfaceQuality;

				if (!buzzActor->collisionFlags && buzzActor->unkShort40)
				{
					surfaceQuality = previousQuality;
					g_unk52B81C = surfaceQuality;
				}
			}
			else
			{
				g_unk52B81C = surfaceQuality;
			}

			g_unk5281A4 = -1;

			if (surfaceQuality >= 0 && !g_levelTransition)
			{
				if (surfaceQuality >> 2)
				{
					if (surfaceQuality >> 2 == 4)
					{
						HandleDamage(0, 4);

						buzzActor->posAngles.pos.x = velX + buzzActor->motionTargetPos.x;
						buzzActor->gravityVel = gravityVel;
						buzzActor->velX = velX;
						buzzActor->velForward = velForward;
						buzzActor->posAngles.pos.y = gravityVel + buzzActor->motionTargetPos.y;
						buzzActor->posAngles.pos.z = velForward + buzzActor->motionTargetPos.z;
						buzzActor->collisionFlags = 0;
						buzzActor->unkShort40 = 0;

						surfaceQuality = g_unk52B81C;
					}
				}
				else if (!buzzActor->inhibitMovementTimer && !buzzActor->stunTimer)
				{
					HandleDamage(0, 2);

					surfaceQuality = g_unk52B81C;
				}
			}

			if (g_unk53C818)
			{
				if (surfaceQuality != 13 && buzzActor->unkShort40)
					g_unk53C818 = 0;
				else
					goto LABEL_19;
			}

			if (surfaceQuality == 13 && buzzActor->unkShort40)
				g_unk53C818 = 2;

		LABEL_19:
			if (abs(buzzActor->gravityVel) < 2)
				buzzActor->gravityVel = 0;
		}

		// STUB: TOY2 0x004A4910
		void RefreshDiscAmm() {}

		// STUB: TOY2 0x004A4E60
		void DeactivateRocketBoots() {}

		// FUNCTION: TOY2 0x00436220
		void HandleGameplay(Toy2::Buzz::Toy2BuzzActor* buzzActor)
		{
			MovementParams movementParams;
			int32_t frameDelta = Renderer::g_frameDelta;
			int32_t turnSpeed;
			int32_t speedMultiplier;
			int32_t actorFlags;
			int32_t turnDelta;
			int32_t turnStep;
			int32_t angleDiff;
			int32_t newYaw;
			int32_t actionState;
			int32_t stunTimer;
			int32_t inhibitMovementTimer;
			int32_t gunFlags;
			int32_t ziplineResult;
			int32_t swingResult;
			int32_t poleResult;
			int32_t moveFlags;
			int32_t facingResult;
			int32_t groundSlamTimer;
			int32_t y;
			int32_t z;
			int32_t gravityVel;
			int32_t velForward;

			g_unk53C634 = 0;

			RefreshDiscAmm();

			movementParams.maxSpeed = 896;
			movementParams.maxAirSpeed = 1024;
			movementParams.initialJumpVelocity = -1536;
			movementParams.maxTurnRate = 768;
			movementParams.groundAccel = (frameDelta << 9) / 4;
			movementParams.airAccel = 192 * frameDelta / 4;

			turnSpeed = 160 * frameDelta / 4;
			movementParams.turnSpeed = turnSpeed;

			if (g_rocketBootsDuration)
				movementParams.initialJumpVelocity = -1600;

			if (buzzActor->inhibitMovementTimer > 0)
			{
				movementParams.initialJumpVelocity = -960;
				movementParams.groundAccel = (frameDelta << 6) / 4;
				movementParams.airAccel = 32 * frameDelta / 4;
				turnSpeed = 80 * frameDelta / 4;
				movementParams.turnSpeed = turnSpeed;
			}

			buzzActor->actorFlags &= ~0x100;
			actorFlags = buzzActor->actorFlags;

			if (g_currentFluidType)
			{
				if (g_currentFluidType == 1)
				{
					if (buzzActor->posAngles.pos.y <= g_underwaterYLevel + 0x2000)
						goto LABEL_13;

					goto LABEL_12;
				}

				if (g_currentFluidType > 1 && g_currentFluidType <= 3 && buzzActor->posAngles.pos.y > g_underwaterYLevel)
				{
				LABEL_12:
					actorFlags |= 0x100;
					buzzActor->actorFlags = actorFlags;
				}
			}

		LABEL_13:
			if ((g_unk52B81C & ~3) == 4)
			{
				movementParams.groundAccel = (frameDelta << 6) / 4;
				movementParams.airAccel = 32 * frameDelta / 4;
				turnSpeed = 80 * frameDelta / 4;
				movementParams.turnSpeed = turnSpeed;
			}

			if (g_unk53C818)
			{
				g_buzzActor.actorFlags &= ~4;
				movementParams.maxTurnRate = 0;
				turnDelta = g_unk53C814 - buzzActor->posAngles.angles.yaw;

				movementParams.groundAccel = 16 * frameDelta / 4;
				movementParams.airAccel = movementParams.groundAccel;

				if (((turnDelta + 1024) & 0xFFFu) <= 0x800)
					turnStep = 5 * frameDelta;
				else
					turnStep = -frameDelta;

				turnSpeed = 8 * turnStep / 4;
				movementParams.turnSpeed = turnSpeed;

				if ((InputManager::g_directionInputState & (DIR_UP | DIR_RIGHT | DIR_DOWN | DIR_LEFT)) != 0)
				{
					movementParams.maxTurnRate = 256;
				}
				else
				{
					angleDiff = turnDelta & 0xFFF;

					if (angleDiff > 2048)
						angleDiff -= 4096;

					newYaw = (buzzActor->posAngles.angles.yaw + (angleDiff >> 3)) & 0xFFF;
					buzzActor->posAngles.angles.yaw = newYaw;
					buzzActor->facingAngle = newYaw;
				}

				if (buzzActor->unkShort40)
					g_unk53C634 |= 4;
			}

			if (g_curMoveableObject)
				movementParams.maxSpeed = 384;

			actorFlags = buzzActor->actorFlags;
			buzzActor->surfaceClampY = 0x80000000;

			if ((actorFlags & 0x100) == 0)
				goto LABEL_34;

			if (g_currentFluidType != 1)
			{
				if (g_currentFluidType >= 2)
				{
					movementParams.maxSpeed = 256;
					movementParams.initialJumpVelocity = -768;
					movementParams.groundAccel = (frameDelta << 6) / 4;
					movementParams.airAccel = 32 * frameDelta / 4;
					movementParams.turnSpeed = 80 * frameDelta / 4;

					buzzActor->surfaceClampY = g_underwaterYLevel;
				}

			LABEL_34:
				speedMultiplier = frameDelta;
				goto LABEL_35;
			}

			speedMultiplier = frameDelta;
			movementParams.maxSpeed = 640;
			movementParams.initialJumpVelocity = -768;
			movementParams.groundAccel = (frameDelta << 9) / 16;
			movementParams.airAccel = (frameDelta << 7) / 16;

			if (turnSpeed >= 0)
				movementParams.turnSpeed = 160 * frameDelta / 16;

		LABEL_35:
			g_buzzActionState = buzzActor->unkShort40 != 0;

			if (g_unk53C83C < 0)
				g_buzzActionState |= 0x2;

			if (g_unk53C660)
				g_buzzActionState |= 0x4;

			if (g_unk53C618)
				g_buzzActionState |= 0x8;

			if (g_unk53C668 > 0)
				g_buzzActionState |= 0x10;

			if (g_unk53C838 < 0 || g_unk53C838 == 80)
				g_buzzActionState |= 0x20;

			if (g_groundSlamTicker)
				g_buzzActionState |= 0x40;

			if (g_fireChargeTimer)
				g_buzzActionState |= 0x80;

			if (Camera::g_cameraState)
				g_buzzActionState |= 0x100;

			if (g_curMoveableObject)
				g_buzzActionState |= 0x200;

			if (g_unk53C66C > 15)
				g_buzzActionState |= 0x400;

			if (buzzActor->inhibitMovementTimer > 0)
				g_buzzActionState |= 0x800;

			if (g_rocketBootsDuration)
				g_buzzActionState |= 0x1000;

			if (g_grappleState)
				g_buzzActionState |= 0x2000;

			if (g_gravityBootsDuration)
				g_buzzActionState |= 0x4000;

			if (g_unk53C608)
				g_buzzActionState |= 0x8000;

			if (g_unk53C818)
				g_buzzActionState |= 0x10000;

			if (buzzActor->unkShort40)
			{
				if (g_unk53C608)
				{
					g_unk53C608 = 0;
					g_buzzActionState &= ~0x8000;
				}
			}
			else
			{
				movementParams.airAccel = (speedMultiplier << 6) / 4;
			}

			if (g_buzzSlipTimer > 0)
			{
				g_buzzSlipTimer -= speedMultiplier;

				if (g_buzzSlipTimer < 0)
					g_buzzSlipTimer = 0;

				if ((InputManager::g_directionInputState & (DIR_UP | DIR_RIGHT | DIR_DOWN | DIR_LEFT)) != 0)
				{
					movementParams.airAccel = 16 * speedMultiplier / 4;
					movementParams.groundAccel = (speedMultiplier << 6) / 4;
				}
			}

			if (buzzActor->stunTimer)
			{
				if (buzzActor->collisionFlags)
				{
					*(uint8_t*)&buzzActor->actorFlags &= 0xFD;
					speedMultiplier = frameDelta;
				}

				actorFlags = buzzActor->actorFlags;

				if ((actorFlags & 0x2) != 0)
				{
					movementParams.groundAccel = 0;
					movementParams.airAccel = 0;
				}

				stunTimer = buzzActor->stunTimer;

				if (stunTimer > 0)
				{
					stunTimer = stunTimer - speedMultiplier;
					buzzActor->stunTimer = stunTimer;

					if (stunTimer <= 0)
					{
						buzzActor->stunTimer = 0;
						buzzActor->actorFlags = actorFlags & 0xFFDF;
					}

					speedMultiplier = frameDelta;
				}

				stunTimer = buzzActor->stunTimer;

				if (stunTimer < 0)
				{
					stunTimer = speedMultiplier + stunTimer;
					buzzActor->stunTimer = stunTimer;

					if (stunTimer >= 0)
					{
						*(uint8_t*)&buzzActor->actorFlags &= 0xDF;
						buzzActor->stunTimer = 0;
					}
				}
			}

			actorFlags = buzzActor->actorFlags;

			if ((actorFlags & 0x20) != 0)
			{
				if (++g_unk53C830 < 3)
				{
					*(uint8_t*)&buzzActor->actorFlags |= 1;
				}
				else
				{
					g_unk53C830 = 0;
					*(uint8_t*)&buzzActor->actorFlags &= 0xFE;
				}
			}
			else
			{
				actorFlags |= 1;
				buzzActor->actorFlags = actorFlags;
			}

			actorFlags = buzzActor->actorFlags;

			if ((actorFlags & 0x40) != 0)
			{
				if (buzzActor->collisionFlags)
				{
					actorFlags &= 0xBB;
					buzzActor->actorFlags = actorFlags;
				}
				else
				{
					movementParams.groundAccel = 0;
					movementParams.maxSpeed = 2944;
					movementParams.maxAirSpeed = 2944;
					movementParams.airAccel = 0;
				}
			}

			inhibitMovementTimer = buzzActor->inhibitMovementTimer;

			if (inhibitMovementTimer > 0)
			{
				inhibitMovementTimer = inhibitMovementTimer - frameDelta;
				buzzActor->inhibitMovementTimer = inhibitMovementTimer;

				if (inhibitMovementTimer <= 0)
				{
					RespawnCosmicShield();
					buzzActor->inhibitMovementTimer = 0;
				}
			}

			if (buzzActor->unkShort40 || (buzzActor->stunTimer <= 0 && buzzActor->health >= 0))
				g_unk53C834 = 0;

			g_unk53C658 -= frameDelta;

			if (g_unk53C658 < 0)
				g_unk53C658 = 0;

			if ((InputManager::g_directionInputState & INPUT_VISOR_TOGGLE) != 0 && (InputManager::g_prevDirectionInputState & 0x400) == 0
				&& (g_buzzActionState & 0xFFF7F) == 1)
			{
				Camera::g_cameraState = 3;
				g_buzzActionState |= 0x100;
				InputManager::g_prevDirectionInputState |= 0x400;

				buzzActor->velX = 0;
				buzzActor->gravityVel = 0;
				buzzActor->velForward = 0;

				g_unk53C83C = 0;
			}

			gunFlags = Camera::g_cameraState != 0 ? 8 : 0;

			TickGunFire(&buzzActor->posAngles);

			moveFlags = gunFlags | TickGroundSlam((Actor::Toy2Actor*)buzzActor);

			TickSpinHover(buzzActor);

			ziplineResult = HandleZipline(buzzActor) - 1;

			if (!ziplineResult)
				goto LABEL_136;

			if (ziplineResult == 1)
				goto LABEL_145;

			swingResult = HandleSwing(buzzActor) - 1;

			if (!swingResult)
				goto LABEL_136;

			if (swingResult == 1)
				goto LABEL_145;

			poleResult = HandlePoleClimb(buzzActor) - 1;

			if (!poleResult)
				goto LABEL_136;

			if (poleResult == 1)
				goto LABEL_145;

			moveFlags = Camera::HandleLedgeClimb(buzzActor) | moveFlags;

			if (g_unk53C838 < 0)
			{
				*(uint8_t*)&buzzActor->actorFlags |= 4;
				g_unk53C838 += frameDelta;

				if (g_unk53C838 >= 0)
				{
					g_unk53C838 = 0;
					*(uint8_t*)&buzzActor->actorFlags &= 0xFB;
				}
			}

			if (g_grappleState == 2)
			{
				moveFlags = (moveFlags & 0xFFFFFF00) | 16;
				goto LABEL_123;
			}

			if (!moveFlags)
			{
				if (g_unk53C5DC > 0)
				{
					g_unk53C5DC -= frameDelta;
					buzzActor->velX = 0;
					buzzActor->velForward = 0;
				}

				if (g_gravityBootsDuration)
					Camera::UpdateGravityBoots(buzzActor);
				else
					UpdateJumpAndGravity(buzzActor, movementParams.initialJumpVelocity, 0);

			LABEL_136:
				if (g_rocketBootsDuration)
					facingResult = Camera::UpdateRocketBoots(buzzActor, &movementParams);
				else
					facingResult = UpdateFacing(buzzActor, &movementParams);

				goto LABEL_140;
			}

		LABEL_123:
			if ((moveFlags & 1) != 0)
				buzzActor->velX = 0;

			if ((moveFlags & 2) != 0)
				buzzActor->gravityVel = 0;

			if ((moveFlags & 4) != 0)
				buzzActor->velForward = 0;

			if ((moveFlags & 8) != 0)
			{
				facingResult = 0;

				UpdateJumpAndGravity(buzzActor, movementParams.initialJumpVelocity, 0);

			LABEL_140:
				if (g_unk53C818)
				{
					if (movementParams.turnSpeed)
					{
						facingResult = 1;

						if (movementParams.turnSpeed < 0)
						{
							facingResult = -1;
							movementParams.turnSpeed = -movementParams.turnSpeed;
						}
					}
				}

				UpdateHorizontalMovement(buzzActor, &movementParams, facingResult);
			}

		LABEL_145:
			y = buzzActor->posAngles.pos.y;
			z = buzzActor->posAngles.pos.z;
			buzzActor->motionTargetPos.x = buzzActor->posAngles.pos.x;
			gravityVel = buzzActor->gravityVel;
			buzzActor->motionTargetPos.y = y;
			buzzActor->motionTargetPos.z = z;
			g_unk53C810 = gravityVel;

			velForward = buzzActor->velForward;
			buzzActor->velX *= frameDelta;
			buzzActor->gravityVel *= frameDelta;
			buzzActor->velForward = frameDelta * velForward;

			MoveableObject::Update(buzzActor);
			ResolveFooting(buzzActor);

			buzzActor->velX /= frameDelta;
			buzzActor->gravityVel /= frameDelta;
			buzzActor->velForward /= frameDelta;

			if (buzzActor->collisionFlags == 0)
			{
				int16_t unkShort40 = buzzActor->unkShort40;

				if (unkShort40 > 0)
				{
					unkShort40 = unkShort40 - frameDelta;
					buzzActor->unkShort40 = unkShort40;

					if (unkShort40 <= 0)
						buzzActor->unkShort40 = 0;
				}
			}
			else
			{
				buzzActor->unkShort40 = 6;
			}

			Shadow::QueueStretchedForBuzz(buzzActor->posAngles.pos.x, buzzActor->floorYPos, buzzActor->posAngles.pos.z, 100);

			if (g_unk53C818 && buzzActor->collisionFlags && abs(buzzActor->floorYPos - buzzActor->posAngles.pos.y) < 4096)
				g_unk53C814 = Nu3D::Math::CartesianToFixedAngle(Collision::g_floorSlope.x, Collision::g_floorSlope.z) & 0xFFF;

			if ((g_buzzActionState & 0x18010) != 0)
			{
				groundSlamTimer = 0;
				g_unk53C838 = 0;
			}
			else
			{
				groundSlamTimer = g_unk53C838;

				if (g_unk53C838 < 0)
					goto LABEL_168;
			}

			if (buzzActor->gravityVel <= 128 || buzzActor->collisionFlags)
			{
				if (buzzActor->unkShort40)
				{
					if (groundSlamTimer == 80)
					{
						g_unk53C838 = -70;
						g_unk53C634 |= 0x10;

						AudioManager::PlaySoundEffect(47, &buzzActor->posAngles.pos);

						buzzActor->velX = 0;
						buzzActor->gravityVel = 0;
						buzzActor->velForward = 0;

						groundSlamTimer = g_unk53C838;
					}

					if (groundSlamTimer > 0)
					{
						g_unk53C838 = 0;
						g_unk53C634 |= 0x10;
					}
				}
				else if (groundSlamTimer != 80)
				{
					g_unk53C838 = 0;
				}
			}
			else
			{
				g_unk53C838 = frameDelta + groundSlamTimer;

				if (frameDelta + groundSlamTimer > 60)
				{
					AudioManager::PlaySoundEffect(22, &buzzActor->posAngles.pos);
					g_buzzActor.actorFlags &= ~4;
					g_unk53C838 = 80;
					g_unk53C83C = 0;
					g_groundSlamTicker = 0;
					DeactivateRocketBoots();
				}
			}

		LABEL_168:
			if (buzzActor->collisionFlags)
				g_unk53C62C = 0;

			if (buzzActor->posAngles.pos.y > Collision::g_unk729124 + 0x2000 && !g_levelTransition)
			{
				if (g_unk53C62C < 1)
				{
					++g_unk53C62C;

					if (buzzActor->lives < 9)
						buzzActor->lives = buzzActor->lives + 1;
				}

				HandleDamage(0, 4);
			}
		}

		// FUNCTION: TOY2 0x00433D50
		void Init(Toy2::Buzz::Toy2BuzzActor* buzzActor, int32_t levelIndex)
		{
			int16_t lives = buzzActor->lives;
			int16_t health = buzzActor->health;

			memset(buzzActor, 0, sizeof(Toy2BuzzActor));

			SpawnLocation* spawnLocation = &g_spawnLocations[levelIndex];

			buzzActor->posAngles.pos.x = spawnLocation->x;
			buzzActor->posAngles.pos.y = spawnLocation->y;
			buzzActor->posAngles.pos.z = spawnLocation->z;

			int32_t floorY = UpdateFloorHeight(buzzActor);

			floorY -= 256;

			buzzActor->motionTargetPos.x = buzzActor->posAngles.pos.x;
			buzzActor->posAngles.pos.y = floorY;
			buzzActor->motionTargetPos.y = floorY;
			buzzActor->motionTargetPos.z = buzzActor->posAngles.pos.z;

			buzzActor->respawnPos.y = floorY;
			buzzActor->respawnPos.x = buzzActor->posAngles.pos.x;
			buzzActor->respawnYawAngle = (int16_t)spawnLocation->rotation;

			buzzActor->posAngles.angles.yaw = (uint16_t)spawnLocation->rotation;
			buzzActor->facingAngle = (int16_t)spawnLocation->rotation;
			buzzActor->surfaceClampY = 0x80000000;
			buzzActor->lives = lives;
			buzzActor->health = health;
			buzzActor->respawnPos.z = buzzActor->posAngles.pos.z;
			buzzActor->isOnWalkableFloor = 0;
			buzzActor->primaryAnimIdx = -1;
			buzzActor->secondaryAnimIdx = -1;
			buzzActor->visibilityDistance = 1280;

			g_unk53C660 = 0;
			g_unk53C838 = 0;
			g_unk53C668 = 0;
			g_unk53C658 = 0;
			g_unk53C5E4 = 0;
			g_buzzSlipTimer = 0;
			g_unk53C618 = 0;
			g_unk53C600 = 0;
			g_unk53C610 = 0;
			g_unk53C83C = 0;
			g_unk53C650 = 0;
			g_groundSlamTicker = 0;
			g_buzzActionState = 0;
			g_fireChargeTimer = 0;
			g_fireHoldTimer = 0;
			g_curMoveableObject = 0;
			g_unk53C624 = 0;
			g_unk53C66C = 0;
			g_unk53C834 = 0;
			g_unk53C830 = 0;
			g_unk52B81C = -1;
			g_unk5281A4 = -1;
			g_unk53C634 = 0;
			g_unk53C60C = 0;
			g_unk53C810 = 0;
			g_underwaterYLevel = 0;
			g_currentFluidType = 0;
			g_unk53C62C = 0;
			g_greenGunCharge = 0;
			g_unk53C608 = 0;
			g_unk53C818 = 0;
			InputManager::g_directionInputState2Frames = 0;
			InputManager::g_directionInputState3Frames = 0;
			g_unk50A098 = 0;
			g_unk53C678 = 0;
			g_unk53C5D0 = 0;
			g_unk53C674 = buzzActor->posAngles.pos.y;
			g_unk53C61C = 0;
			g_unk53C664 = 0;
			g_unk53C5DC = 0;
		}

		// STUB: TOY2 0x004A28F0
		void UpdateRespawnAnchor() {}

		// STUB: TOY2 0x004A2D80
		void UpdateContactEffects() {}

		// STUB: TOY2 0x004A4960
		void FireDiscLauncher(int16_t pitch) {}

		// STUB: TOY2 0x004A4B90
		void ResetGravityBoots() {}

		// STUB: TOY2 0x004A4D60
		void ActivateRocketBoots(Toy2::Levels::Type63* a1) {}

		// STUB: TOY2 0x004A5170
		void TickCosmicShield() {}

		// STUB: TOY2 0x004A5340
		void CancelGrapple() {}

		// STUB: TOY2 0x004A5370
		uint32_t FireGrapple() { return 0; }

		// STUB: TOY2 0x004A5540
		void TickGrapple() {}

		// STUB: TOY2 0x004A5C40
		void TickBeamShots() {}

		// STUB: TOY2 0x004A62A0
		void TickGadgets() {}
	}
}
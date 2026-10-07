#include "Toy2/Buzz.h"
#include "Toy2/Actor.h"
#include "Toy2/Collision.h"
#include "Toy2/Platform.h"
#include "Nu3D/Collision.h"
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

		// STUB: TOY2 0x004340D0
		void UpdateJumpAndGravity(Toy2::Buzz::Toy2BuzzActor* buzzActor, int32_t initialJumpVelocity, int32_t forceJump) {}

		// STUB: TOY2 0x004343D0
		int32_t UpdateHorizontalMovement(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movementParams, int32_t flag) { return 0; }

		// STUB: TOY2 0x00434550
		void ResolveFooting(Toy2::Buzz::Toy2BuzzActor* buzzActor) {}

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

		// STUB: TOY2 0x00436220
		void HandleGameplay(Toy2::Buzz::Toy2BuzzActor* buzzActor) {}

		// STUB: TOY2 0x00484380
		void ResolveCollisions(Toy2::Buzz::Toy2BuzzActor* buzzActor, Vector3I* velocity, uint8_t* flags, int32_t index, int32_t halfStep) {}

		// STUB: TOY2 0x004855F0
		void HandleCollisions(Toy2::Buzz::Toy2BuzzActor* buzzActor, Vector3I* velocity, int16_t* flags, int32_t index) {}

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

		// STUB: TOY2 0x004A4910
		void RefreshDiscAmm() {}

		// STUB: TOY2 0x004A4960
		void FireDiscLauncher(int16_t pitch) {}

		// STUB: TOY2 0x004A4B90
		void ResetGravityBoots() {}

		// STUB: TOY2 0x004A4D60
		void ActivateRocketBoots(Toy2::Levels::Type63* a1) {}

		// STUB: TOY2 0x004A4E60
		void DeactivateRocketBoots() {}

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
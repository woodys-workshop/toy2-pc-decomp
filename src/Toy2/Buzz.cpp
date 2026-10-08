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
#include "Toy2/Levels/EvilEmperorZurg.h"
#include "Nu3D/Particles.h"
#include "Toy2/Camera.h"
#include "Renderer/Renderer.h"
#include "Renderer/Effects.h"
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

		// GLOBAL: TOY2 0x0053C5F0
		int32_t g_unk53C5F0;

		// GLOBAL: TOY2 0x0053C5F4
		int32_t g_unk53C5F4;

		// GLOBAL: TOY2 0x0053C5F8
		int32_t g_unk53C5F8;

		// GLOBAL: TOY2 0x0053C654
		int32_t g_unk53C654;

		// GLOBAL: TOY2 0x0053C604
		int32_t g_unk53C604;

		// GLOBAL: TOY2 0x0053C614
		int32_t g_unk53C614;

		// GLOBAL: TOY2 0x0053C5D8
		int32_t g_unk53C5D8;

		// GLOBAL: TOY2 0x0053C638
		Vector3I g_ziplineDirection;

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

		// the event tracks of Buzz's animations, read by StepEventTrack
		// GLOBAL: TOY2 0x004DF040
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

		// FUNCTION: TOY2 0x00414110
		void Respawn()
		{
			if (g_levelFileIndex == 12)
				EvilEmperorZurg::BuzzRespawn();

			g_buzzActor.posAngles.pos = g_buzzActor.respawnPos;
			g_buzzActor.motionTargetPos = g_buzzActor.respawnPos;
			g_buzzActor.floorYPos = g_buzzActor.respawnPos.y;

			--g_buzzActor.lives;
			g_buzzActor.posAngles.angles.pitch = 0;
			g_buzzActor.posAngles.angles.yaw = g_buzzActor.respawnYawAngle;
			g_buzzActor.rollAngle = 0;
			g_buzzActor.facingAngle = g_buzzActor.respawnYawAngle;
			g_buzzActor.velX = 0;
			g_buzzActor.gravityVel = 0;
			g_buzzActor.velForward = 0;
			g_buzzActor.unkVar30 = 0;
			g_buzzActor.unkVar31 = 0;
			*(int32_t*)&g_buzzActor.unkWord13 = 0;
			g_buzzActor.unkVar33 = 0;
			g_buzzActor.surfaceClampY = 0x80000000;
			g_buzzActor.inhibitMovementTimer = 0;
			g_buzzActor.unkWord15 = 0;
			g_buzzActor.collisionFlags = 0;
			g_buzzActor.unkShort40 = 0;
			g_buzzActor.unkVar37 = 0;
			g_buzzActor.unkVar38 = 0;
			g_buzzActor.actorFlags = 0x20;
			g_buzzActor.stunTimer = (int16_t)65376;
			g_buzzActor.health = 14;

			g_unk52B81C = -1;
			g_unk5281A4 = -1;
			Camera::InitGameplayCamera(&Camera::g_gameplayCameraState, &g_buzzActor);
			Nu3D::Particles::Init();
			RespawnCosmicShield();
			g_curSequencePlayer = 0;
			memset(g_soundSequencePlayers, 0, 160);
			g_feedbackTimerA = 0;
			g_feedbackTimerB = 0;
			g_feedbackTimerA_sub = 0;
			g_hudWidgetTimers[0] = 180;
			g_hudWidgetPositions[0] = 1024;
			ResetBuzzState();
		}

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

		// FUNCTION: TOY2 0x004071E0
		void HandleDamage(int16_t hitAngle, uint8_t hitType)
		{
			int32_t cameraState;
			uint8_t effectiveHitType;

			if (g_buzzActor.health < 0 || (Camera::g_inCutscene & 1) != 0)
				return;

			cameraState = Camera::g_cameraState;

			if (Camera::g_cameraState)
			{
				cameraState = 5;
				Camera::g_cameraState = 5;
			}

			effectiveHitType = hitType;

			if ((hitType & 1) != 0)
			{
				g_buzzActor.velX = Numerics::g_sinCosLUT[hitAngle & 0xFFF] / 16;
				g_buzzActor.velForward = Numerics::g_sinCosLUT[(hitAngle + 1024) & 0xFFF] / 16;
			}

			if ((hitType & 2) == 0)
				goto LABEL_19;

			if (g_buzzActor.stunTimer > 0)
				goto LABEL_18;

			g_buzzActor.actorFlags |= 2;

			if (g_buzzActor.inhibitMovementTimer)
			{
				if (g_buzzActor.inhibitMovementTimer >= 0)
				{
				LABEL_17:
					g_buzzActor.stunTimer = -60;
					goto LABEL_18;
				}

				if (!g_buzzActor.stunTimer)
				{
					++g_buzzActor.inhibitMovementTimer;
					goto LABEL_17;
				}

			LABEL_18:
				effectiveHitType = hitType & 0xFD;
				goto LABEL_19;
			}

			if (g_levelTransition == 1 || g_levelTransition == 5 || g_unk53C660)
				goto LABEL_17;

		LABEL_19:
			if ((effectiveHitType & 4) != 0)
			{
				if (g_levelTransition != 2)
				{
					if (cameraState)
						Camera::g_cameraState = 5;

					AudioManager::Preset::PlayOneShotSound2(21, (Actor::Toy2Actor*)&g_buzzActor);

					if (g_feedbackTimerA < 14)
						g_feedbackTimerA = 14;

					g_buzzActor.actorFlags |= 0x14;
					g_levelTransition = 2;
					g_levelTransitionTimer = 46;
					g_buzzActor.stunTimer = -90;
					g_hudWidgetTimers[0] = 180;
				}
			}
			else if ((effectiveHitType & 2) != 0)
			{
				--g_buzzActor.health;

				g_unk53C61C = 0;
				g_buzzActor.unkWord15 = 5;
				g_buzzActor.gravityVel = -512;
				g_buzzActor.collisionFlags = 0;
				g_buzzActor.unkShort40 = 0;

				ResetBuzzState();
				g_unk53C834 = 1;
				g_hudWidgetTimers[1] = 180;

				if (g_buzzActor.health >= 0)
				{
					g_buzzActor.stunTimer = 90;

					if (!AudioManager::IsActorSoundPlaying(&g_buzzActor))
						AudioManager::Preset::PlayOneShotSound2(26, (Actor::Toy2Actor*)&g_buzzActor);

					if (g_feedbackTimerA < 14)
						g_feedbackTimerA = 14;
				}
				else if (g_levelTransition != 2)
				{
					if (Camera::g_cameraState)
						Camera::g_cameraState = 5;

					AudioManager::Preset::PlayOneShotSound2(21, (Actor::Toy2Actor*)&g_buzzActor);

					if (g_feedbackTimerA < 14)
						g_feedbackTimerA = 14;

					g_buzzActor.actorFlags |= 4;

					g_hudWidgetTimers[0] = 180;
					g_levelTransition = 2;
					g_levelTransitionTimer = 120;

					g_buzzActor.stunTimer = -90;
				}
			}
		}

		// FUNCTION: TOY2 0x004100F0
		void CheckParticleCollisions()
		{
			int32_t hitType = 0;
			Nu3D::Particles::ParticleInstance* instance;
			int32_t distZ;
			int32_t soundId;
			int16_t coinsCollected;
			int16_t hitAngle;
			int32_t knockbackIdx;
			int32_t collisionFlags = 0;
			Nu3D::Particles::ParticleInstance* hitInstance;
			int32_t remaining = 64;
			int32_t x = g_buzzActor.posAngles.pos.x;
			int32_t y = g_buzzActor.posAngles.pos.y - 7360;
			int32_t z = g_buzzActor.posAngles.pos.z;

			instance = Nu3D::Particles::g_particleInstances;

			do
			{
				if (instance->lifetime > 0 && (instance->renderFlags & 2) != 0)
				{
					distZ = (z - instance->pos.z) << 8 >> 13;

					if (((x - instance->pos.x) << 8 >> 13) * ((x - instance->pos.x) << 8 >> 13)
							+ ((y - instance->pos.y) << 7 >> 13) * ((y - instance->pos.y) << 7 >> 13)
							+ distZ * distZ
						< (instance->width + 100) * (instance->width + 100))
					{
						soundId = 0;

						switch (instance->spriteSheet)
						{
						case 0:
						case 1:
						case 0x13:
							goto LABEL_7;

						case 4:
						case 0x33:
						case 0x34:
							hitType = 2;
							collisionFlags = instance->collisionFlags;
							goto LABEL_21;

						case 5:
							hitType = 2;
							collisionFlags = instance->collisionFlags;
							goto LABEL_21;

						case 7:
							if (!g_buzzActor.unkShort40)
								break;

						LABEL_7:
							hitType = 1;
							goto LABEL_20;

						case 0x10:
							coinsCollected = g_buzzActor.coinsCollected;
							soundId = -1;
							collisionFlags = 5;
							g_hudWidgetTimers[2] = 180;

							if (g_buzzActor.coinsCollected < 99)
								coinsCollected = ++g_buzzActor.coinsCollected;

							if (coinsCollected == 50)
								AudioManager::PlaySoundEffect(79, 0);

							break;

						case 0x15:
							collisionFlags = 4;
							soundId = 0;
							hitType = 1;
							goto LABEL_22;

						case 0x16:
							if (g_buzzActor.collisionFlags)
								g_unk5281A4 = (instance->unkByte1 >> 1) - 90;

							break;

						case 0x35:
							if (g_buzzActor.unkShort40)
								goto LABEL_18;

							break;

						case 0x36:
						LABEL_18:
							hitType = 1;
							collisionFlags = instance->collisionFlags;
							goto LABEL_21;

						case 0x37:
							hitType = 2;

						LABEL_20:
							collisionFlags = 0;

						LABEL_21:
							soundId = 0;

						LABEL_22:
							hitInstance = instance;

							break;

						default:
							break;
						}

						if ((g_unk53C650 > 20 || g_unk53C83C <= -120) && hitType == 2)
						{
							hitAngle = Nu3D::Math::CartesianToFixedAngle(
								g_buzzActor.posAngles.pos.x - instance->pos.x,
								g_buzzActor.posAngles.pos.z - instance->pos.z);

							((uint8_t*)&instance->renderFlags)[0] &= 0xF5;
							knockbackIdx = (hitAngle + 3072) & 0xFFF;
							instance->velY = -1024;
							instance->yawAngle = 48;
							instance->lifetime = 50;
							instance->velX = Numerics::g_sinCosLUT[(hitAngle + 2048) & 0xFFF] / 16;
							hitType = 0;
							instance->velZ = Numerics::g_sinCosLUT[knockbackIdx] / 16;
							AudioManager::PlaySoundEffect(7, &instance->pos);
						}
						else
						{
							if (collisionFlags > 0)
							{
								instance->collisionFlags = collisionFlags;
								instance->lifetime = 1;
							}

							if (collisionFlags < 0)
								instance->lifetime = 1;

							if (soundId)
								AudioManager::PlaySoundEffect(soundId, &instance->pos);
						}
					}
				}

				++instance;
				--remaining;
			} while (remaining);

			if (hitType)
			{
				hitAngle = Nu3D::Math::CartesianToFixedAngle(
					g_buzzActor.posAngles.pos.x - hitInstance->pos.x,
					g_buzzActor.posAngles.pos.z - hitInstance->pos.z);

				HandleDamage(hitAngle, 3);
			}

			if (g_unk5281A4 != -1)
				g_unk52B81C = g_unk5281A4;
		}

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

		// FUNCTION: TOY2 0x004343D0
		int32_t UpdateHorizontalMovement(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movementParams, int32_t flag)
		{
			uint16_t yaw = buzzActor->posAngles.angles.yaw;
			int32_t velX = buzzActor->velX;
			int16_t* cosEntry = &Numerics::g_sinCosLUT[(yaw + 1024) & 0xFFF];
			int32_t sinValue = Numerics::g_sinCosLUT[(yaw - 2048) & 0xFFF] >> 2;
			int32_t cosValue = *cosEntry >> 2;
			int32_t sideways = (cosValue * velX + sinValue * buzzActor->velForward) / 4096;
			int32_t forward = (cosValue * buzzActor->velForward - sinValue * velX) / 4096;
			int32_t maxSpeed;
			int32_t maxAirSpeed;
			int32_t result;

			if ((buzzActor->actorFlags & 0x200) != 0)
			{
				if (buzzActor->collisionFlags)
					buzzActor->actorFlags &= 0xFDFF;

				goto LABEL_23;
			}

			// both speeds slow down to 0
			if (sideways < 0)
			{
				sideways += movementParams->groundAccel;

				if (sideways > 0)
					sideways = 0;
			}
			else if (sideways > 0)
			{
				sideways -= movementParams->groundAccel;

				if (sideways < 0)
					sideways = 0;
			}

			if (forward < 0)
			{
				forward += movementParams->airAccel;

				if (forward > 0)
					forward = 0;
			}
			else if (forward > 0)
			{
				forward -= movementParams->airAccel;

				if (forward < 0)
					forward = 0;
			}

			// the stick speeds it up
			if (flag > 0)
			{
				if (forward < movementParams->maxSpeed)
					forward += movementParams->airAccel + movementParams->turnSpeed;
			}

			if (flag < 0 && forward > -movementParams->maxSpeed)
				forward -= movementParams->airAccel + movementParams->turnSpeed;

		LABEL_23:
			maxSpeed = movementParams->maxSpeed;

			if (sideways > maxSpeed)
				sideways = movementParams->maxSpeed;

			if (sideways < -maxSpeed)
				sideways = -maxSpeed;

			maxAirSpeed = movementParams->maxAirSpeed;

			if (forward > maxAirSpeed)
				forward = movementParams->maxAirSpeed;

			if (forward < -maxAirSpeed)
				forward = -maxAirSpeed;

			buzzActor->unkVar30 = forward;
			buzzActor->unkVar31 = sideways;

			sinValue = Numerics::g_sinCosLUT[buzzActor->posAngles.angles.yaw] >> 2;
			cosValue = *cosEntry >> 2;
			buzzActor->velX = (sinValue * forward + cosValue * sideways) / 4096;
			result = (cosValue * forward - sinValue * sideways) / 4096;
			buzzActor->velForward = result;

			return result;
		}

		// FUNCTION: TOY2 0x004346C0
		int32_t UpdateFacing(Toy2::Buzz::Toy2BuzzActor* buzzActor, Toy2::Buzz::MovementParams* movementParams)
		{
			int32_t flag = 0;
			int32_t turnDiff;
			int32_t turnRate;
			int16_t facingAngle;
			uint16_t yawAngle;
			uint32_t angleDiff;
			bool slipping;

			if ((InputManager::g_directionInputState & (DIR_UP | DIR_RIGHT | DIR_DOWN | DIR_LEFT)) != 0)
			{
				if (g_unk53C678 != -1)
				{
					if (!g_unk53C678)
					{
						g_unk53C5D0 = buzzActor->posAngles.angles.yaw;
						goto LABEL_6;
					}

					if (g_unk53C678 < 16)
					{
					LABEL_6:
						g_unk53C678 += Renderer::g_frameDelta;

						if (g_unk53C678 >= 16)
							g_unk53C678 = 16;
					}
				}

				if (!Camera::g_freezeBuzz)
					flag = 1;

				movementParams->maxSpeed = (movementParams->maxSpeed * Camera::CalculateMaxTurnAngle(InputManager::g_directionInputState)) >> 14;
			}
			else
			{
				if (g_unk53C678 > 0 && g_unk53C678 < 16 && buzzActor->unkShort40)
				{
					angleDiff = (g_unk53C5D0 - buzzActor->facingAngle) & 0xFFF;

					if (angleDiff > 2048)
						angleDiff -= 4095;

					if ((Camera::g_inCutscene & 1) == 0)
						buzzActor->facingAngle = (g_unk53C5D0 - (angleDiff * g_unk53C678) / 16) & 0xFFF;

					g_unk53C678 = -1;
				}

				if (g_buzzActor.unkVar30 < 512 && abs(g_buzzActor.unkVar31) < 512 && buzzActor->unkShort40)
					g_unk53C678 = 0;
			}

			facingAngle = buzzActor->facingAngle;
			yawAngle = buzzActor->posAngles.angles.yaw;
			turnDiff = (yawAngle - facingAngle) & 0xFFF;

			if (turnDiff <= 0x800)
			{
				if (((buzzActor->posAngles.angles.yaw - facingAngle) & 0xFFFu) > 0x5DC && !g_unk53C818)
				{
					buzzActor->posAngles.angles.yaw = facingAngle;
					g_unk53C5D0 = facingAngle;
					slipping = (g_buzzActionState & 0xFF481) == 1;

				LABEL_27:
					if (slipping)
					{
						g_buzzSlipTimer = 26;
						buzzActor->unkVar38 = -1;
						AudioManager::PlaySoundEffect(18, &buzzActor->posAngles.pos);
					}

					goto LABEL_38;
				}

				if (turnDiff > movementParams->maxTurnRate)
					turnDiff = movementParams->maxTurnRate;

				buzzActor->posAngles.angles.yaw = yawAngle - turnDiff * (int32_t)Renderer::g_frameDelta / 8;
			}
			else
			{
				turnRate = 4096 - turnDiff;

				if (4096 - turnDiff > 1500 && !g_unk53C818)
				{
					buzzActor->posAngles.angles.yaw = facingAngle;
					g_unk53C5D0 = facingAngle;

					slipping = (g_buzzActionState & 0xFF481) == 1;
					goto LABEL_27;
				}

				if (turnRate > movementParams->maxTurnRate)
					turnRate = movementParams->maxTurnRate;

				buzzActor->posAngles.angles.yaw = yawAngle + turnRate * (int32_t)Renderer::g_frameDelta / 8;
			}

		LABEL_38:
			buzzActor->posAngles.angles.yaw &= 0xFFF;

			if (g_curMoveableObject)
			{
				if (((g_unk53C624 - buzzActor->facingAngle + 1024) & 0xFFFu) > 0x800)
				{
					g_curMoveableObject = 0;

					return flag;
				}

				buzzActor->posAngles.angles.yaw = g_unk53C624;
			}

			return flag;
		}

		// FUNCTION: TOY2 0x004A4960
		void FireDiscLauncher(int16_t pitch)
		{
			int32_t closestDist = 0x7FFFFFFF;
			Toy2::Actor::Toy2Actor* closestActor;
			Toy2::Actor::Toy2Actor* targetActor;
			Toy2::Actor::Toy2Actor* actor;
			Toy2::Actor::Toy2Actor** nextActor;
			Nu3D::Particles::ParticleInstance* disc;
			int32_t deltaX;
			int32_t deltaYSq;
			int32_t distance;
			int32_t cosPitch;
			int32_t sinPitch;
			uint32_t velY;

			if (g_discLauncherAmmo > 0)
			{
				if (Camera::g_cameraState < 3)
				{
					actor = Actor::g_activeActors[0];

					if (actor)
					{
						nextActor = Actor::g_activeActors;

						do
						{
							if ((uint8_t)actor->creatureRam->defenseMode)
							{
								if (actor->unkVar29 >= 0 && (actor->actorFlags & (Actor::ACTOR_CUTSCENE_MOVE | Actor::ACTOR_VISIBLE_ACTIVE)) == 3)
								{
									deltaX = (actor->pos.x - g_buzzActor.posAngles.pos.x) >> 5;
									deltaYSq = ((actor->pos.y - g_buzzActor.posAngles.pos.y) >> 5) * ((actor->pos.y - g_buzzActor.posAngles.pos.y) >> 5);
									distance = deltaX * deltaX + deltaYSq + ((actor->pos.z - g_buzzActor.posAngles.pos.z) >> 5) * ((actor->pos.z - g_buzzActor.posAngles.pos.z) >> 5);

									if (closestDist > distance)
									{
										closestDist = distance;
										closestActor = actor;
									}
								}
							}

							actor = *++nextActor;
						} while (*nextActor);

						if (closestDist < 0x1000000)
						{
							targetActor = closestActor;
							goto LABEL_16;
						}
					}
				}
				else if (Camera::g_lockOnActorIdx != -1 && Camera::g_lockOnActorIdx < 1000)
				{
					targetActor = &g_levelActors[Camera::g_lockOnActorIdx];

				LABEL_16:
					disc = Nu3D::Particles::SpawnInstance(
						g_unk50A0A0.x,
						g_unk50A0A0.y,
						g_unk50A0A0.z,
						0,
						-2,
						0,
						4 * g_buzzActor.posAngles.angles.yaw,
						0,
						0,
						71);

					// the target actor is stored in the velY field
					disc->velY = (int32_t)targetActor;
					disc->pitchAngle = pitch & 0xFFF;

				LABEL_18:
					AudioManager::PlaySoundEffect(84, &g_buzzActor.posAngles.pos);
					--g_discLauncherAmmo;
					--g_discLauncherTimer;

					return;
				}

				cosPitch = Numerics::g_sinCosLUT[(pitch + 1024) & 0xFFF];
				sinPitch = Numerics::g_sinCosLUT[pitch & 0xFFF];

				// signed division by -3
				velY = (((1431655765i64 * sinPitch) >> 32) - sinPitch) >> 1;

				Nu3D::Particles::SpawnInstance(
					g_unk50A0A0.x,
					g_unk50A0A0.y,
					g_unk50A0A0.z,
					((cosPitch * Numerics::g_sinCosLUT[g_buzzActor.posAngles.angles.yaw & 0xFFF]) >> 14) / 3,
					(velY >> 31) + velY,
					((cosPitch * Numerics::g_sinCosLUT[(g_buzzActor.posAngles.angles.yaw + 1024) & 0xFFF]) >> 14) / 3,
					0,
					0,
					0,
					72);

				goto LABEL_18;
			}
		}

		// FUNCTION: TOY2 0x004A5370
		uint32_t FireGrapple()
		{
			int32_t lockOnIdx;
			int32_t targetX;
			int32_t targetY;
			int32_t targetZ;
			int32_t dominantVel;
			int32_t dominantDist;
			uint32_t result;
			WorldPos direction;

			AudioManager::PlaySoundEffect(81, &g_buzzActor.posAngles.pos);

			lockOnIdx = Camera::g_lockOnActorIdx - 1000;
			targetX = Levels::g_recordData[59]->data[lockOnIdx].x;
			targetZ = Levels::g_recordData[59]->data[lockOnIdx].z;
			targetY = Levels::g_recordData[59]->data[lockOnIdx].y;

			g_grappleTargetPos.x = g_buzzActor.posAngles.pos.x;
			direction.y = 32 * (targetY - 192) - (g_buzzActor.posAngles.pos.y - 11264);
			g_grappleTargetPos.y = g_buzzActor.posAngles.pos.y - 11264;
			g_grappleTargetPos.z = g_buzzActor.posAngles.pos.z;

			direction.x = 32 * targetX - g_buzzActor.posAngles.pos.x;
			direction.z = 32 * targetZ - g_buzzActor.posAngles.pos.z;

			g_grappleState = Nu3D::Collision::SweepAndSlide(&g_grappleTargetPos, &direction, 0x8000, 0, 256) != 0 ? 3 : 1;

			g_grappleTargetPos.x += direction.x;
			direction.x = (g_grappleTargetPos.x - g_unk50A0A0.x) >> 2;
			g_grappleTargetPos.y += direction.y;
			g_grappleTargetPos.z += direction.z;

			direction.y = (g_grappleTargetPos.y - g_unk50A0A0.y) >> 2;
			direction.z = (g_grappleTargetPos.z - g_unk50A0A0.z) >> 2;

			Nu3D::Math::NormalizeToFixedPoint((Vector3I*)&direction, (Vector3I*)&direction);

			dominantVel = direction.x >> 1;

			if (abs(direction.x >> 1) <= abs(direction.y >> 1))
			{
				dominantVel = direction.y >> 1;
				dominantDist = g_grappleTargetPos.y - g_buzzActor.posAngles.pos.y + 5632;
			}
			else
			{
				dominantDist = g_grappleTargetPos.x - g_buzzActor.posAngles.pos.x;
			}

			if (abs(dominantVel) <= abs(direction.z >> 1))
			{
				dominantVel = direction.z >> 1;
				dominantDist = g_grappleTargetPos.z - g_buzzActor.posAngles.pos.z;
			}

			if (!dominantVel)
				dominantVel = 1;

			g_buzzActor.actorFlags |= 4;
			g_grappleProgress = 0;

			result = abs(dominantDist / dominantVel);
			InputManager::g_directionInputState &= 0x309;
			g_grappleDistance = result;

			return result;
		}

		// FUNCTION: TOY2 0x00434990
		void TickGunFire(PosAndAngles* muzzlePos)
		{
			int32_t chargeTimer;
			int32_t holdTimer;
			int32_t shotKind;
			int32_t newChargeTimer;
			int32_t newHoldTimer;
			int16_t yaw;
			PosAndAngles* shotPos;
			int32_t pitch;
			int32_t autoAim;
			BeamShot* beamShot;
			int16_t followUpYaw;
			Vector3I* beamOrigin;
			int16_t followUpPitch;
			int32_t shotType;
			WorldPos headOffset;
			WorldPos origin;

			if ((g_buzzActionState & 0xFBE3C) != 0)
			{
				chargeTimer = 0;
				holdTimer = 0;
				g_fireChargeTimer = 0;
				g_fireHoldTimer = 0;
			}
			else
			{
				chargeTimer = g_fireChargeTimer;

				if (g_fireChargeTimer)
				{
					holdTimer = g_fireHoldTimer;
				}
				else
				{
					holdTimer = 0;
					g_fireHoldTimer = 0;
				}
			}

			shotKind = 0;
			shotType = 0;

			if (InputManager::g_directionInputState < 0
				&& (InputManager::g_prevDirectionInputState & 0x8000) == 0
				&& (g_buzzActionState & 0xFBEFE) == 0)
			{
				g_fireHoldTimer = 0;
				g_unk53C83C = 0;
				g_unk53C650 = 0;
				g_buzzActionState |= 0x80u;

				goto LABEL_13;
			}

			if (!chargeTimer)
				return;

			if (chargeTimer < 12)
			{
				if (chargeTimer != -1)
				{
				LABEL_14:
					g_fireChargeTimer = Renderer::g_frameDelta + chargeTimer;

					if (Renderer::g_frameDelta + chargeTimer < 12)
						return;

					shotType = 1;

					goto LABEL_49;
				}

			LABEL_13:
				chargeTimer = 0;

				goto LABEL_14;
			}

			newChargeTimer = Renderer::g_frameDelta + chargeTimer;
			g_fireChargeTimer = newChargeTimer;

			if (InputManager::g_directionInputState < 0)
			{
				newHoldTimer = Renderer::g_frameDelta + holdTimer;
				g_fireHoldTimer = newHoldTimer;

				if (g_discLauncherTimer)
				{
					if (newHoldTimer < 14)
						goto LABEL_23;

				LABEL_22:
					shotKind = 1;
					g_fireHoldTimer = 0;
					shotType = 1;

				LABEL_23:
					if (newChargeTimer >= 52)
					{
						newChargeTimer -= 40;
						g_fireChargeTimer = newChargeTimer;
					}

					goto LABEL_45;
				}

				if (g_greenGunCharge)
				{
					if (newHoldTimer < 6)
						goto LABEL_23;

					goto LABEL_22;
				}

				if (newHoldTimer >= 64)
				{
					newHoldTimer = 64;
					g_fireHoldTimer = 64;
				}

				if (InputManager::g_prevDirectionInputState < 0 || (g_buzzActionState & 0xFBE7E) != 0)
				{
					if (newChargeTimer < 52)
						goto LABEL_35;

					newChargeTimer -= 40;
				}
				else
				{
					if (newChargeTimer <= 52)
					{
						shotKind = 1;
						shotType = 1;

						goto LABEL_35;
					}

					newChargeTimer = 64 - newChargeTimer;
				}

				g_fireChargeTimer = newChargeTimer;

			LABEL_35:
				if (newHoldTimer > 12)
				{
					AudioManager::g_pitchArray[3] = 80 * newHoldTimer + 2048;
					AudioManager::PlaySoundEffect(39, &muzzlePos->pos);
					newChargeTimer = g_fireChargeTimer;
				}

				goto LABEL_45;
			}

			if (holdTimer > 36)
			{
				newChargeTimer = 52;
				g_fireChargeTimer = 52;
			}

			if (holdTimer == 64)
			{
				shotKind = 2;
				shotType = 2;
			}

			g_fireHoldTimer = 0;

			if (g_greenGunCharge)
				g_fireHoldTimer = 6;

			if (g_discLauncherTimer)
				g_fireHoldTimer = 14;

		LABEL_45:
			if (newChargeTimer >= 64)
				g_fireChargeTimer = 0;

			if (shotKind)
			{
				if (shotKind != 1)
				{
				LABEL_52:
					g_fireChargeTimer = 12;

					if (Camera::g_cameraState == 4)
					{
						yaw = Camera::g_gameplayCameraState.angles.yaw;
						shotPos = muzzlePos;
						pitch = -Camera::g_gameplayCameraState.angles.pitch;
						autoAim = 0;
					}
					else
					{
						shotPos = muzzlePos;
						pitch = 0;
						autoAim = 1;
						yaw = muzzlePos->angles.yaw;
					}

					origin.x = shotPos->pos.x;
					origin.y = shotPos->pos.y - 11264;
					origin.z = shotPos->pos.z;
					headOffset.x = g_unk50A0A0.x - shotPos->pos.x;
					headOffset.z = g_unk50A0A0.z - shotPos->pos.z;
					headOffset.y = g_unk50A0A0.y - (shotPos->pos.y - 11264);

					if (g_grappleTimer && Camera::g_cameraState >= 3 && Camera::g_unk50A540 && Camera::g_lockOnActorIdx >= 1000)
					{
						FireGrapple();
					}
					else if (g_discLauncherTimer)
					{
						FireDiscLauncher(pitch);
					}
					else
					{
						if (shotType == 1)
							AudioManager::PlaySoundEffect(2, &g_buzzActor.posAngles.pos);
						else
							AudioManager::PlaySoundEffect(6, &g_buzzActor.posAngles.pos);

						// quirk: the auto aim parameter is a flag here (0 or 1), not an actor
						beamShot = SpawnBeamShot(shotType - 1, yaw, pitch, &origin, &headOffset, 4, (Toy2::Actor::Toy2Actor*)autoAim);

						if (beamShot)
						{
							memset(&headOffset, 0, 12);

							followUpYaw = yaw + 4 * *g_randDatBufferPtr + 1536;
							beamOrigin = &beamShot->originPos;
							followUpPitch = (4 * *g_randDatBufferPtr - 512) & 0xFFF;
							g_randDatBufferPtr += 2;

							SpawnBeamShot(shotType - 1, followUpYaw & 0xFFF, followUpPitch, (WorldPos*)&beamShot->originPos, &headOffset, 4, 0);
							AudioManager::PlaySoundEffect(7, beamOrigin);
						}
					}

					return;
				}

			LABEL_49:
				if (g_greenGunCharge)
				{
					shotType = 3;
					g_greenGunCharge -= 10;

					if (g_greenGunCharge < 0)
						g_greenGunCharge = 0;
				}

				goto LABEL_52;
			}
		}

		// FUNCTION: TOY2 0x00434D20
		int32_t TickGroundSlam(Toy2::Actor::Toy2Actor* actor)
		{
			int32_t ticker;

			if ((InputManager::g_directionInputState & DIR_CAM_FLAG3) != 0
				&& (InputManager::g_prevDirectionInputState & 0x2000) == 0
				&& (g_buzzActionState & 0xFFF7F) == 0
				&& !g_unk53C83C
				&& !g_unk53C650)
			{
				if (actor->unkWord15 == 1 || actor->unkWord15 == 2 || (int16_t)actor->lastValidYPosition == 8)
				{
					g_groundSlamTicker = 1;
					AudioManager::PlaySoundEffect(23, &actor->pos);
				}
			}

			if (!g_groundSlamTicker)
				return 0;

			g_buzzActionState |= 0x40u;

			if (g_groundSlamTicker <= 0)
			{
				ticker = Renderer::g_frameDelta + g_groundSlamTicker;
				g_groundSlamTicker = ticker;

				if (ticker <= 0)
				{
					if (ticker >= -14)
						return 0;

					if (!actor->unkWord16)
						actor->areaIndex += ((int32_t)Renderer::g_frameDelta << 8) / 4;

					if (actor->areaIndex > 2048)
						actor->areaIndex = 2048;

					return 5;
				}

				g_groundSlamTicker = 0;

				return 0;
			}

			if (actor->unkWord16)
			{
				g_groundSlamTicker = -40;
				Camera::g_unk50A510 = 40;
				Nu3D::Particles::SpawnFromPreset(actor->pos.x, actor->pos.y - 1024, actor->pos.z, 18, 11);
				Nu3D::Particles::SpawnFromPreset(actor->pos.x, actor->pos.y - 1024, actor->pos.z, 19, 12);
				AudioManager::PlaySoundEffect(15, &g_buzzActor.posAngles.pos);

				return 0;
			}

			g_groundSlamTicker += Renderer::g_frameDelta;

			if (g_groundSlamTicker <= 14)
				return 7;

			actor->areaIndex = 2048;
			g_groundSlamTicker = 14;

			return 5;
		}

		// FUNCTION: TOY2 0x00434EB0
		void TickSpinHover(Toy2::Buzz::Toy2BuzzActor* buzz)
		{
			int32_t v1 = g_unk53C650;
			int32_t v3;
			int32_t v5;
			int32_t v6;
			int32_t v7;

			if (g_unk53C650 > 0)
			{
				v1 = g_unk53C650 - Renderer::g_frameDelta;
				g_unk53C650 = v1;

				if (v1 < 0)
				{
					v1 = 0;
					g_unk53C650 = 0;
				}

				if (g_tickState[2] && v1 > 20)
				{
					int32_t v2 = *g_randDatBufferPtr++ & 1;

					Nu3D::Particles::SpawnFromPreset(
						buzz->posAngles.pos.x,
						buzz->posAngles.pos.y,
						buzz->posAngles.pos.z,
						v2 + 22,
						2)->groundAlignRot = (-g_unk53C650 & 0xF) << 7;
					v1 = g_unk53C650;
				}
			}

			if ((InputManager::g_directionInputState & 0x2000) == 0 || (InputManager::g_prevDirectionInputState & 0x2000) != 0)
			{
				v3 = g_unk53C83C;
			}
			else
			{
				v3 = g_unk53C83C;

				if ((g_buzzActionState & 0xFFF7F) == 1 && !g_unk53C83C && !v1 && !g_buzzSlipTimer)
				{
					g_unk53C650 = 48;

					if (g_fireChargeTimer)
					{
						g_fireChargeTimer = 0;
						g_buzzActionState &= 0xFF7Fu;
					}

					g_unk53C83C = 1;
					AudioManager::PlaySoundEffect(17, &buzz->posAngles.pos);
				}

				v3 = g_unk53C83C;
			}

			if ((InputManager::g_directionInputState & 0x2000) != 0 && (g_buzzActionState & 0xFFFFE) == 0 && v3 > 0)
			{
				v5 = Renderer::g_frameDelta + v3;
				g_unk53C83C = v5;

				if (v5 <= 60)
				{
					if (v5 <= 12)
						return;
				}
				else
				{
					v5 = 60;
					g_unk53C83C = 60;
				}

				AudioManager::g_pitchArray[3] = 80 * v5 + 2048;
				AudioManager::PlaySoundEffect(39, &buzz->posAngles.pos);

				return;
			}

			if (v3 < 0)
			{
				g_unk53C83C = Renderer::g_frameDelta + v3;

				if ((int32_t)(Renderer::g_frameDelta + v3) <= -120)
				{
					AudioManager::PlaySoundEffect(38, &buzz->posAngles.pos);

					if (g_tickState[2])
					{
						v7 = (*g_randDatBufferPtr++ & 1) + 20;
						Nu3D::Particles::SpawnFromPreset(
							buzz->posAngles.pos.x,
							buzz->posAngles.pos.y - 12288,
							buzz->posAngles.pos.z,
							v7,
							2)->groundAlignRot = 192 * (-g_unk53C83C & 0xF);
					}
				}
				else
				{
					if (v3 <= -120)
						AudioManager::PlaySoundEffect(24, &buzz->posAngles.pos);

					buzz->actorFlags |= 4;
				}

				if (g_unk53C608 || g_unk53C83C >= 0)
				{
					g_unk53C83C = 0;
					buzz->actorFlags &= ~4;
				}
			}
			else if (v3 < 60 || (v6 = g_buzzActionState, (g_buzzActionState & 0xFFF7E) != 0))
			{
				g_unk53C83C = 0;
			}
			else
			{
				g_buzzActionState = v6 | 2;
				g_unk53C83C = -300;
			}
		}

		// FUNCTION: TOY2 0x00435100
		int32_t HandleSwing(Toy2::Buzz::Toy2BuzzActor* buzz)
		{
			int32_t result = 0;
			int32_t v2;
			int32_t v16;
			Vector3I* data;
			Vector3I dir;
			int32_t v9;
			int32_t v24;
			int32_t v23;
			int32_t records;
			int32_t i;

			dir.y = 0;

			if (!Levels::g_recordData[60])
				return result;

			if ((g_buzzActionState & 0xFFB7E) != 0 || g_unk53C834)
			{
				g_unk53C66C = 0;
				g_buzzActionState &= ~0x400;

				return result;
			}

			v2 = g_unk53C66C;

			if (!g_unk53C66C)
			{
				data = Levels::g_recordData[60]->data;
				i = 0;
				records = Levels::g_recordData[60]->recordCount;

				if ((records & 0xFFFE) != 0)
				{
					do
					{
						int32_t heightDiff = buzz->posAngles.pos.y - data->y;

						if (heightDiff > 0 && heightDiff < 0x4000)
						{
							v24 = (data[1].x - data->x) >> 5;
							dir.x = v24;
							v23 = (data[1].z - data->z) >> 5;
							dir.z = v23;
							Nu3D::Math::NormalizeToFixedPoint(&dir, &dir);

							v9 = (dir.x * ((buzz->posAngles.pos.x - data->x) >> 5) + dir.z * ((buzz->posAngles.pos.z - data->z) >> 5)) >> 12;

							if (v9 > 0 && v9 <= (v24 * dir.x + v23 * dir.z) >> 12)
							{
								int32_t offX = ((buzz->posAngles.pos.x - data->x) >> 5) - ((dir.x * v9) >> 12);
								int32_t offZ = ((buzz->posAngles.pos.z - data->z) >> 5) - ((dir.z * v9) >> 12);

								if (offX * offX + offZ * offZ < 4096)
								{
									AudioManager::PlaySoundEffect(27, &buzz->posAngles.pos);
									g_unk53C66C = 69;
									buzz->unkVar37 = 26;
									buzz->unkVar38 = -1;
									g_unk53C5F0 = data->x + ((dir.x * v9) >> 7);
									g_unk53C5F4 = data->y + 0x4000;
									g_unk53C5F8 = data->z + ((dir.z * v9) >> 7);
									g_unk53C654 = (Nu3D::Math::CartesianToFixedAngle(dir.x, dir.z) + 1024) & 0xFFF;

									if (((g_unk53C654 - buzz->posAngles.angles.yaw + 1024) & 0xFFFu) > 0x800)
										g_unk53C654 = (g_unk53C654 - 2048) & 0xFFF;

									g_buzzActionState |= 0x400;
								}
							}
						}

						data += 2;
						++i;
					}
					while (i < (Levels::g_recordData[60]->recordCount >> 1));
				}

				v2 = g_unk53C66C;
				result = 0;

				if (!g_unk53C66C)
					return result;
			}

			v16 = v2 - Renderer::g_frameDelta;
			g_unk53C66C = v16;

			if ((int32_t)Renderer::g_frameDelta + v16 > 15 && v16 <= 15)
			{
				g_unk53C61C = 0;
				buzz->gravityVel = -1536;
				buzz->velX = Numerics::g_sinCosLUT[buzz->posAngles.angles.yaw] >> 1;
				buzz->velForward = Numerics::g_sinCosLUT[(buzz->posAngles.angles.yaw + 1024) & 0xFFF] >> 1;

				if ((InputManager::g_directionInputState & 0x4000) == 0)
					buzz->unkWord15 = 2;

				AudioManager::Preset::PlayOneShotSound2(25, (Toy2::Actor::Toy2Actor*)buzz);
				result = 1;
			}

			if (g_unk53C66C >= 0)
			{
				if (g_unk53C66C > 15)
				{
					buzz->posAngles.pos.x -= (int32_t)Renderer::g_frameDelta * (buzz->posAngles.pos.x - g_unk53C5F0) / 16;
					buzz->posAngles.pos.y -= (int32_t)Renderer::g_frameDelta * (buzz->posAngles.pos.y - g_unk53C5F4) / 16;
					v16 = (int32_t)Renderer::g_frameDelta * (buzz->posAngles.pos.z - g_unk53C5F8);
					buzz->velX = 0;
					buzz->gravityVel = 0;
					buzz->velForward = 0;
					buzz->posAngles.pos.z -= v16 / 16;

					int32_t turn = (buzz->posAngles.angles.yaw - g_unk53C654) & 0xFFF;

					if (turn >= 0x800)
						turn -= 4096;

					buzz->posAngles.angles.yaw -= (turn * (int32_t)Renderer::g_frameDelta) / 16;
					Camera::g_gameplayCameraState.orbitYaw = (Camera::g_gameplayCameraState.orbitYaw - (turn * (int32_t)Renderer::g_frameDelta) / 16) & 0xFFF;
					buzz->posAngles.angles.yaw &= 0xFFFu;
					buzz->facingAngle = buzz->posAngles.angles.yaw;

					return 2;
				}
			}
			else
			{
				if (buzz->unkVar37 == 26)
					buzz->unkVar37 = 3;

				g_unk53C66C = 0;
			}

			return result;
		}

		// FUNCTION: TOY2 0x004354E0
		int32_t HandlePoleClimb(Toy2::Buzz::Toy2BuzzActor* buzz)
		{
			int32_t* data;
			int32_t v2 = 0;
			int32_t v3;
			int32_t v4;
			int32_t v11;
			int32_t v24;
			int32_t* pole;

			if (!Levels::g_recordData[61])
				return 0;

			if (g_unk53C834)
				return 0;

			data = (int32_t*)Levels::g_recordData[61]->data;

			if ((g_buzzActionState & 0xFFF6E) != 0)
				return 0;

			if (!g_unk53C668)
			{
				v3 = 0;
				v4 = Levels::g_recordData[61]->recordCount >> 1;

				if (!v4)
					return 0;

				for (pole = data; ; pole += 6)
				{
					if (pole[3] != 3)
					{
						int32_t dz = (buzz->posAngles.pos.z - pole[2]) >> 8;
						int32_t dx = (buzz->posAngles.pos.x - pole[0]) >> 8;

						if (dx * dx + dz * dz < 512)
						{
							int32_t dy = pole[1] - buzz->posAngles.pos.y;

							if (dy >= -7680 && dy <= pole[4] - 13824)
								break;
						}
					}

					if (++v3 >= v4)
						return 0;
				}

				g_unk53C668 = 8;
				g_unk53C658 = 10;
				g_unk53C5E4 = 6 * v3;
				buzz->posAngles.pos.x -= (buzz->posAngles.pos.x - data[g_unk53C5E4]) >> 2;
				buzz->posAngles.pos.z -= (buzz->posAngles.pos.z - data[g_unk53C5E4 + 2]) >> 2;
				buzz->gravityVel = 0;
				v11 = g_unk53C5E4;
				v2 = 0;
			}
			else if (g_unk53C668 <= 0)
			{
				v11 = g_unk53C5E4;

				int32_t dz = (buzz->posAngles.pos.z - data[g_unk53C5E4 + 2]) >> 5;
				int32_t dx = (buzz->posAngles.pos.x - data[g_unk53C5E4]) >> 5;

				if (dx * dx + dz * dz > 33792)
				{
					g_unk53C668 = 0;
					g_unk53C5E4 = 0;

					return 0;
				}
			}
			else
			{
				v11 = g_unk53C5E4;

				int32_t dy = data[g_unk53C5E4 + 1] - buzz->posAngles.pos.y;

				if (dy < -7680)
				{
					g_unk53C668 = -1;
					buzz->unkWord15 = 5;
					buzz->unkVar37 = 3;
					g_unk53C61C = 0;

					return 1;
				}

				if (dy >= data[g_unk53C5E4 + 4] - 13824)
				{
					buzz->gravityVel = 0;
					v2 = 2;
					buzz->posAngles.pos.y = (data[g_unk53C5E4 + 1] - data[g_unk53C5E4 + 4]) + 13824;
					v11 = g_unk53C5E4;
				}
			}

			if (g_unk53C668 <= 0)
				return 0;

			buzz->posAngles.pos.x -= (buzz->posAngles.pos.x - data[v11]) >> 2;
			buzz->velX = 0;
			buzz->velForward = 0;
			buzz->posAngles.pos.z -= (buzz->posAngles.pos.z - data[g_unk53C5E4 + 2]) >> 2;

			if (data[g_unk53C5E4 + 3] == 2)
			{
				g_unk53C668 = 1;

				if (buzz->unkShort40)
				{
					g_unk53C668 = -1;

					return 1;
				}

				buzz->gravityVel = (((int32_t)Renderer::g_frameDelta << 6) / 4) + buzz->gravityVel;

				if (buzz->gravityVel > 2048)
					buzz->gravityVel = 2048;

				g_unk53C668 |= 4;
			}
			else if (g_unk53C668 != 8 || (InputManager::g_directionInputState & 0x40) == 0)
			{
				g_unk53C668 = 1;

				if ((InputManager::g_directionInputState & 0x10) != 0)
				{
					buzz->collisionFlags = 0;
					buzz->unkShort40 = 0;

					if (v2 != 2)
					{
						buzz->gravityVel = -256;
						g_unk53C668 |= 2u;
					}
				}
				else if ((InputManager::g_directionInputState & 0x40) != 0)
				{
					if (buzz->unkShort40)
					{
						g_unk53C668 = -1;

						return 1;
					}

					buzz->gravityVel = (((int32_t)Renderer::g_frameDelta << 7) / 4) + buzz->gravityVel;

					if (buzz->gravityVel > 1024)
						buzz->gravityVel = 1024;

					buzz->posAngles.angles.yaw += (int32_t)Renderer::g_frameDelta * buzz->gravityVel / 16;
					g_unk53C668 |= 4;
				}
				else
				{
					if (buzz->gravityVel > 0)
						buzz->gravityVel = buzz->gravityVel - (((int32_t)Renderer::g_frameDelta << 8) / 4);

					if (buzz->gravityVel < 0)
						buzz->gravityVel = 0;
				}
			}

			if ((InputManager::g_directionInputState & DIR_LEFT) != 0)
				buzz->posAngles.angles.yaw += -64 * (int32_t)Renderer::g_frameDelta / 2;

			if ((InputManager::g_directionInputState & DIR_RIGHT) != 0)
				buzz->posAngles.angles.yaw += ((int32_t)Renderer::g_frameDelta << 6) / 2;

			buzz->posAngles.angles.yaw &= 0xFFFu;
			buzz->facingAngle = buzz->posAngles.angles.yaw;

			if (v2 != 2 || (v24 = 1, data[g_unk53C5E4 + 3] != 1))
				v24 = 0;

			if ((InputManager::g_directionInputState & 0x4000) == 0 || (InputManager::g_prevDirectionInputState & 0x4000) != 0)
			{
				if (!v24)
					return 2;
			}
			else if (!v24)
			{
				if (g_unk53C658)
					return 2;

				buzz->gravityVel = -1024;

				if ((InputManager::g_directionInputState & (DIR_UP | DIR_RIGHT | DIR_DOWN | DIR_LEFT)) != 0)
				{
					Camera::CalculateMaxTurnAngle((uint16_t)InputManager::g_directionInputState);
					buzz->posAngles.angles.yaw = buzz->facingAngle;
				}

				buzz->velX = Numerics::g_sinCosLUT[buzz->posAngles.angles.yaw & 0xFFF] >> 1;
				buzz->velForward = Numerics::g_sinCosLUT[(buzz->posAngles.angles.yaw + 1024) & 0xFFF] >> 1;
				buzz->unkWord15 = 1;
				g_unk53C61C = 0;
				AudioManager::Preset::PlayOneShotSound2(25, (Toy2::Actor::Toy2Actor*)buzz);

				g_unk53C668 = -1;
				buzz->collisionFlags = 0;
				buzz->unkShort40 = 0;
				buzz->unkVar37 = 2;
				buzz->facingAngle = buzz->posAngles.angles.yaw;

				return 1;
			}

			buzz->actorFlags |= 0x200;
			buzz->gravityVel = -1536;
			buzz->velX = 0;
			buzz->velForward = 0;
			buzz->unkWord15 = 2;
			AudioManager::Preset::PlayOneShotSound2(25, (Toy2::Actor::Toy2Actor*)buzz);
			g_unk53C61C = 0;

			g_unk53C668 = -1;
			buzz->collisionFlags = 0;
			buzz->unkShort40 = 0;
			buzz->unkVar37 = 2;
			buzz->facingAngle = buzz->posAngles.angles.yaw;

			return 1;
		}

		// FUNCTION: TOY2 0x004359D0
		int32_t HandleZipline(Toy2::Buzz::Toy2BuzzActor* actor)
		{
			Vector3I* data;
			int32_t v1;
			int32_t i;
			int32_t v4;
			int32_t v6;
			int32_t v9;
			int32_t v10;
			int32_t v11;
			int32_t v12;
			int32_t v13;
			int32_t v14;
			int32_t v21;
			int32_t v26;
			int32_t v29;
			int32_t v31;
			int32_t v32;
			int32_t v34;
			Vector3I* segment;

			if (!Levels::g_recordData[62] || g_unk53C834)
				return 0;

			v1 = g_unk53C600;
			data = Levels::g_recordData[62]->data;

			if (g_unk53C600 > 0)
			{
				v1 = g_unk53C600 - Renderer::g_frameDelta;
				g_unk53C600 = v1;

				if (v1 < 0)
				{
					v1 = 0;
					g_unk53C600 = 0;
				}
			}

			if ((g_buzzActionState & 0xFFF7F) == 0 && !v1)
			{
				i = 0;

				if ((Levels::g_recordData[62]->recordCount & 0xFFFE) != 0)
				{
					do
					{
						v4 = data[1].x - data->x;

						if (v4 < 0)
						{
							if ((uint32_t)(actor->posAngles.pos.x - data[1].x + 2048) > (uint32_t)(4096 - v4))
								goto next;
						}
						else
						{
							if ((uint32_t)(actor->posAngles.pos.x - data->x + 2048) > (uint32_t)(v4 + 4096))
								goto next;
						}

						v6 = data[1].z - data->z;

						if (v6 < 0)
						{
							if ((uint32_t)(actor->posAngles.pos.z - data[1].z + 2048) > (uint32_t)(4096 - v6))
								goto next;
						}
						else
						{
							if ((uint32_t)(actor->posAngles.pos.z - data->z + 2048) > (uint32_t)(v6 + 4096))
								goto next;
						}

						v9 = data[1].y - data->y;

						if (v9 < 0)
						{
							if ((uint32_t)(actor->posAngles.pos.y - data[1].y + 4096) > (uint32_t)(19968 - v9))
								goto next;
						}
						else
						{
							if ((uint32_t)(actor->posAngles.pos.y - data->y + 4096) > (uint32_t)(v9 + 19968))
								goto next;
						}

						v26 = v4 >> 5;
						v31 = (actor->posAngles.pos.z - data->z) >> 5;
						v10 = (actor->posAngles.pos.x - data->x) >> 5;
						v11 = v9 >> 5;
						v12 = v6 >> 5;

						if (abs(v10) >= abs(v31))
						{
							v13 = v4 >> 5;
						}
						else
						{
							v10 = v31;
							v13 = v12;
						}

						v32 = v26 * v10 / v13;
						v29 = v11 * v10 / v13;
						v14 = v12 * v10 / v13;
						v34 = v14;
						v21 = v14 + ((data->z - actor->posAngles.pos.z) >> 5);

						if (v21 * v21 + (v32 + ((data->x - actor->posAngles.pos.x) >> 5)) * (v32 + ((data->x - actor->posAngles.pos.x) >> 5)) < 0x2000)
						{
							int32_t v16 = data->y + 32 * (v29 + 496) - actor->posAngles.pos.y;

							if (v16 > -4096 && v16 < 15872)
							{
								g_unk53C618 = 1;
								g_unk53C610 = 2 * i;
								g_unk53C604 = data->y + 32 * v29;
								g_ziplineDirection.x = v4 >> 5;
								g_ziplineDirection.y = v11;
								g_ziplineDirection.z = v12;
								Nu3D::Math::NormalizeToFixedPoint(&g_ziplineDirection, &g_ziplineDirection);
								g_unk53C5D8 = (int32_t)sqrt((double)(v26 * v26 + v12 * v12 + v11 * v11));
								g_unk53C614 = (int32_t)sqrt((double)(v32 * v32 + v29 * v29 + v34 * v34));

								if (g_unk53C5D8 - g_unk53C614 >= 200)
									break;

								g_unk53C618 = 0;
							}
						}

next:
						data += 2;
						++i;
					}
					while (i < (Levels::g_recordData[62]->recordCount >> 1));
				}
			}

			if (g_unk53C618 != 1)
			{
				if (g_unk53C618 != 2)
					return 0;
			}
			else
			{
				if (g_unk53C604 - actor->posAngles.pos.y + 15872 >= 0)
				{
					actor->velX = 0;
					actor->velForward = 0;

					if (g_unk53C618 != 2)
						return 0;
				}
				else
				{
					g_unk53C618 = 2;
					g_unk53C604 = 0;
					g_unk53C658 = 10;
				}
			}

			AudioManager::PlaySoundEffect(35, &actor->posAngles.pos);
			segment = &Levels::g_recordData[62]->data[g_unk53C610];
			g_unk53C604 += Renderer::g_frameDelta;

			if (g_unk53C604 > 48)
				g_unk53C604 = 48;

			actor->posAngles.pos.x = segment->x + ((g_ziplineDirection.x * g_unk53C614) >> 7);
			actor->posAngles.pos.y = ((g_ziplineDirection.y * g_unk53C614) >> 7) + segment->y + 15872;
			v1 = g_ziplineDirection.z * g_unk53C614;
			v21 = segment->z;
			actor->velX = 0;
			actor->gravityVel = 0;
			actor->posAngles.pos.z = v21 + (v1 >> 7);
			actor->velForward = 0;

			v21 = (actor->posAngles.angles.yaw - Nu3D::Math::CartesianToFixedAngle(g_ziplineDirection.x, g_ziplineDirection.z)) & 0xFFF;

			if (v21 >= 2048)
				v21 -= 4096;

			actor->posAngles.angles.yaw = (actor->posAngles.angles.yaw - (int32_t)Renderer::g_frameDelta * (v21 >> 3) / 2) & 0xFFF;
			actor->facingAngle = actor->posAngles.angles.yaw;

			g_unk53C614 += Renderer::g_frameDelta * g_unk53C604;

			if (g_unk53C614 < g_unk53C5D8
				&& ((InputManager::g_directionInputState & 0x4000) == 0
					|| (InputManager::g_prevDirectionInputState & 0x4000) != 0
					|| g_unk53C658))
			{
				return 2;
			}

			g_unk53C618 = 0;
			g_unk53C600 = 30;
			actor->velX = g_unk53C604 * (Numerics::g_sinCosLUT[actor->posAngles.angles.yaw & 0xFFF] >> 5);
			actor->velForward = g_unk53C604 * (Numerics::g_sinCosLUT[(actor->posAngles.angles.yaw + 1024) & 0xFFF] >> 5);
			actor->gravityVel = -1472;
			actor->unkWord15 = 1;
			g_unk53C61C = 0;
			actor->collisionFlags = 0;
			actor->unkShort40 = 0;
			actor->unkVar37 = 3;

			if ((InputManager::g_directionInputState & 0x4000) != 0)
				AudioManager::Preset::PlayOneShotSound2(25, (Toy2::Actor::Toy2Actor*)actor);

			return 1;
		}

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

		// FUNCTION: TOY2 0x004A4E60
		void DeactivateRocketBoots()
		{
			int32_t i;

			if (g_rocketBootsDuration)
			{
				g_rocketBootsDuration = 0;

				for (i = 0; i < 64; ++i)
				{
					if (Nu3D::Particles::g_particleInstances[i].typeId == 47)
						Nu3D::Particles::g_particleInstances[i].lifetime = 1;
				}

				Nu3D::Link::SetScaleFromFixedOffsets(g_rocketBootsPickup->props.linkId, 4096, 4096, 4096);
				g_rocketBootsPickup->pos.y = g_rocketBootsPickupY;
			}
		}

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

		// FUNCTION: TOY2 0x004A28F0
		void UpdateRespawnAnchor()
		{
			g_buzzActor.actorFlags &= ~8;

			if (g_buzzActor.collisionFlags && !g_levelTransitionTimer && Collision::IsSafeFooting(0, (uint8_t*)&g_buzzActor.collisionFlags) && (g_unk52B81C & 0xFFFFFFFC) != 0)
			{
				g_buzzActor.actorFlags |= 8;
				g_buzzActor.respawnPos = g_buzzActor.posAngles.pos;
				g_buzzActor.respawnYawAngle = g_buzzActor.posAngles.angles.yaw;
			}
		}

		// STUB: TOY2 0x004A2D80
		void UpdateContactEffects() {}

		// FUNCTION: TOY2 0x004A4B90
		void ResetGravityBoots()
		{
			if (g_gravityBootsDuration)
				g_gravityBootsDuration = 0;
		}

		// FUNCTION: TOY2 0x004A4D60
		void ActivateRocketBoots(Toy2::Levels::Type63* a1)
		{
			int32_t i;
			Nu3D::Particles::ParticleInstance* particle1;
			Nu3D::Particles::ParticleInstance* particle2;

			if (g_rocketBootsDuration)
			{
				g_rocketBootsDuration = 0;

				for (i = 0; i < 64; ++i)
				{
					if (Nu3D::Particles::g_particleInstances[i].typeId == 47)
						Nu3D::Particles::g_particleInstances[i].lifetime = 1;
				}

				Nu3D::Link::SetScaleFromFixedOffsets(g_rocketBootsPickup->props.linkId, 4096, 4096, 4096);
				g_rocketBootsPickup->pos.y = g_rocketBootsPickupY;
			}

			g_rocketBootsPickup = a1;
			g_rocketBootsPickupY = a1->pos.y;

			ResetBuzzState();

			g_rocketBootsDuration = 250;

			particle1 = Nu3D::Particles::SpawnFromPreset(
				g_buzzActor.posAngles.pos.x,
				g_buzzActor.posAngles.pos.y,
				g_buzzActor.posAngles.pos.z,
				47,
				2);

			particle1->rotSpeed = -32;
			particle1->groundAlignRot = 128;
			particle1->lifetime = g_rocketBootsDuration;

			particle2 = Nu3D::Particles::SpawnFromPreset(
				g_buzzActor.posAngles.pos.x,
				g_buzzActor.posAngles.pos.y,
				g_buzzActor.posAngles.pos.z,
				47,
				2);

			particle2->unkByte1 = 101;
			particle2->rotSpeed = 32;
			particle2->lifetime = g_rocketBootsDuration;

			AudioManager::PlaySoundEffect(76, &g_buzzActor.posAngles.pos);
		}

		// FUNCTION: TOY2 0x004A5170
		void TickCosmicShield()
		{
			int32_t scale;
			BOOL playSound;
			Vector3I pos;

			if (g_cosmicShieldPickup)
			{
				pos.x = (g_buzzActor.posAngles.pos.x - Camera::g_activeCameraTransform.pos.x) >> 5;
				pos.y = (g_buzzActor.posAngles.pos.y - Camera::g_activeCameraTransform.pos.y - 0x2000) >> 5;
				pos.z = (g_buzzActor.posAngles.pos.z - Camera::g_activeCameraTransform.pos.z) >> 5;

				Nu3D::Math::NormalizeToFixedPoint(&pos, &pos);

				pos.y = (g_buzzActor.posAngles.pos.y - pos.y - 0x2000) >> 5;
				pos.z = (g_buzzActor.posAngles.pos.z - pos.z) >> 5;
				pos.x = (g_buzzActor.posAngles.pos.x - pos.x) >> 5;

				Nu3D::Link::SetPositionRawAndCommit(g_cosmicShieldPickup->props.linkId, pos.x, pos.y, pos.z);
				Nu3D::Link::SetRotationRelative8bit(g_cosmicShieldPickup->props.linkId, 0, g_cosmicShieldRotYaw, g_cosmicShieldSpin);

				g_cosmicShieldRotYaw = -g_buzzActor.posAngles.angles.yaw & 0xFFF;
				g_cosmicShieldSpin = (g_cosmicShieldSpin - Renderer::g_frameDelta * (g_buzzActor.unkVar30 >> 4)) & 0xFFF;

				playSound = 1;

				if (g_buzzActor.inhibitMovementTimer >= 128)
				{
					scale = (Numerics::g_sinCosLUT[64 * (g_buzzActor.inhibitMovementTimer & 0x3F)] >> 4) + 12288;
				}
				else
				{
					scale = 3 * (Numerics::g_sinCosLUT[8 * g_buzzActor.inhibitMovementTimer] >> 2);
					playSound = g_tickAccumulators.tick16Acc >= 8u;
				}

				Nu3D::Link::SetScaleFromFixedOffsets(g_cosmicShieldPickup->props.linkId, scale, scale, scale);

				if (playSound)
				{
					AudioManager::g_pitchArray[0] = 4 * (2048 - g_buzzActor.inhibitMovementTimer);
					AudioManager::PlaySoundEffect(77, &g_buzzActor.posAngles.pos);
				}

				Renderer::BlitTextureByIndexOffset(
					16,
					128,
					192,
					64,
					64,
					4 * g_tickAccumulators.tick16Acc,
					2 * g_tickAccumulators.tick32Acc,
					0,
					-64);
			}
		}

		// FUNCTION: TOY2 0x004A5340
		void CancelGrapple()
		{
			if (g_grappleState)
			{
				if (g_grappleTimer > 0)
					--g_grappleTimer;

				g_grappleState = 0;
			}
		}

		// FUNCTION: TOY2 0x004A5540
		void TickGrapple()
		{
			WorldPos direction;

			if (!g_grappleState)
				return;

			g_unk53C838 = 0;

			if ((g_grappleState & 1) == 0)
				goto LABEL_14;

			g_buzzActor.facingAngle = Nu3D::Math::CartesianToFixedAngle(
				g_grappleTargetPos.x - g_unk50A0A0.x,
				g_grappleTargetPos.z - g_unk50A0A0.z);

			g_grappleProgress += 2 * Renderer::g_frameDelta;

			if (g_grappleProgress <= g_grappleDistance)
			{
				direction.x = g_grappleProgress * (g_grappleTargetPos.x - g_unk50A0A0.x) / g_grappleDistance;
				direction.y = g_grappleProgress * (g_grappleTargetPos.y - g_unk50A0A0.y) / g_grappleDistance;
				direction.z = g_grappleProgress * (g_grappleTargetPos.z - g_unk50A0A0.z) / g_grappleDistance;

				Renderer::Beam::QueueBeam(34, 30, 400, (WorldPos*)&g_unk50A0A0, &direction, 0x80, 0x60, 0x20);

			LABEL_14:
				if (g_grappleState == 2)
				{
					if (g_grappleProgress > 4 * g_grappleDistance / 3 + 10)
					{
						if (g_grappleTimer > 0)
							--g_grappleTimer;

						g_buzzActor.actorFlags &= ~4;
						g_grappleState = 0;

						g_buzzActor.posAngles.pos.x = g_grappleTargetPos.x;
						g_buzzActor.posAngles.pos.y = g_grappleTargetPos.y + 5632;
						g_buzzActor.posAngles.pos.z = g_grappleTargetPos.z;
						g_buzzActor.velX = 0;
						g_buzzActor.gravityVel = 0;
						g_buzzActor.velForward = 0;
					}
					else
					{
						AudioManager::PlaySoundEffect(83, &g_buzzActor.posAngles.pos);

						direction.x = g_grappleTargetPos.x - g_unk50A0A0.x;
						direction.y = g_grappleTargetPos.y - g_unk50A0A0.y;
						direction.z = g_grappleTargetPos.z - g_unk50A0A0.z;

						Renderer::Beam::QueueBeam(34, 30, 400, (WorldPos*)&g_unk50A0A0, &direction, 0x80, 0x60, 0x20);

						direction.y = (g_grappleTargetPos.y - g_buzzActor.posAngles.pos.y + 5632) >> 3;
						direction.x = (g_grappleTargetPos.x - g_buzzActor.posAngles.pos.x) >> 3;
						direction.z = (g_grappleTargetPos.z - g_buzzActor.posAngles.pos.z) >> 3;

						Nu3D::Math::NormalizeToFixedPoint((Vector3I*)&direction, (Vector3I*)&direction);

						if (g_grappleProgress >= 10)
						{
							g_buzzActor.velX = direction.x / 3;
							g_buzzActor.gravityVel = direction.y / 3;
							g_buzzActor.velForward = direction.z / 3;
						}

						g_grappleProgress += Renderer::g_frameDelta;
					}
				}

				return;
			}

			AudioManager::PlaySoundEffect(82, &g_buzzActor.posAngles.pos);

			if (g_grappleState != 3)
			{
				g_grappleState = 2;
				g_grappleProgress = 0;

				if (Camera::g_cameraState)
					Camera::SnapBehindBuzz(&Camera::g_gameplayCameraState);

				if ((g_buzzActor.posAngles.angles.yaw & 0x200) != 0)
					Camera::g_gameplayCameraState.orbitYaw = (Camera::g_gameplayCameraState.orbitYaw + 1024) & 0xFFF;
				else
					Camera::g_gameplayCameraState.orbitYaw = (Camera::g_gameplayCameraState.orbitYaw - 1024) & 0xFFF;

				goto LABEL_14;
			}

			if (g_grappleTimer > 0)
				--g_grappleTimer;

			g_buzzActor.actorFlags &= ~4;
			g_grappleState = 0;
		}

		// FUNCTION: TOY2 0x004A5C40
		void TickBeamShots()
		{
			BeamShot* beamShot;
			int32_t index;
			int32_t lifeTimer;
			int32_t frameDelta;
			WorldPos direction;
			WorldPos originPos;

			beamShot = &g_beamShots[0];
			index = 4;

			do
			{
				if (beamShot->lifeTimer)
				{
					originPos.x = beamShot->originPos.x;
					originPos.y = beamShot->originPos.y;
					originPos.z = beamShot->originPos.z;

					direction.y = beamShot->headPos.y - originPos.y;
					direction.z = beamShot->headPos.z - originPos.z;
					direction.x = beamShot->headPos.x - originPos.x;

					lifeTimer = beamShot->lifeTimer * 4;

					Renderer::Beam::QueueBeam(
						9,
						beamShot->halfWidth,
						400,
						&originPos,
						&direction,
						(lifeTimer * beamShot->tintB) >> 7,
						(lifeTimer * beamShot->tintG) >> 7,
						(lifeTimer * beamShot->tintR) >> 7);

					frameDelta = Renderer::g_frameDelta;

					beamShot->lifeTimer -= Renderer::g_frameDelta;

					if (beamShot->lifeTimer <= 0)
						beamShot->lifeTimer = 0;

					beamShot->moveTimer -= frameDelta;

					if (beamShot->moveTimer > 0)
					{
						beamShot->headPos.x += frameDelta * beamShot->velocity.x;
						beamShot->headPos.y += frameDelta * beamShot->velocity.y;
						beamShot->headPos.z += frameDelta * beamShot->velocity.z;
					}
					else
					{
						beamShot->lifeTimer = 0;
					}
				}

				++beamShot;
				--index;
			} while (index);
		}

		// FUNCTION: TOY2 0x004A62A0
		void TickGadgets()
		{
			int32_t i;

			if (g_rocketBootsDuration)
			{
				if (g_underwaterYLevel && g_buzzActor.posAngles.pos.y > g_underwaterYLevel && g_buzzActor.gravityVel > -768)
					g_buzzActor.gravityVel += -128 * Renderer::g_frameDelta;

				AudioManager::PlaySoundEffect(64, &g_buzzActor.posAngles.pos);
				g_rocketBootsDuration -= Renderer::g_frameDelta;

				if (g_rocketBootsDuration <= 0)
				{
					g_rocketBootsDuration = 0;

					for (i = 0; i < 64; ++i)
					{
						if (Nu3D::Particles::g_particleInstances[i].typeId == 47)
							Nu3D::Particles::g_particleInstances[i].lifetime = 1;
					}

					Nu3D::Link::SetScaleFromFixedOffsets(g_rocketBootsPickup->props.linkId, 4096, 4096, 4096);
					g_rocketBootsPickup->pos.y = g_rocketBootsPickupY;
				}
			}

			TickCosmicShield();
			TickBeamShots();
			TickGrapple();
		}
	}
}
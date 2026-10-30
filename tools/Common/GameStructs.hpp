#pragma once

#include "InputMappings.hpp"

#include <cstdint>
#include <iostream>

#include <directx7/d3d.h>
#include <directx7/d3dtypes.h>

// game types
struct Vector3I
{
	int32_t x;
	int32_t y;
	int32_t z;

	void print()
	{
		printf("[%d, %d, %d]\n", x, y, z);
	}
};

struct Vector3I16
{
	int16_t x;
	int16_t y;
	int16_t z;

	void print()
	{
		printf("[%d, %d, %d]\n", x, y, z);
	}
};

struct Toy2BuzzActor
{
	Vector3I pos;
	int16_t pitchAngle;
	int16_t yawAngle;
	int16_t rollAngle;
	int16_t primaryAnimIdx;
	int16_t creatureId;
	int16_t secondaryAnimIdx;
	int32_t unkVar7;
	int32_t unkVar8;
	int32_t unkVar9;
	int32_t unkVar10;
	int32_t unkVar11;
	int32_t unkVar12;
	int32_t unkVar13;
	int32_t unkVar14;
	int16_t unkWord1;
	int16_t unkWord2;
	int16_t unkWord3;
	int16_t unkWord4;
	uint16_t actorFlags;
	int16_t visibilityDistance;
	int32_t unkVar22;
	int16_t facingAngle;
	int16_t unkVar23;
	int32_t unkVar24;
	int32_t unkVar25;
	int32_t unkVar26;
	int32_t unkVar27;
	Vector3I motionTargetPos;
	int32_t targetYaw;
	int32_t unkVar28;
	int32_t unkVar29;
	int32_t unkVar30;
	int32_t unkVar31;
	int16_t unkWord13;
	int16_t actorHealth;
	int32_t unkVar33;
	int32_t unkVar34;
	int32_t unkVar35;
	int16_t unkWord15;
	int16_t unkWord16;
	int16_t unkVar37;
	int16_t unkVar38;
	int16_t buzzWord0;
	int16_t health;
	int16_t buzzWord1;
	int16_t lives;
	int16_t unkShort40;
	int16_t unkShort41;
};

struct EntityControl
{
	uint8_t actorPhase;
	uint8_t unkByte;
	uint16_t actorFlags;
};

enum ActorUpdateFlags : int16_t
{
	ACTOR_UPDATE_MOVING = 0x1,
	ACTOR_UPDATE_GROUNDED = 0x2,
	ACTOR_UPDATE_ANIM_TICK = 0x4,
};

enum BehaviourTargetFlags : int16_t
{
	BEHAVIOUR_TARGET_PLAYER_IN_BOUNDS = 0x1,
};

struct CreatureRam
{
	Vector3I pos;
	uint8_t creatureId;
	uint8_t movCtrl;
	uint8_t rotSpeed;
	uint8_t initialFacingAngle;
	EntityControl entCtrl;
	int16_t boundHalfX;
	int16_t boundHalfZ;
	int16_t boundAngle;
	int16_t defenseMode;
	uint8_t latSpeedNoTarget;
	uint8_t latSpeedTarget;
	uint8_t speedNoTarget;
	uint8_t speedTarget;
};

class ActorBehaviourContext;
typedef void(__cdecl* ActorBehaviourFn)(ActorBehaviourContext* ctx);

struct Toy2Actor
{
	Vector3I pos;
	int16_t pitchAngle;
	int16_t yawAngle;
	int16_t rollAngle;
	int16_t primaryAnimIdx;
	int16_t creatureId;
	int16_t secondaryAnimIdx;
	int32_t unkVar7;
	int32_t unkVar8;
	int32_t unkVar9;
	Vector3I16 unkVector;
	int16_t unkVar10;
	int32_t unkVar12;
	int32_t unkVar13;
	int32_t unkVar14;
	int16_t unkWord1;
	int16_t unkWord2;
	int16_t unkWord3;
	int16_t unkWord4;
	uint16_t actorFlags;
	int16_t visibilityDistance;
	int32_t velX;
	int32_t gravityVel;
	int32_t velForward;
	Vector3I boundary;
	Vector3I motionTargetPos;
	int32_t targetYaw;
	int32_t unkVar28;
	int32_t unkVar29;
	int32_t unkVar30;
	int16_t unkShort1;
	int16_t unkShort2;
	int16_t damageCooldownTimer;
	int16_t actorPhase;
	uint16_t* movementData;
	int32_t unkVar34;
	int16_t unkShort3;
	int16_t unkShort4;
	int16_t unkWord15;
	int16_t unkWord16;
	int32_t lastValidYPosition;
	ActorBehaviourFn actorBehaviour;
	CreatureRam* creatureRam;
};

struct ActorBehaviourContext
{
	Toy2Actor* actor;
	ActorUpdateFlags updateFlags;
	BehaviourTargetFlags targetFlags;
	int16_t localStrafeSpeed;
	int16_t localForwardSpeed;
};

constexpr int32_t kMaxActors = 64;

struct Angles
{
	uint16_t pitch;
	uint16_t yaw;
};

struct ActiveCameraTransform
{
	Vector3I pos;
	Angles angles;
	int16_t roll;
	int16_t unkShort;
};

enum ActorFlags : uint16_t
{
	ACTOR_CUTSCENE_MOVE = 0x1,
	ACTOR_VISIBLE_ACTIVE = 0x2,
	ACTOR_LOCKED_MOVE = 0x4,
	ACTOR_GROUNDED = 0x8,
	ACTOR_FLOOR_SNAP = 0x10,
	ACTOR_STUNNED = 0x20,
	ACTOR_OOB_SPECIAL_MOVE = 0x40,
	ACTOR_DEATH_DAMAGE = 0x80,
	ACTOR_AIRBORNE_SPECIAL = 0x100,
	ACTOR_PLAYER_TRIGGER = 0x200,
	ACTOR_VEL_CLAMP_OVERRIDE = 0x400,
	ACTOR_NO_DESPAWN = 0x800,
	ACTOR_UNUSED_1000 = 0x1000,
	ACTOR_FORCE_REMOVE = 0x2000,
	ACTOR_CAMERA_HOVER = 0x4000,
	ACTOR_UNKNOWN_8000 = 0x8000,
};

struct CD3DFramework
{
	int32_t hWnd;
	int32_t bIsFullscreen;
	int32_t dwRenderWidth;
	int32_t dwRenderHeight;
	RECT rcScreenRect;
	RECT rcViewportRect;
	LPDIRECTDRAWSURFACE4 pddsFrontBuffer;
	LPDIRECTDRAWSURFACE4 pddsBackBuffer;
	LPDIRECTDRAWSURFACE4 pddsRenderTarget;
	LPDIRECTDRAWSURFACE4 pddsZBuffer;
	LPDIRECT3DDEVICE3 pd3dDevice;
	LPDIRECT3DVIEWPORT3 pvViewport;
	LPDIRECTDRAW4 pDD;
	LPDIRECT3D3 pD3D;
	D3DDEVICEDESC ddDeviceDesc;
	int32_t dwDeviceMemType;
	DDPIXELFORMAT ddpfZBuffer;
	int32_t initialized;
};

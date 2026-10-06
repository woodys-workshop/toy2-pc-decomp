#pragma once

#include "Common.h"

struct Vector3F
{
	float x;
	float y;
	float z;
};

struct Vector3I
{
	int32_t x;
	int32_t y;
	int32_t z;
};

struct Vector2F
{
	float x;
	float y;
};

struct Vector2I
{
	int32_t x;
	int32_t y;
};

struct Vector3I16
{
	int16_t x;
	int16_t y;
	int16_t z;
};

struct Angles
{
	uint16_t pitch;
	uint16_t yaw;
};

struct AnglesWithRoll
{
	Angles angles;
	int16_t roll;
	int16_t unkRoll;
};

struct PosAndAngles
{
	Vector3I pos;
	Angles angles;
};

struct WorldPos
{
	int32_t x;
	int32_t y;
	int32_t z;
	int32_t x2;
};

struct PointI
{
	int32_t x;
	int32_t y;
};

struct PointI16
{
	int16_t x;
	int16_t y;
};

struct Plane
{
	Vector3F normal;
	float distance;
};

struct Matrix3x3I16
{
	int16_t m00;
	int16_t m01;
	int16_t m02;
	int16_t m10;
	int16_t m11;
	int16_t m12;
	int16_t m20;
	int16_t m21;
	int16_t m22;
};

namespace Numerics
{
	extern float* g_trigLUT;
	extern int16_t g_sinCosLUT[4096];

	int32_t RoundUpToPowerOf2(int32_t number);
	void InitTrigLut();
}
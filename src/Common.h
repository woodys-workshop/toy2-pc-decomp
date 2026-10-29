#pragma once

// Typedefs
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef __int64 int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;

typedef float float32_t;
typedef double float64_t;

// theres some parts of the game that will literally not let it boot, if they aren't fixed
// this macro lets me patch them out in the decomp without modifying the code for the actual function parity build
#if 1
	#define APPLY_FIXES
#endif

// The MSVC 6 way of declaring static asserts
#define STATIC_ASSERT_GLUE(a, b) a##b
#define STATIC_ASSERT_JOIN(a, b) STATIC_ASSERT_GLUE(a, b)

#define STATIC_ASSERT(expr) typedef char STATIC_ASSERT_JOIN(static_assert_failed_at_line_, __LINE__)[(expr) ? 1 : -1]

#ifdef APPLY_FIXES
	#define DECOMP_PRINT(args) printf args
#else
	#define DECOMP_PRINT(args) ((void)0)
#endif

// Colors
struct RGB32
{
	int32_t r;
	int32_t g;
	int32_t b;
};

struct RGBColor
{
	float r;
	float g;
	float b;
};

struct RGB16
{
	int16_t r;
	int16_t g;
	int16_t b;
};

struct ARGB
{
	uint8_t a;
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct RGBColor3B
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

union RGBA
{
	struct
	{
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
	};

	uint32_t value;
};

union BGRA
{
	struct
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	uint32_t value;
};

struct BGRA_F
{
	float b;
	float g;
	float r;
	float a;
};

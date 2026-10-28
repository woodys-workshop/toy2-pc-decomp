#pragma once

#include "Common.h"
#include "Numerics.h"

namespace Nu3D
{
	struct Vertex
	{
		Vector3F position;
		Vector3F normals;
		RGBA diffuse;
		Vector2F coords;
	};

	struct VertexTL
	{
		Vector3F position;
		float rhw;
		RGBA diffuse;
		RGBA specular;
		Vector2F uv;
	};

	struct ShapeVertex
	{
		Vector3F position;
		Vector3F normals;
		RGBA diffuse;
		Vector2F coords;
		int16_t primIndex;
		int16_t primVerticesSize;
	};

	extern int32_t g_useAsDiffuseModulation;
	extern int32_t g_defaultPrimitiveFlags;

	void SetIsSoftwareRendering(int32_t value);
	uint32_t GetHighResolutionTime();
	void PrecisionSleep(int32_t delayMs);
	void MemSet32Util(void* buffer, int32_t count, uint32_t value);

	int32_t SetUseAsDiffuseModulation(int32_t option);
	int32_t SetDefaultPrimFlags(int32_t option);

	STATIC_ASSERT(sizeof(VertexTL) == 32);
	STATIC_ASSERT(sizeof(Vertex) == 36);
	STATIC_ASSERT(sizeof(ShapeVertex) == 40);
}

#pragma once

#include "Common.h"
#include "Nu3D/Primitive.h"
#include <STDIO.H>

namespace NGNLoader
{
	namespace ObjectLoad
	{
		extern int32_t g_curVertexCount;
		extern Nu3D::ShapeVertex g_shapeVertices[2000];

		void PrepareGlobals();
		int32_t ExtractShapeTextures(FILE* stream);
		int32_t ExtractShapeMaterials(FILE* stream);
		int32_t ExtractShapeVertices(FILE* stream);
		Nu3D::Primitive* ExtractShapeData(FILE* stream);
		Nu3D::Material* GetCurrentMatByIndex(uint32_t index);
	}
}
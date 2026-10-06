#pragma once

#include "Common.h"
#include "Numerics.h"
#include "Nu3D/Sprite.h"
#include "Nu3D/Primitive.h"
#include "Nu3D/Material.h"
#include "Renderer/Renderer.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>

namespace Nu3D
{
	struct InstanceData;
}


namespace Renderer
{
	extern Nu3D::Sprite* g_spriteBuckets[256];
	extern Nu3D::InstanceData g_instanceData[1000];
	extern RenderEntry g_renderEntries[3000];
	extern Nu3D::Sprite g_spriteBuffer[2001];

	extern int32_t g_additionalRenderFlags;
	extern float g_lodFactor;
	extern int32_t g_unk9F5FF8;
	extern int32_t g_useVertexColorMod;
	extern int32_t g_vertexColorModBlue;
	extern int32_t g_vertexColorModGreen;
	extern int32_t g_vertexColorModRed;
	extern int32_t g_maxSimultaneousTextures;
	extern Nu3D::Patch::PatchVertices g_FVF_14C_Buffer_1;
	extern Nu3D::Patch::PatchVertices g_FVF_14C_Buffer_2;

	extern int32_t g_instanceDataCounter;
	extern int32_t g_renderEntryCount;
	extern int32_t g_spriteBufferCount;
	extern int32_t g_maxBucketDepth;
	extern float g_instanceHorzOffset;
	extern float g_instanceVertOffset;
	extern int32_t g_useSoftwareRasterizer;
	extern int32_t g_useOptimizedDepth;
	extern int32_t g_unk508718;
	extern int32_t g_type9DrawCallCount;
	extern int32_t g_type9VertexCount;
	extern int32_t g_frameTriCount;
	extern int32_t g_drawWireframe;
	extern Nu3D::VertexTL g_vertsBuffer[1500];

	int32_t Set508718(int32_t value);
	int32_t SetAdditionalRenderFlags(int32_t flags);
	void ResetRenderPools();
	void RenderPrimitive(Nu3D::Primitive* primitive, D3DMATRIX* matrix, int32_t renderStage);
	void ProcessPrimitive(Nu3D::InstanceData* instance, Nu3D::Primitive* prim);

	void FlushMaterialBuckets();
	void FlushTransparentBuckets();
	void FlushPrimitives();

	void BindMaterial(Nu3D::Material* material, int32_t forceBind);
	void UnbindMaterial();
	int32_t SetupMaterialRenderState(Nu3D::Material* material, int32_t renderState);
	void DrawPrimitive(void* vertices, int32_t vertexCount);

	namespace InstanceData
	{
		Nu3D::InstanceData* AllocFromMatrix(D3DMATRIX* matrix, int32_t renderFlags);
		Nu3D::InstanceData* AllocFromNodeMatrices(D3DMATRIX* nodeMatrices, Nu3D::Primitive::Header* headers, int32_t headerCount, int32_t** flags, int32_t renderStage);
	}

	namespace Vertices
	{
		void ApplyMetadata100(Nu3D::Patch::PatchVertices* patchVertices, D3DMATRIX* matrix, Nu3D::InstanceData* instance);
		void ProjectForTex14(Nu3D::Patch::PatchVertices* patchVertices, D3DMATRIX* matrix);
		void ModuleColor(Nu3D::Patch::PatchVertices* patchVertices, int32_t colorR, int32_t colorG, int32_t colorB);
		void ApplyOffset(Nu3D::Patch::PatchVertices* patchVertices, float horzOffset, float vertOffset);
		void ProjectToScreen(Nu3D::Patch::PatchVertices* patchVertices, D3DMATRIX* matrix, int32_t headerCount);
	}

	namespace Sprite
	{
		HRESULT Render2DSprite(Nu3D::Sprite* sprite);
		void DispatchCommand(Nu3D::Sprite* command);
		void RenderType8(Nu3D::Material* material, RenderEntry* entry);
		void RenderType9(Nu3D::Material* material, RenderEntry* entry);
		HRESULT RenderType10(Nu3D::Sprite* sprite);

		// the sprite style render types, these are only needed for queued sprites
		void RenderQuadSprite(Nu3D::Sprite* sprite);
		void RenderGroundAlignedSprite(Nu3D::Sprite* sprite);
		void RenderBillboardSprite(Nu3D::Sprite* sprite);
		void RenderTriangleSprite(Nu3D::Sprite* sprite);
		void RenderQuadSpriteFromVerts(Nu3D::Sprite* sprite);
	}
}

#include "Renderer/RenderQueue.h"
#include "Renderer/Sprite.h"
#include "Nu3D/Math.h"
#include "Nu3D/Camera.h"
#include "Nu3D/Viewport.h"
#include "DrawingDevice.h"
#include "SoftwareRenderer.h"
#include "Logger.h"

#include <STRING.H>

namespace Renderer
{
	// GLOBAL: TOY2 0x008846B8
	Nu3D::Sprite* g_spriteBuckets[256];

	// GLOBAL: TOY2 0x0088C7D0
	Nu3D::InstanceData g_instanceData[1000];

	// GLOBAL: TOY2 0x0095C860
	RenderEntry g_renderEntries[3000];

	// the last sprite is never handed out, it's used as a blank sprite to clear an instance's sprite with
	// GLOBAL: TOY2 0x009B2760
	Nu3D::Sprite g_spriteBuffer[2001];

	// GLOBAL: TOY2 0x009F2F24
	int32_t g_instanceDataCounter;

	// GLOBAL: TOY2 0x0094FCD0
	int32_t g_renderEntryCount;

	// GLOBAL: TOY2 0x00508700
	int32_t g_spriteBufferCount = 2000;

	// GLOBAL: TOY2 0x009F600C
	int32_t g_maxBucketDepth;

	// GLOBAL: TOY2 0x009F5FD8
	float g_instanceHorzOffset;

	// GLOBAL: TOY2 0x009F5FDC
	float g_instanceVertOffset;

	// GLOBAL: TOY2 0x009F5FF0
	int32_t g_useSoftwareRasterizer;

	// GLOBAL: TOY2 0x0050871C
	int32_t g_useOptimizedDepth = 1;

	// GLOBAL: TOY2 0x00508718
	int32_t g_unk508718 = 5;

	// GLOBAL: TOY2 0x00E4D94C
	int32_t g_type9DrawCallCount;

	// GLOBAL: TOY2 0x00E4D8FC
	int32_t g_type9VertexCount;

	// GLOBAL: TOY2 0x00E4D944
	int32_t g_frameTriCount;

	// GLOBAL: TOY2 0x009F5FFC
	int32_t g_drawWireframe;

	// GLOBAL: TOY2 0x0094FCE0
	Nu3D::VertexTL g_vertsBuffer[1500];

	// FUNCTION: TOY2 0x004B5CE0
	int32_t Set508718(int32_t value)
	{
		int32_t previous = g_unk508718;
		g_unk508718 = value;

		return previous;
	}

	// FUNCTION: TOY2 0x004B92B0
	int32_t SetAdditionalRenderFlags(int32_t flags)
	{
		int32_t previous = g_additionalRenderFlags;
		g_additionalRenderFlags = flags;

		return previous;
	}

	// FUNCTION: TOY2 0x004B62C0
	void ResetRenderPools()
	{
		memset(g_spriteBuckets, 0, sizeof(g_spriteBuckets));
		memset(SoftwareRenderer::g_depthBuckets, 0, sizeof(SoftwareRenderer::g_depthBuckets));

		g_instanceDataCounter = 1000;
		SoftwareRenderer::g_instanceCounter = 3000;

		g_renderEntryCount = 3000;
		g_spriteBufferCount = 2000;
	}

	// FUNCTION: TOY2 0x004B84E0
	Nu3D::InstanceData* InstanceData::AllocFromMatrix(D3DMATRIX* matrix, int32_t renderFlags)
	{
		if (! g_instanceDataCounter)
			return 0;

		int32_t index = --g_instanceDataCounter;

		Nu3D::InstanceData* instance = &g_instanceData[index];

		memcpy(&instance->matrices[0], matrix, sizeof(D3DMATRIX));

		instance->renderFlags = renderFlags;
		instance->lodFactor = g_lodFactor;
		instance->horzOffset = g_instanceHorzOffset;
		instance->vertOffset = g_instanceVertOffset;
		instance->renderModeFlags = g_unk9F5FF8 != 0;

		if (g_useVertexColorMod)
		{
			instance->renderModeFlags |= 2;

			instance->vertexModColor.r = g_vertexColorModRed;
			instance->vertexModColor.g = g_vertexColorModGreen;
			instance->vertexModColor.b = g_vertexColorModBlue;
		}

		Nu3D::Viewport::GetViewClipRect(&instance->clipRect);

		instance->unkInt6 = g_unk508718;

		memcpy(&instance->sprite, &g_spriteBuffer[2000], sizeof(Nu3D::Sprite));

		return instance;
	}

	// FUNCTION: TOY2 0x004B86F0
	void RenderEntry::InsertIntoBucket(RenderEntry* renderEntry)
	{
		Vector3F cameraPos;
		Vector3F entryPos;

		Nu3D::Math::GetPositionVector(&Nu3D::Camera::g_secondaryCamera.matrix, &cameraPos);
		Nu3D::Math::GetPositionVector(renderEntry->instanceData->matrices, &entryPos);
		Nu3D::Math::VertexSubtract(&cameraPos, &cameraPos, &entryPos);

		cameraPos.x = cameraPos.x * cameraPos.x;
		cameraPos.y = cameraPos.y * cameraPos.y;
		cameraPos.z = cameraPos.z * cameraPos.z;

		float distance = cameraPos.z + cameraPos.y + cameraPos.x;

		if (distance < 1.0)
			distance = 1.0;

		// TODO: Double check this
		// the exponent of the float is used as the bucket index, so buckets get exponentially further apart
		int32_t bucket = (*(uint32_t*)&distance >> 20) - 1016;

		renderEntry->distanceSquared = distance;

		if (bucket <= 255)
		{
			Nu3D::Sprite* current = g_spriteBuckets[bucket];
			Nu3D::Sprite* previous = 0;
			int32_t depth = 0;

			// sorted furthest first inside of a bucket
			while (current)
			{
				if (current->distanceSquared <= distance)
					break;

				previous = current;
				current = current->next;
				depth++;
			}

			renderEntry->next = (RenderEntry*)current;

			if (previous)
				previous->next = (Nu3D::Sprite*)renderEntry;
			else
				g_spriteBuckets[bucket] = (Nu3D::Sprite*)renderEntry;

			if (g_maxBucketDepth <= depth)
				g_maxBucketDepth = depth;
		}
	}

	// FUNCTION: TOY2 0x004B8670
	RenderEntry* RenderEntry::AllocObj(Nu3D::Material* material, Nu3D::Primitive* prim, Nu3D::InstanceData* instance)
	{
		if (! g_renderEntryCount)
		{
			Logger::DebugLog("rndrentryAllocObj - out of rndrentries");
			return 0;
		}

		int32_t index = --g_renderEntryCount;

		RenderEntry* entry = &g_renderEntries[index];

		entry->primitive = prim;
		entry->instanceData = instance;
		entry->material = material;

		// anything that needs sorting (lod fading, blending, etc.) goes into the depth sorted buckets,
		// everything else gets bucketed by material so it can be drawn with as few state changes as possible
		if (*(uint32_t*)&instance->lodFactor != 0x3F800000 || (material->metadata & 0xE33) != 0)
		{
			entry->type = RENDER_TYPE9;
			InsertIntoBucket(entry);
		}
		else
		{
			entry->type = RENDER_TYPE7;
			entry->next = (RenderEntry*)material->renderEntryHead;
			material->renderEntryHead = entry;
		}

		return &g_renderEntries[index];
	}

	// FUNCTION: TOY2 0x004B85E0
	void ProcessPrimitive(Nu3D::InstanceData* instance, Nu3D::Primitive* prim)
	{
		Nu3D::Material* material = Nu3D::Material::GetFreeByIndex(prim->materialIndex);

		if (! material)
			return;

		for (;;)
		{
			RenderEntry::AllocObj(material, prim, instance);

			if ((material->metadata & 4) != 0)
				RenderEntry::AllocObj(Nu3D::g_tex14Version1, prim, instance);

			if ((material->metadata & 0x40) != 0)
				RenderEntry::AllocObj(Nu3D::g_tex14Version2, prim, instance);

			if ((int8_t)material->metadata < 0)
				RenderEntry::AllocObj(Nu3D::g_tex14Version3, prim, instance);

			// walk the multitexture chain, going around again for the next material in it
			int32_t textureCount = 0;

			if (g_maxSimultaneousTextures > 0)
			{
				while (material)
				{
					material = material->alwaysZero;

					if (++textureCount >= g_maxSimultaneousTextures)
						break;
				}
			}

			if (! material)
				return;
		}
	}

	// FUNCTION: TOY2 0x004B8490
	void RenderPrimitive(Nu3D::Primitive* primitive, D3DMATRIX* matrix, int32_t renderStage)
	{
		if (0.0 != primitive->originRadius)
		{
			Nu3D::InstanceData* instance = InstanceData::AllocFromMatrix(matrix, g_additionalRenderFlags | renderStage);

			if (instance)
			{
				// a primitive can have a list of other primitives hanging off of it
				for (Nu3D::Primitive* prim = primitive; prim; prim = prim->listNext)
					ProcessPrimitive(instance, prim);
			}
		}
	}

	// FUNCTION: TOY2 0x004B8400
	void FlushMaterialBuckets()
	{
		UnbindMaterial();

		for (Nu3D::Material* material = Nu3D::Material::GetHead(); material; material = material->next)
		{
			// the material's entries are RenderEntry's, but they get dispatched like any other command
			Nu3D::Sprite* entryHead = (Nu3D::Sprite*)material->renderEntryHead;

			if (entryHead)
			{
				g_instanceHorzOffset = material->horzOffset;
				g_instanceVertOffset = material->vertOffset;

				BindMaterial(material, 0);
				Sprite::DispatchCommand(entryHead);
			}

			material->renderEntryHead = 0;
		}
	}

	// FUNCTION: TOY2 0x004B6A90
	void FlushTransparentBuckets()
	{
		Nu3D::Sprite** command = &g_spriteBuckets[255];

		// even in hardware mode the depth sorted buckets get sent through the software rasterizer
		g_useSoftwareRasterizer = g_isSoftwareRendering == 0 ? g_useOptimizedDepth : 0;

		do
			Sprite::DispatchCommand(*command--);
		while (command >= g_spriteBuckets);

		g_useSoftwareRasterizer = 0;
	}

	// FUNCTION: TOY2 0x004B5CF0
	void FlushPrimitives()
	{
		// Draws all the triangles that were queued by the SoftwareRenderer::Submit* functions, furthest first.
		// Triangles are batched up into one DrawPrimitive for as long as they use the same state.

		Nu3D::Material* material = 0;
		int32_t renderFlags = 0;
		int32_t textureIdx = 0;
		int32_t vertexCount = 0;

		for (int32_t bucket = 1023; bucket >= 0; bucket--)
		{
			for (SoftwareRenderer::SortedTriangle* triangle = SoftwareRenderer::g_depthBuckets[bucket]; triangle; triangle = triangle->next)
			{
				bool stateChanged = false;

				if (vertexCount == 0)
				{
					stateChanged = true;
				}
				else if (vertexCount >= 1500)
				{
					DrawPrimitive(g_vertsBuffer, vertexCount);
					vertexCount = 0;
					stateChanged = true;
				}
				else if (triangle->renderEntry)
				{
					if (material != triangle->renderEntry->material || renderFlags != triangle->renderFlags)
					{
						DrawPrimitive(g_vertsBuffer, vertexCount);
						vertexCount = 0;
						stateChanged = true;
					}
				}
				else if (renderFlags != triangle->renderFlags || textureIdx != triangle->textureIdx || material)
				{
					DrawPrimitive(g_vertsBuffer, vertexCount);
					vertexCount = 0;
					stateChanged = true;
				}

				if (stateChanged)
				{
					if (triangle->renderEntry)
					{
						BindMaterial(triangle->renderEntry->material, 0);
						SetupMaterialRenderState(triangle->renderEntry->material, triangle->renderFlags);

						renderFlags = triangle->renderFlags;
						material = triangle->renderEntry->material;
					}
					else
					{
						InitRenderState(triangle->renderFlags);
						BindTexture(triangle->textureIdx);

						material = 0;
						renderFlags = triangle->renderFlags;
						textureIdx = triangle->textureIdx;
					}
				}

				memcpy(&g_vertsBuffer[vertexCount], triangle->vertices, sizeof(triangle->vertices));
				vertexCount += 3;
			}

			SoftwareRenderer::g_depthBuckets[bucket] = 0;
		}

		DrawPrimitive(g_vertsBuffer, vertexCount);
		SoftwareRenderer::g_instanceCounter = 3000;
	}

	namespace InstanceData
	{
		// FUNCTION: TOY2 0x004B8840
		Nu3D::InstanceData* AllocFromNodeMatrices(D3DMATRIX* nodeMatrices, int32_t* nodeIndices, int32_t headerCount, int32_t** flags, int32_t renderStage)
		{
			if (! g_instanceDataCounter)
				return 0;

			int32_t index = --g_instanceDataCounter;

			D3DMATRIX* destination = g_instanceData[index].matrices;

			// every entry is the index of a node matrix to copy (4 bytes each), a node that is hidden in the flags makes the whole instance be skipped
			while (headerCount)
			{
				if (flags && ((uint32_t)flags[*nodeIndices] & 1))
				{
					g_instanceDataCounter = index + 1;
					return 0;
				}

				memcpy(destination, &nodeMatrices[*nodeIndices], sizeof(D3DMATRIX));

				destination++;
				nodeIndices++;
				--headerCount;
			}

			g_instanceData[index].renderFlags = renderStage;
			g_instanceData[index].lodFactor = g_lodFactor;
			g_instanceData[index].horzOffset = g_instanceHorzOffset;
			g_instanceData[index].vertOffset = g_instanceVertOffset;
			g_instanceData[index].renderModeFlags = g_unk9F5FF8 != 0;
			Nu3D::Viewport::GetViewClipRect(&g_instanceData[index].clipRect);

			g_instanceData[index].unkInt6 = g_unk508718;

			return &g_instanceData[index];
		}
	}
}

#include "Renderer/RenderQueue.h"
#include "Renderer/Sprite.h"
#include "Nu3D/Math.h"
#include "Nu3D/Camera.h"
#include "DrawingDevice.h"
#include "SoftwareRenderer.h"

#include <STRING.H>

namespace Renderer
{
	// TODO: Fix
	// the instance data's sprite is reused as scratch space for materials with metadata 0x100 (environment mapping)
	struct Metadata100Params
	{
		float scale;
		float unkFloats[4];
		Vector3F position;
		float unkFloats2[9];
		D3DMATRIX matrix;
	};

	STATIC_ASSERT(sizeof(Metadata100Params) == sizeof(Nu3D::Sprite));

	// TODO: Fix, inline
	// the vertex buffer that transformed vertices (XYZRHW) are written to is 32 bytes per vertex, uv lives at +0x18
	static float* TransformedVertexUV(void* lockedData, int32_t vertexIndex)
	{
		return (float*)((uint8_t*)lockedData + 32 * vertexIndex + 0x18);
	}

	// TODO: Fix, inline
	static uint8_t* TransformedVertexColor(void* lockedData, int32_t vertexIndex)
	{
		return (uint8_t*)lockedData + 32 * vertexIndex + 0x10;
	}

	// FUNCTION: TOY2 0x004C27D0
	void BindMaterial(Nu3D::Material* material, int32_t forceBind)
	{
		if (g_boundMaterial != material || forceBind)
		{
			g_boundMaterial = material;

			if (material)
			{
				if (! g_isSoftwareRendering)
					DrawingDevice::SetLightState(D3DLIGHTSTATE_MATERIAL, material->d3dMaterialHandle);

				int32_t textureStage = 0;

				if (g_maxSimultaneousTextures > 0)
				{
					int32_t* boundTextureIndex = g_boundTextureIndices;

					// every material in the chain is one texture stage
					do
					{
						int32_t texDataIndex = material->texDataIndex;

						if (*boundTextureIndex != texDataIndex)
						{
							*boundTextureIndex = texDataIndex;
							DrawingDevice::BindTexWithStage(material->texDataIndex, textureStage);
						}

						material = material->alwaysZero;

						if (! material)
							break;

						++textureStage;
						++boundTextureIndex;
					} while (textureStage < g_maxSimultaneousTextures);
				}
			}
			else if (g_maxSimultaneousTextures > 0)
			{
				memset(g_boundTextureIndices, 0xFF, sizeof(int32_t) * g_maxSimultaneousTextures);
			}
		}
	}

	// FUNCTION: TOY2 0x004B8450
	void UnbindMaterial() { BindMaterial(0, 0); }

	// FUNCTION: TOY2 0x004B6760
	int32_t SetupMaterialRenderState(Nu3D::Material* material, int32_t renderState)
	{
		int32_t textureStage = 0;

		if (g_maxSimultaneousTextures <= 0)
			return 0;

		for (;;)
		{
			int32_t newState = renderState;

			if (material)
			{
				int32_t metadata = material->metadata;

				if ((metadata & 2) != 0)
					newState = renderState | 0xC00;

				if ((metadata & 0x10) != 0)
					newState |= 0x4800;

				if ((metadata & 0x20) != 0)
					newState |= 0x20800;

				if ((metadata & 0x200) != 0)
					newState |= 0x100800;

				if ((metadata & 0x400) != 0)
					newState |= 0x200800;

				if ((metadata & 0x800) != 0)
					newState |= 0x400800;

				if ((metadata & 8) != 0)
					newState = (newState & 0xFFFFFF8F) | 0x40;

				if (material == Nu3D::g_tex14Version1 || material == Nu3D::g_tex14Version2 || material == Nu3D::g_tex14Version3)
					newState &= 0xFFFFFE7F;

				// only the stages after the first one take colour modulation from their material
				if (textureStage != 0)
				{
					if ((metadata & 0x40000) != 0)
						newState |= 0x40000;

					if ((metadata & 0x80000) != 0)
						newState |= 0x80000;
				}
			}

			// the general render state is only set up by the first stage
			if (textureStage == 0)
			{
				newState |= 0x40000;
				SetRenderState(newState);
			}

			SetTextureStageState(newState, textureStage);

			if (material)
				material = material->alwaysZero;

			if (++textureStage >= g_maxSimultaneousTextures)
				return textureStage;
		}
	}

	// FUNCTION: TOY2 0x004B5E20
	void DrawPrimitive(void* vertices, int32_t vertexCount)
	{
		if (vertexCount)
			DrawingDevice::DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_0x1C4, vertices, vertexCount, 16);
	}

	// FUNCTION: TOY2 0x004B7830
	void Vertices::ModuleColor(Nu3D::Patch::PatchVertices* patchVertices, int32_t colorR, int32_t colorG, int32_t colorB)
	{
		LPVOID lockedData;

		if (g_FVF_14C_Buffer_2.vertexBuffer && ! DrawingAPI::LockVertexBuffer(g_FVF_14C_Buffer_2.vertexBuffer, 0x801, &lockedData, 0))
		{
			for (int32_t i = 0; i < patchVertices->vertexCount; i++)
			{
				// modulation is 12 bit fixed point (4096 = 1.0), each channel clamps at 255
				uint8_t* color = TransformedVertexColor(lockedData, i);

				uint32_t red = colorR * color[2];
				color[2] = (red & 0xFFFFF000) <= 0xFF000 ? red >> 12 : 255;

				uint32_t green = colorG * color[1];
				color[1] = (green & 0xFFFFF000) <= 0xFF000 ? green >> 12 : 255;

				uint32_t blue = colorB * color[0];
				color[0] = (blue & 0xFFFFF000) <= 0xFF000 ? blue >> 12 : 255;
			}

			DrawingAPI::UnlockVertexBuffer(g_FVF_14C_Buffer_2.vertexBuffer);
		}
	}

	// FUNCTION: TOY2 0x004B6ED0
	void Vertices::ApplyOffset(Nu3D::Patch::PatchVertices* patchVertices, float horzOffset, float vertOffset)
	{
		LPVOID lockedData;

		if (g_FVF_14C_Buffer_2.vertexBuffer && ! DrawingAPI::LockVertexBuffer(g_FVF_14C_Buffer_2.vertexBuffer, 0x801, &lockedData, 0))
		{
			for (int32_t i = 0; i < patchVertices->vertexCount; i++)
			{
				float* uv = TransformedVertexUV(lockedData, i);

				uv[0] = horzOffset + patchVertices->data.vertices[i].coords.x;
				uv[1] = vertOffset + patchVertices->data.vertices[i].coords.y;
			}

			DrawingAPI::UnlockVertexBuffer(g_FVF_14C_Buffer_2.vertexBuffer);
		}
	}

	// FUNCTION: TOY2 0x004B7710
	void Vertices::ProjectForTex14(Nu3D::Patch::PatchVertices* patchVertices, D3DMATRIX* matrix)
	{
		LPVOID lockedData;

		if (g_FVF_14C_Buffer_2.vertexBuffer && ! DrawingAPI::LockVertexBuffer(g_FVF_14C_Buffer_2.vertexBuffer, 0x801, &lockedData, 0))
		{
			D3DMATRIX combined;
			Nu3D::Math::MultiplyMatrix3x4(&combined, matrix, Nu3D::Camera::GetProjectionMatrix());

			// the vertex normals are projected to get the uv's of the reflection map
			for (int32_t i = 0; i < patchVertices->vertexCount; i++)
			{
				Vector3F* normal = &patchVertices->data.vertices[i].normals;
				float* uv = TransformedVertexUV(lockedData, i);

				uv[0] = (normal->z * combined._31 + normal->y * combined._21 + normal->x * combined._11 + 1.0) * 0.5;
				uv[1] = (1.0 - (normal->z * combined._32 + normal->y * combined._22 + normal->x * combined._12)) * 0.5;

				TransformedVertexColor(lockedData, i)[3] = 96;
			}

			DrawingAPI::UnlockVertexBuffer(g_FVF_14C_Buffer_2.vertexBuffer);
		}
	}

	// FUNCTION: TOY2 0x004B6F50
	void Vertices::ProjectToScreen(Nu3D::Patch::PatchVertices* patchVertices, D3DMATRIX* matrix, int32_t headerCount)
	{
		LPVOID lockedData;

		if (g_FVF_14C_Buffer_2.vertexBuffer && ! DrawingAPI::LockVertexBuffer(g_FVF_14C_Buffer_2.vertexBuffer, 0x801, &lockedData, 0))
		{
			int32_t destIndex = 0;
			int32_t srcIndex = 0;
			int32_t verticesPerHeader = headerCount == 2 ? patchVertices->vertexCount / 2 : 1;

			for (int32_t header = 0; header < headerCount; header++, matrix++)
			{
				D3DMATRIX combined;
				Nu3D::Math::MultiplyMatrix3x4(&combined, matrix, Nu3D::Camera::GetProjectionMatrix());

				for (int32_t i = 0; i < verticesPerHeader; i++)
				{
					Vector3F* normal = &patchVertices->data.vertices[srcIndex + i].normals;
					float* uv = TransformedVertexUV(lockedData, destIndex + i);

					uv[0] = (normal->z * combined._31 + normal->y * combined._21 + normal->x * combined._11 + 1.0) * 0.5;
					uv[1] = (1.0 - (normal->z * combined._32 + normal->y * combined._22 + normal->x * combined._12)) * 0.5;

					TransformedVertexColor(lockedData, destIndex + i)[3] = 0x80;
				}

				if (headerCount == 2)
				{
					srcIndex += verticesPerHeader;
					destIndex = 32;
				}
				else
				{
					srcIndex += 1;
					destIndex += 1;
				}
			}

			DrawingAPI::UnlockVertexBuffer(g_FVF_14C_Buffer_2.vertexBuffer);
		}
	}

	// FUNCTION: TOY2 0x004B75E0
	void Vertices::ApplyMetadata100(Nu3D::Patch::PatchVertices* patchVertices, D3DMATRIX* matrix, Nu3D::InstanceData* instance)
	{
		LPVOID lockedData;

		if (g_FVF_14C_Buffer_2.vertexBuffer && ! DrawingAPI::LockVertexBuffer(g_FVF_14C_Buffer_2.vertexBuffer, 0x801, &lockedData, 0))
		{
			Metadata100Params* params = (Metadata100Params*)&instance->sprite;

			D3DMATRIX combined;
			Nu3D::Math::FullMatrixMultiply(&combined, matrix, &params->matrix);

			Vector3F reference;
			Nu3D::Math::TransformPointPerspective(&reference, &params->position, &combined);

			for (int32_t i = 0; i < patchVertices->vertexCount; i++)
			{
				Vector3F projected;
				float* uv = TransformedVertexUV(lockedData, i);

				Nu3D::Math::ProjectPoint(&projected, &patchVertices->data.vertices[i].position, &combined);

				uv[0] = (projected.x + 1.0) * 0.5;
				uv[1] = 1.0 - (projected.y + 1.0) * 0.5;

				Nu3D::Math::TransformPointPerspective(&projected, &patchVertices->data.vertices[i].normals, &combined);

				uv[0] = (reference.x - projected.x) * params->scale + uv[0];
				uv[1] = uv[1] - (reference.y - projected.y) * params->scale;
			}

			DrawingAPI::UnlockVertexBuffer(g_FVF_14C_Buffer_2.vertexBuffer);
		}
	}

	// FUNCTION: TOY2 0x004B70E0
	void Sprite::RenderType9(Nu3D::Material* material, RenderEntry* entry)
	{
		Nu3D::InstanceData* instanceData = entry->instanceData;

		SoftwareRenderer::g_unkE4D950 = instanceData->unkInt6;

		Nu3D::Primitive* primitive = entry->primitive;

		int32_t renderFlags = primitive->renderFlags | instanceData->renderFlags;
		int32_t drawType = primitive->header->drawType;

		// billboards are never culled
		if (drawType == 4 || drawType == 5)
			renderFlags |= 0x40;

		int32_t isTex14 = 0;

		if (material == Nu3D::g_tex14Version1 || material == Nu3D::g_tex14Version2 || material == Nu3D::g_tex14Version3)
			isTex14 = 1;

		if (! g_useSoftwareRasterizer || (instanceData->renderModeFlags & 1) != 0)
			SetupMaterialRenderState(material, renderFlags);

		LPDIRECT3DVERTEXBUFFER sourceBuffer = primitive->patchVerts.vertexBuffer;
		LPDIRECT3DVERTEXBUFFER destBuffer;

		// vertices that need modifying after they were transformed need to go to a buffer that can be locked
		if (instanceData->horzOffset != 0.0 || instanceData->vertOffset != 0.0 || (instanceData->renderModeFlags & 3) != 0 ||
			(material->metadata & 0x100) != 0 || isTex14 || drawType == 4 || drawType == 5 || g_useSoftwareRasterizer)
		{
			destBuffer = g_FVF_14C_Buffer_2.vertexBuffer;
		}
		else
		{
			destBuffer = g_FVF_14C_Buffer_1.vertexBuffer;
		}

		++g_type9DrawCallCount;
		g_type9VertexCount += primitive->patchVerts.vertexCount;

		if (sourceBuffer && destBuffer)
		{
			DWORD vertexOp = 1;

			if ((instanceData->renderModeFlags & 1) != 0)
				vertexOp = 1025;

			int32_t zFlag = renderFlags & 0x2000;

			if (zFlag == 0 && ! g_useSoftwareRasterizer)
				vertexOp |= 4;

			HRESULT result;

			if (drawType < 4 || drawType > 5)
			{
				DrawingDevice::SetWorldTransform(instanceData->matrices);

				result = DrawingAPI::ProcessVerticesOnBuffer(destBuffer, vertexOp, 0, primitive->patchVerts.vertexCount, sourceBuffer, 0, 0);
			}
			else
			{
				result = 0;
			}

			if ((material->metadata & 0x100) != 0)
				Vertices::ApplyMetadata100(&primitive->patchVerts, instanceData->matrices, instanceData);

			if (isTex14)
				Vertices::ProjectForTex14(&primitive->patchVerts, instanceData->matrices);

			if ((instanceData->renderModeFlags & 2) != 0)
			{
				Vertices::ModuleColor(
					&primitive->patchVerts, instanceData->vertexModColor.r, instanceData->vertexModColor.g, instanceData->vertexModColor.b);
			}

			if (g_instanceHorzOffset != 0.0 || g_instanceVertOffset != 0.0)
				Vertices::ApplyOffset(&primitive->patchVerts, g_instanceHorzOffset, g_instanceVertOffset);

			if (! result)
			{
				DWORD drawFlags = 8;

				if (zFlag || g_useSoftwareRasterizer)
					drawFlags = 12;

				SoftwareRenderer::g_viewportRect = &instanceData->clipRect;

				for (int32_t h = 0; h < primitive->headerCount; h++)
				{
					Nu3D::Primitive::Header* header = &primitive->header[h];

					if (g_drawWireframe)
					{
						for (int32_t i = 0; i < header->indexCount; i += 3)
							DrawingAPI::DrawIndexedPrimitiveVB(D3DPT_LINESTRIP, destBuffer, header->indices + i, 3, drawFlags);

						continue;
					}

					switch (header->drawType)
					{
						case 0:
							g_frameTriCount += header->indexCount / 3;

							if (g_useSoftwareRasterizer)
								SoftwareRenderer::SubmitTriangleList(renderFlags, destBuffer, entry, header->indices, header->indexCount);
							else
								DrawingAPI::DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST, destBuffer, header->indices, header->indexCount, drawFlags);

							break;

						case 2:
							g_frameTriCount += header->indexCount >> 1;

							if (g_useSoftwareRasterizer)
								SoftwareRenderer::SubmitTriangleStrip(renderFlags, destBuffer, entry, header->indices, header->indexCount);
							else
								DrawingAPI::DrawIndexedPrimitiveVB(D3DPT_TRIANGLESTRIP, destBuffer, header->indices, header->indexCount, drawFlags);

							break;

						case 3:
						{
							// a list of quads, each of them drawn as a 4 index triangle strip
							uint16_t* indices = header->indices;
							int32_t indexCount = header->indexCount;

							g_frameTriCount += indexCount >> 1;

							for (; indexCount; indexCount -= 4)
							{
								DrawingAPI::DrawIndexedPrimitiveVB(D3DPT_TRIANGLESTRIP, destBuffer, indices, 4, drawFlags);
								indices += 4;
							}

							break;
						}

						case 4:
						case 5:
						{
							// billboards, indices[0] is the vertex to rotate around, followed by the 4 vertices of the quad
							uint16_t* indices = header->indices;

							Vector3F position;
							Nu3D::Math::TransformPointByMatrix(&position, &primitive->patchVerts.data.vertices[*indices].position, instanceData->matrices);

							D3DMATRIX billboardMatrix;

							if (header->drawType == 4)
							{
								// only turns to face the camera horizontally
								Nu3D::Math::BuildIdentityMatrix(&billboardMatrix);
								Nu3D::Math::RotateYFromLut(&billboardMatrix, Nu3D::Camera::g_billboardYaw);
							}
							else
							{
								// fully faces the camera
								memcpy(&billboardMatrix, &Nu3D::Camera::g_secondaryCamera, sizeof(billboardMatrix));
							}

							billboardMatrix._41 = position.x;
							billboardMatrix._42 = position.y;
							billboardMatrix._43 = position.z;

							DrawingDevice::SetWorldTransform(&billboardMatrix);

							uint16_t* quadIndices = indices + 1;

							DrawingAPI::ProcessVerticesOnBuffer(destBuffer, vertexOp, *quadIndices, 4, sourceBuffer, *quadIndices, 0);

							if (g_useSoftwareRasterizer)
								SoftwareRenderer::SubmitTriangleStrip(renderFlags, destBuffer, entry, quadIndices, 4);
							else
								DrawingAPI::DrawIndexedPrimitiveVB(D3DPT_TRIANGLESTRIP, destBuffer, quadIndices, 4, drawFlags);

							break;
						}
					}
				}
			}
		}
	}

	// STUB: TOY2 0x004B6C10
	void Sprite::RenderType8(Nu3D::Material* material, RenderEntry* entry) {}

	// TODO: Fix, wrong spot
	// GLOBAL: TOY2 0x005087E4
	static WORD g_lineIndices[2] = { 0, 1 };

	// FUNCTION: TOY2 0x004B8330
	HRESULT Sprite::RenderType10(Nu3D::Sprite* sprite)
	{
		Nu3D::Vertex vertices[2];

		vertices[0].coords.x = 0.0;
		vertices[0].coords.y = 0.0;
		vertices[1].coords.x = 0.0;
		vertices[1].coords.y = 0.0;

		vertices[0].position = sprite->position;
		vertices[1].position = sprite->triVerts[0];

		vertices[0].diffuse = sprite->color;
		vertices[1].diffuse = sprite->color;

		D3DMATRIX identity;
		Nu3D::Math::BuildIdentityMatrix(&identity);
		DrawingDevice::SetWorldTransform(&identity);

		InitRenderState(sprite->renderFlags | 0x40);

		SoftwareRenderer::g_unkE4D950 = 5;
		BindTexture(0);

		SoftwareRenderer::g_viewportRect = &sprite->viewportRect;

		return DrawingAPI::DrawIndexedPrimitive(D3DPT_LINESTRIP, D3DFVF_0x152, vertices, 2, g_lineIndices, 2, 24);
	}

	// STUB: TOY2 0x004B7B30
	void Sprite::RenderQuadSprite(Nu3D::Sprite* sprite) {}

	// STUB: TOY2 0x004B7920
	void Sprite::RenderGroundAlignedSprite(Nu3D::Sprite* sprite) {}

	// STUB: TOY2 0x004B7D60
	void Sprite::RenderBillboardSprite(Nu3D::Sprite* sprite) {}

	// STUB: TOY2 0x004B7FC0
	void Sprite::RenderTriangleSprite(Nu3D::Sprite* sprite) {}

	// STUB: TOY2 0x004B8160
	void Sprite::RenderQuadSpriteFromVerts(Nu3D::Sprite* sprite) {}

	// FUNCTION: TOY2 0x004B6AD0
	void Sprite::DispatchCommand(Nu3D::Sprite* command)
	{
		// NOTE: the list can hold either Nu3D::Sprite's or RenderEntry's, they share the same start (next, type, distance)
		RenderEntry* entry = (RenderEntry*)command;

		while (entry)
		{
			Nu3D::Sprite* sprite = (Nu3D::Sprite*)entry;

			switch (entry->type)
			{
				case RENDER_QUADSPRITE:
					RenderQuadSprite(sprite);
					break;

				case RENDER_GROUND_ALIGNED_SPRITE:
					RenderGroundAlignedSprite(sprite);
					break;

				case RENDER_BILLBOARD_SPRITE:
					RenderBillboardSprite(sprite);
					break;

				case RENDER_2D_SPRITE:
					Render2DSprite(sprite);
					break;

				case RENDER_TRIANGLE_SPRITE:
					RenderTriangleSprite(sprite);
					break;

				case RENDER_QUAD_SPRITE_FROM_VERTS:
					RenderQuadSpriteFromVerts(sprite);
					break;

				case RENDER_TYPE6:
					RenderType8(entry->material, entry);
					break;

				case RENDER_TYPE7:
					RenderType9(entry->material, entry);
					break;

				case RENDER_TYPE8:
					if (! g_useSoftwareRasterizer || (entry->instanceData->renderModeFlags & 1) != 0)
						BindMaterial(entry->material, 1);

					RenderType8(entry->material, entry);
					break;

				case RENDER_TYPE9:
					if ((entry->instanceData->renderModeFlags & 1) != 0)
						BindMaterial(entry->material, 1);
					else if (! g_useSoftwareRasterizer)
						BindMaterial(entry->material, 0);

					RenderType9(entry->material, entry);
					break;

				case RENDER_TYPE10:
					RenderType10(sprite);
					break;
			}

			entry = entry->next;
		}
	}
}

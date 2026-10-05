#include "Nu3D/Material.h"
#include "Renderer/Renderer.h"
#include "DrawingDevice.h"
#include "NGNLoader/NGNLoader.h"
#include "NGNLoader/ObjectLoad.h"

namespace Nu3D
{
	// GLOBAL: TOY2 0x00A4CC98
	Material g_materialFreeList[3000];

	// GLOBAL: TOY2 0x00A4CC8C
	Material* g_materialFreeListHead;

	// GLOBAL: TOY2 0x009F5FE4
	Material* g_tex14Version1;

	// GLOBAL: TOY2 0x009F5FE8
	Material* g_tex14Version2;

	// GLOBAL: TOY2 0x009F5FEC
	Material* g_tex14Version3;

	// GLOBAL: TOY2 0x00AAD798
	int32_t g_unusedMaterialVar;

	// GLOBAL: TOY2 0x00A4CC88
	Material* g_materialActiveListHead;

	// FUNCTION: TOY2 0x004C2910
	void Material::Init()
	{
		memset(Renderer::g_boundTextureIndices, 255, sizeof(Renderer::g_boundTextureIndices));

		g_unusedMaterialVar = 0;
		g_materialActiveListHead = 0;
		Renderer::g_boundMaterial = 0;

		int32_t count = 1;
		Material* node = g_materialFreeList;

		do
		{
			node[1].prev = node;
			node->next = node + 1;
			node[1].id = count;

			node = node->next;
			++count;

		} while (node < &g_materialFreeList[2999]);

		g_materialFreeList[0].id = 0;
		g_materialFreeList[0].prev = 0;
		g_materialFreeListHead = g_materialFreeList;
		g_materialFreeList[2999].next = 0;

		DrawingDevice::BindTexToStage0(0);
	}

	// FUNCTION: TOY2 0x004C2150
	Material* Material::Allocate()
	{
		Material* result = g_materialFreeListHead;

		if (g_materialFreeListHead)
		{
			g_materialFreeListHead = g_materialFreeListHead->next;

			result->alwaysZero = 0;
			result->vertOffset = 0.0;
			result->horzOffset = 0.0;

			Material* activeHead = g_materialActiveListHead;

			result->prev = 0;
			result->next = activeHead;

			if (activeHead)
				activeHead->prev = result;

			g_materialActiveListHead = result;

			result->renderEntryHead = 0;
		}

		return result;
	}

	// FUNCTION: TOY2 0x004C28F0
	Material* Material::GetFreeByIndex(int32_t index) { return &g_materialFreeList[index]; }

	// FUNCTION: TOY2 0x004C28E0
	Material* Material::GetHead() { return g_materialActiveListHead; }

	// FUNCTION: TOY2 0x004C2750
	void Material::InsertSorted(Material* material)
	{
		Material* activeHead1 = g_materialActiveListHead;
		Material* activeHead2 = g_materialActiveListHead;

		if (g_materialActiveListHead)
		{
			while (activeHead1->texDataIndex < material->texDataIndex)
			{
				activeHead2 = activeHead1;
				activeHead1 = activeHead1->next;

				if (! activeHead1)
					goto LBL_ASSIGN_NEXT;
			}

			if (activeHead1)
			{
				Material* prev = activeHead1->prev;

				if (prev)
					prev->next = material;
				else
					g_materialActiveListHead = material;

				material->next = activeHead1;
				material->prev = activeHead1->prev;
				activeHead1->prev = material;

				return;
			}

		LBL_ASSIGN_NEXT:

			if (! activeHead2)
			{
				g_materialActiveListHead = material;
				material->prev = 0;
				material->next = 0;
				return;
			}

			activeHead2->next = material;
			material->prev = activeHead2;
			material->next = 0;
		}
		else
		{
			g_materialActiveListHead = material;
			material->prev = 0;
			material->next = 0;
		}
	}

	// FUNCTION: TOY2 0x004AC0F0
	ULONG Material::Release(LPDIRECT3DMATERIAL3 matHandle) { return matHandle->Release(); }

	// FUNCTION: TOY2 0x004C20F0
	void Material::ReleaseFromList(Material* material)
	{
		if (material->direct3DMat3)
			Material::Release(material->direct3DMat3);

		if (material->next)
			material->next->prev = material->prev;

		Material* prev = material->prev;

		if (prev)
			prev->next = material->next;
		else
			g_materialActiveListHead = material->next;

		material->next = g_materialFreeListHead;
		g_materialFreeListHead = material;
	}

	// FUNCTION: TOY2 0x004C23B0
	void Material::ConvertFileToMaterial(Material* material, MaterialFile* materialFile)
	{
		material->d3dMaterial.dwSize = sizeof(D3DMATERIAL);

		material->d3dMaterial.diffuse.r = materialFile->diffuseColor.r;
		material->d3dMaterial.diffuse.g = materialFile->diffuseColor.g;
		material->d3dMaterial.diffuse.b = materialFile->diffuseColor.b;
		material->d3dMaterial.diffuse.a = materialFile->opacity;

		material->d3dMaterial.ambient.r = materialFile->ambientColor.r;
		material->d3dMaterial.ambient.g = materialFile->ambientColor.g;
		material->d3dMaterial.ambient.b = materialFile->ambientColor.b;
		material->d3dMaterial.ambient.a = materialFile->opacity;

		material->d3dMaterial.specular.r = materialFile->specularColor.r;
		material->d3dMaterial.specular.g = materialFile->specularColor.g;
		material->d3dMaterial.specular.b = materialFile->specularColor.b;
		material->d3dMaterial.specular.a = materialFile->opacity;

		material->d3dMaterial.emissive.r = materialFile->emissiveIntensity;
		material->d3dMaterial.emissive.g = material->d3dMaterial.emissive.r * material->d3dMaterial.diffuse.g;
		material->d3dMaterial.emissive.b = material->d3dMaterial.emissive.r * material->d3dMaterial.diffuse.b;
		material->d3dMaterial.emissive.r = material->d3dMaterial.diffuse.r * material->d3dMaterial.emissive.r;

		material->d3dMaterial.emissive.a = materialFile->opacity;

		material->d3dMaterial.power = materialFile->power;
		material->d3dMaterial.hTexture = 0;
		material->d3dMaterial.dwRampSize = 16;

		material->opacity = materialFile->opacity;
		material->originalMetadata = materialFile->metadata;
	}

	// FUNCTION: TOY2 0x004C2450
	Material* Material::TryCache(MaterialFile* materialFile)
	{
		Material nu3dMaterial;
		ConvertFileToMaterial(&nu3dMaterial, materialFile);

		Material* material = g_materialActiveListHead;

		if (g_materialActiveListHead)
		{
			do
			{
				if (material->texDataIndex == materialFile->texDataIndex && material->originalMetadata == nu3dMaterial.originalMetadata
					&& material->opacity == nu3dMaterial.opacity && ! memcmp(&nu3dMaterial.d3dMaterial, &material->d3dMaterial, sizeof(D3DMATERIAL)))
				{
					break;
				}

				material = material->next;

			} while (material);
		}

		return material;
	}

	// FUNCTION: TOY2 0x004C2720
	void Material::Unlink(Material* material)
	{
		if (material == g_materialActiveListHead)
			g_materialActiveListHead = material->next;

		Material* prev = material->prev;

		if (prev)
			prev->next = material->next;

		if (material->next)
			material->next->prev = material->prev;
	}

	// FUNCTION: TOY2 0x004AC140
	HRESULT Material::SetMaterial(LPDIRECT3DMATERIAL3 direct3DMaterial3, LPD3DMATERIAL d3dMaterial) { return direct3DMaterial3->SetMaterial(d3dMaterial); }

	// FUNCTION: TOY2 0x004AC120
	HRESULT Material::GetHandle(LPDIRECT3DMATERIAL3 direct3DMaterial3, LPD3DMATERIALHANDLE d3dMaterialHandle)
	{
		LPDIRECT3DDEVICE3 device = DrawingDevice::g_drawingDevice->m_pd3dDevice;
		return direct3DMaterial3->GetHandle(device, d3dMaterialHandle);
	}
	// FUNCTION: TOY2 0x004C26D0
	void Material::AttachTexture(Material* material, uint32_t texDataIndex)
	{
		NGNLoader::NGNTextureData* textureData = NGNLoader::GetTextureDataByIndex(texDataIndex);

		int32_t metadata = material->metadata;

		material->texDataIndex = texDataIndex;

		int32_t flag = (4 * (textureData->isTex14 & 1)) | metadata;

		material->metadata = flag;
		material->metadata = flag | ((textureData->isTex14 & 2) != 0 ? 3 : 0);

		Unlink(material);
		InsertSorted(material);
	}

	// FUNCTION: TOY2 0x004C22E0
	Material* Material::CreateFromColor(RGBColor* p_rgbColor)
	{
		Material* material = Material::Allocate();

		if (material)
		{
			LPD3DMATERIAL d3dMat = &material->d3dMaterial;
			LPDIRECT3DMATERIAL3* direct3DMat3 = &material->direct3DMat3;

			d3dMat->dwSize = sizeof(D3DMATERIAL);

			material->d3dMaterial.diffuse.r = p_rgbColor->r;
			material->d3dMaterial.diffuse.g = p_rgbColor->g;
			material->d3dMaterial.diffuse.b = p_rgbColor->b;
			material->d3dMaterial.diffuse.a = 1.0f;

			material->d3dMaterial.ambient.r = 0.0f;
			material->d3dMaterial.ambient.g = 0.0f;
			material->d3dMaterial.ambient.b = 0.0f;
			material->d3dMaterial.ambient.a = 1.0f;

			material->d3dMaterial.specular.r = 0.0f;
			material->d3dMaterial.specular.g = 0.0f;
			material->d3dMaterial.specular.b = 0.0f;
			material->d3dMaterial.specular.a = 1.0f;

			material->d3dMaterial.emissive.g = 0.0f;
			material->d3dMaterial.emissive.b = 0.0f;
			material->d3dMaterial.emissive.r = 0.0f;
			material->d3dMaterial.emissive.a = 1.0f;

			material->d3dMaterial.power = 0.0f;
			material->d3dMaterial.hTexture = 0;
			material->d3dMaterial.dwRampSize = 16;

			if (DrawingDevice::CreateMaterial(direct3DMat3) < 0)
			{
				*direct3DMat3 = 0;
				ReleaseFromList(material);
				return 0;
			}

			SetMaterial(*direct3DMat3, d3dMat);
			GetHandle(*direct3DMat3, &material->d3dMaterialHandle);

			material->texDataIndex = 0;
			material->renderEntryHead = 0;
			material->originalMetadata = 0;
			material->metadata = 0;
			material->opacity = 1.0f;
		}

		return material;
	}

	// FUNCTION: TOY2 0x004C24D0
	Material* Material::CreateFromFile(MaterialFile* materialFile)
	{
		Material* cached = TryCache(materialFile);

		if (cached)
			return cached;

		Material* material = Allocate();

		if (material)
		{
			ConvertFileToMaterial(material, materialFile);

			LPDIRECT3DMATERIAL3* direct3DMat3 = &material->direct3DMat3;

			if (DrawingDevice::CreateMaterial(&material->direct3DMat3) < 0)
			{
				*direct3DMat3 = 0;
				ReleaseFromList(material);
				return 0;
			}

			SetMaterial(*direct3DMat3, &material->d3dMaterial);
			GetHandle(*direct3DMat3, &material->d3dMaterialHandle);

			material->texDataIndex = 0;
			material->renderEntryHead = 0;

			int32_t metadata = materialFile->metadata;

			material->metadata = metadata;
			material->opacity = materialFile->opacity;

			if (materialFile->opacity != 1.0f)
				material->metadata = metadata | 3;

			if (materialFile->texDataIndex)
				AttachTexture(material, materialFile->texDataIndex);
		}

		return material;
	}

	// FUNCTION: TOY2 0x004C2630
	void Material::SetOpacity(Material* material, float alpha)
	{
		if (material->opacity != alpha)
		{
			LPDIRECT3DMATERIAL3 direct3DMat3 = material->direct3DMat3;

			material->d3dMaterial.diffuse.a = alpha * material->d3dMaterial.diffuse.a;
			material->d3dMaterial.ambient.a = alpha * material->d3dMaterial.ambient.a;
			material->d3dMaterial.specular.a = alpha * material->d3dMaterial.specular.a;
			material->d3dMaterial.emissive.a = alpha * material->d3dMaterial.emissive.a;

			material->opacity = alpha * material->opacity;

			SetMaterial(direct3DMat3, &material->d3dMaterial);

			int32_t metadata = material->metadata;

			if (material->opacity != 1.0f || (metadata & 2) != 0)
			{
				metadata |= 1;
			}
			else
			{
				metadata &= ~1;
			}

			material->metadata = metadata;
			material->metadata |= material->originalMetadata;
		}
	}
}
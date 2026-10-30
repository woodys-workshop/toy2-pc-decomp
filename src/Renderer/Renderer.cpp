#include "Renderer/Renderer.h"
#include "SoftwareRenderer.h"
#include "DrawingDevice.h"
#include "Nu3D/Light.h"
#include "Nu3D/Font.h"
#include "Nu3D/Camera.h"
#include "Nu3D/BmpDataNode.h"
#include "Nu3D/Material.h"
#include "NGNLoader/NGNLoader.h"
#include "Nu3D/Patch.h"
#include "Renderer/Sprite.h"
#include "Toy2/Toy2.h"
#include "Renderer/Glue.h"
#include "Renderer/RenderQueue.h"
#include "Toy2/D3DApp.h"
#include "Renderer/ApplyFixes.h"
#include "Logger.h"
#include "Nullsub.h"
#include "Nu3D/Nu3D.h"

#include <WINDOWS.H>
#include <STDARG.H>
#include <STDIO.H>
#include <STRING.H>

namespace Renderer
{
	// GLOBAL: TOY2 0x00884484
	int32_t g_isSoftwareRendering;

	// GLOBAL: TOY2 0x0052F2D4
	uint32_t g_frameDelta;

	// GLOBAL: TOY2 0x00884488
	int32_t g_rendererValid;

	// GLOBAL: TOY2 0x00E4D964
	LPDIRECT3D3 g_drawDeviceD3D;

	// GLOBAL: TOY2 0x00E4D968
	LPDIRECT3DDEVICE3 g_drawDeviceD3DDevice;

	// GLOBAL: TOY2 0x0088448C
	int32_t g_fogEnabled;

	// GLOBAL: TOY2 0x00508514
	float g_fogStart = 1.0;

	// GLOBAL: TOY2 0x00508518
	float g_fogEnd = 100.0;

	// GLOBAL: TOY2 0x00884478
	DWORD g_fogColor;

	// GLOBAL: TOY2 0x00E4D95C
	int32_t g_deviceBlendShadeCaps;

	// GLOBAL: TOY2 0x00884AB8
	int32_t g_vertexColorModBlue;

	// GLOBAL: TOY2 0x00884ABC
	int32_t g_vertexColorModGreen;

	// GLOBAL: TOY2 0x0088C7C8
	int32_t g_vertexColorModRed;

	// GLOBAL: TOY2 0x0094FCD8
	int32_t g_additionalRenderFlags;

	// GLOBAL: TOY2 0x0094FCD4
	float g_lodFactor;

	// GLOBAL: TOY2 0x009F5FE0
	Nu3D::Material* g_whiteMaterial;

	// GLOBAL: TOY2 0x009F5FF8
	int32_t g_unk9F5FF8;

	// GLOBAL: TOY2 0x009F6000
	int32_t g_useVertexColorMod;

	// GLOBAL: TOY2 0x009F5FB0
	Nu3D::Patch::PatchVertices g_FVF_14C_Buffer_2;

	// GLOBAL: TOY2 0x009F5FC4
	Nu3D::Patch::PatchVertices g_FVF_14C_Buffer_1;

	// GLOBAL: TOY2 0x009F2F28
	Nu3D::Patch::PatchVertices g_FVF_152_Buffer;

	// GLOBAL: TOY2 0x00E4D920
	int32_t g_renderStateCache[8];

	// GLOBAL: TOY2 0x00E4D8F8
	int32_t g_maxSimultaneousTextures;

	// GLOBAL: TOY2 0x00508704
	int32_t g_maxSimultaneousTexturesMax = 1;

	// GLOBAL: TOY2 0x00508708
	int32_t g_srcBlendMode = 5;

	// GLOBAL: TOY2 0x0050870C
	int32_t g_destBlendMode = 2;

	// GLOBAL: TOY2 0x00508710
	int32_t g_alphaBlendSrc = 1;

	// GLOBAL: TOY2 0x00508714
	int32_t g_alphaBlendDest = 4;

	// GLOBAL: TOY2 0x00830C54
	int32_t g_lastFrameTimestamp;

	// GLOBAL: TOY2 0x00830C58
	int32_t g_frameStabilityCounter;

	// GLOBAL: TOY2 0x00500AA4
	int32_t g_maxSpeedMultiplier = 4;

	// GLOBAL: TOY2 0x00500AAC
	int32_t g_startupDelayFrames = 120;

	// GLOBAL: TOY2 0x00500AA8
	int32_t g_targetSpeedMultiplier = 1;

	// GLOBAL: TOY2 0x0050851C
	uint8_t g_gammaLUT[256] = {
		// clang-format off
		0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
		16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
		32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
		48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
		64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
		80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
		96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
		128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
		144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
		160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
		176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
		192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
		208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
		224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
		240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
		// clang-format on
	};

	// GLOBAL: TOY2 0x005084F4
	float g_gammaCorrection = 1.0;

	// GLOBAL: TOY2 0x005084F8
	int32_t g_gammaFixedPoint = 0x10000;

	// GLOBAL: TOY2 0x005087F4
	int32_t g_drawMaterialBuckets = 1;

	// GLOBAL: TOY2 0x005087F8
	int32_t g_drawTransparentBuckets = 1;

	// GLOBAL: TOY2 0x004F7414
	float g_virtualScreenWidth = 512.0;

	// GLOBAL: TOY2 0x004F7418
	float g_virtualScreenHeight = 256.0;

	// GLOBAL: TOY2 0x00A4CC90
	Nu3D::Material* g_boundMaterial;

	// GLOBAL: TOY2 0x00AAD778
	int32_t g_boundTextureIndices[8];

	// GLOBAL: TOY2 0x00500A54
	int32_t g_drawParallaxTexture = 1;

	// GLOBAL: TOY2 0x0072EF90
	float g_parallaxHorizOffset;

	// GLOBAL: TOY2 0x00830C50
	float g_parallaxCurHorizScroll;

	// GLOBAL: TOY2 0x00731CD0
	float g_parallaxTexHeightRatio;

	// GLOBAL: TOY2 0x00731CD4
	float g_parallaxTexWidthRatio;

	// GLOBAL: TOY2 0x00731D68
	RGBA g_parallaxTexFirstPixel;

	// GLOBAL: TOY2 0x00731CC8
	RGBA g_parallaxTexLastPixel;

	// GLOBAL: TOY2 0x00E4D96C
	int32_t g_deviceBlendShadeCapsCpy;

	// GLOBAL: TOY2 0x005088B0
	float g_nearRenderDistance = 144000000.0;

	// GLOBAL: TOY2 0x005088B4
	float g_farRenderDistance = 1000000.0;

	// GLOBAL: TOY2 0x00830C4C
	float g_parallaxPrevVertInput;

	// GLOBAL: TOY2 0x00731F24
	float g_parallaxScrollDelta;

	// GLOBAL: TOY2 0x004F73B0
	int32_t g_parallaxVertOffset = 32;

	// GLOBAL: TOY2 0x0050A1F8
	int32_t g_enableCinematicBars;

	// GLOBAL: TOY2 0x00559C60
	int32_t g_letterboxTimer;

	// GLOBAL: TOY2 0x00508218
	int32_t g_blitWithWrappingPath = 1;
}

namespace DrawingAPI
{
	// GLOBAL: TOY2 0x00508724
	Device_DrawIndexedPrimitive* DrawIndexedPrimitive = HardwareDevice::DrawIndexedPrimitive;

	// GLOBAL: TOY2 0x00508720
	Device_DrawIndexedPrimitiveVB* DrawIndexedPrimitiveVB = HardwareDevice::DrawIndexedPrimitiveVB;

	// GLOBAL: TOY2 0x005084FC
	Device_ReleaseVertexBuffer* ReleaseVertexBuffer = HardwareDevice::ReleaseVertexBuffer;

	// GLOBAL: TOY2 0x00508500
	Device_CreateVertexBuffer* CreateVertexBuffer = HardwareDevice::CreateVertexBuffer;

	// GLOBAL: TOY2 0x00508504
	Device_LockVertexBuffer* LockVertexBuffer = HardwareDevice::LockVertexBuffer;

	// GLOBAL: TOY2 0x00508508
	Device_UnlockVertexBuffer* UnlockVertexBuffer = HardwareDevice::UnlockVertexBuffer;

	// GLOBAL: TOY2 0x0050850C
	Device_OptimizeVertexBuffer* OptimizeVertexBuffer = HardwareDevice::OptimizeVertexBuffer;

	// GLOBAL: TOY2 0x00508510
	Device_ProcessVerticesOnBuffer* ProcessVerticesOnBuffer = HardwareDevice::ProcessVerticesOnBuffer;

	// FUNCTION: TOY2 0x004B2BF0
	void SetVertexAPIs(int32_t isSoftwareRendering)
	{
		if (isSoftwareRendering)
		{
			ReleaseVertexBuffer = SoftwareDevice::ReleaseVertexBuffer;
			CreateVertexBuffer = SoftwareDevice::CreateVertexBuffer;
			LockVertexBuffer = SoftwareDevice::LockVertexBuffer;
			UnlockVertexBuffer = SoftwareDevice::UnlockVertexBuffer;
			OptimizeVertexBuffer = SoftwareDevice::OptimizeVertexBuffer;
			ProcessVerticesOnBuffer = SoftwareDevice::ProcessVerticesOnBuffer;
		}
		else
		{
			ReleaseVertexBuffer = HardwareDevice::ReleaseVertexBuffer;
			CreateVertexBuffer = HardwareDevice::CreateVertexBuffer;
			LockVertexBuffer = HardwareDevice::LockVertexBuffer;
			UnlockVertexBuffer = HardwareDevice::UnlockVertexBuffer;
			OptimizeVertexBuffer = HardwareDevice::OptimizeVertexBuffer;
			ProcessVerticesOnBuffer = HardwareDevice::ProcessVerticesOnBuffer;
		}
	}
}

namespace Renderer
{
	// FUNCTION: TOY2 0x004B2CE0
	void DisableFog() { g_fogEnabled = 0; }

	// FUNCTION: TOY2 0x004B6320
	void SetRenderState(int32_t newStateFlags)
	{
		if (g_isSoftwareRendering)
		{
			g_renderStateCache[0] = newStateFlags;
		}
		else
		{
			uint32_t flagsToDisable = g_renderStateCache[0] & (newStateFlags ^ g_renderStateCache[0]);

			if (flagsToDisable)
			{
				if ((flagsToDisable & RENDER_ALPHA_DEFAULT) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

				if ((flagsToDisable & RENDER_ALPHA_CUSTOM) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

				if ((flagsToDisable & RENDER_ALPHA_ALT) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

				if ((flagsToDisable & RENDER_ALPHA_TEX_MODULATE) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

				if ((flagsToDisable & RENDER_ALPHA_TEX_MOD_CUSTOM) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

				if ((flagsToDisable & RENDER_ALPHA_TEX_MOD_ALT) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);

				if ((flagsToDisable & RENDER_Z) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ZENABLE, 1);

				if ((flagsToDisable & RENDER_DISABLE_PERSPECTIVE) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1);

				if ((flagsToDisable & RENDER_BILINEAR_FILTER) != 0)
				{
					DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREMAG, 1);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREMIN, 1);
				}

				if ((flagsToDisable & RENDER_ZWRITE) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1);

				if ((flagsToDisable & RENDER_COLOR_VERTEX) != 0)
					DrawingDevice::SetLightState(D3DLIGHTSTATE_COLORVERTEX, 0);

				if ((flagsToDisable & RENDER_ZBIAS_1) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ZBIAS, 0);

				if ((flagsToDisable & RENDER_ZBIAS_2) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ZBIAS, 0);
			}

			uint32_t flagsToEnable = newStateFlags & (newStateFlags ^ g_renderStateCache[0]);

			if (flagsToEnable)
			{
				if ((flagsToEnable & RENDER_Z) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ZENABLE, 0);

				if ((flagsToEnable & RENDER_ZWRITE) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 0);

				if ((flagsToEnable & RENDER_DISABLE_PERSPECTIVE) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 0);

				if ((flagsToEnable & RENDER_CULL_BACK) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_CULLMODE, 2);

				if ((flagsToEnable & RENDER_CULL_FRONT) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_CULLMODE, 3);

				if ((flagsToEnable & RENDER_CULL_NONE) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_CULLMODE, 1);

				if ((flagsToEnable & RENDER_ALPHA_DEFAULT) != 0)
				{
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_SRCBLEND, 5);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_DESTBLEND, 6);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, 4);
				}

				if ((flagsToEnable & RENDER_ALPHA_CUSTOM) != 0)
				{
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_SRCBLEND, g_srcBlendMode);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_DESTBLEND, g_destBlendMode);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, 4);
				}

				if ((flagsToEnable & RENDER_ALPHA_ALT) != 0)
				{
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_SRCBLEND, g_alphaBlendSrc);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_DESTBLEND, g_alphaBlendDest);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, 4);
				}

				if ((flagsToEnable & RENDER_ALPHA_TEX_MODULATE) != 0)
				{
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_SRCBLEND, 5);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_DESTBLEND, 6);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, 3);
				}

				if ((flagsToEnable & RENDER_ALPHA_TEX_MOD_CUSTOM) != 0)
				{
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_SRCBLEND, g_srcBlendMode);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_DESTBLEND, g_destBlendMode);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, 3);
				}

				if ((flagsToEnable & RENDER_ALPHA_TEX_MOD_ALT) != 0)
				{
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_SRCBLEND, g_alphaBlendSrc);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_DESTBLEND, g_alphaBlendDest);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, 3);
				}

				if ((flagsToEnable & RENDER_BILINEAR_FILTER) != 0)
				{
					DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREMAG, 2);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREMIN, 2);
				}

				if ((flagsToEnable & RENDER_COLOR_VERTEX) != 0)
				{
					DrawingDevice::SetLightState(D3DLIGHTSTATE_COLORVERTEX, 1);
					DrawingDevice::SetRenderState(D3DRENDERSTATE_SHADEMODE, 2);
				}

				if ((flagsToEnable & RENDER_ZBIAS_1) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ZBIAS, 1);

				if ((flagsToEnable & RENDER_ZBIAS_2) != 0)
					DrawingDevice::SetRenderState(D3DRENDERSTATE_ZBIAS, 2);
			}

			g_renderStateCache[0] = newStateFlags;
		}
	}

	// FUNCTION: TOY2 0x004B6660
	void SetTextureStageState(int32_t newState, DWORD textureStage)
	{
		uint32_t stateFlagsToDisable = g_renderStateCache[textureStage + 1] & (newState ^ g_renderStateCache[textureStage + 1]);

		if (stateFlagsToDisable)
		{
			if ((stateFlagsToDisable & (RENDER_TEXTURE_WRAP_UV | RENDER_TEXTURE_CLAMP_U)) != 0)
				DrawingDevice::SetTextureStageState(textureStage, D3DTSS_ADDRESS, 1);

			if ((stateFlagsToDisable & (RENDER_COLOR_MODULATE | RENDER_COLOR_BLEND_FACTOR)) != 0)
				DrawingDevice::SetTextureStageState(textureStage, D3DTSS_COLOROP, 1);
		}

		uint32_t flagsToEnable = newState & (newState ^ g_renderStateCache[textureStage + 1]);

		if (flagsToEnable)
		{
			if ((flagsToEnable & RENDER_TEXTURE_WRAP_UV) != 0)
				DrawingDevice::SetTextureStageState(textureStage, D3DTSS_ADDRESS, 3);

			if ((flagsToEnable & RENDER_TEXTURE_CLAMP_U) != 0)
			{
				DrawingDevice::SetTextureStageState(textureStage, D3DTSS_ADDRESS, 2);
				DrawingDevice::SetTextureStageState(textureStage, D3DTSS_ADDRESS, 4);
			}

			if ((flagsToEnable & RENDER_COLOR_MODULATE) != 0)
			{
				DrawingDevice::SetTextureStageState(textureStage, D3DTSS_COLOROP, 4);
				DrawingDevice::SetTextureStageState(textureStage, D3DTSS_COLORARG1, 2);
				DrawingDevice::SetTextureStageState(textureStage, D3DTSS_COLORARG2, 1);
			}

			if ((flagsToEnable & RENDER_COLOR_BLEND_FACTOR) != 0)
			{
				DrawingDevice::SetRenderState(D3DRENDERSTATE_TEXTUREFACTOR, 0x80FFFFFF);
				DrawingDevice::SetTextureStageState(textureStage, D3DTSS_COLOROP, 14);
				DrawingDevice::SetTextureStageState(textureStage, D3DTSS_COLORARG1, 2);
				DrawingDevice::SetTextureStageState(textureStage, D3DTSS_COLORARG2, 1);
			}
		}

		g_renderStateCache[textureStage + 1] = newState;
	}

	// FUNCTION: TOY2 0x004B6850
	void InitRenderState(int32_t newStage)
	{
		SetRenderState(newStage | RENDER_COLOR_MODULATE);
		SetTextureStageState(newStage | RENDER_COLOR_MODULATE, 0);

		for (int32_t texture = 1; texture < g_maxSimultaneousTextures; ++texture)
			SetTextureStageState(0, texture);
	}

	// FUNCTION: TOY2 0x004B9710
	void InitResources()
	{
		g_vertexColorModBlue = 4096;
		g_vertexColorModGreen = 4096;
		g_vertexColorModRed = 4096;
		g_additionalRenderFlags = 0;
		g_lodFactor = 1.0;

		RGBColor whiteColor;
		whiteColor.b = 1.0;
		whiteColor.g = 1.0;
		whiteColor.r = 1.0;

		g_whiteMaterial = Nu3D::Material::CreateFromColor(&whiteColor);

		g_unk9F5FF8 = 0;
		g_useVertexColorMod = 0;

		SoftwareRenderer::g_viewportRect = 0;

		Set508718(5);

		g_FVF_14C_Buffer_2.format = D3DFVF_0x1C4;
		g_FVF_14C_Buffer_2.vertexCount = 1000;
		g_FVF_14C_Buffer_2.vertexBuffer = 0;
		g_FVF_14C_Buffer_2.data.verticesTL = (Nu3D::VertexTL*)malloc(sizeof(Nu3D::VertexTL) * 1000);

		Nu3D::Patch::PatchVertices::CreateVertexBuffer(&g_FVF_14C_Buffer_2, 12);

		g_FVF_14C_Buffer_1.format = D3DFVF_0x1C4;
		g_FVF_14C_Buffer_1.vertexCount = 1000;
		g_FVF_14C_Buffer_1.vertexBuffer = 0;
		g_FVF_14C_Buffer_1.data.verticesTL = (Nu3D::VertexTL*)malloc(sizeof(Nu3D::VertexTL) * 1000);

		Nu3D::Patch::PatchVertices::CreateVertexBuffer(&g_FVF_14C_Buffer_1, 2);

		g_FVF_152_Buffer.format = D3DFVF_0x152;
		g_FVF_152_Buffer.vertexCount = 1000;
		g_FVF_152_Buffer.vertexBuffer = 0;
		g_FVF_152_Buffer.data.vertices = (Nu3D::Vertex*)malloc(sizeof(Nu3D::Vertex) * 1000);

		Nu3D::Patch::PatchVertices::CreateVertexBuffer(&g_FVF_152_Buffer, 14);

		D3DDEVICEDESC outSurfaceDesc;
		memcpy(&outSurfaceDesc, DrawingDevice::CopySurfaceDesc(&outSurfaceDesc), sizeof(outSurfaceDesc));

		memset(g_renderStateCache, 0, sizeof(g_renderStateCache));

		g_maxSimultaneousTextures = outSurfaceDesc.wMaxSimultaneousTextures;

		if (outSurfaceDesc.wMaxSimultaneousTextures >= g_maxSimultaneousTexturesMax)
			g_maxSimultaneousTextures = g_maxSimultaneousTexturesMax;

		if ((outSurfaceDesc.dpcTriCaps.dwShadeCaps & 0x4000) != 0)
		{
			if ((outSurfaceDesc.dpcTriCaps.dwDestBlendCaps & 2) == 0)
				g_destBlendMode = 6;

			if ((outSurfaceDesc.dpcTriCaps.dwDestBlendCaps & 8) == 0)
			{
				g_alphaBlendSrc = 5;
				g_alphaBlendDest = 6;
			}
		}
		else
		{
			g_srcBlendMode = 2;
			g_destBlendMode = 2;

			if ((outSurfaceDesc.dpcTriCaps.dwDestBlendCaps & 8) == 0)
			{
				g_alphaBlendSrc = 5;
				g_alphaBlendDest = 6;
			}
		}

		InitRenderState(3078);
		InitRenderState(4752);

		DrawingAPI::SetVertexAPIs(g_isSoftwareRendering);

		if (g_isSoftwareRendering == 1)
		{
			DrawingAPI::DrawIndexedPrimitiveVB = SoftwareDevice::DrawIndexedPrimitiveVB;
			DrawingAPI::DrawIndexedPrimitive = SoftwareDevice::DrawIndexedPrimitive;
		}
		else
		{
			DrawingAPI::DrawIndexedPrimitiveVB = HardwareDevice::DrawIndexedPrimitiveVB;
			DrawingAPI::DrawIndexedPrimitive = HardwareDevice::DrawIndexedPrimitive;
		}

		ResetRenderPools();
		Renderer::Sprite::ResetQueue();

		SoftwareRenderer::InitialisePrimarySurface();

		DECOMP_PRINT(("Finished Renderer::InitResources\n"));
	}

	// FUNCTION: TOY2 0x004B37F0
	void Cleanup()
	{
		for (Nu3D::Patch* patch = Nu3D::g_patchListHead; Nu3D::g_patchListHead; patch = Nu3D::g_patchListHead)
		{
			patch->listNext = 0;
			Nu3D::Patch::PatchVertices::Destroy(Nu3D::g_patchListHead);
		}

		for (Nu3D::Primitive* prim = Nu3D::g_primListHead; Nu3D::g_primListHead; prim = Nu3D::g_primListHead)
		{
			prim->listNext = 0;
			Nu3D::Primitive::Destroy(Nu3D::g_primListHead);
		}

		Nullsub8();

		NGNLoader::ReleaseAllTextures();
		Nu3D::Light::DestroyAllLights();

		Renderer::g_rendererValid = 0;
	}

	// FUNCTION: TOY2 0x004B3630
	void Init()
	{
		if (g_rendererValid)
			Cleanup();

		g_drawDeviceD3D = DrawingDevice::GetD3D();
		g_drawDeviceD3DDevice = DrawingDevice::GetD3DDevice();

		Nu3D::g_primListHead = 0;
		Nu3D::g_patchListHead = 0;

		Nu3D::SetIsSoftwareRendering(g_isSoftwareRendering);
		Nu3D::SetMinTexSize(g_isSoftwareRendering != 0 ? 256 : 0);

		Nu3D::Viewport::Init();
		Nu3D::Light::InitPool(16);
		NGNLoader::Init();
		Nu3D::Material::Init();

		InitResources();
		g_rendererValid = 1;
		Nu3D::Font::BuildFontTextures();
		DisableFog();

		g_deviceBlendShadeCaps = 0;

		D3DDEVICEDESC outSurfaceDesc;
		memcpy(&outSurfaceDesc, DrawingDevice::CopySurfaceDesc(&outSurfaceDesc), sizeof(outSurfaceDesc));

		if ((outSurfaceDesc.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) != 0)
			g_deviceBlendShadeCaps |= 1;

		if ((outSurfaceDesc.dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_ONE) != 0)
			g_deviceBlendShadeCaps |= 2;

		if ((outSurfaceDesc.dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_INVSRCCOLOR) != 0)
			g_deviceBlendShadeCaps |= 4;

		if ((outSurfaceDesc.dpcTriCaps.dwTextureBlendCaps & D3DPTEXTURECAPS_TRANSPARENCY) != 0)
			g_deviceBlendShadeCaps |= 8;
	}

	// FUNCTION: TOY2 0x004B3860
	void SetIsSoftwareRendering(int32_t value) { g_isSoftwareRendering = value; }

	// FUNCTION: TOY2 0x00453CD0
	void SetVirtualRatioTo54()
	{
		g_virtualScreenWidth = 320.0;
		g_virtualScreenHeight = 256.0;
	}

	// FUNCTION: TOY2 0x00490860 [MATCHED]
	void DoFrameDelay(int32_t isGameplayFrame)
	{
#ifdef APPLY_FIXES
		ApplyFixes::LockFrameRate();
		return;
#endif
		g_lastFrameTimestamp = Nu3D::GetHighResolutionTime();

		// This method does the FPS delay for each frame, it is the source of all pain
		int32_t hrt = Nu3D::GetHighResolutionTime();
		int32_t elapsedMs = hrt - g_lastFrameTimestamp;

		if (Toy2::g_demoMode)
		{
			g_frameDelta = 2;

			if (elapsedMs < 33)
			{
				Nu3D::PrecisionSleep(33 - elapsedMs);
				g_lastFrameTimestamp = Nu3D::GetHighResolutionTime();
				return;
			}

			goto LBL_UPDATE_TIMESTAMP;
		}

		{
			int32_t calculatedMultiplier = 60 * elapsedMs / 1000;
			g_frameDelta = calculatedMultiplier;

			if (1000 * calculatedMultiplier / 60 != elapsedMs)
				g_frameDelta = ++calculatedMultiplier;

			int32_t minMultiplier;

			if (calculatedMultiplier < 1)
				minMultiplier = 1;
			else
				minMultiplier = calculatedMultiplier;

			if (g_maxSpeedMultiplier < minMultiplier)
			{
				calculatedMultiplier = g_maxSpeedMultiplier;
			}
			else
			{
				if (calculatedMultiplier >= 1)
					goto LBL_STARTUP_DELAY;

				calculatedMultiplier = 1;
			}

			g_frameDelta = calculatedMultiplier;

		LBL_STARTUP_DELAY:

			int32_t delayFrames = g_startupDelayFrames;
			if (g_startupDelayFrames > 0)
			{
				delayFrames = g_startupDelayFrames - calculatedMultiplier;
				g_startupDelayFrames -= calculatedMultiplier;
			}

			if (isGameplayFrame)
			{
				if (delayFrames > 0)
					goto LBL_WAIT_NEXT_FRAME;

				int32_t stabilityCounter = g_frameStabilityCounter;
				int32_t targetSpeedMultiplier = g_targetSpeedMultiplier;

				if (g_frameStabilityCounter > 0)
				{
					stabilityCounter = g_frameStabilityCounter - targetSpeedMultiplier;
					g_frameStabilityCounter -= targetSpeedMultiplier;
				}

				if (calculatedMultiplier < targetSpeedMultiplier)
				{
					if (stabilityCounter > 0)
					{
						calculatedMultiplier = targetSpeedMultiplier;
						targetSpeedMultiplier = calculatedMultiplier;
						g_frameDelta = calculatedMultiplier;
					}
				}
				else
				{
					g_frameStabilityCounter = 60;
				}

				g_targetSpeedMultiplier = calculatedMultiplier;
			}
			else
			{
				g_targetSpeedMultiplier = 1;
				g_frameStabilityCounter = 0;
				g_startupDelayFrames = 120;
			}

		LBL_WAIT_NEXT_FRAME:

			int32_t sleepTimeMs = 1000 * calculatedMultiplier / 60 - elapsedMs;

			if (sleepTimeMs > 0)
				Nu3D::PrecisionSleep(sleepTimeMs);
		}

	LBL_UPDATE_TIMESTAMP:

		g_lastFrameTimestamp = Nu3D::GetHighResolutionTime();
	}

	// FUNCTION: TOY2 0x004C2080
	int32_t ConvertRGBATo16Bit(RGBA color) { return 0; }

	// FUNCTION: TOY2 0x004B37B0 [MATCHED]
	RGBA ApplyGammaCorrection(RGBA color)
	{
		color.r = g_gammaLUT[color.r];
		color.g = g_gammaLUT[color.g];
		color.b = g_gammaLUT[color.b];

		return color;
	}

	// FUNCTION: TOY2 0x004B2C80 [MATCHED]
	void ClearScreen(RGBA clearColor, int32_t clearFlags)
	{
		RGBA color = ApplyGammaCorrection(clearColor);

		if (! g_isSoftwareRendering)
		{
			DrawingDevice::ClearScreen(clearFlags, color.value);
			return;
		}

		uint32_t color16 = Renderer::ConvertRGBATo16Bit(color) & 0xFFFF;
		SoftwareRenderer::g_softwareClearColor = color16 | (color16 << 16);
	}

	// FUNCTION: TOY2 0x004B2D80
	void ApplyFogSettings()
	{
		if (g_fogEnabled)
		{
			DrawingDevice::SetRenderState(D3DRENDERSTATE_FOGENABLE, 1);
			DrawingDevice::SetRenderState(D3DRENDERSTATE_FOGCOLOR, g_fogColor & 0xFFFFFF);

			DrawingDevice::SetLightState(D3DLIGHTSTATE_FOGMODE, 3);
			// TODO: Double check this
			// D3D wants the bits of the floats here, not their integer value
			DrawingDevice::SetLightState(D3DLIGHTSTATE_FOGSTART, *(DWORD*)&g_fogStart);
			DrawingDevice::SetLightState(D3DLIGHTSTATE_FOGEND, *(DWORD*)&g_fogEnd);
		}
		else
		{
			DrawingDevice::SetRenderState(D3DRENDERSTATE_FOGENABLE, 0);
		}
	}

	// FUNCTION: TOY2 0x004B2D50
	int32_t BeginScene()
	{
		if (! DrawingDevice::BeginScene())
		{
			Renderer::ApplyFogSettings();
			Nu3D::Viewport::Reset();
			Nu3D::Viewport::SetViewClipRect();

			if (g_isSoftwareRendering)
				SoftwareRenderer::UnkFunc32();

			return 1;
		}

		return 0;
	}

	// FUNCTION: TOY2 0x004B2DE0
	void EndScene(int32_t presentFrame)
	{
		DrawingDevice::EndScene();

#ifdef APPLY_FIXES
		if (presentFrame && ! g_isSoftwareRendering)
			ApplyFixes::DrawVersionStamp();
#endif

		if (presentFrame)
		{
			if (g_isSoftwareRendering)
			{
				SoftwareRenderer::PresentFrame();
			}
			else if (DrawingDevice::PresentFrame() == DDERR_SURFACELOST)
			{
				DrawingDevice::CD3DFramework* device = DrawingDevice::g_drawingDevice;

				LPDIRECTDRAWSURFACE4 frontBuffer = DrawingDevice::g_drawingDevice->m_pddsFrontBuffer;

				if (frontBuffer && frontBuffer->IsLost())
					device->m_pddsFrontBuffer->Restore();

				LPDIRECTDRAWSURFACE4 backBuffer = device->m_pddsBackBuffer;

				if (backBuffer && backBuffer->IsLost())
					device->m_pddsBackBuffer->Restore();

				LPDIRECTDRAWSURFACE4 zBuffer = device->m_pddsZBuffer;

				if (zBuffer)
				{
					if (zBuffer->IsLost())
						device->m_pddsZBuffer->Restore();
				}
			}
		}
	}

	// FUNCTION: TOY2 0x0049B580
	void DrawMainMenuText(int16_t yPos, char* text, int32_t fadeAlpha)
	{
		char* charPtr = text;
		int32_t strLength = 0;

		if (*text)
			while (text[++strLength]) {};

		int16_t xPos = 160 - 6 * strLength;

		if (strLength > 0)
		{
			int32_t remaining = strLength;

			do
			{
				char currentChar = *charPtr++;

				if (currentChar != ' ')
				{
					uint8_t tileIndex;

					if (currentChar == '\'')
						tileIndex = 47;
					else
						tileIndex = currentChar - 97;

					if (fadeAlpha == 128)
						Sprite::DrawScaled(xPos, yPos, 50, tileIndex, 128, 128, 128, 255, 2048, 2048);
					else
						Sprite::DrawScaled(xPos, yPos, 50, tileIndex, 255, 255, 255, (fadeAlpha << 9) + 96, 2048, 2048);
				}

				xPos += 12;
				--remaining;

			} while (remaining);
		}
	}

	// FUNCTION: TOY2 0x0044DD80
	void DrawTintOverlay()
	{
		Vector2F uvTopLeft;
		Vector2F uvBottomRight;
		bool greenAtOrBelowMid;

		if (Nu3D::Camera::g_cameraTintBlue != 128)
		{
			if (Nu3D::Camera::g_cameraTintBlue > 128)
				goto LBL_BRIGHTEN;

			greenAtOrBelowMid = Nu3D::Camera::g_cameraTintGreen <= 128;

			if (greenAtOrBelowMid)
				goto LBL_DARKEN;

		LBL_BRIGHTEN:

			int32_t brightenGreen = 2 * Nu3D::Camera::g_cameraTintGreen - 256;
			int32_t brightenBlue = 2 * Nu3D::Camera::g_cameraTintBlue - 256;
			int32_t brightenRed = 2 * Nu3D::Camera::g_cameraTintRed - 256;

			if (brightenBlue <= 255)
			{
				if (brightenBlue < 0)
					brightenBlue = 0;
			}
			else
			{
				brightenBlue = -1;
			}

			if (brightenGreen <= 255)
			{
				if (brightenGreen < 0)
					brightenGreen = 0;
			}
			else
			{
				brightenGreen = -1;
			}

			if (brightenRed <= 255)
			{
				if (brightenRed < 0)
					brightenRed = 0;
			}
			else
			{
				brightenRed = -1;
			}

			RGBA brightenColor;
			brightenColor.b = brightenBlue;
			brightenColor.g = brightenGreen;
			brightenColor.a = -1 - brightenGreen;
			brightenColor.r = brightenRed;

			uvTopLeft.x = 0.0;
			uvTopLeft.y = 0.0;

			uvBottomRight.x = 1.0;
			uvBottomRight.y = 1.0;

			Sprite::Queue2DSprite(0.0, 0.0, 1.0, 1.0, &uvTopLeft, &uvBottomRight, 0, brightenColor, RENDER_PRESET_COLOR_OVERLAY);

			if (D3DApp::g_renderMode == RENDERMODE_SOFTWARE && SoftwareRenderer::g_bitsPerPixel == 8)
				SoftwareRenderer::UnkFunc7();

			return;
		}

		greenAtOrBelowMid = Nu3D::Camera::g_cameraTintGreen <= 128;

		if (Nu3D::Camera::g_cameraTintGreen != 128)
		{
			if (greenAtOrBelowMid)
				goto LBL_DARKEN;

			goto LBL_BRIGHTEN;
		}

		if (Nu3D::Camera::g_cameraTintRed == 128)
			return;

	LBL_DARKEN:

		if (Nu3D::Camera::g_cameraTintRed > 128)
			goto LBL_BRIGHTEN;

		int32_t darkenBlue = 2 * (128 - Nu3D::Camera::g_cameraTintBlue);
		int32_t darkenGreen = 2 * (128 - Nu3D::Camera::g_cameraTintGreen);
		int32_t darkenRed = 2 * (128 - Nu3D::Camera::g_cameraTintRed);

		if (darkenBlue <= 255)
		{
			if (((128 - Nu3D::Camera::g_cameraTintBlue) & 0x40000000) != 0)
				darkenBlue = 0;
		}
		else
		{
			darkenBlue = -1;
		}

		if (darkenGreen <= 255)
		{
			if (((128 - Nu3D::Camera::g_cameraTintGreen) & 0x40000000) != 0)
				darkenGreen = 0;
		}
		else
		{
			darkenGreen = -1;
		}

		if (darkenRed <= 255)
		{
			if (((128 - Nu3D::Camera::g_cameraTintRed) & 0x40000000) != 0)
				darkenRed = 0;
		}
		else
		{
			darkenRed = -1;
		}

		uvTopLeft.x = 0.0;
		uvTopLeft.y = 0.0;

		uvBottomRight.x = 1.0;
		uvBottomRight.y = 1.0;

		RGBA darkenColor;
		darkenColor.a = -1 - darkenGreen;
		darkenColor.b = darkenBlue;
		darkenColor.g = darkenGreen;
		darkenColor.r = darkenRed;

		int32_t texDataIndex;

		if (darkenBlue == darkenGreen && darkenBlue == darkenRed)
		{
			texDataIndex = NGNLoader::GetTextureDataIndex(14);

			if (! texDataIndex)
			{
				texDataIndex = NGNLoader::GetTextureDataIndex(36);

				if (! texDataIndex)
					texDataIndex = NGNLoader::GetTextureDataIndex(37);
			}
		}
		else
		{
			texDataIndex = 0;
		}

		Sprite::Queue2DSprite(0.0, 0.0, 1.0, 1.0, &uvTopLeft, &uvBottomRight, texDataIndex, darkenColor, RENDER_PRESET_FADE_OVERLAY);

		if (D3DApp::g_renderMode == RENDERMODE_SOFTWARE && SoftwareRenderer::g_bitsPerPixel == 8)
			SoftwareRenderer::UnkFunc7();
	}

	// FUNCTION: TOY2 0x0048F3E0
	void ResetParallax()
	{
		g_parallaxCurHorizScroll = 0.0;
		g_parallaxHorizOffset = 0.0;
		g_parallaxTexHeightRatio = 1.0;
		g_parallaxTexWidthRatio = 1.0;
	}

	// FUNCTION: TOY2 0x004B3870
	int32_t GetIsSoftwareRendering() { return g_isSoftwareRendering; }

	// FUNCTION: TOY2 0x004B3740 [MATCHED]
	float BuildGammaCorrectionLUT(float gammaCorrection)
	{
		float result = g_gammaCorrection;

		g_gammaCorrection = gammaCorrection;
		g_gammaFixedPoint = (gammaCorrection * 65536.0f);

		int32_t curGammaIdx = 0;
		int32_t count = 0;
		int32_t step = g_gammaFixedPoint;

		do
		{
			int32_t outputValue;

			if ((int32_t)(count & 0xFFFF0000) > 0xFF0000)
				outputValue = 0xFF;
			else
				outputValue = count >> 16;

			g_gammaLUT[curGammaIdx++] = outputValue;
			count += step;

		} while (curGammaIdx < 256);

		return result;
	}

	// FUNCTION: TOY2 0x004B2CC0
	void GetBlendShadeCaps(int32_t* capsOut)
	{
		if (capsOut)
			*capsOut = Renderer::g_deviceBlendShadeCaps;
	}

	// FUNCTION: TOY2 0x0048F410
	void RenderParallaxBackground(int32_t forceRender)
	{
		if (g_drawParallaxTexture)
		{
			if (Toy2::g_hasStaticBackdrop)
			{
				g_parallaxHorizOffset = 0.0;
				g_parallaxCurHorizScroll = 0.0;
				g_parallaxTexHeightRatio = 1.0;
				g_parallaxTexWidthRatio = 1.0;
			}
			else if (! Toy2::g_hasBackdrop)
			{
				return;
			}

			if (forceRender || GetIsSoftwareRendering() || (Glue::SetBackdrop(Toy2::g_nextBackdropId), ! Glue::BackdropBltFast()))
			{
				int32_t texIndex = NGNLoader::GetTextureDataIndex(Toy2::g_nextBackdropId);

				if (texIndex)
				{
					Vector2F uvMax;
					Vector2F uvMin;

					uvMin.y = 0.0;
					uvMin.x = 0.0;

					uvMax.y = 1.0;
					uvMax.x = 1.0;

					if (g_parallaxHorizOffset > 0.0)
					{
						float topFillHeight = 1.0 / g_virtualScreenHeight + g_parallaxHorizOffset;
						Sprite::Queue2DSprite(0.0, 0.0, 1.0, topFillHeight, &uvMin, &uvMax, 0, g_parallaxTexLastPixel, RENDER_PARALLAX_BG);
					}

					double verticalExtent = g_parallaxTexHeightRatio + g_parallaxHorizOffset;

					if (verticalExtent < 1.0)
					{
						float bottomFillHeight = 1.0 - g_parallaxHorizOffset - g_parallaxTexHeightRatio + 1.0 / g_virtualScreenHeight;
						float verticalEnd = verticalExtent;

						Sprite::Queue2DSprite(0.0, verticalEnd, 1.0, bottomFillHeight, &uvMin, &uvMax, 0, g_parallaxTexFirstPixel, RENDER_PARALLAX_BG);
					}

					double nextHorizontalPos;

					do
					{
						RGBA color;
						color.value = -1;

						Sprite::Queue2DSprite(
							g_parallaxCurHorizScroll,
							g_parallaxHorizOffset,
							g_parallaxTexWidthRatio,
							g_parallaxTexHeightRatio,
							&uvMin,
							&uvMax,
							texIndex,
							color,
							RENDER_PARALLAX_BG);

						nextHorizontalPos = g_parallaxTexWidthRatio + g_parallaxCurHorizScroll;

						g_parallaxCurHorizScroll = nextHorizontalPos;

					} while (nextHorizontalPos < 1.0);
				}
			}
		}
	}

	// FUNCTION: TOY2 0x004B6A50
	void FlushRenderQueues()
	{
		if (g_drawMaterialBuckets)
			FlushMaterialBuckets();

		if (g_drawTransparentBuckets)
			FlushTransparentBuckets();

		FlushPrimitives();
		ResetRenderPools();

		if (g_isSoftwareRendering)
		{
			SoftwareRenderer::FlushDepthQueue();
		}
	}

	// FUNCTION: TOY2 0x004B8BF0
	RGBA ModulateColorByAlpha(RGBA color, int32_t flags)
	{
		uint8_t blue;
		uint8_t green;
		uint8_t red;

		if (((flags & 0x4000) != 0 || flags == 0x20000000) && g_srcBlendMode == 2)
		{
			blue = (color.a * color.b) >> 8;
			red = (color.a * color.r) >> 8;
			green = (color.a * color.g) >> 8;

			color.b = blue;
			color.r = red;
			color.g = green;
		}
		else
		{
			blue = color.b;
			green = color.g;
			red = color.r;
		}

		if (g_alphaBlendDest != 6 || flags != 0x40000000)
			return color;

		color.a = green;

		if (blue == green && blue == red)
		{
			color.b = 0;
			color.g = 0;
			color.r = 0;
			return color;
		}

		color.b = 255 - blue;
		color.g = 255 - green;
		color.r = 255 - red;

		return color;
	}

	// FUNCTION: TOY2 0x004C2870
	void BindTexture(int32_t texIndex)
	{
		g_boundMaterial = 0;

		if (g_boundTextureIndices[0] != texIndex)
		{
			g_boundTextureIndices[0] = texIndex;
			DrawingDevice::BindTexWithStage(texIndex, 0);
		}

		for (int32_t idx = 1; idx < g_maxSimultaneousTextures; ++idx)
		{
			if (g_boundTextureIndices[0] != texIndex)
			{
				g_boundTextureIndices[0] = -1;
				DrawingDevice::BindTexWithStage(-1, idx);
			}
		}
	}

	// FUNCTION: TOY2 0x004B2CF0
	void ConfigureFog(float fogStart, float fogEnd, RGBA color)
	{
		g_fogEnabled = 1;
		g_fogStart = fogStart;
		g_fogEnd = fogEnd;
		g_fogColor = ApplyGammaCorrection(color).value;
	}

	// FUNCTION: TOY2 0x004B2D20
	void SetFogEnable(int32_t enableFog)
	{
		if (g_fogEnabled)
		{
			if (enableFog)
				DrawingDevice::SetRenderState(D3DRENDERSTATE_FOGENABLE, 1);
			else
				DrawingDevice::SetRenderState(D3DRENDERSTATE_FOGENABLE, 0);
		}
	}

	// FUNCTION: TOY2 0x004BC410
	void SetRenderDistance(float nearDistance, float farDistance)
	{
		// distances are kept squared so they can be compared against squared object distances
		g_nearRenderDistance = nearDistance * nearDistance;
		g_farRenderDistance = farDistance * farDistance;
	}

	// FUNCTION: TOY2 0x004CDD80
	void SetViewportPreset() { SetViewportPresetByDetail(Toy2::g_toyCfgData.detail); }

	// FUNCTION: TOY2 0x004CDD10
	void SetViewportPresetByDetail(int32_t detail)
	{
		int32_t preset = detail;

		if (detail > 2)
			preset = 2;
		else if (detail < 0)
			preset = 0;

		SetRenderDistance(Toy2::g_viewportPresets[preset].nearDistance, Toy2::g_viewportPresets[preset].farDistance);

		float viewportTB = Toy2::g_viewportPresets[preset].viewportTB;
		float nearZ = Toy2::g_viewportPresets[preset].nearZ;

		Nu3D::Camera::g_farLayerViewportLR = Toy2::g_viewportPresets[preset].viewportLR;

		float farLayerNearZ = Toy2::g_viewportPresets[preset].farLayerNearZ;

		Nu3D::Camera::g_cameraViewportTB = viewportTB;
		Nu3D::Camera::g_cameraNearZ = nearZ;
		Nu3D::Camera::g_farLayerNearZ = farLayerNearZ;

#ifdef APPLY_FIXES
		SetRenderDistance(1000000000.0f, 1000000000.0f);
		Nu3D::Camera::g_cameraViewportTB = 3.402823466e+38f;
#endif
	}

	// FUNCTION: TOY2 0x004CE5B0
	void ShowBlackFrames()
	{
		RGBA black;
		black.value = 0;

		// clear and present twice so both buffers of the swap chain are black
		ClearScreen(black, 3);

		if (BeginScene())
			EndScene(1);

		ClearScreen(black, 3);

		if (BeginScene())
			EndScene(1);
	}

	// FUNCTION: TOY2 0x0048F230
	void UpdateBackgroundScroll(int16_t horizontalScroll, int16_t verticalScroll)
	{
		int32_t texIndex;

		if (Toy2::g_hasStaticBackdrop)
			texIndex = NGNLoader::GetTextureDataIndex(37);
		else
			texIndex = NGNLoader::GetTextureDataIndex(Toy2::g_nextBackdropId);

		if (texIndex && g_drawParallaxTexture)
		{
			uint32_t* textureData;
			uint32_t textureWidth;
			uint32_t textureHeight;
			uint32_t bitmapWidth;
			uint32_t bitmapHeight;

			NGNLoader::RetrieveTextureData(texIndex, &bitmapWidth, &bitmapHeight, &textureWidth, &textureHeight, &textureData);

			g_parallaxTexFirstPixel.value = *textureData;
			g_parallaxTexLastPixel.value = textureData[textureHeight * textureWidth - 1];

			g_parallaxTexWidthRatio = bitmapWidth / g_virtualScreenWidth;
			g_parallaxTexHeightRatio = bitmapHeight / g_virtualScreenHeight;

			// the yaw input is a 16-bit angle, work out how far it moved since last frame (accounting for wrap around)
			double verticalInput = verticalScroll;
			int32_t verticalDelta = (int32_t)(verticalInput - g_parallaxPrevVertInput);

			g_parallaxPrevVertInput = verticalInput;

			if (verticalDelta > 32768)
				verticalDelta -= 65536;

			if (verticalDelta < -32768)
				verticalDelta += 65536;

			g_parallaxScrollDelta = verticalDelta * 0.000015258789 * 4.1887903;

			double horizontalPos = g_parallaxCurHorizScroll + g_parallaxScrollDelta;

			g_parallaxCurHorizScroll = horizontalPos;

			// keep the scroll position within one texture width
			float negWidth = -g_parallaxTexWidthRatio;

			if (horizontalPos <= negWidth)
			{
				do
					horizontalPos = horizontalPos + g_parallaxTexWidthRatio;
				while (horizontalPos <= negWidth);

				g_parallaxCurHorizScroll = horizontalPos;
			}

			if (horizontalPos > 0.0)
			{
				do
					horizontalPos = horizontalPos - g_parallaxTexWidthRatio;
				while (horizontalPos > 0.0);

				g_parallaxCurHorizScroll = horizontalPos;
			}

			g_parallaxHorizOffset = horizontalScroll * 0.000015258789;
			g_parallaxHorizOffset =
				g_parallaxHorizOffset / (Nu3D::Camera::g_primaryCamera->aspectHOverW * 0.23873241) + g_parallaxVertOffset / g_virtualScreenHeight;
		}
	}

	// FUNCTION: TOY2 0x00440E90
	void DrawCinematicBars()
	{
		Vector2F uvTopLeft;
		Vector2F uvBottomRight;

		uvTopLeft.x = 0.0;
		uvTopLeft.y = 0.0;

		uvBottomRight.x = 1.0;
		uvBottomRight.y = 1.0;

		// the bars slide in/out over 25 frames, the timer is how far along they currently are
		if (! g_enableCinematicBars)
		{
			int32_t timer = g_letterboxTimer - g_frameDelta;

			if (timer < 0)
			{
				g_letterboxTimer = 0;
				return;
			}

			g_letterboxTimer = timer;

			if (timer <= 0)
				return;
		}
		else
		{
			int32_t timer = g_frameDelta + g_letterboxTimer;

			if (timer > 25)
			{
				g_letterboxTimer = 25;
			}
			else
			{
				g_letterboxTimer = timer;

				if (timer <= 0)
					return;
			}
		}

		float height = g_letterboxTimer * 0.0040000002;

		RGBA black;
		black.value = 0xFF000000;

		Sprite::Queue2DSprite(0.0, 0.0, 1.0, height, &uvTopLeft, &uvBottomRight, 0, black, RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT);

		float yPos = 1.0 - height;

		Sprite::Queue2DSprite(0.0, yPos, 1.0, height, &uvTopLeft, &uvBottomRight, 0, black, RENDER_ZWRITE | RENDER_CULL_NONE | RENDER_ALPHA_DEFAULT);
	}

	// FUNCTION: TOY2 0x00401B60
	void DrawBlackBorderBox(int16_t xPos, int16_t yPos, int32_t width, int32_t height, uint8_t red, uint8_t green, uint8_t blue)
	{
		Sprite::DrawScaled(xPos, yPos, 6, 1, 0, 0, 0, 96, 8192, height);
		Sprite::DrawScaled((width >> 12) + xPos - 2, yPos, 6, 1, 0, 0, 0, 96, 8192, height);
		Sprite::DrawScaled(xPos, yPos, 6, 1, 0, 0, 0, 96, width, 4096);
		Sprite::DrawScaled(xPos, (height >> 12) + yPos - 1, 6, 1, 0, 0, 0, 96, width, 4096);
		Sprite::DrawScaled(xPos + 2, yPos + 1, 6, 1, red, green, blue, 0, width - 0x4000, height - 0x2000);
	}

	// FUNCTION: TOY2 0x00401FB0
	void DrawString(int16_t yPos, char* text, uint8_t red, uint8_t green, uint8_t blue, int32_t useDynamicWidth)
	{
		int32_t textLength = 0;
		char* curText = text;

		if (*text)
		{
			while (text[++textLength])
				;
		}

		// the text is centered, a character is 8 wide
		int32_t xPos = (useDynamicWidth != 0 ? 256 : 160) - 4 * textLength;

		if (textLength > 0)
		{
			int32_t remainingChars = textLength;

			do
			{
				char curChar = *curText++;

				DrawChar(xPos, yPos, curChar, red, green, blue, useDynamicWidth);

				xPos += 8;
				--remainingChars;
			} while (remainingChars);
		}
	}

	// FUNCTION: TOY2 0x0049B2A0
	void DrawMenuText(int16_t yPos, char* text, int32_t shadow)
	{
		char* curText = text;
		int32_t length = 0;

		if (*text)
		{
			while (text[++length])
				;
		}

		// centred on the screen
		int16_t xPos = 4 * (40 - length);

		if (length > 0)
		{
			int32_t remainingChars = length;

			do
			{
				char curChar = *curText++;

				switch (curChar)
				{
					// the button icons
					case '1':
						Sprite::DrawTile(xPos - 4, yPos - 3, 68, 2);
						break;
					case '2':
						Sprite::DrawTile(xPos - 4, yPos - 3, 68, 3);
						break;
					case '3':
						Sprite::DrawTile(xPos - 4, yPos - 3, 68, 1);
						break;
					case '4':
						Sprite::DrawTile(xPos - 4, yPos - 3, 68, 0);
						break;
					default:
						if (curChar != ' ')
						{
							uint8_t tileIndex;

							if (curChar == '\'')
								tileIndex = '/';
							else
								tileIndex = curChar - 'a';

							if (shadow)
								Sprite::DrawScaled(xPos, yPos, 67, tileIndex, 0x80, 0x80, 0x80, 0, 2048, 2048);
							else
								Sprite::DrawScaled(xPos, yPos, 67, tileIndex, 0x80, 0x80, 0x80, 255, 2048, 2048);
						}

						break;
				}

				xPos += 8;
				--remainingChars;
			} while (remainingChars);
		}
	}

	// FUNCTION: TOY2 0x0049B3B0
	void DrawMenuTextScaled(int16_t xPos, int16_t yPos, const char* text, int32_t shadow, int32_t alignment, int32_t scale)
	{
		int32_t length = strlen(text);
		int16_t curXPos;

		if (alignment)
		{
			int32_t offset;

			if (alignment == 2)
				offset = -16 * scale * length / 4096;
			else
				offset = -16 * scale * length / 0x2000;

			curXPos = offset + xPos;
		}
		else
		{
			curXPos = xPos;
		}

		if (length > 0)
		{
			int32_t remainingChars = length;

			do
			{
				char curChar = *text++;

				if (curChar != ' ')
				{
					uint8_t tileIndex;

					switch (curChar)
					{
						case '\'':
							tileIndex = 47;
							break;
						case '(':
							tileIndex = 33;
							break;
						case ')':
							tileIndex = 34;
							break;
						case ',':
							tileIndex = 26;
							break;
						case '.':
							tileIndex = 27;
							break;
						case '/':
							tileIndex = 28;
							break;
						case '0':
							tileIndex = curChar - 4;
							break;
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
							tileIndex = curChar - 14;
							break;
						case ':':
							tileIndex = 31;
							break;
						case ';':
							tileIndex = 32;
							break;
						case '?':
							tileIndex = 30;
							break;
						case 'A':
						case 'B':
						case 'C':
						case 'D':
							tileIndex = curChar - 20;
							break;
						case '\\':
							tileIndex = 29;
							break;
						default:
							tileIndex = curChar - 97;
							break;
					}

					if (shadow)
						Sprite::DrawScaled(curXPos, yPos, 67, tileIndex, 0x80, 0x80, 0x80, 0, scale, scale);
					else
						Sprite::DrawScaled(curXPos, yPos, 67, tileIndex, 0x80, 0x80, 0x80, 255, scale, scale);
				}

				curXPos += 16 * scale / 4096;
				--remainingChars;
			} while (remainingChars);
		}
	}

	// FUNCTION: TOY2 0x0049B630
	void DrawChar(int16_t xPos, int16_t yPos, char character, uint8_t red, uint8_t green, uint8_t blue, int32_t useDynamicWidth)
	{
		if (character == '@')
		{
			Sprite::DrawTiledFixed(xPos, yPos - 2, 38, 1);
		}
		else if (character == '~')
		{
			Sprite::DrawTiledFixed(xPos, yPos - 2, 38, 0);
		}
		else if (character != ' ')
		{
			uint8_t fontIndex;

			switch (character)
			{
				case '!':
					fontIndex = 41;
					break;

				case '\'':
					fontIndex = 48;
					break;

				case '*':
					fontIndex = 50;
					break;

				case ',':
					fontIndex = 37;
					break;

				case '-':
					fontIndex = 49;
					break;

				case '.':
					fontIndex = 36;
					break;

				case '>':
					fontIndex = 45;
					break;

				case '?':
					fontIndex = 40;
					break;

				default:
					// quirk: only lowercase letters and digits have a tile, upper case letters wrap around
					if (character > '9')
						fontIndex = character - 97;
					else
						fontIndex = character - 22;

					break;
			}

			if (useDynamicWidth)
				Sprite::DrawScaled(xPos, yPos, 20, fontIndex, red, green, blue, 255, 2048, 2048);
			else
				Sprite::DrawScaledFixed(xPos, yPos, 20, fontIndex, red, green, blue, 255, 2048, 2048);
		}
	}

	// FUNCTION: TOY2 0x0049B990
	void DrawFormattedText(int32_t screenX, int32_t screenY, char* format, ...)
	{
		char buffer[512];

		va_list argList;
		va_start(argList, format);

		vsprintf(buffer, format, argList);

		Sprite::DrawBitmapText(buffer, screenY, screenX, 255, 255, 255);
	}

	// FUNCTION: TOY2 0x004AFD30
	void BlitBitmapWithWrapping(
		Nu3D::BmpDataNode* bmpDataNode,
		int32_t srcX,
		int32_t srcY,
		int32_t width,
		int32_t height,
		int32_t horizontalWrapOffset,
		int32_t verticalWrapOffset,
		LONG destX,
		LONG destY)
	{
		RECT srcRect;
		RECT destRect;

		LONG destRightEdge;
		LONG destBottomEdge;
		LONG srcBottomEdge;

		if (g_blitWithWrappingPath)
		{
			DDBLTFX bltFx;
			bltFx.dwSize = 100;
			bltFx.dwROP = 0xCC0020;

			LPDIRECTDRAWSURFACE4 surface = bmpDataNode->surface;

			if (verticalWrapOffset)
			{
				destRect.left = srcX;
				destRightEdge = srcX + width;
				destRect.right = srcX + width;
				destRect.top = srcY + verticalWrapOffset;
				destRect.bottom = srcY + height;
				srcRect.left = destX;
				srcRect.top = destY;
				destBottomEdge = destX + width;
				srcRect.right = destX + width;
				srcBottomEdge = height - verticalWrapOffset + destY;
				srcRect.bottom = srcBottomEdge;
				surface->Blt(&srcRect, surface, &destRect, 0x1020000, &bltFx);

				srcRect.left = destX;
				srcRect.top = srcBottomEdge;
				destRect.right = destRightEdge;
				srcRect.bottom = destY + height;
				destRect.left = srcX;
				destRect.top = srcY;
				destRect.bottom = srcY + verticalWrapOffset;
				srcRect.right = destBottomEdge;
				surface->Blt(&srcRect, surface, &destRect, 16908288, &bltFx);
			}
			else
			{
				destRect.top = srcY;
				destRect.right = width + srcX - horizontalWrapOffset;
				destRect.left = srcX + horizontalWrapOffset;
				destBottomEdge = height + srcY;
				destRect.bottom = height + srcY;
				srcRect.left = destX;
				srcRect.top = destY;
				srcBottomEdge = width - horizontalWrapOffset + destX;
				srcRect.right = srcBottomEdge;
				destRightEdge = destY + height;
				srcRect.bottom = destY + height;
				surface->Blt(&srcRect, surface, &destRect, 16908288, &bltFx);

				if (horizontalWrapOffset)
				{
					destRect.top = srcY;
					destRect.bottom = destBottomEdge;
					srcRect.left = srcBottomEdge;
					srcRect.top = destY;
					srcRect.bottom = destRightEdge;
					destRect.left = srcX;
					destRect.right = srcX + horizontalWrapOffset;
					srcRect.right = destX + width;
					surface->Blt(&srcRect, surface, &destRect, 16908288, &bltFx);
				}
			}
		}
	}

	// FUNCTION: TOY2 0x004B8940
	void ProcessPatch(Nu3D::InstanceData* spriteStruct, Nu3D::Patch* patch)
	{
		Nu3D::Material* material = Nu3D::Material::GetFreeByIndex(patch->materialId);

		RenderEntry::AllocPatch(material, (Nu3D::Primitive*)patch, spriteStruct);

		if ((material->metadata & 4) != 0)
			RenderEntry::AllocPatch(Nu3D::g_tex14Version1, (Nu3D::Primitive*)patch, spriteStruct);

		if ((material->metadata & 0x40) != 0)
			RenderEntry::AllocPatch(Nu3D::g_tex14Version2, (Nu3D::Primitive*)patch, spriteStruct);

		if ((int8_t)material->metadata < 0)
			RenderEntry::AllocPatch(Nu3D::g_tex14Version3, (Nu3D::Primitive*)patch, spriteStruct);
	}

	// FUNCTION: TOY2 0x004B87F0
	void RenderPatchList(Nu3D::Patch* patch, D3DMATRIX* nodeMatrices, int32_t** flags, int32_t renderStage)
	{
		int32_t stage = g_additionalRenderFlags | renderStage;

		while (patch)
		{
			// a patch has the layout of a primitive here, unkVar11 is the header count, the headers are the node indices that start at indexData
			Nu3D::InstanceData* instanceData = InstanceData::AllocFromNodeMatrices(nodeMatrices, (int32_t*)patch->indexData, patch->unkVar11, flags, stage);

			if (instanceData)
				ProcessPatch(instanceData, patch);

			patch = patch->listNext;
		}
	}

	// FUNCTION: TOY2 0x004B92E0
	void SetVertexColorModulation(int32_t red, int32_t green, int32_t blue)
	{
		g_vertexColorModRed = red;
		g_vertexColorModGreen = green;
		g_vertexColorModBlue = blue;
	}

	// FUNCTION: TOY2 0x004B9300
	int32_t EnableVertexColorModulation(int32_t a1)
	{
		int32_t result = g_useVertexColorMod;
		g_useVertexColorMod = a1;

		return result;
	}

	// FUNCTION: TOY2 0x004B9600
	void DrawSingleTexturedTriangle(void* vertices, int32_t texIndex, int32_t renderState)
	{
		Renderer::InitRenderState(renderState);
		Renderer::BindTexture(texIndex);
		DrawingDevice::DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_0x1C4, vertices, 3, 16);
	}

	// FUNCTION: TOY2 0x004B9A60
	int32_t Set9F5FF8(int32_t value)
	{
		int32_t result = g_unk9F5FF8;
		g_unk9F5FF8 = value;

		return result;
	}

	// FUNCTION: TOY2 0x004CE510
	void BlitTextureByIndex(
		int32_t texIndex,
		LONG destX,
		LONG destY,
		int32_t width,
		int32_t height,
		int32_t horizontalWrapOffset,
		int32_t verticalWrapOffset,
		int32_t srcX,
		int32_t srcY)
	{
		if ((Toy2::g_toyCfgData.flags & 4) != 0)
		{
			int32_t textureDataIndex = NGNLoader::GetTextureDataIndex(texIndex);

			if (textureDataIndex)
			{
				NGNLoader::NGNTextureData* texData = NGNLoader::GetTextureDataByIndex(textureDataIndex);

				Renderer::BlitBitmapWithWrapping(texData->bmpDataNode, srcX, srcY, width, height, horizontalWrapOffset, verticalWrapOffset, destX, destY);
			}
		}
	}

	// FUNCTION: TOY2 0x0049B260
	void BlitTextureByIndexOffset(
		int32_t texIndex,
		int32_t destX,
		int32_t destY,
		int32_t width,
		int32_t height,
		int32_t horizontalWrapOffset,
		int32_t verticalWrapOffset,
		int32_t srcX,
		int32_t srcY)
	{
		Renderer::BlitTextureByIndex(texIndex, destX, destY, width, height, horizontalWrapOffset, verticalWrapOffset, destX + srcX, destY + srcY);
	}

	// FUNCTION: TOY2 0x004B89B0 [MATCHED]
	Renderer::RenderEntry* RenderEntry::AllocPatch(Nu3D::Material* material, Nu3D::Primitive* prim, Nu3D::InstanceData* spriteStruct)
	{
		if (g_renderEntryCount)
		{
			int32_t index = --g_renderEntryCount;

			RenderEntry* renderEntry = &g_renderEntries[index];

			renderEntry->primitive = prim;
			renderEntry->instanceData = spriteStruct;
			renderEntry->material = material;

			if (*(uint32_t*)&spriteStruct->lodFactor == 0x3F800000 && (material->metadata & 0xE33) == 0)
			{
				renderEntry->type = RENDER_TYPE6;
				renderEntry->next = (RenderEntry*)material->renderEntryHead;
				material->renderEntryHead = renderEntry;
			}
			else
			{
				renderEntry->type = RENDER_TYPE8;
				InsertIntoBucket(renderEntry);
			}

			return renderEntry;
		}

		Logger::DebugLog("rndrentryAllocPatch - out of rndrentries");
		return 0;
	}
}

namespace DevDraw
{
	// GLOBAL: TOY2 0x00732FC8
	DrawBuffer g_drawBufferSrc;

	// GLOBAL: TOY2 0x007D2DA8
	DrawTranBuffer g_drawTranBufferSrc;

	// GLOBAL: TOY2 0x00500A10
	DrawBuffer* g_drawBuffer = &g_drawBufferSrc;

	// GLOBAL: TOY2 0x00500A14
	DrawTranBuffer* g_drawTranBuffer = &g_drawTranBufferSrc;

	// GLOBAL: TOY2 0x00500A28
	int16_t g_bufferIndex = -1;

	// GLOBAL: TOY2 0x00732FBC
	int32_t g_vertexCount;

	// FUNCTION: TOY2 0x004907E0
	int16_t DrawSlots()
	{
		if (D3DApp::g_renderMode == RENDERMODE_SOFTWARE)
		{
			if (SoftwareRenderer::g_unk559C40 != 1 || (Renderer::g_frameDelta & 1) == 0)
				SoftwareRenderer::UnkFunc8();

			SoftwareRenderer::g_unk839280 = 0;

			Nu3D::MemSet32Util(SoftwareRenderer::g_unk504D34, 0x1000, 0);
			return 1;
		}

		if (D3DApp::g_renderMode != RENDERMODE_D3D)
			return 1;

		for (int16_t dbSlot = 0; dbSlot < 64; ++dbSlot)
			FlushDrawBufferSlot(dbSlot);

		for (int16_t tranDbSlot = 0; tranDbSlot < 64; ++tranDbSlot)
			FlushTransparentDrawBufferSlot(tranDbSlot);

		return 1;
	}

	// FUNCTION: TOY2 0x00490470
	int16_t FlushDrawBufferSlot(int16_t bufferIndex)
	{
		Renderer::InitRenderState(0);

		LPDIRECT3DDEVICE3 device = DrawingDevice::GetD3DDevice();

		DrawBuffer* drawBuffer = g_drawBuffer;

		if (g_drawBuffer->vertexCount[bufferIndex])
		{
			// the textures of the draw buffer are called LOADTEXT_tex00, LOADTEXT_tex01 ...
			char nodeName[16];
			memcpy(nodeName, "LOADTEXT_tex", 12);

			nodeName[14] = 0;
			nodeName[12] = bufferIndex / 10 + 48;
			nodeName[13] = bufferIndex % 10 + 48;

			Nu3D::BmpDataNode* bmpNode = Nu3D::GetBmpDataNodeByName_T(nodeName);

			device->SetTexture(0, Nu3D::BmpDataNode::GetTexture(bmpNode));

			HRESULT result = device->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				D3DFVF_0x1C4,
				g_drawBuffer->vertex[bufferIndex],
				g_drawBuffer->vertexCount[bufferIndex],
				g_drawBuffer->index[bufferIndex],
				g_drawBuffer->indexCount[bufferIndex],
				8);

			if (result < 0)
				Logger::LogDDError(
					"dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, ( 0x004 | 0x040 | 0x080 | 0x100 ), (LPVOID) (drawb->Vertice[i]), drawb->VerticeCount[i], "
					"(LPWORD) & (drawb->Index[i]), drawb->IndexCount[i], 0x00000008l)",
					result);

			drawBuffer = g_drawBuffer;

			g_bufferIndex = bufferIndex;
			g_vertexCount += g_drawBuffer->vertexCount[bufferIndex];
		}

		drawBuffer->indexCount[bufferIndex] = 0;

		g_drawBuffer->vertexCount[bufferIndex] = 0;

		return 1;
	}

	// FUNCTION: TOY2 0x004905C0
	int16_t FlushTransparentDrawBufferSlot(int16_t bufferIndex)
	{
		LPDIRECT3DDEVICE3 device = DrawingDevice::GetD3DDevice();

		Renderer::InitRenderState(1024);

		if (bufferIndex < 32 && g_drawTranBuffer->vertexCount[bufferIndex])
		{
			char nodeName[16];
			nodeName[14] = 0;

			memcpy(nodeName, "LOADTEXT_tex", 12);

			nodeName[12] = bufferIndex / 10 + 48;
			nodeName[13] = bufferIndex % 10 + 48;

			Nu3D::BmpDataNode* bmpNode = Nu3D::GetBmpDataNodeByName_T(nodeName);

			device->SetTexture(0, Nu3D::BmpDataNode::GetTexture(bmpNode));

			HRESULT result = device->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				D3DFVF_0x1C4,
				g_drawTranBuffer->vertices[bufferIndex],
				g_drawTranBuffer->vertexCount[bufferIndex],
				g_drawTranBuffer->index[bufferIndex],
				g_drawTranBuffer->indexCount[bufferIndex],
				8);

			if (result < 0)
				Logger::LogDDError(
					"dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, ( 0x004 | 0x040 | 0x080 | 0x100 ), (LPVOID) (drawtranb->Vertice[i]), "
					"drawtranb->VerticeCount[i], (LPWORD) & (drawtranb->Index[i]), drawtranb->IndexCount[i], 0x00000008l)",
					result);

			g_bufferIndex = bufferIndex;
			g_vertexCount += g_drawTranBuffer->vertexCount[bufferIndex];
		}

		Renderer::InitRenderState(0);

#ifdef APPLY_FIXES
		// the slots 32 to 63 are past the end of indexCount, the original zeroes whatever is behind the buffer
		if (bufferIndex < 32)
#endif
			g_drawTranBuffer->indexCount[bufferIndex] = 0;

		g_drawTranBuffer->vertexCount[bufferIndex] = 0;

		return 1;
	}
}

#include "Nu3D/FMV.h"

namespace Nu3D
{
	namespace FMV
	{
		// STUB: TOY2 0x004CE5F0
		int32_t PlayMovie(LPCCH filePath) { return 0; }

		// STUB: TOY2 0x004DB600
		Nu3D::FMV::Nu3DFMV* CreateFMVInstance(LPCCH filePath) { return 0; }

		// STUB: TOY2 0x004DB8C0
		void Destroy(Nu3D::FMV::Nu3DFMV* fmv) {}

		// STUB: TOY2 0x004DB910
		void SetDimensions(Nu3D::FMV::Nu3DFMV* fmv, int32_t left, int32_t top, int32_t right, int32_t bottom) {}

		// STUB: TOY2 0x004DB940
		uint32_t IsPlaying(Nu3D::FMV::Nu3DFMV* fmv) { return 0; }

		// STUB: TOY2 0x004DB950
		void UpdateAndRenderFMV(Nu3D::FMV::Nu3DFMV* fmv) {}

		// STUB: TOY2 0x004DBB00
		void SetPaused(Nu3D::FMV::Nu3DFMV* fmv, int32_t paused) {}

		// STUB: TOY2 0x004DBB80
		HRESULT Seek(Nu3D::FMV::Nu3DFMV* fmv, int32_t streamTime) { return 0; }
	}
}

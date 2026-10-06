#include "Toy2/Animation.h"
#include "Nu3D/Creature.h"
#include <directx6/ddraw.h>
#include <directx6/d3d.h>

namespace Toy2
{
	namespace Animation
	{
		// STUB: TOY2 0x004CD120
		void ResetNodeAngles() {}

		// STUB: TOY2 0x004CD880
		void AnimateActors(Actor::Toy2Actor** actorList) {}

		// STUB: TOY2 0x00401000
		uint32_t StepEventTrack(uint8_t* animStream) { return 0; }

		// STUB: TOY2 0x0043BA80
		void EvaluateClip(int16_t* clipHeader, int32_t time, int32_t baseBone, uint8_t track) {}

		// STUB: TOY2 0x0043C0E0
		void TransformByBone(Vector3I* point, Toy2::Actor::Toy2Actor* actor, int32_t boneIndex) {}

		// STUB: TOY2 0x004CD170
		void ParseHeader(int32_t animData) {}

		// STUB: TOY2 0x004CD1B0
		int32_t SampleNodeTransform(int32_t curNode, uint16_t* animData, uint32_t unk, D3DMATRIX* matrix) { return 0; }

		// STUB: TOY2 0x004CD7B0
		void EvaluateClipToMatrices(int32_t index, Nu3D::Creature* creature, D3DMATRIX* targetMatrix, int16_t* animData, uint32_t unk, int32_t flag) {}

		// STUB: TOY2 0x004CD140
		void GetRootWorldPos(Vector3F* out, int32_t index) {}
	}
}

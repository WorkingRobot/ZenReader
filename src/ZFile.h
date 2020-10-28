#pragma once

#include "ZContainer.h"

namespace Zen {
	class ZFile {
	public:
		ZFile(uint32_t ChunkIdIdx, const ZContainer& Container) :
			ChunkIdIdx(ChunkIdIdx),
			Container(Container)
		{

		}

	private:
		uint32_t ChunkIdIdx;
		const ZContainer& Container;
	};
}
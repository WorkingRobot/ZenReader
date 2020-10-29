#pragma once

#include "ZContainer.h"
#include "Helpers/Align.h"
#include "Helpers/AES.h"
#include "Helpers/Decompress.h"
#include "Streams/ZFileStream.h"

namespace Zen {
	class ZFile {
	public:
		ZFile(uint32_t ChunkIdIdx, const BaseContainer& Container) :
			ChunkIdIdx(ChunkIdIdx),
			Container(Container)
		{

		}

		ZFileStream GetStream() const {
			return ZFileStream(ChunkIdIdx, Container);
		}

	private:
		uint32_t ChunkIdIdx;
		const BaseContainer& Container;
	};
}
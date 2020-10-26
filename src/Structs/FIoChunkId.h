#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FIoChunkId {
	public:
		uint8_t Id[12];

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoChunkId& Value) {
			InputStream >> Value.Id;
			return InputStream;
		}
	};
}
#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FIoChunkHash {
	public:
		uint8_t Hash[32];

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoChunkHash& Value) {
			InputStream >> Value.Hash;
			return InputStream;
		}
	};
}
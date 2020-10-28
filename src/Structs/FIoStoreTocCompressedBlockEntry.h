#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FIoStoreTocCompressedBlockEntry {
	public:
		uint8_t Data[12];

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoStoreTocCompressedBlockEntry& Value) {
			InputStream >> Value.Data;
			return InputStream;
		}

		uint64_t GetOffset() const {
			return *((uint64_t*)Data) & 0xFFFFFFFFFF;
		}

		uint32_t GetCompressedSize() const {
			return (*((uint32_t*)Data + 1) >> 8) & 0xFFFFFF;
		}

		uint32_t GetUncompressedSize() const {
			return *((uint32_t*)Data + 2) & 0xFFFFFF;
		}

		uint8_t GetCompressionMethodIndex() const {
			return *((uint32_t*)Data + 2) >> 24;
		}
	};
}
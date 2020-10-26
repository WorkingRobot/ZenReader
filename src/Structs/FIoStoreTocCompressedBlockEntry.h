#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FIoStoreTocCompressedBlockEntry {
	public:
		uint64_t Offset;
		uint32_t CompressedSize;
		uint32_t UncompressedSize;
		uint8_t CompressionMethodIndex;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoStoreTocCompressedBlockEntry& Value) {
			uint8_t Data[12];
			InputStream >> Data;
			Value.Offset = *((uint64_t*)Data) & 0xFFFFFFFFFF;
			Value.CompressedSize = (*((uint32_t*)Data + 1) >> 8) & 0xFFFFFF;
			Value.UncompressedSize = *((uint32_t*)Data + 2) & 0xFFFFFF;
			Value.CompressionMethodIndex = *((uint32_t*)Data + 2) >> 24;
			return InputStream;
		}
	};
}
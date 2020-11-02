#pragma once

#include "../Streams/BaseStream.h"
#include "../Enums/EIoChunkType.h"

namespace Zen::Structs {
	using namespace Enums;
	class FIoChunkId {
	public:
		uint8_t Id[12] = { 0 };

		constexpr FIoChunkId() {}

		constexpr FIoChunkId(uint64_t GlobalPackageId, uint16_t ChunkIndex, EIoChunkType IoChunkType) {
			*(uint32_t*)(Id + 0) = GlobalPackageId;
			*(uint16_t*)(Id + 4) = ChunkIndex;
			*(uint8_t*)(Id + 11) = (uint8_t)IoChunkType;
		}

		bool operator==(const FIoChunkId&) const = default;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoChunkId& Value) {
			InputStream >> Value.Id;

			return InputStream;
		}
	};
}
#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FIoChunkOffsetAndLength {
	public:
		uint64_t Offset;
		uint64_t Length;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoChunkOffsetAndLength& Value) {
			uint8_t OffsetAndLength[10];
			InputStream >> OffsetAndLength;
			Value.Offset = OffsetAndLength[4]
				| (uint64_t(OffsetAndLength[3]) << 8)
				| (uint64_t(OffsetAndLength[2]) << 16)
				| (uint64_t(OffsetAndLength[1]) << 24)
				| (uint64_t(OffsetAndLength[0]) << 32)
				;
			Value.Length = OffsetAndLength[9]
				| (uint64_t(OffsetAndLength[8]) << 8)
				| (uint64_t(OffsetAndLength[7]) << 16)
				| (uint64_t(OffsetAndLength[6]) << 24)
				| (uint64_t(OffsetAndLength[5]) << 32)
				;
			return InputStream;
		}
	};
}
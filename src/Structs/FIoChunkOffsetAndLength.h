#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FIoChunkOffsetAndLength {
	public:
		uint8_t Data[10];

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoChunkOffsetAndLength& Value) {
			InputStream >> Value.Data;

			return InputStream;
		}

		uint64_t GetOffset() const {
			return Data[4]
				| (uint64_t(Data[3]) << 8)
				| (uint64_t(Data[2]) << 16)
				| (uint64_t(Data[1]) << 24)
				| (uint64_t(Data[0]) << 32);
		}

		uint64_t GetLength() const {
			return Data[9]
				| (uint64_t(Data[8]) << 8)
				| (uint64_t(Data[7]) << 16)
				| (uint64_t(Data[6]) << 24)
				| (uint64_t(Data[5]) << 32);
		}
	};
}
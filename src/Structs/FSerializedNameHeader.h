#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FSerializedNameHeader {
	public:
		uint8_t Data[2];

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FSerializedNameHeader& Value) {
			InputStream >> Value.Data;

			return InputStream;
		}

		bool IsUtf16() const {
			return Data[0] & 0x80;
		}

		uint16_t GetLength() const {
			return ((Data[0] & 0x7Fu) << 8) + Data[1];
		}
	};
}
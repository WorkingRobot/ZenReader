#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FAESKey {
	public:
		char Key[32];

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FAESKey& Value) {
			InputStream >> Value.Key;

			return InputStream;
		}

		constexpr static FAESKey FromHex(const char HexBlob[64]) {
			FAESKey Key;
			for (int i = 0, j = 0; j < 32; i += 2, j++) {
				Key.Key[j] = (HexBlob[i] % 32 + 9) % 25 * 16 + (HexBlob[i + 1] % 32 + 9) % 25;
			}
			return Key;
		}
	};
}
#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FSHAHash {
	public:
		char Hash[20];

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FSHAHash& Value) {
			InputStream >> Value.Hash;
			return InputStream;
		}
	};
}
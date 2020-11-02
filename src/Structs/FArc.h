#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FArc {
	public:
		uint32_t FromNodeIndex;
		uint32_t ToNodeIndex;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FArc& Value) {
			InputStream >> Value.FromNodeIndex;
			InputStream >> Value.ToNodeIndex;

			return InputStream;
		}
	};
}
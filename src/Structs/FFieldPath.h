#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FFieldPath {
	public:
		std::vector<std::string> Names;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FFieldPath& Value) {
			InputStream >> Value.Names;

			return InputStream;
		}
	};
}
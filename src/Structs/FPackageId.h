#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FPackageId {
	public:
		uint64_t Id;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FPackageId& Value) {
			InputStream >> Value.Id;

			return InputStream;
		}
	};
}
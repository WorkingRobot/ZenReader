#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FNameEntryId {
	public:
		// Internally, it's a single uint32_t value, but this is because it's attached to a name pool.
		// We don't have that, so we'll just directly store the string and its hashes
		std::string Name;
		uint64_t Hash;
	};
}
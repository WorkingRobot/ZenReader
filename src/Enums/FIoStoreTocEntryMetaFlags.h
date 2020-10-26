#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum class FIoStoreTocEntryMetaFlags : uint8_t
	{
		None,
		Compressed = (1 << 0),
		MemoryMapped = (1 << 1)
	};
}
#pragma once

#include <stdint.h>

namespace Zen::Enums {
	// Why this is prefixed with F and not E, only per larsson will know
	// It's not because it's enum class either, EIoChunkType is enum class
	// and EIoChunkType is written by per larsson!?!?! we need an explanation man
	enum class FIoStoreTocEntryMetaFlags : uint8_t
	{
		None,
		Compressed = (1 << 0),
		MemoryMapped = (1 << 1)
	};
}
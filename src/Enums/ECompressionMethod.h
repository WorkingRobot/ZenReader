#pragma once

#include <stdint.h>

namespace Zen::Enums {
	// Used for usmap
	// Oodle is best (leviathan is used)
	// Brotli is offered for those who want open source/non-windows
	enum class ECompressionMethod : uint8_t {
		None,
		Oodle,
		Brotli,

		Unknown = 0xFF
	};
}
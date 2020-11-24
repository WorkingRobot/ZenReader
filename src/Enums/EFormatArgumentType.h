#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum class EFormatArgumentType : int8_t
	{
		Int,
		UInt,
		Float,
		Double,
		Text,
		Gender,
		// Add new enum types at the end only! They are serialized by index.
	};
}
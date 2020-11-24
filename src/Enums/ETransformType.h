#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum class ETransformType : uint8_t
	{
		ToLower = 0,
		ToUpper,

		// Add new enum types at the end only! They are serialized by index.
	};
}
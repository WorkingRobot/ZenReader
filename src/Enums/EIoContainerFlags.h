#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum EIoContainerFlags : uint8_t
	{
		None,
		Compressed = (1 << 0),
		Encrypted = (1 << 1),
		Signed = (1 << 2),
		Indexed = (1 << 3)
	};
}
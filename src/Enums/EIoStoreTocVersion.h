#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum EIoStoreTocVersion : uint8_t
	{
		Invalid = 0,
		Initial,
		DirectoryIndex,
		LatestPlusOne,
		Latest = LatestPlusOne - 1
	};
}
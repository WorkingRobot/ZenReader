#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum class EExportFilterFlags : uint8_t
	{
		None,
		NotForClient,
		NotForServer
	};
}
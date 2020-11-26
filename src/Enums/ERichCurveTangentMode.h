#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum ERichCurveTangentMode : uint8_t
	{
		RCTM_Auto,
		RCTM_User,
		RCTM_Break,
		RCTM_None
	};
}
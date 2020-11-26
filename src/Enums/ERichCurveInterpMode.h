#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum ERichCurveInterpMode : uint8_t
	{
		RCIM_Linear,
		RCIM_Constant,
		RCIM_Cubic,
		RCIM_None
	};
}
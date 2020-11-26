#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum ECurveTableMode : uint8_t
	{
		Empty,
		SimpleCurves,
		RichCurves
	};
}
#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum ERichCurveTangentWeightMode : uint8_t
	{
		RCTWM_WeightedNone,
		RCTWM_WeightedArrive,
		RCTWM_WeightedLeave,
		RCTWM_WeightedBoth
	};
}
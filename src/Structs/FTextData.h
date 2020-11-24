#pragma once

#include "../Enums/ETextHistoryType.h"
#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	using namespace Enums;

	struct FTextData {
		ETextHistoryType Type;

		FTextData(ETextHistoryType Type) : Type(Type) {}
	};
}
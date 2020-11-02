#pragma once

#include <stdint.h>

namespace Zen::Enums {
	// https://github.com/EpicGames/UnrealEngine/blob/ac1f4d53af58d8d00f6e2f7124f384d67bcb53c9/Engine/Source/Runtime/CoreUObject/Public/Serialization/AsyncLoading2.h#L32
	enum class EMappedNameType : uint8_t
	{
		Package,
		Container,
		Global
	};
}
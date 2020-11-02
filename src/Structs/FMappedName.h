#pragma once

#include "../Enums/EMappedNameType.h"
#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FMappedName {
	public:
		uint32_t Index;
		uint32_t Number;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FMappedName& Value) {
			InputStream >> Value.Index;
			InputStream >> Value.Number;

			return InputStream;
		}

		bool IsValid() const {
			return Index == -1 && Number == -1;
		}

		EMappedNameType GetType() const {
			return (EMappedNameType)((Index & 0xC0000000) >> 30);
		}

		// Why this isn't just == EMappedNameType::Global (or 2 in UE4's case), who knows
		// Here, a name in the container itself and not in the package is also technically global
		// https://github.com/EpicGames/UnrealEngine/blob/ac1f4d53af58d8d00f6e2f7124f384d67bcb53c9/Engine/Source/Runtime/CoreUObject/Public/Serialization/AsyncLoading2.h#L81
		bool IsGlobal() const {
			return GetType() != EMappedNameType::Package;
		}

		uint32_t GetIndex() const {
			return Index & 0x3FFFFFFF;
		}

		uint32_t GetNumber() const {
			return Number;
		}
	};
}
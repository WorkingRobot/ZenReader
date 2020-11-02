#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	// https://github.com/EpicGames/UnrealEngine/blob/5b0cfe38e0faf3c2f9938d8f6a1cb5805802a180/Engine/Source/Runtime/CoreUObject/Public/Serialization/AsyncLoading2.h#L124
	class FPackageObjectIndex {
	public:
		uint64_t TypeAndId;

		enum EType : uint8_t {
			Export,
			ScriptImport,
			PackageImport,
			Null,

			TypeCountPlusOne,
			TypeCount = TypeCountPlusOne - 1
		};

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FPackageObjectIndex& Value) {
			InputStream >> Value.TypeAndId;

			return InputStream;
		}

		auto operator<=>(const FPackageObjectIndex&) const = default;

		EType GetType() const {
			return EType(TypeAndId >> 62);
		}

		bool IsNull() const {
			return TypeAndId == -1;
		}

		bool IsExport() const {
			return GetType() == Export;
		}

		bool IsImport() const {
			return IsScriptImport() || IsPackageImport();
		}

		bool IsScriptImport() const {
			return GetType() == ScriptImport;
		}

		bool IsPackageImport() const {
			return GetType() == PackageImport;
		}

		uint32_t ToExport() const {
			return TypeAndId;
		}

		uint64_t Value() const {
			return TypeAndId & ((1ull << 62) - 1);
		}
	};
}
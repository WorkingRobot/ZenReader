#pragma once

#include "../Streams/BaseStream.h"
#include "FMappedName.h"
#include "FPackageObjectIndex.h"

namespace Zen::Structs {
	class FScriptObjectEntry {
	public:
		// FMappedName <=> FMinimalName
		// https://github.com/EpicGames/UnrealEngine/blob/405a4f6316f68655bcf48cf7eadd7796e74a5242/Engine/Source/Runtime/CoreUObject/Private/Serialization/AsyncLoading2.cpp#L105
		// https://github.com/EpicGames/UnrealEngine/blob/ac1f4d53af58d8d00f6e2f7124f384d67bcb53c9/Engine/Source/Runtime/CoreUObject/Public/Serialization/AsyncLoading2.h#L47
		FMappedName ObjectName;
		FPackageObjectIndex GlobalIndex;
		FPackageObjectIndex OuterIndex;
		FPackageObjectIndex CDOClassIndex;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FScriptObjectEntry& Value) {
			InputStream >> Value.ObjectName;
			InputStream >> Value.GlobalIndex;
			InputStream >> Value.OuterIndex;
			InputStream >> Value.CDOClassIndex;

			return InputStream;
		}
	};
}
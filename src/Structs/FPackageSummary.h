#pragma once

#include "../Streams/BaseStream.h"
#include "FMappedName.h"

namespace Zen::Structs {
	class FPackageSummary {
	public:
		FMappedName Name;
		FMappedName SourceName;
		uint32_t PackageFlags;
		uint32_t CookedHeaderSize;
		int32_t NameMapNamesOffset;
		int32_t NameMapNamesSize;
		int32_t NameMapHashesOffset;
		int32_t NameMapHashesSize;
		int32_t ImportMapOffset;
		int32_t ExportMapOffset;
		int32_t ExportBundlesOffset;
		int32_t GraphDataOffset;
		int32_t GraphDataSize;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FPackageSummary& Value)
		{
			InputStream >> Value.Name;
			InputStream >> Value.SourceName;
			InputStream >> Value.PackageFlags;
			InputStream >> Value.CookedHeaderSize;
			InputStream >> Value.NameMapNamesOffset;
			InputStream >> Value.NameMapNamesSize;
			InputStream >> Value.NameMapHashesOffset;
			InputStream >> Value.NameMapHashesSize;
			InputStream >> Value.ImportMapOffset;
			InputStream >> Value.ExportMapOffset;
			InputStream >> Value.ExportBundlesOffset;
			InputStream >> Value.GraphDataOffset;
			InputStream >> Value.GraphDataSize;
			// Literal padding value: https://github.com/EpicGames/UnrealEngine/blob/2511e56b6a38db820b96361c1961f5c32d1f11b4/Engine/Source/Runtime/CoreUObject/Public/Serialization/AsyncLoading2.h#L252
			InputStream.seek(4, Streams::BaseStream::Cur);

			return InputStream;
		}
	};
}
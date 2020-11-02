#pragma once

#include "../Enums/EObjectFlags.h"
#include "../Enums/EExportFilterFlags.h"
#include "../Streams/BaseStream.h"
#include "FMappedName.h"
#include "FPackageObjectIndex.h"

namespace Zen::Structs {
	// https://github.com/EpicGames/UnrealEngine/blob/5b0cfe38e0faf3c2f9938d8f6a1cb5805802a180/Engine/Source/Runtime/CoreUObject/Public/Serialization/AsyncLoading2.h#L328
	class FExportMapEntry {
	public:
		uint64_t CookedSerialOffset;
		uint64_t CookedSerialSize;
		FMappedName ObjectName;
		FPackageObjectIndex OuterIndex;
		FPackageObjectIndex ClassIndex;
		FPackageObjectIndex SuperIndex;
		FPackageObjectIndex TemplateIndex;
		FPackageObjectIndex GlobalImportIndex;
		Enums::EObjectFlags ObjectFlags;
		Enums::EExportFilterFlags FilterFlags;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FExportMapEntry& Value) {
			InputStream >> Value.CookedSerialOffset;
			InputStream >> Value.CookedSerialSize;
			InputStream >> Value.ObjectName;
			InputStream >> Value.OuterIndex;
			InputStream >> Value.ClassIndex;
			InputStream >> Value.SuperIndex;
			InputStream >> Value.TemplateIndex;
			InputStream >> Value.GlobalImportIndex;
			uint32_t ObjectFlags;
			InputStream >> ObjectFlags;
			Value.ObjectFlags = (Enums::EObjectFlags)ObjectFlags;
			uint8_t FilterFlags;
			InputStream >> FilterFlags;
			Value.FilterFlags = (Enums::EExportFilterFlags)FilterFlags;
			InputStream.seek(3, BaseStream::Cur);

			return InputStream;
		}
	};
}
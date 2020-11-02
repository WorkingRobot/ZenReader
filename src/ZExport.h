#pragma once

#include "ZFile.h"
#include "ZGlobalData.h"
#include "ZNameMap.h"
#include "Structs/FPackageSummary.h"
#include "Structs/FSerializedNameHeader.h"
#include "Structs/FNameEntrySerialized.h"
#include "Structs/FPackageObjectIndex.h"
#include "Structs/FExportMapEntry.h"
#include "Structs/FPackageId.h"
#include "Structs/FArc.h"

#include <functional>

namespace Zen {
	using namespace Structs;
	class ZExport {
	public:
		// https://github.com/EpicGames/UnrealEngine/blob/15690f68b984ec2fb9269dd59ff9c3c42b77bd67/Engine/Source/Developer/IoStoreUtilities/Private/IoStoreUtilities.cpp#L929
		ZExport(std::function<ZFile*(const char*)> GetExtStream, const ZGlobalData& GlobalData) {
			auto UAssetFile = GetExtStream("uasset");
			if (!UAssetFile) {
				return;
			}
			auto UAsset = UAssetFile->GetStream();

			UAsset >> Summary;
			
			if (Summary.NameMapNamesSize) {
				auto NamesBuffer = std::make_unique<char[]>(Summary.NameMapNamesSize);
				auto HashesBuffer = std::make_unique<char[]>(Summary.NameMapHashesSize);

				UAsset.seek(Summary.NameMapNamesOffset, BaseStream::Beg);
				UAsset.read(NamesBuffer.get(), Summary.NameMapNamesSize);

				UAsset.seek(Summary.NameMapHashesOffset, BaseStream::Beg);
				UAsset.read(HashesBuffer.get(), Summary.NameMapHashesSize);

				NameMap = ZNameMap(NamesBuffer.get(), Summary.NameMapNamesSize, HashesBuffer.get(), Summary.NameMapHashesSize);
			}

			auto ImportCount = (Summary.ExportMapOffset - Summary.ImportMapOffset) / sizeof(FPackageObjectIndex);
			ImportMap.resize(ImportCount);
			UAsset.seek(Summary.ImportMapOffset, BaseStream::Beg);
			for (int i = 0; i < ImportCount; ++i) {
				UAsset >> ImportMap[i];
			}

			static_assert(sizeof(FExportMapEntry) == 72, "FExportMapEntry must be 72 bytes (perhaps your compiler alignment is off?)");
			auto ExportCount = (Summary.ExportBundlesOffset - Summary.ExportMapOffset) / sizeof(FExportMapEntry);
			ExportMap.resize(ExportCount);
			UAsset.seek(Summary.ExportMapOffset, BaseStream::Beg);
			for (int i = 0; i < ExportCount; ++i) {
				UAsset >> ExportMap[i];
			}

			UAsset.seek(Summary.GraphDataOffset, BaseStream::Beg);
			int32_t ReferencedPackageCount;
			UAsset >> ReferencedPackageCount;
			for (int i = 0; i < ReferencedPackageCount; ++i) {
				FPackageId ImportedPackageId;
				UAsset >> ImportedPackageId;
				std::vector<FArc> Arcs; // unused?
				UAsset >> Arcs;
				printf("some arc stuff, breaking here\n");
			}

			for (auto& Export : ExportMap) {
				// GetEntry is always global, it's in the global utoc, anyway, so it's safe to ask the global namemap directly
				auto& Name = GlobalData.NameMap.GetName(GlobalData.GetEntry(Export.ClassIndex).ObjectName).Name;
				
				printf("%s\n", Name.c_str());
			}
		}

	private:
		FPackageSummary Summary;

		ZNameMap NameMap;
		std::vector<FPackageObjectIndex> ImportMap;
		std::vector<FExportMapEntry> ExportMap;
	};
}
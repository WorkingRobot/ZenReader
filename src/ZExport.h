#pragma once

#include "Exceptions/BaseException.h"
#include "Exports/Serializer.h"
#include "Providers/Base.h"
#include "Streams/BufferedStream.h"
#include "Structs/FPackageSummary.h"
#include "Structs/FPackageIndex.h"
#include "Structs/FPackageObjectIndex.h"
#include "Structs/FExportMapEntry.h"
#include "Structs/FPackageId.h"
#include "Structs/FArc.h"
#include "ZFile.h"
#include "ZGlobalData.h"
#include "ZNameMap.h"

#include <any>
#include <functional>

namespace Zen {
	using namespace Exceptions;
	using namespace Structs;

	class ZExport {
	public:
		// https://github.com/EpicGames/UnrealEngine/blob/15690f68b984ec2fb9269dd59ff9c3c42b77bd67/Engine/Source/Developer/IoStoreUtilities/Private/IoStoreUtilities.cpp#L929
		ZExport(std::function<const ZFile*(const char*)> GetExtStream, const ZGlobalData& GlobalData, const Providers::BaseProvider& SchemaProvider) {
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
			}

			Exports.reserve(ExportCount);
			auto ExportOffset = Summary.GraphDataOffset + Summary.GraphDataSize;
			PropertyHolder<PropId::ZExport> UAssetInternalData(UAsset, (void*)this);
			PropertyHolder<PropId::GlobalData> UAssetGlobalData(UAsset, (void*)&GlobalData);
			PropertyHolder<PropId::Provider> UAssetProvider(UAsset, (void*)&SchemaProvider);
			for (auto& Export : ExportMap) {
				UAsset.seek(ExportOffset, BaseStream::Beg);

				auto& ExportType = GlobalData.GetEntryName(Export.ClassIndex);
				
				Exports.emplace_back(Exports::Serialize(UAsset, ExportType));

				ExportOffset += Export.CookedSerialSize;
			}
		}

		template<class T>
		const T* Get() const {
			for (auto& Export : Exports) {
				auto ret = std::any_cast<std::shared_ptr<T>>(&Export);
				if (ret) {
					return ret->get();
				}
			}
			return nullptr;
		}

		const ZNameMap& GetNameMap() const {
			return NameMap;
		}

		const FPackageObjectIndex& GetImport(int i) const {
			return ImportMap[i];
		}

		const FExportMapEntry& GetExport(int i) const {
			return ExportMap[i];
		}

		const FPackageObjectIndex& GetImport(const FPackageIndex& i) const {
			if (!i.IsImport()) {
				throw ArchiveCorruptedException("FPackageIndex must be an import");
			}
			return GetImport(i.ToImport());
		}

		const FExportMapEntry& GetExport(const FPackageIndex& i) const {
			if (!i.IsExport()) {
				throw ArchiveCorruptedException("FPackageIndex must be an export");
			}
			return GetExport(i.ToExport());
		}

	private:
		std::vector<std::any> Exports;

		FPackageSummary Summary;

		ZNameMap NameMap;
		std::vector<FPackageObjectIndex> ImportMap;
		std::vector<FExportMapEntry> ExportMap;
	};
}
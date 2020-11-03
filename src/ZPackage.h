#pragma once

#include "ZSmallMap.h"
#include "ZFile.h"
#include "ZExport.h"
#include "Enums/EAssetType.h"

namespace Zen {
	template<typename Key>
	class ZPackage {
	public:
		template<bool CheckIfAlreadyExists, typename... Args>
		ZFile& AddFile(const char* Extension, typename Key::KeySize ExtensionSize, Args&&... FileArgs) {
			auto AssetType = GetAssetType(Extension, ExtensionSize);
			if (AssetType != EAssetType::UNKNOWN) {
				if constexpr (CheckIfAlreadyExists) {
					auto ChildIter = std::find_if(KnownExts.begin(), KnownExts.end(), [AssetType](const auto& ExtPair) { return ExtPair.first == AssetType; });
					if (ChildIter != KnownExts.end()) {
						return ChildIter->second;
					}
				}
				KnownExts.emplace_back(std::make_pair(AssetType, ZFile(std::forward<Args>(FileArgs)...))).second;
			}

			if (!MiscExts) {
				MiscExts = std::make_unique<MiscExtMap>();
			}
			if constexpr (CheckIfAlreadyExists) {
				auto ChildIter = SearchValues(MiscExts->ExtHashes, MiscExts->Exts, Extension, ExtensionSize);
				if (ChildIter != MiscExts->Exts.end()) {
					return ChildIter->second;
				}
			}
			MiscExts->ExtHashes.emplace_back(Helpers::Hash::Crc32(Extension, ExtensionSize));
			return MiscExts->Exts.emplace_back(std::make_pair(Key(Extension, ExtensionSize), ZFile(std::forward<Args>(FileArgs)...))).second;
		}

		ZFile* GetFile(const char* Extension) {
			auto AssetType = GetAssetType(Extension, strlen(Extension));
			if (AssetType != EAssetType::UNKNOWN) {
				auto ChildIter = std::find_if(KnownExts.begin(), KnownExts.end(), [AssetType](const auto& ExtPair) { return ExtPair.first == AssetType; });
				if (ChildIter != KnownExts.end()) {
					return &ChildIter->second;
				}
			}
			else if (MiscExts) {
				auto ChildIter = SearchValues(MiscExts->ExtHashes, MiscExts->Exts, Extension, strlen(Extension));
				if (ChildIter != MiscExts->Exts.end()) {
					return &ChildIter->second;
				}
			}
			return nullptr;
		}

		ZExport GetExport(const ZGlobalData& GlobalData) {
			return ZExport([this](const char* Extension) {
				return GetFile(Extension);
			}, GlobalData);
		}

	private:
		static constexpr EAssetType GetAssetType(const char* Extension, typename Key::KeySize ExtensionSize) {
			switch (Helpers::Hash::Crc32<true>(Extension, ExtensionSize))
			{
#define CASE(Type) case Helpers::Hash::Crc32<true>(#Type): return EAssetType::Type

				CASE(UASSET);
				CASE(UEXP);
				CASE(UBULK);
				CASE(UMAP);
				CASE(UPROJECT);
				CASE(UPLUGINMANIFEST);
				CASE(UPLUGIN);
				CASE(USHADERBYTECODE);
				CASE(UPIPELINECACHE);
				CASE(UDIC);
				CASE(LOCMETA);
				CASE(LOCRES);
				CASE(PNG);
				CASE(PSD);
				CASE(UFONT);
				CASE(TTF);
				CASE(TPS);
				CASE(ICU);
				CASE(RES);
				CASE(CFU);
				CASE(NRM);
				CASE(BRK);
				CASE(DICT);
				CASE(BIN);
				CASE(INI);
				CASE(PEM);
				CASE(K);

#undef CASE
			default:
				return EAssetType::UNKNOWN;
			}
		}

		struct MiscExtMap {
			std::vector<uint32_t> ExtHashes;
			std::vector<std::pair<Key, ZFile>> Exts;
		};

		std::vector<std::pair<EAssetType, ZFile>> KnownExts;
		std::unique_ptr<MiscExtMap> MiscExts;
	};
}
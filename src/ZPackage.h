#pragma once

#include "ZSmallMap.h"
#include "ZFile.h"
#include "ZExport.h"
#include "Enums/EAssetType.h"
#include "Providers/Base.h"

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
				MiscExts = std::make_unique<ZSmallMap<Key, ZFile>>();
			}
			if constexpr (CheckIfAlreadyExists) {
				auto ChildIter = MiscExts->SearchValues(Extension, ExtensionSize);
				if (ChildIter != MiscExts->end()) {
					return ChildIter->second;
				}
			}

			return MiscExts->emplace_back(Extension, ExtensionSize, ZFile(std::forward<Args>(FileArgs)...));
		}

		const ZFile* TryGetFile(const char* Extension) const {
			auto AssetType = GetAssetType(Extension, strlen(Extension));
			if (AssetType != EAssetType::UNKNOWN) {
				auto ChildIter = std::find_if(KnownExts.begin(), KnownExts.end(), [AssetType](const auto& ExtPair) { return ExtPair.first == AssetType; });
				if (ChildIter != KnownExts.end()) {
					return &ChildIter->second;
				}
			}
			else if (MiscExts) {
				auto ChildIter = MiscExts->SearchValues(Extension, strlen(Extension));
				if (ChildIter != MiscExts->end()) {
					return &ChildIter->second;
				}
			}
			return nullptr;
		}

		const ZFile& GetFile(const char* Extension) const {
			return *TryGetFile(Extension);
		}

		ZExport GetExport(const ZGlobalData& GlobalData, const Providers::BaseProvider& SchemaProvider) const {
			return ZExport([this](const char* Extension) {
				return TryGetFile(Extension);
			}, GlobalData, SchemaProvider);
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

		std::vector<std::pair<EAssetType, ZFile>> KnownExts;
		std::unique_ptr<ZSmallMap<Key, ZFile>> MiscExts;
	};
}
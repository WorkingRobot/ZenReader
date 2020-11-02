#pragma once

#include "ZSmallMap.h"
#include "ZFile.h"
#include "ZExport.h"

namespace Zen {
	template<typename Key = PrefixedKey<uint8_t>>
	class ZPackage {
	public:
		template<bool CheckIfAlreadyExists, typename... Args>
		ZFile& AddFile(const char* Extension, typename Key::KeySize ExtensionSize, Args&&... FileArgs) {
			if constexpr (CheckIfAlreadyExists) {
				auto ChildIter = SearchValues(ExtHashes, Exts, Extension, ExtensionSize);
				if (ChildIter != Exts.end()) {
					return ChildIter->second;
				}
			}
			ExtHashes.emplace_back(Helpers::Hash::Crc32(Extension, ExtensionSize));
			return Exts.emplace_back(std::make_pair(Key(Extension, ExtensionSize), ZFile(std::forward<Args>(FileArgs)...))).second;
		}

		ZFile* GetFile(const char* Extension) {
			auto ChildIter = SearchValues(ExtHashes, Exts, Extension, strlen(Extension));
			if (ChildIter != Exts.end()) {
				return &ChildIter->second;
			}
			return nullptr;
		}

		ZExport GetExport(const ZGlobalData& GlobalData) {
			return ZExport([this](const char* Extension) {
				return GetFile(Extension);
			}, GlobalData);
		}

	private:
		std::vector<uint32_t> ExtHashes;
		std::vector<std::pair<Key, ZFile>> Exts;
	};
}
#pragma once

#include "ZSmallMap.h"
#include "ZFile.h"

namespace Zen {
	template<class Key = PrefixedKey<uint8_t>>
	class ZPackage {
	public:
		template<bool CheckIfAlreadyExists, class KeySize = Key::KeySize, class... Args>
		ZFile& AddFile(const char* Extension, KeySize ExtensionSize, Args&&... FileArgs) {
			if constexpr (CheckIfAlreadyExists) {
				auto ChildIter = SearchValues(ExtHashes, Exts, Extension, ExtensionSize);
				if (ChildIter != Exts.end()) {
					return ChildIter->second;
				}
			}
			ExtHashes.emplace_back(Helpers::Hash::Crc32(Extension, ExtensionSize));
			return Exts.emplace_back(std::make_pair(Key(Extension, ExtensionSize), ZFile(std::forward<Args>(FileArgs)...))).second;
		}

	private:
		std::vector<uint32_t> ExtHashes;
		std::vector<std::pair<Key, ZFile>> Exts;
	};
}
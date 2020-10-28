#pragma once

#include "ZPackage.h"
#include "ZSmallMap.h"
#include "Helpers/Hash.h"

namespace Zen {
	// This is a super memory-optimized tree to help lower RAM usage while still keeping some level of speed
	// uint8_t is best, but might be unsafe (if your game's developer is really bad at naming things). StrlenKey is faster for some reason though?
	template<class Key = StrlenKey<uint8_t>>
	class ZFileTree {
	public:
		ZFileTree()
		{

		}

		~ZFileTree()
		{

		}

		ZFileTree(ZFileTree&&) = default;

		template<bool CheckIfAlreadyExists, class KeySize = Key::KeySize>
		ZFileTree& AddFolders(const char* MountPoint) {
			if (!*MountPoint) {
				return *this;
			}
			auto NextSeparator = strchr(MountPoint, '/');
			return AddFolder<CheckIfAlreadyExists>(MountPoint, NextSeparator - MountPoint).AddFolders<CheckIfAlreadyExists>(NextSeparator + 1);
		}

		template<bool CheckIfAlreadyExists, class KeySize = Key::KeySize>
		ZFileTree& AddFolder(const char* FolderName, KeySize NameSize) {
			if constexpr (CheckIfAlreadyExists) {
				auto ChildIter = SearchValues(FolderHashes, Folders, FolderName, NameSize);
				if (ChildIter != Folders.end()) {
					return ChildIter->second;
				}
			}
			FolderHashes.emplace_back(Helpers::Hash::Crc32(FolderName, NameSize));
			return Folders.emplace_back(Key(FolderName, NameSize), ZFileTree()).second;
		}

		template<bool CheckIfAlreadyExists, class KeySize = Key::KeySize>
		ZPackage<Key>& AddPackage(const char* PackageName, KeySize NameSize) {
			if constexpr (CheckIfAlreadyExists) {
				auto ChildIter = SearchValues(FileHashes, Files, PackageName, NameSize);
				if (ChildIter != Files.end()) {
					return ChildIter->second;
				}
			}
			FileHashes.emplace_back(Helpers::Hash::Crc32(PackageName, NameSize));
			return Files.emplace_back(Key(PackageName, NameSize), ZPackage<Key>()).second;
		}

		// TODO: make const, compiler errors Sadge
		ZPackage<Key>* TryGetPackage(const char* Path) {
			const char* Separator = strchr(Path, '/');
			if (Separator) {
				auto ChildIter = SearchValues(FolderHashes, Folders, Path, Separator - Path);
				if (ChildIter != Folders.end()) {
					return ChildIter->second.TryGetPackage(Separator + 1);
				}
			}
			else {
				auto ChildIter = SearchValues(FileHashes, Files, Path, strlen(Path));
				if (ChildIter != Files.end()) {
					return &ChildIter->second;
				}
			}
			return nullptr;
		}

	private:
		std::vector<uint32_t> FolderHashes;
		std::vector<uint32_t> FileHashes;
		std::vector<std::pair<Key, ZFileTree>> Folders;
		std::vector<std::pair<Key, ZPackage<Key>>> Files;
	};
}
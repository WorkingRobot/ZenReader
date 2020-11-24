#pragma once

#include "ZPackage.h"
#include "ZSmallMap.h"
#include "Helpers/Hash.h"

namespace Zen {
	// This is a super memory-optimized tree to help lower RAM usage while still keeping some level of speed
	// uint8_t is best, but might be unsafe (if your game's developer is really bad at naming things). StrlenKey is faster for some reason though?
	template<typename Key = StrlenKey<uint8_t>>
	class ZFileTree {
	public:
		ZFileTree()
		{

		}

		~ZFileTree()
		{

		}

		ZFileTree(ZFileTree&&) = default;

		template<bool CheckIfAlreadyExists>
		ZFileTree& AddFolders(const char* MountPoint) {
			if (!*MountPoint) {
				return *this;
			}
			auto NextSeparator = strchr(MountPoint, '/');
			return AddFolder<CheckIfAlreadyExists>(MountPoint, NextSeparator - MountPoint).template AddFolders<CheckIfAlreadyExists>(NextSeparator + 1);
		}

		template<bool CheckIfAlreadyExists>
		ZFileTree& AddFolder(const char* FolderName, typename Key::KeySize NameSize) {
			if constexpr (CheckIfAlreadyExists) {
				auto ChildIter = Folders.SearchValues(FolderName, NameSize);
				if (ChildIter != Folders.end()) {
					return ChildIter->second;
				}
			}

			return Folders.emplace_back(FolderName, NameSize, ZFileTree());
		}

		template<bool CheckIfAlreadyExists>
		ZPackage<Key>& AddPackage(const char* PackageName, typename Key::KeySize NameSize) {
			if constexpr (CheckIfAlreadyExists) {
				auto ChildIter = Files.SearchValues(PackageName, NameSize);
				if (ChildIter != Files.end()) {
					return ChildIter->second;
				}
			}

			return Files.emplace_back(PackageName, NameSize, ZPackage<Key>());
		}

		// TODO: make const, compiler errors Sadge
		ZPackage<Key>* TryGetPackage(const char* Path) {
			const char* Separator = strchr(Path, '/');
			if (Separator) {
				auto ChildIter = Folders.SearchValues(Path, Separator - Path);
				if (ChildIter != Folders.end()) {
					return ChildIter->second.TryGetPackage(Separator + 1);
				}
			}
			else {
				auto ChildIter = Files.SearchValues(Path, strlen(Path));
				if (ChildIter != Files.end()) {
					return &ChildIter->second;
				}
			}
			return nullptr;
		}

	private:
		ZSmallMap<Key, ZFileTree> Folders;
		ZSmallMap<Key, ZPackage<Key>> Files;
	};
}
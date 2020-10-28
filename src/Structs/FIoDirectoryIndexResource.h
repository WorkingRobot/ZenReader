#pragma once

#include "../Streams/BaseStream.h"
#include "FIoDirectoryIndexEntry.h"
#include "FIoFileIndexEntry.h"

#include <filesystem>

namespace Zen::Structs {
	class FIoDirectoryIndexResource {
	public:
		std::string MountPoint;
		std::vector<FIoDirectoryIndexEntry> DirectoryEntries;
		std::vector<FIoFileIndexEntry> FileEntries;
		std::vector<std::string> StringTable;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoDirectoryIndexResource& Value) {
			InputStream >> Value.MountPoint;
			if (Value.MountPoint.starts_with("../../..")) {
				Value.MountPoint = Value.MountPoint.substr(9);
			}
			InputStream >> Value.DirectoryEntries;
			InputStream >> Value.FileEntries;
			InputStream >> Value.StringTable;

			return InputStream;
		}

		// Sorry in advance for all the std::forward, it's done to ensure rvalue refs stay as rvalue refs
		// The std::make_unique does that, for example
		template<class FileTree, class... Args>
		void ReadIndex(FileTree& Tree, Args&&... args) {
			ReadIndex<FileTree>(Tree.AddFolders<true>(MountPoint.c_str()), DirectoryEntries[0].FirstChildEntry, std::forward<Args>(args)...);
		}

		template<class FileTree, class... Args>
		void ReadIndex(FileTree& Tree, uint32_t DirIdx, Args&&... FileArgs) {
			while (DirIdx != UINT32_MAX) {
				auto& Dir = DirectoryEntries[DirIdx];
				auto& DirName = GetString(Dir.Name);
				auto& DirTree = Tree.AddFolder<true>(DirName.c_str(), DirName.size());

				uint32_t FileIdx = Dir.FirstFileEntry;
				while (FileIdx != UINT32_MAX) {
					auto& File = FileEntries[FileIdx];
					auto& FileName = GetString(File.Name);
					auto Dot = FileName.rfind('.');
					if (Dot != std::string::npos) {
						auto& Pkg = DirTree.AddPackage<true>(FileName.c_str(), Dot);
						Pkg.AddFile<false>(FileName.c_str() + Dot + 1, FileName.size() - Dot - 1, File.UserData, std::forward<Args>(FileArgs)...);
					}
					else {
						auto& Pkg = DirTree.AddPackage<true>(FileName.c_str(), FileName.size());
						Pkg.AddFile<false>(NULL, 0, File.UserData, std::forward<Args>(FileArgs)...);
					}

					FileIdx = File.NextFileEntry;
				}

				ReadIndex<FileTree>(DirTree, Dir.FirstChildEntry, std::forward<Args>(FileArgs)...);

				DirIdx = Dir.NextSiblingEntry;
			}
		}

	private:
		const std::string& GetString(uint32_t StringIdx, const std::string& Default = "") {
			return StringIdx >= StringTable.size() ? Default : StringTable[StringIdx];
		}

		//std::filesystem::path BasePath;
	};
}
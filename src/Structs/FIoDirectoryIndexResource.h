#pragma once

#include "../Streams/BaseStream.h"
#include "../Structs/FIoDirectoryIndexEntry.h"
#include "../Structs/FIoFileIndexEntry.h"

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

		void ReadIndex(const std::filesystem::path& BasePath = "", uint32_t DirIdx = 0) {
			while (DirIdx != UINT32_MAX) {
				auto& Dir = DirectoryEntries[DirIdx];
				auto DirectoryPath = BasePath / GetString(Dir.Name);

				uint32_t FileIdx = Dir.FirstFileEntry;
				while (FileIdx != UINT32_MAX) {
					auto& File = FileEntries[FileIdx];
					auto FilePath = DirectoryPath / GetString(File.Name);

					printf("%s: %zu\n", FilePath.string().c_str(), File.UserData);

					FileIdx = File.NextFileEntry;
				}

				ReadIndex(DirectoryPath, Dir.FirstChildEntry);

				DirIdx = Dir.NextSiblingEntry;
			}
		}

	private:
		const std::string& GetString(uint32_t StringIdx, const std::string& Default = "") {
			return StringIdx >= StringTable.size() ? Default : StringTable[StringIdx];
		}
	};
}
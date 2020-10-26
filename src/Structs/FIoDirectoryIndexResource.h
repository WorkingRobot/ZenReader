#pragma once

#include "../Streams/BaseStream.h"
#include "../Structs/FIoDirectoryIndexEntry.h"
#include "../Structs/FIoFileIndexEntry.h"

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

			auto DirEntryOffset = InputStream.tell();
			int DirEntryCount;
			InputStream >> DirEntryCount;
			InputStream.seek(DirEntryCount * 16, BaseStream::Cur);
			int FileEntryCount;
			InputStream >> FileEntryCount;
			InputStream.seek(FileEntryCount * 12, BaseStream::Cur);

			InputStream >> Value.StringTable;
			auto EndingOffset = InputStream.tell();

			PropertyHolder<PropId::DirIndexStringTable> Holder(InputStream, &Value.StringTable);
			InputStream.seek(DirEntryOffset, BaseStream::Beg);
			InputStream >> Value.DirectoryEntries;
			InputStream >> Value.FileEntries;
			InputStream.seek(EndingOffset, BaseStream::Beg);
			return InputStream;
		}
	};
}
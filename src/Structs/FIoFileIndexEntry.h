#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FIoFileIndexEntry {
	public:
		std::string* Name;
		std::string* NextFileEntry;
		std::string* UserData;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoFileIndexEntry& Value) {
			auto Table = (std::vector<std::string>*)InputStream.GetProperty<PropId::DirIndexStringTable>();
			uint32_t Name, NextFileEntry, UserData;
			InputStream >> Name;
			InputStream >> NextFileEntry;
			InputStream >> UserData;

			Value.Name = Name == -1 ? nullptr : &Table->at(Name);
			Value.NextFileEntry = NextFileEntry == -1 ? nullptr : &Table->at(NextFileEntry);
			Value.UserData = UserData == -1 ? nullptr : &Table->at(UserData);
			return InputStream;
		}
	};
}
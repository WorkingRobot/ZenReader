#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FIoDirectoryIndexEntry {
	public:
		std::string* Name;
		std::string* FirstChildEntry;
		std::string* NextSiblingEntry;
		std::string* FirstFileEntry;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoDirectoryIndexEntry& Value) {
			auto Table = (std::vector<std::string>*)InputStream.GetProperty<PropId::DirIndexStringTable>();
			uint32_t Name, FirstChildEntry, NextSiblingEntry, FirstFileEntry;
			InputStream >> Name;
			InputStream >> FirstChildEntry;
			InputStream >> NextSiblingEntry;
			InputStream >> FirstFileEntry;

			Value.Name = Name == -1 ? nullptr : &Table->at(Name);
			Value.FirstChildEntry = FirstChildEntry == -1 ? nullptr : &Table->at(FirstChildEntry);
			Value.NextSiblingEntry = NextSiblingEntry == -1 ? nullptr : &Table->at(NextSiblingEntry);
			Value.FirstFileEntry = FirstFileEntry == -1 ? nullptr : &Table->at(FirstFileEntry);
			return InputStream;
		}
	};
}
#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FIoDirectoryIndexEntry {
	public:
		uint32_t Name;
		uint32_t FirstChildEntry;
		uint32_t NextSiblingEntry;
		uint32_t FirstFileEntry;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoDirectoryIndexEntry& Value) {
			InputStream >> Value.Name;
			InputStream >> Value.FirstChildEntry;
			InputStream >> Value.NextSiblingEntry;
			InputStream >> Value.FirstFileEntry;

			return InputStream;
		}
	};
}
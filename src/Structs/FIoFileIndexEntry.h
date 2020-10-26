#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FIoFileIndexEntry {
	public:
		uint32_t Name;
		uint32_t NextFileEntry;
		uint32_t UserData;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoFileIndexEntry& Value) {
			InputStream >> Value.Name;
			InputStream >> Value.NextFileEntry;
			InputStream >> Value.UserData;

			return InputStream;
		}
	};
}
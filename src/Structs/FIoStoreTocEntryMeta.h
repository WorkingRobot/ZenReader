#pragma once

#include "../Enums/FIoStoreTocEntryMetaFlags.h"
#include "../Streams/BaseStream.h"
#include "../Structs/FIoChunkHash.h"

namespace Zen::Structs {
	class FIoStoreTocEntryMeta {
	public:
		FIoChunkHash ChunkHash;
		FIoStoreTocEntryMetaFlags Flags;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoStoreTocEntryMeta& Value) {
			InputStream >> Value.ChunkHash;
			uint8_t Flags;
			InputStream >> Flags;
			Value.Flags = (FIoStoreTocEntryMetaFlags)Flags;

			return InputStream;
		}
	};
}
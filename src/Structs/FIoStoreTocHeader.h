#pragma once

#include "../Streams/BaseStream.h"
#include "../Exceptions/BaseException.h"
#include "../Enums/EIoStoreTocVersion.h"
#include "../Enums/EIoContainerFlags.h"
#include "FGuid.h"
#include "FIoContainerId.h"

namespace Zen::Structs {
	using namespace Enums;
	using namespace Exceptions;
	using namespace Streams;

	class FIoStoreTocHeader {
	public:
		static constexpr char TocMagicImg[] = "-==--==--==--==-";

		uint8_t TocMagic[16];
		EIoStoreTocVersion Version;
		uint32_t TocHeaderSize;
		uint32_t TocEntryCount;
		uint32_t TocCompressedBlockEntryCount;
		uint32_t TocCompressedBlockEntrySize;
		uint32_t CompressionMethodNameCount;
		uint32_t CompressionMethodNameLength;
		uint32_t CompressionBlockSize;
		uint32_t DirectoryIndexSize;
		FIoContainerId ContainerId;
		FGuid EncryptionKeyGuid;
		EIoContainerFlags ContainerFlags;

		// I still don't see any reason why you made this code in UE4 not portable at all
		// I thought we moved on from reading and writing to direct pointers
		friend BaseStream& operator>>(BaseStream& InputStream, FIoStoreTocHeader& Value) {
			InputStream >> Value.TocMagic;
			if (memcmp(Value.TocMagic, FIoStoreTocHeader::TocMagicImg, 16)) {
				throw InvalidMagicException("UToc file has an invalid magic constant");
			}
			int32_t Version; // The 1 byte enums are 32 bit because of alignement
			InputStream >> Version;
			Value.Version = (EIoStoreTocVersion)Version;
			InputStream >> Value.TocHeaderSize;
			InputStream >> Value.TocEntryCount;
			InputStream >> Value.TocCompressedBlockEntryCount;
			InputStream >> Value.TocCompressedBlockEntrySize;
			InputStream >> Value.CompressionMethodNameCount;
			InputStream >> Value.CompressionMethodNameLength;
			InputStream >> Value.CompressionBlockSize;
			InputStream >> Value.DirectoryIndexSize;
			InputStream.seek(4, BaseStream::Cur); // ContinerId is 8 byte aligned
			InputStream >> Value.ContainerId;
			InputStream >> Value.EncryptionKeyGuid;
			int32_t ContainerFlags;
			InputStream >> ContainerFlags;
			Value.ContainerFlags = (EIoContainerFlags)ContainerFlags;
			InputStream.seek(60, BaseStream::Cur);

			return InputStream;
		}
	};
}
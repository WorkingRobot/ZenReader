#pragma once

#include "../Helpers/AES.h"
#include "../Streams/BufferStream.h"
#include "FIoStoreTocHeader.h"
#include "FIoChunkId.h"
#include "FIoChunkOffsetAndLength.h"
#include "FIoStoreTocCompressedBlockEntry.h"
#include "FSHAHash.h"
#include "FIoStoreTocEntryMeta.h"
#include "FIoDirectoryIndexResource.h"

#include <filesystem>

namespace Zen::Structs {

	class FIoStoreTocResource {
	public:
		FIoStoreTocHeader Header;
		std::vector<FIoChunkId> ChunkIds;
		std::vector<FIoChunkOffsetAndLength> ChunkOffsetLengths;
		std::vector<FIoStoreTocCompressedBlockEntry> CompressionBlocks;
		std::vector<std::string> CompressionMethodNames;
		std::unique_ptr<char[]> TocSignature;
		std::unique_ptr<char[]> BlockSignature;
		std::vector<FSHAHash> ChunkBlockSignatures;
		std::unique_ptr<char[]> DirectoryBuffer;
		std::vector<FIoStoreTocEntryMeta> ChunkMetas;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& Stream, FIoStoreTocResource& Value)
		{
			Stream >> Value.Header;

			Value.ChunkIds.reserve(Value.Header.TocEntryCount);
			for (int i = 0; i < Value.Header.TocEntryCount; ++i) {
				Stream >> Value.ChunkIds.emplace_back();
			}

			Value.ChunkOffsetLengths.reserve(Value.Header.TocEntryCount);
			for (int i = 0; i < Value.Header.TocEntryCount; ++i) {
				Stream >> Value.ChunkOffsetLengths.emplace_back();
			}

			Value.CompressionBlocks.reserve(Value.Header.TocCompressedBlockEntryCount);
			for (int i = 0; i < Value.Header.TocCompressedBlockEntryCount; ++i) {
				Stream >> Value.CompressionBlocks.emplace_back();
			}

			Value.CompressionMethodNames.reserve(Value.Header.CompressionMethodNameCount);
			{
				auto Methods = std::make_unique<char[]>(Value.Header.CompressionMethodNameCount * Value.Header.CompressionMethodNameLength);
				Stream.read(Methods.get(), Value.Header.CompressionMethodNameCount * Value.Header.CompressionMethodNameLength);
				for (int i = 0; i < Value.Header.CompressionMethodNameCount; ++i) {
					Value.CompressionMethodNames.emplace_back(Methods.get() + Value.Header.CompressionMethodNameLength * i);
				}
			}

			if (Value.Header.ContainerFlags & EIoContainerFlags::Signed) {
				int32_t HashSize;
				Stream >> HashSize;
				Value.TocSignature = std::make_unique<char[]>(HashSize);
				Value.BlockSignature = std::make_unique<char[]>(HashSize);
				Stream.read(Value.TocSignature.get(), HashSize);
				Stream.read(Value.BlockSignature.get(), HashSize);

				Value.ChunkBlockSignatures.reserve(Value.Header.TocCompressedBlockEntryCount);
				for (int i = 0; i < Value.Header.TocCompressedBlockEntryCount; ++i) {
					Stream >> Value.ChunkBlockSignatures.emplace_back();
				}
			}

			if (Value.Header.Version >= EIoStoreTocVersion::DirectoryIndex && Value.Header.ContainerFlags & EIoContainerFlags::Indexed && Value.Header.DirectoryIndexSize > 0) {
				Value.DirectoryBuffer = std::make_unique<char[]>(Value.Header.DirectoryIndexSize);
				Stream.read(Value.DirectoryBuffer.get(), Value.Header.DirectoryIndexSize);
			}

			Value.ChunkMetas.reserve(Value.Header.TocEntryCount);
			for (int i = 0; i < Value.Header.TocEntryCount; ++i) {
				Stream >> Value.ChunkMetas.emplace_back();
			}

			return Stream;
		}
	};
}
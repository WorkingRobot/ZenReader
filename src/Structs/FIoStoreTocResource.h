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

			Value.ChunkIds.resize(Value.Header.TocEntryCount);
			Stream.read((char*)Value.ChunkIds.data(), sizeof(FIoChunkId) * Value.Header.TocEntryCount);

			Value.ChunkOffsetLengths.resize(Value.Header.TocEntryCount);
			Stream.read((char*)Value.ChunkOffsetLengths.data(), sizeof(FIoChunkOffsetAndLength) * Value.Header.TocEntryCount);

			Value.CompressionBlocks.resize(Value.Header.TocCompressedBlockEntryCount);
			Stream.read((char*)Value.CompressionBlocks.data(), sizeof(FIoStoreTocCompressedBlockEntry) * Value.Header.TocCompressedBlockEntryCount);

			Value.CompressionMethodNames.resize(Value.Header.CompressionMethodNameCount);
			{
				auto Methods = std::make_unique<char[]>(Value.Header.CompressionMethodNameCount * Value.Header.CompressionMethodNameLength);
				Stream.read(Methods.get(), Value.Header.CompressionMethodNameCount * Value.Header.CompressionMethodNameLength);
				for (int i = 0; i < Value.Header.CompressionMethodNameCount; ++i) {
					Value.CompressionMethodNames[i] = Methods.get() + Value.Header.CompressionMethodNameLength * i;
				}
			}

			if (Value.Header.ContainerFlags & EIoContainerFlags::Signed) {
				int32_t HashSize;
				Stream >> HashSize;
				Value.TocSignature = std::make_unique<char[]>(HashSize);
				Value.BlockSignature = std::make_unique<char[]>(HashSize);
				Stream.read(Value.TocSignature.get(), HashSize);
				Stream.read(Value.BlockSignature.get(), HashSize);

				Value.ChunkBlockSignatures.resize(Value.Header.TocCompressedBlockEntryCount);
				Stream.read((char*)Value.ChunkBlockSignatures.data(), sizeof(FSHAHash) * Value.Header.TocCompressedBlockEntryCount);
			}

			if (Value.Header.Version >= EIoStoreTocVersion::DirectoryIndex && Value.Header.ContainerFlags & EIoContainerFlags::Indexed && Value.Header.DirectoryIndexSize > 0) {
				Value.DirectoryBuffer = std::make_unique<char[]>(Value.Header.DirectoryIndexSize);
				Stream.read(Value.DirectoryBuffer.get(), Value.Header.DirectoryIndexSize);
			}

			Value.ChunkMetas.resize(Value.Header.TocEntryCount);
			Stream.read((char*)Value.ChunkMetas.data(), sizeof(FIoStoreTocEntryMeta) * Value.Header.TocEntryCount);

			return Stream;
		}

		uint32_t GetChunkIdx(const FIoChunkId& ChunkId) const {
			auto Itr = std::find(ChunkIds.begin(), ChunkIds.end(), ChunkId);
			if (Itr == ChunkIds.end()) {
				return -1;
			}
			return std::distance(ChunkIds.begin(), Itr);
		}

		void InvalidateBuffers() {
			TocSignature.reset();
			BlockSignature.reset();
			DirectoryBuffer.reset();

			ChunkBlockSignatures.clear();
			ChunkMetas.clear();

			ChunkBlockSignatures.shrink_to_fit();
			ChunkMetas.shrink_to_fit();
		}
	};
}
#pragma once

#include "ZContainer.h"
#include "Helpers/Align.h"
#include "Helpers/AES.h"
#include "Helpers/Decompress.h"

namespace Zen {
	class ZFile {
	public:
		ZFile(uint32_t ChunkIdIdx, const BaseContainer& Container) :
			ChunkIdIdx(ChunkIdIdx),
			Container(Container)
		{

		}

		// TODO: switch to ZFileStream
		std::unique_ptr<char[]> ReadFile(uint64_t& OutputLength) {
			auto& LengthData = Container.Toc.ChunkOffsetLengths[ChunkIdIdx];
			auto FileBuffer = std::make_unique<char[]>(LengthData.GetLength());
			auto BlockIdx = LengthData.GetOffset() / Container.Toc.Header.CompressionBlockSize;
			auto BlockOffset = LengthData.GetOffset() % Container.Toc.Header.CompressionBlockSize;
			auto EndBlockIdx = (Helpers::Align<size_t>(LengthData.GetOffset() + LengthData.GetLength(), Container.Toc.Header.CompressionBlockSize) - 1) / Container.Toc.Header.CompressionBlockSize;
			auto ReadBytes = 0;
			for (int i = BlockIdx; i <= EndBlockIdx; ++i) {
				auto& Entry = Container.Toc.CompressionBlocks[i];
				auto ReadAmt = Helpers::Align<16>(Entry.GetCompressedSize());
				auto ReadBuffer = std::make_unique<char[]>(ReadAmt);
				auto DecompBuffer = std::make_unique<char[]>(Entry.GetUncompressedSize());
				Container.CasStream.seek(Entry.GetOffset(), BaseStream::Beg);
				Container.CasStream.read(ReadBuffer.get(), ReadAmt);
				if (Container.Toc.Header.ContainerFlags & EIoContainerFlags::Encrypted) {
					// TODO: Throw exception if the key has no value
					// But how would you get here if there was no key?
					Helpers::AES::DecodeInPlace(Container.Key.value(), ReadBuffer.get(), ReadAmt);
				}
				if (Entry.GetCompressionMethodIndex()) {
					Helpers::Decompress(DecompBuffer.get(), Entry.GetUncompressedSize(), ReadBuffer.get(), ReadAmt, Container.Toc.CompressionMethodNames[Entry.GetCompressionMethodIndex() - 1]);
				}
				else {
					// TODO: remove this memcpy and check if it's uncompressed before all that other stuff to remove extra moves/copies
					memcpy(DecompBuffer.get(), ReadBuffer.get(), Entry.GetUncompressedSize());
				}
				auto CopyAmt = std::min(Container.Toc.Header.CompressionBlockSize - BlockOffset, LengthData.GetLength() - ReadBytes);
				memcpy(FileBuffer.get() + ReadBytes, DecompBuffer.get() + BlockOffset, CopyAmt);
				BlockOffset = 0;
				ReadBytes += CopyAmt;
			}
			OutputLength = LengthData.GetLength();
			return FileBuffer;
		}

	private:
		uint32_t ChunkIdIdx;
		const BaseContainer& Container;
	};
}
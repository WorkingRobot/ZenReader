#pragma once

#include "../Exceptions/BaseException.h"
#include "../Helpers/AES.h"
#include "../Helpers/Align.h"
#include "../Helpers/Decompress.h"
#include "../ZContainer.h"
#include "BufferedStream.h"

#include <filesystem>

namespace Zen::Streams {
    class ZUnoptimizedFileStream : public BaseStream {
    public:
        ZUnoptimizedFileStream(uint32_t ChunkIdIdx, const BaseContainer& Container) :
            ChunkIdIdx(ChunkIdIdx),
            Container(Container),
            Buffer(std::make_unique<char[]>(Container.Toc.Header.CompressionBlockSize)),
            BufferBlockIdx(-1),
            Position(0)
        {

        }

        ZUnoptimizedFileStream(const FIoChunkId& ChunkId, const BaseContainer& Container) : ZUnoptimizedFileStream(Container.Toc.GetChunkIdx(ChunkId), Container) {
            if (ChunkIdIdx == -1) {
                throw Exceptions::InvalidChunkIdException("The chunk id used is invalid");
            }
        }

        BaseStream& write(const char* Buf, size_t BufCount) override {
            throw Exceptions::UnsupportedOperationException("Write operations aren't supported for ZFileStreams");
        }

        BaseStream& read(char* Buf, size_t BufCount) override {
            size_t BlockOffset;
            auto BlockIdx = GetBlockIdx(Position, BlockOffset);
            if (BlockIdx == -1) {
                return *this;
            }
            while (true) {
                LoadBlock(BlockIdx++);
                if (BlockOffset + BufCount <= Container.Toc.Header.CompressionBlockSize) {
                    memcpy(Buf, Buffer.get() + BlockOffset, BufCount);
                    Position += BufCount;
                    return *this;
                }
                else {
                    memcpy(Buf, Buffer.get() + BlockOffset, Container.Toc.Header.CompressionBlockSize - BlockOffset);
                    Buf += Container.Toc.Header.CompressionBlockSize - BlockOffset;
                    BufCount -= Container.Toc.Header.CompressionBlockSize - BlockOffset;
                }
                BlockOffset = 0;
            }
            return *this;
        }

        BaseStream& seek(size_t Position, SeekPosition SeekFrom) override {
            switch (SeekFrom)
            {
            case BaseStream::Beg:
                this->Position = Position;
                break;
            case BaseStream::Cur:
                this->Position += Position;
                break;
            case BaseStream::End:
                this->Position = size() + Position;
                break;
            }

            return *this;
        }

        size_t tell() override {
            return Position;
        }

        size_t size() override {
            return Container.Toc.ChunkOffsetLengths[ChunkIdIdx].GetLength();
        }

    private:
        void LoadBlock(size_t BlockIdx) {
            if (BufferBlockIdx == BlockIdx) {
                return;
            }
            auto& Entry = Container.Toc.CompressionBlocks[BlockIdx];
            auto ReadAmt = Helpers::Align<16>(Entry.GetCompressedSize());
            auto ReadBuffer = std::make_unique<char[]>(ReadAmt);
            Container.CasStream.seek(Entry.GetOffset(), BaseStream::Beg);
            Container.CasStream.read(ReadBuffer.get(), ReadAmt);
            if (Container.Toc.Header.ContainerFlags & EIoContainerFlags::Encrypted) {
                if (Container.Key.has_value()) {
                    Helpers::AES::DecodeInPlace(Container.Key.value(), ReadBuffer.get(), ReadAmt);
                }
                throw Exceptions::KeyRequiredException("An AES key is required for this asset");
            }
            if (Entry.GetCompressionMethodIndex()) {
                Helpers::Decompress(Buffer.get(), Entry.GetUncompressedSize(), ReadBuffer.get(), ReadAmt, Container.Toc.CompressionMethodNames[Entry.GetCompressionMethodIndex() - 1]);
            }
            else {
                // TODO: remove this memcpy and check if it's uncompressed before all that other stuff to remove extra moves/copies
                memcpy(Buffer.get(), ReadBuffer.get(), Entry.GetUncompressedSize());
            }
        }

        size_t GetBlockIdx(size_t Position, size_t& OutBlockOffset) const {
            auto& LengthData = Container.Toc.ChunkOffsetLengths[ChunkIdIdx];
            auto BlockOffset = LengthData.GetOffset() % Container.Toc.Header.CompressionBlockSize;
            auto EndBlockIdx = (Helpers::Align<size_t>(LengthData.GetOffset() + LengthData.GetLength(), Container.Toc.Header.CompressionBlockSize) - 1) / Container.Toc.Header.CompressionBlockSize;
            for (auto BlockIdx = LengthData.GetOffset() / Container.Toc.Header.CompressionBlockSize; BlockIdx <= EndBlockIdx; ++BlockIdx) {
                if (Position < Container.Toc.Header.CompressionBlockSize - BlockOffset) {
                    OutBlockOffset = BlockOffset + Position;
                    return BlockIdx;
                }
                Position -= Container.Toc.Header.CompressionBlockSize - BlockOffset;
            }
            return -1;
        }

        uint32_t ChunkIdIdx;
        const BaseContainer& Container;

        std::unique_ptr<char[]> Buffer;
        size_t BufferBlockIdx;
        size_t Position;
    };

    // Having a buffered stream instead saves potentially millions of calls to SetFilePointerEx (saves a massive amount of time when exporting many packages)
    // 4kb default buffer size (BufferedStream's default 256kb isn't efficient for small assets, and the penalty isn't very big for larger assets)
    using ZFileStream = BufferedStream<ZUnoptimizedFileStream, 1 << 12>;
}

#pragma once

#include "BaseStream.h"
#include "../Helpers/AES.h"
#include "../Helpers/Align.h"
#include "../Helpers/Decompress.h"

#include <filesystem>

namespace Zen::Streams {
    class ZFileStream : public BaseStream {
    public:
        ZFileStream(uint32_t ChunkIdIdx, const BaseContainer& Container) :
            ChunkIdIdx(ChunkIdIdx),
            Container(Container),
            Buffer(std::make_unique<char[]>(Container.Toc.Header.CompressionBlockSize)),
            BufferBlockIdx(-1),
            Position(0)
        {

        }

        ZFileStream(const FIoChunkId& ChunkId, const BaseContainer& Container) : ZFileStream(Container.Toc.GetChunkIdx(ChunkId), Container)
        {
            if (ChunkIdIdx == -1) {
                // TODO: THROW EXCEPTION
            }
        }

        BaseStream& write(const char* Buf, size_t BufCount) override {
            // unsupported lol
            return *this;
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
                // TODO: Throw exception if the key has no value
                // But how would you get here if there was no key?
                Helpers::AES::DecodeInPlace(Container.Key.value(), ReadBuffer.get(), ReadAmt);
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
}

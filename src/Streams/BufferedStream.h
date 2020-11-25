#pragma once

#include "BaseStream.h"

#include <filesystem>

namespace Zen::Streams {
    // 256kb by default
    template<class WrappedStream, size_t BufferSize = 1 << 18>
    class BufferedStream : public BaseStream {
    public:
        template<typename... Args>
        BufferedStream(Args&&... args) :
            BaseStream(std::forward<Args>(args)...),
            Buffer(new char[BufferSize]),
            BufferPosition(-1),
            Position(BaseStream.tell())
        {}

        BufferedStream(WrappedStream&& Stream) :
            BaseStream(std::forward<WrappedStream>(Stream)),
            Buffer(new char[BufferSize]),
            BufferPosition(-1),
            Position(BaseStream.tell())
        {
        }

        BaseStream& write(const char* Buf, size_t BufCount) override {
            // unsupported lol
            return *this;
        }

        BaseStream& read(char* Buf, size_t BufCount) override {
            if (BufferPosition <= Position && Position < BufferPosition + BufferSize) {
                auto BufferDataAvailable = BufferSize + BufferPosition - Position;
                if (BufCount < BufferDataAvailable) {
                    memcpy(Buf, Buffer.get() + (Position - BufferPosition), BufCount);
                    Position += BufCount;
                    return *this;
                }
                memcpy(Buf, Buffer.get() + (Position - BufferPosition), BufferDataAvailable);
                BufCount -= BufferDataAvailable;
                Position += BufferDataAvailable;
                Buf += BufferDataAvailable;
            }
            BaseStream.seek(Position, Beg);
            while (BufCount >= BufferSize) {
                BaseStream.read(Buf, BufferSize);
                BufCount -= BufferSize;
                Position += BufferSize;
                Buf += BufferSize;
            }
            BufferPosition = BaseStream.tell();
            BaseStream.read(Buffer.get(), BufferSize);
            memcpy(Buf, Buffer.get(), BufCount);
            Position += BufCount;
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
            return BaseStream.size();
        }

    private:
        WrappedStream BaseStream;

        std::unique_ptr<char[]> Buffer;
        size_t BufferPosition; // Index 0 of buffer is this position in the file
        size_t Position; // Position of the file where to read from
    };
}

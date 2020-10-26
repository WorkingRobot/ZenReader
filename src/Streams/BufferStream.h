#pragma once

#include "BaseStream.h"

namespace Zen::Streams {
    class BufferStream : public BaseStream {
    public:
        BufferStream(char* Start, size_t Size) :
            Buffer(Start),
            Position(0),
            Size(Size)
        {}

        BaseStream& write(const char* Buf, size_t BufCount) override {
            if (Position + BufCount > Size) {
                BufCount = Size - Position;
            }
            memcpy(Buffer + Position, Buf, BufCount);
            Position += BufCount;
            return *this;
        }

        BaseStream& read(char* Buf, size_t BufCount) override {
            if (Position + BufCount > Size) {
                BufCount = Size - Position;
            }
            memcpy(Buf, Buffer + Position, BufCount);
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
                this->Position = Size + Position;
            }
            return *this;
        }

        size_t tell() override {
            return Position;
        }

        size_t size() {
            return Size;
        }

    private:
        char* Buffer;
        size_t Position;
        size_t Size;
    };
}

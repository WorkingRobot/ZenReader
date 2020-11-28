#pragma once

#include "BaseStream.h"

namespace Zen::Streams {
    class MemoryStream : public BaseStream {
    public:
        MemoryStream() {
            Buffer.resize(1024);
            Size = 0;
            Position = 0;
        }

        MemoryStream(size_t Capacity) {
            Buffer.resize(Capacity);
            Size = 0;
            Position = 0;
        }

        BaseStream& write(const char* Buf, size_t BufCount) override {
            while (Buffer.size() < BufCount + Position) {
                Buffer.resize(Buffer.size() * 2);
            }
            memcpy(Buffer.data() + Position, Buf, BufCount);
            Position += BufCount;
            if (Position > Size) {
                Size = Position;
            }
            return *this;
        }

        BaseStream& read(char* Buf, size_t BufCount) override {
            if (Position + BufCount < Size) {
                BufCount = Size - Position;
            }
            memcpy(Buf, Buffer.data() + Position, BufCount);
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

        size_t size() override {
            return Size;
        }

        const char* get() const {
            return Buffer.data();
        }

    private:
        std::vector<char> Buffer;
        size_t Position;
        size_t Size;
    };
}

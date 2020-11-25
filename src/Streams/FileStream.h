#pragma once

#include "BaseStream.h"

#include <filesystem>
#include <fstream>

namespace Zen::Streams {
    class FileStream : public BaseStream {
    public:
        FileStream(const std::filesystem::path& FilePath) :
            BaseStream(FilePath, std::ios::in | std::ios::binary),
            Filename(FilePath.string()),
            FileSize(std::filesystem::file_size(FilePath))
        {
            
        }

        FileStream(FileStream&&) = default;
        FileStream& operator=(FileStream&&) = default;
        FileStream(const FileStream&) = delete;
        FileStream& operator=(const FileStream&) = delete;

        BaseStream& write(const char* Buf, size_t BufCount) override {
            throw Exceptions::UnsupportedOperationException("Write operations aren't supported for FileStreams");
        }

        BaseStream& read(char* Buf, size_t BufCount) override {
            BaseStream.read(Buf, BufCount);
            return *this;
        }

        BaseStream& seek(size_t Position, SeekPosition SeekFrom) override {
            switch (SeekFrom)
            {
            case BaseStream::Beg:
                BaseStream.seekg(Position, std::ios::beg);
                break;
            case BaseStream::Cur:
                BaseStream.seekg(Position, std::ios::cur);
                break;
            case BaseStream::End:
                BaseStream.seekg(Position, std::ios::end);
                break;
            }

            return *this;
        }

        size_t tell() override {
            return std::streamoff(BaseStream.tellg());
        }

        size_t size() override {
            return FileSize;
        }

    private:
        std::ifstream BaseStream;
        std::string Filename;
        size_t FileSize;
    };
}

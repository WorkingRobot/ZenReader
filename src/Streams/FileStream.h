#pragma once

#include "BaseStream.h"

#include <filesystem>

namespace Zen::Streams {
    class FileStream : public BaseStream {
    public:
        FileStream() : BaseStream(NULL), OpenAndValid(false) {}

        FileStream(const std::filesystem::path& FilePath, const char* Mode) : FileStream()
        {
            open(FilePath, Mode);
        }

        FileStream(FileStream&&) = default;
        FileStream& operator=(FileStream&&) = default;
        FileStream(const FileStream&) = delete;
        FileStream& operator=(const FileStream&) = delete;

        ~FileStream() {
            close();
        }

        // weird crashes happen when i try RAII with a destructor
        bool open(const std::filesystem::path& FilePath, const char* Mode) {
            BaseStream = fopen(FilePath.string().c_str(), Mode);
            return OpenAndValid = valid();
        }

        bool open(const char* FilePath, const char* Mode) {
            BaseStream = fopen(FilePath, Mode);
            return OpenAndValid = valid();
        }

        bool close() {
            if (OpenAndValid && valid()) {
                fclose(BaseStream);
                BaseStream = NULL;
                return true;
            }
            return false;
        }

        bool valid() {
            return BaseStream;
        }

        BaseStream& write(const char* Buf, size_t BufCount) override {
            fwrite(Buf, 1, BufCount, BaseStream);
            return *this;
        }

        BaseStream& read(char* Buf, size_t BufCount) override {
            fread(Buf, 1, BufCount, BaseStream);
            return *this;
        }

        BaseStream& seek(size_t Position, SeekPosition SeekFrom) override {
            switch (SeekFrom)
            {
            case BaseStream::Beg:
                _fseeki64(BaseStream, Position, SEEK_SET);
                break;
            case BaseStream::Cur:
                _fseeki64(BaseStream, Position, SEEK_CUR);
                break;
            case BaseStream::End:
                _fseeki64(BaseStream, Position, SEEK_END);
                break;
            }

            return *this;
        }

        size_t tell() override {
            return _ftelli64(BaseStream);
        }

        size_t size() override {
            auto cur = tell();
            _fseeki64(BaseStream, 0, SEEK_END);
            auto ret = tell();
            _fseeki64(BaseStream, cur, SEEK_SET);
            return ret;
        }

    private:
        FILE* BaseStream;
        bool OpenAndValid;
    };
}

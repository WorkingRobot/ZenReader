#pragma once

#include <unordered_map>

namespace Zen::Streams {
    class BaseStream {
    public:
        enum SeekPosition : int8_t {
            Beg,
            Cur,
            End
        };

        virtual BaseStream& write(const char* Buf, size_t BufCount) = 0;
        virtual BaseStream& read(char* Buf, size_t BufCount) = 0;
        virtual BaseStream& seek(size_t Position, SeekPosition SeekFrom) = 0;
        virtual size_t tell() = 0;
        virtual size_t size() = 0;

        // Write ops

        BaseStream& operator<<(bool Val) {
            write((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator<<(char Val) {
            write((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator<<(int8_t Val) {
            write((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator<<(int16_t Val) {
            write((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator<<(int32_t Val) {
            write((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator<<(int64_t Val) {
            write((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator<<(uint8_t Val) {
            write((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator<<(uint16_t Val) {
            write((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator<<(uint32_t Val) {
            write((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator<<(uint64_t Val) {
            write((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator<<(float Val) {
            write((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator<<(double Val) {
            write((char*)&Val, sizeof(Val));
            return *this;
        }

        template<class T, uint32_t Size>
        BaseStream& operator<<(const T(&Val)[Size]) {
            for (int i = 0; i < Size; ++i) {
                *this << Val[i];
            }
            return *this;
        }

        template<class K, class V>
        BaseStream& operator<<(const std::pair<K, V>& Val) {
            *this << Val.first;
            *this << Val.second;
            return *this;
        }

        template<class K, class V>
        BaseStream& operator<<(const std::unordered_map<K, V>& Val) {
            *this << Val.size();
            for (auto& Item : Val) {
                *this << Item;
            }
            return *this;
        }

        // Read ops

        BaseStream& operator>>(bool& Val) {
            read((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator>>(char& Val) {
            read((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator>>(int8_t& Val) {
            read((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator>>(int16_t& Val) {
            read((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator>>(int32_t& Val) {
            read((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator>>(int64_t& Val) {
            read((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator>>(uint8_t& Val) {
            read((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator>>(uint16_t& Val) {
            read((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator>>(uint32_t& Val) {
            read((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator>>(uint64_t& Val) {
            read((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator>>(float& Val) {
            read((char*)&Val, sizeof(Val));
            return *this;
        }

        BaseStream& operator>>(double& Val) {
            read((char*)&Val, sizeof(Val));
            return *this;
        }

        template<class T, uint32_t Size>
        BaseStream& operator>>(T(&Val)[Size]) {
            for (int i = 0; i < Size; ++i) {
                *this >> Val[i];
            }
            return *this;
        }

        template<class K, class V>
        BaseStream& operator>>(std::pair<K, V>& Val) {
            *this >> Val.first;
            *this >> Val.second;
            return *this;
        }

        template<class K, class V>
        BaseStream& operator>>(std::unordered_map<K, V>& Val) {
            size_t Size;
            *this >> Size;
            Val.reserve(Size);
            K Key;
            V Value;
            for (int i = 0; i < Size; ++i) {
                *this >> Key;
                *this >> Value;
                Val.emplace(Key, Value);
            }
            return *this;
        }
    };
}
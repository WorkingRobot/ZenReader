#pragma once

#include "Properties.h"

#include <codecvt>
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

        /*
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
        */

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

        template<class T>
        BaseStream& operator>>(std::vector<T>& Val) {
            int SerializeNum;
            *this >> SerializeNum;
            Val.reserve(SerializeNum);
            for (int i = 0; i < SerializeNum; ++i) {
                auto& Item = Val.emplace_back();
                *this >> Item;
            }
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

        // Only reading FStrings is supported, I don't have reason to write these yet
        BaseStream& operator>>(std::string& Val) {
            // > 0 for ANSICHAR, < 0 for UCS2CHAR serialization
            int SaveNum;
            *this >> SaveNum;

            bool LoadUCS2Char = SaveNum < 0;

            if (LoadUCS2Char)
            {
                // If SaveNum cannot be negated due to integer overflow, Ar is corrupted.
                if (SaveNum == INT_MIN)
                {
                    // TODO: THROW HERE
                    return *this; // archive corrupted
                }

                SaveNum = -SaveNum;
            }

            if (SaveNum == 0)
            {
                Val.clear();
                return *this; // blank
            }

            if (LoadUCS2Char)
            {
                static std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> ucs2conv;

                std::unique_ptr<char16_t[]> StringData = std::make_unique<char16_t[]>(SaveNum);
                read((char*)StringData.get(), SaveNum * sizeof(char16_t));
                Val = ucs2conv.to_bytes(StringData.get());
            }
            else {
                std::unique_ptr<char[]> StringData = std::make_unique<char[]>(SaveNum);
                read(StringData.get(), SaveNum);
                Val = StringData.get();
            }

            return *this;
        }

        // Properties

        template<PropId Id>
        void* GetProperty() {
            auto& Stack = Props[(uint8_t)Id];
            return Stack.empty() ? nullptr : Stack.top();
        }

        std::stack<void*> Props[(uint8_t)PropId::Count];
    };

    template<PropId Id>
    struct PropertyHolder {
        PropertyHolder(BaseStream& Stream, void* Data) : Stack(Stream.Props[(uint8_t)Id])
        {
            Stack.push(Data);
        }

        ~PropertyHolder() {
            Stack.pop();
        }

    private:
        std::stack<void*>& Stack;
    };
}
#pragma once

#include "Helpers/Hash.h"

#include <memory>
#include <vector>
#include <string.h>

namespace Zen {
	// Adds a null terminator to compute the size
	template<typename SizeType>
	struct StrlenKey {
		typedef SizeType KeySize;

		StrlenKey(std::unique_ptr<char[]>&& Data) : Data(std::move(Data)) {}

		// Using initializers here forces the optimization to not create an empty unique ptr first
		StrlenKey(const char* String, KeySize Size) : Data(new char[Size + 1])
		{
			memcpy(Data.get(), String, Size);
			Data[Size] = '\0';
		}

		StrlenKey(StrlenKey&& Old) : Data(std::move(Old.Data)) {}

		KeySize GetSize() const {
			return strlen(Data.get());
		}

		const char* GetString() const {
			return Data.get();
		}
	private:
		std::unique_ptr<char[]> Data;
	};

	// Simply places the length behind the name
	template<typename SizeType>
	struct PrefixedKey {
		typedef SizeType KeySize;

		PrefixedKey(std::unique_ptr<char[]>&& Data) : Data(std::move(Data)) {}

		PrefixedKey(const char* String, KeySize Size) : Data(new char[sizeof(KeySize) + Size])
		{
			*(KeySize*)Data.get() = Size;
			memcpy(Data.get() + sizeof(KeySize), String, Size);
		}

		PrefixedKey(PrefixedKey&& Old) : Data(std::move(Old.Data)) {}

		KeySize GetSize() const {
			return *(KeySize*)Data.get();
		}

		const char* GetString() const {
			return Data.get() + sizeof(KeySize);
		}
	private:
		std::unique_ptr<char[]> Data;
	};

	template<typename KeyType, typename ValueType>
	class ZSmallMap {
		std::vector<uint32_t> Hashes;
		std::vector<std::pair<KeyType, ValueType>> Names;

	public:
		decltype(auto) end() const {
			return Names.end();
		}

		ValueType& emplace_back(const char* KeyString, typename KeyType::KeySize KeyStringSize, ValueType&& Value) {
			Hashes.emplace_back(Helpers::Hash::Crc32(KeyString, KeyStringSize));
			return Names.emplace_back(KeyType(KeyString, KeyStringSize), std::forward<ValueType&&>(Value)).second;
		}

		typename decltype(Names)::iterator SearchValues(const char* KeyString, typename KeyType::KeySize KeyStringSize) {
			if (Hashes.size() != 0) {
				uint32_t TargetHash = Helpers::Hash::Crc32(KeyString, KeyStringSize);
				for (int i = 0; i < Hashes.size(); ++i) {
					if (TargetHash == Hashes[i]) {
						auto& Key = Names[i].first;
						if (Key.GetSize() == KeyStringSize) {
							if (memcmp(Key.GetString(), KeyString, KeyStringSize) == 0) {
								return Names.begin() + i;
							}
						}
					}
				}
			}
			return Names.end();
		}

		typename decltype(Names)::const_iterator SearchValues(const char* KeyString, typename KeyType::KeySize KeyStringSize) const {
			// Forces a call to the non const SearchValues and converts the return value to a const_iterator
			return ((std::remove_cvref<decltype(*this)>::type)*this).SearchValues(KeyString, KeyStringSize);
		}
	};
}
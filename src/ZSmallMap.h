#pragma once

#include "Helpers/Hash.h"

#include <memory>
#include <vector>

namespace Zen {
	// Adds a null terminator to compute the size
	template<class Size>
	struct StrlenKey {
		typedef Size KeySize;

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
	template<class Size>
	struct PrefixedKey {
		typedef Size KeySize;

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

	namespace {
		template<class ValueType, class Key, class KeySize = Key::KeySize>
		decltype(auto) SearchValues(const std::vector<uint32_t>& Hashes, std::vector<std::pair<Key, ValueType>>& Names, const char* KeyString, KeySize KeyStringSize) {
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
	}
}
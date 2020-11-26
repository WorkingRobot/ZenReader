#pragma once

#include "Streams/BufferStream.h"
#include "Structs/FMappedName.h"
#include "Structs/FNameEntryId.h"
#include "Structs/FSerializedNameHeader.h"

namespace Zen {
	using namespace Streams;
	using namespace Structs;

	class ZNameMap {
	public:
		ZNameMap() = default;

		// https://github.com/EpicGames/UnrealEngine/blob/c7c6766c2fe5278024139d9330d872bd139633c8/Engine/Source/Runtime/Core/Private/UObject/UnrealNames.cpp#L2888
		ZNameMap(const char* NameData, size_t NameDataSize, const char* HashData, size_t HashDataSize) {
			BufferStream NameStream((char*)NameData, NameDataSize);
			BufferStream HashStream((char*)HashData, HashDataSize);

			NameMap.resize(HashDataSize / 8 - 1);

			for (auto& Entry : NameMap) {
				// https://github.com/EpicGames/UnrealEngine/blob/34d9401ee50bf67206af9e21217966de5f997ba9/Engine/Source/Runtime/Core/Private/UObject/UnrealNames.cpp#L2709
				FSerializedNameHeader NameHeader;
				NameStream >> NameHeader;
				if (NameHeader.IsUtf16()) {
					std::unique_ptr<char16_t[]> StringData = std::make_unique<char16_t[]>(NameHeader.GetLength());
					// NameStream should be aligned to sizeof(char16_t) here actually, but that sounds like an unnecessary hassle
					NameStream.read((char*)StringData.get(), NameHeader.GetLength() * sizeof(char16_t));
					Entry.Name = { StringData.get(), StringData.get() + NameHeader.GetLength() };
				}
				else {
					Entry.Name.resize(NameHeader.GetLength());
					NameStream.read(Entry.Name.data(), NameHeader.GetLength());
				}
			}

			uint64_t AlgorithmId;
			HashStream >> AlgorithmId;
			// https://github.com/EpicGames/UnrealEngine/blob/c7c6766c2fe5278024139d9330d872bd139633c8/Engine/Source/Runtime/Core/Private/UObject/UnrealNames.cpp#L478
			if (AlgorithmId == 0xC1640000) { // Load with hash if AlgorithmId matches
				for (auto& Entry : NameMap) {
					HashStream >> Entry.Hash;
				}
			}
			else {
				// Technically you should actually calculate the hashes here if they're not given
				// But honestly, I don't think we're going to be using them at all, so I might skip and remove the fields
				// altogether to save ram and compute time if it's reasonable to do so
			}
		}

		const std::string& operator[](int i) const {
			return NameMap[i].Name;
		}

		const FNameEntryId& GetName(const FMappedName& Name) const {
			return NameMap[Name.GetIndex()];
		}

	private:
		std::vector<FNameEntryId> NameMap;
	};
}
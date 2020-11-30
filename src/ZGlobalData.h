#pragma once

#include "Exceptions/BaseException.h"
#include "Structs/FScriptObjectEntry.h"
#include "ZContainer.h"
#include "Streams/ZFileStream.h"
#include "ZNameMap.h"

namespace Zen {
	using namespace Structs;
	using namespace Streams;

	class ZGlobalData {
	public:
		ZGlobalData() = default;

		// TODO: https://github.com/EpicGames/UnrealEngine/blob/5c904997491e4f504c4cd3d7548540685016fbf9/Engine/Source/Runtime/CoreUObject/Private/Serialization/AsyncLoading2.cpp#L858
		// https://github.com/EpicGames/UnrealEngine/blob/5c904997491e4f504c4cd3d7548540685016fbf9/Engine/Source/Runtime/CoreUObject/Private/Serialization/AsyncLoading2.cpp#L454
		ZGlobalData(const BaseContainer& Container) {
			{
				ZFileStream NamesStream(FIoChunkId(0, 0, EIoChunkType::LoaderGlobalNames), Container);
				ZFileStream HashesStream(FIoChunkId(0, 0, EIoChunkType::LoaderGlobalNameHashes), Container);

				auto NamesBuffer = std::make_unique<char[]>(NamesStream.size());
				auto HashesBuffer = std::make_unique<char[]>(HashesStream.size());

				NamesStream.read(NamesBuffer.get(), NamesStream.size());
				HashesStream.read(HashesBuffer.get(), HashesStream.size());

				NameMap = ZNameMap(NamesBuffer.get(), NamesStream.size(), HashesBuffer.get(), HashesStream.size());
			}

			{
				ZFileStream InitialLoadStream(FIoChunkId(0, 0, EIoChunkType::LoaderInitialLoadMeta), Container);
				int32_t EntrySize;
				InitialLoadStream >> EntrySize;
				ScriptObjectEntries.resize(EntrySize);
				// This should be safe, done for performance :)
				InitialLoadStream.read((char*)ScriptObjectEntries.data(), sizeof(FScriptObjectEntry) * EntrySize);
			}
		}

		const FScriptObjectEntry& GetEntry(const FPackageObjectIndex& Idx) const {
			for (auto& Entry : ScriptObjectEntries) {
				if (Entry.GlobalIndex == Idx) {
					return Entry;
				}
			}
			throw Exceptions::ScriptObjectNotFoundException("A matching ScriptObjectEntry was not found for a FPackageObjectIndex (%lld)", Idx.TypeAndId);
		}

		const std::string& GetEntryName(const FPackageObjectIndex& Idx) const {
			// GetEntry is always global (in the global utoc), so it's safe to ask the global namemap directly
			return NameMap.GetName(GetEntry(Idx).ObjectName).Name;
		}

	private:
		ZNameMap NameMap;
		// maybe make this some other type to be faster?
		std::vector<FScriptObjectEntry> ScriptObjectEntries;
	};
}
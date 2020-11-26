#pragma once

#include "../Enums/EMappedNameType.h"
#include "../Streams/BaseStream.h"
#include "../ZNameMap.h"

namespace Zen::Structs {
	class FName {
	public:
		// Technically FNameEntryId, but who cares
		uint32_t Index;
		uint32_t Number;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FName& Value) {
			InputStream >> Value.Index;
			InputStream >> Value.Number;

			return InputStream;
		}

		const std::string& Get(const ZNameMap& NameMap) const {
			return NameMap[Index];
		}

		// TODO: use a hash lookup or something, that would be better
		bool IsNone(const ZNameMap& NameMap) const {
			return Get(NameMap) == "None";
		}
	};
}
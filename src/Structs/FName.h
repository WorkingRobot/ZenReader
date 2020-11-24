#pragma once

#include "../Enums/EMappedNameType.h"
#include "../Streams/BaseStream.h"

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

		bool IsNone() const {
			printf("yo\n");
			return false;
		}
	};
}
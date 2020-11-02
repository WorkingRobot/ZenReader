#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FNameEntrySerialized {
	public:
		std::string Name;
		uint16_t NonCasePreservingHash;
		uint16_t CasePreservingHash;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FNameEntrySerialized& Value) {
			InputStream >> Value.Name;
			InputStream >> Value.NonCasePreservingHash;
			InputStream >> Value.CasePreservingHash;

			return InputStream;
		}
	};
}
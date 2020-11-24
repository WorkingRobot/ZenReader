#pragma once

#include "../Streams/BaseStream.h"
#include "FFormatArgumentValue.h"

namespace Zen::Structs {
	// UE4 defines this as a TMap<FString, FFormatArgumentValue>
	// Maps take a lot of memory, so a vector of pairs will do
	class FFormatNamedArguments {
	public:
        std::vector<std::pair<std::string, FFormatArgumentValue>> Value;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FFormatNamedArguments& Value) {
            InputStream >> Value.Value;

            return InputStream;
		}
	};
}
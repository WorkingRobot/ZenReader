#pragma once

#include "../Streams/BaseStream.h"
#include "FName.h"

namespace Zen::Structs {
	class FGameplayTagContainer {
	public:
		// It's technically a TArray<FGameplayTag> but FGameplayTag is just a fancy wrapper around an FName
		std::vector<FName> GameplayTags;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FGameplayTagContainer& Value) {
			InputStream >> Value.GameplayTags;

			return InputStream;
		}
	};
}
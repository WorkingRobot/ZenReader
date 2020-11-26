#pragma once

#include "Base.h"
#include "Serializer.h"

namespace Zen::Properties {
	class SetProperty : public BaseProperty {
	public:
		std::vector<std::unique_ptr<BaseProperty>> Value;

		// https://github.com/EpicGames/UnrealEngine/blob/bf95c2cbc703123e08ab54e3ceccdd47e48d224a/Engine/Source/Runtime/CoreUObject/Private/UObject/PropertySet.cpp#L216
		SetProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData) {
			int NumKeysToRemove;
			InputStream >> NumKeysToRemove;
			for (int i = 0; i < NumKeysToRemove; ++i) {
				Serialize<EReadType::ARRAY>(InputStream, PropData, PropData.GetArrayInnerType());
			}

			int NumEntries;
			InputStream >> NumEntries;
			Value.reserve(NumEntries);

			for (int i = 0; i < NumEntries; ++i) {
				Value.emplace_back(Serialize<EReadType::ARRAY>(InputStream, PropData, PropData.GetArrayInnerType()));
			}
		}

		SetProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadArray) : SetProperty(InputStream, PropData) {}

		SetProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadMap) : SetProperty(InputStream, PropData) {}

		SetProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::SetProperty;
		}
	};
}
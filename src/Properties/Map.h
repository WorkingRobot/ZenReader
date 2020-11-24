#pragma once

#include "Base.h"

namespace Zen::Properties {
	class MapProperty : public BaseProperty {
	public:
		std::vector<std::pair<std::unique_ptr<BaseProperty>, std::unique_ptr<BaseProperty>>> Value;

		MapProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData) {
			int NumKeysToRemove;
			InputStream >> NumKeysToRemove;
			for (int i = 0; i < NumKeysToRemove; ++i) {
				BaseProperty::Serialize<EReadType::MAP>(InputStream, PropData, PropData.GetMapKeyType());
			}

			int NumEntries;
			InputStream >> NumEntries;
			Value.reserve(NumEntries);
			for (int i = 0; i < NumEntries; ++i) {
				Value.emplace_back(BaseProperty::Serialize<EReadType::MAP>(InputStream, PropData, PropData.GetMapKeyType()), BaseProperty::Serialize<EReadType::MAP>(InputStream, PropData, PropData.GetMapValueType()));
			}
		}

		MapProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadArray) : MapProperty(InputStream, PropData) {}

		MapProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadMap) : MapProperty(InputStream, PropData) {}

		MapProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::MapProperty;
		}
	};
}
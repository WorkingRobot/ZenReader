#pragma once

#include "Base.h"
#include "Serializer.h"

namespace Zen::Properties {
	class MapProperty : public BaseProperty {
	public:
		std::vector<std::pair<std::unique_ptr<BaseProperty>, std::unique_ptr<BaseProperty>>> Value;

		MapProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData) {
			int NumKeysToRemove;
			InputStream >> NumKeysToRemove;
			for (int i = 0; i < NumKeysToRemove; ++i) {
				Serialize<EReadType::MAP>(InputStream, PropData, PropData.GetMapKeyType());
			}

			int NumEntries;
			InputStream >> NumEntries;
			Value.reserve(NumEntries);
			for (int i = 0; i < NumEntries; ++i) {
				Value.emplace_back(Serialize<EReadType::MAP>(InputStream, PropData, PropData.GetMapKeyType()), Serialize<EReadType::MAP>(InputStream, PropData, PropData.GetMapValueType()));
			}
		}

		MapProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData, EReadArray) : MapProperty(InputStream, PropData) {}

		MapProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData, EReadMap) : MapProperty(InputStream, PropData) {}

		MapProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::MapProperty;
		}
	};
}
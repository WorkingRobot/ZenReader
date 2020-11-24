#pragma once

#include "Base.h"

namespace Zen::Properties {
	class ArrayProperty : public BaseProperty {
	public:
		std::vector<std::unique_ptr<BaseProperty>> Value;

		ArrayProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData) {
			int SerializeNum;
			InputStream >> SerializeNum;
			Value.reserve(SerializeNum);

			// More code is run here when it's a StructProperty if it's a normal (not unversioned) reader
			// https://github.com/iAmAsval/FModel/blob/e256486b98551c467fa835c0518a00e9f9d97462/FModel/PakReader/Parsers/PropertyTagData/ArrayProperty.cs#L20

			for (int i = 0; i < SerializeNum; ++i) {
				Value.emplace_back(BaseProperty::Serialize<EReadType::ARRAY>(InputStream, PropData, PropData.GetArrayInnerType()));
			}
		}

		ArrayProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadArray) : ArrayProperty(InputStream, PropData) {}

		ArrayProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadMap) : ArrayProperty(InputStream, PropData) {}

		ArrayProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::ArrayProperty;
		}
	};
}
#pragma once

#include "Base.h"
#include "Serializer.h"

namespace Zen::Properties {
	class ArrayProperty : public BaseProperty {
	public:
		std::vector<std::unique_ptr<BaseProperty>> Value;

		ArrayProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData) {
			int SerializeNum;
			InputStream >> SerializeNum;
			Value.reserve(SerializeNum);

			// More code is run here when it's a StructProperty if it's a normal (not unversioned) reader
			// https://github.com/iAmAsval/FModel/blob/e256486b98551c467fa835c0518a00e9f9d97462/FModel/PakReader/Parsers/PropertyTagData/ArrayProperty.cs#L20

			for (int i = 0; i < SerializeNum; ++i) {
				Value.emplace_back(Serialize<EReadType::ARRAY>(InputStream, *PropData.GetData().Array.InnerType));
			}
		}

		ArrayProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData, EReadArray) : ArrayProperty(InputStream, PropData) {}

		ArrayProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData, EReadMap) : ArrayProperty(InputStream, PropData) {}

		ArrayProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::ArrayProperty;
		}
	};
}
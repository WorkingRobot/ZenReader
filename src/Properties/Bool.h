#pragma once

#include "Base.h"

namespace Zen::Properties {
	class BoolProperty : public BaseProperty {
	public:
		bool Value;

		BoolProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData) {
			// If it's a normal (not unversioned) reader, use BoolVal
			InputStream >> Value;
		}

		BoolProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData, EReadArray) {
			InputStream >> Value;
		}

		BoolProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData, EReadMap) {
			InputStream >> Value;
		}

		BoolProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData, EReadZero) : Value(PropData.GetBoolVal()) {}

		EPropertyType GetType() const override {
			return EPropertyType::BoolProperty;
		}
	};
}
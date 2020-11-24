#pragma once

#include "Base.h"

namespace Zen::Properties {
	class BoolProperty : public BaseProperty {
	public:
		bool Value;

		BoolProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData) {
			// If it's a normal (not unversioned) reader, use BoolVal
			InputStream >> Value;
		}

		BoolProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadArray) {
			InputStream >> Value;
		}

		BoolProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadMap) {
			InputStream >> Value;
		}

		BoolProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadZero) : Value(PropData.GetBoolVal()) {}

		EPropertyType GetType() const override {
			return EPropertyType::BoolProperty;
		}
	};
}
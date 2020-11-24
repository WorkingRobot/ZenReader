#pragma once

#include "Base.h"

namespace Zen::Properties {
	class FloatProperty : public BaseProperty {
	public:
		float Value;

		FloatProperty(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		FloatProperty(Streams::BaseStream& InputStream, EReadArray) : FloatProperty(InputStream) {}

		FloatProperty(Streams::BaseStream& InputStream, EReadMap) : FloatProperty(InputStream) {}

		FloatProperty(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::FloatProperty;
		}
	};
}
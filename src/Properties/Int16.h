#pragma once

#include "Base.h"

namespace Zen::Properties {
	class Int16Property : public BaseProperty {
	public:
		int16_t Value;

		Int16Property(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		Int16Property(Streams::BaseStream& InputStream, EReadArray) : Int16Property(InputStream) {}

		Int16Property(Streams::BaseStream& InputStream, EReadMap) : Int16Property(InputStream) {}

		Int16Property(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::Int16Property;
		}
	};
}
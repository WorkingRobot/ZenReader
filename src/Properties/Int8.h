#pragma once

#include "Base.h"

namespace Zen::Properties {
	class Int8Property : public BaseProperty {
	public:
		int8_t Value;

		Int8Property(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		Int8Property(Streams::BaseStream& InputStream, EReadArray) : Int8Property(InputStream) {}

		Int8Property(Streams::BaseStream& InputStream, EReadMap) : Int8Property(InputStream) {}

		Int8Property(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::Int8Property;
		}
	};
}
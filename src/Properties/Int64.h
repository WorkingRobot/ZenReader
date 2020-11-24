#pragma once

#include "Base.h"

namespace Zen::Properties {
	class Int64Property : public BaseProperty {
	public:
		int64_t Value;

		Int64Property(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		Int64Property(Streams::BaseStream& InputStream, EReadArray) : Int64Property(InputStream) {}

		Int64Property(Streams::BaseStream& InputStream, EReadMap) : Int64Property(InputStream) {}

		Int64Property(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::Int64Property;
		}
	};
}
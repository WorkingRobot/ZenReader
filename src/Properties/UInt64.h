#pragma once

#include "Base.h"

namespace Zen::Properties {
	class UInt64Property : public BaseProperty {
	public:
		uint64_t Value;

		UInt64Property(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		UInt64Property(Streams::BaseStream& InputStream, EReadArray) : UInt64Property(InputStream) {}

		UInt64Property(Streams::BaseStream& InputStream, EReadMap) : UInt64Property(InputStream) {}

		UInt64Property(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::UInt64Property;
		}
	};
}
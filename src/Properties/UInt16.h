#pragma once

#include "Base.h"

namespace Zen::Properties {
	class UInt16Property : public BaseProperty {
	public:
		uint16_t Value;

		UInt16Property(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		UInt16Property(Streams::BaseStream& InputStream, EReadArray) : UInt16Property(InputStream) {}

		UInt16Property(Streams::BaseStream& InputStream, EReadMap) : UInt16Property(InputStream) {}

		UInt16Property(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::UInt16Property;
		}
	};
}
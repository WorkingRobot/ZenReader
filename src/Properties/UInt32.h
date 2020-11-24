#pragma once

#include "Base.h"

namespace Zen::Properties {
	class UInt32Property : public BaseProperty {
	public:
		uint32_t Value;

		UInt32Property(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		UInt32Property(Streams::BaseStream& InputStream, EReadArray) : UInt32Property(InputStream) {}

		UInt32Property(Streams::BaseStream& InputStream, EReadMap) : UInt32Property(InputStream) {}

		UInt32Property(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::UInt32Property;
		}
	};
}
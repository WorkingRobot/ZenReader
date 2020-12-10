#pragma once

#include "Base.h"

namespace Zen::Properties {
	using namespace Exceptions;

	class ByteProperty : public BaseProperty {
	public:
		uint8_t Value;

		ByteProperty(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		ByteProperty(Streams::BaseStream& InputStream, EReadArray) {
			InputStream >> Value;
		}

		ByteProperty(Streams::BaseStream& InputStream, EReadMap) {
			uint32_t Val;
			InputStream >> Val;
			Value = Val;
		}

		ByteProperty(Streams::BaseStream& InputStream, EReadZero) : Value(0) {}

		EPropertyType GetType() const override {
			return EPropertyType::ByteProperty;
		}
	};
}
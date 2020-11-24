#pragma once

#include "Base.h"

namespace Zen::Properties {
	class StrProperty : public BaseProperty {
	public:
		std::string Value;

		StrProperty(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		StrProperty(Streams::BaseStream& InputStream, EReadArray) : StrProperty(InputStream) {}

		StrProperty(Streams::BaseStream& InputStream, EReadMap) : StrProperty(InputStream) {}

		StrProperty(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::StrProperty;
		}
	};
}
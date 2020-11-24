#pragma once

#include "Base.h"

namespace Zen::Properties {
	class IntProperty : public BaseProperty {
	public:
		int Value;

		IntProperty(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		IntProperty(Streams::BaseStream& InputStream, EReadArray) : IntProperty(InputStream) {}

		IntProperty(Streams::BaseStream& InputStream, EReadMap) : IntProperty(InputStream) {}

		IntProperty(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::IntProperty;
		}
	};
}
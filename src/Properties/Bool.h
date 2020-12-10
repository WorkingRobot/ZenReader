#pragma once

#include "Base.h"

namespace Zen::Properties {
	class BoolProperty : public BaseProperty {
	public:
		bool Value;

		BoolProperty(Streams::BaseStream& InputStream) {
			// If it's a normal (not unversioned) reader, use BoolVal
			InputStream >> Value;
		}

		BoolProperty(Streams::BaseStream& InputStream, EReadArray) {
			InputStream >> Value;
		}

		BoolProperty(Streams::BaseStream& InputStream, EReadMap) {
			InputStream >> Value;
		}

		BoolProperty(Streams::BaseStream& InputStream, EReadZero) : Value(false) {}

		EPropertyType GetType() const override {
			return EPropertyType::BoolProperty;
		}
	};
}
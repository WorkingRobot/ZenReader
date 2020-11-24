#pragma once

#include "Base.h"

namespace Zen::Properties {
	class DoubleProperty : public BaseProperty {
	public:
		double Value;

		DoubleProperty(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		DoubleProperty(Streams::BaseStream& InputStream, EReadArray) : DoubleProperty(InputStream) {}

		DoubleProperty(Streams::BaseStream& InputStream, EReadMap) : DoubleProperty(InputStream) {}

		DoubleProperty(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::DoubleProperty;
		}
	};
}
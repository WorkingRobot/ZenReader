#pragma once

#include "../Structs/FFieldPath.h"
#include "Base.h"

namespace Zen::Properties {
	class FieldPathProperty : public BaseProperty {
	public:
		FFieldPath Value;

		FieldPathProperty(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		FieldPathProperty(Streams::BaseStream& InputStream, EReadArray) : FieldPathProperty(InputStream) {}

		FieldPathProperty(Streams::BaseStream& InputStream, EReadMap) : FieldPathProperty(InputStream) {}

		FieldPathProperty(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::FieldPathProperty;
		}
	};
}
#pragma once

#include "../Structs/FName.h"
#include "Base.h"

namespace Zen::Properties {
	class NameProperty : public BaseProperty {
	public:
		Structs::FName Value;

		NameProperty(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		NameProperty(Streams::BaseStream& InputStream, EReadArray) : NameProperty(InputStream) {}

		NameProperty(Streams::BaseStream& InputStream, EReadMap) : NameProperty(InputStream) {}

		NameProperty(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::NameProperty;
		}
	};
}
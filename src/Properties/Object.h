#pragma once

#include "../Structs/FPackageIndex.h"
#include "Base.h"

namespace Zen::Properties {
	class ObjectProperty : public BaseProperty {
	public:
		Structs::FPackageIndex Value;

		ObjectProperty(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		ObjectProperty(Streams::BaseStream& InputStream, EReadArray) : ObjectProperty(InputStream) {}

		ObjectProperty(Streams::BaseStream& InputStream, EReadMap) : ObjectProperty(InputStream) {}

		ObjectProperty(Streams::BaseStream& InputStream, EReadZero) : Value{0} {}

		EPropertyType GetType() const override {
			return EPropertyType::ObjectProperty;
		}
	};
}
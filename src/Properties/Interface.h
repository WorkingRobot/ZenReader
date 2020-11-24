#pragma once

#include "Base.h"

namespace Zen::Properties {
	class InterfaceProperty : public BaseProperty {
	public:
		uint32_t ObjectRef;

		InterfaceProperty(Streams::BaseStream& InputStream) {
			InputStream >> ObjectRef;
		}

		InterfaceProperty(Streams::BaseStream& InputStream, EReadArray) : InterfaceProperty(InputStream) {}

		InterfaceProperty(Streams::BaseStream& InputStream, EReadMap) : InterfaceProperty(InputStream) {}

		InterfaceProperty(Streams::BaseStream& InputStream, EReadZero) : ObjectRef() {}

		EPropertyType GetType() const override {
			return EPropertyType::InterfaceProperty;
		}
	};
}
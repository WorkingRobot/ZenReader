#pragma once

#include "../Structs/FName.h"
#include "Base.h"

namespace Zen::Properties {
	class DelegateProperty : public BaseProperty {
	public:
		int Object;
		Structs::FName Name;

		DelegateProperty(Streams::BaseStream& InputStream) {
			InputStream >> Object;
			InputStream >> Name;
		}

		DelegateProperty(Streams::BaseStream& InputStream, EReadArray) : DelegateProperty(InputStream) {}

		DelegateProperty(Streams::BaseStream& InputStream, EReadMap) : DelegateProperty(InputStream) {}

		DelegateProperty(Streams::BaseStream& InputStream, EReadZero) : Object(), Name() {}

		EPropertyType GetType() const override {
			return EPropertyType::DelegateProperty;
		}
	};
}
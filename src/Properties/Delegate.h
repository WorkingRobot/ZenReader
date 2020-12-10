#pragma once

#include "../Structs/FScriptDelegate.h"
#include "Base.h"

namespace Zen::Properties {
	class DelegateProperty : public BaseProperty {
	public:
		FScriptDelegate Value;

		DelegateProperty(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		DelegateProperty(Streams::BaseStream& InputStream, EReadArray) : DelegateProperty(InputStream) {}

		DelegateProperty(Streams::BaseStream& InputStream, EReadMap) : DelegateProperty(InputStream) {}

		DelegateProperty(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::DelegateProperty;
		}
	};
}
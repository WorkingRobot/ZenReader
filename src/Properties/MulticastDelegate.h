#pragma once

#include "../Structs/FScriptDelegate.h"
#include "Base.h"

namespace Zen::Properties {
	class MulticastDelegateProperty : public BaseProperty {
	public:
		std::vector<Structs::FScriptDelegate> Value;

		MulticastDelegateProperty(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		MulticastDelegateProperty(Streams::BaseStream& InputStream, EReadArray) : MulticastDelegateProperty(InputStream) {}

		MulticastDelegateProperty(Streams::BaseStream& InputStream, EReadMap) : MulticastDelegateProperty(InputStream) {}

		MulticastDelegateProperty(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::MulticastDelegateProperty;
		}
	};
}
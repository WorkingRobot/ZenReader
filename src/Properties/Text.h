#pragma once

#include "../Structs/FText.h"
#include "Base.h"

namespace Zen::Properties {
	class TextProperty : public BaseProperty {
	public:
		Structs::FText Value;

		TextProperty(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		TextProperty(Streams::BaseStream& InputStream, EReadArray) : TextProperty(InputStream) {}

		TextProperty(Streams::BaseStream& InputStream, EReadMap) : TextProperty(InputStream) {}

		TextProperty(Streams::BaseStream& InputStream, EReadZero) : Value{ nullptr, ETextFlag::Immutable } {}

		EPropertyType GetType() const override {
			return EPropertyType::TextProperty;
		}
	};
}
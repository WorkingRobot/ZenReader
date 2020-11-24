#pragma once

#include "../Structs/FName.h"
#include "Base.h"

namespace Zen::Properties {
	class ByteProperty : public BaseProperty {
	public:
		uint8_t Value;

		ByteProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData) {
			if (PropData.GetByteEnumName()) {
				Structs::FName Name;
				InputStream >> Name;
				Value = Name.Index; // not accurate, use the string value
			}
			else {
				// if the FPropertyTag has an EnumName, this is an FName instead
				InputStream >> Value;
			}
		}

		ByteProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadArray) {
			InputStream >> Value;
		}

		ByteProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadMap) {
			uint32_t Val;
			InputStream >> Val;
			Value = Val;
		}

		ByteProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadZero) : Value(0) {}

		EPropertyType GetType() const override {
			return EPropertyType::ByteProperty;
		}
	};
}
#pragma once

#include "Base.h"

namespace Zen::Properties {
	class EnumProperty : public BaseProperty {
	public:
		std::string Value;

		// https://github.com/EpicGames/UnrealEngine/blob/bf95c2cbc703123e08ab54e3ceccdd47e48d224a/Engine/Source/Runtime/CoreUObject/Private/UObject/PropertySet.cpp#L216
		EnumProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData) {
			int EnumIdx;
			if (PropData.GetEnumType() != EPropertyType::IntProperty) {
				uint8_t Val;
				InputStream >> Val;
				EnumIdx = Val;
			}
			else {
				InputStream >> EnumIdx;
			}
			// If a normal (not unversioned) reader, read it a as FName instead
			auto Provider = (const Providers::BaseProvider*)InputStream.GetProperty<Streams::PropId::Provider>();
			if (Provider) {
				auto Enum = Provider->GetEnum(PropData.GetEnumName());
				if (Enum) {
					this->Value = PropData.GetEnumName().string() + "::" + (*Enum)[EnumIdx].string();
					return;
				}
			}
			this->Value = PropData.GetEnumName().string() + "::" + std::to_string(EnumIdx);
		}

		EnumProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadArray) : EnumProperty(InputStream, PropData) {}

		EnumProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadMap) : EnumProperty(InputStream, PropData) {}

		EnumProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::EnumProperty;
		}
	};
}
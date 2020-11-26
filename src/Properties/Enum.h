#pragma once

#include "../Exceptions/BaseException.h"
#include "Base.h"

namespace Zen::Properties {
	using namespace Exceptions;

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
			if (!Provider) {
				throw EnumNotFoundException("A schema/enum provider was not given");
			}
			auto Enum = Provider->GetEnum(PropData.GetEnumName());
			if (!Enum) {
				throw EnumNotFoundException("The enum \"%s\" was not found", PropData.GetEnumName().c_str());
			}
			auto EnumIdxName = (*Enum)[EnumIdx];
			if (EnumIdxName) {
				this->Value = PropData.GetEnumName().string() + "::" + EnumIdxName->string();
			}
			else {
				this->Value = PropData.GetEnumName().string() + "::" + std::to_string(EnumIdx);
			}
		}

		EnumProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadArray) : EnumProperty(InputStream, PropData) {}

		EnumProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadMap) : EnumProperty(InputStream, PropData) {}

		EnumProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::EnumProperty;
		}
	};
}
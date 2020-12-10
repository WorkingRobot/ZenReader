#pragma once

#include "../Exceptions/BaseException.h"
#include "Base.h"

namespace Zen::Properties {
	using namespace Exceptions;

	class EnumProperty : public BaseProperty {
	public:
		std::string Value;

		// https://github.com/EpicGames/UnrealEngine/blob/bf95c2cbc703123e08ab54e3ceccdd47e48d224a/Engine/Source/Runtime/CoreUObject/Private/UObject/PropertySet.cpp#L216
		EnumProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData) {
			int EnumIdx;
			switch (PropData.GetData().Enum.InnerType->GetType())
			{
			case EPropertyType::ByteProperty:
			{
				uint8_t Val;
				InputStream >> Val;
				EnumIdx = Val;
				break;
			}
			case EPropertyType::IntProperty:
				InputStream >> EnumIdx;
				break;
			default:
				throw PropertyTypeNotFoundException("Enum properties only support Byte or Int enum types");
			}

			// If a normal (not unversioned) reader, read it a as FName instead
			auto Provider = (const Providers::BaseProvider*)InputStream.GetProperty<Streams::PropId::Provider>();
			if (!Provider) {
				throw StreamPropertyNotFoundException("EnumProperty must be deserialized from ZExport");
			}
			auto& EnumName = PropData.GetData().Enum.EnumName.get();
			auto Enum = Provider->GetEnum(EnumName);
			if (!Enum) {
				throw EnumNotFoundException("The enum \"%s\" was not found", EnumName.c_str());
			}
			if (EnumIdx < Enum->GetNameCount()) {
				this->Value = EnumName.string() + "::" + Enum->GetName(EnumIdx).string();
			}
			else {
				this->Value = EnumName.string() + "::" + std::to_string(EnumIdx);
			}
		}

		EnumProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData, EReadArray) : EnumProperty(InputStream, PropData) {}

		EnumProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData, EReadMap) : EnumProperty(InputStream, PropData) {}

		EnumProperty(Streams::BaseStream& InputStream, const Providers::PropertyData& PropData, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::EnumProperty;
		}
	};
}
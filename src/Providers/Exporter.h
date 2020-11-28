#pragma once

// No reason to use this code in client code, this is only used to export existing providers
// to the more optimized SmartProvider

#include "../Exceptions/BaseException.h"
#include "../Helpers/Compress.h"
#include "../Streams/MemoryStream.h"
#include "Base.h"

namespace Zen::Providers {
	using namespace Exceptions;

	namespace {
		template<typename T>
		uint16_t GetNameIdx(const BaseName* Val, const T& NameLUT) {
			uint16_t i = 0;
			for (auto& Name : NameLUT) {
				if (Val == &Name) {
					return i;
				}
				++i;
			}
			throw NameNotFoundException("The name \"%s\" could not be found", Val->c_str());
		}
	}

	template<typename T>
	void Export(const char* OutputFile, const T& Provider) {
		Streams::MemoryStream OutputStream;

		OutputStream << (uint16_t)Provider.NameLUT.size();
		for (auto& Name : Provider.NameLUT) {
			OutputStream << (uint8_t)Name.size();
			OutputStream.write(Name.c_str(), Name.size());
		}

		OutputStream << (uint16_t)Provider.Enums.size();
		for (auto& Enum : Provider.Enums) {
			OutputStream << GetNameIdx(Enum.first, Provider.NameLUT);

			OutputStream << (uint8_t)Enum.second.Names.size();
			for (auto& Name : Enum.second.Names) {
				OutputStream << GetNameIdx(Name, Provider.NameLUT);
			}
		}

		OutputStream << (uint16_t)Provider.Schemas.size();
		for (auto& Schema : Provider.Schemas) {
			OutputStream << GetNameIdx(Schema.first, Provider.NameLUT);

			OutputStream << (uint8_t)Schema.second.Properties.size();
			for (auto& Prop : Schema.second.Properties) {
				OutputStream << Prop.GetSchemaIdx();
				OutputStream << GetNameIdx(&Prop.GetName(), Provider.NameLUT);
				OutputStream << (uint8_t)Prop.GetType();
				auto& Data = Prop.GetData();
				switch (Prop.GetType())
				{
				case EPropertyType::BoolProperty:
					OutputStream << Data.GetBoolVal();
					break;
				case EPropertyType::EnumProperty:
					OutputStream << GetNameIdx(&Data.GetEnumName(), Provider.NameLUT);
					OutputStream << (uint8_t)Data.GetEnumType();
					break;
				case EPropertyType::ByteProperty:
					if (Data.GetByteEnumName()) {
						OutputStream << GetNameIdx(Data.GetByteEnumName(), Provider.NameLUT);
					}
					else {
						OutputStream << (uint16_t)USHRT_MAX;
					}
					break;
				case EPropertyType::StructProperty:
					OutputStream << GetNameIdx(&Data.GetStructType(), Provider.NameLUT);
					break;
				case EPropertyType::SetProperty:
				case EPropertyType::ArrayProperty:
					OutputStream << (uint8_t)Data.GetArrayInnerType();
					if (Data.GetArrayInnerType() == EPropertyType::StructProperty) {
						OutputStream << GetNameIdx(&Data.GetStructType(), Provider.NameLUT);
					}
					break;
				case EPropertyType::MapProperty:
					OutputStream << (uint8_t)Data.GetMapKeyType();
					OutputStream << (uint8_t)Data.GetMapValueType();
					if (Data.GetMapKeyType() == EPropertyType::StructProperty || Data.GetMapValueType() == EPropertyType::StructProperty) {
						OutputStream << GetNameIdx(&Data.GetStructType(), Provider.NameLUT);
					}
					break;
				}
			}
		}

		SINTa CompSize;
		auto CompData = Helpers::Compress(OutputStream.get(), OutputStream.size(), OodleLZ_Compressor_Leviathan, OodleLZ_CompressionLevel_Max, CompSize);

		Streams::MemoryStream OutputFileStream;
		OutputFileStream << (uint16_t)0x30C4; // japanese smiley face :)
		OutputFileStream << (uint32_t)CompSize;
		OutputFileStream << (uint32_t)OutputStream.size();
		OutputFileStream.write(CompData.get(), CompSize);

		OutputFileStream.Dump(OutputFile);
	}
}
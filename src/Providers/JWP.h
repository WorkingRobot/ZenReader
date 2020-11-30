#pragma once

#ifndef USE_JSON
#error JWP providers cannot be used without the USE_JSON CMake option
#endif

#include "Base.h"
#include "../Exceptions/BaseException.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <deque>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace Zen::Providers::JWP {
	using namespace Exceptions;

	class Provider : public BaseProvider {
	public:
		Provider(const std::filesystem::path& MappingPath) {
			auto ReadBuffer = std::make_unique<char[]>(1 << 16);
			for (auto& EnumFile : std::filesystem::directory_iterator(MappingPath / "enums")) {
				rapidjson::Document EnumDocument;
				auto fp = fopen(EnumFile.path().string().c_str(), "rb");
				rapidjson::FileReadStream is(fp, ReadBuffer.get(), 1 << 16);
				ParseEnum(EnumDocument.ParseStream(is));
				fclose(fp);
			}
			for (auto& ClassFile : std::filesystem::directory_iterator(MappingPath / "classes")) {
				rapidjson::Document SchemaDocument;
				auto fp = fopen(ClassFile.path().string().c_str(), "rb");
				rapidjson::FileReadStream is(fp, ReadBuffer.get(), 1 << 16);
				ParseSchema(SchemaDocument.ParseStream(is));
				fclose(fp);
			}
		}

	private:
		void ParseEnum(const rapidjson::Document& EnumDocument) {
			std::vector<std::reference_wrapper<const NameEntry>> Names;
			auto ValuesJson = EnumDocument["values"].GetArray();
			Names.reserve(ValuesJson.Size());
			for (auto& Value : ValuesJson) {
				Names.emplace_back(GetOrCreateName(Value));
			}
			Enums.emplace_back(GetOrCreateName(EnumDocument["name"]), std::move(Names));
		}

		void ParseSchema(const rapidjson::Document& SchemaDocument) {
			std::vector<Property> Props;
			auto PropsJson = SchemaDocument["properties"].GetArray();
			Props.reserve(PropsJson.Size());
			for (auto& PropJson : PropsJson) {
				auto& MappingData = PropJson["mapping_type"];

				// https://github.com/SirWaddles/JohnWickParse/blob/29bd789abaedcbaa7cfbd1752cf99562dac87730/mappings/classes/FortWeaponMeleeItemDefinition.json#L263
				// Note: Type can be unknown due to "DebugProperty". JWP apparently just throws when it encounters one
				auto& Prop = Props.emplace_back(GetOrCreateName(PropJson["name"]), PropJson["index"].GetUint(), GetPropertyType(MappingData["type"]));

				auto& PropData = Prop.GetEditableData();
				switch (Prop.Type)
				{
				case EPropertyType::BoolProperty:
					PropData.Bool.Bool = false;
					break;
				case EPropertyType::EnumProperty:
					PropData.Enum.Name = GetOrCreateName(MappingData["enum_type"]);
					PropData.Enum.Type = EPropertyType::ByteProperty;
					break;
				case EPropertyType::ByteProperty:
					PropData.Byte.EnumName = nullptr;
					break;
				case EPropertyType::StructProperty:
					PropData.Struct.Type = GetOrCreateName(MappingData["struct_type"]);
					break;
				case EPropertyType::SetProperty:
				case EPropertyType::ArrayProperty:
				{
					auto& InnerJson = MappingData["sub_type"];
					PropData.Array.InnerType = GetPropertyType(InnerJson["type"]);
					if (PropData.Array.InnerType == EPropertyType::StructProperty) {
						PropData.Array.StructType = GetOrCreateName(InnerJson["struct_type"]);
					}
					break;
				}
				case EPropertyType::MapProperty:
				{
					auto& KeyJson = MappingData["key_type"];
					auto& ValueJson = MappingData["value_type"];
					PropData.Map.KeyType = GetPropertyType(KeyJson["type"]);
					PropData.Map.ValueType = GetPropertyType(ValueJson["type"]);
					if (PropData.Map.KeyType == EPropertyType::StructProperty) {
						PropData.Map.StructType = GetOrCreateName(KeyJson["struct_type"]);
					}
					else if (PropData.Map.ValueType == EPropertyType::StructProperty) {
						PropData.Map.StructType = GetOrCreateName(ValueJson["struct_type"]);
					}
					break;
				}
				}
			}
			Schemas.emplace_back(GetOrCreateName(SchemaDocument["name"]), std::move(Props));
		}

		static constexpr EPropertyType GetPropertyType(const rapidjson::Value& Str) {
			return BaseProvider::GetPropertyType(Str.GetString(), Str.GetStringLength());
		}

		const NameEntry& GetOrCreateName(const rapidjson::Value& Str) {
			return BaseProvider::GetOrCreateName(Str.GetString(), Str.GetStringLength());
		}
	};
}

namespace Zen::Providers {
	using JWPProvider = JWP::Provider;
}
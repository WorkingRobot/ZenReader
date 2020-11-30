#pragma once

#ifndef USE_JSON
#error FModel providers cannot be used without the USE_JSON CMake option
#endif

#include "Base.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <filesystem>

namespace Zen::Providers::FModel {
	class Provider : public BaseProvider {
	public:
		Provider(const std::filesystem::path& TypesPath, const std::filesystem::path& EnumsPath) {
			auto readBuffer = std::make_unique<char[]>(1 << 16);
			rapidjson::Document TypeMappings;
			rapidjson::Document EnumMappings;
			{
				auto fp = fopen(TypesPath.string().c_str(), "rb");
				rapidjson::FileReadStream is(fp, readBuffer.get(), 1 << 16);
				TypeMappings.ParseStream(is);
				fclose(fp);
			}
			{
				auto fp = fopen(EnumsPath.string().c_str(), "rb");
				rapidjson::FileReadStream is(fp, readBuffer.get(), 1 << 16);
				EnumMappings.ParseStream(is);
				fclose(fp);
			}
			ParseDocuments(TypeMappings, EnumMappings);
		}

	private:
		void ParseDocuments(const rapidjson::Document& TypeMappings, const rapidjson::Document& EnumMappings) {
			for (auto EnumItr = EnumMappings.MemberBegin(); EnumItr != EnumMappings.MemberEnd(); ++EnumItr) {
				std::vector<std::reference_wrapper<const NameEntry>> EnumNames;
				EnumNames.reserve(EnumItr->value.MemberCount());
				for (auto ValueItr = EnumItr->value.MemberBegin(); ValueItr != EnumItr->value.MemberEnd(); ++ValueItr) {
					EnumNames.emplace_back(GetOrCreateName(ValueItr->value));
					// Simple check, atm all enums always start at 0 and increment by 1
					if ((EnumNames.size() - 1) != atoi(ValueItr->name.GetString())) {
						printf("%s::%s IS INVALID\nis %zu, should be %d\n", EnumItr->name.GetString(), ValueItr->value.GetString(), EnumNames.size() - 1, atoi(ValueItr->name.GetString()));
					}
				}

				Enums.emplace_back(GetOrCreateName(EnumItr->name), std::move(EnumNames));
			}

			for (auto SchemaItr = TypeMappings.MemberBegin(); SchemaItr != TypeMappings.MemberEnd(); ++SchemaItr) {
				std::vector<Property> Props;
				Props.reserve(SchemaItr->value.MemberCount());
				for (auto PropItr = SchemaItr->value.MemberBegin(); PropItr != SchemaItr->value.MemberEnd(); ++PropItr) {
					auto& PropVal = PropItr->value;

					auto& Prop = Props.emplace_back(GetOrCreateName(PropItr->value["name"]), atoi(PropItr->name.GetString()), GetPropertyType(PropItr->value["type"]));

					auto& PropData = Prop.GetEditableData();
					switch (Prop.Type)
					{
					case EPropertyType::BoolProperty:
						PropData.Bool.Bool = PropVal.HasMember("bool") ? PropVal["bool"].GetBool() : false;
						break;
					case EPropertyType::EnumProperty:
						PropData.Enum.Name = GetOrCreateName(PropVal["enumName"]);
						PropData.Enum.Type = PropVal.HasMember("enumType") ? GetPropertyType(PropVal["enumType"]) : EPropertyType::ByteProperty;
						break;
					case EPropertyType::ByteProperty:
						PropData.Byte.EnumName = PropVal.HasMember("enumName") ? &GetOrCreateName(PropVal["enumName"]) : nullptr;
						break;
					case EPropertyType::StructProperty:
						PropData.Struct.Type = GetOrCreateName(PropVal["structType"]);
						break;
					case EPropertyType::SetProperty:
					case EPropertyType::ArrayProperty:
						PropData.Array.InnerType = GetPropertyType(PropVal["innerType"]);
						if (PropData.Array.InnerType == EPropertyType::StructProperty) {
							PropData.Array.StructType = GetOrCreateName(PropVal["structType"]);
						}
						break;
					case EPropertyType::MapProperty:
						PropData.Map.KeyType = GetPropertyType(PropVal["innerType"]);
						PropData.Map.ValueType = GetPropertyType(PropVal["valueType"]);
						if (PropData.Map.KeyType == EPropertyType::StructProperty || PropData.Map.ValueType == EPropertyType::StructProperty) {
							PropData.Map.StructType = GetOrCreateName(PropVal["structType"]);
						}
						break;
					}
				}

				Schemas.emplace_back(GetOrCreateName(SchemaItr->name), std::move(Props));
			}
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
	using FModelProvider = FModel::Provider;
}
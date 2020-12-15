#pragma once

#ifndef USE_JSON
#error FunGames providers cannot be used without the USE_JSON CMake option
#endif

#include "Base.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <filesystem>

namespace Zen::Providers::FunGames {
	class Provider : public BaseProvider {
	public:
		Provider(const std::filesystem::path& MappingPath) {
			static constexpr size_t BufferSize = 1 << 16;
			auto ReadBuffer = std::make_unique<char[]>(BufferSize);
			for (auto& NamespaceEntry : std::filesystem::directory_iterator(MappingPath)) {
				if (NamespaceEntry.is_directory()) {
					ParseNamespaceMappings(NamespaceEntry.path(), ReadBuffer.get(), BufferSize);
				}
			}
		}

	private:
		void ParseNamespaceMappings(const std::filesystem::path& Path, char* ReadBuffer, size_t BufferSize) {
			auto NamespacePrefix = (Path / Path.filename()).string();
			{
				rapidjson::Document EnumDocument;
				auto fp = fopen((NamespacePrefix + "_EnumMappings.json").c_str(), "rb");
				rapidjson::FileReadStream is(fp, ReadBuffer, BufferSize);
				ParseEnumMappings(EnumDocument.ParseStream(is));
				fclose(fp);
			}
			{
				rapidjson::Document StructDocument;
				auto fp = fopen((NamespacePrefix + "_StructMappings.json").c_str(), "rb");
				rapidjson::FileReadStream is(fp, ReadBuffer, BufferSize);
				ParseSchemaMappings(StructDocument.ParseStream(is));
				fclose(fp);
			}
			{
				rapidjson::Document ClassDocument;
				auto fp = fopen((NamespacePrefix + "_ClassMappings.json").c_str(), "rb");
				rapidjson::FileReadStream is(fp, ReadBuffer, BufferSize);
				ParseSchemaMappings(ClassDocument.ParseStream(is));
				fclose(fp);
			}
		}

		void ParseEnumMappings(const rapidjson::Document& Mappings) {
			for (auto& EnumType : Mappings.GetArray()) {
				std::vector<std::reference_wrapper<const NameEntry>> Values;
				auto ValuesJson = EnumType["values"].GetArray();
				Values.reserve(ValuesJson.Size());
				for (auto& ValueJson : ValuesJson) {
					Values.emplace_back(GetOrCreateName(ValueJson));
				}

				Enums.emplace_back(GetOrCreateName(EnumType["name"]), std::move(Values));
			}
		}

		void PopulatePropertyData(const rapidjson::Value& Value, PropertyData& Data) {
			auto PropType = GetPropertyType(Value["type"]);
			auto& PropData = Data.GetEditableData(PropType);
			switch (PropType)
			{
			case EPropertyType::EnumProperty:
			{
				PopulatePropertyData(Value["innerType"], *PropData.Enum.InnerType);
				PropData.Enum.EnumName = GetOrCreateName(Value["enumName"]);
				break;
			}
			case EPropertyType::StructProperty:
			{
				PropData.Struct.StructType = GetOrCreateName(Value["structType"]);
				break;
			}
			case EPropertyType::SetProperty:
			case EPropertyType::ArrayProperty:
			{
				PopulatePropertyData(Value["innerType"], *PropData.Array.InnerType);
				break;
			}
			case EPropertyType::MapProperty:
			{
				PopulatePropertyData(Value["innerType"], *PropData.Map.InnerType);
				PopulatePropertyData(Value["valueType"], *PropData.Map.ValueType);
				break;
			}
			default:
				break;
			}
		}

		void ParseSchemaMappings(const rapidjson::Document& Mappings) {
			for (auto& SchemaType : Mappings.GetArray()) {
				auto PropsJson = SchemaType["properties"].GetArray();
				std::vector<Property> Props;
				Props.reserve(PropsJson.Size());
				for (auto& PropJson : PropsJson) {

					auto& Prop = Props.emplace_back(GetOrCreateName(PropJson["name"]), PropJson["index"].GetUint(), PropJson.HasMember("arraySize") ? PropJson["arraySize"].GetUint() : 1);

					PopulatePropertyData(PropJson["mappingType"], Prop.GetEditableData());
				}

				Schemas.emplace_back(GetOrCreateName(SchemaType["name"]), SchemaType.HasMember("superType") ? &GetOrCreateName(SchemaType["superType"]) : nullptr, SchemaType["propertyCount"].GetUint(), std::move(Props));
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
	using FunGamesProvider = FunGames::Provider;
}
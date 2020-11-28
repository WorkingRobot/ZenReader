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
	class Provider;

	namespace {
		class NameImpl : public BaseName {
			std::string String;

		public:
			NameImpl(const char* Str, size_t StrSize) : String(Str, StrSize) {}

			bool compare(const char* Str, size_t StrSize) const override {
				return size() == StrSize && !memcmp(Str, c_str(), StrSize);
			}

			size_t size() const override {
				return String.size();
			}

			const char* c_str() const override {
				return String.c_str();
			}
		};

		class EnumImpl : public BaseEnum {
		public:
			EnumImpl(std::vector<const BaseName*>&& Names) : Names(std::move(Names)) {}

			const BaseName* operator[](int i) const override {
				if (i >= Names.size()) {
					return nullptr;
				}
				return Names[i];
			}

			std::vector<const BaseName*> Names;
		};

		class PropertyDataImpl : public BasePropertyData {
			union {
				struct {
					bool Bool;
				} Bool;
				struct {
					std::reference_wrapper<const BaseName> Name;
					EPropertyType Type;
				} Enum;
				struct {
					const BaseName* EnumName;
				} Byte;
				struct {
					std::reference_wrapper<const BaseName> Type;
				} Struct;
				struct {
					// UB if not set
					std::reference_wrapper<const BaseName> StructType;
					EPropertyType InnerType;
				} Array;
				struct {
					// UB if not set
					std::reference_wrapper<const BaseName> StructType;
					EPropertyType KeyType;
					EPropertyType ValueType;
				} Map;
			} Data;

			friend class Zen::Providers::JWP::Provider;

		public:
			PropertyDataImpl() : Data{false} {};

			bool GetBoolVal() const override {
				return Data.Bool.Bool;
			}

			const BaseName& GetEnumName() const override {
				return Data.Enum.Name;
			}

			EPropertyType GetEnumType() const override {
				return Data.Enum.Type;
			}

			const BaseName* GetByteEnumName() const override {
				return Data.Byte.EnumName;
			}

			const BaseName& GetStructType() const override {
				return Data.Struct.Type;
			}

			EPropertyType GetArrayInnerType() const override {
				return Data.Array.InnerType;
			}

			EPropertyType GetMapKeyType() const override {
				return Data.Map.KeyType;
			}

			EPropertyType GetMapValueType() const override {
				return Data.Map.ValueType;
			}
		};

		class PropertyImpl : public BaseProperty {
			const BaseName& Name;
			uint16_t SchemaIdx;
			EPropertyType Type;
			PropertyDataImpl Data;

			friend class Zen::Providers::JWP::Provider;

		public:
			PropertyImpl(uint32_t SchemaIdx, const BaseName& Name, EPropertyType Type) :
				SchemaIdx(SchemaIdx),
				Name(Name),
				Type(Type),
				Data()
			{}

			const BaseName& GetName() const override {
				return Name;
			}

			EPropertyType GetType() const override {
				return Type;
			}

			uint16_t GetSchemaIdx() const override {
				return SchemaIdx;
			}

			const BasePropertyData& GetData() const override {
				return Data;
			}
		};

		class SchemaImpl : public BaseSchema {
		public:
			SchemaImpl(std::vector<PropertyImpl>&& Properties) : Properties(std::move(Properties)) {}

			const BaseProperty& operator[](int i) const override {
				if (Properties.size() > i && Properties[i].GetSchemaIdx() == i) {
					return Properties[i];
				}
				auto PropItr = std::find_if(Properties.begin(), Properties.end(), [i](const PropertyImpl& Prop) {
					return Prop.GetSchemaIdx() == i;
				});
				if (PropItr != Properties.end()) {
					return *PropItr;
				}
				throw PropertyNotFoundException("Property %d does not exist", i);
			}

			std::vector<PropertyImpl> Properties;
		};
	}

	class Provider : public BaseProvider {
	public:
		Provider() {
			auto ReadBuffer = std::make_unique<char[]>(1 << 16);
#define BASE_PATH "J:/Code/Visual Studio 2017/Projects/ZenReader/mappings/jwp/"
			for (auto& EnumFile : std::filesystem::directory_iterator(BASE_PATH "enums")) {
				rapidjson::Document EnumDocument;
				auto fp = fopen(EnumFile.path().string().c_str(), "rb");
				rapidjson::FileReadStream is(fp, ReadBuffer.get(), 1 << 16);
				ParseEnum(EnumDocument.ParseStream(is));
				fclose(fp);
			}
			for (auto& ClassFile : std::filesystem::directory_iterator(BASE_PATH "classes")) {
				rapidjson::Document SchemaDocument;
				auto fp = fopen(ClassFile.path().string().c_str(), "rb");
				rapidjson::FileReadStream is(fp, ReadBuffer.get(), 1 << 16);
				ParseSchema(SchemaDocument.ParseStream(is));
				fclose(fp);
			}
#undef BASE_PATH
		}

	private:
		void ParseEnum(const rapidjson::Document& EnumDocument) {
			std::vector<const BaseName*> Names;
			auto ValuesJson = EnumDocument["values"].GetArray();
			Names.reserve(ValuesJson.Size());
			for (auto& Value : ValuesJson) {
				Names.emplace_back(&GetOrCreateName(Value));
			}
			Enums.emplace_back(&GetOrCreateName(EnumDocument["name"]), std::move(Names));
		}

		void ParseSchema(const rapidjson::Document& SchemaDocument) {
			std::vector<PropertyImpl> Props;
			auto PropsJson = SchemaDocument["properties"].GetArray();
			Props.reserve(PropsJson.Size());
			for (auto& PropJson : PropsJson) {
				auto& MappingData = PropJson["mapping_type"];

				// https://github.com/SirWaddles/JohnWickParse/blob/29bd789abaedcbaa7cfbd1752cf99562dac87730/mappings/classes/FortWeaponMeleeItemDefinition.json#L263
				// Note: Type can be unknown due to "DebugProperty". JWP apparently just throws when it encounters one
				auto& Prop = Props.emplace_back(PropJson["index"].GetUint(), GetOrCreateName(PropJson["name"]), GetPropertyType(MappingData["type"]));

				switch (Prop.Type)
				{
				case EPropertyType::BoolProperty:
					Prop.Data.Data.Bool.Bool = false;
					break;
				case EPropertyType::EnumProperty:
					Prop.Data.Data.Enum.Name = GetOrCreateName(MappingData["enum_type"]);
					Prop.Data.Data.Enum.Type = EPropertyType::ByteProperty;
					break;
				case EPropertyType::ByteProperty:
					Prop.Data.Data.Byte.EnumName = nullptr;
					break;
				case EPropertyType::StructProperty:
					Prop.Data.Data.Struct.Type = GetOrCreateName(MappingData["struct_type"]);
					break;
				case EPropertyType::SetProperty:
				case EPropertyType::ArrayProperty:
				{
					auto& InnerJson = MappingData["sub_type"];
					Prop.Data.Data.Array.InnerType = GetPropertyType(InnerJson["type"]);
					if (Prop.Data.Data.Array.InnerType == EPropertyType::StructProperty) {
						Prop.Data.Data.Array.StructType = GetOrCreateName(InnerJson["struct_type"]);
					}
					break;
				}
				case EPropertyType::MapProperty:
				{
					auto& KeyJson = MappingData["key_type"];
					auto& ValueJson = MappingData["value_type"];
					Prop.Data.Data.Map.KeyType = GetPropertyType(KeyJson["type"]);
					Prop.Data.Data.Map.ValueType = GetPropertyType(ValueJson["type"]);
					if (Prop.Data.Data.Map.KeyType == EPropertyType::StructProperty) {
						Prop.Data.Data.Map.StructType = GetOrCreateName(KeyJson["struct_type"]);
					}
					else if (Prop.Data.Data.Map.ValueType == EPropertyType::StructProperty) {
						Prop.Data.Data.Map.StructType = GetOrCreateName(ValueJson["struct_type"]);
					}
					break;
				}
				}
			}
			Schemas.emplace_back(&GetOrCreateName(SchemaDocument["name"]), std::move(Props));
		}

		static constexpr EPropertyType GetPropertyType(const rapidjson::Value& Str) {
			return BaseProvider::GetPropertyType(Str.GetString(), Str.GetStringLength());
		}

		const BaseName& GetOrCreateName(const rapidjson::Value& Str) {
			return GetOrCreateName(Str.GetString(), Str.GetStringLength());
		}

		const BaseName& GetOrCreateName(const char* Str, size_t StrSize) {
			auto Name = GetName(Str, StrSize);
			if (Name) {
				return *Name;
			}
			return NameLUT.emplace_back(Str, StrSize);
		}

	public:
		std::vector<std::pair<const BaseName*, EnumImpl>> Enums;
		std::vector<std::pair<const BaseName*, SchemaImpl>> Schemas;

		// We use pointers to the values inside, only emplace_back is allowed, don't invalidate anything!
		std::deque<NameImpl> NameLUT;

		const BaseName* GetName(const char* Str, size_t StrSize) const override {
			auto Itr = std::find_if(NameLUT.begin(), NameLUT.end(), [&](const NameImpl& Name) {
				return Name.compare(Str, StrSize);
			});
			if (Itr == NameLUT.end()) {
				return nullptr;
			}
			return &*Itr;
		}

		const BaseEnum* GetEnum(const BaseName& Str) const override {
			auto Itr = std::find_if(Enums.begin(), Enums.end(), [&](const std::pair<const BaseName*, EnumImpl>& Enum) {
				return Enum.first == &Str;
			});
			if (Itr == Enums.end()) {
				return nullptr;
			}
			return &Itr->second;
		}

		const BaseSchema* GetSchema(const BaseName& Str) const override {
			auto Itr = std::find_if(Schemas.begin(), Schemas.end(), [&](const std::pair<const BaseName*, SchemaImpl>& Schema) {
				return Schema.first == &Str;
			});
			if (Itr == Schemas.end()) {
				return nullptr;
			}
			return &Itr->second;
		}
	};
}

namespace Zen::Providers {
	using JWPProvider = JWP::Provider;
}
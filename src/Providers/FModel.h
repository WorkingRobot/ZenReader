#pragma once

#ifndef USE_JSON
#error FModel providers cannot be used without the USE_JSON CMake option
#endif

#include "Base.h"
#include "../Exceptions/BaseException.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <deque>
#include <string>
#include <memory>
#include <vector>

namespace Zen::Providers::FModel {
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
			std::vector<const BaseName*> Names;

		public:
			EnumImpl(std::vector<const BaseName*>&& Names) : Names(std::move(Names)) {}

			const BaseName* operator[](int i) const override {
				if (i >= Names.size()) {
					return nullptr;
				}
				return Names[i];
			}
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

			friend class Zen::Providers::FModel::Provider;

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

			friend class Zen::Providers::FModel::Provider;

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
			std::vector<PropertyImpl> Properties;

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
		};
	}

	class Provider : public BaseProvider {
	public:
		Provider() {
			auto readBuffer = std::make_unique<char[]>(1 << 16);
			rapidjson::Document TypeMappings;
			rapidjson::Document EnumMappings;
#define BASE_PATH "J:/Code/Visual Studio 2017/Projects/ZenReader/mappings/fmodel/"
			{
				auto fp = fopen(BASE_PATH "TypeMappings.json", "rb");
				rapidjson::FileReadStream is(fp, readBuffer.get(), 1 << 16);
				TypeMappings.ParseStream(is);
				fclose(fp);
			}
			{
				auto fp = fopen(BASE_PATH "EnumMappings.json", "rb");
				rapidjson::FileReadStream is(fp, readBuffer.get(), 1 << 16);
				EnumMappings.ParseStream(is);
				fclose(fp);
			}
#undef BASE_PATH
			ParseDocuments(TypeMappings, EnumMappings);
		}

	private:
		void ParseDocuments(const rapidjson::Document& TypeMappings, const rapidjson::Document& EnumMappings) {
			for (auto EnumItr = EnumMappings.MemberBegin(); EnumItr != EnumMappings.MemberEnd(); ++EnumItr) {
				std::vector<const BaseName*> EnumNames;
				EnumNames.reserve(EnumItr->value.MemberCount());
				for (auto ValueItr = EnumItr->value.MemberBegin(); ValueItr != EnumItr->value.MemberEnd(); ++ValueItr) {
					EnumNames.emplace_back(&GetOrCreateName(ValueItr->value));
					// Simple check, atm all enums always start at 0 and increment by 1
					if ((EnumNames.size() - 1) != atoi(ValueItr->name.GetString())) {
						printf("%s::%s IS INVALID\nis %zu, should be %d\n", EnumItr->name.GetString(), ValueItr->value.GetString(), EnumNames.size() - 1, atoi(ValueItr->name.GetString()));
					}
				}
				Enums.emplace_back(&GetOrCreateName(EnumItr->name), std::move(EnumNames));
			}

			for (auto SchemaItr = TypeMappings.MemberBegin(); SchemaItr != TypeMappings.MemberEnd(); ++SchemaItr) {
				std::vector<PropertyImpl> Props;
				Props.reserve(SchemaItr->value.MemberCount());
				for (auto PropItr = SchemaItr->value.MemberBegin(); PropItr != SchemaItr->value.MemberEnd(); ++PropItr) {
					auto& PropVal = PropItr->value;
					auto& PropType = PropItr->value["type"];
					auto EnumType = GetPropertyType(PropType);
					// assert(TypeEnum != PropertyType::Unknown)
					auto& Prop = Props.emplace_back(atoi(PropItr->name.GetString()), GetOrCreateName(PropItr->value["name"]), EnumType);
					switch (EnumType)
					{
					case EPropertyType::BoolProperty:
						Prop.Data.Data.Bool.Bool = PropVal.HasMember("bool") ? PropVal["bool"].GetBool() : false;
						break;
					case EPropertyType::EnumProperty:
						Prop.Data.Data.Enum.Name = GetOrCreateName(PropVal["enumName"]);
						Prop.Data.Data.Enum.Type = PropVal.HasMember("enumType") ? GetPropertyType(PropVal["enumType"]) : EPropertyType::Unknown;
						break;
					case EPropertyType::ByteProperty:
						Prop.Data.Data.Byte.EnumName = PropVal.HasMember("enumName") ? &GetOrCreateName(PropVal["enumName"]) : nullptr;
						break;
					case EPropertyType::StructProperty:
						Prop.Data.Data.Struct.Type = GetOrCreateName(PropVal["structType"]);
						break;
					case EPropertyType::SetProperty:
					case EPropertyType::ArrayProperty:
						Prop.Data.Data.Array.InnerType = GetPropertyType(PropVal["innerType"]);
						if (Prop.Data.Data.Array.InnerType == EPropertyType::StructProperty) {
							Prop.Data.Data.Array.StructType = GetOrCreateName(PropVal["structType"]);
						}
						break;
					case EPropertyType::MapProperty:
						Prop.Data.Data.Map.KeyType = GetPropertyType(PropVal["innerType"]);
						Prop.Data.Data.Map.ValueType = GetPropertyType(PropVal["valueType"]);
						if (Prop.Data.Data.Map.KeyType == EPropertyType::StructProperty || Prop.Data.Data.Map.ValueType == EPropertyType::StructProperty) {
							Prop.Data.Data.Map.StructType = GetOrCreateName(PropVal["structType"]);
						}
						break;
					}
				}
				Schemas.emplace_back(&GetOrCreateName(SchemaItr->name), std::move(Props));
			}
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

		std::vector<std::pair<const BaseName*, EnumImpl>> Enums;
		std::vector<std::pair<const BaseName*, SchemaImpl>> Schemas;

		// We use pointers to the values inside, only emplace_back is allowed, don't invalidate anything!
		std::deque<NameImpl> NameLUT;

	public:
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
	using FModelProvider = FModel::Provider;
}
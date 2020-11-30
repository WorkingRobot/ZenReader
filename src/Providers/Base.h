#pragma once

#include "../Exceptions/BaseException.h"
#include "../Enums/EPropertyType.h"
#include "../Helpers/Hash.h"
#include "../ZSmallMap.h"

#include <deque>
#include <memory>
#include <vector>

namespace Zen::Providers {
	using namespace Exceptions;
	using namespace Enums;

	class NameEntry {
		StrlenKey<uint8_t> Key;

	public:
		template<typename... Args>
		NameEntry(Args&&... KeyArgs) : Key(std::forward<Args>(KeyArgs)...) {}

		std::string string() const {
			return std::string(c_str(), size());
		}

		bool operator==(const NameEntry& other) const {
			return !strcmp(c_str(), other.c_str());
		}

		bool compare(const char* Str, uint8_t StrSize) const {
			return size() == StrSize && !memcmp(Str, c_str(), StrSize);
		}

		uint8_t size() const {
			return Key.GetSize();
		}

		const char* c_str() const {
			return Key.GetString();
		}
	};

	class Enum {
	public:
		std::vector<std::reference_wrapper<const NameEntry>> Names;
		const NameEntry& Name;

		Enum(const NameEntry& Name, std::vector<std::reference_wrapper<const NameEntry>>&& Names) :
			Name(Name),
			Names(std::move(Names))
		{}

		const NameEntry* operator[](int i) const {
			if (i >= Names.size()) {
				return nullptr;
			}
			return &Names[i].get();
		}
	};

	class PropertyData {
		union {
			struct {
				bool Bool;
			} Bool;
			struct {
				std::reference_wrapper<const NameEntry> Name;
				EPropertyType Type;
			} Enum;
			struct {
				const NameEntry* EnumName;
			} Byte;
			struct {
				std::reference_wrapper<const NameEntry> Type;
			} Struct;
			struct {
				// UB if not set
				std::reference_wrapper<const NameEntry> StructType;
				EPropertyType InnerType;
			} Array;
			struct {
				// UB if not set
				std::reference_wrapper<const NameEntry> StructType;
				EPropertyType KeyType;
				EPropertyType ValueType;
			} Map;
		} Data;

	public:
		PropertyData() : Data{ false } {};

		decltype(Data)& GetEditableData() {
			return Data;
		}

		bool GetBoolVal() const {
			return Data.Bool.Bool;
		}

		const NameEntry& GetEnumName() const {
			return Data.Enum.Name;
		}

		EPropertyType GetEnumType() const {
			return Data.Enum.Type;
		}

		const NameEntry* GetByteEnumName() const {
			return Data.Byte.EnumName;
		}

		// Note: This will need to be valid in Array and Map properties
		const NameEntry& GetStructType() const {
			return Data.Struct.Type;
		}

		EPropertyType GetArrayInnerType() const {
			return Data.Array.InnerType;
		}

		EPropertyType GetMapKeyType() const {
			return Data.Map.KeyType;
		}

		EPropertyType GetMapValueType() const {
			return Data.Map.ValueType;
		}
	};

	class Property {
	public:
		const NameEntry& Name;
		uint16_t SchemaIdx;
		EPropertyType Type;
		PropertyData Data;

		Property(const NameEntry& Name, uint16_t SchemaIdx, EPropertyType Type) :
			Name(Name),
			SchemaIdx(SchemaIdx),
			Type(Type)
		{}

		const NameEntry& GetName() const {
			return Name;
		}

		EPropertyType GetType() const {
			return Type;
		}

		uint16_t GetSchemaIdx() const {
			return SchemaIdx;
		}

		const PropertyData& GetData() const {
			return Data;
		}

		decltype(auto) GetEditableData() {
			return Data.GetEditableData();
		}
	};

	class Schema {
	public:
		std::vector<Property> Properties;
		const NameEntry& Name;

		Schema(const NameEntry& Name, std::vector<Property>&& Properties) :
			Name(Name),
			Properties(std::move(Properties))
		{}

		const Property& operator[](int i) const {
			if (Properties.size() > i && Properties[i].GetSchemaIdx() == i) {
				return Properties[i];
			}
			auto PropItr = std::find_if(Properties.begin(), Properties.end(), [i](const Property& Prop) {
				return Prop.GetSchemaIdx() == i;
			});
			if (PropItr != Properties.end()) {
				return *PropItr;
			}
			throw PropertyNotFoundException("Property %d does not exist", i);
		}
	};

	class BaseProvider {
	protected:
		// Only allow inherited classes
		BaseProvider() = default;

	public:
		const NameEntry* GetName(const std::string& Name) const {
			return GetName(Name.c_str(), Name.size());
		}

		const Enum* GetEnum(const std::string& Name) const {
			auto NamePtr = GetName(Name);
			if (NamePtr) {
				return GetEnum(*NamePtr);
			}
			return nullptr;
		}

		const Schema* GetSchema(const std::string& Name) const {
			auto NamePtr = GetName(Name);
			if (NamePtr) {
				return GetSchema(*NamePtr);
			}
			return nullptr;
		}

		const NameEntry* GetName(const char* Str, size_t StrSize) const {
			auto Itr = std::find_if(NameLUT.begin(), NameLUT.end(), [&](const NameEntry& Name) {
				return Name.compare(Str, StrSize);
				});
			if (Itr == NameLUT.end()) {
				return nullptr;
			}
			return &*Itr;
		}

		const Enum* GetEnum(const NameEntry& Str) const {
			auto Itr = std::find_if(Enums.begin(), Enums.end(), [&](const Enum& Enum) {
				return Enum.Name == Str;
			});
			if (Itr == Enums.end()) {
				return nullptr;
			}
			return &*Itr;
		}

		const Schema* GetSchema(const NameEntry& Str) const {
			auto Itr = std::find_if(Schemas.begin(), Schemas.end(), [&](const Schema& Schema) {
				return Schema.Name == Str;
			});
			if (Itr == Schemas.end()) {
				return nullptr;
			}
			return &*Itr;
		}

		std::vector<Enum> Enums;
		std::vector<Schema> Schemas;

		// We use references to the values inside, don't cause any reallocations after the constructor!
		std::deque<NameEntry> NameLUT;

	protected:
		const NameEntry& GetOrCreateName(const char* Str, size_t StrSize) {
			auto Name = GetName(Str, StrSize);
			if (Name) {
				return *Name;
			}
			return NameLUT.emplace_back(Str, StrSize);
		}

		static constexpr EPropertyType GetPropertyType(const char* Str, size_t StrSize) {
			switch (Helpers::Hash::Crc32(Str, StrSize))
			{
#define CASE(Type) case Helpers::Hash::Crc32(#Type): return EPropertyType::Type

				CASE(ByteProperty);
				CASE(BoolProperty);
				CASE(IntProperty);
				CASE(FloatProperty);
				CASE(ObjectProperty);
				CASE(NameProperty);
				CASE(DelegateProperty);
				CASE(DoubleProperty);
				CASE(ArrayProperty);
				CASE(StructProperty);
				CASE(StrProperty);
				CASE(TextProperty);
				CASE(InterfaceProperty);
				//CASE(MulticastDelegateProperty);
				//CASE(LazyObjectProperty);
				CASE(SoftObjectProperty);
				CASE(AssetObjectProperty);
				CASE(UInt64Property);
				CASE(UInt32Property);
				CASE(UInt16Property);
				CASE(Int64Property);
				CASE(Int16Property);
				CASE(Int8Property);
				CASE(MapProperty);
				CASE(SetProperty);
				CASE(EnumProperty);

#undef CASE
			default:
				return EPropertyType::Unknown;
			}
		}
	};
}
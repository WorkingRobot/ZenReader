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
		std::vector<std::reference_wrapper<const NameEntry>> Names;
		const NameEntry& Name;

	public:
		Enum(const NameEntry& Name, std::vector<std::reference_wrapper<const NameEntry>>&& Names) :
			Name(Name),
			Names(std::move(Names))
		{}

		const NameEntry& GetName() const {
			return Name;
		}

		const decltype(Names)& GetNames() const {
			return Names;
		}

		size_t GetNameCount() const {
			return Names.size();
		}

		// No bounds checking, that's up to you :)
		const NameEntry& GetName(int i) const {
			return Names[i].get();
		}
	};

	class PropertyData {
		union DataUnion {
			struct EnumData {
				PropertyData* InnerType;
				std::reference_wrapper<const NameEntry> EnumName;
			} Enum;
			struct StructData {
				std::reference_wrapper<const NameEntry> StructType;
			} Struct;
			struct ArrayData {
				PropertyData* InnerType;
			} Array;
			struct MapData {
				PropertyData* InnerType;
				PropertyData* ValueType;
			} Map;

			DataUnion() {}
		} Data;

		EPropertyType Type = EPropertyType::Unknown;

		void Reconstruct(EPropertyType Type) {
			this->Type = Type;
			switch (Type)
			{
			case EPropertyType::EnumProperty:
				Data.Enum.InnerType = new PropertyData;
				break;
			case EPropertyType::StructProperty:
				break;
			case EPropertyType::SetProperty:
			case EPropertyType::ArrayProperty:
				Data.Array.InnerType = new PropertyData;
				break;
			case EPropertyType::MapProperty:
				Data.Map.InnerType = new PropertyData;
				Data.Map.ValueType = new PropertyData;
				break;
			default:
				break;
			}
		}

	public:
		PropertyData() = default;

		PropertyData(PropertyData&& o) : Type(o.Type) {
			switch (o.Type)
			{
			case EPropertyType::EnumProperty:
				Data.Enum = std::move(o.Data.Enum);
				break;
			case EPropertyType::StructProperty:
				Data.Struct = std::move(o.Data.Struct);
				break;
			case EPropertyType::SetProperty:
			case EPropertyType::ArrayProperty:
				Data.Array = std::move(o.Data.Array);
				break;
			case EPropertyType::MapProperty:
				Data.Map = std::move(o.Data.Map);
				break;
			default:
				break;
			}
		}

		PropertyData(EPropertyType Type) {
			Reconstruct(Type);
		}

		~PropertyData() {
			switch (Type)
			{
			case EPropertyType::EnumProperty:
				delete Data.Enum.InnerType;
				break;
			case EPropertyType::StructProperty:
				break;
			case EPropertyType::SetProperty:
			case EPropertyType::ArrayProperty:
				delete Data.Array.InnerType;
				break;
			case EPropertyType::MapProperty:
				delete Data.Map.InnerType;
				delete Data.Map.ValueType;
				break;
			default:
				break;
			}
		}

		EPropertyType GetType() const {
			return Type;
		}

		const DataUnion& GetData() const {
			return Data;
		}

		DataUnion& GetEditableData(EPropertyType Type) {
			Reconstruct(Type);
			return Data;
		}
	};

	class Property {
		const NameEntry& Name;
		uint16_t SchemaIdx;
		PropertyData Data;

	public:
		Property(const NameEntry& Name, uint16_t SchemaIdx) :
			Name(Name),
			SchemaIdx(SchemaIdx)
		{}

		const NameEntry& GetName() const {
			return Name;
		}

		uint16_t GetSchemaIdx() const {
			return SchemaIdx;
		}

		const PropertyData& GetData() const {
			return Data;
		}

		PropertyData& GetEditableData() {
			return Data;
		}
	};

	class Schema {
		std::vector<Property> Properties;
		const NameEntry& Name;
		const NameEntry* SuperType;
		uint16_t PropCount;

	public:
		Schema(const NameEntry& Name, const NameEntry* SuperType, uint16_t PropCount, std::vector<Property>&& Properties) :
			Name(Name),
			SuperType(SuperType),
			PropCount(PropCount),
			Properties(std::move(Properties))
		{}

		const NameEntry& GetName() const {
			return Name;
		}

		bool HasSuperType() const {
			return SuperType != nullptr;
		}

		// Check if HasSuperType first!
		const NameEntry& GetSuperType() const {
			return *SuperType;
		}

		uint16_t GetPropCount() const {
			return PropCount;
		}

		const decltype(Properties)& GetProps() const {
			return Properties;
		}

		uint16_t GetSerializablePropCount() const {
			return Properties.size();
		}

		const Property& GetProp(int SchemaIdx) const {
			if (Properties.size() > SchemaIdx && Properties[SchemaIdx].GetSchemaIdx() == SchemaIdx) {
				return Properties[SchemaIdx];
			}
			auto PropItr = std::find_if(Properties.begin(), Properties.end(), [SchemaIdx](const Property& Prop) {
				return Prop.GetSchemaIdx() == SchemaIdx;
			});
			if (PropItr != Properties.end()) {
				return *PropItr;
			}
			throw PropertyNotFoundException("Property at %d does not exist", SchemaIdx);
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
				return Enum.GetName() == Str;
			});
			if (Itr == Enums.end()) {
				return nullptr;
			}
			return &*Itr;
		}

		const Schema* GetSchema(const NameEntry& Str) const {
			auto Itr = std::find_if(Schemas.begin(), Schemas.end(), [&](const Schema& Schema) {
				return Schema.GetName() == Str;
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
				CASE(MulticastDelegateProperty);
				CASE(WeakObjectProperty);
				CASE(LazyObjectProperty);
				CASE(AssetObjectProperty);
				CASE(SoftObjectProperty);
				CASE(UInt64Property);
				CASE(UInt32Property);
				CASE(UInt16Property);
				CASE(Int64Property);
				CASE(Int16Property);
				CASE(Int8Property);
				CASE(MapProperty);
				CASE(SetProperty);
				CASE(EnumProperty);
				CASE(FieldPathProperty);

#undef CASE
			default:
				throw Exceptions::BaseException("Invalid prop name: %.*s", StrSize, Str);
			}
		}
	};
}
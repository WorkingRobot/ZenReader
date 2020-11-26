#pragma once

#include "../Enums/EPropertyType.h"
#include "../Helpers/Hash.h"

namespace Zen::Providers {
	using namespace Enums;

	class BaseName {
	public:
		std::string string() const {
			return std::string(c_str(), size());
		}

		virtual bool compare(const char* Str, size_t StrSize) const = 0;
		virtual size_t size() const = 0;
		virtual const char* c_str() const = 0;
	};

	class BaseEnum {
	public:
		virtual const BaseName* operator[](int) const = 0;
	};

	class BasePropertyData {
	public:
		virtual bool GetBoolVal() const = 0;

		virtual const BaseName& GetEnumName() const = 0;
		virtual EPropertyType GetEnumType() const = 0;

		virtual const BaseName* GetByteEnumName() const = 0;

		// Note: This will need to be valid in Array and Map properties
		virtual const BaseName& GetStructType() const = 0;

		virtual EPropertyType GetArrayInnerType() const = 0;

		virtual EPropertyType GetMapKeyType() const = 0;
		virtual EPropertyType GetMapValueType() const = 0;
	};

	class BaseProperty {
	public:
		virtual const BaseName& GetName() const = 0;
		virtual EPropertyType GetType() const = 0;
		virtual uint16_t GetSchemaIdx() const = 0;
		virtual const BasePropertyData& GetData() const = 0;
	};

	class BaseSchema {
	public:
		virtual const BaseProperty& operator[](int) const = 0;
	};

	class BaseProvider {
	public:
		const BaseName* GetName(const std::string& Name) const {
			return GetName(Name.c_str(), Name.size());
		}

		const BaseEnum* GetEnum(const std::string& Name) const {
			auto NamePtr = GetName(Name);
			if (NamePtr) {
				return GetEnum(*NamePtr);
			}
			return nullptr;
		}

		const BaseSchema* GetSchema(const std::string& Name) const {
			auto NamePtr = GetName(Name);
			if (NamePtr) {
				return GetSchema(*NamePtr);
			}
			return nullptr;
		}

		virtual const BaseName* GetName(const char* Str, size_t StrSize) const = 0;
		virtual const BaseEnum* GetEnum(const BaseName& Str) const = 0;
		virtual const BaseSchema* GetSchema(const BaseName& Str) const = 0;

	protected:
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
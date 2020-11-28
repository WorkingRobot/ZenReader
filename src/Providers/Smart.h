#pragma once

#include "Base.h"
#include "../Exceptions/BaseException.h"
#include "../Helpers/Decompress.h"
#include "../Streams/BufferStream.h"
#include "../Streams/FileStream.h"

#include <memory>
#include <vector>

namespace Zen::Providers::Smart {
	using namespace Exceptions;
	class Provider;

	namespace {
		class NameImpl : public BaseName {
			uint8_t Size;
			std::unique_ptr<char[]> Data;

		public:
			friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, NameImpl& Value) {
				InputStream >> Value.Size;
				Value.Data = std::make_unique<char[]>(Value.Size + 1);
				InputStream.read(Value.Data.get(), Value.Size);
				Value.Data[Value.Size] = '\0';

				return InputStream;
			}

			bool compare(const char* Str, size_t StrSize) const override {
				return size() == StrSize && !memcmp(Str, c_str(), StrSize);
			}

			size_t size() const override {
				return Size;
			}

			const char* c_str() const override {
				return Data.get();
			}
		};

		class EnumImpl : public BaseEnum {
		public:
			friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, EnumImpl& Value) {
				auto NameLUT = (const NameImpl*)InputStream.GetProperty<Streams::PropId::SmartProviderNameLUT>();
				if (!NameLUT) {
					throw StreamPropertyNotFoundException("This must be deserialized from SmartProvider");
				}

				uint8_t NamesSize;
				InputStream >> NamesSize;
				Value.Names.reserve(NamesSize);
				for (uint8_t i = 0; i < NamesSize; ++i) {
					uint16_t Idx;
					InputStream >> Idx;
					Value.Names.emplace_back(NameLUT + Idx);
				}

				return InputStream;
			}

			const BaseName* operator[](int i) const override {
				if (i >= Names.size()) {
					return nullptr;
				}
				return Names[i];
			}

			std::vector<const NameImpl*> Names;
		};

		class PropertyDataImpl : public BasePropertyData {
			union {
				struct {
					bool Bool;
				} Bool;
				struct {
					std::reference_wrapper<const NameImpl> Name;
					EPropertyType Type;
				} Enum;
				struct {
					const NameImpl* EnumName;
				} Byte;
				struct {
					std::reference_wrapper<const NameImpl> Type;
				} Struct;
				struct {
					// UB if not set
					std::reference_wrapper<const NameImpl> StructType;
					EPropertyType InnerType;
				} Array;
				struct {
					// UB if not set
					std::reference_wrapper<const NameImpl> StructType;
					EPropertyType KeyType;
					EPropertyType ValueType;
				} Map;
			} Data;

		public:
			PropertyDataImpl() : Data{ false } {};

			friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, PropertyDataImpl& Value) {
				auto NameLUT = (const NameImpl*)InputStream.GetProperty<Streams::PropId::SmartProviderNameLUT>();
				if (!NameLUT) {
					throw StreamPropertyNotFoundException("This must be deserialized from SmartProvider");
				}
				auto TargetType = (EPropertyType)(size_t)InputStream.GetProperty<Streams::PropId::SmartProviderPropDataType>();

				switch (TargetType)
				{
				case EPropertyType::BoolProperty:
				{
					InputStream >> Value.Data.Bool.Bool;
					break;
				}
				case EPropertyType::EnumProperty:
				{
					uint16_t Idx;
					uint8_t Type;
					InputStream >> Idx;
					InputStream >> Type;
					Value.Data.Enum.Name = NameLUT[Idx];
					Value.Data.Enum.Type = (EPropertyType)Type;
					break;
				}
				case EPropertyType::ByteProperty:
				{
					uint16_t Idx;
					InputStream >> Idx;
					Value.Data.Byte.EnumName = Idx != USHRT_MAX ? NameLUT + Idx : nullptr;
					break;
				}
				case EPropertyType::StructProperty:
				{
					uint16_t Idx;
					InputStream >> Idx;
					Value.Data.Struct.Type = NameLUT[Idx];
					break;
				}
				case EPropertyType::SetProperty:
				case EPropertyType::ArrayProperty:
				{
					uint8_t Type;
					InputStream >> Type;
					Value.Data.Array.InnerType = (EPropertyType)Type;
					if (Value.Data.Array.InnerType == EPropertyType::StructProperty) {
						uint16_t Idx;
						InputStream >> Idx;
						Value.Data.Array.StructType = NameLUT[Idx];
					}
					break;
				}
				case EPropertyType::MapProperty:
				{
					uint8_t KeyType;
					uint8_t ValueType;
					InputStream >> KeyType;
					InputStream >> ValueType;
					Value.Data.Map.KeyType = (EPropertyType)KeyType;
					Value.Data.Map.ValueType = (EPropertyType)ValueType;
					if (Value.Data.Map.KeyType == EPropertyType::StructProperty || Value.Data.Map.ValueType == EPropertyType::StructProperty) {
						uint16_t Idx;
						InputStream >> Idx;
						Value.Data.Map.StructType = NameLUT[Idx];
					}
					break;
				}
				}

				return InputStream;
			}

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
			const NameImpl* Name; // Pointer to let it be default contstructible
			uint16_t SchemaIdx;
			EPropertyType Type;
			PropertyDataImpl Data;

		public:
			friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, PropertyImpl& Value) {
				auto NameLUT = (const NameImpl*)InputStream.GetProperty<Streams::PropId::SmartProviderNameLUT>();
				if (!NameLUT) {
					throw StreamPropertyNotFoundException("This must be deserialized from SmartProvider");
				}

				uint16_t Idx;
				uint8_t Type;
				InputStream >> Value.SchemaIdx;
				InputStream >> Idx;
				InputStream >> Type;
				Value.Name = NameLUT + Idx;
				Value.Type = (EPropertyType)Type;

				Streams::PropertyHolder<Streams::PropId::SmartProviderPropDataType> PropIdHolder(InputStream, (void*)(size_t)Type);
				InputStream >> Value.Data;

				return InputStream;
			}

			const BaseName& GetName() const override {
				return *Name;
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
			friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, SchemaImpl& Value) {
				uint8_t Size;
				InputStream >> Size;
				Value.Properties.reserve(Size);
				for (uint8_t i = 0; i < Size; ++i) {
					InputStream >> Value.Properties.emplace_back();
				}

				return InputStream;
			}

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
			uint32_t DataSize;
			auto Data = ReadCompressedUsmap(DataSize);

			Streams::BufferStream DataStream(Data.get(), DataSize);
			ParseData(DataStream);
		}

	private:
		std::unique_ptr<char[]> ReadCompressedUsmap(uint32_t& DecompSize) {
#define BASE_PATH "J:/Code/Visual Studio 2017/Projects/ZenReader/mappings/smart.usmap"
			Streams::FileStream CompressedInputStream(BASE_PATH);
#undef BASE_PATH

			uint16_t Magic;
			CompressedInputStream >> Magic;
			if (Magic != 0x30C4) {
				throw InvalidMagicException(".usmap file has an invalid magic constant");
			}

			uint32_t CompSize;
			CompressedInputStream >> CompSize;
			CompressedInputStream >> DecompSize;
			if (CompressedInputStream.size() - CompressedInputStream.tell() < CompSize) {
				throw ArchiveCorruptedException("There is not enough data in the .usmap file");
			}

			auto Data = std::make_unique<char[]>(DecompSize);
			auto CompData = std::make_unique<char[]>(CompSize);
			CompressedInputStream.read(CompData.get(), CompSize);

			Helpers::Decompress(Data.get(), DecompSize, CompData.get(), CompSize, "Oodle");
			return Data;
		}

		void ParseData(Streams::BaseStream& InputStream) {
			{
				uint16_t Size;
				InputStream >> Size;
				NameLUT.resize(Size);
				for (auto& Name : NameLUT) {
					InputStream >> Name;
				}
			}

			Streams::PropertyHolder<Streams::PropId::SmartProviderNameLUT> LUTProp(InputStream, (void*)NameLUT.data());

			{
				uint16_t Size;
				InputStream >> Size;
				Enums.reserve(Size);
				for (uint16_t i = 0; i < Size; ++i) {
					uint16_t Idx;
					InputStream >> Idx;

					auto& Enum = Enums.emplace_back(NameLUT.data() + Idx, EnumImpl());
					InputStream >> Enum.second;
				}
			}

			{
				uint16_t Size;
				InputStream >> Size;
				Schemas.reserve(Size);
				for (uint16_t i = 0; i < Size; ++i) {
					uint16_t Idx;
					InputStream >> Idx;

					auto& Schema = Schemas.emplace_back(NameLUT.data() + Idx, SchemaImpl());
					InputStream >> Schema.second;
				}
			}
		}

	public:
		std::vector<std::pair<const NameImpl*, EnumImpl>> Enums;
		std::vector<std::pair<const NameImpl*, SchemaImpl>> Schemas;

		// We use pointers to the values inside, don't cause any reallocations!
		std::vector<NameImpl> NameLUT;

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
	using SmartProvider = Smart::Provider;
}
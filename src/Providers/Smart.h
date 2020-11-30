#pragma once

#include "Base.h"
#include "../Exceptions/BaseException.h"
#include "../Helpers/Decompress.h"
#include "../Streams/BufferStream.h"
#include "../Streams/FileStream.h"

#include <filesystem>

namespace Zen::Providers::Smart {
	using namespace Exceptions;

	class Provider : public BaseProvider {
	public:
		Provider(const std::filesystem::path& UsmapPath) {
			uint32_t DataSize;
			auto Data = ReadCompressedUsmap(UsmapPath, DataSize);

			Streams::BufferStream DataStream(Data.get(), DataSize);
			ParseData(DataStream);
		}

	private:
		std::unique_ptr<char[]> ReadCompressedUsmap(const std::filesystem::path& UsmapPath, uint32_t& DecompSize) {
			Streams::FileStream CompressedInputStream(UsmapPath);

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
				for (uint16_t i = 0; i < Size; ++i) {
					uint8_t NameSize;
					InputStream >> NameSize;
					auto NameData = std::make_unique<char[]>(NameSize + 1);
					InputStream.read(NameData.get(), NameSize);
					NameData[NameSize] = '\0';
					NameLUT.emplace_back(std::move(NameData));
				}
			}

			{
				uint16_t Size;
				InputStream >> Size;
				Enums.reserve(Size);
				for (uint16_t i = 0; i < Size; ++i) {
					uint16_t Idx;
					InputStream >> Idx;

					std::vector<std::reference_wrapper<const NameEntry>> EnumNames;
					uint8_t EnumNamesSize;
					InputStream >> EnumNamesSize;
					EnumNames.reserve(EnumNamesSize);
					for (uint8_t i = 0; i < EnumNamesSize; ++i) {
						uint16_t Idx;
						InputStream >> Idx;
						EnumNames.emplace_back(NameLUT[Idx]);
					}

					Enums.emplace_back(NameLUT[Idx], std::move(EnumNames));
				}
			}

			{
				uint16_t Size;
				InputStream >> Size;
				Enums.reserve(Size);
				for (uint16_t i = 0; i < Size; ++i) {
					uint16_t Idx;
					InputStream >> Idx;

					std::vector<Property> Props;
					uint8_t PropsSize;
					InputStream >> PropsSize;
					Props.reserve(PropsSize);
					for (uint8_t i = 0; i < PropsSize; ++i) {
						uint16_t SchemaIdx;
						uint16_t Idx;
						uint8_t Type;
						InputStream >> SchemaIdx;
						InputStream >> Idx;
						InputStream >> Type;
						auto& Prop = Props.emplace_back(NameLUT[Idx], SchemaIdx, (EPropertyType)Type);

						auto& PropData = Prop.GetEditableData();
						switch (Prop.Type)
						{
						case EPropertyType::BoolProperty: {
							InputStream >> PropData.Bool.Bool;
							break;
						}
						case EPropertyType::EnumProperty:
						{
							uint16_t Idx;
							uint8_t Type;
							InputStream >> Idx;
							InputStream >> Type;
							PropData.Enum.Name = NameLUT[Idx];
							PropData.Enum.Type = (EPropertyType)Type;
							break;
						}
						case EPropertyType::ByteProperty:
						{
							uint16_t Idx;
							InputStream >> Idx;
							PropData.Byte.EnumName = Idx != USHRT_MAX ? &NameLUT[Idx] : nullptr;
							break;
						}
						case EPropertyType::StructProperty:
						{
							uint16_t Idx;
							InputStream >> Idx;
							PropData.Struct.Type = NameLUT[Idx];
							break;
						}
						case EPropertyType::SetProperty:
						case EPropertyType::ArrayProperty:
						{
							uint8_t Type;
							InputStream >> Type;
							PropData.Array.InnerType = (EPropertyType)Type;
							if (PropData.Array.InnerType == EPropertyType::StructProperty) {
								uint16_t Idx;
								InputStream >> Idx;
								PropData.Array.StructType = NameLUT[Idx];
							}
							break;
						}
						case EPropertyType::MapProperty:
						{
							uint8_t KeyType;
							uint8_t ValueType;
							InputStream >> KeyType;
							InputStream >> ValueType;
							PropData.Map.KeyType = (EPropertyType)KeyType;
							PropData.Map.ValueType = (EPropertyType)ValueType;
							if (PropData.Map.KeyType == EPropertyType::StructProperty || PropData.Map.ValueType == EPropertyType::StructProperty) {
								uint16_t Idx;
								InputStream >> Idx;
								PropData.Map.StructType = NameLUT[Idx];
							}
							break;
						}
						}
					}

					Schemas.emplace_back(NameLUT[Idx], std::move(Props));
				}
			}
		}
	};
}

namespace Zen::Providers {
	using SmartProvider = Smart::Provider;
}
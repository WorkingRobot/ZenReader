#pragma once

#include "Base.h"
#include "../Enums/ECompressionMethod.h"
#include "../Exceptions/BaseException.h"
#include "../Helpers/Decompress.h"
#include "../Streams/BufferStream.h"
#include "../Streams/FileStream.h"

#include <filesystem>

namespace Zen::Providers::Smart {
	using namespace Enums;
	using namespace Exceptions;

	namespace {
		// Type to use for indexing the name lut
		using NameIdx = uint32_t;
		// Maximum size of a name (in name lut)
		using NameSize = uint8_t;
		// Type to use for indicating the size of the enum list
		// Literally only saves like 2 bytes or something that's going to be
		// compressed anyway.
		using EnumIdx = uint32_t;
		using EnumNameIdx = uint8_t;

		using SchemaIdx = uint32_t;
		// Has to be the same as PropCount
		// PostProcessSettings struct in Engine has like 300 props, too
		using SchemaPropIdx = uint16_t;

		// First 2 bytes of the usmap
		static constexpr uint16_t FileMagic = 0x30C4;
		static constexpr NameIdx InvalidName = std::numeric_limits<NameIdx>::max();
	}

	enum class Version {
		INITIAL,

		LATEST_PLUS_ONE,
		LATEST = LATEST_PLUS_ONE - 1
	};

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
			if (Magic != FileMagic) {
				throw InvalidMagicException(".usmap file has an invalid magic constant");
			}

			uint8_t Version;
			CompressedInputStream >> Version;
			if (Version != (uint8_t)Version::LATEST) {
				throw InvalidVersionException(".usmap file has invalid version %d", (int)Version);
			}

			uint8_t Method;
			CompressedInputStream >> Method;

			uint32_t CompSize;
			CompressedInputStream >> CompSize;
			CompressedInputStream >> DecompSize;
			if (CompressedInputStream.size() - CompressedInputStream.tell() < CompSize) {
				throw ArchiveCorruptedException("There is not enough data in the .usmap file");
			}

			auto CompData = std::make_unique<char[]>(CompSize);
			CompressedInputStream.read(CompData.get(), CompSize);

			switch ((ECompressionMethod)Method)
			{
			case ECompressionMethod::None:
				if (CompSize != DecompSize) {
					throw DecompressionException("No compression: Compression size must be equal to decompression size");
				}
				return CompData;
			case ECompressionMethod::Oodle:
			{
				auto Data = std::make_unique<char[]>(DecompSize);
				Helpers::DecompressOodle(Data.get(), DecompSize, CompData.get(), CompSize);
				return Data;
			}
#ifdef USE_BROTLI
			case ECompressionMethod::Brotli:
			{
				auto Data = std::make_unique<char[]>(DecompSize);
				Helpers::DecompressBrotli(Data.get(), DecompSize, CompData.get(), CompSize);
				return Data;
			}
#endif
			default:
				throw DecompressionException("Unknown compression method: %d", (int)Method);
			}
		}

		void DeserializePropData(Streams::BaseStream& Stream, PropertyData& PropData) {
			uint8_t PropType;
			Stream >> PropType;
			auto& Data = PropData.GetEditableData((EPropertyType)PropType);
			switch (PropData.GetType())
			{
			case EPropertyType::EnumProperty:
			{
				DeserializePropData(Stream, *Data.Enum.InnerType);
				NameIdx Idx;
				Stream >> Idx;
				Data.Enum.EnumName = NameLUT[Idx];
				break;
			}
			case EPropertyType::StructProperty:
			{
				NameIdx Idx;
				Stream >> Idx;
				Data.Struct.StructType = NameLUT[Idx];
				break;
			}
			case EPropertyType::SetProperty:
			case EPropertyType::ArrayProperty:
				DeserializePropData(Stream, *Data.Array.InnerType);
				break;
			case EPropertyType::MapProperty:
				DeserializePropData(Stream, *Data.Map.InnerType);
				DeserializePropData(Stream, *Data.Map.ValueType);
				break;
			default:
				break;
			}
		}

		void ParseData(Streams::BaseStream& InputStream) {
			{
				NameIdx Size;
				InputStream >> Size;
#ifndef USE_JSON
				NameLUT.reserve(Size);
#endif
				for (NameIdx i = 0; i < Size; ++i) {
					NameSize NameSize;
					InputStream >> NameSize;
					auto NameData = std::make_unique<char[]>(NameSize + 1);
					InputStream.read(NameData.get(), NameSize);
					NameData[NameSize] = '\0';
					NameLUT.emplace_back(std::move(NameData));
				}
			}

			{
				EnumIdx Size;
				InputStream >> Size;
				Enums.reserve(Size);
				for (EnumIdx i = 0; i < Size; ++i) {
					NameIdx Idx;
					InputStream >> Idx;

					std::vector<std::reference_wrapper<const NameEntry>> EnumNames;
					EnumNameIdx EnumNamesSize;
					InputStream >> EnumNamesSize;
					EnumNames.reserve(EnumNamesSize);
					for (EnumNameIdx i = 0; i < EnumNamesSize; ++i) {
						NameIdx Idx;
						InputStream >> Idx;
						EnumNames.emplace_back(NameLUT[Idx]);
					}

					Enums.emplace_back(NameLUT[Idx], std::move(EnumNames));
				}
			}

			{
				SchemaIdx Size;
				InputStream >> Size;
				Schemas.reserve(Size);
				for (SchemaIdx i = 0; i < Size; ++i) {
					NameIdx Idx;
					InputStream >> Idx;
					NameIdx SuperIdx;
					InputStream >> SuperIdx;
					uint16_t PropCount;
					InputStream >> PropCount;

					SchemaPropIdx SerializablePropCount;
					InputStream >> SerializablePropCount;
					std::vector<Property> Props;
					Props.reserve(SerializablePropCount);
					for (SchemaPropIdx i = 0; i < SerializablePropCount; ++i) {
						uint16_t SchemaIdx;
						uint8_t ArraySize;
						NameIdx Idx;
						InputStream >> SchemaIdx;
						InputStream >> ArraySize;
						InputStream >> Idx;
						auto& Prop = Props.emplace_back(NameLUT[Idx], SchemaIdx, ArraySize);
						DeserializePropData(InputStream, Prop.GetEditableData());
					}

					Schemas.emplace_back(NameLUT[Idx], SuperIdx != InvalidName ? &NameLUT[SuperIdx] : nullptr, PropCount, std::move(Props));
				}
			}
		}
	};
}

namespace Zen::Providers {
	using SmartProvider = Smart::Provider;
}
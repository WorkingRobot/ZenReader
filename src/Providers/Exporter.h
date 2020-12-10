#pragma once

// No reason to use this code in client code, this is only used to export existing providers
// to the more optimized SmartProvider

#include "../Enums/ECompressionMethod.h"
#include "../Exceptions/BaseException.h"
#include "../Helpers/Compress.h"
#include "../Streams/MemoryStream.h"
#include "Smart.h"

#include <algorithm>

namespace Zen::Providers {
	using namespace Enums;
	using namespace Exceptions;
	using namespace Smart;

	namespace {
		NameIdx GetNameIdx(const NameEntry& Val, const std::deque<NameEntry>& NameLUT) {
			NameIdx i = 0;
			for (auto& Name : NameLUT) {
				if (Val == Name) {
					return i;
				}
				++i;
			}
			throw NameNotFoundException("The name \"%s\" could not be found", Val.c_str());
		}

		void SerializePropData(Streams::BaseStream& Stream, const PropertyData& PropData, const std::deque<NameEntry>& NameLUT) {
			Stream << (uint8_t)PropData.GetType();
			auto& Data = PropData.GetData();
			switch (PropData.GetType())
			{
			case EPropertyType::EnumProperty:
				SerializePropData(Stream, *Data.Enum.InnerType, NameLUT);
				Stream << GetNameIdx(Data.Enum.EnumName, NameLUT);
				break;
			case EPropertyType::StructProperty:
				Stream << GetNameIdx(Data.Struct.StructType, NameLUT);
				break;
			case EPropertyType::SetProperty:
			case EPropertyType::ArrayProperty:
				SerializePropData(Stream, *Data.Array.InnerType, NameLUT);
				break;
			case EPropertyType::MapProperty:
				SerializePropData(Stream, *Data.Map.InnerType, NameLUT);
				SerializePropData(Stream, *Data.Map.ValueType, NameLUT);
				break;
			default:
				break;
			}
		}

		void WriteUsmapData(const char* OutputFile, ECompressionMethod Compression, uint32_t CompSize, uint32_t DecompSize, const char* CompData) {
			Streams::MemoryStream OutputFileStream;
			OutputFileStream << FileMagic;
			OutputFileStream << (uint8_t)Compression;
			OutputFileStream << CompSize;
			OutputFileStream << DecompSize;
			OutputFileStream.write(CompData, CompSize);

			OutputFileStream.Dump(OutputFile);
		}
	}

	void Export(const char* OutputFile, const BaseProvider& Provider, ECompressionMethod Compression) {
		if (Provider.NameLUT.size() >= std::numeric_limits<NameIdx>::max()) {
			throw BaseException("Name LUT would overflow the index type. LUT size: %zu", Provider.NameLUT.size());
		}

		Streams::MemoryStream OutputStream;

		OutputStream << (NameIdx)Provider.NameLUT.size();
		for (auto& Name : Provider.NameLUT) {
			if (Name.size() > std::numeric_limits<NameSize>::max()) {
				throw BaseException("Name would overflow the size type. Name size: %zu", Name.size());
			}
			OutputStream << (NameSize)Name.size();
			OutputStream.write(Name.c_str(), Name.size());
		}

		OutputStream << (EnumIdx)Provider.Enums.size();
		for (auto& Enum : Provider.Enums) {
			OutputStream << GetNameIdx(Enum.GetName(), Provider.NameLUT);

			OutputStream << (EnumNameIdx)Enum.GetNameCount();
			for (auto& Name : Enum.GetNames()) {
				OutputStream << GetNameIdx(Name, Provider.NameLUT);
			}
		}

		OutputStream << (SchemaIdx)Provider.Schemas.size();
		for (auto& Schema : Provider.Schemas) {
			OutputStream << GetNameIdx(Schema.GetName(), Provider.NameLUT);
			OutputStream << (Schema.HasSuperType() ? GetNameIdx(Schema.GetSuperType(), Provider.NameLUT) : InvalidName);
			OutputStream << Schema.GetPropCount();

			OutputStream << (SchemaPropIdx)Schema.GetSerializablePropCount();
			for (auto& Prop : Schema.GetProps()) {
				OutputStream << Prop.GetSchemaIdx();
				OutputStream << GetNameIdx(Prop.GetName(), Provider.NameLUT);
				SerializePropData(OutputStream, Prop.GetData(), Provider.NameLUT);
			}
		}

		uint32_t CompSize;
		uint32_t DecompSize;
		const char* CompData;
		switch (Compression)
		{
		case ECompressionMethod::None:
		{
			WriteUsmapData(OutputFile, Compression, OutputStream.size(), OutputStream.size(), OutputStream.get());
			break;
		}
		case ECompressionMethod::Oodle:
		{
			SINTa CompSize;
			auto CompData = Helpers::CompressOodle(OutputStream.get(), OutputStream.size(), OodleLZ_Compressor_Leviathan, OodleLZ_CompressionLevel_Max, CompSize);
			WriteUsmapData(OutputFile, Compression, CompSize, OutputStream.size(), CompData.get());
			break;
		}
#ifdef USE_BROTLI
		case ECompressionMethod::Brotli:
		{
			size_t CompSize;
			auto CompData = Helpers::CompressBrotli(OutputStream.get(), OutputStream.size(), BROTLI_MODE_GENERIC, BROTLI_MAX_QUALITY, CompSize);
			WriteUsmapData(OutputFile, Compression, CompSize, OutputStream.size(), CompData.get());
			break;
		}
#endif
		default:
			break;
		}
	}
}
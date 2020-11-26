#pragma once

#include "../Exceptions/BaseException.h"
#include "../Helpers/Hash.h"
#include "../Providers/Base.h"
#include "../Streams/BaseStream.h"
#include "UObject.h"
#include "UCurveTable.h"

#include <any>
#include <memory>

namespace Zen::Exports {
	using namespace Exceptions;

	std::any Serialize(Streams::BaseStream& InputStream, const std::string& ExportType) {
		auto Provider = (const Providers::BaseProvider*)InputStream.GetProperty<Streams::PropId::Provider>();
		if (!Provider) {
			throw SchemaNotFoundException("A schema provider was not given");
		}
		auto Schema = Provider->GetSchema(ExportType);
		if (!Schema) {
			throw SchemaNotFoundException("The schema for the export \"%s\" was not found", ExportType.c_str());
		}

		printf("Exporting %s\n", ExportType.c_str());
		switch (Helpers::Hash::Crc32(ExportType.c_str(), ExportType.size()))
		{
		case Helpers::Hash::Crc32("Texture2D"):
		case Helpers::Hash::Crc32("VirtualTexture2D"):
		case Helpers::Hash::Crc32("CurveTable"):
			return std::make_shared<UCurveTable>(InputStream, *Schema);
		case Helpers::Hash::Crc32("DataTable"):
		case Helpers::Hash::Crc32("FontFace"):
		case Helpers::Hash::Crc32("SoundWave"):
		case Helpers::Hash::Crc32("StringTable"):
		default:
			return std::make_shared<UObject>(InputStream, *Schema);
		}
	}
}
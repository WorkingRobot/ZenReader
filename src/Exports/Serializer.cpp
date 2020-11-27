#include "Serializer.h"

#include "../Exceptions/BaseException.h"
#include "../Helpers/Hash.h"
#include "../Providers/Base.h"
#include "UObject.h"
#include "UCurveTable.h"
#include "UDataTable.h"
#include "UTexture2D.h"

namespace Zen::Exports {
	using namespace Exceptions;

	std::any Serialize(Streams::BaseStream& InputStream, const std::string& ExportType, Streams::BaseStream* BulkStream, size_t BulkOffset) {
		auto Provider = (const Providers::BaseProvider*)InputStream.GetProperty<Streams::PropId::Provider>();
		if (!Provider) {
			throw StreamPropertyNotFoundException("Exports must be deserialized from ZExport");
		}
		auto Schema = Provider->GetSchema(ExportType);
		if (!Schema) {
			throw SchemaNotFoundException("The schema for the export \"%s\" was not found", ExportType.c_str());
		}

		printf("Exporting %s\n", ExportType.c_str());
		switch (Helpers::Hash::Crc32(ExportType.c_str(), ExportType.size()))
		{
		case Helpers::Hash::Crc32("Texture2D"):
		case Helpers::Hash::Crc32("TextureCube"):
		case Helpers::Hash::Crc32("VirtualTexture2D"):
			return std::make_shared<UTexture2D>(InputStream, *Schema, BulkStream, BulkOffset);
		case Helpers::Hash::Crc32("CurveTable"):
			return std::make_shared<UCurveTable>(InputStream, *Schema);
		case Helpers::Hash::Crc32("DataTable"):
			return std::make_shared<UDataTable>(InputStream, *Schema);
		case Helpers::Hash::Crc32("FontFace"):
		case Helpers::Hash::Crc32("SoundWave"):
		default:
			return std::make_shared<UObject>(InputStream, *Schema);
		}
	}
}
#pragma once

#include "../Exceptions/BaseException.h"
#include "../Structs/FStripDataFlags.h"
#include "../Structs/FTexturePlatformData.h"
#include "../Structs/FName.h"
#include "../ZExport.h"
#include "UObject.h"

namespace Zen::Exports {
	using namespace Exceptions;

	class UTexture2D : public UObject {
	public:
		UTexture2D(Streams::BaseStream& InputStream, const Providers::BaseSchema& Schema, Streams::BaseStream* BulkStream, size_t BulkOffset) : UObject(InputStream, Schema) {
			auto ZExp = (const ZExport*)InputStream.GetProperty<Streams::PropId::ZExport>();
			if (!ZExp) {
				throw StreamPropertyNotFoundException("UDataTable must be deserialized from ZExport");
			}

			FStripDataFlags StripFlag0(InputStream);
			FStripDataFlags StripFlag1(InputStream);

			int bIsCooked;
			InputStream >> bIsCooked;
			if (!bIsCooked) {
				return;
			}

			FName PixelFormatName;
			InputStream >> PixelFormatName;
			while (!PixelFormatName.IsNone(ZExp->GetNameMap())) {
				int SkipOffset;
				InputStream >> SkipOffset;

				// if pak version >= RELATIVE_CHUNK_OFFSETS
				int SkipOffsetH;
				InputStream >> SkipOffsetH;

				PlatformDatas.emplace_back(InputStream, BulkStream, BulkOffset);
				InputStream >> PixelFormatName;

				// if pak version < RELATIVE_CHUNK_OFFSETS: break
			}
		}

		std::vector<FTexturePlatformData> PlatformDatas;
	};
}
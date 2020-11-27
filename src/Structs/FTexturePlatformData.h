#pragma once

#include "../Enums/EPixelFormat.h"
#include "../Streams/BaseStream.h"
#include "FTexture2DMipMap.h"
#include "FVirtualTextureBuiltData.h"

namespace Zen::Structs {
	using namespace Enums;

	class FTexturePlatformData {
	public:
		int SizeX;
		int SizeY;
		int NumSlices;
		EPixelFormat PixelFormat;
		std::vector<FTexture2DMipMap> Mips;
		std::optional<FVirtualTextureBuiltData> VTData;

		// This doesn't provide a >> operator since it needs a bulk stream
		FTexturePlatformData(Streams::BaseStream& InputStream, Streams::BaseStream* BulkStream, size_t BulkOffset) {
			InputStream >> SizeX;
			InputStream >> SizeY;
			InputStream >> NumSlices;
			std::string PixFormat;
			InputStream >> PixFormat;
			PixelFormat = GetPixelFormat(PixFormat);

			int FirstMipToSerialize;
			InputStream >> FirstMipToSerialize;
			// Unused, but set to 0 later anyway for some reason

			{
				int SerializeNum;
				InputStream >> SerializeNum;
				Mips.reserve(SerializeNum);
				for (int i = 0; i < SerializeNum; ++i) {
					Mips.emplace_back(InputStream, BulkStream, BulkOffset);
				}
			}

			// if pak version > FNAME_BASED_COMPRESSION_METHOD (for fortnite anyway)
			// https://github.com/iAmAsval/FModel/blob/e256486b98551c467fa835c0518a00e9f9d97462/FModel/PakReader/Parsers/Objects/FTexturePlatformData.cs#L30
			int bIsVirtual;
			InputStream >> bIsVirtual;
			if (bIsVirtual) {
				VTData.emplace(InputStream, BulkStream, BulkOffset);
			}
		}
	};
}
#pragma once

#include "../Enums/EVirtualTextureCodec.h"
#include "../Streams/BaseStream.h"
#include "FByteBulkData.h"

namespace Zen::Structs {
	using namespace Enums;

	class FVirtualTextureDataChunk {
	public:
		uint32_t SizeInBytes;
		uint32_t CodecPayloadSize;
		std::vector<EVirtualTextureCodec> CodecType;
		std::vector<uint16_t> CodecPayloadOffset;
		FByteBulkData BulkData;

		// This doesn't provide a >> operator since it needs a bulk stream and NumLayers from FVirtualTextureBuiltData
		FVirtualTextureDataChunk(Streams::BaseStream& InputStream, Streams::BaseStream* BulkStream, size_t BulkOffset, uint32_t NumLayers) {
			InputStream >> SizeInBytes;
			InputStream >> CodecPayloadSize;

			CodecType.reserve(NumLayers);
			CodecPayloadOffset.reserve(NumLayers);
			for (int i = 0; i < NumLayers; ++i) {
				uint8_t CodecTypeAsByte;
				InputStream >> CodecTypeAsByte;
				CodecType.emplace_back((EVirtualTextureCodec)CodecTypeAsByte);
				InputStream >> CodecPayloadOffset.emplace_back();
			}

			BulkData = FByteBulkData(InputStream, BulkStream, BulkOffset);
		}
	};
}
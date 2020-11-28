#pragma once

#include "../Exceptions/BaseException.h"
#include "oodle2.h"

namespace Zen::Helpers {
	static std::unique_ptr<char[]> Compress(const char* Src, size_t SrcSize, OodleLZ_Compressor CompMethod, OodleLZ_CompressionLevel CompLevel, SINTa& DstSize) {
		auto CompMaxSize = OodleLZ_GetCompressedBufferSizeNeeded(CompMethod, SrcSize);
		if (CompMaxSize == OODLELZ_FAILED) {
			throw Exceptions::CompressionException("Oodle compression method is invalid");
		}
		auto Dst = std::make_unique<char[]>(CompMaxSize);
		DstSize = OodleLZ_Compress(CompMethod, Src, SrcSize, Dst.get(), CompLevel, NULL, NULL, NULL, NULL, 0);
		if (DstSize == OODLELZ_FAILED) {
			throw Exceptions::CompressionException("Oodle failed to compress data");
		}
		return Dst;
	}
}
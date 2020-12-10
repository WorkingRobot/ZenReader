#pragma once

#include "../Exceptions/BaseException.h"
#include "oodle2.h"

#ifdef USE_BROTLI
#include <brotli/encode.h>
#endif

namespace Zen::Helpers {
	static std::unique_ptr<char[]> CompressOodle(const char* Src, size_t SrcSize, OodleLZ_Compressor CompMethod, OodleLZ_CompressionLevel CompLevel, SINTa& DstSize) {
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

#ifdef USE_BROTLI

	static std::unique_ptr<char[]> CompressBrotli(const char* Src, size_t SrcSize, BrotliEncoderMode CompMode, int CompLevel, size_t& DstSize) {
		DstSize = BrotliEncoderMaxCompressedSize(SrcSize);
		if (DstSize == 0) {
			throw Exceptions::CompressionException("Brotli compressed size is too large");
		}

		auto Dst = std::make_unique<char[]>(DstSize);
		auto Result = BrotliEncoderCompress(CompLevel, BROTLI_MAX_WINDOW_BITS, CompMode, SrcSize, (uint8_t*)Src, &DstSize, (uint8_t*)Dst.get());
		if (Result == BROTLI_FALSE || DstSize == 0) {
			throw Exceptions::CompressionException("Brotli failed to compress data");
		}
		
		return Dst;
	}

#endif
}
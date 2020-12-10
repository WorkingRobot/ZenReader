#pragma once

#include "../Exceptions/BaseException.h"
#include "Hash.h"
#include "oodle2.h"

#ifdef USE_BROTLI
#include <brotli/decode.h>
#endif

#include <string>

namespace Zen::Helpers {
	static void DecompressOodle(char* Dst, size_t DstSize, const char* Src, size_t SrcSize) {
		// Good suggestion if necessary:
		// https://github.com/WorkingRobot/EGL2/blob/6f750679da8b71a82f5d271ca1d98f8957488668/storage/compression.cpp#L229
		auto DecompSize = OodleLZ_Decompress(Src, SrcSize, Dst, DstSize, OodleLZ_FuzzSafe_No, OodleLZ_CheckCRC_No, OodleLZ_Verbosity_None, NULL, 0, NULL, NULL, NULL, 0, OodleLZ_Decode_Unthreaded);
		if (DecompSize == OODLELZ_FAILED) {
			throw Exceptions::DecompressionException("Oodle failed to decompress data");
		}
	}

#ifdef USE_BROTLI

	static void DecompressBrotli(char* Dst, size_t DstSize, const char* Src, size_t SrcSize) {
		auto Result = BrotliDecoderDecompress(SrcSize, (uint8_t*)Src, &DstSize, (uint8_t*)Dst);
		if (Result != BROTLI_DECODER_RESULT_SUCCESS) {
			throw Exceptions::DecompressionException("Brotli failed to decompress data");
		}
	}

#endif

	static void Decompress(char* Dst, size_t DstSize, const char* Src, size_t SrcSize, const std::string& Method) {
		switch (Hash::Crc32(Method.c_str(), Method.size()))
		{
		//case Hash::Crc32("Zlib"):
		//	break;
		//case Hash::Crc32("Gzip"):
		//	break;
		case Hash::Crc32("Oodle"):
			DecompressOodle(Dst, DstSize, Src, SrcSize);
			break;
		default:
			throw Exceptions::DecompressionException("Unknown decompression method: %s", Method.c_str());
		}
	}
}
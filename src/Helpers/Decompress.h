#pragma once

#include "Hash.h"
#include "oodle2.h"

#include <string>

namespace Zen::Helpers {
	static void Decompress(char* Dst, size_t DstSize, const char* Src, size_t SrcSize, const std::string& Method) {
		switch (Hash::Crc32(Method.c_str(), Method.size()))
		{
		case Hash::Crc32("Zlib"):
			break;
		case Hash::Crc32("Gzip"):
			break;
		case Hash::Crc32("Oodle"):
		{
			// Good suggestion if necessary:
			// https://github.com/WorkingRobot/EGL2/blob/6f750679da8b71a82f5d271ca1d98f8957488668/storage/compression.cpp#L229
			// Returns bytes decompressed or -1 if error
			OodleLZ_Decompress(Src, SrcSize, Dst, DstSize, OodleLZ_FuzzSafe_No, OodleLZ_CheckCRC_No, OodleLZ_Verbosity_None, NULL, 0, NULL, NULL, NULL, 0, OodleLZ_Decode_Unthreaded);
			break;
		}
		default:
			break;
		}
	}
}
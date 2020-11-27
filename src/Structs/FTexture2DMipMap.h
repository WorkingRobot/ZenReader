#pragma once

#include "../Streams/BaseStream.h"
#include "FByteBulkData.h"

namespace Zen::Structs {
	class FTexture2DMipMap {
	public:
		FByteBulkData BulkData;
		int SizeX;
		int SizeY;
		int SizeZ;

		// This doesn't provide a >> operator since it needs a bulk stream
		FTexture2DMipMap(Streams::BaseStream& InputStream, Streams::BaseStream* BulkStream, size_t BulkOffset) {
			int bCooked;
			InputStream >> bCooked;
			BulkData = FByteBulkData(InputStream, BulkStream, BulkOffset);
			InputStream >> SizeX;
			InputStream >> SizeY;
			InputStream >> SizeZ;
		}
	};
}
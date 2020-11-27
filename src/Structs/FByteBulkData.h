#pragma once

#include "../Exceptions/BaseException.h"
#include "../Enums/EBulkDataFlags.h"
#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	using namespace Enums;
	using namespace Exceptions;

	class FByteBulkData {
	public:
		std::unique_ptr<char[]> Data;
		int DataSize;

		FByteBulkData() = default;

		// This doesn't provide a >> operator since it needs a bulk stream
		FByteBulkData(Streams::BaseStream& InputStream, Streams::BaseStream* BulkStream, size_t BulkOffset) {
			int BulkDataFlags;
			InputStream >> BulkDataFlags;

			InputStream >> DataSize; // called ElementCount, but we're saving this value publicly
			int BulkDataOnDisk; // unused
			InputStream >> BulkDataOnDisk;

			int64_t BulkDataOffsetInFile;
			InputStream >> BulkDataOffsetInFile;

			if (!(BulkDataFlags & (int)EBulkDataFlags::BULKDATA_NoOffsetFixUp)) {
				BulkDataOffsetInFile += BulkOffset;
			}

			if (BulkDataFlags & (int)EBulkDataFlags::BULKDATA_Unused || BulkDataFlags & (int)EBulkDataFlags::BULKDATA_OptionalPayload /* uptnl */ || !DataSize) {
				return;
			}

			Data = std::make_unique<char[]>(DataSize);

			if (BulkDataFlags & (int)EBulkDataFlags::BULKDATA_PayloadAtEndOfFile && BulkDataOffsetInFile + DataSize <= InputStream.size()) { // ".uasset" (not sure how this would make sense, especially on non-unversioned readers)
				auto Pos = InputStream.tell();
				InputStream.seek(BulkDataOffsetInFile, Streams::BaseStream::Beg);
				InputStream.read(Data.get(), DataSize);
				InputStream.seek(Pos, Streams::BaseStream::Beg);
			}
			else if (BulkDataFlags & (int)EBulkDataFlags::BULKDATA_ForceInlinePayload) {
				InputStream.read(Data.get(), DataSize);
			}
			else if (BulkDataFlags & (int)EBulkDataFlags::BULKDATA_PayloadInSeperateFile && BulkStream) {
				BulkStream->seek(BulkDataOffsetInFile, Streams::BaseStream::Beg);
				BulkStream->read(Data.get(), DataSize);
			}
			else {
				Data.reset();
				throw ArchiveCorruptedException("Could not read FByteBulkData");
			}
		}
	};
}
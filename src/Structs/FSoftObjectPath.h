#pragma once

#include "../Streams/BaseStream.h"
#include "FName.h"

namespace Zen::Structs {
	class FSoftObjectPath {
	public:
		FName AssetPathName;
		std::string SubPathString;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FSoftObjectPath& Value) {
			InputStream >> Value.AssetPathName;

			// SubPathString is only set if EPakVersion is at least RELATIVE_CHUNK_OFFSETS
			InputStream >> Value.SubPathString;

			return InputStream;
		}
	};
}
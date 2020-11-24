#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FPackageIndex {
	public:
		int32_t Index;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FPackageIndex& Value) {
			InputStream >> Value.Index;

			return InputStream;
		}

		bool IsNull() const {
			return Index == 0;
		}

		bool IsImport() const {
			return Index < 0;
		}

		bool IsExport() const {
			return Index > 0;
		}

		int32_t ToImport() const {
			return -Index - 1;
		}

		int32_t ToExport() const {
			return Index - 1;
		}
	};
}
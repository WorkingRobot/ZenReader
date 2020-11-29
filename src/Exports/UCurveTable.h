#pragma once

#include "../Enums/ECurveTableMode.h"
#include "../Structs/FName.h"
#include "UObject.h"

namespace Zen::Exports {
	class UCurveTable : public UExport {
	public:
		UCurveTable(Streams::BaseStream& InputStream, const Providers::Schema& Schema) {
			InputStream.seek(6, Streams::BaseStream::Cur);
			// u16: 256, u32: 0

			int NumRows;
			InputStream >> NumRows;

			uint8_t CurveMode;
			InputStream >> CurveMode;
			CurveTableMode = (ECurveTableMode)CurveMode;

			RowMap.reserve(NumRows);
			for (int i = 0; i < NumRows; ++i) {
				FName Key;
				InputStream >> Key;
				RowMap.emplace_back(std::move(Key), UObject(InputStream, Schema, StructFallback));
			}
		}

		ECurveTableMode CurveTableMode;
		std::vector<std::pair<FName, UObject>> RowMap;
	};
}
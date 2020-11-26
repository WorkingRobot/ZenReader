#pragma once

#include "../Exceptions/BaseException.h"
#include "../Enums/ECurveTableMode.h"
#include "../Structs/FName.h"
#include "UObject.h"
#include "../ZExport.h"

namespace Zen::Exports {
	using namespace Exceptions;

	class UDataTable : public UObject {
	public:
		UDataTable(Streams::BaseStream& InputStream, const Providers::BaseSchema& Schema) : UObject(InputStream, Schema) {
			auto ZExp = (const ZExport*)InputStream.GetProperty<Streams::PropId::ZExport>();
			auto GlobalData = (const ZGlobalData*)InputStream.GetProperty<Streams::PropId::GlobalData>();
			auto Provider = (const Providers::BaseProvider*)InputStream.GetProperty<Streams::PropId::Provider>();
			if (!ZExp || !GlobalData || !Provider) {
				throw StreamPropertyNotFoundException("UDataTable must be deserialized from ZExport");
			}

			auto RowStructProp = TryGet<Properties::ObjectProperty>("RowStruct");
			if (!RowStructProp) {
				throw ArchiveCorruptedException("A RowStruct property is required for the DataTable");
			}
			auto& RowStructImport = ZExp->GetImport(RowStructProp->Value);
			if (!RowStructImport.IsScriptImport()) {
				throw ArchiveCorruptedException("The RowStruct property must be a ScriptImport");
			}
			auto& RowStructType = GlobalData->GetEntryName(RowStructImport);
			auto RowSchema = Provider->GetSchema(RowStructType);
			if (!RowSchema) {
				throw SchemaNotFoundException("The schema for the row type \"%s\" was not found", RowStructType.c_str());
			}

			int NumRows;
			InputStream >> NumRows;

			RowMap.reserve(NumRows);
			for (int i = 0; i < NumRows; ++i) {
				FName Key;
				InputStream >> Key;
				RowMap.emplace_back(std::move(Key), UObject(InputStream, *RowSchema, StructFallback));
			}
		}

		ECurveTableMode CurveTableMode;
		std::vector<std::pair<FName, UObject>> RowMap;
	};
}
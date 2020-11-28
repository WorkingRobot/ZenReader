#pragma once

#include "../Exceptions/BaseException.h"
#include "../Structs/FName.h"
#include "../ZExport.h"
#include "Base.h"

namespace Zen::Properties {
	using namespace Exceptions;

	class ByteProperty : public BaseProperty {
	public:
		uint8_t Value;

		ByteProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData) {
			if (PropData.GetByteEnumName()) {
				Structs::FName Name;
				InputStream >> Name;
				
				auto ZExp = (const ZExport*)InputStream.GetProperty<Streams::PropId::ZExport>();
				if (!ZExp) {
					throw StreamPropertyNotFoundException("ByteProperty must be deserialized from ZExport");
				}
				Value = atoi(Name.Get(ZExp->GetNameMap()).c_str());
			}
			else {
				// if the FPropertyTag has an EnumName, this is an FName instead
				InputStream >> Value;
			}
		}

		ByteProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadArray) {
			InputStream >> Value;
		}

		ByteProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadMap) {
			uint32_t Val;
			InputStream >> Val;
			Value = Val;
		}

		ByteProperty(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EReadZero) : Value(0) {}

		EPropertyType GetType() const override {
			return EPropertyType::ByteProperty;
		}
	};
}
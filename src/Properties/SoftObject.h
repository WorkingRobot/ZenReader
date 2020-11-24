#pragma once

#include "../Structs/FSoftObjectPath.h"
#include "Base.h"

namespace Zen::Properties {
	class SoftObjectProperty : public BaseProperty {
	public:
		Structs::FSoftObjectPath Value;

		SoftObjectProperty(Streams::BaseStream& InputStream) {
			InputStream >> Value;
		}

		SoftObjectProperty(Streams::BaseStream& InputStream, EReadArray) : SoftObjectProperty(InputStream) {}

		// Always read exactly 16 bytes
		SoftObjectProperty(Streams::BaseStream& InputStream, EReadMap) {
			auto StartPos = InputStream.tell();
			InputStream >> Value;
			InputStream.seek(StartPos + 16, Streams::BaseStream::Beg);
		}

		SoftObjectProperty(Streams::BaseStream& InputStream, EReadZero) : Value() {}

		EPropertyType GetType() const override {
			return EPropertyType::SoftObjectProperty;
		}
	};
}
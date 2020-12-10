#include "Struct.h"

#include "../Exports/UObject.h"

namespace Zen::Properties {
	void StructProperty::EmplaceUStructFallback(Streams::BaseStream& InputStream, const Providers::Struct& Struct) {
		this->Value.emplace<std::shared_ptr<Exports::UStructFallback>>(std::make_shared<Exports::UStructFallback>(InputStream, Struct));
	}
}
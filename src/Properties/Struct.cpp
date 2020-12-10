#include "Struct.h"

#include "../Exports/UObject.h"

namespace Zen::Properties {
	void StructProperty::EmplaceUObject(Streams::BaseStream& InputStream, const Providers::Schema& Schema) {
		this->Value.emplace<std::shared_ptr<Exports::UObject>>(std::make_shared<Exports::UObject>(InputStream, Schema, Exports::StructFallback));
	}
}
#pragma once

#include "../Enums/EPropertyType.h"
#include "../Providers/Base.h"
#include "../Streams/BaseStream.h"
#include "Base.h"

#include <memory>

namespace Zen::Properties {
	using namespace Enums;

	template<EReadType ReadType>
	std::unique_ptr<BaseProperty> Serialize(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EPropertyType Type);
}
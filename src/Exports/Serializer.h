#pragma once

#include "../Streams/BaseStream.h"

#include <any>
#include <string>

namespace Zen::Exports {
	std::any Serialize(Streams::BaseStream& InputStream, const std::string& ExportType);
}
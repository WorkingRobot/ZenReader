#pragma once

#include <memory>
#include <string>

namespace Zen::Helpers {
	template<typename... Args>
	static std::string Format(const char* Format, Args&&... Data) {
		auto size = snprintf(nullptr, 0, Format, Data...) + 1;
		if (size <= 0) {
			return "";
		}
		auto buf = std::make_unique<char[]>(size);
		snprintf(buf.get(), size, Format, Data...);
		return std::string(buf.get(), buf.get() + size - 1);
	}
}
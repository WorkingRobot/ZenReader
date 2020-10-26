#pragma once

#include "../Streams/BaseStream.h"

#include <type_traits>

namespace Zen::Structs {
	class FIoContainerId {
	public:
		uint64_t Id;

		constexpr FIoContainerId() : FIoContainerId(-1) {}
		constexpr FIoContainerId(uint64_t Id) : Id(Id) {}

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIoContainerId& Value) {
			InputStream >> Value.Id;
			return InputStream;
		}

		static constexpr FIoContainerId InvalidId() {
			return {};
		}
	};
}
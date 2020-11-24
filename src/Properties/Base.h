#pragma once

#include "../Enums/EPropertyType.h"
#include "../Providers/Base.h"
#include "../Streams/BaseStream.h"

namespace Zen::Properties {
	using namespace Enums;

	enum class EReadType : uint8_t {
		NORMAL,
		ARRAY,
		MAP,
		ZERO
	};

	namespace {
		struct EReadArray {
			explicit EReadArray() = default;
		};
		struct EReadMap {
			explicit EReadMap() = default;
		};
		struct EReadZero {
			explicit EReadZero() = default;
		};

		inline constexpr EReadArray ReadArray{};
		inline constexpr EReadMap ReadMap{};
		inline constexpr EReadZero ReadZero{};
	}

	class BaseProperty {
	public:
		virtual EPropertyType GetType() const = 0;

		template<EReadType ReadType>
		static std::unique_ptr<Properties::BaseProperty> Serialize(Streams::BaseStream& InputStream, const Providers::BasePropertyData& PropData, EPropertyType Type);
	};
}
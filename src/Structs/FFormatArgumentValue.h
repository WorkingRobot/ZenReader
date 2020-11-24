#pragma once

#include "../Enums/EFormatArgumentType.h"
#include "../Streams/BaseStream.h"
#include "FText.h"

#include <variant>

namespace Zen::Structs {
    using namespace Enums;
	class FFormatArgumentValue {
	public:
		std::variant<double, float, int64_t, uint64_t, FText> Value;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FFormatArgumentValue& Value) {
            int8_t TypeAsInt8;
            InputStream >> TypeAsInt8;

            // https://github.com/EpicGames/UnrealEngine/blob/master/Engine/Source/Runtime/Core/Private/Internationalization/Text.cpp#L1210
            // EFormatArgumentType has a gender type, but it's not serializable? Yet, it has a GetGenderValue function??
            switch ((EFormatArgumentType)TypeAsInt8)
            {
            case EFormatArgumentType::Int:
                InputStream >> Value.Value.emplace<int64_t>();
                break;
            case EFormatArgumentType::UInt:
                InputStream >> Value.Value.emplace<uint64_t>();
                break;
            case EFormatArgumentType::Float:
                InputStream >> Value.Value.emplace<float>();
                break;
            case EFormatArgumentType::Double:
                InputStream >> Value.Value.emplace<double>();
                break;
            case EFormatArgumentType::Text:
                InputStream >> Value.Value.emplace<FText>();
                break;
            }

            return InputStream;
		}
	};
}
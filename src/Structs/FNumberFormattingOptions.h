#pragma once

#include "../Enums/ERoundingMode.h"
#include "../Streams/BaseStream.h"

namespace Zen::Structs {
    using namespace Enums;
	class FNumberFormattingOptions {
	public:
        bool AlwaysSign;
        bool UseGrouping;
        ERoundingMode RoundingMode;
        int MinimumIntegralDigits;
        int MaximumIntegralDigits;
        int MinimumFractionalDigits;
        int MaximumFractionalDigits;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FNumberFormattingOptions& Value) {
            InputStream >> Value.AlwaysSign;
            InputStream >> Value.UseGrouping;

            int8_t RoundingModeInt8;
            InputStream >> RoundingModeInt8;
            Value.RoundingMode = (ERoundingMode)RoundingModeInt8;

            InputStream >> Value.MinimumIntegralDigits;
            InputStream >> Value.MaximumIntegralDigits;
            InputStream >> Value.MinimumFractionalDigits;
            InputStream >> Value.MaximumFractionalDigits;

            return InputStream;
		}
	};
}
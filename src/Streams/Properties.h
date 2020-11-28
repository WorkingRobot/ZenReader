#pragma once

#include <stack>

namespace Zen::Streams {
    enum class PropId : uint8_t {
        Provider,
        ZExport,
        GlobalData,

        SmartProviderNameLUT,
        SmartProviderPropDataType,

        Count
    };
}
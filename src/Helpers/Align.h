#pragma once

namespace Zen::Helpers {
	template<class N>
	static constexpr N Align(N Value, N Alignment) {
		return Value + (-Value & (Alignment - 1));
	}

	template<int Alignment, class N>
	static constexpr N Align(N Value) {
		return Value + (-Value & (Alignment - 1));
	}
}
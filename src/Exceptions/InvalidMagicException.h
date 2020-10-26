#pragma once

#include "BaseException.h"

namespace Zen::Exceptions {
	class InvalidMagicException : BaseException {
	public:
		InvalidMagicException(const char* Description) : BaseException(Description) {}
	};
}
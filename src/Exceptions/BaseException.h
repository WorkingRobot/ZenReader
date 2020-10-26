#pragma once

#include <exception>

namespace Zen::Exceptions {
	class BaseException : std::exception {
	public:
		BaseException(const char* Description) : Description(Description) {}

		const char* what() const override
		{
			return Description;
		}

	private:
		const char* Description;
	};
}
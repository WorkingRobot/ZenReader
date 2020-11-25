#pragma once

#include "../Helpers/Format.h"

#include <exception>
#include <string>

namespace Zen::Exceptions {
	class BaseException : std::exception {
	public:
		template<typename... Args>
		BaseException(const char* Format, Args&&... Data) : Description(Helpers::Format(Format, std::forward<Args>(Data)...)) {}

		BaseException(std::string&& Description) : Description(std::forward<std::string>(Description)) {}

		const char* what() const override
		{
			return Description.c_str();
		}

	private:
		std::string Description;
	};

#define DEFINE_EXCEPTION(ExceptionName) \
	class ExceptionName : BaseException { \
	public: \
		template<typename... Args> \
		ExceptionName(const char* Format, Args&&... Data) : BaseException(Format, std::forward<Args>(Data)...) {} \
		ExceptionName(std::string&& Description) : BaseException(std::forward<std::string>(Description)) {} \
	}

	DEFINE_EXCEPTION(InvalidMagicException);
	DEFINE_EXCEPTION(InvalidChunkIdException);
	DEFINE_EXCEPTION(SchemaNotFoundException);
	DEFINE_EXCEPTION(PropertyTypeNotFoundException);
	DEFINE_EXCEPTION(ScriptObjectNotFoundException);
	DEFINE_EXCEPTION(UnsupportedOperationException);
	DEFINE_EXCEPTION(DecompressionException);
	DEFINE_EXCEPTION(KeyRequiredException);
	DEFINE_EXCEPTION(ArchiveCorruptedException);

#undef DEFINE_EXCEPTION
}
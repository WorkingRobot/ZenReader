#include "ZGame.h"
#include "Helpers/oodle2.h"

#include <cstdarg>

#ifdef TRACY_ENABLE
#include <Tracy.hpp>
void* operator new(std::size_t count)
{
	auto ptr = malloc(count);
	TracyAlloc(ptr, count);
	return ptr;
}
void operator delete(void* ptr) noexcept
{
	TracyFree(ptr);
	free(ptr);
}
#endif

using namespace Zen;

int main() {
	if constexpr (false) {
		OodleCore_Plugins_SetPrintf([](bool debug, const char* filename, uint32_t line_num, const char* format, ...) {
			va_list args;
			va_start(args, format);
			vprintf(format, args);
			va_end(args);
		});
		Oodle_LogHeader();
	}
	else {
		OodleCore_Plugins_SetPrintf(NULL);
	}

	ZGame Game("J:/Programs/FN/cache/fn/FortniteGame/Content/Paks");
	return 0;
}
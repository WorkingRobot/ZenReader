#include "ZGame.h"

#include "Helpers/oodle2.h"
#include <cstdarg>

using namespace Zen;

int main() {
	OodleCore_Plugins_SetPrintf([](bool debug, const char* filename, uint32_t line_num, const char* format, ...) {
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
	});

	Oodle_LogHeader();

	ZGame Game("J:/Programs/FN/cache/fn/FortniteGame/Content/Paks");
	return 0;
}
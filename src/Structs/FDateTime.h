#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FDateTime {
	public:
        // Holds the ticks in 100 nanoseconds resolution since January 1, 0001 A.D.
        int64_t Ticks;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FDateTime& Value) {
			InputStream >> Value.Ticks;

            return InputStream;
		}
	};
}
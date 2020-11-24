#pragma once

#include <chrono>

namespace Zen::Helpers {
	class Stopwatch {
	public:
		Stopwatch(const char* Name) :
			Name(Name),
			Ended(false),
			StartTime(std::chrono::steady_clock::now())
		{}

		void End() {
			if (Ended) return;
			auto EndTime = std::chrono::steady_clock::now();
			printf("%s: %.2f ms\n", Name, (EndTime - StartTime).count() / 1000000.f);
			Ended = true;
		}

		~Stopwatch() {
			End();
		}

	private:
		const char* Name;
		bool Ended;
		std::chrono::steady_clock::time_point StartTime;
	};
}
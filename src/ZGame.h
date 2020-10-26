#pragma once

#include "Readers/UTocFile.h"

namespace Zen {
	class ZGame {
	public:
		ZGame(const std::filesystem::path& DataFolder)
		{
			Readers::UTocFile(DataFolder / "global.utoc");
		}

		~ZGame()
		{

		}

	private:

	};
}
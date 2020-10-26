#pragma once

#include "Readers/UTocFile.h"

namespace Zen {
	class ZGame {
	public:
		ZGame(const std::filesystem::path& DataFolder)
		{
			Readers::UTocFile(DataFolder / "pakchunk0-WindowsClient.utoc");
		}

		~ZGame()
		{

		}

	private:

	};
}
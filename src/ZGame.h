#pragma once

#include "ZContainer.h"
#include "Streams/FileStream.h"
#include "Structs/FIoStoreTocResource.h"

namespace Zen {
	class ZGame {
	public:
		ZGame(const std::filesystem::path& DataFolder)
		{
			Streams::FileStream Toc(DataFolder / "pakchunk0-WindowsClient.utoc", "rb");
			Streams::FileStream Cas(DataFolder / "pakchunk0-WindowsClient.ucas", "rb");
			ZContainer(Toc, Cas, [](const Structs::FGuid& guid) {return Structs::FAESKey(); });
		}

		~ZGame()
		{

		}

	private:

	};
}
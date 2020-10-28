#pragma once

#include "ZFileTree.h"
#include "ZContainer.h"
#include "Streams/BufferedStream.h"
#include "Streams/FileStream.h"

#include <chrono>

namespace Zen {
	class ZGame {
	public:
		ZGame(const std::filesystem::path& DataFolder)
		{
			//Streams::FileStream Toc(DataFolder / "pakchunk0-WindowsClient.utoc", "rb");
			Streams::BufferedStream Cas(Streams::FileStream(DataFolder / "pakchunk0-WindowsClient.ucas", "rb"));
			auto StartTime = std::chrono::steady_clock::now();
			for (auto& file : std::filesystem::directory_iterator(DataFolder)) {
				if (file.path().string().ends_with(".utoc")) {
					printf("reading %s\n", file.path().string().c_str());
					ZContainer Container(Streams::BufferedStream(Streams::FileStream(file.path(), "rb")), Cas, [](const Structs::FGuid& guid, Structs::FAESKey& Key) { return false; }, Tree);
				}
			}
			auto ms = (std::chrono::steady_clock::now() - StartTime).count() / 1000000.f;
			printf("%.2f\n", ms);
			//ZContainer Container(Toc, Cas, [](const Structs::FGuid& guid) { return Structs::FAESKey(); }, Tree);
		}

		~ZGame()
		{

		}

	private:
		ZFileTree<> Tree;
	};
}
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
			auto StartTime = std::chrono::steady_clock::now();
			for (auto& file : std::filesystem::directory_iterator(DataFolder)) {
				if (file.path().string().ends_with(".utoc")) {
					printf("reading %s\n", file.path().string().c_str());
					auto UcasPath = file.path();
					Containers.emplace_back(
						Streams::BufferedStream(Streams::FileStream(file.path())),
						Streams::FileStream(UcasPath.replace_extension(".ucas")),
						[](const Structs::FGuid& Guid, Structs::FAESKey& Key) {
							// Nope
							return false;
						},
						Tree
						);
				}
			}
			auto ms = (std::chrono::steady_clock::now() - StartTime).count() / 1000000.f;
			printf("%.2f ms\n", ms);
			//ZContainer Container(Toc, Cas, [](const Structs::FGuid& guid) { return Structs::FAESKey(); }, Tree);
			auto Package = Tree.TryGetPackage("FortniteGame/Content/Athena/Apollo/Maps/UI/Apollo_Terrain_Minimap");
			auto File = Package->GetFile("uasset");
			uint64_t DataSize;
			auto Data = File->ReadFile(DataSize);
			auto outFile = fopen("map.uasset", "wb");
			fwrite(Data.get(), 1, DataSize, outFile);
			fclose(outFile);
		}

		~ZGame()
		{

		}

	private:
		// Moves screw up the reference ZFile's BaseContainer, deque is required
		std::deque<ZContainer<Streams::BufferedStream<Streams::FileStream>, Streams::FileStream>> Containers;

		ZFileTree<> Tree;
	};
}
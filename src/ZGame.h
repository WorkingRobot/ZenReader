#pragma once

#include "ZFileTree.h"
#include "ZContainer.h"
#include "Providers/Smart.h"
#include "Providers/FModel.h"
#include "Providers/JWP.h"
#include "Providers/Exporter.h"
#include "Streams/BufferedStream.h"
#include "Streams/FileStream.h"
#include "Helpers/Stopwatch.h"
#include "Exports/UTexture2D.h"
#include "ZGlobalData.h"

#include <chrono>
#include <filesystem>

namespace Zen {
	class ZGame {
	public:
		ZGame(const std::filesystem::path& DataFolder)
		{
			Helpers::Stopwatch s0("Load provider");
			Providers::SmartProvider Provider("J:/Code/Visual Studio 2017/Projects/ZenReader/mappings/smart.usmap");
			Providers::Export<true>("J:/Code/Visual Studio 2017/Projects/ZenReader/mappings/smarter.usmap", Provider);
			std::terminate();
			//Providers::FModelProvider Provider("J:/Code/Visual Studio 2017/Projects/ZenReader/mappings/fmodel/TypeMappings.json", "J:/Code/Visual Studio 2017/Projects/ZenReader/mappings/fmodel/EnumMappings.json");
			//Providers::JWPProvider Provider("J:/Code/Visual Studio 2017/Projects/ZenReader/mappings/jwp");
			s0.End();

			Helpers::Stopwatch s1("Load utocs");
			for (auto& file : std::filesystem::directory_iterator(DataFolder)) {
				if (file.path().string().ends_with(".utoc")) {
					printf("reading %s\n", file.path().string().c_str());
					auto UcasPath = file.path();
					auto& Container = Containers.emplace_back(
						Streams::BufferedStream(Streams::FileStream(file.path())),
						Streams::FileStream(UcasPath.replace_extension(".ucas")),
						[](const Structs::FGuid& Guid, Structs::FAESKey& Key) {
							// Nope
							return false;
						},
						Tree
					);
					if (file.path().stem() == "global") {
						Helpers::Stopwatch s2("Load global data");
						GlobalData = ZGlobalData(Container);
						s2.End();
					}
				}
			}
			s1.End();

			Helpers::Stopwatch s3("Get package");
			// TODO: FortniteGame/Content/Athena/Playlists/AthenaCompositeLP CompositeDataTable
			auto Package = Tree.TryGetPackage("FortniteGame/Content/Items/Datatables/AthenaLootTierData_Client");
			if (!Package) {
				printf("doesn't exist\n");
				return;
			}
			s3.End();
			//Package->GetFile("uasset")->GetStream().Dump("athenadatatable.ucasset");

			Helpers::Stopwatch s4("Get exports");
			auto Export = Package->GetExport(GlobalData, Provider);
			//auto Texture = Export.Get<Exports::UTexture2D>();
			s4.End();
			getchar();
			//printf("%p\n", Texture);
		}

		~ZGame()
		{

		}

	private:
		// Moves screw up ZFile's BaseContainer reference, deque is required
		std::deque<ZContainer<Streams::BufferedStream<Streams::FileStream>, Streams::FileStream>> Containers;

		ZGlobalData GlobalData;

		ZFileTree<> Tree;
	};
}
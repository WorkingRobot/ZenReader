#pragma once

#include "ZFileTree.h"
#include "ZContainer.h"
#include "Providers/FModel.h"
#include "Streams/BufferedStream.h"
#include "Streams/FileStream.h"
#include "Helpers/Stopwatch.h"
#include "Exports/UCurveTable.h"
#include "ZGlobalData.h"

#include <chrono>
#include <filesystem>

namespace Zen {
	class ZGame {
	public:
		ZGame(const std::filesystem::path& DataFolder)
		{
			Helpers::Stopwatch s0("Load utocs");
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
						Helpers::Stopwatch s1("Load global data");
						GlobalData = ZGlobalData(Container);
						s1.End();
					}
				}
			}
			s0.End();

			Helpers::Stopwatch s2("Get package");
			auto Package = Tree.TryGetPackage("FortniteGame/Content/Athena/Balance/DataTables/AthenaGameData");
			s2.End();
			//Package->GetFile("uasset")->GetStream().Dump("athenadatatable.ucasset");

			Helpers::Stopwatch s3("Load provider");
			Providers::FModelProvider Provider;
			s3.End();

			Helpers::Stopwatch s4("Get exports");
			auto Export = Package->GetExport(GlobalData, Provider);
			auto Map = Export.Get<Exports::UCurveTable>();
			s4.End();
			getchar();
			for (auto& Entry : Map->RowMap) {
				Entry.first.Get(Export.GetNameMap()).c_str();
			}
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
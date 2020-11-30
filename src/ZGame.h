#pragma once

#include "Providers/Base.h"
#include "Streams/BufferedStream.h"
#include "Streams/FileStream.h"
#include "ZFileTree.h"
#include "ZContainer.h"
#include "ZGlobalData.h"

#include <filesystem>

namespace Zen {
	class ZGame {
	public:
		// KeyDelegateCall is a callable
		template<typename KeyDelegateCall>
		ZGame(const std::filesystem::path& DataFolder, const KeyDelegateCall& KeyDelegate)
		{
			for (auto& file : std::filesystem::directory_iterator(DataFolder)) {
				if (file.path().string().ends_with(".utoc")) {
					auto UcasPath = file.path();
					auto& Container = Containers.emplace_back(
						Streams::BufferedStream(Streams::FileStream(file.path())),
						Streams::FileStream(UcasPath.replace_extension(".ucas")),
						KeyDelegate,
						Tree
					);
					if (file.path().stem() == "global") {
						GlobalData = ZGlobalData(Container);
					}
				}
			}
		}

		ZGame(const std::filesystem::path& DataFolder) : ZGame(DataFolder, [](const FGuid&, FAESKey&) { return false; }) {}

		decltype(auto) TryGetPackage(const char* Path) const {
			return Tree.TryGetPackage(Path);
		}

		decltype(auto) GetPackage(const char* Path) const {
			return *Tree.TryGetPackage(Path);
		}

		const ZGlobalData& GetGlobalData() const {
			return GlobalData;
		}

	private:
		// Moves screw up ZFile's BaseContainer reference, deque is required
		std::deque<ZContainer<Streams::BufferedStream<Streams::FileStream>, Streams::FileStream>> Containers;

		ZGlobalData GlobalData;

		ZFileTree<> Tree;
	};
}
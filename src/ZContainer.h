#pragma once

#include "Helpers/AES.h"
#include "Structs/FGuid.h"
#include "Structs/FIoStoreTocResource.h"
#include "Streams/BufferStream.h"

#include <functional>
#include <optional>

namespace Zen {
	using namespace Exceptions;
	using namespace Streams;
	using namespace Structs;

	class BaseContainer {
	public:
		BaseContainer(BaseStream& Toc, BaseStream& Cas) :
			TocStream(Toc),
			CasStream(Cas)
		{

		}

		BaseStream& TocStream;
		BaseStream& CasStream;
		FIoStoreTocResource Toc;
		std::optional<FAESKey> Key; // TODO: Check if this is necessary for saving significant memory, it might just be a big slowdown
	};

	template<class TocStreamType, class CasStreamType>
	class ZContainer : public BaseContainer {
	public:
		template<typename FileTree>
		ZContainer(TocStreamType&& _TocStream, CasStreamType&& _CasStream, std::function<bool(const FGuid&, FAESKey&)> KeyDelegate, FileTree& Tree) :
			BaseContainer(TocStreamHolder, CasStreamHolder),
			TocStreamHolder(std::move(_TocStream)),
			CasStreamHolder(std::move(_CasStream))
		{
			TocStream >> Toc;

			if (Toc.DirectoryBuffer) {
				if (Toc.Header.ContainerFlags & Enums::EIoContainerFlags::Encrypted) {
					if (!KeyDelegate(Toc.Header.EncryptionKeyGuid, Key.emplace())) {
						return;
					}
					Helpers::AES::DecodeInPlace(*Key, Toc.DirectoryBuffer.get(), Toc.Header.DirectoryIndexSize);
				}
				BufferStream DirectoryStream(Toc.DirectoryBuffer.get(), Toc.Header.DirectoryIndexSize);

				FIoDirectoryIndexResource DirectoryIndex;
				DirectoryStream >> DirectoryIndex;

				// The *this gets passed to the ZFile constructor after the UserData field
				DirectoryIndex.ReadIndex<FileTree>(Tree, *this);
			}
		}

		~ZContainer()
		{

		}

		ZContainer(ZContainer&&) = default;

	private:
		TocStreamType TocStreamHolder;
		CasStreamType CasStreamHolder;

		friend class ZFile;
	};
}
#pragma once

#include "Helpers/AES.h"
#include "Structs/FGuid.h"
#include "Structs/FIoStoreTocResource.h"
#include "Streams/BufferStream.h"

#include <functional>

namespace Zen {
	using namespace Exceptions;
	using namespace Streams;
	using namespace Structs;

	class ZContainer {
	public:
		template<class FileTree, class File = ZFile>
		ZContainer(BaseStream& TocStream, BaseStream& CasStream, std::function<bool(const FGuid&, FAESKey&)> KeyDelegate, FileTree& Tree)
		{
			FIoStoreTocResource Toc;
			TocStream >> Toc;

			if (Toc.DirectoryBuffer) {
				if (Toc.Header.ContainerFlags & Enums::EIoContainerFlags::Encrypted) {
					FAESKey Key;
					if (!KeyDelegate(Toc.Header.EncryptionKeyGuid, Key)) {
						return;
					}
					Helpers::AES::DecodeInPlace(Key, Toc.DirectoryBuffer.get(), Toc.Header.DirectoryIndexSize);
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

	private:

	};
}
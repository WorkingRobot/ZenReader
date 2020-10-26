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
		ZContainer(BaseStream& TocStream, BaseStream& CasStream, std::function<const FAESKey&(const FGuid&)> KeyDelegate)
		{
			FIoStoreTocResource Toc;
			TocStream >> Toc;

			if (Toc.DirectoryBuffer) {
				if (Toc.Header.ContainerFlags & Enums::EIoContainerFlags::Encrypted) {
					Helpers::AES::DecodeInPlace(KeyDelegate(Toc.Header.EncryptionKeyGuid), Toc.DirectoryBuffer.get(), Toc.Header.DirectoryIndexSize);
				}
				BufferStream DirectoryStream(Toc.DirectoryBuffer.get(), Toc.Header.DirectoryIndexSize);

				FIoDirectoryIndexResource DirectoryIndex;
				DirectoryStream >> DirectoryIndex;

				DirectoryIndex.ReadIndex();
			}
		}

		~ZContainer()
		{

		}

	private:

	};
}
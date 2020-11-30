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
		{}

		BaseStream& TocStream;
		BaseStream& CasStream;
		FIoStoreTocResource Toc;
		std::optional<FAESKey> Key;
	};

	template<class TocStreamType, class CasStreamType>
	class ZContainer : public BaseContainer {
	public:
		// KeyDelegate is of type bool(const FGuid&, FAESKey&)
		template<typename KeyDelegateCall, typename FileTree>
		ZContainer(TocStreamType&& _TocStream, CasStreamType&& _CasStream, const KeyDelegateCall& KeyDelegate, FileTree& Tree) :
			BaseContainer(TocStreamHolder, CasStreamHolder),
			TocStreamHolder(std::forward<TocStreamType>(_TocStream)),
			CasStreamHolder(std::forward<CasStreamType>(_CasStream))
		{
			TocStream >> Toc;

			if (Toc.DirectoryBuffer) {
				bool BufferValid = true;
				if (Toc.Header.ContainerFlags & Enums::EIoContainerFlags::Encrypted) {
					if (KeyDelegate(Toc.Header.EncryptionKeyGuid, Key.emplace())) {
						Helpers::AES::DecodeInPlace(*Key, Toc.DirectoryBuffer.get(), Toc.Header.DirectoryIndexSize);
					}
					else {
						BufferValid = false;
					}
				}

				if (BufferValid) {
					BufferStream DirectoryStream(Toc.DirectoryBuffer.get(), Toc.Header.DirectoryIndexSize);

					FIoDirectoryIndexResource DirectoryIndex;
					DirectoryStream >> DirectoryIndex;

					// The *this gets passed to the ZFile constructor after the UserData field
					DirectoryIndex.ReadIndex<FileTree>(Tree, *this);
				}
			}

			Toc.InvalidateBuffers();
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
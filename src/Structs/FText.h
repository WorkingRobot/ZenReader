#pragma once

#include "../Enums/ETextHistoryType.h"
#include "../Enums/ETextFlag.h"
#include "../Streams/BaseStream.h"
#include "FTextData.h"

namespace Zen::Structs {
	class FText {
	public:
		std::shared_ptr<FTextData> TextData;
		ETextFlag Flags;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FText& Value);

		// For debugging
		void Dump();
	};
}
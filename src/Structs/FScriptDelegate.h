#pragma once

#include "../Streams/BaseStream.h"
#include "../Structs/FName.h"

namespace Zen::Structs {
	class FScriptDelegate {
	public:
		int Object;
		FName Name;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FScriptDelegate& Value) {
			InputStream >> Value.Object;
			InputStream >> Value.Name;

			return InputStream;
		}
	};
}
#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FLinearColor {
	public:
		float R;
		float G;
		float B;
		float A;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FLinearColor& Value) {
			InputStream >> Value.R;
			InputStream >> Value.G;
			InputStream >> Value.B;
			InputStream >> Value.A;

			return InputStream;
		}
	};
}
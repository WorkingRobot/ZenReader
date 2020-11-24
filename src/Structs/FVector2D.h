#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FVector2D {
	public:
		float X;
		float Y;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FVector2D& Value) {
			InputStream >> Value.X;
			InputStream >> Value.Y;

			return InputStream;
		}
	};
}
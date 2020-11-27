#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	class FIntPoint {
	public:
		int X;
		int Y;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FIntPoint& Value) {
			InputStream >> Value.X;
			InputStream >> Value.Y;

			return InputStream;
		}
	};
}
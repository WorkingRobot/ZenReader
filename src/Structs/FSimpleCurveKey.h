#pragma once

#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	// https://github.com/EpicGames/UnrealEngine/blob/152d20bb0a4b87f09d256e04b32a4753d46a27fe/Engine/Source/Runtime/Engine/Classes/Curves/SimpleCurve.h#L11
	class FSimpleCurveKey {
	public:
		float Time;
		float Value;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FSimpleCurveKey& Value) {
			InputStream >> Value.Time;
			InputStream >> Value.Value;

			return InputStream;
		}
	};
}
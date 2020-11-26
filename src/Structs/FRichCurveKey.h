#pragma once

#include "../Enums/ERichCurveInterpMode.h"
#include "../Enums/ERichCurveTangentMode.h"
#include "../Enums/ERichCurveTangentWeightMode.h"
#include "../Streams/BaseStream.h"

namespace Zen::Structs {
	using namespace Enums;

	class FRichCurveKey {
	public:
		ERichCurveInterpMode InterpMode;
		ERichCurveTangentMode TangentMode;
		ERichCurveTangentWeightMode TangentWeightMode;
		float Time;
		float Value;
		float ArriveTangent;
		float ArriveTangentWeight;
		float LeaveTangent;
		float LeaveTangentWeight;

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FRichCurveKey& Value) {
			uint8_t InterpMode;
			InputStream >> InterpMode;
			Value.InterpMode = (ERichCurveInterpMode)InterpMode;

			uint8_t TangentMode;
			InputStream >> TangentMode;
			Value.TangentMode = (ERichCurveTangentMode)TangentMode;

			uint8_t TangentWeightMode;
			InputStream >> TangentWeightMode;
			Value.TangentWeightMode = (ERichCurveTangentWeightMode)TangentWeightMode;

			InputStream >> Value.Time;
			InputStream >> Value.Value;
			InputStream >> Value.ArriveTangent;
			InputStream >> Value.ArriveTangentWeight;
			InputStream >> Value.LeaveTangent;
			InputStream >> Value.LeaveTangentWeight;

			return InputStream;
		}
	};
}
#pragma once

#include "../Streams/BaseStream.h"
#include "../Enums/EStrippedData.h"

namespace Zen::Structs {
	using namespace Enums;

	// https://github.com/EpicGames/UnrealEngine/blob/c8b97592b9adbe93d85199937120645655fe3651/Engine/Source/Runtime/Engine/Public/EngineUtils.h#L809
	class FStripDataFlags {
	public:
		uint8_t GlobalStripFlags;
		uint8_t ClassStripFlags;

		FStripDataFlags() = default;

		FStripDataFlags(Streams::BaseStream& InputStream) {
			InputStream >> *this;
		}

		friend Streams::BaseStream& operator>>(Streams::BaseStream& InputStream, FStripDataFlags& Value) {
			InputStream >> Value.GlobalStripFlags;
			InputStream >> Value.ClassStripFlags;

			return InputStream;
		}

		bool IsEditorDataStripped() const {
			return (GlobalStripFlags & (uint8_t)EStrippedData::Editor) != 0;
		}

		bool IsDataStrippedForServer() const {
			return (GlobalStripFlags & (uint8_t)EStrippedData::Server) != 0;
		}

		bool IsClassDataStripped(uint8_t InFlags) const {
			return (ClassStripFlags & InFlags) != 0;
		}
	};
}
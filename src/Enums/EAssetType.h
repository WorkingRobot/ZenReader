#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum class EAssetType : uint8_t {
		UASSET,				// name maps, export/import maps, etc
		UEXP,				// asset data
		UBULK,				// images, skins, etc
		UMAP,				// maps

		UPROJECT,			// FortniteGame.uproject, just json
		UPLUGINMANIFEST,	// FortniteGame.upluginmanifest, just json
		UPLUGIN,			// just json

		USHADERBYTECODE,	// shader bytecode
		UPIPELINECACHE,		// something?
		UDIC,				// oodle dictionary network compression

		LOCMETA,			// localization
		LOCRES,				// localization

		PNG,				// images
		PSD,				// photoshop psd

		UFONT,				// otf format
		TTF,				// font
		TPS,				// font metadata, xml

		ICU,				// icudt64l stuff
		RES,				// icudt64l stuff
		CFU,				// icudt64l stuff
		NRM,				// icudt64l stuff
		BRK,				// icudt64l stuff
		DICT,				// icudt64l stuff

		BIN,				// shader cache, asset registry, PCM audio (Content/VoiceData/player_message_01_24k.bin)
		INI,				// hotfixes, engine inis, etc
		PEM,				// cacert.pem
		K,					// facial animations, they're zip files with stuff in em

		UNKNOWN = 0xFF,		// extension is not known
	};
}
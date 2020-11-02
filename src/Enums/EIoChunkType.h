#pragma once

#include <stdint.h>

namespace Zen::Enums {
	enum class EIoChunkType : uint8_t
	{
		Invalid,
		InstallManifest,
		ExportBundleData,
		BulkData,
		OptionalBulkData,
		MemoryMappedBulkData,
		LoaderGlobalMeta,
		LoaderInitialLoadMeta,
		LoaderGlobalNames,
		LoaderGlobalNameHashes,
		ContainerHeader
	};
}
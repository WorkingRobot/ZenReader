#pragma once

#include "../Enums/EPixelFormat.h"
#include "../Streams/BaseStream.h"
#include "FVirtualTextureDataChunk.h"

namespace Zen::Structs {
	using namespace Enums;

	class FVirtualTextureBuiltData {
	public:
		uint32_t NumLayers;
		uint32_t WidthInBlocks;
		uint32_t HeightInBlocks;
		uint32_t TileSize;
		uint32_t TileBorderSize;

		uint32_t NumMips;
		int Width;
		int Height;
		std::vector<uint32_t> TileIndexPerChunk;
		std::vector<uint32_t> TileIndexPerMip;
		std::vector<uint32_t> TileOffsetInChunk;

		std::vector<EPixelFormat> LayerTypes;
		std::vector<FVirtualTextureDataChunk> Chunks;
		

		// This doesn't provide a >> operator since it needs a bulk stream
		FVirtualTextureBuiltData(Streams::BaseStream& InputStream, Streams::BaseStream* BulkStream, size_t BulkOffset) {
			int bCooked;
			InputStream >> bCooked;
			InputStream >> NumLayers;
			InputStream >> WidthInBlocks;
			InputStream >> HeightInBlocks;
			InputStream >> TileSize;
			InputStream >> TileBorderSize;

			InputStream >> NumMips;
			InputStream >> Width;
			InputStream >> Height;
			InputStream >> TileIndexPerChunk;
			InputStream >> TileIndexPerMip;
			InputStream >> TileOffsetInChunk;

			LayerTypes.resize(NumLayers);
			for (int i = 0; i < NumLayers; ++i) {
				std::string PixFormat;
				InputStream >> PixFormat;
				LayerTypes.emplace_back(GetPixelFormat(PixFormat));
			}

			{
				int SerializeNum;
				InputStream >> SerializeNum;
				Chunks.reserve(SerializeNum);
				for (int i = 0; i < SerializeNum; ++i) {
					Chunks.emplace_back(InputStream, BulkStream, BulkOffset, NumLayers);
				}
			}
		}
	};
}
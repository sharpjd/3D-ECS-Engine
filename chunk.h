#pragma once

#include <unordered_map>
#include "block.h"

static constexpr int BLOCKS_PER_CHUNK = 16;
static constexpr int BLOCKS_PER_CHUNK_SQUARED = BLOCKS_PER_CHUNK * BLOCKS_PER_CHUNK;
static constexpr int BPC = BLOCKS_PER_CHUNK;
static constexpr int BPCSQ = BLOCKS_PER_CHUNK_SQUARED;

class BlockList {
private:
	//0 = present, 1 = not present
	std::vector<bool> blockList;
public:
	//BlockList(int sx, int sy, int sz) {
	//	blockList.resize(sx * sy * sz);
	//}

	BlockList() {
		blockList.resize(BLOCKS_PER_CHUNK * BLOCKS_PER_CHUNK * BLOCKS_PER_CHUNK);
	}

	inline bool at(int i) {
		return blockList[i];
	}

	std::vector<bool>& data() {
		return blockList;
	}

};

static int globalChunkId = 0;

class Chunk {
private:
	int chunkId = -1;
public:
	std::unordered_map<BlockType, BlockList> blockMaps;

	Chunk()
	{
		chunkId = globalChunkId++;
	}

	//if block is not tracked, return what the chunk generator provides

	//unplaced air is never tracked; if everything falls through, return air
	
};
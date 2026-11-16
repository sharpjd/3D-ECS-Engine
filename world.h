#pragma once 

#include <vector>

#include "block.h"
#include "chunk.h"
#include "chunkGenerator.h"

class World {
private:
public:

	ChunkGenerator chunkGenerator;

	int xChunksMax = -1;
	int yChunksMax = -1;
	int zChunksMax = -1;

	std::vector<Chunk*> chunks;
	//TODO use SparseSet for this
	std::vector<int> chunkIdMap; //maps chunkId to the chunks pointer list above for O(1) Chunk lookup by chunkId

	World(int xChunksMax, int yChunksMax, int zChunksMax)
		: xChunksMax(xChunksMax), yChunksMax(yChunksMax), zChunksMax(zChunksMax)
	{
		int max = std::max({ xChunksMax, yChunksMax, zChunksMax });
		chunks.resize(xChunksMax * yChunksMax * zChunksMax);
	}

	/// <summary>
	/// takes WORLDSPACE coordinates; can be any block within a chunk
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="z"></param>
	/// <returns></returns>
	Chunk& chunkAt(int x, int y, int z) {
		//luckily for us, integer division truncates down
		int cx = x / BPC;
		int cy = y / BPC;
		int cz = z / BPC;
		assert(cx >= 0 && cx <= xChunksMax);
		assert(cy >= 0 && cy <= yChunksMax);
		assert(cz >= 0 && cz <= zChunksMax);

		int index = cx + cz * xChunksMax + cy * (xChunksMax * zChunksMax);
		assert(index < chunks.size());

		return *(chunks[index]);
	}

	/*
	glm::ivec3 chunkBaseCorner(Chunk* chunk) {
		for (int i = 0; i < chunks.size(); i++) {
			if (chunk == chunks[i]) {
				int cx = i % xChunksMax;
				int cy = i / (xChunksMax * zChunksMax);
				int cz = i / xChunksMax;
				return glm::ivec3(
						cx * BPC,
						cy * BPC,
						cz * BPC
					);
			}
		}
		throw std::runtime_error("the chunk is not tracked in the list");
		return glm::ivec3(-1, -1, -1);
	}
	*/

	glm::ivec3 chunkBaseCorner(Chunk* chunk) {
		int i = chunkIdMap[chunk->chunkId()];
		int cx = i % xChunksMax;
		int cy = i / (xChunksMax * zChunksMax);
		int cz = (i % (xChunksMax * zChunksMax)) / zChunksMax;
		cx *= BPC;
		cy *= BPC;
		cz *= BPC;
		return glm::ivec3(cx, cy, cz);
	}

	bool blockExistsAt(int wx, int wy, int wz) { 
		if (not (wx >= 0 && wx < xChunksMax*BPC)) return false;
		if (not (wy >= 0 && wy < yChunksMax*BPC)) return false;
		if (not (wz >= 0 && wz < zChunksMax*BPC)) return false;

		Chunk& chunk = chunkAt(wx, wy, wz);

		int cx = wx % BPC;
		int cy = wy % BPC;
		int cz = wz % BPC;

		for (auto blockList : chunk.blockMaps) {
			bool present = blockList.second.at(cx, cy, cz);
			if (present) return true;
		}

		return false;
	}

	BlockData blockAt(int wx, int wy, int wz) {
		assert(wx >= 0 && wx <= xChunksMax*BPC);
		assert(wy >= 0 && wy <= yChunksMax*BPC);
		assert(wz >= 0 && wz <= zChunksMax*BPC);

		Chunk& chunk = chunkAt(wx, wy, wz);

		int cx = wx % BPC;
		int cy = wy % BPC;
		int cz = wz % BPC;

		for (auto pair : chunk.blockMaps) {
			bool present = pair.second.at(cx, cy, cz);
			if (present) return BlockData{
				pair.first
			};
		}

		return BlockData{
			BlockType::Air
		};
	}

	void generate() {

		/*
		* watch the order; chunks are placed x-by-x, then z-by-z, then from bottom to top (y-by-y)
		*/
		for (int cy = 0; cy < yChunksMax; cy++) {
			for (int cz = 0; cz < zChunksMax; cz++) {
				for (int cx = 0; cx < xChunksMax; cx++) {
					//chunks.push_back(chunkGenerator.generateChunk(cx, cy, cz));
					int index = cx + cz * xChunksMax + cy * (xChunksMax * zChunksMax);
					Chunk* chunk = chunkGenerator.generateChunk(cx, cy, cz);
					chunks[index] = chunk;

					//update the chunk index -> chunk* map
					int chunkId = chunk->chunkId();
					if (chunkIdMap.size() <= chunkId) chunkIdMap.resize(chunkId + 1);
					chunkIdMap[chunkId] = index;
				}
			}
		}
	}
		
};
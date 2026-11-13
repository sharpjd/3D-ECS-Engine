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

	World(int xChunksMax, int yChunksMax, int zChunksMax)
		: xChunksMax(xChunksMax), yChunksMax(yChunksMax), zChunksMax(zChunksMax)
	{
		int max = std::max({ xChunksMax, yChunksMax, zChunksMax });
		chunks.resize(max * max * max);
	}

	Chunk& chunkAt_ChunkXYZ(int chunkX, int chunkY, int chunkZ) {
		int index = chunkX + chunkZ*xChunksMax + chunkY*(xChunksMax*zChunksMax);
		return *(chunks[index]);
	}

	//Chunk& chunkAt_BlockXYZ(int x, int y, int z) {
	//	//not implemented
	//}

	void generate() {

		/*
		* watch the order; chunks are placed x-by-x, then z-by-z, then from bottom to top (y-by-y)
		*/
		for (int cy = 0; cy < yChunksMax; cy++) {
			for (int cz = 0; cz < zChunksMax; cz++) {
				for (int cx = 0; cx < xChunksMax; cx++) {
					//chunks.push_back(chunkGenerator.generateChunk(cx, cy, cz));
					int index = cx + cz * xChunksMax + cy * (xChunksMax * zChunksMax);
					chunks[index] = chunkGenerator.generateChunk(cx, cy, cz);
				}
			}
		}
	}
		
};
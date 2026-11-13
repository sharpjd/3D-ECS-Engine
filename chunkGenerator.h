#pragma once 

#include "block.h"
#include "chunk.h"

#include <glm/gtc/noise.hpp>

class ChunkGenerator {
private:
public:

	BlockType at(int x, int y, int z) {
		BlockType b;
		//if (y < 20) {
		//	b = BlockType::DebugBlock;
		//	return b;
		//}
		/*
		classic Perlin noise is built on a lattice grid where the gradient 
		vectors at the integer grid corners evaluate to zero offset/influence 
		right on the grid points.
		*/
		float floor = 0.0f;
		float scale = 0.03f;
		glm::vec2 pos = glm::vec2(x * scale, z * scale);
		float yCutoff = floor + (glm::perlin(pos) + 1.0f) * 20.0f;
		if (y < yCutoff) {
			b = BlockType::DebugBlock;
			return b;
		}
		b = BlockType::Air;
		return b;
	}

	Chunk* generateChunk(int chunkX, int chunkY, int chunkZ) {
		Chunk* c = new Chunk();

		/*
		* watch the order; chunks are built x-by-x, then z-by-z, then from bottom to top (y-by-y)
		*/
		for (int byLocal = 0; byLocal < BPC; byLocal++) {
			for (int bzLocal = 0; bzLocal < BPC; bzLocal++) {
				for (int bxLocal = 0; bxLocal < BPC; bxLocal++) {

					int bxWorld = chunkX * BPC + bxLocal;
					int byWorld = chunkY * BPC + byLocal;
					int bzWorld = chunkZ * BPC + bzLocal;

					BlockType blockType = at(bxWorld, byWorld, bzWorld);
					if (blockType == BlockType::Air) {
						continue;
					}

					int index = bxLocal + (bzLocal * BPC) + (byLocal * BPC * BPC);

					//note that [blockType] inserts key if it doesnt exist
					c->blockMaps[blockType].data()[index] = true;
				}
			}
		}

		return c;
	}

};
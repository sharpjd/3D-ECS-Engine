#pragma once

#include "chunk.h"
#include "geometries.h"
#include "world.h"
#include <vector>

class ChunkMesher {
private:
	World& world;
	Chunk& chunk_;
	unsigned int VAO_, SSBO_;
	bool ready = false;

	std::vector<uint32_t> ssboVertices_;
	int rightFaces_ = -1, 
		leftFaces_ = -1,
		topFaces_ = -1,
		bottomFaces_ = -1,
		frontFaces_ = -1,
		backFaces_ = -1;
public:
	ChunkMesher(Chunk& chunk, World& world) :
		chunk_(chunk), world(world)
	{
		
	}

	void init() {

		glGenVertexArrays(1, &VAO_);
		glGenBuffers(1, &SSBO_);

		generateSsbo();

		ready = true;
	}

	void generateSsbo() {
		rightFaces_ = 0, 
		leftFaces_ = 0, 
		topFaces_ = 0, 
		bottomFaces_ = 0, 
		frontFaces_ = 0, 
		backFaces_ = 0;

		ssboVertices_.clear();

		BlockList blockList = chunk_.blockMaps[BlockType::DebugBlock];

		/*
		* PACKING ORDER MATTERS (Right, Left, Top, Bottom, Front, Back)
		*/

		auto chunkBaseCorner = world.chunkBaseCorner(&chunk_);

		//right faces
		for (int i = 0; i < blockList.data().size(); i++) {
			if (blockList.data()[i] == false) continue;

			int cx = i % BPC;
			int cy = i / BPCSQ;
			int cz = (i % BPCSQ) / BPC;

			auto chunkSpacePos = glm::ivec3(cx, cy, cz);
			auto worldSpacePos = chunkSpacePos + chunkBaseCorner;
			
			int wx = worldSpacePos.x;
			int wy = worldSpacePos.y;
			int wz = worldSpacePos.z;

			if (not world.blockExistsAt(wx + 1, wy, wz)) {
				ssboVertices_.push_back(packXYZ(cx, cy, cz));
				rightFaces_++;
			}
		}

		//left faces
		for (int i = 0; i < blockList.data().size(); i++) {
			if (blockList.data()[i] == false) continue;

			int cx = i % BPC;
			int cy = i / BPCSQ;
			int cz = (i % BPCSQ) / BPC;

			auto chunkSpacePos = glm::ivec3(cx, cy, cz);
			auto worldSpacePos = chunkSpacePos + chunkBaseCorner;

			int wx = worldSpacePos.x;
			int wy = worldSpacePos.y;
			int wz = worldSpacePos.z;

			if (not world.blockExistsAt(wx - 1, wy, wz)) {
				ssboVertices_.push_back(packXYZ(cx, cy, cz));
				leftFaces_++;
			}
		}

		//top faces
		for (int i = 0; i < blockList.data().size(); i++) {
			if (blockList.data()[i] == false) continue;

			int cx = i % BPC;
			int cy = i / BPCSQ;
			int cz = (i % BPCSQ) / BPC;

			auto chunkSpacePos = glm::ivec3(cx, cy, cz);
			auto worldSpacePos = chunkSpacePos + chunkBaseCorner;
			int wx = worldSpacePos.x;
			int wy = worldSpacePos.y;
			int wz = worldSpacePos.z;

			if (not world.blockExistsAt(wx, wy+1, wz)) {
				ssboVertices_.push_back(packXYZ(cx, cy, cz));
				topFaces_++;
			}
		}

		//bottom faces
		for (int i = 0; i < blockList.data().size(); i++) {
			if (blockList.data()[i] == false) continue;

			int cx = i % BPC;
			int cy = i / BPCSQ;
			int cz = (i % BPCSQ) / BPC;

			auto chunkSpacePos = glm::ivec3(cx, cy, cz);
			auto worldSpacePos = chunkSpacePos + chunkBaseCorner;
			int wx = worldSpacePos.x;
			int wy = worldSpacePos.y;
			int wz = worldSpacePos.z;

			if (not world.blockExistsAt(wx, wy-1, wz)) {
				ssboVertices_.push_back(packXYZ(cx, cy, cz));
				bottomFaces_++;
			}
		}
		
		//front faces
		for (int i = 0; i < blockList.data().size(); i++) {
			if (blockList.data()[i] == false) continue;

			int cx = i % BPC;
			int cy = i / BPCSQ;
			int cz = (i % BPCSQ) / BPC;

			auto chunkSpacePos = glm::ivec3(cx, cy, cz);
			auto worldSpacePos = chunkSpacePos + chunkBaseCorner;
			int wx = worldSpacePos.x;
			int wy = worldSpacePos.y;
			int wz = worldSpacePos.z;

			if (not world.blockExistsAt(wx, wy, wz+1)) {
				ssboVertices_.push_back(packXYZ(cx, cy, cz));
				frontFaces_++;
			}
		}

		//back faces
		for (int i = 0; i < blockList.data().size(); i++) {
			if (blockList.data()[i] == false) continue;

			int cx = i % BPC;
			int cy = i / BPCSQ;
			int cz = (i % BPCSQ) / BPC;

			auto chunkSpacePos = glm::ivec3(cx, cy, cz);
			auto worldSpacePos = chunkSpacePos + chunkBaseCorner;
			int wx = worldSpacePos.x;
			int wy = worldSpacePos.y;
			int wz = worldSpacePos.z;

			if (not world.blockExistsAt(wx, wy, wz-1)) {
				ssboVertices_.push_back(packXYZ(cx, cy, cz));
				backFaces_++;
			}
		}
	}

	uint32_t packXYZ(int cx, int cy, int cz) {
		//ensure in chunkspace; chunks are only 16x16x16
		assert(cx <= 15 && cx >= 0); 
		assert(cy <= 15 && cy >= 0);
		assert(cz <= 15 && cz >= 0);

		uint32_t packed = 0;
		packed |= (cx);
		packed |= (cy << 6);
		packed |= (cz << 12);

		return packed;
	}

	unsigned int VAO() { assert(ready); return VAO_; }
	unsigned int SSBO() { assert(ready); return SSBO_; }
	int rightFaces()  { assert(ready); return rightFaces_; }
	int leftFaces()   { assert(ready); return leftFaces_; }
	int topFaces()    { assert(ready); return topFaces_; }
	int bottomFaces() { assert(ready); return bottomFaces_; }
	int frontFaces()  { assert(ready); return frontFaces_; }
	int backFaces()   { assert(ready); return backFaces_; }
	const std::vector<uint32_t>& ssboVertices() { assert(ready); return ssboVertices_; }
	Chunk& chunk() { return chunk_; }

	bool isReady() {
		return ready;
	}
};
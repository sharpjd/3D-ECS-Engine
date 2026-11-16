#pragma once

enum BlockType {
	Undefined,
	Air,
	DebugBlock
};

class BlockData {
private:
public:
	BlockType blockType = BlockType::Undefined;
};

//class BlockArchetype {
//private:
//public:
//	BlockType blockType;
//
//
//	bool operator==(const BlockArchetype& other) const = default;
//};


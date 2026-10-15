#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <assert.h>

using sparse_index_type = uint32_t; //we have to place this before the template 

template <typename T>
class SparseSet;

struct ISparseCompStorage {
private:
	std::vector<bool> deadCompMap;
public:
	bool isCompDead(int denseIndex) {
		if (denseIndex >= deadCompMap.size())
			return false; //as long as our implementation is correct, it means the comp is not dead

		return deadCompMap[denseIndex];
	}
	void setCompDead(int denseIndex) {
		/*
		* check your logic; there should be no need to set a dead comp index higher than
		* the max index of the dense?
		*/
		assert(denseIndex < denseSize());

		if (denseIndex >= deadCompMap.size()) {
			deadCompMap.resize(denseIndex + 1);
		}

		deadCompMap[denseIndex] = true;
	}
	void resetDeadMap() {
		if (deadCompMap.size() > 0)
			deadCompMap.clear();
	}

	virtual ~ISparseCompStorage() = default;

	virtual void remove(int entityIndex, bool assertExists = true) = 0;

	virtual void resize(int size) = 0;

	virtual bool isMapped(int index) = 0;

	virtual int denseToSparseIndex(int denseIndex) = 0;

	virtual int sparseToDenseIndex(int sparseIndex) = 0;

	virtual int sparseSize() = 0;

	virtual int denseSize() = 0;
};

template<typename T>
class SparseCompStorage : public ISparseCompStorage {
public:
	SparseSet<T> storage;

	T& operator[](sparse_index_type sparseIndex) { //access via sparse index
		return storage[sparseIndex];
	}

	bool isMapped(int index) {
		return storage.isMapped(index);
	}

	void remove(int entityIndex, bool checkExists) override {
		storage.remove(entityIndex, checkExists);
	}

	sparse_index_type add(T item) {
		return storage.add(item);
	}

	int denseToSparseIndex(int denseIndex) {
		return storage.denseToSparseIndex(denseIndex);
	}

	int sparseToDenseIndex(int sparseIndex) {
		return storage.sparseToDenseIndex(sparseIndex);
	}

	int sparseSize() {
		return storage.size();
	}

	int denseSize() {
		return storage.getDense().size();
	}

	void resize(int size) {
		storage.resize(size);
	}
};



template <typename T>
class SparseSet {

	struct Item {
		T data;
		int sparseIndex;
	};

private:
	std::vector<Item> dense; //the dense vector containing the data
	std::vector<int> sparse; //each index contains the actual index of the data in the dense vector (if any, otherwise -1)
	std::vector<int> freeIndices;

public:

	T& operator[](sparse_index_type sparseIndex) {
		int denseIndex = sparse[sparseIndex];

		assert(denseIndex >= 0); //if false, this means we tried to access an unassigned sparse index

		return dense[denseIndex].data;
	}

	/*T& at(sparse_index_type sparseIndex) {
		if (sparse[sparseIndex] < 0) {
			std::string error = "Tried to access a sparse index that "
				+ "is not mapped to anything ("
				+ std::to_string(sparseIndex)
				+ "), use addAt to assign something to this index"
				;
			throw std::runtime_error(error);
		}
		return dense[sparse[sparseIndex]].data;
	}*/

	size_t size() {
		return sparse.size();
	}

	/*
	Resize the sparse vector to the given size and set any new indices to -1
	*/
	void resize(int newSize) {
		int oldSize = sparse.size();
		sparse.resize(newSize);
		for (int i = oldSize; i < newSize; i++) {
			sparse[i] = -1;
			//freeIndices.push_back(i); //not properly implemented, not needed for now
		}
		//dense.resize(newSize); //the dense has to be big enough, too //WRONG: we handle this in addAt()
	}

	/*
	since the [] operator returns the address of an item in the dense vector
	(which may not exist / is already gone), a simple sparseSet[i] = <thing> sometimes means
	you're trying to write to an undefined location. addAt() adds the item to the dense vector
	and maps this sparse index to the item in the dense vector. 
	*/
	void addAt(int sparseIndex, T item) {
		if (sparseIndex > sparse.size()) {
			std::string error = "(addAt) tried to assign an index ("
				+ std::to_string(sparseIndex)
				+ ") that is bigger than the size of the sparse itself ("
				+ std::to_string(sparse.size())
				+ ")"
				;
			throw std::runtime_error(error);
		}
		if (sparse[sparseIndex] > 0) {
			std::string error = "(addAt) tried to assign an index that is already mapped to a dense index( ["
				+ std::to_string(sparseIndex)
				+ "] = "
				+ std::to_string(sparse[sparseIndex])
				+ ")"
				;
			throw std::runtime_error(error);
		}
		dense.push_back({
			std::move(item),
			sparseIndex
		});
		int denseIndex = dense.size()-1;

		sparse[sparseIndex] = denseIndex;
	}

	/*
	add the item and return the sparse index of the item just added
	*/
	sparse_index_type add(T item) {

		sparse_index_type sparseIndex;
		if (freeIndices.size() > 0) { //recycle an indice that's unused
			sparseIndex = freeIndices.back;
			freeIndices.pop_back();
		}
		else { //no free indices, so expand the sparse vector
			sparseIndex = sparse.size();
			sparseIndex = sparse.push_back(-1);
		}

		sparse_index_type denseIndex = dense.size();

		dense.push_back({
			std::move(item),
			sparseIndex
		});

		sparse[sparseIndex] = denseIndex;

		return sparseIndex;
	}

	/*
	Removes the item associated with the index from the dense vector and marks the index as unused 
	(to be recycled for future adds)
	*/
	void remove(int index, bool assertExists = true) {
		/*
		* if we need to remove something in the middle of the dense, we don't resize/shift
		* the dense vector and instead overwrite the slot with the last item,
		* then update the sparse index mapping; this gives O(1) removals
		*/
		int denseIndex = sparse[index]; //to remove from the dense vector

		if (assertExists) {
			assert(denseIndex > 0);
		}

		if (denseIndex < 0) return; //skip if nothing is mapped to this sparse index

		Item last = dense.back(); //grab the last item in the dense vector

		dense[denseIndex] = last; //copy the last item to the removed index
		sparse[last.sparseIndex] = denseIndex; //update the sparse to point to the new dense index

		//if (dense.size() != 1) //edge case: if size=1, popping it makes the size 0 (vector becomes unindexable) //WRONG: lol u fool
		dense.pop_back(); //pop the last item after we've copied it; pay attention, this needs to come after the copy

		sparse[index] = -1; //nothing is assigned to this index anymore!
		//freeIndices.push_back(index); //not properly implemented, not needed for now
	}

	bool isMapped(int index) {
		if (index > sparse.size() - 1) return false;
		return sparse[index] >= 0;
	}

	int denseToSparseIndex(int denseIndex) {
		assert(denseIndex < dense.size());
		return dense[denseIndex].sparseIndex;
	}

	int sparseToDenseIndex(int sparseIndex) {
		assert(isMapped(sparseIndex));
		return sparse[sparseIndex];
	}

	std::vector<Item>& getDense() {
		return dense;
	}

	//TODO trimming extremely sparse denses

};
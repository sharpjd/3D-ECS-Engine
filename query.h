#pragma once

#include "componentStorage.h"
#include "registry.h"


template<typename... Components>
class Query {
public:
	Registry& reg;

	ISparseCompStorage* smallestStorage = nullptr;
	std::vector<ISparseCompStorage*> relevantStorages;

	Query(
		Registry& reg
	)
		: reg(reg)
	{
		(relevantStorages.push_back(reg.getComponentStorage<Components>()), ...);
		selectSmallestDense();
	}

	int count() {
		if (smallestStorage == nullptr) return 0;
		return smallestStorage->denseSize();
	}

	class Iterator {
	public:
		Query* query;
		size_t index = 0;

		Iterator(Query* query, size_t index)
			: query(query), index(index)
		{
			skipNonMatches();
		}

		Iterator& operator++() {
			++index;
			skipNonMatches();
			return *this;
		}

		bool operator!=(const Iterator& other) const {
			return index != other.index;
		}

		auto operator*() {
			int entityIndex = query->smallestStorage->denseToSparseIndex(index);

			return std::tuple<EntityHandle, Components&...> {
				query->reg.getEntityHandle(entityIndex),
					query->reg.getComponentStorageDirect<Components>()[entityIndex]...
			};
		}

		void skipNonMatches() {

			while (index < query->smallestStorage->denseSize()) {
				bool matches = true;

				for (ISparseCompStorage* storage : query->relevantStorages) {
					//skip checking against ourself (i.e. the smallest dense we are already iterating over)
					if (storage == query->smallestStorage) continue;

					int entityIndex = query->smallestStorage->denseToSparseIndex(index);

					//skip if the entity does not contain the component
					if (!storage->isMapped(entityIndex)) {
						matches = false;
						break;
					}

					//skip if the entity is marked as dead
					if (query->reg.isEntityIndexDeadThisCycle(entityIndex)) {
						matches = false;
						break;
					}

					//skip if the component is marked as dead
					if (storage->isCompDead(storage->sparseToDenseIndex(entityIndex))) {
						matches = false;
						break;
					}

				}

				if (matches) {
					return;
				}

				index++;
			}

		}
	};

	Iterator begin() {
		return Iterator(this, 0);
	}

	Iterator end() {
		return Iterator(this, smallestStorage->denseSize());
	}

private:
	void selectSmallestDense() {
		int smallestDenseSize = INT32_MAX;

		for (auto& storage : relevantStorages) {
			if (storage->denseSize() < smallestDenseSize) {
				smallestDenseSize = storage->denseSize();
				smallestStorage = storage;
			}
		}
	}
};
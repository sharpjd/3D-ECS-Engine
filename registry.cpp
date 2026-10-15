#include "registry.h"

Registry::Registry() {
	initialize();
}

void Registry::initialize() {
	//this is kind of just OCD...
	int bytes = sizeof(ComponentType);
	int permutations = 2;
	for (int i = 0; i < bytes * 8 - 1; i++) {
		permutations *= 2;
	}
	//gauruntee we have enough indexes for the number of components
	//we will probably need dynamic/manual max index if we want to use >uint8_t tags
	//but that day will never come, right?
	componentStorages.resize(permutations);
}

EntityHandle Registry::newEntity() {
	if (inactiveIndices.size() == 0) {
		//all occupied, increment entity index by 1
		entityList.resize(entityList.size() + 1);
		////(implicitly, we have to also expand the component sets by 1) //actually, no we don't
		//for (auto& s : componentSets) {
		//	IComponentStorage* cs = s.get();
		//	cs->resize(cs->size()+1);
		//	//TODO: maybe we should expand by more than just 1
		//}
		return EntityHandle{
			(uint32_t)(entityList.size()) - 1, //index
			0 //implicitly, the generation is 0 because we haven't used this index before
		};
	}
	else {
		//recycle an old, inactive index
		int index = inactiveIndices.back();
		inactiveIndices.pop_back();
		int generation = entityList[index] * -1;
		if (generation < 0) {
			std::string error = "(newEntity) index "
				+ std::to_string(index)
				+ " was in the inactiveIndices list but "
				+ " the value (generation) of the index was still positive ("
				+ std::to_string(entityList[index])
				+ "), how did this happen?"
				;
			throw std::runtime_error(error);
		}

		entityList[index] = generation;

		return EntityHandle{
			(uint32_t)index,
			(uint32_t)(generation)
		};
	}
}

void Registry::deleteEntity(EntityHandle entityHandle) {
	entityDeletionQueue.push_back(entityHandle);

	if (deadEntityMap.size() <= entityHandle.index)
		deadEntityMap.resize(entityHandle.index + 1);

	deadEntityMap[entityHandle.index] = true;
}

bool Registry::isEntityIndexDeadThisCycle(int entityIndex) {
	if (entityIndex >= deadEntityMap.size())
		return false; //as long as our implementation is correct, it means the entity is not dead
	return deadEntityMap[entityIndex];
}

inline const std::vector<int>& Registry::getActiveEntities() {
	return entityList;
}

EntityHandle Registry::getEntityHandle(int index) {
	if (entityList[index] < 0) {
		throw std::runtime_error("Entity index is inactive");
	}
	EntityHandle eh = {
		index,
		entityList[index]
	};
	return eh;
}

/// <summary>
/// 
/// </summary>
/// <param name="entity"></param>
/// <returns>
/// 1: entity index is active and the generation matches
/// -1: entity index is inactive 
/// -2: entity index is active but generation does not match
/// -3: entity handle index exceeds the size of the active entities list
/// -4: entity index is active and generation matches, but it has been marked as dead this cycle
/// </returns>
int Registry::entityValid(EntityHandle entity) {
	auto& entities = getActiveEntities();
	if (entity.index >= entities.size())
		return -3;
	if (entities[entity.index] < 0)
		return -1;
	if (entities[entity.index] != entity.generation)
		return -2;
	if (isEntityIndexDeadThisCycle(entity.index))
		return -4;
	return 1;
}

void Registry::assertEntityValid(EntityHandle entity) {
	if (!entityValid(entity)) throw std::runtime_error("Entity invalid");
}

void Registry::flush() {

	//remove components before entities get de-registered
	for (auto& toDelete : componentDeletionQueue) {
		//componentSets[toDelete.componentType].get()->remove(toDelete.entityHandle.index false);
		removeComponent_internal(toDelete.entityHandle, toDelete.componentType, false); //superior error checking!
	}

	for (auto& toDelete : entityDeletionQueue) {
		deleteEntity_internal(toDelete);
	}

	//flush the dead entity map and dead component map of all storages
	for (auto& storage : componentStorages) {
		if (storage == nullptr) continue;
		storage.get()->resetDeadMap();
	}

	if (deadEntityMap.size() > 0)
		deadEntityMap.clear();

	if (componentDeletionQueue.size() > 0)
		componentDeletionQueue.clear();

	if (entityDeletionQueue.size() > 0)
		entityDeletionQueue.clear();
}

void Registry::deleteEntity_internal(EntityHandle handle, bool deleteComponents) {

	if (handle.index > entityList.size()) {
		std::string error = "tried to delete entity with index/id ("
			+ std::to_string(handle.index)
			+ ") but the index is higher than the size of the Entity tracking list "
			+ "in the registry, this indicates some logic error "
			;
		throw std::runtime_error(error);
	}
	if (entityList[handle.index] < 0) {
		std::string error =
			"Tried to delete entity ID "
			+ std::to_string(handle.index)
			+ " but it was already inactive (value="
			+ std::to_string(entityList[handle.index])
			+ ")"
			;
		throw std::runtime_error(error);
	}
	if (entityList[handle.index] != handle.generation) {
		std::string error =
			"(deleteEntity) The given EntityHandle's generation ("
			+ std::to_string(handle.generation)
			+ ") differs from the one registered ("
			+ std::to_string(entityList[handle.index])
			+ ") (entity ID/index "
			+ std::to_string(handle.index)
			+ "), this indicates a logic error — in particular, "
			+ "the entity with the given generation has already been deleted."
			;
		throw std::runtime_error(error);
	}

	if (deleteComponents) {
		for (auto& s : componentStorages) {
			if (s == nullptr) continue;
			if (s->isMapped(handle.index)) {
				s->remove(handle.index, false);
			}
		}
	}

	//increment the generation for the next entity
	entityList[handle.index]++;
	//flip the generation to negative to keep the generation number
	entityList[handle.index] *= -1;
	inactiveIndices.push_back(handle.index);

}

void Registry::removeComponent_internal(EntityHandle handle, ComponentType componentType, bool checkExists) {
	//first, let's check if the entity even exists/is valid
	if (entityList[handle.index] < 0) {
		std::string error =
			"Tried to remove a component from an entity index that isn't registered: "
			+ std::to_string(handle.index)
			;
		throw std::runtime_error(error);
	}
	if (entityList[handle.index] != handle.generation) {
		std::string error =
			"(removeComponent) The given EntityHandle's generation ("
			+ std::to_string(handle.generation)
			+ ") differs from the one registered ("
			+ std::to_string(entityList[handle.index])
			+ "), this is most definitely a bad logic error (entity ID/index "
			+ std::to_string(handle.index)
			+ "). "
			;
		throw std::runtime_error(error);
	}

	//make sure our storage is big enough to actually index
	auto* storage = componentStorages[componentType].get();

	if (storage->sparseSize() <= handle.index) {
		if (handle.index - storage->sparseSize() > STORAGE_EXPANSION_WARN_SIZE) {
			Debug::log("Warning: indice expansion by >10, investigate potential bad registry bookkeeping");
		}
		storage->resize(handle.index + 1);
	}

	//make sure we are not trying to remove from an index with nothing in it (even if technically we can just skip)
	if (checkExists && !storage->isMapped(handle.index)) {
		std::string error =
			"Tried to remove component of ComponentType "
			+ std::to_string(componentType)
			+ " from entity ("
			+ std::to_string(handle.index)
			+ ") that does not have a component "
			+ "mapped to that index (component might've already been deleted). "
			+ "The existence of components should be "
			+ "bookkept properly to encourage "
			+ "good performant code "
			;
		throw std::runtime_error(error);
	}

	//finally remove
	storage->remove(handle.index, checkExists);
}

#include "registry.h"

template<typename T>
void Registry::removeComponent(EntityHandle handle) {
	componentDeletionQueue.push_back(
		{
			handle,
			getComponentType<T>()
		}
	);
	ISparseCompStorage* storage = getComponentStorage<T>();
	storage->setCompDead(storage->sparseToDenseIndex(handle.index));
}

template<typename T>
inline void Registry::addComponent(EntityHandle handle, T component) {

	//first, let's check if the entity we are adding to even exists/is correct
	if (entityList[handle.index] < 0) {
		std::string error =
			"Tried to add a component to an entity index that isn't registered: "
			+ std::to_string(handle.index)
			;
		throw std::runtime_error(error);
	}
	if (entityList[handle.index] != handle.generation) {
		std::string error =
			"(addComponent) The given EntityHandle's generation ("
			+ std::to_string(handle.generation)
			+ ") differs from the one registered ("
			+ std::to_string(entityList[handle.index])
			+ "), this is most definitely a bad logic error (entity ID/index "
			+ std::to_string(handle.index)
			+ "). "
			;
		throw std::runtime_error(error);
	}

	auto& storage = getComponentStorageDirect<T>();
	//make sure our storage is big enough to actually index
	if (storage.size() <= handle.index) {
		if (handle.index - storage.size() > STORAGE_EXPANSION_WARN_SIZE) {
			Debug::log("Warning: indice expansion by >10, investigate potential bad registry bookkeeping");
		}
		storage.resize(handle.index + 1);
	}

	ComponentType type = getComponentType<T>();
	//make sure we are not overwriting a component
	if (storage.isMapped(handle.index)) {
		std::string error =
			"Tried to add a component of type ";
		error.append(
			typeid(T).name()
		);
		error.append(
			" to entity ("
			+ std::to_string(handle.index)
			+ ") that already has the component "
			+ "mapped to that index. Components should be "
			+ "removed in advance to encourage "
			+ "safeguards against bugs like "
			+ "memory leaks. "
		);
		throw std::runtime_error(error);
	}

	//yay, finally we can add our component
	storage.addAt(handle.index, component);
	//getStorage<T>().add(std::move(component));
}

template<typename T>
inline SparseSet<T>& Registry::getComponentStorageDirect()
{
	ComponentType type = getComponentType<T>();

	//ensure the storage for this component type exists
	if (!componentStorages[type])
	{
		componentStorages[type] =
			std::make_unique<SparseCompStorage<T>>();
	}

	return static_cast<SparseCompStorage<T>*>(
		componentStorages[type].get()
		)->storage;
}

template<typename T>
inline SparseCompStorage<T>* Registry::getComponentStorage() {
	ComponentType type = getComponentType<T>();

	//ensure the storage for this component type exists
	if (!componentStorages[type])
	{
		componentStorages[type] =
			std::make_unique<SparseCompStorage<T>>();
	}

	return static_cast<SparseCompStorage<T>*>(
		componentStorages[type].get()
		);
}

/// <summary>
/// Note: iterating over EntityHandles is not as performant because you are 
/// jumping unpredictably within dense vectors.
/// </summary>
/// <typeparam name="...Components"></typeparam>
/// <returns></returns>
template<typename ...Components>
inline std::vector<EntityHandle> Registry::queryComponents()
{
	std::vector<EntityHandle> result;

	for (int index = 0; index < entityList.size(); ++index)
	{
		if (entityList[index] < 0)
			continue;

		EntityHandle entity{
			index,
			entityList[index]
		};

		if (hasComponents<Components...>(entity))
			result.push_back(entity);
	}

	return result;
}

template<typename ...Components>
inline bool Registry::hasComponents(EntityHandle entity) {
	return (hasComponent<Components>(entity) && ...);
}

template<typename T>
inline bool Registry::hasComponent(EntityHandle entity) {
	assertEntityValid(entity);

	auto& storage = getComponentStorageDirect<T>();
	return storage.isMapped(entity.index);
}

template<typename ...Components>
inline Query<Components...> Registry::query() {
	return Query<Components...>(
		*this
	);
}

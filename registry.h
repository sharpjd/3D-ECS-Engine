#pragma once
#include "entityHandle.h"
#include "componentStorage.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include "debug.h"

template <typename... Components>
class Query;

using ComponentType = uint8_t;

inline ComponentType nextComponentType = 0;

template<typename T>
ComponentType getComponentType()
{
	static ComponentType type = nextComponentType++;
	return type;
}

class Registry {
public:

	/*
	if any singular storage vector has to be expanded by more indices than this in a single
	indexing or add operation throw a warning; this might mean we are not doing 
	bookkeeping properly
	*/
	const int STORAGE_EXPANSION_WARN_SIZE = 10;

	Registry();

	EntityHandle newEntity();

	std::vector<bool> deadEntityMap;
	void deleteEntity(EntityHandle entityHandle);
	bool isEntityIndexDeadThisCycle(int entityIndex);

	template <typename T>
	void removeComponent(EntityHandle handle);

	template <typename T>
	void addComponent(EntityHandle handle, T component);

	template <typename T>
	SparseSet<T>& getComponentStorageDirect();

	template <typename T>
	SparseCompStorage<T>* getComponentStorage();

	template <typename... Components>
	std::vector<EntityHandle> queryComponents();

	const std::vector<int>& getActiveEntities();

	EntityHandle getEntityHandle(int index);

	template <typename... Components>
	bool hasComponents(EntityHandle entity);

	template <typename T>
	bool hasComponent(EntityHandle entity);

	int entityValid(EntityHandle entity);

	void assertEntityValid(EntityHandle entity);

	template<typename... Components>
	Query<Components...> query();

	void flush();

	struct ComponentToDelete {
		EntityHandle entityHandle;
		ComponentType componentType;
	};

private:

	std::vector<EntityHandle> entityDeletionQueue;
	std::vector<ComponentToDelete> componentDeletionQueue;

	void deleteEntity_internal(EntityHandle handle, bool deleteComponents = true);

	void removeComponent_internal(EntityHandle handle, ComponentType componentType, bool checkExists = true);

	void initialize();

	//index of each comp type == comp type()
	std::vector<std::unique_ptr<ISparseCompStorage>> componentStorages; 
	//index is Entity ID/index, value is generation, negative value means the indice is inactive (the absolute value is the generation)
	std::vector<int> entityList;
	std::vector<int> inactiveIndices;
};

#include "registry_tpp.cpp" // VCIC-Excluded 
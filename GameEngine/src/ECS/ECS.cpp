#include "ECS.h"
#include "../Logger/Logger.h"
#include <iostream>

// ---------- Component ----------
int IComponent::nextId = 0;

// ---------- Entity ----------

int Entity::GetId() const
{
	return id;
}

// ---------- System ----------
void System::AddEntityToSystem(Entity entity)
{
	entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{
	entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
			return entity == other;
		}), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const
{
	return std::vector<Entity>();
}

const Signature& System::GetComponentSignature() const
{
	return componentSignature;
}

// ---------- Registry ----------
Entity Registry::CreateEntity()
{
	int entityId = numEntities++;
	if (entityId >= entityComponentSignatures.size()) {
		entityComponentSignatures.resize(entityId + 1);
	}

	Entity entity(entityId);
	entity.registry = this;
	entitiesToBeAdded.push_back(entity);

	if (entityId >= entityComponentSignatures.size()) {
		entityComponentSignatures.resize(entityId + 1);
	}

	Logger::Log("Entity Created with id = " + std::to_string(entityId));
	return entity;
}

void Registry::Update()
{
	for (auto entity : entitiesToBeAdded) {
		AddEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();

}

void Registry::AddEntityToSystems(Entity entity)
{
	const auto& entityId = entity.GetId();

	const auto& entityComponentSignature = entityComponentSignatures[entityId];

	for (auto& system : systems) {
		const auto& systemComponentSignature = system.second->GetComponentSignature();

		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested) {
			system.second->AddEntityToSystem(entity);
		}


	}

}

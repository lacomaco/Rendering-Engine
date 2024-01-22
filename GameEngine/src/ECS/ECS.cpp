#include "ECS.h"
#include "../Logger/Logger.h"

// ---------- Component ----------


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
	entitiesToBeAdded.push_back(entity);

	Logger::Log("Entity Created with id = " + entityId);
	return entity;
}

void Registry::Update()
{

}

void Registry::AddEntityToSystem(Entity entity)
{
}

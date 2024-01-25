#pragma once

#include "../Logger/Logger.h"
#include <vector>
#include <bitset>
#include <set>
#include <deque>
#include <unordered_map>
#include <typeindex>
#include <memory>

const unsigned int MAX_COMPONENTS = 32;

typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
    protected:
        static int nextId;
};

template <typename T>
class Component: public IComponent {
    public:
        static int GetId() {
            // 주의! : 지연 초기화를 위해서 GetId 호출 시점에 id를 부여하였음.
            // Component 생성 시점이 아닌 GetId 호출 시점이니 유의해야함.
            static auto id = nextId++;
            return id;
        }
};

class Entity {
    private:
        int id;

    public:
        Entity(int id): id(id) {};
        Entity(const Entity& entity) = default;
        void Kill();
        int GetId() const;

		void Tag(const std::string& tag);
    	bool HasTag(const std::string& tag) const;
    	void Group(const std::string& group);
    	bool BelongsToGroup(const std::string& group) const;
       
        template <typename TComponent, typename ...TArgs> void AddComponent(TArgs&& ...args);
        template <typename TComponent> void RemoveComponent();
        template <typename TComponent> bool HasComponent() const;
        template <typename TComponent> TComponent& GetComponent() const;

        Entity& operator =(const Entity& other) = default;
        bool operator ==(const Entity& other) const { return id == other.id; }
        bool operator !=(const Entity& other) const { return id != other.id; }
        bool operator >(const Entity& other) const { return id > other.id; }
        bool operator <(const Entity& other) const { return id < other.id; }

        // registry 전방선언
        // registry가 전방선언 되었기 때문에 template 코드는 Registry 정의부 아래에 위치해야한다.
        class Registry* registry;
};
class System {
    private:
        Signature componentSignature;
        std::vector<Entity> entities;

    public:
        System() = default;
        ~System() = default;
        
        void AddEntityToSystem(Entity entity);
        void RemoveEntityFromSystem(Entity entity);
        std::vector<Entity> GetSystemEntities() const;
        const Signature& GetComponentSignature() const;

        template <typename TComponent> void RequireComponent();
};

class IPool {
    public:
        virtual ~IPool() = default; 
        virtual void RemoveEntityFromPool(int entityId) = 0;
};

template <typename T>
class Pool: public IPool {
    private:
        std::vector<T> data;
        int size;

        std::unordered_map<int, int> entityIdToIndex;
        std::unordered_map<int, int> indexToEntityId;

    public:
        Pool(int capacity = 100) {
            size = 0;
            data.resize(capacity);
        }

        virtual ~Pool() = default;

        bool IsEmpty() const {
            return size == 0;
        }

        int GetSize() const {
            return size;
        }

        void Clear() {
            data.clear();
            entityIdToIndex.clear();
            indexToEntityId.clear();
            size = 0;
        }

        void Set(int entityId, T object) {
            if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
                int index = entityIdToIndex[entityId];
                data[index] = object;
            } else {
                int index = size;
                /*
                * push_back: 요소를 이동,복사
                * emplace: 요소를 직접생성
                * 
                * 이동과 복사는 외부에서 생성 후 컨테이너로 들어가지만
                * emplace는 컨테이너 내부에서 생성된다.
                * 
                */
                entityIdToIndex.emplace(entityId, index);
                indexToEntityId.emplace(index, entityId);
                if (index >= data.capacity()) {
                    data.resize(size * 2);
                }
                data[index] = object;
                size++;
            }
        }

        void Remove(int entityId) {
            /*
            * 
            * 실제로 data에서 entity를 제거하지는 않는다.
            * 제거할 entity를 끝자리로 옮긴 후 size를 축소시킨다.
            * 그 후 나중에 새로운 Entity가 들어올때 끝자리를 덮어씌우게되서 결과적으로 데이터를 지운 효과를 가져온다.
            * 
            * -----------------------------------
            * 요소를 삭제할때 맨 뒤의 요소를 지울 위치의 요소와 바꿔치기 한 후 size를 축소시킨다.
            * 
            * 1. 풀에서 지울 entity 정보를 얻어낸다.
            * 2. 풀의 마지막 요소를 지울 entity 자리로 옮긴다.
            * 3. 지울 entity를 마지막 요소로 이동시킨다.
            * 4. 끝자리를 지운다.
            * 
            * 지울때 entityIdToindex, IndexToEntityId를 갱신하는걸 잊지 말아야한다.
            */
		    int indexOfRemoved = entityIdToIndex[entityId];
		    int indexOfLast = size - 1;
		    data[indexOfRemoved] = data[indexOfLast];

            int entityIdOfLastElement = indexToEntityId[indexOfLast];
            entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
            indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

            entityIdToIndex.erase(entityId);
            indexToEntityId.erase(indexOfLast);

            size--;
        }

        void RemoveEntityFromPool(int entityId) override {
            // PS. entityIdToIndex.find(entityId) != entityIdToIndex.end()
            // 왜 end와 비교하냐면... C++에서 find할때 찾지 못하면 end를 반환하기 때문이다.
            // 맘에 안들지만 이렇다 하니 따라야한다.
            if (entityIdToIndex.find(entityId) != entityIdToIndex.end()) {
                Remove(entityId);
            }
        }

        T& Get(int entityId) {
            int index = entityIdToIndex[entityId];
            return static_cast<T&>(data[index]);
        }

        T& operator [](unsigned int index) {
            return data[index];
        }
};

class Registry {
    private:
        int numEntities = 0;
        std::vector<std::shared_ptr<IPool>> componentPools;
        /*
        * entityComponentSignature 자체가 대명사 아님
        * entity가 가지고 있는 ComponentSignature를 의미한다.
        * 
        * 이 벡터의 index <- entityId
        * key값은 컴포넌트 시그니쳐이다.
        */
        std::vector<Signature> entityComponentSignatures;

        std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

        std::set<Entity> entitiesToBeAdded;
        std::set<Entity> entitiesToBeKilled;

		std::unordered_map<std::string, Entity> entityPerTag;
        std::unordered_map<int, std::string> tagPerEntity;

		std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
        std::unordered_map<int, std::string> groupPerEntity;
        std::deque<int> freeIds;

    public:
        Registry() {
            Logger::Log("Registry constructor called!");
        }
        
        ~Registry() {
            Logger::Log("Registry destructor called!");
        }

        void Update();
        
        Entity CreateEntity();
        void KillEntity(Entity entity);

		void TagEntity(Entity entity, const std::string& tag);
		bool EntityHasTag(Entity entity, const std::string& tag) const;
		Entity GetEntityByTag(const std::string& tag) const;
		void RemoveEntityTag(Entity entity);

		void GroupEntity(Entity entity, const std::string& group);
		bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
		std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
		void RemoveEntityGroup(Entity entity);

        template <typename TComponent, typename ...TArgs> void AddComponent(Entity entity, TArgs&& ...args);
        template <typename TComponent> void RemoveComponent(Entity entity);
		template <typename TComponent> bool HasComponent(Entity entity) const;
        template <typename TComponent> TComponent& GetComponent(Entity entity) const;

        template <typename TSystem, typename ...TArgs> void AddSystem(TArgs&& ...args);
        template <typename TSystem> void RemoveSystem();
        template <typename TSystem> bool HasSystem() const;
        template <typename TSystem> TSystem& GetSystem() const;

        void AddEntityToSystems(Entity entity);
        void RemoveEntityFromSystems(Entity entity);
};

template <typename TComponent>
void System::RequireComponent() {
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

template <typename TSystem, typename ...TArgs>
void Registry::AddSystem(TArgs&& ...args) {
    // use case: registry->AddSystem<MovementSystem>();
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::RemoveSystem() {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template <typename TSystem>
bool Registry::HasSystem() const {
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::GetSystem() const {
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&& ...args) {
    /*
    * use case: registry->AddComponent<TransformComponent>(entity, 0, 0, 0, 32, 32, 1);
    * 
    * 이 메소드를 통해서 컴포넌트를 Registry에 등록한다.
    * 
    * 이때 반드시 Entity가 있어야 등록 가능하다. (컴포넌트만 있는 경우는 사실상 없다고 봐야한다.)
    * 
    * 이 메소드 내에서 componentPool의 생성과 등록을 함께 수행하며
    * 
    * entity - component를 연결하는 작업도 수행한다. (componentSignature);
    */
    const auto componentId = Component<TComponent>::GetId();
    const auto entityId = entity.GetId();

    if (componentId >= componentPools.size()) {
        componentPools.resize(componentId + 1, nullptr);
    }

    if (!componentPools[componentId]) {
        std::shared_ptr<Pool<TComponent>> newComponentPool(new Pool<TComponent>());
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    TComponent newComponent(std::forward<TArgs>(args)...);

    componentPool->Set(entityId, newComponent);

    entityComponentSignatures[entityId].set(componentId);

    Logger::Log("Component id = " + std::to_string(componentId) + " was added to entity id " + std::to_string(entityId));
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity) {
    /*
    * 컴포넌트에 바인딩된 Entity를 제거한다.
    * 
    * 컴포넌트 풀에서 entity 데이터를 제거하고
    * 엔티티 시그니처에서도 해당 컴포넌트를 false 처리해야한다.
    */
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
    
    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    componentPool->Remove(entityId);

	entityComponentSignatures[entityId].set(componentId, false);
    
    Logger::Log("Component id = " + std::to_string(componentId) + " was removed from entity id " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const {
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Registry::GetComponent(Entity entity) const {
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return componentPool->Get(entityId);
}

template <typename TComponent, typename ...TArgs>
void Entity::AddComponent(TArgs&& ...args) {
    registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent() {
    registry->RemoveComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::HasComponent() const {
    return registry->HasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::GetComponent() const {
    return registry->GetComponent<TComponent>(*this);
}

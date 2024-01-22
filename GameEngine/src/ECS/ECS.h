#pragma once
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>

const unsigned int MAX_COMPONENTS = 32;

// 비트로 엔티티가 어떤 컴포넌트를 가지고 있는지 표시
// 비트연산을 통해 확인한다.
typedef std::bitset<MAX_COMPONENTS> Signature;

struct IComponent {
protected:
	static int nextId;
};

template <typename T>
class Component : public IComponent {
	static int GetId() {
		static auto id = nextId++;
		return id;
	}
};

class Entity {
private:
	int id;

public:
	Entity(int id): id(id) {}
	Entity(const Entity& other) = default;
	int GetId() const;

	Entity& operator = (const Entity& other) = default;
	bool operator==(const Entity& other) const {
		return id == other.id;
	}
	bool operator!=(const Entity& other) const {
		return id != other.id;
	}
	bool operator<(const Entity & other) const {
		return id < other.id;
	}
	bool operator>(const Entity & other) const {
		return id > other.id;
	}
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

	template <typename T> void RequireComponent();
};

class IPool {
public:
	virtual ~IPool() {}

};


template <typename T>
class Pool: public IPool {
private:
	std::vector<T> data;

public:
	Pool(int size = 100) {
		data.resize(size);
	}

	virtual ~Pool() = default;

	bool isEmpty() const{
		return data.empty();
	}
	int GetSize() const {
		return data.size();
	}

	void Resize(int n) {
		data.resize(n);
	}

	void Clear() {
		data.clear();
	}

	void Add(T object) {
		data.push_back(object);
	}

	void Set(int index, T object) {
		data[index] = object;
	}

	T& Get(int index) const {
		return static_cast<T&>(data[index]);
	}

	T& operator[](int index) const {
		return static_cast<T&>(data[index]);
	}

};

/*
* Registry는 다음과 같은 역할을 한다.
* 1. 엔티티 생성과 파괴에 관여한다.
* 2. 엔티티에 컴포넌트, 시스템을 추가하는것을 관여한다.
*/
class Registry { 
private:
	int numEntities = 0;

	/*
	 * componentPools 배열의 구조를 나타내는 표:
	 * 총 entity 수: 2
	 * 총 component 유형: 3
	 * +-------------+-------------+-------------+-------------+--------+
	 * | component/poolIndex	|    [0]		|    [1]    |    [2]	|
	 * +-------------+-------------+-------------+-------------+--------+
	 * | entity 1				|     nullptr	|   {...}	|   nullptr	|
	 * | entity 2				|     nullptr	|   {...}   |   {...}	|
	 * +-------------+-------------+-------------+-------------+--------+
	 *
	 * 설명:
	 * componentPools 벡터 배열에는 componentPool들을 가지고 있습니다.
	 * componentPool는 각 컴포넌트 정보를 가지고 있습니다. componentPool의 인덱스는 컴포넌트의 id와 같습니다.
	 * componentPool을 통해서 특정 엔티티가 가지고 있는 컴포넌트에 접근할 수 잇는 테이블구조입니다.
	 * 
	 * 엔티티가 컴포넌트를 가지고 있는지 조회하는건 entityComponentSignatures을 통해서 하는것이 더 편합니다.
	 * 
	*/
	std::vector<IPool*> componentPools;
	std::vector<Signature> entityComponentSignatures;
	std::unordered_map<std::type_index, System&> systems;

	std::vector<Entity> entitiesToBeAdded;
	std::vector<Entity> entitiesToBeKilled;
public:
	Registry() = default;
	~Registry() = default;

	Entity CreateEntity();
	void Update();
	void AddEntityToSystem(Entity entity);

	template <typename T, typename ...TArgs> void AddComponent(Entity entitiy, TArgs&& ...args);
	

};

template<typename T>
void System::RequireComponent()
{
	const auto componentId = Component<T>::GetId();
	componentSignature.set(componentId);
}

template <typename T, typename ...TArgs>
void Registry::AddComponent(Entity entity, TArgs&&... args) {
	const auto componentId = Component<T>::GetId();
	const auto entityId = entity.GetId();

	// 컴포넌트 풀에 entity 자리가 컴포넌트 풀을 늘린다.
    if (componentId >= componentPools.size()) {
		componentPools.resize(componentId + 1, nullptr);
	}

	// 컴포넌트 풀이 없으면 생성한다.
    if (componentPools[componentId] == nullptr) {
		Pool<T>* newComponentPool = new Pool<T>();
        componentPools[componentId] = newComponentPool;
	}

	Pool<T>* componentPool = componentPools[componentId];

	if(entityId >= componentPool->GetSize()) {
		componentPool->Resize(entityId + 1);
	}

	T newComponent = T(std::forward<TArgs>(args));
	componentPool->Set(entityId, newComponent);
	entityComponentSignatures[entityId].set(componentId);
}

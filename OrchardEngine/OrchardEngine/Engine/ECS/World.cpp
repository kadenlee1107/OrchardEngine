#include "World.hpp"
#include <algorithm>

namespace Orchard::ECS {

World::World() {
    m_EntityRecords.reserve(1000);
}

World::~World() {
}

Entity World::CreateEntity() {
    EntityID id;
    
    if (!m_FreeEntities.empty()) {
        id = m_FreeEntities.back();
        m_FreeEntities.pop_back();
    } else {
        id = m_NextEntityID++;
        if (id >= m_EntityRecords.size()) {
            m_EntityRecords.resize(id + 1);
        }
    }
    
    EntityRecord& record = m_EntityRecords[id];
    record.alive = true;
    record.archetype = nullptr;
    record.indexInArchetype = 0;
    
    return Entity(id, 0);
}

void World::DestroyEntity(Entity entity) {
    if (!IsEntityValid(entity)) return;
    
    EntityRecord& record = m_EntityRecords[entity.id];
    
    if (record.archetype) {
        record.archetype->RemoveEntity(record.indexInArchetype);
    }
    
    record.alive = false;
    record.archetype = nullptr;
    
    m_FreeEntities.push_back(entity.id);
}

bool World::IsEntityValid(Entity entity) const {
    if (entity.id >= m_EntityRecords.size()) return false;
    return m_EntityRecords[entity.id].alive;
}

void World::RemoveComponent(Entity entity) {
}

void World::AddSystem(std::unique_ptr<System> system) {
    system->OnInit(this);
    m_Systems.push_back(std::move(system));
}

void World::Update(double deltaTime) {
    for (auto& system : m_Systems) {
        system->OnUpdate(this, deltaTime);
    }
}

Archetype* World::GetOrCreateArchetype(const std::vector<ComponentTypeID>& componentTypes) {
    uint64_t hash = GetArchetypeHash(componentTypes);
    
    auto it = m_Archetypes.find(hash);
    if (it != m_Archetypes.end()) {
        return it->second.get();
    }
    
    std::vector<ComponentInfo> infos;
    for (ComponentTypeID typeID : componentTypes) {
        infos.push_back(ComponentInfo(typeID, 0, 16));
    }
    
    auto archetype = std::make_unique<Archetype>(infos);
    Archetype* ptr = archetype.get();
    m_Archetypes[hash] = std::move(archetype);
    
    return ptr;
}

uint64_t World::GetArchetypeHash(const std::vector<ComponentTypeID>& componentTypes) {
    std::vector<ComponentTypeID> sorted = componentTypes;
    std::sort(sorted.begin(), sorted.end());
    
    uint64_t hash = 0;
    for (ComponentTypeID typeID : sorted) {
        hash ^= std::hash<ComponentTypeID>{}(typeID) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    
    return hash;
}

void World::MoveEntity(Entity entity, Archetype* newArchetype) {
    if (!IsEntityValid(entity)) return;
    
    EntityRecord& record = m_EntityRecords[entity.id];
    
    if (record.archetype == newArchetype) return;
    
    record.archetype = newArchetype;
    if (newArchetype) {
        record.indexInArchetype = newArchetype->AddEntity(entity);
    }
}

}

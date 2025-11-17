#pragma once

#include "Component.hpp"
#include "Entity.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstring>

namespace Orchard::ECS {

constexpr size_t CHUNK_SIZE = 16384;

struct ComponentInfo {
    ComponentTypeID typeID;
    size_t size;
    size_t alignment;
    size_t offsetInChunk;
    
    ComponentInfo(ComponentTypeID id, size_t s, size_t a)
        : typeID(id), size(s), alignment(a), offsetInChunk(0) {}
};

class Archetype {
public:
    struct Chunk {
        uint8_t* data = nullptr;
        size_t entityCount = 0;
        size_t capacity = 0;
        
        Chunk(size_t cap) : capacity(cap) {
            data = static_cast<uint8_t*>(std::aligned_alloc(64, CHUNK_SIZE));
            std::memset(data, 0, CHUNK_SIZE);
        }
        
        ~Chunk() {
            if (data) std::free(data);
        }
        
        Chunk(const Chunk&) = delete;
        Chunk& operator=(const Chunk&) = delete;
        
        Chunk(Chunk&& other) noexcept
            : data(other.data), entityCount(other.entityCount), capacity(other.capacity)
        {
            other.data = nullptr;
        }
    };
    
    Archetype(const std::vector<ComponentInfo>& components);
    ~Archetype() = default;
    
    size_t AddEntity(Entity entity);
    void RemoveEntity(size_t index);
    
    void* GetComponent(size_t entityIndex, ComponentTypeID typeID);
    const void* GetComponent(size_t entityIndex, ComponentTypeID typeID) const;
    
    template<typename T>
    T* GetComponent(size_t entityIndex) {
        return static_cast<T*>(GetComponent(entityIndex, ComponentRegistry::GetTypeID<T>()));
    }
    
    size_t GetEntityCount() const;
    size_t GetChunkCount() const { return m_Chunks.size(); }
    
    const std::vector<ComponentInfo>& GetComponentTypes() const { return m_ComponentTypes; }
    
    void IterateEntities(std::function<void(Entity, void**)> callback);
    
private:
    std::vector<ComponentInfo> m_ComponentTypes;
    std::vector<std::unique_ptr<Chunk>> m_Chunks;
    std::unordered_map<ComponentTypeID, size_t> m_ComponentIndexMap;
    
    size_t m_EntitiesPerChunk = 0;
    size_t CalculateEntitiesPerChunk();
    void AllocateNewChunk();
};

}

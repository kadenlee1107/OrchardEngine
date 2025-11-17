#include "Archetype.hpp"
#include <algorithm>

namespace Orchard::ECS {

Archetype::Archetype(const std::vector<ComponentInfo>& components)
    : m_ComponentTypes(components)
{
    std::sort(m_ComponentTypes.begin(), m_ComponentTypes.end(),
        [](const ComponentInfo& a, const ComponentInfo& b) {
            return a.alignment > b.alignment;
        });
    
    size_t offset = 0;
    for (size_t i = 0; i < m_ComponentTypes.size(); ++i) {
        offset = (offset + m_ComponentTypes[i].alignment - 1) & ~(m_ComponentTypes[i].alignment - 1);
        m_ComponentTypes[i].offsetInChunk = offset;
        m_ComponentIndexMap[m_ComponentTypes[i].typeID] = i;
        offset += m_ComponentTypes[i].size;
    }
    
    m_EntitiesPerChunk = CalculateEntitiesPerChunk();
}

size_t Archetype::CalculateEntitiesPerChunk() {
    size_t totalComponentSize = 0;
    for (const auto& info : m_ComponentTypes) {
        totalComponentSize += info.size;
    }
    
    if (totalComponentSize == 0) return 0;
    
    return CHUNK_SIZE / totalComponentSize;
}

void Archetype::AllocateNewChunk() {
    m_Chunks.push_back(std::make_unique<Chunk>(m_EntitiesPerChunk));
}

size_t Archetype::AddEntity(Entity entity) {
    if (m_Chunks.empty() || m_Chunks.back()->entityCount >= m_Chunks.back()->capacity) {
        AllocateNewChunk();
    }
    
    auto& chunk = m_Chunks.back();
    size_t index = chunk->entityCount++;
    
    return (m_Chunks.size() - 1) * m_EntitiesPerChunk + index;
}

void Archetype::RemoveEntity(size_t index) {
    size_t chunkIndex = index / m_EntitiesPerChunk;
    size_t entityIndex = index % m_EntitiesPerChunk;
    
    if (chunkIndex >= m_Chunks.size()) return;
    
    auto& chunk = m_Chunks[chunkIndex];
    if (entityIndex >= chunk->entityCount) return;
    
    size_t lastEntityIndex = chunk->entityCount - 1;
    if (entityIndex != lastEntityIndex) {
        for (const auto& info : m_ComponentTypes) {
            uint8_t* src = chunk->data + info.offsetInChunk + lastEntityIndex * info.size;
            uint8_t* dst = chunk->data + info.offsetInChunk + entityIndex * info.size;
            std::memcpy(dst, src, info.size);
        }
    }
    
    chunk->entityCount--;
}

void* Archetype::GetComponent(size_t entityIndex, ComponentTypeID typeID) {
    auto it = m_ComponentIndexMap.find(typeID);
    if (it == m_ComponentIndexMap.end()) return nullptr;
    
    size_t chunkIndex = entityIndex / m_EntitiesPerChunk;
    size_t indexInChunk = entityIndex % m_EntitiesPerChunk;
    
    if (chunkIndex >= m_Chunks.size()) return nullptr;
    
    auto& chunk = m_Chunks[chunkIndex];
    if (indexInChunk >= chunk->entityCount) return nullptr;
    
    const auto& info = m_ComponentTypes[it->second];
    return chunk->data + info.offsetInChunk + indexInChunk * info.size;
}

const void* Archetype::GetComponent(size_t entityIndex, ComponentTypeID typeID) const {
    return const_cast<Archetype*>(this)->GetComponent(entityIndex, typeID);
}

size_t Archetype::GetEntityCount() const {
    size_t count = 0;
    for (const auto& chunk : m_Chunks) {
        count += chunk->entityCount;
    }
    return count;
}

void Archetype::IterateEntities(std::function<void(Entity, void**)> callback) {
    std::vector<void*> components(m_ComponentTypes.size());
    
    for (const auto& chunk : m_Chunks) {
        for (size_t i = 0; i < chunk->entityCount; ++i) {
            for (size_t j = 0; j < m_ComponentTypes.size(); ++j) {
                const auto& info = m_ComponentTypes[j];
                components[j] = chunk->data + info.offsetInChunk + i * info.size;
            }
            
            callback(Entity(), components.data());
        }
    }
}

}

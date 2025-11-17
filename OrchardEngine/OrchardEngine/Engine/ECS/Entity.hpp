#pragma once

#include <cstdint>

namespace Orchard::ECS {

using EntityID = uint64_t;
using EntityVersion = uint32_t;

constexpr EntityID NULL_ENTITY = 0;

struct Entity {
    EntityID id;
    EntityVersion version;
    
    Entity() : id(NULL_ENTITY), version(0) {}
    Entity(EntityID id, EntityVersion version) : id(id), version(version) {}
    
    bool IsValid() const { return id != NULL_ENTITY; }
    
    bool operator==(const Entity& other) const {
        return id == other.id && version == other.version;
    }
    
    bool operator!=(const Entity& other) const {
        return !(*this == other);
    }
};

}

namespace std {
    template<>
    struct hash<Orchard::ECS::Entity> {
        size_t operator()(const Orchard::ECS::Entity& entity) const {
            return hash<uint64_t>()(entity.id) ^ (hash<uint32_t>()(entity.version) << 1);
        }
    };
}

#pragma once

#include <cstdint>
#include <typeindex>
#include <string>

namespace Orchard::ECS {

using ComponentTypeID = uint32_t;

class ComponentRegistry {
public:
    template<typename T>
    static ComponentTypeID GetTypeID() {
        static ComponentTypeID id = s_NextID++;
        return id;
    }
    
    template<typename T>
    static const char* GetTypeName() {
        return typeid(T).name();
    }
    
private:
    static inline ComponentTypeID s_NextID = 0;
};

template<typename T>
struct ComponentTrait {
    static constexpr bool is_component = true;
    static constexpr size_t size = sizeof(T);
    static constexpr size_t alignment = alignof(T);
};

}

#pragma once

#include <cstdint>
#include <string>
#include <functional>

namespace Orchard {

class UUID {
public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID&) = default;
    
    operator uint64_t() const { return m_UUID; }
    
    std::string ToString() const;
    
    bool operator==(const UUID& other) const { return m_UUID == other.m_UUID; }
    bool operator!=(const UUID& other) const { return m_UUID != other.m_UUID; }
    
private:
    uint64_t m_UUID;
};

}

namespace std {
    template<>
    struct hash<Orchard::UUID> {
        size_t operator()(const Orchard::UUID& uuid) const {
            return hash<uint64_t>()((uint64_t)uuid);
        }
    };
}

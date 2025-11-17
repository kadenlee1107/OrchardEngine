#include "Collider.hpp"

namespace Orchard::Physics {

AABB BoxCollider::GetAABB(const Math::Vector3& position, const Math::Quaternion& rotation) const {
    Math::Vector3 corners[8] = {
        Math::Vector3(-m_HalfExtents.x, -m_HalfExtents.y, -m_HalfExtents.z),
        Math::Vector3( m_HalfExtents.x, -m_HalfExtents.y, -m_HalfExtents.z),
        Math::Vector3(-m_HalfExtents.x,  m_HalfExtents.y, -m_HalfExtents.z),
        Math::Vector3( m_HalfExtents.x,  m_HalfExtents.y, -m_HalfExtents.z),
        Math::Vector3(-m_HalfExtents.x, -m_HalfExtents.y,  m_HalfExtents.z),
        Math::Vector3( m_HalfExtents.x, -m_HalfExtents.y,  m_HalfExtents.z),
        Math::Vector3(-m_HalfExtents.x,  m_HalfExtents.y,  m_HalfExtents.z),
        Math::Vector3( m_HalfExtents.x,  m_HalfExtents.y,  m_HalfExtents.z),
    };
    
    Math::Vector3 min = position;
    Math::Vector3 max = position;
    
    for (int i = 0; i < 8; ++i) {
        Math::Vector3 worldCorner = rotation * corners[i] + position;
        
        min.x = std::min(min.x, worldCorner.x);
        min.y = std::min(min.y, worldCorner.y);
        min.z = std::min(min.z, worldCorner.z);
        
        max.x = std::max(max.x, worldCorner.x);
        max.y = std::max(max.y, worldCorner.y);
        max.z = std::max(max.z, worldCorner.z);
    }
    
    return AABB(min, max);
}

Math::Vector3 BoxCollider::GetSupport(const Math::Vector3& direction,
                                      const Math::Vector3& position,
                                      const Math::Quaternion& rotation) const {
    Math::Vector3 localDir = rotation.Inverse() * direction;
    
    Math::Vector3 result(
        (localDir.x > 0) ? m_HalfExtents.x : -m_HalfExtents.x,
        (localDir.y > 0) ? m_HalfExtents.y : -m_HalfExtents.y,
        (localDir.z > 0) ? m_HalfExtents.z : -m_HalfExtents.z
    );
    
    return rotation * result + position;
}

AABB CapsuleCollider::GetAABB(const Math::Vector3& position, const Math::Quaternion& rotation) const {
    Math::Vector3 up = rotation * Math::Vector3(0, 1, 0);
    Math::Vector3 halfHeight = up * (m_Height * 0.5f);
    
    Math::Vector3 p1 = position + halfHeight;
    Math::Vector3 p2 = position - halfHeight;
    
    Math::Vector3 extent(m_Radius, m_Radius, m_Radius);
    
    Math::Vector3 min = Math::Vector3(
        std::min(p1.x, p2.x) - m_Radius,
        std::min(p1.y, p2.y) - m_Radius,
        std::min(p1.z, p2.z) - m_Radius
    );
    
    Math::Vector3 max = Math::Vector3(
        std::max(p1.x, p2.x) + m_Radius,
        std::max(p1.y, p2.y) + m_Radius,
        std::max(p1.z, p2.z) + m_Radius
    );
    
    return AABB(min, max);
}

Math::Vector3 CapsuleCollider::GetSupport(const Math::Vector3& direction,
                                          const Math::Vector3& position,
                                          const Math::Quaternion& rotation) const {
    Math::Vector3 up = rotation * Math::Vector3(0, 1, 0);
    Math::Vector3 halfHeight = up * (m_Height * 0.5f);
    
    Math::Vector3 center = position;
    if (Math::Vector3::Dot(direction, up) > 0) {
        center += halfHeight;
    } else {
        center -= halfHeight;
    }
    
    return center + direction.Normalized() * m_Radius;
}

}

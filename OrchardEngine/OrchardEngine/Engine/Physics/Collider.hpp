#pragma once

#include "../Math/Vector.hpp"
#include "../Math/Quaternion.hpp"

namespace Orchard::Physics {

enum class ColliderType {
    Sphere,
    Box,
    Capsule,
    Mesh,
    Convex
};

struct AABB {
    Math::Vector3 min;
    Math::Vector3 max;
    
    AABB() : min(0, 0, 0), max(0, 0, 0) {}
    AABB(const Math::Vector3& min, const Math::Vector3& max) : min(min), max(max) {}
    
    Math::Vector3 GetCenter() const { return (min + max) * 0.5f; }
    Math::Vector3 GetExtents() const { return (max - min) * 0.5f; }
    
    bool Intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }
};

class Collider {
public:
    virtual ~Collider() = default;
    
    virtual ColliderType GetType() const = 0;
    virtual AABB GetAABB(const Math::Vector3& position, const Math::Quaternion& rotation) const = 0;
    
    virtual Math::Vector3 GetSupport(const Math::Vector3& direction,
                                     const Math::Vector3& position,
                                     const Math::Quaternion& rotation) const = 0;
    
    void SetIsTrigger(bool trigger) { m_IsTrigger = trigger; }
    bool IsTrigger() const { return m_IsTrigger; }
    
    void SetMaterial(float friction, float restitution) {
        m_Friction = friction;
        m_Restitution = restitution;
    }
    
    float GetFriction() const { return m_Friction; }
    float GetRestitution() const { return m_Restitution; }
    
protected:
    bool m_IsTrigger = false;
    float m_Friction = 0.5f;
    float m_Restitution = 0.3f;
};

class SphereCollider : public Collider {
public:
    SphereCollider(float radius) : m_Radius(radius) {}
    
    ColliderType GetType() const override { return ColliderType::Sphere; }
    
    AABB GetAABB(const Math::Vector3& position, const Math::Quaternion& rotation) const override {
        Math::Vector3 extent(m_Radius, m_Radius, m_Radius);
        return AABB(position - extent, position + extent);
    }
    
    Math::Vector3 GetSupport(const Math::Vector3& direction,
                            const Math::Vector3& position,
                            const Math::Quaternion& rotation) const override {
        return position + direction.Normalized() * m_Radius;
    }
    
    float GetRadius() const { return m_Radius; }
    
private:
    float m_Radius;
};

class BoxCollider : public Collider {
public:
    BoxCollider(const Math::Vector3& halfExtents) : m_HalfExtents(halfExtents) {}
    
    ColliderType GetType() const override { return ColliderType::Box; }
    
    AABB GetAABB(const Math::Vector3& position, const Math::Quaternion& rotation) const override;
    
    Math::Vector3 GetSupport(const Math::Vector3& direction,
                            const Math::Vector3& position,
                            const Math::Quaternion& rotation) const override;
    
    const Math::Vector3& GetHalfExtents() const { return m_HalfExtents; }
    
private:
    Math::Vector3 m_HalfExtents;
};

class CapsuleCollider : public Collider {
public:
    CapsuleCollider(float radius, float height) : m_Radius(radius), m_Height(height) {}
    
    ColliderType GetType() const override { return ColliderType::Capsule; }
    
    AABB GetAABB(const Math::Vector3& position, const Math::Quaternion& rotation) const override;
    
    Math::Vector3 GetSupport(const Math::Vector3& direction,
                            const Math::Vector3& position,
                            const Math::Quaternion& rotation) const override;
    
    float GetRadius() const { return m_Radius; }
    float GetHeight() const { return m_Height; }
    
private:
    float m_Radius;
    float m_Height;
};

}

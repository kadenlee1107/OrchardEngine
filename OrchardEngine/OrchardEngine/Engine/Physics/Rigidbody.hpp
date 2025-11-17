#pragma once

#include "../Math/Vector.hpp"
#include "../Math/Quaternion.hpp"
#include "../Math/Matrix.hpp"
#include "Collider.hpp"
#include <memory>

namespace Orchard::Physics {

enum class MotionType {
    Static,
    Kinematic,
    Dynamic
};

class Rigidbody {
public:
    Rigidbody();
    ~Rigidbody();
    
    void SetPosition(const Math::Vector3& position) { m_Position = position; }
    const Math::Vector3& GetPosition() const { return m_Position; }
    
    void SetRotation(const Math::Quaternion& rotation) { m_Rotation = rotation; }
    const Math::Quaternion& GetRotation() const { return m_Rotation; }
    
    void SetLinearVelocity(const Math::Vector3& velocity) { m_LinearVelocity = velocity; }
    const Math::Vector3& GetLinearVelocity() const { return m_LinearVelocity; }
    
    void SetAngularVelocity(const Math::Vector3& velocity) { m_AngularVelocity = velocity; }
    const Math::Vector3& GetAngularVelocity() const { return m_AngularVelocity; }
    
    void SetMass(float mass);
    float GetMass() const { return m_Mass; }
    float GetInverseMass() const { return m_InverseMass; }
    
    void SetInertia(const Math::Vector3& inertia);
    const Math::Vector3& GetInertia() const { return m_Inertia; }
    Math::Vector3 GetInverseInertia() const { return m_InverseInertia; }
    
    Math::Matrix4 GetInverseInertiaTensor() const;
    
    void SetMotionType(MotionType type) { m_MotionType = type; }
    MotionType GetMotionType() const { return m_MotionType; }
    
    void SetGravityScale(float scale) { m_GravityScale = scale; }
    float GetGravityScale() const { return m_GravityScale; }
    
    void SetLinearDamping(float damping) { m_LinearDamping = damping; }
    float GetLinearDamping() const { return m_LinearDamping; }
    
    void SetAngularDamping(float damping) { m_AngularDamping = damping; }
    float GetAngularDamping() const { return m_AngularDamping; }
    
    void AddForce(const Math::Vector3& force);
    void AddTorque(const Math::Vector3& torque);
    void AddForceAtPoint(const Math::Vector3& force, const Math::Vector3& point);
    
    void ClearForces();
    
    void SetCollider(std::unique_ptr<Collider> collider);
    Collider* GetCollider() const { return m_Collider.get(); }
    
    void Integrate(float deltaTime);
    
private:
    Math::Vector3 m_Position{0, 0, 0};
    Math::Quaternion m_Rotation = Math::Quaternion::Identity();
    
    Math::Vector3 m_LinearVelocity{0, 0, 0};
    Math::Vector3 m_AngularVelocity{0, 0, 0};
    
    Math::Vector3 m_Force{0, 0, 0};
    Math::Vector3 m_Torque{0, 0, 0};
    
    float m_Mass = 1.0f;
    float m_InverseMass = 1.0f;
    
    Math::Vector3 m_Inertia{1, 1, 1};
    Math::Vector3 m_InverseInertia{1, 1, 1};
    
    MotionType m_MotionType = MotionType::Dynamic;
    
    float m_GravityScale = 1.0f;
    float m_LinearDamping = 0.01f;
    float m_AngularDamping = 0.05f;
    
    std::unique_ptr<Collider> m_Collider;
};

}

#include "Rigidbody.hpp"

namespace Orchard::Physics {

Rigidbody::Rigidbody() {
}

Rigidbody::~Rigidbody() {
}

void Rigidbody::SetMass(float mass) {
    m_Mass = mass;
    m_InverseMass = (mass > 0.0f) ? (1.0f / mass) : 0.0f;
}

void Rigidbody::SetInertia(const Math::Vector3& inertia) {
    m_Inertia = inertia;
    m_InverseInertia = Math::Vector3(
        (inertia.x > 0.0f) ? (1.0f / inertia.x) : 0.0f,
        (inertia.y > 0.0f) ? (1.0f / inertia.y) : 0.0f,
        (inertia.z > 0.0f) ? (1.0f / inertia.z) : 0.0f
    );
}

Math::Matrix4 Rigidbody::GetInverseInertiaTensor() const {
    Math::Matrix4 rotation = Math::Matrix4::Rotation(m_Rotation);
    Math::Matrix4 rotationT = rotation.Transposed();
    
    Math::Matrix4 inertiaTensor = Math::Matrix4::Scale(m_InverseInertia);
    
    return rotation * inertiaTensor * rotationT;
}

void Rigidbody::AddForce(const Math::Vector3& force) {
    m_Force += force;
}

void Rigidbody::AddTorque(const Math::Vector3& torque) {
    m_Torque += torque;
}

void Rigidbody::AddForceAtPoint(const Math::Vector3& force, const Math::Vector3& point) {
    AddForce(force);
    
    Math::Vector3 r = point - m_Position;
    AddTorque(Math::Vector3::Cross(r, force));
}

void Rigidbody::ClearForces() {
    m_Force = Math::Vector3(0, 0, 0);
    m_Torque = Math::Vector3(0, 0, 0);
}

void Rigidbody::Integrate(float deltaTime) {
    if (m_MotionType != MotionType::Dynamic) return;
    
    Math::Vector3 acceleration = m_Force * m_InverseMass;
    m_LinearVelocity += acceleration * deltaTime;
    
    m_LinearVelocity *= (1.0f - m_LinearDamping * deltaTime);
    
    m_Position += m_LinearVelocity * deltaTime;
    
    Math::Vector3 angularAcceleration = m_Torque;
    angularAcceleration.x *= m_InverseInertia.x;
    angularAcceleration.y *= m_InverseInertia.y;
    angularAcceleration.z *= m_InverseInertia.z;
    
    m_AngularVelocity += angularAcceleration * deltaTime;
    
    m_AngularVelocity *= (1.0f - m_AngularDamping * deltaTime);
    
    if (m_AngularVelocity.LengthSquared() > 0.0001f) {
        float angle = m_AngularVelocity.Length() * deltaTime;
        Math::Vector3 axis = m_AngularVelocity.Normalized();
        Math::Quaternion deltaRotation = Math::Quaternion::FromAxisAngle(axis, angle);
        m_Rotation = deltaRotation * m_Rotation;
        m_Rotation = m_Rotation.Normalized();
    }
    
    ClearForces();
}

void Rigidbody::SetCollider(std::unique_ptr<Collider> collider) {
    m_Collider = std::move(collider);
}

}

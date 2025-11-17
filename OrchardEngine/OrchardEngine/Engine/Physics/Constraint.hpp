#pragma once

#include "../Math/Vector.hpp"
#include "Rigidbody.hpp"

namespace Orchard::Physics {

class Constraint {
public:
    virtual ~Constraint() = default;
    
    virtual void Prepare(float deltaTime) = 0;
    virtual void Solve(float deltaTime) = 0;
    
    void SetBodies(Rigidbody* bodyA, Rigidbody* bodyB) {
        m_BodyA = bodyA;
        m_BodyB = bodyB;
    }
    
protected:
    Rigidbody* m_BodyA = nullptr;
    Rigidbody* m_BodyB = nullptr;
};

class DistanceConstraint : public Constraint {
public:
    DistanceConstraint(float distance) : m_Distance(distance) {}
    
    void Prepare(float deltaTime) override;
    void Solve(float deltaTime) override;
    
private:
    float m_Distance;
    Math::Vector3 m_LocalAnchorA;
    Math::Vector3 m_LocalAnchorB;
};

class HingeConstraint : public Constraint {
public:
    HingeConstraint(const Math::Vector3& axis) : m_Axis(axis) {}
    
    void Prepare(float deltaTime) override;
    void Solve(float deltaTime) override;
    
    void SetLimits(float minAngle, float maxAngle) {
        m_MinAngle = minAngle;
        m_MaxAngle = maxAngle;
        m_UseLimits = true;
    }
    
private:
    Math::Vector3 m_Axis;
    Math::Vector3 m_LocalAnchorA;
    Math::Vector3 m_LocalAnchorB;
    
    bool m_UseLimits = false;
    float m_MinAngle = 0.0f;
    float m_MaxAngle = 0.0f;
};

class SliderConstraint : public Constraint {
public:
    SliderConstraint(const Math::Vector3& axis) : m_Axis(axis) {}
    
    void Prepare(float deltaTime) override;
    void Solve(float deltaTime) override;
    
    void SetLimits(float min, float max) {
        m_MinDistance = min;
        m_MaxDistance = max;
        m_UseLimits = true;
    }
    
private:
    Math::Vector3 m_Axis;
    
    bool m_UseLimits = false;
    float m_MinDistance = 0.0f;
    float m_MaxDistance = 0.0f;
};

class ConstraintSolver {
public:
    void SolveConstraints(const std::vector<Constraint*>& constraints, float deltaTime);
    void SolveContacts(const std::vector<CollisionInfo>& collisions, float deltaTime);
    
private:
    void SolveContact(const ContactPoint& contact, Rigidbody* bodyA, Rigidbody* bodyB, float deltaTime);
};

}

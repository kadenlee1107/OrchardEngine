#pragma once

#include "../Math/Vector.hpp"
#include "../Math/Quaternion.hpp"
#include <vector>
#include <memory>

namespace Orchard::Physics {

struct RaycastHit;
class Rigidbody;
class Collider;
class Constraint;

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();
    
    bool Initialize();
    void Shutdown();
    
    void Step(double deltaTime);
    
    void AddRigidbody(Rigidbody* rb);
    void RemoveRigidbody(Rigidbody* rb);
    
    void AddConstraint(Constraint* constraint);
    void RemoveConstraint(Constraint* constraint);
    
    bool Raycast(const Math::Vector3& origin, const Math::Vector3& direction,
                 float maxDistance, RaycastHit* hit = nullptr);
    
    void SetGravity(const Math::Vector3& gravity) { m_Gravity = gravity; }
    const Math::Vector3& GetGravity() const { return m_Gravity; }
    
private:
    void Integrate(double deltaTime);
    void DetectCollisions();
    void SolveConstraints(double deltaTime);
    void SolveContacts(double deltaTime);
    
    std::vector<Rigidbody*> m_Rigidbodies;
    std::vector<Constraint*> m_Constraints;
    
    Math::Vector3 m_Gravity{0, -9.81f, 0};
    
    class BroadPhase;
    class NarrowPhase;
    class ConstraintSolver;
    
    std::unique_ptr<BroadPhase> m_BroadPhase;
    std::unique_ptr<NarrowPhase> m_NarrowPhase;
    std::unique_ptr<ConstraintSolver> m_ConstraintSolver;
};

}

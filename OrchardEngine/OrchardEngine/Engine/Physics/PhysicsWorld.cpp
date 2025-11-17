#include "PhysicsWorld.hpp"
#include "CollisionDetection.hpp"
#include <iostream>

namespace Orchard::Physics {

PhysicsWorld::PhysicsWorld() {
}

PhysicsWorld::~PhysicsWorld() {
}

bool PhysicsWorld::Initialize() {
    m_BroadPhase = std::make_unique<SweepAndPrune>();
    m_NarrowPhase = std::make_unique<NarrowPhase>();
    m_ConstraintSolver = std::make_unique<ConstraintSolver>();
    
    std::cout << "Physics World initialized" << std::endl;
    return true;
}

void PhysicsWorld::Shutdown() {
    m_Rigidbodies.clear();
    m_Constraints.clear();
    m_BroadPhase.reset();
    m_NarrowPhase.reset();
    m_ConstraintSolver.reset();
    
    std::cout << "Physics World shut down" << std::endl;
}

void PhysicsWorld::Step(double deltaTime) {
    Integrate(deltaTime);
    DetectCollisions();
    SolveConstraints(deltaTime);
    SolveContacts(deltaTime);
}

void PhysicsWorld::Integrate(double deltaTime) {
    float dt = static_cast<float>(deltaTime);
    
    for (Rigidbody* rb : m_Rigidbodies) {
        if (rb->GetMotionType() == MotionType::Dynamic) {
            rb->AddForce(m_Gravity * rb->GetMass() * rb->GetGravityScale());
            rb->Integrate(dt);
        }
    }
}

void PhysicsWorld::DetectCollisions() {
    m_BroadPhase->Update(m_Rigidbodies);
    auto pairs = m_BroadPhase->GetPotentialCollisions();
    
    auto collisions = m_NarrowPhase->DetectCollisions(pairs);
}

void PhysicsWorld::SolveConstraints(double deltaTime) {
    m_ConstraintSolver->SolveConstraints(m_Constraints, static_cast<float>(deltaTime));
}

void PhysicsWorld::SolveContacts(double deltaTime) {
}

void PhysicsWorld::AddRigidbody(Rigidbody* rb) {
    m_Rigidbodies.push_back(rb);
}

void PhysicsWorld::RemoveRigidbody(Rigidbody* rb) {
    m_Rigidbodies.erase(
        std::remove(m_Rigidbodies.begin(), m_Rigidbodies.end(), rb),
        m_Rigidbodies.end()
    );
}

void PhysicsWorld::AddConstraint(Constraint* constraint) {
    m_Constraints.push_back(constraint);
}

void PhysicsWorld::RemoveConstraint(Constraint* constraint) {
    m_Constraints.erase(
        std::remove(m_Constraints.begin(), m_Constraints.end(), constraint),
        m_Constraints.end()
    );
}

bool PhysicsWorld::Raycast(const Math::Vector3& origin, const Math::Vector3& direction,
                           float maxDistance, RaycastHit* hit) {
    return false;
}

}

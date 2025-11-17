#include "CollisionDetection.hpp"
#include <algorithm>
#include <cmath>

namespace Orchard::Physics {

Math::Vector3 GJK::Support(const Collider* colliderA, const Math::Vector3& posA, const Math::Quaternion& rotA,
                           const Collider* colliderB, const Math::Vector3& posB, const Math::Quaternion& rotB,
                           const Math::Vector3& direction) {
    Math::Vector3 supportA = colliderA->GetSupport(direction, posA, rotA);
    Math::Vector3 supportB = colliderB->GetSupport(direction * -1.0f, posB, rotB);
    return supportA - supportB;
}

bool GJK::Intersects(const Collider* colliderA, const Math::Vector3& posA, const Math::Quaternion& rotA,
                     const Collider* colliderB, const Math::Vector3& posB, const Math::Quaternion& rotB,
                     Math::Vector3* penetrationVector) {
    Simplex simplex;
    Math::Vector3 direction = posB - posA;
    
    if (direction.LengthSquared() < 0.0001f) {
        direction = Math::Vector3(1, 0, 0);
    }
    
    Math::Vector3 support = Support(colliderA, posA, rotA, colliderB, posB, rotB, direction);
    simplex.Add(support);
    direction = support * -1.0f;
    
    const int maxIterations = 32;
    for (int i = 0; i < maxIterations; ++i) {
        support = Support(colliderA, posA, rotA, colliderB, posB, rotB, direction);
        
        if (Math::Vector3::Dot(support, direction) <= 0) {
            return false;
        }
        
        simplex.Add(support);
        
        if (UpdateSimplex(simplex, direction)) {
            return true;
        }
    }
    
    return false;
}

bool GJK::UpdateSimplex(Simplex& simplex, Math::Vector3& direction) {
    switch (simplex.count) {
        case 2: return Line(simplex, direction);
        case 3: return Triangle(simplex, direction);
        case 4: return Tetrahedron(simplex, direction);
    }
    return false;
}

bool GJK::Line(Simplex& simplex, Math::Vector3& direction) {
    Math::Vector3 a = simplex[1];
    Math::Vector3 b = simplex[0];
    
    Math::Vector3 ab = b - a;
    Math::Vector3 ao = a * -1.0f;
    
    if (Math::Vector3::Dot(ab, ao) > 0) {
        direction = Math::Vector3::Cross(Math::Vector3::Cross(ab, ao), ab);
    } else {
        simplex.count = 1;
        simplex[0] = a;
        direction = ao;
    }
    
    return false;
}

bool GJK::Triangle(Simplex& simplex, Math::Vector3& direction) {
    Math::Vector3 a = simplex[2];
    Math::Vector3 b = simplex[1];
    Math::Vector3 c = simplex[0];
    
    Math::Vector3 ab = b - a;
    Math::Vector3 ac = c - a;
    Math::Vector3 ao = a * -1.0f;
    
    Math::Vector3 abc = Math::Vector3::Cross(ab, ac);
    
    if (Math::Vector3::Dot(Math::Vector3::Cross(abc, ac), ao) > 0) {
        if (Math::Vector3::Dot(ac, ao) > 0) {
            simplex.count = 2;
            simplex[0] = c;
            simplex[1] = a;
            direction = Math::Vector3::Cross(Math::Vector3::Cross(ac, ao), ac);
        } else {
            simplex.count = 2;
            simplex[0] = b;
            simplex[1] = a;
            return Line(simplex, direction);
        }
    } else {
        if (Math::Vector3::Dot(Math::Vector3::Cross(ab, abc), ao) > 0) {
            simplex.count = 2;
            simplex[0] = b;
            simplex[1] = a;
            return Line(simplex, direction);
        } else {
            if (Math::Vector3::Dot(abc, ao) > 0) {
                direction = abc;
            } else {
                simplex[0] = b;
                simplex[1] = c;
                direction = abc * -1.0f;
            }
        }
    }
    
    return false;
}

bool GJK::Tetrahedron(Simplex& simplex, Math::Vector3& direction) {
    Math::Vector3 a = simplex[3];
    Math::Vector3 b = simplex[2];
    Math::Vector3 c = simplex[1];
    Math::Vector3 d = simplex[0];
    
    Math::Vector3 ab = b - a;
    Math::Vector3 ac = c - a;
    Math::Vector3 ad = d - a;
    Math::Vector3 ao = a * -1.0f;
    
    Math::Vector3 abc = Math::Vector3::Cross(ab, ac);
    Math::Vector3 acd = Math::Vector3::Cross(ac, ad);
    Math::Vector3 adb = Math::Vector3::Cross(ad, ab);
    
    if (Math::Vector3::Dot(abc, ao) > 0) {
        simplex.count = 3;
        simplex[0] = c;
        simplex[1] = b;
        simplex[2] = a;
        return Triangle(simplex, direction);
    }
    
    if (Math::Vector3::Dot(acd, ao) > 0) {
        simplex.count = 3;
        simplex[0] = d;
        simplex[1] = c;
        simplex[2] = a;
        return Triangle(simplex, direction);
    }
    
    if (Math::Vector3::Dot(adb, ao) > 0) {
        simplex.count = 3;
        simplex[0] = b;
        simplex[1] = d;
        simplex[2] = a;
        return Triangle(simplex, direction);
    }
    
    return true;
}

void SweepAndPrune::Update(const std::vector<Rigidbody*>& rigidbodies) {
    m_Endpoints.clear();
    m_Pairs.clear();
    
    for (Rigidbody* rb : rigidbodies) {
        if (!rb->GetCollider()) continue;
        
        AABB aabb = rb->GetCollider()->GetAABB(rb->GetPosition(), rb->GetRotation());
        
        m_Endpoints.push_back({aabb.min.x, rb, true});
        m_Endpoints.push_back({aabb.max.x, rb, false});
    }
    
    std::sort(m_Endpoints.begin(), m_Endpoints.end());
    
    std::vector<Rigidbody*> active;
    for (const auto& endpoint : m_Endpoints) {
        if (endpoint.isMin) {
            for (Rigidbody* other : active) {
                m_Pairs.push_back({endpoint.body, other});
            }
            active.push_back(endpoint.body);
        } else {
            active.erase(std::remove(active.begin(), active.end(), endpoint.body), active.end());
        }
    }
}

std::vector<std::pair<Rigidbody*, Rigidbody*>> SweepAndPrune::GetPotentialCollisions() {
    return m_Pairs;
}

std::vector<CollisionInfo> NarrowPhase::DetectCollisions(
    const std::vector<std::pair<Rigidbody*, Rigidbody*>>& pairs) {
    
    std::vector<CollisionInfo> collisions;
    
    for (const auto& [bodyA, bodyB] : pairs) {
        CollisionInfo info = TestCollision(bodyA, bodyB);
        if (info.hasCollision) {
            collisions.push_back(info);
        }
    }
    
    return collisions;
}

CollisionInfo NarrowPhase::TestCollision(Rigidbody* bodyA, Rigidbody* bodyB) {
    CollisionInfo info;
    info.bodyA = bodyA;
    info.bodyB = bodyB;
    
    if (!bodyA->GetCollider() || !bodyB->GetCollider()) {
        return info;
    }
    
    info.hasCollision = GJK::Intersects(
        bodyA->GetCollider(), bodyA->GetPosition(), bodyA->GetRotation(),
        bodyB->GetCollider(), bodyB->GetPosition(), bodyB->GetRotation()
    );
    
    if (info.hasCollision) {
        GenerateContactPoints(info);
    }
    
    return info;
}

void NarrowPhase::GenerateContactPoints(CollisionInfo& info) {
}

void ConstraintSolver::SolveConstraints(const std::vector<Constraint*>& constraints, float deltaTime) {
    for (Constraint* constraint : constraints) {
        constraint->Prepare(deltaTime);
    }
    
    const int iterations = 10;
    for (int i = 0; i < iterations; ++i) {
        for (Constraint* constraint : constraints) {
            constraint->Solve(deltaTime);
        }
    }
}

void ConstraintSolver::SolveContacts(const std::vector<CollisionInfo>& collisions, float deltaTime) {
    for (const auto& collision : collisions) {
        for (const auto& contact : collision.contacts) {
            SolveContact(contact, collision.bodyA, collision.bodyB, deltaTime);
        }
    }
}

void ConstraintSolver::SolveContact(const ContactPoint& contact, Rigidbody* bodyA, Rigidbody* bodyB, float deltaTime) {
}

EPA::Face::Face(const Math::Vector3& a, const Math::Vector3& b, const Math::Vector3& c)
    : a(a), b(b), c(c) {
    normal = Math::Vector3::Cross(b - a, c - a).Normalized();
    distance = Math::Vector3::Dot(normal, a);
}

bool EPA::GetPenetrationDepth(const Collider* colliderA, const Math::Vector3& posA, const Math::Quaternion& rotA,
                              const Collider* colliderB, const Math::Vector3& posB, const Math::Quaternion& rotB,
                              const GJK::Simplex& simplex,
                              Math::Vector3& normal, float& depth) {
    return false;
}

void DistanceConstraint::Prepare(float deltaTime) {
}

void DistanceConstraint::Solve(float deltaTime) {
}

void HingeConstraint::Prepare(float deltaTime) {
}

void HingeConstraint::Solve(float deltaTime) {
}

void SliderConstraint::Prepare(float deltaTime) {
}

void SliderConstraint::Solve(float deltaTime) {
}

}

#pragma once

#include "../Math/Vector.hpp"
#include "Collider.hpp"
#include <vector>

namespace Orchard::Physics {

struct ContactPoint {
    Math::Vector3 pointA;
    Math::Vector3 pointB;
    Math::Vector3 normal;
    float penetration;
};

struct CollisionInfo {
    Rigidbody* bodyA;
    Rigidbody* bodyB;
    std::vector<ContactPoint> contacts;
    bool hasCollision = false;
};

class GJK {
public:
    static bool Intersects(const Collider* colliderA, const Math::Vector3& posA, const Math::Quaternion& rotA,
                          const Collider* colliderB, const Math::Vector3& posB, const Math::Quaternion& rotB,
                          Math::Vector3* penetrationVector = nullptr);
    
private:
    struct Simplex {
        Math::Vector3 points[4];
        int count = 0;
        
        void Add(const Math::Vector3& point) {
            points[count++] = point;
        }
        
        Math::Vector3& operator[](int index) { return points[index]; }
        const Math::Vector3& operator[](int index) const { return points[index]; }
    };
    
    static Math::Vector3 Support(const Collider* colliderA, const Math::Vector3& posA, const Math::Quaternion& rotA,
                                const Collider* colliderB, const Math::Vector3& posB, const Math::Quaternion& rotB,
                                const Math::Vector3& direction);
    
    static bool UpdateSimplex(Simplex& simplex, Math::Vector3& direction);
    static bool Line(Simplex& simplex, Math::Vector3& direction);
    static bool Triangle(Simplex& simplex, Math::Vector3& direction);
    static bool Tetrahedron(Simplex& simplex, Math::Vector3& direction);
};

class EPA {
public:
    static bool GetPenetrationDepth(const Collider* colliderA, const Math::Vector3& posA, const Math::Quaternion& rotA,
                                   const Collider* colliderB, const Math::Vector3& posB, const Math::Quaternion& rotB,
                                   const GJK::Simplex& simplex,
                                   Math::Vector3& normal, float& depth);
    
private:
    struct Face {
        Math::Vector3 a, b, c;
        Math::Vector3 normal;
        float distance;
        
        Face(const Math::Vector3& a, const Math::Vector3& b, const Math::Vector3& c);
    };
};

class BroadPhase {
public:
    virtual ~BroadPhase() = default;
    
    virtual void Update(const std::vector<Rigidbody*>& rigidbodies) = 0;
    virtual std::vector<std::pair<Rigidbody*, Rigidbody*>> GetPotentialCollisions() = 0;
};

class SweepAndPrune : public BroadPhase {
public:
    void Update(const std::vector<Rigidbody*>& rigidbodies) override;
    std::vector<std::pair<Rigidbody*, Rigidbody*>> GetPotentialCollisions() override;
    
private:
    struct Endpoint {
        float value;
        Rigidbody* body;
        bool isMin;
        
        bool operator<(const Endpoint& other) const {
            return value < other.value;
        }
    };
    
    std::vector<Endpoint> m_Endpoints;
    std::vector<std::pair<Rigidbody*, Rigidbody*>> m_Pairs;
};

class NarrowPhase {
public:
    std::vector<CollisionInfo> DetectCollisions(
        const std::vector<std::pair<Rigidbody*, Rigidbody*>>& pairs);
    
private:
    CollisionInfo TestCollision(Rigidbody* bodyA, Rigidbody* bodyB);
    void GenerateContactPoints(CollisionInfo& info);
};

}

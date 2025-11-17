#pragma once

#include "../../Math/Transform.hpp"

namespace Orchard::ECS {

struct TransformComponent {
    Math::Transform transform;
    Entity parent;
    std::vector<Entity> children;
    
    TransformComponent()
        : parent()
    {}
    
    const Math::Vector3& GetPosition() const { return transform.GetPosition(); }
    const Math::Quaternion& GetRotation() const { return transform.GetRotation(); }
    const Math::Vector3& GetScale() const { return transform.GetScale(); }
    
    void SetPosition(const Math::Vector3& pos) { transform.SetPosition(pos); }
    void SetRotation(const Math::Quaternion& rot) { transform.SetRotation(rot); }
    void SetScale(const Math::Vector3& scale) { transform.SetScale(scale); }
    
    Math::Matrix4 GetWorldMatrix() const {
        return transform.GetMatrix();
    }
};

struct MeshRendererComponent {
    UUID meshID;
    UUID materialID;
    bool castShadows = true;
    bool receiveShadows = true;
};

struct CameraComponent {
    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    bool isPrimary = false;
    
    Math::Matrix4 GetProjectionMatrix(float aspectRatio) const {
        return Math::Matrix4::Perspective(
            fov * 3.14159f / 180.0f,
            aspectRatio,
            nearPlane,
            farPlane
        );
    }
};

struct LightComponent {
    enum class Type {
        Directional,
        Point,
        Spot
    };
    
    Type type = Type::Directional;
    Math::Vector3 color = Math::Vector3(1, 1, 1);
    float intensity = 1.0f;
    float range = 10.0f;
    float spotAngle = 45.0f;
    bool castShadows = true;
};

}

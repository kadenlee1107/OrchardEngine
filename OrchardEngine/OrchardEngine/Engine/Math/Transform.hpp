#pragma once

#include "Vector.hpp"
#include "Quaternion.hpp"
#include "Matrix.hpp"

namespace Orchard::Math {

class Transform {
public:
    Transform()
        : m_Position(0, 0, 0)
        , m_Rotation(Quaternion::Identity())
        , m_Scale(1, 1, 1)
        , m_MatrixDirty(true)
    {}
    
    const Vector3& GetPosition() const { return m_Position; }
    const Quaternion& GetRotation() const { return m_Rotation; }
    const Vector3& GetScale() const { return m_Scale; }
    
    void SetPosition(const Vector3& position) {
        m_Position = position;
        m_MatrixDirty = true;
    }
    
    void SetRotation(const Quaternion& rotation) {
        m_Rotation = rotation;
        m_MatrixDirty = true;
    }
    
    void SetScale(const Vector3& scale) {
        m_Scale = scale;
        m_MatrixDirty = true;
    }
    
    void Translate(const Vector3& translation) {
        m_Position += translation;
        m_MatrixDirty = true;
    }
    
    void Rotate(const Quaternion& rotation) {
        m_Rotation = rotation * m_Rotation;
        m_MatrixDirty = true;
    }
    
    Vector3 Forward() const {
        return m_Rotation * Vector3(0, 0, 1);
    }
    
    Vector3 Right() const {
        return m_Rotation * Vector3(1, 0, 0);
    }
    
    Vector3 Up() const {
        return m_Rotation * Vector3(0, 1, 0);
    }
    
    const Matrix4& GetMatrix() const {
        if (m_MatrixDirty) {
            m_Matrix = Matrix4::TRS(m_Position, m_Rotation, m_Scale);
            m_MatrixDirty = false;
        }
        return m_Matrix;
    }
    
private:
    Vector3 m_Position;
    Quaternion m_Rotation;
    Vector3 m_Scale;
    
    mutable Matrix4 m_Matrix;
    mutable bool m_MatrixDirty;
};

}

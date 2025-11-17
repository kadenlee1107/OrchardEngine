#pragma once

#include "Vector.hpp"
#include "Quaternion.hpp"
#include <arm_neon.h>

namespace Orchard::Math {

struct alignas(16) Matrix4 {
    union {
        float m[16];
        float m2[4][4];
        float32x4_t rows[4];
    };
    
    Matrix4() {
        Identity();
    }
    
    Matrix4(float diagonal) {
        for (int i = 0; i < 16; ++i) m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = diagonal;
    }
    
    void Identity() {
        for (int i = 0; i < 16; ++i) m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }
    
    static Matrix4 Translation(const Vector3& translation) {
        Matrix4 result(1.0f);
        result.m[12] = translation.x;
        result.m[13] = translation.y;
        result.m[14] = translation.z;
        return result;
    }
    
    static Matrix4 Rotation(const Quaternion& rotation);
    
    static Matrix4 Scale(const Vector3& scale) {
        Matrix4 result;
        result.m[0] = scale.x;
        result.m[5] = scale.y;
        result.m[10] = scale.z;
        result.m[15] = 1.0f;
        return result;
    }
    
    static Matrix4 TRS(const Vector3& translation, const Quaternion& rotation, const Vector3& scale);
    
    static Matrix4 Perspective(float fov, float aspect, float near, float far) {
        Matrix4 result;
        for (int i = 0; i < 16; ++i) result.m[i] = 0.0f;
        
        float tanHalfFov = std::tan(fov * 0.5f);
        
        result.m[0] = 1.0f / (aspect * tanHalfFov);
        result.m[5] = 1.0f / tanHalfFov;
        result.m[10] = far / (far - near);
        result.m[11] = 1.0f;
        result.m[14] = -(far * near) / (far - near);
        
        return result;
    }
    
    static Matrix4 Orthographic(float left, float right, float bottom, float top, float near, float far) {
        Matrix4 result;
        for (int i = 0; i < 16; ++i) result.m[i] = 0.0f;
        
        result.m[0] = 2.0f / (right - left);
        result.m[5] = 2.0f / (top - bottom);
        result.m[10] = 1.0f / (far - near);
        result.m[12] = -(right + left) / (right - left);
        result.m[13] = -(top + bottom) / (top - bottom);
        result.m[14] = -near / (far - near);
        result.m[15] = 1.0f;
        
        return result;
    }
    
    static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
        Vector3 f = (center - eye).Normalized();
        Vector3 s = Vector3::Cross(f, up).Normalized();
        Vector3 u = Vector3::Cross(s, f);
        
        Matrix4 result(1.0f);
        result.m[0] = s.x;
        result.m[4] = s.y;
        result.m[8] = s.z;
        result.m[1] = u.x;
        result.m[5] = u.y;
        result.m[9] = u.z;
        result.m[2] = -f.x;
        result.m[6] = -f.y;
        result.m[10] = -f.z;
        result.m[12] = -Vector3::Dot(s, eye);
        result.m[13] = -Vector3::Dot(u, eye);
        result.m[14] = Vector3::Dot(f, eye);
        
        return result;
    }
    
    Matrix4 operator*(const Matrix4& other) const {
        Matrix4 result;
        
        for (int i = 0; i < 4; ++i) {
            float32x4_t row = rows[i];
            
            for (int j = 0; j < 4; ++j) {
                float32x4_t col = {
                    other.m[j],
                    other.m[j + 4],
                    other.m[j + 8],
                    other.m[j + 12]
                };
                
                float32x4_t mul = vmulq_f32(row, col);
                float32x2_t sum = vpadd_f32(vget_low_f32(mul), vget_high_f32(mul));
                sum = vpadd_f32(sum, sum);
                result.m[i * 4 + j] = vget_lane_f32(sum, 0);
            }
        }
        
        return result;
    }
    
    Vector4 operator*(const Vector4& vec) const {
        Vector4 result;
        
        for (int i = 0; i < 4; ++i) {
            float32x4_t row = rows[i];
            float32x4_t v = vld1q_f32(vec.data);
            float32x4_t mul = vmulq_f32(row, v);
            float32x2_t sum = vpadd_f32(vget_low_f32(mul), vget_high_f32(mul));
            sum = vpadd_f32(sum, sum);
            result.data[i] = vget_lane_f32(sum, 0);
        }
        
        return result;
    }
    
    Matrix4 Transposed() const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m2[i][j] = m2[j][i];
            }
        }
        return result;
    }
    
    Matrix4 Inverted() const;
};

}

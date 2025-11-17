#pragma once

#include "Vector.hpp"
#include <cmath>

namespace Orchard::Math {

struct Quaternion {
    float x, y, z, w;
    
    Quaternion() : x(0), y(0), z(0), w(1) {}
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    
    static Quaternion Identity() {
        return Quaternion(0, 0, 0, 1);
    }
    
    static Quaternion FromEuler(float pitch, float yaw, float roll) {
        float cy = std::cos(yaw * 0.5f);
        float sy = std::sin(yaw * 0.5f);
        float cp = std::cos(pitch * 0.5f);
        float sp = std::sin(pitch * 0.5f);
        float cr = std::cos(roll * 0.5f);
        float sr = std::sin(roll * 0.5f);
        
        Quaternion q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;
        
        return q;
    }
    
    static Quaternion FromAxisAngle(const Vector3& axis, float angle) {
        float halfAngle = angle * 0.5f;
        float s = std::sin(halfAngle);
        
        Quaternion q;
        q.x = axis.x * s;
        q.y = axis.y * s;
        q.z = axis.z * s;
        q.w = std::cos(halfAngle);
        
        return q;
    }
    
    Vector3 ToEuler() const {
        Vector3 angles;
        
        float sinr_cosp = 2 * (w * x + y * z);
        float cosr_cosp = 1 - 2 * (x * x + y * y);
        angles.x = std::atan2(sinr_cosp, cosr_cosp);
        
        float sinp = 2 * (w * y - z * x);
        if (std::abs(sinp) >= 1)
            angles.y = std::copysign(3.14159265f / 2, sinp);
        else
            angles.y = std::asin(sinp);
        
        float siny_cosp = 2 * (w * z + x * y);
        float cosy_cosp = 1 - 2 * (y * y + z * z);
        angles.z = std::atan2(siny_cosp, cosy_cosp);
        
        return angles;
    }
    
    float Length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }
    
    Quaternion Normalized() const {
        float len = Length();
        if (len > 0.0001f) {
            float invLen = 1.0f / len;
            return Quaternion(x * invLen, y * invLen, z * invLen, w * invLen);
        }
        return *this;
    }
    
    Quaternion Conjugate() const {
        return Quaternion(-x, -y, -z, w);
    }
    
    Quaternion Inverse() const {
        return Conjugate().Normalized();
    }
    
    static float Dot(const Quaternion& a, const Quaternion& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }
    
    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t) {
        float dot = Dot(a, b);
        
        Quaternion qb = b;
        if (dot < 0.0f) {
            qb = Quaternion(-b.x, -b.y, -b.z, -b.w);
            dot = -dot;
        }
        
        if (dot > 0.9995f) {
            Quaternion result(
                a.x + t * (qb.x - a.x),
                a.y + t * (qb.y - a.y),
                a.z + t * (qb.z - a.z),
                a.w + t * (qb.w - a.w)
            );
            return result.Normalized();
        }
        
        float theta = std::acos(dot);
        float sinTheta = std::sin(theta);
        float wa = std::sin((1.0f - t) * theta) / sinTheta;
        float wb = std::sin(t * theta) / sinTheta;
        
        return Quaternion(
            wa * a.x + wb * qb.x,
            wa * a.y + wb * qb.y,
            wa * a.z + wb * qb.z,
            wa * a.w + wb * qb.w
        );
    }
    
    Quaternion operator*(const Quaternion& other) const {
        return Quaternion(
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w,
            w * other.w - x * other.x - y * other.y - z * other.z
        );
    }
    
    Vector3 operator*(const Vector3& vec) const {
        Vector3 qv(x, y, z);
        Vector3 uv = Vector3::Cross(qv, vec);
        Vector3 uuv = Vector3::Cross(qv, uv);
        
        return vec + ((uv * w) + uuv) * 2.0f;
    }
};

}

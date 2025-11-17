#pragma once

#include <cmath>
#include <arm_neon.h>

namespace Orchard::Math {

struct alignas(16) Vector3 {
    union {
        struct { float x, y, z; };
        float data[3];
        float32x4_t neon;
    };
    
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float scalar) : x(scalar), y(scalar), z(scalar) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    float Length() const {
        float32x4_t v = vld1q_f32(data);
        float32x4_t sq = vmulq_f32(v, v);
        float32x2_t sum = vpadd_f32(vget_low_f32(sq), vget_high_f32(sq));
        sum = vpadd_f32(sum, sum);
        return std::sqrt(vget_lane_f32(sum, 0));
    }
    
    float LengthSquared() const {
        float32x4_t v = vld1q_f32(data);
        float32x4_t sq = vmulq_f32(v, v);
        float32x2_t sum = vpadd_f32(vget_low_f32(sq), vget_high_f32(sq));
        sum = vpadd_f32(sum, sum);
        return vget_lane_f32(sum, 0);
    }
    
    Vector3 Normalized() const {
        float len = Length();
        if (len > 0.0001f) {
            float invLen = 1.0f / len;
            return Vector3(x * invLen, y * invLen, z * invLen);
        }
        return *this;
    }
    
    void Normalize() {
        *this = Normalized();
    }
    
    static float Dot(const Vector3& a, const Vector3& b) {
        float32x4_t va = vld1q_f32(a.data);
        float32x4_t vb = vld1q_f32(b.data);
        float32x4_t mul = vmulq_f32(va, vb);
        float32x2_t sum = vpadd_f32(vget_low_f32(mul), vget_high_f32(mul));
        sum = vpadd_f32(sum, sum);
        return vget_lane_f32(sum, 0);
    }
    
    static Vector3 Cross(const Vector3& a, const Vector3& b) {
        return Vector3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }
    
    Vector3 operator+(const Vector3& other) const {
        float32x4_t va = vld1q_f32(data);
        float32x4_t vb = vld1q_f32(other.data);
        float32x4_t result = vaddq_f32(va, vb);
        Vector3 r;
        vst1q_f32(r.data, result);
        return r;
    }
    
    Vector3 operator-(const Vector3& other) const {
        float32x4_t va = vld1q_f32(data);
        float32x4_t vb = vld1q_f32(other.data);
        float32x4_t result = vsubq_f32(va, vb);
        Vector3 r;
        vst1q_f32(r.data, result);
        return r;
    }
    
    Vector3 operator*(float scalar) const {
        float32x4_t v = vld1q_f32(data);
        float32x4_t s = vdupq_n_f32(scalar);
        float32x4_t result = vmulq_f32(v, s);
        Vector3 r;
        vst1q_f32(r.data, result);
        return r;
    }
    
    Vector3 operator/(float scalar) const {
        float invScalar = 1.0f / scalar;
        return *this * invScalar;
    }
    
    Vector3& operator+=(const Vector3& other) {
        *this = *this + other;
        return *this;
    }
    
    Vector3& operator-=(const Vector3& other) {
        *this = *this - other;
        return *this;
    }
    
    Vector3& operator*=(float scalar) {
        *this = *this * scalar;
        return *this;
    }
    
    float& operator[](int index) { return data[index]; }
    const float& operator[](int index) const { return data[index]; }
};

struct alignas(16) Vector4 {
    union {
        struct { float x, y, z, w; };
        float data[4];
        float32x4_t neon;
    };
    
    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4(const Vector3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
    
    float Length() const {
        float32x4_t v = vld1q_f32(data);
        float32x4_t sq = vmulq_f32(v, v);
        float32x2_t sum = vpadd_f32(vget_low_f32(sq), vget_high_f32(sq));
        sum = vpadd_f32(sum, sum);
        return std::sqrt(vget_lane_f32(sum, 0));
    }
    
    Vector4 Normalized() const {
        float len = Length();
        if (len > 0.0001f) {
            float32x4_t v = vld1q_f32(data);
            float32x4_t lenVec = vdupq_n_f32(len);
            float32x4_t result = vdivq_f32(v, lenVec);
            Vector4 r;
            vst1q_f32(r.data, result);
            return r;
        }
        return *this;
    }
    
    static float Dot(const Vector4& a, const Vector4& b) {
        float32x4_t va = vld1q_f32(a.data);
        float32x4_t vb = vld1q_f32(b.data);
        float32x4_t mul = vmulq_f32(va, vb);
        float32x2_t sum = vpadd_f32(vget_low_f32(mul), vget_high_f32(mul));
        sum = vpadd_f32(sum, sum);
        return vget_lane_f32(sum, 0);
    }
    
    Vector4 operator+(const Vector4& other) const {
        float32x4_t va = vld1q_f32(data);
        float32x4_t vb = vld1q_f32(other.data);
        float32x4_t result = vaddq_f32(va, vb);
        Vector4 r;
        vst1q_f32(r.data, result);
        return r;
    }
    
    Vector4 operator*(float scalar) const {
        float32x4_t v = vld1q_f32(data);
        float32x4_t s = vdupq_n_f32(scalar);
        float32x4_t result = vmulq_f32(v, s);
        Vector4 r;
        vst1q_f32(r.data, result);
        return r;
    }
};

}

#pragma once

#include <algorithm>
#include <cmath>
#include <ostream>

#include "config.h"

class Vec3 {
public:
    Vec3() :
        m_X(0), m_Y(0), m_Z(0) {}

    Vec3(real x) :
        m_X(x), m_Y(x), m_Z(x) {}

    Vec3(real x, real y, real z) :
        m_X(x), m_Y(y), m_Z(z) {}

    real X() const { return m_X; }
    real Y() const { return m_Y; }
    real Z() const { return m_Z; }

    real R() const { return m_X; }
    real G() const { return m_Y; }
    real B() const { return m_Z; }

    real Length() const {
        return std::sqrt(m_X * m_X + m_Y * m_Y + m_Z * m_Z);
    }

    real LengthSquared() const {
        return m_X * m_X + m_Y * m_Y + m_Z * m_Z;
    }

    Vec3 Normalized() const {
        const real length = Length();
        return Vec3(m_X / length, m_Y / length, m_Z / length);
    }

    real MinComponent() const {
        return std::min(m_X, std::min(m_Y, m_Z));
    }

    real MaxComponent() const {
        return std::max(m_X, std::max(m_Y, m_Z));
    }

    bool operator==(const Vec3 &other) const {
        return m_X == other.m_X && m_Y == other.m_Y && m_Z == other.m_Z;
    }

private:
    real m_X, m_Y, m_Z;
};

inline std::ostream &operator<<(std::ostream &os, const Vec3 &v) { 
    return os << "Vec3(" << v.X() << ", " << v.Y() << ", " << v.Z() << ")";
}

namespace std {
template <>
struct hash<Vec3> {
    std::size_t operator()(const Vec3 &v) const {
        const auto h = std::hash<real>();
        return h(v.X()) ^ h(v.Y()) ^ h(v.Z());
    }
};

}

// scalar
inline Vec3 operator+(const Vec3 &v, const real t) {
    return Vec3(v.X() + t, v.Y() + t, v.Z() + t);
}

inline Vec3 operator-(const Vec3 &v, const real t) {
    return Vec3(v.X() - t, v.Y() - t, v.Z() - t);
}

inline Vec3 operator*(const real t, const Vec3 &v) {
    return Vec3(v.X() * t, v.Y() * t, v.Z() * t);
}

inline Vec3 operator*(const Vec3 &v, const real t) {
    return Vec3(v.X() * t, v.Y() * t, v.Z() * t);
}

inline Vec3 operator/(const Vec3 &v, const real t) {
    return Vec3(v.X() / t, v.Y() / t, v.Z() / t);
}

// vector
inline Vec3 operator-(const Vec3 &a) {
    return Vec3(-a.X(), -a.Y(), -a.Z());
}

inline Vec3 operator+(const Vec3 &a, const Vec3 &b) {
    return Vec3(a.X() + b.X(), a.Y() + b.Y(), a.Z() + b.Z());
}

inline Vec3 operator-(const Vec3 &a, const Vec3 &b) {
    return Vec3(a.X() - b.X(), a.Y() - b.Y(), a.Z() - b.Z());
}

inline Vec3 operator*(const Vec3 &a, const Vec3 &b) {
    return Vec3(a.X() * b.X(), a.Y() * b.Y(), a.Z() * b.Z());
}

inline Vec3 operator/(const Vec3 &a, const Vec3 &b) {
    return Vec3(a.X() / b.X(), a.Y() / b.Y(), a.Z() / b.Z());
}

// functions
inline Vec3 Normalized(const Vec3 &v) {
    return v.Normalized();
}

inline real Dot(const Vec3 &a, const Vec3 &b) {
    return a.X() * b.X() + a.Y() * b.Y() + a.Z() * b.Z();
}

inline Vec3 Cross(const Vec3 &a, const Vec3 &b) {
    const real x = a.Y() * b.Z() - a.Z() * b.Y();
    const real y = a.Z() * b.X() - a.X() * b.Z();
    const real z = a.X() * b.Y() - a.Y() * b.X();
    return Vec3(x, y, z);
}

inline Vec3 Pow(const Vec3 &v, const real a) {
    return Vec3(std::pow(v.X(), a), std::pow(v.Y(), a), std::pow(v.Z(), a));
}

inline Vec3 Floor(const Vec3 &v) {
    return Vec3(std::floor(v.X()), std::floor(v.Y()), std::floor(v.Z()));
}

inline Vec3 Fract(const Vec3 &v) {
    return v - Floor(v);
    const real x = std::fmod(v.X(), 1);
    const real y = std::fmod(v.Y(), 1);
    const real z = std::fmod(v.Z(), 1);
    return Vec3(x, y, z);
}

inline Vec3 TriangleNormal(const Vec3 &v1, const Vec3 &v2, const Vec3 &v3) {
    return Normalized(Cross(v2 - v1, v3 - v1));
}

inline Vec3 Abs(const Vec3 &v) {
    return Vec3(std::abs(v.X()), std::abs(v.Y()), std::abs(v.Z()));
}

inline Vec3 Min(const Vec3 &a, const Vec3 &b) {
    return Vec3(std::min(a.X(), b.X()), std::min(a.Y(), b.Y()), std::min(a.Z(), b.Z()));
}

inline Vec3 Max(const Vec3 &a, const Vec3 &b) {
    return Vec3(std::max(a.X(), b.X()), std::max(a.Y(), b.Y()), std::max(a.Z(), b.Z()));
}

inline Vec3 HexColor(const int hex) {
    const real r = real((hex >> 16) & 0xff) / 255;
    const real g = real((hex >> 8) & 0xff) / 255;
    const real b = real((hex >> 0) & 0xff) / 255;
    return Pow(Vec3(r, g, b), 2.2);
}

#pragma once

#include "vec3.h"

const real INV_SQRT_3 = 1 / std::sqrt(3);

class ONB {
public:
    ONB(const Vec3 &n) {
        const Vec3 normal = Normalized(n);
        Vec3 majorAxis;
        if (std::abs(normal.X()) < INV_SQRT_3) {
            majorAxis = Vec3(1, 0, 0);
        } else if (std::abs(normal.Y()) < INV_SQRT_3) {
            majorAxis = Vec3(0, 1, 0);
        } else {
            majorAxis = Vec3(0, 0, 1);
        }
        m_S = Normalized(Cross(normal, majorAxis));
        m_T = Cross(normal, m_S);
        m_N = normal;
    }

    Vec3 WorldToLocal(const Vec3 &v) const {
        return Vec3(Dot(v, m_S), Dot(v, m_T), Dot(v, m_N));
    }

    Vec3 LocalToWorld(const Vec3 &v) const {
        const real x = m_S.X() * v.X() + m_T.X() * v.Y() + m_N.X() * v.Z();
        const real y = m_S.Y() * v.X() + m_T.Y() * v.Y() + m_N.Y() * v.Z();
        const real z = m_S.Z() * v.X() + m_T.Z() * v.Y() + m_N.Z() * v.Z();
        return Vec3(x, y, z);
    }

private:
    Vec3 m_S;
    Vec3 m_T;
    Vec3 m_N;
};

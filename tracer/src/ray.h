#pragma once

#include "vec3.h"

class Ray {
public:
    Ray() {}

    Ray(const Vec3 &origin, const Vec3 &direction) :
        m_Origin(origin), m_Direction(direction) {}

    const Vec3 &Origin() const {
        return m_Origin;
    }

    const Vec3 &Direction() const {
        return m_Direction;
    }

    Vec3 At(const real t) const {
        return m_Origin + t * m_Direction;
    }

private:
    Vec3 m_Origin;
    Vec3 m_Direction;
};

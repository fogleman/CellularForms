#pragma once

#include "ray.h"
#include "vec3.h"

class Camera {
public:
    Camera(
        const Vec3 &eye,
        const Vec3 &center,
        const Vec3 &up,
        const real fovy,
        const real aspect,
        const real aperture,
        const real focusDistance);

    Ray MakeRay(const real u, const real v) const;

private:
    Vec3 m_Origin;
    Vec3 m_LowerLeft;
    Vec3 m_Horizontal;
    Vec3 m_Vertical;
    Vec3 m_U;
    Vec3 m_V;
    Vec3 m_W;
    real m_Aperture;
};

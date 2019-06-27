#include "camera.h"

#include <cmath>

#include "util.h"

Camera::Camera(
    const Vec3 &eye,
    const Vec3 &center,
    const Vec3 &up,
    const real fovy,
    const real aspect,
    const real aperture,
    const real focusDistance)
{
    const real d = focusDistance;
    const real theta = fovy * M_PI / 180;
    const real halfHeight = std::tan(theta / 2);
    const real halfWidth = halfHeight * aspect;
    m_W = Normalized(eye - center);
    m_U = Normalized(Cross(up, m_W));
    m_V = Cross(m_W, m_U);
    m_LowerLeft = eye - halfWidth * d * m_U - halfHeight * d * m_V - d * m_W;
    m_Horizontal = 2 * halfWidth * d * m_U;
    m_Vertical = 2 * halfHeight * d * m_V;
    m_Origin = eye;
    m_Aperture = aperture;
}

Ray Camera::MakeRay(const real u, const real v) const {
    const Vec3 rd = RandomInUnitDisk() * (m_Aperture / 2);
    const Vec3 offset = m_U * rd.X() + m_V * rd.Y();
    const Vec3 dir = Normalized(
        m_LowerLeft + m_Horizontal * u + m_Vertical * v - m_Origin - offset);
    return Ray(m_Origin + offset, dir);
}

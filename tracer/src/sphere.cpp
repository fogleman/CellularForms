#include "sphere.h"

#include <cmath>

#include "onb.h"
#include "util.h"

bool Sphere::Hit(const Ray &ray, const real tmin, const real tmax, HitInfo &hit) const {
    const Vec3 oc = ray.Origin() - m_Center;
    const real a = Dot(ray.Direction(), ray.Direction());
    const real b = Dot(oc, ray.Direction());
    const real c = Dot(oc, oc) - m_Radius * m_Radius;
    const real d = b * b - a * c;
    if (d > 0) {
        real t = (-b - std::sqrt(b * b - a * c)) / a;
        if (t < tmax && t > tmin) {
            hit.T = t;
            hit.Position = ray.At(t);
            hit.Normal = (hit.Position - m_Center) / m_Radius;
            hit.Material = m_Material;
            return true;
        }
        t = (-b + std::sqrt(b * b - a * c)) / a;
        if (t < tmax && t > tmin) {
            hit.T = t;
            hit.Position = ray.At(t);
            hit.Normal = (hit.Position - m_Center) / m_Radius;
            hit.Material = m_Material;
            return true;
        }
    }
    return false;
}

Ray Sphere::RandomRay(const Vec3 &o) const {
    const Vec3 dir = m_Center - o;
    const ONB onb(dir);
    const Vec3 p = m_Center + onb.LocalToWorld(RandomInUnitDisk() * m_Radius);
    return Ray(o, Normalized(p - o));
}

real Sphere::Pdf(const Ray &ray) const {
    HitInfo hit;
    if (!Hit(ray, eps, inf, hit)) {
        return 0;
    }
    const real costhetamax = std::sqrt(1 - m_Radius * m_Radius / (m_Center - ray.Origin()).LengthSquared());
    const real solidangle = 2 * M_PI * (1 - costhetamax);
    return 1 / solidangle;
}

#pragma once

#include "hit.h"
#include "ray.h"
#include "vec3.h"

class Sphere : public Hittable {
public:
    Sphere(const Vec3 &center, const real radius, const P_Material &material) :
        m_Center(center), m_Radius(radius), m_Material(material) {}

    virtual bool Hit(const Ray &ray, const real tmin, const real tmax, HitInfo &hit) const;

    virtual Ray RandomRay(const Vec3 &o) const;

    virtual real Pdf(const Ray &ray) const;

private:
    Vec3 m_Center;
    real m_Radius;
    P_Material m_Material;
};

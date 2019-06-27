#pragma once

#include <memory>
#include <vector>

#include "material.h"
#include "ray.h"
#include "vec3.h"

struct HitInfo {
    real T;
    Vec3 Position;
    Vec3 Normal;
    P_Material Material;
};

class Hittable {
public:
    virtual bool Hit(const Ray &ray, const real tmin, const real tmax, HitInfo &hit) const = 0;

    virtual Ray RandomRay(const Vec3 &o) const {
        return Ray();
    }

    virtual real Pdf(const Ray &ray) const {
        return 0;
    }

    virtual ~Hittable() {}
};

typedef std::shared_ptr<Hittable> P_Hittable;

class HittableList : public Hittable {
public:
    void Add(const P_Hittable &item);
    void AddLight(const P_Hittable &item);

    const std::vector<P_Hittable> &Lights() const {
        return m_Lights;
    }

    virtual bool Hit(const Ray &ray, const real tmin, const real tmax, HitInfo &hit) const;
private:
    std::vector<P_Hittable> m_Items;
    std::vector<P_Hittable> m_Lights;
};

#pragma once

#include <embree3/rtcore.h>
#include <string>
#include <vector>

#include "hit.h"
#include "material.h"

class Triangle {
public:
    Vec3 V1, V2, V3;
    Vec3 N1, N2, N3;

    Vec3 Normal() const {
        return TriangleNormal(V1, V2, V3);
    }

    Vec3 Normal(const Vec3 &p) const {
        const Vec3 b = Barycentric(p);
        return Normalized(b.X() * N1 + b.Y() * N2 + b.Z() * N3);
    }

    Vec3 Barycentric(const Vec3 &p) const {
        const Vec3 v0 = V2 - V1;
        const Vec3 v1 = V3 - V1;
        const Vec3 v2 = p - V1;
        const real d00 = Dot(v0, v0);
        const real d01 = Dot(v0, v1);
        const real d11 = Dot(v1, v1);
        const real d20 = Dot(v2, v0);
        const real d21 = Dot(v2, v1);
        const real d = d00*d11 - d01*d01;
        const real v = (d11*d20 - d01*d21) / d;
        const real w = (d00*d21 - d01*d20) / d;
        const real u = 1 - v - w;
        return Vec3(u, v, w);
    }
};

class EmbreeMesh : public Hittable {
public:
    EmbreeMesh(std::string path, const P_Material &material);
    virtual bool Hit(const Ray &ray, const real tmin, const real tmax, HitInfo &hit) const;
private:
    RTCScene m_Scene;
    std::vector<Triangle> m_Triangles;
    P_Material m_Material;
};

class EmbreeSpheres : public Hittable {
public:
    EmbreeSpheres(std::string path, const P_Material &material);
    virtual bool Hit(const Ray &ray, const real tmin, const real tmax, HitInfo &hit) const;
private:
    RTCScene m_Scene;
    P_Material m_Material;
    std::vector<P_Material> m_Materials;
};

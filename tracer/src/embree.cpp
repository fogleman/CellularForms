#include "embree.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

#include "colormap.h"
#include "stl.h"

const RTCDevice device = rtcNewDevice(NULL);

struct EmbreeVertex {
    float x, y, z, a;
};

struct EmbreeTriangle {
    int v0, v1, v2;
};

EmbreeMesh::EmbreeMesh(std::string path, const P_Material &material) :
    m_Material(material)
{
    // load stl
    std::vector<Vec3> points = LoadBinarySTL(path);

    // compute bounding box
    Vec3 min = points[0];
    Vec3 max = points[0];
    for (const auto &v : points) {
        min = Min(min, v);
        max = Max(max, v);
    }

    // transform points
    const Vec3 size = max - min;
    const Vec3 center = min + size / 2;
    const real scale = 1 / size.MaxComponent();
    for (int i = 0; i < points.size(); i++) {
        points[i] = (points[i] - center) * scale;
        // points[i] = Vec3(points[i].X(), points[i].Y(), points[i].Z());
    }

    const int numTriangles = points.size() / 3;

    // store triangles
    m_Triangles.resize(numTriangles);
    for (int i = 0; i < numTriangles; i++) {
        m_Triangles[i].V1 = points[i*3+0];
        m_Triangles[i].V2 = points[i*3+1];
        m_Triangles[i].V3 = points[i*3+2];
        const Vec3 n = m_Triangles[i].Normal();
        m_Triangles[i].N1 = n;
        m_Triangles[i].N2 = n;
        m_Triangles[i].N3 = n;
    }

    // smooth normals
    std::unordered_map<Vec3, Vec3> map;
    for (const auto &t : m_Triangles) {
        const Vec3 n = t.Normal();
        map[t.V1] = map[t.V1] + n;
        map[t.V2] = map[t.V2] + n;
        map[t.V3] = map[t.V3] + n;
    }

    for (const auto &pair : map) {
        map[pair.first] = Normalized(pair.second);
    }

    for (auto &t : m_Triangles) {
        t.N1 = map[t.V1];
        t.N2 = map[t.V2];
        t.N3 = map[t.V3];
    }

    m_Scene = rtcNewScene(device);
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

    EmbreeVertex *vertices = (EmbreeVertex *)rtcSetNewGeometryBuffer(
        geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(EmbreeVertex), points.size());
    for (int i = 0; i < points.size(); i++) {
        vertices[i] = EmbreeVertex{
            static_cast<float>(points[i].X()),
            static_cast<float>(points[i].Y()),
            static_cast<float>(points[i].Z()),
            0};
    }

    EmbreeTriangle *triangles = (EmbreeTriangle *)rtcSetNewGeometryBuffer(
        geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(EmbreeTriangle), numTriangles);
    for (int i = 0; i < numTriangles; i++) {
        triangles[i] = EmbreeTriangle{i*3+0, i*3+1, i*3+2};
    }

    rtcCommitGeometry(geom);
    rtcAttachGeometry(m_Scene, geom);
    rtcReleaseGeometry(geom);
    rtcCommitScene(m_Scene);
}

bool EmbreeMesh::Hit(const Ray &ray, const real tmin, const real tmax, HitInfo &hit) const {
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    const Vec3 &org = ray.Origin();
    const Vec3 &dir = ray.Direction();

    RTCRayHit r;
    r.ray.org_x = org.X(); r.ray.org_y = org.Y(); r.ray.org_z = org.Z();
    r.ray.dir_x = dir.X(); r.ray.dir_y = dir.Y(); r.ray.dir_z = dir.Z();
    r.ray.tnear = tmin;
    r.ray.tfar = tmax;
    r.ray.mask = -1;
    r.ray.flags = 0;
    r.ray.time = 0;
    r.ray.id = 0;

    r.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    r.hit.primID = RTC_INVALID_GEOMETRY_ID;

    rtcIntersect1(m_Scene, &context, &r);

    if (r.hit.primID == RTC_INVALID_GEOMETRY_ID) {
        return false;
    }

    const float x = r.ray.org_x + r.ray.dir_x * r.ray.tfar;
    const float y = r.ray.org_y + r.ray.dir_y * r.ray.tfar;
    const float z = r.ray.org_z + r.ray.dir_z * r.ray.tfar;

    hit.T = r.ray.tfar;
    hit.Position = Vec3(x, y, z);
    hit.Normal = m_Triangles[r.hit.primID].Normal(hit.Position);
    hit.Material = m_Material;
    return true;
}

typedef struct {
    float X;
    float Y;
    float Z;
    float R;
} Sphere;

EmbreeSpheres::EmbreeSpheres(std::string path, const P_Material &material) :
    m_Material(material)
{
    std::vector<Vec3> points = LoadBinarySTL(path);

    std::unordered_map<Vec3, std::vector<float>> linkLengths;
    for (int i = 0; i < points.size(); i += 3) {
        const auto &a = points[i+0];
        const auto &b = points[i+1];
        const auto &c = points[i+2];
        const float ab = (a - b).Length();
        const float ac = (a - c).Length();
        const float bc = (b - c).Length();
        linkLengths[a].push_back(ab);
        linkLengths[a].push_back(ac);
        linkLengths[b].push_back(ab);
        linkLengths[b].push_back(bc);
        linkLengths[c].push_back(ac);
        linkLengths[c].push_back(bc);
    }

    std::unordered_map<Vec3, float> radius;
    for (const auto &it : linkLengths) {
        const auto &p = it.first;
        const auto &lengths = it.second;
        const float mean = std::accumulate(
            lengths.begin(), lengths.end(), 0.f) / lengths.size();
        radius[p] = mean * 0.5f;
    }

    std::vector<Sphere> spheres;
    for (const auto &it : radius) {
        const auto &p = it.first;
        const float x = p.X();
        const float y = p.Y();
        const float z = p.Z();
        const float r = it.second;
        if (x > 0) {
            // continue;
        }
        spheres.push_back(Sphere{x, y, z, r});
    }

    std::sort(spheres.begin(), spheres.end(), [](const auto &a, const auto &b) {
        return a.R < b.R;
    });

    const float minRadius = spheres[spheres.size() * 1 / 1000].R;
    const float maxRadius = spheres[spheres.size() * 999 / 1000].R;

    // float minRadius = spheres[0].R;
    // float maxRadius = spheres[0].R;
    // for (const auto &s : spheres) {
    //     minRadius = std::min(minRadius, s.R);
    //     maxRadius = std::max(maxRadius, s.R);
    // }

    for (const auto &s : spheres) {
        const float t = (s.R - minRadius) / (maxRadius - minRadius);
        m_Materials.push_back(std::make_shared<Lambertian>(
            std::make_shared<SolidTexture>(Viridis.At(t))));
    }

    // compute bounding box
    Vec3 min = points[0];
    Vec3 max = points[0];
    for (const auto &p : points) {
        min = Min(min, p);
        max = Max(max, p);
    }

    // transform spheres
    const Vec3 size = max - min;
    const Vec3 center = min + size / 2;
    const real scale = 1 / size.MaxComponent();
    for (int i = 0; i < spheres.size(); i++) {
        Vec3 v(spheres[i].X, spheres[i].Y, spheres[i].Z);
        v = (v - center) * scale;
        spheres[i] = Sphere{
            static_cast<float>(v.X()),
            static_cast<float>(v.Y()),
            static_cast<float>(v.Z()),
            static_cast<float>(spheres[i].R * scale)};
    }

    m_Scene = rtcNewScene(device);
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_SPHERE_POINT);
    Sphere *buf = (Sphere *)rtcSetNewGeometryBuffer(
        geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT4, sizeof(Sphere), spheres.size());
    for (int i = 0; i < spheres.size(); i++) {
        buf[i] = spheres[i];
    }

    rtcCommitGeometry(geom);
    unsigned int geomID = rtcAttachGeometry(m_Scene, geom);
    rtcReleaseGeometry(geom);
    rtcCommitScene(m_Scene);

    std::cout << geomID << " " << spheres.size() << std::endl;
}

bool EmbreeSpheres::Hit(const Ray &ray, const real tmin, const real tmax, HitInfo &hit) const {
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    const Vec3 &org = ray.Origin();
    const Vec3 &dir = ray.Direction();

    RTCRayHit r;
    r.ray.org_x = org.X(); r.ray.org_y = org.Y(); r.ray.org_z = org.Z();
    r.ray.dir_x = dir.X(); r.ray.dir_y = dir.Y(); r.ray.dir_z = dir.Z();
    r.ray.tnear = tmin;
    r.ray.tfar = tmax;
    r.ray.mask = -1;
    r.ray.flags = 0;
    r.ray.time = 0;
    r.ray.id = 0;

    r.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    r.hit.primID = RTC_INVALID_GEOMETRY_ID;

    rtcIntersect1(m_Scene, &context, &r);

    if (r.hit.primID == RTC_INVALID_GEOMETRY_ID) {
        return false;
    }

    const float x = r.ray.org_x + r.ray.dir_x * r.ray.tfar;
    const float y = r.ray.org_y + r.ray.dir_y * r.ray.tfar;
    const float z = r.ray.org_z + r.ray.dir_z * r.ray.tfar;

    hit.T = r.ray.tfar;
    hit.Position = Vec3(x, y, z);
    hit.Normal = Normalized(Vec3(r.hit.Ng_x, r.hit.Ng_y, r.hit.Ng_z));
    hit.Material = m_Materials[r.hit.primID];
    // hit.Material = m_Material;
    return true;
}

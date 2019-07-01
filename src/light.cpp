#include "light.h"

#include "util.h"

const RTCDevice device = rtcNewDevice(NULL);

Light::Light() {
    m_Scene = rtcNewScene(device);

    rtcSetSceneBuildQuality(m_Scene, RTC_BUILD_QUALITY_LOW);
    rtcSetSceneFlags(m_Scene, RTC_SCENE_FLAG_DYNAMIC);

    m_BufferSize = 16 * 1024 * 1024; // 1M spheres (16 bytes per)
    m_Buffer = rtcNewBuffer(device, m_BufferSize);

    m_Geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_SPHERE_POINT);
    rtcAttachGeometry(m_Scene, m_Geom);
    // rtcReleaseGeometry(m_Geom);
}

void Light::UpdateSpheres(const std::vector<glm::vec4> &spheres) {
    void *data = rtcGetBufferData(m_Buffer);
    std::memcpy(data, spheres.data(), sizeof(spheres.front()) * spheres.size());
    rtcSetGeometryBuffer(
        m_Geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT4,
        m_Buffer, 0, sizeof(spheres.front()), spheres.size());
    rtcUpdateGeometryBuffer(m_Geom, RTC_BUFFER_TYPE_VERTEX, 0);
    rtcCommitGeometry(m_Geom);
    rtcCommitScene(m_Scene);
}

bool Light::Occluded(const glm::vec3 &org, const glm::vec3 &dir) const {
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    RTCRay r;
    r.org_x = org.x; r.org_y = org.y; r.org_z = org.z;
    r.dir_x = dir.x; r.dir_y = dir.y; r.dir_z = dir.z;
    r.tnear = 0;
    r.tfar = std::numeric_limits<float>::infinity();
    r.mask = -1;
    r.flags = 0;
    r.time = 0;
    r.id = 0;

    rtcOccluded1(m_Scene, &context, &r);

    return r.tfar < 0;
}

float Light::Occlusion(
    const glm::vec3 &org, const float radius, const int rayCount) const
{
    int occludedCount = 0;
    for (int i = 0; i < rayCount; i++) {
        const glm::vec3 dir = RandomUnitVector();
        if (Occluded(org + dir * radius, dir)) {
            occludedCount++;
        }
    }
    return (float)occludedCount / (float)rayCount;
}

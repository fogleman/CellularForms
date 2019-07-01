#pragma once

#include <embree3/rtcore.h>
#include <glm/glm.hpp>
#include <vector>

class Light {
public:
    Light();

    void UpdateSpheres(const std::vector<glm::vec4> &spheres);

    bool Occluded(const glm::vec3 &org, const glm::vec3 &dir) const;

    float Occlusion(
        const glm::vec3 &org, const float radius, const int rayCount) const;

private:
    RTCScene m_Scene;
    size_t m_BufferSize;
    RTCBuffer m_Buffer;
    RTCGeometry m_Geom;
};

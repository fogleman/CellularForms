#pragma once

#include <glm/glm.hpp>
#include <vector>

int RandomIntN(const int n);

glm::vec3 PlaneNormalFromPoints(
    const std::vector<glm::vec3> &points,
    const glm::vec3 &front);

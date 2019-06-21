#pragma once

#include "triangle.h"

#include <glm/fwd.hpp>
#include <vector>

std::vector<Triangle> IcosahedronTriangles();
std::vector<glm::vec3> SpherePoints(const int detail);

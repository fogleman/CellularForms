#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "triangle.h"

std::vector<glm::vec3> LoadBinarySTL(std::string path);

void SaveBinarySTL(std::string path, const std::vector<Triangle> &triangles);

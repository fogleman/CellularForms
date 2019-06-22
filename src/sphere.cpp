#include "sphere.h"

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <iostream>
#include <unordered_set>

std::vector<Triangle> IcosahedronTriangles() {
    const float a = 0.8506507174597755;
    const float b = 0.5257312591858783;

    const auto vertices = std::vector<glm::vec3>{
        {-a, -b,  0}, {-a,  b,  0}, {-b,  0, -a}, {-b,  0,  a},
        { 0, -a, -b}, { 0, -a,  b}, { 0,  a, -b}, { 0,  a,  b},
        { b,  0, -a}, { b,  0,  a}, { a, -b,  0}, { a,  b,  0},
    };

    const auto indices = std::vector<glm::ivec3>{
        { 0,  3,  1}, { 1,  3,  7}, { 2,  0,  1}, { 2,  1,  6},
        { 4,  0,  2}, { 4,  5,  0}, { 5,  3,  0}, { 6,  1,  7},
        { 6,  7, 11}, { 7,  3,  9}, { 8,  2,  6}, { 8,  4,  2},
        { 8,  6, 11}, { 8, 10,  4}, { 8, 11, 10}, { 9,  3,  5},
        {10,  5,  4}, {10,  9,  5}, {11,  7,  9}, {11,  9, 10},
    };

    std::vector<Triangle> triangles;
    triangles.reserve(indices.size());
    for (const auto &i : indices) {
        triangles.emplace_back(
            vertices[i.x],
            vertices[i.y],
            vertices[i.z]);
    }
    return triangles;
}

std::vector<Triangle> SphereTriangles(const int detail) {
    std::vector<Triangle> triangles;
    triangles.reserve(20 * std::pow(4, detail));

    std::function<void(
        const int,
        const glm::vec3 &, const glm::vec3 &, const glm::vec3 &)> helper;

    helper = [&helper, &triangles](
        const int detail,
        const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3)
    {
        if (detail == 0) {
            triangles.emplace_back(v1, v2, v3);
            return;
        }
        const glm::vec3 v12 = glm::normalize((v1 + v2) / 2.0f);
        const glm::vec3 v13 = glm::normalize((v1 + v3) / 2.0f);
        const glm::vec3 v23 = glm::normalize((v2 + v3) / 2.0f);
        helper(detail - 1, v1, v12, v13);
        helper(detail - 1, v2, v23, v12);
        helper(detail - 1, v3, v13, v23);
        helper(detail - 1, v12, v23, v13);
    };

    for (const auto &t : IcosahedronTriangles()) {
        helper(detail, t.A(), t.B(), t.C());
    }

    return triangles;
}

std::vector<glm::vec3> SpherePoints(const int detail) {
    std::unordered_set<glm::vec3> unique;
    for (const auto &t : SphereTriangles(detail)) {
        unique.insert(t.A());
        unique.insert(t.B());
        unique.insert(t.C());
    }
    std::vector<glm::vec3> result;
    result.insert(result.end(), unique.begin(), unique.end());
    return result;
}

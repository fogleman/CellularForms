#include "sphere.h"

#include <glm/glm.hpp>

std::vector<Triangle> IcosahedronTriangles() {
    const double a = 0.8506507174597755;
    const double b = 0.5257312591858783;

    const auto vertices = std::vector<glm::vec3>{
        {-a, -b, 0},
        {-a, b, 0},
        {-b, 0, -a},
        {-b, 0, a},
        {0, -a, -b},
        {0, -a, b},
        {0, a, -b},
        {0, a, b},
        {b, 0, -a},
        {b, 0, a},
        {a, -b, 0},
        {a, b, 0},
    };

    const auto indices = std::vector<glm::ivec3>{
        {0, 3, 1},
        {1, 3, 7},
        {2, 0, 1},
        {2, 1, 6},
        {4, 0, 2},
        {4, 5, 0},
        {5, 3, 0},
        {6, 1, 7},
        {6, 7, 11},
        {7, 3, 9},
        {8, 2, 6},
        {8, 4, 2},
        {8, 6, 11},
        {8, 10, 4},
        {8, 11, 10},
        {9, 3, 5},
        {10, 5, 4},
        {10, 9, 5},
        {11, 7, 9},
        {11, 9, 10},
    };

    std::vector<Triangle> result;
    result.reserve(indices.size());
    for (const auto &i : indices) {
        result.emplace_back(
            vertices[i.x],
            vertices[i.y],
            vertices[i.z]);
    }
    return result;
}

std::vector<glm::vec3> SpherePoints(const int detail) {
    std::vector<glm::vec3> result;
    return result;
}

// func NewSphere(detail int) *Mesh {
//     var triangles []*Triangle
//     ico := NewIcosahedron()
//     for _, t := range ico.Triangles {
//         v1 := t.V1.Position
//         v2 := t.V2.Position
//         v3 := t.V3.Position
//         triangles = append(triangles, newSphereHelper(detail, v1, v2, v3)...)
//     }
//     return NewTriangleMesh(triangles)
// }

// func newSphereHelper(detail int, v1, v2, v3 Vector) []*Triangle {
//     if detail == 0 {
//         t := NewTriangleForPoints(v1, v2, v3)
//         return []*Triangle{t}
//     }
//     var triangles []*Triangle
//     v12 := v1.Add(v2).DivScalar(2).Normalize()
//     v13 := v1.Add(v3).DivScalar(2).Normalize()
//     v23 := v2.Add(v3).DivScalar(2).Normalize()
//     triangles = append(triangles, newSphereHelper(detail-1, v1, v12, v13)...)
//     triangles = append(triangles, newSphereHelper(detail-1, v2, v23, v12)...)
//     triangles = append(triangles, newSphereHelper(detail-1, v3, v13, v23)...)
//     triangles = append(triangles, newSphereHelper(detail-1, v12, v23, v13)...)
//     return triangles
// }

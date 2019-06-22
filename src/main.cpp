#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "model.h"
#include "sphere.h"

// TODO: spatial hash index

int main() {
    const auto triangles = SphereTriangles(1);
    Model model(triangles);
    for (int i = 0; i < 100000; i++) {
        model.Update();
    }

    const auto &positions = model.Positions();
    const auto &normals = model.Normals();
    for (int i = 0; i < positions.size(); i++) {
        const auto p = positions[i];
        const auto n = normals[i];
        printf("(%g,%g,%g,%g,%g,%g),\n", p.x, p.y, p.z, n.x, n.y, n.z);
    }
}

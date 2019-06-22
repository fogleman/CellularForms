#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "model.h"
#include "sphere.h"

// TODO: spatial hash index

int main() {
    const auto triangles = SphereTriangles(1);
    Model model(triangles);
    for (int i = 0; i < 100000; i++) {
        // std::cout << glm::to_string(model.Cells()[0]) << std::endl;
        model.Update();
    }

    const auto &cells = model.Cells();
    for (const auto &p : cells) {
        std::cout << glm::to_string(p) << std::endl;
    }
}

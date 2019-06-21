#include "sphere.h"

#include <glm/gtx/string_cast.hpp>
#include <iostream>

int main() {
    const auto points = SpherePoints(2);
    // std::cout << points.size() << std::endl;
    for (const auto &p : points) {
        std::cout << glm::to_string(p) << std::endl;
    }
    return 0;
}

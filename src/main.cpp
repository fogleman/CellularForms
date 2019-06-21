#include "sphere.h"

#include <glm/gtx/string_cast.hpp>
#include <iostream>

int main() {
    const auto points = SpherePoints(1);
    std::cout << points.size() << std::endl;
    return 0;
}

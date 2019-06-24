#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "ctpl.h"
#include "model.h"
#include "sphere.h"
#include "stl.h"
#include "util.h"

int main() {
    const auto sphereTriangles = SphereTriangles(1);
    Model model(sphereTriangles);

    ctpl::thread_pool tp(4);

    for (int i = 0; ; i++) {
        // break;
        const int n = model.Positions().size();
        fprintf(stderr, "%d: %d\n", i, n);
        // model.Update();
        model.UpdateWithThreadPool(tp);
        if (n > 336*8*8) {
            break;
        }
    }

    const auto &positions = model.Positions();
    const auto &links = model.Links();
    for (int i = 0; i < positions.size(); i++) {
        for (const int j : links[i]) {
            if (j < i) {
                continue;
            }
            const auto p = positions[i];
            const auto q = positions[j];
            printf("%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n", p.x, p.y, p.z, q.x, q.y, q.z);
        }
    }

    const auto triangles = model.Triangulate();
    SaveBinarySTL("out.stl", triangles);
}

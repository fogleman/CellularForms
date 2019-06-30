#include <iostream>

#include "gui.h"
#include "model.h"
#include "pool.h"
#include "sphere.h"
#include "stl.h"
#include "util.h"

void RunForever(Model &model) {
    const auto startTime = std::chrono::steady_clock::now();
    ThreadPool pool;
    int iterations = 0;
    while (1) {
        model.UpdateWithThreadPool(pool);
        iterations++;
        if (iterations % 1000 == 0) {
            char filename[1024];
            sprintf(filename, "out%08d.stl", iterations);
            SaveBinarySTL(filename, model.Triangulate());
            const std::chrono::duration<double> elapsed =
                std::chrono::steady_clock::now() - startTime;
            std::cout
                << iterations << " "
                << elapsed.count() << " "
                << model.Positions().size()
                << std::endl;
        }
    }
}

int main() {
    float SplitThreshold = 1000;
    float LinkRestLength = 1;
    float RadiusOfInfluence = Random(LinkRestLength, LinkRestLength * 5);
    float RepulsionFactor = Random(0, 0.5);
    float SpringFactor = Random(0, 0.5);
    float PlanarFactor = Random(0, 0.5);
    float BulgeFactor = Random(0, 0.5);

    std::cout << "SplitThreshold    = " << SplitThreshold << std::endl;
    std::cout << "LinkRestLength    = " << LinkRestLength << std::endl;
    std::cout << "RadiusOfInfluence = " << RadiusOfInfluence << std::endl;
    std::cout << "RepulsionFactor   = " << RepulsionFactor << std::endl;
    std::cout << "SpringFactor      = " << SpringFactor << std::endl;
    std::cout << "PlanarFactor      = " << PlanarFactor << std::endl;
    std::cout << "BulgeFactor       = " << BulgeFactor << std::endl;
    std::cout << std::endl;

    const auto triangles = SphereTriangles(1);

    // const auto triangles = LoadBinarySTL(argv[1]);
    // const float averageEdgeLength = [&triangles]() {
    //     float sum = 0;
    //     for (const auto &t : triangles) {
    //         sum += glm::distance(t.A(), t.B());
    //         sum += glm::distance(t.B(), t.C());
    //         sum += glm::distance(t.C(), t.A());
    //     }
    //     return sum / (triangles.size() * 3);
    // }();

    Model model(
        triangles,
        SplitThreshold, LinkRestLength, RadiusOfInfluence,
        RepulsionFactor, SpringFactor, PlanarFactor, BulgeFactor);

    RunGUI(model);
    // RunForever(model);

    return 0;
}

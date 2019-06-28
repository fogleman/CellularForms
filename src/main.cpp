#include <iostream>

#include "ctpl.h"
#include "gui.h"
#include "model.h"
#include "sphere.h"
#include "stl.h"

const float SplitThreshold = 2000;
const float LinkRestLength = 1;
const float RadiusOfInfluence = 20;
const float RepulsionFactor = 0.2;
const float SpringFactor = 0.8;
const float PlanarFactor = 0.2;
const float BulgeFactor = 0.2;

void RunForever(Model &model) {
    const auto startTime = std::chrono::steady_clock::now();
    ctpl::thread_pool tp(4);
    int iterations = 0;
    while (1) {
        model.UpdateWithThreadPool(tp);
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

    Model model(
        triangles,
        SplitThreshold, LinkRestLength, RadiusOfInfluence,
        RepulsionFactor, SpringFactor, PlanarFactor, BulgeFactor);

    // RunGUI(model);
    RunForever(model);

    return 0;
}

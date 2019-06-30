#include <iostream>

#include "ctpl.h"
#include "gui.h"
#include "model.h"
#include "sphere.h"
#include "stl.h"
#include "util.h"

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
    float SplitThreshold = 2000;
    float LinkRestLength = 1;
    float RadiusOfInfluence = Random(LinkRestLength, LinkRestLength * 5);
    float RepulsionFactor = Random(0, 1);
    float SpringFactor = Random(0, 1);
    float PlanarFactor = Random(0, 1);
    float BulgeFactor = Random(0, 1);

    float sum = RepulsionFactor + SpringFactor + PlanarFactor + BulgeFactor;
    RepulsionFactor /= sum;
    SpringFactor /= sum;
    PlanarFactor /= sum;
    BulgeFactor /= sum;

    SplitThreshold    = 1000;
    LinkRestLength    = 1;
    RadiusOfInfluence = 1.5;
    RepulsionFactor   = 0.2;
    SpringFactor      = 0.4;
    PlanarFactor      = 0.4;
    BulgeFactor       = 0.2;

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

    RunGUI(model);
    // RunForever(model);

    return 0;
}

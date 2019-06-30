#include <iostream>
#include <thread>
#include <vector>

#include <xmmintrin.h>
#include <pmmintrin.h>

#include "camera.h"
#include "embree.h"
#include "hit.h"
#include "image.h"
#include "material.h"
#include "onb.h"
#include "ray.h"
#include "sphere.h"
#include "util.h"
#include "vec3.h"

const int w = 1920*4;
const int h = 1080*4;
const int ns = 16;
const int wn = 4;

Vec3 background(const Ray &ray) {
    return Vec3(0);
}

Vec3 iterative(const HittableList &world, const Ray &cameraRay) {
    const int minBounces = 4;
    const int maxBounces = 8;

    Vec3 color(0, 0, 0);
    Vec3 throughput(1, 1, 1);
    bool specular = true;
    Ray ray(cameraRay);

    const P_Hittable light = world.Lights()[0];

    for (int bounces = 0; bounces < maxBounces; bounces++) {
        HitInfo hit;
        if (!world.Hit(ray, eps, inf, hit)) {
            color = color + throughput * background(ray);
            break;
        }

        const Vec3 emitted = hit.Material->Emitted(0, 0, hit.Position);
        if (emitted.MaxComponent() > 0) {
            if (specular && Dot(hit.Normal, ray.Direction()) < 0) {
                color = color + throughput * emitted;
            }
            break;
        }

        const ONB onb(hit.Normal);
        const Vec3 p(hit.Position);
        const Vec3 wo(onb.WorldToLocal(Normalized(-ray.Direction())));

        Vec3 wi;
        real pdf;
        const Vec3 a = hit.Material->Sample_f(p, wo, wi, pdf, specular);

        // direct lighting
        if (!specular) {
            const Ray lightRay = light->RandomRay(p);
            HitInfo lightHit;
            if (world.Hit(lightRay, eps, inf, lightHit)) {
                const Vec3 Li = lightHit.Material->Emitted(0, 0, lightHit.Position);
                if (Li.MaxComponent() > 0 && Dot(lightHit.Normal, lightRay.Direction()) < 0) {
                    const real lightPdf = light->Pdf(lightRay);
                    const Vec3 lwi = onb.WorldToLocal(lightRay.Direction());
                    const Vec3 direct = hit.Material->f(p, wo, lwi) * Li / lightPdf;
                    color = color + throughput * direct * std::abs(lwi.Z());
                }
            }
        }

        // Vec3 a;
        // Vec3 wi;
        // real pdf;
        // if (Random() < 0.5) {
        //     // use light pdf
        //     const Ray lightRay = light->RandomRay(p);
        //     pdf = light->Pdf(lightRay);
        //     wi = onb.WorldToLocal(lightRay.Direction());
        //     a = hit.Material->f(p, wo, wi);
        //     pdf = (pdf + hit.Material->Pdf(wo, wi)) / 2;
        //     specular = false;
        // } else {
        //     // use material pdf
        //     a = hit.Material->Sample_f(p, wo, wi, pdf, specular);
        //     pdf = (pdf + light->Pdf(Ray(p, onb.LocalToWorld(wi)))) / 2;
        // }

        if (specular) {
            throughput = throughput * a;
        } else {
            if (pdf < eps) {
                break;
            }
            throughput = throughput * a * std::abs(wi.Z()) / pdf;
        }

        ray = Ray(p, onb.LocalToWorld(wi));

        if (bounces >= minBounces) {
            const real prob = throughput.MaxComponent();
            if (Random() > prob) {
                break;
            }
            throughput = throughput / prob;
        }
    }

    return color;
}

int main(int argc, char **argv) {
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

    HittableList world;

    const auto material = std::make_shared<Lambertian>(
        std::make_shared<SolidTexture>(HexColor(0x808080)));
    const auto geom = std::make_shared<EmbreeSpheres>(argv[1], material);

    world.Add(geom);

    const auto light = std::make_shared<DiffuseLight>(
        std::make_shared<SolidTexture>(Kelvin(5000) * 30));
    const auto L = std::make_shared<Sphere>(Vec3(3, 4, 1), 1, light);
    world.Add(L);
    world.AddLight(L);

    // const auto floorMaterial = std::make_shared<Lambertian>(
    //     std::make_shared<SolidTexture>(HexColor(0xFFFFFF)));
    // const auto floor = std::make_shared<Sphere>(Vec3(0, 0, -1000), 1000, floorMaterial);
    // world.Add(floor);

    const Vec3 eye(2, 1, 0);
    const Vec3 center(0, 0.035, 0);
    const Vec3 up(0, 1, 0);
    const real fovy = 16;
    const real aspect = real(w) / real(h);
    const real aperture = 0.003;
    const real focusDistance = (eye - center).Length();
    const Camera camera(eye, center, up, fovy, aspect, aperture, focusDistance);

    Image im(w, h);
    for (int frame = 1; ; frame++) {
        std::vector<std::thread> threads;
        for (int wi = 0; wi < wn; wi++) {
            threads.push_back(std::thread([&](int i) {
                _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
                _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
                for (int y = 0; y < h; y++) {
                    if (y % wn != i) {
                        continue;
                    }
                    for (int x = 0; x < w; x++) {
                        Vec3 c;
                        for (int s = 0; s < ns; s++) {
                            const real u = (x + Random()) / w;
                            const real v = (y + Random()) / h;
                            const Ray ray = camera.MakeRay(u, 1 - v);
                            c = c + iterative(world, ray);
                        }
                        im.Add(x, y, c);
                    }
                }
            }, wi));
        }
        for (int wi = 0; wi < wn; wi++) {
            threads[wi].join();
        }
        im.SavePPM("out.ppm", ns * frame);
        std::cout << (frame * ns) << std::endl;
    }
    return 0;
}

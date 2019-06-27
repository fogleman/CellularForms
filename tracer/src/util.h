#pragma once

#include <chrono>
#include <cmath>
#include <random>

#include "vec3.h"

inline real Random() {
    static thread_local std::mt19937 gen(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<real> dist(real(0), real(1));
    return dist(gen);
}

inline Vec3 RandomInUnitSphere() {
    while (true) {
        const Vec3 p = Vec3(
            Random() * 2 - 1,
            Random() * 2 - 1,
            Random() * 2 - 1);
        if (p.LengthSquared() < 1) {
            return p;
        }
    }
}

inline Vec3 RandomInUnitDisk() {
    while (true) {
        const Vec3 p = Vec3(
            Random() * 2 - 1,
            Random() * 2 - 1,
            0);
        if (p.LengthSquared() < 1) {
            return p;
        }
    }
}

inline Vec3 CosineSampleHemisphere() {
    const Vec3 d = RandomInUnitDisk();
    const real z = std::sqrt(std::max(real(0), 1 - d.X() * d.X() - d.Y() * d.Y()));
    return Vec3(d.X(), d.Y(), z);
}

inline Vec3 Reflect(const Vec3 &v, const Vec3 &n) {
    return v - 2 * Dot(v, n) * n;
}

inline bool Refract(const Vec3 &v, const Vec3 &n, const real ratio, Vec3 &refracted) {
    const Vec3 uv = Normalized(v);
    const real dt = Dot(uv, n);
    const real discriminant = 1 - ratio * ratio * (1 - dt * dt);
    if (discriminant <= 0) {
        return false;
    }
    refracted = ratio * (uv - n * dt) - n * std::sqrt(discriminant);
    return true;
}

inline real Schlick(const real cosine, const real index) {
    real r0 = (1 - index) / (1 + index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

inline Vec3 Kelvin(const real K) {
    real red, green, blue;
    // red
    if (K >= 6600) {
        const real a = 351.97690566805693;
        const real b = 0.114206453784165;
        const real c = -40.25366309332127;
        const real x = K/100 - 55;
        red = a + b*x + c*std::log(x);
    } else {
        red = 255;
    }
    // green
    if (K >= 6600) {
        const real a = 325.4494125711974;
        const real b = 0.07943456536662342;
        const real c = -28.0852963507957;
        const real x = K/100 - 50;
        green = a + b*x + c*std::log(x);
    } else if (K >= 1000) {
        const real a = -155.25485562709179;
        const real b = -0.44596950469579133;
        const real c = 104.49216199393888;
        const real x = K/100 - 2;
        green = a + b*x + c*std::log(x);
    } else {
        green = 0;
    }
    // blue
    if (K >= 6600) {
        blue = 255;
    } else if (K >= 2000) {
        const real a = -254.76935184120902;
        const real b = 0.8274096064007395;
        const real c = 115.67994401066147;
        const real x = K/100 - 10;
        blue = a + b*x + c*std::log(x);
    } else {
        blue = 0;
    }
    red = std::min(real(1), red / 255);
    green = std::min(real(1), green / 255);
    blue = std::min(real(1), blue / 255);
    return Vec3(red, green, blue);
}

inline real Clamp(const real value, const real lo, const real hi) {
    if (value <= lo) {
        return lo;
    }
    if (value >= hi) {
        return hi;
    }
    return value;
}

inline real CosTheta(const Vec3 &w) {
    return w.Z();
}

inline real AbsCosTheta(const Vec3 &w) {
    return std::abs(w.Z());
}

inline real SinTheta2(const Vec3 &w) {
    return std::max(real(0), 1 - CosTheta(w) * CosTheta(w));
}

inline real SinTheta(const Vec3 &w) {
    return std::sqrt(SinTheta2(w));
}

inline real CosPhi(const Vec3 &w) {
    const real sintheta = SinTheta(w);
    if (sintheta == 0) {
        return 1;
    }
    return Clamp(w.X() / sintheta, -1, 1);
}

inline real SinPhi(const Vec3 &w) {
    const real sintheta = SinTheta(w);
    if (sintheta == 0) {
        return 0;
    }
    return Clamp(w.Y() / sintheta, -1, 1);
}

inline bool SameHemisphere(const Vec3 &a, const Vec3 &b) {
    return a.Z() * b.Z() > 0;
}

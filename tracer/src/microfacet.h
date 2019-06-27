#pragma once

#include <cmath>
#include <memory>

#include "util.h"

class MicrofacetDistribution {
public:
    virtual real D(const Vec3 &wh) const = 0;
    virtual real Pdf(const Vec3 &wo, const Vec3 &wi) const = 0;
    virtual void Sample_f(const Vec3 &p, const Vec3 &wo, Vec3 &wi, real &pdf) const = 0;
    virtual ~MicrofacetDistribution() {}
};

typedef std::shared_ptr<MicrofacetDistribution> P_MicrofacetDistribution;

class BlinnDistribution : public MicrofacetDistribution {
public:
    BlinnDistribution(const real exponent) :
        m_Exponent(exponent) {}

    virtual real D(const Vec3 &wh) const {
        return (m_Exponent + 2) * std::pow(AbsCosTheta(wh), m_Exponent) / M_PI;
    }

    virtual real Pdf(const Vec3 &wo, const Vec3 &wi) const {
        const Vec3 wh = Normalized(wo + wi);
        if (Dot(wo, wh) <= 0) {
            return 0;
        }
        const real costheta = AbsCosTheta(wh);
        return ((m_Exponent + 1) * std::pow(costheta, m_Exponent)) / (2 * M_PI * 4 * Dot(wo, wh));
    }

    virtual void Sample_f(const Vec3 &p, const Vec3 &wo, Vec3 &wi, real &pdf) const {
        const real costheta = std::pow(Random(), 1 / (m_Exponent + 1));
        const real sintheta = std::sqrt(std::max(real(0), 1 - costheta * costheta));
        const real phi = Random() * 2 * M_PI;
        Vec3 wh = Vec3(sintheta * std::cos(phi), sintheta * std::sin(phi), costheta);
        if (wh.Z() * wo.Z() < 0) {
            wh = -wh;
        }
        wi = Normalized(-wo + 2 * Dot(wo, wh) * wh);
        pdf = Pdf(wo, wi);
    }

private:
    real m_Exponent;
};

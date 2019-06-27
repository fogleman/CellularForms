#pragma once

#include <iostream>

#include <memory>

#include "microfacet.h"
#include "ray.h"
#include "texture.h"
#include "util.h"
#include "vec3.h"

class Material {
public:
    virtual Vec3 f(const Vec3 &p, const Vec3 &wo, const Vec3 &wi) const = 0;

    virtual Vec3 Sample_f(const Vec3 &p, const Vec3 &wo, Vec3 &wi, real &pdf, bool &specular) const {
        wi = CosineSampleHemisphere();
        if (wo.Z() < 0) {
            wi = Vec3(wi.X(), wi.Y(), -wi.Z());
        }
        pdf = Pdf(wo, wi);
        specular = false;
        return f(p, wo, wi);
    }

    virtual real Pdf(const Vec3 &wo, const Vec3 &wi) const {
        if (wo.Z() * wi.Z() <= 0) {
            return 0;
        }
        return std::abs(wi.Z()) / M_PI;
    }

    virtual Vec3 Emitted(const real u, const real v, const Vec3 &p) const {
        return Vec3();
    }

    virtual ~Material() {}
};

typedef std::shared_ptr<Material> P_Material;

inline real G(const Vec3 &wo, const Vec3 &wi, const Vec3 &wh) {
    const real NdotWh = AbsCosTheta(wh);
    const real NdotWo = AbsCosTheta(wo);
    const real NdotWi = AbsCosTheta(wi);
    const real WOdotWh = std::abs(Dot(wo, wh));
    return std::min(real(1), std::min(
        2 * NdotWh * NdotWo / WOdotWh,
        2 * NdotWh * NdotWi / WOdotWh));
}

class FresnelBlend : public Material {
public:
    FresnelBlend(const P_Texture &Rd, const P_Texture &Rs, const P_MicrofacetDistribution &distribution) :
        m_Rd(Rd), m_Rs(Rs), m_Distribution(distribution) {}

    Vec3 SchlickFresnel(const Vec3 &rs, const real costheta) const {
        return rs + std::pow(1 - costheta, 5) * (Vec3(1) - rs);
    }

    virtual Vec3 f(const Vec3 &p, const Vec3 &wo, const Vec3 &wi) const {
        const Vec3 rd = m_Rd->Sample(0, 0, p);
        const Vec3 rs = m_Rs->Sample(0, 0, p);
        const Vec3 diffuse = (28 / (23 * M_PI)) * rd *
            (Vec3(1) - rs) *
            (1 - std::pow(1 - 0.5 * AbsCosTheta(wi), 5)) *
            (1 - std::pow(1 - 0.5 * AbsCosTheta(wo), 5));
        const Vec3 wh = Normalized(wi + wo);
        const Vec3 specular = m_Distribution->D(wh) /
            (4 * std::abs(Dot(wi, wh)) * std::max(AbsCosTheta(wi), AbsCosTheta(wo))) *
            SchlickFresnel(rs, Dot(wi, wh));
        return diffuse + specular;
    }

    virtual real Pdf(const Vec3 &wo, const Vec3 &wi) const {
        if (!SameHemisphere(wo, wi)) {
            return 0;
        }
        return 0.5 * (AbsCosTheta(wi) / M_PI + m_Distribution->Pdf(wo, wi));
    }

    virtual Vec3 Sample_f(const Vec3 &p, const Vec3 &wo, Vec3 &wi, real &pdf, bool &specular) const {
        if (Random() < 0.5) {
            wi = CosineSampleHemisphere();
            if (wo.Z() < 0) {
                wi = Vec3(wi.X(), wi.Y(), -wi.Z());
            }
        } else {
            m_Distribution->Sample_f(p, wo, wi, pdf);
            if (!SameHemisphere(wo, wi)) {
                return Vec3();
            }
        }
        pdf = Pdf(wo, wi);
        specular = false;
        return f(p, wo, wi);
    }

private:
    P_Texture m_Rd;
    P_Texture m_Rs;
    P_MicrofacetDistribution m_Distribution;
};

class Microfacet : public Material {
public:
    Microfacet(const P_Texture &albedo, const P_MicrofacetDistribution &distribution, const real eta) :
        m_Albedo(albedo), m_Distribution(distribution), m_Eta(eta) {}

    virtual Vec3 f(const Vec3 &p, const Vec3 &wo, const Vec3 &wi) const {
        const real cosThetaO = AbsCosTheta(wo);
        const real cosThetaI = AbsCosTheta(wi);
        if (cosThetaO == 0 || cosThetaI == 0) {
            return Vec3();
        }
        const Vec3 wh = Normalized(wi + wo);
        const real cosThetaH = Dot(wi, wh);
        const real F = Schlick(cosThetaH, m_Eta);
        const Vec3 R = m_Albedo->Sample(0, 0, p);
        return R * m_Distribution->D(wh) * G(wo, wi, wh) * F / (4 * cosThetaI * cosThetaO);
    }

    virtual real Pdf(const Vec3 &wo, const Vec3 &wi) const {
        if (!SameHemisphere(wo, wi)) {
            return 0;
        }
        return m_Distribution->Pdf(wo, wi);
    }

    virtual Vec3 Sample_f(const Vec3 &p, const Vec3 &wo, Vec3 &wi, real &pdf, bool &specular) const {
        m_Distribution->Sample_f(p, wo, wi, pdf);
        if (!SameHemisphere(wo, wi)) {
            return Vec3();
        }
        specular = false;
        return f(p, wo, wi);
    }

private:
    P_Texture m_Albedo;
    P_MicrofacetDistribution m_Distribution;
    real m_Eta;
};

class Metal : public Material {
public:
    Metal(const P_Texture &albedo) :
        m_Albedo(albedo) {}

    virtual Vec3 f(const Vec3 &p, const Vec3 &wo, const Vec3 &wi) const {
        return Vec3(0);
    }

    virtual real Pdf(const Vec3 &wo, const Vec3 &wi) const {
        return 0;
    }

    virtual Vec3 Sample_f(const Vec3 &p, const Vec3 &wo, Vec3 &wi, real &pdf, bool &specular) const {
        wi = Vec3(-wo.X(), -wo.Y(), wo.Z());
        pdf = 1;
        specular = true;
        return m_Albedo->Sample(0, 0, p);
    }

private:
    P_Texture m_Albedo;
};

class Dielectric : public Material {
public:
    Dielectric(const P_Texture &albedo, const real eta) :
        m_Albedo(albedo), m_Eta(eta) {}

    virtual Vec3 f(const Vec3 &p, const Vec3 &wo, const Vec3 &wi) const {
        return Vec3(0);
    }

    virtual real Pdf(const Vec3 &wo, const Vec3 &wi) const {
        return 0;
    }

    virtual Vec3 Sample_f(const Vec3 &p, const Vec3 &wo, Vec3 &wi, real &pdf, bool &specular) const {
        Vec3 outwardNormal;
        real ratio;
        if (wo.Z() < 0) {
            outwardNormal = Vec3(0, 0, -1);
            ratio = m_Eta;
        } else {
            outwardNormal = Vec3(0, 0, 1);
            ratio = 1 / m_Eta;
        }

        Vec3 refracted;
        real reflectProbability;
        if (Refract(-wo, outwardNormal, ratio, refracted)) {
            reflectProbability = Schlick(AbsCosTheta(wo), m_Eta);
        } else {
            reflectProbability = 1;
        }

        if (Random() < reflectProbability) {
            wi = Vec3(-wo.X(), -wo.Y(), wo.Z());
        } else {
            wi = refracted;
        }

        pdf = 1;
        specular = true;
        return m_Albedo->Sample(0, 0, p);
    }

private:
    P_Texture m_Albedo;
    real m_Eta;
};

class SpecularReflection : public Material {
public:
    SpecularReflection(const P_Texture &albedo, const real eta) :
        m_Albedo(albedo), m_Eta(eta) {}

    virtual Vec3 f(const Vec3 &p, const Vec3 &wo, const Vec3 &wi) const {
        return Vec3();
    }

    virtual Vec3 Sample_f(const Vec3 &p, const Vec3 &wo, Vec3 &wi, real &pdf, bool &specular) const {
        wi = Vec3(-wo.X(), -wo.Y(), wo.Z());
        pdf = 1;
        specular = true;
        const real fr = Schlick(AbsCosTheta(wo), m_Eta);
        return m_Albedo->Sample(0, 0, p) * fr;
    }

    virtual real Pdf(const Vec3 &wo, const Vec3 &wi) const {
        return 0;
    }

private:
    P_Texture m_Albedo;
    real m_Eta;
};

class Lambertian : public Material {
public:
    Lambertian(const P_Texture &albedo) :
        m_Albedo(albedo) {}

    virtual Vec3 f(const Vec3 &p, const Vec3 &wo, const Vec3 &wi) const {
        return m_Albedo->Sample(0, 0, p) / M_PI;
    }

private:
    P_Texture m_Albedo;
};

class OrenNayar : public Material {
public:
    OrenNayar(const P_Texture &albedo, const real sigma_degrees) :
        m_Albedo(albedo)
    {
        const real sigma = sigma_degrees * M_PI / 180;
        const real sigma2 = sigma * sigma;
        m_A = 1 - (sigma2 / (2 * (sigma2 + 0.33)));
        m_B = 0.45 * sigma2 / (sigma2 + 0.09);
    }

    virtual Vec3 f(const Vec3 &p, const Vec3 &wo, const Vec3 &wi) const {
        const real sinthetai = SinTheta(wi);
        const real sinthetao = SinTheta(wo);
        real maxcos = 0;
        if (sinthetai > eps && sinthetao > eps) {
            const real sinphii = SinPhi(wi);
            const real cosphii = CosPhi(wi);
            const real sinphio = SinPhi(wo);
            const real cosphio = CosPhi(wo);
            const real dcos = cosphii * cosphio + sinphii * sinphio;
            maxcos = std::max(real(0), dcos);
        }
        real sinalpha, tanbeta;
        if (AbsCosTheta(wi) > AbsCosTheta(wo)) {
            sinalpha = sinthetao;
            tanbeta = sinthetai / AbsCosTheta(wi);
        } else {
            sinalpha = sinthetai;
            tanbeta = sinthetao / AbsCosTheta(wo);
        }
        return m_Albedo->Sample(0, 0, p) * (m_A + m_B * maxcos * sinalpha * tanbeta) / M_PI;
    }

private:
    P_Texture m_Albedo;
    real m_A, m_B;
};

class DiffuseLight : public Material {
public:
    DiffuseLight(const P_Texture &emit) :
        m_Emit(emit) {}

    virtual Vec3 Emitted(const real u, const real v, const Vec3 &p) const {
        return m_Emit->Sample(u, v, p);
    }

    virtual Vec3 Sample_f(const Vec3 &p, const Vec3 &wo, Vec3 &wi, real &pdf, bool &specular) const {
        pdf = 0;
        specular = false;
        return Vec3();
    }

    virtual real Pdf(const Vec3 &wo, const Vec3 &wi) const {
        return 0;
    }

    virtual Vec3 f(const Vec3 &p, const Vec3 &wo, const Vec3 &wi) const {
        return Vec3();
    }

private:
    P_Texture m_Emit;
};

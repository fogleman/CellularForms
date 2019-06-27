#pragma once

#include <memory>

#include "vec3.h"

class Texture {
public:
    virtual Vec3 Sample(const real u, const real v, const Vec3 &p) const = 0;
    virtual ~Texture() {}
};

typedef std::shared_ptr<Texture> P_Texture;

class SolidTexture : public Texture {
public:
    SolidTexture(const Vec3 &color) :
        m_Color(color) {}

    virtual Vec3 Sample(const real u, const real v, const Vec3 &p) const {
        return m_Color;
    }

private:
    Vec3 m_Color;
};

class CheckeredTexture : public Texture {
public:
    CheckeredTexture(const P_Texture &a, const P_Texture &b, const real size) :
        m_A(a), m_B(b), m_Frequency(2 * M_PI / size) {}

    virtual Vec3 Sample(const real u, const real v, const Vec3 &p) const {
        const real f = m_Frequency;
        const real s = std::sin(f * p.X()) * std::sin(f * p.Y()) * std::sin(f * p.Z());
        if (s < 0) {
            return m_A->Sample(u, v, p);
        } else {
            return m_B->Sample(u, v, p);
        }
    }

private:
    P_Texture m_A;
    P_Texture m_B;
    real m_Frequency;
};

class GridTexture : public Texture {
public:
    GridTexture(const P_Texture &a, const P_Texture &b, const real lineSpacing, const real lineWidth) :
        m_A(a), m_B(b), m_LineSpacing(lineSpacing), m_LineWidth(lineWidth) {}

    virtual Vec3 Sample(const real u, const real v, const Vec3 &p) const {
        const Vec3 q = Abs(Fract(p / m_LineSpacing - 0.5) - 0.5);
        const real line = std::min(q.X(), q.Z());
        if (line > m_LineWidth / 2) {
            return m_A->Sample(u, v, p);
        } else {
            return m_B->Sample(u, v, p);
        }
    }

private:
    P_Texture m_A;
    P_Texture m_B;
    real m_LineSpacing;
    real m_LineWidth;
};

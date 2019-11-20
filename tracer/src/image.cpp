#include "image.h"

#include <cmath>
#include <fstream>

Image::Image(int width, int height) :
    m_Width(width), m_Height(height)
{
    m_Data.resize(width * height);
}

void Image::SavePPM(const std::string &path, const real divider) const {
    const real depth = 65535;
    const real multiplier = 1 / divider;
    const real exponent = 1 / 2.2;
    std::ofstream out(path);
    out << "P3\n";
    out << m_Width << " " << m_Height << "\n";
    out << depth << "\n";
    int i = 0;
    for (int y = 0; y < m_Height; y++) {
        for (int x = 0; x < m_Width; x++) {
            const Vec3 &c = Pow(m_Data[i++] * multiplier, exponent);
            const int r = std::min(c.R() * depth, depth);
            const int g = std::min(c.G() * depth, depth);
            const int b = std::min(c.B() * depth, depth);
            out << r << " " << g << " " << b << "\n";
        }
    }
    out.close();
}

void Image::SaveDepthPPM(const std::string &path, const real divider, const real min, const real max) const {
    const real depth = 65535;
    const real multiplier = 1 / divider;
    std::ofstream out(path);
    out << "P3\n";
    out << m_Width << " " << m_Height << "\n";
    out << depth << "\n";
    int i = 0;
    for (int y = 0; y < m_Height; y++) {
        for (int x = 0; x < m_Width; x++) {
            const Vec3 &c = (m_Data[i++] * multiplier - min) / (max - min);
            const int r = std::min(c.R() * depth, depth);
            const int g = std::min(c.G() * depth, depth);
            const int b = std::min(c.B() * depth, depth);
            out << r << " " << g << " " << b << "\n";
        }
    }
    out.close();
}

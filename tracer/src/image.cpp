#include "image.h"

#include <cmath>
#include <fstream>

Image::Image(int width, int height) :
    m_Width(width), m_Height(height)
{
    m_Data.resize(width * height);
}

void Image::SavePPM(const std::string &path, const real divider) const {
    const real multiplier = 1 / divider;
    const real exponent = 1 / 2.2;
    std::ofstream out(path);
    out << "P3\n";
    out << m_Width << " " << m_Height << "\n";
    out << 255 << "\n";
    int i = 0;
    for (int y = 0; y < m_Height; y++) {
        for (int x = 0; x < m_Width; x++) {
            const Vec3 &c = Pow(m_Data[i++] * multiplier, exponent);
            const int r = std::min(c.R() * 256, (real)255);
            const int g = std::min(c.G() * 256, (real)255);
            const int b = std::min(c.B() * 256, (real)255);
            out << r << " " << g << " " << b << "\n";
        }
    }
    out.close();
}

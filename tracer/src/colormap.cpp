#include "colormap.h"

#include <iostream>
#include <sstream>

#include "util.h"

Colormap::Colormap(const std::string &data) {
    for (int i = 0; i < data.size(); i += 6) {
        std::stringstream ss;
        ss << std::hex << data.substr(i, 6);
        int x;
        ss >> x;
        m_Colors.push_back(HexColor(x));
    }
}

Vec3 Colormap::At(const double t) const {
    const int n = m_Colors.size();

    if (t <= 0) {
        return m_Colors.front();
    }
    if (t >= 1) {
        return m_Colors.back();
    }

    const int i = t * (n - 1);
    const double s = 1.0 / (n - 1);
    const double p = (t - i * s) / s;

    const Vec3 c0 = m_Colors[i];
    const Vec3 c1 = m_Colors[i+1];

    if (p <= 0) {
        return c0;
    }
    if (p >= 1) {
        return c1;
    }

    const double r = Clamp(c0.R() + (c1.R() - c0.R()) * p, 0, 1);
    const double g = Clamp(c0.G() + (c1.G() - c0.G()) * p, 0, 1);
    const double b = Clamp(c0.B() + (c1.B() - c0.B()) * p, 0, 1);

    return Vec3(r, g, b);
}

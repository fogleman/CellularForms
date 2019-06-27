#pragma once

#include <string>
#include <vector>

#include "vec3.h"

class Image {
public:
    Image(int width, int height);

    int Width() const {
        return m_Width;
    }

    int Height() const {
        return m_Height;
    }

    const Vec3 &Get(int x, int y) const {
        return m_Data[y * m_Width + x];
    }

    void Set(int x, int y, const Vec3 &c) {
        m_Data[y * m_Width + x] = c;
    }

    void Add(int x, int y, const Vec3 &c) {
        const int i = y * m_Width + x;
        m_Data[i] = m_Data[i] + c;
    }

    void SavePPM(const std::string &path, const real divider = 1) const;

private:
    int m_Width;
    int m_Height;
    std::vector<Vec3> m_Data;
};

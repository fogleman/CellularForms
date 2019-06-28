#include "stl.h"

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <fstream>

using namespace boost::interprocess;

std::vector<Triangle> LoadBinarySTL(std::string path) {
    file_mapping fm(path.c_str(), read_only);
    mapped_region mr(fm, read_only);
    uint8_t *src = (uint8_t *)mr.get_address();
    const int numBytes = mr.get_size();
    const int numTriangles = std::max(0, (numBytes - 84) / 50);
    const int numVertices = numTriangles * 3;
    std::vector<glm::vec3> positions(numVertices);
    auto dst = positions.data();
    src += 96;
    for (int i = 0; i < numTriangles; i++) {
        memcpy(dst, src, 36);
        src += 50;
        dst += 3;
    }
    std::vector<Triangle> triangles;
    triangles.reserve(numTriangles);
    for (int i = 0; i < positions.size(); i += 3) {
        triangles.emplace_back(
            positions[i+0],
            positions[i+1],
            positions[i+2]);
    }
    return triangles;
}

void SaveBinarySTL(std::string path, const std::vector<Triangle> &triangles) {
    const uint64_t numBytes = uint64_t(triangles.size()) * 50 + 84;

    {
        file_mapping::remove(path.c_str());
        std::filebuf fbuf;
        fbuf.open(path.c_str(),
            std::ios_base::in | std::ios_base::out | std::ios_base::trunc |
            std::ios_base::binary);
        fbuf.pubseekoff(numBytes - 1, std::ios_base::beg);
        fbuf.sputc(0);
    }

    file_mapping fm(path.c_str(), read_write);
    mapped_region mr(fm, read_write);
    uint8_t *dst = (uint8_t *)mr.get_address();

    const uint32_t count = triangles.size();
    memcpy(dst + 80, &count, 4);

    for (uint32_t i = 0; i < triangles.size(); i++) {
        const Triangle &t = triangles[i];
        const glm::vec3 normal = t.Normal();
        const uint64_t idx = 84 + i * 50;
        memcpy(dst + idx + 0, &normal, 12);
        memcpy(dst + idx + 12, &t.A(), 12);
        memcpy(dst + idx + 24, &t.B(), 12);
        memcpy(dst + idx + 36, &t.C(), 12);
    }
}

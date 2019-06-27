#include "stl.h"

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace boost::interprocess;

std::vector<Vec3> LoadBinarySTL(std::string path) {
    file_mapping fm(path.c_str(), read_only);
    mapped_region mr(fm, read_only);
    uint8_t *src = (uint8_t *)mr.get_address();
    const int numBytes = mr.get_size();
    const int numTriangles = std::max(0, (numBytes - 84) / 50);
    const int numVertices = numTriangles * 3;
    std::vector<Vec3> result(numVertices);
    src += 96;
    for (int i = 0; i < numTriangles; i++) {
        const float *p = (float *)src;
        result[i*3+0] = Vec3(p[0], p[1], p[2]);
        result[i*3+1] = Vec3(p[3], p[4], p[5]);
        result[i*3+2] = Vec3(p[6], p[7], p[8]);
        src += 50;
    }
    return result;
}

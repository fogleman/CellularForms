#include "index.h"

#include "util.h"

glm::ivec3 Index::KeyForPoint(const glm::vec3 &point) const {
    const int x = std::roundf(point.x / m_CellSize);
    const int y = std::roundf(point.y / m_CellSize);
    const int z = std::roundf(point.z / m_CellSize);
    return glm::ivec3(x, y, z);
}

void Index::Add(const glm::vec3 &point, const int id) {
    const glm::ivec3 key = KeyForPoint(point);
    const auto k0 = key - 1;
    const auto k1 = key + 1;
    for (int x = k0.x; x <= k1.x; x++) {
        for (int y = k0.y; y <= k1.y; y++) {
            for (int z = k0.z; z <= k1.z; z++) {
                auto &ids = m_Cells[glm::ivec3(x, y, z)];
                // const auto it = std::find(ids.begin(), ids.end(), id);
                // if (it != ids.end()) {
                //     Panic("id already present in Add");
                // }
                ids.push_back(id);
            }
        }
    }
}

void Index::Remove(const glm::vec3 &point, const int id) {
    const glm::ivec3 key = KeyForPoint(point);
    const auto k0 = key - 1;
    const auto k1 = key + 1;
    for (int x = k0.x; x <= k1.x; x++) {
        for (int y = k0.y; y <= k1.y; y++) {
            for (int z = k0.z; z <= k1.z; z++) {
                auto &ids = m_Cells[glm::ivec3(x, y, z)];
                const auto it = std::find(ids.begin(), ids.end(), id);
                // if (it == ids.end()) {
                //     Panic("id not found in Remove");
                // }
                std::swap(*it, ids.back());
                ids.pop_back();
            }
        }
    }
}

void Index::Update(const glm::vec3 &p0, const glm::vec3 &p1, const int id) {
    const auto k0 = KeyForPoint(p0);
    const auto k1 = KeyForPoint(p1);
    if (k0 != k1) {
        Remove(p0, id);
        Add(p1, id);
    }
}

const std::vector<int> &Index::Nearby(const glm::vec3 &point) const {
    const auto it = m_Cells.find(KeyForPoint(point));
    if (it == m_Cells.end()) {
        return m_Empty;
    }
    return it->second;
}

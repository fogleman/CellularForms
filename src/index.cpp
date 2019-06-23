#include "index.h"

glm::ivec3 Index::KeyForPoint(const glm::vec3 &point) const {
    const int x = std::roundf(point.x / m_CellSize);
    const int y = std::roundf(point.y / m_CellSize);
    const int z = std::roundf(point.z / m_CellSize);
    return glm::ivec3(x, y, z);
}

void Index::Add(const glm::vec3 &point, const int id) {
    m_Cells[KeyForPoint(point)].push_back(id);
}

void Index::Remove(const glm::vec3 &point, const int id) {
    auto &ids = m_Cells[KeyForPoint(point)];
    const auto it = std::find(ids.begin(), ids.end(), id);
    std::swap(*it, ids.back());
    ids.pop_back();
}

void Index::Update(const glm::vec3 &p0, const glm::vec3 &p1, const int id) {
    const auto k0 = KeyForPoint(p0);
    const auto k1 = KeyForPoint(p1);
    if (k0 != k1) {
        Remove(p0, id);
        Add(p1, id);
    }
}

void Index::Search(
    const glm::vec3 &point,
    const float distance,
    std::vector<int> &result) const
{
    const auto k0 = KeyForPoint(point - distance);
    const auto k1 = KeyForPoint(point + distance);
    for (int x = k0.x; x <= k1.x; x++) {
        for (int y = k0.y; y <= k1.y; y++) {
            for (int z = k0.z; z <= k1.z; z++) {
                const auto it = m_Cells.find(glm::ivec3(x, y, z));
                if (it != m_Cells.end()) {
                    result.insert(
                        result.end(), it->second.begin(), it->second.end());
                }
            }
        }
    }
}

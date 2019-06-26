#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>
#include <vector>

class Index {
public:
    Index(const float cellSize) :
        m_CellSize(cellSize) {}

    glm::ivec3 KeyForPoint(const glm::vec3 &point) const;

    const std::vector<int> &Nearby(const glm::vec3 &point) const;

    void Add(const glm::vec3 &point, const int id);

    void Remove(const glm::vec3 &point, const int id);

    bool Update(const glm::vec3 &p0, const glm::vec3 &p1, const int id);

private:
    float m_CellSize;
    std::unordered_map<glm::ivec3, std::vector<int>> m_Cells;
    std::vector<int> m_Empty;
};

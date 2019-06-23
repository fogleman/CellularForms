#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>
#include <vector>

// XYZ => bucket
// points within distance (compute *which* neigboring buckets)
// point IDs
// update point

class Index {
public:
    Index(const float cellSize) :
        m_CellSize(cellSize) {}

    glm::ivec3 KeyForPoint(const glm::vec3 &point) const;

    void Add(const glm::vec3 &point, const int id);

    void Remove(const glm::vec3 &point, const int id);

    void Update(const glm::vec3 &p0, const glm::vec3 &p1, const int id);

    void Search(
        const glm::vec3 &point,
        const float distance,
        std::vector<int> &result) const;

private:
    float m_CellSize;
    std::unordered_map<glm::ivec3, std::vector<int>> m_Cells;
};

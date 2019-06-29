#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <mutex>
#include <unordered_map>
#include <vector>

class Index {
public:
    Index(const float cellSize);

    glm::ivec3 KeyForPoint(const glm::vec3 &point) const;

    int IndexForKey(const glm::ivec3 &key) const;

    const std::vector<int> &Nearby(const glm::vec3 &point) const;

    void Add(const glm::vec3 &point, const int id);

    void Remove(const glm::vec3 &point, const int id);

    bool Update(const glm::vec3 &p0, const glm::vec3 &p1, const int id);

private:
    float m_CellSize;
    glm::ivec3 m_Start;
    glm::ivec3 m_Size;
    std::vector<std::vector<int>> m_Cells;
    std::vector<std::mutex> m_Locks;
};

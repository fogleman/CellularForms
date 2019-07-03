#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "index.h"
#include "pool.h"
#include "triangle.h"

class Model {
public:
    Model(
        const std::vector<Triangle> &triangles,
        const float splitThreshold,
        const float linkRestLength,
        const float radiusOfInfluence,
        const float repulsionFactor,
        const float springFactor,
        const float planarFactor,
        const float bulgeFactor);

    // getter methods
    const std::vector<glm::vec3> &Positions() const { return m_Positions; }
    const std::vector<glm::vec3> &Normals() const { return m_Normals; }
    const std::vector<float> &Food() const { return m_Food; }
    const std::vector<std::vector<int>> &Links() const { return m_Links; }
    float SplitThreshold() const { return m_SplitThreshold; }
    float LinkRestLength() const { return m_LinkRestLength; }
    float RadiusOfInfluence() const { return m_RadiusOfInfluence; }
    float RepulsionFactor() const { return m_RepulsionFactor; }
    float SpringFactor() const { return m_SpringFactor; }
    float PlanarFactor() const { return m_PlanarFactor; }
    float BulgeFactor() const { return m_BulgeFactor; }

    // Bounds computes the min / max bounds of all cells
    void Bounds(glm::vec3 &min, glm::vec3 &max) const;

    // Update runs one iteration of simulation using the provided thread pool
    void Update(ThreadPool &pool, const bool split = true);

    void Split(const int i);

    void Merge(const int mergeIndex, const int removeIndex);

    void Remove(const int removeIndex);

    std::vector<Triangle> Triangulate() const;

    void TriangleIndexes(std::vector<glm::uvec3> &result) const;

    void VertexAttributes(std::vector<float> &result) const;

private:
    void Ensure();

    void UpdateBatch(const int wi, const int wn);

    glm::vec3 CellNormal(const int index) const;

    void ChangeLink(const int i, const int from, const int to);

    void RemoveLink(const int i, const int link);

    void InsertLinkBefore(const int i, const int before, const int link);

    void InsertLinkAfter(const int i, const int after, const int link);

    // amount of food required for a cell to split
    float m_SplitThreshold;

    // preferred distance between linked cells
    float m_LinkRestLength;

    // distance at which non-linked cells will repel each other
    float m_RadiusOfInfluence;

    // weights
    float m_RepulsionFactor;
    float m_SpringFactor;
    float m_PlanarFactor;
    float m_BulgeFactor;

    // position of each cell
    std::vector<glm::vec3> m_Positions;

    // normal of each cell
    std::vector<glm::vec3> m_Normals;

    // food level of each cell
    std::vector<float> m_Food;

    // alive / dead status of each cell
    std::vector<bool> m_Alive;

    // list of indexes of linked cells
    std::vector<std::vector<int>> m_Links;

    // spatial hash index
    Index m_Index;

    // buffers
    std::vector<glm::vec3> m_NewPositions;
    std::vector<glm::vec3> m_NewNormals;
};

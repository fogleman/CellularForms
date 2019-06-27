#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "ctpl.h"
#include "index.h"
#include "triangle.h"

class Model {
public:
    Model(const std::vector<Triangle> &triangles);

    const std::vector<glm::vec3> &Positions() const {
        return m_Positions;
    }

    const std::vector<glm::vec3> &Normals() const {
        return m_Normals;
    }

    const std::vector<std::vector<int>> &Links() const {
        return m_Links;
    }

    void UpdateWithThreadPool(ctpl::thread_pool &tp);

    void UpdateBatch(const int wi, const int wn);

    glm::vec3 CellNormal(const int index) const;

    void ChangeLink(const int i, const int from, const int to);

    void InsertLinkBefore(const int i, const int before, const int link);

    void InsertLinkAfter(const int i, const int after, const int link);

    void Split(const int i);

    std::vector<Triangle> Triangulate() const;

    void TriangleIndexes(std::vector<glm::uvec3> &result) const;

    void VertexAttributes(std::vector<float> &result) const;

private:
    float m_LinkRestLength;
    float m_SpringFactor;
    float m_PlanarFactor;
    float m_BulgeFactor;
    float m_RepulsionFactor;
    float m_RadiusOfInfluence;
    float m_SplitThreshold;

    // position of each cell
    std::vector<glm::vec3> m_Positions;

    // normal of each cell
    std::vector<glm::vec3> m_Normals;

    // food level of each cell
    std::vector<float> m_Food;

    // list of indexes of linked cells
    std::vector<std::vector<int>> m_Links;

    // spatial hash index
    Index m_Index;

    // buffers
    std::vector<glm::vec3> m_NewPositions;
    std::vector<glm::vec3> m_NewNormals;
    std::vector<float> m_NewFood;
};

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

    void Update();

    void UpdateWithThreadPool(ctpl::thread_pool &tp);

    void UpdateBatch(
        const int wi, const int wn,
        std::vector<glm::vec3> &newPositions,
        std::vector<glm::vec3> &newNormals) const;

    void UpdateFood();

    bool Linked(const int i, const int j) const;

    std::vector<int> OrderedLinks(const int parentIndex) const;

    void Split(const int i);

    void Link(const int i0, const int i1);

    void Unlink(const int i0, const int i1);

    std::vector<Triangle> Triangulate() const;

private:
    float m_LinkRestLength;
    float m_SpringFactor;
    float m_PlanarFactor;
    float m_BulgeFactor;
    float m_RepulsionFactor;
    float m_RadiusOfInfluence;

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
};

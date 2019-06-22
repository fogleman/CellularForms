#pragma once

#include <glm/glm.hpp>
#include <vector>

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

    void Split(const int index);

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

    // list of indexes of linked cells
    std::vector<std::vector<int>> m_Links;
};

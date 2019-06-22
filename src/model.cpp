#include "model.h"

#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <unordered_map>

#include "util.h"

Model::Model(const std::vector<Triangle> &triangles) {
    // default parameters
    m_LinkRestLength = 1;
    m_SpringFactor = 1;
    m_PlanarFactor = 1;
    m_BulgeFactor = 1;
    m_RadiusOfInfluence = 1;
    m_RepulsionStrength = 1;

    // find unique vertices
    std::unordered_map<glm::vec3, int> indexes;
    for (const auto &t : triangles) {
        for (const auto &v : {t.A(), t.B(), t.C()}) {
            if (indexes.find(v) == indexes.end()) {
                indexes[v] = m_Cells.size();
                m_Cells.push_back(v);
            }
        }
    }

    // create links
    m_Links.resize(m_Cells.size());
    for (const auto &t : triangles) {
        const int a = indexes[t.A()];
        const int b = indexes[t.B()];
        const int c = indexes[t.C()];
        m_Links[a].push_back(b);
        m_Links[a].push_back(c);
        m_Links[b].push_back(a);
        m_Links[b].push_back(c);
        m_Links[c].push_back(a);
        m_Links[c].push_back(b);
    }

    // make links unique
    for (int i = 0; i < m_Links.size(); i++) {
        auto &links = m_Links[i];
        std::sort(links.begin(), links.end());
        links.resize(std::distance(
            links.begin(), std::unique(links.begin(), links.end())));
    }
}

void Model::Update() {
    std::vector<glm::vec3> linkedCells;
    std::vector<glm::vec3> cells;
    cells.resize(m_Cells.size());
    for (int i = 0; i < m_Cells.size(); i++) {
        // get linked cells
        linkedCells.resize(0);
        for (const int j : m_Links[i]) {
            linkedCells.push_back(m_Cells[j]);
        }

        // get point and normal
        const glm::vec3 &P = m_Cells[i];
        const glm::vec3 N = PlaneNormalFromPoints(linkedCells);

        // accumulate
        glm::vec3 springTarget(0);
        glm::vec3 planarTarget(0);
        float bulgeDistance = 0;
        for (const glm::vec3 &L : linkedCells) {
            springTarget += L + glm::normalize(P - L) * m_LinkRestLength;
            planarTarget += L;
            const glm::vec3 D = L - P;
            // std::cout << glm::length(D) << std::endl;
            if (m_LinkRestLength > glm::length(D)) {
                const float dot = glm::dot(D, N);
                bulgeDistance += std::sqrt(
                    m_LinkRestLength * m_LinkRestLength -
                    glm::dot(D, D) + dot * dot) + dot;
            }
        }

        // average
        const float m = 1 / static_cast<float>(linkedCells.size());
        springTarget *= m;
        planarTarget *= m;
        bulgeDistance *= m;
        const glm::vec3 bulgeTarget = P + bulgeDistance * N;

        cells[i] = P +
            0.70f * m_SpringFactor * (springTarget - P) +
            0.20f * m_PlanarFactor * (planarTarget - P) +
            0.05f * m_BulgeFactor * (bulgeTarget - P);
    }
    m_Cells = cells;
}

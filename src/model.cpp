#include "model.h"

#include <glm/gtx/hash.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <unordered_map>

#include "util.h"

Model::Model(const std::vector<Triangle> &triangles) {
    // default parameters
    m_LinkRestLength = 1;
    m_SpringFactor = 1;
    m_PlanarFactor = 0.1;
    m_BulgeFactor = 0.01;
    m_RepulsionFactor = 1;
    m_RadiusOfInfluence = 1;

    // find unique vertices
    std::unordered_map<glm::vec3, int> indexes;
    std::unordered_map<glm::vec3, glm::vec3> normals;
    for (const auto &t : triangles) {
        const glm::vec3 normal = t.Normal();
        for (const auto &v : {t.A(), t.B(), t.C()}) {
            if (indexes.find(v) == indexes.end()) {
                normals[v] = normal;
                indexes[v] = m_Positions.size();
                m_Positions.push_back(v);
            } else {
                normals[v] += normal;
            }
        }
    }

    // create normals
    m_Normals.resize(m_Positions.size());
    for (const auto &el : indexes) {
        m_Normals[el.second] = glm::normalize(normals[el.first]);
    }

    // create links
    m_Links.resize(m_Positions.size());
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
    std::vector<glm::vec3> positions;
    positions.resize(m_Positions.size());
    for (int i = 0; i < m_Positions.size(); i++) {
        // get linked cells
        linkedCells.resize(0);
        for (const int j : m_Links[i]) {
            linkedCells.push_back(m_Positions[j]);
        }

        // get cell position
        const glm::vec3 &P = m_Positions[i];

        // update normal
        linkedCells.push_back(P);
        const glm::vec3 N = PlaneNormalFromPoints(linkedCells, m_Normals[i]);
        linkedCells.pop_back();
        m_Normals[i] = N;

        // accumulate
        glm::vec3 springTarget(0);
        glm::vec3 planarTarget(0);
        float bulgeDistance = 0;
        for (const glm::vec3 &L : linkedCells) {
            springTarget += L + glm::normalize(P - L) * m_LinkRestLength;
            planarTarget += L;
            const glm::vec3 D = L - P;
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

        // repulsion
        glm::vec3 repulsionVector(0);
        const float roi2 = m_RadiusOfInfluence * m_RadiusOfInfluence;
        for (int j = 0; j < m_Positions.size(); j++) {
            if (j == i) {
                continue;
            }
            const auto &links = m_Links[i];
            if (std::find(links.begin(), links.end(), j) != links.end()) {
                continue;
            }
            const glm::vec3 D = P - m_Positions[j];
            const float d2 = glm::length2(D);
            if (d2 >= roi2) {
                continue;
            }
            const float d = (roi2 - d2) / roi2;
            repulsionVector += glm::normalize(D) * d;
        }

        // new position
        positions[i] = P +
            m_SpringFactor * (springTarget - P) +
            m_PlanarFactor * (planarTarget - P) +
            (m_BulgeFactor * bulgeDistance) * N +
            m_RepulsionFactor * repulsionVector;
    }
    m_Positions = positions;

    // split one randomly
    Split(RandomIntN(m_Positions.size()));
}

void Model::Split(const int parentIndex) {
    // get links (make a copy)
    const std::vector<int> parentLinks = m_Links[parentIndex];

    // select a linked cell at random
    const int i0 = parentLinks[RandomIntN(parentLinks.size())];
    const glm::vec3 &L0 = m_Positions[i0];

    // find another linked cell furthest from the selected one
    const int i1 = [this, &parentLinks, &L0]() {
        int furthestIndex = 0;
        float furthestDistance = 0;
        for (const int j : parentLinks) {
            const float d = glm::distance(L0, m_Positions[j]);
            if (d > furthestDistance) {
                furthestDistance = d;
                furthestIndex = j;
            }
        }
        return furthestIndex;
    }();
    const glm::vec3 &L1 = m_Positions[i1];

    // compute position and normal
    const glm::vec3 &P = m_Positions[parentIndex];
    const glm::vec3 N = glm::normalize(glm::cross(L0 - P, L1 - P));
    // std::cout << glm::to_string(N) << std::endl;

    // create the child in the same spot as the parent for now
    const int childIndex = m_Links.size();
    m_Positions.push_back(m_Positions[parentIndex]);
    m_Normals.push_back(m_Normals[parentIndex]);
    m_Links.emplace_back();

    // All the links to one side of the plane of cleavage are left connected to
    // the parent cell, while the links to the other side are disconnected from
    // the parent and replaced with links to the daughter cell.
    for (const int j : parentLinks) {
        if (j == i0 || j == i1) {
            continue;
        }
        const glm::vec3 &L = m_Positions[j];
        if (glm::dot(N, L - P) >= 0) {
            continue;
        }
        Unlink(parentIndex, j);
        Link(childIndex, j);
    }

    // Along the plane of cleavage links are made to both the parent and
    // daughter cells. A new link is also created directly between the parent
    // and daughter.
    Link(childIndex, parentIndex);
    Link(childIndex, i0);
    Link(childIndex, i1);

    // finally, move the parent and child apart a bit
    m_Positions[parentIndex] += N * m_LinkRestLength * 0.1f;
    m_Positions[childIndex] -= N * m_LinkRestLength * 0.1f;
}

void Model::Link(const int i0, const int i1) {
    m_Links[i0].push_back(i1);
    m_Links[i1].push_back(i0);
}

void Model::Unlink(const int i0, const int i1) {
    auto &links0 = m_Links[i0];
    auto &links1 = m_Links[i1];
    const auto f0 = std::find(links0.begin(), links0.end(), i1);
    const auto f1 = std::find(links1.begin(), links1.end(), i0);
    std::iter_swap(f0, links0.end() - 1);
    std::iter_swap(f1, links1.end() - 1);
    links0.pop_back();
    links1.pop_back();
}

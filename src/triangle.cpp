#include "triangle.h"

#include <glm/gtx/normal.hpp>

#include "util.h"

glm::vec3 Triangle::Normal() const {
    return glm::triangleNormal(m_A, m_B, m_C);
}

const glm::vec3 &Triangle::VertexAfter(const glm::vec3 &point) const {
    if (m_A == point) {
        return m_B;
    } else if (m_B == point) {
        return m_C;
    } else if (m_C == point) {
        return m_A;
    } else {
        Panic("point not found in VertexAfter");
        return m_A;
    }
}

const glm::vec3 &Triangle::VertexBefore(const glm::vec3 &point) const {
    if (m_A == point) {
        return m_C;
    } else if (m_B == point) {
        return m_A;
    } else if (m_C == point) {
        return m_B;
    } else {
        Panic("point not found in VertexBefore");
        return m_A;
    }
}

#pragma once

#include <glm/glm.hpp>
#include <vector>

class Triangle {
public:
    Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) :
        m_A(a), m_B(b), m_C(c) {}

    const glm::vec3 &A() const {
        return m_A;
    }

    const glm::vec3 &B() const {
        return m_B;
    }

    const glm::vec3 &C() const {
        return m_C;
    }

    glm::vec3 Normal() const;

    const glm::vec3 &VertexAfter(const glm::vec3 &point) const;

    const glm::vec3 &VertexBefore(const glm::vec3 &point) const;

private:
    glm::vec3 m_A;
    glm::vec3 m_B;
    glm::vec3 m_C;
};

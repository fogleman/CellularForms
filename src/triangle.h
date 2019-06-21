#pragma once

#include <glm/glm.hpp>

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

private:
    glm::vec3 m_A;
    glm::vec3 m_B;
    glm::vec3 m_C;
};

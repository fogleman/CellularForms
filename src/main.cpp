#define GL_SILENCE_DEPRECATION

#include <chrono>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>

#include "ctpl.h"
#include "model.h"
#include "program.h"
#include "sphere.h"
#include "stl.h"

std::string vertexSource = R"(
#version 120

uniform mat4 matrix;

attribute vec4 position;

varying vec3 ec_pos;

void main() {
    gl_Position = matrix * position;
    ec_pos = vec3(gl_Position);
}
)";

std::string fragmentSource = R"(
#version 120

varying vec3 ec_pos;

const vec3 light_direction0 = normalize(vec3(0.5, -2, 1));
const vec3 light_direction1 = normalize(vec3(-0.5, -1, 1));
// const vec3 object_color = vec3(0x30 / 255.0, 0x73 / 255.0, 0x47 / 255.0);
const vec3 color0 = vec3(0.0, 0.15, 0.11);
const vec3 color1 = vec3(0.59, 0.93, 0.54);
// const vec3 color0 = vec3(0.12, 0.12, 0.13);
// const vec3 color1 = vec3(0.86, 0.21, 0.13);

void main() {
    vec3 ec_normal = normalize(cross(dFdx(ec_pos), dFdy(ec_pos)));
    float diffuse0 = max(0, dot(ec_normal, light_direction0));
    float diffuse1 = max(0, dot(ec_normal, light_direction1));
    float diffuse = diffuse0 * 0.75 + diffuse1 * 0.25;
    // vec3 color = object_color * diffuse;
    vec3 color = mix(color0, color1, diffuse);
    gl_FragColor = vec4(color, 1);
}
)";

int main() {
    auto startTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed;

    const auto sphereTriangles = SphereTriangles(1);
    Model model(sphereTriangles);
    ctpl::thread_pool tp(4);

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    GLFWwindow *window = glfwCreateWindow(800, 800, "Cellular Forms", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor((float)0x2a/255, (float)0x2c/255, (float)0x2b/255, 1);

    Program p(vertexSource, fragmentSource);

    const auto positionAttrib = p.GetAttribLocation("position");
    const auto matrixUniform = p.GetUniformLocation("matrix");

    const glm::vec3 minPosition(-30);
    const glm::vec3 maxPosition(30);

    glm::vec3 size = maxPosition - minPosition;
    glm::vec3 center = (minPosition + maxPosition) / 2.0f;
    const float scale = glm::compMax(glm::vec3(2) / size);
    glm::mat4 modelTransform =
        glm::scale(glm::mat4(1.0f), glm::vec3(scale)) *
        glm::translate(glm::mat4(1.0f), -center);

    GLuint arrayBuffer;
    GLuint elementBuffer;
    glGenBuffers(1, &arrayBuffer);
    glGenBuffers(1, &elementBuffer);

    std::vector<glm::uvec3> indexes;

    const auto update = [&]() {
        const std::vector<glm::vec3> &positions = model.Positions();

        indexes.resize(0);
        model.TriangleIndexes(indexes);

        glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            positions.size() * sizeof(positions.front()),
            positions.data(),
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indexes.size() * sizeof(indexes.front()),
            indexes.data(),
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    };

    while (!glfwWindowShouldClose(window)) {
        elapsed = std::chrono::steady_clock::now() - startTime;

        if (elapsed.count() > 0) {
            model.UpdateWithThreadPool(tp);
        }

        update();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        p.Use();

        int w, h;
        glfwGetWindowSize(window, &w, &h);
        const float aspect = (float)w / (float)h;
        const float angle = elapsed.count() * 3;
        glm::mat4 rotation = glm::rotate(
            glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 0, 1));
        glm::mat4 projection = glm::perspective(
            glm::radians(30.f), aspect, 1.f, 1000.f);
        glm::vec3 eye(0, -5, 0);
        glm::vec3 center(0);
        glm::vec3 up(0, 0, 1);
        glm::mat4 lookAt = glm::lookAt(eye, center, up);
        glm::mat4 matrix = projection * lookAt * rotation * modelTransform;
        glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, glm::value_ptr(matrix));

        glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glEnableVertexAttribArray(positionAttrib);
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, false, 12, 0);
        glDrawElements(GL_TRIANGLES, indexes.size() * 3, GL_UNSIGNED_INT, 0);
        glDisableVertexAttribArray(positionAttrib);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// #include <glm/gtx/string_cast.hpp>
// #include <iostream>

// #include "ctpl.h"
// #include "model.h"
// #include "sphere.h"
// #include "stl.h"
// #include "util.h"

// int main() {
//     const auto sphereTriangles = SphereTriangles(1);
//     Model model(sphereTriangles);

//     ctpl::thread_pool tp(4);

//     for (int i = 0; ; i++) {
//         const int n = model.Positions().size();
//         std::cerr << i << ": " << n << std::endl;
//         model.UpdateWithThreadPool(tp);
//         if (n > 10752*2) {
//             break;
//         }
//     }

//     const auto &positions = model.Positions();
//     const auto &links = model.Links();
//     for (int i = 0; i < positions.size(); i++) {
//         for (const int j : links[i]) {
//             if (j < i) {
//                 continue;
//             }
//             const auto p = positions[i];
//             const auto q = positions[j];
//             printf("%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n", p.x, p.y, p.z, q.x, q.y, q.z);
//         }
//     }

//     const auto triangles = model.Triangulate();
//     SaveBinarySTL("out.stl", triangles);
// }

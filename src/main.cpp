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
attribute vec3 normal;

varying vec3 ec_pos;
varying vec3 ec_normal;

void main() {
    gl_Position = matrix * position;
    ec_pos = vec3(gl_Position);
    ec_normal = normal;
}
)";

std::string fragmentSource = R"(
#version 120

varying vec3 ec_pos;
varying vec3 ec_normal;

const vec3 light_direction0 = normalize(vec3(0.5, -2, 1));
const vec3 light_direction1 = normalize(vec3(-0.5, -1, 1));
const vec3 color0 = vec3(0.0, 0.15, 0.11);
const vec3 color1 = vec3(0.59, 0.93, 0.54);

void main() {
    vec3 normal = ec_normal;
    normal = normalize(cross(dFdx(ec_pos), dFdy(ec_pos)));
    float diffuse0 = max(0, dot(normal, light_direction0));
    float diffuse1 = max(0, dot(normal, light_direction1));
    float diffuse = diffuse0 * 0.75 + diffuse1 * 0.25;
    // vec3 color = object_color * diffuse;
    vec3 color = mix(color0, color1, diffuse);
    gl_FragColor = vec4(color, 1);
}
)";

int main(int argc, char **argv) {
    auto startTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed;

    const auto triangles = SphereTriangles(1);
    // const auto triangles = LoadBinarySTL(argv[1]);
    Model model(triangles);
    ctpl::thread_pool tp(4);

    // while (model.Positions().size() < 50000) {
    //     model.UpdateWithThreadPool(tp);
    // }
    // elapsed = std::chrono::steady_clock::now() - startTime;
    // std::cout << elapsed.count() << std::endl;

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
    const auto normalAttrib = p.GetAttribLocation("normal");
    const auto matrixUniform = p.GetUniformLocation("matrix");

    glm::vec3 minPosition = triangles.front().A();
    glm::vec3 maxPosition = triangles.front().A();
    for (const auto &t : triangles) {
        minPosition = glm::min(minPosition, t.A());
        maxPosition = glm::max(maxPosition, t.A());
        minPosition = glm::min(minPosition, t.B());
        maxPosition = glm::max(maxPosition, t.B());
        minPosition = glm::min(minPosition, t.C());
        maxPosition = glm::max(maxPosition, t.C());
    }
    minPosition = glm::vec3(-30);
    maxPosition = glm::vec3(30);

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

    std::vector<glm::vec3> positionsAndNormals;
    std::vector<glm::uvec3> indexes;

    const auto update = [&]() {
        positionsAndNormals.resize(0);
        model.PositionsAndNormals(positionsAndNormals);

        indexes.resize(0);
        model.TriangleIndexes(indexes);

        glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            positionsAndNormals.size() * sizeof(positionsAndNormals.front()),
            positionsAndNormals.data(),
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indexes.size() * sizeof(indexes.front()),
            indexes.data(),
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // std::cout << model.Positions().size() << std::endl;
    };

    while (!glfwWindowShouldClose(window)) {
        elapsed = std::chrono::steady_clock::now() - startTime;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            model = Model(triangles);
        }

        if (model.Positions().size() > 42 * std::pow(2, 10)) {
            // SaveBinarySTL("out.stl", model.Triangulate());
            model = Model(triangles);
        }

        if (elapsed.count() > 0) {
            for (int i = 0; i < 1; i++) {
                model.UpdateWithThreadPool(tp);
            }
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
        glm::vec3 center(0, 0, 1);
        glm::vec3 up(0, 0, 1);
        glm::mat4 lookAt = glm::lookAt(eye, center, up);
        glm::mat4 matrix = projection * lookAt * rotation * modelTransform;
        glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, glm::value_ptr(matrix));

        glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glEnableVertexAttribArray(positionAttrib);
        glEnableVertexAttribArray(normalAttrib);
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, false, 24, 0);
        glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, false, 24, (void *)12);
        glDrawElements(GL_TRIANGLES, indexes.size() * 3, GL_UNSIGNED_INT, 0);
        glDisableVertexAttribArray(positionAttrib);
        glDisableVertexAttribArray(normalAttrib);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

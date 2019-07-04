#define GL_SILENCE_DEPRECATION
#define GLM_ENABLE_EXPERIMENTAL
#define GL_GLEXT_PROTOTYPES

#include <chrono>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>

#include "gui.h"
#include "pool.h"
#include "program.h"
#include "stl.h"

const std::string VertexSource = R"(
#version 120

uniform mat4 matrix;

attribute vec4 position;
attribute vec3 normal;
attribute float value;

varying vec3 ec_pos;
varying vec3 ec_normal;
varying float ec_value;

void main() {
    gl_Position = matrix * position;
    ec_pos = vec3(gl_Position);
    ec_normal = normal;
    ec_value = value;
}
)";

const std::string FragmentSource = R"(
#version 120

varying vec3 ec_pos;
varying vec3 ec_normal;
varying float ec_value;

const vec3 light_direction0 = normalize(vec3(0.5, -2, 1));
const vec3 light_direction1 = normalize(vec3(-0.5, -1, 1));
const vec3 color1 = vec3(0.59, 0.93, 0.54);
const vec3 color0 = color1 * 0.1;//, 0.15, 0.11);

void main() {
    vec3 normal = ec_normal;
    normal = normalize(cross(dFdx(ec_pos), dFdy(ec_pos)));
    float diffuse0 = max(0, dot(normal, light_direction0));
    float diffuse1 = max(0, dot(normal, light_direction1));
    float diffuse = diffuse0 * 0.75 + diffuse1 * 0.25;
    vec3 valueColor = vec3(ec_value * 0.8 + 0.2);
    valueColor = color1;
    vec3 color = mix(color0, valueColor, diffuse);
    gl_FragColor = vec4(color, 1);
}
)";

void RunGUI(Model &model) {
    auto startTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed;

    ThreadPool pool;

    for (int i = 0; i < 100; i++) {
        model.Update(pool, false);
    }

    if (!glfwInit()) {
        return;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    GLFWwindow *window = glfwCreateWindow(
        1600, 1200, "Cellular Forms", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor((float)0x2a/255, (float)0x2c/255, (float)0x2b/255, 1);

    Program program(VertexSource, FragmentSource);

    const auto positionAttrib = program.GetAttribLocation("position");
    const auto normalAttrib = program.GetAttribLocation("normal");
    const auto valueAttrib = program.GetAttribLocation("value");
    const auto matrixUniform = program.GetUniformLocation("matrix");

    GLuint arrayBuffer;
    GLuint elementBuffer;
    glGenBuffers(1, &arrayBuffer);
    glGenBuffers(1, &elementBuffer);

    glm::vec3 targetMin, targetMax;
    model.Bounds(targetMin, targetMax);
    glm::vec3 currentMin = targetMin;
    glm::vec3 currentMax = targetMax;
    const glm::vec3 minSize(glm::distance(targetMin, targetMax) * 5);

    const auto getModelTransform = [&]() {
        glm::vec3 min, max;
        model.Bounds(min, max);
        targetMin = glm::min(targetMin, min);
        targetMax = glm::max(targetMax, max);
        currentMin += (targetMin - currentMin) * 0.01f;
        currentMax += (targetMax - currentMax) * 0.01f;
        currentMin = glm::min(currentMin, -minSize);
        currentMax = glm::max(currentMax, minSize);
        const glm::vec3 size = currentMax - currentMin;
        const glm::vec3 center = (currentMin + currentMax) / 2.0f;
        const float scale = glm::compMin(glm::vec3(2) / size);
        const glm::mat4 modelTransform =
            glm::scale(glm::mat4(1.0f), glm::vec3(scale)) *
            glm::translate(glm::mat4(1.0f), -center);
        return modelTransform;
    };

    std::vector<float> vertexAttributes;
    std::vector<glm::uvec3> indexes;

    const auto updateBuffers = [&]() {
        vertexAttributes.resize(0);
        model.VertexAttributes(vertexAttributes);

        indexes.resize(0);
        model.TriangleIndexes(indexes);

        glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexAttributes.size() * sizeof(vertexAttributes.front()),
            vertexAttributes.data(),
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

        for (int i = 0; i < 1; i++) {
            model.Update(pool);
        }

        updateBuffers();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        int w, h;
        glfwGetWindowSize(window, &w, &h);
        const float aspect = (float)w / (float)h;
        const float angle = 0;//elapsed.count() * 3;
        glm::mat4 rotation = glm::rotate(
            glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 0, 1));
        glm::mat4 projection = glm::perspective(
            glm::radians(25.f), aspect, 1.f, 1000.f);
        glm::vec3 eye(0, -5, 0);
        glm::vec3 center(0, 0, 0);
        glm::vec3 up(0, 0, 1);
        glm::mat4 lookAt = glm::lookAt(eye, center, up);
        glm::mat4 matrix = projection * lookAt * rotation * getModelTransform();
        glUniformMatrix4fv(matrixUniform, 1, GL_FALSE, glm::value_ptr(matrix));

        glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glEnableVertexAttribArray(positionAttrib);
        glEnableVertexAttribArray(normalAttrib);
        glEnableVertexAttribArray(valueAttrib);
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, false, 28, 0);
        glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, false, 28, (void *)12);
        glVertexAttribPointer(valueAttrib, 1, GL_FLOAT, false, 28, (void *)24);
        glDrawElements(GL_TRIANGLES, indexes.size() * 3, GL_UNSIGNED_INT, 0);
        glDisableVertexAttribArray(positionAttrib);
        glDisableVertexAttribArray(normalAttrib);
        glDisableVertexAttribArray(valueAttrib);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

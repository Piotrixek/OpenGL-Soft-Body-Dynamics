// main.cpp
#define GLFW_EXPOSE_NATIVE_WIN32
#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Camera.h"
#include "Logger.h"
#include "Shader.h"
#include "Callbacks.h"
#include "Globals.h"
#include "SoftBody.h"
#include "Cube.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Water Simulation", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int skyVAO, skyVBO;
    float skyVertices[] = { -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int skyShader = loadShader("shaders/sky.vert", "shaders/sky.frag");
    if (!skyShader) {
        std::cerr << "Critical shader load error!\n";
        glfwTerminate();
        return -1;
    }
    unsigned int softBodyShader = loadShader("shaders/softbody.vert", "shaders/softbody.frag");
    if (!softBodyShader) {
        std::cerr << "Critical soft body shader load error!\n";
        glfwTerminate();
        return -1;
    }

    SoftBody softBody;
    Cube cube;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    static glm::vec3 prevExternalPos(2.0f, 1.0f, 0.0f);
    static glm::vec3 prevExternalRot(0.0f, 0.0f, 0.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = camera.getProjectionMatrix(1280.0f / 720.0f);
        glm::mat4 view = camera.getViewMatrix();

        glDisable(GL_DEPTH_TEST);
        glUseProgram(skyShader);
        glBindVertexArray(skyVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glEnable(GL_DEPTH_TEST);
        glUseProgram(softBodyShader);
        int projLoc = glGetUniformLocation(softBodyShader, "projection");
        int viewLoc = glGetUniformLocation(softBodyShader, "view");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        int modelLoc = glGetUniformLocation(softBodyShader, "model");

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Controls");
        static bool pauseSimulation = false;
        ImGui::Checkbox("Pause Soft Body", &pauseSimulation);
        static float softBodyPos[3] = { 2.0f, 1.0f, 0.0f };
        ImGui::SliderFloat3("Soft Body Position", softBodyPos, -10.0f, 10.0f);
        static float softBodyRot[3] = { 0.0f, 0.0f, 0.0f };
        ImGui::SliderFloat3("Soft Body Rotation", softBodyRot, 0.0f, 360.0f);
        static float softBodyScale[3] = { 1.0f, 1.0f, 1.0f };
        ImGui::SliderFloat3("Soft Body Scale", softBodyScale, 0.1f, 5.0f);
        ImGui::End();

        glm::vec3 currentExternalPos(softBodyPos[0], softBodyPos[1], softBodyPos[2]);
        glm::vec3 posImpulse = (currentExternalPos - prevExternalPos) * 5.0f;
        softBody.applyExternalImpulse(posImpulse);
        prevExternalPos = currentExternalPos;

        glm::vec3 currentExternalRot(softBodyRot[0], softBodyRot[1], softBodyRot[2]);
        glm::vec3 rotImpulse = (currentExternalRot - prevExternalRot) * 5.0f;
        softBody.applyExternalRotationImpulse(rotImpulse);
        prevExternalRot = currentExternalRot;

        if (!pauseSimulation) {
            softBody.update(deltaTime);
        }

        glm::mat4 softBodyModel = glm::mat4(1.0f);
        softBodyModel = glm::translate(softBodyModel, glm::vec3(softBodyPos[0], softBodyPos[1], softBodyPos[2]));
        softBodyModel = glm::rotate(softBodyModel, glm::radians(softBodyRot[0]), glm::vec3(1.0f, 0.0f, 0.0f));
        softBodyModel = glm::rotate(softBodyModel, glm::radians(softBodyRot[1]), glm::vec3(0.0f, 1.0f, 0.0f));
        softBodyModel = glm::rotate(softBodyModel, glm::radians(softBodyRot[2]), glm::vec3(0.0f, 0.0f, 1.0f));
        softBodyModel = glm::scale(softBodyModel, glm::vec3(softBodyScale[0], softBodyScale[1], softBodyScale[2]));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &softBodyModel[0][0]);
        softBody.draw();

        logger.draw("Application Log");
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glDeleteVertexArrays(1, &skyVAO);
    glDeleteBuffers(1, &skyVBO);
    glfwTerminate();
    return 0;
}

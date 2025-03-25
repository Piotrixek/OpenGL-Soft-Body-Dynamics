// Callbacks.cpp
#include "Callbacks.h"
#include "Globals.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!mouseCaptured) {
        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
        return;
    }
    if (camera.firstMouse) {
        camera.lastX = xpos;
        camera.lastY = ypos;
        camera.firstMouse = false;
    }
    float xoffset = xpos - camera.lastX;
    float yoffset = camera.lastY - ypos;
    camera.lastX = xpos;
    camera.lastY = ypos;
    xoffset *= camera.sensitivity;
    yoffset *= camera.sensitivity;
    camera.yaw += xoffset;
    camera.pitch += yoffset;
    if (camera.pitch > 89.0f) camera.pitch = 89.0f;
    if (camera.pitch < -89.0f) camera.pitch = -89.0f;
    glm::vec3 direction;
    direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    direction.y = sin(glm::radians(camera.pitch));
    direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front = glm::normalize(direction);
}
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (mouseCaptured) camera.processInput(window, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        mouseCaptured = !mouseCaptured;
        glfwSetInputMode(window, GLFW_CURSOR, mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        if (mouseCaptured) camera.firstMouse = true;
        else ImGui::GetIO().ClearInputKeys();
    }
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (!mouseCaptured) ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (!mouseCaptured) ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

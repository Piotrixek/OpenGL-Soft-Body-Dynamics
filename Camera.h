// Camera.h
#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
class Camera {
public:
    glm::vec3 position = glm::vec3(0.0f, 2.0f, 5.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float speed = 5.0f;
    float sensitivity = 0.1f;
    float zoom = 45.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    double lastX = 400, lastY = 300;
    bool firstMouse = true;
    void processInput(GLFWwindow* window, float deltaTime);
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix(float aspectRatio);
};

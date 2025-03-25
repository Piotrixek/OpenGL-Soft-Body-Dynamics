// Cube.h
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
class Cube {
public:
    Cube();
    void draw();
    void setModel(const glm::mat4& model);
private:
    unsigned int VAO, VBO, EBO;
    int modelLoc;
};

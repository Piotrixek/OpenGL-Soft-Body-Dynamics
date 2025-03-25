// SoftBody.h
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
class SoftBody {
public:
    SoftBody(int gridWidth = 10, int gridHeight = 10, float spacing = 0.5f);
    void update(float dt);
    void draw();
    void applyExternalImpulse(const glm::vec3& impulse);
    void applyExternalRotationImpulse(const glm::vec3& impulse);
private:
    struct Particle {
        glm::vec3 pos;
        glm::vec3 velocity;
        glm::vec3 acceleration;
        bool fixed;
    };
    struct Spring {
        int p1, p2;
        float restLength;
    };
    std::vector<Particle> particles;
    std::vector<Spring> springs;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    int gridWidth, gridHeight;
    void setupMesh();
    void setupSprings();
    void updateMesh();
};

// SoftBody.cpp
#include "SoftBody.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
SoftBody::SoftBody(int gridW, int gridH, float spacing) : gridWidth(gridW), gridHeight(gridH) {
    for (int j = 0; j < gridHeight; j++) {
        for (int i = 0; i < gridWidth; i++) {
            Particle p;
            p.pos = glm::vec3((i - gridWidth / 2) * spacing, 5.0f, (j - gridHeight / 2) * spacing);
            p.velocity = glm::vec3(0.0f);
            p.acceleration = glm::vec3(0.0f);
            p.fixed = (j == gridHeight - 1 && (i == 0 || i == gridW - 1));
            particles.push_back(p);
        }
    }
    setupSprings();
    setupMesh();
}
void SoftBody::setupSprings() {
    auto idx = [this](int i, int j) -> int { return j * gridWidth + i; };
    float spacing = 0.5f;
    float rest = spacing;
    for (int j = 0; j < gridHeight; j++) {
        for (int i = 0; i < gridWidth; i++) {
            if (i < gridWidth - 1) {
                Spring s;
                s.p1 = idx(i, j);
                s.p2 = idx(i + 1, j);
                s.restLength = rest;
                springs.push_back(s);
            }
            if (j < gridHeight - 1) {
                Spring s;
                s.p1 = idx(i, j);
                s.p2 = idx(i, j + 1);
                s.restLength = rest;
                springs.push_back(s);
            }
            if (i < gridWidth - 1 && j < gridHeight - 1) {
                Spring s;
                s.p1 = idx(i, j);
                s.p2 = idx(i + 1, j + 1);
                s.restLength = rest * std::sqrt(2.0f);
                springs.push_back(s);
                Spring s2;
                s2.p1 = idx(i + 1, j);
                s2.p2 = idx(i, j + 1);
                s2.restLength = rest * std::sqrt(2.0f);
                springs.push_back(s2);
            }
        }
    }
}
void SoftBody::setupMesh() {
    vertices.resize(particles.size() * 3);
    for (size_t i = 0; i < particles.size(); i++) {
        vertices[i * 3 + 0] = particles[i].pos.x;
        vertices[i * 3 + 1] = particles[i].pos.y;
        vertices[i * 3 + 2] = particles[i].pos.z;
    }
    for (int j = 0; j < gridHeight - 1; j++) {
        for (int i = 0; i < gridWidth - 1; i++) {
            int topLeft = j * gridWidth + i;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + gridWidth;
            int bottomRight = bottomLeft + 1;
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}
void SoftBody::updateMesh() {
    for (size_t i = 0; i < particles.size(); i++) {
        vertices[i * 3 + 0] = particles[i].pos.x;
        vertices[i * 3 + 1] = particles[i].pos.y;
        vertices[i * 3 + 2] = particles[i].pos.z;
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
}
void SoftBody::applyExternalImpulse(const glm::vec3& impulse) {
    for (auto& p : particles) {
        if (!p.fixed)
            p.velocity += impulse;
    }
}
void SoftBody::applyExternalRotationImpulse(const glm::vec3& impulse) {
    glm::vec3 center(0.0f);
    int count = 0;
    for (auto& p : particles) {
        if (!p.fixed) {
            center += p.pos;
            count++;
        }
    }
    if (count > 0)
        center /= count;
    for (auto& p : particles) {
        if (!p.fixed) {
            glm::vec3 r = p.pos - center;
            p.velocity += glm::cross(impulse, r);
        }
    }
}
void SoftBody::update(float dt) {
    float stiffness = 500.0f;
    float damping = 0.98f;
    glm::vec3 gravity(0.0f, -9.81f, 0.0f);
    for (auto& p : particles) {
        if (!p.fixed) {
            p.acceleration = gravity;
        }
    }
    for (auto& s : springs) {
        Particle& p1 = particles[s.p1];
        Particle& p2 = particles[s.p2];
        glm::vec3 delta = p2.pos - p1.pos;
        float dist = glm::length(delta);
        if (dist == 0.0f) continue;
        glm::vec3 force = stiffness * (dist - s.restLength) * (delta / dist);
        if (!p1.fixed)
            p1.acceleration += force;
        if (!p2.fixed)
            p2.acceleration -= force;
    }
    for (auto& p : particles) {
        if (!p.fixed) {
            p.velocity += p.acceleration * dt;
            p.velocity *= damping;
            p.pos += p.velocity * dt;
            if (p.pos.y < 0.0f) {
                p.pos.y = 0.0f;
                p.velocity.y *= -0.5f;
            }
        }
    }
    updateMesh();
}
void SoftBody::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
